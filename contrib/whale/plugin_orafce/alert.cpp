#include "postgres.h"
#include "executor/spi.h"

#include "access/htup.h"
#include "catalog/pg_type.h"
#include "commands/trigger.h"
#include "funcapi.h"
#include "miscadmin.h"
#include "string.h"
#include "storage/lock/lwlock.h"
#include "storage/procarray.h"
#include "plugin_utils/timestamp.h"

#include "plugin_postgres.h"
#include "plugin_orafce/builtins.h"
#include "plugin_orafce/pipe.h"
#include "plugin_orafce/shmmc.h"
#include "utils/rel.h"
#include "plugin_postgres.h"

PG_FUNCTION_INFO_V1_PUBLIC(gms_alert_register);
PG_FUNCTION_INFO_V1_PUBLIC(gms_alert_remove);
PG_FUNCTION_INFO_V1_PUBLIC(gms_alert_removeall);
PG_FUNCTION_INFO_V1_PUBLIC(gms_alert_set_defaults);
PG_FUNCTION_INFO_V1_PUBLIC(gms_alert_signal);
PG_FUNCTION_INFO_V1_PUBLIC(gms_alert_waitany);
PG_FUNCTION_INFO_V1_PUBLIC(gms_alert_waitone);
PG_FUNCTION_INFO_V1_PUBLIC(gms_alert_defered_signal);

#ifndef _GetCurrentTimestamp
#define _GetCurrentTimestamp() GetCurrentTimestamp()
#endif

#ifndef GetNowFloat
#ifdef HAVE_INT64_TIMESTAMP
#define GetNowFloat() ((float8)_GetCurrentTimestamp() / 1000000.0)
#else
#define GetNowFloat() _GetCurrentTimestamp()
#endif
#endif

#define TDAYS (1000 * 24 * 3600)

static void unregister_event(int event_id, int sid);
static char *find_and_remove_message_item(int message_id, int sid, bool all, bool remove_all, bool filter_message,
                                          int *sleep, char **event_name);

/*
 * There are maximum 30 events and 255 collaborating sessions
 *
 */

#define NOT_FOUND -1
#define NOT_USED -1

/*
 * Compare text and cstr
 */

static int textcmpm(text *txt, char *str)
{
    char *p;
    int len;

    len = VARSIZE(txt) - VARHDRSZ;
    p = VARDATA(txt);

    while (len-- && *p != '\0') {
        int retval;
        if (0 != (retval = *p++ - *str++))
            return retval;
    }

    if (len > 0)
        return 1;

    if (*str != '\0')
        return -1;

    return 0;
}

/*
 * this function is called when we know so session with sid is not valid
 * anymore.
 */
static void purge_shared_alert_mem()
{
    int i;

#if PG_VERSION_NUM >= 90600

    LWLockAcquire(ProcArrayLock, LW_SHARED);

#endif

    for (i = 0; i < MAX_LOCKS; i++) {
        PGPROC *proc;

        if (GetSessionContext()->alert_locks[i].sid == NOT_USED)
            continue;

#if PG_VERSION_NUM < 90600

        proc = BackendPidGetProc(GetSessionContext()->alert_locks[i].pid);

#else

        proc = BackendPidGetProcWithLock(GetSessionContext()->alert_locks[i].pid);

#endif

        if (proc == NULL) {
            int j;
            int invalid_sid = GetSessionContext()->alert_locks[i].sid;

            for (j = 0; j < ORA_MAX_EVENTS; j++) {
                if (GetSessionContext()->alert_events[j].event_name != NULL) {
                    find_and_remove_message_item(j, invalid_sid, false, true, true, NULL, NULL);
                    unregister_event(j, invalid_sid);
                }
            }

            GetSessionContext()->alert_locks[i].sid = NOT_USED;
        }
    }

#if PG_VERSION_NUM >= 90600

    LWLockRelease(ProcArrayLock);

#endif
}

/*
 * find or create event rec
 *
 */

static alert_lock *find_lock(int sid, bool create)
{
    int i;
    int first_free = NOT_FOUND;

    if (GetSessionContext()->alert_session_lock != NULL)
        return GetSessionContext()->alert_session_lock;

    for (i = 0; i < MAX_LOCKS; i++) {
        if (GetSessionContext()->alert_locks[i].sid == sid)
            return &(GetSessionContext()->alert_locks[i]);
        else if (GetSessionContext()->alert_locks[i].sid == NOT_USED && first_free == NOT_FOUND)
            first_free = i;
    }

    if (create) {
        if (first_free == NOT_FOUND) {
            purge_shared_alert_mem();

            for (i = 0; i < MAX_LOCKS; i++) {
                if (GetSessionContext()->alert_locks[i].sid == NOT_USED) {
                    first_free = i;
                    break;
                }
            }
        }

        if (first_free != NOT_FOUND) {
            GetSessionContext()->alert_locks[first_free].sid = sid;
            GetSessionContext()->alert_locks[first_free].echo = NULL;
            GetSessionContext()->alert_locks[first_free].pid =
                (IS_THREAD_POOL_WORKER ? u_sess->session_id : t_thrd.proc_cxt.MyProcPid);
            GetSessionContext()->alert_session_lock = &(GetSessionContext()->alert_locks[first_free]);
            return &(GetSessionContext()->alert_locks[first_free]);
        } else
            ereport(ERROR, (errcode(ERRCODE_ORA_PACKAGES_LOCK_REQUEST_ERROR), errmsg("lock request error"),
                            errdetail("Failed to create session lock."),
                            errhint("There are too many collaborating sessions. Increase MAX_LOCKS in 'pipe.h'.")));
    }

    return NULL;
}

static alert_event *find_event(text *event_name, bool create, int *event_id)
{
    int i;

    for (i = 0; i < ORA_MAX_EVENTS; i++) {
        if (GetSessionContext()->alert_events[i].event_name != NULL &&
            textcmpm(event_name, GetSessionContext()->alert_events[i].event_name) == 0) {
            if (event_id != NULL)
                *event_id = i;
            return &(GetSessionContext()->alert_events[i]);
        }
    }

    if (create) {
        for (i = 0; i < ORA_MAX_EVENTS; i++) {
            if (GetSessionContext()->alert_events[i].event_name == NULL) {
                GetSessionContext()->alert_events[i].event_name = ora_scstring(event_name);

                GetSessionContext()->alert_events[i].max_receivers = 0;
                GetSessionContext()->alert_events[i].receivers = NULL;
                GetSessionContext()->alert_events[i].messages = NULL;
                GetSessionContext()->alert_events[i].receivers_number = 0;

                if (event_id != NULL)
                    *event_id = i;
                return &(GetSessionContext()->alert_events[i]);
            }
        }

        ereport(ERROR, (errcode(ERRCODE_PROGRAM_LIMIT_EXCEEDED), errmsg("event registration error"),
                        errdetail("Too many registered events."),
                        errhint("There are too many collaborating sessions. Increase ORA_MAX_EVENTS in 'pipe.h'.")));
    }

    return NULL;
}

static void register_event(text *event_name)
{
    alert_event *ev;
    int *new_receivers;
    int first_free;
    int i;

    find_lock(GetSessionContext()->pipe_sid, true);
    ev = find_event(event_name, true, NULL);

    first_free = NOT_FOUND;
    for (i = 0; i < ev->max_receivers; i++) {
        if (ev->receivers[i] == GetSessionContext()->pipe_sid)
            return; /* event is registered */
        if (ev->receivers[i] == NOT_USED && first_free == NOT_FOUND)
            first_free = i;
    }

    /*
     * I can have a maximum of MAX_LOCKS receivers for one event.
     * Array receivers is increased for 16 fields
     */
    if (first_free == NOT_FOUND) {
        if (ev->max_receivers + 16 > MAX_LOCKS)
            ereport(ERROR, (errcode(ERRCODE_ORA_PACKAGES_LOCK_REQUEST_ERROR), errmsg("lock request error"),
                            errdetail("Failed to create session lock."),
                            errhint("There are too many collaborating sessions. Increase MAX_LOCKS in 'pipe.h'.")));

        /* increase receiver's array */

        new_receivers = (int *)salloc((ev->max_receivers + 16) * sizeof(int));

        for (i = 0; i < ev->max_receivers + 16; i++) {
            if (i < ev->max_receivers)
                new_receivers[i] = ev->receivers[i];
            else
                new_receivers[i] = NOT_USED;
        }

        ev->max_receivers += 16;
        if (ev->receivers)
            ora_sfree(ev->receivers);

        ev->receivers = new_receivers;

        first_free = ev->max_receivers - 16;
    }

    ev->receivers_number += 1;
    ev->receivers[first_free] = GetSessionContext()->pipe_sid;
}

/*
 * Remove receiver from default receivers of message,
 * I expect clean all message_items
 */

static void unregister_event(int event_id, int sid)
{
    alert_event *ev;

    ev = &(GetSessionContext()->alert_events[event_id]);
    if (ev->receivers_number > 0) {
        for (int i = 0; i < ev->max_receivers; i++) {
            if (ev->receivers[i] == sid) {
                ev->receivers[i] = NOT_USED;
                ev->receivers_number -= 1;
                break;
            }
        }
        if (ev->receivers_number == 0) {
            ora_sfree(ev->receivers);
            ora_sfree(ev->event_name);
            ev->receivers = NULL;
            ev->event_name = NULL;
        }
    }
}

/*
 * remove receiver from list of receivers.
 * Message has always minimal one receiver
 * Return true, if exist other receiver
 */

static bool remove_receiver(message_item *msg, int sid)
{
    int i;
    bool find_other = false;
    bool found = false;

    for (i = 0; i < msg->receivers_number; i++) {
        if (msg->receivers[i] == sid) {
            msg->receivers[i] = NOT_USED;
            found = true;
        } else if (msg->receivers[i] != NOT_USED) {
            find_other = true;
        }
        if (found && find_other)
            break;
    }

    return find_other;
}

/*
 *
 * Reads message message_id for user sid. If arg:all is true,
 * then get any message. If arg:remove_all then remove all
 * signaled messages for sid. If arg:filter_message then
 * skip other messages than message_id, else read and remove
 * all others messages than message_id.
 *
 */

static char *find_and_remove_message_item(int message_id, int sid, bool all, bool remove_all, bool filter_message,
                                          int *sleep, char **event_name)
{
    alert_lock *alck;

    char *result = NULL;
    if (sleep != NULL)
        *sleep = 0;

    alck = find_lock(sid, false);

    if (event_name)
        *event_name = NULL;

    if (alck != NULL && alck->echo != NULL) {
        /* if I have registered and created item */
        struct _message_echo *echo, *last_echo;

        echo = alck->echo;
        last_echo = NULL;

        while (echo != NULL) {
            char *message_text;
            bool destroy_msg_item = false;
            int _message_id;

            if (filter_message && echo->message_id != message_id) {
                last_echo = echo;
                echo = echo->next_echo;
                continue;
            }

            message_text = echo->message->message;
            _message_id = echo->message_id;

            if (!remove_receiver(echo->message, sid)) {
                destroy_msg_item = true;
                if (echo->message->prev_message != NULL)
                    echo->message->prev_message->next_message = echo->message->next_message;
                else
                    GetSessionContext()->alert_events[echo->message_id].messages = echo->message->next_message;
                if (echo->message->next_message != NULL)
                    echo->message->next_message->prev_message = echo->message->prev_message;
                ora_sfree(echo->message->receivers);
                ora_sfree(echo->message);
            }
            if (last_echo == NULL) {
                alck->echo = echo->next_echo;
                ora_sfree(echo);
                echo = alck->echo;
            } else {
                last_echo->next_echo = echo->next_echo;
                ora_sfree(echo);
                echo = last_echo;
            }
            if (remove_all) {
                if (message_text != NULL && destroy_msg_item)
                    ora_sfree(message_text);

                continue;
            } else if (_message_id == message_id || all) {
                /* I have to do local copy */
                if (message_text) {
                    result = pstrdup(message_text);
                    if (destroy_msg_item)
                        ora_sfree(message_text);
                }

                if (event_name != NULL)
                    *event_name = pstrdup(GetSessionContext()->alert_events[_message_id].event_name);

                break;
            }
        }
    }

    return result;
}

/*
 * Queue mustn't to contain duplicate messages
 */

static void create_message(text *event_name, text *message)
{
    int event_id;
    alert_event *ev;
    message_item *msg_item = NULL;

    find_event(event_name, false, &event_id);

    /* process event only when any recipient exitsts */
    if (NULL != (ev = find_event(event_name, false, &event_id))) {
        if (ev->receivers_number > 0) {
            int i, j;
            msg_item = ev->messages;
            while (msg_item != NULL) {
                if (msg_item->message == NULL && message == NULL)
                    return;
                if (msg_item->message != NULL && message != NULL)
                    if (0 == textcmpm(message, msg_item->message))
                        return;

                msg_item = msg_item->next_message;
            }

            msg_item = (message_item *)salloc(sizeof(message_item));

            msg_item->receivers = (int *)salloc(ev->receivers_number * sizeof(int));
            msg_item->receivers_number = ev->receivers_number;

            if (message != NULL)
                msg_item->message = ora_scstring(message);
            else
                msg_item->message = NULL;

            msg_item->message_id = event_id;
            for (i = j = 0; j < ev->max_receivers; j++) {
                if (ev->receivers[j] != NOT_USED) {
                    msg_item->receivers[i++] = ev->receivers[j];
                    for (int k = 0; k < MAX_LOCKS; k++)
                        if (GetSessionContext()->alert_locks[k].sid == ev->receivers[j]) {
                            /* create echo */

                            message_echo *echo = (message_echo *)salloc(sizeof(message_echo));
                            echo->message = msg_item;
                            echo->message_id = event_id;
                            echo->next_echo = NULL;

                            if (GetSessionContext()->alert_locks[k].echo == NULL)
                                GetSessionContext()->alert_locks[k].echo = echo;
                            else {
                                message_echo *p;
                                p = GetSessionContext()->alert_locks[k].echo;

                                while (p->next_echo != NULL)
                                    p = p->next_echo;

                                p->next_echo = echo;
                            }
                        }
                }
            }

            msg_item->next_message = NULL;
            if (ev->messages == NULL) {
                msg_item->prev_message = NULL;
                ev->messages = msg_item;
            } else {
                message_item *p;
                p = ev->messages;
                while (p->next_message != NULL)
                    p = p->next_message;

                p->next_message = msg_item;
                msg_item->prev_message = p;
            }
        }
    }
}

#define WATCH_PRE(t, et, c)         \
    et = GetNowFloat() + (float8)t; \
    c = 0;                          \
    do {

#define WATCH_POST(t, et, c)    \
    if (GetNowFloat() >= et)    \
        break;                  \
    if (cycle++ % 100 == 0)     \
        CHECK_FOR_INTERRUPTS(); \
    pg_usleep(10000L);          \
    }                           \
    while (t != 0)              \
        ;

/*
 *
 *  PROCEDURE GMS_ALERT.REGISTER (name IN VARCHAR2);
 *
 *  Registers the calling session to receive notification of alert name.
 *
 */

Datum gms_alert_register(PG_FUNCTION_ARGS)
{
    text *name = PG_GETARG_TEXT_P(0);
    int cycle = 0;
    float8 endtime;
    float8 timeout = 2;

    WATCH_PRE(timeout, endtime, cycle);
    if (ora_lock_shmem(SHMEMMSGSZ, MAX_PIPES, ORA_MAX_EVENTS, MAX_LOCKS, false)) {
        register_event(name);
        LWLockRelease(GetSessionContext()->shmem_lockid);
        PG_RETURN_VOID();
    }
    WATCH_POST(timeout, endtime, cycle);
    LOCK_ERROR();
    PG_RETURN_VOID();
}

/*
 *
 *  PROCEDURE GMS_ALERT.REMOVE(name IN VARCHAR2);
 *
 *  Unregisters the calling session from receiving notification of alert name.
 *  Don't raise any exceptions.
 *
 */

Datum gms_alert_remove(PG_FUNCTION_ARGS)
{
    text *name = PG_GETARG_TEXT_P(0);

    int ev_id;
    int cycle = 0;
    float8 endtime;
    float8 timeout = 2;

    WATCH_PRE(timeout, endtime, cycle);
    if (ora_lock_shmem(SHMEMMSGSZ, MAX_PIPES, ORA_MAX_EVENTS, MAX_LOCKS, false)) {
        alert_event *ev;
        ev = find_event(name, false, &ev_id);
        if (NULL != ev) {
            find_and_remove_message_item(ev_id, GetSessionContext()->pipe_sid, false, true, true, NULL, NULL);
            unregister_event(ev_id, GetSessionContext()->pipe_sid);
        }
        LWLockRelease(GetSessionContext()->shmem_lockid);
        PG_RETURN_VOID();
    }
    WATCH_POST(timeout, endtime, cycle);
    LOCK_ERROR();
    PG_RETURN_VOID();
}

/*
 *
 *  PROCEDURE GMS_ALERT.REMOVEALL;
 *
 *  Unregisters the calling session from notification of all alerts.
 *
 */

Datum gms_alert_removeall(PG_FUNCTION_ARGS)
{
    int cycle = 0;
    float8 endtime;
    float8 timeout = 2;

    WATCH_PRE(timeout, endtime, cycle);
    if (ora_lock_shmem(SHMEMMSGSZ, MAX_PIPES, ORA_MAX_EVENTS, MAX_LOCKS, false)) {
        alert_lock *alck;
        for (int i = 0; i < ORA_MAX_EVENTS; i++) {
            if (GetSessionContext()->alert_events[i].event_name != NULL) {
                find_and_remove_message_item(i, GetSessionContext()->pipe_sid, false, true, true, NULL, NULL);
                unregister_event(i, GetSessionContext()->pipe_sid);
            }
        }

        alck = find_lock(GetSessionContext()->pipe_sid, false);
        if (alck) {
            /* After all events unregistration, an echo field should NULL */
            Assert(alck->echo == NULL);

            alck->sid = NOT_USED;
            GetSessionContext()->alert_session_lock = NULL;
        }

        LWLockRelease(GetSessionContext()->shmem_lockid);
        PG_RETURN_VOID();
    }
    WATCH_POST(timeout, endtime, cycle);
    LOCK_ERROR();
    PG_RETURN_VOID();
}

/*
 *
 *  PROCEDURE GMS_ALERT.WAITANY(name OUT VARCHAR2 ,message OUT VARCHAR2
 *                              ,status OUT INTEGER
 *                              ,timeout IN NUMBER DEFAULT MAXWAIT);
 *
 *  Waits for up to timeout seconds to be notified of any alerts for which
 *  the session is registered. If status = 0 then name and message contain
 *  alert information. If status = 1 then timeout seconds elapsed without
 *  notification of any alert.
 *
 */

Datum gms_alert_waitany(PG_FUNCTION_ARGS)
{
    float8 timeout;
    TupleDesc tupdesc;
    AttInMetadata *attinmeta;
    HeapTuple tuple;
    Datum result;
    char *str[3] = {NULL, NULL, "1"};
    int cycle = 0;
    float8 endtime;
    TupleDesc btupdesc;

    if (PG_ARGISNULL(0))
        timeout = TDAYS;
    else
        timeout = PG_GETARG_FLOAT8(0);

    WATCH_PRE(timeout, endtime, cycle);
    if (ora_lock_shmem(SHMEMMSGSZ, MAX_PIPES, ORA_MAX_EVENTS, MAX_LOCKS, false)) {
        str[1] = find_and_remove_message_item(-1, GetSessionContext()->pipe_sid, true, false, false, NULL, &str[0]);
        if (str[0]) {
            str[2] = "0";
            LWLockRelease(GetSessionContext()->shmem_lockid);
            break;
        }
        LWLockRelease(GetSessionContext()->shmem_lockid);
    }
    WATCH_POST(timeout, endtime, cycle);

    get_call_result_type(fcinfo, NULL, &tupdesc);
    btupdesc = BlessTupleDesc(tupdesc);
    attinmeta = TupleDescGetAttInMetadata(btupdesc);
    tuple = BuildTupleFromCStrings(attinmeta, str);
    result = HeapTupleGetDatum(tuple);

    if (str[0])
        pfree(str[0]);

    if (str[1])
        pfree(str[1]);

    return result;
}

/*
 *
 *  PROCEDURE GMS_ALERT.WAITONE(name IN VARCHAR2, message OUT VARCHAR2
 *                              ,status OUT INTEGER
 *                              ,timeout IN NUMBER DEFAULT MAXWAIT);
 *
 *  Waits for up to timeout seconds for notification of alert name. If status = 0
 *  then message contains alert information. If status = 1 then timeout
 *  seconds elapsed without notification.
 *
 */

Datum gms_alert_waitone(PG_FUNCTION_ARGS)
{
    text *name;
    float8 timeout;
    TupleDesc tupdesc;
    AttInMetadata *attinmeta;
    HeapTuple tuple;
    Datum result;
    int message_id;
    char *str[2] = {NULL, "1"};
    char *event_name;
    int cycle = 0;
    float8 endtime;
    TupleDesc btupdesc;

    if (PG_ARGISNULL(0))
        ereport(ERROR, (errcode(ERRCODE_NULL_VALUE_NOT_ALLOWED), errmsg("event name is NULL"),
                        errdetail("Eventname may not be NULL.")));

    if (PG_ARGISNULL(1))
        timeout = TDAYS;
    else
        timeout = PG_GETARG_FLOAT8(1);

    name = PG_GETARG_TEXT_P(0);

    WATCH_PRE(timeout, endtime, cycle);
    if (ora_lock_shmem(SHMEMMSGSZ, MAX_PIPES, ORA_MAX_EVENTS, MAX_LOCKS, false)) {
        if (NULL != find_event(name, false, &message_id)) {
            str[0] = find_and_remove_message_item(message_id, GetSessionContext()->pipe_sid, false, false, false,
                                                  NULL, &event_name);
            if (event_name != NULL) {
                str[1] = "0";
                pfree(event_name);
                LWLockRelease(GetSessionContext()->shmem_lockid);
                break;
            }
        }
        LWLockRelease(GetSessionContext()->shmem_lockid);
    }
    WATCH_POST(timeout, endtime, cycle);

    get_call_result_type(fcinfo, NULL, &tupdesc);
    btupdesc = BlessTupleDesc(tupdesc);
    attinmeta = TupleDescGetAttInMetadata(btupdesc);
    tuple = BuildTupleFromCStrings(attinmeta, str);
    result = HeapTupleGetDatum(tuple);

    if (str[0])
        pfree(str[0]);

    return result;
}

/*
 *
 *  PROCEDURE GMS_ALERT.SET_DEFAULTS(sensitivity IN NUMBER);
 *
 *  The SET_DEFAULTS procedure is used to set session configurable settings
 *  used by the GMS_ALERT package. Currently, the polling loop interval sleep time
 *  is the only session setting that can be modified using this procedure. The
 *  header for this procedure is,
 *
 */

Datum gms_alert_set_defaults(PG_FUNCTION_ARGS)
{
    ereport(ERROR, (errcode(ERRCODE_FEATURE_NOT_SUPPORTED), errmsg("feature not supported"),
                    errdetail("Sensitivity isn't supported.")));

    PG_RETURN_VOID();
}

/*
 * This code was originally in plpgsql
 *
 */

/*
CREATE OR REPLACE FUNCTION gms_alert._defered_signal() RETURNS trigger AS $$
BEGIN
  PERFORM gms_alert._signal(NEW.event, NEW.message);
  DELETE FROM ora_alerts WHERE oid=NEW.oid;
  RETURN NEW;
END;
$$ LANGUAGE plpgsql SECURITY DEFINER VOLATILE;
*/

#define DatumGetItemPointer(X) ((ItemPointer)DatumGetPointer(X))
#define ItemPointerGetDatum(X) PointerGetDatum(X)

Datum gms_alert_defered_signal(PG_FUNCTION_ARGS)
{
    TriggerData *trigdata = (TriggerData *)fcinfo->context;
    TupleDesc tupdesc;
    HeapTuple rettuple;
    char *relname;
    text *name;
    text *message;
    int event_col;
    int message_col;

    Datum datum;
    bool isnull;
    int cycle = 0;
    float8 endtime;
    float8 timeout = 2;

    if (!CALLED_AS_TRIGGER(fcinfo))
        ereport(ERROR, (errcode(ERRCODE_TRIGGERED_ACTION_EXCEPTION), errmsg("not called by trigger manager")));

    if (!TRIGGER_FIRED_BY_INSERT(trigdata->tg_event))
        ereport(ERROR, (errcode(ERRCODE_TRIGGERED_ACTION_EXCEPTION), errmsg("not called on valid event")));

    if (SPI_connect() < 0)
        ereport(ERROR, (errcode(ERRCODE_TRIGGERED_ACTION_EXCEPTION), errmsg("SPI_connect failed")));

    if (strcmp((relname = SPI_getrelname(trigdata->tg_relation)), "ora_alerts") != 0)
        ereport(ERROR, (errcode(ERRCODE_TRIGGERED_ACTION_EXCEPTION), errmsg("not called with valid relation")));

    rettuple = trigdata->tg_trigtuple;
    tupdesc = trigdata->tg_relation->rd_att;

    if (SPI_ERROR_NOATTRIBUTE == (event_col = SPI_fnumber(tupdesc, "event")))
        ereport(ERROR, (errcode(ERRCODE_TRIGGERED_ACTION_EXCEPTION), errmsg("attribute event not found")));

    if (SPI_ERROR_NOATTRIBUTE == (message_col = SPI_fnumber(tupdesc, "message")))
        ereport(ERROR, (errcode(ERRCODE_TRIGGERED_ACTION_EXCEPTION), errmsg("attribute message not found")));

    datum = SPI_getbinval(rettuple, tupdesc, event_col, &isnull);
    if (isnull)
        ereport(ERROR, (errcode(ERRCODE_NULL_VALUE_NOT_ALLOWED), errmsg("event name is NULL"),
                        errdetail("Eventname may not be NULL.")));
    name = DatumGetTextP(datum);

    datum = SPI_getbinval(rettuple, tupdesc, message_col, &isnull);
    if (isnull)
        message = NULL;
    else
        message = DatumGetTextP(datum);

    WATCH_PRE(timeout, endtime, cycle);
    if (ora_lock_shmem(SHMEMMSGSZ, MAX_PIPES, ORA_MAX_EVENTS, MAX_LOCKS, false)) {
        ItemPointer tid;
        Oid argtypes[1] = {TIDOID};
        char nulls[1] = {' '};
        Datum values[1];
        SPIPlanPtr plan;

        create_message(name, message);
        LWLockRelease(GetSessionContext()->shmem_lockid);

        tid = &rettuple->t_data->t_ctid;

        if (!(plan = SPI_prepare("DELETE FROM ora_alerts WHERE ctid = $1", 1, argtypes)))
            ereport(ERROR, (errcode(ERRCODE_TRIGGERED_ACTION_EXCEPTION), errmsg("SPI_prepare failed")));

        values[0] = ItemPointerGetDatum(tid);

        if (SPI_OK_DELETE != SPI_execute_plan(plan, values, nulls, false, 1))
            ereport(ERROR, (errcode(ERRCODE_TRIGGERED_ACTION_EXCEPTION), errmsg("can't execute sql")));

        SPI_finish();
        return PointerGetDatum(rettuple);
    }
    WATCH_POST(timeout, endtime, cycle);
    LOCK_ERROR();

    PG_RETURN_NULL();
}

/*
 *
 *  PROCEDURE GMS_ALERT.SIGNAL(name IN VARCHAR2,message IN VARCHAR2);
 *
 *  Signals the occurrence of alert name and attaches message. (Sessions
 *  registered for alert name are notified only when the signaling transaction
 *  commits.)
 *
 */

/*

CREATE OR REPLACE FUNCTION gms_alert.signal(_event text, _message text) RETURNS void AS $$
BEGIN
  PERFORM 1 FROM pg_catalog.pg_class c
            WHERE pg_catalog.pg_table_is_visible(c.oid)
            AND c.relkind='r' AND c.relname = 'ora_alerts';
  IF NOT FOUND THEN
    CREATE TEMP TABLE ora_alerts(event text, message text) WITH OIDS;
    REVOKE ALL ON TABLE ora_alerts FROM PUBLIC;
    CREATE CONSTRAINT TRIGGER ora_alert_signal AFTER INSERT ON ora_alerts
      INITIALLY DEFERRED FOR EACH ROW EXECUTE PROCEDURE gms_alert._defered_signal();
  END IF;
  INSERT INTO ora_alerts(event, message) VALUES(_event, _message);
END;
$$ LANGUAGE plpgsql VOLATILE SECURITY DEFINER;
*/

#define SPI_EXEC(cmd, _type_)                \
    if (SPI_OK_##_type_ != SPI_exec(cmd, 1)) \
        ereport(ERROR,                       \
                (errcode(ERRCODE_INTERNAL_ERROR), errmsg("SPI execute error"), errdetail("Can't execute %s.", cmd)));

Datum gms_alert_signal(PG_FUNCTION_ARGS)
{
    SPIPlanPtr plan;
    Oid argtypes[] = {TEXTOID, TEXTOID};
    Datum values[2];
    char nulls[2] = {' ', ' '};

    if (PG_ARGISNULL(0))
        ereport(ERROR, (errcode(ERRCODE_NULL_VALUE_NOT_ALLOWED), errmsg("event name is NULL"),
                        errdetail("Eventname may not be NULL.")));

    if (PG_ARGISNULL(1))
        nulls[1] = 'n';

    values[0] = PG_GETARG_DATUM(0);
    values[1] = PG_GETARG_DATUM(1);

    if (SPI_connect() < 0)
        ereport(ERROR, (errcode(ERRCODE_INTERNAL_ERROR), errmsg("SPI_connect failed")));

    SPI_EXEC("SELECT 1 FROM pg_catalog.pg_class c "
             "WHERE pg_catalog.pg_table_is_visible(c.oid) "
             "AND c.relkind='r' AND c.relname = 'ora_alerts'",
             SELECT);
    if (0 == SPI_processed) {
        SPI_EXEC("CREATE TABLE ora_alerts(event text, message text, session_id bigint)", UTILITY);
        SPI_EXEC("REVOKE ALL ON TABLE ora_alerts FROM PUBLIC", UTILITY);
        SPI_EXEC("CREATE CONSTRAINT TRIGGER ora_alert_signal AFTER INSERT ON ora_alerts "
                 "INITIALLY DEFERRED FOR EACH ROW EXECUTE PROCEDURE gms_alert.defered_signal()",
                 UTILITY);
    }

    if (!(plan = SPI_prepare(
              "INSERT INTO ora_alerts(event,message,session_id) SELECT $1, $2, (SELECT PG_CURRENT_SESSID())", 2,
              argtypes)))
        ereport(ERROR, (errcode(ERRCODE_INTERNAL_ERROR), errmsg("SPI_prepare failed")));

    if (SPI_OK_INSERT != SPI_execute_plan(plan, values, nulls, false, 1))
        ereport(ERROR, (errcode(ERRCODE_INTERNAL_ERROR), errmsg("can't execute sql")));

    SPI_finish();
    PG_RETURN_VOID();
}

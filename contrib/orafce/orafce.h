#ifndef __ORAFCE__
#define __ORAFCE__

#include "postgres.h"
#include "catalog/catversion.h"
#include "nodes/pg_list.h"
#include <sys/time.h>
#include "utils/datetime.h"
#include "utils/datum.h"
#include "utils/date.h"
#define MAX_MULTIBYTE_CHAR_LEN  4
#include "storage/lock/lwlock.h"

#include "commands/extension.h"
#include "pipe.h"

#ifndef PLVLEX_D
#include "plvlex.h"
#endif

typedef LWLock *LWLockId;

#define TextPCopy(t) \
	DatumGetTextP(datumCopy(PointerGetDatum(t), false, -1))

#define PG_GETARG_IF_EXISTS(n, type, defval) \
	((PG_NARGS() > (n) && !PG_ARGISNULL(n)) ? PG_GETARG_##type(n) : (defval))

/* alignment of this struct must fit for all types */
typedef union vardata
{
	char	c;
	short	s;
	int		i;
	long	l;
	float	f;
	double	d;
	void   *p;
} vardata;

extern int ora_instr(text *txt, text *pattern, int start, int nth);
extern int ora_mb_strlen(text *str, char **sizes, int **positions);
extern int ora_mb_strlen1(text *str);


/*
 * Version compatibility
 */

extern Oid	equality_oper_funcid(Oid argtype);

/*
 * Date utils
 */
#define STRING_PTR_FIELD_TYPE const char *const

extern STRING_PTR_FIELD_TYPE ora_days[];

extern int ora_seq_search(const char *name, STRING_PTR_FIELD_TYPE array[], size_t max);

#ifdef _MSC_VER

#define int2size(v)			(size_t)(v)
#define size2int(v)			(int)(v)

#else

#define int2size(v)			v
#define size2int(v)			v

#endif

#endif

typedef enum {
    IT_NO_MORE_ITEMS = 0,
    IT_NUMBER = 9,
    IT_VARCHAR = 11,
    IT_DATE = 12,
    IT_TIMESTAMPTZ = 13,
    IT_BYTEA = 23,
    IT_RECORD = 24
} message_data_type;

typedef struct {
    int32 size;
    message_data_type type;
    Oid tupType;
} message_data_item;

typedef struct {
    int32 size;
    int32 items_count;
    message_data_item *next;
} message_buffer;

typedef struct WeekDays
{
    int	encoding;
    const char *names[7];
} WeekDays;

typedef struct OraFileSlot
{
    FILE   *file;
    int	   max_linesize;
    int	   encoding;
    int32  id;
} OraFileSlot;

typedef struct {
    char day;
    char month;
} holiday_desc;

typedef struct {
	size_t size;
	void* first_byte_ptr;
	bool dispossible;
/*	int16 context; */
} list_item;

typedef struct {
	bool is_valid;
	bool registered;
	char *pipe_name;
	char *creator;
	Oid  uid;
	struct _queue_item *items;
	int16 count;
	int16 limit;
	int size;
} orafce_pipe;

#define MAX_SLOTS		50			/* Oracle 10g supports 50 files */
#define MAX_holidays   30
#define MAX_EXCEPTIONS 50

typedef struct orafce_session_context {
    char* nls_date_format;
    char* orafce_timezone;
    bool orafce_varchar2_null_safe_concat;
    /* pipe.c */
    message_buffer *output_buffer = NULL;
    message_buffer *input_buffer = NULL;
    int pipe_sid;                                 /* session id */
    orafce_pipe* pipes = NULL;
    LWLockId shmem_lockid;
//    LWLockTranche tranche;
    /* putline.c */
    bool is_server_output = false;
    char *buffer = NULL;
    int  buffer_size = 0;	/* allocated bytes in buffer */
    int  buffer_len = 0;	/* used bytes in buffer */
    int  buffer_get = 0;	/* retrieved bytes in buffer */
    /* convert.c */
    FmgrInfo *orafce_Utf8ToServerConvProc = NULL;
    /* datefce.c */
    const WeekDays *mru_weekdays = NULL;
    /* file.c */
    OraFileSlot	slots[MAX_SLOTS];	/* initilaized with zeros */
    int32	slotid = 0;	/* next slot id */
    /* others.c */
    char *lc_collate_cache = NULL;
    size_t multiplication = 1;
    text *def_locale = NULL;
    /* plvdate.c */
    bool use_easter = true;
    bool use_great_friday = true;
    bool include_start = true;
    int country_id = -1;	/* unknown */
    holiday_desc holidays[MAX_holidays];  /* sorted array */
    DateADT exceptions[MAX_EXCEPTIONS];   /* sorted array */
    int holidays_c = 0;
    int exceptions_c = 0;
    /* plvlex.c */
    orafce_lexnode *__node;
    char *__result;
    int __len;
    /* sqlparse.y */
    char *scanbuf;
    int	scanbuflen;
    /* sqlscan.l */
    int	xcdepth = 0;	/* depth of nesting in slash-star comments */
    char *dolqstart;      /* current $foo$ quote start string */
    bool extended_string = false;
    /* plvsubst.c */
    text *c_subst = NULL;
    /* random.c */
    unsigned int seed = 0;
    /* alert.c */
    alert_event *alert_events;
    alert_lock  *alert_locks;
    alert_lock *alert_session_lock;
    float8 sensitivity;
    /* shmmc.c */
    int *list_c = NULL;
    list_item *list = NULL;
    size_t max_size;
} orafce_session_context;

extern uint32 orafce_index;

void set_extension_index(uint32 index);
void init_session_vars(void);
orafce_session_context* get_session_context();


/*-------------------------------------------------------------------------
 *
 * connection_configuration.c
 *   Functions for controlling configuration of Citus connections
 *
 * Copyright (c) Citus Data, Inc.
 *
 *-------------------------------------------------------------------------
 */

#include "postgres.h"

#include "access/transam.h"
#include "access/xact.h"
#include "mb/pg_wchar.h"
#include "postmaster/postmaster.h"
#include "utils/builtins.h"

#include "distributed/backend_data.h"
#include "distributed/citus_safe_lib.h"
#include "distributed/connection_management.h"
#include "distributed/intermediate_result_pruning.h"
#include "distributed/metadata_cache.h"
#include "distributed/session_ctx.h"
#include "distributed/worker_manager.h"

char* NodeConninfo = "sslmode=disable";

/* represents a list of libpq parameter settings */
typedef struct ConnParamsInfo {
    char** keywords; /* libpq keywords */
    char** values;   /* desired values for above */
    Size size;       /* current used size of arrays */
    Size maxSize;    /* maximum allocated size of arrays (similar to e.g. StringInfo) */
} ConnParamsInfo;

static ConnParamsInfo ConnParams;

static Size CalculateMaxSize(void);
static int uri_prefix_length(const char* connstr);

/*
 * InitConnParams initializes the ConnParams field to point to enough memory to
 * store settings for every valid libpq value, though these regions are set to
 * zeros from the outset and the size appropriately also set to zero.
 *
 * This function must be called before others in this file, though calling it
 * after use of the initialized ConnParams structure will result in any
 * populated parameter settings being lost.
 */
void InitConnParams()
{
    Size maxSize = CalculateMaxSize();
    ConnParamsInfo connParams = {.keywords = (char**)malloc(maxSize * sizeof(char*)),
                                 .values = (char**)malloc(maxSize * sizeof(char*)),
                                 .size = 0,
                                 .maxSize = maxSize};

    memset(connParams.keywords, 0, maxSize * sizeof(char*));
    memset(connParams.values, 0, maxSize * sizeof(char*));

    ConnParams = connParams;
}

/*
 * ResetConnParams frees all strings in the keywords and parameters arrays,
 * sets their elements to null, and resets the ConnParamsSize to zero before
 * adding back any hardcoded global connection settings (at present, there
 * are no).
 */
void ResetConnParams()
{
    for (Size paramIdx = 0; paramIdx < ConnParams.size; paramIdx++) {
        free((void*)ConnParams.keywords[paramIdx]);
        free((void*)ConnParams.values[paramIdx]);

        ConnParams.keywords[paramIdx] = ConnParams.values[paramIdx] = NULL;
    }

    ConnParams.size = 0;

    InvalidateConnParamsHashEntries();
}

/*
 * AddConnParam adds a parameter setting to the global libpq settings according
 * to the provided keyword and value.
 */
void AddConnParam(const char* keyword, const char* value)
{
    auto& connParams = ConnParams;

    if (connParams.size + 1 >= connParams.maxSize) {
        /* hopefully this error is only seen by developers */
        ereport(ERROR, (errcode(ERRCODE_INSUFFICIENT_RESOURCES),
                        errmsg("ConnParams arrays bound check failed")));
    }

    /*
     * Don't use pstrdup here to avoid being tied to a memory context, we free
     * these later using ResetConnParams
     */
    connParams.keywords[connParams.size] = strdup(keyword);
    connParams.values[connParams.size] = strdup(value);
    connParams.size++;

    connParams.keywords[connParams.size] = connParams.values[connParams.size] = NULL;
}

/*
 * CheckConninfo is a building block to help implement check constraints and
 * other check hooks against libpq-like conninfo strings. In particular, the
 * provided conninfo must:
 *
 *   - Not use a uri-prefix such as postgres:// (it must be only keys and values)
 *   - Parse using PQconninfoParse
 *   - Only set keywords contained in the provided allowedConninfoKeywords
 *
 * This function returns true if all of the above are satisfied, otherwise it
 * returns false. If the provided errmsg pointer is not NULL, it will be set
 * to an appropriate message if the check fails.
 *
 * The provided allowedConninfoKeywords must be sorted in a manner usable by bsearch,
 * though this is only validated during assert-enabled builds.
 */
bool CheckConninfo(const char* conninfo, const char** allowedConninfoKeywords,
                   Size allowedConninfoKeywordsLength, char** errorMsg)
{
    PQconninfoOption* option = NULL;
    char* errorMsgString = NULL;

    /*
     * If the user doesn't need a message, just overwrite errmsg with a stack
     * variable so we can always safely write to it.
     */
    if (errorMsg == NULL) {
        errorMsg = &errorMsgString;
    }

    /* sure, it can be null */
    if (conninfo == NULL) {
        return true;
    }

    /* the libpq prefix form is more complex than we need; ban it */
    if (uri_prefix_length(conninfo) != 0) {
        *errorMsg = "Citus connection info strings must be in "
                    "'k1=v1 k2=v2 [...] kn=vn' format";

        return false;
    }

    /* this should at least parse */
    PQconninfoOption* optionArray = PQconninfoParse(conninfo, NULL);
    if (optionArray == NULL) {
        *errorMsg = "Provided string is not a valid libpq connection info string";

        return false;
    }

#ifdef USE_ASSERT_CHECKING

    /* verify that the allowedConninfoKeywords is in ascending order */
    for (Size keywordIdx = 1; keywordIdx < allowedConninfoKeywordsLength; keywordIdx++) {
        const char* prev = allowedConninfoKeywords[keywordIdx - 1];
        const char* curr = allowedConninfoKeywords[keywordIdx];

        Assert(strcmp(prev, curr) < 0);
    }
#endif

    for (option = optionArray; option->keyword != NULL; option++) {
        if (option->val == NULL || option->val[0] == '\0') {
            continue;
        }

        void* matchingKeyword =
            SafeBsearch(&option->keyword, allowedConninfoKeywords,
                        allowedConninfoKeywordsLength, sizeof(char*), pg_qsort_strcmp);
        if (matchingKeyword == NULL) {
            /* the allowedConninfoKeywords lacks this keyword; error out! */
            StringInfoData msgString;
            initStringInfo(&msgString);

            appendStringInfo(&msgString, "Prohibited conninfo keyword detected: %s",
                             option->keyword);

            *errorMsg = msgString.data;

            break;
        }
    }

    PQconninfoFree(optionArray);

    /* if error message is set we found an invalid keyword */
    return (*errorMsg == NULL);
}

/*
 * GetConnParams uses the provided key to determine libpq parameters needed to
 * establish a connection using that key. The keywords and values are placed in
 * the like-named out parameters. All parameter strings are allocated in the
 * context provided by the caller, to save the caller needing to copy strings
 * into an appropriate context later.
 */
void GetConnParams(ConnectionHashKey* key, char*** keywords, char*** values,
                   Index* runtimeParamStart, MemoryContext context)
{
    /*
     * make space for the port as a string: sign, 10 digits, NUL. We keep it on the stack
     * till we can later copy it to the right context. By having the declaration here
     * already we can add a pointer to the runtimeValues.
     */
    char nodePortString[12] = "";
    ConnectionHashKey* effectiveKey = key;

    StringInfo applicationName = makeStringInfo();
    appendStringInfo(applicationName, "%s%ld", SPQ_APPLICATION_NAME_PREFIX,
                     GetGlobalPID());

    /*
     * This function has three sections:
     *   - Initialize the keywords and values (to be copied later) of global parameters
     *   - Append user/host-specific parameters calculated from the given key
     *   - (Enterprise-only) append user/host-specific authentication params
     *
     * The global parameters have already been assigned from a GUC, so begin by
     * calculating the key-specific parameters (basically just the fields of
     * the key and the active database encoding).
     *
     * We allocate everything in the provided context so as to facilitate using
     * pfree on all runtime parameters when connections using these entries are
     * invalidated during config reloads.
     *
     * Also, when "host" is already provided in global parameters, we use hostname
     * from the key as "hostaddr" instead of "host" to avoid host name lookup. In
     * that case, the value for "host" becomes useful only if the authentication
     * method requires it.
     */
    bool gotHostParamFromGlobalParams = false;
    auto& connParams = ConnParams;
    for (Size paramIndex = 0; paramIndex < connParams.size; paramIndex++) {
        if (strcmp(connParams.keywords[paramIndex], "host") == 0) {
            gotHostParamFromGlobalParams = true;
            break;
        }
    }

    const char* runtimeKeywords[] = {gotHostParamFromGlobalParams ? "hostaddr" : "host",
                                     "port",
                                     "dbname",
                                     "user",
                                     "client_encoding",
                                     "application_name",
                                     "options"};
    const char* runtimeValues[] = {effectiveKey->hostname,     nodePortString,
                                   effectiveKey->database,     effectiveKey->user,
                                   GetDatabaseEncodingName(),  applicationName->data,
                                   "-c remotetype=coordinator"};

    /*
     * remember where global/GUC params end and runtime ones start, all entries after this
     * point should be allocated in context and will be freed upon
     * FreeConnParamsHashEntryFields
     */
    *runtimeParamStart = connParams.size;

    /*
     * Declare local params for readability;
     *
     * assignment is done directly to not lose the pointers if any of the later
     * allocations cause an error. FreeConnParamsHashEntryFields knows about the
     * possibility of half initialized keywords or values and correctly reclaims them when
     * the cache is reused.
     *
     * Need to zero enough space for all possible libpq parameters.
     */
    char** connKeywords = *keywords =
        (char**)MemoryContextAllocZero(context, connParams.maxSize * sizeof(char*));
    char** connValues = *values =
        (char**)MemoryContextAllocZero(context, connParams.maxSize * sizeof(char*));

    /* auth keywords will begin after global and runtime ones are appended */
    Index authParamsIdx = connParams.size + lengthof(runtimeKeywords);

    if (connParams.size + lengthof(runtimeKeywords) >= connParams.maxSize) {
        /* hopefully this error is only seen by developers */
        ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                        errmsg("too many connParams entries")));
    }

    pg_ltoa(effectiveKey->port, nodePortString); /* populate node port string with port */

    /* first step: copy global parameters to beginning of array */
    for (Size paramIndex = 0; paramIndex < connParams.size; paramIndex++) {
        /* copy the keyword&value pointers to the new array */
        connKeywords[paramIndex] = connParams.keywords[paramIndex];
        connValues[paramIndex] = connParams.values[paramIndex];
    }

    /* second step: begin after global params and copy runtime params into our context */
    for (Index runtimeParamIndex = 0; runtimeParamIndex < lengthof(runtimeKeywords);
         runtimeParamIndex++) {
        /* copy the keyword & value into our context and append to the new array */
        connKeywords[connParams.size + runtimeParamIndex] =
            MemoryContextStrdup(context, runtimeKeywords[runtimeParamIndex]);
        connValues[connParams.size + runtimeParamIndex] =
            MemoryContextStrdup(context, runtimeValues[runtimeParamIndex]);
    }

    /* we look up authinfo by original key, not effective one */
    char* authinfo = GetAuthinfo(key->hostname, key->port, key->user);
    char* pqerr = NULL;
    PQconninfoOption* optionArray = PQconninfoParse(authinfo, &pqerr);
    if (optionArray == NULL) {
        /* PQconninfoParse failed, it's unsafe to continue as this has caused segfaults in
         * production */
        if (pqerr == NULL) {
            /* parse failed without an error message, treat as OOM error  */
            ereport(ERROR,
                    (errcode(ERRCODE_OUT_OF_MEMORY), errmsg("out of memory"),
                     errdetail("Failed to parse authentication information via libpq")));
        } else {
            /*
             * Parse error, should not be possible as the validity is checked upon insert
             * into pg_dist_authinfo, however, better safe than sorry
             */

            /*
             * errmsg is populated by PQconninfoParse which requires us to free the
             * message. Since we want to incorporate the parse error into the detail of
             * our message we need to copy the error message before freeing it. Not
             * freeing the message will leak memory.
             */
            char* pqerrcopy = pstrdup(pqerr);
            PQfreemem(pqerr);

            ereport(
                ERROR,
                (errmsg("failed to parse node authentication information for %s@%s:%d",
                        key->user, key->hostname, key->port),
                 errdetail("%s", pqerrcopy)));
        }
    }

    for (PQconninfoOption* option = optionArray; option->keyword != NULL; option++) {
        if (option->val == NULL || option->val[0] == '\0') {
            continue;
        }

        connKeywords[authParamsIdx] = MemoryContextStrdup(context, option->keyword);
        connValues[authParamsIdx] = MemoryContextStrdup(context, option->val);

        authParamsIdx++;
    }

    if (key->replicationConnParam) {
        connKeywords[authParamsIdx] = MemoryContextStrdup(context, "replication");
        connValues[authParamsIdx] = MemoryContextStrdup(context, "database");

        authParamsIdx++;
    }

    PQconninfoFree(optionArray);

    /* final step: add terminal NULL, required by libpq */
    connKeywords[authParamsIdx] = connValues[authParamsIdx] = NULL;
}

/*
 * GetConnParam finds the keyword in the configured connection parameters and returns its
 * value.
 */
const char* GetConnParam(const char* keyword)
{
    auto& connParams = ConnParams;

    for (Size i = 0; i < connParams.size; i++) {
        if (strcmp(keyword, connParams.keywords[i]) == 0) {
            return connParams.values[i];
        }
    }

    return NULL;
}

/*
 * GetAuthinfo simply returns the string representation of authentication info
 * for a specified hostname/port/user combination. If the current transaction
 * is valid, then we use the catalog, otherwise a shared memory hash is used,
 * a mode that is currently only useful for getting authentication information
 * to the Task Tracker, which lacks a database connection and transaction.
 * @TODO: do we need this ?
 */
char* GetAuthinfo(char* hostname, int32 port, char* user)
{
    char* authinfo = NULL;
    return (authinfo != NULL) ? authinfo : (char*)"";
}

/*
 * CalculateMaxSize simply counts the number of elements returned by
 * PQconnDefaults, including the final NULL. This helps us know how space would
 * be used if a connection utilizes every known libpq parameter.
 */
static Size CalculateMaxSize()
{
    PQconninfoOption* defaults = PQconndefaults();
    Size maxSize = 0;

    for (PQconninfoOption* option = defaults; option->keyword != NULL;
         option++, maxSize++) {
        /* do nothing, we're just counting the elements */
    }

    PQconninfoFree(defaults);

    /* we've counted elements but libpq needs a final NULL, so add one */
    maxSize++;

    return maxSize;
}

/* *INDENT-OFF* */

/*
 * Checks if connection string starts with either of the valid URI prefix
 * designators.
 *
 * Returns the URI prefix length, 0 if the string doesn't contain a URI prefix.
 *
 * This implementation (mostly) taken from libpq/fe-connect.c.
 */
static int uri_prefix_length(const char* connstr)
{
    const char uri_designator[] = "postgresql://";
    const char short_uri_designator[] = "postgres://";

    if (strncmp(connstr, uri_designator, sizeof(uri_designator) - 1) == 0)
        return sizeof(uri_designator) - 1;

    if (strncmp(connstr, short_uri_designator, sizeof(short_uri_designator) - 1) == 0)
        return sizeof(short_uri_designator) - 1;

    return 0;
}

/* *INDENT-ON* */

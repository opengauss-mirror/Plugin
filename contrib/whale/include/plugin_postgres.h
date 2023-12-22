#ifndef PLUGIN_POSTGRES_H
#define PLUGIN_POSTGRES_H
#include "plugin_orafce/orafce.h"

#include "executor/spi.h"

#if defined _WIN32 || defined __CYGWIN__
  #ifdef BUILDING_DLL
    #ifdef __GNUC__
      #define DLL_PUBLIC __attribute__ ((dllexport))
    #else
      #define DLL_PUBLIC __declspec(dllexport) // Note: actually gcc seems to also supports this syntax.
    #endif
  #else
    #ifdef __GNUC__
      #define DLL_PUBLIC __attribute__ ((dllimport))
    #else
      #define DLL_PUBLIC __declspec(dllimport) // Note: actually gcc seems to also supports this syntax.
    #endif
  #endif
  #define DLL_LOCAL
#else
  #if __GNUC__ >= 4
    #define DLL_PUBLIC __attribute__ ((visibility ("default")))
    #define DLL_LOCAL  __attribute__ ((visibility ("hidden")))
  #else
    #define DLL_PUBLIC
    #define DLL_LOCAL
  #endif
#endif

#define PG_MODULE_MAGIC_PUBLIC                                                         \
    extern "C" PGDLLEXPORT DLL_PUBLIC const Pg_magic_struct* PG_MAGIC_FUNCTION_NAME(void); \
    const Pg_magic_struct* PG_MAGIC_FUNCTION_NAME(void)                         \
    {                                                                           \
        static const Pg_magic_struct Pg_magic_data = PG_MODULE_MAGIC_DATA;      \
        return &Pg_magic_data;                                                  \
    }                                                                           \
    extern int no_such_variable

#define PG_FUNCTION_INFO_V1_PUBLIC(funcname)                                                   \
    extern "C" PGDLLEXPORT DLL_PUBLIC const Pg_finfo_record* CppConcat(pg_finfo_, funcname)(void); \
    const Pg_finfo_record* CppConcat(pg_finfo_, funcname)(void)                         \
    {                                                                                   \
        static const Pg_finfo_record my_finfo = {1};                                    \
        return &my_finfo;                                                               \
    }                                                                                   \
    extern int no_such_variable

typedef enum { QUERY_MESSAGE = 0, HYBRID_MESSAGE } MessageType;

extern "C" DLL_PUBLIC void whale_invoke(void);
extern "C" DLL_PUBLIC void init_plugin_object();
extern "C" DLL_PUBLIC void init_session_vars(void);
extern "C" DLL_PUBLIC void create_whale_extension();
extern "C" DLL_PUBLIC void set_extension_index(uint32 index);

typedef  struct ASqlPluginContext {
    bool enableACmptMode;
    struct HTAB* a_stmtInputTypeHash;
    struct HTAB* a_sendBlobHash;
    char* default_database_name;
    bool is_a_declare;
    bool is_whale_call_stmt;

    // come from orafce
    char* nls_date_format;
    char* orafce_timezone;
    bool orafce_varchar2_null_safe_concat;
    /* pipe.c */
    message_buffer *output_buffer = NULL;
    message_buffer *input_buffer = NULL;
    int pipe_sid;                                 /* session id */
    orafce_pipe* pipes = NULL;
    LWLockId shmem_lockid;
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
    SPIPlanPtr safe_named_location_plan = NULL;
    SPIPlanPtr check_secure_locality_plan = NULL;
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
    char *dolqstart;	/* current $foo$ quote start string */
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
} aSqlPluginContext;

ASqlPluginContext* GetSessionContext();

#endif

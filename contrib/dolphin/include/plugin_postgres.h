#ifndef PLUGIN_POSTGRES_H
#define PLUGIN_POSTGRES_H

#include "nodes/pg_list.h"
#include "fmgr.h"

#define ENABLE_B_CMPT_MODE (GetSessionContext()->enableBCmptMode)

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

extern "C" DLL_PUBLIC void dolphin_invoke(void);
extern "C" DLL_PUBLIC void init_plugin_object();
extern "C" DLL_PUBLIC void init_session_vars(void);
extern "C" DLL_PUBLIC void create_dolphin_extension();

typedef struct BSqlPluginContext {
    bool enableBCmptMode;
    char* sqlModeString;
    unsigned int sqlModeFlags;
    List* lockNameList;
    double b_db_timestamp;
    int lower_case_table_names;
    int default_week_format;
    char* lc_time_names;
    bool scan_from_pl;
    char* default_database_name;
    int paramIdx;
#ifdef DOLPHIN
    char* version_comment;
    int auto_increment_increment;
    char* character_set_client;
    char* character_set_connection;
    char* character_set_results;
    char* character_set_server;
    char* collation_server;
    char* collation_connection;
    char* init_connect;
    int interactive_timeout;
    char* license;
    int max_allowed_packet;
    int net_buffer_length;
    int net_write_timeout;
    long int query_cache_size;
    int query_cache_type;
    char* system_time_zone;
    char* time_zone;
    int wait_timeout;
#endif
} bSqlPluginContext;

BSqlPluginContext* GetSessionContext();

#define PG_RETURN_UINT64(x) return UInt64GetDatum(x)
#define PG_GETARG_UINT64(n) DatumGetUInt64(PG_GETARG_DATUM(n))
#define PG_GETARG_UINT32(n) DatumGetUInt32(PG_GETARG_DATUM(n))
#define PG_GETARG_UINT64(n) DatumGetUInt64(PG_GETARG_DATUM(n))
#define DEFAULT_GUC_B_DB_TIMESTAMP 0.0
#define MAX_GUC_B_DB_TIMESTAMP 2147483647.0
#ifdef DOLPHIN
#define DEFAULT_GUC_WEEK_FORMAT 0
#define MAX_GUC_WEEK_FORMAT 7
#define DEFAULT_AUTO_INCREMENT 1
#define MIN_AUTO_INCREMENT 1
#define MAX_AUTO_INCREMENT 65535
#define DEFAULT_INTERACTIVE_TIMEOUT 28800
#define MIN_INTERACTIVE_TIMEOUT 1
#define MAX_INTERACTIVE_TIMEOUT 31536000
#define DEFAULT_MAX_ALLOWED_PACKET 4194304
#define MIN_MAX_ALLOWED_PACKET 1024
#define MAX_MAX_ALLOWED_PACKET 1073741824
#define DEFAULT_NET_BUFFER_LENGTH 16384
#define MIN_NET_BUFFER_LENGTH 1024
#define MAX_NET_BUFFER_LENGTH 1048576
#define DEFAULT_NET_WRITE_TIMEOUT 60
#define MIN_NET_WRITE_TIMEOUT 1
#define MAX_NET_WRITE_TIMEOUT 31536000
#define DEFAULT_QUREY_CACHE_SIZE 1048576
#define MIN_QUREY_CACHE_SIZE 0
#define MAX_QUREY_CACHE_SIZE LONG_MAX
#define QUERY_CACHE_OFF 0
#define QUERY_CACHE_ON 1
#define QUERY_CACHE_DEMAND 2
#define DEFAULT_WAIT_TIMEOUT 28800
#define MIN_WAIT_TIMEOUT 1
#define MAX_WAIT_TIMEOUT 31536000
#endif

typedef enum {
    INVALID_OID = -1,
    UINT1_OID,
    UINT2_OID,
    UINT4_OID,
    UINT8_OID,
    YEAR_OID
} addedType;

#endif

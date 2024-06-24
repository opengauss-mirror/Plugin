#ifndef PLUGIN_POSTGRES_H
#define PLUGIN_POSTGRES_H

#include "nodes/pg_list.h"
#include "nodes/primnodes.h"
#include "plugin_utils/fmgr.h"

#define ENABLE_B_CMPT_MODE (GetSessionContext()->enableBCmptMode)
#define ENABLE_NULLS_MINIMAL_POLICY_MODE (!u_sess->attr.attr_common.IsInplaceUpgrade && GetSessionContext()->enable_nulls_minimal_policy)

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
extern "C" DLL_PUBLIC void set_extension_index(uint32 index);

#define FLOAT8PLOID 591
#define FLOAT8MIOID 592
#define FLOAT8MULOID 594
#define FLOAT8DIVOID 593
#define TINYINT_LENGTH 3
#define SMALLINT_LENGTH 5
#define INTEGER_LENGTH 10
#define BIGINT_LENGTH 20

/*
 * "+", "-", "*", "/"
 */
typedef enum A_ExprSubKind {
    OTHERS = -1,
    AEXPR_PLUS_INT4,
    AEXPR_PLUS_INT8,
    AEXPR_MINUS_INT4,
    AEXPR_MINUS_INT8,
    AEXPR_MUL_INT4,
    AEXPR_MUL_INT8,
    AEXPR_DIV_INT4,
    AEXPR_DIV_INT8,
    SUB_KIND_LENGTH
} A_ExprSubKind;

typedef enum DataKind {
    INVALID_OP = -1,
    INT_OP,
    INT_UINT_OP,
    UINT_INT_OP,
    UINT_OP,
    REAL_OP,
    DECIMAL_OP,
    DATA_KIND_LENGTH
} DataKind;

#define TINYBLOBOID (GetSessionContext()->tinyblobOid)
#define MEDIUMBLOBOID (GetSessionContext()->mediumblobOid)
#define LONGBLOBOID (GetSessionContext()->longblobOid)
#define BINARYOID (GetSessionContext()->binaryOid)
#define VARBINARYOID (GetSessionContext()->varbinaryOid)
#define UINT1OID (GetSessionContext()->uint1Oid)
#define UINT2OID (GetSessionContext()->uint2Oid)
#define UINT4OID (GetSessionContext()->uint4Oid)
#define UINT8OID (GetSessionContext()->uint8Oid)
#define YEAROID (GetSessionContext()->yearOid)

typedef struct BSqlPluginContext {
    bool enableBCmptMode;
    bool enable_nulls_minimal_policy;
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
    bool isUpsert;
#ifdef DOLPHIN
    Oid tinyblobOid;
    Oid mediumblobOid;
    Oid longblobOid;
    Oid binaryOid;
    Oid varbinaryOid;
    Oid uint1Oid;
    Oid uint2Oid;
    Oid uint4Oid;
    Oid uint8Oid;
    Oid yearOid;
    Oid dolphin_oprs[SUB_KIND_LENGTH][DATA_KIND_LENGTH];
    char* version_comment;
    int auto_increment_increment;
    char* character_set_client;
    char* character_set_results;
    char* character_set_server;
    char* collation_server;
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
    int single_line_trigger_begin;
    char* do_sconst;
    int single_line_proc_begin;
    char* optimizer_switch_string;
    unsigned int optimizer_switch_flags;
    int div_precision_increment;
    int dolphin_kw_mask;
    bool is_schema_name;
    bool is_first_lable;
    bool is_b_declare;
    bool group_by_error;
    char* useless_sql_mode;
    int useless_lower_case_table_names;
    Alias *upSertAliasName;
    bool is_create_alter_stmt;
    bool isDoCopy;
    char* performance_schema;
    bool isInTransformSet;
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

typedef struct optimizer_switch_entry {
    const char* name; /* name of optimizer_switch entry */
    int flag;         /* bit flag position */
    bool defaultValue;
} optimizer_switch_entry;

#define OPT_USE_INVISIBLE_INDEXES 1
#define OPT_OPTIMIZER_SWITCH_MAX 1

#define USE_INVISIBLE_INDEXES (GetSessionContext()->optimizer_switch_flags & OPT_USE_INVISIBLE_INDEXES)

static const struct optimizer_switch_entry optimizer_switch_options[OPT_OPTIMIZER_SWITCH_MAX] = {
    {"use_invisible_indexes", OPT_USE_INVISIBLE_INDEXES, false}
};
#define DEFAULT_DIV_PRECISION_INC 4
#define MIN_DIV_PRECISION_INC 0
#define MAX_DIV_PRECISION_INC 30
#define B_KWMASK_CREATE 0x01
#define B_KWMASK_TRIGGER (0x01<<1)
#define B_KWMASK_CREATE_TRIGGER  (0x01 | (0x01<<1))
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

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

extern "C" DLL_PUBLIC void _PG_init(void);
extern "C" DLL_PUBLIC void _PG_fini(void);
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
} bSqlPluginContext;

BSqlPluginContext* GetSessionContext();

#define DEFAULT_GUC_B_DB_TIMESTAMP 0.0
#define MAX_GUC_B_DB_TIMESTAMP 2147483647.0

#endif
# access

## xact.h

在枚举XactEvent中加入了

XACT_EVENT_PARALLEL_ABORT

XACT_EVENT_PRE_COMMIT

在枚举SubXactEvent中加入了

SUBXACT_EVENT_PRE_COMMIT_SUB

# catalog

## pg_aggregate.h

在结构体FormData_pg_aggregate中加入了

Oid aggcombinefn

Oid aggdeserialfn

Oid aggserialfn

int32 aggtransspace

bool aggfinalextra

## pg_authid.h

在结构体FormData_pg_authid加入了

rolbypassrls

## pg_class.h

在结构FormData_pg_class加入了

relrowsecurity

relforcerowsecurity

relminmxid

## pg_proc.h

在结构体FormData_pg_proc中加入了

proparallel

# commands

## explain.h

在结构ExplainState中加入了 

 deparse_cxt

# executor

## tuptable.h

在结构TupleTableSlot 加入

tts_tuple

# nodes

## execnodes.h

在结构ResultRelInfo加入

ri_WithCheckOptions

ri_WithCheckOptionExprs

ri_onConflictSetProj

ri_onConflictSetWhere

在结构ModifyTableState加入

mt_arbiterindexes

mt_existing

mt_conflproj

## nodes.h

在枚举NodeTag加入了

​    
​    T_Result,
​    T_SampleScan,
​    T_ModifyTablePath,
​    T_AggPath,
​    T_EventTriggerData,
​    T_RoleSpec,
​    T_ForeignKeyCacheInfo,
​    T_GatherState,
​    T_WindowAggPath,
​    T_SortPath,
​    T_MinMaxAggPath,
​    T_GatherPath,
​    T_Gather,
​    T_PathTarget,
​    T_ProjectionPath

## parsenodes_common.h

在ObjectType 加入 OBJECT_TABCONSTRAINT
在CreateSchemaStmt加入authrole、if_not_exists

在AlterTableType加入
AT_EnableRowSecurity,
AT_DisableRowSecurity,
AT_ForceRowSecurity,
AT_NoForceRowSecurity,
AT_SetUnLogged,
AT_AlterConstraint,
AT_ReAddComment,
AT_SetLogged,

在AlterTableCmd加入newowner

在ReindexStmt加入options

在ColumnDef加入typeName

在CopyStmt加入is_program

在Query加入onConflict

## parsenodes.h

在IndexStmt加入if_not_exists

## plannodes.h

在结构体ModifyTable加入
Node *onConflictWhere; 
List *onConflictSet; 
Oid onConflictAction; 
List *arbiterIndexes

## primnodes.h

在结构体Aggref加入
Expr* aggfilter;
List* aggargtypes;
int aggsplit;
Oid aggtranstype

在NullTest加入location

## relation.h

在AggClauseCosts加入hasNonPartial、hasNonSerial

在PlannerInfo加入
List* processed_tlist;
struct PathTarget *upper_targets[6];
Bitmapset *outer_params;
List *upper_rels[6];

在RelOptInfo加入
List *partial_pathlist;
bool consider_parallel;
bool consider_startup;
bool consider_param_startup;
struct PathTarget *reltarget; 

在IndexOptInfo加入indrestrictinfo

在Path加入
PathTarget *pathtarget; 
bool parallel_aware;
bool parallel_safe;
int parallel_workers;
struct PathTarget *reltarget; 

# optimizer

## var.h

在PVCAggregateBehavior加入
PVC_RECURSE_WINDOWFUNCS,
PVC_INCLUDE_WINDOWFUNCS,

# postmaster

## bgworker.h

在BgwHandleStatus加入了
BGWH_STOPPED,		
BGWH_POSTMASTER_DIED,
BGWH_STARTED,	
BGWH_NOT_YET_STARTED

在BackgroundWorker加入了
char  bgw_name[64];//tsdb
char  bgw_extra[128];;//tsdb
Datum bgw_main_arg;//tsdb
int	bgw_flags; //tsdb
int bgw_start_time;//tsdb,本为BgWorkerStartTime
int	bgw_restart_time;//tsdb 
char bgw_library_name[64];//tsdb	
char bgw_function_name[64]; 

# storage

## cache_mgr.h

在CacheType加入了
CACHE_TYPE_HYPERTABLE,
CACHE_TYPE_BGW_JOB,
_MAX_CACHE_TYPES

## pg_shmem.h

在PGShmemHeader加入了dsm_control

## proc.h

在struct PGPROC加入了 isBackgroundWorker

# utils

## array.h

在ArrayBuildState加入了private_cxt

## catcache.h

在CatCList加入了members[FLEXIBLE_ARRAY_MEMBER]

## guc.h

在GucSource加入了PGC_S_GLOBAL

## jsonb.h

在Jsonb 加入了root
在struct JsonbValue加入了结构体命名val

## rel.h

在RelationData加入了
bool rd_fkeyvalid;//tsdb
List* rd_fkeylist;//tsdb

## snapshot.h

在SnapshotData加入了speculativeToken
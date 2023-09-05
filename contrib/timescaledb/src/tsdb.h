#ifndef TSDB_H
#define TSDB_H
#include "catalog/namespace.h"
#include "tsdb_head.h"
#include "utils/acl.h"
#include "catalog/dependency.h"
#include "access/rewriteheap.h"
#include "parser/parse_node.h"
#include "optimizer/planner.h"
#include "catalog/pg_inherits_fn.h"
#include "utils/hsearch.h"
#include "storage/ipc.h"
#include "storage/sinvaladt.h"
#include "parser/parse_coerce.h"
#include "access/hash.h"
#include "parser/parse_relation.h"
#include "commands/tablespace.h"
#include "catalog/index.h"
#include "bootstrap/bootstrap.h"
#include "parser/parser.h"
#include "nodes/nodeFuncs.h"
#include "catalog/index.h"
#include "access/heapam.h"
#include "access/genam.h"
#include "commands/vacuum.h"
#include "utils/evtcache.h"
#include "tsdb_head.h"

extern CopyState BeginCopyFrom(Relation rel, const char *filename,
			  bool is_program, List *attnamelist, List *options);
extern ObjectAddress CreateTrigger(CreateTrigStmt *stmt, const char *queryString,
			  Oid relOid, Oid refRelOid, Oid constraintOid, Oid indexOid,
			  bool isInternal,int tsdb = 0); 
extern ObjectAddress DefineRelation(CreateStmt *stmt, char relkind, Oid ownerId,
			   ObjectAddress *typaddress, int tsdb=0);    
extern void heap_endscan(HeapScanDescData*& scan);
extern PGFunction load_external_function(char *filename, char *funcname,
					   bool signalNotFound, void **filehandle,int tsdb=0); 
extern List *make_pathkeys_for_sortclauses(PlannerInfo *root,
							  List *sortclauses,
							  List *tlist);
extern RestrictInfo *
make_restrictinfo(Expr *clause,
				  bool is_pushed_down,
				  bool outerjoin_delayed,
				  bool pseudoconstant,
				  Relids required_relids,
				  Relids outer_relids,
				  Relids nullable_relids);
extern void NewRelationCreateToastTable(Oid relOid, Datum reloptions); 
extern void RegisterCustomScanMethods(const ExtensiblePlanMethods *methods);
extern void ExecARInsertTriggers_tsdb(EState *estate,
					 ResultRelInfo *relinfo,
					 HeapTuple trigtuple,
					 List *recheckIndexes); 
extern Datum slot_getattr(TupleTableSlot *slot, int attnum, bool *isnull);
extern HTSU_Result heap_lock_tuple(Relation relation, HeapTuple tuple,
				CommandId cid, LockTupleMode mode, LockWaitPolicy wait_policy,
				bool follow_update,
				Buffer *buffer, HeapUpdateFailureData *hufd);
extern Param *SS_make_initplan_output_param(PlannerInfo *root,
							  Oid resulttype, int32 resulttypmod,
							  Oid resultcollation);
extern TypeName *typeStringToTypeName(const char *str);
List *pull_var_clause(Node *node, int flags);

char *psprintf(const char *fmt,...);            
extern int namecpy(Name n1, Name n2);    

extern List *RelationGetFKeyList(Relation relation);    

void mark_index_clustered(Relation rel, Oid indexOid, bool is_internal);
extern AttrNumber *convert_tuples_by_name_map(TupleDesc indesc,
						   TupleDesc outdesc,
						   const char *msg); 
extern int	check_enable_rls(Oid relid, Oid checkAsUser, bool noError); 

extern void PreventCommandIfParallelMode(const char *cmdname); 

extern List *ExecInsertIndexTuples(TupleTableSlot *slot, ItemPointer tupleid,
					  EState *estate, bool noDupErr, bool *specConflict,
					  List *arbiterIndexes);

extern void CreateCacheMemoryContext(void); 
ArrayIterator array_create_iterator(ArrayType *arr, int slice_ndim, ArrayMetaState *mstate);
extern Oid RangeVarGetRelidExtended_tsdb(const RangeVar *relation,
						 LOCKMODE lockmode, bool missing_ok, bool nowait,
						 RangeVarGetRelidCallback_tsdb callback,
						 void *callback_arg,int tsdb = 0); 

extern ObjectAddress
DefineIndex_tsdb(Oid relationId,
			IndexStmt *stmt,
			Oid indexRelationId,
			bool is_alter_table,
			bool check_rights,
			bool skip_build,
			bool quiet); 


void
add_partial_path(RelOptInfo *parent_rel, Path *new_path);

AggPath*
create_agg_path(PlannerInfo *root,
				RelOptInfo *rel,
				Path *subpath,
				PathTarget *target,
				AggStrategy aggstrategy,
				AggSplit aggsplit,
				List *groupClause,
				List *qual,
				const AggClauseCosts *aggcosts,
				double numGroups);
GatherPath* create_gather_path(PlannerInfo *root, RelOptInfo *rel, Path *subpath,
				   PathTarget *target, Relids required_outer, double *rows) ;
extern void get_agg_clause_costs(PlannerInfo *root, Node *clause,
					 AggSplit aggsplit, AggClauseCosts *costs); 
void add_path(RelOptInfo *parent_rel, Path *new_path);
Path *apply_projection_to_path(PlannerInfo *root,
						 RelOptInfo *rel,
						 Path *path,
						 PathTarget *target);


RelOptInfo *fetch_upper_rel(PlannerInfo *root, UpperRelationKind kind, Relids relids);
MinMaxAggPath *create_minmaxagg_path(PlannerInfo *root,
					  RelOptInfo *rel,
					  PathTarget *target,
					  List *mmaggregates,
					  List *quals);
PathTarget *make_pathtarget_from_tlist(List *tlist);
PathTarget *set_pathtarget_cost_width(PlannerInfo *root, PathTarget *target); 
void SS_identify_outer_params(PlannerInfo *root);
void SS_charge_for_initplans(PlannerInfo *root, RelOptInfo *final_rel); 
PathKey *make_canonical_pathkey(PlannerInfo *root,
					   EquivalenceClass *eclass, Oid opfamily,
					   int strategy, bool nulls_first);

char *GetUserNameFromId(Oid roleid, bool noerr);
Oid get_role_oid_or_public(const char *rolname); 
Oid get_rolespec_oid(const Node *node, bool missing_ok);

extern void ExecVacuum(VacuumStmt *vacstmt, bool isTopLevel); 
extern void cluster_rel(Oid tableOid, Oid indexOid, bool recheck,
			bool verbose); 
extern void standard_ProcessUtility(Node *parsetree, const char *queryString,
						ProcessUtilityContext context, ParamListInfo params,
						DestReceiver *dest, char *completionTag); 
extern Oid	ReindexTable(RangeVar *relation, int options); 
extern void BackgroundWorkerBlockSignals(void); 
extern void BackgroundWorkerUnblockSignals(void);
extern void BackgroundWorkerInitializeConnectionByOid(Oid dboid, Oid useroid); 
extern void BackgroundWorkerInitializeConnection(char *dbname, char *username);  
extern bool RegisterDynamicBackgroundWorker(BackgroundWorker *worker,
								BackgroundWorkerHandle **handle); 

extern void TerminateBackgroundWorker(BackgroundWorkerHandle *handle); 
extern BgwHandleStatus WaitForBackgroundWorkerStartup(BackgroundWorkerHandle *
							   handle, pid_t *pid); 
extern BgwHandleStatus GetBackgroundWorkerPid(BackgroundWorkerHandle *handle,
					   pid_t *pidp); 
extern BgwHandleStatus WaitForBackgroundWorkerShutdown(BackgroundWorkerHandle *);
extern void ExplainPropertyBool(const char *qlabel, bool value,
					ExplainState *es);
extern void slot_getallattrs(TupleTableSlot *slot);

extern PathTarget *create_empty_pathtarget(void); 
extern SortGroupClause *get_sortgroupref_clause_noerr(Index sortref,
							  List *clauses); 
extern void add_column_to_pathtarget(PathTarget *target,
						 Expr *expr, Index sortgroupref);
extern void add_new_columns_to_pathtarget(PathTarget *target, List *exprs); 
extern void mark_partial_aggref(Aggref *agg, AggSplit aggsplit); 
extern List *roleSpecsToIds(List *memberNames); 

extern LWLockPadded *GetNamedLWLockTranche(const char *tranche_name);
extern void RequestNamedLWLockTranche(const char *tranche_name, int num_lwlocks);   

extern HeapScanDesc heap_beginscan_catalog(Relation relation, int nkeys,
					   ScanKey key);
extern void ApplySetting(Snapshot snapshot, Oid databaseid, Oid roleid,
			 Relation relsetting, GucSource source);
 
extern RewriteState begin_heap_rewrite(Relation OldHeap, Relation NewHeap,
				   TransactionId OldestXmin, TransactionId FreezeXid,
				   MultiXactId MultiXactCutoff, bool use_wal);  

extern Oid	toast_get_valid_index(Oid toastoid, LOCKMODE lock);
extern bool IsSystemClass(Oid relid, Form_pg_class reltuple);
extern void build_aggregate_finalfn_expr(Oid *agg_input_types,
							 int num_finalfn_inputs,
							 Oid agg_state_type,
							 Oid agg_result_type,
							 Oid agg_input_collation,
							 Oid finalfn_oid,
							 Expr **finalfnexpr);  
extern bool reindex_relation(Oid relid, int flags, int options); 
extern ColumnDef *makeColumnDef(const char *colname,
			  Oid typeOid, int32 typmod, Oid collOid); 
extern ArrayBuildStateArr *initArrayResultArr(Oid array_type, Oid element_type,
				   MemoryContext rcontext, bool subcontext);
extern ArrayBuildState *initArrayResult(Oid element_type,
				MemoryContext rcontext, bool subcontext); 
extern ArrayBuildStateArr *accumArrayResultArr(ArrayBuildStateArr *astate,
					Datum dvalue, bool disnull,
					Oid array_type,
					MemoryContext rcontext);  
extern Query *get_view_query(Relation view);
extern Datum makeArrayResultArr(ArrayBuildStateArr *astate,
				   MemoryContext rcontext, bool release);
extern int SPI_execute_with_args(const char *src,
					  int nargs, Oid *argtypes,
					  Datum *Values, const char *Nulls,
					  bool read_only, long tcount); 
extern bool scanint8(const char *str, bool errorOK, int64 *result);   
extern void cost_sort(Path *path, PlannerInfo *root,
		  List *pathkeys, Cost input_cost, double tuples, int width,
		  Cost comparison_cost, int sort_mem,
		  double limit_tuples); 
extern void check_index_predicates(PlannerInfo *root, RelOptInfo *rel); 
extern SortPath *create_sort_path(PlannerInfo *root,
				 RelOptInfo *rel,
				 Path *subpath,
				 List *pathkeys,
				 double limit_tuples);

extern bool IsInParallelMode(void);

extern void InitPostgres(const char *in_dbname, Oid dboid, const char *username,
 			 Oid useroid, char *out_dbname);

extern void cost_gather(GatherPath *path, PlannerInfo *root,
 			RelOptInfo *baserel, ParamPathInfo *param_info, double *rows); 
extern bool is_projection_capable_path(Path *path);
extern ProjectionPath *create_projection_path(PlannerInfo *root,
					   RelOptInfo *rel,
					   Path *subpath,
					   PathTarget *target); 

extern bool has_parallel_hazard(Node *node, bool allow_restricted); 
extern bool pg_event_trigger_ownercheck(Oid et_oid, Oid roleid); 
extern bool is_objectclass_supported(Oid class_id);

extern List *EventCacheLookup(EventTriggerEvent event); 
extern bool has_bypassrls_privilege(Oid roleid); 
extern bool InNoForceRLSOperation(void); 
extern int	pg_qsort_strcmp(const void *a, const void *b); 
extern int	get_aggregate_argtypes(Aggref *aggref, Oid *inputTypes); 

extern void add_new_column_to_pathtarget(PathTarget *target, Expr *expr);
extern void SpeculativeInsertionWait(TransactionId xid, uint32 token); 

extern size_t pvsnprintf(char *buf, size_t len, const char *fmt, va_list args) pg_attribute_printf(3, 0); 
extern void slot_getsomeattrs(TupleTableSlot *slot, int attnum); 

extern void ResourceOwnerEnlargeDSMs(ResourceOwner owner);
extern void ResourceOwnerRememberDSM(ResourceOwner owner,
						 dsm_segment *); 
extern void ResourceOwnerForgetDSM(ResourceOwner owner,
					   dsm_segment *); 

extern Oid resolve_aggregate_transtype(Oid aggfuncid,
							Oid aggtranstype,
							Oid *inputTypes,
							int numArguments);


extern bool check_functions_in_node(Node *node, check_function_callback checker,
						void *context);
extern char func_parallel(Oid funcid); 
extern Relids find_childrel_parents(PlannerInfo *root, RelOptInfo *rel); 
#endif
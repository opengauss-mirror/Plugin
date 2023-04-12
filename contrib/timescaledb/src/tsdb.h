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

// #include "commands/copy.h"
// #include "commands/trigger.h"
// #include "access/heapam.h"
// #include "commands/tablecmds.h"
// #include "fmgr.h"
// #include "optimizer/paths.h"
// #include "optimizer/restrictinfo.h"
// #include "access/tuptoaster.h"
// #include "executor/node/nodeExtensible.h"
// #include "access/htup.h"
// #include "optimizer/subselect.h"
// #include "parser/parse_type.h"
// #include "catalog/index.h"
// #include "commands/defrem.h"
// #include "commands/cluster.h"
// #include "commands/explain.h"
// #include "utils/postinit.h"
// #include "optimizer/var.h"

extern CopyState BeginCopyFrom(Relation rel, const char *filename,
			  bool is_program, List *attnamelist, List *options); //需要放入og内核实现
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
extern void NewRelationCreateToastTable(Oid relOid, Datum reloptions);// 还没有实现,之后再og中实现    
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





// extern void CacheRegisterRelcacheCallback(RelcacheCallbackFunction func,
// 							  Datum arg);
char *psprintf(const char *fmt,...);            
extern int namecpy(Name n1, Name n2);    

extern List *RelationGetFKeyList(Relation relation);    

// HeapScanDesc heap_beginscan(Relation relation, Snapshot snapshot,
// 			   int nkeys, ScanKey key,int tsdb = 0);

// Oid
// index_create(Relation heapRelation,
// 			 const char *indexRelationName,
// 			 Oid indexRelationId,
// 			 Oid relFileNode,
// 			 IndexInfo *indexInfo,
// 			 List *indexColNames,
// 			 Oid accessMethodObjectId,
// 			 Oid tableSpaceId,
// 			 Oid *collationObjectId,
// 			 Oid *classObjectId,
// 			 int16 *coloptions,
// 			 Datum reloptions,
// 			 bool isprimary,
// 			 bool isconstraint,
// 			 bool deferrable,
// 			 bool initdeferred,
// 			 bool allow_system_table_mods,
// 			 bool skip_build,
// 			 bool concurrent,
// 			 bool is_internal,
// 			 bool if_not_exists) ;

//void RenameRelationInternal(Oid myrelid, const char *newrelname, bool is_internal) ;

void mark_index_clustered(Relation rel, Oid indexOid, bool is_internal);
extern AttrNumber *convert_tuples_by_name_map(TupleDesc indesc,
						   TupleDesc outdesc,
						   const char *msg); 
extern int	check_enable_rls(Oid relid, Oid checkAsUser, bool noError); 

extern void PreventCommandIfParallelMode(const char *cmdname); 
// extern HeapTuple heap_getnext(HeapScanDesc scan, ScanDirection direction); og中有
//extern void ExecuteTruncate(TruncateStmt *stmt); //这个在og里面有
// extern char *GetConfigOptionByName(const char *name, const char **varname,
// 					  bool missing_ok); 

extern List *ExecInsertIndexTuples(TupleTableSlot *slot, ItemPointer tupleid,
					  EState *estate, bool noDupErr, bool *specConflict,
					  List *arbiterIndexes);


// extern Oid CreateSchemaCommand(CreateSchemaStmt *parsetree,
// 					const char *queryString); 在og中有
extern void CreateCacheMemoryContext(void); 
ArrayIterator array_create_iterator(ArrayType *arr, int slice_ndim, ArrayMetaState *mstate);
extern Oid RangeVarGetRelidExtended_tsdb(const RangeVar *relation,
						 LOCKMODE lockmode, bool missing_ok, bool nowait,
						 RangeVarGetRelidCallback_tsdb callback,
						 void *callback_arg,int tsdb = 0); 

//这个没有实现,需要在og中重载
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
					   int strategy, bool nulls_first);//暂时注释
// EquivalenceClass *
// get_eclass_for_sort_expr_tsdb(PlannerInfo *root,
// 						 Expr *expr,
// 						 Relids nullable_relids,
// 						 List *opfamilies,
// 						 Oid opcintype,
// 						 Oid collation,
// 						 Index sortref,
// 						 Relids rel,
// 						 bool create_it);
char *GetUserNameFromId(Oid roleid, bool noerr);
Oid get_role_oid_or_public(const char *rolname); 
Oid get_rolespec_oid(const Node *node, bool missing_ok);


//start EventTrigger
// extern Oid	CreateEventTrigger(CreateEventTrigStmt *stmt);
// extern void RemoveEventTriggerById(Oid ctrigOid);
// extern Oid	get_event_trigger_oid(const char *trigname, bool missing_ok);

// extern Oid	AlterEventTrigger(AlterEventTrigStmt *stmt);
// extern ObjectAddress AlterEventTriggerOwner(const char *name, Oid newOwnerId);
// extern void AlterEventTriggerOwner_oid(Oid, Oid newOwnerId);

// extern bool EventTriggerSupportsObjectType(ObjectType obtype);
// extern bool EventTriggerSupportsObjectClass(ObjectClass objclass);
// extern bool EventTriggerSupportsGrantObjectType(GrantObjectType objtype);
// extern void EventTriggerDDLCommandStart(Node *parsetree);
// extern void EventTriggerDDLCommandEnd(Node *parsetree);
// extern void EventTriggerSQLDrop(Node *parsetree);
// extern void EventTriggerTableRewrite(Node *parsetree, Oid tableOid, int reason);

// extern bool EventTriggerBeginCompleteQuery(void);
// extern void EventTriggerEndCompleteQuery(void);
// extern bool trackDroppedObjectsNeeded(void);
// extern void EventTriggerSQLDropAddObject(const ObjectAddress *object,
// 							 bool original, bool normal);

// extern void EventTriggerInhibitCommandCollection(void);
// extern void EventTriggerUndoInhibitCommandCollection(void);

// extern void EventTriggerCollectSimpleCommand(ObjectAddress address,
// 								 ObjectAddress secondaryObject,
// 								 Node *parsetree);

// extern void EventTriggerAlterTableStart(Node *parsetree);
// extern void EventTriggerAlterTableRelid(Oid objectId);
// extern void EventTriggerCollectAlterTableSubcmd(Node *subcmd,
// 									ObjectAddress address);
// extern void EventTriggerAlterTableEnd(void);

// extern void EventTriggerCollectGrant(InternalGrant *istmt);
// extern void EventTriggerCollectAlterOpFam(AlterOpFamilyStmt *stmt,
// 							  Oid opfamoid, List *operators,
// 							  List *procedures);
// extern void EventTriggerCollectCreateOpClass(CreateOpClassStmt *stmt,
// 								 Oid opcoid, List *operators,
// 								 List *procedures);
// extern void EventTriggerCollectAlterTSConfig(AlterTSConfigurationStmt *stmt,
// 								 Oid cfgId, Oid *dictIds, int ndicts);
// extern void EventTriggerCollectAlterDefPrivs(AlterDefaultPrivilegesStmt *stmt);
//end EventTrigger

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
//extern HeapTuple index_getnext(IndexScanDesc scan, ScanDirection direction,int tsdb = 0);  
extern void slot_getallattrs(TupleTableSlot *slot);

extern PathTarget *create_empty_pathtarget(void); 
extern SortGroupClause *get_sortgroupref_clause_noerr(Index sortref,
							  List *clauses); 
extern void add_column_to_pathtarget(PathTarget *target,
						 Expr *expr, Index sortgroupref);
extern void add_new_columns_to_pathtarget(PathTarget *target, List *exprs); 
extern void mark_partial_aggref(Aggref *agg, AggSplit aggsplit); 
extern List *roleSpecsToIds(List *memberNames); 

// extern dsm_segment *dsm_create(Size size, int flags);
// extern dsm_handle dsm_segment_handle(dsm_segment *seg);
// extern dsm_segment *dsm_find_mapping(dsm_handle h); 
// extern void *dsm_segment_address(dsm_segment *seg);
// extern void dsm_detach(dsm_segment *seg); 
// extern dsm_segment *dsm_attach(dsm_handle h); 

extern LWLockPadded *GetNamedLWLockTranche(const char *tranche_name);
extern void RequestNamedLWLockTranche(const char *tranche_name, int num_lwlocks);   
// extern PGPROC *shm_mq_get_sender(shm_mq *); 
// extern shm_mq *shm_mq_get_queue(shm_mq_handle *mqh); 
// extern shm_mq_result shm_mq_receive(shm_mq_handle *mqh,
// 			   Size *nbytesp, void **datap, bool nowait);
// extern shm_mq *shm_mq_create(void *address, Size size);
// extern void shm_mq_set_receiver(shm_mq *mq, PGPROC *);  
// extern shm_mq_handle *shm_mq_attach(shm_mq *mq, dsm_segment *seg,
// 			  BackgroundWorkerHandle *handle); 
// extern void shm_mq_set_sender(shm_mq *mq, PGPROC *); 
// extern shm_mq_result shm_mq_send(shm_mq_handle *mqh,
// 			Size nbytes, const void *data, bool nowait); 

extern HeapScanDesc heap_beginscan_catalog(Relation relation, int nkeys,
					   ScanKey key);
// extern Snapshot GetCatalogSnapshot(Oid relid); 
extern void ApplySetting(Snapshot snapshot, Oid databaseid, Oid roleid,
			 Relation relsetting, GucSource source);
 
//  extern Oid make_new_heap(Oid OIDOldHeap, Oid NewTableSpace, char relpersistence,
// 			  LOCKMODE lockmode);
// extern void vacuum_set_xid_limits(Relation rel,
// 					  int freeze_min_age, int freeze_table_age,
// 					  int multixact_freeze_min_age,
// 					  int multixact_freeze_table_age,
// 					  TransactionId *oldestXmin,
// 					  TransactionId *freezeLimit,
// 					  TransactionId *xidFullScanLimit,
// 					  MultiXactId *multiXactCutoff,
// 					  MultiXactId *mxactFullScanLimit);

extern RewriteState begin_heap_rewrite(Relation OldHeap, Relation NewHeap,
				   TransactionId OldestXmin, TransactionId FreezeXid,
				   MultiXactId MultiXactCutoff, bool use_wal);  
// extern Tuplesortstate *tuplesort_begin_cluster(TupleDesc tupDesc,
// 						Relation indexRel,
// 						int workMem, bool randomAccess); 
// extern void tuplesort_putheaptuple(Tuplesortstate *state, HeapTuple tup);
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
// static Compressor *compressor_for_algorithm_and_type(CompressionAlgorithms algorithm, Oid type);在tsdb中    
extern ColumnDef *makeColumnDef(const char *colname,
			  Oid typeOid, int32 typmod, Oid collOid); 
extern void *MemoryContextAllocExtended(MemoryContext context,
						   Size size, int flags); 
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
// extern const CustomScanMethods *GetCustomScanMethods(const char *CustomName,
// 					 bool missing_ok);
extern SortPath *create_sort_path(PlannerInfo *root,
				 RelOptInfo *rel,
				 Path *subpath,
				 List *pathkeys,
				 double limit_tuples);
//tsdb.cpp


extern bool IsInParallelMode(void);
//static void AfterTriggerEnlargeQueryState(void);
// extern bool RI_FKey_fk_upd_check_required(Trigger *trigger, Relation fk_rel,
// 							  HeapTuple old_row, HeapTuple new_row);    									    
// extern bool contain_volatile_functions_not_nextval(Node *clause); 
// extern FILE *OpenPipeStream(const char *command, const char *mode); 
// extern Node *transformWhereClause(ParseState *pstate, Node *clause,
// 					 ParseExprKind exprKind, const char *constructName); 
extern void RunObjectPostCreateHook(Oid classId, Oid objectId, int subId,
						bool is_internal); 
// extern Oid CreateConstraintEntry(const char *constraintName,
// 					  Oid constraintNamespace,
// 					  char constraintType,
// 					  bool isDeferrable,
// 					  bool isDeferred,
// 					  bool isValidated,
// 					  Oid relId,
// 					  const int16 *constraintKey,
// 					  int constraintNKeys,
// 					  Oid domainId,
// 					  Oid indexRelId,
// 					  Oid foreignRelId,
// 					  const int16 *foreignKey,
// 					  const Oid *pfEqOp,
// 					  const Oid *ppEqOp,
// 					  const Oid *ffEqOp,
// 					  int foreignNKeys,
// 					  char foreignUpdateType,
// 					  char foreignDeleteType,
// 					  char foreignMatchType,
// 					  const Oid *exclOp,
// 					  Node *conExpr,
// 					  const char *conBin,
// 					  const char *conSrc,
// 					  bool conIsLocal,
// 					  int conInhCount,
// 					  bool conNoInherit,
// 					  bool is_internal);
extern void InitPostgres(const char *in_dbname, Oid dboid, const char *username,
 			 Oid useroid, char *out_dbname); //因为太长了,没有实现,之后再说

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
// extern IndexAmRoutine *GetIndexAmRoutine(Oid amhandler); 
// extern void WaitForLockers(LOCKTAG heaplocktag, LOCKMODE lockmode); 
extern bool has_bypassrls_privilege(Oid roleid); 
extern bool InNoForceRLSOperation(void); 
extern int	pg_qsort_strcmp(const void *a, const void *b); 
extern int	get_aggregate_argtypes(Aggref *aggref, Oid *inputTypes); 
// extern void vacuum(int options, RangeVar *relation, Oid relid,
// 	   VacuumParams *params, List *va_cols,
// 	   BufferAccessStrategy bstrategy, bool isTopLevel); 
// extern Relation heap_create(const char *relname,
// 			Oid relnamespace,
// 			Oid reltablespace,
// 			Oid relid,
// 			Oid relfilenode,
// 			TupleDesc tupDesc,
// 			char relkind,
// 			char relpersistence,
// 			bool shared_relation,
// 			bool mapped_relation,
// 			bool allow_system_table_mods); 
extern void add_new_column_to_pathtarget(PathTarget *target, Expr *expr);
extern void SpeculativeInsertionWait(TransactionId xid, uint32 token); 
// extern int	errtableconstraint(Relation rel, const char *conname); 
// extern Relids find_childrel_parents(PlannerInfo *root, RelOptInfo *rel); 
extern size_t pvsnprintf(char *buf, size_t len, const char *fmt, va_list args) pg_attribute_printf(3, 0); 
extern void slot_getsomeattrs(TupleTableSlot *slot, int attnum); 
// extern HeapTuple index_fetch_heap(IndexScanDesc scan); 
// extern bool HeapTupleHeaderIsOnlyLocked(HeapTupleHeader tuple);
// extern bytea *view_reloptions(Datum reloptions, bool validate);
// extern bool binary_upgrade_is_next_part_toast_pg_class_oid_valid(); 
// //extern void LockTuple(Relation relation, ItemPointer tid, LOCKMODE lockmode,int tsdb = 0); 
// extern bool visibilitymap_clear(Relation rel, BlockNumber heapBlk,
// 					Buffer vmbuf, uint8 flags); 
// extern bool dsm_impl_op(dsm_op op, dsm_handle handle, Size request_size,
// 			void **impl_private, void **mapped_address, Size *mapped_size,
// 			int elevel);
extern void ResourceOwnerEnlargeDSMs(ResourceOwner owner);
extern void ResourceOwnerRememberDSM(ResourceOwner owner,
						 dsm_segment *); 
extern void ResourceOwnerForgetDSM(ResourceOwner owner,
					   dsm_segment *); 
// extern char *get_am_name(Oid amOid);
// extern void WaitForLockersMultiple(List *locktags, LOCKMODE lockmode);
// extern void CacheRegisterSyscacheCallback(int cacheid,
// 							  SyscacheCallbackFunction func,
// 							  Datum arg);
// extern int	errtable(Relation rel);
// extern int	err_generic_string(int field, const char *str);
// extern void NewHeapCreateToastTable(Oid relOid, Datum reloptions,
// 						LOCKMODE lockmode);
// extern TransactionId TransactionIdLimitedForOldSnapshots(TransactionId recentXmin,
// 									Relation relation);
// extern int	MultiXactMemberFreezeThreshold(void);
// // extern void lazy_vacuum_rel(Relation onerel, int options,
// // 				VacuumParams *params, BufferAccessStrategy bstrategy);
// extern bool MultiXactIdIsRunning(MultiXactId multi, bool isLockOnly);
// extern void XactLockTableWait(TransactionId xid, Relation rel,
// 				  ItemPointer ctid, XLTW_Oper oper);
// //extern void LockTuple(Relation relation, ItemPointer tid, LOCKMODE lockmode); 
// extern int GetMultiXactIdMembers(MultiXactId multi, MultiXactMember **xids,
// 					  bool allow_old, bool isLockOnly);

// extern void index_build(Relation heapRelation,
// 			Relation indexRelation,
// 			IndexInfo *indexInfo,
// 			bool isprimary,
// 			bool isreindex); 

// extern void finish_heap_swap(Oid OIDOldHeap, Oid OIDNewHeap,
// 				 bool is_system_catalog,
// 				 bool swap_toast_by_content,
// 				 bool check_constraints,
// 				 bool is_internal,
// 				 TransactionId frozenXid,
// 				 MultiXactId minMulti,
// 				 char newrelpersistence);
// extern bool IsAutoVacuumWorkerProcess(void); 
// //extern Datum hash_any(register const unsigned char *k, register int keylen); 
// //extern bool plan_cluster_use_sort(Oid tableOid, Oid indexOid);
// //extern bool TransactionIdIsInProgress(TransactionId xid);
// extern Node *build_column_default(Relation rel, int attrno);
// extern Expr *expression_planner(Expr *expr);
// extern void *hash_search(HTAB *hashp, const void *keyPtr, HASHACTION action,
// 			bool *foundPtr);

// // extern List *find_all_inheritors(Oid parentrelId, LOCKMODE lockmode,
// // 					List **parents);
// //extern void CheckTableForSerializableConflictIn(Relation relation);
// //extern void ResetSequence(Oid seq_relid);
// extern void pq_beginmessage(StringInfo buf, char msgtype);
// extern void pq_sendbyte(StringInfo buf, int byt);
// extern void pq_sendint(StringInfo buf, int i, int b);
// extern void pq_endmessage(StringInfo buf);
// extern void pq_startmsgread(void);
// extern void pq_putemptymessage(char msgtype);
// extern int	pq_getbytes(char *s, size_t len);
// extern int	pq_getmessage(StringInfo s, int maxlen);
// extern const char *pq_getmsgstring(StringInfo msg);
// extern void pq_copymsgbytes(StringInfo msg, char *buf, int datalen);
// extern int	pq_getbyte(void);
// extern ssize_t secure_read(Port *port, void *ptr, size_t len);
// // extern RangeTblEntry *addRangeTableEntryForRelation(ParseState *pstate,
// // 							  Relation rel,
// // 							  Alias *alias,
// // 							  bool inh,
// // 							  bool inFromCl);

// extern void addRTEtoQuery(ParseState *pstate, RangeTblEntry *rte,
// 			  bool addToJoinList,
// 			  bool addToRelNameSpace, bool addToVarNameSpace);
// //extern void assign_expr_collations(ParseState *pstate, Node *expr);
// //extern int	attnameAttNum(Relation rd, const char *attname, bool sysColOK);

// extern void TransferPredicateLocksToHeapRelation(Relation relation);
// //extern void index_register(Oid heap, Oid ind, IndexInfo *indexInfo);
// extern void RenameConstraintById(Oid conId, const char *newname);
// // extern ObjectAddress index_constraint_create(Relation heapRelation,
// // 						Oid indexRelationId,
// // 						IndexInfo *indexInfo,
// // 						const char *constraintName,
// // 						char constraintType,
// // 						bool deferrable,
// // 						bool initdeferred,
// // 						bool mark_as_primary,
// // 						bool update_pgindex,
// // 						bool remove_old_dependencies,
// // 						bool allow_system_table_mods,
// // 						bool is_internal);

extern Oid resolve_aggregate_transtype(Oid aggfuncid,
							Oid aggtranstype,
							Oid *inputTypes,
							int numArguments);
// extern IndexAmRoutine *GetIndexAmRoutineByAmId(Oid amoid, bool noerror);

// //extern List *transformCreateSchemaStmt(CreateSchemaStmt *stmt);
// extern char *get_database_name(Oid dbid);
// //extern Oid	get_tablespace_oid(const char *tablespacename, bool missing_ok);
// //extern Oid	GetDefaultTablespace(char relpersistence);
// //extern char *get_tablespace_name(Oid spc_oid);
// // extern Datum transformRelOptions(Datum oldOptions, List *defList,
// // 					char *namspace, char *validnsps[],
// // 					bool ignoreOids, bool isReset);//og有

// extern bytea *index_reloptions(amoptions_function amoptions, Datum reloptions,
// 				 bool validate);
// extern char *ChooseRelationName(const char *name1, const char *name2,
// 				   const char *label, Oid namespaceid);
// extern void CreateComments(Oid oid, Oid classoid, int32 subid, char *comment);
// // extern VirtualTransactionId *GetCurrentVirtualXIDs(TransactionId limitXmin,
// // 					  bool excludeXmin0, bool allDbs, int excludeVacuum,
// // 					  int *nvxids);
// //extern List *raw_parser(const char *str);
// extern void InsertPgClassTuple(Relation pg_class_desc,
// 				   Relation new_rel_desc,
// 				   Oid new_rel_oid,
// 				   Datum relacl,
// 				   Datum reloptions);

// //extern void visibilitymap_count(Relation rel, BlockNumber *all_visible, BlockNumber *all_frozen);//在og中
// //extern void heapgetpage(HeapScanDesc scan, BlockNumber page);//在og中
// extern bool HeapTupleSatisfiesMVCC(HeapTuple htup,
// 					   Snapshot snapshot, Buffer buffer);
// extern bool HeapTupleSatisfiesToast(HeapTuple htup,
// 						Snapshot snapshot, Buffer buffer);
// extern void TestForOldSnapshot_impl(Snapshot snapshot, Relation relation);
// extern void TransferPredicateLocksToHeapRelation(Relation relation);
// //extern TransactionId GetOldestXmin(Relation rel, bool ignoreVacuum);在og中
// extern Snapshot GetNonHistoricCatalogSnapshot(Oid relid);
// extern void CheckForSerializableConflictOut(bool valid, Relation relation, HeapTuple tuple,
// 								Buffer buffer, Snapshot snapshot);
// extern FuncExpr *makeFuncExpr(Oid funcid, Oid rettype, List *args,
// 			 Oid funccollid, Oid inputcollid, CoercionForm fformat);
// extern bytea *heap_reloptions(char relkind, Datum reloptions, bool validate);
// extern void visibilitymap_pin(Relation rel, BlockNumber heapBlk,
// 				  Buffer *vmbuf);
// extern void on_dsm_detach(dsm_segment *seg,
// 			  on_dsm_detach_callback function, Datum arg);
// extern Oid heap_create_with_catalog(const char *relname,
// 						 Oid relnamespace,
// 						 Oid reltablespace,
// 						 Oid relid,
// 						 Oid reltypeid,
// 						 Oid reloftypeid,
// 						 Oid ownerid,
// 						 TupleDesc tupdesc,
// 						 List *cooked_constraints,
// 						 char relkind,
// 						 char relpersistence,
// 						 bool shared_relation,
// 						 bool mapped_relation,
// 						 bool oidislocal,
// 						 int oidinhcount,
// 						 OnCommitAction oncommit,
// 						 Datum reloptions,
// 						 bool use_user_acl,
// 						 bool allow_system_table_mods,
// 						 bool is_internal,
// 						 ObjectAddress *typaddress);

// extern char *escape_xml(const char *str);
// extern void PredicateLockRelation(Relation relation, Snapshot snapshot);
// extern void shm_mq_detach(shm_mq *);
// extern bool join_clause_is_movable_to(RestrictInfo *rinfo, RelOptInfo *baserel);
// extern PlanRowMark *get_plan_rowmark(List *rowmarks, Index rtindex);
// extern bool predicate_implied_by(List *predicate_list,
// 					 List *restrictinfo_list);
// extern void escape_json(StringInfo buf, const char *str);
// extern bool isTempOrTempToastNamespace(Oid namespaceId);
extern void ProcArrayGetReplicationSlotXmin(TransactionId *xmin,
								TransactionId *catalog_xmin);
// extern Oid RangeVarGetAndCheckCreationNamespace(RangeVar *newRelation,
// 									 LOCKMODE lockmode,
// 									 Oid *existing_relation_id);
// extern bool interpretOidsOption(List *defList, bool allowOids);
// // extern List *AddRelationNewConstraints(Relation rel,
// // 						  List *newColDefaults,
// // 						  List *newConstraints,
// // 						  bool allow_merge,
// // 						  bool is_local,
// // 						  bool is_internal);//catalog/heap.h

// // extern HTSU_Result HeapTupleSatisfiesUpdate(HeapTuple htup,
// // 						 CommandId curcid, Buffer buffer,int tsdb = 0);
// extern void PrepareSortSupportFromIndexRel(Relation indexRel, int16 strategy,
// 							   SortSupport ssup);
// extern Node *transformExpr(ParseState *pstate, Node *expr, ParseExprKind exprKind);
// extern CommandId HeapTupleHeaderGetCmax(HeapTupleHeader tup);
// extern void reindex_index(Oid indexId, bool skip_constraint_checks,
// 			  char relpersistence, int options);
// // extern void RangeVarCallbackOwnsTable(const RangeVar *relation,
// // 						  Oid relId, Oid oldRelId, void *arg); 
extern bool check_functions_in_node(Node *node, check_function_callback checker,
						void *context);
// extern CoercionPathType find_coercion_pathway(Oid targetTypeId,
// 					  Oid sourceTypeId,
// 					  CoercionContext ccontext,
// 					  Oid *funcid);

// extern bool IsBinaryCoercible(Oid srctype, Oid targettype);
// extern void PrepareTempTablespaces(void);
// //extern void SharedInvalBackendInit(bool sendOnly);
// extern Node *coerce_to_boolean(ParseState *pstate, Node *node,
// 				  const char *constructName);
// //extern Oid	attnumTypeId(Relation rd, int attid);
// extern TimeoutId RegisterTimeout(TimeoutId id, timeout_handler_proc handler);
// extern void CheckDeadLockAlert(void);
// extern void on_shmem_exit(pg_on_exit_callback function, Datum arg);
// extern void before_shmem_exit(pg_on_exit_callback function, Datum arg);
// extern bool IsAutoVacuumLauncherProcess(void);
// extern void InvalidateCatalogSnapshot(void);
// //extern void SetDatabasePath(const char *path);
// extern int	get_object_catcache_oid(Oid class_id);
// extern Oid	get_object_oid_index(Oid class_id);
// extern char *format_procedure_qualified(Oid procedure_oid);
// extern void format_procedure_parts(Oid operator_oid, List **objnames,
// 					   List **objargs);
// extern char *format_type_be_qualified(Oid type_oid);
// extern char *get_namespace_name_or_temp(Oid nspid);
// extern char *format_operator_qualified(Oid operator_oid);
// extern void format_operator_parts(Oid operator_oid, List **objnames,
// 					  List **objargs);
// extern char *format_type_be_qualified(Oid type_oid);
// extern char *get_namespace_name_or_temp(Oid nspid);
// extern char *format_type_be_qualified(Oid type_oid);
// extern char *get_language_name(Oid langoid, bool missing_ok);
// //extern void RelationCreateStorage(RelFileNode rnode, char relpersistence);
// extern relopt_value *parseRelOptions(Datum options, bool validate,
// 				relopt_kind kind, int *numrelopts);
// extern void *allocateReloptStruct(Size base, relopt_value *options,
// 					 int numoptions);
// extern void fillRelOptions(void *rdopts, Size basesize,
// 			   relopt_value *options, int numoptions,
// 			   bool validate,
// 			   const relopt_parse_elt *elems, int nelems);
// extern int64 GetSnapshotCurrentTimestamp(void);
// extern void pgstat_progress_start_command(ProgressCommandType cmdtype,
// 							  Oid relid);
// extern void pgstat_progress_update_param(int index, int64 val);
// extern void FreeSpaceMapVacuum(Relation rel);


// //tsdb_extension3.cpp
// extern void pgstat_progress_end_command(void);

// extern TransactionId SubTransGetParent(TransactionId xid);

// extern int SimpleLruReadPage(SlruCtl ctl, int pageno, bool write_ok,
// 				  TransactionId xid);

// extern int32 get_relation_data_width(Oid relid, int32 *attr_widths);

// // extern Node *coerce_to_target_type(ParseState *pstate,
// // 					  Node *expr, Oid exprtype,
// // 					  Oid targettype, int32 targettypmod,
// // 					  CoercionContext ccontext,
// // 					  CoercionForm cformat,
// // 					  int location);

// //extern List *find_inheritance_children(Oid parentrelId, LOCKMODE lockmode);
// extern ssize_t be_tls_read(Port *port, void *ptr, size_t len, int *waitfor);
// extern ssize_t secure_raw_read(Port *port, void *ptr, size_t len);

// extern void ModifyWaitEvent(WaitEventSet *set, int pos, uint32 events, Latch *latch);

// extern int	WaitEventSetWait(WaitEventSet *set, long timeout, WaitEvent *occurred_events, int nevents);

// extern void ProcessClientReadInterrupt(bool blocked);

// extern int RTERangeTablePosn(ParseState *pstate,
// 				  RangeTblEntry *rte,
// 				  int *sublevels_up);

// extern bool ConstraintNameIsUsed(ConstraintCategory conCat, Oid objId,
// 					 Oid objNamespace, const char *conname);

// extern List *SystemFuncName(char *name);

// extern Oid enforce_generic_type_consistency(Oid *actual_arg_types,
// 								 Oid *declared_arg_types,
// 								 int nargs,
// 								 Oid rettype,
// 								 bool allow_poly);

// extern void parser_init(void);



// extern TYPCATEGORY TypeCategory(Oid type);

// extern Node *coerce_to_target_type(ParseState *pstate,
// 					  Node *expr, Oid exprtype,
// 					  Oid targettype, int32 targettypmod,
// 					  CoercionContext ccontext,
// 					  CoercionForm cformat,
// 					  int location);

// extern void pairingheap_remove(pairingheap *heap, pairingheap_node *node);

// //extern void log_smgrcreate(RelFileNode *rnode, ForkNumber forkNum);

// extern int64 GetCurrentIntegerTimestamp(void);
// extern bool RelationHasUnloggedIndex(Relation rel);
// extern int64 GetOldSnapshotThresholdTimestamp(void);
// extern bool RelationInvalidatesSnapshotsOnly(Oid relid);
// extern bool RelationHasSysCache(Oid relid);
// extern void pairingheap_add(pairingheap *heap, pairingheap_node *node);

extern char func_parallel(Oid funcid); 
extern Relids find_childrel_parents(PlannerInfo *root, RelOptInfo *rel); 


#endif
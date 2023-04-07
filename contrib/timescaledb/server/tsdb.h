// #include "nodes/plannodes.h"
// #include "nodes/relation.h"
// #include "access/tupdesc.h"
// #include "catalog/objectaddress.h"
// #include "utils/array.h"
// #include "lib/pairingheap.h"
// #include "access/heapam.h"
// #include <stdbool.h>
// #include "tsdb_head.h"
// //first
// extern HeapScanDesc heap_beginscan_catalog(Relation relation, int nkeys,
// 					   ScanKey key);
// extern void heap_setscanlimits(HeapScanDesc scan, BlockNumber startBlk,
// 				   BlockNumber endBlk);
// extern void heap_rescan_set_params(HeapScanDesc scan, ScanKey key,
// 					 bool allow_strat, bool allow_sync, bool allow_pagemode);
// extern Size heap_parallelscan_estimate(Snapshot snapshot);
// extern void heap_parallelscan_initialize(ParallelHeapScanDesc target,
// 							 Relation relation, Snapshot snapshot);
// extern HeapScanDesc heap_beginscan_parallel(Relation, ParallelHeapScanDesc);
// extern bool heap_tuple_needs_eventual_freeze(HeapTupleHeader tuple);
// extern HeapScanDesc heap_beginscan(Relation relation, Snapshot snapshot,
// 			   int nkeys, ScanKey key);
// // extern HeapScanDesc heap_beginscan_strat(Relation relation, Snapshot snapshot,
// // 					 int nkeys, ScanKey key,
// // 					 bool allow_strat, bool allow_sync);
// // extern HeapScanDesc heap_beginscan_bm(Relation relation, Snapshot snapshot,
// // 				  int nkeys, ScanKey key);
// extern HeapScanDesc heap_beginscan_sampling(Relation relation,
// 						Snapshot snapshot, int nkeys, ScanKey key,
// 					 bool allow_strat, bool allow_sync, bool allow_pagemode);
// // extern HTSU_Result heap_delete(Relation relation, ItemPointer tid,
// // 			CommandId cid, Snapshot crosscheck, bool wait,
// // 			HeapUpdateFailureData *hufd);
// // extern HTSU_Result heap_update(Relation relation, ItemPointer otid,
// // 			HeapTuple newtup,
// // 			CommandId cid, Snapshot crosscheck, bool wait,
// // 			HeapUpdateFailureData *hufd, LockTupleMode *lockmode);
// extern HTSU_Result heap_lock_tuple(Relation relation, HeapTuple tuple,
// 				CommandId cid, LockTupleMode mode, LockWaitPolicy wait_policy,
// 				bool follow_update,
// 				Buffer *buffer, HeapUpdateFailureData *hufd);
// #define HeapTupleHeaderGetXvac(tup) \
// ( \
// 	((tup)->t_infomask & HEAP_MOVED) ? \
// 		(tup)->t_choice.t_heap.t_field3.t_xvac \
// 	: \
// 		InvalidTransactionId \
// )
// #define HeapTupleHeaderSetXvac(tup, xid) \
// do { \
// 	Assert((tup)->t_infomask & HEAP_MOVED); \
// 	(tup)->t_choice.t_heap.t_field3.t_xvac = (xid); \
// } while (0)
// #define HeapTupleHeaderIsSpeculative(tup) \
// ( \
// 	(tup)->t_ctid.ip_posid == SpecTokenOffsetNumber \
// )
// #define HeapTupleHeaderGetSpeculativeToken(tup) \
// ( \
// 	AssertMacro(HeapTupleHeaderIsSpeculative(tup)), \
// 	ItemPointerGetBlockNumber(&(tup)->t_ctid) \
// )
// #define HeapTupleHeaderGetDatumLength(tup) \
// 	VARSIZE(tup)
// #define HANDLE_STRING_RELOPTION(optname, var, option, base, offset, wasset) \
// 	do {														\
// 		relopt_string *optstring = (relopt_string *) option.gen;\
// 		char *string_val;										\
// 		if (option.isset)										\
// 			string_val = option.values.string_val;				\
// 		else if (!optstring->default_isnull)					\
// 			string_val = optstring->default_val;				\
// 		else													\
// 			string_val = NULL;									\
// 		(wasset) != NULL ? *(wasset) = option.isset : (dummyret) NULL; \
// 		if (string_val == NULL)									\
// 			var = 0;											\
// 		else													\
// 		{														\
// 			strcpy(((char *)(base)) + (offset), string_val);	\
// 			var = (offset);										\
// 			(offset) += strlen(string_val) + 1;					\
// 		}														\
// 	} while (0)
// extern LOCKMODE AlterTableGetRelOptionsLockLevel(List *defList);
// extern void TupleDescCopyEntry(TupleDesc dst, AttrNumber dstAttno,
// 				   TupleDesc src, AttrNumber srcAttno);
// #define XactCompletionApplyFeedback(xinfo) \
// 	((xinfo & XACT_COMPLETION_APPLY_FEEDBACK) != 0)
// extern Size EstimateTransactionStateSpace(void);
// extern void SerializeTransactionState(Size maxsize, char *start_address);
// extern void StartParallelWorkerTransaction(char *tstatespace);
// extern XLogRecPtr XactLogCommitRecord(TimestampTz commit_time,
// 					int nsubxacts, TransactionId *subxacts,
// 					int nrels, RelFileNode *rels,
// 					int nmsgs, SharedInvalidationMessage *msgs,
// 					bool relcacheInval, bool forceSync,
// 					TransactionId twophase_xid);
// extern XLogRecPtr XactLogAbortRecord(TimestampTz abort_time,
// 				   int nsubxacts, TransactionId *subxacts,
// 				   int nrels, RelFileNode *rels,
// 				   TransactionId twophase_xid);
// extern const char *xact_identify(uint8 info);
// extern void ParseCommitRecord(uint8 info, xl_xact_commit *xlrec, xl_xact_parsed_commit *parsed);
// extern void ParseAbortRecord(uint8 info, xl_xact_abort *xlrec, xl_xact_parsed_abort *parsed);
// extern void EnterParallelMode(void);
// extern void ExitParallelMode(void);
// extern bool IsInParallelMode(void);
// #define DECLARE_UNIQUE_INDEX(name,oid,decl) extern int no_such_variable
// DECLARE_UNIQUE_INDEX(pg_init_privs_o_c_o_index, 3395, on pg_init_privs using btree(objoid oid_ops, classoid oid_ops, objsubid int4_ops));
// #define DECLARE_UNIQUE_INDEX(name,oid,decl) extern int no_such_variable
// DECLARE_UNIQUE_INDEX(pg_event_trigger_evtname_index, 3467, on pg_event_trigger using btree(evtname name_ops));
// #define DECLARE_UNIQUE_INDEX(name,oid,decl) extern int no_such_variable
// DECLARE_UNIQUE_INDEX(pg_transform_oid_index, 3574, on pg_transform using btree(oid oid_ops));
// #define DECLARE_UNIQUE_INDEX(name,oid,decl) extern int no_such_variable
// DECLARE_UNIQUE_INDEX(pg_transform_type_lang_index, 3575, on pg_transform using btree(trftype oid_ops, trflang oid_ops));
// #define DECLARE_UNIQUE_INDEX(name,oid,decl) extern int no_such_variable
// DECLARE_UNIQUE_INDEX(pg_policy_oid_index, 3257, on pg_policy using btree(oid oid_ops));
// #define DECLARE_UNIQUE_INDEX(name,oid,decl) extern int no_such_variable
// DECLARE_UNIQUE_INDEX(pg_policy_polrelid_polname_index, 3258, on pg_policy using btree(polrelid oid_ops, polname name_ops)); 
// // extern FuncCandidateList FuncnameGetCandidates(List *names,
// // 					  int nargs, List *argnames,
// // 					  bool expand_variadic,
// // 					  bool expand_defaults,
// // 					  bool missing_ok);
// // extern FuncCandidateList OpernameGetCandidates(List *names, char oprkind,
// // 					  bool missing_schema_ok);
// extern int	GetTempNamespaceBackendId(Oid namespaceId);
// extern void GetTempNamespaceState(Oid *tempNamespaceId,
// 					  Oid *tempToastNamespaceId);
// extern void SetTempNamespaceState(Oid tempNamespaceId,
// 					  Oid tempToastNamespaceId);
// extern void AtEOXact_Namespace(bool isCommit, bool parallel);
// extern Oid	FindDefaultConversionProc(int32 for_encoding, int32 to_encoding);
// #define ObjectAddressSubSet(addr, class_id, object_id, object_sub_id) \
// 	do { \
// 		(addr).classId = (class_id); \
// 		(addr).objectId = (object_id); \
// 		(addr).objectSubId = (object_sub_id); \
// 	} while (0)
// #define ObjectAddressSet(addr, class_id, object_id) \
// 	ObjectAddressSubSet(addr, class_id, object_id, 0)
// extern ObjectAddress get_object_address_rv(ObjectType objtype, RangeVar *rel,
// 					  List *objname, List *objargs, Relation *relp,
// 					  LOCKMODE lockmode, bool missing_ok);
// extern bool is_objectclass_supported(Oid class_id);
// extern Oid	get_object_oid_index(Oid class_id);
// extern int	get_object_catcache_oid(Oid class_id);
// extern int	get_object_catcache_name(Oid class_id);
// extern AttrNumber get_object_attnum_name(Oid class_id);
// extern AttrNumber get_object_attnum_namespace(Oid class_id);
// extern AttrNumber get_object_attnum_owner(Oid class_id);
// extern AttrNumber get_object_attnum_acl(Oid class_id);
// // extern AclObjectKind get_object_aclkind(Oid class_id);
// extern bool get_object_namensp_unique(Oid class_id);
// extern HeapTuple get_catalog_object_by_oid(Relation catalog,
// 						  Oid objectId);

// extern char *getObjectDescription(const ObjectAddress *object);
// extern char *getObjectDescriptionOids(Oid classid, Oid objid);

// extern int	read_objtype_from_string(const char *objtype);
// extern char *getObjectTypeDescription(const ObjectAddress *object);
// extern char *getObjectIdentity(const ObjectAddress *address);
// extern char *getObjectIdentityParts(const ObjectAddress *address,
// 					   List **objname, List **objargs);
// extern ArrayType *strlist_to_textarray(List *list);
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
// extern void NewRelationCreateToastTable(Oid relOid, Datum reloptions);
// extern void NewHeapCreateToastTable(Oid relOid, Datum reloptions,
// 						LOCKMODE lockmode);

// #define DECLARE_TOAST(name,toastoid,indexoid) extern int no_such_variable 
// DECLARE_TOAST(pg_shseclabel, 4060, 4061);

// //second
// extern void pull_up_subqueries(PlannerInfo *root);
// extern Expr *canonicalize_qual(Expr *qual);
// extern void expand_security_quals(PlannerInfo *root, List *tlist);
// extern List *preprocess_onconflict_targetlist(List *tlist,
// 								 int result_relation, List *range_table);
// extern RelOptInfo *plan_set_operations(PlannerInfo *root);
// extern Node *adjust_appendrel_attrs_multilevel(PlannerInfo *root, Node *node,
// 								  RelOptInfo *child_rel);
// extern Relids pull_varnos(Node *node);
// extern Node *ParseFuncOrColumn(ParseState *pstate, List *funcname, List *fargs,
// 				  FuncCall *fn, int location);
// // extern FuncDetailCode func_get_detail(List *funcname,
// // 				List *fargs, List *fargnames,
// // 				int nargs, Oid *argtypes,
// // 				bool expand_variadic, bool expand_defaults,
// // 				Oid *funcid, Oid *rettype,
// // 				bool *retset, int *nvargs, Oid *vatype,
// // 				Oid **true_typeids, List **argdefaults);
// extern List *raw_parser(const char *str);
// extern void ExceptionalCondition(const char *conditionName,
// 					 const char *errorType,
// 			   const char *fileName, int lineNumber);
// #define pg_attribute_noreturn() 
// pg_attribute_noreturn();
// extern bool contain_windowfuncs(Node *node);
// extern Node *ReplaceVarsFromTargetList(Node *node,
// 						  int target_varno, int sublevels_up,
// 						  RangeTblEntry *target_rte,
// 						  List *targetlist,
// 						  ReplaceVarsNoMatchOption nomatch_option,
// 						  int nomatch_varno,
// 						  bool *outer_hasSubLinks);

// extern void XactLockTableWait(TransactionId xid, Relation rel,
// 				  ItemPointer ctid, XLTW_Oper oper);
// extern bool ConditionalXactLockTableWait(TransactionId xid);
// extern void WaitForLockers(LOCKTAG heaplocktag, LOCKMODE lockmode);
// extern uint32 SpeculativeInsertionLockAcquire(TransactionId xid);
// extern void SpeculativeInsertionLockRelease(TransactionId xid);
// extern void SpeculativeInsertionWait(TransactionId xid, uint32 token);

// extern List *pg_parse_query(const char *query_string);
// extern PlannedStmt *pg_plan_query(Query *querytree, int cursorOptions,
// 			  ParamListInfo boundParams);
// extern void quickdie(SIGNAL_ARGS); pg_attribute_noreturn();
// extern void ProcessClientReadInterrupt(bool blocked);
// extern void ProcessClientWriteInterrupt(bool blocked);
// extern void PostgresMain(int argc, char *argv[],
// 			 const char *dbname,
// 			 const char *username); pg_attribute_noreturn();

// extern Datum array_cardinality(PG_FUNCTION_ARGS);
// extern Datum array_remove(PG_FUNCTION_ARGS);
// extern Datum array_replace(PG_FUNCTION_ARGS);
// extern Datum width_bucket_array(PG_FUNCTION_ARGS);
// extern Datum array_get_element(Datum arraydatum, int nSubscripts, int *indx,
// 				  int arraytyplen, int elmlen, bool elmbyval, char elmalign,
// 				  bool *isNull);
// extern Datum array_set_element(Datum arraydatum, int nSubscripts, int *indx,
// 				  Datum dataValue, bool isNull,
// 				  int arraytyplen, int elmlen, bool elmbyval, char elmalign);
// extern Datum array_get_slice(Datum arraydatum, int nSubscripts,
// 				int *upperIndx, int *lowerIndx,
// 				bool *upperProvided, bool *lowerProvided,
// 				int arraytyplen, int elmlen, bool elmbyval, char elmalign);
// extern Datum array_set_slice(Datum arraydatum, int nSubscripts,
// 				int *upperIndx, int *lowerIndx,
// 				bool *upperProvided, bool *lowerProvided,
// 				Datum srcArrayDatum, bool isNull,
// 				int arraytyplen, int elmlen, bool elmbyval, char elmalign);
// extern Datum array_map(FunctionCallInfo fcinfo, Oid retType,
// 		  ArrayMapState *amstate);
// extern ArrayBuildState *initArrayResult(Oid element_type,
// 				MemoryContext rcontext, bool subcontext);
// // extern ArrayBuildStateArr *initArrayResultArr(Oid array_type, Oid element_type,
// // 				   MemoryContext rcontext, bool subcontext);
// // extern ArrayBuildStateArr *accumArrayResultArr(ArrayBuildStateArr *astate,
// // 					Datum dvalue, bool disnull,
// // 					Oid array_type,
// // 					MemoryContext rcontext);
// // extern Datum makeArrayResultArr(ArrayBuildStateArr *astate,
// // 				   MemoryContext rcontext, bool release);
// // extern ArrayBuildStateAny *initArrayResultAny(Oid input_type,
// // 				   MemoryContext rcontext, bool subcontext);
// extern ArrayBuildStateAny *accumArrayResultAny(ArrayBuildStateAny *astate,
// 					Datum dvalue, bool disnull,
// 					Oid input_type,
// 					MemoryContext rcontext);
// extern Datum makeArrayResultAny(ArrayBuildStateAny *astate,
// 				   MemoryContext rcontext, bool release);
// extern ArrayIterator array_create_iterator(ArrayType *arr, int slice_ndim, ArrayMetaState *mstate);
// extern Datum expand_array(Datum arraydatum, MemoryContext parentcontext,
// 			 ArrayMetaState *metacache);
// // extern ExpandedArrayHeader *DatumGetExpandedArrayX(Datum d,
// // 					   ArrayMetaState *metacache);
// // extern ExpandedArrayHeader *DatumGetExpandedArray(Datum d);
// // extern AnyArrayType *DatumGetAnyArray(Datum d);
// extern void deconstruct_expanded_array(ExpandedArrayHeader *eah);
// extern Datum array_append(PG_FUNCTION_ARGS);
// extern Datum array_prepend(PG_FUNCTION_ARGS);
// extern Datum array_agg_array_transfn(PG_FUNCTION_ARGS);
// extern Datum array_agg_array_finalfn(PG_FUNCTION_ARGS);
// extern Datum array_position(PG_FUNCTION_ARGS);
// extern Datum array_position_start(PG_FUNCTION_ARGS);
// extern Datum array_positions(PG_FUNCTION_ARGS);

// extern Datum datumTransfer(Datum value, bool typByVal, int typLen);
// extern Size datumEstimateSpace(Datum value, bool isnull, bool typByVal,
// 				   int typLen);
// extern void datumSerialize(Datum value, bool isnull, bool typByVal,
// 			   int typLen, char **start_address);
// extern Datum datumRestore(char **start_address, bool *isnull);

// extern bool ParseConfigFile(const char *config_file, bool strict,
// 				const char *calling_file, int calling_lineno,
// 				int depth, int elevel,
// 				ConfigVariable **head_p, ConfigVariable **tail_p);
// extern bool ParseConfigDirectory(const char *includedir,
// 					 const char *calling_file, int calling_lineno,
// 					 int depth, int elevel,
// 					 ConfigVariable **head_p,
// 					 ConfigVariable **tail_p);
// extern char *GetConfigOptionByName(const char *name, const char **varname,
// 					  bool missing_ok);
// extern Size EstimateGUCStateSpace(void);
// extern void SerializeGUCState(Size maxsize, char *start_address);
// extern void RestoreGUCState(void *gucstate);
// extern bool hash_update_hash_key(HTAB *hashp, void *existingEntry,
// 					 const void *newKeyPtr);

// extern void CacheRegisterSyscacheCallback(int cacheid,
// 							  SyscacheCallbackFunction func,
// 							  Datum arg);
// extern void CacheRegisterRelcacheCallback(RelcacheCallbackFunction func,
// 							  Datum arg);
// extern void CallSyscacheCallbacks(int cacheid, uint32 hashvalue);

// extern char *get_language_name(Oid langoid, bool missing_ok);
// extern Oid	get_op_rettype(Oid opno);
// extern char func_parallel(Oid funcid);
// extern Oid	get_transform_fromsql(Oid typid, Oid langid, List *trftypes);
// extern Oid	get_transform_tosql(Oid typid, Oid langid, List *trftypes);
// extern Oid	get_promoted_array_type(Oid typid);
// extern int32 get_attavgwidth(Oid relid, AttrNumber attnum);
// extern char *get_namespace_name_or_temp(Oid nspid);

// extern void MemoryContextResetOnly(MemoryContext context);
// extern void MemoryContextStatsDetail(MemoryContext context, int max_children);
// extern bool RelationHasUnloggedIndex(Relation rel);

// extern List *RelationGetFKeyList(Relation relation);
// extern List *RelationGetIndexList(Relation relation);
// extern int	errtable(Relation rel);
// extern int	errtablecolname(Relation rel, const char *colname);
// extern int	errtableconstraint(Relation rel, const char *conname);
// extern Relation RelationBuildLocalRelation(const char *relname,
// 						   Oid relnamespace,
// 						   TupleDesc tupDesc,
// 						   Oid relid,
// 						   Oid relfilenode,
// 						   Oid reltablespace,
// 						   bool shared_relation,
// 						   bool mapped_relation,
// 						   char relpersistence,
// 						   char relkind);
// extern void RelationSetNewRelfilenode(Relation relation, char persistence,
// 						  TransactionId freezeXid, MultiXactId minmulti);
// extern int	check_enable_rls(Oid relid, Oid checkAsUser, bool noError);

// extern Size SnapMgrShmemSize(void);
// extern void SnapMgrInit(void);
// extern int64 GetSnapshotCurrentTimestamp(void);
// extern int64 GetOldSnapshotThresholdTimestamp(void);
// extern Snapshot GetTransactionSnapshot(void);
// extern Snapshot GetOldestSnapshot(void);
// extern Snapshot GetCatalogSnapshot(Oid relid);
// extern Snapshot GetNonHistoricCatalogSnapshot(Oid relid);
// extern void InvalidateCatalogSnapshot(void);
// extern void InvalidateCatalogSnapshotConditionally(void);
// extern TransactionId TransactionIdLimitedForOldSnapshots(TransactionId recentXmin,
// 									Relation relation);
// extern void MaintainOldSnapshotTimeMapping(int64 whenTaken, TransactionId xmin);
// extern char *ExportSnapshot(Snapshot snapshot);
// extern Size EstimateSnapshotSpace(Snapshot snapshot);
// extern void SerializeSnapshot(Snapshot snapshot, char *start_address);
// extern Snapshot RestoreSnapshot(char *start_address);
// extern void RestoreTransactionSnapshot(Snapshot snapshot, void *master_pgproc);

// extern bool RelationInvalidatesSnapshotsOnly(Oid relid);
// extern bool RelationHasSysCache(Oid relid);
// extern bool RelationSupportsSysCache(Oid relid);


// extern Datum make_timestamp(PG_FUNCTION_ARGS);
// extern Datum make_timestamptz(PG_FUNCTION_ARGS);
// extern Datum make_timestamptz_at_timezone(PG_FUNCTION_ARGS);
// extern Datum float8_timestamptz(PG_FUNCTION_ARGS);
// extern Datum make_interval(PG_FUNCTION_ARGS);
// extern Datum timestamp_zone_transform(PG_FUNCTION_ARGS);
// extern Datum timestamp_izone_transform(PG_FUNCTION_ARGS);
// extern Datum interval_combine(PG_FUNCTION_ARGS);
// extern Datum interval_accum_inv(PG_FUNCTION_ARGS);
// #ifndef HAVE_INT64_TIMESTAMP
// extern int64 GetCurrentIntegerTimestamp(void);
// extern TimestampTz IntegerTimestampToTimestampTz(int64 timestamp);
// #endif

// extern void InitDomainConstraintRef(Oid type_id, DomainConstraintRef *ref,
// 						MemoryContext refctx);
// extern void UpdateDomainConstraintRef(DomainConstraintRef *ref);
// extern bool DomainHasConstraints(Oid type_id);



// //third
// //commands/cluster.h
// extern void cluster_rel(Oid tableOid, Oid indexOid, bool recheck,bool verbose);
// extern void check_index_is_clusterable(Relation OldHeap, Oid indexOid,bool recheck, LOCKMODE lockmode);
// extern void mark_index_clustered(Relation rel, Oid indexOid, bool is_internal);
// extern Oid make_new_heap(Oid OIDOldHeap, Oid NewTableSpace, char relpersistence,
// 			  LOCKMODE lockmode);
// extern void finish_heap_swap(Oid OIDOldHeap, Oid OIDNewHeap,
// 				 bool is_system_catalog,
// 				 bool swap_toast_by_content,
// 				 bool check_constraints,
// 				 bool is_internal,
// 				 TransactionId frozenXid,
// 				 MultiXactId minMulti,
// 				 char newrelpersistence);
// //end

// //commands/dbcommands.h
// // extern ObjectAddress RenameDatabase(const char *oldname, const char *newname);
// // extern ObjectAddress AlterDatabaseOwner(const char *dbname, Oid newOwnerId);
// extern Oid	createdb(const CreatedbStmt *stmt);
// extern Oid	AlterDatabase(AlterDatabaseStmt *stmt, bool isTopLevel);
// extern Oid	AlterDatabaseSet(AlterDatabaseSetStmt *stmt);

// //end

// //commands/defrem.h


// /* commands/operatorcmds.c */
// extern ObjectAddress AlterOperator(AlterOperatorStmt *stmt);
// // extern ObjectAddress DefineOperator(List *names, List *parameters);

// /* commands/aggregatecmds.c */
// // extern ObjectAddress DefineAggregate(List *name, List *args, bool oldstyle,
// // 				List *parameters, const char *queryString);

// /* commands/opclasscmds.c */

// // extern ObjectAddress DefineOpClass(CreateOpClassStmt *stmt);
// // extern ObjectAddress DefineOpFamily(CreateOpFamilyStmt *stmt);
// // extern Oid	AlterOpFamily(AlterOpFamilyStmt *stmt);
// extern void IsThereOpClassInNamespace(const char *opcname, Oid opcmethod,
// 						  Oid opcnamespace);
// extern void IsThereOpFamilyInNamespace(const char *opfname, Oid opfmethod,
// 						   Oid opfnamespace);

// /* commands/tsearchcmds.c */
// // extern ObjectAddress DefineTSParser(List *names, List *parameters);
// // extern ObjectAddress DefineTSDictionary(List *names, List *parameters);
// // extern ObjectAddress AlterTSDictionary(AlterTSDictionaryStmt *stmt);
// // extern ObjectAddress DefineTSTemplate(List *names, List *parameters);
// extern ObjectAddress DefineTSConfiguration(List *names, List *parameters,
// 					  ObjectAddress *copied);
// // extern ObjectAddress AlterTSConfiguration(AlterTSConfigurationStmt *stmt);

// // /* commands/foreigncmds.c */
// // extern ObjectAddress AlterForeignServerOwner(const char *name, Oid newOwnerId);
// // extern ObjectAddress AlterForeignDataWrapperOwner(const char *name, Oid newOwnerId);
// // extern ObjectAddress CreateForeignDataWrapper(CreateFdwStmt *stmt);
// // extern ObjectAddress AlterForeignDataWrapper(AlterFdwStmt *stmt);
// // extern ObjectAddress CreateForeignServer(CreateForeignServerStmt *stmt);
// // extern ObjectAddress AlterForeignServer(AlterForeignServerStmt *stmt);
// // extern ObjectAddress CreateUserMapping(CreateUserMappingStmt *stmt);
// // extern ObjectAddress AlterUserMapping(AlterUserMappingStmt *stmt);
// // extern Oid	RemoveUserMapping(DropUserMappingStmt *stmt);
// extern void ImportForeignSchema(ImportForeignSchemaStmt *stmt);

// /* commands/dropcmds.c */
// extern void RemoveObjects(DropStmt *stmt);

// /* commands/indexcmds.c */
// // extern ObjectAddress DefineIndex(Oid relationId,
// // 			IndexStmt *stmt,
// // 			Oid indexRelationId,
// // 			bool is_alter_table,
// // 			bool check_rights,
// // 			bool skip_build,
// // 			bool quiet);
// extern Oid	ReindexIndex(RangeVar *indexRelation, int options);
// extern Oid	ReindexTable(RangeVar *relation, int options);
// extern char *makeObjectName(const char *name1, const char *name2,
// 			   const char *label);
// extern char *ChooseRelationName(const char *name1, const char *name2,
// 				   const char *label, Oid namespaceid);
// extern void ReindexMultipleTables(const char *objectName, ReindexObjectType objectKind,
// 					  int options);

// /* commands/functioncmds.c */
// // extern ObjectAddress CreateFunction(CreateFunctionStmt *stmt, const char *queryString);
// // extern ObjectAddress AlterFunction(AlterFunctionStmt *stmt);
// // extern ObjectAddress CreateCast(CreateCastStmt *stmt);
// extern void ExecuteDoStmt(DoStmt *stmt);
// extern ObjectAddress CreateTransform(CreateTransformStmt *stmt);
// extern void DropTransformById(Oid transformOid);
// extern void IsThereFunctionInNamespace(const char *proname, int pronargs,
// 						   oidvector *proargtypes, Oid nspOid);
// extern Oid	get_transform_oid(Oid type_id, Oid lang_id, bool missing_ok);
// extern void interpret_function_parameter_list(List *parameters,
// 								  Oid languageOid,
// 								  bool is_aggregate,
// 								  const char *queryString,
// 								  oidvector **parameterTypes,
// 								  ArrayType **allParameterTypes,
// 								  ArrayType **parameterModes,
// 								  ArrayType **parameterNames,
// 								  List **parameterDefaults,
// 								  Oid *variadicArgType,
// 								  Oid *requiredResultType);






// /* commands/amcmds.c */
// extern ObjectAddress CreateAccessMethod(CreateAmStmt *stmt);
// extern void RemoveAccessMethodById(Oid amOid);
// extern Oid	get_index_am_oid(const char *amname, bool missing_ok);
// extern char *get_am_name(Oid amOid);

// /* support routines in commands/define.c */
// extern int32 defGetInt32(DefElem *def);
// //end
// //end defrem.h

// //commands/explain.h
// extern void ExplainQuery(ExplainStmt *stmt, const char *queryString,
// 			 ParamListInfo params, DestReceiver *dest);
// extern void ExplainOnePlan(PlannedStmt *plannedstmt, IntoClause *into,
// 			   ExplainState *es, const char *queryString,
// 			   ParamListInfo params, const instr_time *planduration);
// // extern ExplainState *NewExplainState(void);
// extern void ExplainPrintTriggers(ExplainState *es, QueryDesc *queryDesc);
// extern void ExplainPropertyBool(const char *qlabel, bool value,
// 					ExplainState *es);
// extern PGDLLIMPORT explain_get_index_name_hook_type explain_get_index_name_hook;
// extern PGDLLIMPORT ExplainOneQuery_hook_type ExplainOneQuery_hook;
// //end explain.h

// //commands/sequence.h



// extern void seq_desc(StringInfo buf, XLogReaderState *rptr);//这个函数没有实现
// // extern ObjectAddress DefineSequence(CreateSeqStmt *stmt);
// // extern ObjectAddress AlterSequence(AlterSeqStmt *stmt);
// extern void ResetSequenceCaches(void);
// extern const char *seq_identify(uint8 info);//这个函数没有实现

// //end sequence.h

// //commands/tablecmds.h
// extern ObjectAddress DefineRelation(CreateStmt *stmt, char relkind, Oid ownerId,
// 			   ObjectAddress *typaddress);
// extern void RemoveRelations(DropStmt *drop);
// extern Oid	AlterTableLookupRelation(AlterTableStmt *stmt, LOCKMODE lockmode);
// // extern ObjectAddress AlterTableNamespace(AlterObjectSchemaStmt *stmt,
// // 					Oid *oldschema);
// extern void ExecuteTruncate(TruncateStmt *stmt);
// // extern ObjectAddress renameatt(RenameStmt *stmt);
// // extern ObjectAddress RenameConstraint(RenameStmt *stmt);
// // extern ObjectAddress RenameRelation(RenameStmt *stmt);
// extern void RenameRelationInternal(Oid myrelid,
// 					   const char *newrelname, bool is_internal);
// extern void RangeVarCallbackOwnsTable(const RangeVar *relation,
// 						  Oid relId, Oid oldRelId, void *arg);
// extern void RangeVarCallbackOwnsRelation(const RangeVar *relation,
// 							 Oid relId, Oid oldRelId, void *noCatalogs);
// extern Oid	AlterTableMoveAll(AlterTableMoveAllStmt *stmt);
// // extern ObjectAddress renameatt_type(RenameStmt *stmt);//这个函数没有实现

// //end tablecmds.h

// //commands/trigger.h
// // extern ObjectAddress CreateTrigger(CreateTrigStmt *stmt, const char *queryString,
// // 			  Oid relOid, Oid refRelOid, Oid constraintOid, Oid indexOid,
// // 			  bool isInternal);
// // extern ObjectAddress renametrig(RenameStmt *stmt);
// extern void ExecARInsertTriggers(EState *estate,
// 					 ResultRelInfo *relinfo,
// 					 HeapTuple trigtuple,
// 					 List *recheckIndexes);
// extern bool ExecBRDeleteTriggers(EState *estate,
// 					 EPQState *epqstate,
// 					 ResultRelInfo *relinfo,
// 					 ItemPointer tupleid,
// 					 HeapTuple fdw_trigtuple);
// extern void ExecARDeleteTriggers(EState *estate,
// 					 ResultRelInfo *relinfo,
// 					 ItemPointer tupleid,
// 					 HeapTuple fdw_trigtuple);
// extern TupleTableSlot *ExecBRUpdateTriggers(EState *estate,
// 					 EPQState *epqstate,
// 					 ResultRelInfo *relinfo,
// 					 ItemPointer tupleid,
// 					 HeapTuple fdw_trigtuple,
// 					 TupleTableSlot *slot);
// extern void ExecARUpdateTriggers(EState *estate,
// 					 ResultRelInfo *relinfo,
// 					 ItemPointer tupleid,
// 					 HeapTuple fdw_trigtuple,
// 					 HeapTuple newtuple,
// 					 List *recheckIndexes);
// extern bool RI_FKey_pk_upd_check_required(Trigger *trigger, Relation pk_rel,
// 							  HeapTuple old_row, HeapTuple new_row);
// extern bool RI_FKey_fk_upd_check_required(Trigger *trigger, Relation fk_rel,
// 							  HeapTuple old_row, HeapTuple new_row);
// extern int	RI_FKey_trigger_type(Oid tgfoid);
// //end trigger.h

// //新文件夹目录
// //executor/executor.h
// /*
//  * prototypes from functions in execAmi.c
//  */

// extern bool ExecSupportsMarkRestore(struct Path *pathnode);
// /*
//  * prototypes from functions in execCurrent.c
//  */

// extern bool execCurrentOf(CurrentOfExpr *cexpr,
// 			  ExprContext *econtext,
// 			  Oid table_oid,
// 			  ItemPointer current_tid);


// /*
//  * prototypes from functions in execGrouping.c
//  */
// // extern TupleHashTable BuildTupleHashTable(int numCols, AttrNumber *keyColIdx,
// // 					FmgrInfo *eqfunctions,
// // 					FmgrInfo *hashfunctions,
// // 					long nbuckets, Size entrysize,
// // 					MemoryContext tablecxt,
// // 					MemoryContext tempcxt);
// extern TupleHashEntry LookupTupleHashEntry(TupleHashTable hashtable,
// 					 TupleTableSlot *slot,
// 					 bool *isnew);

// /*
//  * prototypes from functions in execJunk.c
//  */
// // extern JunkFilter *ExecInitJunkFilter(List *targetList, bool hasoid,
// // 				   TupleTableSlot *slot);
// /*
//  * prototypes from functions in execMain.c
//  */
// extern void ExecutorRun(QueryDesc *queryDesc,
// 			ScanDirection direction, uint64 count);
// extern void standard_ExecutorRun(QueryDesc *queryDesc,
// 					 ScanDirection direction, uint64 count);
// extern ExecRowMark *ExecFindRowMark(EState *estate, Index rti, bool missing_ok);
// extern TupleTableSlot *EvalPlanQual(EState *estate, EPQState *epqstate,
// 			 Relation relation, Index rti, int lockmode,
// 			 ItemPointer tid, TransactionId priorXmax);
// extern HeapTuple EvalPlanQualFetch(EState *estate, Relation relation,
// 				  int lockmode, LockWaitPolicy wait_policy, ItemPointer tid,
// 				  TransactionId priorXmax);
// extern void EvalPlanQualSetTuple(EPQState *epqstate, Index rti,
// 					 HeapTuple tuple);
// extern HeapTuple EvalPlanQualGetTuple(EPQState *epqstate, Index rti);
// extern void EvalPlanQualBegin(EPQState *epqstate, EState *parentestate);
// extern void ExecWithCheckOptions(WCOKind kind, ResultRelInfo *resultRelInfo,
// 					 TupleTableSlot *slot, EState *estate);
// extern LockTupleMode ExecUpdateLockMode(EState *estate, ResultRelInfo *relinfo);


// /*
//  * prototypes from functions in execQual.c
//  */
// extern Tuplestorestate *ExecMakeTableFunctionResult(ExprState *funcexpr,
// 							ExprContext *econtext,
// 							MemoryContext argContext,
// 							TupleDesc expectedDesc,
// 							bool randomAccess);
// /*
//  * prototypes from functions in execTuples.c
//  */
// extern void ExecInitResultTupleSlot(EState *estate, PlanState *planstate);
// extern void ExecInitScanTupleSlot(EState *estate, ScanState *scanstate);
// extern TupleTableSlot *ExecInitExtraTupleSlot(EState *estate);
// extern TupleDesc ExecTypeFromTL(List *targetList, bool hasoid);
// extern TupleDesc ExecCleanTypeFromTL(List *targetList, bool hasoid);
// extern TupleDesc ExecTypeFromExprList(List *exprList);
// extern void do_tup_output(TupOutputState *tstate, Datum *values, bool *isnull);
// extern void do_text_output_multiline(TupOutputState *tstate, const char *txt);
// extern void ExecTypeSetColNames(TupleDesc typeInfo, List *namesList);

// /*
//  * prototypes from functions in execUtils.c
//  */
// // extern EState *CreateExecutorState(void);
// extern void ExecAssignResultTypeFromTL(PlanState *planstate);
// extern Relation ExecOpenScanRelation(EState *estate, Index scanrelid, int eflags);
// extern void ExecAssignExprContext(EState *estate, PlanState *planstate);
// /*
//  * prototypes from functions in execIndexing.c
//  */
// extern List *ExecInsertIndexTuples(TupleTableSlot *slot, ItemPointer tupleid,
// 					  EState *estate, bool noDupErr, bool *specConflict,
// 					  List *arbiterIndexes);
// extern bool ExecCheckIndexConstraints(TupleTableSlot *slot, EState *estate,
// 						  ItemPointer conflictTid, List *arbiterIndexes);
// extern void check_exclusion_constraint(Relation heap, Relation index,
// 						   IndexInfo *indexInfo,
// 						   ItemPointer tupleid,
// 						   Datum *values, bool *isnull,
// 						   EState *estate, bool newIndex);
// /*
//  * prototypes from functions in execProcnode.c
//  */
// extern bool ExecShutdownNode(PlanState *node);
// //end executor.h


// //executor/tuptable.h

// /* in executor/execTuples.c */
// extern TupleTableSlot *MakeTupleTableSlot(void);
// extern TupleTableSlot *ExecAllocTableSlot(List **tupleTable);
// extern TupleTableSlot *MakeSingleTupleTableSlot(TupleDesc tupdesc);
// extern TupleTableSlot *ExecStoreTuple(HeapTuple tuple,
// 			   TupleTableSlot *slot,
// 			   Buffer buffer,
// 			   bool shouldFree);
// extern MinimalTuple ExecCopySlotMinimalTuple(TupleTableSlot *slot);

// /* in access/common/heaptuple.c */
// extern Datum slot_getattr(TupleTableSlot *slot, int attnum, bool *isnull);
// extern void slot_getallattrs(TupleTableSlot *slot);
// extern void slot_getsomeattrs(TupleTableSlot *slot, int attnum);
// extern bool slot_attisnull(TupleTableSlot *slot, int attnum);

// //end tuptable.h

// //fmgr.h
// extern void fmgr_info_cxt(Oid functionId, FmgrInfo *finfo,
// 			  MemoryContext mcxt);
// extern void fmgr_info_copy(FmgrInfo *dstinfo, FmgrInfo *srcinfo,
// 			   MemoryContext destcxt);
// extern struct varlena *pg_detoast_datum(struct varlena * datum);
// extern struct varlena *pg_detoast_datum_copy(struct varlena * datum);
// extern struct varlena *pg_detoast_datum_slice(struct varlena * datum,
// 					   int32 first, int32 count);
// extern struct varlena *pg_detoast_datum_packed(struct varlena * datum);
// extern void fmgr_info(Oid functionId, FmgrInfo *finfo);
// //end fmgr.h

// //funcapi.h
// // extern TypeFuncClass get_expr_result_type(Node *expr,
// // 					 Oid *resultTypeId,
// // 					 TupleDesc *resultTupleDesc);
// extern bool resolve_polymorphic_argtypes(int numargs, Oid *argtypes,
// 							 char *argmodes,
// 							 Node *call_expr);
// extern TupleDesc build_function_result_tupdesc_d(Datum proallargtypes,
// 								Datum proargmodes,
// 								Datum proargnames);
// extern int	get_func_trftypes(HeapTuple procTup, Oid **p_trftypes);

// /* from execTuples.c */
// extern Datum HeapTupleHeaderGetDatum(HeapTupleHeader tuple);
// //end funcapi.h

// //新文件目录
// //lib/stringinfo.h
// // extern int	appendStringInfoVA(StringInfo str, const char *fmt, va_list args) pg_attribute_printf(2, 0);
// //end stringinfo.h

// //新文件目录
// //libpq/pqformat.h
// extern const char *pq_getmsgrawstring(StringInfo msg);
// extern void pq_sendbyte(StringInfo buf, int byt);
// extern void pq_sendint(StringInfo buf, int i, int b);
// extern void pq_sendint64(StringInfo buf, int64 i);
// //end pqformat.h

// //miscadmin.h
// /* now in utils/init/miscinit.c */
// extern char *GetUserNameFromId(Oid roleid, bool noerr);
// extern void InitializeSessionUserId(const char *rolename, Oid useroid);
// extern void CreateSocketLockFile(const char *socketfile, bool amPostmaster,
// 					 const char *socketDir);
// extern void InitPostmasterChild(void);
// extern void InitStandaloneProcess(const char *argv0);
// extern void SetDatabasePath(const char *path);
// extern bool InNoForceRLSOperation(void);
// extern void SwitchToSharedLatch(void);
// extern void SwitchBackToLocalLatch(void);
// extern void TouchSocketLockFiles(void);
// extern bool RecheckDataDirLockFile(void);
// extern void process_session_preload_libraries(void);

// /* in utils/init/postinit.c */
// extern void pg_split_opts(char **argv, int *argcp, const char *optstr);
// extern void InitializeMaxBackends(void);
// extern void InitPostgres(const char *in_dbname, Oid dboid, const char *username,
// 			 Oid useroid, char *out_dbname);

// /* in tcop/utility.c */
// extern void PreventCommandIfParallelMode(const char *cmdname);

// /* in tcop/postgres.c */
// extern void ProcessInterrupts(void);
// //end miscadmin.h


// //新文件夹
// //nodes/execnodes.h
// //此文件没有定义函数,都是结构体和定义

// //nodes/makefuncs.h
// extern Const *makeConst(Oid consttype,
// 		  int32 consttypmod,
// 		  Oid constcollid,
// 		  int constlen,
// 		  Datum constvalue,
// 		  bool constisnull,
// 		  bool constbyval);
// extern FuncCall *makeFuncCall(List *name, List *args, int location);
// extern ColumnDef *makeColumnDef(const char *colname,
// 			  Oid typeOid, int32 typmod, Oid collOid);

// //end makefuncs.h

// //nodes/nodeFuncs.h
// extern Node *strip_implicit_coercions(Node *node);
// extern void fix_opfuncids(Node *node);
// extern void set_opfuncid(OpExpr *opexpr);
// extern void set_sa_opfuncid(ScalarArrayOpExpr *opexpr);
// extern bool check_functions_in_node(Node *node, check_function_callback checker,
// 						void *context);
// struct PlanState;
// extern bool planstate_tree_walker(struct PlanState *planstate, bool (*walker) (),
// 											  void *context);
// //end nodeFuncs.h

// //nodes/readfuncs.h
// /*
//  * prototypes for functions in read.c (the lisp token parser)
//  */
// extern char *pg_strtok(int *length);
// extern char *debackslash(char *token, int length);
// //end readfuncs.h

// //新文件目录
// //optimizer/clauses.h
// // extern WindowFuncLists *find_window_functions(Node *clause, Index maxWinRef);
// extern bool contain_volatile_functions(Node *clause);
// extern bool contain_nonstrict_functions(Node *clause);
// extern Node *estimate_expression_value(PlannerInfo *root, Node *node);
// extern void get_agg_clause_costs(PlannerInfo *root, Node *clause,
// 					 AggSplit aggsplit, AggClauseCosts *costs);
// extern bool contain_volatile_functions_not_nextval(Node *clause);
// extern bool has_parallel_hazard(Node *node, bool allow_restricted);
// extern bool contain_leaked_vars(Node *clause);

// //end clauses.h


// //optimizer/cost.h
// /*
//  * prototypes for costsize.c
//  *	  routines to compute costs and sizes
//  */
// extern double index_pages_fetched(double tuples_fetched, BlockNumber pages,
// 					double index_pages, PlannerInfo *root);
// extern void cost_tidscan(Path *path, PlannerInfo *root,
// 			 RelOptInfo *baserel, List *tidquals, ParamPathInfo *param_info);
// extern void cost_subqueryscan(SubqueryScanPath *path, PlannerInfo *root,
// 				  RelOptInfo *baserel, ParamPathInfo *param_info);
// extern void cost_functionscan(Path *path, PlannerInfo *root,
// 				  RelOptInfo *baserel, ParamPathInfo *param_info);
// extern void cost_valuesscan(Path *path, PlannerInfo *root,
// 				RelOptInfo *baserel, ParamPathInfo *param_info);
// extern void cost_ctescan(Path *path, PlannerInfo *root,
// 			 RelOptInfo *baserel, ParamPathInfo *param_info);
// extern void cost_recursive_union(Path *runion, Path *nrterm, Path *rterm);
// extern void cost_sort(Path *path, PlannerInfo *root,
// 		  List *pathkeys, Cost input_cost, double tuples, int width,
// 		  Cost comparison_cost, int sort_mem,
// 		  double limit_tuples);
// extern void cost_agg(Path *path, PlannerInfo *root,
// 		 AggStrategy aggstrategy, const AggClauseCosts *aggcosts,
// 		 int numGroupCols, double numGroups,
// 		 Cost input_startup_cost, Cost input_total_cost,
// 		 double input_tuples);
// extern void initial_cost_nestloop(PlannerInfo *root,
// 					  JoinCostWorkspace *workspace,
// 					  JoinType jointype,
// 					  Path *outer_path, Path *inner_path,
// 					  SpecialJoinInfo *sjinfo,
// 					  SemiAntiJoinFactors *semifactors);
// extern void final_cost_nestloop(PlannerInfo *root, NestPath *path,
// 					JoinCostWorkspace *workspace,
// 					SpecialJoinInfo *sjinfo,
// 					SemiAntiJoinFactors *semifactors);
// extern void final_cost_mergejoin(PlannerInfo *root, MergePath *path,
// 					 JoinCostWorkspace *workspace,
// 					 SpecialJoinInfo *sjinfo);
// extern void initial_cost_hashjoin(PlannerInfo *root,
// 					  JoinCostWorkspace *workspace,
// 					  JoinType jointype,
// 					  List *hashclauses,
// 					  Path *outer_path, Path *inner_path,
// 					  SpecialJoinInfo *sjinfo,
// 					  SemiAntiJoinFactors *semifactors);
// extern void final_cost_hashjoin(PlannerInfo *root, HashPath *path,
// 					JoinCostWorkspace *workspace,
// 					SpecialJoinInfo *sjinfo,
// 					SemiAntiJoinFactors *semifactors);
// extern void cost_gather(GatherPath *path, PlannerInfo *root,
// 			RelOptInfo *baserel, ParamPathInfo *param_info, double *rows);
// //end cost.h

// //optimizer/plancat.h
// extern void estimate_rel_size(Relation rel, int32 *attr_widths,
// 				  BlockNumber *pages, double *tuples, double *allvisfrac);
// extern int32 get_relation_data_width(Oid relid, int32 *attr_widths);
// extern List *infer_arbiter_indexes(PlannerInfo *root);
// extern bool has_row_triggers(PlannerInfo *root, Index rti, CmdType event);

// //end plancat.h

// //optimizer/planner.h
// extern PlannerInfo *subquery_planner(PlannerGlobal *glob, Query *parse,
// 				 PlannerInfo *parent_root,
// 				 bool hasRecursion, double tuple_fraction);
// extern RowMarkType select_rowmark_type(RangeTblEntry *rte,
// 					LockClauseStrength strength);

// extern void mark_partial_aggref(Aggref *agg, AggSplit aggsplit);
// extern Path *get_cheapest_fractional_path(RelOptInfo *rel,
// 							 double tuple_fraction);

// //end planner.h
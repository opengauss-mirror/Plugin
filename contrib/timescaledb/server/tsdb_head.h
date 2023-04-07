// //frist
// #include "storage/buf/bufpage.h"
// #include "nodes/lockoptions.h"
// #include "access/amapi.h"
// #include "access/tupdesc.h"
// #include "storage/lock/lock.h"
// #include "access/attnum.h"
// #include "access/htup.h"
// #include "storage/sinval.h"
// #include "lib/stringinfo.h"
// #include "storage/lock/lock.h"
// #include "nodes/pg_list.h"
// #include "utils/acl.h"
// #include "nodes/params.h"
// #include "nodes/plannodes.h"
// #include "utils/expandeddatum.h"

// #include "storage/smgr/relfilenode.h"
// #include "access/attnum.h"
// #include "utils/lsyscache.h"
// #include "nodes/pg_list.h"
// #include "access/xlog.h"
// #include "fmgr.h"
// #include "utils/relcache.h"

// typedef struct ParseState ParseState; 
// typedef bool (*check_function_callback) (Oid func_id, void *context); 
// typedef int32 (*get_attavgwidth_hook_type) (Oid relid, AttrNumber attnum); 
// typedef const char *(*explain_get_index_name_hook_type) (Oid indexId); 
// typedef struct DomainConstraintCache DomainConstraintCache; 
// typedef void (*MemoryContextCallbackFunction) (void *arg); 
// typedef void (*ExplainOneQuery_hook_type) (Query *query,
// 													   IntoClause *into,
// 													   ExplainState *es,
// 													 const char *queryString,
// 													   ParamListInfo params);
 
// #define AGGSPLITOP_COMBINE		0x01	/* substitute combinefn for transfn */
// #define AGGSPLITOP_SKIPFINAL	0x02	/* skip finalfn, return state as-is */
// #define AGGSPLITOP_SERIALIZE	0x04	/* apply serializefn to output */
// #define AGGSPLITOP_DESERIALIZE	0x08	/* apply deserializefn to input */ 
// typedef enum
// {
// 	HeapTupleMayBeUpdated,
// 	HeapTupleInvisible,
// 	HeapTupleSelfUpdated,
// 	HeapTupleUpdated,
// 	HeapTupleBeingUpdated,
// 	HeapTupleWouldBlock			/* can be returned by heap_tuple_lock */
// } HTSU_Result; 
// typedef struct ObjectAddress
// {
// 	Oid			classId;		/* Class Id from pg_class */
// 	Oid			objectId;		/* OID of the object */
// 	int32		objectSubId;	/* Subitem within object (eg column), or 0 */
// } ObjectAddress; 
// typedef struct HeapUpdateFailureData
// {
// 	ItemPointerData ctid;
// 	TransactionId xmax;
// 	CommandId	cmax;
// } HeapUpdateFailureData; 

// typedef enum
// {
// 	FUNCDETAIL_NOTFOUND,		/* no matching function */
// 	FUNCDETAIL_MULTIPLE,		/* too many matching functions */
// 	FUNCDETAIL_NORMAL,			/* found a matching regular function */
// 	FUNCDETAIL_AGGREGATE,		/* found a matching aggregate function */
// 	FUNCDETAIL_WINDOWFUNC,		/* found a matching window function */
// 	FUNCDETAIL_COERCION			/* it's a type coercion request */
// } FuncDetailCode; 

// typedef struct
// {
// 	int			numWindowFuncs; /* total number of WindowFuncs found */
// 	Index		maxWinRef;		/* windowFuncs[] is indexed 0 .. maxWinRef */
// 	List	  **windowFuncs;	/* lists of WindowFuncs for each winref */
// } WindowFuncLists; 

// typedef struct GatherPath
// {
// 	Path		path;
// 	Path	   *subpath;		/* path for each worker */
// 	bool		single_copy;	/* path must not be executed >1x */
// 	int			num_workers;	/* number of workers sought to help */
// } GatherPath; 

// typedef struct TupOutputState
// {
// 	TupleTableSlot *slot;
// 	DestReceiver *dest;
// } TupOutputState; 

// typedef enum TypeFuncClass
// {
// 	TYPEFUNC_SCALAR,			/* scalar result type */
// 	TYPEFUNC_COMPOSITE,			/* determinable rowtype result */
// 	TYPEFUNC_RECORD,			/* indeterminate rowtype result */
// 	TYPEFUNC_OTHER				/* bogus type, eg pseudotype */
// } TypeFuncClass; 

// typedef enum
// {
// 	XACT_EVENT_COMMIT,
// 	XACT_EVENT_PARALLEL_COMMIT,
// 	XACT_EVENT_ABORT,
// 	XACT_EVENT_PARALLEL_ABORT,
// 	XACT_EVENT_PREPARE,
// 	XACT_EVENT_PRE_COMMIT,
// 	XACT_EVENT_PARALLEL_PRE_COMMIT,
// 	XACT_EVENT_PRE_PREPARE
// } XactEvent;
// typedef enum
// {
// 	SUBXACT_EVENT_START_SUB,
// 	SUBXACT_EVENT_COMMIT_SUB,
// 	SUBXACT_EVENT_ABORT_SUB,
// 	SUBXACT_EVENT_PRE_COMMIT_SUB
// } SubXactEvent;

// struct ParseState
// {
// 	struct ParseState *parentParseState;		/* stack link */
// 	const char *p_sourcetext;	/* source text, or NULL if not available */
// 	List	   *p_rtable;		/* range table so far */
// 	List	   *p_joinexprs;	/* JoinExprs for RTE_JOIN p_rtable entries */
// 	List	   *p_joinlist;		/* join items so far (will become FromExpr
// 								 * node's fromlist) */
// 	List	   *p_namespace;	/* currently-referenceable RTEs (List of
// 								 * ParseNamespaceItem) */
// 	bool		p_lateral_active;		/* p_lateral_only items visible? */
// 	List	   *p_ctenamespace; /* current namespace for common table exprs */
// 	List	   *p_future_ctes;	/* common table exprs not yet in namespace */
// 	CommonTableExpr *p_parent_cte;		/* this query's containing CTE */
// 	List	   *p_windowdefs;	/* raw representations of window clauses */
// 	//ParseExprKind p_expr_kind;	/* what kind of expression we're parsing */
// 	int			p_next_resno;	/* next targetlist resno to assign */
// 	List	   *p_multiassign_exprs;	/* junk tlist entries for multiassign */
// 	List	   *p_locking_clause;		/* raw FOR UPDATE/FOR SHARE info */
// 	Node	   *p_value_substitute;		/* what to replace VALUE with, if any */
// 	bool		p_hasAggs;
// 	bool		p_hasWindowFuncs;
// 	bool		p_hasSubLinks;
// 	bool		p_hasModifyingCTE;
// 	bool		p_is_insert;
// 	bool		p_locked_from_parent;
// 	Relation	p_target_relation;
// 	RangeTblEntry *p_target_rangetblentry;

// 	/*
// 	 * Optional hook functions for parser callbacks.  These are null unless
// 	 * set up by the caller of make_parsestate.
// 	 */
// 	// PreParseColumnRefHook p_pre_columnref_hook;
// 	// PostParseColumnRefHook p_post_columnref_hook;
// 	// ParseParamRefHook p_paramref_hook;
// 	// CoerceParamHook p_coerce_param_hook;
// 	void	   *p_ref_hook_state;		/* common passthrough link for above */
// }; 
// typedef struct ExplainState
// {
// 	StringInfo	str;			/* output buffer */
// 	/* options */
// 	bool		verbose;		/* be verbose */
// 	bool		analyze;		/* print actual times */
// 	bool		costs;			/* print estimated costs */
// 	bool		buffers;		/* print buffer usage */
// 	bool		timing;			/* print detailed node timing */
// 	bool		summary;		/* print total planning and execution timing */
// 	//ExplainFormat format;		/* output format */
// 	/* state for output formatting --- not reset for each new plan tree */
// 	int			indent;			/* current indentation level */
// 	List	   *grouping_stack; /* format-specific grouping state */
// 	/* state related to the current plan tree (filled by ExplainPrintPlan) */
// 	PlannedStmt *pstmt;			/* top of plan */
// 	List	   *rtable;			/* range table */
// 	List	   *rtable_names;	/* alias names for RTEs */
// 	List	   *deparse_cxt;	/* context list for deparsing expressions */
// 	Bitmapset  *printed_subplans;		/* ids of SubPlans we've printed */
// } ExplainState; 
// #define XLOG_XACT_OPMASK			0x70
// #define XLOG_XACT_HAS_INFO			0x80
// #define XACT_XINFO_HAS_DBINFO			(1U << 0)
// #define XACT_XINFO_HAS_SUBXACTS			(1U << 1)
// #define XACT_XINFO_HAS_RELFILENODES		(1U << 2)
// #define XACT_XINFO_HAS_INVALS			(1U << 3)
// #define XACT_XINFO_HAS_TWOPHASE			(1U << 4)
// #define XACT_XINFO_HAS_ORIGIN			(1U << 5)
// #define XACT_COMPLETION_APPLY_FEEDBACK			(1U << 29)
// #define XACT_COMPLETION_UPDATE_RELCACHE_FILE	(1U << 30)
// #define XACT_COMPLETION_FORCE_SYNC_COMMIT		(1U << 31)
// typedef struct xl_xact_assignment
// {
// 	TransactionId xtop;			/* assigned XID's top-level XID */
// 	int			nsubxacts;		/* number of subtransaction XIDs */
// 	TransactionId xsub[FLEXIBLE_ARRAY_MEMBER];	/* assigned subxids */
// } xl_xact_assignment;
// typedef struct xl_xact_xinfo
// {
// 	/*
// 	 * Even though we right now only require 1 byte of space in xinfo we use
// 	 * four so following records don't have to care about alignment. Commit
// 	 * records can be large, so copying large portions isn't attractive.
// 	 */
// 	uint32		xinfo;
// } xl_xact_xinfo;
// typedef struct xl_xact_dbinfo
// {
// 	Oid			dbId;			/* MyDatabaseId */
// 	Oid			tsId;			/* MyDatabaseTableSpace */
// } xl_xact_dbinfo;
// typedef struct xl_xact_subxacts
// {
// 	int			nsubxacts;		/* number of subtransaction XIDs */
// 	TransactionId subxacts[FLEXIBLE_ARRAY_MEMBER];
// } xl_xact_subxacts;
// typedef struct xl_xact_relfilenodes
// {
// 	int			nrels;			/* number of subtransaction XIDs */
// 	RelFileNode xnodes[FLEXIBLE_ARRAY_MEMBER];
// } xl_xact_relfilenodes;
// typedef struct xl_xact_invals
// {
// 	int			nmsgs;			/* number of shared inval msgs */
// 	SharedInvalidationMessage msgs[FLEXIBLE_ARRAY_MEMBER];
// } xl_xact_invals;
// typedef struct xl_xact_twophase
// {
// 	TransactionId xid;
// } xl_xact_twophase;
// typedef struct xl_xact_parsed_commit
// {
// 	TimestampTz xact_time;

// 	uint32		xinfo;

// 	Oid			dbId;			/* MyDatabaseId */
// 	Oid			tsId;			/* MyDatabaseTableSpace */

// 	int			nsubxacts;
// 	TransactionId *subxacts;

// 	int			nrels;
// 	RelFileNode *xnodes;

// 	int			nmsgs;
// 	SharedInvalidationMessage *msgs;

// 	TransactionId twophase_xid; /* only for 2PC */

// 	XLogRecPtr	origin_lsn;
// 	TimestampTz origin_timestamp;
// } xl_xact_parsed_commit;
// typedef struct xl_xact_parsed_abort
// {
// 	TimestampTz xact_time;
// 	uint32		xinfo;

// 	int			nsubxacts;
// 	TransactionId *subxacts;

// 	int			nrels;
// 	RelFileNode *xnodes;

// 	TransactionId twophase_xid; /* only for 2PC */
// } xl_xact_parsed_abort;
// typedef enum DependencyType
// {
// 	DEPENDENCY_NORMAL = 'n',
// 	DEPENDENCY_AUTO = 'a',
// 	DEPENDENCY_INTERNAL = 'i',
// 	DEPENDENCY_EXTENSION = 'e',
// 	DEPENDENCY_AUTO_EXTENSION = 'x',
// 	DEPENDENCY_PIN = 'p'
// } DependencyType;
// typedef enum SharedDependencyType
// {
// 	SHARED_DEPENDENCY_PIN = 'p',
// 	SHARED_DEPENDENCY_OWNER = 'o',
// 	SHARED_DEPENDENCY_ACL = 'a',
// 	SHARED_DEPENDENCY_POLICY = 'r',
// 	SHARED_DEPENDENCY_INVALID = 0
// } SharedDependencyType;
// #define LAST_OCLASS		OCLASS_TRANSFORM
// #define InitPrivsObjIndexId  3395
// #define EventTriggerNameIndexId  3467
// #define TransformOidIndexId 3574
// #define TransformTypeLangIndexId  3575
// #define PolicyOidIndexId
// #define PolicyPolrelidPolnameIndexId

// extern const ObjectAddress InvalidObjectAddress;
// DATA(insert OID = 32 ( pg_ddl_command	PGNSP PGUID SIZEOF_POINTER t p P f t \054 0 0 0 pg_ddl_command_in pg_ddl_command_out pg_ddl_command_recv pg_ddl_command_send - - - ALIGNOF_POINTER p f 0 -1 0 0 _null_ _null_ _null_ ));
// DATA(insert OID = 4096 ( regrole	   PGNSP PGUID	4 t b N f t \054 0	 0 4097 regrolein regroleout regrolerecv regrolesend - - - i p f 0 -1 0 0 _null_ _null_ _null_ ));
// DATA(insert OID = 4089 ( regnamespace  PGNSP PGUID	4 t b N f t \054 0	 0 4090 regnamespacein regnamespaceout regnamespacerecv regnamespacesend - - - i p f 0 -1 0 0 _null_ _null_ _null_ ));
// DATA(insert OID = 4097 ( _regrole	   PGNSP PGUID -1 f b A f t \054 0 4096 0 array_in array_out array_recv array_send - - array_typanalyze i x f 0 -1 0 0 _null_ _null_ _null_ ));
// DATA(insert OID = 4090 ( _regnamespace PGNSP PGUID -1 f b A f t \054 0 4089 0 array_in array_out array_recv array_send - - array_typanalyze i x f 0 -1 0 0 _null_ _null_ _null_ ));
// DATA(insert OID = 3220 ( pg_lsn			PGNSP PGUID 8 FLOAT8PASSBYVAL b U f t \054 0 0 3221 pg_lsn_in pg_lsn_out pg_lsn_recv pg_lsn_send - - - d p f 0 -1 0 0 _null_ _null_ _null_ ));
// DATA(insert OID = 3221 ( _pg_lsn			PGNSP PGUID -1 f b A f t \054 0 3220 0 array_in array_out array_recv array_send - - array_typanalyze d x f 0 -1 0 0 _null_ _null_ _null_ ));
// DATA(insert OID = 3838 ( event_trigger		PGNSP PGUID  4 t p P f t \054 0 0 0 event_trigger_in event_trigger_out - - - - - i p f 0 -1 0 0 _null_ _null_ _null_ ));
// DATA(insert OID = 325 ( index_am_handler	PGNSP PGUID  4 t p P f t \054 0 0 0 index_am_handler_in index_am_handler_out - - - - - i p f 0 -1 0 0 _null_ _null_ _null_ ));
// DATA(insert OID = 3310 ( tsm_handler	PGNSP PGUID  4 t p P f t \054 0 0 0 tsm_handler_in tsm_handler_out - - - - - i p f 0 -1 0 0 _null_ _null_ _null_ ));
// #define PgShseclabelToastTable 4060
// #define PgShseclabelToastIndex 4061



// DATA(insert OID = 3315 (  "<>"	   PGNSP PGUID b f f	28	28	16	3315   352 xidneq neqsel neqjoinsel ));
// DESCR("not equal");
// DATA(insert OID = 3316 (  "<>"	   PGNSP PGUID b f f	28	23	16	0	   353 xidneqint4 neqsel neqjoinsel ));
// DESCR("not equal");
// DATA(insert OID = 3291 (  "<->"   PGNSP PGUID b f f  718	600  701   1522    0 dist_cpoint - - ));
// DESCR("distance between");
// DATA(insert OID = 3276 (  "<->"   PGNSP PGUID b f f  600	604  701   3289    0 dist_ppoly - - ));
// DESCR("distance between");
// DATA(insert OID = 3289 (  "<->"   PGNSP PGUID b f f  604	600  701   3276    0 dist_polyp - - ));
// DESCR("distance between");
// DATA(insert OID = 3552	(  "&&"    PGNSP PGUID b f f 869 869	 16 3552	0 network_overlap networksel networkjoinsel ));
// DESCR("overlaps (is subnet or supernet)");
// #define OID_INET_OVERLAP_OP		3552
// DATA(insert OID = 3222 (  "="	   PGNSP PGUID b t t 3220 3220 16 3222 3223 pg_lsn_eq eqsel eqjoinsel ));
// DESCR("equal");
// DATA(insert OID = 3223 (  "<>"	   PGNSP PGUID b f f 3220 3220 16 3223 3222 pg_lsn_ne neqsel neqjoinsel ));
// DESCR("not equal");
// DATA(insert OID = 3224 (  "<"	   PGNSP PGUID b f f 3220 3220 16 3225 3227 pg_lsn_lt scalarltsel scalarltjoinsel ));
// DESCR("less than");
// DATA(insert OID = 3225 (  ">"	   PGNSP PGUID b f f 3220 3220 16 3224 3226 pg_lsn_gt scalargtsel scalargtjoinsel ));
// DESCR("greater than");
// DATA(insert OID = 3226 (  "<="	   PGNSP PGUID b f f 3220 3220 16 3227 3225 pg_lsn_le scalarltsel scalarltjoinsel ));
// DESCR("less than or equal");
// DATA(insert OID = 3227 (  ">="	   PGNSP PGUID b f f 3220 3220 16 3226 3224 pg_lsn_ge scalargtsel scalargtjoinsel ));
// DESCR("greater than or equal");
// DATA(insert OID = 3228 (  "-"	   PGNSP PGUID b f f 3220 3220 1700    0	0 pg_lsn_mi - - ));
// DESCR("minus");
// DATA(insert OID = 5005 (  "<->"    PGNSP PGUID b f f 3615	 3615	 3615  0	0	 5003	-		-	  ));
// DESCR("phrase-concatenate");
// DATA(insert OID = 3188 (  "*="	   PGNSP PGUID b t f 2249 2249 16 3188 3189 record_image_eq eqsel eqjoinsel ));
// DESCR("identical");
// DATA(insert OID = 3189 (  "*<>"   PGNSP PGUID b f f 2249 2249 16 3189 3188 record_image_ne neqsel neqjoinsel ));
// DESCR("not identical");
// DATA(insert OID = 3190 (  "*<"	   PGNSP PGUID b f f 2249 2249 16 3191 3193 record_image_lt scalarltsel scalarltjoinsel ));
// DESCR("less than");
// DATA(insert OID = 3191 (  "*>"	   PGNSP PGUID b f f 2249 2249 16 3190 3192 record_image_gt scalargtsel scalargtjoinsel ));
// DESCR("greater than");
// DATA(insert OID = 3192 (  "*<="   PGNSP PGUID b f f 2249 2249 16 3193 3191 record_image_le scalarltsel scalarltjoinsel ));
// DESCR("less than or equal");
// DATA(insert OID = 3193 (  "*>="   PGNSP PGUID b f f 2249 2249 16 3192 3190 record_image_ge scalargtsel scalargtjoinsel ));
// DESCR("greater than or equal");
// DATA(insert OID = 3188 (  "*="	   PGNSP PGUID b t f 2249 2249 16 3188 3189 record_image_eq eqsel eqjoinsel ));
// DESCR("identical");
// DATA(insert OID = 3189 (  "*<>"   PGNSP PGUID b f f 2249 2249 16 3189 3188 record_image_ne neqsel neqjoinsel ));
// DESCR("not identical");
// DATA(insert OID = 3190 (  "*<"	   PGNSP PGUID b f f 2249 2249 16 3191 3193 record_image_lt scalarltsel scalarltjoinsel ));
// DESCR("less than");
// DATA(insert OID = 3191 (  "*>"	   PGNSP PGUID b f f 2249 2249 16 3190 3192 record_image_gt scalargtsel scalargtjoinsel ));
// DESCR("greater than");
// DATA(insert OID = 3192 (  "*<="   PGNSP PGUID b f f 2249 2249 16 3193 3191 record_image_le scalarltsel scalarltjoinsel ));
// DESCR("less than or equal");
// DATA(insert OID = 3193 (  "*>="   PGNSP PGUID b f f 2249 2249 16 3192 3190 record_image_ge scalargtsel scalargtjoinsel ));
// DESCR("greater than or equal");



// DATA(insert OID = 3494 (  to_regproc		PGNSP PGUID 12 1 0 0 0 f f f f t f s s 1 0 24 "25" _null_ _null_ _null_ _null_ _null_ to_regproc _null_ _null_ _null_ ));
// DESCR("convert proname to regproc");
// DATA(insert OID = 3479 (  to_regprocedure	PGNSP PGUID 12 1 0 0 0 f f f f t f s s 1 0 2202 "25" _null_ _null_ _null_ _null_ _null_ to_regprocedure _null_ _null_ _null_ ));
// DESCR("convert proname to regprocedure");
// DATA(insert OID = 3308 (  xidneq		   PGNSP PGUID 12 1 0 0 0 f f f t t f i s 2 0 16 "28 28" _null_ _null_ _null_ _null_ _null_ xidneq _null_ _null_ _null_ ));
// DATA(insert OID = 86  (  pg_ddl_command_in		PGNSP PGUID 12 1 0 0 0 f f f f t f i s 1 0 32 "2275" _null_ _null_ _null_ _null_ _null_ pg_ddl_command_in _null_ _null_ _null_ ));
// DESCR("I/O");
// DATA(insert OID = 87  (  pg_ddl_command_out		PGNSP PGUID 12 1 0 0 0 f f f f t f i s 1 0 2275 "32" _null_ _null_ _null_ _null_ _null_ pg_ddl_command_out _null_ _null_ _null_ ));
// DESCR("I/O");
// DATA(insert OID = 88  (  pg_ddl_command_recv	PGNSP PGUID 12 1 0 0 0 f f f f t f i s 1 0 32 "2281" _null_ _null_ _null_ _null_ _null_ pg_ddl_command_recv _null_ _null_ _null_ ));
// DESCR("I/O");
// DATA(insert OID = 3275 (  dist_ppoly	   PGNSP PGUID 12 1 0 0 0 f f f f t f i s 2 0 701 "600 604" _null_ _null_ _null_ _null_ _null_	dist_ppoly _null_ _null_ _null_ ));
// DATA(insert OID = 3292 (  dist_polyp	   PGNSP PGUID 12 1 0 0 0 f f f f t f i s 2 0 701 "604 600" _null_ _null_ _null_ _null_ _null_	dist_polyp _null_ _null_ _null_ ));
// DATA(insert OID = 3290 (  dist_cpoint	   PGNSP PGUID 12 1 0 0 0 f f f f t f i s 2 0 701 "718 600" _null_ _null_ _null_ _null_ _null_	dist_cpoint _null_ _null_ _null_ ));
// DATA(insert OID = 3277 (  array_position		   PGNSP PGUID 12 1 0 0 0 f f f f f f i s 2 0 23 "2277 2283" _null_ _null_ _null_ _null_ _null_ array_position _null_ _null_ _null_ ));
// DESCR("returns an offset of value in array");
// DATA(insert OID = 3278 (  array_position		   PGNSP PGUID 12 1 0 0 0 f f f f f f i s 3 0 23 "2277 2283 23" _null_ _null_ _null_ _null_ _null_ array_position_start _null_ _null_ _null_ ));
// DESCR("returns an offset of value in array with start index");
// DATA(insert OID = 3279 (  array_positions		   PGNSP PGUID 12 1 0 0 0 f f f f f f i s 2 0 1007 "2277 2283" _null_ _null_ _null_ _null_ _null_ array_positions _null_ _null_ _null_ ));
// DESCR("returns an array of offsets of some value in array");
// DATA(insert OID = 4051 (  array_agg_array_transfn	PGNSP PGUID 12 1 0 0 0 f f f f f f i s 2 0 2281 "2281 2277" _null_ _null_ _null_ _null_ _null_ array_agg_array_transfn _null_ _null_ _null_ ));
// DESCR("aggregate transition function");
// DATA(insert OID = 4052 (  array_agg_array_finalfn	PGNSP PGUID 12 1 0 0 0 f f f f f f i s 2 0 2277 "2281 2277" _null_ _null_ _null_ _null_ _null_ array_agg_array_finalfn _null_ _null_ _null_ ));
// DESCR("aggregate final function");
// DATA(insert OID = 4053 (  array_agg		   PGNSP PGUID 12 1 0 0 0 t f f f f f i s 1 0 2277 "2277" _null_ _null_ _null_ _null_ _null_ aggregate_dummy _null_ _null_ _null_ ));
// DESCR("concatenate aggregate input into an array");
// DATA(insert OID = 3218 ( width_bucket	   PGNSP PGUID 12 1 0 0 0 f f f f t f i s 2 0 23 "2283 2277" _null_ _null_ _null_ _null_ _null_ width_bucket_array _null_ _null_ _null_ ));
// DESCR("bucket number of operand given a sorted array of bucket lower bounds");
// DATA(insert OID = 3457 (  lo_from_bytea    PGNSP PGUID 12 1 0 0 0 f f f f t f v u 2 0 26 "26 17" _null_ _null_ _null_ _null_ _null_ lo_from_bytea _null_ _null_ _null_ ));
// DESCR("create new large object with given content");
// DATA(insert OID = 3458 (  lo_get		   PGNSP PGUID 12 1 0 0 0 f f f f t f v u 1 0 17 "26" _null_ _null_ _null_ _null_ _null_ lo_get _null_ _null_ _null_ ));
// DESCR("read entire large object");
// DATA(insert OID = 3459 (  lo_get		   PGNSP PGUID 12 1 0 0 0 f f f f t f v u 3 0 17 "26 20 23" _null_ _null_ _null_ _null_ _null_ lo_get_fragment _null_ _null_ _null_ ));
// DESCR("read large object from offset for length");
// DATA(insert OID = 3460 (  lo_put		   PGNSP PGUID 12 1 0 0 0 f f f f t f v u 3 0 2278 "26 20 17" _null_ _null_ _null_ _null_ _null_ lo_put _null_ _null_ _null_ ));
// DESCR("write data at offset");
// DATA(insert OID = 3328 ( bpchar_sortsupport PGNSP PGUID 12 1 0 0 0 f f f f t f i s 1 0 2278 "2281" _null_ _null_ _null_ _null_ _null_ bpchar_sortsupport _null_ _null_ _null_ ));
// DESCR("sort support");
// DATA(insert OID = 3546 (  int8dec		   PGNSP PGUID 12 1 0 0 0 f f f f t f i s 1 0 20 "20" _null_ _null_ _null_ _null_ _null_ int8dec _null_ _null_ _null_ ));
// DESCR("decrement");
// DATA(insert OID = 3547 (  int8dec_any	   PGNSP PGUID 12 1 0 0 0 f f f f t f i s 2 0 20 "20 2276" _null_ _null_ _null_ _null_ _null_ int8dec_any _null_ _null_ _null_ ));
// DESCR("decrement, ignores second argument");
// DATA(insert OID = 3309 (  xidneqint4		 PGNSP PGUID 12 1 0 0 0 f f f t t f i s 2 0 16 "28 23" _null_ _null_ _null_ _null_ _null_ xidneq _null_ _null_ _null_ ));
// DATA(insert OID = 4063 (  inet_merge		PGNSP PGUID 12 1 0 0 0 f f f f t f i s 2 0 650 "869 869" _null_ _null_ _null_ _null_ _null_ inet_merge _null_ _null_ _null_ ));
// DESCR("the smallest network which includes both of the given networks");
// DATA(insert OID = 3553 (  inet_gist_consistent	PGNSP PGUID 12 1 0 0 0 f f f f t f i s 5 0 16 "2281 869 21 26 2281" _null_ _null_ _null_ _null_ _null_ inet_gist_consistent _null_ _null_ _null_ ));
// DESCR("GiST support");
// DATA(insert OID = 3554 (  inet_gist_union		PGNSP PGUID 12 1 0 0 0 f f f f t f i s 2 0 869 "2281 2281" _null_ _null_ _null_ _null_ _null_ inet_gist_union _null_ _null_ _null_ ));
// DESCR("GiST support");
// DATA(insert OID = 3555 (  inet_gist_compress	PGNSP PGUID 12 1 0 0 0 f f f f t f i s 1 0 2281 "2281" _null_ _null_ _null_ _null_ _null_ inet_gist_compress _null_ _null_ _null_ ));
// DESCR("GiST support");
// DATA(insert OID = 3556 (  inet_gist_decompress	PGNSP PGUID 12 1 0 0 0 f f f f t f i s 1 0 2281 "2281" _null_ _null_ _null_ _null_ _null_ inet_gist_decompress _null_ _null_ _null_ ));
// DESCR("GiST support");
// DATA(insert OID = 3573 (  inet_gist_fetch		PGNSP PGUID 12 1 0 0 0 f f f f t f i s 1 0 2281 "2281" _null_ _null_ _null_ _null_ _null_ inet_gist_fetch _null_ _null_ _null_ ));
// DESCR("GiST support");
// DATA(insert OID = 3557 (  inet_gist_penalty		PGNSP PGUID 12 1 0 0 0 f f f f t f i s 3 0 2281 "2281 2281 2281" _null_ _null_ _null_ _null_ _null_ inet_gist_penalty _null_ _null_ _null_ ));
// DESCR("GiST support");
// DATA(insert OID = 3558 (  inet_gist_picksplit	PGNSP PGUID 12 1 0 0 0 f f f f t f i s 2 0 2281 "2281 2281" _null_ _null_ _null_ _null_ _null_ inet_gist_picksplit _null_ _null_ _null_ ));
// DESCR("GiST support");
// DATA(insert OID = 3559 (  inet_gist_same		PGNSP PGUID 12 1 0 0 0 f f f f t f i s 3 0 2281 "869 869 2281" _null_ _null_ _null_ _null_ _null_ inet_gist_same _null_ _null_ _null_ ));
// DESCR("GiST support");
// DATA(insert OID = 3560 (  networksel		   PGNSP PGUID 12 1 0 0 0 f f f f t f s s 4 0 701 "2281 26 2281 23" _null_ _null_ _null_ _null_ _null_	networksel _null_ _null_ _null_ ));
// DESCR("restriction selectivity for network operators");
// DATA(insert OID = 3561 (  networkjoinsel	   PGNSP PGUID 12 1 0 0 0 f f f f t f s s 5 0 701 "2281 26 2281 21 2281" _null_ _null_ _null_ _null_ _null_ networkjoinsel _null_ _null_ _null_ ));
// DESCR("join selectivity for network operators");
// DATA(insert OID = 3341 (  numeric_combine	 PGNSP PGUID 12 1 0 0 0 f f f f f f i s 2 0 2281 "2281 2281" _null_ _null_ _null_ _null_ _null_ numeric_combine _null_ _null_ _null_ ));
// DESCR("aggregate combine function");
// DATA(insert OID = 3337 (  numeric_avg_combine	 PGNSP PGUID 12 1 0 0 0 f f f f f f i s 2 0 2281 "2281 2281" _null_ _null_ _null_ _null_ _null_ numeric_avg_combine _null_ _null_ _null_ ));
// DESCR("aggregate combine function");
// DATA(insert OID = 2740 (  numeric_avg_serialize    PGNSP PGUID 12 1 0 0 0 f f f f t f i s 1 0 17 "2281" _null_ _null_ _null_ _null_ _null_ numeric_avg_serialize _null_ _null_ _null_ ));
// DESCR("aggregate serial function");
// DATA(insert OID = 2741 (  numeric_avg_deserialize	 PGNSP PGUID 12 1 0 0 0 f f f f t f i s 2 0 2281 "17 2281" _null_ _null_ _null_ _null_ _null_ numeric_avg_deserialize _null_ _null_ _null_ ));
// DESCR("aggregate deserial function");
// DATA(insert OID = 3335 (  numeric_serialize    PGNSP PGUID 12 1 0 0 0 f f f f t f i s 1 0 17 "2281" _null_ _null_ _null_ _null_ _null_ numeric_serialize _null_ _null_ _null_ ));
// DESCR("aggregate serial function");
// DATA(insert OID = 3336 (  numeric_deserialize	 PGNSP PGUID 12 1 0 0 0 f f f f t f i s 2 0 2281 "17 2281" _null_ _null_ _null_ _null_ _null_ numeric_deserialize _null_ _null_ _null_ ));
// DESCR("aggregate deserial function");
// DATA(insert OID = 3548 (  numeric_accum_inv    PGNSP PGUID 12 1 0 0 0 f f f f f f i s 2 0 2281 "2281 1700" _null_ _null_ _null_ _null_ _null_ numeric_accum_inv _null_ _null_ _null_ ));
// DESCR("aggregate transition function");
// DATA(insert OID = 3338 (  numeric_poly_combine	  PGNSP PGUID 12 1 0 0 0 f f f f f f i s 2 0 2281 "2281 2281" _null_ _null_ _null_ _null_ _null_ numeric_poly_combine _null_ _null_ _null_ ));
// DESCR("aggregate combine function");
// DATA(insert OID = 3339 (  numeric_poly_serialize	PGNSP PGUID 12 1 0 0 0 f f f f t f i s 1 0 17 "2281" _null_ _null_ _null_ _null_ _null_ numeric_poly_serialize _null_ _null_ _null_ ));
// DESCR("aggregate serial function");
// DATA(insert OID = 3340 (  numeric_poly_deserialize	  PGNSP PGUID 12 1 0 0 0 f f f f t f i s 2 0 2281 "17 2281" _null_ _null_ _null_ _null_ _null_ numeric_poly_deserialize _null_ _null_ _null_ ));
// DESCR("aggregate deserial function");
// DATA(insert OID = 3338 (  numeric_poly_combine	  PGNSP PGUID 12 1 0 0 0 f f f f f f i s 2 0 2281 "2281 2281" _null_ _null_ _null_ _null_ _null_ numeric_poly_combine _null_ _null_ _null_ ));
// DESCR("aggregate combine function");
// DATA(insert OID = 3339 (  numeric_poly_serialize	PGNSP PGUID 12 1 0 0 0 f f f f t f i s 1 0 17 "2281" _null_ _null_ _null_ _null_ _null_ numeric_poly_serialize _null_ _null_ _null_ ));
// DESCR("aggregate serial function");
// DATA(insert OID = 3340 (  numeric_poly_deserialize	  PGNSP PGUID 12 1 0 0 0 f f f f t f i s 2 0 2281 "17 2281" _null_ _null_ _null_ _null_ _null_ numeric_poly_deserialize _null_ _null_ _null_ ));
// DESCR("aggregate deserial function");
// DATA(insert OID = 2746 (  int8_avg_accum	   PGNSP PGUID 12 1 0 0 0 f f f f f f i s 2 0 2281 "2281 20" _null_ _null_ _null_ _null_ _null_ int8_avg_accum _null_ _null_ _null_ ));
// DESCR("aggregate transition function");
// DATA(insert OID = 3567 (  int2_accum_inv   PGNSP PGUID 12 1 0 0 0 f f f f f f i s 2 0 2281 "2281 21" _null_ _null_ _null_ _null_ _null_ int2_accum_inv _null_ _null_ _null_ ));
// DESCR("aggregate transition function");
// DATA(insert OID = 3568 (  int4_accum_inv   PGNSP PGUID 12 1 0 0 0 f f f f f f i s 2 0 2281 "2281 23" _null_ _null_ _null_ _null_ _null_ int4_accum_inv _null_ _null_ _null_ ));
// DESCR("aggregate transition function");
// DATA(insert OID = 3569 (  int8_accum_inv   PGNSP PGUID 12 1 0 0 0 f f f f f f i s 2 0 2281 "2281 20" _null_ _null_ _null_ _null_ _null_ int8_accum_inv _null_ _null_ _null_ ));
// DESCR("aggregate transition function");
// DATA(insert OID = 3387 (  int8_avg_accum_inv   PGNSP PGUID 12 1 0 0 0 f f f f f f i s 2 0 2281 "2281 20" _null_ _null_ _null_ _null_ _null_ int8_avg_accum_inv _null_ _null_ _null_ ));
// DESCR("aggregate transition function");
// DATA(insert OID = 3324 (  int4_avg_combine	  PGNSP PGUID 12 1 0 0 0 f f f f t f i s 2 0 1016 "1016 1016" _null_ _null_ _null_ _null_ _null_ int4_avg_combine _null_ _null_ _null_ ));
// DESCR("aggregate combine function");
// DATA(insert OID = 3388 (  numeric_poly_sum	   PGNSP PGUID 12 1 0 0 0 f f f f f f i s 1 0 1700 "2281" _null_ _null_ _null_ _null_ _null_ numeric_poly_sum _null_ _null_ _null_ ));
// DESCR("aggregate final function");
// DATA(insert OID = 3389 (  numeric_poly_avg	   PGNSP PGUID 12 1 0 0 0 f f f f f f i s 1 0 1700 "2281" _null_ _null_ _null_ _null_ _null_ numeric_poly_avg _null_ _null_ _null_ ));
// DESCR("aggregate final function");
// DATA(insert OID = 3390 (  numeric_poly_var_pop	PGNSP PGUID 12 1 0 0 0 f f f f f f i s 1 0 1700 "2281" _null_ _null_ _null_ _null_ _null_ numeric_poly_var_pop _null_ _null_ _null_ ));
// DESCR("aggregate final function");
// DATA(insert OID = 3391 (  numeric_poly_var_samp PGNSP PGUID 12 1 0 0 0 f f f f f f i s 1 0 1700 "2281" _null_ _null_ _null_ _null_ _null_ numeric_poly_var_samp _null_ _null_ _null_ ));
// DESCR("aggregate final function");
// DATA(insert OID = 3392 (  numeric_poly_stddev_pop PGNSP PGUID 12 1 0 0 0 f f f f f f i s 1 0 1700 "2281" _null_ _null_ _null_ _null_ _null_ numeric_poly_stddev_pop _null_ _null_ _null_ ));
// DESCR("aggregate final function");
// DATA(insert OID = 3393 (  numeric_poly_stddev_samp	PGNSP PGUID 12 1 0 0 0 f f f f f f i s 1 0 1700 "2281" _null_ _null_ _null_ _null_ _null_ numeric_poly_stddev_samp _null_ _null_ _null_ ));
// DESCR("aggregate final function");
// DATA(insert OID = 3325 (  interval_combine	 PGNSP PGUID 12 1 0 0 0 f f f f t f i s 2 0 1187 "1187 1187" _null_ _null_ _null_ _null_ _null_ interval_combine _null_ _null_ _null_ ));
// DESCR("aggregate combine function");
// DATA(insert OID = 3549 (  interval_accum_inv   PGNSP PGUID 12 1 0 0 0 f f f f t f i s 2 0 1187 "1187 1186" _null_ _null_ _null_ _null_ _null_ interval_accum_inv _null_ _null_ _null_ ));
// DESCR("aggregate transition function");
// DATA(insert OID = 3570 (  int2_avg_accum_inv   PGNSP PGUID 12 1 0 0 0 f f f f t f i s 2 0 1016 "1016 21" _null_ _null_ _null_ _null_ _null_ int2_avg_accum_inv _null_ _null_ _null_ ));
// DESCR("aggregate transition function");
// DATA(insert OID = 3571 (  int4_avg_accum_inv   PGNSP PGUID 12 1 0 0 0 f f f f t f i s 2 0 1016 "1016 23" _null_ _null_ _null_ _null_ _null_ int4_avg_accum_inv _null_ _null_ _null_ ));
// DESCR("aggregate transition function");
// DATA(insert OID = 3572 (  int2int4_sum	   PGNSP PGUID 12 1 0 0 0 f f f f t f i s 1 0 20 "1016" _null_ _null_ _null_ _null_ _null_ int2int4_sum _null_ _null_ _null_ ));
// DESCR("aggregate final function");
// DATA(insert OID = 3342 (  float8_regr_combine		PGNSP PGUID 12 1 0 0 0 f f f f t f i s 2 0 1022 "1022 1022" _null_ _null_ _null_ _null_ _null_ float8_regr_combine _null_ _null_ _null_ ));
// DESCR("aggregate combine function");
// DATA(insert OID = 3318 (  pg_stat_get_progress_info			  PGNSP PGUID 12 1 100 0 0 f f f f t t s r 1 0 2249 "25" "{25,23,26,26,20,20,20,20,20,20,20,20,20,20}" "{i,o,o,o,o,o,o,o,o,o,o,o,o,o}" "{cmdtype,pid,datid,relid,param1,param2,param3,param4,param5,param6,param7,param8,param9,param10}" _null_ _null_ pg_stat_get_progress_info _null_ _null_ _null_ ));
// DESCR("statistics: information about progress of backends running maintenance command");
// DATA(insert OID = 3317 (  pg_stat_get_wal_receiver	PGNSP PGUID 12 1 0 0 0 f f f f f f s r 0 0 2249 "" "{23,25,3220,23,3220,23,1184,1184,3220,1184,25,25}" "{o,o,o,o,o,o,o,o,o,o,o,o}" "{pid,status,receive_start_lsn,receive_start_tli,received_lsn,received_tli,last_msg_send_time,last_msg_receipt_time,latest_end_lsn,latest_end_time,slot_name,conninfo}" _null_ _null_ pg_stat_get_wal_receiver _null_ _null_ _null_ ));
// DESCR("statistics: information about WAL receiver");
// DATA(insert OID = 3331 (  bytea_sortsupport PGNSP PGUID 12 1 0 0 0 f f f f t f i s 1 0 2278 "2281" _null_ _null_ _null_ _null_ _null_ bytea_sortsupport _null_ _null_ _null_ ));
// DESCR("sort support");
// DATA(insert OID = 3294 (  current_setting	PGNSP PGUID 12 1 0 0 0 f f f f t f s s 2 0 25 "25 16" _null_ _null_ _null_ _null_ _null_ show_config_by_name_missing_ok _null_ _null_ _null_ ));
// DESCR("SHOW X as a function, optionally no error for missing variable");
// DATA(insert OID = 3329 (  pg_show_all_file_settings PGNSP PGUID 12 1 1000 0 0 f f f f t t v s 0 0 2249 "" "{25,23,23,25,25,16,25}" "{o,o,o,o,o,o,o}" "{sourcefile,sourceline,seqno,name,setting,applied,error}" _null_ _null_ show_all_file_settings _null_ _null_ _null_ ));
// DESCR("show config file settings");
// DATA(insert OID = 3581 ( pg_xact_commit_timestamp PGNSP PGUID 12 1 0 0 0 f f f f t f v s 1 0 1184 "28" _null_ _null_ _null_ _null_ _null_ pg_xact_commit_timestamp _null_ _null_ _null_ ));
// DESCR("get commit timestamp of a transaction");
// DATA(insert OID = 3583 ( pg_last_committed_xact PGNSP PGUID 12 1 0 0 0 f f f f t f v s 0 0 2249 "" "{28,1184}" "{o,o}" "{xid,timestamp}" _null_ _null_ pg_last_committed_xact _null_ _null_ _null_ ));
// DESCR("get transaction Id and commit timestamp of latest transaction commit");
// DATA(insert OID = 3839 (  pg_identify_object		PGNSP PGUID 12 1 0 0 0 f f f f t f s s 3 0 2249 "26 26 23" "{26,26,23,25,25,25,25}" "{i,i,i,o,o,o,o}" "{classid,objid,subobjid,type,schema,name,identity}" _null_ _null_ pg_identify_object _null_ _null_ _null_ ));
// DESCR("get machine-parseable identification of SQL object");
// DATA(insert OID = 3382 (  pg_identify_object_as_address PGNSP PGUID 12 1 0 0 0 f f f f t f s s 3 0 2249 "26 26 23" "{26,26,23,25,1009,1009}" "{i,i,i,o,o,o}" "{classid,objid,subobjid,type,object_names,object_args}" _null_ _null_ pg_identify_object_as_address _null_ _null_ _null_ ));
// DESCR("get identification of SQL object for pg_get_object_address()");
// DATA(insert OID = 3330 ( pg_current_xlog_flush_location PGNSP PGUID 12 1 0 0 0 f f f f t f v s 0 0 3220 "" _null_ _null_ _null_ _null_ _null_ pg_current_xlog_flush_location _null_ _null_ _null_ ));
// DESCR("current xlog flush location");
// DATA(insert OID = 3307 ( pg_stat_file		PGNSP PGUID 12 1 0 0 0 f f f f t f v s 2 0 2249 "25 16" "{25,16,20,1184,1184,1184,1184,16}" "{i,i,o,o,o,o,o,o}" "{filename,missing_ok,size,access,modification,change,creation,isdir}" _null_ _null_ pg_stat_file _null_ _null_ _null_ ));
// DESCR("get information about file");
// DATA(insert OID = 3293 ( pg_read_file		PGNSP PGUID 12 1 0 0 0 f f f f t f v s 4 0 25 "25 20 20 16" _null_ _null_ _null_ _null_ _null_ pg_read_file _null_ _null_ _null_ ));
// DESCR("read text from a file");
// DATA(insert OID = 3295 ( pg_read_binary_file	PGNSP PGUID 12 1 0 0 0 f f f f t f v s 4 0 17 "25 20 20 16" _null_ _null_ _null_ _null_ _null_ pg_read_binary_file _null_ _null_ _null_ ));
// DESCR("read bytea from a file");
// DATA(insert OID = 3297 ( pg_ls_dir			PGNSP PGUID 12 1 1000 0 0 f f f f t t v s 3 0 25 "25 16 16" _null_ _null_ _null_ _null_ _null_ pg_ls_dir _null_ _null_ _null_ ));
// DESCR("list all files in a directory");
// DATA(insert OID = 3564 (  max				PGNSP PGUID 12 1 0 0 0 t f f f f f i s 1 0 869 "869" _null_ _null_ _null_ _null_ _null_ aggregate_dummy _null_ _null_ _null_ ));
// DESCR("maximum value of all inet input values");
// DATA(insert OID = 3565 (  min				PGNSP PGUID 12 1 0 0 0 t f f f f f i s 1 0 869 "869" _null_ _null_ _null_ _null_ _null_ aggregate_dummy _null_ _null_ _null_ ));
// DESCR("minimum value of all inet input values");
// DATA(insert OID = 3332 ( bttext_pattern_sortsupport PGNSP PGUID 12 1 0 0 0 f f f f t f i s 1 0 2278 "2281" _null_ _null_ _null_ _null_ _null_ bttext_pattern_sortsupport _null_ _null_ _null_ ));
// DESCR("sort support");
// DATA(insert OID = 3333 ( btbpchar_pattern_sortsupport PGNSP PGUID 12 1 0 0 0 f f f f t f i s 1 0 2278 "2281" _null_ _null_ _null_ _null_ _null_ btbpchar_pattern_sortsupport _null_ _null_ _null_ ));
// DESCR("sort support");
// DATA(insert OID = 3492 (  to_regoper		PGNSP PGUID 12 1 0 0 0 f f f f t f s s 1 0 2203 "25" _null_ _null_ _null_ _null_ _null_ to_regoper _null_ _null_ _null_ ));
// DESCR("convert operator name to regoper");
// DATA(insert OID = 3495 (  to_regclass		PGNSP PGUID 12 1 0 0 0 f f f f t f s s 1 0 2205 "25" _null_ _null_ _null_ _null_ _null_ to_regclass _null_ _null_ _null_ ));
// DESCR("convert classname to regclass");
// DATA(insert OID = 3493 (  to_regtype		PGNSP PGUID 12 1 0 0 0 f f f f t f s s 1 0 2206 "25" _null_ _null_ _null_ _null_ _null_ to_regtype _null_ _null_ _null_ ));
// DESCR("convert type name to regtype");
// DATA(insert OID = 3493 (  to_regtype		PGNSP PGUID 12 1 0 0 0 f f f f t f s s 1 0 2206 "25" _null_ _null_ _null_ _null_ _null_ to_regtype _null_ _null_ _null_ ));
// DESCR("convert type name to regtype");
// DATA(insert OID = 1079 (  regclass			PGNSP PGUID 12 1 0 0 0 f f f f t f s s 1 0 2205 "25" _null_ _null_ _null_ _null_ _null_ text_regclass _null_ _null_ _null_ ));
// DESCR("convert text to regclass");
// DATA(insert OID = 4098 (  regrolein			PGNSP PGUID 12 1 0 0 0 f f f f t f s s 1 0 4096 "2275" _null_ _null_ _null_ _null_ _null_ regrolein _null_ _null_ _null_ ));
// DESCR("I/O");
// DATA(insert OID = 4092 (  regroleout		PGNSP PGUID 12 1 0 0 0 f f f f t f s s 1 0 2275 "4096" _null_ _null_ _null_ _null_ _null_ regroleout _null_ _null_ _null_ ));
// DESCR("I/O");
// DATA(insert OID = 4093 (  to_regrole		PGNSP PGUID 12 1 0 0 0 f f f f t f s s 1 0 4096 "25" _null_ _null_ _null_ _null_ _null_ to_regrole _null_ _null_ _null_ ));
// DESCR("convert role name to regrole");
// DATA(insert OID = 4084 (  regnamespacein	PGNSP PGUID 12 1 0 0 0 f f f f t f s s 1 0 4089 "2275" _null_ _null_ _null_ _null_ _null_ regnamespacein _null_ _null_ _null_ ));
// DESCR("I/O");
// DATA(insert OID = 4085 (  regnamespaceout	PGNSP PGUID 12 1 0 0 0 f f f f t f s s 1 0 2275 "4089" _null_ _null_ _null_ _null_ _null_ regnamespaceout _null_ _null_ _null_ ));
// DESCR("I/O");
// DATA(insert OID = 4086 (  to_regnamespace	PGNSP PGUID 12 1 0 0 0 f f f f t f s s 1 0 4089 "25" _null_ _null_ _null_ _null_ _null_ to_regnamespace _null_ _null_ _null_ ));
// DESCR("convert namespace name to regnamespace");
// DATA(insert OID = 3334 ( pg_size_bytes			PGNSP PGUID 12 1 0 0 0 f f f f t f i s 1 0 20 "25" _null_ _null_ _null_ _null_ _null_ pg_size_bytes _null_ _null_ _null_ ));
// DESCR("convert a size in human-readable format with size units into bytes");
// DATA(insert OID = 3454 ( pg_filenode_relation PGNSP PGUID 12 1 0 0 0 f f f f t f s s 2 0 2205 "26 26" _null_ _null_ _null_ _null_ _null_ pg_filenode_relation _null_ _null_ _null_ ));
// DESCR("relation OID for filenode and tablespace");
// DATA(insert OID = 3594 (  event_trigger_in	PGNSP PGUID 12 1 0 0 0 f f f f f f i s 1 0 3838 "2275" _null_ _null_ _null_ _null_ _null_ event_trigger_in _null_ _null_ _null_ ));
// DESCR("I/O");
// DATA(insert OID = 3595 (  event_trigger_out PGNSP PGUID 12 1 0 0 0 f f f f t f i s 1 0 2275 "3838" _null_ _null_ _null_ _null_ _null_ event_trigger_out _null_ _null_ _null_ ));
// DESCR("I/O");
// DATA(insert OID = 3311 (  tsm_handler_in	PGNSP PGUID 12 1 0 0 0 f f f f f f i s 1 0 3310 "2275" _null_ _null_ _null_ _null_ _null_ tsm_handler_in _null_ _null_ _null_ ));
// DESCR("I/O");
// DATA(insert OID = 3312 (  tsm_handler_out	PGNSP PGUID 12 1 0 0 0 f f f f t f i s 1 0 2275 "3310" _null_ _null_ _null_ _null_ _null_ tsm_handler_out _null_ _null_ _null_ ));
// DESCR("I/O");
// DATA(insert OID = 3313 (  bernoulli			PGNSP PGUID 12 1 0 0 0 f f f f t f v s 1 0 3310 "2281" _null_ _null_ _null_ _null_ _null_ tsm_bernoulli_handler _null_ _null_ _null_ ));
// DESCR("BERNOULLI tablesample method handler");
// DATA(insert OID = 3314 (  system			PGNSP PGUID 12 1 0 0 0 f f f f t f v s 1 0 3310 "2281" _null_ _null_ _null_ _null_ _null_ tsm_system_handler _null_ _null_ _null_ ));
// DESCR("SYSTEM tablesample method handler");
// DATA(insert OID = 4094 (  regrolerecv		   PGNSP PGUID 12 1 0 0 0 f f f f t f i s 1 0 4096 "2281" _null_ _null_ _null_ _null_ _null_	regrolerecv _null_ _null_ _null_ ));
// DESCR("I/O");
// DATA(insert OID = 4095 (  regrolesend		   PGNSP PGUID 12 1 0 0 0 f f f f t f i s 1 0 17 "4096" _null_ _null_ _null_ _null_ _null_	regrolesend _null_ _null_ _null_ ));
// DESCR("I/O");
// DATA(insert OID = 4087 (  regnamespacerecv	   PGNSP PGUID 12 1 0 0 0 f f f f t f i s 1 0 4089 "2281" _null_ _null_ _null_ _null_ _null_ regnamespacerecv _null_ _null_ _null_ ));
// DESCR("I/O");
// DATA(insert OID = 4088 (  regnamespacesend	   PGNSP PGUID 12 1 0 0 0 f f f f t f i s 1 0 17 "4089" _null_ _null_ _null_ _null_ _null_	regnamespacesend _null_ _null_ _null_ ));
// DESCR("I/O");
// DATA(insert OID = 3296 (  pg_notification_queue_usage	PGNSP PGUID 12 1 0 0 0 f f f f t f v s 0 0 701 "" _null_ _null_ _null_ _null_ _null_ pg_notification_queue_usage _null_ _null_ _null_ ));
// DESCR("get the fraction of the asynchronous notification queue currently in use");
// DATA(insert OID = 3259 (  generate_series PGNSP PGUID 12 1 1000 0 0 f f f f t t i s 3 0 1700 "1700 1700 1700" _null_ _null_ _null_ _null_ _null_ generate_series_step_numeric _null_ _null_ _null_ ));
// DESCR("non-persistent series generator");
// DATA(insert OID = 3260 (  generate_series PGNSP PGUID 12 1 1000 0 0 f f f f t t i s 2 0 1700 "1700 1700" _null_ _null_ _null_ _null_ _null_ generate_series_numeric _null_ _null_ _null_ ));
// DESCR("non-persistent series generator");
// DATA(insert OID = 3496 ( bool_accum					   PGNSP PGUID 12 1 0 0 0 f f f f f f i s 2 0 2281 "2281 16" _null_ _null_ _null_ _null_ _null_ bool_accum _null_ _null_ _null_ ));
// DESCR("aggregate transition function");
// DATA(insert OID = 3497 ( bool_accum_inv				   PGNSP PGUID 12 1 0 0 0 f f f f f f i s 2 0 2281 "2281 16" _null_ _null_ _null_ _null_ _null_ bool_accum_inv _null_ _null_ _null_ ));
// DESCR("aggregate transition function");
// DATA(insert OID = 3498 ( bool_alltrue				   PGNSP PGUID 12 1 0 0 0 f f f f t f i s 1 0 16 "2281" _null_ _null_ _null_ _null_ _null_ bool_alltrue _null_ _null_ _null_ ));
// DESCR("aggregate final function");
// DATA(insert OID = 3499 ( bool_anytrue				   PGNSP PGUID 12 1 0 0 0 f f f f t f i s 1 0 16 "2281" _null_ _null_ _null_ _null_ _null_ bool_anytrue _null_ _null_ _null_ ));
// DESCR("aggregate final function");
// DATA(insert OID = 3281 (  gist_box_fetch	PGNSP PGUID 12 1 0 0 0 f f f f t f i s 1 0 2281 "2281" _null_ _null_ _null_ _null_ _null_ gist_box_fetch _null_ _null_ _null_ ));
// DESCR("GiST support");
// DATA(insert OID = 3282 (  gist_point_fetch	PGNSP PGUID 12 1 0 0 0 f f f f t f i s 1 0 2281 "2281" _null_ _null_ _null_ _null_ _null_ gist_point_fetch _null_ _null_ _null_ ));
// DESCR("GiST support");
// DATA(insert OID = 3280 (  gist_circle_distance	PGNSP PGUID 12 1 0 0 0 f f f f t f i s 5 0 701 "2281 718 21 26 2281" _null_ _null_ _null_ _null_ _null_ gist_circle_distance _null_ _null_ _null_ ));
// DESCR("GiST support");
// DATA(insert OID = 3288 (  gist_poly_distance	PGNSP PGUID 12 1 0 0 0 f f f f t f i s 5 0 701 "2281 604 21 26 2281" _null_ _null_ _null_ _null_ _null_ gist_poly_distance _null_ _null_ _null_ ));
// DESCR("GiST support");
// DATA(insert OID = 3383 ( brin_minmax_opcinfo	PGNSP PGUID 12 1 0 0 0 f f f f t f i s 1 0 2281 "2281" _null_ _null_ _null_ _null_ _null_ brin_minmax_opcinfo _null_ _null_ _null_ ));
// DESCR("BRIN minmax support");
// DATA(insert OID = 3384 ( brin_minmax_add_value	PGNSP PGUID 12 1 0 0 0 f f f f t f i s 4 0 16 "2281 2281 2281 2281" _null_ _null_ _null_ _null_ _null_ brin_minmax_add_value _null_ _null_ _null_ ));
// DESCR("BRIN minmax support");
// DATA(insert OID = 3385 ( brin_minmax_consistent PGNSP PGUID 12 1 0 0 0 f f f f t f i s 3 0 16 "2281 2281 2281" _null_ _null_ _null_ _null_ _null_ brin_minmax_consistent _null_ _null_ _null_ ));
// DESCR("BRIN minmax support");
// DATA(insert OID = 3386 ( brin_minmax_union		PGNSP PGUID 12 1 0 0 0 f f f f t f i s 3 0 16 "2281 2281 2281" _null_ _null_ _null_ _null_ _null_ brin_minmax_union _null_ _null_ _null_ ));
// DESCR("BRIN minmax support");
// DATA(insert OID = 3173 (  json_agg_transfn	 PGNSP PGUID 12 1 0 0 0 f f f f f f s s 2 0 2281 "2281 2283" _null_ _null_ _null_ _null_ _null_ json_agg_transfn _null_ _null_ _null_ ));
// DESCR("json aggregate transition function");
// DATA(insert OID = 3174 (  json_agg_finalfn	 PGNSP PGUID 12 1 0 0 0 f f f f f f i s 1 0 114 "2281" _null_ _null_ _null_ _null_ _null_ json_agg_finalfn _null_ _null_ _null_ ));
// DESCR("json aggregate final function");
// DATA(insert OID = 3261 (  json_strip_nulls	   PGNSP PGUID 12 1 0 0 0 f f f f t f i s 1 0 114 "114" _null_ _null_ _null_ _null_ _null_ json_strip_nulls _null_ _null_ _null_ ));
// DESCR("remove object fields with null values from json");
// DATA(insert OID = 3948 (  json_object_field_text	PGNSP PGUID 12 1 0 0 0 f f f f t f i s 2 0 25  "114 25" _null_ _null_ "{from_json, field_name}" _null_ _null_ json_object_field_text _null_ _null_ _null_ ));
// DATA(insert OID = 3949 (  json_array_element		PGNSP PGUID 12 1 0 0 0 f f f f t f i s 2 0 114 "114 23" _null_ _null_ "{from_json, element_index}" _null_ _null_ json_array_element _null_ _null_ _null_ ));
// DATA(insert OID = 3956 (  json_array_length			PGNSP PGUID 12 1 0 0 0 f f f f t f i s 1 0 23 "114" _null_ _null_ _null_ _null_ _null_ json_array_length _null_ _null_ _null_ ));
// DESCR("length of json array");
// DATA(insert OID = 3957 (  json_object_keys			PGNSP PGUID 12 1 100 0 0 f f f f t t i s 1 0 25 "114" _null_ _null_ _null_ _null_ _null_ json_object_keys _null_ _null_ _null_ ));
// DESCR("get json object keys");
// DATA(insert OID = 3958 (  json_each				   PGNSP PGUID 12 1 100 0 0 f f f f t t i s 1 0 2249 "114" "{114,25,114}" "{i,o,o}" "{from_json,key,value}" _null_ _null_ json_each _null_ _null_ _null_ ));
// DESCR("key value pairs of a json object");
// DATA(insert OID = 3959 (  json_each_text		   PGNSP PGUID 12 1 100 0 0 f f f f t t i s 1 0 2249 "114" "{114,25,25}" "{i,o,o}" "{from_json,key,value}" _null_ _null_ json_each_text _null_ _null_ _null_ ));
// DESCR("key value pairs of a json object");
// DATA(insert OID = 3960 (  json_populate_record	   PGNSP PGUID 12 1 0 0 0 f f f f f f s s 3 0 2283 "2283 114 16" _null_ _null_ _null_ _null_ _null_ json_populate_record _null_ _null_ _null_ ));
// DESCR("get record fields from a json object");
// DATA(insert OID = 3968 (  json_typeof			   PGNSP PGUID 12 1 0 0 0 f f f f t f i s 1 0 25 "114" _null_ _null_ _null_ _null_ _null_ json_typeof _null_ _null_ _null_ ));
// DESCR("get the type of a json value");
// DATA(insert OID = 3300 (  uuid_sortsupport PGNSP PGUID 12 1 0 0 0 f f f f t f i s 1 0 2278 "2281" _null_ _null_ _null_ _null_ _null_ uuid_sortsupport _null_ _null_ _null_ ));
// DESCR("sort support");
// DATA(insert OID = 3229 (  pg_lsn_in		PGNSP PGUID 12 1 0 0 0 f f f f t f i s 1 0 3220 "2275" _null_ _null_ _null_ _null_ _null_ pg_lsn_in _null_ _null_ _null_ ));
// DESCR("I/O");
// DATA(insert OID = 3230 (  pg_lsn_out		PGNSP PGUID 12 1 0 0 0 f f f f t f i s 1 0 2275 "3220" _null_ _null_ _null_ _null_ _null_ pg_lsn_out _null_ _null_ _null_ ));
// DESCR("I/O");
// DATA(insert OID = 3231 (  pg_lsn_lt		PGNSP PGUID 12 1 0 0 0 f f f f t f i s 2 0 16 "3220 3220" _null_ _null_ _null_ _null_ _null_ pg_lsn_lt _null_ _null_ _null_ ));
// DATA(insert OID = 3232 (  pg_lsn_le		PGNSP PGUID 12 1 0 0 0 f f f f t f i s 2 0 16 "3220 3220" _null_ _null_ _null_ _null_ _null_ pg_lsn_le _null_ _null_ _null_ ));
// DATA(insert OID = 3233 (  pg_lsn_eq		PGNSP PGUID 12 1 0 0 0 f f f f t f i s 2 0 16 "3220 3220" _null_ _null_ _null_ _null_ _null_ pg_lsn_eq _null_ _null_ _null_ ));
// DATA(insert OID = 3234 (  pg_lsn_ge		PGNSP PGUID 12 1 0 0 0 f f f f t f i s 2 0 16 "3220 3220" _null_ _null_ _null_ _null_ _null_ pg_lsn_ge _null_ _null_ _null_ ));
// DATA(insert OID = 3235 (  pg_lsn_gt		PGNSP PGUID 12 1 0 0 0 f f f f t f i s 2 0 16 "3220 3220" _null_ _null_ _null_ _null_ _null_ pg_lsn_gt _null_ _null_ _null_ ));
// DATA(insert OID = 3236 (  pg_lsn_ne		PGNSP PGUID 12 1 0 0 0 f f f f t f i s 2 0 16 "3220 3220" _null_ _null_ _null_ _null_ _null_ pg_lsn_ne _null_ _null_ _null_ ));
// DATA(insert OID = 3237 (  pg_lsn_mi		PGNSP PGUID 12 1 0 0 0 f f f f t f i s 2 0 1700 "3220 3220" _null_ _null_ _null_ _null_ _null_ pg_lsn_mi _null_ _null_ _null_ ));
// DATA(insert OID = 3238 (  pg_lsn_recv	PGNSP PGUID 12 1 0 0 0 f f f f t f i s 1 0 3220 "2281" _null_ _null_ _null_ _null_ _null_ pg_lsn_recv _null_ _null_ _null_ ));
// DESCR("I/O");
// DATA(insert OID = 3239 (  pg_lsn_send	PGNSP PGUID 12 1 0 0 0 f f f f t f i s 1 0 17 "3220" _null_ _null_ _null_ _null_ _null_ pg_lsn_send _null_ _null_ _null_ ));
// DESCR("I/O");
// DATA(insert OID = 3251 (  pg_lsn_cmp	PGNSP PGUID 12 1 0 0 0 f f f f t f i s 2 0 23 "3220 3220" _null_ _null_ _null_ _null_ _null_ pg_lsn_cmp _null_ _null_ _null_ ));
// DESCR("less-equal-greater");
// DATA(insert OID = 3252 (  pg_lsn_hash	PGNSP PGUID 12 1 0 0 0 f f f f t f i s 1 0 23 "3220" _null_ _null_ _null_ _null_ _null_ pg_lsn_hash _null_ _null_ _null_ ));
// DESCR("hash");
// DATA(insert OID = 3320 (  setweight				PGNSP PGUID 12 1 0	0 0 f f f f t f i s 3 0 3614 "3614 18 1009" _null_ _null_ _null_ _null_ _null_ tsvector_setweight_by_filter _null_ _null_ _null_ ));
// DESCR("set given weight for given lexemes");
// DATA(insert OID = 3321 (  ts_delete				PGNSP PGUID 12 1 0	0 0 f f f f t f i s 2 0 3614 "3614 25" _null_ _null_ _null_ _null_ _null_ tsvector_delete_str _null_ _null_ _null_ ));
// DESCR("delete lexeme");
// DATA(insert OID = 3323 (  ts_delete				PGNSP PGUID 12 1 0	0 0 f f f f t f i s 2 0 3614 "3614 1009" _null_ _null_ _null_ _null_ _null_ tsvector_delete_arr _null_ _null_ _null_ ));
// DESCR("delete given lexemes");
// DATA(insert OID = 3322 (  unnest				PGNSP PGUID 12 1 10 0 0 f f f f t t i s 1 0 2249 "3614" "{3614,25,1005,1009}" "{i,o,o,o}" "{tsvector,lexeme,positions,weights}"  _null_ _null_ tsvector_unnest _null_ _null_ _null_ ));
// DESCR("expand tsvector to set of rows");
// DATA(insert OID = 3326 (  tsvector_to_array		PGNSP PGUID 12 1 0	0 0 f f f f t f i s 1 0 1009 "3614" _null_ _null_ _null_ _null_ _null_ tsvector_to_array _null_ _null_ _null_ ));
// DESCR("convert tsvector to array of lexemes");
// DATA(insert OID = 3327 (  array_to_tsvector		PGNSP PGUID 12 1 0	0 0 f f f f t f i s 1 0 3614 "1009" _null_ _null_ _null_ _null_ _null_ array_to_tsvector _null_ _null_ _null_ ));
// DESCR("build tsvector from array of lexemes");
// DATA(insert OID = 3319 (  ts_filter				PGNSP PGUID 12 1 0	0 0 f f f f t f i s 2 0 3614 "3614 1002" _null_ _null_ _null_ _null_ _null_ tsvector_filter _null_ _null_ _null_ ));
// DESCR("delete lexemes that do not have one of the given weights");
// DATA(insert OID = 3790 (  gtsvector_consistent	PGNSP PGUID 12 1 0 0 0 f f f f t f i s 5 0 16 "2281 3642 23 26 2281" _null_ _null_ _null_ _null_ _null_ gtsvector_consistent_oldsig _null_ _null_ _null_ ));
// DESCR("GiST tsvector support (obsolete)");
// DATA(insert OID = 3791 (  gin_extract_tsquery	PGNSP PGUID 12 1 0 0 0 f f f f t f i s 7 0 2281 "3615 2281 21 2281 2281 2281 2281" _null_ _null_ _null_ _null_ _null_ gin_extract_tsquery_oldsig _null_ _null_ _null_ ));
// DESCR("GIN tsvector support (obsolete)");
// DATA(insert OID =  3806 (  jsonb_in			PGNSP PGUID 12 1 0 0 0 f f f f t f i s 1 0 3802 "2275" _null_ _null_ _null_ _null_ _null_ jsonb_in _null_ _null_ _null_ ));
// DESCR("I/O");
// DATA(insert OID =  3805 (  jsonb_recv		PGNSP PGUID 12 1 0 0 0 f f f f t f i s 1 0 3802 "2281" _null_ _null_ _null_ _null_ _null_ jsonb_recv _null_ _null_ _null_ ));
// DESCR("I/O");
// DATA(insert OID =  3804 (  jsonb_out		PGNSP PGUID 12 1 0 0 0 f f f f t f i s 1 0 2275 "3802" _null_ _null_ _null_ _null_ _null_ jsonb_out _null_ _null_ _null_ ));
// DESCR("I/O");
// DATA(insert OID =  3803 (  jsonb_send		PGNSP PGUID 12 1 0 0 0 f f f f t f i s 1 0 17 "3802" _null_ _null_ _null_ _null_ _null_ jsonb_send _null_ _null_ _null_ ));
// DESCR("I/O");
// DATA(insert OID = 3263 (  jsonb_object	 PGNSP PGUID 12 1 0 0 0 f f f f t f i s 1 0 3802 "1009" _null_ _null_ _null_ _null_ _null_ jsonb_object _null_ _null_ _null_ ));
// DESCR("map text array of key value pairs to jsonb object");
// DATA(insert OID = 3264 (  jsonb_object	 PGNSP PGUID 12 1 0 0 0 f f f f t f i s 2 0 3802 "1009 1009" _null_ _null_ _null_ _null_ _null_ jsonb_object_two_arg _null_ _null_ _null_ ));
// DESCR("map text array of key value pairs to jsonb object");
// DATA(insert OID = 3265 (  jsonb_agg_transfn  PGNSP PGUID 12 1 0 0 0 f f f f f f s s 2 0 2281 "2281 2283" _null_ _null_ _null_ _null_ _null_ jsonb_agg_transfn _null_ _null_ _null_ ));
// DESCR("jsonb aggregate transition function");
// DATA(insert OID = 3266 (  jsonb_agg_finalfn  PGNSP PGUID 12 1 0 0 0 f f f f f f s s 1 0 3802 "2281" _null_ _null_ _null_ _null_ _null_ jsonb_agg_finalfn _null_ _null_ _null_ ));
// DESCR("jsonb aggregate final function");
// DATA(insert OID = 3267 (  jsonb_agg		   PGNSP PGUID 12 1 0 0 0 t f f f f f s s 1 0 3802 "2283" _null_ _null_ _null_ _null_ _null_ aggregate_dummy _null_ _null_ _null_ ));
// DESCR("aggregate input into jsonb");
// DATA(insert OID = 3268 (  jsonb_object_agg_transfn	 PGNSP PGUID 12 1 0 0 0 f f f f f f s s 3 0 2281 "2281 2276 2276" _null_ _null_ _null_ _null_ _null_ jsonb_object_agg_transfn _null_ _null_ _null_ ));
// DESCR("jsonb object aggregate transition function");
// DATA(insert OID = 3269 (  jsonb_object_agg_finalfn	 PGNSP PGUID 12 1 0 0 0 f f f f f f s s 1 0 3802 "2281" _null_ _null_ _null_ _null_ _null_ jsonb_object_agg_finalfn _null_ _null_ _null_ ));
// DESCR("jsonb object aggregate final function");
// DATA(insert OID = 3270 (  jsonb_object_agg		   PGNSP PGUID 12 1 0 0 0 t f f f f f i s 2 0 3802 "2276 2276" _null_ _null_ _null_ _null_ _null_ aggregate_dummy _null_ _null_ _null_ ));
// DESCR("aggregate inputs into jsonb object");
// DATA(insert OID = 3271 (  jsonb_build_array    PGNSP PGUID 12 1 0 2276 0 f f f f f f s s 1 0 3802 "2276" "{2276}" "{v}" _null_ _null_ _null_ jsonb_build_array _null_ _null_ _null_ ));
// DESCR("build a jsonb array from any inputs");
// DATA(insert OID = 3272 (  jsonb_build_array    PGNSP PGUID 12 1 0 0 0 f f f f f f s s 0 0 3802	"" _null_ _null_ _null_ _null_ _null_ jsonb_build_array_noargs _null_ _null_ _null_ ));
// DESCR("build an empty jsonb array");
// DATA(insert OID = 3273 (  jsonb_build_object	PGNSP PGUID 12 1 0 2276 0 f f f f f f s s 1 0 3802 "2276" "{2276}" "{v}" _null_ _null_ _null_ jsonb_build_object _null_ _null_ _null_ ));
// DESCR("build a jsonb object from pairwise key/value inputs");
// DATA(insert OID = 3274 (  jsonb_build_object	PGNSP PGUID 12 1 0 0 0 f f f f f f s s 0 0 3802  "" _null_ _null_ _null_ _null_ _null_ jsonb_build_object_noargs _null_ _null_ _null_ ));
// DESCR("build an empty jsonb object");
// DATA(insert OID = 3262 (  jsonb_strip_nulls    PGNSP PGUID 12 1 0 0 0 f f f f t f i s 1 0 3802 "3802" _null_ _null_ _null_ _null_ _null_ jsonb_strip_nulls _null_ _null_ _null_ ));
// DESCR("remove object fields with null values from jsonb");
// DATA(insert OID = 3214 (  jsonb_object_field_text	PGNSP PGUID 12 1 0 0 0 f f f f t f i s 2 0 25  "3802 25" _null_ _null_ "{from_json, field_name}" _null_ _null_ jsonb_object_field_text _null_ _null_ _null_ ));
// DATA(insert OID = 3215 (  jsonb_array_element		PGNSP PGUID 12 1 0 0 0 f f f f t f i s 2 0 3802 "3802 23" _null_ _null_ "{from_json, element_index}" _null_ _null_ jsonb_array_element _null_ _null_ _null_ ));
// DATA(insert OID = 3217 (  jsonb_extract_path			PGNSP PGUID 12 1 0 25 0 f f f f t f i s 2 0 3802 "3802 1009" "{3802,1009}" "{i,v}" "{from_json,path_elems}" _null_ _null_ jsonb_extract_path _null_ _null_ _null_ ));
// DESCR("get value from jsonb with path elements");
// DATA(insert OID = 3219 (  jsonb_array_elements		PGNSP PGUID 12 1 100 0 0 f f f f t t i s 1 0 3802 "3802" "{3802,3802}" "{i,o}" "{from_json,value}" _null_ _null_ jsonb_array_elements _null_ _null_ _null_ ));
// DESCR("elements of a jsonb array");
// DATA(insert OID = 3490 (  jsonb_to_record			PGNSP PGUID 12 1 0 0 0 f f f f t f s s 1 0 2249 "3802" _null_ _null_ _null_ _null_ _null_ jsonb_to_record _null_ _null_ _null_ ));
// DESCR("get record fields from a jsonb object");
// DATA(insert OID = 3491 (  jsonb_to_recordset		PGNSP PGUID 12 1 100 0 0 f f f f f t s s 1 0 2249 "3802" _null_ _null_ _null_ _null_ _null_ jsonb_to_recordset _null_ _null_ _null_ ));
// DESCR("get set of records with fields from a jsonb array of objects");
// DATA(insert OID = 4039 (  jsonb_lt		   PGNSP PGUID 12 1 0 0 0 f f f f t f i s 2 0 16 "3802 3802" _null_ _null_ _null_ _null_ _null_ jsonb_lt _null_ _null_ _null_ ));
// DATA(insert OID = 4050 (  jsonb_contained	PGNSP PGUID 12 1 0 0 0 f f f f t f i s 2 0 16 "3802 3802" _null_ _null_ _null_ _null_ _null_ jsonb_contained _null_ _null_ _null_ ));
// DATA(insert OID = 3482 (  gin_extract_jsonb  PGNSP PGUID 12 1 0 0 0 f f f f t f i s 3 0 2281 "3802 2281 2281" _null_ _null_ _null_ _null_ _null_ gin_extract_jsonb _null_ _null_ _null_ ));
// DESCR("GIN support");
// DATA(insert OID = 3486 (  gin_extract_jsonb_query_path	PGNSP PGUID 12 1 0 0 0 f f f f t f i s 7 0 2281 "3802 2281 21 2281 2281 2281 2281" _null_ _null_ _null_ _null_ _null_ gin_extract_jsonb_query_path _null_ _null_ _null_ ));
// DESCR("GIN support");
// DATA(insert OID = 3487 (  gin_consistent_jsonb_path  PGNSP PGUID 12 1 0 0 0 f f f f t f i s 8 0 16 "2281 21 3802 23 2281 2281 2281 2281" _null_ _null_ _null_ _null_ _null_ gin_consistent_jsonb_path _null_ _null_ _null_ ));
// DESCR("GIN support");
// DATA(insert OID = 3301 (  jsonb_concat	   PGNSP PGUID 12 1 0 0 0 f f f f t f i s 2 0 3802 "3802 3802" _null_ _null_ _null_ _null_ _null_ jsonb_concat _null_ _null_ _null_ ));
// DATA(insert OID = 3302 (  jsonb_delete	   PGNSP PGUID 12 1 0 0 0 f f f f t f i s 2 0 3802 "3802 25" _null_ _null_ _null_ _null_ _null_ jsonb_delete _null_ _null_ _null_ ));
// DATA(insert OID = 3303 (  jsonb_delete	   PGNSP PGUID 12 1 0 0 0 f f f f t f i s 2 0 3802 "3802 23" _null_ _null_ _null_ _null_ _null_ jsonb_delete_idx _null_ _null_ _null_ ));
// DATA(insert OID = 3304 (  jsonb_delete_path    PGNSP PGUID 12 1 0 0 0 f f f f t f i s 2 0 3802 "3802 1009" _null_ _null_ _null_ _null_ _null_ jsonb_delete_path _null_ _null_ _null_ ));
// DATA(insert OID = 3305 (  jsonb_set    PGNSP PGUID 12 1 0 0 0 f f f f t f i s 4 0 3802 "3802 1009 3802 16" _null_ _null_ _null_ _null_ _null_ jsonb_set _null_ _null_ _null_ ));
// DESCR("Set part of a jsonb");
// DATA(insert OID = 3306 (  jsonb_pretty	   PGNSP PGUID 12 1 0 0 0 f f f f t f i s 1 0 25 "3802" _null_ _null_ _null_ _null_ _null_ jsonb_pretty _null_ _null_ _null_ ));
// DESCR("Indented text from jsonb");
// DATA(insert OID = 3579 (  jsonb_insert	  PGNSP PGUID 12 1 0 0 0 f f f f t f i s 4 0 3802 "3802 1009 3802 16" _null_ _null_ _null_ _null_ _null_ jsonb_insert _null_ _null_ _null_ ));
// DESCR("Insert value into a jsonb");
// DATA(insert OID = 4057 (  range_merge		PGNSP PGUID 12 1 0 0 0 f f f f t f i s 2 0 3831 "3831 3831" _null_ _null_ _null_ _null_ _null_ range_merge _null_ _null_ _null_ ));
// DESCR("the smallest range which includes both of the given ranges");
// DATA(insert OID = 3996 (  range_gist_fetch		PGNSP PGUID 12 1 0 0 0 f f f f t f i s 1 0 2281 "2281" _null_ _null_ _null_ _null_ _null_ range_gist_fetch _null_ _null_ _null_ ));
// DESCR("GiST support");
// DATA(insert OID = 3846 ( make_date	PGNSP PGUID 12 1 0 0 0 f f f f t f i s 3 0 1082 "23 23 23" _null_ _null_ "{year,month,day}" _null_ _null_ make_date _null_ _null_ _null_ ));
// DESCR("construct date");
// DATA(insert OID = 3847 ( make_time	PGNSP PGUID 12 1 0 0 0 f f f f t f i s 3 0 1083 "23 23 701" _null_ _null_ "{hour,min,sec}" _null_ _null_ make_time _null_ _null_ _null_ ));
// DESCR("construct time");
// DATA(insert OID = 3461 ( make_timestamp PGNSP PGUID 12 1 0 0 0 f f f f t f i s 6 0 1114 "23 23 23 23 23 701" _null_ _null_ "{year,month,mday,hour,min,sec}" _null_ _null_ make_timestamp _null_ _null_ _null_ ));
// DESCR("construct timestamp");
// DATA(insert OID = 3462 ( make_timestamptz	PGNSP PGUID 12 1 0 0 0 f f f f t f s s 6 0 1184 "23 23 23 23 23 701" _null_ _null_ "{year,month,mday,hour,min,sec}" _null_ _null_ make_timestamptz _null_ _null_ _null_ ));
// DESCR("construct timestamp with time zone");
// DATA(insert OID = 3463 ( make_timestamptz	PGNSP PGUID 12 1 0 0 0 f f f f t f s s 7 0 1184 "23 23 23 23 23 701 25" _null_ _null_ "{year,month,mday,hour,min,sec,timezone}" _null_ _null_ make_timestamptz_at_timezone _null_ _null_ _null_ ));
// DESCR("construct timestamp with time zone");
// DATA(insert OID = 3473 (  spg_range_quad_leaf_consistent	PGNSP PGUID 12 1 0 0 0 f f f f t f i s 2 0 16 "2281 2281" _null_ _null_ _null_ _null_  _null_ spg_range_quad_leaf_consistent _null_ _null_ _null_ ));
// DESCR("SP-GiST support for quad tree over range");
// DATA(insert OID = 3577 (  pg_logical_emit_message PGNSP PGUID 12 1 0 0 0 f f f f t f v u 3 0 3220 "16 25 25" _null_ _null_ _null_ _null_ _null_ pg_logical_emit_message_text _null_ _null_ _null_ ));
// DESCR("emit a textual logical decoding message");
// DATA(insert OID = 3578 (  pg_logical_emit_message PGNSP PGUID 12 1 0 0 0 f f f f t f v u 3 0 3220 "16 25 17" _null_ _null_ _null_ _null_ _null_ pg_logical_emit_message_bytea _null_ _null_ _null_ ));
// DESCR("emit a binary logical decoding message");
// DATA(insert OID = 3566 (  pg_event_trigger_dropped_objects		PGNSP PGUID 12 10 100 0 0 f f f f t t s s 0 0 2249 "" "{26,26,23,16,16,16,25,25,25,25,1009,1009}" "{o,o,o,o,o,o,o,o,o,o,o,o}" "{classid, objid, objsubid, original, normal, is_temporary, object_type, schema_name, object_name, object_identity, address_names, address_args}" _null_ _null_ pg_event_trigger_dropped_objects _null_ _null_ _null_ ));
// DESCR("list objects dropped by the current command");
// DATA(insert OID = 4566 (  pg_event_trigger_table_rewrite_oid	PGNSP PGUID 12 1 0 0 0 f f f f t f s s 0 0 26 "" "{26}" "{o}" "{oid}" _null_ _null_ pg_event_trigger_table_rewrite_oid _null_ _null_ _null_ ));
// DESCR("return Oid of the table getting rewritten");
// DATA(insert OID = 4567 (  pg_event_trigger_table_rewrite_reason PGNSP PGUID 12 1 0 0 0 f f f f t f s s 0 0 23 "" _null_ _null_ _null_ _null_ _null_ pg_event_trigger_table_rewrite_reason _null_ _null_ _null_ ));
// DESCR("return reason code for table getting rewritten");
// DATA(insert OID = 4568 (  pg_event_trigger_ddl_commands			PGNSP PGUID 12 10 100 0 0 f f f f t t s s 0 0 2249 "" "{26,26,23,25,25,25,25,16,32}" "{o,o,o,o,o,o,o,o,o}" "{classid, objid, objsubid, command_tag, object_type, schema_name, object_identity, in_extension, command}" _null_ _null_ pg_event_trigger_ddl_commands _null_ _null_ _null_ ));
// DESCR("list DDL actions being executed by the current command");
// DATA(insert OID = 3992 ( dense_rank			PGNSP PGUID 12 1 0 2276 0 t f f f f f i s 1 0 20 "2276" "{2276}" "{v}" _null_ _null_ _null_ aggregate_dummy _null_ _null_ _null_ ));
// DESCR("rank of hypothetical row without gaps");
// DATA(insert OID = 3582 ( binary_upgrade_set_next_pg_type_oid PGNSP PGUID  12 1 0 0 0 f f f f t f v r 1 0 2278 "26" _null_ _null_ _null_ _null_ _null_ binary_upgrade_set_next_pg_type_oid _null_ _null_ _null_ ));
// DESCR("for use by pg_upgrade");
// DATA(insert OID = 3584 ( binary_upgrade_set_next_array_pg_type_oid PGNSP PGUID	12 1 0 0 0 f f f f t f v r 1 0 2278 "26" _null_ _null_ _null_ _null_ _null_ binary_upgrade_set_next_array_pg_type_oid _null_ _null_ _null_ ));
// DESCR("for use by pg_upgrade");
// DATA(insert OID = 3585 ( binary_upgrade_set_next_toast_pg_type_oid PGNSP PGUID	12 1 0 0 0 f f f f t f v r 1 0 2278 "26" _null_ _null_ _null_ _null_ _null_ binary_upgrade_set_next_toast_pg_type_oid _null_ _null_ _null_ ));
// DESCR("for use by pg_upgrade");
// DATA(insert OID = 3586 ( binary_upgrade_set_next_heap_pg_class_oid PGNSP PGUID	12 1 0 0 0 f f f f t f v r 1 0 2278 "26" _null_ _null_ _null_ _null_ _null_ binary_upgrade_set_next_heap_pg_class_oid _null_ _null_ _null_ ));
// DESCR("for use by pg_upgrade");
// DATA(insert OID = 3587 ( binary_upgrade_set_next_index_pg_class_oid PGNSP PGUID  12 1 0 0 0 f f f f t f v r 1 0 2278 "26" _null_ _null_ _null_ _null_ _null_ binary_upgrade_set_next_index_pg_class_oid _null_ _null_ _null_ ));
// DESCR("for use by pg_upgrade");
// DATA(insert OID = 3588 ( binary_upgrade_set_next_toast_pg_class_oid PGNSP PGUID  12 1 0 0 0 f f f f t f v r 1 0 2278 "26" _null_ _null_ _null_ _null_ _null_ binary_upgrade_set_next_toast_pg_class_oid _null_ _null_ _null_ ));
// DESCR("for use by pg_upgrade");
// DATA(insert OID = 3589 ( binary_upgrade_set_next_pg_enum_oid PGNSP PGUID  12 1 0 0 0 f f f f t f v r 1 0 2278 "26" _null_ _null_ _null_ _null_ _null_ binary_upgrade_set_next_pg_enum_oid _null_ _null_ _null_ ));
// DESCR("for use by pg_upgrade");
// DATA(insert OID = 3590 ( binary_upgrade_set_next_pg_authid_oid PGNSP PGUID	12 1 0 0 0 f f f f t f v r 1 0 2278 "26" _null_ _null_ _null_ _null_ _null_ binary_upgrade_set_next_pg_authid_oid _null_ _null_ _null_ ));
// DESCR("for use by pg_upgrade");
// DATA(insert OID = 3591 ( binary_upgrade_create_empty_extension PGNSP PGUID	12 1 0 0 0 f f f f f f v r 7 0 2278 "25 25 16 25 1028 1009 1009" _null_ _null_ _null_ _null_ _null_ binary_upgrade_create_empty_extension _null_ _null_ _null_ ));
// DESCR("for use by pg_upgrade");
// DATA(insert OID = 4083 ( binary_upgrade_set_record_init_privs PGNSP PGUID	12 1 0 0 0 f f f f t f v r 1 0 2278 "16" _null_ _null_ _null_ _null_ _null_ binary_upgrade_set_record_init_privs _null_ _null_ _null_ ));
// DESCR("for use by pg_upgrade");
// DATA(insert OID = 6003 ( pg_replication_origin_create PGNSP PGUID 12 1 0 0 0 f f f f t f v u 1 0 26 "25" _null_ _null_ _null_ _null_ _null_ pg_replication_origin_create _null_ _null_ _null_ ));
// DESCR("create a replication origin");
// DATA(insert OID = 6004 ( pg_replication_origin_drop PGNSP PGUID 12 1 0 0 0 f f f f t f v u 1 0 2278 "25" _null_ _null_ _null_ _null_ _null_ pg_replication_origin_drop _null_ _null_ _null_ ));
// DESCR("drop replication origin identified by its name");
// DATA(insert OID = 6005 ( pg_replication_origin_oid PGNSP PGUID 12 1 0 0 0 f f f f t f s s 1 0 26 "25" _null_ _null_ _null_ _null_ _null_ pg_replication_origin_oid _null_ _null_ _null_ ));
// DESCR("translate the replication origin's name to its id");
// DATA(insert OID = 6006 ( pg_replication_origin_session_setup PGNSP PGUID 12 1 0 0 0 f f f f t f v u 1 0 2278 "25" _null_ _null_ _null_ _null_ _null_ pg_replication_origin_session_setup _null_ _null_ _null_ ));
// DESCR("configure session to maintain replication progress tracking for the passed in origin");
// DATA(insert OID = 6007 ( pg_replication_origin_session_reset PGNSP PGUID 12 1 0 0 0 f f f f t f v u 0 0 2278 "" _null_ _null_ _null_ _null_ _null_ pg_replication_origin_session_reset _null_ _null_ _null_ ));
// DESCR("teardown configured replication progress tracking");
// DATA(insert OID = 6008 ( pg_replication_origin_session_is_setup PGNSP PGUID 12 1 0 0 0 f f f f t f v r 0 0 16 "" _null_ _null_ _null_ _null_ _null_ pg_replication_origin_session_is_setup _null_ _null_ _null_ ));
// DESCR("is a replication origin configured in this session");
// DATA(insert OID = 6009 ( pg_replication_origin_session_progress PGNSP PGUID 12 1 0 0 0 f f f f t f v u 1 0 3220 "16" _null_ _null_ _null_ _null_ _null_ pg_replication_origin_session_progress _null_ _null_ _null_ ));
// DESCR("get the replication progress of the current session");
// DATA(insert OID = 6010 ( pg_replication_origin_xact_setup PGNSP PGUID 12 1 0 0 0 f f f f t f v r 2 0 2278 "3220 1184" _null_ _null_ _null_ _null_ _null_ pg_replication_origin_xact_setup _null_ _null_ _null_ ));
// DESCR("setup the transaction's origin lsn and timestamp");
// DATA(insert OID = 6011 ( pg_replication_origin_xact_reset PGNSP PGUID 12 1 0 0 0 f f f f t f v r 0 0 2278 "" _null_ _null_ _null_ _null_ _null_ pg_replication_origin_xact_reset _null_ _null_ _null_ ));
// DESCR("reset the transaction's origin lsn and timestamp");
// DATA(insert OID = 6012 ( pg_replication_origin_advance PGNSP PGUID 12 1 0 0 0 f f f f t f v u 2 0 2278 "25 3220" _null_ _null_ _null_ _null_ _null_ pg_replication_origin_advance _null_ _null_ _null_ ));
// DESCR("advance replication itentifier to specific location");
// DATA(insert OID = 6013 ( pg_replication_origin_progress PGNSP PGUID 12 1 0 0 0 f f f f t f v u 2 0 3220 "25 16" _null_ _null_ _null_ _null_ _null_ pg_replication_origin_progress _null_ _null_ _null_ ));
// DESCR("get an individual replication origin's replication progress");
// DATA(insert OID = 6014 ( pg_show_replication_origin_status PGNSP PGUID 12 1 100 0 0 f f f f f t v r 0 0 2249 "" "{26,25,3220,3220}" "{o,o,o,o}" "{local_id, external_id, remote_lsn, local_lsn}" _null_ _null_ pg_show_replication_origin_status _null_ _null_ _null_ ));
// DESCR("get progress for all replication origins");
// DATA(insert OID = 3298 (  row_security_active	   PGNSP PGUID 12 1 0 0 0 f f f f t f s s 1 0 16 "26" _null_ _null_ _null_ _null_ _null_	row_security_active _null_ _null_ _null_ ));
// DESCR("row security for current context active on table by table oid");
// DATA(insert OID = 3299 (  row_security_active	   PGNSP PGUID 12 1 0 0 0 f f f f t f s s 1 0 16 "25" _null_ _null_ _null_ _null_ _null_	row_security_active_name _null_ _null_ _null_ ));
// DESCR("row security for current context active on table by table name");
// DATA(insert OID = 3400 ( pg_config PGNSP PGUID 12 1 23 0 0 f f f f t t i r 0 0 2249 "" "{25,25}" "{o,o}" "{name,setting}" _null_ _null_ pg_config _null_ _null_ _null_ ));
// DESCR("pg_config binary as a function");
// DATA(insert OID = 3441 ( pg_control_system PGNSP PGUID 12 1 0 0 0 f f f f t f v s 0 0 2249 "" "{23,23,20,1184}" "{o,o,o,o}" "{pg_control_version,catalog_version_no,system_identifier,pg_control_last_modified}" _null_ _null_ pg_control_system _null_ _null_ _null_ ));
// DESCR("pg_controldata general state information as a function");
// DATA(insert OID = 3442 ( pg_control_checkpoint PGNSP PGUID 12 1 0 0 0 f f f f t f v s 0 0 2249 "" "{3220,3220,3220,25,23,23,16,25,26,28,28,28,26,28,28,26,28,28,1184}" "{o,o,o,o,o,o,o,o,o,o,o,o,o,o,o,o,o,o,o}" "{checkpoint_location,prior_location,redo_location,redo_wal_file,timeline_id,prev_timeline_id,full_page_writes,next_xid,next_oid,next_multixact_id,next_multi_offset,oldest_xid,oldest_xid_dbid,oldest_active_xid,oldest_multi_xid,oldest_multi_dbid,oldest_commit_ts_xid,newest_commit_ts_xid,checkpoint_time}" _null_ _null_ pg_control_checkpoint _null_ _null_ _null_ ));
// DESCR("pg_controldata checkpoint state information as a function");
// DATA(insert OID = 3443 ( pg_control_recovery PGNSP PGUID 12 1 0 0 0 f f f f t f v s 0 0 2249 "" "{3220,23,3220,3220,16}" "{o,o,o,o,o}" "{min_recovery_end_location,min_recovery_end_timeline,backup_start_location,backup_end_location,end_of_backup_record_required}" _null_ _null_ pg_control_recovery _null_ _null_ _null_ ));
// DESCR("pg_controldata recovery state information as a function");
// DATA(insert OID = 3444 ( pg_control_init PGNSP PGUID 12 1 0 0 0 f f f f t f v s 0 0 2249 "" "{23,23,23,23,23,23,23,23,23,16,16,16,23}" "{o,o,o,o,o,o,o,o,o,o,o,o,o}" "{max_data_alignment,database_block_size,blocks_per_segment,wal_block_size,bytes_per_wal_segment,max_identifier_length,max_index_columns,max_toast_chunk_size,large_object_chunk_size,bigint_timestamps,float4_pass_by_value,float8_pass_by_value,data_page_checksum_version}" _null_ _null_ pg_control_init _null_ _null_ _null_ ));
// DESCR("pg_controldata init state information as a function");

// //second

// typedef enum
// {
// 	BACKSLASH_QUOTE_OFF,
// 	BACKSLASH_QUOTE_ON,
// 	BACKSLASH_QUOTE_SAFE_ENCODING
// }	BackslashQuoteType;
// extern int	backslash_quote;
// extern bool escape_string_warning;
// extern PGDLLIMPORT bool standard_conforming_strings;
// typedef struct ExpandedObjectHeader ExpandedObjectHeader;
// typedef struct varatt_expanded
// {
// 	ExpandedObjectHeader *eohptr;
// } varatt_expanded;
// typedef enum vartag_external
// {
// 	VARTAG_INDIRECT = 1,
// 	VARTAG_EXPANDED_RO = 2,
// 	VARTAG_EXPANDED_RW = 3,
// 	VARTAG_ONDISK = 18
// } vartag_external;
// #define VARATT_IS_EXTERNAL_EXPANDED(PTR) \
// 	(VARATT_IS_EXTERNAL(PTR) && VARTAG_IS_EXPANDED(VARTAG_EXTERNAL(PTR)))
// #define VARTAG_SIZE(tag) \
// 	((tag) == VARTAG_INDIRECT ? sizeof(varatt_indirect) : \
// 	 VARTAG_IS_EXPANDED(tag) ? sizeof(varatt_expanded) : \
// 	 (tag) == VARTAG_ONDISK ? sizeof(varatt_external) : \
// 	 TrapMacro(true, "unrecognized TOAST vartag"))
// #define Int16GetDatum(X) ((Datum) SET_2_BYTES(X))
// #define Int32GetDatum(X) ((Datum) SET_4_BYTES(X))
// #define DatumGetTransactionId(X) ((TransactionId) GET_4_BYTES(X))
// #define TransactionIdGetDatum(X) ((Datum) SET_4_BYTES((X)))
// #define MultiXactIdGetDatum(X) ((Datum) SET_4_BYTES((X)))

// typedef enum ReplaceVarsNoMatchOption
// {
// 	REPLACEVARS_REPORT_ERROR,	/* throw error if no match */
// 	REPLACEVARS_CHANGE_VARNO,	/* change the Var's varno, nothing else */
// 	REPLACEVARS_SUBSTITUTE_NULL /* replace with a NULL Const */
// } ReplaceVarsNoMatchOption;
// typedef enum XLTW_Oper
// {
// 	XLTW_None,
// 	XLTW_Update,
// 	XLTW_Delete,
// 	XLTW_Lock,
// 	XLTW_LockUpdated,
// 	XLTW_InsertIndex,
// 	XLTW_InsertIndexUnique,
// 	XLTW_FetchUpdated,
// 	XLTW_RecheckExclusionConstr
// } XLTW_Oper;

// extern CommandDest whereToSendOutput;
// extern PGDLLIMPORT const char *debug_query_string;
// extern int	max_stack_depth;
// extern int	PostAuthDelay;
// extern int	log_statement;
// typedef struct ExpandedArrayHeader
// {
// 	/* Standard header for expanded objects */
// 	ExpandedObjectHeader hdr;

// 	/* Magic value identifying an expanded array (for debugging only) */
// 	int			ea_magic;

// 	/* Dimensionality info (always valid) */
// 	int			ndims;			/* # of dimensions */
// 	int		   *dims;			/* array dimensions */
// 	int		   *lbound;			/* index lower bounds for each dimension */

// 	/* Element type info (always valid) */
// 	Oid			element_type;	/* element type OID */
// 	int16		typlen;			/* needed info about element datatype */
// 	bool		typbyval;
// 	char		typalign;

// 	/*
// 	 * If we have a Datum-array representation of the array, it's kept here;
// 	 * else dvalues/dnulls are NULL.  The dvalues and dnulls arrays are always
// 	 * palloc'd within the object private context, but may change size from
// 	 * time to time.  For pass-by-ref element types, dvalues entries might
// 	 * point either into the fstartptr..fendptr area, or to separately
// 	 * palloc'd chunks.  Elements should always be fully detoasted, as they
// 	 * are in the standard flat representation.
// 	 *
// 	 * Even when dvalues is valid, dnulls can be NULL if there are no null
// 	 * elements.
// 	 */
// 	Datum	   *dvalues;		/* array of Datums */
// 	bool	   *dnulls;			/* array of is-null flags for Datums */
// 	int			dvalueslen;		/* allocated length of above arrays */
// 	int			nelems;			/* number of valid entries in above arrays */

// 	/*
// 	 * flat_size is the current space requirement for the flat equivalent of
// 	 * the expanded array, if known; otherwise it's 0.  We store this to make
// 	 * consecutive calls of get_flat_size cheap.
// 	 */
// 	Size		flat_size;

// 	/*
// 	 * fvalue points to the flat representation if it is valid, else it is
// 	 * NULL.  If we have or ever had a flat representation then
// 	 * fstartptr/fendptr point to the start and end+1 of its data area; this
// 	 * is so that we can tell which Datum pointers point into the flat
// 	 * representation rather than being pointers to separately palloc'd data.
// 	 */
// 	ArrayType  *fvalue;			/* must be a fully detoasted array */
// 	char	   *fstartptr;		/* start of its data area */
// 	char	   *fendptr;		/* end+1 of its data area */
// } ExpandedArrayHeader;

// typedef union AnyArrayType
// {
// 	ArrayType	flt;
// 	ExpandedArrayHeader xpn;
// } AnyArrayType;

// typedef struct ArrayBuildStateArr
// {
// 	MemoryContext mcontext;		/* where all the temp stuff is kept */
// 	char	   *data;			/* accumulated data */
// 	bits8	   *nullbitmap;		/* bitmap of is-null flags, or NULL if none */
// 	int			abytes;			/* allocated length of "data" */
// 	int			nbytes;			/* number of bytes used so far */
// 	int			aitems;			/* allocated length of bitmap (in elements) */
// 	int			nitems;			/* total number of elements in result */
// 	int			ndims;			/* current dimensions of result */
// 	int			dims[MAXDIM];
// 	int			lbs[MAXDIM];
// 	Oid			array_type;		/* data type of the arrays */
// 	Oid			element_type;	/* data type of the array elements */
// 	bool		private_cxt;	/* use private memory context */
// } ArrayBuildStateArr;

// typedef struct ArrayBuildStateAny
// {
// 	/* Exactly one of these is not NULL: */
// 	ArrayBuildState *scalarstate;
// 	ArrayBuildStateArr *arraystate;
// } ArrayBuildStateAny;
// #define PG_GETARG_EXPANDED_ARRAY(n)  DatumGetExpandedArray(PG_GETARG_DATUM(n))
// #define PG_GETARG_EXPANDED_ARRAYX(n, metacache) \
// 	DatumGetExpandedArrayX(PG_GETARG_DATUM(n), metacache)
// #define PG_RETURN_EXPANDED_ARRAY(x)  PG_RETURN_DATUM(EOHPGetRWDatum(&(x)->hdr))
// #define PG_GETARG_ANY_ARRAY(n)	DatumGetAnyArray(PG_GETARG_DATUM(n))
// #define AARR_NDIM(a) \
// 	(VARATT_IS_EXPANDED_HEADER(a) ? (a)->xpn.ndims : ARR_NDIM(&(a)->flt))
// #define AARR_HASNULL(a) \
// 	(VARATT_IS_EXPANDED_HEADER(a) ? \
// 	 ((a)->xpn.dvalues != NULL ? (a)->xpn.dnulls != NULL : ARR_HASNULL((a)->xpn.fvalue)) : \
// 	 ARR_HASNULL(&(a)->flt))
// #define AARR_ELEMTYPE(a) \
// 	(VARATT_IS_EXPANDED_HEADER(a) ? (a)->xpn.element_type : ARR_ELEMTYPE(&(a)->flt))
// #define AARR_DIMS(a) \
// 	(VARATT_IS_EXPANDED_HEADER(a) ? (a)->xpn.dims : ARR_DIMS(&(a)->flt))
// #define AARR_LBOUND(a) \
// 	(VARATT_IS_EXPANDED_HEADER(a) ? (a)->xpn.lbound : ARR_LBOUND(&(a)->flt))
// extern bool Array_nulls;
// typedef enum
// {
// 	PGC_INTERNAL,
// 	PGC_POSTMASTER,
// 	PGC_SIGHUP,
// 	PGC_SU_BACKEND,
// 	PGC_BACKEND,
// 	PGC_SUSET,
// 	PGC_USERSET
// } GucContext;
// typedef enum
// {
// 	PGC_S_DEFAULT,				/* hard-wired default ("boot_val") */
// 	PGC_S_DYNAMIC_DEFAULT,		/* default computed during initialization */
// 	PGC_S_ENV_VAR,				/* postmaster environment variable */
// 	PGC_S_FILE,					/* postgresql.conf */
// 	PGC_S_ARGV,					/* postmaster command line */
// 	PGC_S_GLOBAL,				/* global in-database setting */
// 	PGC_S_DATABASE,				/* per-database setting */
// 	PGC_S_USER,					/* per-user setting */
// 	PGC_S_DATABASE_USER,		/* per-user-and-database setting */
// 	PGC_S_CLIENT,				/* from client connection request */
// 	PGC_S_OVERRIDE,				/* special case to forcibly set default */
// 	PGC_S_INTERACTIVE,			/* dividing line for error reporting */
// 	PGC_S_TEST,					/* test per-database or per-user setting */
// 	PGC_S_SESSION				/* SET command */
// } GucSource;
// typedef struct ConfigVariable
// {
// 	char	   *name;
// 	char	   *value;
// 	char	   *errmsg;
// 	char	   *filename;
// 	int			sourceline;
// 	bool		ignore;
// 	bool		applied;
// 	struct ConfigVariable *next;
// } ConfigVariable;
// #define GUC_NOT_WHILE_SEC_REST	0x0400	/* can't set if security restricted */
// #define GUC_DISALLOW_IN_AUTO_FILE 0x0800		/* can't set in
// #define GUC_UNIT_KB				0x1000	/* value is in kilobytes */
// #define GUC_UNIT_BLOCKS			0x2000	/* value is in blocks */
// #define GUC_UNIT_XBLOCKS		0x3000	/* value is in xlog blocks */
// #define GUC_UNIT_XSEGS			0x4000	/* value is in xlog segments */
// #define GUC_UNIT_MEMORY			0xF000	/* mask for size-related units */
// #define GUC_UNIT_MS			   0x10000	/* value is in milliseconds */
// #define GUC_UNIT_S			   0x20000	/* value is in seconds */
// #define GUC_UNIT_MIN		   0x30000	/* value is in minutes */
// #define GUC_UNIT_TIME		   0xF0000	/* mask for time-related units */
// /* GUC vars that are actually declared in guc.c, rather than elsewhere */
// extern bool log_duration;
// extern bool Debug_print_plan;
// extern bool Debug_print_parse;
// extern bool Debug_print_rewritten;
// extern bool Debug_pretty_print;

// extern bool log_parser_stats;
// extern bool log_planner_stats;
// extern bool log_executor_stats;
// extern bool log_statement_stats;
// extern bool log_btree_build_stats;
// extern PGDLLIMPORT bool check_function_bodies;
// extern bool default_with_oids;
// extern bool SQL_inheritance;
// extern int	log_min_error_statement;
// extern int	log_min_duration_statement;
// extern int	log_temp_files;

// extern int	temp_file_limit;

// extern int	num_temp_buffers;

// extern char *cluster_name;
// extern char *ConfigFileName;
// extern char *HbaFileName;
// extern char *IdentFileName;
// extern char *external_pid_file;

// extern char *application_name;

// extern int	tcp_keepalives_idle;
// extern int	tcp_keepalives_interval;
// extern int	tcp_keepalives_count;

// #ifdef TRACE_SORT
// extern bool trace_sort;
// #endif

// extern PGDLLIMPORT char *GUC_check_errmsg_string;
// extern PGDLLIMPORT char *GUC_check_errdetail_string;
// extern PGDLLIMPORT char *GUC_check_errhint_string;
// #define GUC_check_errmsg \
// 	pre_format_elog_string(errno, TEXTDOMAIN), \
// 	GUC_check_errmsg_string = format_elog_string

// #define GUC_check_errdetail \
// 	pre_format_elog_string(errno, TEXTDOMAIN), \
// 	GUC_check_errdetail_string = format_elog_string

// #define GUC_check_errhint \
// 	pre_format_elog_string(errno, TEXTDOMAIN), \
// 	GUC_check_errhint_string = format_elog_string

// /* Flags to indicate which parameters are supplied */
// #define HASH_PARTITION	0x0001	/* Hashtable is used w/partitioned locking */
// #define HASH_SEGMENT	0x0002	/* Set segment size */
// #define HASH_DIRSIZE	0x0004	/* Set directory size (initial and max) */
// #define HASH_FFACTOR	0x0008	/* Set fill factor */
// #define HASH_ELEM		0x0010	/* Set keysize and entrysize */
// #define HASH_BLOBS		0x0020	/* Select support functions for binary keys */
// #define HASH_FUNCTION	0x0040	/* Set user defined hash function */
// #define HASH_COMPARE	0x0080	/* Set user defined comparison function */
// #define HASH_KEYCOPY	0x0100	/* Set user defined key-copying function */
// #define HASH_ALLOC		0x0200	/* Set memory allocator */
// #define HASH_CONTEXT	0x0400	/* Set memory allocation context */
// #define HASH_SHARED_MEM 0x0800	/* Hashtable is in shared memory */
// #define HASH_ATTACH		0x1000	/* Do not initialize hctl */
// #define HASH_FIXED_SIZE 0x2000	/* Initial size is a hard limit */
// #define oid_hash uint32_hash	/* Remove me eventually */

// extern PGDLLIMPORT get_attavgwidth_hook_type get_attavgwidth_hook;
// extern PGDLLIMPORT MemoryContext PostmasterContext;
// extern PGDLLIMPORT MemoryContext CacheMemoryContext;
// extern PGDLLIMPORT MemoryContext MessageContext;
// extern PGDLLIMPORT MemoryContext TopTransactionContext;
// extern PGDLLIMPORT MemoryContext CurTransactionContext;
// /* This is a transient link to the active portal's memory context: */
// extern PGDLLIMPORT MemoryContext PortalContext;
// #define MemoryContextResetAndDeleteChildren(ctx) MemoryContextReset(ctx)

// #define ALLOCSET_SMALL_SIZES \
// 	ALLOCSET_SMALL_MINSIZE, ALLOCSET_SMALL_INITSIZE, ALLOCSET_SMALL_MAXSIZE
// #define ALLOCSET_START_SMALL_SIZES \
// 	ALLOCSET_SMALL_MINSIZE, ALLOCSET_SMALL_INITSIZE, ALLOCSET_DEFAULT_MAXSIZE
// #define ALLOCSET_SEPARATE_THRESHOLD  8192

// typedef struct RelationData
// {
// 	RelFileNode rd_node;		/* relation physical identifier */
// 	/* use "struct" here to avoid needing to include smgr.h: */
// 	struct SMgrRelationData *rd_smgr;	/* cached file handle, or NULL */
// 	int			rd_refcnt;		/* reference count */
// 	BackendId	rd_backend;		/* owning backend id, if temporary relation */
// 	bool		rd_islocaltemp; /* rel is a temp rel of this session */
// 	bool		rd_isnailed;	/* rel is nailed in cache */
// 	bool		rd_isvalid;		/* relcache entry is valid */
// 	char		rd_indexvalid;	/* state of rd_indexlist: 0 = not valid, 1 =
// 								 * valid, 2 = temporarily forced */

// 	/*
// 	 * rd_createSubid is the ID of the highest subtransaction the rel has
// 	 * survived into; or zero if the rel was not created in the current top
// 	 * transaction.  This can be now be relied on, whereas previously it could
// 	 * be "forgotten" in earlier releases. Likewise, rd_newRelfilenodeSubid is
// 	 * the ID of the highest subtransaction the relfilenode change has
// 	 * survived into, or zero if not changed in the current transaction (or we
// 	 * have forgotten changing it). rd_newRelfilenodeSubid can be forgotten
// 	 * when a relation has multiple new relfilenodes within a single
// 	 * transaction, with one of them occurring in a subsequently aborted
// 	 * subtransaction, e.g. BEGIN; TRUNCATE t; SAVEPOINT save; TRUNCATE t;
// 	 * ROLLBACK TO save; -- rd_newRelfilenode is now forgotten
// 	 */
// 	SubTransactionId rd_createSubid;	/* rel was created in current xact */
// 	SubTransactionId rd_newRelfilenodeSubid;	/* new relfilenode assigned in
// 												 * current xact */

// 	Form_pg_class rd_rel;		/* RELATION tuple */
// 	TupleDesc	rd_att;			/* tuple descriptor */
// 	Oid			rd_id;			/* relation's object id */
// 	LockInfoData rd_lockInfo;	/* lock mgr's info for locking relation */
// 	RuleLock   *rd_rules;		/* rewrite rules */
// 	MemoryContext rd_rulescxt;	/* private memory cxt for rd_rules, if any */
// 	TriggerDesc *trigdesc;		/* Trigger info, or NULL if rel has none */
// 	/* use "struct" here to avoid needing to include rowsecurity.h: */
// 	struct RowSecurityDesc *rd_rsdesc;	/* row security policies, or NULL */

// 	/* data managed by RelationGetFKeyList: */
// 	List	   *rd_fkeylist;	/* list of ForeignKeyCacheInfo (see below) */
// 	bool		rd_fkeyvalid;	/* true if list has been computed */

// 	/* data managed by RelationGetIndexList: */
// 	List	   *rd_indexlist;	/* list of OIDs of indexes on relation */
// 	Oid			rd_oidindex;	/* OID of unique index on OID, if any */
// 	Oid			rd_replidindex; /* OID of replica identity index, if any */

// 	/* data managed by RelationGetIndexAttrBitmap: */
// 	Bitmapset  *rd_indexattr;	/* identifies columns used in indexes */
// 	Bitmapset  *rd_keyattr;		/* cols that can be ref'd by foreign keys */
// 	Bitmapset  *rd_idattr;		/* included in replica identity index */

// 	/*
// 	 * rd_options is set whenever rd_rel is loaded into the relcache entry.
// 	 * Note that you can NOT look into rd_rel for this data.  NULL means "use
// 	 * defaults".
// 	 */
// 	bytea	   *rd_options;		/* parsed pg_class.reloptions */

// 	/* These are non-NULL only for an index relation: */
// 	Form_pg_index rd_index;		/* pg_index tuple describing this index */
// 	/* use "struct" here to avoid needing to include htup.h: */
// 	struct HeapTupleData *rd_indextuple;		/* all of pg_index tuple */

// 	/*
// 	 * index access support info (used only for an index relation)
// 	 *
// 	 * Note: only default support procs for each opclass are cached, namely
// 	 * those with lefttype and righttype equal to the opclass's opcintype. The
// 	 * arrays are indexed by support function number, which is a sufficient
// 	 * identifier given that restriction.
// 	 *
// 	 * Note: rd_amcache is available for index AMs to cache private data about
// 	 * an index.  This must be just a cache since it may get reset at any time
// 	 * (in particular, it will get reset by a relcache inval message for the
// 	 * index).  If used, it must point to a single memory chunk palloc'd in
// 	 * rd_indexcxt.  A relcache reset will include freeing that chunk and
// 	 * setting rd_amcache = NULL.
// 	 */
// 	Oid			rd_amhandler;	/* OID of index AM's handler function */
// 	MemoryContext rd_indexcxt;	/* private memory cxt for this stuff */
// 	/* use "struct" here to avoid needing to include amapi.h: */
// 	struct IndexAmRoutine *rd_amroutine;		/* index AM's API struct */
// 	Oid		   *rd_opfamily;	/* OIDs of op families for each index col */
// 	Oid		   *rd_opcintype;	/* OIDs of opclass declared input data types */
// 	RegProcedure *rd_support;	/* OIDs of support procedures */
// 	FmgrInfo   *rd_supportinfo; /* lookup info for support procedures */
// 	int16	   *rd_indoption;	/* per-column AM-specific flags */
// 	List	   *rd_indexprs;	/* index expression trees, if any */
// 	List	   *rd_indpred;		/* index predicate tree, if any */
// 	Oid		   *rd_exclops;		/* OIDs of exclusion operators, if any */
// 	Oid		   *rd_exclprocs;	/* OIDs of exclusion ops' procs, if any */
// 	uint16	   *rd_exclstrats;	/* exclusion ops' strategy numbers, if any */
// 	void	   *rd_amcache;		/* available for use by index AM */
// 	Oid		   *rd_indcollation;	/* OIDs of index collations */

// 	/*
// 	 * foreign-table support
// 	 *
// 	 * rd_fdwroutine must point to a single memory chunk palloc'd in
// 	 * CacheMemoryContext.  It will be freed and reset to NULL on a relcache
// 	 * reset.
// 	 */

// 	/* use "struct" here to avoid needing to include fdwapi.h: */
// 	struct FdwRoutine *rd_fdwroutine;	/* cached function pointers, or NULL */

// 	/*
// 	 * Hack for CLUSTER, rewriting ALTER TABLE, etc: when writing a new
// 	 * version of a table, we need to make any toast pointers inserted into it
// 	 * have the existing toast table's OID, not the OID of the transient toast
// 	 * table.  If rd_toastoid isn't InvalidOid, it is the OID to place in
// 	 * toast pointers inserted into this rel.  (Note it's set on the new
// 	 * version of the main heap, not the toast table itself.)  This also
// 	 * causes toast_save_datum() to try to preserve toast value OIDs.
// 	 */
// 	Oid			rd_toastoid;	/* Real TOAST table's OID, or InvalidOid */

// 	/* use "struct" here to avoid needing to include pgstat.h: */
// 	struct PgStat_TableStatus *pgstat_info;		/* statistics collection area */
// } RelationData;
// typedef struct ForeignKeyCacheInfo
// {
// 	NodeTag		type;
// 	Oid			conrelid;		/* relation constrained by the foreign key */
// 	Oid			confrelid;		/* relation referenced by the foreign key */
// 	int			nkeys;			/* number of columns in the foreign key */
// 	/* these arrays each have nkeys valid entries: */
// 	AttrNumber	conkey[INDEX_MAX_KEYS]; /* cols in referencing table */
// 	AttrNumber	confkey[INDEX_MAX_KEYS];		/* cols in referenced table */
// 	Oid			conpfeqop[INDEX_MAX_KEYS];		/* PK = FK operator OIDs */
// } ForeignKeyCacheInfo;

// typedef struct AutoVacOpts
// {
// 	bool		enabled;
// 	int			vacuum_threshold;
// 	int			analyze_threshold;
// 	int			vacuum_cost_delay;
// 	int			vacuum_cost_limit;
// 	int			freeze_min_age;
// 	int			freeze_max_age;
// 	int			freeze_table_age;
// 	int			multixact_freeze_min_age;
// 	int			multixact_freeze_max_age;
// 	int			multixact_freeze_table_age;
// 	int			log_min_duration;
// 	float8		vacuum_scale_factor;
// 	float8		analyze_scale_factor;
// } AutoVacOpts;

// typedef struct StdRdOptions
// {
// 	int32		vl_len_;		/* varlena header (do not touch directly!) */
// 	int			fillfactor;		/* page fill factor in percent (0..100) */
// 	AutoVacOpts autovacuum;		/* autovacuum-related options */
// 	bool		user_catalog_table;		/* use as an additional catalog
// 										 * relation */
// 	int			parallel_workers;		/* max number of parallel workers */
// } StdRdOptions;
// #define RelationIsUsedAsCatalogTable(relation)	\
// 	((relation)->rd_options && \
// 	 ((relation)->rd_rel->relkind == RELKIND_RELATION || \
// 	  (relation)->rd_rel->relkind == RELKIND_MATVIEW) ? \
// 	 ((StdRdOptions *) (relation)->rd_options)->user_catalog_table : false)
// typedef struct ViewOptions
// {
// 	int32		vl_len_;		/* varlena header (do not touch directly!) */
// 	bool		security_barrier;
// 	int			check_option_offset;
// } ViewOptions;
// #define RelationIsSecurityView(relation)	\
// 	((relation)->rd_options ?				\
// 	 ((ViewOptions *) (relation)->rd_options)->security_barrier : false)
// #define RelationHasCheckOption(relation)									\
// 	((relation)->rd_options &&												\
// 	 ((ViewOptions *) (relation)->rd_options)->check_option_offset != 0)
// #define RelationHasLocalCheckOption(relation)								\
// 	((relation)->rd_options &&												\
// 	 ((ViewOptions *) (relation)->rd_options)->check_option_offset != 0 ?	\
// 	 strcmp((char *) (relation)->rd_options +								\
// 			((ViewOptions *) (relation)->rd_options)->check_option_offset,	\
// 			"local") == 0 : false)
// #define RelationHasCascadedCheckOption(relation)							\
// 	((relation)->rd_options &&												\
// 	 ((ViewOptions *) (relation)->rd_options)->check_option_offset != 0 ?	\
// 	 strcmp((char *) (relation)->rd_options +								\
// 			((ViewOptions *) (relation)->rd_options)->check_option_offset,	\
// 			"cascaded") == 0 : false)
// #define RelationUsesLocalBuffers(relation) \
// 	((relation)->rd_rel->relpersistence == RELPERSISTENCE_TEMP)
// #define RELATION_IS_OTHER_TEMP(relation) \
// 	((relation)->rd_rel->relpersistence == RELPERSISTENCE_TEMP && \
// 	 !(relation)->rd_islocaltemp)
// #define RelationIsScannable(relation) ((relation)->rd_rel->relispopulated)
// #define RelationIsPopulated(relation) ((relation)->rd_rel->relispopulated)
// /* should be used only by relcache.c and catcache.c */
// extern bool criticalRelcachesBuilt;

// /* should be used only by relcache.c and postinit.c */
// extern bool criticalSharedRelcachesBuilt;
// extern bool row_security;
// enum CheckEnableRlsResult
// {
// 	RLS_NONE,
// 	RLS_NONE_ENV,
// 	RLS_ENABLED
// };

// #define OLD_SNAPSHOT_PADDING_ENTRIES 10
// #define OLD_SNAPSHOT_TIME_MAP_ENTRIES (old_snapshot_threshold + OLD_SNAPSHOT_PADDING_ENTRIES)
// #define RelationAllowsEarlyPruning(rel) \
// ( \
// 	 RelationNeedsWAL(rel) \
//   && !IsCatalogRelation(rel) \
//   && !RelationIsAccessibleInLogicalDecoding(rel) \
//   && !RelationHasUnloggedIndex(rel) \
// )
// #define EarlyPruningEnabled(rel) (old_snapshot_threshold >= 0 && RelationAllowsEarlyPruning(rel))
// extern PGDLLIMPORT int old_snapshot_threshold;

// extern bool FirstSnapshotSet;
// extern TransactionId TransactionXmin;
// extern TransactionId RecentXmin;
// extern TransactionId RecentGlobalXmin;
// extern TransactionId RecentGlobalDataXmin;

// enum SysCacheIdentifier
// {
// 	AGGFNOID = 0,
// 	AMNAME,
// 	AMOID,
// 	AMOPOPID,
// 	AMOPSTRATEGY,
// 	AMPROCNUM,
// 	ATTNAME,
// 	ATTNUM,
// 	AUTHMEMMEMROLE,
// 	AUTHMEMROLEMEM,
// 	AUTHNAME,
// 	AUTHOID,
// 	CASTSOURCETARGET,
// 	CLAAMNAMENSP,
// 	CLAOID,
// 	COLLNAMEENCNSP,
// 	COLLOID,
// 	CONDEFAULT,
// 	CONNAMENSP,
// 	CONSTROID,
// 	CONVOID,
// 	DATABASEOID,
// 	DEFACLROLENSPOBJ,
// 	ENUMOID,
// 	ENUMTYPOIDNAME,
// 	EVENTTRIGGERNAME,
// 	EVENTTRIGGEROID,
// 	FOREIGNDATAWRAPPERNAME,
// 	FOREIGNDATAWRAPPEROID,
// 	FOREIGNSERVERNAME,
// 	FOREIGNSERVEROID,
// 	FOREIGNTABLEREL,
// 	INDEXRELID,
// 	LANGNAME,
// 	LANGOID,
// 	NAMESPACENAME,
// 	NAMESPACEOID,
// 	OPERNAMENSP,
// 	OPEROID,
// 	OPFAMILYAMNAMENSP,
// 	OPFAMILYOID,
// 	PROCNAMEARGSNSP,
// 	PROCOID,
// 	RANGETYPE,
// 	RELNAMENSP,
// 	RELOID,
// 	REPLORIGIDENT,
// 	REPLORIGNAME,
// 	RULERELNAME,
// 	STATRELATTINH,
// 	TABLESPACEOID,
// 	TRFOID,
// 	TRFTYPELANG,
// 	TSCONFIGMAP,
// 	TSCONFIGNAMENSP,
// 	TSCONFIGOID,
// 	TSDICTNAMENSP,
// 	TSDICTOID,
// 	TSPARSERNAMENSP,
// 	TSPARSEROID,
// 	TSTEMPLATENAMENSP,
// 	TSTEMPLATEOID,
// 	TYPENAMENSP,
// 	TYPEOID,
// 	USERMAPPINGOID,
// 	USERMAPPINGUSERSERVER
// };
// #define ReleaseSysCacheList(x)	ReleaseCatCacheList(x)
// /* Set at postmaster start */
// extern TimestampTz PgStartTime;

// /* Set at configuration reload */
// extern TimestampTz PgReloadTime;


// /* Bit flags to indicate which fields a given caller needs to have set */
// #define TYPECACHE_EQ_OPR			0x0001
// #define TYPECACHE_LT_OPR			0x0002
// #define TYPECACHE_GT_OPR			0x0004
// #define TYPECACHE_CMP_PROC			0x0008
// #define TYPECACHE_HASH_PROC			0x0010
// #define TYPECACHE_EQ_OPR_FINFO		0x0020
// #define TYPECACHE_CMP_PROC_FINFO	0x0040
// #define TYPECACHE_HASH_PROC_FINFO	0x0080
// #define TYPECACHE_TUPDESC			0x0100
// #define TYPECACHE_BTREE_OPFAMILY	0x0200
// #define TYPECACHE_HASH_OPFAMILY		0x0400
// #define TYPECACHE_RANGE_INFO		0x0800
// #define TYPECACHE_DOMAIN_INFO		0x1000

// typedef struct TypeCacheEntry
// {
// 	/* typeId is the hash lookup key and MUST BE FIRST */
// 	Oid			type_id;		/* OID of the data type */

// 	/* some subsidiary information copied from the pg_type row */
// 	int16		typlen;
// 	bool		typbyval;
// 	char		typalign;
// 	char		typstorage;
// 	char		typtype;
// 	Oid			typrelid;

// 	/*
// 	 * Information obtained from opfamily entries
// 	 *
// 	 * These will be InvalidOid if no match could be found, or if the
// 	 * information hasn't yet been requested.  Also note that for array and
// 	 * composite types, typcache.c checks that the contained types are
// 	 * comparable or hashable before allowing eq_opr etc to become set.
// 	 */
// 	Oid			btree_opf;		/* the default btree opclass' family */
// 	Oid			btree_opintype; /* the default btree opclass' opcintype */
// 	Oid			hash_opf;		/* the default hash opclass' family */
// 	Oid			hash_opintype;	/* the default hash opclass' opcintype */
// 	Oid			eq_opr;			/* the equality operator */
// 	Oid			lt_opr;			/* the less-than operator */
// 	Oid			gt_opr;			/* the greater-than operator */
// 	Oid			cmp_proc;		/* the btree comparison function */
// 	Oid			hash_proc;		/* the hash calculation function */

// 	/*
// 	 * Pre-set-up fmgr call info for the equality operator, the btree
// 	 * comparison function, and the hash calculation function.  These are kept
// 	 * in the type cache to avoid problems with memory leaks in repeated calls
// 	 * to functions such as array_eq, array_cmp, hash_array.  There is not
// 	 * currently a need to maintain call info for the lt_opr or gt_opr.
// 	 */
// 	FmgrInfo	eq_opr_finfo;
// 	FmgrInfo	cmp_proc_finfo;
// 	FmgrInfo	hash_proc_finfo;

// 	/*
// 	 * Tuple descriptor if it's a composite type (row type).  NULL if not
// 	 * composite or information hasn't yet been requested.  (NOTE: this is a
// 	 * reference-counted tupledesc.)
// 	 */
// 	TupleDesc	tupDesc;

// 	/*
// 	 * Fields computed when TYPECACHE_RANGE_INFO is requested.  Zeroes if not
// 	 * a range type or information hasn't yet been requested.  Note that
// 	 * rng_cmp_proc_finfo could be different from the element type's default
// 	 * btree comparison function.
// 	 */
// 	struct TypeCacheEntry *rngelemtype; /* range's element type */
// 	Oid			rng_collation;	/* collation for comparisons, if any */
// 	FmgrInfo	rng_cmp_proc_finfo;		/* comparison function */
// 	FmgrInfo	rng_canonical_finfo;	/* canonicalization function, if any */
// 	FmgrInfo	rng_subdiff_finfo;		/* difference function, if any */

// 	/*
// 	 * Domain constraint data if it's a domain type.  NULL if not domain, or
// 	 * if domain has no constraints, or if information hasn't been requested.
// 	 */
// 	DomainConstraintCache *domainData;

// 	/* Private data, for internal use of typcache.c only */
// 	int			flags;			/* flags about what we've computed */

// 	/*
// 	 * Private information about an enum type.  NULL if not enum or
// 	 * information hasn't been requested.
// 	 */
// 	struct TypeCacheEnumData *enumData;

// 	/* We also maintain a list of all known domain-type cache entries */
// 	struct TypeCacheEntry *nextDomain;
// } TypeCacheEntry; 

// typedef struct MemoryContextCallback
// {
// 	MemoryContextCallbackFunction func; /* function to call */
// 	void	   *arg;			/* argument to pass it */
// 	struct MemoryContextCallback *next; /* next in list of callbacks */
// } MemoryContextCallback; 

// typedef struct DomainConstraintRef
// {
// 	List	   *constraints;	/* list of DomainConstraintState nodes */
// 	MemoryContext refctx;		/* context holding DomainConstraintRef */
// 	TypeCacheEntry *tcache;		/* typcache entry for domain type */

// 	/* Management data --- treat these fields as private to typcache.c */
// 	DomainConstraintCache *dcc; /* current constraints, or NULL if none */
// 	MemoryContextCallback callback;		/* used to release refcount when done */
// } DomainConstraintRef;

// //thrid
// //commands/sequence.h
// typedef struct xl_seq_rec
// {
// 	RelFileNode node;
// 	/* SEQUENCE TUPLE DATA FOLLOWS AT THE END */
// } xl_seq_rec;

// #define SEQ_COL_LASTCOL SEQ_COL_CALLED

// //end sequence.h

// //commands/explain.h
// extern PGDLLIMPORT ExplainOneQuery_hook_type ExplainOneQuery_hook;
// //end explain.h

// //commands/trigger.h
// #define TRIGGER_FIRED_BY_INSERT(event) \
// 	(((event) & TRIGGER_EVENT_OPMASK) == TRIGGER_EVENT_INSERT)

// #define TRIGGER_FIRED_BY_DELETE(event) \
// 	(((event) & TRIGGER_EVENT_OPMASK) == TRIGGER_EVENT_DELETE)

// #define TRIGGER_FIRED_BY_UPDATE(event) \
// 	(((event) & TRIGGER_EVENT_OPMASK) == TRIGGER_EVENT_UPDATE)

// #define TRIGGER_FIRED_BY_TRUNCATE(event) \
// 	(((event) & TRIGGER_EVENT_OPMASK) == TRIGGER_EVENT_TRUNCATE)

// #define TRIGGER_FIRED_FOR_ROW(event) \
// 	((event) & TRIGGER_EVENT_ROW)

// #define TRIGGER_FIRED_FOR_STATEMENT(event) \
// 	(!TRIGGER_FIRED_FOR_ROW(event))

// #define TRIGGER_FIRED_BEFORE(event) \
// 	(((event) & TRIGGER_EVENT_TIMINGMASK) == TRIGGER_EVENT_BEFORE)

// #define TRIGGER_FIRED_AFTER(event) \
// 	(((event) & TRIGGER_EVENT_TIMINGMASK) == TRIGGER_EVENT_AFTER)

// #define TRIGGER_FIRED_INSTEAD(event) \
// 	(((event) & TRIGGER_EVENT_TIMINGMASK) == TRIGGER_EVENT_INSTEAD)

// #define CALLED_AS_TRIGGER(fcinfo) \
// 	((fcinfo)->context != NULL && IsA((fcinfo)->context, TriggerData))

// //end trigger.h


// //executor/executor.h

// #define ExecEvalExpr(expr, econtext, isNull, isDone) \
// 	((*(expr)->evalfunc) (expr, econtext, isNull, isDone))

// #define GetPerTupleMemoryContext(estate) \
// 	(GetPerTupleExprContext(estate)->ecxt_per_tuple_memory)

// #define ResetExprContext(econtext) \
// 	MemoryContextReset((econtext)->ecxt_per_tuple_memory)

// //end executor.h

// //execnodes.h
// typedef struct DomainConstraintState
// {
// 	NodeTag		type;
// 	DomainConstraintType constrainttype;		/* constraint type */
// 	char	   *name;			/* name of constraint (for error msgs) */
// 	ExprState  *check_expr;		/* for CHECK, a boolean expression */
// } DomainConstraintState;

// // typedef struct SeqScanState
// // {
// // 	ScanState	ss;				/* its first field is NodeTag */
// // 	Size		pscan_len;		/* size of parallel heap scan descriptor */
// // } SeqScanState;

// // typedef struct SampleScanState
// // {
// // 	ScanState	ss;
// // 	List	   *args;			/* expr states for TABLESAMPLE params */
// // 	ExprState  *repeatable;		/* expr state for REPEATABLE expr */
// // 	/* use struct pointer to avoid including tsmapi.h here */
// // 	struct TsmRoutine *tsmroutine;		/* descriptor for tablesample method */
// // 	void	   *tsm_state;		/* tablesample method can keep state here */
// // 	bool		use_bulkread;	/* use bulkread buffer access strategy? */
// // 	bool		use_pagemode;	/* use page-at-a-time visibility checking? */
// // 	bool		begun;			/* false means need to call BeginSampleScan */
// // 	uint32		seed;			/* random seed */
// // } SampleScanState;

// // typedef struct CustomScanState
// // {
// // 	ScanState	ss;
// // 	uint32		flags;			/* mask of CUSTOMPATH_* flags, see
// // 								 * nodes/extensible.h */
// // 	List	   *custom_ps;		/* list of child PlanState nodes, if any */
// // 	Size		pscan_len;		/* size of parallel coordination information */
// // 	const struct CustomExecMethods *methods;
// // } CustomScanState;

// // typedef struct GatherState
// // {
// // 	PlanState	ps;				/* its first field is NodeTag */
// // 	bool		initialized;
// // 	struct ParallelExecutorInfo *pei;
// // 	int			nreaders;
// // 	int			nextreader;
// // 	int			nworkers_launched;
// // 	struct TupleQueueReader **reader;
// // 	TupleTableSlot *funnel_slot;
// // 	bool		need_to_scan_locally;
// // } GatherState;

// //end execnodes.h

// //nodes/nodes.h
// typedef enum AggStrategy
// {
// 	AGG_PLAIN,					/* simple agg across all input rows */
// 	AGG_SORTED,					/* grouped agg, input must be sorted */
// 	AGG_HASHED					/* grouped agg, use internal hashtable */
// } AggStrategy;

// typedef enum AggSplit
// {
// 	/* Basic, non-split aggregation: */
// 	AGGSPLIT_SIMPLE = 0,
// 	/* Initial phase of partial aggregation, with serialization: */
// 	AGGSPLIT_INITIAL_SERIAL = AGGSPLITOP_SKIPFINAL | AGGSPLITOP_SERIALIZE,
// 	/* Final phase of partial aggregation, with deserialization: */
// 	AGGSPLIT_FINAL_DESERIAL = AGGSPLITOP_COMBINE | AGGSPLITOP_DESERIALIZE
// } AggSplit;

// typedef enum SetOpCmd
// {
// 	SETOPCMD_INTERSECT,
// 	SETOPCMD_INTERSECT_ALL,
// 	SETOPCMD_EXCEPT,
// 	SETOPCMD_EXCEPT_ALL
// } SetOpCmd;

// typedef enum SetOpStrategy
// {
// 	SETOP_SORTED,				/* input must be sorted */
// 	SETOP_HASHED				/* use internal hashtable */
// } SetOpStrategy;

// typedef enum OnConflictAction
// {
// 	ONCONFLICT_NONE,			/* No "ON CONFLICT" clause */
// 	ONCONFLICT_NOTHING,			/* ON CONFLICT ... DO NOTHING */
// 	ONCONFLICT_UPDATE			/* ON CONFLICT ... DO UPDATE */
// } OnConflictAction;

// //end nodes.h

// //nodes/parsenodes.h
// typedef enum RoleSpecType
// {
// 	ROLESPEC_CSTRING,			/* role name is stored as a C string */
// 	ROLESPEC_CURRENT_USER,		/* role spec is CURRENT_USER */
// 	ROLESPEC_SESSION_USER,		/* role spec is SESSION_USER */
// 	ROLESPEC_PUBLIC				/* role name is "public" */
// } RoleSpecType;

// typedef struct RoleSpec
// {
// 	NodeTag		type;
// 	RoleSpecType roletype;		/* Type of this rolespec */
// 	char	   *rolename;		/* filled only for ROLESPEC_CSTRING */
// 	int			location;		/* token location, or -1 if unknown */
// } RoleSpec;

// typedef struct MultiAssignRef
// {
// 	NodeTag		type;
// 	Node	   *source;			/* the row-valued expression */
// 	int			colno;			/* column number for this target (1..n) */
// 	int			ncolumns;		/* number of targets in the construct */
// } MultiAssignRef;

// typedef struct RangeTblFunction
// {
// 	NodeTag		type;

// 	Node	   *funcexpr;		/* expression tree for func call */
// 	int			funccolcount;	/* number of columns it contributes to RTE */
// 	/* These fields record the contents of a column definition list, if any: */
// 	List	   *funccolnames;	/* column names (list of String) */
// 	List	   *funccoltypes;	/* OID list of column type OIDs */
// 	List	   *funccoltypmods; /* integer list of column typmods */
// 	List	   *funccolcollations;		/* OID list of column collation OIDs */
// 	/* This is set during planning for use by the executor: */
// 	Bitmapset  *funcparams;		/* PARAM_EXEC Param IDs affecting this func */
// } RangeTblFunction;

// typedef enum WCOKind
// {
// 	WCO_VIEW_CHECK,				/* WCO on an auto-updatable view */
// 	WCO_RLS_INSERT_CHECK,		/* RLS INSERT WITH CHECK policy */
// 	WCO_RLS_UPDATE_CHECK,		/* RLS UPDATE WITH CHECK policy */
// 	WCO_RLS_CONFLICT_CHECK		/* RLS ON CONFLICT DO UPDATE USING policy */
// } WCOKind;

// typedef struct WithCheckOption
// {
// 	NodeTag		type;
// 	WCOKind		kind;			/* kind of WCO */
// 	char	   *relname;		/* name of relation that specified the WCO */
// 	char	   *polname;		/* name of RLS policy being checked */
// 	Node	   *qual;			/* constraint qual to check */
// 	bool		cascaded;		/* true for a cascaded WCO on a view */
// } WithCheckOption;

// typedef struct InferClause
// {
// 	NodeTag		type;
// 	List	   *indexElems;		/* IndexElems to infer unique index */
// 	Node	   *whereClause;	/* qualification (partial-index predicate) */
// 	char	   *conname;		/* Constraint name, or NULL if unnamed */
// 	int			location;		/* token location, or -1 if unknown */
// } InferClause;

// typedef struct OnConflictClause
// {
// 	NodeTag		type;
// 	OnConflictAction action;	/* DO NOTHING or UPDATE? */
// 	InferClause *infer;			/* Optional index inference clause */
// 	List	   *targetList;		/* the target list (of ResTarget) */
// 	Node	   *whereClause;	/* qualifications */
// 	int			location;		/* token location, or -1 if unknown */
// } OnConflictClause;

// typedef enum ObjectType
// {
// 	OBJECT_ACCESS_METHOD,
// 	OBJECT_AGGREGATE,
// 	OBJECT_AMOP,
// 	OBJECT_AMPROC,
// 	OBJECT_ATTRIBUTE,			/* type's attribute, when distinct from column */
// 	OBJECT_CAST,
// 	OBJECT_COLUMN,
// 	OBJECT_COLLATION,
// 	OBJECT_CONVERSION,
// 	OBJECT_DATABASE,
// 	OBJECT_DEFAULT,
// 	OBJECT_DEFACL,
// 	OBJECT_DOMAIN,
// 	OBJECT_DOMCONSTRAINT,
// 	OBJECT_EVENT_TRIGGER,
// 	OBJECT_EXTENSION,
// 	OBJECT_FDW,
// 	OBJECT_FOREIGN_SERVER,
// 	OBJECT_FOREIGN_TABLE,
// 	OBJECT_FUNCTION,
// 	OBJECT_INDEX,
// 	OBJECT_LANGUAGE,
// 	OBJECT_LARGEOBJECT,
// 	OBJECT_MATVIEW,
// 	OBJECT_OPCLASS,
// 	OBJECT_OPERATOR,
// 	OBJECT_OPFAMILY,
// 	OBJECT_POLICY,
// 	OBJECT_ROLE,
// 	OBJECT_RULE,
// 	OBJECT_SCHEMA,
// 	OBJECT_SEQUENCE,
// 	OBJECT_TABCONSTRAINT,
// 	OBJECT_TABLE,
// 	OBJECT_TABLESPACE,
// 	OBJECT_TRANSFORM,
// 	OBJECT_TRIGGER,
// 	OBJECT_TSCONFIGURATION,
// 	OBJECT_TSDICTIONARY,
// 	OBJECT_TSPARSER,
// 	OBJECT_TSTEMPLATE,
// 	OBJECT_TYPE,
// 	OBJECT_USER_MAPPING,
// 	OBJECT_VIEW
// } ObjectType;

// typedef struct AlterTableMoveAllStmt
// {
// 	NodeTag		type;
// 	char	   *orig_tablespacename;
// 	ObjectType	objtype;		/* Object type to move */
// 	List	   *roles;			/* List of roles to move objects of */
// 	char	   *new_tablespacename;
// 	bool		nowait;
// } AlterTableMoveAllStmt;

// typedef enum ImportForeignSchemaType
// {
// 	FDW_IMPORT_SCHEMA_ALL,		/* all relations wanted */
// 	FDW_IMPORT_SCHEMA_LIMIT_TO, /* include only listed tables in import */
// 	FDW_IMPORT_SCHEMA_EXCEPT	/* exclude listed tables from import */
// } ImportForeignSchemaType;

// typedef struct ImportForeignSchemaStmt
// {
// 	NodeTag		type;
// 	char	   *server_name;	/* FDW server name */
// 	char	   *remote_schema;	/* remote schema name to query */
// 	char	   *local_schema;	/* local schema to create objects in */
// 	ImportForeignSchemaType list_type;	/* type of table list */
// 	List	   *table_list;		/* List of RangeVar */
// 	List	   *options;		/* list of options to pass to FDW */
// } ImportForeignSchemaStmt;

// typedef struct CreatePolicyStmt
// {
// 	NodeTag		type;
// 	char	   *policy_name;	/* Policy's name */
// 	RangeVar   *table;			/* the table name the policy applies to */
// 	char	   *cmd_name;		/* the command name the policy applies to */
// 	List	   *roles;			/* the roles associated with the policy */
// 	Node	   *qual;			/* the policy's condition */
// 	Node	   *with_check;		/* the policy's WITH CHECK condition. */
// } CreatePolicyStmt;

// /*----------------------
//  *		Alter POLICY Statement
//  *----------------------
//  */
// typedef struct AlterPolicyStmt
// {
// 	NodeTag		type;
// 	char	   *policy_name;	/* Policy's name */
// 	RangeVar   *table;			/* the table name the policy applies to */
// 	List	   *roles;			/* the roles associated with the policy */
// 	Node	   *qual;			/* the policy's condition */
// 	Node	   *with_check;		/* the policy's WITH CHECK condition. */
// } AlterPolicyStmt;

// /*----------------------
//  *		Create ACCESS METHOD Statement
//  *----------------------
//  */
// typedef struct CreateAmStmt
// {
// 	NodeTag		type;
// 	char	   *amname;			/* access method name */
// 	List	   *handler_name;	/* handler function name */
// 	char		amtype;			/* type of access method */
// } CreateAmStmt;

// typedef struct CreateEventTrigStmt
// {
// 	NodeTag		type;
// 	char	   *trigname;		/* TRIGGER's name */
// 	char	   *eventname;		/* event's identifier */
// 	List	   *whenclause;		/* list of DefElems indicating filtering */
// 	List	   *funcname;		/* qual. name of function to call */
// } CreateEventTrigStmt;

// /* ----------------------
//  *		Alter EVENT TRIGGER Statement
//  * ----------------------
//  */
// typedef struct AlterEventTrigStmt
// {
// 	NodeTag		type;
// 	char	   *trigname;		/* TRIGGER's name */
// 	char		tgenabled;		/* trigger's firing configuration WRT
// 								 * session_replication_role */
// } AlterEventTrigStmt;

// typedef struct AlterObjectDependsStmt
// {
// 	NodeTag		type;
// 	ObjectType	objectType;		/* OBJECT_FUNCTION, OBJECT_TRIGGER, etc */
// 	RangeVar   *relation;		/* in case a table is involved */
// 	List	   *objname;		/* name of the object */
// 	List	   *objargs;		/* argument types, if applicable */
// 	Value	   *extname;		/* extension name */
// } AlterObjectDependsStmt;

// typedef struct AlterOperatorStmt
// {
// 	NodeTag		type;
// 	List	   *opername;		/* operator name */
// 	List	   *operargs;		/* operator's argument TypeNames */
// 	List	   *options;		/* List of DefElem nodes */
// } AlterOperatorStmt;

// typedef enum ViewCheckOption
// {
// 	NO_CHECK_OPTION,
// 	LOCAL_CHECK_OPTION,
// 	CASCADED_CHECK_OPTION
// } ViewCheckOption;

// typedef enum ReindexObjectType
// {
// 	REINDEX_OBJECT_INDEX,		/* index */
// 	REINDEX_OBJECT_TABLE,		/* table or materialized view */
// 	REINDEX_OBJECT_SCHEMA,		/* schema */
// 	REINDEX_OBJECT_SYSTEM,		/* system catalogs */
// 	REINDEX_OBJECT_DATABASE		/* database */
// } ReindexObjectType;

// typedef struct CreateTransformStmt
// {
// 	NodeTag		type;
// 	bool		replace;
// 	TypeName   *type_name;
// 	char	   *lang;
// 	FuncWithArgs *fromsql;
// 	FuncWithArgs *tosql;
// } CreateTransformStmt;

// typedef enum AlterTSConfigType
// {
// 	ALTER_TSCONFIG_ADD_MAPPING,
// 	ALTER_TSCONFIG_ALTER_MAPPING_FOR_TOKEN,
// 	ALTER_TSCONFIG_REPLACE_DICT,
// 	ALTER_TSCONFIG_REPLACE_DICT_FOR_TOKEN,
// 	ALTER_TSCONFIG_DROP_MAPPING
// } AlterTSConfigType;

// //end parsenodes.h

// //nodes/plannodes.h
// typedef struct Result
// {
// 	Plan		plan;
// 	Node	   *resconstantqual;
// } Result;

// typedef struct SampleScan
// {
// 	Scan		scan;
// 	/* use struct pointer to avoid including parsenodes.h here */
// 	struct TableSampleClause *tablesample;
// } SampleScan;

// struct CustomScanMethods;

// typedef struct CustomScan
// {
// 	Scan		scan;
// 	uint32		flags;			/* mask of CUSTOMPATH_* flags, see
// 								 * nodes/extensible.h */
// 	List	   *custom_plans;	/* list of Plan nodes, if any */
// 	List	   *custom_exprs;	/* expressions that custom code may evaluate */
// 	List	   *custom_private; /* private data for custom code */
// 	List	   *custom_scan_tlist;		/* optional tlist describing scan
// 										 * tuple */
// 	Bitmapset  *custom_relids;	/* RTIs generated by this scan */
// 	const struct CustomScanMethods *methods;
// } CustomScan;

// typedef struct Gather
// {
// 	Plan		plan;
// 	int			num_workers;
// 	bool		single_copy;
// 	bool		invisible;		/* suppress EXPLAIN display (for testing)? */
// } Gather;
// //end plannodes.h


// //executor/tuptable.h
// #define TTS_HAS_PHYSICAL_TUPLE(slot)  \
// 	((slot)->tts_tuple != NULL && (slot)->tts_tuple != &((slot)->tts_minhdr))


// //end tuptable.h

// //funcapi.h
// #define HeapTupleGetDatum(tuple)		HeapTupleHeaderGetDatum((tuple)->t_data)

// #define TupleGetDatum(_slot, _tuple)	HeapTupleGetDatum(_tuple)
// //end funcapi.h
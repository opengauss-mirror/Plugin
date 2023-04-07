//#include "tsdb.h"
#include "compat.h"

#include "commands/cluster.h"
//#include "gtm/utils/pqsignal.h"
#include "utils.h"
//#include "plan_agg_bookend.cpp"

// #include "/home/lhy/og/openGauss-server/src/common/port/qsort.cpp"
#include "access/skey.h"
#include "commands/extension.h"
#include "access/heapam.h"

#include "knl/knl_session.h"
#include "knl/knl_guc/knl_session_attr_sql.h"
#include "knl/knl_guc/knl_instance_attr_common.h"
#include "knl/knl_thread.h"

#include "utils/syscache.h"
#include "commands/copy.h"

#include "catalog/pg_type_fn.h"
#include "catalog/pg_ts_config.h"

#include "utils/dynamic_loader.h"
#include "utils/globalplancore.h"

#include "compat/tableam.h"
#include "utils/guc_tables.h"
#include "utils/array.h"
#include "commands/explain.h"
#include "compression/compression.h"
#include "compression/array.h"
#include "compression/dictionary.h"
#include "compression/gorilla.h"
#include "compression/deltadelta.h"

#include "catalog/pg_cast.h"
#include "catalog/pg_database.h"
#include "catalog/pg_conversion.h"
#include "catalog/pg_attrdef.h"
#include "catalog/pg_language.h"
#include "catalog/pg_amop.h"
#include "catalog/pg_amproc.h"
#include "catalog/pg_rewrite.h"

//#include "catalog/pg_policy.h"
#include "catalog/pg_ts_parser.h"
#include "catalog/pg_ts_dict.h"
#include "catalog/pg_ts_template.h"
#include "catalog/pg_opfamily.h"
#include "catalog/pg_operator.h"
#include "catalog/pg_largeobject_metadata.h"
#include "catalog/pg_foreign_server.h"

#include "storage/fsm_internals.h"
#include "storage/freespace.h"

#include "optimizer/cost.h"





static MultiXactId *OldestVisibleMXactId;

static SlruCtlData MultiXactOffsetCtlData;

static bool am_autovacuum_worker = false;

static IndexList *ILHead = NULL;

static bool all_timeouts_initialized = false;

static volatile sig_atomic_t got_deadlock_timeout;

static int	on_proc_exit_index,
			on_shmem_exit_index,
			before_shmem_exit_index;
			
static bool am_autovacuum_launcher = false;

static Vfd *VfdCache;
static int	nfile = 0;
static List *pendingReindexedIndexes = NIL;

static TransactionStateData TopTransactionStateData = {
	0,							/* transaction id */
	0,							/* subtransaction id */
	NULL,						/* savepoint name */
	0,							/* savepoint level */
	TRANS_DEFAULT,				/* transaction state */
	TBLOCK_DEFAULT,				/* transaction block state from the client
								 * perspective */
	0,							/* transaction nesting depth */
	0,							/* GUC context nesting depth */
	NULL,						/* cur transaction context */
	NULL,						/* cur transaction resource owner */
	NULL,						/* subcommitted child Xids */
	0,							/* # of subcommitted child Xids */
	0,							/* allocated size of childXids[] */
	InvalidOid,					/* previous CurrentUserId setting */
	0,							/* previous SecurityRestrictionContext */
	false,						/* entry-time xact r/o state */
	false,						/* startedInRecovery */
	false,						/* didLogXid */
	0,							/* parallelMode */
	NULL						/* link to parent state block */
};



#define CastRelationId	2605
#define ConversionRelationId  2607
#define Anum_pg_conversion_conname		1
#define Anum_pg_conversion_connamespace 2
#define Anum_pg_conversion_conowner		3
#define Anum_pg_conversion_conforencoding		4
#define Anum_pg_conversion_contoencoding		5
#define Anum_pg_conversion_conproc		6
#define Anum_pg_conversion_condefault	7
#define DatabaseRelationId	1262
#define LanguageRelationId	2612
#define Anum_pg_language_lanname		1
#define Anum_pg_language_lanowner		2
#define Anum_pg_language_lanispl		3
#define Anum_pg_language_lanpltrusted	4
#define Anum_pg_language_lanplcallfoid	5
#define Anum_pg_language_laninline		6
#define Anum_pg_language_lanvalidator	7
#define Anum_pg_language_lanacl			8
#define Anum_pg_database_datname		1
#define Anum_pg_database_datdba			2
#define Anum_pg_database_encoding		3
#define Anum_pg_database_datcollate		4
#define Anum_pg_database_datctype		5
#define Anum_pg_database_datistemplate	6
#define Anum_pg_database_datallowconn	7
#define Anum_pg_database_datconnlimit	8
#define Anum_pg_database_datlastsysoid	9
#define Anum_pg_database_datfrozenxid	10
#define Anum_pg_database_dattablespace	11
#define Anum_pg_database_compatibility	12
#define Anum_pg_database_datacl			13
#define Anum_pg_database_datfrozenxid64	14
#define Anum_pg_database_datminmxid		15
#define ExtensionRelationId 3079
#define Anum_pg_extension_extname			1
#define Anum_pg_extension_extowner			2
#define Anum_pg_extension_extnamespace		3
#define Anum_pg_extension_extrelocatable	4
#define Anum_pg_extension_extversion		5
#define Anum_pg_extension_extconfig			6
#define Anum_pg_extension_extcondition		7
#define ForeignDataWrapperRelationId	2328
#define Anum_pg_foreign_data_wrapper_fdwname         1
#define Anum_pg_foreign_data_wrapper_fdwowner        2
#define Anum_pg_foreign_data_wrapper_fdwhandler      3
#define Anum_pg_foreign_data_wrapper_fdwvalidator    4
#define Anum_pg_foreign_data_wrapper_fdwacl          5
#define Anum_pg_foreign_data_wrapper_fdwoptions      6
#define TemplateDbOid			1
#define AuthIdRelationId	1260
#define AttrDefaultRelationId  2604
#define AccessMethodOperatorRelationId	2602

#define PROPARALLEL_UNSAFE		'u' 
#define PROPARALLEL_SAFE		's' 

#ifdef MULTIXACT_DEBUG
#define debug_elog2(a,b) elog(a,b)
#define debug_elog3(a,b,c) elog(a,b,c)
#define debug_elog4(a,b,c,d) elog(a,b,c,d)
#define debug_elog5(a,b,c,d,e) elog(a,b,c,d,e)
#define debug_elog6(a,b,c,d,e,f) elog(a,b,c,d,e,f)
#else
#define debug_elog2(a,b)
#define debug_elog3(a,b,c)
#define debug_elog4(a,b,c,d)
#define debug_elog5(a,b,c,d,e)
#define debug_elog6(a,b,c,d,e,f)
#endif

#define MultiXactIdToOffsetPage(xid) \
	((xid) / (MultiXactOffset) MULTIXACT_OFFSETS_PER_PAGE)
#define MultiXactIdToOffsetEntry(xid) \
	((xid) % (MultiXactOffset) MULTIXACT_OFFSETS_PER_PAGE)

#define MultiXactOffsetCtl	(&MultiXactOffsetCtlData)


#define USEMEM(state,amt)	((state)->availMem -= (amt))
#define LACKMEM(state)		((state)->availMem < 0)
#define FREEMEM(state,amt)	((state)->availMem += (amt))




#define DATATYPE_PAIR(left, right, type1, type2)                                                   \
	((left == type1 && right == type2) || (left == type2 && right == type1))
static HTAB *custom_scan_methods = NULL; 
static int	NamedLWLockTrancheRequestsAllocated = 0;
static bool lock_named_request_allowed = true;



static EventTriggerCacheStateType EventTriggerCacheState = ETCS_NEEDS_REBUILD;
static HTAB *EventTriggerCache;
static MemoryContext EventTriggerCacheContext;
static TransactionState CurrentTransactionState = &TopTransactionStateData;
static MultiXactStateData *MultiXactState;
static ErrorData errordata[ERRORDATA_STACK_SIZE];

static MemoryContext vac_context = NULL;
static int	errordata_stack_depth = -1; 
static volatile OldSnapshotControlData *oldSnapshotControl;
static BufferAccessStrategy vac_strategy;
static TransactionId OldestXmin;
static TransactionId FreezeLimit;
static MultiXactId MultiXactCutoff;



static const char BinarySignature[12] = "PGCOPY\n\377\r\n\0";
static struct config_generic **guc_variables;
static int	num_guc_variables;
static const char *const map_old_guc_names[] = {
	"sort_mem", "work_mem",
	"vacuum_mem", "maintenance_work_mem",
	NULL
};

static const unit_conversion memory_unit_conversion_table[] =
{
	{"TB", GUC_UNIT_KB, 1024 * 1024 * 1024},
	{"GB", GUC_UNIT_KB, 1024 * 1024},
	{"MB", GUC_UNIT_KB, 1024},
	{"kB", GUC_UNIT_KB, 1},

	{"TB", GUC_UNIT_BLOCKS, (1024 * 1024 * 1024) / (BLCKSZ / 1024)},
	{"GB", GUC_UNIT_BLOCKS, (1024 * 1024) / (BLCKSZ / 1024)},
	{"MB", GUC_UNIT_BLOCKS, 1024 / (BLCKSZ / 1024)},
	{"kB", GUC_UNIT_BLOCKS, -(BLCKSZ / 1024)},

	{"TB", GUC_UNIT_XBLOCKS, (1024 * 1024 * 1024) / (XLOG_BLCKSZ / 1024)},
	{"GB", GUC_UNIT_XBLOCKS, (1024 * 1024) / (XLOG_BLCKSZ / 1024)},
	{"MB", GUC_UNIT_XBLOCKS, 1024 / (XLOG_BLCKSZ / 1024)},
	{"kB", GUC_UNIT_XBLOCKS, -(XLOG_BLCKSZ / 1024)},

	{"TB", GUC_UNIT_XSEGS, (1024 * 1024 * 1024) / (XLOG_SEG_SIZE / 1024)},
	{"GB", GUC_UNIT_XSEGS, (1024 * 1024) / (XLOG_SEG_SIZE / 1024)},
	{"MB", GUC_UNIT_XSEGS, -(XLOG_SEG_SIZE / (1024 * 1024))},
	{"kB", GUC_UNIT_XSEGS, -(XLOG_SEG_SIZE / 1024)},

	{""}						/* end of table marker */
}; 


static const unit_conversion time_unit_conversion_table[] =
{
	{"d", GUC_UNIT_MS, 1000 * 60 * 60 * 24},
	{"h", GUC_UNIT_MS, 1000 * 60 * 60},
	{"min", GUC_UNIT_MS, 1000 * 60},
	{"s", GUC_UNIT_MS, 1000},
	{"ms", GUC_UNIT_MS, 1},

	{"d", GUC_UNIT_S, 60 * 60 * 24},
	{"h", GUC_UNIT_S, 60 * 60},
	{"min", GUC_UNIT_S, 60},
	{"s", GUC_UNIT_S, 1},
	{"ms", GUC_UNIT_S, -1000},

	{"d", GUC_UNIT_MIN, 60 * 24},
	{"h", GUC_UNIT_MIN, 60},
	{"min", GUC_UNIT_MIN, 1},
	{"s", GUC_UNIT_MIN, -60},
	{"ms", GUC_UNIT_MIN, -1000 * 60},

	{""}						/* end of table marker */
}; 
static HTAB *ri_compare_cache = NULL;
static HTAB *ri_constraint_cache = NULL;
static int	ri_constraint_cache_valid_count = 0;
static HTAB *ri_query_cache = NULL;
static dlist_head ri_constraint_cache_valid_list;


#define ARRAY_ALGORITHM_DEFINITION                                                                 \
	{                                                                                              \
		.iterator_init_forward = tsl_array_decompression_iterator_from_datum_forward,              \
		.iterator_init_reverse = tsl_array_decompression_iterator_from_datum_reverse,              \
		.compressed_data_send = array_compressed_send,                                             \
		.compressed_data_recv = array_compressed_recv,                                             \
		.compressor_for_type = array_compressor_for_type,                                          \
		.compressed_data_storage = TOAST_STORAGE_EXTENDED,                                         \
	}
	
#define DICTIONARY_ALGORITHM_DEFINITION                                                            \
	{                                                                                              \
		.iterator_init_forward = tsl_dictionary_decompression_iterator_from_datum_forward,         \
		.iterator_init_reverse = tsl_dictionary_decompression_iterator_from_datum_reverse,         \
		.compressed_data_send = dictionary_compressed_send,                                        \
		.compressed_data_recv = dictionary_compressed_recv,                                        \
		.compressor_for_type = dictionary_compressor_for_type,                                     \
		.compressed_data_storage = TOAST_STORAGE_EXTENDED,                                         \
	}
	
#define GORILLA_ALGORITHM_DEFINITION                                                               \
	{                                                                                              \
		.iterator_init_forward = gorilla_decompression_iterator_from_datum_forward,                \
		.iterator_init_reverse = gorilla_decompression_iterator_from_datum_reverse,                \
		.compressed_data_send = gorilla_compressed_send,                                           \
		.compressed_data_recv = gorilla_compressed_recv,                                           \
		.compressor_for_type = gorilla_compressor_for_type,                                        \
		.compressed_data_storage = TOAST_STORAGE_EXTERNAL,                                         \
	}
	
#define DELTA_DELTA_ALGORITHM_DEFINITION                                                           \
	{                                                                                              \
		.iterator_init_forward = delta_delta_decompression_iterator_from_datum_forward,            \
		.iterator_init_reverse = delta_delta_decompression_iterator_from_datum_reverse,            \
		.compressed_data_send = deltadelta_compressed_send,                                        \
		.compressed_data_recv = deltadelta_compressed_recv,                                        \
		.compressor_for_type = delta_delta_compressor_for_type,                                    \
		.compressed_data_storage = TOAST_STORAGE_EXTERNAL,                                         \
	}

// static const CompressionAlgorithmDefinition definitions[_END_COMPRESSION_ALGORITHMS] = {
// 	[COMPRESSION_ALGORITHM_ARRAY] = ARRAY_ALGORITHM_DEFINITION,
// 	[COMPRESSION_ALGORITHM_DICTIONARY] = DICTIONARY_ALGORITHM_DEFINITION,
// 	[COMPRESSION_ALGORITHM_GORILLA] = GORILLA_ALGORITHM_DEFINITION,
// 	[COMPRESSION_ALGORITHM_DELTADELTA] = DELTA_DELTA_ALGORITHM_DEFINITION,
// }; 

static bool PqCommReadingMsg;	/* in the middle of reading a message */
static int	PqRecvPointer;		/* Next index to read a byte from PqRecvBuffer */
static int	PqRecvLength;		/* End of data available in PqRecvBuffer */
static char PqRecvBuffer[PQ_RECV_BUFFER_SIZE];

static int	size_guc_variables;
// PGDLLIMPORT struct Port *MyProcPort;
static Snapshot HistoricSnapshot = NULL;

static dlist_head dsm_segment_list = DLIST_STATIC_INIT(dsm_segment_list);

// struct AllocateDesc* allocatedDescs; 

static AfterTriggersData afterTriggers;

// static knl_t_log_context* og_knl_thread1;

// static knl_u_upgrade_context* og_knl_session1;
// static knl_u_catalog_context* og_knl_session2;
// static knl_u_inval_context * og_knl_session3;
// static knl_session_attr_sql* og_knl_session4;
// static knl_g_cost_context* og_knl_session5;


// static knl_t_utils_context* og_knl;
// static knl_t_postgres_context* og_knl2;
// static knl_instance_attr_common* og_knl3;
// static knl_session_attr_sql* og_knl4;
// static knl_u_storage_context* og_knl5;
// static knl_t_proc_context* og_knl6;
// static knl_u_proc_context* og_knl7;
// static knl_u_utils_context* og_knl8;
// static knl_misc_context* og_knl9;
// static knl_u_analyze_context* og_knl10;

// //extension2
// static knl_t_relopt_context *og_knl11;
// static knl_t_shemem_ptr_context* og_knl12;
// static knl_t_xact_context* og_knl13;
// static knl_session_attr_common* og_knl14;
// static knl_session_attr_storage* og_knl15;
// static knl_t_shemem_ptr_context* og_knl16; 

// static GPCPlainEnv* og_gpc;
 
static pqsigfunc pg_signal_array[PG_SIGNAL_COUNT];
static BackgroundWorkerArray *BackgroundWorkerData; 
// static EventTriggerQueryState *currentEventTriggerState = NULL; 
static const Pg_magic_struct magic_data = PG_MODULE_MAGIC_DATA;
static int	numAllocatedDescs = 0;
static int	maxAllocatedDescs = 0;

// static event_trigger_support_data event_trigger_support[] = {
// 	{"ACCESS METHOD", true},
// 	{"AGGREGATE", true},
// 	{"CAST", true},
// 	{"CONSTRAINT", true},
// 	{"COLLATION", true},
// 	{"CONVERSION", true},
// 	{"DATABASE", false},
// 	{"DOMAIN", true},
// 	{"EXTENSION", true},
// 	{"EVENT TRIGGER", false},
// 	{"FOREIGN DATA WRAPPER", true},
// 	{"FOREIGN TABLE", true},
// 	{"FUNCTION", true},
// 	{"INDEX", true},
// 	{"LANGUAGE", true},
// 	{"MATERIALIZED VIEW", true},
// 	{"OPERATOR", true},
// 	{"OPERATOR CLASS", true},
// 	{"OPERATOR FAMILY", true},
// 	{"POLICY", true},
// 	{"ROLE", false},
// 	{"RULE", true},
// 	{"SCHEMA", true},
// 	{"SEQUENCE", true},
// 	{"SERVER", true},
// 	{"TABLE", true},
// 	{"TABLESPACE", false},
// 	{"TRANSFORM", true},
// 	{"TRIGGER", true},
// 	{"TEXT SEARCH CONFIGURATION", true},
// 	{"TEXT SEARCH DICTIONARY", true},
// 	{"TEXT SEARCH PARSER", true},
// 	{"TEXT SEARCH TEMPLATE", true},
// 	{"TYPE", true},
// 	{"USER MAPPING", true},
// 	{"VIEW", true},
// 	{NULL, false}
// }; 
// static const ObjectPropertyType ObjectProperty[] =
// {
// 	{
// 		AccessMethodRelationId,
// 		AmOidIndexId,
// 		AMOID,
// 		AMNAME,
// 		Anum_pg_am_amname,
// 		InvalidAttrNumber,
// 		InvalidAttrNumber,
// 		InvalidAttrNumber,
// 		(AclObjectKind)-1,
// 		true
// 	},
// 	{
// 		CastRelationId,
// 		CastOidIndexId,
// 		-1,
// 		-1,
// 		InvalidAttrNumber,
// 		InvalidAttrNumber,
// 		InvalidAttrNumber,
// 		InvalidAttrNumber,
// 		(AclObjectKind)-1,
// 		false
// 	},
// 	{
// 		CollationRelationId,
// 		CollationOidIndexId,
// 		COLLOID,
// 		-1,						/* COLLNAMEENCNSP also takes encoding */
// 		Anum_pg_collation_collname,
// 		Anum_pg_collation_collnamespace,
// 		Anum_pg_collation_collowner,
// 		InvalidAttrNumber,
// 		ACL_KIND_COLLATION,
// 		true
// 	},
// 	{
// 		ConstraintRelationId,
// 		ConstraintOidIndexId,
// 		CONSTROID,
// 		-1,
// 		Anum_pg_constraint_conname,
// 		Anum_pg_constraint_connamespace,
// 		InvalidAttrNumber,
// 		InvalidAttrNumber,
// 		(AclObjectKind)-1,
// 		false
// 	},
// 	{
// 		ConversionRelationId,
// 		ConversionOidIndexId,
// 		CONVOID,
// 		CONNAMENSP,
// 		Anum_pg_conversion_conname,
// 		Anum_pg_conversion_connamespace,
// 		Anum_pg_conversion_conowner,
// 		InvalidAttrNumber,
// 		ACL_KIND_CONVERSION,
// 		true
// 	},
// 	{
// 		DatabaseRelationId,
// 		DatabaseOidIndexId,
// 		DATABASEOID,
// 		-1,
// 		Anum_pg_database_datname,
// 		InvalidAttrNumber,
// 		Anum_pg_database_datdba,
// 		Anum_pg_database_datacl,
// 		ACL_KIND_DATABASE,
// 		true
// 	},
// 	{
// 		ExtensionRelationId,
// 		ExtensionOidIndexId,
// 		-1,
// 		-1,
// 		Anum_pg_extension_extname,
// 		InvalidAttrNumber,		/* extension doesn't belong to extnamespace */
// 		Anum_pg_extension_extowner,
// 		InvalidAttrNumber,
// 		ACL_KIND_EXTENSION,
// 		true
// 	},
// 	{
// 		ForeignDataWrapperRelationId,
// 		ForeignDataWrapperOidIndexId,
// 		FOREIGNDATAWRAPPEROID,
// 		FOREIGNDATAWRAPPERNAME,
// 		Anum_pg_foreign_data_wrapper_fdwname,
// 		InvalidAttrNumber,
// 		Anum_pg_foreign_data_wrapper_fdwowner,
// 		Anum_pg_foreign_data_wrapper_fdwacl,
// 		ACL_KIND_FDW,
// 		true
// 	},
// 	{
// 		ForeignServerRelationId,
// 		ForeignServerOidIndexId,
// 		FOREIGNSERVEROID,
// 		FOREIGNSERVERNAME,
// 		Anum_pg_foreign_server_srvname,
// 		InvalidAttrNumber,
// 		Anum_pg_foreign_server_srvowner,
// 		Anum_pg_foreign_server_srvacl,
// 		ACL_KIND_FOREIGN_SERVER,
// 		true
// 	},
// 	{
// 		ProcedureRelationId,
// 		ProcedureOidIndexId,
// 		PROCOID,
// 		-1,						/* PROCNAMEARGSNSP also takes argument types */
// 		Anum_pg_proc_proname,
// 		Anum_pg_proc_pronamespace,
// 		Anum_pg_proc_proowner,
// 		Anum_pg_proc_proacl,
// 		ACL_KIND_PROC,
// 		false
// 	},
// 	{
// 		LanguageRelationId,
// 		LanguageOidIndexId,
// 		LANGOID,
// 		LANGNAME,
// 		Anum_pg_language_lanname,
// 		InvalidAttrNumber,
// 		Anum_pg_language_lanowner,
// 		Anum_pg_language_lanacl,
// 		ACL_KIND_LANGUAGE,
// 		true
// 	},
// 	{
// 		LargeObjectMetadataRelationId,
// 		LargeObjectMetadataOidIndexId,
// 		-1,
// 		-1,
// 		InvalidAttrNumber,
// 		InvalidAttrNumber,
// 		Anum_pg_largeobject_metadata_lomowner,
// 		Anum_pg_largeobject_metadata_lomacl,
// 		ACL_KIND_LARGEOBJECT,
// 		false
// 	},
// 	{
// 		OperatorClassRelationId,
// 		OpclassOidIndexId,
// 		CLAOID,
// 		-1,						/* CLAAMNAMENSP also takes opcmethod */
// 		Anum_pg_opclass_opcname,
// 		Anum_pg_opclass_opcnamespace,
// 		Anum_pg_opclass_opcowner,
// 		InvalidAttrNumber,
// 		ACL_KIND_OPCLASS,
// 		true
// 	},
// 	{
// 		OperatorRelationId,
// 		OperatorOidIndexId,
// 		OPEROID,
// 		-1,						/* OPERNAMENSP also takes left and right type */
// 		Anum_pg_operator_oprname,
// 		Anum_pg_operator_oprnamespace,
// 		Anum_pg_operator_oprowner,
// 		InvalidAttrNumber,
// 		ACL_KIND_OPER,
// 		false
// 	},
// 	{
// 		OperatorFamilyRelationId,
// 		OpfamilyOidIndexId,
// 		OPFAMILYOID,
// 		-1,						/* OPFAMILYAMNAMENSP also takes opfmethod */
// 		Anum_pg_opfamily_opfname,
// 		Anum_pg_opfamily_opfnamespace,
// 		Anum_pg_opfamily_opfowner,
// 		InvalidAttrNumber,
// 		ACL_KIND_OPFAMILY,
// 		true
// 	},
// 	{
// 		AuthIdRelationId,
// 		AuthIdOidIndexId,
// 		AUTHOID,
// 		AUTHNAME,
// 		Anum_pg_authid_rolname,
// 		InvalidAttrNumber,
// 		InvalidAttrNumber,
// 		InvalidAttrNumber,
// 		(AclObjectKind)-1,
// 		true
// 	},
// 	{
// 		RewriteRelationId,
// 		RewriteOidIndexId,
// 		-1,
// 		-1,
// 		Anum_pg_rewrite_rulename,
// 		InvalidAttrNumber,
// 		InvalidAttrNumber,
// 		InvalidAttrNumber,
// 		(AclObjectKind)-1,
// 		false
// 	},
// 	{
// 		NamespaceRelationId,
// 		NamespaceOidIndexId,
// 		NAMESPACEOID,
// 		NAMESPACENAME,
// 		Anum_pg_namespace_nspname,
// 		InvalidAttrNumber,
// 		Anum_pg_namespace_nspowner,
// 		Anum_pg_namespace_nspacl,
// 		ACL_KIND_NAMESPACE,
// 		true
// 	},
// 	{
// 		RelationRelationId,
// 		ClassOidIndexId,
// 		RELOID,
// 		RELNAMENSP,
// 		Anum_pg_class_relname,
// 		Anum_pg_class_relnamespace,
// 		Anum_pg_class_relowner,
// 		Anum_pg_class_relacl,
// 		ACL_KIND_CLASS,
// 		true
// 	},
// 	{
// 		TableSpaceRelationId,
// 		TablespaceOidIndexId,
// 		TABLESPACEOID,
// 		-1,
// 		Anum_pg_tablespace_spcname,
// 		InvalidAttrNumber,
// 		Anum_pg_tablespace_spcowner,
// 		Anum_pg_tablespace_spcacl,
// 		ACL_KIND_TABLESPACE,
// 		true
// 	},
// 	{
// 		TransformRelationId,
// 		TransformOidIndexId,
// 		TRFOID,
// 		InvalidAttrNumber
// 	},
// 	{
// 		TriggerRelationId,
// 		TriggerOidIndexId,
// 		-1,
// 		-1,
// 		Anum_pg_trigger_tgname,
// 		InvalidAttrNumber,
// 		InvalidAttrNumber,
// 		InvalidAttrNumber,
// 		(AclObjectKind)-1,
// 		false
// 	},
// 	{
// 		PolicyRelationId,
// 		PolicyOidIndexId,
// 		-1,
// 		-1,
// 		Anum_pg_policy_polname,
// 		InvalidAttrNumber,
// 		InvalidAttrNumber,
// 		InvalidAttrNumber,
// 		(AclObjectKind)-1,
// 		false
// 	},
// 	{
// 		EventTriggerRelationId,
// 		EventTriggerOidIndexId,
// 		EVENTTRIGGEROID,
// 		EVENTTRIGGERNAME,
// 		Anum_pg_event_trigger_evtname,
// 		InvalidAttrNumber,
// 		Anum_pg_event_trigger_evtowner,
// 		InvalidAttrNumber,
// 		(AclObjectKind)ACL_KIND_EVENT_TRIGGER,
// 		true
// 	},
// 	{
// 		TSConfigRelationId,
// 		TSConfigOidIndexId,
// 		TSCONFIGOID,
// 		TSCONFIGNAMENSP,
// 		Anum_pg_ts_config_cfgname,
// 		Anum_pg_ts_config_cfgnamespace,
// 		Anum_pg_ts_config_cfgowner,
// 		InvalidAttrNumber,
// 		ACL_KIND_TSCONFIGURATION,
// 		true
// 	},
// 	{
// 		TSDictionaryRelationId,
// 		TSDictionaryOidIndexId,
// 		TSDICTOID,
// 		TSDICTNAMENSP,
// 		Anum_pg_ts_dict_dictname,
// 		Anum_pg_ts_dict_dictnamespace,
// 		Anum_pg_ts_dict_dictowner,
// 		InvalidAttrNumber,
// 		ACL_KIND_TSDICTIONARY,
// 		true
// 	},
// 	{
// 		TSParserRelationId,
// 		TSParserOidIndexId,
// 		TSPARSEROID,
// 		TSPARSERNAMENSP,
// 		Anum_pg_ts_parser_prsname,
// 		Anum_pg_ts_parser_prsnamespace,
// 		InvalidAttrNumber,
// 		InvalidAttrNumber,
// 		(AclObjectKind)-1,
// 		true
// 	},
// 	{
// 		TSTemplateRelationId,
// 		TSTemplateOidIndexId,
// 		TSTEMPLATEOID,
// 		TSTEMPLATENAMENSP,
// 		Anum_pg_ts_template_tmplname,
// 		Anum_pg_ts_template_tmplnamespace,
// 		InvalidAttrNumber,
// 		InvalidAttrNumber,
// 		(AclObjectKind)-1,
// 		true,
// 	},
// 	{
// 		TypeRelationId,
// 		TypeOidIndexId,
// 		TYPEOID,
// 		TYPENAMENSP,
// 		Anum_pg_type_typname,
// 		Anum_pg_type_typnamespace,
// 		Anum_pg_type_typowner,
// 		Anum_pg_type_typacl,
// 		ACL_KIND_TYPE,
// 		true
// 	}
// };



#define RelationAllowsEarlyPruning(rel) \
( \
	 RelationNeedsWAL(rel) \
  && !IsCatalogRelation(rel) \
  && !RelationIsAccessibleInLogicalDecoding(rel) \
  && !RelationHasUnloggedIndex(rel) \
)
#define OLD_SNAPSHOT_TIME_MAP_ENTRIES (old_snapshot_threshold + OLD_SNAPSHOT_PADDING_ENTRIES)

#define MULTIXACT_OFFSETS_PER_PAGE (BLCKSZ / sizeof(MultiXactOffset))

#define MXOffsetToMemberPage(xid) ((xid) / (TransactionId) MULTIXACT_MEMBERS_PER_PAGE)

#define MXOffsetToMemberOffset(xid) \
	(MXOffsetToFlagsOffset(xid) + MULTIXACT_FLAGBYTES_PER_GROUP + \
	 ((xid) % MULTIXACT_MEMBERS_PER_MEMBERGROUP) * sizeof(TransactionId))

#define MXOffsetToFlagsOffset(xid) \
	((((xid) / (TransactionId) MULTIXACT_MEMBERS_PER_MEMBERGROUP) % \
	  (TransactionId) MULTIXACT_MEMBERGROUPS_PER_PAGE) * \
	 (TransactionId) MULTIXACT_MEMBERGROUP_SIZE)

#define MXOffsetToFlagsBitShift(xid) \
	(((xid) % (TransactionId) MULTIXACT_MEMBERS_PER_MEMBERGROUP) * \
	 MXACT_MEMBER_BITS_PER_XACT)

#define MXACT_MEMBER_XACT_BITMASK	((1 << MXACT_MEMBER_BITS_PER_XACT) - 1)

#define EarlyPruningEnabled(rel) (old_snapshot_threshold >= 0 && RelationAllowsEarlyPruning(rel))

#define REINDEX_REL_FORCE_INDEXES_PERMANENT 0x10

#define UINT32_ALIGN_MASK (sizeof(uint32) - 1)


#define mix(a,b,c) \
{ \
  a -= c;  a ^= rot(c, 4);	c += b; \
  b -= a;  b ^= rot(a, 6);	a += c; \
  c -= b;  c ^= rot(b, 8);	b += a; \
  a -= c;  a ^= rot(c,16);	c += b; \
  b -= a;  b ^= rot(a,19);	a += c; \
  c -= b;  c ^= rot(b, 4);	b += a; \
}

#ifdef XIDCACHE_DEBUG

/* counters for XidCache measurement */
static long xc_by_recent_xmin = 0;
static long xc_by_known_xact = 0;
static long xc_by_my_xact = 0;
static long xc_by_latest_xid = 0;
static long xc_by_main_xid = 0;
static long xc_by_child_xid = 0;
static long xc_by_known_assigned = 0;
static long xc_no_overflow = 0;
static long xc_slow_answer = 0;

#define xc_by_recent_xmin_inc()		(xc_by_recent_xmin++)
#define xc_by_known_xact_inc()		(xc_by_known_xact++)
#define xc_by_my_xact_inc()			(xc_by_my_xact++)
#define xc_by_latest_xid_inc()		(xc_by_latest_xid++)
#define xc_by_main_xid_inc()		(xc_by_main_xid++)
#define xc_by_child_xid_inc()		(xc_by_child_xid++)
#define xc_by_known_assigned_inc()	(xc_by_known_assigned++)
#define xc_no_overflow_inc()		(xc_no_overflow++)
#define xc_slow_answer_inc()		(xc_slow_answer++)

static void DisplayXidCache(void);
#else							/* !XIDCACHE_DEBUG */

#define xc_by_recent_xmin_inc()		((void) 0)
#define xc_by_known_xact_inc()		((void) 0)
#define xc_by_my_xact_inc()			((void) 0)
#define xc_by_latest_xid_inc()		((void) 0)
#define xc_by_main_xid_inc()		((void) 0)
#define xc_by_child_xid_inc()		((void) 0)
#define xc_by_known_assigned_inc()	((void) 0)
#define xc_no_overflow_inc()		((void) 0)
#define xc_slow_answer_inc()		((void) 0)
#endif   /* XIDCACHE_DEBUG */

#define TOTAL_MAX_CACHED_SUBXIDS \
	((PGPROC_MAX_CACHED_SUBXIDS + 1) * PROCARRAY_MAXPROCS)

#define PredicateLockHashPartitionLockByIndex(i) \
	(&t_thrd.shemem_ptr_cxt.mainLWLockArray[PREDICATELOCK_MANAGER_LWLOCK_OFFSET + (i)].lock)

#define AMTYPE_INDEX					'i'		/* index access method */

#define HEAP_MOVED (HEAP_MOVED_OFF | HEAP_MOVED_IN)

#define EarlyPruningEnabled(rel) (old_snapshot_threshold >= 0 && RelationAllowsEarlyPruning(rel))

#define REINDEXOPT_VERBOSE 1 << 0		/* print progress info */



#define PGSTAT_NUM_PROGRESS_PARAM	10
#define SxactIsDoomed(sxact) (((sxact)->flags & SXACT_FLAG_DOOMED) != 0)
#define SxactIsReadOnly(sxact) (((sxact)->flags & SXACT_FLAG_READ_ONLY) != 0)
#define SxactHasSummaryConflictIn(sxact) (((sxact)->flags & SXACT_FLAG_SUMMARY_CONFLICT_IN) != 0)
#define SxactHasSummaryConflictOut(sxact) (((sxact)->flags & SXACT_FLAG_SUMMARY_CONFLICT_OUT) != 0)
#define Anum_pg_class_relrowsecurity		24
#define Anum_pg_class_relforcerowsecurity	25
#define Anum_pg_class_relispopulated		26
#define SxactIsPrepared(sxact) (((sxact)->flags & SXACT_FLAG_PREPARED) != 0)
#define SxactIsReadOnly(sxact) (((sxact)->flags & SXACT_FLAG_READ_ONLY) != 0)
#define SxactIsCommitted(sxact) (((sxact)->flags & SXACT_FLAG_COMMITTED) != 0)
#define SxactHasConflictOut(sxact) (((sxact)->flags & SXACT_FLAG_CONFLICT_OUT) != 0)

#define MultiXactIdGetDatum(X) ((Datum) SET_4_BYTES((X)))
#define ERRCODE_SNAPSHOT_TOO_OLD MAKE_SQLSTATE('7','2','0','0','0')
#define HEAP_MOVED_OFF			0x4000
#define HEAP_MOVED_IN			0x8000	
#define HeapTupleHeaderGetXvac(tup) \
( \
	((tup)->t_infomask & HEAP_MOVED) ? \
		(tup)->t_choice.t_heap.t_field3.t_xvac \
	: \
		InvalidTransactionId \
)



static ProcArrayStruct *procArray;
static const FSMAddress FSM_ROOT_ADDRESS = {FSM_ROOT_LEVEL, 0};
static const uint8 number_of_ones_for_visible[256] = {
	0, 1, 0, 1, 1, 2, 1, 2, 0, 1, 0, 1, 1, 2, 1, 2,
	1, 2, 1, 2, 2, 3, 2, 3, 1, 2, 1, 2, 2, 3, 2, 3,
	0, 1, 0, 1, 1, 2, 1, 2, 0, 1, 0, 1, 1, 2, 1, 2,
	1, 2, 1, 2, 2, 3, 2, 3, 1, 2, 1, 2, 2, 3, 2, 3,
	1, 2, 1, 2, 2, 3, 2, 3, 1, 2, 1, 2, 2, 3, 2, 3,
	2, 3, 2, 3, 3, 4, 3, 4, 2, 3, 2, 3, 3, 4, 3, 4,
	1, 2, 1, 2, 2, 3, 2, 3, 1, 2, 1, 2, 2, 3, 2, 3,
	2, 3, 2, 3, 3, 4, 3, 4, 2, 3, 2, 3, 3, 4, 3, 4,
	0, 1, 0, 1, 1, 2, 1, 2, 0, 1, 0, 1, 1, 2, 1, 2,
	1, 2, 1, 2, 2, 3, 2, 3, 1, 2, 1, 2, 2, 3, 2, 3,
	0, 1, 0, 1, 1, 2, 1, 2, 0, 1, 0, 1, 1, 2, 1, 2,
	1, 2, 1, 2, 2, 3, 2, 3, 1, 2, 1, 2, 2, 3, 2, 3,
	1, 2, 1, 2, 2, 3, 2, 3, 1, 2, 1, 2, 2, 3, 2, 3,
	2, 3, 2, 3, 3, 4, 3, 4, 2, 3, 2, 3, 3, 4, 3, 4,
	1, 2, 1, 2, 2, 3, 2, 3, 1, 2, 1, 2, 2, 3, 2, 3,
	2, 3, 2, 3, 3, 4, 3, 4, 2, 3, 2, 3, 3, 4, 3, 4
};
static const uint8 number_of_ones_for_frozen[256] = {
	0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 2, 2, 1, 1, 2, 2,
	0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 2, 2, 1, 1, 2, 2,
	1, 1, 2, 2, 1, 1, 2, 2, 2, 2, 3, 3, 2, 2, 3, 3,
	1, 1, 2, 2, 1, 1, 2, 2, 2, 2, 3, 3, 2, 2, 3, 3,
	0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 2, 2, 1, 1, 2, 2,
	0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 2, 2, 1, 1, 2, 2,
	1, 1, 2, 2, 1, 1, 2, 2, 2, 2, 3, 3, 2, 2, 3, 3,
	1, 1, 2, 2, 1, 1, 2, 2, 2, 2, 3, 3, 2, 2, 3, 3,
	1, 1, 2, 2, 1, 1, 2, 2, 2, 2, 3, 3, 2, 2, 3, 3,
	1, 1, 2, 2, 1, 1, 2, 2, 2, 2, 3, 3, 2, 2, 3, 3,
	2, 2, 3, 3, 2, 2, 3, 3, 3, 3, 4, 4, 3, 3, 4, 4,
	2, 2, 3, 3, 2, 2, 3, 3, 3, 3, 4, 4, 3, 3, 4, 4,
	1, 1, 2, 2, 1, 1, 2, 2, 2, 2, 3, 3, 2, 2, 3, 3,
	1, 1, 2, 2, 1, 1, 2, 2, 2, 2, 3, 3, 2, 2, 3, 3,
	2, 2, 3, 3, 2, 2, 3, 3, 3, 3, 4, 4, 3, 3, 4, 4,
	2, 2, 3, 3, 2, 2, 3, 3, 3, 3, 4, 4, 3, 3, 4, 4
};







// static void
// FinishSortSupportFunction(Oid opfamily, Oid opcintype, SortSupport ssup)
// {
// 	Oid			sortSupportFunction;

// 	/* Look for a sort support function */
// 	sortSupportFunction = get_opfamily_proc(opfamily, opcintype, opcintype,
// 											BTSORTSUPPORT_PROC);
// 	if (OidIsValid(sortSupportFunction))
// 	{
// 		/*
// 		 * The sort support function can provide a comparator, but it can also
// 		 * choose not to so (e.g. based on the selected collation).
// 		 */
// 		OidFunctionCall1(sortSupportFunction, PointerGetDatum(ssup));
// 	}

// 	if (ssup->comparator == NULL)
// 	{
// 		Oid			sortFunction;

// 		sortFunction = get_opfamily_proc(opfamily, opcintype, opcintype,
// 										 BTORDER_PROC);

// 		if (!OidIsValid(sortFunction))
// 			elog(ERROR, "missing support function %d(%u,%u) in opfamily %u",
// 				 BTORDER_PROC, opcintype, opcintype, opfamily);

// 		/* We'll use a shim to call the old-style btree comparator */
// 		PrepareSortSupportComparisonShim(sortFunction, ssup);
// 	}
// } 

// static bool
// XidInMVCCSnapshot(TransactionId xid, Snapshot snapshot)
// {
// 	uint32		i;

// 	/*
// 	 * Make a quick range check to eliminate most XIDs without looking at the
// 	 * xip arrays.  Note that this is OK even if we convert a subxact XID to
// 	 * its parent below, because a subxact with XID < xmin has surely also got
// 	 * a parent with XID < xmin, while one with XID >= xmax must belong to a
// 	 * parent that was not yet committed at the time of this snapshot.
// 	 */

// 	/* Any xid < xmin is not in-progress */
// 	if (TransactionIdPrecedes(xid, snapshot->xmin))
// 		return false;
// 	/* Any xid >= xmax is in-progress */
// 	if (TransactionIdFollowsOrEquals(xid, snapshot->xmax))
// 		return true;

// 	/*
// 	 * Snapshot information is stored slightly differently in snapshots taken
// 	 * during recovery.
// 	 */
// 	if (!snapshot->takenDuringRecovery)
// 	{
// 		/*
// 		 * If the snapshot contains full subxact data, the fastest way to
// 		 * check things is just to compare the given XID against both subxact
// 		 * XIDs and top-level XIDs.  If the snapshot overflowed, we have to
// 		 * use pg_subtrans to convert a subxact XID to its parent XID, but
// 		 * then we need only look at top-level XIDs not subxacts.
// 		 */
// 		if (!snapshot->suboverflowed)
// 		{
// 			/* we have full data, so search subxip */
// 			int32		j;

// 			for (j = 0; j < snapshot->subxcnt; j++)
// 			{
// 				if (TransactionIdEquals(xid, snapshot->subxip[j]))
// 					return true;
// 			}

// 			/* not there, fall through to search xip[] */
// 		}
// 		else
// 		{
// 			/*
// 			 * Snapshot overflowed, so convert xid to top-level.  This is safe
// 			 * because we eliminated too-old XIDs above.
// 			 */
// 			xid = SubTransGetTopmostTransaction(xid);

// 			/*
// 			 * If xid was indeed a subxact, we might now have an xid < xmin,
// 			 * so recheck to avoid an array scan.  No point in rechecking
// 			 * xmax.
// 			 */
// 			if (TransactionIdPrecedes(xid, snapshot->xmin))
// 				return false;
// 		}

// 		for (i = 0; i < snapshot->xcnt; i++)
// 		{
// 			if (TransactionIdEquals(xid, snapshot->xip[i]))
// 				return true;
// 		}
// 	}
// 	else
// 	{
// 		int32		j;

// 		/*
// 		 * In recovery we store all xids in the subxact array because it is by
// 		 * far the bigger array, and we mostly don't know which xids are
// 		 * top-level and which are subxacts. The xip array is empty.
// 		 *
// 		 * We start by searching subtrans, if we overflowed.
// 		 */
// 		if (snapshot->suboverflowed)
// 		{
// 			/*
// 			 * Snapshot overflowed, so convert xid to top-level.  This is safe
// 			 * because we eliminated too-old XIDs above.
// 			 */
// 			xid = SubTransGetTopmostTransaction(xid);

// 			/*
// 			 * If xid was indeed a subxact, we might now have an xid < xmin,
// 			 * so recheck to avoid an array scan.  No point in rechecking
// 			 * xmax.
// 			 */
// 			if (TransactionIdPrecedes(xid, snapshot->xmin))
// 				return false;
// 		}

// 		/*
// 		 * We now have either a top-level xid higher than xmin or an
// 		 * indeterminate xid. We don't know whether it's top level or subxact
// 		 * but it doesn't matter. If it's present, the xid is visible.
// 		 */
// 		for (j = 0; j < snapshot->subxcnt; j++)
// 		{
// 			if (TransactionIdEquals(xid, snapshot->subxip[j]))
// 				return true;
// 		}
// 	}

// 	return false;
// }

// static TransactionId
// KnownAssignedXidsGetOldestXmin(void)
// {
// 	int			head,
// 				tail;
// 	int			i;

// 	/*
// 	 * Fetch head just once, since it may change while we loop.
// 	 */
// 	SpinLockAcquire(&procArray->known_assigned_xids_lck);
// 	tail = procArray->tailKnownAssignedXids;
// 	head = procArray->headKnownAssignedXids;
// 	SpinLockRelease(&procArray->known_assigned_xids_lck);

// 	for (i = tail; i < head; i++)
// 	{
// 		/* Skip any gaps in the array */
// 		if (KnownAssignedXidsValid[i])
// 			return KnownAssignedXids[i];
// 	}

// 	return InvalidTransactionId;
// }








// static void
// XactLockTableWaitErrorCb(void *arg)
// {
// 	XactLockTableWaitInfo *info = (XactLockTableWaitInfo *) arg;

// 	/*
// 	 * We would like to print schema name too, but that would require a
// 	 * syscache lookup.
// 	 */
// 	if (info->oper != XLTW_None &&
// 		ItemPointerIsValid(info->ctid) && RelationIsValid(info->rel))
// 	{
// 		const char *cxt;

// 		switch (info->oper)
// 		{
// 			case XLTW_Update:
// 				cxt = gettext_noop("while updating tuple (%u,%u) in relation \"%s\"");
// 				break;
// 			case XLTW_Delete:
// 				cxt = gettext_noop("while deleting tuple (%u,%u) in relation \"%s\"");
// 				break;
// 			case XLTW_Lock:
// 				cxt = gettext_noop("while locking tuple (%u,%u) in relation \"%s\"");
// 				break;
// 			case XLTW_LockUpdated:
// 				cxt = gettext_noop("while locking updated version (%u,%u) of tuple in relation \"%s\"");
// 				break;
// 			case XLTW_InsertIndex:
// 				cxt = gettext_noop("while inserting index tuple (%u,%u) in relation \"%s\"");
// 				break;
// 			case XLTW_InsertIndexUnique:
// 				cxt = gettext_noop("while checking uniqueness of tuple (%u,%u) in relation \"%s\"");
// 				break;
// 			case XLTW_FetchUpdated:
// 				cxt = gettext_noop("while rechecking updated tuple (%u,%u) in relation \"%s\"");
// 				break;
// 			case XLTW_RecheckExclusionConstr:
// 				cxt = gettext_noop("while checking exclusion constraint on tuple (%u,%u) in relation \"%s\"");
// 				break;

// 			default:
// 				return;
// 		}

// 		errcontext(cxt,
// 				   ItemPointerGetBlockNumber(info->ctid),
// 				   ItemPointerGetOffsetNumber(info->ctid),
// 				   RelationGetRelationName(info->rel));
// 	}
// }

// static bool
// KnownAssignedXidExists(TransactionId xid)
// {
// 	Assert(TransactionIdIsValid(xid));

// 	return KnownAssignedXidsSearch(xid, false);
// }
// static int
// KnownAssignedXidsGet(TransactionId *xarray, TransactionId xmax)
// {
// 	TransactionId xtmp = InvalidTransactionId;

// 	return KnownAssignedXidsGetAndSetXmin(xarray, &xtmp, xmax);
// }

// static void
// init_sequence(Oid relid, SeqTable *p_elm, Relation *p_rel)
// {
// 	SeqTable	elm;
// 	Relation	seqrel;
// 	bool		found;

// 	/* Find or create a hash table entry for this sequence */
// 	if (seqhashtab == NULL)
// 		create_seq_hashtable();

// 	elm = (SeqTable) hash_search(seqhashtab, &relid, HASH_ENTER, &found);

// 	/*
// 	 * Initialize the new hash table entry if it did not exist already.
// 	 *
// 	 * NOTE: seqtable entries are stored for the life of a backend (unless
// 	 * explicitly discarded with DISCARD). If the sequence itself is deleted
// 	 * then the entry becomes wasted memory, but it's small enough that this
// 	 * should not matter.
// 	 */
// 	if (!found)
// 	{
// 		/* relid already filled in */
// 		elm->filenode = InvalidOid;
// 		elm->lxid = InvalidLocalTransactionId;
// 		elm->last_valid = false;
// 		elm->last = elm->cached = elm->increment = 0;
// 	}

// 	/*
// 	 * Open the sequence relation.
// 	 */
// 	seqrel = open_share_lock(elm);

// 	if (seqrel->rd_rel->relkind != RELKIND_SEQUENCE)
// 		ereport(ERROR,
// 				(errcode(ERRCODE_WRONG_OBJECT_TYPE),
// 				 errmsg("\"%s\" is not a sequence",
// 						RelationGetRelationName(seqrel))));

// 	/*
// 	 * If the sequence has been transactionally replaced since we last saw it,
// 	 * discard any cached-but-unissued values.  We do not touch the currval()
// 	 * state, however.
// 	 */
// 	if (seqrel->rd_rel->relfilenode != elm->filenode)
// 	{
// 		elm->filenode = seqrel->rd_rel->relfilenode;
// 		elm->cached = elm->last;
// 	}

// 	/* Return results */
// 	*p_elm = elm;
// 	*p_rel = seqrel;
// }

// static void
// fill_seq_with_data(Relation rel, HeapTuple tuple)
// {
// 	Buffer		buf;
// 	Page		page;
// 	sequence_magic *sm;
// 	OffsetNumber offnum;

// 	/* Initialize first page of relation with special magic number */

// 	buf = ReadBuffer(rel, P_NEW);
// 	Assert(BufferGetBlockNumber(buf) == 0);

// 	page = BufferGetPage(buf);

// 	PageInit(page, BufferGetPageSize(buf), sizeof(sequence_magic));
// 	sm = (sequence_magic *) PageGetSpecialPointer(page);
// 	sm->magic = SEQ_MAGIC;

// 	/* Now insert sequence tuple */

// 	LockBuffer(buf, BUFFER_LOCK_EXCLUSIVE);

// 	/*
// 	 * Since VACUUM does not process sequences, we have to force the tuple to
// 	 * have xmin = FrozenTransactionId now.  Otherwise it would become
// 	 * invisible to SELECTs after 2G transactions.  It is okay to do this
// 	 * because if the current transaction aborts, no other xact will ever
// 	 * examine the sequence tuple anyway.
// 	 */
// 	HeapTupleHeaderSetXmin(tuple->t_data, FrozenTransactionId);
// 	HeapTupleHeaderSetXminFrozen(tuple->t_data);
// 	HeapTupleHeaderSetCmin(tuple->t_data, FirstCommandId);
// 	HeapTupleHeaderSetXmax(tuple->t_data, InvalidTransactionId);
// 	tuple->t_data->t_infomask |= HEAP_XMAX_INVALID;
// 	ItemPointerSet(&tuple->t_data->t_ctid, 0, FirstOffsetNumber);

// 	/* check the comment above nextval_internal()'s equivalent call. */
// 	if (RelationNeedsWAL(rel))
// 		GetTopTransactionId();

// 	START_CRIT_SECTION();

// 	MarkBufferDirty(buf);

// 	offnum = PageAddItem(page, (Item) tuple->t_data, tuple->t_len,
// 						 InvalidOffsetNumber, false, false);
// 	if (offnum != FirstOffsetNumber)
// 		elog(ERROR, "failed to add sequence tuple to page");

// 	/* XLOG stuff */
// 	if (RelationNeedsWAL(rel))
// 	{
// 		xl_seq_rec	xlrec;
// 		XLogRecPtr	recptr;

// 		XLogBeginInsert();
// 		XLogRegisterBuffer(0, buf, REGBUF_WILL_INIT);

// 		xlrec.node = rel->rd_node;

// 		XLogRegisterData((char *) &xlrec, sizeof(xl_seq_rec));
// 		XLogRegisterData((char *) tuple->t_data, tuple->t_len);

// 		recptr = XLogInsert(RM_SEQ_ID, XLOG_SEQ_LOG);

// 		PageSetLSN(page, recptr);
// 	}

// 	END_CRIT_SECTION();

// 	UnlockReleaseBuffer(buf);
// }

// static Node *
// transformExprRecurse(ParseState *pstate, Node *expr)
// {
// 	Node	   *result;

// 	if (expr == NULL)
// 		return NULL;

// 	/* Guard against stack overflow due to overly complex expressions */
// 	check_stack_depth();

// 	switch (nodeTag(expr))
// 	{
// 		case T_ColumnRef:
// 			result = transformColumnRef(pstate, (ColumnRef *) expr);
// 			break;

// 		case T_ParamRef:
// 			result = transformParamRef(pstate, (ParamRef *) expr);
// 			break;

// 		case T_A_Const:
// 			{
// 				A_Const    *con = (A_Const *) expr;
// 				Value	   *val = &con->val;

// 				result = (Node *) make_const(pstate, val, con->location);
// 				break;
// 			}

// 		case T_A_Indirection:
// 			{
// 				A_Indirection *ind = (A_Indirection *) expr;

// 				result = transformExprRecurse(pstate, ind->arg);
// 				result = transformIndirection(pstate, result,
// 											  ind->indirection);
// 				break;
// 			}

// 		case T_A_ArrayExpr:
// 			result = transformArrayExpr(pstate, (A_ArrayExpr *) expr,
// 										InvalidOid, InvalidOid, -1);
// 			break;

// 		case T_TypeCast:
// 			result = transformTypeCast(pstate, (TypeCast *) expr);
// 			break;

// 		case T_CollateClause:
// 			result = transformCollateClause(pstate, (CollateClause *) expr);
// 			break;

// 		case T_A_Expr:
// 			{
// 				A_Expr	   *a = (A_Expr *) expr;

// 				switch (a->kind)
// 				{
// 					case AEXPR_OP:
// 						result = transformAExprOp(pstate, a);
// 						break;
// 					case AEXPR_OP_ANY:
// 						result = transformAExprOpAny(pstate, a);
// 						break;
// 					case AEXPR_OP_ALL:
// 						result = transformAExprOpAll(pstate, a);
// 						break;
// 					case AEXPR_DISTINCT:
// 					case AEXPR_NOT_DISTINCT:
// 						result = transformAExprDistinct(pstate, a);
// 						break;
// 					case AEXPR_NULLIF:
// 						result = transformAExprNullIf(pstate, a);
// 						break;
// 					case AEXPR_OF:
// 						result = transformAExprOf(pstate, a);
// 						break;
// 					case AEXPR_IN:
// 						result = transformAExprIn(pstate, a);
// 						break;
// 					case AEXPR_LIKE:
// 					case AEXPR_ILIKE:
// 					case AEXPR_SIMILAR:
// 						/* we can transform these just like AEXPR_OP */
// 						result = transformAExprOp(pstate, a);
// 						break;
// 					case AEXPR_BETWEEN:
// 					case AEXPR_NOT_BETWEEN:
// 					case AEXPR_BETWEEN_SYM:
// 					case AEXPR_NOT_BETWEEN_SYM:
// 						result = transformAExprBetween(pstate, a);
// 						break;
// 					case AEXPR_PAREN:
// 						result = transformExprRecurse(pstate, a->lexpr);
// 						break;
// 					default:
// 						elog(ERROR, "unrecognized A_Expr kind: %d", a->kind);
// 						result = NULL;	/* keep compiler quiet */
// 						break;
// 				}
// 				break;
// 			}

// 		case T_BoolExpr:
// 			result = transformBoolExpr(pstate, (BoolExpr *) expr);
// 			break;

// 		case T_FuncCall:
// 			result = transformFuncCall(pstate, (FuncCall *) expr);
// 			break;

// 		case T_MultiAssignRef:
// 			result = transformMultiAssignRef(pstate, (MultiAssignRef *) expr);
// 			break;

// 		case T_GroupingFunc:
// 			result = transformGroupingFunc(pstate, (GroupingFunc *) expr);
// 			break;

// 		case T_NamedArgExpr:
// 			{
// 				NamedArgExpr *na = (NamedArgExpr *) expr;

// 				na->arg = (Expr *) transformExprRecurse(pstate, (Node *) na->arg);
// 				result = expr;
// 				break;
// 			}

// 		case T_SubLink:
// 			result = transformSubLink(pstate, (SubLink *) expr);
// 			break;

// 		case T_CaseExpr:
// 			result = transformCaseExpr(pstate, (CaseExpr *) expr);
// 			break;

// 		case T_RowExpr:
// 			result = transformRowExpr(pstate, (RowExpr *) expr);
// 			break;

// 		case T_CoalesceExpr:
// 			result = transformCoalesceExpr(pstate, (CoalesceExpr *) expr);
// 			break;

// 		case T_MinMaxExpr:
// 			result = transformMinMaxExpr(pstate, (MinMaxExpr *) expr);
// 			break;

// 		case T_XmlExpr:
// 			result = transformXmlExpr(pstate, (XmlExpr *) expr);
// 			break;

// 		case T_XmlSerialize:
// 			result = transformXmlSerialize(pstate, (XmlSerialize *) expr);
// 			break;

// 		case T_NullTest:
// 			{
// 				NullTest   *n = (NullTest *) expr;

// 				if (operator_precedence_warning)
// 					emit_precedence_warnings(pstate, PREC_GROUP_POSTFIX_IS, "IS",
// 											 (Node *) n->arg, NULL,
// 											 n->location);

// 				n->arg = (Expr *) transformExprRecurse(pstate, (Node *) n->arg);
// 				/* the argument can be any type, so don't coerce it */
// 				n->argisrow = type_is_rowtype(exprType((Node *) n->arg));
// 				result = expr;
// 				break;
// 			}

// 		case T_BooleanTest:
// 			result = transformBooleanTest(pstate, (BooleanTest *) expr);
// 			break;

// 		case T_CurrentOfExpr:
// 			result = transformCurrentOfExpr(pstate, (CurrentOfExpr *) expr);
// 			break;

// 			/*
// 			 * CaseTestExpr and SetToDefault don't require any processing;
// 			 * they are only injected into parse trees in fully-formed state.
// 			 *
// 			 * Ordinarily we should not see a Var here, but it is convenient
// 			 * for transformJoinUsingClause() to create untransformed operator
// 			 * trees containing already-transformed Vars.  The best
// 			 * alternative would be to deconstruct and reconstruct column
// 			 * references, which seems expensively pointless.  So allow it.
// 			 */
// 		case T_CaseTestExpr:
// 		case T_SetToDefault:
// 		case T_Var:
// 			{
// 				result = (Node *) expr;
// 				break;
// 			}

// 		default:
// 			/* should not reach here */
// 			elog(ERROR, "unrecognized node type: %d", (int) nodeTag(expr));
// 			result = NULL;		/* keep compiler quiet */
// 			break;
// 	}

// 	return result;
// }

// static void
// getConstraintTypeDescription(StringInfo buffer, Oid constroid)
// {
// 	Relation	constrRel;
// 	HeapTuple	constrTup;
// 	Form_pg_constraint constrForm;

// 	constrRel = heap_open(ConstraintRelationId, AccessShareLock);
// 	constrTup = get_catalog_object_by_oid(constrRel, constroid);
// 	if (!HeapTupleIsValid(constrTup))
// 		elog(ERROR, "cache lookup failed for constraint %u", constroid);

// 	constrForm = (Form_pg_constraint) GETSTRUCT(constrTup);

// 	if (OidIsValid(constrForm->conrelid))
// 		appendStringInfoString(buffer, "table constraint");
// 	else if (OidIsValid(constrForm->contypid))
// 		appendStringInfoString(buffer, "domain constraint");
// 	else
// 		elog(ERROR, "invalid constraint %u", HeapTupleGetOid(constrTup));

// 	heap_close(constrRel, AccessShareLock);
// }
// static void
// set_errdata_field(MemoryContextData *cxt, char **ptr, const char *str)
// {
// 	Assert(*ptr == NULL);
// 	*ptr = MemoryContextStrdup(cxt, str);
// }
// static void
// SetOldSnapshotThresholdTimestamp(int64 ts, TransactionId xlimit)
// {
// 	SpinLockAcquire(&oldSnapshotControl->mutex_threshold);
// 	oldSnapshotControl->threshold_timestamp = ts;
// 	oldSnapshotControl->threshold_xid = xlimit;
// 	SpinLockRelease(&oldSnapshotControl->mutex_threshold);
// }
// static int64
// AlignTimestampToMinuteBoundary(int64 ts)
// {
// 	int64		retval = ts + (USECS_PER_MINUTE - 1);

// 	return retval - (retval % USECS_PER_MINUTE);
// }
// static void
// SetOldSnapshotThresholdTimestamp(int64 ts, TransactionId xlimit)
// {
// 	SpinLockAcquire(&oldSnapshotControl->mutex_threshold);
// 	oldSnapshotControl->threshold_timestamp = ts;
// 	oldSnapshotControl->threshold_xid = xlimit;
// 	SpinLockRelease(&oldSnapshotControl->mutex_threshold);
// }
// static bool
// ReadMultiXactCounts(uint32 *multixacts, MultiXactOffset *members)
// {
// 	MultiXactOffset nextOffset;
// 	MultiXactOffset oldestOffset;
// 	MultiXactId oldestMultiXactId;
// 	MultiXactId nextMultiXactId;
// 	bool		oldestOffsetKnown;

// 	LWLockAcquire(MultiXactGenLock, LW_SHARED);
// 	nextOffset = MultiXactState->nextOffset;
// 	oldestMultiXactId = MultiXactState->oldestMultiXactId;
// 	nextMultiXactId = MultiXactState->nextMXact;
// 	oldestOffset = MultiXactState->oldestOffset;
// 	oldestOffsetKnown = MultiXactState->oldestOffsetKnown;
// 	LWLockRelease(MultiXactGenLock);

// 	if (!oldestOffsetKnown)
// 		return false;

// 	*members = nextOffset - oldestOffset;
// 	*multixacts = nextMultiXactId - oldestMultiXactId;
// 	return true;
// }

// static bool
// should_attempt_truncation(LVRelStats *vacrelstats)
// {
// 	BlockNumber possibly_freeable;

// 	possibly_freeable = vacrelstats->rel_pages - vacrelstats->nonempty_pages;
// 	if (possibly_freeable > 0 &&
// 		(possibly_freeable >= REL_TRUNCATE_MINIMUM ||
// 	  possibly_freeable >= vacrelstats->rel_pages / REL_TRUNCATE_FRACTION) &&
// 		old_snapshot_threshold < 0)
// 		return true;
// 	else
// 		return false;
// }

// static void
// getRelationIdentity(StringInfo buffer, Oid relid, List **objname)
// {
// 	HeapTuple	relTup;
// 	Form_pg_class relForm;
// 	char	   *schema;

// 	relTup = SearchSysCache1(RELOID,
// 							 ObjectIdGetDatum(relid));
// 	if (!HeapTupleIsValid(relTup))
// 		elog(ERROR, "cache lookup failed for relation %u", relid);
// 	relForm = (Form_pg_class) GETSTRUCT(relTup);

// 	schema = get_namespace_name_or_temp(relForm->relnamespace);
// 	appendStringInfoString(buffer,
// 						   quote_qualified_identifier(schema,
// 												 NameStr(relForm->relname)));
// 	if (objname)
// 		*objname = list_make2(schema, pstrdup(NameStr(relForm->relname)));

// 	ReleaseSysCache(relTup);
// }
// static void
// getOpFamilyIdentity(StringInfo buffer, Oid opfid, List **objname)
// {
// 	HeapTuple	opfTup;
// 	Form_pg_opfamily opfForm;
// 	HeapTuple	amTup;
// 	Form_pg_am	amForm;
// 	char	   *schema;

// 	opfTup = SearchSysCache1(OPFAMILYOID, ObjectIdGetDatum(opfid));
// 	if (!HeapTupleIsValid(opfTup))
// 		elog(ERROR, "cache lookup failed for opfamily %u", opfid);
// 	opfForm = (Form_pg_opfamily) GETSTRUCT(opfTup);

// 	amTup = SearchSysCache1(AMOID, ObjectIdGetDatum(opfForm->opfmethod));
// 	if (!HeapTupleIsValid(amTup))
// 		elog(ERROR, "cache lookup failed for access method %u",
// 			 opfForm->opfmethod);
// 	amForm = (Form_pg_am) GETSTRUCT(amTup);

// 	schema = get_namespace_name_or_temp(opfForm->opfnamespace);
// 	appendStringInfo(buffer, "%s USING %s",
// 					 quote_qualified_identifier(schema,
// 												NameStr(opfForm->opfname)),
// 					 NameStr(amForm->amname));

// 	if (objname)
// 		*objname = list_make3(pstrdup(NameStr(amForm->amname)),
// 							  pstrdup(schema),
// 							  pstrdup(NameStr(opfForm->opfname)));

// 	ReleaseSysCache(amTup);
// 	ReleaseSysCache(opfTup);
// }

static bool
has_parallel_hazard_checker(Oid func_id, void *context)
{
#ifdef OG30
return true;
#endif
	char		proparallel = 'u';//tsdb 原为 func_parallel(func_id)

	if (((has_parallel_hazard_arg *) context)->allow_restricted)
		return (proparallel == PROPARALLEL_UNSAFE);
	else
		return (proparallel != PROPARALLEL_SAFE);
} 

static bool
has_parallel_hazard_walker(Node *node, has_parallel_hazard_arg *context)
{
	if (node == NULL)
		return false;

	/* Check for hazardous functions in node itself */
	if (check_functions_in_node(node, has_parallel_hazard_checker,
								context))
		return true;

	/*
	 * It should be OK to treat MinMaxExpr as parallel-safe, since btree
	 * opclass support functions are generally parallel-safe.  XmlExpr is a
	 * bit more dubious but we can probably get away with it.  We err on the
	 * side of caution by treating CoerceToDomain as parallel-restricted.
	 * (Note: in principle that's wrong because a domain constraint could
	 * contain a parallel-unsafe function; but useful constraints probably
	 * never would have such, and assuming they do would cripple use of
	 * parallel query in the presence of domain types.)
	 */
	if (IsA(node, CoerceToDomain))
	{
		if (!context->allow_restricted)
			return true;
	}

	/*
	 * As a notational convenience for callers, look through RestrictInfo.
	 */
	else if (IsA(node, RestrictInfo))
	{
		RestrictInfo *rinfo = (RestrictInfo *) node;

		return has_parallel_hazard_walker((Node *) rinfo->clause, context);
	}

	/*
	 * Since we don't have the ability to push subplans down to workers at
	 * present, we treat subplan references as parallel-restricted.  We need
	 * not worry about examining their contents; if they are unsafe, we would
	 * have found that out while examining the whole tree before reduction of
	 * sublinks to subplans.  (Really we should not see SubLink during a
	 * not-allow_restricted scan, but if we do, return true.)
	 */
	else if (IsA(node, SubLink) ||
			 IsA(node, SubPlan) ||
			 IsA(node, AlternativeSubPlan))
	{
		if (!context->allow_restricted)
			return true;
	}

	/*
	 * We can't pass Params to workers at the moment either, so they are also
	 * parallel-restricted.
	 */
	else if (IsA(node, Param))
	{
		if (!context->allow_restricted)
			return true;
	}

	/*
	 * When we're first invoked on a completely unplanned tree, we must
	 * recurse into subqueries so to as to locate parallel-unsafe constructs
	 * anywhere in the tree.
	 */
	else if (IsA(node, Query))
	{
		Query	   *query = (Query *) node;

		/* SELECT FOR UPDATE/SHARE must be treated as unsafe */
		if (query->rowMarks != NULL)
			return true;

		/* Recurse into subselects */
		return query_tree_walker(query,
								(bool (*)()) has_parallel_hazard_walker,
								 context, 0);
	}

	/* Recurse to check arguments */
	return expression_tree_walker(node,
								 (bool (*)()) has_parallel_hazard_walker,
								  context);
}


// static SortTuple *
// med3_tuple(SortTuple *a, SortTuple *b, SortTuple *c, SortTupleComparator cmp_tuple, Tuplesortstate *state)
// {
// 	return cmp_tuple(a, b, state) < 0 ?
// 		(cmp_tuple(b, c, state) < 0 ? b :
// 			(cmp_tuple(a, c, state) < 0 ? c : a))
// 		: (cmp_tuple(b, c, state) > 0 ? b :
// 			(cmp_tuple(a, c, state) < 0 ? a : c));
// }

// static SortTuple *
// med3_ssup(SortTuple *a, SortTuple *b, SortTuple *c, SortSupport ssup)
// {
// 	return cmp_ssup(a, b, ssup) < 0 ?
// 		(cmp_ssup(b, c, ssup) < 0 ? b :
// 			(cmp_ssup(a, c, ssup) < 0 ? c : a))
// 		: (cmp_ssup(b, c, ssup) > 0 ? b :
// 			(cmp_ssup(a, c, ssup) < 0 ? a : c));
// }




// static void
// qsort_tuple(SortTuple *a, size_t n, SortTupleComparator cmp_tuple, Tuplesortstate *state)
// {
// 	SortTuple  *pa,
// 			   *pb,
// 			   *pc,
// 			   *pd,
// 			   *pl,
// 			   *pm,
// 			   *pn;
// 	size_t		d1,
// 				d2;
// 	int			r,
// 				presorted;

// loop:
// 	CHECK_FOR_INTERRUPTS();
// 	if (n < 7)
// 	{
// 		for (pm = a + 1; pm < a + n; pm++)
// 			for (pl = pm; pl > a && cmp_tuple(pl - 1, pl, state) > 0; pl--)
// 				swap(pl, pl-1);
// 		return;
// 	}
// 	presorted = 1;
// 	for (pm = a + 1; pm < a + n; pm++)
// 	{
// 		CHECK_FOR_INTERRUPTS();
// 		if (cmp_tuple(pm - 1, pm, state) > 0)
// 		{
// 			presorted = 0;
// 			break;
// 		}
// 	}
// 	if (presorted)
// 		return;
// 	pm = a + (n / 2);
// 	if (n > 7)
// 	{
// 		pl = a;
// 		pn = a + (n - 1);
// 		if (n > 40)
// 		{
// 			size_t		d = (n / 8);

// 			pl = med3_tuple(pl, pl + d, pl + 2 * d, cmp_tuple, state);
// 			pm = med3_tuple(pm - d, pm, pm + d, cmp_tuple, state);
// 			pn = med3_tuple(pn - 2 * d, pn - d, pn, cmp_tuple, state);
// 		}
// 		pm = med3_tuple(pl, pm, pn, cmp_tuple, state);
// 	}
// 	swap(a, pm);
// 	pa = pb = a + 1;
// 	pc = pd = a + (n - 1);
// 	for (;;)
// 	{
// 		while (pb <= pc && (r = cmp_tuple(pb, a, state)) <= 0)
// 		{
// 			if (r == 0)
// 			{
// 				swap(pa, pb);
// 				pa++;
// 			}
// 			pb++;
// 			CHECK_FOR_INTERRUPTS();
// 		}
// 		while (pb <= pc && (r = cmp_tuple(pc, a, state)) >= 0)
// 		{
// 			if (r == 0)
// 			{
// 				swap(pc, pd);
// 				pd--;
// 			}
// 			pc--;
// 			CHECK_FOR_INTERRUPTS();
// 		}
// 		if (pb > pc)
// 			break;
// 		swap(pb, pc);
// 		pb++;
// 		pc--;
// 	}
// 	pn = a + n;
// 	d1 = Min(pa - a, pb - pa);
// 	vecswap(a, pb - d1, d1);
// 	d1 = Min(pd - pc, pn - pd - 1);
// 	vecswap(pb, pn - d1, d1);
// 	d1 = pb - pa;
// 	d2 = pd - pc;
// 	if (d1 <= d2)
// 	{
// 		/* Recurse on left partition, then iterate on right partition */
// 		if (d1 > 1)
// 			qsort_tuple(a, d1, cmp_tuple, state);
// 		if (d2 > 1)
// 		{
// 			/* Iterate rather than recurse to save stack space */
// 			/* qsort_tuple(pn - d2, d2, cmp_tuple, state); */
// 			a = pn - d2;
// 			n = d2;
// 			goto loop;
// 		}
// 	}
// 	else
// 	{
// 		/* Recurse on right partition, then iterate on left partition */
// 		if (d2 > 1)
// 			qsort_tuple(pn - d2, d2, cmp_tuple, state);
// 		if (d1 > 1)
// 		{
// 			/* Iterate rather than recurse to save stack space */
// 			/* qsort_tuple(a, d1, cmp_tuple, state); */
// 			n = d1;
// 			goto loop;
// 		}
// 	}
// }

// static void
// qsort_ssup(SortTuple *a, size_t n, SortSupport ssup)
// {
// 	SortTuple  *pa,
// 			   *pb,
// 			   *pc,
// 			   *pd,
// 			   *pl,
// 			   *pm,
// 			   *pn;
// 	size_t		d1,
// 				d2;
// 	int			r,
// 				presorted;

// loop:
// 	CHECK_FOR_INTERRUPTS();
// 	if (n < 7)
// 	{
// 		for (pm = a + 1; pm < a + n; pm++)
// 			for (pl = pm; pl > a && cmp_ssup(pl - 1, pl, ssup) > 0; pl--)
// 				swap(pl, pl - 1);
// 		return;
// 	}
// 	presorted = 1;
// 	for (pm = a + 1; pm < a + n; pm++)
// 	{
// 		CHECK_FOR_INTERRUPTS();
// 		if (cmp_ssup(pm - 1, pm, ssup) > 0)
// 		{
// 			presorted = 0;
// 			break;
// 		}
// 	}
// 	if (presorted)
// 		return;
// 	pm = a + (n / 2);
// 	if (n > 7)
// 	{
// 		pl = a;
// 		pn = a + (n - 1);
// 		if (n > 40)
// 		{
// 			size_t		d = (n / 8);

// 			pl = med3_ssup(pl, pl + d, pl + 2 * d, ssup);
// 			pm = med3_ssup(pm - d, pm, pm + d, ssup);
// 			pn = med3_ssup(pn - 2 * d, pn - d, pn, ssup);
// 		}
// 		pm = med3_ssup(pl, pm, pn, ssup);
// 	}
// 	swap(a, pm);
// 	pa = pb = a + 1;
// 	pc = pd = a + (n - 1);
// 	for (;;)
// 	{
// 		while (pb <= pc && (r = cmp_ssup(pb, a, ssup)) <= 0)
// 		{
// 			if (r == 0)
// 			{
// 				swap(pa, pb);
// 				pa++;
// 			}
// 			pb++;
// 			CHECK_FOR_INTERRUPTS();
// 		}
// 		while (pb <= pc && (r = cmp_ssup(pc, a, ssup)) >= 0)
// 		{
// 			if (r == 0)
// 			{
// 				swap(pc, pd);
// 				pd--;
// 			}
// 			pc--;
// 			CHECK_FOR_INTERRUPTS();
// 		}
// 		if (pb > pc)
// 			break;
// 		swap(pb, pc);
// 		pb++;
// 		pc--;
// 	}
// 	pn = a + n;
// 	d1 = Min(pa - a, pb - pa);
// 	vecswap(a, pb - d1, d1);
// 	d1 = Min(pd - pc, pn - pd - 1);
// 	vecswap(pb, pn - d1, d1);
// 	d1 = pb - pa;
// 	d2 = pd - pc;
// 	if (d1 <= d2)
// 	{
// 		/* Recurse on left partition, then iterate on right partition */
// 		if (d1 > 1)
// 			qsort_ssup(a, d1, ssup);
// 		if (d2 > 1)
// 		{
// 			/* Iterate rather than recurse to save stack space */
// 			/* qsort_ssup(pn - d2, d2, ssup); */
// 			a = pn - d2;
// 			n = d2;
// 			goto loop;
// 		}
// 	}
// 	else
// 	{
// 		/* Recurse on right partition, then iterate on left partition */
// 		if (d2 > 1)
// 			qsort_ssup(pn - d2, d2, ssup);
// 		if (d1 > 1)
// 		{
// 			/* Iterate rather than recurse to save stack space */
// 			/* qsort_ssup(a, d1, ssup); */
// 			n = d1;
// 			goto loop;
// 		}
// 	}
// }


// static bool
// useselection(Tuplesortstate *state)
// {
// 	/*
// 	 * memtupsize might be noticeably higher than memtupcount here in atypical
// 	 * cases.  It seems slightly preferable to not allow recent outliers to
// 	 * impact this determination.  Note that caller's trace_sort output
// 	 * reports memtupcount instead.
// 	 */
// 	if (state->memtupsize <= replacement_sort_tuples)
// 		return true;

// 	return false;
// }

// static void
// tuplesort_heap_siftup(Tuplesortstate *state, bool checkIndex)
// {
// 	SortTuple  *memtuples = state->memtuples;
// 	SortTuple  *tuple;
// 	int			i,
// 				n;

// 	Assert(!checkIndex || state->currentRun == RUN_FIRST);
// 	if (--state->memtupcount <= 0)
// 		return;

// 	CHECK_FOR_INTERRUPTS();

// 	n = state->memtupcount;
// 	tuple = &memtuples[n];		/* tuple that must be reinserted */
// 	i = 0;						/* i is where the "hole" is */
// 	for (;;)
// 	{
// 		int			j = 2 * i + 1;

// 		if (j >= n)
// 			break;
// 		if (j + 1 < n &&
// 			HEAPCOMPARE(&memtuples[j], &memtuples[j + 1]) > 0)
// 			j++;
// 		if (HEAPCOMPARE(tuple, &memtuples[j]) <= 0)
// 			break;
// 		memtuples[i] = memtuples[j];
// 		i = j;
// 	}
// 	memtuples[i] = *tuple;
// }

// static void
// reversedirection(Tuplesortstate *state)
// {
// 	SortSupport sortKey = state->sortKeys;
// 	int			nkey;

// 	for (nkey = 0; nkey < state->nKeys; nkey++, sortKey++)
// 	{
// 		sortKey->ssup_reverse = !sortKey->ssup_reverse;
// 		sortKey->ssup_nulls_first = !sortKey->ssup_nulls_first;
// 	}
// }
// static void *
// mergebatchalloc(Tuplesortstate *state, int tapenum, Size tuplen)
// {
// 	Size		reserve_tuplen = MAXALIGN(tuplen);
// 	char	   *ret;

// 	/* Should overflow at most once before mergebatchone() call: */
// 	Assert(state->mergeoverflow[tapenum] == NULL);
// 	Assert(state->batchUsed);

// 	/* It should be possible to use precisely spacePerTape memory at once */
// 	if (state->mergecurrent[tapenum] + reserve_tuplen <=
// 		state->mergetuples[tapenum] + state->spacePerTape)
// 	{
// 		/*
// 		 * Usual case -- caller is returned pointer into its tape's buffer,
// 		 * and an offset from that point is recorded as where tape has
// 		 * consumed up to for current round of preloading.
// 		 */
// 		ret = state->mergetail[tapenum] = state->mergecurrent[tapenum];
// 		state->mergecurrent[tapenum] += reserve_tuplen;
// 	}
// 	else
// 	{
// 		/*
// 		 * Allocate memory, and record as tape's overflow allocation.  This
// 		 * will be detected quickly, in a similar fashion to a LACKMEM()
// 		 * condition, and should not happen again before a new round of
// 		 * preloading for caller's tape.  Note that we deliberately allocate
// 		 * this in the parent tuplesort context, to be on the safe side.
// 		 *
// 		 * Sometimes, this does not happen because merging runs out of slots
// 		 * before running out of memory.
// 		 */
// 		ret = state->mergeoverflow[tapenum] =(char*)
// 			MemoryContextAlloc(state->sortcontext, tuplen);
// 	}

// 	return ret;
// }

/*-------------------------------------------------------------------------
 *
 * objectaddress.c
 *	  functions for working with ObjectAddresses
 *
 * Portions Copyright (c) 1996-2016, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 *
 * IDENTIFICATION
 *	  src/backend/catalog/objectaddress.c
 *
 *-------------------------------------------------------------------------
 */

#include "postgres.h"

#include "access/htup.h"
#include "access/sysattr.h"
#include "catalog/catalog.h"
#include "catalog/indexing.h"
#include "catalog/objectaddress.h"
#include "catalog/pg_am.h"
#include "catalog/pg_amop.h"
#include "catalog/pg_amproc.h"
#include "catalog/pg_attrdef.h"
#include "catalog/pg_authid.h"
#include "catalog/pg_cast.h"
#include "catalog/pg_default_acl.h"
#include "catalog/pg_collation.h"
#include "catalog/pg_constraint.h"
#include "catalog/pg_conversion.h"
#include "catalog/pg_database.h"
#include "catalog/pg_extension.h"
#include "catalog/pg_foreign_data_wrapper.h"
#include "catalog/pg_foreign_server.h"
#include "catalog/pg_language.h"
#include "catalog/pg_largeobject.h"
#include "catalog/pg_largeobject_metadata.h"
#include "catalog/pg_namespace.h"
#include "catalog/pg_opclass.h"
#include "catalog/pg_opfamily.h"
#include "catalog/pg_operator.h"
#include "catalog/pg_proc.h"
#include "catalog/pg_rewrite.h"
#include "catalog/pg_tablespace.h"
#include "catalog/pg_trigger.h"
#include "catalog/pg_ts_config.h"
#include "catalog/pg_ts_dict.h"
#include "catalog/pg_ts_parser.h"
#include "catalog/pg_ts_template.h"
#include "catalog/pg_type.h"
#include "catalog/pg_user_mapping.h"
#include "commands/dbcommands.h"
#include "commands/defrem.h"
#include "commands/extension.h"
#include "commands/proclang.h"
#include "commands/tablespace.h"
#include "commands/trigger.h"
#include "foreign/foreign.h"
#include "funcapi.h"
#include "libpq/be-fsstubs.h"
#include "miscadmin.h"
#include "nodes/makefuncs.h"
#include "parser/parse_func.h"
#include "parser/parse_oper.h"
#include "parser/parse_type.h"
#include "rewrite/rewriteSupport.h"
#include "storage/lmgr.h"
#include "storage/sinval.h"
#include "utils/builtins.h"
#include "utils/fmgroids.h"
#include "utils/lsyscache.h"
#include "utils/memutils.h"
#include "utils/syscache.h"


#include "compat.h"
#include "tsdb_get_obj.h"


#define TransformOidIndexId 3574 
#define PolicyOidIndexId				3257 
#define EventTriggerOidIndexId	3468 

#define Anum_pg_policy_polname		1
/*
 * ObjectProperty
 *
 * This array provides a common part of system object structure; to help
 * consolidate routines to handle various kind of object classes.
 */
typedef struct
{
	Oid			class_oid;		/* oid of catalog */
	Oid			oid_index_oid;	/* oid of index on system oid column */
	int			oid_catcache_id;	/* id of catcache on system oid column	*/
	int			name_catcache_id;		/* id of catcache on (name,namespace),
										 * or (name) if the object does not
										 * live in a namespace */
	AttrNumber	attnum_name;	/* attnum of name field */
	AttrNumber	attnum_namespace;		/* attnum of namespace field */
	AttrNumber	attnum_owner;	/* attnum of owner field */
	AttrNumber	attnum_acl;		/* attnum of acl field */
	AclObjectKind acl_kind;		/* ACL_KIND_* of this object type */
	bool		is_nsp_name_unique;		/* can the nsp/name combination (or
										 * name alone, if there's no
										 * namespace) be considered a unique
										 * identifier for an object of this
										 * class? */
} ObjectPropertyType;

static const ObjectPropertyType ObjectProperty[] =
{
	{
		AccessMethodRelationId,
		AmOidIndexId,
		AMOID,
		AMNAME,
		Anum_pg_am_amname,
		InvalidAttrNumber,
		InvalidAttrNumber,
		InvalidAttrNumber,
		(AclObjectKind)-1,
		true
	},
	{
		CastRelationId,
		CastOidIndexId,
		-1,
		-1,
		InvalidAttrNumber,
		InvalidAttrNumber,
		InvalidAttrNumber,
		InvalidAttrNumber,
		(AclObjectKind)-1,
		false
	},
	{
		CollationRelationId,
		CollationOidIndexId,
		COLLOID,
		-1,						/* COLLNAMEENCNSP also takes encoding */
		Anum_pg_collation_collname,
		Anum_pg_collation_collnamespace,
		Anum_pg_collation_collowner,
		InvalidAttrNumber,
		ACL_KIND_COLLATION,
		true
	},
	{
		ConstraintRelationId,
		ConstraintOidIndexId,
		CONSTROID,
		-1,
		Anum_pg_constraint_conname,
		Anum_pg_constraint_connamespace,
		InvalidAttrNumber,
		InvalidAttrNumber,
		(AclObjectKind)-1,
		false
	},
	{
		ConversionRelationId,
		ConversionOidIndexId,
		CONVOID,
		CONNAMENSP,
		Anum_pg_conversion_conname,
		Anum_pg_conversion_connamespace,
		Anum_pg_conversion_conowner,
		InvalidAttrNumber,
		ACL_KIND_CONVERSION,
		true
	},
	{
		DatabaseRelationId,
		DatabaseOidIndexId,
		DATABASEOID,
		-1,
		Anum_pg_database_datname,
		InvalidAttrNumber,
		Anum_pg_database_datdba,
		Anum_pg_database_datacl,
		ACL_KIND_DATABASE,
		true
	},
	{
		ExtensionRelationId,
		ExtensionOidIndexId,
		-1,
		-1,
		Anum_pg_extension_extname,
		InvalidAttrNumber,		/* extension doesn't belong to extnamespace */
		Anum_pg_extension_extowner,
		InvalidAttrNumber,
		ACL_KIND_EXTENSION,
		true
	},
	{
		ForeignDataWrapperRelationId,
		ForeignDataWrapperOidIndexId,
		FOREIGNDATAWRAPPEROID,
		FOREIGNDATAWRAPPERNAME,
		Anum_pg_foreign_data_wrapper_fdwname,
		InvalidAttrNumber,
		Anum_pg_foreign_data_wrapper_fdwowner,
		Anum_pg_foreign_data_wrapper_fdwacl,
		ACL_KIND_FDW,
		true
	},
	{
		ForeignServerRelationId,
		ForeignServerOidIndexId,
		FOREIGNSERVEROID,
		FOREIGNSERVERNAME,
		Anum_pg_foreign_server_srvname,
		InvalidAttrNumber,
		Anum_pg_foreign_server_srvowner,
		Anum_pg_foreign_server_srvacl,
		ACL_KIND_FOREIGN_SERVER,
		true
	},
	{
		ProcedureRelationId,
		ProcedureOidIndexId,
		PROCOID,
		-1,						/* PROCNAMEARGSNSP also takes argument types */
		Anum_pg_proc_proname,
		Anum_pg_proc_pronamespace,
		Anum_pg_proc_proowner,
		Anum_pg_proc_proacl,
		ACL_KIND_PROC,
		false
	},
	{
		LanguageRelationId,
		LanguageOidIndexId,
		LANGOID,
		LANGNAME,
		Anum_pg_language_lanname,
		InvalidAttrNumber,
		Anum_pg_language_lanowner,
		Anum_pg_language_lanacl,
		ACL_KIND_LANGUAGE,
		true
	},
	{
		LargeObjectMetadataRelationId,
		LargeObjectMetadataOidIndexId,
		-1,
		-1,
		InvalidAttrNumber,
		InvalidAttrNumber,
		Anum_pg_largeobject_metadata_lomowner,
		Anum_pg_largeobject_metadata_lomacl,
		ACL_KIND_LARGEOBJECT,
		false
	},
	{
		OperatorClassRelationId,
		OpclassOidIndexId,
		CLAOID,
		-1,						/* CLAAMNAMENSP also takes opcmethod */
		Anum_pg_opclass_opcname,
		Anum_pg_opclass_opcnamespace,
		Anum_pg_opclass_opcowner,
		InvalidAttrNumber,
		ACL_KIND_OPCLASS,
		true
	},
	{
		OperatorRelationId,
		OperatorOidIndexId,
		OPEROID,
		-1,						/* OPERNAMENSP also takes left and right type */
		Anum_pg_operator_oprname,
		Anum_pg_operator_oprnamespace,
		Anum_pg_operator_oprowner,
		InvalidAttrNumber,
		ACL_KIND_OPER,
		false
	},
	{
		OperatorFamilyRelationId,
		OpfamilyOidIndexId,
		OPFAMILYOID,
		-1,						/* OPFAMILYAMNAMENSP also takes opfmethod */
		Anum_pg_opfamily_opfname,
		Anum_pg_opfamily_opfnamespace,
		Anum_pg_opfamily_opfowner,
		InvalidAttrNumber,
		ACL_KIND_OPFAMILY,
		true
	},
	{
		AuthIdRelationId,
		AuthIdOidIndexId,
		AUTHOID,
		AUTHNAME,
		Anum_pg_authid_rolname,
		InvalidAttrNumber,
		InvalidAttrNumber,
		InvalidAttrNumber,
		(AclObjectKind)-1,
		true
	},
	{
		RewriteRelationId,
		RewriteOidIndexId,
		-1,
		-1,
		Anum_pg_rewrite_rulename,
		InvalidAttrNumber,
		InvalidAttrNumber,
		InvalidAttrNumber,
		(AclObjectKind)-1,
		false
	},
	{
		NamespaceRelationId,
		NamespaceOidIndexId,
		NAMESPACEOID,
		NAMESPACENAME,
		Anum_pg_namespace_nspname,
		InvalidAttrNumber,
		Anum_pg_namespace_nspowner,
		Anum_pg_namespace_nspacl,
		ACL_KIND_NAMESPACE,
		true
	},
	{
		RelationRelationId,
		ClassOidIndexId,
		RELOID,
		RELNAMENSP,
		Anum_pg_class_relname,
		Anum_pg_class_relnamespace,
		Anum_pg_class_relowner,
		Anum_pg_class_relacl,
		ACL_KIND_CLASS,
		true
	},
	{
		TableSpaceRelationId,
		TablespaceOidIndexId,
		TABLESPACEOID,
		-1,
		Anum_pg_tablespace_spcname,
		InvalidAttrNumber,
		Anum_pg_tablespace_spcowner,
		Anum_pg_tablespace_spcacl,
		ACL_KIND_TABLESPACE,
		true
	},
	{
		TransformRelationId,
		TransformOidIndexId,
		51,
		InvalidAttrNumber
	},
	{
		TriggerRelationId,
		TriggerOidIndexId,
		-1,
		-1,
		Anum_pg_trigger_tgname,
		InvalidAttrNumber,
		InvalidAttrNumber,
		InvalidAttrNumber,
		(AclObjectKind)-1,
		false
	},
	{
		PolicyRelationId,
		PolicyOidIndexId,
		-1,
		-1,
		Anum_pg_policy_polname,
		InvalidAttrNumber,
		InvalidAttrNumber,
		InvalidAttrNumber,
		(AclObjectKind)-1,
		false
	},
	{
		EventTriggerRelationId,
		EventTriggerOidIndexId,
		EVENTTRIGGEROID,
		EVENTTRIGGERNAME,
		Anum_pg_event_trigger_evtname,
		InvalidAttrNumber,
		Anum_pg_event_trigger_evtowner,
		InvalidAttrNumber,
		(AclObjectKind)19,
		true
	},
	{
		TSConfigRelationId,
		TSConfigOidIndexId,
		TSCONFIGOID,
		TSCONFIGNAMENSP,
		Anum_pg_ts_config_cfgname,
		Anum_pg_ts_config_cfgnamespace,
		Anum_pg_ts_config_cfgowner,
		InvalidAttrNumber,
		ACL_KIND_TSCONFIGURATION,
		true
	},
	{
		TSDictionaryRelationId,
		TSDictionaryOidIndexId,
		TSDICTOID,
		TSDICTNAMENSP,
		Anum_pg_ts_dict_dictname,
		Anum_pg_ts_dict_dictnamespace,
		Anum_pg_ts_dict_dictowner,
		InvalidAttrNumber,
		ACL_KIND_TSDICTIONARY,
		true
	},
	{
		TSParserRelationId,
		TSParserOidIndexId,
		TSPARSEROID,
		TSPARSERNAMENSP,
		Anum_pg_ts_parser_prsname,
		Anum_pg_ts_parser_prsnamespace,
		InvalidAttrNumber,
		InvalidAttrNumber,
		(AclObjectKind)-1,
		true
	},
	{
		TSTemplateRelationId,
		TSTemplateOidIndexId,
		TSTEMPLATEOID,
		TSTEMPLATENAMENSP,
		Anum_pg_ts_template_tmplname,
		Anum_pg_ts_template_tmplnamespace,
		InvalidAttrNumber,
		InvalidAttrNumber,
		(AclObjectKind)-1,
		true,
	},
	{
		TypeRelationId,
		TypeOidIndexId,
		TYPEOID,
		TYPENAMENSP,
		Anum_pg_type_typname,
		Anum_pg_type_typnamespace,
		Anum_pg_type_typowner,
		Anum_pg_type_typacl,
		ACL_KIND_TYPE,
		true
	}
};

/*
 * This struct maps the string object types as returned by
 * getObjectTypeDescription into ObjType enum values.  Note that some enum
 * values can be obtained by different names, and that some string object types
 * do not have corresponding values in the output enum.  The user of this map
 * must be careful to test for invalid values being returned.
 *
 * To ease maintenance, this follows the order of getObjectTypeDescription.
 */
static const struct object_type_map
{
	const char *tm_name;
	ObjectType	tm_type;
}

			ObjectTypeMap[] =
{
	/* OCLASS_CLASS, all kinds of relations */
	{
		"table", OBJECT_TABLE
	},
	{
		"index", OBJECT_INDEX
	},
	{
		"sequence", OBJECT_SEQUENCE
	},
	{
		"toast table", (ObjectType)-1
	},							/* unmapped */
	{
		"view", OBJECT_VIEW
	},
	{
		"materialized view", OBJECT_MATVIEW
	},
	{
		"composite type",(ObjectType) -1
	},							/* unmapped */
	{
		"foreign table", OBJECT_FOREIGN_TABLE
	},
	{
		"table column", OBJECT_COLUMN
	},
	{
		"index column", (ObjectType)-1
	},							/* unmapped */
	{
		"sequence column",(ObjectType) -1
	},							/* unmapped */
	{
		"toast table column",(ObjectType) -1
	},							/* unmapped */
	{
		"view column", (ObjectType)-1
	},							/* unmapped */
	{
		"materialized view column", (ObjectType)-1
	},							/* unmapped */
	{
		"composite type column",(ObjectType) -1
	},							/* unmapped */
	{
		"foreign table column", OBJECT_COLUMN
	},
	/* OCLASS_PROC */
	{
		"aggregate", OBJECT_AGGREGATE
	},
	{
		"function", OBJECT_FUNCTION
	},
	/* OCLASS_TYPE */
	{
		"type", OBJECT_TYPE
	},
	/* OCLASS_CAST */
	{
		"cast", OBJECT_CAST
	},
	/* OCLASS_COLLATION */
	{
		"collation", OBJECT_COLLATION
	},
	/* OCLASS_CONSTRAINT */
	{
		"table constraint", OBJECT_TABCONSTRAINT
	},
	{
		"domain constraint",(ObjectType) 13
	},
	/* OCLASS_CONVERSION */
	{
		"conversion", OBJECT_CONVERSION
	},
	/* OCLASS_DEFAULT */
	{
		"default value", (ObjectType)10
	},
	/* OCLASS_LANGUAGE */
	{
		"language", OBJECT_LANGUAGE
	},
	/* OCLASS_LARGEOBJECT */
	{
		"large object", OBJECT_LARGEOBJECT
	},
	/* OCLASS_OPERATOR */
	{
		"operator", OBJECT_OPERATOR
	},
	/* OCLASS_OPCLASS */
	{
		"operator class", OBJECT_OPCLASS
	},
	/* OCLASS_OPFAMILY */
	{
		"operator family", OBJECT_OPFAMILY
	},
	/* OCLASS_AM */
	{
		"access method",(ObjectType) 0
	},
	/* OCLASS_AMOP */
	{
		"operator of access method",(ObjectType) 2
	},
	/* OCLASS_AMPROC */
	{
		"function of access method",(ObjectType) 3
	},
	/* OCLASS_REWRITE */
	{
		"rule", OBJECT_RULE
	},
	/* OCLASS_TRIGGER */
	{
		"trigger", OBJECT_TRIGGER
	},
	/* OCLASS_SCHEMA */
	{
		"schema", OBJECT_SCHEMA
	},
	/* OCLASS_TSPARSER */
	{
		"text search parser", OBJECT_TSPARSER
	},
	/* OCLASS_TSDICT */
	{
		"text search dictionary", OBJECT_TSDICTIONARY
	},
	/* OCLASS_TSTEMPLATE */
	{
		"text search template", OBJECT_TSTEMPLATE
	},
	/* OCLASS_TSCONFIG */
	{
		"text search configuration", OBJECT_TSCONFIGURATION
	},
	/* OCLASS_ROLE */
	{
		"role", OBJECT_ROLE
	},
	/* OCLASS_DATABASE */
	{
		"database", OBJECT_DATABASE
	},
	/* OCLASS_TBLSPACE */
	{
		"tablespace", OBJECT_TABLESPACE
	},
	/* OCLASS_FDW */
	{
		"foreign-data wrapper", OBJECT_FDW
	},
	/* OCLASS_FOREIGN_SERVER */
	{
		"server", OBJECT_FOREIGN_SERVER
	},
	/* OCLASS_USER_MAPPING */
	{
		"user mapping",(ObjectType) 42
	},
	/* OCLASS_DEFACL */
	{
		"default acl", (ObjectType) 11
	},
	/* OCLASS_EXTENSION */
	{
		"extension", OBJECT_EXTENSION
	},
	/* OCLASS_EVENT_TRIGGER */
	{
		"event trigger",(ObjectType)31
	},
	/* OCLASS_POLICY */
	{
		"policy",(ObjectType)32
	},
	/* OCLASS_TRANSFORM */
	{
		"transform",(ObjectType) 33
	}
};

const ObjectAddress InvalidObjectAddress =
{
	InvalidOid,
	InvalidOid,
	0
};

static char* format_type_internal(
    Oid type_oid, int32 typemod, bool typemod_given, bool allow_invalid, bool include_nspname = false);
static char* printTypmod(const char* typname, int32 typmod, Oid typmodout);
static char* psnprintf(size_t len, const char* fmt, ...)
    /* This lets gcc check the format string for consistency. */
    __attribute__((format(PG_PRINTF_ATTRIBUTE, 2, 3)));

static char *format_procedure_internal(Oid procedure_oid, bool force_qualify);
static char *format_operator_internal(Oid operator_oid, bool force_qualify);

static const ObjectPropertyType *get_object_property_data(Oid class_id);

static void getRelationTypeDescription(StringInfo buffer, Oid relid,
						   int32 objectSubId);
static void getProcedureTypeDescription(StringInfo buffer, Oid procid);
static void getConstraintTypeDescription(StringInfo buffer, Oid constroid);
static void getOpFamilyIdentity(StringInfo buffer, Oid opfid, List **objname);
static void getRelationIdentity(StringInfo buffer, Oid relid, List **objname);

Oid
get_object_oid_index(Oid class_id)
{
	const ObjectPropertyType *prop = get_object_property_data(class_id);

	return prop->oid_index_oid;
}

int
get_object_catcache_oid(Oid class_id)
{
	const ObjectPropertyType *prop = get_object_property_data(class_id);

	return prop->oid_catcache_id;
}

AttrNumber
get_object_attnum_name(Oid class_id)
{
	const ObjectPropertyType *prop = get_object_property_data(class_id);

	return prop->attnum_name;
}

AttrNumber
get_object_attnum_namespace(Oid class_id)
{
	const ObjectPropertyType *prop = get_object_property_data(class_id);

	return prop->attnum_namespace;
}

bool
get_object_namensp_unique(Oid class_id)
{
	const ObjectPropertyType *prop = get_object_property_data(class_id);

	return prop->is_nsp_name_unique;
}

static const ObjectPropertyType *
get_object_property_data(Oid class_id)
{
	static const ObjectPropertyType *prop_last = NULL;
	int			index;

	/*
	 * A shortcut to speed up multiple consecutive lookups of a particular
	 * object class.
	 */
	if (prop_last && prop_last->class_oid == class_id)
		return prop_last;

	for (index = 0; index < lengthof(ObjectProperty); index++)
	{
		if (ObjectProperty[index].class_oid == class_id)
		{
			prop_last = &ObjectProperty[index];
			return &ObjectProperty[index];
		}
	}

	ereport(ERROR,
			(errmsg_internal("unrecognized class ID: %u", class_id)));

	return NULL;				/* keep MSC compiler happy */
}

HeapTuple
get_catalog_object_by_oid(Relation catalog, Oid objectId)
{
	HeapTuple	tuple;
	Oid			classId = RelationGetRelid(catalog);
	int			oidCacheId = get_object_catcache_oid(classId);

	if (oidCacheId > 0)
	{
		tuple = SearchSysCacheCopy1(oidCacheId, ObjectIdGetDatum(objectId));
		if (!HeapTupleIsValid(tuple))	/* should not happen */
			return NULL;
	}
	else
	{
		Oid			oidIndexId = get_object_oid_index(classId);
		SysScanDesc scan;
		ScanKeyData skey;

		Assert(OidIsValid(oidIndexId));

		ScanKeyInit(&skey,
					ObjectIdAttributeNumber,
					BTEqualStrategyNumber, F_OIDEQ,
					ObjectIdGetDatum(objectId));

		scan = systable_beginscan(catalog, oidIndexId, true,
								  NULL, 1, &skey);
		tuple = systable_getnext(scan);
		if (!HeapTupleIsValid(tuple))
		{
			systable_endscan(scan);
			return NULL;
		}
		tuple = heap_copytuple(tuple);

		systable_endscan(scan);
	}

	return tuple;
}

/*
 * Return a palloc'ed string that describes the type of object that the
 * passed address is for.
 *
 * Keep ObjectTypeMap in sync with this.
 */
char *
getObjectTypeDescription(const ObjectAddress *object)
{
	StringInfoData buffer;

	initStringInfo(&buffer);

	switch (getObjectClass(object))
	{
		case OCLASS_CLASS:
			getRelationTypeDescription(&buffer, object->objectId,
									   object->objectSubId);
			break;

		case OCLASS_PROC:
			getProcedureTypeDescription(&buffer, object->objectId);
			break;

		case OCLASS_TYPE:
			appendStringInfoString(&buffer, "type");
			break;

		case OCLASS_CAST:
			appendStringInfoString(&buffer, "cast");
			break;

		case OCLASS_COLLATION:
			appendStringInfoString(&buffer, "collation");
			break;

		case OCLASS_CONSTRAINT:
			getConstraintTypeDescription(&buffer, object->objectId);
			break;

		case OCLASS_CONVERSION:
			appendStringInfoString(&buffer, "conversion");
			break;

		case OCLASS_DEFAULT:
			appendStringInfoString(&buffer, "default value");
			break;

		case OCLASS_LANGUAGE:
			appendStringInfoString(&buffer, "language");
			break;

		case OCLASS_LARGEOBJECT:
			appendStringInfoString(&buffer, "large object");
			break;

		case OCLASS_OPERATOR:
			appendStringInfoString(&buffer, "operator");
			break;

		case OCLASS_OPCLASS:
			appendStringInfoString(&buffer, "operator class");
			break;

		case OCLASS_OPFAMILY:
			appendStringInfoString(&buffer, "operator family");
			break;

		case OCLASS_AMOP:
			appendStringInfoString(&buffer, "operator of access method");
			break;

		case OCLASS_AMPROC:
			appendStringInfoString(&buffer, "function of access method");
			break;

		case OCLASS_REWRITE:
			appendStringInfoString(&buffer, "rule");
			break;

		case OCLASS_TRIGGER:
			appendStringInfoString(&buffer, "trigger");
			break;

		case OCLASS_SCHEMA:
			appendStringInfoString(&buffer, "schema");
			break;

		case OCLASS_TSPARSER:
			appendStringInfoString(&buffer, "text search parser");
			break;

		case OCLASS_TSDICT:
			appendStringInfoString(&buffer, "text search dictionary");
			break;

		case OCLASS_TSTEMPLATE:
			appendStringInfoString(&buffer, "text search template");
			break;

		case OCLASS_TSCONFIG:
			appendStringInfoString(&buffer, "text search configuration");
			break;

		case OCLASS_ROLE:
			appendStringInfoString(&buffer, "role");
			break;

		case OCLASS_DATABASE:
			appendStringInfoString(&buffer, "database");
			break;

		case OCLASS_TBLSPACE:
			appendStringInfoString(&buffer, "tablespace");
			break;

		case OCLASS_FDW:
			appendStringInfoString(&buffer, "foreign-data wrapper");
			break;

		case OCLASS_FOREIGN_SERVER:
			appendStringInfoString(&buffer, "server");
			break;

		case OCLASS_USER_MAPPING:
			appendStringInfoString(&buffer, "user mapping");
			break;

		case OCLASS_DEFACL:
			appendStringInfoString(&buffer, "default acl");
			break;

		case OCLASS_EXTENSION:
			appendStringInfoString(&buffer, "extension");
			break;

		case 31:
			appendStringInfoString(&buffer, "event trigger");
			break;

		case 32:
			appendStringInfoString(&buffer, "policy");
			break;

		case 33:
			appendStringInfoString(&buffer, "transform");
			break;

		case 29:
			appendStringInfoString(&buffer, "access method");
			break;

		default:
			appendStringInfo(&buffer, "unrecognized %u", object->classId);
			break;
	}

	return buffer.data;
}

static void
getRelationTypeDescription(StringInfo buffer, Oid relid, int32 objectSubId)
{
	HeapTuple	relTup;
	Form_pg_class relForm;

	relTup = SearchSysCache1(RELOID,
							 ObjectIdGetDatum(relid));
	if (!HeapTupleIsValid(relTup))
		elog(ERROR, "cache lookup failed for relation %u", relid);
	relForm = (Form_pg_class) GETSTRUCT(relTup);

	switch (relForm->relkind)
	{
		case RELKIND_RELATION:
			appendStringInfoString(buffer, "table");
			break;
		case RELKIND_INDEX:
			appendStringInfoString(buffer, "index");
			break;
		case RELKIND_SEQUENCE:
			appendStringInfoString(buffer, "sequence");
			break;
		case RELKIND_TOASTVALUE:
			appendStringInfoString(buffer, "toast table");
			break;
		case RELKIND_VIEW:
			appendStringInfoString(buffer, "view");
			break;
		case RELKIND_MATVIEW:
			appendStringInfoString(buffer, "materialized view");
			break;
		case RELKIND_COMPOSITE_TYPE:
			appendStringInfoString(buffer, "composite type");
			break;
		case RELKIND_FOREIGN_TABLE:
			appendStringInfoString(buffer, "foreign table");
			break;
		default:
			/* shouldn't get here */
			appendStringInfoString(buffer, "relation");
			break;
	}

	if (objectSubId != 0)
		appendStringInfoString(buffer, " column");

	ReleaseSysCache(relTup);
}

/*
 * subroutine for getObjectTypeDescription: describe a constraint type
 */
static void
getConstraintTypeDescription(StringInfo buffer, Oid constroid)
{
	Relation	constrRel;
	HeapTuple	constrTup;
	Form_pg_constraint constrForm;

	constrRel = heap_open(ConstraintRelationId, AccessShareLock);
	constrTup = get_catalog_object_by_oid(constrRel, constroid);
	if (!HeapTupleIsValid(constrTup))
		elog(ERROR, "cache lookup failed for constraint %u", constroid);

	constrForm = (Form_pg_constraint) GETSTRUCT(constrTup);

	if (OidIsValid(constrForm->conrelid))
		appendStringInfoString(buffer, "table constraint");
	else if (OidIsValid(constrForm->contypid))
		appendStringInfoString(buffer, "domain constraint");
	else
		elog(ERROR, "invalid constraint %u", HeapTupleGetOid(constrTup));

	heap_close(constrRel, AccessShareLock);
}

/*
 * subroutine for getObjectTypeDescription: describe a procedure type
 */
static void
getProcedureTypeDescription(StringInfo buffer, Oid procid)
{
	HeapTuple	procTup;
	Form_pg_proc procForm;

	procTup = SearchSysCache1(PROCOID,
							  ObjectIdGetDatum(procid));
	if (!HeapTupleIsValid(procTup))
		elog(ERROR, "cache lookup failed for procedure %u", procid);
	procForm = (Form_pg_proc) GETSTRUCT(procTup);

	if (procForm->proisagg)
		appendStringInfoString(buffer, "aggregate");
	else
		appendStringInfoString(buffer, "function");

	ReleaseSysCache(procTup);
}

/*
 * Obtain a given object's identity, as a palloc'ed string.
 *
 * This is for machine consumption, so it's not translated.  All elements are
 * schema-qualified when appropriate.
 */
char *
getObjectIdentity(const ObjectAddress *object)
{
	return getObjectIdentityParts(object, NULL, NULL);
}

/*
 * As above, but more detailed.
 *
 * There are two sets of return values: the identity itself as a palloc'd
 * string is returned.  objname and objargs, if not NULL, are output parameters
 * that receive lists of C-strings that are useful to give back to
 * get_object_address() to reconstruct the ObjectAddress.
 */
char *
getObjectIdentityParts(const ObjectAddress *object,
					   List **objname, List **objargs)
{
	StringInfoData buffer;

	initStringInfo(&buffer);

	/*
	 * Make sure that both objname and objargs were passed, or none was; and
	 * initialize them to empty lists.  For objname this is useless because it
	 * will be initialized in all cases inside the switch; but we do it anyway
	 * so that we can test below that no branch leaves it unset.
	 */
	Assert(PointerIsValid(objname) == PointerIsValid(objargs));
	if (objname)
	{
		*objname = NIL;
		*objargs = NIL;
	}

	switch (getObjectClass(object))
	{
		case OCLASS_CLASS:
			getRelationIdentity(&buffer, object->objectId, objname);
			if (object->objectSubId != 0)
			{
				char	   *attr;

				attr = get_relid_attribute_name(object->objectId,
												object->objectSubId);
				appendStringInfo(&buffer, ".%s", quote_identifier(attr));
				if (objname)
					*objname = lappend(*objname, attr);
			}
			break;

		case OCLASS_PROC:
			appendStringInfoString(&buffer,
							   format_procedure_qualified(object->objectId));
			if (objname)
				format_procedure_parts(object->objectId, objname, objargs);
			break;

		case OCLASS_TYPE:
			{
				char	   *typeout;

				typeout = format_type_be_qualified(object->objectId);
				appendStringInfoString(&buffer, typeout);
				if (objname)
					*objname = list_make1(typeout);
			}
			break;

		case OCLASS_CAST:
			{
				Relation	castRel;
				HeapTuple	tup;
				Form_pg_cast castForm;

				castRel = heap_open(CastRelationId, AccessShareLock);

				tup = get_catalog_object_by_oid(castRel, object->objectId);

				if (!HeapTupleIsValid(tup))
					elog(ERROR, "could not find tuple for cast %u",
						 object->objectId);

				castForm = (Form_pg_cast) GETSTRUCT(tup);

				appendStringInfo(&buffer, "(%s AS %s)",
							  format_type_be_qualified(castForm->castsource),
							 format_type_be_qualified(castForm->casttarget));

				if (objname)
				{
					*objname = list_make1(format_type_be_qualified(castForm->castsource));
					*objargs = list_make1(format_type_be_qualified(castForm->casttarget));
				}

				heap_close(castRel, AccessShareLock);
				break;
			}

		case OCLASS_COLLATION:
			{
				HeapTuple	collTup;
				Form_pg_collation coll;
				char	   *schema;

				collTup = SearchSysCache1(COLLOID,
										  ObjectIdGetDatum(object->objectId));
				if (!HeapTupleIsValid(collTup))
					elog(ERROR, "cache lookup failed for collation %u",
						 object->objectId);
				coll = (Form_pg_collation) GETSTRUCT(collTup);
				schema = get_namespace_name_or_temp(coll->collnamespace);
				appendStringInfoString(&buffer,
									   quote_qualified_identifier(schema,
												   NameStr(coll->collname)));
				if (objname)
					*objname = list_make2(schema,
										  pstrdup(NameStr(coll->collname)));
				ReleaseSysCache(collTup);
				break;
			}

		case OCLASS_CONSTRAINT:
			{
				HeapTuple	conTup;
				Form_pg_constraint con;

				conTup = SearchSysCache1(CONSTROID,
										 ObjectIdGetDatum(object->objectId));
				if (!HeapTupleIsValid(conTup))
					elog(ERROR, "cache lookup failed for constraint %u",
						 object->objectId);
				con = (Form_pg_constraint) GETSTRUCT(conTup);

				if (OidIsValid(con->conrelid))
				{
					appendStringInfo(&buffer, "%s on ",
									 quote_identifier(NameStr(con->conname)));
					getRelationIdentity(&buffer, con->conrelid, objname);
					if (objname)
						*objname = lappend(*objname, pstrdup(NameStr(con->conname)));
				}
				else
				{
					ObjectAddress domain;

					Assert(OidIsValid(con->contypid));
					domain.classId = TypeRelationId;
					domain.objectId = con->contypid;
					domain.objectSubId = 0;

					appendStringInfo(&buffer, "%s on %s",
									 quote_identifier(NameStr(con->conname)),
						  getObjectIdentityParts(&domain, objname, objargs));

					if (objname)
						*objargs = lappend(*objargs, pstrdup(NameStr(con->conname)));
				}

				ReleaseSysCache(conTup);
				break;
			}

		case OCLASS_CONVERSION:
			{
				HeapTuple	conTup;
				Form_pg_conversion conForm;
				char	   *schema;

				conTup = SearchSysCache1(CONVOID,
										 ObjectIdGetDatum(object->objectId));
				if (!HeapTupleIsValid(conTup))
					elog(ERROR, "cache lookup failed for conversion %u",
						 object->objectId);
				conForm = (Form_pg_conversion) GETSTRUCT(conTup);
				schema = get_namespace_name_or_temp(conForm->connamespace);
				appendStringInfoString(&buffer,
									   quote_qualified_identifier(schema,
												 NameStr(conForm->conname)));
				if (objname)
					*objname = list_make2(schema,
										  pstrdup(NameStr(conForm->conname)));
				ReleaseSysCache(conTup);
				break;
			}

		case OCLASS_DEFAULT:
			{
				Relation	attrdefDesc;
				ScanKeyData skey[1];
				SysScanDesc adscan;

				HeapTuple	tup;
				Form_pg_attrdef attrdef;
				ObjectAddress colobject;

				attrdefDesc = heap_open(AttrDefaultRelationId, AccessShareLock);

				ScanKeyInit(&skey[0],
							ObjectIdAttributeNumber,
							BTEqualStrategyNumber, F_OIDEQ,
							ObjectIdGetDatum(object->objectId));

				adscan = systable_beginscan(attrdefDesc, AttrDefaultOidIndexId,
											true, NULL, 1, skey);

				tup = systable_getnext(adscan);

				if (!HeapTupleIsValid(tup))
					elog(ERROR, "could not find tuple for attrdef %u",
						 object->objectId);

				attrdef = (Form_pg_attrdef) GETSTRUCT(tup);

				colobject.classId = RelationRelationId;
				colobject.objectId = attrdef->adrelid;
				colobject.objectSubId = attrdef->adnum;

				appendStringInfo(&buffer, "for %s",
								 getObjectIdentityParts(&colobject,
														objname, objargs));

				systable_endscan(adscan);
				heap_close(attrdefDesc, AccessShareLock);
				break;
			}

		case OCLASS_LANGUAGE:
			{
				HeapTuple	langTup;
				Form_pg_language langForm;

				langTup = SearchSysCache1(LANGOID,
										  ObjectIdGetDatum(object->objectId));
				if (!HeapTupleIsValid(langTup))
					elog(ERROR, "cache lookup failed for language %u",
						 object->objectId);
				langForm = (Form_pg_language) GETSTRUCT(langTup);
				appendStringInfoString(&buffer,
							   quote_identifier(NameStr(langForm->lanname)));
				if (objname)
					*objname = list_make1(pstrdup(NameStr(langForm->lanname)));
				ReleaseSysCache(langTup);
				break;
			}
		case OCLASS_LARGEOBJECT:
			appendStringInfo(&buffer, "%u",
							 object->objectId);
			if (objname)
				*objname = list_make1(psprintf("%u", object->objectId));
			break;

		case OCLASS_OPERATOR:
			appendStringInfoString(&buffer,
								format_operator_qualified(object->objectId));
			if (objname)
				format_operator_parts(object->objectId, objname, objargs);
			break;

		case OCLASS_OPCLASS:
			{
				HeapTuple	opcTup;
				Form_pg_opclass opcForm;
				HeapTuple	amTup;
				Form_pg_am	amForm;
				char	   *schema;

				opcTup = SearchSysCache1(CLAOID,
										 ObjectIdGetDatum(object->objectId));
				if (!HeapTupleIsValid(opcTup))
					elog(ERROR, "cache lookup failed for opclass %u",
						 object->objectId);
				opcForm = (Form_pg_opclass) GETSTRUCT(opcTup);
				schema = get_namespace_name_or_temp(opcForm->opcnamespace);

				amTup = SearchSysCache1(AMOID,
										ObjectIdGetDatum(opcForm->opcmethod));
				if (!HeapTupleIsValid(amTup))
					elog(ERROR, "cache lookup failed for access method %u",
						 opcForm->opcmethod);
				amForm = (Form_pg_am) GETSTRUCT(amTup);

				appendStringInfo(&buffer, "%s USING %s",
								 quote_qualified_identifier(schema,
												  NameStr(opcForm->opcname)),
								 quote_identifier(NameStr(amForm->amname)));
				if (objname)
					*objname = list_make3(pstrdup(NameStr(amForm->amname)),
										  schema,
										  pstrdup(NameStr(opcForm->opcname)));

				ReleaseSysCache(amTup);
				ReleaseSysCache(opcTup);
				break;
			}

		case OCLASS_OPFAMILY:
			getOpFamilyIdentity(&buffer, object->objectId, objname);
			break;

		case OCLASS_AMOP:
			{
				Relation	amopDesc;
				HeapTuple	tup;
				ScanKeyData skey[1];
				SysScanDesc amscan;
				Form_pg_amop amopForm;
				StringInfoData opfam;
				char	   *ltype;
				char	   *rtype;

				amopDesc = heap_open(AccessMethodOperatorRelationId,
									 AccessShareLock);

				ScanKeyInit(&skey[0],
							ObjectIdAttributeNumber,
							BTEqualStrategyNumber, F_OIDEQ,
							ObjectIdGetDatum(object->objectId));

				amscan = systable_beginscan(amopDesc, AccessMethodOperatorOidIndexId, true,
											NULL, 1, skey);

				tup = systable_getnext(amscan);

				if (!HeapTupleIsValid(tup))
					elog(ERROR, "could not find tuple for amop entry %u",
						 object->objectId);

				amopForm = (Form_pg_amop) GETSTRUCT(tup);

				initStringInfo(&opfam);
				getOpFamilyIdentity(&opfam, amopForm->amopfamily, objname);

				ltype = format_type_be_qualified(amopForm->amoplefttype);
				rtype = format_type_be_qualified(amopForm->amoprighttype);

				if (objname)
				{
					*objname = lappend(*objname,
									 psprintf("%d", amopForm->amopstrategy));
					*objargs = list_make2(ltype, rtype);
				}

				appendStringInfo(&buffer, "operator %d (%s, %s) of %s",
								 amopForm->amopstrategy,
								 ltype, rtype, opfam.data);

				pfree(opfam.data);

				systable_endscan(amscan);
				heap_close(amopDesc, AccessShareLock);
				break;
			}

		case OCLASS_AMPROC:
			{
				Relation	amprocDesc;
				ScanKeyData skey[1];
				SysScanDesc amscan;
				HeapTuple	tup;
				Form_pg_amproc amprocForm;
				StringInfoData opfam;
				char	   *ltype;
				char	   *rtype;

				amprocDesc = heap_open(AccessMethodProcedureRelationId,
									   AccessShareLock);

				ScanKeyInit(&skey[0],
							ObjectIdAttributeNumber,
							BTEqualStrategyNumber, F_OIDEQ,
							ObjectIdGetDatum(object->objectId));

				amscan = systable_beginscan(amprocDesc, AccessMethodProcedureOidIndexId, true,
											NULL, 1, skey);

				tup = systable_getnext(amscan);

				if (!HeapTupleIsValid(tup))
					elog(ERROR, "could not find tuple for amproc entry %u",
						 object->objectId);

				amprocForm = (Form_pg_amproc) GETSTRUCT(tup);

				initStringInfo(&opfam);
				getOpFamilyIdentity(&opfam, amprocForm->amprocfamily, objname);

				ltype = format_type_be_qualified(amprocForm->amproclefttype);
				rtype = format_type_be_qualified(amprocForm->amprocrighttype);

				if (objname)
				{
					*objname = lappend(*objname,
									   psprintf("%d", amprocForm->amprocnum));
					*objargs = list_make2(ltype, rtype);
				}

				appendStringInfo(&buffer, "function %d (%s, %s) of %s",
								 amprocForm->amprocnum,
								 ltype, rtype, opfam.data);

				pfree(opfam.data);

				systable_endscan(amscan);
				heap_close(amprocDesc, AccessShareLock);
				break;
			}

		case OCLASS_REWRITE:
			{
				Relation	ruleDesc;
				HeapTuple	tup;
				Form_pg_rewrite rule;

				ruleDesc = heap_open(RewriteRelationId, AccessShareLock);

				tup = get_catalog_object_by_oid(ruleDesc, object->objectId);

				if (!HeapTupleIsValid(tup))
					elog(ERROR, "could not find tuple for rule %u",
						 object->objectId);

				rule = (Form_pg_rewrite) GETSTRUCT(tup);

				appendStringInfo(&buffer, "%s on ",
								 quote_identifier(NameStr(rule->rulename)));
				getRelationIdentity(&buffer, rule->ev_class, objname);
				if (objname)
					*objname = lappend(*objname, pstrdup(NameStr(rule->rulename)));

				heap_close(ruleDesc, AccessShareLock);
				break;
			}

		case OCLASS_TRIGGER:
			{
				Relation	trigDesc;
				HeapTuple	tup;
				Form_pg_trigger trig;

				trigDesc = heap_open(TriggerRelationId, AccessShareLock);

				tup = get_catalog_object_by_oid(trigDesc, object->objectId);

				if (!HeapTupleIsValid(tup))
					elog(ERROR, "could not find tuple for trigger %u",
						 object->objectId);

				trig = (Form_pg_trigger) GETSTRUCT(tup);

				appendStringInfo(&buffer, "%s on ",
								 quote_identifier(NameStr(trig->tgname)));
				getRelationIdentity(&buffer, trig->tgrelid, objname);
				if (objname)
					*objname = lappend(*objname, pstrdup(NameStr(trig->tgname)));

				heap_close(trigDesc, AccessShareLock);
				break;
			}

		case 32:
			{
				Relation	polDesc;
				HeapTuple	tup;
				Form_pg_policy policy;

				polDesc = heap_open(PolicyRelationId, AccessShareLock);

				tup = get_catalog_object_by_oid(polDesc, object->objectId);

				if (!HeapTupleIsValid(tup))
					elog(ERROR, "could not find tuple for policy %u",
						 object->objectId);

				policy = (Form_pg_policy) GETSTRUCT(tup);

				appendStringInfo(&buffer, "%s on ",
								 quote_identifier(NameStr(policy->polname)));
				getRelationIdentity(&buffer, policy->polrelid, objname);
				if (objname)
					*objname = lappend(*objname, pstrdup(NameStr(policy->polname)));

				heap_close(polDesc, AccessShareLock);
				break;
			}

		case OCLASS_SCHEMA:
			{
				char	   *nspname;

				nspname = get_namespace_name_or_temp(object->objectId);
				if (!nspname)
					elog(ERROR, "cache lookup failed for namespace %u",
						 object->objectId);
				appendStringInfoString(&buffer,
									   quote_identifier(nspname));
				if (objname)
					*objname = list_make1(nspname);
				break;
			}

		case OCLASS_TSPARSER:
			{
				HeapTuple	tup;
				Form_pg_ts_parser formParser;
				char	   *schema;

				tup = SearchSysCache1(TSPARSEROID,
									  ObjectIdGetDatum(object->objectId));
				if (!HeapTupleIsValid(tup))
					elog(ERROR, "cache lookup failed for text search parser %u",
						 object->objectId);
				formParser = (Form_pg_ts_parser) GETSTRUCT(tup);
				schema = get_namespace_name_or_temp(formParser->prsnamespace);
				appendStringInfoString(&buffer,
									   quote_qualified_identifier(schema,
											  NameStr(formParser->prsname)));
				if (objname)
					*objname = list_make2(schema,
									  pstrdup(NameStr(formParser->prsname)));
				ReleaseSysCache(tup);
				break;
			}

		case OCLASS_TSDICT:
			{
				HeapTuple	tup;
				Form_pg_ts_dict formDict;
				char	   *schema;

				tup = SearchSysCache1(TSDICTOID,
									  ObjectIdGetDatum(object->objectId));
				if (!HeapTupleIsValid(tup))
					elog(ERROR, "cache lookup failed for text search dictionary %u",
						 object->objectId);
				formDict = (Form_pg_ts_dict) GETSTRUCT(tup);
				schema = get_namespace_name_or_temp(formDict->dictnamespace);
				appendStringInfoString(&buffer,
									   quote_qualified_identifier(schema,
											   NameStr(formDict->dictname)));
				if (objname)
					*objname = list_make2(schema,
									   pstrdup(NameStr(formDict->dictname)));
				ReleaseSysCache(tup);
				break;
			}

		case OCLASS_TSTEMPLATE:
			{
				HeapTuple	tup;
				Form_pg_ts_template formTmpl;
				char	   *schema;

				tup = SearchSysCache1(TSTEMPLATEOID,
									  ObjectIdGetDatum(object->objectId));
				if (!HeapTupleIsValid(tup))
					elog(ERROR, "cache lookup failed for text search template %u",
						 object->objectId);
				formTmpl = (Form_pg_ts_template) GETSTRUCT(tup);
				schema = get_namespace_name_or_temp(formTmpl->tmplnamespace);
				appendStringInfoString(&buffer,
									   quote_qualified_identifier(schema,
											   NameStr(formTmpl->tmplname)));
				if (objname)
					*objname = list_make2(schema,
									   pstrdup(NameStr(formTmpl->tmplname)));
				ReleaseSysCache(tup);
				break;
			}

		case OCLASS_TSCONFIG:
			{
				HeapTuple	tup;
				Form_pg_ts_config formCfg;
				char	   *schema;

				tup = SearchSysCache1(TSCONFIGOID,
									  ObjectIdGetDatum(object->objectId));
				if (!HeapTupleIsValid(tup))
					elog(ERROR, "cache lookup failed for text search configuration %u",
						 object->objectId);
				formCfg = (Form_pg_ts_config) GETSTRUCT(tup);
				schema = get_namespace_name_or_temp(formCfg->cfgnamespace);
				appendStringInfoString(&buffer,
									   quote_qualified_identifier(schema,
												 NameStr(formCfg->cfgname)));
				if (objname)
					*objname = list_make2(schema,
										  pstrdup(NameStr(formCfg->cfgname)));
				ReleaseSysCache(tup);
				break;
			}

		case OCLASS_ROLE:
			{
				char	   *username;

				username = GetUserNameFromId(object->objectId, false);
				if (objname)
					*objname = list_make1(username);
				appendStringInfoString(&buffer,
									   quote_identifier(username));
				break;
			}

		case OCLASS_DATABASE:
			{
				char	   *datname;

				datname = get_database_name(object->objectId);
				if (!datname)
					elog(ERROR, "cache lookup failed for database %u",
						 object->objectId);
				if (objname)
					*objname = list_make1(datname);
				appendStringInfoString(&buffer,
									   quote_identifier(datname));
				break;
			}

		case OCLASS_TBLSPACE:
			{
				char	   *tblspace;

				tblspace = get_tablespace_name(object->objectId);
				if (!tblspace)
					elog(ERROR, "cache lookup failed for tablespace %u",
						 object->objectId);
				if (objname)
					*objname = list_make1(tblspace);
				appendStringInfoString(&buffer,
									   quote_identifier(tblspace));
				break;
			}

		case OCLASS_FDW:
			{
				ForeignDataWrapper *fdw;

				fdw = GetForeignDataWrapper(object->objectId);
				appendStringInfoString(&buffer, quote_identifier(fdw->fdwname));
				if (objname)
					*objname = list_make1(pstrdup(fdw->fdwname));
				break;
			}

		case OCLASS_FOREIGN_SERVER:
			{
				ForeignServer *srv;

				srv = GetForeignServer(object->objectId);
				appendStringInfoString(&buffer,
									   quote_identifier(srv->servername));
				if (objname)
					*objname = list_make1(pstrdup(srv->servername));
				break;
			}

		case OCLASS_USER_MAPPING:
			{
				HeapTuple	tup;
				Oid			useid;
				Form_pg_user_mapping umform;
				ForeignServer *srv;
				const char *usename;

				tup = SearchSysCache1(USERMAPPINGOID,
									  ObjectIdGetDatum(object->objectId));
				if (!HeapTupleIsValid(tup))
					elog(ERROR, "cache lookup failed for user mapping %u",
						 object->objectId);
				umform = (Form_pg_user_mapping) GETSTRUCT(tup);
				useid = umform->umuser;
				srv = GetForeignServer(umform->umserver);

				ReleaseSysCache(tup);

				if (OidIsValid(useid))
					usename = GetUserNameFromId(useid, false);
				else
					usename = "public";

				if (objname)
				{
					*objname = list_make1(pstrdup(usename));
					*objargs = list_make1(pstrdup(srv->servername));
				}

				appendStringInfo(&buffer, "%s on server %s",
								 quote_identifier(usename),
								 srv->servername);
				break;
			}

		case OCLASS_DEFACL:
			{
				Relation	defaclrel;
				ScanKeyData skey[1];
				SysScanDesc rcscan;
				HeapTuple	tup;
				Form_pg_default_acl defacl;
				char	   *schema;
				char	   *username;

				defaclrel = heap_open(DefaultAclRelationId, AccessShareLock);

				ScanKeyInit(&skey[0],
							ObjectIdAttributeNumber,
							BTEqualStrategyNumber, F_OIDEQ,
							ObjectIdGetDatum(object->objectId));

				rcscan = systable_beginscan(defaclrel, DefaultAclOidIndexId,
											true, NULL, 1, skey);

				tup = systable_getnext(rcscan);

				if (!HeapTupleIsValid(tup))
					elog(ERROR, "could not find tuple for default ACL %u",
						 object->objectId);

				defacl = (Form_pg_default_acl) GETSTRUCT(tup);

				username = GetUserNameFromId(defacl->defaclrole, false);
				appendStringInfo(&buffer,
								 "for role %s",
								 quote_identifier(username));

				if (OidIsValid(defacl->defaclnamespace))
				{
					schema = get_namespace_name_or_temp(defacl->defaclnamespace);
					appendStringInfo(&buffer,
									 " in schema %s",
									 quote_identifier(schema));
				}
				else
					schema = NULL;

				switch (defacl->defaclobjtype)
				{
					case DEFACLOBJ_RELATION:
						appendStringInfoString(&buffer,
											   " on tables");
						break;
					case DEFACLOBJ_SEQUENCE:
						appendStringInfoString(&buffer,
											   " on sequences");
						break;
					case DEFACLOBJ_FUNCTION:
						appendStringInfoString(&buffer,
											   " on functions");
						break;
					case DEFACLOBJ_TYPE:
						appendStringInfoString(&buffer,
											   " on types");
						break;
				}

				if (objname)
				{
					*objname = list_make1(username);
					if (schema)
						*objname = lappend(*objname, schema);
					*objargs = list_make1(psprintf("%c", defacl->defaclobjtype));
				}

				systable_endscan(rcscan);
				heap_close(defaclrel, AccessShareLock);
				break;
			}

		case OCLASS_EXTENSION:
			{
				char	   *extname;

				extname = get_extension_name(object->objectId);
				if (!extname)
					elog(ERROR, "cache lookup failed for extension %u",
						 object->objectId);
				appendStringInfoString(&buffer, quote_identifier(extname));
				if (objname)
					*objname = list_make1(extname);
				break;
			}

		case 31:
			{
				HeapTuple	tup;
				Form_pg_event_trigger trigForm;

				/* no objname support here */
				if (objname)
					*objname = NIL;

				tup = SearchSysCache1(EVENTTRIGGEROID,
									  ObjectIdGetDatum(object->objectId));
				if (!HeapTupleIsValid(tup))
					elog(ERROR, "cache lookup failed for event trigger %u",
						 object->objectId);
				trigForm = (Form_pg_event_trigger) GETSTRUCT(tup);
				appendStringInfoString(&buffer,
							   quote_identifier(NameStr(trigForm->evtname)));
				ReleaseSysCache(tup);
				break;
			}

		case 33:
			{
				Relation	transformDesc;
				HeapTuple	tup;
				Form_pg_transform transform;
				char	   *transformLang;
				char	   *transformType;

				transformDesc = heap_open(TransformRelationId, AccessShareLock);

				tup = get_catalog_object_by_oid(transformDesc, object->objectId);

				if (!HeapTupleIsValid(tup))
					elog(ERROR, "could not find tuple for transform %u",
						 object->objectId);

				transform = (Form_pg_transform) GETSTRUCT(tup);

				transformType = format_type_be_qualified(transform->trftype);
				transformLang = get_language_name(transform->trflang);

				appendStringInfo(&buffer, "for %s on language %s",
								 transformType,
								 transformLang);
				if (objname)
				{
					*objname = list_make1(transformType);
					*objargs = list_make1(pstrdup(transformLang));
				}

				heap_close(transformDesc, AccessShareLock);
			}
			break;

		case 29:
			{
				char	   *amname;

				amname = get_am_name(object->objectId);
				if (!amname)
					elog(ERROR, "cache lookup failed for access method %u",
						 object->objectId);
				appendStringInfoString(&buffer, quote_identifier(amname));
				if (objname)
					*objname = list_make1(amname);
			}
			break;

		default:
			appendStringInfo(&buffer, "unrecognized object %u %u %d",
							 object->classId,
							 object->objectId,
							 object->objectSubId);
			break;
	}

	/*
	 * If a get_object_address representation was requested, make sure we are
	 * providing one.  We don't check objargs, because many of the cases above
	 * leave it as NIL.
	 */
	if (objname && *objname == NIL)
		elog(ERROR, "requested object address for unsupported object class %d: text result \"%s\"",
			 (int) getObjectClass(object), buffer.data);

	return buffer.data;
}

static void
getOpFamilyIdentity(StringInfo buffer, Oid opfid, List **objname)
{
	HeapTuple	opfTup;
	Form_pg_opfamily opfForm;
	HeapTuple	amTup;
	Form_pg_am	amForm;
	char	   *schema;

	opfTup = SearchSysCache1(OPFAMILYOID, ObjectIdGetDatum(opfid));
	if (!HeapTupleIsValid(opfTup))
		elog(ERROR, "cache lookup failed for opfamily %u", opfid);
	opfForm = (Form_pg_opfamily) GETSTRUCT(opfTup);

	amTup = SearchSysCache1(AMOID, ObjectIdGetDatum(opfForm->opfmethod));
	if (!HeapTupleIsValid(amTup))
		elog(ERROR, "cache lookup failed for access method %u",
			 opfForm->opfmethod);
	amForm = (Form_pg_am) GETSTRUCT(amTup);

	schema = get_namespace_name_or_temp(opfForm->opfnamespace);
	appendStringInfo(buffer, "%s USING %s",
					 quote_qualified_identifier(schema,
												NameStr(opfForm->opfname)),
					 NameStr(amForm->amname));

	if (objname)
		*objname = list_make3(pstrdup(NameStr(amForm->amname)),
							  pstrdup(schema),
							  pstrdup(NameStr(opfForm->opfname)));

	ReleaseSysCache(amTup);
	ReleaseSysCache(opfTup);
}

/*
 * Append the relation identity (quoted qualified name) to the given
 * StringInfo.
 */
static void
getRelationIdentity(StringInfo buffer, Oid relid, List **objname)
{
	HeapTuple	relTup;
	Form_pg_class relForm;
	char	   *schema;

	relTup = SearchSysCache1(RELOID,
							 ObjectIdGetDatum(relid));
	if (!HeapTupleIsValid(relTup))
		elog(ERROR, "cache lookup failed for relation %u", relid);
	relForm = (Form_pg_class) GETSTRUCT(relTup);

	schema = get_namespace_name_or_temp(relForm->relnamespace);
	appendStringInfoString(buffer,
						   quote_qualified_identifier(schema,
												 NameStr(relForm->relname)));
	if (objname)
		*objname = list_make2(schema, pstrdup(NameStr(relForm->relname)));

	ReleaseSysCache(relTup);
}

/*
 * Auxiliary function to return a TEXT array out of a list of C-strings.
 */
ArrayType *
strlist_to_textarray(List *list)
{
	ArrayType  *arr;
	Datum	   *datums;
	int			j = 0;
	ListCell   *cell;
	MemoryContext memcxt;
	MemoryContext oldcxt;

	memcxt = AllocSetContextCreate(CurrentMemoryContext,
								   "strlist to array",
								   ALLOCSET_DEFAULT_SIZES);
	oldcxt = MemoryContextSwitchTo(memcxt);

	datums =(Datum	*) palloc(sizeof(text *) * list_length(list));
	foreach(cell, list)
	{
		char	   *name =(char	 *) lfirst(cell);

		datums[j++] = CStringGetTextDatum(name);
	}

	MemoryContextSwitchTo(oldcxt);

	arr = construct_array(datums, list_length(list),
						  TEXTOID, -1, false, 'i');
	MemoryContextDelete(memcxt);

	return arr;
}


#define MAX_INT32_LEN 11



char *
format_procedure_qualified(Oid procedure_oid)
{
	return format_procedure_internal(procedure_oid, true);
}

void
format_procedure_parts(Oid procedure_oid, List **objnames, List **objargs)
{
	HeapTuple	proctup;
	Form_pg_proc procform;
	int			nargs;
	int			i;

	proctup = SearchSysCache1(PROCOID, ObjectIdGetDatum(procedure_oid));

	if (!HeapTupleIsValid(proctup))
		elog(ERROR, "cache lookup failed for procedure with OID %u", procedure_oid);

	procform = (Form_pg_proc) GETSTRUCT(proctup);
	nargs = procform->pronargs;

	*objnames = list_make2(get_namespace_name_or_temp(procform->pronamespace),
						   pstrdup(NameStr(procform->proname)));
	*objargs = NIL;
	for (i = 0; i < nargs; i++)
	{
		Oid			thisargtype = procform->proargtypes.values[i];

		*objargs = lappend(*objargs, format_type_be_qualified(thisargtype));
	}

	ReleaseSysCache(proctup);
}

char *
get_namespace_name_or_temp(Oid nspid)
{
	if (isTempNamespace(nspid))
		return "pg_temp";
	else
		return get_namespace_name(nspid);
}

char *
get_am_name(Oid amOid)
{
	HeapTuple	tup;
	char	   *result = NULL;

	tup = SearchSysCache1(AMOID, ObjectIdGetDatum(amOid));
	if (HeapTupleIsValid(tup))
	{
		Form_pg_am	amform = (Form_pg_am) GETSTRUCT(tup);

		result = pstrdup(NameStr(amform->amname));
		ReleaseSysCache(tup);
	}
	return result;
}


char *
format_type_be_qualified(Oid type_oid)
{
	return format_type_internal(type_oid, -1, false, false, true);
}

void
format_operator_parts(Oid operator_oid, List **objnames, List **objargs)
{
	HeapTuple	opertup;
	Form_pg_operator oprForm;

	opertup = SearchSysCache1(OPEROID, ObjectIdGetDatum(operator_oid));
	if (!HeapTupleIsValid(opertup))
		elog(ERROR, "cache lookup failed for operator with OID %u",
			 operator_oid);

	oprForm = (Form_pg_operator) GETSTRUCT(opertup);
	*objnames = list_make2(get_namespace_name_or_temp(oprForm->oprnamespace),
						   pstrdup(NameStr(oprForm->oprname)));
	*objargs = NIL;
	if (oprForm->oprleft)
		*objargs = lappend(*objargs,
						   format_type_be_qualified(oprForm->oprleft));
	if (oprForm->oprright)
		*objargs = lappend(*objargs,
						   format_type_be_qualified(oprForm->oprright));

	ReleaseSysCache(opertup);
}

char *
format_operator_qualified(Oid operator_oid)
{
	return format_operator_internal(operator_oid, true);
}

bool
is_objectclass_supported(Oid class_id)
{
	int			index;

	for (index = 0; index < lengthof(ObjectProperty); index++)
	{
		if (ObjectProperty[index].class_oid == class_id)
			return true;
	}

	return false;
}


static char* format_type_internal(
    Oid type_oid, int32 typemod, bool typemod_given, bool allow_invalid, bool include_nspname)
{
    bool with_typemod = typemod_given && (typemod >= 0);
    HeapTuple tuple;
    Form_pg_type typeform;
    Oid array_base_type;
    bool is_array = false;
    char* buf = NULL;

    if (type_oid == InvalidOid && allow_invalid)
        return pstrdup("-");

    tuple = SearchSysCache1(TYPEOID, ObjectIdGetDatum(type_oid));
    if (!HeapTupleIsValid(tuple)) {
        if (allow_invalid)
            return pstrdup("???");
        else
            ereport(ERROR, (errcode(ERRCODE_CACHE_LOOKUP_FAILED), errmsg("cache lookup failed for type %u", type_oid)));
    }
    typeform = (Form_pg_type)GETSTRUCT(tuple);

    /*
     * Check if it's a regular (variable length) array type.  Fixed-length
     * array types such as "name" shouldn't get deconstructed.  As of openGauss
     * 8.1, rather than checking typlen we check the toast property, and don't
     * deconstruct "plain storage" array types --- this is because we don't
     * want to show oidvector as oid[].
     */
    array_base_type = typeform->typelem;

    if (array_base_type != InvalidOid && typeform->typstorage != 'p' && type_oid != OIDVECTOREXTENDOID &&
        type_oid != INT2VECTOREXTENDOID) {
        /* Switch our attention to the array element type */
        ReleaseSysCache(tuple);
        tuple = SearchSysCache1(TYPEOID, ObjectIdGetDatum(array_base_type));
        if (!HeapTupleIsValid(tuple)) {
            if (allow_invalid)
                return pstrdup("???[]");
            else
                ereport(
                    ERROR, (errcode(ERRCODE_CACHE_LOOKUP_FAILED), errmsg("cache lookup failed for type %u", type_oid)));
        }
        typeform = (Form_pg_type)GETSTRUCT(tuple);
        type_oid = array_base_type;
        is_array = true;
    } else
        is_array = false;

    /*
     * See if we want to special-case the output for certain built-in types.
     * Note that these special cases should all correspond to special
     * productions in gram.y, to ensure that the type name will be taken as a
     * system type, not a user type of the same name.
     *
     * If we do not provide a special-case output here, the type name will be
     * handled the same way as a user type name --- in particular, it will be
     * double-quoted if it matches any lexer keyword.  This behavior is
     * essential for some cases, such as types "bit" and "char".
     */
    buf = NULL; /* flag for no special case */
    if ((type_oid == BYTEAWITHOUTORDERWITHEQUALCOLOID || type_oid == BYTEAWITHOUTORDERCOLOID) && typemod > 0) {
        type_oid = typemod;
        with_typemod = false;
    }

    switch (type_oid) {
        case BITOID:
            if (with_typemod)
                buf = printTypmod("bit", typemod, typeform->typmodout);
            else if (typemod_given) {
                /*
                 * bit with typmod -1 is not the same as BIT, which means
                 * BIT(1) per SQL spec.  Report it as the quoted typename so
                 * that parser will not assign a bogus typmod.
                 */
            } else
                buf = pstrdup("bit");
            break;

        case BOOLOID:
            buf = pstrdup("boolean");
            break;

        case BPCHAROID:
            if (with_typemod)
                buf = printTypmod("character", typemod, typeform->typmodout);
            else if (typemod_given) {
                /*
                 * bpchar with typmod -1 is not the same as CHARACTER, which
                 * means CHARACTER(1) per SQL spec.  Report it as bpchar so
                 * that parser will not assign a bogus typmod.
                 */
            } else
                buf = pstrdup("character");
            break;

        case DATEOID:
            buf = pstrdup("date");
            break;

        case FLOAT4OID:
            buf = pstrdup("real");
            break;

        case FLOAT8OID:
            buf = pstrdup("double precision");
            break;
        case INT1OID:
            buf = pstrdup("tinyint");
            break;
        case INT2OID:
            buf = pstrdup("smallint");
            break;

        case INT4OID:
            buf = pstrdup("integer");
            break;

        case INT8OID:
            buf = pstrdup("bigint");
            break;

        case NUMERICOID:
            if (with_typemod)
                buf = printTypmod("numeric", typemod, typeform->typmodout);
            else
                buf = pstrdup("numeric");
            break;

        case INTERVALOID:
            if (with_typemod)
                buf = printTypmod("interval", typemod, typeform->typmodout);
            else
                buf = pstrdup("interval");
            break;

        case TIMEOID:
            if (with_typemod)
                buf = printTypmod("time", typemod, typeform->typmodout);
            else
                buf = pstrdup("time without time zone");
            break;

        case TIMETZOID:
            if (with_typemod)
                buf = printTypmod("time", typemod, typeform->typmodout);
            else
                buf = pstrdup("time with time zone");
            break;

        case TIMESTAMPOID:
            if (with_typemod)
                buf = printTypmod("timestamp", typemod, typeform->typmodout);
            else
                buf = pstrdup("timestamp without time zone");
            break;

        case TIMESTAMPTZOID:
            if (with_typemod)
                buf = printTypmod("timestamp", typemod, typeform->typmodout);
            else
                buf = pstrdup("timestamp with time zone");
            break;
        case SMALLDATETIMEOID:
            buf = pstrdup("smalldatetime");
            break;
        case VARBITOID:
            if (with_typemod)
                buf = printTypmod("bit varying", typemod, typeform->typmodout);
            else
                buf = pstrdup("bit varying");
            break;

        case VARCHAROID:
            if (with_typemod)
                buf = printTypmod("character varying", typemod, typeform->typmodout);
            else
                buf = pstrdup("character varying");
            break;

        case NVARCHAR2OID:
            if (with_typemod)
                buf = printTypmod("nvarchar2", typemod, typeform->typmodout);
            else
                buf = pstrdup("nvarchar2");
            break;
        case TEXTOID:
            buf = pstrdup("text");
            break;
        case BYTEAOID:
            buf = pstrdup("bytea");
            break;
        case BYTEAWITHOUTORDERWITHEQUALCOLOID:
            buf = pstrdup("byteawithoutorderwithequalcol");
            break;
        default:
            break;
    }

    if (buf == NULL) {
        /*
         * Default handling: report the name as it appears in the catalog.
         * Here, we must qualify the name if it is not visible in the search
         * path, and we must double-quote it if it's not a standard identifier
         * or if it matches any keyword.
         */
        char* nspname = NULL;
        char* typname = NULL;

        if (TypeIsVisible(type_oid)) {
            nspname = NULL;

            /* get namespace string if we foce to deparse namespace name */
            if (include_nspname && PG_CATALOG_NAMESPACE != typeform->typnamespace)
                nspname = get_namespace_name(typeform->typnamespace);
        } else {
            nspname = get_namespace_name(typeform->typnamespace);
        }

        typname = NameStr(typeform->typname);

        buf = quote_qualified_identifier(nspname, typname);

        if (with_typemod && !(type_oid == BYTEAWITHOUTORDERWITHEQUALCOLOID || type_oid == BYTEAWITHOUTORDERCOLOID))
            buf = printTypmod(buf, typemod, typeform->typmodout);
    }

    if (is_array)
        buf = psnprintf(strlen(buf) + 3, "%s[]", buf);

    ReleaseSysCache(tuple);

    return buf;
}



static char* printTypmod(const char* typname, int32 typmod, Oid typmodout)
{
    char* res = NULL;

    /* Shouldn't be called if typmod is -1 */
    Assert(typmod >= 0);

    if (typmodout == InvalidOid) {
        /* Default behavior: just print the integer typmod with parens */
        res = psnprintf(strlen(typname) + MAX_INT32_LEN + 3, "%s(%d)", typname, (int)typmod);
    } else {
        /* Use the type-specific typmodout procedure */
        char* tmstr = NULL;

        tmstr = DatumGetCString(OidFunctionCall1(typmodout, Int32GetDatum(typmod)));
        res = psnprintf(strlen(typname) + strlen(tmstr) + 1, "%s%s", typname, tmstr);
    }

    return res;
}


static char* psnprintf(size_t len, const char* fmt, ...)
{
    va_list ap;
    char* buf = NULL;

    buf = (char*)palloc0(len);

    va_start(ap, fmt);
    errno_t rc = vsnprintf_s(buf, len, len - 1, fmt, ap);
    securec_check_ss(rc, "\0", "\0");
    va_end(ap);

    return buf;
}

static char *
format_procedure_internal(Oid procedure_oid, bool force_qualify)
{
	char	   *result;
	HeapTuple	proctup;

	proctup = SearchSysCache1(PROCOID, ObjectIdGetDatum(procedure_oid));

	if (HeapTupleIsValid(proctup))
	{
		Form_pg_proc procform = (Form_pg_proc) GETSTRUCT(proctup);
		char	   *proname = NameStr(procform->proname);
		int			nargs = procform->pronargs;
		int			i;
		char	   *nspname;
		StringInfoData buf;

		/* XXX no support here for bootstrap mode */

		initStringInfo(&buf);

		/*
		 * Would this proc be found (given the right args) by regprocedurein?
		 * If not, or if caller requests it, we need to qualify it.
		 */
		if (!force_qualify && FunctionIsVisible(procedure_oid))
			nspname = NULL;
		else
			nspname = get_namespace_name(procform->pronamespace);

		appendStringInfo(&buf, "%s(",
						 quote_qualified_identifier(nspname, proname));
		for (i = 0; i < nargs; i++)
		{
			Oid			thisargtype = procform->proargtypes.values[i];

			if (i > 0)
				appendStringInfoChar(&buf, ',');
			appendStringInfoString(&buf,
								   force_qualify ?
								   format_type_be_qualified(thisargtype) :
								   format_type_be(thisargtype));
		}
		appendStringInfoChar(&buf, ')');

		result = buf.data;

		ReleaseSysCache(proctup);
	}
	else
	{
		/* If OID doesn't match any pg_proc entry, return it numerically */
		result = (char *) palloc(NAMEDATALEN);
		snprintf(result, NAMEDATALEN, "%u", procedure_oid);
	}

	return result;
}


static char *
format_operator_internal(Oid operator_oid, bool force_qualify)
{
	char	   *result;
	HeapTuple	opertup;

	opertup = SearchSysCache1(OPEROID, ObjectIdGetDatum(operator_oid));

	if (HeapTupleIsValid(opertup))
	{
		Form_pg_operator operform = (Form_pg_operator) GETSTRUCT(opertup);
		char	   *oprname = NameStr(operform->oprname);
		char	   *nspname;
		StringInfoData buf;

		/* XXX no support here for bootstrap mode */

		initStringInfo(&buf);

		/*
		 * Would this oper be found (given the right args) by regoperatorin?
		 * If not, or if caller explicitly requests it, we need to qualify it.
		 */
		if (force_qualify || !OperatorIsVisible(operator_oid))
		{
			nspname = get_namespace_name(operform->oprnamespace);
			appendStringInfo(&buf, "%s.",
							 quote_identifier(nspname));
		}

		appendStringInfo(&buf, "%s(", oprname);

		if (operform->oprleft)
			appendStringInfo(&buf, "%s,",
							 force_qualify ?
							 format_type_be_qualified(operform->oprleft) :
							 format_type_be(operform->oprleft));
		else
			appendStringInfoString(&buf, "NONE,");

		if (operform->oprright)
			appendStringInfo(&buf, "%s)",
							 force_qualify ?
							 format_type_be_qualified(operform->oprright) :
							 format_type_be(operform->oprright));
		else
			appendStringInfoString(&buf, "NONE)");

		result = buf.data;

		ReleaseSysCache(opertup);
	}
	else
	{
		/*
		 * If OID doesn't match any pg_operator entry, return it numerically
		 */
		result = (char *) palloc(NAMEDATALEN);
		snprintf(result, NAMEDATALEN, "%u", operator_oid);
	}

	return result;
}
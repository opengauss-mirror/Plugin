#include <postgres.h>
#include <access/xact.h>
#include <access/transam.h>
//#include <commands/event_trigger.h>
#include <catalog/namespace.h>
#include <utils/lsyscache.h>
#include <utils/inval.h>

#include "compat-msvc-enter.h" /* To label externs in extension.h and
								 * miscadmin.h correctly */
#include <commands/extension.h>
#include <miscadmin.h>
#include "compat-msvc-exit.h"

#include <access/relscan.h>
#include <catalog/indexing.h>
#include <catalog/pg_extension.h>
#include <utils/builtins.h>
#include <utils/fmgroids.h>

#include "config.h"
#include "catalog.h"
#include "extension.h"
#include "guc.h"
#include "config.h"
#include "extension_utils.cpp"
#include "compat.h"

#define TS_UPDATE_SCRIPT_CONFIG_VAR "timescaledb.update_script_stage"
#define POST_UPDATE "post"
static Oid extension_proxy_oid = InvalidOid;

Oid
ts_extension_schema_oid(void)
{
	Datum result;
	Relation rel;
	SysScanDesc scandesc;
	HeapTuple tuple;
	ScanKeyData entry[1];
	bool is_null = true;
	Oid schema = InvalidOid;

	rel = table_open(ExtensionRelationId, AccessShareLock);

	ScanKeyInit(&entry[0],
				Anum_pg_extension_extname,
				BTEqualStrategyNumber,
				F_NAMEEQ,
				DirectFunctionCall1(namein, CStringGetDatum(EXTENSION_NAME)));

	scandesc = systable_beginscan(rel, ExtensionNameIndexId, true, NULL, 1, entry);

	tuple = systable_getnext(scandesc);

	/* We assume that there can be at most one matching tuple */
	if (HeapTupleIsValid(tuple))
	{
		result =
			heap_getattr(tuple, Anum_pg_extension_extnamespace, RelationGetDescr(rel), &is_null);

		if (!is_null)
			schema = DatumGetObjectId(result);
	}

	systable_endscan(scandesc);
	table_close(rel, AccessShareLock);

	if (schema == InvalidOid)
		elog(ERROR, "extension schema not found");
	return schema;
}

char *
ts_extension_schema_name(void)
{
	return get_namespace_name(ts_extension_schema_oid());
}
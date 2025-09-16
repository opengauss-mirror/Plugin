/*-------------------------------------------------------------------------
 *
 * tdigest_extension.c
 *    Helper functions to get access to tdigest specific data.
 *
 * Copyright (c) Citus Data, Inc.
 *-------------------------------------------------------------------------
 */
#include "postgres.h"

#include "access/genam.h"
#include "access/htup.h"
#include "catalog/pg_extension.h"
#include "catalog/pg_type.h"
#include "parser/parse_func.h"
#include "utils/fmgroids.h"
#include "utils/lsyscache.h"
#include "distributed/commands.h"
#include "distributed/metadata_cache.h"
#include "distributed/tdigest_extension.h"
#include "distributed/version_compat.h"

static Oid LookupTDigestFunction(const char* functionName, int argcount, Oid* argtypes);

/*
 * TDigestExtensionSchema finds the schema the tdigest extension is installed in. The
 * function will return InvalidOid if the extension is not installed.
 */
Oid TDigestExtensionSchema()
{
    ScanKeyData entry[1];
    Form_pg_extension extensionForm = NULL;
    Oid tdigestExtensionSchema = InvalidOid;

    Relation relation = table_open(ExtensionRelationId, AccessShareLock);

    ScanKeyInit(&entry[0], Anum_pg_extension_extname, BTEqualStrategyNumber, F_NAMEEQ,
                CStringGetDatum("tdigest"));

    SysScanDesc scandesc =
        systable_beginscan(relation, ExtensionNameIndexId, true, NULL, 1, entry);

    HeapTuple extensionTuple = systable_getnext(scandesc);

    /*
     * We assume that there can be at most one matching tuple, if no tuple found the
     * extension is not installed. The value of InvalidOid will not be changed.
     */
    if (HeapTupleIsValid(extensionTuple)) {
        extensionForm = (Form_pg_extension)GETSTRUCT(extensionTuple);
        tdigestExtensionSchema = extensionForm->extnamespace;
        Assert(OidIsValid(tdigestExtensionSchema));
    }

    systable_endscan(scandesc);

    table_close(relation, AccessShareLock);

    return tdigestExtensionSchema;
}

/*
 * TDigestExtensionTypeOid performs a lookup for the Oid of the type representing the
 * tdigest as installed by the tdigest extension returns InvalidOid if the type cannot be
 * found.
 */
Oid TDigestExtensionTypeOid()
{
    Oid tdigestSchemaOid = TDigestExtensionSchema();
    if (!OidIsValid(tdigestSchemaOid)) {
        return InvalidOid;
    }
    char* namespaceName = get_namespace_name(tdigestSchemaOid);
    return LookupTypeOid(namespaceName, "tdigest");
}

/*
 * LookupTDigestFunction is a helper function specifically to lookup functions in the
 * namespace/schema where the tdigest extension is installed. This makes the lookup of
 * following aggregate functions easier and less repetitive.
 */
static Oid LookupTDigestFunction(const char* functionName, int argcount, Oid* argtypes)
{
    Oid tdigestSchemaOid = TDigestExtensionSchema();
    if (!OidIsValid(tdigestSchemaOid)) {
        return InvalidOid;
    }

    char* namespaceName = get_namespace_name(tdigestSchemaOid);
    return LookupFuncName(
        list_make2(makeString(namespaceName), makeString(pstrdup(functionName))),
        argcount, argtypes, true);
}

/*
 * TDigestExtensionAggTDigest1 performs a lookup for the Oid of the tdigest aggregate;
 *   tdigest(tdigest)
 *
 * If the aggregate is not found InvalidOid is returned.
 */
Oid TDigestExtensionAggTDigest1()
{
    Oid argList[] = {TDigestExtensionTypeOid()};
    return LookupTDigestFunction("tdigest", 1, argList);
}

/*
 * TDigestExtensionAggTDigest2 performs a lookup for the Oid of the tdigest aggregate;
 *   tdigest(value double precision, compression int)
 *
 * If the aggregate is not found InvalidOid is returned.
 */
Oid TDigestExtensionAggTDigest2()
{
    Oid argList[] = {FLOAT8OID, INT4OID};
    return LookupTDigestFunction("tdigest", 2, argList);
}

/*
 * TDigestExtensionAggTDigestPercentile2 performs a lookup for the Oid of the tdigest
 * aggregate;
 *   tdigest_percentile(tdigest, double precision)
 *
 * If the aggregate is not found InvalidOid is returned.
 */
Oid TDigestExtensionAggTDigestPercentile2()
{
    Oid argList[] = {TDigestExtensionTypeOid(), FLOAT8OID};
    return LookupTDigestFunction("tdigest_percentile", 2, argList);
}

/*
 * TDigestExtensionAggTDigestPercentile2a performs a lookup for the Oid of the tdigest
 * aggregate;
 *   tdigest_percentile(tdigest, double precision[])
 *
 * If the aggregate is not found InvalidOid is returned.
 */
Oid TDigestExtensionAggTDigestPercentile2a(void)
{
    Oid argList[] = {TDigestExtensionTypeOid(), FLOAT8ARRAYOID};
    return LookupTDigestFunction("tdigest_percentile", 2, argList);
}

/*
 * TDigestExtensionAggTDigestPercentile3 performs a lookup for the Oid of the tdigest
 * aggregate;
 *   tdigest_percentile(double precision, int, double precision)
 *
 * If the aggregate is not found InvalidOid is returned.
 */
Oid TDigestExtensionAggTDigestPercentile3()
{
    Oid argList[] = {FLOAT8OID, INT4OID, FLOAT8OID};
    return LookupTDigestFunction("tdigest_percentile", 3, argList);
}

/*
 * TDigestExtensionAggTDigestPercentile3a performs a lookup for the Oid of the tdigest
 * aggregate;
 *   tdigest_percentile(double precision, int, double precision[])
 *
 * If the aggregate is not found InvalidOid is returned.
 */
Oid TDigestExtensionAggTDigestPercentile3a(void)
{
    Oid argList[] = {FLOAT8OID, INT4OID, FLOAT8ARRAYOID};
    return LookupTDigestFunction("tdigest_percentile", 3, argList);
}

/*
 * TDigestExtensionAggTDigestPercentileOf2 performs a lookup for the Oid of the tdigest
 * aggregate;
 *   tdigest_percentile_of(tdigest, double precision)
 *
 * If the aggregate is not found InvalidOid is returned.
 */
Oid TDigestExtensionAggTDigestPercentileOf2()
{
    Oid argList[] = {TDigestExtensionTypeOid(), FLOAT8OID};
    return LookupTDigestFunction("tdigest_percentile_of", 2, argList);
}

/*
 * TDigestExtensionAggTDigestPercentileOf2a performs a lookup for the Oid of the tdigest
 * aggregate;
 *   tdigest_percentile_of(tdigest, double precision[])
 *
 * If the aggregate is not found InvalidOid is returned.
 */
Oid TDigestExtensionAggTDigestPercentileOf2a(void)
{
    Oid argList[] = {TDigestExtensionTypeOid(), FLOAT8ARRAYOID};
    return LookupTDigestFunction("tdigest_percentile_of", 2, argList);
}

/*
 * TDigestExtensionAggTDigestPercentileOf3 performs a lookup for the Oid of the tdigest
 * aggregate;
 *   tdigest_percentile_of(double precision, int, double precision)
 *
 * If the aggregate is not found InvalidOid is returned.
 */
Oid TDigestExtensionAggTDigestPercentileOf3()
{
    Oid argList[] = {FLOAT8OID, INT4OID, FLOAT8OID};
    return LookupTDigestFunction("tdigest_percentile_of", 3, argList);
}

/*
 * TDigestExtensionAggTDigestPercentileOf3a performs a lookup for the Oid of the tdigest
 * aggregate;
 *   tdigest_percentile_of(double precision, int, double precision[])
 *
 * If the aggregate is not found InvalidOid is returned.
 */
Oid TDigestExtensionAggTDigestPercentileOf3a(void)
{
    Oid argList[] = {FLOAT8OID, INT4OID, FLOAT8ARRAYOID};
    return LookupTDigestFunction("tdigest_percentile_of", 3, argList);
}

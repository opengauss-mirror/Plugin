/* --------------------------------------------------------------------------
 *
 * spq_hash.cpp
 * 	  Provides hashing routines to support consistant data distribution/location
 * within Greenplum Database.
 *
 * Portions Copyright (c) 2005-2008, Greenplum inc
 * Portions Copyright (c) 2012-Present VMware, Inc. or its affiliates.
 *
 *
 * IDENTIFICATION
 * 	   /spq/spq_hash.cpp
 *
 * --------------------------------------------------------------------------
 */
#include "utils/typcache.h"
#include "spq/spq_hash.h"

/*
 * Return a random segment number, for randomly distributed policy.
 */
unsigned int spqhashrandomseg(int numsegs)
{

    return random() % numsegs;
}

/*
 * Return the default operator family to use for distributing the given type.
 *
 * This is used when redistributing data, e.g. for GROUP BY, or DISTINCT.
 */
Oid spq_default_distribution_opfamily_for_type(Oid typeoid)
{
        TypeCacheEntry *tcache;

        tcache = lookup_type_cache(typeoid, TYPECACHE_HASH_OPFAMILY |
                                            TYPECACHE_HASH_PROC |
                                            TYPECACHE_EQ_OPR);

        if (!tcache->hash_opf)
                return InvalidOid;
        if (!tcache->hash_proc)
                return InvalidOid;
        if (!tcache->eq_opr)
                return InvalidOid;

        return tcache->hash_opf;
}


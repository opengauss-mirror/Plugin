/* -------------------------------------------------------------------------
 *
 * spq_cat.cpp
 * 	  Routines for dealing with GpPolicy
 *
 * Portions Copyright (c) 2005-2008, Greenplum inc
 * Portions Copyright (c) 2012-Present VMware, Inc. or its affiliates.
 *
 *
 * IDENTIFICATION
 * 	    src/spq/spq_cat.cpp
 *
 * -------------------------------------------------------------------------
 */
#include "spq/catalog/spq_policy.h"
#include "utils/palloc.h"

GpPolicy *makeSpqPolicy(GpPolicyType ptype, int nattrs, int numsegments)
{
    GpPolicy *policy;
    size_t size;
    char *p;

    size = MAXALIGN(sizeof(GpPolicy)) + MAXALIGN(nattrs * sizeof(AttrNumber)) + MAXALIGN(nattrs * sizeof(Oid));
    p = (char *)palloc(size);
    policy = (GpPolicy *)p;
    p += MAXALIGN(sizeof(GpPolicy));
    if (nattrs > 0) {
        policy->attrs = (AttrNumber *)p;
        p += MAXALIGN(nattrs * sizeof(AttrNumber));
        policy->opclasses = (Oid *)p;
        p += MAXALIGN(nattrs * sizeof(Oid));
    } else {
        policy->attrs = NULL;
        policy->opclasses = NULL;
    }

    policy->type = T_GpPolicy;
    policy->ptype = ptype;
    policy->numsegments = numsegments;
    policy->nattrs = nattrs;

    Assert(numsegments > 0 || (ptype == POLICYTYPE_ENTRY && numsegments == -1));

    return policy;
}

/*
 * createRandomPartitionedPolicy -- Create a policy with randomly
 * partitioned distribution
 */
GpPolicy *createRandomPartitionedPolicy(int numsegments)
{
    return makeSpqPolicy(POLICYTYPE_PARTITIONED, 0, numsegments);
}
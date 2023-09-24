/* -------------------------------------------------------------------------
 *
 * spq_policy.h
 * 	  definitions for the gauss_policy catalog table
 *
 * Portions Copyright (c) 2005-2011, Greenplum inc
 * Portions Copyright (c) 2012-Present Pivotal Software, Inc.
 * Portions Copyright (c) 2022, Huawei Technologies Co.,Ltd.
 *
 * IDENTIFICATION
 * 	    catalog/spq_policy.h
 *
 * NOTES
 *
 * -------------------------------------------------------------------------
 */

#ifndef _SPQ_POLICY_H_
#define _SPQ_POLICY_H_

#include "access/attnum.h"
#include "catalog/genbki.h"
#include "nodes/pg_list.h"

typedef enum GpPolicyType {
    POLICYTYPE_PARTITIONED, /* Tuples partitioned onto segment database. */
    POLICYTYPE_ENTRY,       /* Tuples stored on entry database. */
    POLICYTYPE_REPLICATED   /* Tuples stored a copy on all segment database. */
} GpPolicyType;

typedef struct GpPolicy {
    NodeTag type;
    GpPolicyType ptype;
    int numsegments;

    /* These fields apply to POLICYTYPE_PARTITIONED. */
    int nattrs;
    AttrNumber *attrs;    /* array of attribute numbers  */
    Oid *opclasses; /* and their opclasses */
} GpPolicy;
extern GpPolicy *makeSpqPolicy(GpPolicyType ptype, int nattrs, int numsegments);
extern GpPolicy *createRandomPartitionedPolicy(int numsegments);
#endif /* _SPQ_POLICY_H_ */
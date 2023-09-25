/* --------------------------------------------------------------------------
 *
 * spq_hash.h
 * 	 Definitions and API functions for spq_hash.cpp
 *
 * Portions Copyright (c) 2005-2008, Greenplum inc
 * Portions Copyright (c) 2012-Present VMware, Inc. or its affiliates.
 *
 *
 * IDENTIFICATION
 * 	   spq/spq_hash.h
 *
 * --------------------------------------------------------------------------
 */
#ifndef SPQ_HASH_H
#define SPQ_HASH_H

#include "utils/rel.h"

extern unsigned int spqhashrandomseg(int numsegs);

extern Oid spq_default_distribution_opfamily_for_type(Oid typid);

#endif /* SPQ_HASH_H */

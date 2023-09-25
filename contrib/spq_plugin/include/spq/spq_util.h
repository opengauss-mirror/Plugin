/* -------------------------------------------------------------------------
 *
 * spq_util.h
 * 	  Header file for routines in spq_util.cpp and results returned by
 * 	  those routines.
 *
 * Portions Copyright (c) 2005-2008, Greenplum inc
 * Portions Copyright (c) 2012-Present VMware, Inc. or its affiliates.
 *
 *
 * IDENTIFICATION
 * 	    src/spq/spq_util.h
 *
 * -------------------------------------------------------------------------
 */

#ifndef SPQ_UTIL_H
#define SPQ_UTIL_H
#include "nodes/pg_list.h"
#define foreach_with_count(cell, list, counter) \
	for ((cell) = list_head(list), (counter)=0; \
	     (cell) != NULL; \
	     (cell) = lnext(cell), ++(counter))

extern bool *makeRandomSegMap(int total_primaries, int total_to_skip);
extern bool *spq_xforms(void);
extern int getSpqsegmentCount(void);
#endif /* SPQ_UTIL_H */
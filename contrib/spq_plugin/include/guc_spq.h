/* --------------------------------------------------------------------
 * guc_spq.h
 *
 * External declarations pertaining to backend/utils/misc/guc-file.l
 * and backend/utils/misc/guc/guc_spq.cpp
 *
 * Copyright (c) 2000-2012, PostgreSQL Global Development Group
 * Written by Peter Eisentraut <peter_e@gmx.net>.
 *
 * src/include/utils/guc_spq.h
 * --------------------------------------------------------------------
 */
#ifndef GUC_SPQ_H
#define GUC_SPQ_H

extern void InitSpqConfigureNames();

/* optimizer minidump mode */
#define OPTIMIZER_MINIDUMP_FAIL 0 /* create optimizer minidump on failure */
#define OPTIMIZER_MINIDUMP_ALWAYS 1 /* always create optimizer minidump */

/* optimizer cost model */
#define OPTIMIZER_SPQDB_LEGACY 0 /* OpenGauss's legacy cost model */
#define OPTIMIZER_SPQDB_CALIBRATED 1 /* OpenGauss's calibrated cost model */
#define OPTIMIZER_SPQDB_EXPERIMENTAL 2 /* OpenGauss's experimental cost model */

/* optimizer join heuristic models */
#define JOIN_ORDER_IN_QUERY 0
#define JOIN_ORDER_GREEDY_SEARCH 1
#define JOIN_ORDER_EXHAUSTIVE_SEARCH 2
#define JOIN_ORDER_EXHAUSTIVE2_SEARCH 3

void make_spq_sync_options(StringInfo option);

#endif /* GUC_SPQ_H */

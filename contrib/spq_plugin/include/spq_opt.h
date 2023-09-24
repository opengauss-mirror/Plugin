/* -------------------------------------------------------------------------
 *
 * spq_opt.h
 * 	  prototypes for the ORCA query planner
 *
 *
 * Portions Copyright (c) 2023 Huawei Technologies Co.,Ltd.
 * Portions Copyright (c) 2010-Present, VMware, Inc. or its affiliates
 * Portions Copyright (c) 2005-2009, Greenplum inc
 * Portions Copyright (c) 1996-2009, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * IDENTIFICATION
 * 			contrib/spq/spq_opt.h
 *
 * -------------------------------------------------------------------------
 */
#ifndef SPQ_OPT_H
#define SPQ_OPT_H

#include "nodes/plannodes.h"

extern PlannedStmt *spq_planner(Query *parse, ParamListInfo boundParams);

#endif /* SPQ_OPT_H */

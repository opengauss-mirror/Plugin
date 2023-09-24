//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 Greenplum, Inc.
//
//	@filename:
//		spqdbdefs.h
//
//	@doc:
//		C Linkage for SPQDB functions used by SPQ optimizer
//
//	@test:
//
//
//---------------------------------------------------------------------------

#ifndef SPQDBDefs_H
#define SPQDBDefs_H

#include "postgres.h"

#include <string.h>

#include "access/heapam.h"
#include "access/relscan.h"
#include "catalog/namespace.h"
#include "catalog/pg_operator.h"
#include "catalog/pg_proc.h"
#include "commands/defrem.h"
#include "commands/trigger.h"
#include "executor/exec/execdesc.h"
#include "funcapi.h"
#include "lib/stringinfo.h"
#include "nodes/execnodes.h"
#include "nodes/makefuncs.h"
#include "nodes/nodes.h"
#include "nodes/pg_list.h"
#include "nodes/plannodes.h"
#include "nodes/print.h"
#include "optimizer/planmain.h"
#include "optimizer/tlist.h"
#include "parser/parse_clause.h"
#include "parser/parse_coerce.h"
#include "parser/parse_expr.h"
#include "parser/parse_oper.h"
#include "parser/parse_relation.h"
#include "parser/parsetree.h"
#include "tcop/dest.h"
#include "utils/array.h"
#include "utils/builtins.h"
#include "utils/datum.h"
#include "utils/elog.h"
#include "utils/inval.h"
#include "utils/lsyscache.h"
#include "utils/numeric.h"
#include "utils/rel.h"
#include "utils/selfuncs.h"
#include "utils/syscache.h"
#include "utils/typcache.h"
#include "spq/uri.h"


extern List *pg_parse_and_rewrite(const char *query_string, Oid *paramTypes,
								  int iNumParams);

extern PlannedStmt *pg_plan_query(Query *pqueryTree, ParamListInfo boundParams);

extern char *get_rel_name(Oid relid);

extern Relation RelationIdGetRelation(Oid relationId);

extern void RelationClose(Relation relation);

extern Oid get_atttype(Oid relid, AttrNumber attnum);

extern RegProcedure get_opcode(Oid opid);

extern void ExecutorStart(QueryDesc *pqueryDesc, int iEFlags);

extern void ExecutorRun(QueryDesc *pqueryDesc, ScanDirection direction,
						long lCount);

extern void ExecutorEnd(QueryDesc *pqueryDesc);



#endif	// SPQDBDefs_H

// EOF

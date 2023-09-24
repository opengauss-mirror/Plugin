//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 Greenplum, Inc.
//
//	@filename:
//		SPQOptimizer.h
//
//	@doc:
//		Entry point to SPQ optimizer
//
//	@test:
//
//
//---------------------------------------------------------------------------
#ifndef SPQOptimizer_H
#define SPQOptimizer_H

#include "postgres.h"

#include "nodes/params.h"
#include "nodes/parsenodes.h"
#include "nodes/plannodes.h"

class SPQOptimizer
{
public:
	// optimize given query using SPQ optimizer
	static PlannedStmt *SPQOPTOptimizedPlan(
		Query *query,
		bool *
			had_unexpected_failure	// output : set to true if optimizer unexpectedly failed to produce plan
	);

	// serialize planned statement into DXL
	static char *SerializeDXLPlan(Query *query);

	// spqopt initialize and terminate
	static void InitSPQOPT();

	static void TerminateSPQOPT();
};

extern PlannedStmt *SPQOPTOptimizedPlan(Query *query,
									   bool *had_unexpected_failure);
extern char *SerializeDXLPlan(Query *query);
extern void InitSPQOPT();
extern void TerminateSPQOPT();
extern void UnInitSPQOPT(int status, Datum arg);

#endif	// SPQOptimizer_H

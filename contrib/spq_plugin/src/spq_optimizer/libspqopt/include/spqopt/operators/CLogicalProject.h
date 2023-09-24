//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		CLogicalProject.h
//
//	@doc:
//		Project operator
//---------------------------------------------------------------------------
#ifndef SPQOS_CLogicalProject_H
#define SPQOS_CLogicalProject_H

#include "spqos/base.h"

#include "spqopt/operators/CExpressionHandle.h"
#include "spqopt/operators/CLogicalUnary.h"

namespace spqopt
{
// fwd declaration
class CColRefSet;

//---------------------------------------------------------------------------
//	@class:
//		CLogicalProject
//
//	@doc:
//		Project operator
//
//---------------------------------------------------------------------------
class CLogicalProject : public CLogicalUnary
{
private:
	// private copy ctor
	CLogicalProject(const CLogicalProject &);

	// return equivalence class from scalar ident project element
	static CColRefSetArray *PdrspqcrsEquivClassFromScIdent(
		CMemoryPool *mp, CExpression *pexprPrEl, CColRefSet *not_null_columns);

	// extract constraint from scalar constant project element
	static void ExtractConstraintFromScConst(CMemoryPool *mp,
											 CExpression *pexprPrEl,
											 CConstraintArray *pdrspqcnstr,
											 CColRefSetArray *pdrspqcrs);

public:
	// ctor
	explicit CLogicalProject(CMemoryPool *mp);

	// dtor
	virtual ~CLogicalProject()
	{
	}

	// ident accessors
	virtual EOperatorId
	Eopid() const
	{
		return EopLogicalProject;
	}

	virtual const CHAR *
	SzId() const
	{
		return "CLogicalProject";
	}

	//-------------------------------------------------------------------------------------
	// Derived Relational Properties
	//-------------------------------------------------------------------------------------

	// derive output columns
	virtual CColRefSet *DeriveOutputColumns(CMemoryPool *mp,
											CExpressionHandle &exprhdl);

	// dervive keys
	virtual CKeyCollection *DeriveKeyCollection(
		CMemoryPool *mp, CExpressionHandle &exprhdl) const;

	// derive max card
	virtual CMaxCard DeriveMaxCard(CMemoryPool *mp,
								   CExpressionHandle &exprhdl) const;

	// derive constraint property
	virtual CPropConstraint *DerivePropertyConstraint(
		CMemoryPool *mp, CExpressionHandle &exprhdl) const;

	//-------------------------------------------------------------------------------------
	// Transformations
	//-------------------------------------------------------------------------------------

	// candidate set of xforms
	virtual CXformSet *PxfsCandidates(CMemoryPool *mp) const;

	// derive statistics
	virtual IStatistics *PstatsDerive(CMemoryPool *mp,
									  CExpressionHandle &exprhdl,
									  IStatisticsArray *stats_ctxt) const;

	//-------------------------------------------------------------------------------------
	//-------------------------------------------------------------------------------------
	//-------------------------------------------------------------------------------------

	// conversion function
	static CLogicalProject *
	PopConvert(COperator *pop)
	{
		SPQOS_ASSERT(NULL != pop);
		SPQOS_ASSERT(EopLogicalProject == pop->Eopid());

		return dynamic_cast<CLogicalProject *>(pop);
	}

};	// class CLogicalProject

}  // namespace spqopt

#endif	// !SPQOS_CLogicalProject_H

// EOF

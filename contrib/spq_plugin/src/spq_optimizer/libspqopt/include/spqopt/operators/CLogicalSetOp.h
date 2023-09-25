//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2009 Greenplum, Inc.
//
//	@filename:
//		CLogicalSetOp.h
//
//	@doc:
//		Base for set operations
//---------------------------------------------------------------------------
#ifndef SPQOS_CLogicalSetOp_H
#define SPQOS_CLogicalSetOp_H

#include "spqos/base.h"

#include "spqopt/base/CColRefSet.h"
#include "spqopt/operators/CLogical.h"

namespace spqopt
{
//---------------------------------------------------------------------------
//	@class:
//		CLogicalSetOp
//
//	@doc:
//		Base for all set operations
//
//---------------------------------------------------------------------------
class CLogicalSetOp : public CLogical
{
protected:
	// output column array
	CColRefArray *m_pdrspqcrOutput;

	// input column array
	CColRef2dArray *m_pdrspqdrspqcrInput;

	// set representation of output columns
	CColRefSet *m_pcrsOutput;

	// set representation of input columns
	CColRefSetArray *m_pdrspqcrsInput;

	// private copy ctor
	CLogicalSetOp(const CLogicalSetOp &);

	// build set representation of input/output columns for faster set operations
	void BuildColumnSets(CMemoryPool *mp);

	// output equivalence classes
	CColRefSetArray *PdrspqcrsOutputEquivClasses(CMemoryPool *mp,
												CExpressionHandle &exprhdl,
												BOOL fIntersect) const;

	// equivalence classes from one input child, mapped to output columns
	CColRefSetArray *PdrspqcrsInputMapped(CMemoryPool *mp,
										 CExpressionHandle &exprhdl,
										 ULONG ulChild) const;

	// constraints for a given output column from all children
	CConstraintArray *PdrspqcnstrColumn(CMemoryPool *mp,
									   CExpressionHandle &exprhdl,
									   ULONG ulColIndex, ULONG ulStart) const;

	// get constraint for a given output column from a given children
	CConstraint *PcnstrColumn(CMemoryPool *mp, CExpressionHandle &exprhdl,
							  ULONG ulColIndex, ULONG ulChild) const;

	// derive constraint property for difference, intersect, and union
	// operators
	CPropConstraint *PpcDeriveConstraintSetop(CMemoryPool *mp,
											  CExpressionHandle &exprhdl,
											  BOOL fIntersect) const;

public:
	// ctor
	explicit CLogicalSetOp(CMemoryPool *mp);

	CLogicalSetOp(CMemoryPool *mp, CColRefArray *pdrgOutput,
				  CColRefArray *pdrspqcrLeft, CColRefArray *pdrspqcrRight);

	CLogicalSetOp(CMemoryPool *mp, CColRefArray *pdrspqcrOutput,
				  CColRef2dArray *pdrspqdrspqcrInput);

	// dtor
	virtual ~CLogicalSetOp();

	// ident accessors
	virtual EOperatorId Eopid() const = 0;

	// return a string for operator name
	virtual const CHAR *SzId() const = 0;

	// accessor of output column array
	CColRefArray *
	PdrspqcrOutput() const
	{
		SPQOS_ASSERT(NULL != m_pdrspqcrOutput);
		return m_pdrspqcrOutput;
	}

	// accessor of input column array
	CColRef2dArray *
	PdrspqdrspqcrInput() const
	{
		SPQOS_ASSERT(NULL != m_pdrspqdrspqcrInput);
		return m_pdrspqdrspqcrInput;
	}

	// return true if we can pull projections up past this operator from its given child
	virtual BOOL FCanPullProjectionsUp(ULONG  //child_index
	) const
	{
		return false;
	}

	// match function
	BOOL Matches(COperator *pop) const;

	virtual IOstream &OsPrint(IOstream &os) const;

	//-------------------------------------------------------------------------------------
	// Derived Relational Properties
	//-------------------------------------------------------------------------------------

	// derive output columns
	virtual CColRefSet *DeriveOutputColumns(CMemoryPool *, CExpressionHandle &);

	// derive key collections
	virtual CKeyCollection *DeriveKeyCollection(
		CMemoryPool *mp, CExpressionHandle &exprhdl) const;

	// derive partition consumer info
	virtual CPartInfo *DerivePartitionInfo(CMemoryPool *mp,
										   CExpressionHandle &exprhdl) const;

	//-------------------------------------------------------------------------------------
	// Required Relational Properties
	//-------------------------------------------------------------------------------------

	// compute required stat columns of the n-th child
	virtual CColRefSet *PcrsStat(CMemoryPool *,		   // mp
								 CExpressionHandle &,  // exprhdl
								 CColRefSet *pcrsInput,
								 ULONG	// child_index
	) const;

	// conversion function
	static CLogicalSetOp *
	PopConvert(COperator *pop)
	{
		SPQOS_ASSERT(NULL != pop);
		SPQOS_ASSERT(CUtils::FLogicalSetOp(pop));

		return dynamic_cast<CLogicalSetOp *>(pop);
	}

};	// class CLogicalSetOp

}  // namespace spqopt


#endif	// !SPQOS_CLogicalSetOp_H

// EOF

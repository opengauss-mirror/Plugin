//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CLogicalSplit.h
//
//	@doc:
//		Logical split operator used for DML updates
//---------------------------------------------------------------------------
#ifndef SPQOPT_CLogicalSplit_H
#define SPQOPT_CLogicalSplit_H

#include "spqos/base.h"

#include "spqopt/operators/CExpressionHandle.h"
#include "spqopt/operators/CLogical.h"

namespace spqopt
{
// fwd declarations
class CTableDescriptor;

//---------------------------------------------------------------------------
//	@class:
//		CLogicalSplit
//
//	@doc:
//		Logical split operator
//
//---------------------------------------------------------------------------
class CLogicalSplit : public CLogical
{
private:
	// deletion columns
	CColRefArray *m_pdrspqcrDelete;

	// insertion columns
	CColRefArray *m_pdrspqcrInsert;

	// ctid column
	CColRef *m_pcrCtid;

	// segmentId column
	CColRef *m_pcrSegmentId;

	// action column
	CColRef *m_pcrAction;

	// tuple oid column
	CColRef *m_pcrTupleOid;

	// private copy ctor
	CLogicalSplit(const CLogicalSplit &);

public:
	// ctor
	explicit CLogicalSplit(CMemoryPool *mp);

	// ctor
	CLogicalSplit(CMemoryPool *mp, CColRefArray *pdrspqcrDelete,
				  CColRefArray *pdrspqcrInsert, CColRef *pcrCtid,
				  CColRef *pcrSegmentId, CColRef *pcrAction,
				  CColRef *pcrTupleOid);

	// dtor
	virtual ~CLogicalSplit();

	// ident accessors
	virtual EOperatorId
	Eopid() const
	{
		return EopLogicalSplit;
	}

	// return a string for operator name
	virtual const CHAR *
	SzId() const
	{
		return "CLogicalSplit";
	}

	// deletion columns
	CColRefArray *
	PdrspqcrDelete() const
	{
		return m_pdrspqcrDelete;
	}

	// insertion columns
	CColRefArray *
	PdrspqcrInsert() const
	{
		return m_pdrspqcrInsert;
	}

	// ctid column
	CColRef *
	PcrCtid() const
	{
		return m_pcrCtid;
	}

	// segmentId column
	CColRef *
	PcrSegmentId() const
	{
		return m_pcrSegmentId;
	}

	// action column
	CColRef *
	PcrAction() const
	{
		return m_pcrAction;
	}

	// tuple oid column
	CColRef *
	PcrTupleOid() const
	{
		return m_pcrTupleOid;
	}

	// operator specific hash function
	virtual ULONG HashValue() const;

	// match function
	BOOL Matches(COperator *pop) const;

	// sensitivity to order of inputs
	BOOL
	FInputOrderSensitive() const
	{
		return true;
	}

	// return a copy of the operator with remapped columns
	virtual COperator *PopCopyWithRemappedColumns(
		CMemoryPool *mp, UlongToColRefMap *colref_mapping, BOOL must_exist);

	//-------------------------------------------------------------------------------------
	// Derived Relational Properties
	//-------------------------------------------------------------------------------------

	// derive output columns
	virtual CColRefSet *DeriveOutputColumns(CMemoryPool *mp,
											CExpressionHandle &exprhdl);


	// derive constraint property
	virtual CPropConstraint *
	DerivePropertyConstraint(CMemoryPool *,	 // mp
							 CExpressionHandle &exprhdl) const
	{
		return CLogical::PpcDeriveConstraintPassThru(exprhdl, 0 /*ulChild*/);
	}

	// derive max card
	virtual CMaxCard DeriveMaxCard(CMemoryPool *mp,
								   CExpressionHandle &exprhdl) const;

	// derive partition consumer info
	virtual CPartInfo *
	DerivePartitionInfo(CMemoryPool *,	// mp,
						CExpressionHandle &exprhdl) const
	{
		return PpartinfoPassThruOuter(exprhdl);
	}

	// compute required stats columns of the n-th child
	virtual CColRefSet *
	PcrsStat(CMemoryPool *mp, CExpressionHandle &exprhdl, CColRefSet *pcrsInput,
			 ULONG child_index) const
	{
		return PcrsReqdChildStats(mp, exprhdl, pcrsInput,
								  exprhdl.DeriveUsedColumns(1), child_index);
	}

	//-------------------------------------------------------------------------------------
	// Transformations
	//-------------------------------------------------------------------------------------

	// candidate set of xforms
	CXformSet *PxfsCandidates(CMemoryPool *mp) const;

	// derive key collections
	virtual CKeyCollection *DeriveKeyCollection(
		CMemoryPool *mp, CExpressionHandle &exprhdl) const;

	// derive statistics
	virtual IStatistics *PstatsDerive(CMemoryPool *mp,
									  CExpressionHandle &exprhdl,
									  IStatisticsArray *stats_ctxt) const;

	// stat promise
	virtual EStatPromise
	Esp(CExpressionHandle &) const
	{
		return CLogical::EspHigh;
	}

	//-------------------------------------------------------------------------------------
	//-------------------------------------------------------------------------------------
	//-------------------------------------------------------------------------------------

	// conversion function
	static CLogicalSplit *
	PopConvert(COperator *pop)
	{
		SPQOS_ASSERT(NULL != pop);
		SPQOS_ASSERT(EopLogicalSplit == pop->Eopid());

		return dynamic_cast<CLogicalSplit *>(pop);
	}

	// debug print
	virtual IOstream &OsPrint(IOstream &) const;

};	// class CLogicalSplit
}  // namespace spqopt

#endif	// !SPQOPT_CLogicalSplit_H

// EOF

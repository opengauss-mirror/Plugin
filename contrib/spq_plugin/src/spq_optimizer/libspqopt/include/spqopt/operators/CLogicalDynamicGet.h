//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CLogicalDynamicGet.h
//
//	@doc:
//		Dynamic table accessor for partitioned tables
//---------------------------------------------------------------------------
#ifndef SPQOPT_CLogicalDynamicGet_H
#define SPQOPT_CLogicalDynamicGet_H

#include "spqos/base.h"

#include "spqopt/operators/CLogicalDynamicGetBase.h"

namespace spqopt
{
// fwd declarations
class CTableDescriptor;
class CName;
class CColRefSet;

//---------------------------------------------------------------------------
//	@class:
//		CLogicalDynamicGet
//
//	@doc:
//		Dynamic table accessor
//
//---------------------------------------------------------------------------
class CLogicalDynamicGet : public CLogicalDynamicGetBase
{
private:
	// private copy ctor
	CLogicalDynamicGet(const CLogicalDynamicGet &);

public:
	// ctors
	explicit CLogicalDynamicGet(CMemoryPool *mp);

	CLogicalDynamicGet(CMemoryPool *mp, const CName *pnameAlias,
					   CTableDescriptor *ptabdesc, ULONG ulPartIndex,
					   CColRefArray *colref_array,
					   CColRef2dArray *pdrspqdrspqcrPart,
					   ULONG ulSecondaryPartIndexId, BOOL is_partial,
					   CPartConstraint *ppartcnstr,
					   CPartConstraint *ppartcnstrRel);

	CLogicalDynamicGet(CMemoryPool *mp, const CName *pnameAlias,
					   CTableDescriptor *ptabdesc, ULONG ulPartIndex);

	// dtor
	virtual ~CLogicalDynamicGet();

	// ident accessors
	virtual EOperatorId
	Eopid() const
	{
		return EopLogicalDynamicGet;
	}

	// return a string for operator name
	virtual const CHAR *
	SzId() const
	{
		return "CLogicalDynamicGet";
	}

	// operator specific hash function
	virtual ULONG HashValue() const;

	// match function
	BOOL Matches(COperator *pop) const;

	// sensitivity to order of inputs
	BOOL FInputOrderSensitive() const;

	// return a copy of the operator with remapped columns
	virtual COperator *PopCopyWithRemappedColumns(
		CMemoryPool *mp, UlongToColRefMap *colref_mapping, BOOL must_exist);

	//-------------------------------------------------------------------------------------
	// Derived Relational Properties
	//-------------------------------------------------------------------------------------


	// derive join depth
	virtual ULONG
	DeriveJoinDepth(CMemoryPool *,		 // mp
					CExpressionHandle &	 // exprhdl
	) const
	{
		return 1;
	}

	// derive table descriptor
	virtual CTableDescriptor *
	DeriveTableDescriptor(CMemoryPool *,	   // mp
						  CExpressionHandle &  // exprhdl
	) const
	{
		return m_ptabdesc;
	}

	//-------------------------------------------------------------------------------------
	// Required Relational Properties
	//-------------------------------------------------------------------------------------

	// compute required stat columns of the n-th child
	virtual CColRefSet *
	PcrsStat(CMemoryPool *,		   // mp,
			 CExpressionHandle &,  // exprhdl
			 CColRefSet *,		   //pcrsInput
			 ULONG				   // child_index
	) const
	{
		SPQOS_ASSERT(!"CLogicalDynamicGet has no children");
		return NULL;
	}

	//-------------------------------------------------------------------------------------
	// Transformations
	//-------------------------------------------------------------------------------------

	// candidate set of xforms
	CXformSet *PxfsCandidates(CMemoryPool *mp) const;

	//-------------------------------------------------------------------------------------
	// Statistics
	//-------------------------------------------------------------------------------------

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
	static CLogicalDynamicGet *
	PopConvert(COperator *pop)
	{
		SPQOS_ASSERT(NULL != pop);
		SPQOS_ASSERT(EopLogicalDynamicGet == pop->Eopid());

		return dynamic_cast<CLogicalDynamicGet *>(pop);
	}

	// debug print
	virtual IOstream &OsPrint(IOstream &) const;

};	// class CLogicalDynamicGet

}  // namespace spqopt


#endif	// !SPQOPT_CLogicalDynamicGet_H

// EOF

//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2009 Greenplum, Inc.
//
//	@filename:
//		CLogicalGet.h
//
//	@doc:
//		Basic table accessor
//---------------------------------------------------------------------------
#ifndef SPQOPT_CLogicalGet_H
#define SPQOPT_CLogicalGet_H

#include "spqos/base.h"

#include "spqopt/operators/CLogical.h"

namespace spqopt
{
// fwd declarations
class CTableDescriptor;
class CName;
class CColRefSet;

//---------------------------------------------------------------------------
//	@class:
//		CLogicalGet
//
//	@doc:
//		Basic table accessor
//
//---------------------------------------------------------------------------
class CLogicalGet : public CLogical
{
private:
	// alias for table
	const CName *m_pnameAlias;

	// table descriptor
	CTableDescriptor *m_ptabdesc;

	// output columns
	CColRefArray *m_pdrspqcrOutput;

	// partition keys
	CColRef2dArray *m_pdrspqdrspqcrPart;

	// distribution columns (empty for master only tables)
	CColRefSet *m_pcrsDist;

	void CreatePartCols(CMemoryPool *mp, const ULongPtrArray *pdrspqulPart);

	// private copy ctor
	CLogicalGet(const CLogicalGet &);

public:
	// ctors
	explicit CLogicalGet(CMemoryPool *mp);

	CLogicalGet(CMemoryPool *mp, const CName *pnameAlias,
				CTableDescriptor *ptabdesc);

	CLogicalGet(CMemoryPool *mp, const CName *pnameAlias,
				CTableDescriptor *ptabdesc, CColRefArray *pdrspqcrOutput);

	// dtor
	virtual ~CLogicalGet();

	// ident accessors
	virtual EOperatorId
	Eopid() const
	{
		return EopLogicalGet;
	}

	// distribution columns
	virtual const CColRefSet *
	PcrsDist() const
	{
		return m_pcrsDist;
	}

	// return a string for operator name
	virtual const CHAR *
	SzId() const
	{
		return "CLogicalGet";
	}

	// accessors
	CColRefArray *
	PdrspqcrOutput() const
	{
		return m_pdrspqcrOutput;
	}

	// return table's name
	const CName &
	Name() const
	{
		return *m_pnameAlias;
	}

	// return table's descriptor
	CTableDescriptor *
	Ptabdesc() const
	{
		return m_ptabdesc;
	}

	// partition columns
	CColRef2dArray *
	PdrspqdrspqcrPartColumns() const
	{
		return m_pdrspqdrspqcrPart;
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

	// derive output columns
	virtual CColRefSet *DeriveOutputColumns(CMemoryPool *mp,
											CExpressionHandle &exprhdl);

	// derive not nullable output columns
	virtual CColRefSet *DeriveNotNullColumns(CMemoryPool *mp,
											 CExpressionHandle &exprhdl) const;

	// derive partition consumer info
	virtual CPartInfo *
	DerivePartitionInfo(CMemoryPool *mp,
						CExpressionHandle &	 // exprhdl
	) const
	{
		return SPQOS_NEW(mp) CPartInfo(mp);
	}

	// derive constraint property
	virtual CPropConstraint *
	DerivePropertyConstraint(CMemoryPool *mp,
							 CExpressionHandle &  // exprhdl
	) const
	{
		return PpcDeriveConstraintFromTable(mp, m_ptabdesc, m_pdrspqcrOutput);
	}

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
			 CColRefSet *,		   // pcrsInput
			 ULONG				   // child_index
	) const
	{
		SPQOS_ASSERT(!"CLogicalGet has no children");
		return NULL;
	}

	//-------------------------------------------------------------------------------------
	// Transformations
	//-------------------------------------------------------------------------------------

	// candidate set of xforms
	virtual CXformSet *PxfsCandidates(CMemoryPool *mp) const;

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
	static CLogicalGet *
	PopConvert(COperator *pop)
	{
		SPQOS_ASSERT(NULL != pop);
		SPQOS_ASSERT(EopLogicalGet == pop->Eopid() ||
					EopLogicalExternalGet == pop->Eopid() ||
					EopLogicalMultiExternalGet == pop->Eopid());

		return dynamic_cast<CLogicalGet *>(pop);
	}

	// debug print
	virtual IOstream &OsPrint(IOstream &) const;

};	// class CLogicalGet

}  // namespace spqopt


#endif	// !SPQOPT_CLogicalGet_H

// EOF

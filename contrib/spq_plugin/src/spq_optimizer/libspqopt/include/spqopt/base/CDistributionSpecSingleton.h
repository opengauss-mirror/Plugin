//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		CDistributionSpecSingleton.h
//
//	@doc:
//		Description of a singleton distribution;
//		Can be used as required or derived property;
//---------------------------------------------------------------------------
#ifndef SPQOPT_CDistributionSpecSingleton_H
#define SPQOPT_CDistributionSpecSingleton_H

#include "spqos/base.h"
#include "spqos/utils.h"

#include "spqopt/base/CDistributionSpec.h"

namespace spqopt
{
using namespace spqos;

//---------------------------------------------------------------------------
//	@class:
//		CDistributionSpecSingleton
//
//	@doc:
//		Class for representing singleton distribution specification.
//
//---------------------------------------------------------------------------
class CDistributionSpecSingleton : public CDistributionSpec
{
public:
	// type of segment
	enum ESegmentType
	{
		EstMaster,
		EstSegment,
		EstSentinel
	};

protected:
	// what type of segment is data distributed on
	ESegmentType m_est;

	// segment type name
	static const CHAR *m_szSegmentType[EstSentinel];

private:
	// private copy ctor
	CDistributionSpecSingleton(const CDistributionSpecSingleton &);

public:
	// ctor
	explicit CDistributionSpecSingleton(ESegmentType esegtype);

	CDistributionSpecSingleton();

	// distribution type accessor
	virtual EDistributionType
	Edt() const
	{
		return CDistributionSpec::EdtSingleton;
	}

	// segment type accessor
	ESegmentType
	Est() const
	{
		return m_est;
	}

	// is this a master-only distribution
	BOOL
	FOnMaster() const
	{
		return EstMaster == m_est;
	}

	// return distribution partitioning type
	virtual EDistributionPartitioningType
	Edpt() const
	{
		return EdptNonPartitioned;
	}

	// does this distribution satisfy the given one
	virtual BOOL FSatisfies(const CDistributionSpec *pds) const;

	// hash function for singleton distribution spec
	virtual ULONG
	HashValue() const
	{
		ULONG ulEdt = (ULONG) Edt();
		BOOL fOnMaster = FOnMaster();

		return spqos::CombineHashes(spqos::HashValue<ULONG>(&ulEdt),
								   spqos::HashValue<BOOL>(&fOnMaster));
	}

	// match function for singleton distribution specs
	virtual BOOL
	Matches(const CDistributionSpec *pds) const
	{
		return Edt() == pds->Edt() &&
			   FOnMaster() ==
				   dynamic_cast<const CDistributionSpecSingleton *>(pds)
					   ->FOnMaster();
	}

	// append enforcers to dynamic array for the given plan properties
	virtual void AppendEnforcers(CMemoryPool *mp, CExpressionHandle &exprhdl,
								 CReqdPropPlan *prpp,
								 CExpressionArray *pdrspqexpr,
								 CExpression *pexpr);

	// print
	virtual IOstream &OsPrint(IOstream &os) const;

	// conversion function
	static CDistributionSpecSingleton *
	PdssConvert(CDistributionSpec *pds)
	{
		SPQOS_ASSERT(NULL != pds);
		SPQOS_ASSERT(EdtSingleton == pds->Edt() ||
					EdtStrictSingleton == pds->Edt());

		return dynamic_cast<CDistributionSpecSingleton *>(pds);
	}

	// conversion function
	static const CDistributionSpecSingleton *
	PdssConvert(const CDistributionSpec *pds)
	{
		SPQOS_ASSERT(NULL != pds);
		SPQOS_ASSERT(EdtSingleton == pds->Edt() ||
					EdtStrictSingleton == pds->Edt());

		return dynamic_cast<const CDistributionSpecSingleton *>(pds);
	}

};	// class CDistributionSpecSingleton

}  // namespace spqopt

#endif	// !SPQOPT_CDistributionSpecSingleton_H

// EOF

//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		CDistributionSpecReplicated.h
//
//	@doc:
//		Description of a replicated distribution;
//		Can be used as required or derived property;
//---------------------------------------------------------------------------
#ifndef SPQOPT_CDistributionSpecReplicated_H
#define SPQOPT_CDistributionSpecReplicated_H

#include "spqos/base.h"

#include "spqopt/base/CDistributionSpec.h"

namespace spqopt
{
using namespace spqos;

class CDistributionSpecReplicated : public CDistributionSpec
{
private:
	CDistributionSpecReplicated(const CDistributionSpecReplicated &);

	// replicated support
	CDistributionSpec::EDistributionType m_replicated;

public:
	// ctor
	CDistributionSpecReplicated(
		CDistributionSpec::EDistributionType replicated_type)
		: m_replicated(replicated_type)
	{
		SPQOS_ASSERT(replicated_type == CDistributionSpec::EdtReplicated ||
					replicated_type ==
						CDistributionSpec::EdtTaintedReplicated ||
					replicated_type == CDistributionSpec::EdtStrictReplicated);
	}

	// accessor
	virtual EDistributionType
	Edt() const
	{
		return m_replicated;
	}

	// does this distribution satisfy the given one
	virtual BOOL FSatisfies(const CDistributionSpec *pds) const;

	// append enforcers to dynamic array for the given plan properties
	virtual void AppendEnforcers(CMemoryPool *mp, CExpressionHandle &exprhdl,
								 CReqdPropPlan *prpp,
								 CExpressionArray *pdrspqexpr,
								 CExpression *pexpr);

	// return distribution partitioning type
	virtual EDistributionPartitioningType
	Edpt() const
	{
		return EdptNonPartitioned;
	}

	// print
	virtual IOstream &
	OsPrint(IOstream &os) const
	{
		switch (Edt())
		{
			case CDistributionSpec::EdtReplicated:
				os << "REPLICATED";
				break;
			case CDistributionSpec::EdtTaintedReplicated:
				os << "TAINTED REPLICATED";
				break;
			case CDistributionSpec::EdtStrictReplicated:
				os << "STRICT REPLICATED";
				break;
			default:
				SPQOS_ASSERT(
					!"Replicated type must be General, Tainted, or Strict");
		}
		return os;
	}

	// conversion function
	static CDistributionSpecReplicated *
	PdsConvert(CDistributionSpec *pds)
	{
		SPQOS_ASSERT(NULL != pds);
		SPQOS_ASSERT(EdtStrictReplicated == pds->Edt() ||
					EdtReplicated == pds->Edt() ||
					EdtTaintedReplicated == pds->Edt());

		return dynamic_cast<CDistributionSpecReplicated *>(pds);
	}

};	// class CDistributionSpecReplicated

}  // namespace spqopt

#endif	// !SPQOPT_CDistributionSpecReplicated_H

// EOF

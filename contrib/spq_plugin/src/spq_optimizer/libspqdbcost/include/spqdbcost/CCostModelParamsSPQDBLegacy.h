//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2014 Pivotal Inc.
//
//	@filename:
//		CCostModelParamsSPQDBLegacy.h
//
//	@doc:
//		Parameters in SPQDB's legacy cost model
//---------------------------------------------------------------------------
#ifndef SPQDBCOST_CCostModelParamsSPQDBLegacy_H
#define SPQDBCOST_CCostModelParamsSPQDBLegacy_H

#include "spqos/base.h"
#include "spqos/common/CDouble.h"
#include "spqos/common/CRefCount.h"
#include "spqos/string/CWStringConst.h"

#include "spqopt/cost/ICostModelParams.h"



namespace spqopt
{
using namespace spqos;


//---------------------------------------------------------------------------
//	@class:
//		CCostModelParamsSPQDBLegacy
//
//	@doc:
//		Parameters in SPQDB's legacy cost model
//
//---------------------------------------------------------------------------
class CCostModelParamsSPQDBLegacy : public ICostModelParams
{
public:
	// enumeration of cost model params
	enum ECostParam
	{
		EcpSeqIOBandwidth = 0,	// sequential i/o bandwidth
		EcpRandomIOBandwidth,	// random i/o bandwidth
		EcpTupProcBandwidth,	// tuple processing bandwidth
		EcpTupUpdateBandwith,	// tuple update bandwidth
		EcpNetBandwidth,		// network bandwidth
		EcpSegments,			// number of segments
		EcpNLJOuterFactor,		// factor for nested loop outer child
		EcpNLJFactor,			// factor for nested loop
		EcpHJFactor,			// hash join factor - to represent spilling cost
		EcpHashFactor,			// hash building factor
		EcpDefaultCost,			// default cost

		EcpSentinel
	};

private:
	// memory pool
	CMemoryPool *m_mp;

	// array of parameters
	// cost param enum is used as index in this array
	SCostParam *m_rspqcp[EcpSentinel];

	// default value of sequential i/o bandwidth
	static const CDouble DSeqIOBandwidthVal;

	// default value of random i/o bandwidth
	static const CDouble DRandomIOBandwidthVal;

	// default value of tuple processing bandwidth
	static const CDouble DTupProcBandwidthVal;

	// default value of tuple update bandwidth
	static const CDouble DTupUpdateBandwidthVal;

	// default value of network bandwidth
	static const CDouble DNetBandwidthVal;

	// default value of number of segments
	static const CDouble DSegmentsVal;

	// default value of nested loop outer child factor
	static const CDouble DNLJOuterFactorVal;

	// default value of nested loop factor
	static const CDouble DNLJFactorVal;

	// default value of hash join factor
	static const CDouble DHJFactorVal;

	// default value of hash building factor
	static const CDouble DHashFactorVal;

	// default cost value when one is not computed
	static const CDouble DDefaultCostVal;

	// private copy ctor
	CCostModelParamsSPQDBLegacy(CCostModelParamsSPQDBLegacy &);

public:
	// ctor
	explicit CCostModelParamsSPQDBLegacy(CMemoryPool *mp);

	// dtor
	virtual ~CCostModelParamsSPQDBLegacy();

	// lookup param by id
	virtual SCostParam *PcpLookup(ULONG id) const;

	// lookup param by name
	virtual SCostParam *PcpLookup(const CHAR *szName) const;

	// set param by id
	virtual void SetParam(ULONG id, CDouble dVal, CDouble dLowerBound,
						  CDouble dUpperBound);

	// set param by name
	virtual void SetParam(const CHAR *szName, CDouble dVal, CDouble dLowerBound,
						  CDouble dUpperBound);

	// print function
	virtual IOstream &OsPrint(IOstream &os) const;

	virtual BOOL Equals(ICostModelParams *pcm) const;

	virtual const CHAR *SzNameLookup(ULONG id) const;

};	// class CCostModelParamsSPQDBLegacy

}  // namespace spqopt

#endif	// !SPQDBCOST_CCostModelParamsSPQDBLegacy_H

// EOF

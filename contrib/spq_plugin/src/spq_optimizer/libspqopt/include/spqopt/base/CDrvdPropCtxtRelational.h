//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2013 EMC CORP.
//
//	@filename:
//		CDrvdPropCtxtRelational.h
//
//	@doc:
//		Container of information passed among expression nodes during
//		derivation of relational properties
//
//---------------------------------------------------------------------------
#ifndef SPQOPT_CDrvdPropCtxtRelational_H
#define SPQOPT_CDrvdPropCtxtRelational_H

#include "spqos/base.h"

#include "spqopt/base/CDrvdPropCtxt.h"


namespace spqopt
{
using namespace spqos;

//---------------------------------------------------------------------------
//	@class:
//		CDrvdPropCtxtRelational
//
//	@doc:
//		Container of information passed among expression nodes during
//		derivation of relational properties
//
//---------------------------------------------------------------------------
class CDrvdPropCtxtRelational : public CDrvdPropCtxt
{
private:
	// private copy ctor
	CDrvdPropCtxtRelational(const CDrvdPropCtxtRelational &);

protected:
	// copy function
	virtual CDrvdPropCtxt *
	PdpctxtCopy(CMemoryPool *mp) const
	{
		return SPQOS_NEW(mp) CDrvdPropCtxtRelational(mp);
	}

	// add props to context
	virtual void
	AddProps(CDrvdProp *  // pdp
	)
	{
		// derived relational context is currently empty
	}

public:
	// ctor
	CDrvdPropCtxtRelational(CMemoryPool *mp) : CDrvdPropCtxt(mp)
	{
	}

	// dtor
	virtual ~CDrvdPropCtxtRelational()
	{
	}

#ifdef SPQOS_DEBUG

	// is it a relational property context?
	virtual BOOL
	FRelational() const
	{
		return true;
	}

#endif	// SPQOS_DEBUG

	// conversion function
	static CDrvdPropCtxtRelational *
	PdpctxtrelConvert(CDrvdPropCtxt *pdpctxt)
	{
		SPQOS_ASSERT(NULL != pdpctxt);

		return reinterpret_cast<CDrvdPropCtxtRelational *>(pdpctxt);
	}

};	// class CDrvdPropCtxtRelational

}  // namespace spqopt


#endif	// !SPQOPT_CDrvdPropCtxtRelational_H

// EOF

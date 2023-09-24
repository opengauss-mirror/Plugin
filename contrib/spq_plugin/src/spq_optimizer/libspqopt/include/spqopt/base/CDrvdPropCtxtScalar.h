//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2013 EMC CORP.
//
//	@filename:
//		CDrvdPropCtxtScalar.h
//
//	@doc:
//		Container of information passed among expression nodes during
//		derivation of scalar properties
//
//---------------------------------------------------------------------------
#ifndef SPQOPT_CDrvdPropCtxtScalar_H
#define SPQOPT_CDrvdPropCtxtScalar_H

#include "spqos/base.h"

#include "spqopt/base/CDrvdPropCtxt.h"


namespace spqopt
{
using namespace spqos;

//---------------------------------------------------------------------------
//	@class:
//		CDrvdPropCtxtScalar
//
//	@doc:
//		Container of information passed among expression nodes during
//		derivation of plan properties
//
//---------------------------------------------------------------------------
class CDrvdPropCtxtScalar : public CDrvdPropCtxt
{
private:
	// private copy ctor
	CDrvdPropCtxtScalar(const CDrvdPropCtxtScalar &);

protected:
	// copy function
	virtual CDrvdPropCtxt *
	PdpctxtCopy(CMemoryPool *mp) const
	{
		return SPQOS_NEW(mp) CDrvdPropCtxtScalar(mp);
	}

	// add props to context
	virtual void
	AddProps(CDrvdProp *  // pdp
	)
	{
		// derived scalar context is currently empty
	}

public:
	// ctor
	CDrvdPropCtxtScalar(CMemoryPool *mp) : CDrvdPropCtxt(mp)
	{
	}

	// dtor
	virtual ~CDrvdPropCtxtScalar()
	{
	}

	// print
	virtual IOstream &
	OsPrint(IOstream &os) const
	{
		return os;
	}

#ifdef SPQOS_DEBUG

	// is it a scalar property context?
	virtual BOOL
	FScalar() const
	{
		return true;
	}

#endif	// SPQOS_DEBUG

	// conversion function
	static CDrvdPropCtxtScalar *
	PdpctxtscalarConvert(CDrvdPropCtxt *pdpctxt)
	{
		SPQOS_ASSERT(NULL != pdpctxt);

		return reinterpret_cast<CDrvdPropCtxtScalar *>(pdpctxt);
	}


};	// class CDrvdPropCtxtScalar

}  // namespace spqopt


#endif	// !SPQOPT_CDrvdPropCtxtScalar_H

// EOF

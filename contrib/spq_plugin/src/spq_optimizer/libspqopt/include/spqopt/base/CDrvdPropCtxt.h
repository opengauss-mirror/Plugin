//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2013 EMC Corp.
//
//	@filename:
//		CDrvdPropCtxt.h
//
//	@doc:
//		Base class for derived properties context;
//
//---------------------------------------------------------------------------
#ifndef SPQOPT_CDrvdPropCtxt_H
#define SPQOPT_CDrvdPropCtxt_H

#include "spqos/base.h"
#include "spqos/common/CDynamicPtrArray.h"
#include "spqos/common/CRefCount.h"


namespace spqopt
{
using namespace spqos;

// fwd declarations
class CDrvdPropCtxt;
class CDrvdProp;

// dynamic array for properties
typedef CDynamicPtrArray<CDrvdPropCtxt, CleanupRelease> CDrvdPropCtxtArray;

//---------------------------------------------------------------------------
//	@class:
//		CDrvdPropCtxt
//
//	@doc:
//		Container of information passed among expression nodes during
//		property derivation
//
//---------------------------------------------------------------------------
class CDrvdPropCtxt : public CRefCount
{
private:
	// private copy ctor
	CDrvdPropCtxt(const CDrvdPropCtxt &);

protected:
	// memory pool
	CMemoryPool *m_mp;

	// copy function
	virtual CDrvdPropCtxt *PdpctxtCopy(CMemoryPool *mp) const = 0;

	// add props to context
	virtual void AddProps(CDrvdProp *pdp) = 0;

public:
	// ctor
	CDrvdPropCtxt(CMemoryPool *mp) : m_mp(mp)
	{
	}

	// dtor
	virtual ~CDrvdPropCtxt()
	{
	}

#ifdef SPQOS_DEBUG

	// is it a relational property context?
	virtual BOOL
	FRelational() const
	{
		return false;
	}

	// is it a plan property context?
	virtual BOOL
	FPlan() const
	{
		return false;
	}

	// is it a scalar property context?
	virtual BOOL
	FScalar() const
	{
		return false;
	}

#endif	// SPQOS_DEBUG

	// copy function
	static CDrvdPropCtxt *
	PdpctxtCopy(CMemoryPool *mp, CDrvdPropCtxt *pdpctxt)
	{
		if (NULL == pdpctxt)
		{
			return NULL;
		}

		return pdpctxt->PdpctxtCopy(mp);
	}

	// add derived props to context
	static void
	AddDerivedProps(CDrvdProp *pdp, CDrvdPropCtxt *pdpctxt)
	{
		if (NULL != pdpctxt)
		{
			pdpctxt->AddProps(pdp);
		}
	}

};	// class CDrvdPropCtxt

// shorthand for printing
IOstream &operator<<(IOstream &os, CDrvdPropCtxt &drvdpropctxt);

}  // namespace spqopt


#endif	// !SPQOPT_CDrvdPropCtxt_H

// EOF

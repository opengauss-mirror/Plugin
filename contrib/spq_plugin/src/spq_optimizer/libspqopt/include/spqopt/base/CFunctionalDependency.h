//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2009 - 2012 EMC CORP.
//
//	@filename:
//		CFunctionalDependency.h
//
//	@doc:
//		Functional dependency representation
//---------------------------------------------------------------------------
#ifndef SPQOPT_CFunctionalDependency_H
#define SPQOPT_CFunctionalDependency_H

#include "spqos/base.h"
#include "spqos/common/CDynamicPtrArray.h"
#include "spqos/common/CRefCount.h"

#include "spqopt/base/CColRefSet.h"


namespace spqopt
{
// fwd declarations
class CFunctionalDependency;

// definition of array of functional dependencies
typedef CDynamicPtrArray<CFunctionalDependency, CleanupRelease>
	CFunctionalDependencyArray;

using namespace spqos;

//---------------------------------------------------------------------------
//	@class:
//		CFunctionalDependency
//
//	@doc:
//		Functional dependency representation
//
//---------------------------------------------------------------------------
class CFunctionalDependency : public CRefCount,
							  public DbgPrintMixin<CFunctionalDependency>
{
private:
	// the left hand side of the FD
	CColRefSet *m_pcrsKey;

	// the right hand side of the FD
	CColRefSet *m_pcrsDetermined;

	// private copy ctor
	CFunctionalDependency(const CFunctionalDependency &);

public:
	// ctor
	CFunctionalDependency(CColRefSet *pcrsKey, CColRefSet *pcrsDetermined);

	// dtor
	virtual ~CFunctionalDependency();

	// key set accessor
	CColRefSet *
	PcrsKey() const
	{
		return m_pcrsKey;
	}

	// determined set accessor
	CColRefSet *
	PcrsDetermined() const
	{
		return m_pcrsDetermined;
	}

	// determine if all FD columns are included in the given column set
	BOOL FIncluded(CColRefSet *pcrs) const;

	// hash function
	virtual ULONG HashValue() const;

	// equality function
	BOOL Equals(const CFunctionalDependency *pfd) const;

	// do the given arguments form a functional dependency
	BOOL
	FFunctionallyDependent(CColRefSet *pcrsKey, CColRef *colref)
	{
		SPQOS_ASSERT(NULL != pcrsKey);
		SPQOS_ASSERT(NULL != colref);

		return m_pcrsKey->Equals(pcrsKey) && m_pcrsDetermined->FMember(colref);
	}

	// print
	virtual IOstream &OsPrint(IOstream &os) const;

	// hash function
	static ULONG HashValue(const CFunctionalDependencyArray *pdrspqfd);

	// equality function
	static BOOL Equals(const CFunctionalDependencyArray *pdrspqfdFst,
					   const CFunctionalDependencyArray *pdrspqfdSnd);

	// create a set of all keys in the passed FD's array
	static CColRefSet *PcrsKeys(CMemoryPool *mp,
								const CFunctionalDependencyArray *pdrspqfd);

	// create an array of all keys in the passed FD's array
	static CColRefArray *PdrspqcrKeys(CMemoryPool *mp,
									 const CFunctionalDependencyArray *pdrspqfd);


};	// class CFunctionalDependency

// shorthand for printing
inline IOstream &
operator<<(IOstream &os, CFunctionalDependency &fd)
{
	return fd.OsPrint(os);
}

}  // namespace spqopt

#endif	// !SPQOPT_CFunctionalDependency_H

// EOF

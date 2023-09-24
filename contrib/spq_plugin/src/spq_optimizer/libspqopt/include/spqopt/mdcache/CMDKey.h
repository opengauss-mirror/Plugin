//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		CMDKey.h
//
//	@doc:
//		Key for metadata objects in the cache
//---------------------------------------------------------------------------



#ifndef SPQOPT_CMDKey_H
#define SPQOPT_CMDKey_H

#include "spqos/base.h"
#include "spqos/string/CWStringConst.h"

#include "naucrates/md/IMDId.h"

namespace spqopt
{
using namespace spqos;
using namespace spqmd;



//---------------------------------------------------------------------------
//	@class:
//		CMDKey
//
//	@doc:
//		Key for metadata objects in the cache
//
//---------------------------------------------------------------------------
class CMDKey
{
private:
	// id of the object in the underlying source
	const IMDId *m_mdid;

public:
	// ctors
	explicit CMDKey(const IMDId *mdid);

	// dtor
	~CMDKey()
	{
	}


	const IMDId *
	MDId() const
	{
		return m_mdid;
	}

	// equality function
	BOOL Equals(const CMDKey &mdkey) const;

	// hash function
	ULONG HashValue() const;


	// equality function for using MD keys in a cache
	static BOOL FEqualMDKey(CMDKey *const &pvLeft, CMDKey *const &pvRight);

	// hash function for using MD keys in a cache
	static ULONG UlHashMDKey(CMDKey *const &pv);
};
}  // namespace spqopt



#endif	// !SPQOPT_CMDKey_H

// EOF

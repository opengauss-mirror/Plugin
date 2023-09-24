//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CSystemId.h
//
//	@doc:
//		Class for representing the system id of a metadata provider
//---------------------------------------------------------------------------



#ifndef SPQMD_CSystemId_H
#define SPQMD_CSystemId_H

#include "spqos/base.h"

#include "naucrates/md/IMDId.h"

#define SPQDXL_SYSID_LENGTH 10

namespace spqmd
{
using namespace spqos;


//---------------------------------------------------------------------------
//	@class:
//		CSystemId
//
//	@doc:
//		Class for representing the system id of a metadata provider
//
//---------------------------------------------------------------------------
class CSystemId
{
private:
	// system id type
	IMDId::EMDIdType m_mdid_type;

	// system id
	WCHAR m_sysid_char[SPQDXL_SYSID_LENGTH + 1];

public:
	// ctor
	CSystemId(IMDId::EMDIdType mdid_type, const WCHAR *sysid_char,
			  ULONG length = SPQDXL_SYSID_LENGTH);

	// copy ctor
	CSystemId(const CSystemId &);

	// type of system id
	IMDId::EMDIdType
	MdidType() const
	{
		return m_mdid_type;
	}

	// system id string
	const WCHAR *
	GetBuffer() const
	{
		return m_sysid_char;
	}

	// equality
	BOOL Equals(const CSystemId &sysid) const;

	// hash function
	ULONG HashValue() const;
};

// dynamic arrays over md system id elements
typedef CDynamicPtrArray<CSystemId, CleanupDelete> CSystemIdArray;
}  // namespace spqmd



#endif	// !SPQMD_CSystemId_H

// EOF

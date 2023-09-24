//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2013 EMC Corp.
//
//	@filename:
//		CMDIdCast.h
//
//	@doc:
//		Class for representing mdids of cast functions
//---------------------------------------------------------------------------

#ifndef SPQMD_CMDIdCastFunc_H
#define SPQMD_CMDIdCastFunc_H

#include "spqos/base.h"
#include "spqos/common/CDynamicPtrArray.h"
#include "spqos/string/CWStringConst.h"

#include "naucrates/dxl/spqdb_types.h"
#include "naucrates/md/CMDIdSPQDB.h"
#include "naucrates/md/CSystemId.h"

namespace spqmd
{
using namespace spqos;


//---------------------------------------------------------------------------
//	@class:
//		CMDIdCast
//
//	@doc:
//		Class for representing ids of cast objects
//
//---------------------------------------------------------------------------
class CMDIdCast : public IMDId
{
private:
	// mdid of source type
	CMDIdSPQDB *m_mdid_src;

	// mdid of destinatin type
	CMDIdSPQDB *m_mdid_dest;


	// buffer for the serialized mdid
	WCHAR m_mdid_buffer[SPQDXL_MDID_LENGTH];

	// string representation of the mdid
	CWStringStatic m_str;

	// private copy ctor
	CMDIdCast(const CMDIdCast &);

	// serialize mdid
	void Serialize();

public:
	// ctor
	CMDIdCast(CMDIdSPQDB *mdid_src, CMDIdSPQDB *mdid_dest);

	// dtor
	virtual ~CMDIdCast();

	virtual EMDIdType
	MdidType() const
	{
		return EmdidCastFunc;
	}

	// string representation of mdid
	virtual const WCHAR *GetBuffer() const;

	// source system id
	virtual CSystemId
	Sysid() const
	{
		return m_mdid_src->Sysid();
	}

	// source type id
	IMDId *MdidSrc() const;

	// destination type id
	IMDId *MdidDest() const;

	// equality check
	virtual BOOL Equals(const IMDId *mdid) const;

	// computes the hash value for the metadata id
	virtual ULONG
	HashValue() const
	{
		return spqos::CombineHashes(
			MdidType(), spqos::CombineHashes(m_mdid_src->HashValue(),
											m_mdid_dest->HashValue()));
	}

	// is the mdid valid
	virtual BOOL
	IsValid() const
	{
		return IMDId::IsValid(m_mdid_src) && IMDId::IsValid(m_mdid_dest);
	}

	// serialize mdid in DXL as the value of the specified attribute
	virtual void Serialize(CXMLSerializer *xml_serializer,
						   const CWStringConst *pstrAttribute) const;

	// debug print of the metadata id
	virtual IOstream &OsPrint(IOstream &os) const;

	// const converter
	static const CMDIdCast *
	CastMdid(const IMDId *mdid)
	{
		SPQOS_ASSERT(NULL != mdid && EmdidCastFunc == mdid->MdidType());

		return dynamic_cast<const CMDIdCast *>(mdid);
	}

	// non-const converter
	static CMDIdCast *
	CastMdid(IMDId *mdid)
	{
		SPQOS_ASSERT(NULL != mdid && EmdidCastFunc == mdid->MdidType());

		return dynamic_cast<CMDIdCast *>(mdid);
	}
};
}  // namespace spqmd

#endif	// !SPQMD_CMDIdCastFunc_H

// EOF

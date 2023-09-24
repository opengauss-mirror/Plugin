//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2013 EMC Corp.
//
//	@filename:
//		CMDIdScCmp.h
//
//	@doc:
//		Class for representing mdids of scalar comparison operators
//---------------------------------------------------------------------------

#ifndef SPQMD_CMDIdScCmpFunc_H
#define SPQMD_CMDIdScCmpFunc_H

#include "spqos/base.h"

#include "naucrates/md/CMDIdSPQDB.h"
#include "naucrates/md/IMDType.h"

namespace spqmd
{
using namespace spqos;


//---------------------------------------------------------------------------
//	@class:
//		CMDIdScCmp
//
//	@doc:
//		Class for representing ids of scalar comparison operators
//
//---------------------------------------------------------------------------
class CMDIdScCmp : public IMDId
{
private:
	// mdid of source type
	CMDIdSPQDB *m_mdid_left;

	// mdid of destinatin type
	CMDIdSPQDB *m_mdid_right;

	// comparison type
	IMDType::ECmpType m_comparision_type;

	// buffer for the serialized mdid
	WCHAR m_mdid_array[SPQDXL_MDID_LENGTH];

	// string representation of the mdid
	CWStringStatic m_str;

	// private copy ctor
	CMDIdScCmp(const CMDIdScCmp &);

	// serialize mdid
	void Serialize();

public:
	// ctor
	CMDIdScCmp(CMDIdSPQDB *left_mdid, CMDIdSPQDB *right_mdid,
			   IMDType::ECmpType cmp_type);

	// dtor
	virtual ~CMDIdScCmp();

	virtual EMDIdType
	MdidType() const
	{
		return EmdidScCmp;
	}

	// string representation of mdid
	virtual const WCHAR *GetBuffer() const;

	// source system id
	virtual CSystemId
	Sysid() const
	{
		return m_mdid_left->Sysid();
	}

	// left type id
	IMDId *GetLeftMdid() const;

	// right type id
	IMDId *GetRightMdid() const;

	IMDType::ECmpType
	ParseCmpType() const
	{
		return m_comparision_type;
	}

	// equality check
	virtual BOOL Equals(const IMDId *mdid) const;

	// computes the hash value for the metadata id
	virtual ULONG HashValue() const;

	// is the mdid valid
	virtual BOOL
	IsValid() const
	{
		return IMDId::IsValid(m_mdid_left) && IMDId::IsValid(m_mdid_right) &&
			   IMDType::EcmptOther != m_comparision_type;
	}

	// serialize mdid in DXL as the value of the specified attribute
	virtual void Serialize(CXMLSerializer *xml_serializer,
						   const CWStringConst *attribute_str) const;

	// debug print of the metadata id
	virtual IOstream &OsPrint(IOstream &os) const;

	// const converter
	static const CMDIdScCmp *
	CastMdid(const IMDId *mdid)
	{
		SPQOS_ASSERT(NULL != mdid && EmdidScCmp == mdid->MdidType());

		return dynamic_cast<const CMDIdScCmp *>(mdid);
	}

	// non-const converter
	static CMDIdScCmp *
	CastMdid(IMDId *mdid)
	{
		SPQOS_ASSERT(NULL != mdid && EmdidScCmp == mdid->MdidType());

		return dynamic_cast<CMDIdScCmp *>(mdid);
	}
};
}  // namespace spqmd

#endif	// !SPQMD_CMDIdScCmpFunc_H

// EOF

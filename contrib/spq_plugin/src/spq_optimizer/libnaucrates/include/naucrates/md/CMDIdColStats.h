//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CMDIdColStats.h
//
//	@doc:
//		Class for representing mdids for column statistics
//---------------------------------------------------------------------------



#ifndef SPQMD_CMDIdColStats_H
#define SPQMD_CMDIdColStats_H

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
//		CMDIdColStats
//
//	@doc:
//		Class for representing ids of column stats objects
//
//---------------------------------------------------------------------------
class CMDIdColStats : public IMDId
{
private:
	// mdid of base relation
	CMDIdSPQDB *m_rel_mdid;

	// position of the attribute in the base relation
	ULONG m_attr_pos;

	// buffer for the serialized mdid
	WCHAR m_mdid_buffer[SPQDXL_MDID_LENGTH];

	// string representation of the mdid
	CWStringStatic m_str;

	// private copy ctor
	CMDIdColStats(const CMDIdColStats &);

	// serialize mdid
	void Serialize();

public:
	// ctor
	CMDIdColStats(CMDIdSPQDB *rel_mdid, ULONG attno);

	// dtor
	virtual ~CMDIdColStats();

	virtual EMDIdType
	MdidType() const
	{
		return EmdidColStats;
	}

	// string representation of mdid
	virtual const WCHAR *GetBuffer() const;

	// source system id
	virtual CSystemId
	Sysid() const
	{
		return m_rel_mdid->Sysid();
	}

	// accessors
	IMDId *GetRelMdId() const;
	ULONG Position() const;

	// equality check
	virtual BOOL Equals(const IMDId *mdid) const;

	// computes the hash value for the metadata id
	virtual ULONG
	HashValue() const
	{
		return spqos::CombineHashes(m_rel_mdid->HashValue(),
								   spqos::HashValue(&m_attr_pos));
	}

	// is the mdid valid
	virtual BOOL
	IsValid() const
	{
		return IMDId::IsValid(m_rel_mdid);
	}

	// serialize mdid in DXL as the value of the specified attribute
	virtual void Serialize(CXMLSerializer *xml_serializer,
						   const CWStringConst *attribute_str) const;

	// debug print of the metadata id
	virtual IOstream &OsPrint(IOstream &os) const;

	// const converter
	static const CMDIdColStats *
	CastMdid(const IMDId *mdid)
	{
		SPQOS_ASSERT(NULL != mdid && EmdidColStats == mdid->MdidType());

		return dynamic_cast<const CMDIdColStats *>(mdid);
	}

	// non-const converter
	static CMDIdColStats *
	CastMdid(IMDId *mdid)
	{
		SPQOS_ASSERT(NULL != mdid && EmdidColStats == mdid->MdidType());

		return dynamic_cast<CMDIdColStats *>(mdid);
	}
};

}  // namespace spqmd



#endif	// !SPQMD_CMDIdColStats_H

// EOF

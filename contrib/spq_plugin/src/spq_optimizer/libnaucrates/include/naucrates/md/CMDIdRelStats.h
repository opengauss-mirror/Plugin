//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CMDIdRelStats.h
//
//	@doc:
//		Class for representing mdids for relation statistics
//---------------------------------------------------------------------------



#ifndef SPQMD_CMDIdRelStats_H
#define SPQMD_CMDIdRelStats_H

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
//		CMDIdRelStats
//
//	@doc:
//		Class for representing ids of relation stats objects
//
//---------------------------------------------------------------------------
class CMDIdRelStats : public IMDId
{
private:
	// mdid of base relation
	CMDIdSPQDB *m_rel_mdid;

	// buffer for the serialzied mdid
	WCHAR m_mdid_array[SPQDXL_MDID_LENGTH];

	// string representation of the mdid
	CWStringStatic m_str;

	// private copy ctor
	CMDIdRelStats(const CMDIdRelStats &);

	// serialize mdid
	void Serialize();

public:
	// ctor
	explicit CMDIdRelStats(CMDIdSPQDB *rel_mdid);

	// dtor
	virtual ~CMDIdRelStats();

	virtual EMDIdType
	MdidType() const
	{
		return EmdidRelStats;
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

	// equality check
	virtual BOOL Equals(const IMDId *mdid) const;

	// computes the hash value for the metadata id
	virtual ULONG
	HashValue() const
	{
		return m_rel_mdid->HashValue();
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
	static const CMDIdRelStats *
	CastMdid(const IMDId *mdid)
	{
		SPQOS_ASSERT(NULL != mdid && EmdidRelStats == mdid->MdidType());

		return dynamic_cast<const CMDIdRelStats *>(mdid);
	}

	// non-const converter
	static CMDIdRelStats *
	CastMdid(IMDId *mdid)
	{
		SPQOS_ASSERT(NULL != mdid && EmdidRelStats == mdid->MdidType());

		return dynamic_cast<CMDIdRelStats *>(mdid);
	}
};

}  // namespace spqmd



#endif	// !SPQMD_CMDIdRelStats_H

// EOF

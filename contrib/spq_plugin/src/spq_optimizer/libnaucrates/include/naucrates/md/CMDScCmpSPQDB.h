//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2013 EMC Corp.
//
//	@filename:
//		CMDScCmpSPQDB.h
//
//	@doc:
//		Implementation of SPQDB-specific scalar comparison operators in the MD cache
//---------------------------------------------------------------------------


#ifndef SPQMD_CMDScCmpSPQDB_H
#define SPQMD_CMDScCmpSPQDB_H

#include "spqos/base.h"

#include "naucrates/md/IMDScCmp.h"

namespace spqmd
{
using namespace spqdxl;

//---------------------------------------------------------------------------
//	@class:
//		CMDScCmpSPQDB
//
//	@doc:
//		Implementation for SPQDB-specific scalar comparison operators in the
//		MD cache
//
//---------------------------------------------------------------------------
class CMDScCmpSPQDB : public IMDScCmp
{
private:
	// memory pool
	CMemoryPool *m_mp;

	// DXL for object
	const CWStringDynamic *m_dxl_str;

	// object id
	IMDId *m_mdid;

	// operator name
	CMDName *m_mdname;

	// left type
	IMDId *m_mdid_left;

	// right type
	IMDId *m_mdid_right;

	// comparison type
	IMDType::ECmpType m_comparision_type;

	// comparison operator id
	IMDId *m_mdid_op;

	// private copy ctor
	CMDScCmpSPQDB(const CMDScCmpSPQDB &);

public:
	// ctor
	CMDScCmpSPQDB(CMemoryPool *mp, IMDId *mdid, CMDName *mdname,
				 IMDId *left_mdid, IMDId *right_mdid,
				 IMDType::ECmpType cmp_type, IMDId *mdid_op);

	// dtor
	virtual ~CMDScCmpSPQDB();

	// accessors
	virtual const CWStringDynamic *
	GetStrRepr() const
	{
		return m_dxl_str;
	}

	// copmarison object id
	virtual IMDId *MDId() const;

	// cast object name
	virtual CMDName Mdname() const;

	// left type
	virtual IMDId *GetLeftMdid() const;

	// right type
	virtual IMDId *GetRightMdid() const;

	// comparison type
	virtual IMDType::ECmpType ParseCmpType() const;

	// comparison operator id
	virtual IMDId *MdIdOp() const;

	// serialize object in DXL format
	virtual void Serialize(spqdxl::CXMLSerializer *xml_serializer) const;

#ifdef SPQOS_DEBUG
	// debug print of the type in the provided stream
	virtual void DebugPrint(IOstream &os) const;
#endif
};
}  // namespace spqmd

#endif	// !SPQMD_CMDScCmpSPQDB_H

// EOF

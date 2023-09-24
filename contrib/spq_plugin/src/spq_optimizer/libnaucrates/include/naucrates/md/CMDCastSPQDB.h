//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2013 EMC Corp.
//
//	@filename:
//		CMDCastSPQDB.h
//
//	@doc:
//		Implementation of SPQDB-specific cast functions in the metadata cache
//---------------------------------------------------------------------------


#ifndef SPQMD_CMDCastSPQDB_H
#define SPQMD_CMDCastSPQDB_H

#include "spqos/base.h"

#include "naucrates/md/IMDCast.h"

namespace spqmd
{
using namespace spqdxl;

//---------------------------------------------------------------------------
//	@class:
//		CMDCastSPQDB
//
//	@doc:
//		Implementation for SPQDB-specific cast functions in the metadata cache
//
//---------------------------------------------------------------------------
class CMDCastSPQDB : public IMDCast
{
private:
	// private copy ctor
	CMDCastSPQDB(const CMDCastSPQDB &);

protected:
	// memory pool
	CMemoryPool *m_mp;

	// DXL for object
	const CWStringDynamic *m_dxl_str;

	// func id
	IMDId *m_mdid;

	// func name
	CMDName *m_mdname;

	// source type
	IMDId *m_mdid_src;

	// destination type
	IMDId *m_mdid_dest;

	// is cast between binary coercible types, i.e. the types are binary compatible
	BOOL m_is_binary_coercible;

	// cast func id
	IMDId *m_mdid_cast_func;

	// coercion path type
	EmdCoercepathType m_path_type;

public:
	// ctor
	CMDCastSPQDB(CMemoryPool *mp, IMDId *mdid, CMDName *mdname, IMDId *mdid_src,
				IMDId *mdid_dest, BOOL is_binary_coercible,
				IMDId *mdid_cast_func, EmdCoercepathType path_type = EmdtNone);

	// dtor
	virtual ~CMDCastSPQDB();

	// accessors
	virtual const CWStringDynamic *
	GetStrRepr() const
	{
		return m_dxl_str;
	}

	// cast object id
	virtual IMDId *MDId() const;

	// cast object name
	virtual CMDName Mdname() const;

	// source type
	virtual IMDId *MdidSrc() const;

	// destination type
	virtual IMDId *MdidDest() const;

	// is this a cast between binary coeercible types, i.e. the types are binary compatible
	virtual BOOL IsBinaryCoercible() const;

	// return the coercion path type
	virtual EmdCoercepathType GetMDPathType() const;

	// cast function id
	virtual IMDId *GetCastFuncMdId() const;

	// serialize object in DXL format
	virtual void Serialize(spqdxl::CXMLSerializer *xml_serializer) const;

#ifdef SPQOS_DEBUG
	// debug print of the type in the provided stream
	virtual void DebugPrint(IOstream &os) const;
#endif
};
}  // namespace spqmd

#endif	// !SPQMD_CMDCastSPQDB_H

// EOF

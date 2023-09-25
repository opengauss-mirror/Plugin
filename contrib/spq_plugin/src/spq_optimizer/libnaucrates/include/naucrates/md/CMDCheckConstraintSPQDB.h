//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CMDCheckConstraintSPQDB.h
//
//	@doc:
//		Class representing a SPQDB check constraint in a metadata cache relation
//---------------------------------------------------------------------------

#ifndef SPQMD_CMDCheckConstraintSPQDB_H
#define SPQMD_CMDCheckConstraintSPQDB_H

#include "spqos/base.h"
#include "spqos/string/CWStringDynamic.h"

#include "naucrates/md/CMDName.h"
#include "naucrates/md/IMDCheckConstraint.h"

// fwd decl
namespace spqdxl
{
class CXMLSerializer;
}

namespace spqmd
{
using namespace spqos;
using namespace spqdxl;

//---------------------------------------------------------------------------
//	@class:
//		CMDCheckConstraintSPQDB
//
//	@doc:
//		Class representing a SPQDB check constraint in a metadata cache relation
//
//---------------------------------------------------------------------------
class CMDCheckConstraintSPQDB : public IMDCheckConstraint
{
private:
	// memory pool
	CMemoryPool *m_mp;

	// check constraint mdid
	IMDId *m_mdid;

	// check constraint name
	CMDName *m_mdname;

	// relation mdid
	IMDId *m_rel_mdid;

	// the DXL representation of the check constraint
	CDXLNode *m_dxl_node;

	// DXL for object
	const CWStringDynamic *m_dxl_str;

public:
	// ctor
	CMDCheckConstraintSPQDB(CMemoryPool *mp, IMDId *mdid, CMDName *mdname,
						   IMDId *rel_mdid, CDXLNode *dxlnode);

	// dtor
	virtual ~CMDCheckConstraintSPQDB();

	// check constraint mdid
	virtual IMDId *
	MDId() const
	{
		return m_mdid;
	}

	// check constraint name
	virtual CMDName
	Mdname() const
	{
		return *m_mdname;
	}

	// mdid of the relation
	virtual IMDId *
	GetRelMdId() const
	{
		return m_rel_mdid;
	}

	// DXL string for check constraint
	virtual const CWStringDynamic *
	GetStrRepr() const
	{
		return m_dxl_str;
	}

	// the scalar expression of the check constraint
	virtual CExpression *GetCheckConstraintExpr(
		CMemoryPool *mp, CMDAccessor *md_accessor,
		CColRefArray *colref_array) const;

	// serialize MD check constraint in DXL format given a serializer object
	virtual void Serialize(spqdxl::CXMLSerializer *) const;

#ifdef SPQOS_DEBUG
	// debug print of the MD check constraint
	virtual void DebugPrint(IOstream &os) const;
#endif
};
}  // namespace spqmd

#endif	// !SPQMD_CMDCheckConstraintSPQDB_H

// EOF

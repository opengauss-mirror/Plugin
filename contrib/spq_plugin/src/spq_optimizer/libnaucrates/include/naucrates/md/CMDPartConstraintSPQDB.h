//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CMDPartConstraintSPQDB.h
//
//	@doc:
//		Class representing a SPQDB partition constraint in the MD cache
//---------------------------------------------------------------------------

#ifndef SPQMD_CMDPartConstraintSPQDB_H
#define SPQMD_CMDPartConstraintSPQDB_H

#include "spqos/base.h"
#include "spqos/string/CWStringDynamic.h"

#include "naucrates/md/CMDName.h"
#include "naucrates/md/IMDPartConstraint.h"

// fwd decl
namespace spqdxl
{
class CXMLSerializer;
}

namespace spqopt
{
class CExpression;
class CMDAccessor;
}  // namespace spqopt

namespace spqmd
{
using namespace spqos;
using namespace spqdxl;

//---------------------------------------------------------------------------
//	@class:
//		CMDPartConstraintSPQDB
//
//	@doc:
//		Class representing a SPQDB partition constraint in the MD cache
//
//---------------------------------------------------------------------------
class CMDPartConstraintSPQDB : public IMDPartConstraint
{
private:
	// memory pool
	CMemoryPool *m_mp;

	// included default partitions
	ULongPtrArray *m_level_with_default_part_array;

	// is constraint unbounded
	BOOL m_is_unbounded;

	// the DXL representation of the part constraint
	CDXLNode *m_dxl_node;

public:
	// ctor
	CMDPartConstraintSPQDB(CMemoryPool *mp,
						  ULongPtrArray *level_with_default_part_array,
						  BOOL is_unbounded, CDXLNode *dxlnode);

	// dtor
	virtual ~CMDPartConstraintSPQDB();

	// serialize constraint in DXL format
	virtual void Serialize(CXMLSerializer *xml_serializer) const;

	// the scalar expression of the part constraint
	virtual CExpression *GetPartConstraintExpr(
		CMemoryPool *mp, CMDAccessor *md_accessor,
		CColRefArray *colref_array) const;

	// included default partitions
	virtual ULongPtrArray *GetDefaultPartsArray() const;

	// is constraint unbounded
	virtual BOOL IsConstraintUnbounded() const;
};
}  // namespace spqmd

#endif	// !SPQMD_CMDPartConstraintSPQDB_H

// EOF

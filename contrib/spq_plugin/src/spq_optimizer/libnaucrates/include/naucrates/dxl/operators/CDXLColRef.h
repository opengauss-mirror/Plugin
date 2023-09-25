//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2010 Greenplum, Inc.
//
//	@filename:
//		CDXLColRef.h
//
//	@doc:
//		Class for representing column references.
//---------------------------------------------------------------------------



#ifndef SPQDXL_CDXLColRef_H
#define SPQDXL_CDXLColRef_H

#include "spqos/base.h"
#include "spqos/common/CDynamicPtrArray.h"
#include "spqos/common/CRefCount.h"

#include "naucrates/md/CMDName.h"
#include "naucrates/md/IMDId.h"

namespace spqdxl
{
using namespace spqmd;
using namespace spqos;

// fwd decl
class CXMLSerializer;
class CDXLColRef;

// arrays of column references
typedef CDynamicPtrArray<CDXLColRef, CleanupRelease> CDXLColRefArray;

//---------------------------------------------------------------------------
//	@class:
//		CDXLColRef
//
//	@doc:
//		Class for representing references to columns in DXL trees
//
//---------------------------------------------------------------------------
class CDXLColRef : public CRefCount
{
private:
	// memory pool
	CMemoryPool *m_mp;

	// name
	CMDName *m_mdname;

	// id
	const ULONG m_id;

	// column type
	IMDId *m_mdid_type;

	// column type modifier
	INT m_iTypeModifer;

	// private copy ctor
	CDXLColRef(const CDXLColRef &);

public:
	// ctor/dtor
	CDXLColRef(CMemoryPool *mp, CMDName *mdname, ULONG id, IMDId *mdid_type,
			   INT type_modifier);

	~CDXLColRef();

	// accessors
	const CMDName *MdName() const;

	IMDId *MdidType() const;

	INT TypeModifier() const;

	ULONG Id() const;
};
}  // namespace spqdxl



#endif	// !SPQDXL_CDXLColRef_H

// EOF

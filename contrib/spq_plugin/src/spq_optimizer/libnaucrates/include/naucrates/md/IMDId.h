//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		IMDId.h
//
//	@doc:
//		Abstract class for representing metadata object ids
//---------------------------------------------------------------------------



#ifndef SPQMD_IMDId_H
#define SPQMD_IMDId_H

#include "spqos/base.h"
#include "spqos/common/CDynamicPtrArray.h"
#include "spqos/common/CHashSet.h"
#include "spqos/common/CHashSetIter.h"
#include "spqos/common/DbgPrintMixin.h"
#include "spqos/string/CWStringConst.h"

#include "naucrates/dxl/spqdb_types.h"

#define SPQDXL_MDID_LENGTH 100

// fwd decl
namespace spqdxl
{
class CXMLSerializer;
}

namespace spqmd
{
using namespace spqos;
using namespace spqdxl;

// fwd decl
class CSystemId;

static const INT default_type_modifier = -1;

//---------------------------------------------------------------------------
//	@class:
//		IMDId
//
//	@doc:
//		Abstract class for representing metadata objects ids
//
//---------------------------------------------------------------------------
class IMDId : public CRefCount, public DbgPrintMixin<IMDId>
{
private:
	// number of deletion locks -- each MDAccessor adds a new deletion lock if it uses
	// an MDId object in its internal hash-table, the deletion lock is released when
	// MDAccessor is destroyed
	ULONG_PTR m_deletion_locks;

public:
	//------------------------------------------------------------------
	//	@doc:
	//		Type of md id.
	//		The exact values are important when parsing mdids from DXL and
	//		should not be modified without modifying the parser
	//
	//------------------------------------------------------------------
	enum EMDIdType
	{
		EmdidGeneral = 0,
		EmdidColStats = 1,
		EmdidRelStats = 2,
		EmdidCastFunc = 3,
		EmdidScCmp = 4,
		EmdidSPQDBCtas = 5,
		EmdidRel = 6,
		EmdidInd = 7,
		EmdidCheckConstraint = 8,
		EmdidSentinel
	};

	// ctor
	IMDId() : m_deletion_locks(0)
	{
	}

	// dtor
	virtual ~IMDId(){};

	// type of mdid
	virtual EMDIdType MdidType() const = 0;

	// string representation of mdid
	virtual const WCHAR *GetBuffer() const = 0;

	// system id
	virtual CSystemId Sysid() const = 0;


	// equality check
	virtual BOOL Equals(const IMDId *mdid) const = 0;

	// computes the hash value for the metadata id
	virtual ULONG HashValue() const = 0;

	// return true if calling object's destructor is allowed
	virtual BOOL
	Deletable() const
	{
		return (0 == m_deletion_locks);
	}

	// increase number of deletion locks
	void
	AddDeletionLock()
	{
		m_deletion_locks++;
	}

	// decrease number of deletion locks
	void
	RemoveDeletionLock()
	{
		SPQOS_ASSERT(0 < m_deletion_locks);

		m_deletion_locks--;
	}

	// return number of deletion locks
	ULONG_PTR
	DeletionLocks() const
	{
		return m_deletion_locks;
	}

	// static hash functions for use in different indexing structures,
	// e.g. hashmaps, MD cache, etc.
	static ULONG
	MDIdHash(const IMDId *mdid)
	{
		SPQOS_ASSERT(NULL != mdid);
		return mdid->HashValue();
	}

	// static equality functions for use in different structures,
	// e.g. hashmaps, MD cache, etc.
	static BOOL
	MDIdCompare(const IMDId *left_mdid, const IMDId *right_mdid)
	{
		SPQOS_ASSERT(NULL != left_mdid && NULL != right_mdid);
		return left_mdid->Equals(right_mdid);
	}


	// is the mdid valid
	virtual BOOL IsValid() const = 0;

	// serialize mdid in DXL as the value for the specified attribute
	virtual void Serialize(CXMLSerializer *xml_serializer,
						   const CWStringConst *pstrAttribute) const = 0;

	// safe validity function
	static BOOL
	IsValid(const IMDId *mdid)
	{
		return NULL != mdid && mdid->IsValid();
	}
};

// common structures over metadata id elements
typedef CDynamicPtrArray<IMDId, CleanupRelease> IMdIdArray;

// hash set for mdid
typedef CHashSet<IMDId, IMDId::MDIdHash, IMDId::MDIdCompare,
				 CleanupRelease<IMDId> >
	MdidHashSet;

// iterator over the hash set for column id information for missing statistics
typedef CHashSetIter<IMDId, IMDId::MDIdHash, IMDId::MDIdCompare,
					 CleanupRelease<IMDId> >
	MdidHashSetIter;
}  // namespace spqmd

FORCE_GENERATE_DBGSTR(spqmd::IMDId);

#endif	// !SPQMD_IMDId_H

// EOF

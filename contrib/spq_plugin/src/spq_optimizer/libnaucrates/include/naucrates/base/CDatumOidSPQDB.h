//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CDatumOidSPQDB.h
//
//	@doc:
//		SPQDB-specific oid representation
//---------------------------------------------------------------------------
#ifndef SPQNAUCRATES_CDatumOidSPQDB_H
#define SPQNAUCRATES_CDatumOidSPQDB_H

#include "spqos/base.h"

#include "naucrates/base/IDatumOid.h"

namespace spqnaucrates
{
//---------------------------------------------------------------------------
//	@class:
//		CDatumOidSPQDB
//
//	@doc:
//		SPQDB-specific oid representation
//
//---------------------------------------------------------------------------
class CDatumOidSPQDB : public IDatumOid
{
private:
	// type information
	IMDId *m_mdid;

	// oid value
	OID m_val;

	// is null
	BOOL m_is_null;

	// private copy ctor
	CDatumOidSPQDB(const CDatumOidSPQDB &);

public:
	// ctors
	CDatumOidSPQDB(CSystemId sysid, OID oid_val, BOOL is_null = false);
	CDatumOidSPQDB(IMDId *mdid, OID oid_val, BOOL is_null = false);

	// dtor
	virtual ~CDatumOidSPQDB();

	// accessor of metadata type id
	virtual IMDId *MDId() const;

	// accessor of size
	virtual ULONG Size() const;

	// accessor of oid value
	virtual OID OidValue() const;

	// accessor of is null
	virtual BOOL IsNull() const;

	// return string representation
	virtual const CWStringConst *GetStrRepr(CMemoryPool *mp) const;

	// hash function
	virtual ULONG HashValue() const;

	// match function for datums
	virtual BOOL Matches(const IDatum *) const;

	// copy datum
	virtual IDatum *MakeCopy(CMemoryPool *mp) const;

	// print function
	virtual IOstream &OsPrint(IOstream &os) const;

};	// class CDatumOidSPQDB
}  // namespace spqnaucrates

#endif	// !SPQNAUCRATES_CDatumOidSPQDB_H

// EOF

//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		CDatumBoolSPQDB.h
//
//	@doc:
//		SPQDB-specific bool representation
//---------------------------------------------------------------------------
#ifndef SPQNAUCRATES_CDatumBoolSPQDB_H
#define SPQNAUCRATES_CDatumBoolSPQDB_H

#include "spqos/base.h"

#include "naucrates/base/IDatumBool.h"
#include "naucrates/md/CMDTypeBoolSPQDB.h"

namespace spqnaucrates
{
//---------------------------------------------------------------------------
//	@class:
//		CDatumBoolSPQDB
//
//	@doc:
//		SPQDB-specific bool representation
//
//---------------------------------------------------------------------------
class CDatumBoolSPQDB : public IDatumBool
{
private:
	// type information
	IMDId *m_mdid;

	// boolean value
	BOOL m_value;

	// is null
	BOOL m_is_null;

	// private copy ctor
	CDatumBoolSPQDB(const CDatumBoolSPQDB &);

public:
	// ctors
	CDatumBoolSPQDB(CSystemId sysid, BOOL value, BOOL is_null = false);
	CDatumBoolSPQDB(IMDId *mdid, BOOL value, BOOL is_null = false);

	// dtor
	virtual ~CDatumBoolSPQDB();

	// accessor of metadata type mdid
	virtual IMDId *MDId() const;

	// accessor of boolean value
	virtual BOOL GetValue() const;

	// accessor of size
	virtual ULONG Size() const;

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

};	// class CDatumBoolSPQDB
}  // namespace spqnaucrates

#endif	// !SPQNAUCRATES_CDatumBoolSPQDB_H

// EOF

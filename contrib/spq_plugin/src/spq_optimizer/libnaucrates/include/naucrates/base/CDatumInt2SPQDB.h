//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2014 Pivotal Inc.
//
//	@filename:
//		CDatumInt2SPQDB.h
//
//	@doc:
//		SPQDB-specific int2 representation
//---------------------------------------------------------------------------
#ifndef SPQNAUCRATES_CDatumInt2SPQDB_H
#define SPQNAUCRATES_CDatumInt2SPQDB_H

#include "spqos/base.h"

#include "naucrates/base/IDatumInt2.h"
#include "naucrates/md/CMDTypeInt2SPQDB.h"

namespace spqnaucrates
{
//---------------------------------------------------------------------------
//	@class:
//		CDatumInt2SPQDB
//
//	@doc:
//		SPQDB-specific int2 representation
//
//---------------------------------------------------------------------------
class CDatumInt2SPQDB : public IDatumInt2
{
private:
	// type information
	IMDId *m_mdid;

	// integer value
	SINT m_val;

	// is null
	BOOL m_is_null;

	// private copy ctor
	CDatumInt2SPQDB(const CDatumInt2SPQDB &);

public:
	// ctors
	CDatumInt2SPQDB(CSystemId sysid, SINT val, BOOL is_null = false);
	CDatumInt2SPQDB(IMDId *mdid, SINT val, BOOL is_null = false);

	// dtor
	virtual ~CDatumInt2SPQDB();

	// accessor of metadata type id
	virtual IMDId *MDId() const;

	// accessor of size
	virtual ULONG Size() const;

	// accessor of integer value
	virtual SINT Value() const;

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

};	// class CDatumInt2SPQDB

}  // namespace spqnaucrates


#endif	// !SPQNAUCRATES_CDatumInt2SPQDB_H

// EOF

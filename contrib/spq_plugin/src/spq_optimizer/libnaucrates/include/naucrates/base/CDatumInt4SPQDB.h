//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		CDatumInt4SPQDB.h
//
//	@doc:
//		SPQDB-specific int4 representation
//---------------------------------------------------------------------------
#ifndef SPQNAUCRATES_CDatumInt4SPQDB_H
#define SPQNAUCRATES_CDatumInt4SPQDB_H

#include "spqos/base.h"

#include "naucrates/base/IDatumInt4.h"
#include "naucrates/md/CMDTypeInt4SPQDB.h"

namespace spqnaucrates
{
//---------------------------------------------------------------------------
//	@class:
//		CDatumInt4SPQDB
//
//	@doc:
//		SPQDB-specific int4 representation
//
//---------------------------------------------------------------------------
class CDatumInt4SPQDB : public IDatumInt4
{
private:
	// type information
	IMDId *m_mdid;

	// integer value
	INT m_val;

	// is null
	BOOL m_is_null;

	// private copy ctor
	CDatumInt4SPQDB(const CDatumInt4SPQDB &);

public:
	// ctors
	CDatumInt4SPQDB(CSystemId sysid, INT val, BOOL is_null = false);
	CDatumInt4SPQDB(IMDId *mdid, INT val, BOOL is_null = false);

	// dtor
	virtual ~CDatumInt4SPQDB();

	// accessor of metadata type id
	virtual IMDId *MDId() const;

	// accessor of size
	virtual ULONG Size() const;

	// accessor of integer value
	virtual INT Value() const;

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

};	// class CDatumInt4SPQDB

}  // namespace spqnaucrates


#endif	// !SPQNAUCRATES_CDatumInt4SPQDB_H

// EOF

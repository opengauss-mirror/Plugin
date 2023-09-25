//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CDatumInt8SPQDB.h
//
//	@doc:
//		SPQDB-specific Int8 representation
//---------------------------------------------------------------------------
#ifndef SPQNAUCRATES_CDatumInt8SPQDB_H
#define SPQNAUCRATES_CDatumInt8SPQDB_H

#include "spqos/base.h"

#include "naucrates/base/IDatumInt8.h"
#include "naucrates/md/CMDTypeInt8SPQDB.h"

namespace spqnaucrates
{
//---------------------------------------------------------------------------
//	@class:
//		CDatumInt8SPQDB
//
//	@doc:
//		SPQDB-specific Int8 representation
//
//---------------------------------------------------------------------------
class CDatumInt8SPQDB : public IDatumInt8
{
private:
	// type information
	IMDId *m_mdid;

	// integer value
	LINT m_val;

	// is null
	BOOL m_is_null;


	// private copy ctor
	CDatumInt8SPQDB(const CDatumInt8SPQDB &);

public:
	// ctors
	CDatumInt8SPQDB(CSystemId sysid, LINT val, BOOL is_null = false);
	CDatumInt8SPQDB(IMDId *mdid, LINT val, BOOL is_null = false);

	// dtor
	virtual ~CDatumInt8SPQDB();

	// accessor of metadata type id
	virtual IMDId *MDId() const;

	// accessor of size
	virtual ULONG Size() const;

	// accessor of integer value
	virtual LINT Value() const;

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

};	// class CDatumInt8SPQDB

}  // namespace spqnaucrates


#endif	// !SPQNAUCRATES_CDatumInt8SPQDB_H

// EOF

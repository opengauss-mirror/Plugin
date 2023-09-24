//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2014 Pivotal Inc.
//
//	@filename:
//		CMDIdSPQDBCtas.h
//
//	@doc:
//		Class for representing mdids for SPQDB CTAS queries
//---------------------------------------------------------------------------



#ifndef SPQMD_CMDIdSPQDBCTAS_H
#define SPQMD_CMDIdSPQDBCTAS_H

#include "spqos/base.h"

#include "naucrates/md/CMDIdSPQDB.h"

namespace spqmd
{
using namespace spqos;


//---------------------------------------------------------------------------
//	@class:
//		CMDIdSPQDBCtas
//
//	@doc:
//		Class for representing ids of SPQDB CTAS metadata objects
//
//---------------------------------------------------------------------------
class CMDIdSPQDBCtas : public CMDIdSPQDB
{
public:
	// ctor
	explicit CMDIdSPQDBCtas(OID oid);

	// copy ctor
	explicit CMDIdSPQDBCtas(const CMDIdSPQDBCtas &mdid_source);

	// mdid type
	virtual EMDIdType
	MdidType() const
	{
		return EmdidSPQDBCtas;
	}

	// source system id
	virtual CSystemId
	Sysid() const
	{
		return m_sysid;
	}

	// equality check
	virtual BOOL Equals(const IMDId *mdid) const;

	// is the mdid valid
	virtual BOOL IsValid() const;

	// debug print of the metadata id
	virtual IOstream &OsPrint(IOstream &os) const;

	// invalid mdid
	static CMDIdSPQDBCtas m_mdid_invalid_key;

	// const converter
	static const CMDIdSPQDBCtas *
	CastMdid(const IMDId *mdid)
	{
		SPQOS_ASSERT(NULL != mdid && EmdidSPQDBCtas == mdid->MdidType());

		return dynamic_cast<const CMDIdSPQDBCtas *>(mdid);
	}

	// non-const converter
	static CMDIdSPQDBCtas *
	CastMdid(IMDId *mdid)
	{
		SPQOS_ASSERT(NULL != mdid && EmdidSPQDBCtas == mdid->MdidType());

		return dynamic_cast<CMDIdSPQDBCtas *>(mdid);
	}
};

}  // namespace spqmd

#endif	// !SPQMD_CMDIdSPQDBCTAS_H

// EOF

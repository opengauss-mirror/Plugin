//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CMDProviderGeneric.h
//
//	@doc:
//		Provider of system-independent metadata objects.
//---------------------------------------------------------------------------



#ifndef SPQMD_CMDProviderGeneric_H
#define SPQMD_CMDProviderGeneric_H

#include "spqos/base.h"

#include "naucrates/md/IMDId.h"
#include "naucrates/md/IMDType.h"

#define SPQMD_DEFAULT_SYSID SPQOS_WSZ_LIT("SPQDB")

namespace spqmd
{
using namespace spqos;

//---------------------------------------------------------------------------
//	@class:
//		CMDProviderGeneric
//
//	@doc:
//		Provider of system-independent metadata objects.
//
//---------------------------------------------------------------------------
class CMDProviderGeneric
{
private:
	// mdid of int2
	IMDId *m_mdid_int2;

	// mdid of int4
	IMDId *m_mdid_int4;

	// mdid of int8
	IMDId *m_mdid_int8;

	// mdid of bool
	IMDId *m_mdid_bool;

	// mdid of oid
	IMDId *m_mdid_oid;

	// private copy ctor
	CMDProviderGeneric(const CMDProviderGeneric &);

public:
	// ctor/dtor
	CMDProviderGeneric(CMemoryPool *mp);

	// dtor
	~CMDProviderGeneric();

	// return the mdid for the requested type
	IMDId *MDId(IMDType::ETypeInfo type_info) const;

	// default system id
	CSystemId SysidDefault() const;
};
}  // namespace spqmd



#endif	// !SPQMD_CMDProviderGeneric_H

// EOF

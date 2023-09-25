//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2017 Pivotal Software Inc.
//
//	@filename:
//		CMDIndexInfo.h
//
//	@doc:
//		Implementation of indexinfo in relation metadata
//---------------------------------------------------------------------------

#ifndef SPQMD_CMDIndexInfo_H
#define SPQMD_CMDIndexInfo_H

#include "spqos/base.h"

#include "naucrates/md/IMDId.h"
#include "naucrates/md/IMDInterface.h"

namespace spqmd
{
using namespace spqos;
using namespace spqdxl;

// class for indexinfo in relation metadata
class CMDIndexInfo : public IMDInterface
{
private:
	// index mdid
	IMDId *m_mdid;

	// is the index partial
	BOOL m_is_partial;

public:
	// ctor
	CMDIndexInfo(IMDId *mdid, BOOL is_partial);

	// dtor
	virtual ~CMDIndexInfo();

	// index mdid
	IMDId *MDId() const;

	// is the index partial
	BOOL IsPartial() const;

	// serialize indexinfo in DXL format given a serializer object
	virtual void Serialize(CXMLSerializer *) const;

#ifdef SPQOS_DEBUG
	// debug print of the index info
	virtual void DebugPrint(IOstream &os) const;
#endif
};

}  // namespace spqmd

#endif	// !SPQMD_CMDIndexInfo_H

// EOF

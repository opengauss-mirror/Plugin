//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 Greenplum, Inc.
//
//	@filename:
//		CMDName.h
//
//	@doc:
//		Class for representing metadata names.
//---------------------------------------------------------------------------

#ifndef SPQMD_CMDName_H
#define SPQMD_CMDName_H

#include "spqos/base.h"
#include "spqos/common/CDynamicPtrArray.h"
#include "spqos/string/CWStringConst.h"

namespace spqmd
{
using namespace spqos;

//---------------------------------------------------------------------------
//	@class:
//		CMDName
//
//	@doc:
//		Class for representing metadata names.
//
//---------------------------------------------------------------------------
class CMDName
{
private:
	// the string holding the name
	const CWStringConst *m_name;

	// keep track of copy status
	BOOL m_deep_copy;

public:
	// ctor/dtor
	CMDName(CMemoryPool *mp, const CWStringBase *str);
	CMDName(const CWStringConst *, BOOL fOwnsMemory = false);

	// shallow copy ctor
	CMDName(const CMDName &);

	~CMDName();

	// accessors
	const CWStringConst *
	GetMDName() const
	{
		return m_name;
	}
};

// array of names
typedef CDynamicPtrArray<CMDName, CleanupDelete> CMDNameArray;
}  // namespace spqmd

#endif	// !SPQMD_CMDName_H

// EOF

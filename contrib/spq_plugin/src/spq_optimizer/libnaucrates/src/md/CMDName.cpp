//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 Greenplum, Inc.
//
//	@filename:
//		CMDName.cpp
//
//	@doc:
//		Metadata name of objects
//---------------------------------------------------------------------------

#include "naucrates/md/CMDName.h"

#include "spqos/base.h"
#include "spqos/string/CWStringDynamic.h"

using namespace spqmd;


//---------------------------------------------------------------------------
//	@function:
//		CMDName::CMDName
//
//	@doc:
//		Constructor
//		Creates a deep copy of the provided string
//
//---------------------------------------------------------------------------
CMDName::CMDName(CMemoryPool *mp, const CWStringBase *str)
	: m_name(NULL), m_deep_copy(true)
{
	m_name = SPQOS_NEW(mp) CWStringConst(mp, str->GetBuffer());
}

//---------------------------------------------------------------------------
//	@function:
//		CMDName::CMDName
//
//	@doc:
//		ctor
//		Depending on the value of the the owns_memory argument, the string object
//		can become property of the CMDName object
//
//---------------------------------------------------------------------------
CMDName::CMDName(const CWStringConst *str, BOOL owns_memory)
	: m_name(str), m_deep_copy(owns_memory)
{
	SPQOS_ASSERT(NULL != m_name);
	SPQOS_ASSERT(m_name->IsValid());
}

//---------------------------------------------------------------------------
//	@function:
//		CMDName::CMDName
//
//	@doc:
//		Shallow copy constructor
//
//---------------------------------------------------------------------------
CMDName::CMDName(const CMDName &name)
	: m_name(name.GetMDName()), m_deep_copy(false)
{
	SPQOS_ASSERT(NULL != m_name->GetBuffer());
	SPQOS_ASSERT(m_name->IsValid());
}


//---------------------------------------------------------------------------
//	@function:
//		CMDName::~CMDName
//
//	@doc:
//		dtor
//
//---------------------------------------------------------------------------
CMDName::~CMDName()
{
	SPQOS_ASSERT(m_name->IsValid());

	if (m_deep_copy)
	{
		SPQOS_DELETE(m_name);
	}
}

// EOF

//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2008 Greenplum, Inc.
//
//	@filename:
//		CWStringConst.cpp
//
//	@doc:
//		Implementation of the wide character constant string class
//---------------------------------------------------------------------------

#include "spqos/string/CWStringConst.h"

#include "spqos/base.h"
#include "spqos/common/clibwrapper.h"

using namespace spqos;


//---------------------------------------------------------------------------
//	@function:
//		CWStringConst::CWStringConst
//
//	@doc:
//		Initializes a constant string with a given character buffer. The string
//		does not own the memory
//
//---------------------------------------------------------------------------
CWStringConst::CWStringConst(const WCHAR *w_str_buffer)
	: CWStringBase(SPQOS_WSZ_LENGTH(w_str_buffer),
				   false  // owns_memory
				   ),
	  m_w_str_buffer(w_str_buffer)
{
	SPQOS_ASSERT(NULL != w_str_buffer);
	SPQOS_ASSERT(IsValid());
}

//---------------------------------------------------------------------------
//	@function:
//		CWStringConst::CWStringConst
//
//	@doc:
//		Initializes a constant string by making a copy of the given character buffer.
//		The string owns the memory.
//
//---------------------------------------------------------------------------
CWStringConst::CWStringConst(CMemoryPool *mp, const WCHAR *w_str_buffer)
	: CWStringBase(SPQOS_WSZ_LENGTH(w_str_buffer),
				   true	 // owns_memory
				   ),
	  m_w_str_buffer(NULL)
{
	SPQOS_ASSERT(NULL != mp);
	SPQOS_ASSERT(NULL != w_str_buffer);

	if (0 == m_length)
	{
		// string is empty
		m_w_str_buffer = &m_empty_wcstr;
	}
	else
	{
		// make a copy of the string
		WCHAR *w_str_temp_buffer = SPQOS_NEW_ARRAY(mp, WCHAR, m_length + 1);
		clib::WcStrNCpy(w_str_temp_buffer, w_str_buffer, m_length + 1);
		m_w_str_buffer = w_str_temp_buffer;
	}

	SPQOS_ASSERT(IsValid());
}

//---------------------------------------------------------------------------
//	@function:
//		CWStringConst::CWStringConst
//
//	@doc:
//		Shallow copy constructor.
//
//---------------------------------------------------------------------------
CWStringConst::CWStringConst(const CWStringConst &str)
	: CWStringBase(str.Length(),
				   false  // owns_memory
				   ),
	  m_w_str_buffer(str.GetBuffer())
{
	SPQOS_ASSERT(NULL != m_w_str_buffer);
	SPQOS_ASSERT(IsValid());
}
//---------------------------------------------------------------------------
//	@function:
//		CWStringConst::~CWStringConst
//
//	@doc:
//		Destroys a constant string. This involves releasing the character buffer
//		provided the string owns it.
//
//---------------------------------------------------------------------------
CWStringConst::~CWStringConst()
{
	if (m_owns_memory && m_w_str_buffer != &m_empty_wcstr)
	{
		SPQOS_DELETE_ARRAY(m_w_str_buffer);
	}
}

//---------------------------------------------------------------------------
//	@function:
//		CWStringConst::GetBuffer
//
//	@doc:
//		Returns the wide character buffer
//
//---------------------------------------------------------------------------
const WCHAR *
CWStringConst::GetBuffer() const
{
	return m_w_str_buffer;
}

// EOF

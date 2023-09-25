//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2010 Greenplum, Inc.
//
//	@filename:
//		CStringStatic.h
//
//	@doc:
//		ASCII-character String class with buffer
//---------------------------------------------------------------------------
#ifndef SPQOS_CStringStatic_H
#define SPQOS_CStringStatic_H

#include "spqos/base.h"
#include "spqos/common/clibwrapper.h"

#define SPQOS_SZ_LENGTH(x) spqos::clib::Strlen(x)

// use this character to substitute non-ASCII wide characters
#define SPQOS_WCHAR_UNPRINTABLE '.'

// end-of-string character
#define CHAR_EOS '\0'

namespace spqos
{
//---------------------------------------------------------------------------
//	@class:
//		CStringStatic
//
//	@doc:
//		ASCII-character string interface with buffer pre-allocation.
//		Internally, the class uses a null-terminated CHAR buffer to store the string
//		characters.	The buffer is assigned at construction time; its capacity cannot be
//		modified, thus restricting the maximum size of the stored string. Attempting to
//		store a larger string than the available buffer capacity results in truncation.
//		CStringStatic owner is responsible for allocating the buffer and releasing it
//		after the object is destroyed.
//
//---------------------------------------------------------------------------
class CStringStatic
{
private:
	// null-terminated wide character buffer
	CHAR *m_buffer;

	// size of the string in number of CHAR units,
	// not counting the terminating '\0'
	ULONG m_length;

	// buffer capacity
	ULONG m_capacity;

#ifdef SPQOS_DEBUG
	// checks whether a string is properly null-terminated
	bool IsValid() const;
#endif	// SPQOS_DEBUG

	// private copy ctor
	CStringStatic(const CStringStatic &);

public:
	// ctor
	CStringStatic(CHAR buffer[], ULONG capacity);

	// ctor with string initialization
	CStringStatic(CHAR buffer[], ULONG capacity, const CHAR init_str[]);

	// dtor - owner is responsible for releasing the buffer
	~CStringStatic()
	{
	}

	// returns the wide character buffer storing the string
	const CHAR *
	Buffer() const
	{
		return m_buffer;
	}

	// returns the string length
	ULONG
	Length() const
	{
		return m_length;
	}

	// checks whether the string contains any characters
	BOOL
	IsEmpty() const
	{
		return (0 == m_length);
	}

	// checks whether the string is byte-wise equal to a given string literal
	BOOL Equals(const CHAR *buf) const;

	// appends a string
	void Append(const CStringStatic *str);

	// appends the contents of a buffer to the current string
	void AppendBuffer(const CHAR *buf);

	// appends a formatted string
	void AppendFormat(const CHAR *format, ...);

	// appends a formatted string based on passed va list
	void AppendFormatVA(const CHAR *format, VA_LIST va_args);

	// appends wide character string
	void AppendConvert(const WCHAR *wc_str);

	// resets string
	void Reset();
};
}  // namespace spqos

#endif	// !SPQOS_CStringStatic_H

// EOF

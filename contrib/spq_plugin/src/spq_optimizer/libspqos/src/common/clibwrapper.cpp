//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (c) 2004-2015 Pivotal Software, Inc.
//
//	@filename:
//		clibwrapper.cpp
//
//	@doc:
//		Wrapper for functions in C library
//
//---------------------------------------------------------------------------

#include "spqos/common/clibwrapper.h"

#include <cxxabi.h>
#include <dlfcn.h>
#include <errno.h>
#include <fenv.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <wchar.h>

#include "spqos/assert.h"
#include "spqos/base.h"
#include "spqos/error/CException.h"
#include "spqos/utils.h"

using namespace spqos;

//---------------------------------------------------------------------------
//	@function:
//		clib::USleep
//
//	@doc:
//		Sleep given number of microseconds
//
//---------------------------------------------------------------------------
void
spqos::clib::USleep(ULONG usecs)
{
	SPQOS_ASSERT(1000000 >= usecs);

	// ignore return value
	(void) usleep(usecs);
}


//---------------------------------------------------------------------------
//	@function:
//		clib::Strcmp
//
//	@doc:
//		Compare two strings
//
//---------------------------------------------------------------------------
INT
spqos::clib::Strcmp(const CHAR *left, const CHAR *right)
{
	SPQOS_ASSERT(NULL != left);
	SPQOS_ASSERT(NULL != right);

	return strcmp(left, right);
}


//---------------------------------------------------------------------------
//	@function:
//		clib::Strncmp
//
//	@doc:
//		Compare two strings up to a specified number of characters
//
//---------------------------------------------------------------------------
INT
spqos::clib::Strncmp(const CHAR *left, const CHAR *right, SIZE_T num_bytes)
{
	SPQOS_ASSERT(NULL != left);
	SPQOS_ASSERT(NULL != right);

	return strncmp(left, right, num_bytes);
}


//---------------------------------------------------------------------------
//	@function:
//		clib::Memcmp
//
//	@doc:
//		Compare a specified number of bytes of two regions of memory
//---------------------------------------------------------------------------
INT
spqos::clib::Memcmp(const void *left, const void *right, SIZE_T num_bytes)
{
	SPQOS_ASSERT(NULL != left);
	SPQOS_ASSERT(NULL != right);

	return memcmp(left, right, num_bytes);
}


//---------------------------------------------------------------------------
//	@function:
//		clib::Wcsncmp
//
//	@doc:
//		Compare two strings up to a specified number of wide characters
//
//---------------------------------------------------------------------------
INT
spqos::clib::Wcsncmp(const WCHAR *left, const WCHAR *right, SIZE_T num_bytes)
{
	SPQOS_ASSERT(NULL != left);
	SPQOS_ASSERT(NULL != right);

	return wcsncmp(left, right, num_bytes);
}


//---------------------------------------------------------------------------
//	@function:
//		clib::WcStrNCpy
//
//	@doc:
//		Copy two strings up to a specified number of wide characters
//
//---------------------------------------------------------------------------
WCHAR *
spqos::clib::WcStrNCpy(WCHAR *dest, const WCHAR *src, SIZE_T num_bytes)
{
	SPQOS_ASSERT(NULL != dest);
	SPQOS_ASSERT(NULL != src && num_bytes > 0);

	// check for overlap
	SPQOS_ASSERT(((src + num_bytes) <= dest) || ((dest + num_bytes) <= src));

	return wcsncpy(dest, src, num_bytes);
}


//---------------------------------------------------------------------------
//	@function:
//		clib::Memcpy
//
//	@doc:
//		Copy a specified number of bytes between two memory areas
//
//---------------------------------------------------------------------------
void *
spqos::clib::Memcpy(void *dest, const void *src, SIZE_T num_bytes)
{
	SPQOS_ASSERT(NULL != dest);

	SPQOS_ASSERT(NULL != src && num_bytes > 0);

#ifdef SPQOS_DEBUG
	const BYTE *src_addr = static_cast<const BYTE *>(src);
	const BYTE *dest_addr = static_cast<const BYTE *>(dest);
#endif	// SPQOS_DEBUG

	// check for overlap
	SPQOS_ASSERT(((src_addr + num_bytes) <= dest_addr) ||
				((dest_addr + num_bytes) <= src_addr));

	return memcpy(dest, src, num_bytes);
}


//---------------------------------------------------------------------------
//	@function:
//		clib::Wmemcpy
//
//	@doc:
//		Copy a specified number of wide characters
//
//---------------------------------------------------------------------------
WCHAR *
spqos::clib::Wmemcpy(WCHAR *dest, const WCHAR *src, SIZE_T num_bytes)
{
	SPQOS_ASSERT(NULL != dest);
	SPQOS_ASSERT(NULL != src && num_bytes > 0);

#ifdef SPQOS_DEBUG
	const WCHAR *src_addr = static_cast<const WCHAR *>(src);
	const WCHAR *dest_addr = static_cast<WCHAR *>(dest);
#endif

	// check for overlap
	SPQOS_ASSERT(((src_addr + num_bytes) <= dest_addr) ||
				((dest_addr + num_bytes) <= src_addr));

	return wmemcpy(dest, src, num_bytes);
}


//---------------------------------------------------------------------------
//	@function:
//		clib::Strncpy
//
//	@doc:
//		Copy a specified number of characters
//
//---------------------------------------------------------------------------
CHAR *
spqos::clib::Strncpy(CHAR *dest, const CHAR *src, SIZE_T num_bytes)
{
	SPQOS_ASSERT(NULL != dest);
	SPQOS_ASSERT(NULL != src && num_bytes > 0);
	SPQOS_ASSERT(((src + num_bytes) <= dest) || ((dest + num_bytes) <= src));

	return strncpy(dest, src, num_bytes);
}

//---------------------------------------------------------------------------
//	@function:
//		clib::Strchr
//
//	@doc:
//		Find the first occurrence of the character c (converted to a char) in
//		the null-terminated string beginning at src. Returns a pointer to the
//		located character, or a null pointer if no match was found
//
//---------------------------------------------------------------------------
CHAR *
spqos::clib::Strchr(const CHAR *src, INT c)
{
	SPQOS_ASSERT(NULL != src);

	return (CHAR *) strchr(src, c);
}

//---------------------------------------------------------------------------
//	@function:
//		clib::Memset
//
//	@doc:
//		Set the bytes of a given memory block to a specific value
//
//---------------------------------------------------------------------------
void *
spqos::clib::Memset(void *dest, INT value, SIZE_T num_bytes)
{
	SPQOS_ASSERT(NULL != dest);
	SPQOS_ASSERT_IFF(0 <= value, 255 >= value);

	return memset(dest, value, num_bytes);
}


//---------------------------------------------------------------------------
//	@function:
//		clib::Qsort
//
//	@doc:
//		Sort a specified number of elements
//
//---------------------------------------------------------------------------
void
spqos::clib::Qsort(void *dest, SIZE_T num_bytes, SIZE_T size,
				  Comparator comparator)
{
	SPQOS_ASSERT(NULL != dest);

	qsort(dest, num_bytes, size, comparator);
}


//---------------------------------------------------------------------------
//	@function:
//		clib::Getopt
//
//	@doc:
//		Parse the command-line arguments
//
//---------------------------------------------------------------------------
INT
spqos::clib::Getopt(INT argc, CHAR *const argv[], const CHAR *opt_string)
{
	return getopt(argc, argv, opt_string);
}


//---------------------------------------------------------------------------
//	@function:
//		clib::Strtol
//
//	@doc:
//		Convert string to long integer
//
//---------------------------------------------------------------------------
LINT
spqos::clib::Strtol(const CHAR *val, CHAR **end, ULONG base)
{
	SPQOS_ASSERT(NULL != val);
	SPQOS_ASSERT(0 == base || 2 == base || 10 == base || 16 == base);

	return strtol(val, end, base);
}

//---------------------------------------------------------------------------
//	@function:
//		clib::Strtoll
//
//	@doc:
//		Convert string to long long integer
//
//---------------------------------------------------------------------------
LINT
spqos::clib::Strtoll(const CHAR *val, CHAR **end, ULONG base)
{
	SPQOS_ASSERT(NULL != val);
	SPQOS_ASSERT(0 == base || 2 == base || 10 == base || 16 == base);

	return strtoll(val, end, base);
}

//---------------------------------------------------------------------------
//	@function:
//		clib::Rand
//
//	@doc:
//		Return a pseudo-random integer between 0 and RAND_MAX
//
//---------------------------------------------------------------------------
ULONG
spqos::clib::Rand(ULONG *seed)
{
	SPQOS_ASSERT(NULL != seed);

	INT res = rand_r(seed);

	SPQOS_ASSERT(res >= 0 && res <= RAND_MAX);

	return static_cast<ULONG>(res);
}


//---------------------------------------------------------------------------
//	@function:
//		clib::Vswprintf
//
//	@doc:
//		Format wide character output conversion
//
//---------------------------------------------------------------------------
INT
spqos::clib::Vswprintf(WCHAR *wcstr, SIZE_T max_len, const WCHAR *format,
					  VA_LIST vaArgs)
{
	SPQOS_ASSERT(NULL != wcstr);
	SPQOS_ASSERT(NULL != format);

	INT res = vswprintf(wcstr, max_len, format, vaArgs);
	if (-1 == res && EILSEQ == errno)
	{
		// Invalid multibyte character encountered. This can happen if the byte sequence does not
		// match with the server encoding.
		SPQOS_RAISE(CException::ExmaSystem, CException::ExmiIllegalByteSequence);
	}

	return res;
}


//---------------------------------------------------------------------------
//	@function:
//		clib::Vsnprintf
//
//	@doc:
//		Format string
//
//---------------------------------------------------------------------------
INT
spqos::clib::Vsnprintf(CHAR *src, SIZE_T size, const CHAR *format,
					  VA_LIST vaArgs)
{
	SPQOS_ASSERT(NULL != src);
	SPQOS_ASSERT(NULL != format);
	// TODO SPQ need support
	//return vsnprintf(src, size, format, vaArgs);
	return 0;
}


//---------------------------------------------------------------------------
//	@function:
//		clib::Strerror_r
//
//	@doc:
//		Return string describing error number
//
//---------------------------------------------------------------------------
void
spqos::clib::Strerror_r(INT errnum, CHAR *buf, SIZE_T buf_len)
{
	SPQOS_ASSERT(NULL != buf);

#ifdef _GNU_SOURCE
	// GNU-specific strerror_r() returns char*.
	CHAR *error_str = strerror_r(errnum, buf, buf_len);
	SPQOS_ASSERT(NULL != error_str);

	// GNU strerror_r() may return a pointer to a static error string.
	// Copy it into 'buf' if that is the case.
	if (error_str != buf)
	{
		strncpy(buf, error_str, buf_len);
		// Ensure null-terminated.
		buf[buf_len - 1] = '\0';
	}
#else  // !_GNU_SOURCE
	// POSIX.1-2001 standard strerror_r() returns int.
#ifdef SPQOS_DEBUG
	INT str_err_code =
#endif
		strerror_r(errnum, buf, buf_len);
	SPQOS_ASSERT(0 == str_err_code);

#endif
}


//---------------------------------------------------------------------------
//	@function:
//		clib::Wcslen
//
//	@doc:
//		Calculate the length of a wide-character string
//
//---------------------------------------------------------------------------
ULONG
spqos::clib::Wcslen(const WCHAR *dest)
{
	SPQOS_ASSERT(NULL != dest);

	return (ULONG) wcslen(dest);
}


//---------------------------------------------------------------------------
//	@function:
//		clib::Localtime_r
//
//	@doc:
//		Convert the calendar time time to broken-time representation;
//		Expressed relative to the user's specified time zone
//
//---------------------------------------------------------------------------
struct tm *
spqos::clib::Localtime_r(const TIME_T *time, TIME *result)
{
	SPQOS_ASSERT(NULL != time);

	localtime_r(time, result);

	SPQOS_ASSERT(NULL != result);

	return result;
}


//---------------------------------------------------------------------------
//	@function:
//		clib::Malloc
//
//	@doc:
//		Allocate dynamic memory
//
//---------------------------------------------------------------------------
void *
spqos::clib::Malloc(SIZE_T size)
{
	return malloc(size);
}


//---------------------------------------------------------------------------
//	@function:
//		clib::Free
//
//	@doc:
//		Free dynamic memory
//
//---------------------------------------------------------------------------
void
spqos::clib::Free(void *src)
{
	free(src);
}


//---------------------------------------------------------------------------
//	@function:
//		clib::Strlen
//
//	@doc:
//		Calculate the length of a string
//
//---------------------------------------------------------------------------
ULONG
spqos::clib::Strlen(const CHAR *buf)
{
	SPQOS_ASSERT(NULL != buf);

	return (ULONG) strlen(buf);
}


//---------------------------------------------------------------------------
//	@function:
//		clib::Wctomb
//
//	@doc:
//		Convert a wide character to a multibyte sequence
//
//---------------------------------------------------------------------------
INT
spqos::clib::Wctomb(CHAR *dest, WCHAR src)
{
	return wctomb(dest, src);
}


//---------------------------------------------------------------------------
//	@function:
//		clib::Mbstowcs
//
//	@doc:
//		Convert a multibyte sequence to wide character array
//
//---------------------------------------------------------------------------
ULONG
spqos::clib::Mbstowcs(WCHAR *dest, const CHAR *src, SIZE_T len)
{
	SPQOS_ASSERT(NULL != dest);
	SPQOS_ASSERT(NULL != src);

	return (ULONG) mbstowcs(dest, src, len);
}


//---------------------------------------------------------------------------
//	@function:
//		clib::Wcstombs
//
//	@doc:
//		Convert a wide-character string to a multi-byte string
//
//---------------------------------------------------------------------------
LINT
spqos::clib::Wcstombs(CHAR *dest, WCHAR *src, ULONG_PTR dest_size)
{
	return wcstombs(dest, src, dest_size);
}


//---------------------------------------------------------------------------
//	@function:
//		clib::Strtod
//
//	@doc:
//		Convert string to double;
//		if conversion fails, return 0.0
//
//---------------------------------------------------------------------------
DOUBLE
spqos::clib::Strtod(const CHAR *str)
{
	return strtod(str, NULL);
}


//---------------------------------------------------------------------------
//	@function:
//		clib::Demangle
//
//	@doc:
//		Return a pointer to the start of the NULL-terminated
//		symbol or NULL if demangling fails
//
//---------------------------------------------------------------------------
CHAR *
spqos::clib::Demangle(const CHAR *symbol, CHAR *buf, SIZE_T *len, INT *status)
{
	SPQOS_ASSERT(NULL != symbol);

	CHAR *res = abi::__cxa_demangle(symbol, buf, len, status);

	SPQOS_ASSERT(-3 != *status && "One of the arguments is invalid.");

	return res;
}


//---------------------------------------------------------------------------
//	@function:
//		clib::Dladdr
//
//	@doc:
//		Resolve symbol information from its address
//
//---------------------------------------------------------------------------
void
spqos::clib::Dladdr(void *addr, DL_INFO *info)
{
#ifdef SPQOS_DEBUG
	INT res =
#endif
		dladdr(addr, info);

	SPQOS_ASSERT(0 != res);
}

// EOF

//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2010 Greenplum, Inc.
//
//	@filename:
//		CStackTrace.h
//
//	@doc:
//		Interface class for execution stack tracing.
//---------------------------------------------------------------------------

#ifndef SPQOS_CStackDescriptor_H
#define SPQOS_CStackDescriptor_H

#include "spqos/common/clibtypes.h"
#include "spqos/io/IOstream.h"
#include "spqos/types.h"

#define SPQOS_STACK_TRACE_BUFFER_SIZE 4096
#define SPQOS_STACK_TRACE_DEPTH 32
#define SPQOS_STACK_SYMBOL_SIZE 16384

#define SPQOS_STACK_TRACE_FORMAT_SIZE 192

namespace spqos
{
// prototype
class CWString;

class CStackDescriptor
{
private:
	// stack depth
	ULONG m_depth;

	// array with frame return addresses
	void *m_array_of_addresses[SPQOS_STACK_TRACE_DEPTH];

	// append formatted symbol description
	void AppendSymbolInfo(CWString *ws, CHAR *demangling_symbol_buffer,
						  SIZE_T size, const Dl_info &symbol_info_array,
						  ULONG index) const;

	// reset descriptor
	void
	Reset()
	{
		// reset stack depth
		m_depth = 0;
	}

public:
	// ctor
	CStackDescriptor() : m_depth(0)
	{
		Reset();
	}

	// store current stack skipping (top_frames_to_skip) top frames
	void BackTrace(ULONG top_frames_to_skip = 0);

	// append trace of stored stack to string
	void AppendTrace(CWString *ws, ULONG depth = SPQOS_STACK_TRACE_DEPTH) const;

	// append trace of stored stack to stream
	void AppendTrace(IOstream &os, ULONG depth = SPQOS_STACK_TRACE_DEPTH) const;

	// get hash value for stored stack
	ULONG HashValue() const;

};	// class CStackTrace

}  // namespace spqos

#endif	// !SPQOS_CStackDescriptor_H_

// EOF

//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2008-2010 Greenplum Inc.
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		CMemoryVisitorPrint.h
//
//	@doc:
//		Memory object visitor that prints debug information for all allocated
//		objects inside a memory pool.
//
//	@owner:
//
//	@test:
//
//---------------------------------------------------------------------------
#ifndef SPQOS_CMemoryVisitorPrint_H
#define SPQOS_CMemoryVisitorPrint_H

#include "spqos/assert.h"
#include "spqos/memory/IMemoryVisitor.h"
#include "spqos/types.h"
#include "spqos/utils.h"

namespace spqos
{
// specialization of memory object visitor that prints out
// the debugging information to a stream
class CMemoryVisitorPrint : public IMemoryVisitor
{
private:
	// call counter for the visit function
	ULLONG m_visits;

	// stream used for writing debug information
	IOstream &m_os;

	// private copy ctor
	CMemoryVisitorPrint(CMemoryVisitorPrint &);

public:
	// ctor
	CMemoryVisitorPrint(IOstream &os);

	// dtor
	virtual ~CMemoryVisitorPrint();

	// output information about a memory allocation
	virtual void Visit(void *user_addr, SIZE_T user_size, void *total_addr,
					   SIZE_T total_size, const CHAR *alloc_filename,
					   const ULONG alloc_line, ULLONG alloc_seq_number,
					   CStackDescriptor *stack_desc);

	// visit counter accessor
	ULLONG
	GetNumVisits() const
	{
		return m_visits;
	}
};
}  // namespace spqos

#endif	// SPQOS_CMemoryVisitorPrint_H

// EOF

//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		CMappingElementColIdTE.h
//
//	@doc:
//		Wrapper class providing functions for the mapping element (between ColId and TE)
//		during DXL->Query translation
//
//	@test:
//
//
//---------------------------------------------------------------------------
#ifndef SPQDXL_CMappingElementColIdTE_H
#define SPQDXL_CMappingElementColIdTE_H

#include "postgres.h"

#include "nodes/primnodes.h"

#include "spqos/base.h"

#include "naucrates/dxl/operators/CDXLNode.h"

// fwd decl
struct TargetEntry;

namespace spqdxl
{
using namespace spqos;

//---------------------------------------------------------------------------
//	@class:
//		CMappingElementColIdTE
//
//	@doc:
//		Wrapper class providing functions for the mapping element (between ColId and TE)
//		during DXL->Query translation
//
//---------------------------------------------------------------------------
class CMappingElementColIdTE : public CRefCount
{
private:
	// the column identifier that is used as the key
	ULONG m_colid;

	// the query level
	ULONG m_query_level;

	// the target entry
	TargetEntry *m_target_entry;

public:
	// ctors and dtor
	CMappingElementColIdTE(ULONG, ULONG, TargetEntry *);

	// return the ColId
	ULONG
	GetColId() const
	{
		return m_colid;
	}

	// return the query level
	ULONG
	GetQueryLevel() const
	{
		return m_query_level;
	}

	// return the column name for the given attribute no
	const TargetEntry *
	GetTargetEntry() const
	{
		return m_target_entry;
	}
};
}  // namespace spqdxl

#endif	// SPQDXL_CMappingElementColIdTE_H

// EOF

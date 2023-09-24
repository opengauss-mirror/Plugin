//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 Greenplum, Inc.
//
//	@filename:
//		CDXLLogical.h
//
//	@doc:
//		Base class for DXL logical operators.
//
//---------------------------------------------------------------------------

#ifndef SPQDXL_CDXLLogical_H
#define SPQDXL_CDXLLogical_H

#include "spqos/base.h"

#include "naucrates/dxl/operators/CDXLColRef.h"
#include "naucrates/dxl/operators/CDXLOperator.h"

namespace spqdxl
{
using namespace spqos;

// fwd decl
class CXMLSerializer;

//---------------------------------------------------------------------------
//	@class:
//		CDXLLogical
//
//	@doc:
//		Base class the DXL logical operators
//
//---------------------------------------------------------------------------
class CDXLLogical : public CDXLOperator
{
private:
	// private copy ctor
	CDXLLogical(const CDXLLogical &);

public:
	// ctor/dtor
	explicit CDXLLogical(CMemoryPool *mp);

	// Get operator type
	Edxloptype GetDXLOperatorType() const;
};
}  // namespace spqdxl

#endif	// !SPQDXL_CDXLLogical_H

// EOF

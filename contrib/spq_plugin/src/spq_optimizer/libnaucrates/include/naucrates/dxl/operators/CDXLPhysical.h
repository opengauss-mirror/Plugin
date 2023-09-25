//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2010 Greenplum, Inc.
//
//	@filename:
//		CDXLPhysical.h
//
//	@doc:
//		Base class for DXL physical operators.
//---------------------------------------------------------------------------


#ifndef SPQDXL_CDXLPhysical_H
#define SPQDXL_CDXLPhysical_H

#include "spqos/base.h"

#include "naucrates/dxl/operators/CDXLOperator.h"
#include "naucrates/dxl/operators/CDXLPhysicalProperties.h"

namespace spqdxl
{
using namespace spqos;

// fwd decl
class CXMLSerializer;
//---------------------------------------------------------------------------
//	@class:
//		CDXLPhysical
//
//	@doc:
//		Base class the DXL physical operators
//
//---------------------------------------------------------------------------
class CDXLPhysical : public CDXLOperator
{
private:
	// private copy ctor
	CDXLPhysical(const CDXLPhysical &);

public:
	// ctor/dtor
	explicit CDXLPhysical(CMemoryPool *mp);

	virtual ~CDXLPhysical();

	// Get operator type
	Edxloptype GetDXLOperatorType() const;

#ifdef SPQOS_DEBUG
	// checks whether the operator has valid structure, i.e. number and
	// types of child nodes
	virtual void AssertValid(const CDXLNode *, BOOL validate_children) const;
#endif	// SPQOS_DEBUG
};
}  // namespace spqdxl

#endif	// !SPQDXL_CDXLPhysical_H

// EOF

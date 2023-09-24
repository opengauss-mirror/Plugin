//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2010 Greenplum, Inc.
//
//	@filename:
//		CDXLScalar.h
//
//	@doc:
//		Base class for representing scalar DXL operators.
//---------------------------------------------------------------------------

#ifndef SPQDXL_CDXLScalar_H
#define SPQDXL_CDXLScalar_H

#include "spqos/base.h"

#include "naucrates/dxl/spqdb_types.h"
#include "naucrates/dxl/operators/CDXLOperator.h"

// fwd declarations
namespace spqopt
{
class CMDAccessor;
}

namespace spqdxl
{
using namespace spqopt;

//---------------------------------------------------------------------------
//	@class:
//		CDXLScalar
//
//	@doc:
//		Base class for representing scalar DXL operators
//
//---------------------------------------------------------------------------
class CDXLScalar : public CDXLOperator
{
private:
	// private copy ctor
	CDXLScalar(CDXLScalar &);

public:
	// ctor/dtor
	explicit CDXLScalar(CMemoryPool *mp);

	virtual ~CDXLScalar(){};

	Edxloptype GetDXLOperatorType() const;

	// does the operator return a boolean result
	virtual BOOL HasBoolResult(CMDAccessor *md_accessor) const = 0;

#ifdef SPQOS_DEBUG
	virtual void AssertValid(const CDXLNode *dxlnode,
							 BOOL validate_children) const = 0;
#endif	// SPQOS_DEBUG
};
}  // namespace spqdxl

#endif	// !SPQDXL_CDXLScalar_H

// EOF

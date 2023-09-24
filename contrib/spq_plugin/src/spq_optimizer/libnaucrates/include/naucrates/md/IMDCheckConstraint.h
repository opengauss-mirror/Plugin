//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		IMDCheckConstraint.h
//
//	@doc:
//		Interface class for check constraint in a metadata cache relation
//---------------------------------------------------------------------------

#ifndef SPQMD_IMDCheckConstraint_H
#define SPQMD_IMDCheckConstraint_H

#include "spqos/base.h"

#include "spqopt/base/CColRef.h"
#include "naucrates/md/IMDCacheObject.h"

// fwd decl
namespace spqdxl
{
class CDXLNode;
}

namespace spqopt
{
class CExpression;
class CMDAccessor;
}  // namespace spqopt

namespace spqmd
{
using namespace spqos;
using namespace spqopt;

//---------------------------------------------------------------------------
//	@class:
//		IMDCheckConstraint
//
//	@doc:
//		Interface class for check constraint in a metadata cache relation
//
//---------------------------------------------------------------------------
class IMDCheckConstraint : public IMDCacheObject
{
public:
	// object type
	virtual Emdtype
	MDType() const
	{
		return EmdtCheckConstraint;
	}

	// mdid of the relation
	virtual IMDId *GetRelMdId() const = 0;

	// the scalar expression of the check constraint
	virtual CExpression *GetCheckConstraintExpr(
		CMemoryPool *mp, CMDAccessor *md_accessor,
		CColRefArray *colref_array) const = 0;
};
}  // namespace spqmd

#endif	// !SPQMD_IMDCheckConstraint_H

// EOF

//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		IMDPartConstraint.h
//
//	@doc:
//		Interface class for partition constraints in the MD cache
//---------------------------------------------------------------------------



#ifndef SPQMD_IMDPartConstraint_H
#define SPQMD_IMDPartConstraint_H

#include "spqos/base.h"

#include "spqopt/base/CColRef.h"
#include "naucrates/md/IMDInterface.h"

// fwd decl
namespace spqopt
{
class CExpression;
class CMDAccessor;
}  // namespace spqopt

namespace spqmd
{
using namespace spqos;

//---------------------------------------------------------------------------
//	@class:
//		IMDPartConstraint
//
//	@doc:
//		Interface class for partition constraints in the MD cache
//
//---------------------------------------------------------------------------
class IMDPartConstraint : public IMDInterface
{
public:
	// extract the scalar expression of the constraint with the given
	// column mappings
	virtual CExpression *GetPartConstraintExpr(
		CMemoryPool *mp, CMDAccessor *md_accessor,
		CColRefArray *colref_array) const = 0;

	// included default partitions
	virtual ULongPtrArray *GetDefaultPartsArray() const = 0;

	// is constraint unbounded
	virtual BOOL IsConstraintUnbounded() const = 0;

	// serialize constraint in DXL format
	virtual void Serialize(CXMLSerializer *xml_serializer) const = 0;
};
}  // namespace spqmd



#endif	// !SPQMD_IMDPartConstraint_H

// EOF

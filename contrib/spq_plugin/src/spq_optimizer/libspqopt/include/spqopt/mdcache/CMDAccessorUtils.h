//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CMDAccessorUtils.h
//
//	@doc:
//		Utility functions associated with the metadata cache accessor
//---------------------------------------------------------------------------



#ifndef SPQOPT_CMDAccessorUtils_H
#define SPQOPT_CMDAccessorUtils_H

#include "spqopt/mdcache/CMDAccessor.h"
#include "spqopt/operators/CExpression.h"

namespace spqopt
{
using namespace spqos;
using namespace spqmd;

//---------------------------------------------------------------------------
//	@class:
//		CMDAccessorUtils
//
//	@doc:
//		Utility functions associated with the metadata cache accessor
//
//---------------------------------------------------------------------------
class CMDAccessorUtils
{
public:
	// return the name of the window operation
	static const CWStringConst *PstrWindowFuncName(CMDAccessor *md_accessor,
												   IMDId *mdid);

	// return the return type of the window operation
	static IMDId *PmdidWindowReturnType(CMDAccessor *md_accessor, IMDId *mdid);

	// does a cast object between given source and destination types exist
	static BOOL FCastExists(CMDAccessor *md_accessor, IMDId *mdid_src,
							IMDId *mdid_dest);

	// does a scalar comparison object between given types exist
	static BOOL FCmpExists(CMDAccessor *md_accessor, IMDId *left_mdid,
						   IMDId *right_mdid, IMDType::ECmpType cmp_type);

	// get scalar comparison mdid between the given types
	static IMDId *GetScCmpMdid(CMDAccessor *md_accessor, IMDId *left_mdid,
							   IMDId *right_mdid, IMDType::ECmpType cmp_type);

	// check is a comparison between given types or a comparison after casting
	// one side to an another exists
	static BOOL FCmpOrCastedCmpExists(IMDId *left_mdid, IMDId *right_mdid,
									  IMDType::ECmpType cmp_type);

	// return the mdid of the given scalar comparison between the two types
	// also considering casts
	static IMDId *GetScCmpMdIdConsiderCasts(CMDAccessor *md_accessor,
											IMDId *left_mdid, IMDId *right_mdid,
											IMDType::ECmpType cmp_type);

	static IMDId *GetScCmpMdIdConsiderCasts(CMDAccessor *md_accessor,
											CExpression *pexprLeft,
											CExpression *pexprRight,
											IMDType::ECmpType cmp_type);

	// similar to GetScCmpMdIdConsiderCasts() but also add the appropriate casts
	static void ApplyCastsForScCmp(CMemoryPool *mp, CMDAccessor *md_accessor,
								   CExpression *&pexprLeft,
								   CExpression *&pexprRight, IMDId *op_mdid);

	// is scalar operator commutative? this can be used with ScalarOp and ScalarCmp
	static BOOL FCommutativeScalarOp(CMDAccessor *md_accessor, IMDId *mdid_op);

	// does scalar operator return NULL on NULL input?
	static BOOL FScalarOpReturnsNullOnNullInput(CMDAccessor *md_accessor,
												IMDId *mdid_op);

	// return True if passed mdid is for BOOL type
	static BOOL FBoolType(CMDAccessor *md_accessor, IMDId *mdid_type);
};
}  // namespace spqopt

#endif	// !SPQOPT_CMDAccessorUtils_H

// EOF

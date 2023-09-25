//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CXformDynamicGet2DynamicTableScan.h
//
//	@doc:
//		Transform DynamicGet to DynamicTableScan
//---------------------------------------------------------------------------
#ifndef SPQOPT_CXformDynamicGet2DynamicTableScan_H
#define SPQOPT_CXformDynamicGet2DynamicTableScan_H

#include "spqos/base.h"

#include "spqopt/operators/CExpressionHandle.h"
#include "spqopt/operators/CLogicalDynamicGet.h"
#include "spqopt/xforms/CXformImplementation.h"

namespace spqopt
{
using namespace spqos;

//---------------------------------------------------------------------------
//	@class:
//		CXformDynamicGet2DynamicTableScan
//
//	@doc:
//		Transform DynamicGet to DynamicTableScan
//
//---------------------------------------------------------------------------
class CXformDynamicGet2DynamicTableScan : public CXformImplementation
{
private:
	// private copy ctor
	CXformDynamicGet2DynamicTableScan(
		const CXformDynamicGet2DynamicTableScan &);

public:
	// ctor
	explicit CXformDynamicGet2DynamicTableScan(CMemoryPool *mp);

	// dtor
	virtual ~CXformDynamicGet2DynamicTableScan()
	{
	}

	// ident accessors
	virtual EXformId
	Exfid() const
	{
		return ExfDynamicGet2DynamicTableScan;
	}

	// return a string for xform name
	virtual const CHAR *
	SzId() const
	{
		return "CXformDynamicGet2DynamicTableScan";
	}

	// compute xform promise for a given expression handle
	virtual EXformPromise
	Exfp(CExpressionHandle &exprhdl) const
	{
		CLogicalDynamicGet *popGet =
			CLogicalDynamicGet::PopConvert(exprhdl.Pop());
		CTableDescriptor *ptabdesc = popGet->Ptabdesc();
		CMDAccessor *mda = COptCtxt::PoctxtFromTLS()->Pmda();

		const IMDRelation *relation = mda->RetrieveRel(ptabdesc->MDId());
		if (relation->HasExternalPartitions() && !popGet->IsPartial())
		{
			SPQOS_ASSERT(SPQOS_FTRACE(EopttraceEnableExternalPartitionedTables));
			// In case the relation has any external partition tables, they must
			// first be extracted into partial scans and a MulitExternalGet by
			// CXformExpandDynamicGetWithExternalPartitions, before this
			// DynamicGet can be implemented as a DynamicTableScan
			return CXform::ExfpNone;
		}
		return CXform::ExfpHigh;
	}

	// actual transform
	void Transform(CXformContext *pxfctxt, CXformResult *pxfres,
				   CExpression *pexpr) const;

};	// class CXformDynamicGet2DynamicTableScan

}  // namespace spqopt


#endif	// !SPQOPT_CXformDynamicGet2DynamicTableScan_H

// EOF

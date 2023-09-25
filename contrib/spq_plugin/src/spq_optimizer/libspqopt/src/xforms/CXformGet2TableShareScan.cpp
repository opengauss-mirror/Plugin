//---------------------------------------------------------------------------
//    Greenplum Database
//    Copyright (C) 2009 Greenplum, Inc.
//    Copyright (C) 2021, HuaWei Group Holding Limited
//
//    @filename:
//        CXformGet2TableShareScan.cpp
//
//    @doc:
//        Implementation of transform
//---------------------------------------------------------------------------

#include "spqopt/xforms/CXformGet2TableShareScan.h"

#include "spqopt/operators/CExpressionHandle.h"
#include "spqopt/operators/CLogicalGet.h"
#include "spqopt/operators/CPhysicalTableShareScan.h"
#include "spqopt/metadata/CTableDescriptor.h"

using namespace spqopt;


//---------------------------------------------------------------------------
//    @function:
//        CXformGet2TableShareScan::CXformGet2TableShareScan
//
//    @doc:
//        Ctor
//
//---------------------------------------------------------------------------
CXformGet2TableShareScan::CXformGet2TableShareScan
    (
    CMemoryPool *mp
    )
    :
    CXformImplementation
        (
         // pattern
        SPQOS_NEW(mp) CExpression
                (
                mp,
                SPQOS_NEW(mp) CLogicalGet(mp)
                )
        )
{}

//---------------------------------------------------------------------------
//    @function:
//        CXformGet2TableShareScan::Exfp
//
//    @doc:
//        Compute promise of xform
//
//---------------------------------------------------------------------------
CXform::EXformPromise 
CXformGet2TableShareScan::Exfp
    (
    CExpressionHandle &exprhdl
    )
    const
{
    CLogicalGet *popGet = CLogicalGet::PopConvert(exprhdl.Pop());
    
    CTableDescriptor *ptabdesc = popGet->Ptabdesc();
    if (ptabdesc->IsPartitioned())
    {
        return CXform::ExfpNone;
    }
    
    return CXform::ExfpHigh;
}


//---------------------------------------------------------------------------
//    @function:
//        CXformGet2TableShareScan::Transform
//
//    @doc:
//        Actual transformation
//
//---------------------------------------------------------------------------
void
CXformGet2TableShareScan::Transform
    (
    CXformContext *pxfctxt,
    CXformResult *pxfres,
    CExpression *pexpr
    )
    const
{
    SPQOS_ASSERT(NULL != pxfctxt);
    SPQOS_ASSERT(FPromising(pxfctxt->Pmp(), this, pexpr));
    SPQOS_ASSERT(FCheckPattern(pexpr));

    CLogicalGet *popGet = CLogicalGet::PopConvert(pexpr->Pop());
    CMemoryPool *mp = pxfctxt->Pmp();

    // create/extract components for alternative
    CName *pname = SPQOS_NEW(mp) CName(mp, popGet->Name());
    
    CTableDescriptor *ptabdesc = popGet->Ptabdesc();
    ptabdesc->AddRef();
    
    CColRefArray *pdrgpcrOutput = popGet->PdrspqcrOutput();
    SPQOS_ASSERT(NULL != pdrgpcrOutput);

    pdrgpcrOutput->AddRef();
    
    // create alternative expression
    CExpression *pexprAlt = SPQOS_NEW(mp) CExpression(
        mp,
        SPQOS_NEW(mp) CPhysicalTableShareScan(mp, pname, ptabdesc, pdrgpcrOutput));
    // add alternative to transformation result
    pxfres->Add(pexprAlt);
}


// EOF


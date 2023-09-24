//---------------------------------------------------------------------------
//
// GaussDB SPQ Optimizer
//
//    Copyright (C) 2021, HuaWei Group Holding Limited
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
//    @filename:
//        CXformIndexGet2ShareIndexScan.cpp
//
//    @doc:
//        Implementation of transform
//---------------------------------------------------------------------------

#include "spqopt/xforms/CXformIndexGet2ShareIndexScan.h"


#include "spqopt/metadata/CIndexDescriptor.h"
#include "spqopt/metadata/CTableDescriptor.h"
#include "spqopt/operators/CExpressionHandle.h"
#include "spqopt/operators/CLogicalIndexGet.h"
#include "spqopt/operators/CPatternLeaf.h"
#include "spqopt/operators/CPhysicalShareIndexScan.h"

using namespace spqopt;


//---------------------------------------------------------------------------
//    @function:
//        CXformIndexGet2ShareIndexScan::CXformIndexGet2ShareIndexScan
//
//    @doc:
//        Ctor
//
//---------------------------------------------------------------------------
CXformIndexGet2ShareIndexScan::CXformIndexGet2ShareIndexScan(CMemoryPool *mp)
    :  // pattern
      CXformImplementation(SPQOS_NEW(mp) CExpression(
          mp, SPQOS_NEW(mp) CLogicalIndexGet(mp),
          SPQOS_NEW(mp) CExpression(
              mp, SPQOS_NEW(mp) CPatternLeaf(mp))  // index lookup predicate
          ))
{
}

CXform::EXformPromise
CXformIndexGet2ShareIndexScan::Exfp(CExpressionHandle &exprhdl) const
{
    CLogicalIndexGet *popGet = CLogicalIndexGet::PopConvert(exprhdl.Pop());

    CTableDescriptor *ptabdesc = popGet->Ptabdesc();
    CIndexDescriptor *pindexdesc = popGet->Pindexdesc();

    if (pindexdesc->IndexType() == IMDIndex::EmdindBtree &&
        ptabdesc->IsAORowOrColTable())
    {
        // we don't support btree index scans on AO tables
        return CXform::ExfpNone;
    }

    if (exprhdl.DeriveHasSubquery(0))
    {
        return CXform::ExfpNone;
    }

    return CXform::ExfpHigh;
}

//---------------------------------------------------------------------------
//    @function:
//        CXformIndexGet2ShareIndexScan::Transform
//
//    @doc:
//        Actual transformation
//
//---------------------------------------------------------------------------
void
CXformIndexGet2ShareIndexScan::Transform(CXformContext *pxfctxt,
                                    CXformResult *pxfres,
                                    CExpression *pexpr) const
{
    SPQOS_ASSERT(nullptr != pxfctxt);
    SPQOS_ASSERT(FPromising(pxfctxt->Pmp(), this, pexpr));
    SPQOS_ASSERT(FCheckPattern(pexpr));

    CLogicalIndexGet *pop = CLogicalIndexGet::PopConvert(pexpr->Pop());
    CMemoryPool *mp = pxfctxt->Pmp();
    CIndexDescriptor *pindexdesc = pop->Pindexdesc();
    CTableDescriptor *ptabdesc = pop->Ptabdesc();

    // extract components
    CExpression *pexprIndexCond = (*pexpr)[0];
    if (pexprIndexCond->DeriveHasSubquery())
    {
        return;
    }

    pindexdesc->AddRef();
    ptabdesc->AddRef();

    CColRefArray *pdrspqcrOutput = pop->PdrspqcrOutput();
    SPQOS_ASSERT(nullptr != pdrspqcrOutput);
    pdrspqcrOutput->AddRef();

    COrderSpec *pos = pop->Pos();
    SPQOS_ASSERT(nullptr != pos);
    pos->AddRef();

    // addref all children
    pexprIndexCond->AddRef();

    CExpression *pexprAlt = SPQOS_NEW(mp) CExpression(
        mp,
        SPQOS_NEW(mp) CPhysicalShareIndexScan(mp, pindexdesc, ptabdesc, pexpr->Pop()->UlOpId(),
            SPQOS_NEW(mp) CName(mp, pop->NameAlias()), pdrspqcrOutput, pos),
        pexprIndexCond);
    pxfres->Add(pexprAlt);
}


// EOF

//---------------------------------------------------------------------------
//
// gaussdb SPQ Optimizer
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
//        CPhysicalShareIndexScan.cpp
//
//    @doc:
//        Implementation of index scan operator
//---------------------------------------------------------------------------

#include "spqopt/operators/CPhysicalShareIndexScan.h"

#include "spqos/base.h"

#include "spqopt/base/CUtils.h"
#include "spqopt/operators/CExpressionHandle.h"
#include "spqopt/base/CDistributionSpec.h"
#include "spqopt/base/CDistributionSpecReplicated.h"

using namespace spqopt;


//---------------------------------------------------------------------------
//    @function:
//        CPhysicalShareIndexScan::CPhysicalShareIndexScan
//
//    @doc:
//        Ctor
//
//---------------------------------------------------------------------------
CPhysicalShareIndexScan::CPhysicalShareIndexScan(
    CMemoryPool *mp, CIndexDescriptor *pindexdesc, CTableDescriptor *ptabdesc,
    ULONG ulOriginOpId, const CName *pnameAlias, CColRefArray *pdrspqcrOutput,
    COrderSpec *pos)
    : CPhysicalScan(mp, pnameAlias, ptabdesc, pdrspqcrOutput),
      m_pindexdesc(pindexdesc),
      m_ulOriginOpId(ulOriginOpId),
      m_pos(pos)
{
    SPQOS_ASSERT(nullptr != pindexdesc);
    SPQOS_ASSERT(nullptr != pos);

    SPQOS_DELETE(m_pds);
    m_pds = SPQOS_NEW(mp) CDistributionSpecReplicated(
                CDistributionSpec::EdtStrictReplicated);
}


//---------------------------------------------------------------------------
//    @function:
//        CPhysicalShareIndexScan::~CPhysicalShareIndexScan
//
//    @doc:
//        Dtor
//
//---------------------------------------------------------------------------
CPhysicalShareIndexScan::~CPhysicalShareIndexScan()
{
    m_pindexdesc->Release();
    m_pos->Release();
}

//---------------------------------------------------------------------------
//    @function:
//        CPhysicalShareIndexScan::EpetOrder
//
//    @doc:
//        Return the enforcing type for order property based on this operator
//
//---------------------------------------------------------------------------
CEnfdProp::EPropEnforcingType
CPhysicalShareIndexScan::EpetOrder(CExpressionHandle &,    // exprhdl
                              const CEnfdOrder *peo) const
{
    SPQOS_ASSERT(nullptr != peo);
    SPQOS_ASSERT(!peo->PosRequired()->IsEmpty());

    if (peo->FCompatible(m_pos))
    {
        // required order is already established by the index
        return CEnfdProp::EpetUnnecessary;
    }

    return CEnfdProp::EpetRequired;
}

//---------------------------------------------------------------------------
//    @function:
//        CPhysicalShareIndexScan::HashValue
//
//    @doc:
//        Combine pointers for table descriptor, index descriptor and Eop
//
//---------------------------------------------------------------------------
ULONG
CPhysicalShareIndexScan::HashValue() const
{
    ULONG ulHash = spqos::CombineHashes(
        COperator::HashValue(),
        spqos::CombineHashes(m_pindexdesc->MDId()->HashValue(),
                            spqos::HashPtr<CTableDescriptor>(m_ptabdesc)));
    ulHash =
        spqos::CombineHashes(ulHash, CUtils::UlHashColArray(m_pdrspqcrOutput));

    return ulHash;
}


//---------------------------------------------------------------------------
//    @function:
//        CPhysicalShareIndexScan::Matches
//
//    @doc:
//        match operator
//
//---------------------------------------------------------------------------
BOOL
CPhysicalShareIndexScan::Matches(COperator *pop) const
{
    return CUtils::FMatchIndex(this, pop);
}

//---------------------------------------------------------------------------
//    @function:
//        CPhysicalShareIndexScan::OsPrint
//
//    @doc:
//        debug print
//
//---------------------------------------------------------------------------
IOstream &
CPhysicalShareIndexScan::OsPrint(IOstream &os) const
{
    if (m_fPattern)
    {
        return COperator::OsPrint(os);
    }

    os << SzId() << " ";
    // index name
    os << "  Index Name: (";
    m_pindexdesc->Name().OsPrint(os);
    // table name
    os << ")";
    os << ", Table Name: (";
    m_ptabdesc->Name().OsPrint(os);
    os << ")";
    os << ", Columns: [";
    CUtils::OsPrintDrgPcr(os, m_pdrspqcrOutput);
    os << "]";

    return os;
}

//---------------------------------------------------------------------------
//    @function:
//        CPhysicalShareIndexScan::PdsDerive
//
//    @doc:
//        Derive distribution
//
//---------------------------------------------------------------------------
CDistributionSpec *
CPhysicalShareIndexScan::PdsDerive
    (
    CMemoryPool *mp,
    CExpressionHandle &
    )
    const
{
    return SPQOS_NEW(mp) CDistributionSpecReplicated(CDistributionSpec::EdtStrictReplicated);
}
// EOF

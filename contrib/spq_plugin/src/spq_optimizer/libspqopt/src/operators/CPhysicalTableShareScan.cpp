//---------------------------------------------------------------------------
//    Greenplum Database
//    Copyright (C) 2009 Greenplum, Inc.
//    Copyright (C) 2021, HuaWei Group Holding Limited
//
//    @filename:
//        CPhysicalTableShareScan.cpp
//
//    @doc:
//        Implementation of basic table scan operator
//---------------------------------------------------------------------------

#include "spqopt/base/CUtils.h"
#include "spqopt/base/CDistributionSpec.h"
#include "spqopt/base/CDistributionSpecHashed.h"
#include "spqopt/base/CDistributionSpecReplicated.h"
#include "spqopt/base/CDistributionSpecRandom.h"
#include "spqopt/base/CDistributionSpecSingleton.h"

#include "spqopt/operators/CPhysicalTableShareScan.h"
#include "spqopt/metadata/CTableDescriptor.h"
#include "spqopt/metadata/CName.h"

using namespace spqopt;


//---------------------------------------------------------------------------
//    @function:
//        CPhysicalTableShareScan::CPhysicalTableShareScan
//
//    @doc:
//        ctor
//
//---------------------------------------------------------------------------
CPhysicalTableShareScan::CPhysicalTableShareScan
    (
    CMemoryPool *mp,
    const CName *pnameAlias,
    CTableDescriptor *ptabdesc,
    CColRefArray *pdrgpcrOutput
    )
    :
    CPhysicalScan(mp, pnameAlias, ptabdesc, pdrgpcrOutput)
{
    SPQOS_DELETE(m_pds);
    m_pds = SPQOS_NEW(mp) CDistributionSpecReplicated(
                CDistributionSpec::EdtStrictReplicated);
}

//---------------------------------------------------------------------------
//    @function:
//        CPhysicalTableShareScan::HashValue
//
//    @doc:
//        Combine pointer for table descriptor and Eop
//
//---------------------------------------------------------------------------
ULONG
CPhysicalTableShareScan::HashValue() const
{
    ULONG ulHash = spqos::CombineHashes(COperator::HashValue(), m_ptabdesc->MDId()->HashValue());
    ulHash = spqos::CombineHashes(ulHash, CUtils::UlHashColArray(m_pdrspqcrOutput));

    return ulHash;
}

    
//---------------------------------------------------------------------------
//    @function:
//        CPhysicalTableShareScan::Matches
//
//    @doc:
//        match operator
//
//---------------------------------------------------------------------------
BOOL
CPhysicalTableShareScan::Matches
    (
    COperator *pop
    )
    const
{
    if (Eopid() != pop->Eopid())
    {
        return false;
    }

    CPhysicalTableShareScan *popTableScan = CPhysicalTableShareScan::PopConvert(pop);
    return m_ptabdesc->MDId()->Equals(popTableScan->Ptabdesc()->MDId()) &&
            m_pdrspqcrOutput->Equals(popTableScan->PdrspqcrOutput());
}

//---------------------------------------------------------------------------
//    @function:
//        CPhysicalScan::PdsDerive
//
//    @doc:
//        Derive distribution
//
//---------------------------------------------------------------------------
CDistributionSpec *
CPhysicalTableShareScan::PdsDerive
    (
    CMemoryPool *mp,
    CExpressionHandle &
    )
    const
{
    return SPQOS_NEW(mp) CDistributionSpecReplicated(CDistributionSpec::EdtStrictReplicated);
}


//---------------------------------------------------------------------------
//    @function:
//        CPhysicalTableShareScan::OsPrint
//
//    @doc:
//        debug print
//
//---------------------------------------------------------------------------
IOstream &
CPhysicalTableShareScan::OsPrint
    (
    IOstream &os
    )
    const
{
    os << SzId() << " ";
    
    // alias of table as referenced in the query
    m_pnameAlias->OsPrint(os);

    // actual name of table in catalog and columns
    os << " (";
    m_ptabdesc->Name().OsPrint(os);
    os << ")";
    
    return os;
}



// EOF


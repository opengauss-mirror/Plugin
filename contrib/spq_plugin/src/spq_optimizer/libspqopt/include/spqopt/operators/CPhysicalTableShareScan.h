//---------------------------------------------------------------------------
//    Greenplum Database
//    Copyright (C) 2009 Greenplum, Inc.
//    Copyright (C) 2021, HuaWei Group Holding Limited
//
//    @filename:
//        CPhysicalTableShareScan.h
//
//    @doc:
//        Table scan operator
//---------------------------------------------------------------------------
#ifndef SPQOPT_CPhysicalTableShareScan_H
#define SPQOPT_CPhysicalTableShareScan_H

#include "spqos/base.h"
#include "spqopt/operators/CPhysicalScan.h"

namespace spqopt
{
    
    //---------------------------------------------------------------------------
    //    @class:
    //        CPhysicalTableShareScan
    //
    //    @doc:
    //        Table scan operator
    //
    //---------------------------------------------------------------------------
    class CPhysicalTableShareScan : public CPhysicalScan
    {

        private:

            // private copy ctor
            CPhysicalTableShareScan(const CPhysicalTableShareScan&);

        public:
        
            // ctors
            explicit CPhysicalTableShareScan(CMemoryPool *mp);
            CPhysicalTableShareScan(CMemoryPool *, const CName *, CTableDescriptor *, CColRefArray *);

            // ident accessors
            virtual 
            EOperatorId Eopid() const override
            {
                return EopPhysicalTableShareScan;
            }
            
            // return a string for operator name
            virtual 
            const CHAR *SzId() const override
            {
                return "CPhysicalTableShareScan";
            }
            
            // operator specific hash function
            virtual ULONG HashValue() const override;
            
            // match function
            BOOL Matches(COperator *) const override;
            // derive partition index map
            virtual CPartIndexMap *PpimDerive(CMemoryPool *mp, CExpressionHandle &,	 // exprhdl
            CDrvdPropCtxt *) const      //pdpctxt
            {
                return SPQOS_NEW(mp) CPartIndexMap(mp);
            }
            //-------------------------------------------------------------------------------------
            //-------------------------------------------------------------------------------------
            //-------------------------------------------------------------------------------------

            // debug print
            virtual 
            IOstream &OsPrint(IOstream &) const override;


            // conversion function
            static
            CPhysicalTableShareScan *PopConvert
                (
                COperator *pop
                )
            {
                SPQOS_ASSERT(NULL != pop);
                SPQOS_ASSERT(EopPhysicalTableShareScan == pop->Eopid());

                return reinterpret_cast<CPhysicalTableShareScan*>(pop);
            }

            // statistics derivation during costing
            virtual
            IStatistics *PstatsDerive
                (
                CMemoryPool *, // mp
                CExpressionHandle &, // exprhdl
                CReqdPropPlan *, // prpplan
                IStatisticsArray * //stats_ctxt
                )
                const override
            {
                SPQOS_ASSERT(!"stats derivation during costing for table scan is invalid");

                return NULL;
            }

            // derive distribution
            virtual
            CDistributionSpec *PdsDerive(CMemoryPool *mp, CExpressionHandle &exprhdl) const override; 
            
            virtual
            CRewindabilitySpec *PrsDerive
                (
                CMemoryPool *mp,
                CExpressionHandle & // exprhdl
                )
                const override
            {
                // mark-restorability of output is always true
                return SPQOS_NEW(mp) CRewindabilitySpec(CRewindabilitySpec::ErtMarkRestore, CRewindabilitySpec::EmhtNoMotion);
            }


    }; // class CPhysicalTableShareScan

}

#endif // !SPQOPT_CPhysicalTableShareScan_H

// EOF

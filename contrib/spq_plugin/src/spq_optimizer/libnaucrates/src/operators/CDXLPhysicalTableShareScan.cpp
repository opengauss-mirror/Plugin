//---------------------------------------------------------------------------
//    Greenplum Database
//    Copyright (C) 2010 Greenplum, Inc.
//    Copyright (C) 2021, HuaWei Group Holding Limited
//
//    @filename:
//        CDXLPhysicalTableShareScan.cpp
//
//    @doc:
//        Implementation of DXL physical table scan operators
//---------------------------------------------------------------------------


#include "naucrates/dxl/operators/CDXLPhysicalTableShareScan.h"

#include "naucrates/dxl/operators/CDXLNode.h"
#include "naucrates/dxl/xml/CXMLSerializer.h"

using namespace spqos;
using namespace spqdxl;

//---------------------------------------------------------------------------
//    @function:
//        CDXLPhysicalTableShareScan::CDXLPhysicalTableShareScan
//
//    @doc:
//        Construct a table scan node with uninitialized table descriptor
//
//---------------------------------------------------------------------------
CDXLPhysicalTableShareScan::CDXLPhysicalTableShareScan
    (
    CMemoryPool *mp
    )
    :
    CDXLPhysical(mp),
    m_dxl_table_descr(NULL)
{
}


//---------------------------------------------------------------------------
//    @function:
//        CDXLPhysicalTableShareScan::CDXLPhysicalTableShareScan
//
//    @doc:
//        Construct a table scan node given its table descriptor
//
//---------------------------------------------------------------------------
CDXLPhysicalTableShareScan::CDXLPhysicalTableShareScan
    (
    CMemoryPool *mp,
    CDXLTableDescr *table_descr
    )
    :CDXLPhysical(mp),
     m_dxl_table_descr(table_descr)
{
}


//---------------------------------------------------------------------------
//    @function:
//        CDXLPhysicalTableShareScan::~CDXLPhysicalTableShareScan
//
//    @doc:
//        Destructor
//
//---------------------------------------------------------------------------
CDXLPhysicalTableShareScan::~CDXLPhysicalTableShareScan()
{
    CRefCount::SafeRelease(m_dxl_table_descr);
}


//---------------------------------------------------------------------------
//    @function:
//        CDXLPhysicalTableShareScan::SetTableDescriptor
//
//    @doc:
//        Set table descriptor
//
//---------------------------------------------------------------------------
void
CDXLPhysicalTableShareScan::SetTableDescriptor
    (
    CDXLTableDescr *table_descr
    )
{
    // allow setting table descriptor only once
    SPQOS_ASSERT(NULL == m_dxl_table_descr);
    
    m_dxl_table_descr = table_descr;
}

//---------------------------------------------------------------------------
//    @function:
//        CDXLPhysicalTableShareScan::GetDXLOperator
//
//    @doc:
//        Operator type
//
//---------------------------------------------------------------------------
Edxlopid
CDXLPhysicalTableShareScan::GetDXLOperator() const
{
    return EdxlopPhysicalTableShareScan;
}


//---------------------------------------------------------------------------
//    @function:
//        CDXLPhysicalTableShareScan::GetOpNameStr
//
//    @doc:
//        Operator name
//
//---------------------------------------------------------------------------
const CWStringConst *
CDXLPhysicalTableShareScan::GetOpNameStr() const
{
    return CDXLTokens::GetDXLTokenStr(EdxltokenPhysicalTableShareScan);
}

//---------------------------------------------------------------------------
//    @function:
//        CDXLPhysicalTableShareScan::GetDXLTableDescr
//
//    @doc:
//        Table descriptor for the table scan
//
//---------------------------------------------------------------------------
const CDXLTableDescr *
CDXLPhysicalTableShareScan::GetDXLTableDescr()
{
    return m_dxl_table_descr;
}


//---------------------------------------------------------------------------
//    @function:
//        CDXLPhysicalTableShareScan::SerializeToDXL
//
//    @doc:
//        Serialize operator in DXL format
//
//---------------------------------------------------------------------------
void
CDXLPhysicalTableShareScan::SerializeToDXL
    (
    CXMLSerializer *xml_serializer,
    const CDXLNode *dxlnode
    )
    const
{
    const CWStringConst *element_name = GetOpNameStr();
    
    xml_serializer->OpenElement(CDXLTokens::GetDXLTokenStr(EdxltokenNamespacePrefix), element_name);
    
    // serialize properties
    dxlnode->SerializePropertiesToDXL(xml_serializer);
    
    // serialize children
    dxlnode->SerializeChildrenToDXL(xml_serializer);
    
    // serialize table descriptor
    m_dxl_table_descr->SerializeToDXL(xml_serializer);
    
    xml_serializer->CloseElement(CDXLTokens::GetDXLTokenStr(EdxltokenNamespacePrefix), element_name);        
}

#ifdef SPQOS_DEBUG
//---------------------------------------------------------------------------
//    @function:
//        CDXLPhysicalTableShareScan::AssertValid
//
//    @doc:
//        Checks whether operator node is well-structured 
//
//---------------------------------------------------------------------------
void
CDXLPhysicalTableShareScan::AssertValid
    (
    const CDXLNode *dxlnode,
    BOOL validate_children
    ) 
    const
{
    // assert proj list and filter are valid
    CDXLPhysical::AssertValid(dxlnode, validate_children);
    
    // table scan has only 2 children
    SPQOS_ASSERT(2 == dxlnode->Arity());
    
    // assert validity of table descriptor
    SPQOS_ASSERT(NULL != m_dxl_table_descr);
    SPQOS_ASSERT(NULL != m_dxl_table_descr->MdName());
    SPQOS_ASSERT(m_dxl_table_descr->MdName()->GetMDName()->IsValid());
}
#endif // SPQOS_DEBUG

// EOF

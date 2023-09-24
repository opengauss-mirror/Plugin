//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2009 Greenplum, Inc.
//
//	@filename:
//		CDXLPhysicalProperties.h
//
//	@doc:
//		Representation of properties of a physical DXL operator
//---------------------------------------------------------------------------
#ifndef SPQDXL_CDXLPhysicalProperties_H
#define SPQDXL_CDXLPhysicalProperties_H

#include "spqos/base.h"

#include "naucrates/dxl/operators/CDXLOperatorCost.h"
#include "naucrates/dxl/operators/CDXLProperties.h"

namespace spqdxl
{
using namespace spqos;

//---------------------------------------------------------------------------
//	@class:
//		CDXLPhysicalProperties
//
//	@doc:
//		Container for the properties of a physical operator node, such as
//		cost and output columns
//
//---------------------------------------------------------------------------
class CDXLPhysicalProperties : public CDXLProperties
{
private:
	// cost estimate
	CDXLOperatorCost *m_operator_cost_dxl;

	// private copy ctor
	CDXLPhysicalProperties(const CDXLPhysicalProperties &);

public:
	// ctor
	explicit CDXLPhysicalProperties(CDXLOperatorCost *cost);

	// dtor
	virtual ~CDXLPhysicalProperties();

	// serialize properties in DXL format
	void SerializePropertiesToDXL(CXMLSerializer *xml_serializer) const;

	// accessors
	// the cost estimates for the operator node
	CDXLOperatorCost *GetDXLOperatorCost() const;

	virtual Edxlproperty
	GetDXLPropertyType() const
	{
		return EdxlpropertyPhysical;
	}

	// conversion function
	static CDXLPhysicalProperties *
	PdxlpropConvert(CDXLProperties *dxl_properties)
	{
		SPQOS_ASSERT(NULL != dxl_properties);
		SPQOS_ASSERT(EdxlpropertyPhysical ==
					dxl_properties->GetDXLPropertyType());
		return dynamic_cast<CDXLPhysicalProperties *>(dxl_properties);
	}
};

}  // namespace spqdxl

#endif	// !SPQDXL_CDXLPhysicalProperties_H

// EOF

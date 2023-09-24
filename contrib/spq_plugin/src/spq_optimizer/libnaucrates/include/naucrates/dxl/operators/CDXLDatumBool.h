//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CDXLDatumBool.h
//
//	@doc:
//		Class for representing DXL boolean datum
//
//	@owner:
//
//
//	@test:
//
//---------------------------------------------------------------------------

#ifndef SPQDXL_CDXLDatumBool_H
#define SPQDXL_CDXLDatumBool_H

#include "spqos/base.h"

#include "naucrates/dxl/operators/CDXLDatum.h"

namespace spqdxl
{
using namespace spqos;

// fwd decl
class CXMLSerializer;

//---------------------------------------------------------------------------
//	@class:
//		CDXLDatumBool
//
//	@doc:
//		Class for representing DXL boolean datums
//
//---------------------------------------------------------------------------
class CDXLDatumBool : public CDXLDatum
{
private:
	// boolean value
	BOOL m_value;

	// private copy ctor
	CDXLDatumBool(const CDXLDatumBool &);

public:
	// ctor
	CDXLDatumBool(CMemoryPool *mp, IMDId *mdid_type, BOOL is_null, BOOL value);

	// dtor
	virtual ~CDXLDatumBool()
	{
	}

	// serialize the datum as the given element
	virtual void Serialize(CXMLSerializer *xml_serializer);

	// datum type
	virtual EdxldatumType
	GetDatumType() const
	{
		return CDXLDatum::EdxldatumBool;
	}

	// accessor of boolean value
	BOOL
	GetValue() const
	{
		return m_value;
	}

	// conversion function
	static CDXLDatumBool *
	Cast(CDXLDatum *dxl_datum)
	{
		SPQOS_ASSERT(NULL != dxl_datum);
		SPQOS_ASSERT(CDXLDatum::CDXLDatum::EdxldatumBool ==
					dxl_datum->GetDatumType());

		return dynamic_cast<CDXLDatumBool *>(dxl_datum);
	}
};
}  // namespace spqdxl

#endif	// !SPQDXL_CDXLDatumBool_H

// EOF

//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CDXLDatumInt4.h
//
//	@doc:
//		Class for representing DXL integer datum
//
//	@owner:
//
//
//	@test:
//
//---------------------------------------------------------------------------

#ifndef SPQDXL_CDXLDatumInt4_H
#define SPQDXL_CDXLDatumInt4_H

#include "spqos/base.h"

#include "naucrates/dxl/operators/CDXLDatum.h"

namespace spqdxl
{
using namespace spqos;

// fwd decl
class CXMLSerializer;

//---------------------------------------------------------------------------
//	@class:
//		CDXLDatumInt4
//
//	@doc:
//		Class for representing DXL integer datums
//
//---------------------------------------------------------------------------
class CDXLDatumInt4 : public CDXLDatum
{
private:
	// int4 value
	INT m_val;

	// private copy ctor
	CDXLDatumInt4(const CDXLDatumInt4 &);

public:
	// ctor
	CDXLDatumInt4(CMemoryPool *mp, IMDId *mdid_type, BOOL is_null, INT val);

	// dtor
	virtual ~CDXLDatumInt4(){};

	// accessor of int value
	INT Value() const;

	// serialize the datum as the given element
	virtual void Serialize(CXMLSerializer *xml_serializer);

	// datum type
	virtual EdxldatumType
	GetDatumType() const
	{
		return CDXLDatum::EdxldatumInt4;
	}

	// conversion function
	static CDXLDatumInt4 *
	Cast(CDXLDatum *dxl_datum)
	{
		SPQOS_ASSERT(NULL != dxl_datum);
		SPQOS_ASSERT(CDXLDatum::EdxldatumInt4 == dxl_datum->GetDatumType());

		return dynamic_cast<CDXLDatumInt4 *>(dxl_datum);
	}
};
}  // namespace spqdxl



#endif	// !SPQDXL_CDXLDatumInt4_H

// EOF

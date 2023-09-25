//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CDXLDatumGeneric.h
//
//	@doc:
//		Class for representing DXL datum of type generic
//
//	@owner:
//
//
//	@test:
//
//---------------------------------------------------------------------------

#ifndef SPQDXL_CDXLDatumGeneric_H
#define SPQDXL_CDXLDatumGeneric_H

#include "spqos/base.h"
#include "spqos/common/CDouble.h"

#include "naucrates/dxl/operators/CDXLDatum.h"

namespace spqdxl
{
using namespace spqos;

// fwd decl
class CXMLSerializer;

//---------------------------------------------------------------------------
//	@class:
//		CDXLDatumGeneric
//
//	@doc:
//		Class for representing DXL datum of type generic
//
//---------------------------------------------------------------------------
class CDXLDatumGeneric : public CDXLDatum
{
private:
	// private copy ctor
	CDXLDatumGeneric(const CDXLDatumGeneric &);

protected:
	// datum byte array
	BYTE *m_byte_array;

public:
	// ctor
	CDXLDatumGeneric(CMemoryPool *mp, IMDId *mdid_type, INT type_modifier,
					 BOOL is_null, BYTE *data, ULONG length);

	// dtor
	virtual ~CDXLDatumGeneric();

	// byte array
	const BYTE *GetByteArray() const;

	// serialize the datum as the given element
	virtual void Serialize(CXMLSerializer *xml_serializer);

	// datum type
	virtual EdxldatumType
	GetDatumType() const
	{
		return CDXLDatum::EdxldatumGeneric;
	}

	// conversion function
	static CDXLDatumGeneric *
	Cast(CDXLDatum *dxl_datum)
	{
		SPQOS_ASSERT(NULL != dxl_datum);
		SPQOS_ASSERT(CDXLDatum::EdxldatumGeneric == dxl_datum->GetDatumType() ||
					CDXLDatum::EdxldatumStatsDoubleMappable ==
						dxl_datum->GetDatumType() ||
					CDXLDatum::EdxldatumStatsLintMappable ==
						dxl_datum->GetDatumType());

		return dynamic_cast<CDXLDatumGeneric *>(dxl_datum);
	}

	// statistics related APIs

	// can datum be mapped to LINT
	virtual BOOL
	IsDatumMappableToLINT() const
	{
		return false;
	}

	// return the lint mapping needed for statistics computation
	virtual LINT
	GetLINTMapping() const
	{
		SPQOS_ASSERT(IsDatumMappableToLINT());

		return 0;
	}

	// can datum be mapped to a double
	virtual BOOL
	IsDatumMappableToDouble() const
	{
		return false;
	}

	// return the double mapping needed for statistics computation
	virtual CDouble
	GetDoubleMapping() const
	{
		SPQOS_ASSERT(IsDatumMappableToDouble());
		return 0;
	}
};
}  // namespace spqdxl

#endif	// !SPQDXL_CDXLDatumGeneric_H

// EOF

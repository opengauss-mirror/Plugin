//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2014 Pivotal Inc.
//
//	@filename:
//		CDXLDatumStatsDoubleMappable.h
//
//	@doc:
//		Class for representing DXL datum of types having double mapping
//
//	@owner:
//
//
//	@test:
//
//---------------------------------------------------------------------------

#ifndef SPQDXL_CDXLDatumStatsDoubleMappable_H
#define SPQDXL_CDXLDatumStatsDoubleMappable_H

#include "spqos/base.h"
#include "spqos/common/CDouble.h"

#include "naucrates/dxl/operators/CDXLDatumGeneric.h"

namespace spqdxl
{
using namespace spqos;

// fwd decl
class CXMLSerializer;

//---------------------------------------------------------------------------
//	@class:
//		CDXLDatumStatsDoubleMappable
//
//	@doc:
//		Class for representing DXL datum of types having double mapping
//
//---------------------------------------------------------------------------
class CDXLDatumStatsDoubleMappable : public CDXLDatumGeneric
{
private:
	// for statistics computation, map to double
	CDouble m_val;

	// private copy ctor
	CDXLDatumStatsDoubleMappable(const CDXLDatumStatsDoubleMappable &);

public:
	// ctor
	CDXLDatumStatsDoubleMappable(CMemoryPool *mp, IMDId *mdid_type,
								 INT type_modifier, BOOL is_null, BYTE *data,
								 ULONG length, CDouble val);

	// dtor
	virtual ~CDXLDatumStatsDoubleMappable(){};

	// serialize the datum as the given element
	virtual void Serialize(CXMLSerializer *xml_serializer);

	// datum type
	virtual EdxldatumType
	GetDatumType() const
	{
		return CDXLDatum::EdxldatumStatsDoubleMappable;
	}

	// statistics related APIs

	// can datum be mapped to double
	virtual BOOL
	IsDatumMappableToDouble() const
	{
		return true;
	}

	// return the double mapping needed for statistics computation
	virtual CDouble
	GetDoubleMapping() const
	{
		return m_val;
	}

	// conversion function
	static CDXLDatumStatsDoubleMappable *
	Cast(CDXLDatum *dxl_datum)
	{
		SPQOS_ASSERT(NULL != dxl_datum);
		SPQOS_ASSERT(CDXLDatum::EdxldatumStatsDoubleMappable ==
					dxl_datum->GetDatumType());

		return dynamic_cast<CDXLDatumStatsDoubleMappable *>(dxl_datum);
	}
};
}  // namespace spqdxl

#endif	// !SPQDXL_CDXLDatumStatsDoubleMappable_H

// EOF

//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2010 Greenplum, Inc.
//
//	@filename:
//		CParseHandlerProperties.h
//
//	@doc:
//		SAX parse handler class for parsing properties of physical operators.
//---------------------------------------------------------------------------

#ifndef SPQDXL_CParseHandlerProperties_H
#define SPQDXL_CParseHandlerProperties_H

#include "spqos/base.h"

#include "naucrates/dxl/operators/CDXLPhysicalProperties.h"
#include "naucrates/dxl/parser/CParseHandlerBase.h"
#include "naucrates/md/CDXLStatsDerivedRelation.h"

namespace spqdxl
{
using namespace spqos;
using namespace spqnaucrates;

XERCES_CPP_NAMESPACE_USE

//---------------------------------------------------------------------------
//	@class:
//		CParseHandlerProperties
//
//	@doc:
//		Parse handler for parsing the properties of a physical operator
//
//---------------------------------------------------------------------------
class CParseHandlerProperties : public CParseHandlerBase
{
private:
	// physical properties container
	CDXLPhysicalProperties *m_dxl_properties;

	// statistics of the physical plan
	CDXLStatsDerivedRelation *m_dxl_stats_derived_relation;

	// private ctor
	CParseHandlerProperties(const CParseHandlerProperties &);

	// process the start of an element
	void StartElement(
		const XMLCh *const element_uri,			// URI of element's namespace
		const XMLCh *const element_local_name,	// local part of element's name
		const XMLCh *const element_qname,		// element's qname
		const Attributes &attr					// element's attributes
	);

	// process the end of an element
	void EndElement(
		const XMLCh *const element_uri,			// URI of element's namespace
		const XMLCh *const element_local_name,	// local part of element's name
		const XMLCh *const element_qname		// element's qname
	);

public:
	// ctor
	CParseHandlerProperties(CMemoryPool *mp,
							CParseHandlerManager *parse_handler_mgr,
							CParseHandlerBase *parse_handler_root);

	// dtor
	virtual ~CParseHandlerProperties();

	// returns the constructed properties container
	CDXLPhysicalProperties *GetProperties() const;

	// return the derived relation statistics
	CDXLStatsDerivedRelation *
	GetDxlStatsDrvdRelation() const
	{
		return m_dxl_stats_derived_relation;
	}
};
}  // namespace spqdxl

#endif	// !SPQDXL_CParseHandlerProperties_H

// EOF

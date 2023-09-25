//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CParseHandlerStatistics.h
//
//	@doc:
//		SAX parse handler class for parsing statistics from a DXL document.
//---------------------------------------------------------------------------

#ifndef SPQDXL_CParseHandlerStatistics_H
#define SPQDXL_CParseHandlerStatistics_H

#include "spqos/base.h"

#include "naucrates/dxl/parser/CParseHandlerBase.h"
#include "naucrates/md/CDXLStatsDerivedRelation.h"

namespace spqdxl
{
using namespace spqos;
using namespace spqmd;
using namespace spqnaucrates;

XERCES_CPP_NAMESPACE_USE

//---------------------------------------------------------------------------
//	@class:
//		CParseHandlerStatistics
//
//	@doc:
//		Parse handler for statistics.
//
//---------------------------------------------------------------------------
class CParseHandlerStatistics : public CParseHandlerBase
{
private:
	// list of derived table statistics
	CDXLStatsDerivedRelationArray *m_dxl_stats_derived_rel_array;

	// private copy ctor
	CParseHandlerStatistics(const CParseHandlerStatistics &);

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
	// ctor/dtor
	CParseHandlerStatistics(CMemoryPool *mp,
							CParseHandlerManager *parse_handler_mgr,
							CParseHandlerBase *parse_handler_root);

	~CParseHandlerStatistics();

	virtual EDxlParseHandlerType GetParseHandlerType() const;

	// return the list of statistics objects
	CDXLStatsDerivedRelationArray *GetStatsDerivedRelDXLArray() const;
};
}  // namespace spqdxl

#endif	// !SPQDXL_CParseHandlerStatistics_H

// EOF

//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		CParseHandlerMDSPQDBAgg.h
//
//	@doc:
//		SAX parse handler class for SPQDB aggregate metadata
//---------------------------------------------------------------------------

#ifndef SPQDXL_CParseHandlerMDSPQDBAgg_H
#define SPQDXL_CParseHandlerMDSPQDBAgg_H

#include "spqos/base.h"

#include "naucrates/dxl/parser/CParseHandlerMetadataObject.h"
#include "naucrates/md/CMDAggregateSPQDB.h"


namespace spqdxl
{
using namespace spqos;
using namespace spqmd;

XERCES_CPP_NAMESPACE_USE

//---------------------------------------------------------------------------
//	@class:
//		CParseHandlerMDSPQDBAgg
//
//	@doc:
//		Parse handler for SPQDB aggregate metadata
//
//---------------------------------------------------------------------------
class CParseHandlerMDSPQDBAgg : public CParseHandlerMetadataObject
{
private:
	// metadata id comprising of id and version info.
	IMDId *m_mdid;

	// name
	CMDName *m_mdname;

	// result type
	IMDId *m_mdid_type_result;

	// intermediate result type
	IMDId *m_mdid_type_intermediate;

	// is aggregate ordered
	BOOL m_is_ordered;

	// is aggregate splittable
	BOOL m_is_splittable;

	// can we use hash aggregation to compute agg function
	BOOL m_hash_agg_capable;

	// private copy ctor
	CParseHandlerMDSPQDBAgg(const CParseHandlerMDSPQDBAgg &);

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
	CParseHandlerMDSPQDBAgg(CMemoryPool *mp,
						   CParseHandlerManager *parse_handler_mgr,
						   CParseHandlerBase *parse_handler_root);
};
}  // namespace spqdxl

#endif	// !SPQDXL_CParseHandlerMDSPQDBAgg_H

// EOF

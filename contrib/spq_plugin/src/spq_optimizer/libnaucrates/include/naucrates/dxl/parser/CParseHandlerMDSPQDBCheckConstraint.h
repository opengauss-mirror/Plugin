//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CParseHandlerMDSPQDBCheckConstraint.h
//
//	@doc:
//		SAX parse handler class for parsing an MD check constraint
//---------------------------------------------------------------------------

#ifndef SPQDXL_CParseHandlerMDSPQDBCheckConstraint_H
#define SPQDXL_CParseHandlerMDSPQDBCheckConstraint_H

#include "spqos/base.h"

#include "naucrates/dxl/parser/CParseHandlerMetadataObject.h"

namespace spqdxl
{
using namespace spqos;
using namespace spqmd;
using namespace spqnaucrates;

XERCES_CPP_NAMESPACE_USE

//---------------------------------------------------------------------------
//	@class:
//		CParseHandlerMDSPQDBCheckConstraint
//
//	@doc:
//		Parse handler class for parsing an MD check constraint
//
//---------------------------------------------------------------------------
class CParseHandlerMDSPQDBCheckConstraint : public CParseHandlerMetadataObject
{
private:
	// mdid of the check constraint
	IMDId *m_mdid;

	// name of the check constraint
	CMDName *m_mdname;

	// mdid of the relation
	IMDId *m_rel_mdid;

	// private copy ctor
	CParseHandlerMDSPQDBCheckConstraint(
		const CParseHandlerMDSPQDBCheckConstraint &);

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
	CParseHandlerMDSPQDBCheckConstraint(CMemoryPool *mp,
									   CParseHandlerManager *parse_handler_mgr,
									   CParseHandlerBase *parse_handler_root);
};
}  // namespace spqdxl

#endif	// !SPQDXL_CParseHandlerMDSPQDBCheckConstraint_H

// EOF

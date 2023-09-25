//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CParseHandlerMDSPQDBTrigger.h
//
//	@doc:
//		SAX parse handler class for SPQDB trigger metadata
//---------------------------------------------------------------------------

#ifndef SPQDXL_CParseHandlerMDSPQDBTrigger_H
#define SPQDXL_CParseHandlerMDSPQDBTrigger_H

#include "spqos/base.h"

#include "naucrates/dxl/parser/CParseHandlerMetadataObject.h"
#include "naucrates/md/CMDTriggerSPQDB.h"


namespace spqdxl
{
using namespace spqos;
using namespace spqmd;

XERCES_CPP_NAMESPACE_USE

//---------------------------------------------------------------------------
//	@class:
//		CParseHandlerMDSPQDBTrigger
//
//	@doc:
//		Parse handler for SPQDB trigger metadata
//
//---------------------------------------------------------------------------
class CParseHandlerMDSPQDBTrigger : public CParseHandlerMetadataObject
{
private:
	// trigger id
	IMDId *m_mdid;

	// trigger name
	CMDName *m_mdname;

	// relation id
	IMDId *m_rel_mdid;

	// function id
	IMDId *m_func_mdid;

	// trigger type
	INT m_type;

	// is trigger enabled
	BOOL m_is_enabled;

	// private copy ctor
	CParseHandlerMDSPQDBTrigger(const CParseHandlerMDSPQDBTrigger &);

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
	CParseHandlerMDSPQDBTrigger(CMemoryPool *mp,
							   CParseHandlerManager *parse_handler_mgr,
							   CParseHandlerBase *parse_handler_root);
};
}  // namespace spqdxl

#endif	// !SPQDXL_CParseHandlerMDSPQDBTrigger_H

// EOF

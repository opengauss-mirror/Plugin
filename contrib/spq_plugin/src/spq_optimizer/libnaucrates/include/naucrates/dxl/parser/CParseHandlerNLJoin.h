//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2010 Greenplum, Inc.
//
//	@filename:
//		CParseHandlerNLJoin.h
//
//	@doc:
//		SAX parse handler class for parsing nested loop join operator nodes.
//---------------------------------------------------------------------------

#ifndef SPQDXL_CParseHandlerNLJoin_H
#define SPQDXL_CParseHandlerNLJoin_H

#include "spqos/base.h"

#include "naucrates/dxl/operators/CDXLPhysicalNLJoin.h"
#include "naucrates/dxl/parser/CParseHandlerPhysicalOp.h"

namespace spqdxl
{
using namespace spqos;

XERCES_CPP_NAMESPACE_USE

// indices of nested loop join elements in the children array
enum ENLJoinParseHandlerChildIndices
{
	EdxlParseHandlerNLJIndexProp = 0,
	EdxlParseHandlerNLJIndexProjList,
	EdxlParseHandlerNLJIndexFilter,
	EdxlParseHandlerNLJIndexJoinFilter,
	EdxlParseHandlerNLJIndexLeftChild,
	EdxlParseHandlerNLJIndexRightChild,
	EdxlParseHandlerNLJIndexNestLoopParams,
	EdxlParseHandlerNLJIndexSentinel
};

//---------------------------------------------------------------------------
//	@class:
//		CParseHandlerNLJoin
//
//	@doc:
//		Parse handler for nested loop join operators
//
//---------------------------------------------------------------------------
class CParseHandlerNLJoin : public CParseHandlerPhysicalOp
{
private:
	// the nested loop join operator
	CDXLPhysicalNLJoin *m_dxl_op;

	// private copy ctor
	CParseHandlerNLJoin(const CParseHandlerNLJoin &);

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
	CParseHandlerNLJoin(CMemoryPool *mp,
						CParseHandlerManager *parse_handler_mgr,
						CParseHandlerBase *parse_handler_root);
};
}  // namespace spqdxl

#endif	// !SPQDXL_CParseHandlerNLJoin_H

// EOF

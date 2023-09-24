//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		CParseHandlerMDSPQDBScalarOp.h
//
//	@doc:
//		SAX parse handler class for SPQDB scalar operator metadata
//---------------------------------------------------------------------------

#ifndef SPQDXL_CParseHandlerMDSPQDBScalarOp_H
#define SPQDXL_CParseHandlerMDSPQDBScalarOp_H

#include "spqos/base.h"

#include "naucrates/dxl/parser/CParseHandlerMetadataObject.h"

namespace spqdxl
{
using namespace spqos;

XERCES_CPP_NAMESPACE_USE

//---------------------------------------------------------------------------
//	@class:
//		CParseHandlerMDSPQDBScalarOp
//
//	@doc:
//		Parse handler for SPQDB scalar operator metadata
//
//---------------------------------------------------------------------------
class CParseHandlerMDSPQDBScalarOp : public CParseHandlerMetadataObject
{
private:
	// id and version
	IMDId *m_mdid;

	// name
	CMDName *m_mdname;

	// type of left operand
	IMDId *m_mdid_type_left;

	// type of right operand
	IMDId *m_mdid_type_right;

	// type of result operand
	IMDId *m_mdid_type_result;

	// id of function which implements the operator
	IMDId *m_func_mdid;

	// id of commute operator
	IMDId *m_mdid_commute_opr;

	// id of inverse operator
	IMDId *m_mdid_inverse_opr;

	// comparison type
	IMDType::ECmpType m_comparision_type;

	// does operator return NULL on NULL input?
	BOOL m_returns_null_on_null_input;

	IMDId *m_mdid_hash_opfamily;
	IMDId *m_mdid_legacy_hash_opfamily;

	// preserves NDVs of inputs
	BOOL m_is_ndv_preserving;

	// private copy ctor
	CParseHandlerMDSPQDBScalarOp(const CParseHandlerMDSPQDBScalarOp &);

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

	// is this a supported child elem of the scalar op
	BOOL IsSupportedChildElem(const XMLCh *const xml_str);

public:
	// ctor
	CParseHandlerMDSPQDBScalarOp(CMemoryPool *mp,
								CParseHandlerManager *parse_handler_mgr,
								CParseHandlerBase *parse_handler_root);
};
}  // namespace spqdxl

#endif	// !SPQDXL_CParseHandlerMDSPQDBScalarOp_H

// EOF

//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2013 EMC Corp.
//
//	@filename:
//		CParseHandlerCostParams.h
//
//	@doc:
//		Parse handler for cost model paerameters
//---------------------------------------------------------------------------

#ifndef SPQDXL_CParseHandlerCostParams_H
#define SPQDXL_CParseHandlerCostParams_H

#include "spqos/base.h"

#include "spqopt/cost/ICostModelParams.h"
#include "naucrates/dxl/parser/CParseHandlerBase.h"

namespace spqdxl
{
using namespace spqos;

//---------------------------------------------------------------------------
//	@class:
//		CParseHandlerCostParams
//
//	@doc:
//		Parse handler for cost model parameters
//
//---------------------------------------------------------------------------
class CParseHandlerCostParams : public CParseHandlerBase
{
private:
	// cost params
	ICostModelParams *m_cost_model_params;

	// private ctor
	CParseHandlerCostParams(const CParseHandlerCostParams &);

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
	CParseHandlerCostParams(CMemoryPool *mp,
							CParseHandlerManager *parse_handler_mgr,
							CParseHandlerBase *parse_handler_root);

	virtual ~CParseHandlerCostParams();

	// returns the dxl representation of cost parameters
	ICostModelParams *
	GetCostModelParams()
	{
		return m_cost_model_params;
	}

	EDxlParseHandlerType
	GetParseHandlerType() const
	{
		return EdxlphCostParams;
	}
};
}  // namespace spqdxl

#endif	// !SPQDXL_CParseHandlerCostParams_H

// EOF

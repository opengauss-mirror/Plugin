//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CParseHandlerStatsDerivedColumn.h
//
//	@doc:
//		Parse handler for derived column statistics
//---------------------------------------------------------------------------

#ifndef SPQDXL_CParseHandlerStatsDerivedColumn_H
#define SPQDXL_CParseHandlerStatsDerivedColumn_H

#include "spqos/base.h"

#include "naucrates/dxl/parser/CParseHandlerBase.h"
#include "naucrates/md/CDXLStatsDerivedColumn.h"

namespace spqdxl
{
using namespace spqos;
using namespace spqmd;

XERCES_CPP_NAMESPACE_USE

//---------------------------------------------------------------------------
//	@class:
//		CParseHandlerStatsDerivedColumn
//
//	@doc:
//		Parse handler for derived column statistics
//
//---------------------------------------------------------------------------
class CParseHandlerStatsDerivedColumn : public CParseHandlerBase
{
private:
	// column id
	ULONG m_colid;

	// width
	CDouble m_width;

	// null fraction
	CDouble m_null_freq;

	// ndistinct of remaining tuples
	CDouble m_distinct_remaining;

	// frequency of remaining tuples
	CDouble m_freq_remaining;

	// derived column stats
	CDXLStatsDerivedColumn *m_dxl_stats_derived_col;

	// private copy ctor
	CParseHandlerStatsDerivedColumn(const CParseHandlerStatsDerivedColumn &);

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
	CParseHandlerStatsDerivedColumn(CMemoryPool *mp,
									CParseHandlerManager *parse_handler_mgr,
									CParseHandlerBase *parse_handler_root);

	//dtor
	~CParseHandlerStatsDerivedColumn();

	// derived column stats
	CDXLStatsDerivedColumn *
	GetDxlStatsDerivedCol() const
	{
		return m_dxl_stats_derived_col;
	}
};
}  // namespace spqdxl

#endif	// !SPQDXL_CParseHandlerStatsDerivedColumn_H

// EOF

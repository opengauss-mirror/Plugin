//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CParseHandlerColStatsBucket.h
//
//	@doc:
//		SAX parse handler class for parsing a bucket of a column stats object
//---------------------------------------------------------------------------

#ifndef SPQDXL_CParseHandlerColStatsBucket_H
#define SPQDXL_CParseHandlerColStatsBucket_H

#include "spqos/base.h"

#include "naucrates/dxl/parser/CParseHandlerMetadataObject.h"

// fwd decl
namespace spqmd
{
class CMDIdColStats;
class CDXLBucket;
}  // namespace spqmd

namespace spqdxl
{
using namespace spqos;
using namespace spqmd;
using namespace spqnaucrates;

XERCES_CPP_NAMESPACE_USE

// fwd decl
class CDXLDatum;

//---------------------------------------------------------------------------
//	@class:
//		CParseHandlerColStatsBucket
//
//	@doc:
//		Parse handler class for buckets of column stats objects
//
//---------------------------------------------------------------------------
class CParseHandlerColStatsBucket : public CParseHandlerBase
{
private:
	// frequency
	CDouble m_frequency;

	// distinct values
	CDouble m_distinct;

	// lower bound value for the bucket
	CDXLDatum *m_lower_bound_dxl_datum;

	// upper bound value for the bucket
	CDXLDatum *m_upper_bound_dxl_datum;

	// is lower bound closed
	BOOL m_is_lower_closed;

	// is upper bound closed
	BOOL m_is_upper_closed;

	// dxl bucket object
	CDXLBucket *m_dxl_bucket;

	// private copy ctor
	CParseHandlerColStatsBucket(const CParseHandlerColStatsBucket &);

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
	CParseHandlerColStatsBucket(CMemoryPool *mp,
								CParseHandlerManager *parse_handler_mgr,
								CParseHandlerBase *parse_handler_base);

	// dtor
	virtual ~CParseHandlerColStatsBucket();

	// returns the constructed bucket
	CDXLBucket *GetDXLBucketAt() const;
};
}  // namespace spqdxl

#endif	// !SPQDXL_CParseHandlerColStatsBucket_H

// EOF

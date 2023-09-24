//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2010 Greenplum, Inc.
//
//	@filename:
//		CParseHandlerMetadataObject.h
//
//	@doc:
//		Base SAX parse handler class for parsing metadata objects.
//---------------------------------------------------------------------------

#ifndef SPQDXL_CParseHandlerMetadataObject_H
#define SPQDXL_CParseHandlerMetadataObject_H

#include "spqos/base.h"

#include "naucrates/dxl/parser/CParseHandlerBase.h"
#include "naucrates/md/IMDCacheObject.h"

namespace spqdxl
{
using namespace spqos;
using namespace spqmd;

XERCES_CPP_NAMESPACE_USE

//---------------------------------------------------------------------------
//	@class:
//		CParseHandlerMetadataObject
//
//	@doc:
//		Base parse handler class for metadata objects
//
//
//---------------------------------------------------------------------------
class CParseHandlerMetadataObject : public CParseHandlerBase
{
private:
	// private copy ctor
	CParseHandlerMetadataObject(const CParseHandlerMetadataObject &);


protected:
	// the metadata object constructed by the parse handler
	IMDCacheObject *m_imd_obj;

public:
	// ctor/dtor
	CParseHandlerMetadataObject(CMemoryPool *mp,
								CParseHandlerManager *parse_handler_mgr,
								CParseHandlerBase *parse_handler_root);

	virtual ~CParseHandlerMetadataObject();

	// returns constructed metadata object
	IMDCacheObject *GetImdObj() const;
};
}  // namespace spqdxl

#endif	// !SPQDXL_CParseHandlerMetadataObject_H

// EOF

#ifndef SPQDXL_CDXLMemoryManager_H
#define SPQDXL_CDXLMemoryManager_H

#include "CDXLMemoryManager.h"

namespace spqdxl
{
using namespace spqos;

XERCES_CPP_NAMESPACE_USE

//---------------------------------------------------------------------------
//	@class:
//		SPQCDXLMemoryManager
//
//	@doc:
//		Memory manager for parsing DXL documents used in the Xerces XML parser.
//		Provides a wrapper around the SPQOS CMemoryPool interface.
//
//---------------------------------------------------------------------------
class SPQCDXLMemoryManager : public CDXLMemoryManager
{
public:
    SPQCDXLMemoryManager(CMemoryPool *mp):CDXLMemoryManager(mp) {

    }
    virtual void *allocate(XMLSize_t xmlsize);

    virtual void deallocate(void *pv);

    };
}

#endif
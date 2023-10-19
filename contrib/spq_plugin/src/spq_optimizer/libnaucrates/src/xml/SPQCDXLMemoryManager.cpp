#include "knl/knl_session.h"
#include "naucrates/dxl/xml/SPQCDXLMemoryManager.h"
using namespace spqdxl;

//---------------------------------------------------------------------------
//	@function:
//		SPQCDXLMemoryManager::allocate
//
//	@doc:
//		Memory allocation.
//
//---------------------------------------------------------------------------
void*
SPQCDXLMemoryManager::allocate(XMLSize_t xmlsize) {

    if (u_sess->spq_cxt.pmpXerces == NULL) {
        return SPQOS_NEW_ARRAY(Pmp(), BYTE, xmlsize);
    } else {
        return SPQOS_NEW_ARRAY(u_sess->spq_cxt.pmpXerces, BYTE, xmlsize);
    }

}

//---------------------------------------------------------------------------
//	@function:
//		SPQCDXLMemoryManager::deallocate
//
//	@doc:
//		Memory deallocation.
//
//---------------------------------------------------------------------------
void
SPQCDXLMemoryManager::deallocate(void *pv)
{
    SPQOS_DELETE_DXL_ARRAY(reinterpret_cast<BYTE *>(pv));
}

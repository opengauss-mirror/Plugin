// This is a generated file

#ifndef _STORAGE_GSTRACE_H
#define _STORAGE_GSTRACE_H

#include "comps.h"
#include "gstrace_infra.h"

#define GS_TRC_ID_InitBufferPool GS_TRC_ID_PACK(COMP_STORAGE, 1)
#define GS_TRC_ID_InitBufferPoolAccess GS_TRC_ID_PACK(COMP_STORAGE, 2)
#define GS_TRC_ID_InitBufferPoolBackend GS_TRC_ID_PACK(COMP_STORAGE, 3)
#define GS_TRC_ID_AtEOXact_Buffers GS_TRC_ID_PACK(COMP_STORAGE, 4)
#define GS_TRC_ID_PrintBufferLeakWarning GS_TRC_ID_PACK(COMP_STORAGE, 5)
#define GS_TRC_ID_CheckPointBuffers GS_TRC_ID_PACK(COMP_STORAGE, 6)
#define GS_TRC_ID_RelationGetNumberOfBlocksInFork GS_TRC_ID_PACK(COMP_STORAGE, 7)
#define GS_TRC_ID_FlushRelationBuffers GS_TRC_ID_PACK(COMP_STORAGE, 8)
#define GS_TRC_ID_FlushDatabaseBuffers GS_TRC_ID_PACK(COMP_STORAGE, 9)
#define GS_TRC_ID_DropRelFileNodeBuffers GS_TRC_ID_PACK(COMP_STORAGE, 10)
#define GS_TRC_ID_DropRelFileNodeAllBuffers GS_TRC_ID_PACK(COMP_STORAGE, 11)
#define GS_TRC_ID_DropDatabaseBuffers GS_TRC_ID_PACK(COMP_STORAGE, 12)
#define GS_TRC_ID_ProcArrayShmemSize GS_TRC_ID_PACK(COMP_STORAGE, 13)
#define GS_TRC_ID_ProcArrayAdd GS_TRC_ID_PACK(COMP_STORAGE, 14)
#define GS_TRC_ID_ProcArrayRemove GS_TRC_ID_PACK(COMP_STORAGE, 15)
#define GS_TRC_ID_ProcArrayEndTransaction GS_TRC_ID_PACK(COMP_STORAGE, 16)
#define GS_TRC_ID_ProcArrayClearTransaction GS_TRC_ID_PACK(COMP_STORAGE, 17)
#define GS_TRC_ID_SetGlobalSnapshotData GS_TRC_ID_PACK(COMP_STORAGE, 18)
#define GS_TRC_ID_UnsetGlobalSnapshotData GS_TRC_ID_PACK(COMP_STORAGE, 19)
#define GS_TRC_ID_ReloadConnInfoOnBackends GS_TRC_ID_PACK(COMP_STORAGE, 20)
#define GS_TRC_ID_DumpMemoryCtxOnBackend GS_TRC_ID_PACK(COMP_STORAGE, 21)
#define GS_TRC_ID_ProcArrayInitRecovery GS_TRC_ID_PACK(COMP_STORAGE, 22)
#define GS_TRC_ID_ProcArrayApplyRecoveryInfo GS_TRC_ID_PACK(COMP_STORAGE, 23)
#define GS_TRC_ID_ProcArrayApplyXidAssignment GS_TRC_ID_PACK(COMP_STORAGE, 24)
#define GS_TRC_ID_RecordKnownAssignedTransactionIds GS_TRC_ID_PACK(COMP_STORAGE, 25)
#define GS_TRC_ID_ExpireTreeKnownAssignedTransactionIds GS_TRC_ID_PACK(COMP_STORAGE, 26)
#define GS_TRC_ID_ExpireAllKnownAssignedTransactionIds GS_TRC_ID_PACK(COMP_STORAGE, 27)
#define GS_TRC_ID_ExpireOldKnownAssignedTransactionIds GS_TRC_ID_PACK(COMP_STORAGE, 28)
#define GS_TRC_ID_GetMaxSnapshotXidCount GS_TRC_ID_PACK(COMP_STORAGE, 29)
#define GS_TRC_ID_GetMaxSnapshotSubxidCount GS_TRC_ID_PACK(COMP_STORAGE, 30)
#define GS_TRC_ID_smgrinit GS_TRC_ID_PACK(COMP_STORAGE, 31)
#define GS_TRC_ID_smgropen GS_TRC_ID_PACK(COMP_STORAGE, 32)
#define GS_TRC_ID_smgrexists GS_TRC_ID_PACK(COMP_STORAGE, 33)
#define GS_TRC_ID_smgrsetowner GS_TRC_ID_PACK(COMP_STORAGE, 34)
#define GS_TRC_ID_smgrclearowner GS_TRC_ID_PACK(COMP_STORAGE, 35)
#define GS_TRC_ID_smgrclose GS_TRC_ID_PACK(COMP_STORAGE, 36)
#define GS_TRC_ID_smgrcloseall GS_TRC_ID_PACK(COMP_STORAGE, 37)
#define GS_TRC_ID_smgrclosenode GS_TRC_ID_PACK(COMP_STORAGE, 38)
#define GS_TRC_ID_smgrcreate GS_TRC_ID_PACK(COMP_STORAGE, 39)
#define GS_TRC_ID_smgrdounlink GS_TRC_ID_PACK(COMP_STORAGE, 40)
#define GS_TRC_ID_smgrdounlinkfork GS_TRC_ID_PACK(COMP_STORAGE, 41)
#define GS_TRC_ID_smgrextend GS_TRC_ID_PACK(COMP_STORAGE, 42)
#define GS_TRC_ID_smgrprefetch GS_TRC_ID_PACK(COMP_STORAGE, 43)
#define GS_TRC_ID_smgrread GS_TRC_ID_PACK(COMP_STORAGE, 44)
#define GS_TRC_ID_smgrwrite GS_TRC_ID_PACK(COMP_STORAGE, 45)
#define GS_TRC_ID_smgrwriteback GS_TRC_ID_PACK(COMP_STORAGE, 46)
#define GS_TRC_ID_smgrnblocks GS_TRC_ID_PACK(COMP_STORAGE, 47)
#define GS_TRC_ID_smgrtruncate GS_TRC_ID_PACK(COMP_STORAGE, 48)
#define GS_TRC_ID_smgrimmedsync GS_TRC_ID_PACK(COMP_STORAGE, 49)
#define GS_TRC_ID_smgrpreckpt GS_TRC_ID_PACK(COMP_STORAGE, 50)
#define GS_TRC_ID_smgrsync GS_TRC_ID_PACK(COMP_STORAGE, 51)
#define GS_TRC_ID_smgrpostckpt GS_TRC_ID_PACK(COMP_STORAGE, 52)
#define GS_TRC_ID_AtEOXact_SMgr GS_TRC_ID_PACK(COMP_STORAGE, 53)
#define GS_TRC_ID_BufferAlloc GS_TRC_ID_PACK(COMP_STORAGE, 54)
#define GS_TRC_ID_SyncLocalXidWait GS_TRC_ID_PACK(COMP_STORAGE, 55)
#define GS_TRC_ID_BufferSync GS_TRC_ID_PACK(COMP_STORAGE, 56)
#define GS_TRC_ID_BgBufferSync GS_TRC_ID_PACK(COMP_STORAGE, 57)
#define GS_TRC_ID_fsm_vacuum_page GS_TRC_ID_PACK(COMP_STORAGE, 58)
#define GS_TRC_ID_fsm_search GS_TRC_ID_PACK(COMP_STORAGE, 59)
#define GS_TRC_ID_fsm_set_and_search GS_TRC_ID_PACK(COMP_STORAGE, 60)

#endif
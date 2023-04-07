// This is a generated file

#ifndef _REPLICATION_GSTRACE_H
#define _REPLICATION_GSTRACE_H

#include "comps.h"
#include "gstrace_infra.h"

#define GS_TRC_ID_WalReceiverMain GS_TRC_ID_PACK(COMP_REPLICATION, 1)
#define GS_TRC_ID_walrcvWriterMain GS_TRC_ID_PACK(COMP_REPLICATION, 2)
#define GS_TRC_ID_WalRcvShmemSize GS_TRC_ID_PACK(COMP_REPLICATION, 3)
#define GS_TRC_ID_WalRcvShmemInit GS_TRC_ID_PACK(COMP_REPLICATION, 4)
#define GS_TRC_ID_ShutdownWalRcv GS_TRC_ID_PACK(COMP_REPLICATION, 5)
#define GS_TRC_ID_WalRcvInProgress GS_TRC_ID_PACK(COMP_REPLICATION, 6)
#define GS_TRC_ID_connect_dn_str GS_TRC_ID_PACK(COMP_REPLICATION, 7)
#define GS_TRC_ID_RequestXLogStreaming GS_TRC_ID_PACK(COMP_REPLICATION, 8)
#define GS_TRC_ID_get_rcv_slot_name GS_TRC_ID_PACK(COMP_REPLICATION, 9)
#define GS_TRC_ID_GetWalRcvWriteRecPtr GS_TRC_ID_PACK(COMP_REPLICATION, 10)
#define GS_TRC_ID_GetReplicationApplyDelay GS_TRC_ID_PACK(COMP_REPLICATION, 11)
#define GS_TRC_ID_GetReplicationTransferLatency GS_TRC_ID_PACK(COMP_REPLICATION, 12)
#define GS_TRC_ID_GetWalRcvDummyStandbySyncPercent GS_TRC_ID_PACK(COMP_REPLICATION, 13)
#define GS_TRC_ID_SetWalRcvDummyStandbySyncPercent GS_TRC_ID_PACK(COMP_REPLICATION, 14)
#define GS_TRC_ID_getCurrentWalRcvCtlBlock GS_TRC_ID_PACK(COMP_REPLICATION, 15)
#define GS_TRC_ID_walRcvWrite GS_TRC_ID_PACK(COMP_REPLICATION, 16)
#define GS_TRC_ID_WalRcvXLogClose GS_TRC_ID_PACK(COMP_REPLICATION, 17)
#define GS_TRC_ID_WalRcvIsShutdown GS_TRC_ID_PACK(COMP_REPLICATION, 18)
#define GS_TRC_ID_ProcessRmXLog GS_TRC_ID_PACK(COMP_REPLICATION, 19)
#define GS_TRC_ID_SetWalRcvWriterPID GS_TRC_ID_PACK(COMP_REPLICATION, 20)
#define GS_TRC_ID_WalRcvWriterInProgress GS_TRC_ID_PACK(COMP_REPLICATION, 21)
#define GS_TRC_ID_walRcvCtlBlockIsEmpty GS_TRC_ID_PACK(COMP_REPLICATION, 22)
#define GS_TRC_ID_GetRepConnArray GS_TRC_ID_PACK(COMP_REPLICATION, 23)
#define GS_TRC_ID_get_sync_percent GS_TRC_ID_PACK(COMP_REPLICATION, 24)
#define GS_TRC_ID_wal_get_role_string GS_TRC_ID_PACK(COMP_REPLICATION, 25)
#define GS_TRC_ID_pg_stat_get_stream_replications GS_TRC_ID_PACK(COMP_REPLICATION, 26)
#define GS_TRC_ID_DataReceiverMain GS_TRC_ID_PACK(COMP_REPLICATION, 27)
#define GS_TRC_ID_DataRcvShmemSize GS_TRC_ID_PACK(COMP_REPLICATION, 28)
#define GS_TRC_ID_DataRcvShmemInit GS_TRC_ID_PACK(COMP_REPLICATION, 29)
#define GS_TRC_ID_DataRcvInProgress GS_TRC_ID_PACK(COMP_REPLICATION, 30)
#define GS_TRC_ID_ShutdownDataRcv GS_TRC_ID_PACK(COMP_REPLICATION, 31)
#define GS_TRC_ID_StartupDataStreaming GS_TRC_ID_PACK(COMP_REPLICATION, 32)
#define GS_TRC_ID_RequestDataStreaming GS_TRC_ID_PACK(COMP_REPLICATION, 33)
#define GS_TRC_ID_InitDummyDataNum GS_TRC_ID_PACK(COMP_REPLICATION, 34)
#define GS_TRC_ID_WakeupDataRcvWriter GS_TRC_ID_PACK(COMP_REPLICATION, 35)
#define GS_TRC_ID_DataRcvWriterInProgress GS_TRC_ID_PACK(COMP_REPLICATION, 36)
#define GS_TRC_ID_DataRcvDataCleanup GS_TRC_ID_PACK(COMP_REPLICATION, 37)
#define GS_TRC_ID_DataRcvWriterMain GS_TRC_ID_PACK(COMP_REPLICATION, 38)
#define GS_TRC_ID_DataRcvWrite GS_TRC_ID_PACK(COMP_REPLICATION, 39)
#define GS_TRC_ID_SetDataRcvDummyStandbySyncPercent GS_TRC_ID_PACK(COMP_REPLICATION, 40)
#define GS_TRC_ID_GetDataRcvDummyStandbySyncPercent GS_TRC_ID_PACK(COMP_REPLICATION, 41)
#define GS_TRC_ID_SetDataRcvWriterPID GS_TRC_ID_PACK(COMP_REPLICATION, 42)
#define GS_TRC_ID_ProcessDataRcvInterrupts GS_TRC_ID_PACK(COMP_REPLICATION, 43)
#define GS_TRC_ID_CloseDataFile GS_TRC_ID_PACK(COMP_REPLICATION, 44)
#define GS_TRC_ID_WalSenderMain GS_TRC_ID_PACK(COMP_REPLICATION, 45)
#define GS_TRC_ID_WalSndSignals GS_TRC_ID_PACK(COMP_REPLICATION, 46)
#define GS_TRC_ID_WalSndShmemSize GS_TRC_ID_PACK(COMP_REPLICATION, 47)
#define GS_TRC_ID_WalSndShmemInit GS_TRC_ID_PACK(COMP_REPLICATION, 48)
#define GS_TRC_ID_WalSndWakeup GS_TRC_ID_PACK(COMP_REPLICATION, 49)
#define GS_TRC_ID_WalSndRqstFileReload GS_TRC_ID_PACK(COMP_REPLICATION, 50)
#define GS_TRC_ID_WalSndInProgress GS_TRC_ID_PACK(COMP_REPLICATION, 51)
#define GS_TRC_ID_StandbyOrSecondaryIsAlive GS_TRC_ID_PACK(COMP_REPLICATION, 52)

#endif
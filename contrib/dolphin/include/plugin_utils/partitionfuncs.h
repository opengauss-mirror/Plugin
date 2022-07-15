/* -------------------------------------------------------------------------
 *
 * partitionfuncs.h
 *	  Definition of Functions about partition table.
 *
 * contrib/dolphin/include/plugin_utils/partitionfuncs.h
 *
 * -------------------------------------------------------------------------
 */
#ifndef PLUGIN_PARTITIONFUNCS_H
#define PLUGIN_PARTITIONFUNCS_H

#include "postgres.h"
#include "access/heapam.h"
#include "access/tableam.h"
#include "catalog/pg_type.h"
#include "catalog/pg_database.h"
#include "utils/builtins.h"
#include "plugin_postgres.h"
#include "commands/cluster.h"
#include "catalog/pg_partition_fn.h"
#include "plugin_parser/parse_utilcmd.h"

extern void ExecRemovePartition(Oid relid, char* tableName);
extern void ExecRebuildPartition(List* partList, Relation rel);
void dropPartitionTableInfo(Relation relation);
void drop_partition_info(Relation rel);
void updatePGClass(Oid relid, Oid tempTableOid, bool isIndex);

#endif /* PARTITIONFUNCS_H */

/*
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * Plugin is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 */

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

#ifdef DOLPHIN
extern void ExecRemovePartition(Oid relid, char* tableName);
extern void ExecRebuildPartition(List* partList, Relation rel);

extern Datum RemovePartitioningExt(const char* tabname);
extern Datum RebuildPartitionExt(ArrayType* args_array);
extern Datum AnalyzePartitionExt(ArrayType* args_array, const char* catalog, const char* schema);

void dropPartitionTableInfo(Relation relation);
void drop_partition_info(Relation rel);
void updatePGClass(Oid relid, Oid tempTableOid, bool isIndex);
#endif

#endif /* PARTITIONFUNCS_H */

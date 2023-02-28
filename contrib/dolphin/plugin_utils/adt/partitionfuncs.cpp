#include "plugin_utils/partitionfuncs.h"
#ifdef DOLPHIN
PG_FUNCTION_INFO_V1_PUBLIC(RemovePartitioning);
extern "C" DLL_PUBLIC Datum RemovePartitioning(PG_FUNCTION_ARGS);

PG_FUNCTION_INFO_V1_PUBLIC(RebuildPartition);
extern "C" DLL_PUBLIC Datum RebuildPartition(PG_FUNCTION_ARGS);

PG_FUNCTION_INFO_V1_PUBLIC(AnalyzePartitions);
extern "C" DLL_PUBLIC Datum AnalyzePartitions(PG_FUNCTION_ARGS);

void dropPartitionTableInfo(Relation relation)
{
    List* partCacheList = NIL;
    List* partitionOidList = NIL;
    List* toastOidList = NIL;
    List* cuDescOidList = NIL;
    List* deltaOidList = NIL;
    Oid partOid = InvalidOid;
    Oid toastOid = InvalidOid;
    Oid cuDesc = InvalidOid;
    Oid delta = InvalidOid;
    Partition partTable = NULL;
    HeapTuple partTuple = NULL;
    HeapTuple tableTuple = NULL;
    Form_pg_partition partForm = NULL;
    ListCell* cell = NULL;

    partCacheList = searchPgPartitionByParentId(PART_OBJ_TYPE_TABLE_PARTITION, RelationGetRelid(relation));
    tableTuple = searchPgPartitionByParentIdCopy(PART_OBJ_TYPE_PARTED_TABLE, RelationGetRelid(relation));

    /*first form partition list and partitioned table list
     *the opened partition must already LOCKED before realy dropped.
     */
    foreach (cell, partCacheList) {
        partTuple = (HeapTuple)lfirst(cell);

        if (PointerIsValid(partTuple)) {
            partOid = HeapTupleGetOid(partTuple);
            partitionOidList = lappend_oid(partitionOidList, partOid);

            partForm = (Form_pg_partition)GETSTRUCT(partTuple);
            toastOid = partForm->reltoastrelid;
            cuDesc = partForm->relcudescrelid;
            delta = partForm->reldeltarelid;

            if (OidIsValid(toastOid)) {
                toastOidList = lappend_oid(toastOidList, toastOid);
            }

            if (OidIsValid(cuDesc)) {
                cuDescOidList = lappend_oid(cuDescOidList, cuDesc);
            }

            if (OidIsValid(delta)) {
                deltaOidList = lappend_oid(deltaOidList, delta);
            }
        }
    }

    /*
     * drop table partition's toast table
     */
    if (PointerIsValid(toastOidList)) {
        heapDropPartitionToastList(toastOidList);
        list_free(toastOidList);
    }

    if (PointerIsValid(cuDescOidList)) {
        heapDropPartitionToastList(cuDescOidList);
        list_free(cuDescOidList);
    }

    if (PointerIsValid(deltaOidList)) {
        heapDropPartitionToastList(deltaOidList);
        list_free(deltaOidList);
    }

    /*
     * drop table partition
     */
    if (PointerIsValid(partitionOidList)) {
        heapDropPartitionList(relation, partitionOidList);
        list_free(partitionOidList);
    }

    /*
     * drop partitioned table
     */
    if (PointerIsValid(tableTuple)) {
        partOid = HeapTupleGetOid(tableTuple);
        partTable = partitionOpen(relation, partOid, AccessExclusiveLock);
        heapDropPartition(relation, partTable);
        heap_freetuple(tableTuple);
    } else {
        ereport(ERROR,
            (errcode(ERRCODE_CACHE_LOOKUP_FAILED),
                errmsg("cache lookup failed for partitioned table %u in pg_partition", RelationGetRelid(relation))));
    }

    freePartList(partCacheList);
}

void drop_partition_info(Relation rel)
{
    List* allOidList = NULL;
    List* partOidList = relationGetPartitionOidList(rel);
    if (RelationIsSubPartitioned(rel)) {
        List* subpartOidList = RelationGetSubPartitionOidList(rel);
        allOidList = list_concat(allOidList, subpartOidList); 
    }
    allOidList = list_concat(allOidList, partOidList);
    ListCell* lc = NULL;
    foreach (lc, allOidList) {
        dropIndexForPartition(lfirst_oid(lc));
    }
    if (RelationIsValuePartitioned(rel)) {
        HeapTuple partTuple = searchPgPartitionByParentIdCopy(PART_OBJ_TYPE_PARTED_TABLE, RelationGetRelid(rel));
        if (partTuple) {
            Relation pg_partitionRel = heap_open(PartitionRelationId, RowExclusiveLock);
            simple_heap_delete(pg_partitionRel, &partTuple->t_self);

            heap_freetuple(partTuple);
            heap_close(pg_partitionRel, RowExclusiveLock);
        } else
            ereport(ERROR,
                (errcode(ERRCODE_DATA_EXCEPTION),
                    errmsg(
                        "Catalog table pg_partition may get trashed on table %s as it is not consitant with pg_class",
                        RelationGetRelationName(rel))));
    }
    if (RELATION_IS_PARTITIONED(rel)) {
        dropPartitionTableInfo(rel);
    }
}

void updatePGClass(Oid relid, Oid tempTableOid, bool isIndex) {
    Relation sysRel = heap_open(RelationRelationId, RowExclusiveLock);
    CatalogIndexState indstate = NULL;
    Form_pg_class classForm = NULL;
    Form_pg_class tempClassForm = NULL;
    HeapTuple tuple = NULL;
    HeapTuple tempTuple = NULL;
    tuple = SearchSysCacheCopy1(RELOID, ObjectIdGetDatum(relid));
    if (!HeapTupleIsValid(tuple))
        ereport(ERROR,
            (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                errmsg("could not find tuple for relation %u", relid)));
    classForm = (Form_pg_class)GETSTRUCT(tuple);
    classForm->parttype = 'n';
    if (isIndex) {
        classForm->relkind = 'i';
    } else {
        tempTuple = SearchSysCacheCopy1(RELOID, ObjectIdGetDatum(tempTableOid));
        tempClassForm = (Form_pg_class)GETSTRUCT(tempTuple);
        Oid tempOid = classForm->relfilenode;
        classForm->relfilenode = tempClassForm->relfilenode;
        tempClassForm->relfilenode = tempOid;
    }
    indstate = CatalogOpenIndexes(sysRel);
    simple_heap_update(sysRel, &tuple->t_self, tuple);
    CatalogIndexInsert(indstate, tuple);
    if (!isIndex && tempTuple) {
        simple_heap_update(sysRel, &tempTuple->t_self, tempTuple);
        CatalogIndexInsert(indstate, tempTuple);
    }
    CatalogCloseIndexes(indstate);
    tableam_tops_free_tuple(tuple);
    tableam_tops_free_tuple(tempTuple);
    heap_close(sysRel, RowExclusiveLock);
}

Datum RemovePartitioning(PG_FUNCTION_ARGS)
{
    char *tableName = text_to_cstring(PG_GETARG_TEXT_PP(0));
    Oid relid = RelnameGetRelid(tableName);
    if (!OidIsValid(relid))
        ereport(ERROR, (errcode(ERRCODE_PARTITION_ERROR), errmsg("The table %s can't be found", tableName)));
    ExecRemovePartition(relid, tableName);
    PG_RETURN_TEXT_P(cstring_to_text(tableName));
}

Datum RebuildPartition(PG_FUNCTION_ARGS)
{
    /* get args */
    ArrayType* args = PG_GETARG_ARRAYTYPE_P(0);
	Datum* argText = NULL;
    bool* argmnulls = NULL;
    int argnum = 0;
    if (array_contains_nulls(args)) {
        ereport(ERROR,
                (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                 errmsg("cannot call function with null arg elements")));
    }
    deconstruct_array(args, TEXTOID, -1, false, 'i', &argText, &argmnulls, &argnum);
    if (argnum <= 0)
        PG_RETURN_NULL();
    char *tableName = TextDatumGetCString(argText[0]);
    Oid relid = RelnameGetRelid(tableName);
    if (!OidIsValid(relid))
        ereport(ERROR, (errcode(ERRCODE_PARTITION_ERROR), errmsg("The table %s can't be found", tableName)));
    Relation rel = relation_open(relid, AccessShareLock);
    if (!RelationIsPartitioned(rel))
        ereport(ERROR, (errcode(ERRCODE_PARTITION_ERROR), errmsg("The table %s is not partition table", tableName)));
    if (rel->rd_rel->relkind != RELKIND_RELATION)
        ereport(ERROR, (errcode(ERRCODE_PARTITION_ERROR), errmsg("The table %s is not an ordinary table", tableName)));

    ListCell* cell = NULL;
    List* partOidList = NIL;
    List* partList = NIL;
    List* globalIndexList = RelationGetSpecificKindIndexList(rel, true);
    if (argnum < 2) {
        if (RelationIsSubPartitioned(rel)) {
            partList = RelationGetSubPartitionList(rel, AccessExclusiveLock);
            partOidList = RelationGetSubPartitionOidList(rel);
        } else {
            partList = relationGetPartitionList(rel, AccessExclusiveLock);
            partOidList = relationGetPartitionOidList(rel);
        }
    } else {
        int count = 1;
        while (count < argnum) 
        {
            char *partName = TextDatumGetCString(argText[count]);
            char objectType = PART_OBJ_TYPE_TABLE_PARTITION;
            Oid partOid = PartitionNameGetPartitionOid(relid, partName, objectType, AccessExclusiveLock, true, false, NULL, NULL, NoLock);
            if (!OidIsValid(partOid))
                ereport(ERROR, (errcode(ERRCODE_PARTITION_ERROR), errmsg("The partition %s can't be found in table %s", partName, tableName)));
            Partition part = partitionOpen(rel, partOid, AccessExclusiveLock);
            if (RelationIsSubPartitioned(rel)) {
                Relation partRel = partitionGetRelation(rel, part);
                List* subPartListTmp = relationGetPartitionList(partRel, AccessExclusiveLock);
                partList = list_concat(partList, subPartListTmp);
                List* subPartOidList = relationGetPartitionOidList(partRel);
                partOidList = list_concat(partOidList, subPartOidList);
                releaseDummyRelation(&partRel);
                partitionClose(rel, part, AccessExclusiveLock);
            } else {
                partList = lappend(partList, part);
                partOidList = lappend_oid(partOidList, partOid);
            }
            count++;
        }
    }
    ExecRebuildPartition(partList, rel);
    foreach (cell, globalIndexList) {
        Oid indexId = lfirst_oid(cell);
        reindex_index(indexId, InvalidOid, false, NULL, false);
    }
    foreach (cell, partOidList) {
        Oid partOid = lfirst_oid(cell);
        reindexPartition(relid, partOid, REINDEX_REL_SUPPRESS_INDEX_USE, REINDEX_ALL_INDEX);
    }
    releasePartitionList(rel, &partList, AccessExclusiveLock);
    relation_close(rel, AccessShareLock);
    RelationForgetRelation(relid);
    PG_RETURN_TEXT_P(cstring_to_text(tableName));
}

extern void DoVacuumMppTable(VacuumStmt* stmt, const char* query_string, bool is_top_level, bool sent_to_remote);
extern RangeVar *makeRangeVar(char *schemaname, char *relname, int location);

List* GetSubpartitionNameList(List* subpartNameList, char* tableName, char* partName)
{
    Oid relid = RelnameGetRelid(tableName);
    Relation rel = relation_open(relid, NoLock);
    if (RelationIsSubPartitioned(rel)) {
        char objectType = PART_OBJ_TYPE_TABLE_PARTITION;
        Oid partOid = PartitionNameGetPartitionOid(relid, partName, objectType, AccessExclusiveLock, true, false, NULL, NULL, NoLock);
        if (!OidIsValid(partOid))
            ereport(ERROR, (errcode(ERRCODE_PARTITION_ERROR), errmsg("The partition %s can't be found in table %s", partName, tableName)));
        Partition part = partitionOpen(rel, partOid, AccessExclusiveLock);
        Relation partRel = partitionGetRelation(rel, part);
        List* subPartOidList = relationGetPartitionOidList(partRel);
        ListCell* cell = NULL;
        foreach (cell, subPartOidList) {
            Oid subPartOid = lfirst_oid(cell);
            char *srcSubPartName = getPartitionName(subPartOid, false);
            subpartNameList = lappend(subpartNameList, srcSubPartName);
        }
        releaseDummyRelation(&partRel);
        partitionClose(rel, part, AccessExclusiveLock);
    }
    relation_close(rel, NoLock);
    return subpartNameList;
}

Datum AnalyzePartitions(PG_FUNCTION_ARGS)
{
    /* get args */
    ArrayType* args = PG_GETARG_ARRAYTYPE_P(0);
    char *schemaname = PG_GETARG_DATUM(2) ? text_to_cstring(PG_GETARG_TEXT_PP(2)) : NULL;
    char *catalogname = PG_GETARG_DATUM(1) ? text_to_cstring(PG_GETARG_TEXT_PP(1)) : NULL;
	Datum* argText = NULL;
    bool* argmnulls = NULL;
    int argnum = 0;
    if (array_contains_nulls(args)) {
        ereport(ERROR,
                (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                 errmsg("cannot call function with null arg elements")));
    }
    deconstruct_array(args, TEXTOID, -1, false, 'i', &argText, &argmnulls, &argnum);
    if (argnum <= 0)
        PG_RETURN_NULL();
    char *tableName = TextDatumGetCString(argText[0]);
    int count = 1;
    char *partName = NULL;
    VacuumStmt *vacstmt = makeNode(VacuumStmt);
    RangeVar *rangevar = makeRangeVar(NULL, NULL, -1);
    rangevar->relname = tableName;
    rangevar->catalogname = catalogname;
    rangevar->schemaname = schemaname;
    vacstmt->type = T_VacuumStmt;
    vacstmt->options = VACOPT_ANALYZE;
    vacstmt->freeze_min_age = -1;
    vacstmt->freeze_table_age = -1;
    vacstmt->relation = rangevar;
    StringInfoData str;
    bool tmp_enable_autoanalyze = u_sess->attr.attr_sql.enable_autoanalyze;
    /* forbid auto-analyze inside vacuum/analyze */
    u_sess->attr.attr_sql.enable_autoanalyze = false;
    List* subpartNameList = NULL;
    while (count < argnum) {
        MemoryContext currentContext = CurrentMemoryContext;
        partName = TextDatumGetCString(argText[count]);
        subpartNameList = GetSubpartitionNameList(subpartNameList, tableName, partName);
        if (subpartNameList) {
            ListCell* cell = NULL;
            foreach (cell, subpartNameList) {
                char* subpartName = (char*)lfirst(cell);
                rangevar->subpartitionname = subpartName;
                initStringInfo(&str);
                appendStringInfo(&str, "ANALYZE ");
                appendStringInfo(&str, "%s SUBPARTITION (%s)", tableName, subpartName);
                DoVacuumMppTable(vacstmt, str.data, true, false);
                MemoryContextSwitchTo(currentContext);
                pfree_ext(subpartName);
                pfree_ext(str.data);
            }
            pfree_ext(partName);
            list_free_ext(subpartNameList);
        } else {
            rangevar->partitionname = partName;
            initStringInfo(&str);
            appendStringInfo(&str, "ANALYZE ");
            appendStringInfo(&str, "%s PARTITION (%s)", tableName, partName);
            DoVacuumMppTable(vacstmt, str.data, true, false);
            MemoryContextSwitchTo(currentContext);
            pfree_ext(partName);
            pfree_ext(str.data);
        }
        count++;
    }
    u_sess->attr.attr_sql.enable_autoanalyze = tmp_enable_autoanalyze;
    pfree_ext(rangevar);
    pfree_ext(vacstmt);
    PG_RETURN_TEXT_P(cstring_to_text(tableName));
}
#endif

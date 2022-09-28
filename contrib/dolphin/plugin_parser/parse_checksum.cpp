#include "access/tableam.h"
#include "postgres.h"
#include "commands/dbcommands.h"
#include "nodes/makefuncs.h"
#include "utils/builtins.h"
#include "utils/snapmgr.h"
#include "storage/lmgr.h"
#include "plugin_parser/parse_show.h"
#include "plugin_parser/parse_relation.h"

#define MAX_INT_TO_STRLEN (11)

#define PLPS_CHKSUM_ZEROVAL   "0"
#define PLPS_NULL_VAL          ""

extern uint64 pg_relation_table_size(Relation rel);

static int checksum_table_impl(Relation rel, uint32 &crc32)
{
    TableScanDesc scan;
    HeapTuple tup;
    HeapTupleHeader tupHdr;
    UHeapTuple utup;
    UHeapDiskTuple utupHdr;
    bool isUstoreRel = RelationIsUstoreFormat(rel);
    char *t_data;
    uint32_t row_crc32;
    uint64_t total_tup_count = 0;

    scan = tableam_scan_begin(rel, SnapshotNow, 0, NULL);

    if (!isUstoreRel) {
        while(HeapTupleIsValid(tup = (HeapTuple)tableam_scan_getnexttuple(scan, ForwardScanDirection))) {
            tupHdr = tup->t_data;
            t_data = (char*)tupHdr + tupHdr->t_hoff;
            row_crc32 = 0;
            COMP_CRC32C(row_crc32, t_data, tup->t_len - tupHdr->t_hoff);
            crc32 += row_crc32;
            total_tup_count++;
        }
    } else {
	while(UHeapTupleIsValid(utup = (UHeapTuple)tableam_scan_getnexttuple(scan, ForwardScanDirection))) {
    	    utupHdr = utup->disk_tuple;
    	    t_data = (char*)utupHdr + utupHdr->t_hoff;
    	    row_crc32 = 0;
    	    COMP_CRC32C(row_crc32, t_data, utup->disk_tuple_size - utupHdr->t_hoff);
    	    crc32 += row_crc32;
    	    total_tup_count++;
	}
    }

    tableam_scan_end(scan);
   
    return 0;
}

Node* makeChecksumTextAgg(RangeVar *rv)
{
    Node *argCol = NULL;
    Node *funcNode = NULL;
    Node *typCast = NULL;

    /* little differences, its the relname */
    argCol = plpsMakeColumnRef(NULL, rv->relname);
   
    /* the ::text */
    typCast = plpsMakeTypeCast(argCol, TYPE_NAME_TEXT, PLPS_LOC_UNKNOWN);
    funcNode = plpsMakeFunc(PLPS_FUNCNAME_CHECKSUM, list_make1(typCast), PLPS_LOC_UNKNOWN);

    ResTarget *rt = makeNode(ResTarget);
    rt->name = NULL;
    rt->indirection = NIL;
    rt->val = plpsMakeCoalesce(funcNode, plpsMakeStringConst(PLPS_CHKSUM_ZEROVAL));
    rt->location = PLPS_LOC_UNKNOWN;

    SelectStmt *stmt = plpsMakeSelectStmt(list_make1(rt), list_make1(rv), NULL, NIL);
    return (Node*)stmt;
}

static bool isToastRelationEmpty(Oid toastRelid)
{
    Relation toastRel;
    bool ret = FALSE;
    uint64 rel_size;

    /* lock relation first */
    LockRelationOid(toastRelid, AccessShareLock);

    do {
        if (!SearchSysCacheExists1(RELOID, ObjectIdGetDatum(toastRelid))) {
            /* should not happen for toast error exit ??? */
            break;
        }

        toastRel = RelationIdGetRelation(toastRelid);
        if (!RelationIsValid(toastRel)) {
            break;
        }

        rel_size = (uint64)pg_relation_table_size(toastRel);
        if (rel_size <= 0) {
            ret = TRUE;
        }

        relation_close(toastRel, AccessShareLock);
        return ret;
    } while(0);

    UnlockRelationOid(toastRelid, AccessShareLock);
    return ret;
}

/* only Astore table support current in Compatability B database */
List* checksums_tables(List *tableList, bool isExtendMode, bool doFastChksum)
{
    ListCell *fl = NULL;
    Node *n = NULL;
    RangeVar *rv = NULL;
    Relation rel;
    List *vl = NULL;
    List *sub_vl = NULL;
    Node *typCast = NULL;
    char *crc_str = NULL;
    char *compTablename = NULL;
    int len;
    int rc;

    foreach(fl, tableList) {
        n = (Node*)lfirst(fl);
        /* only rangevar support for checksum_tables table list */
        rv = (RangeVar*)n;
        if (rv->schemaname == NULL) {
            rv->schemaname = DatumGetCString(DirectFunctionCall1(current_schema, PointerGetDatum(NULL)));
            if (rv->schemaname == NULL) {
                /* should not happend, error ret */
                ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE), errmsg("current_schema is NULL!!!!")));
            }
        }

        len = strlen(rv->schemaname) + strlen(rv->relname) + 2; /* include . and \0 end */
        compTablename = (char*)palloc0(len);
        rc = sprintf_s(compTablename, len, "%s.%s", rv->schemaname, rv->relname);
        if (!isExtendMode) {
            sub_vl = list_make2(plpsMakeStringConst(compTablename), plpsMakeStringConst(PLPS_NULL_VAL));
            vl = lappend(vl, sub_vl);
            continue;
        }
        
        /* not filtering out not-existing tables in gram.y */
        rel = HeapOpenrvExtended(rv, AccessShareLock, true, true, NULL);

        /* for not exist tables or not ordinary table, just return NULL result */
        if (rel == NULL || RelationGetRelkind(rel) != RELKIND_RELATION) {
            sub_vl = list_make2(plpsMakeStringConst(compTablename), plpsMakeStringConst(PLPS_NULL_VAL));
            vl = lappend(vl, sub_vl);

	    if(rel != NULL) {
		heap_close(rel, AccessShareLock);
            }
            continue;
        }

        RangeTblEntry *rte = addRangeTableEntryForRelation(NULL, rel, NULL, false, false);
        ExecCheckRTPerms(list_make1(rte), TRUE);

        if (doFastChksum && OidIsValid(rel->rd_rel->reltoastrelid) && isToastRelationEmpty(rel->rd_rel->reltoastrelid)) {
            uint32 crc32 = 0;
            checksum_table_impl(rel, crc32);

            heap_close(rel, AccessShareLock);

            crc_str = (char*)palloc0(MAX_INT_TO_STRLEN);
            rc = sprintf_s(crc_str, MAX_INT_TO_STRLEN, "%u", crc32);
            securec_check_ss(rc, "", "");

            sub_vl = list_make2(plpsMakeStringConst(compTablename), plpsMakeStringConst(crc_str));
            vl = lappend(vl, sub_vl);
            continue;
        }

        heap_close(rel, AccessShareLock);

        /* for relation with toast value ==> do the result string checksum calculation */
	typCast = plpsMakeTypeCast(plpsMakeSubLink(makeChecksumTextAgg(rv)), TYPE_NAME_TEXT, PLPS_LOC_UNKNOWN);
        sub_vl = list_make2(plpsMakeStringConst(compTablename), typCast);
        vl = lappend(vl, sub_vl);        
    }

    return vl;
} 

SelectStmt* makeChecksumTableSubQuery(List *tablelist, bool isExtendMode, bool doFastChksum)
{
    List *vl = checksums_tables(tablelist, isExtendMode, doFastChksum);
    SelectStmt *stmt = plpsMakeSelectStmt(NIL, NIL, NULL, NIL);

    stmt->valuesLists = vl;
    
    return stmt;
}

SelectStmt* makeChecksumsTablesQuery(List *tablelist, bool isExtendMode, bool doFastChksum)
{
    Alias* alias = makeNode(Alias);
    ColumnRef* colRef = makeNode(ColumnRef);
    ResTarget* rt = makeNode(ResTarget);
    List* cl = NULL;

    cl = list_make2(makeString(SHOW_TABLE_COL), makeString(SHOW_CHECKSUM_COL));

    alias->aliasname = "__unnamed_subquery__";
    alias->colnames = cl;

    RangeSubselect* rsubselect = makeNode(RangeSubselect);
    rsubselect->subquery = (Node*)makeChecksumTableSubQuery(tablelist, isExtendMode, doFastChksum);
    rsubselect->alias = alias;

    colRef->fields = list_make1(makeNode(A_Star));
    colRef->location = PLPS_LOC_UNKNOWN;
    colRef->indnum = 0;

    rt->val = (Node*)colRef;

    SelectStmt *stmt = plpsMakeSelectStmt(list_make1(rt), list_make1(rsubselect), NULL, NIL);

    return stmt;
}

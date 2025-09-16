/*-------------------------------------------------------------------------
 * policy.c
 *
 * This file contains functions to create, alter and drop policies on
 * distributed tables.
 *
 * Copyright (c) Citus Data, Inc.
 *
 *-------------------------------------------------------------------------
 */
#include "postgres.h"

#include "miscadmin.h"

#include "catalog/namespace.h"
#include "commands/sec_rls_cmds.h"
#include "nodes/makefuncs.h"
#include "parser/parse_clause.h"
#include "parser/parse_relation.h"
#include "rewrite/rewriteManip.h"
#ifdef DISABLE_OG_COMMENTS
#include "rewrite/rowsecurity.h"
#else
#include "rewrite/rewriteRlsPolicy.h"
#endif
#include "utils/builtins.h"
#ifdef DISABLE_OG_COMMENTS
#include "utils/ruleutils.h"
#endif

#include "distributed/citus_ruleutils.h"
#include "distributed/commands.h"
#include "distributed/commands/utility_hook.h"
#include "distributed/coordinator_protocol.h"
#include "distributed/listutils.h"
#include "distributed/metadata_cache.h"

// @FIXME: void compile error
typedef struct RowSecurityPolicy {
    char* policy_name;     /* Name of the policy */
    char polcmd;           /* Type of command policy is for */
    ArrayType* roles;      /* Array of roles policy is for */
    bool permissive;       /* restrictive or permissive policy */
    Expr* qual;            /* Expression to filter rows */
    Expr* with_check_qual; /* Expression to limit rows allowed */
    bool hassublinks;      /* If either expression has sublinks */
} RowSecurityPolicy;

static const char* unparse_policy_command(const char aclchar);
static RowSecurityPolicy* GetPolicyByName(Oid relationId, const char* policyName);
static List* GetPolicyListForRelation(Oid relationId);
static char* CreatePolicyCommandForPolicy(Oid relationId, RowSecurityPolicy* policy);

/*
 * CreatePolicyCommands takes in a relationId, and returns the list of create policy
 * commands needed to reconstruct the policies of that table.
 */
List* CreatePolicyCommands(Oid relationId)
{
    List* commands = NIL;

    List* policyList = GetPolicyListForRelation(relationId);

    RowSecurityPolicy* policy;
    foreach_declared_ptr(policy, policyList)
    {
        char* createPolicyCommand = CreatePolicyCommandForPolicy(relationId, policy);
        commands = lappend(commands, makeTableDDLCommandString(createPolicyCommand));
    }

    return commands;
}

/*
 * GetPolicyListForRelation returns a list of RowSecurityPolicy objects identifying
 * the policies on the relation with relationId. Note that this function acquires
 * AccessShareLock on relation and does not release it in the end to make sure that
 * caller will process valid policies through the transaction.
 */
static List* GetPolicyListForRelation(Oid relationId)
{
    Relation relation = table_open(relationId, AccessShareLock);

    if (!RelationHasRlspolicy(relation->rd_id)) {
        table_close(relation, NoLock);

        return NIL;
    }

    if (relation->rd_rlsdesc == NULL) {
        /*
         * there are policies, but since RLS is not enabled they are not loaded into
         * cache, we will do so here for us to access
         */
        RelationBuildRlsPolicies(relation);
    }

    List* policyList = NIL;

    RowSecurityPolicy* policy;
    foreach_declared_ptr(policy, relation->rd_rlsdesc->rlsPolicies)
    {
        policyList = lappend(policyList, policy);
    }

    table_close(relation, NoLock);

    return policyList;
}

/*
 * CreatePolicyCommandForPolicy takes a relationId and a policy, returns
 * the CREATE POLICY command needed to reconstruct the policy identified
 * by the "policy" object on the relation with relationId.
 */
static char* CreatePolicyCommandForPolicy(Oid relationId, RowSecurityPolicy* policy)
{
    char* relationName = generate_qualified_relation_name(relationId);
    List* relationContext = deparse_context_for(relationName, relationId);

    StringInfo createPolicyCommand = makeStringInfo();

    appendStringInfo(createPolicyCommand, "CREATE POLICY %s ON %s FOR %s",
                     quote_identifier(policy->policy_name), relationName,
                     unparse_policy_command(policy->polcmd));

    appendStringInfoString(createPolicyCommand, " TO ");

    /*
     * iterate over all roles and append them to the ddl command with commas
     * separating the role names
     */
    Oid* roles = (Oid*)ARR_DATA_PTR(policy->roles);
    for (int roleIndex = 0; roleIndex < ARR_DIMS(policy->roles)[0]; roleIndex++) {
        const char* roleName;

        if (roleIndex > 0) {
            appendStringInfoString(createPolicyCommand, ", ");
        }

        if (roles[roleIndex] == ACL_ID_PUBLIC) {
            roleName = "PUBLIC";
        } else {
            roleName = quote_identifier(GetUserNameFromId(roles[roleIndex]));
        }

        appendStringInfoString(createPolicyCommand, roleName);
    }

    if (policy->qual) {
        char* qualString =
            deparse_expression((Node*)(policy->qual), relationContext, false, false);
        appendStringInfo(createPolicyCommand, " USING (%s)", qualString);
    }

    if (policy->with_check_qual) {
        char* withCheckQualString = deparse_expression((Node*)(policy->with_check_qual),
                                                       relationContext, false, false);
        appendStringInfo(createPolicyCommand, " WITH CHECK (%s)", withCheckQualString);
    }

    return createPolicyCommand->data;
}

/*
 * unparse_policy_command takes the type of a policy command and converts it to its full
 * command string. This function is the exact inverse of parse_policy_command that is in
 * postgres.
 */
static const char* unparse_policy_command(const char aclchar)
{
    switch (aclchar) {
        case '*': {
            return "ALL";
        }

        case ACL_SELECT_CHR: {
            return "SELECT";
        }

        case ACL_INSERT_CHR: {
            return "INSERT";
        }

        case ACL_UPDATE_CHR: {
            return "UPDATE";
        }

        case ACL_DELETE_CHR: {
            return "DELETE";
        }

        default: {
            elog(ERROR, "unrecognized aclchar: %d", aclchar);
            return NULL;
        }
    }
}

/*
 * PostprocessCreatePolicyStmt determines when a CREATE POLICY statement involves
 * a distributed table. If so, it creates DDLJobs to encapsulate information
 * needed during the worker node portion of DDL execution before returning the
 * DDLJobs in a List. If no distributed table is involved, this returns NIL.
 */
List* PostprocessCreatePolicyStmt(Node* node, const char* queryString)
{
    CreatePolicyStmt* stmt = castNode(CreatePolicyStmt, node);

    /* load relation information */
    RangeVar* relvar = stmt->relation;
    Oid relationId = RangeVarGetRelid(relvar, NoLock, false);
    if (!IsCitusTable(relationId)) {
        return NIL;
    }

    Relation relation = table_open(relationId, AccessShareLock);

    ParseState* qual_pstate = make_parsestate(NULL);
    AddRangeTableEntryToQueryCompat(qual_pstate, relation);
    Node* qual =
        transformWhereClause(qual_pstate, static_cast<Node*>(copyObject(stmt->usingQual)),
                             EXPR_KIND_POLICY, "POLICY");
    if (qual) {
        ErrorIfUnsupportedPolicyExpr(qual);
    }

    ParseState* with_check_pstate = make_parsestate(NULL);
    AddRangeTableEntryToQueryCompat(with_check_pstate, relation);
    Node* with_check_qual =
        transformWhereClause(with_check_pstate, NULL, EXPR_KIND_POLICY, "POLICY");
    if (with_check_qual) {
        ErrorIfUnsupportedPolicyExpr(with_check_qual);
    }

    RowSecurityPolicy* policy = GetPolicyByName(relationId, stmt->policyName);

    if (policy == NULL) {
        /*
         * As this function is executed after standard process utility created the
         * policy, we should be able to find & deparse the policy with policy_name.
         * But to be more safe, error out here.
         */
        ereport(ERROR, (errmsg("cannot create policy, policy does not exist.")));
    }

    EnsureCoordinator();

    char* ddlCommand = CreatePolicyCommandForPolicy(relationId, policy);

    /*
     * create the DDLJob that needs to be executed both on the local relation and all its
     * placements.
     */
    DDLJob* ddlJob = static_cast<DDLJob*>(palloc0(sizeof(DDLJob)));
    ObjectAddressSet(ddlJob->targetObjectAddress, RelationRelationId, relationId);
    ddlJob->metadataSyncCommand = pstrdup(ddlCommand);
    ddlJob->taskList = DDLTaskList(relationId, ddlCommand);

    relation_close(relation, NoLock);

    return list_make1(ddlJob);
}

/*
 * AddRangeTableEntryToQueryCompat adds the given relation to query.
 * This method is a compatibility wrapper.
 */
void AddRangeTableEntryToQueryCompat(ParseState* parseState, Relation relation)
{
    RangeTblEntry* rte =
        addRangeTableEntryForRelation(parseState, relation, NULL, false, false);
    addRTEtoQuery(parseState, rte, false, true, true);
}

/*
 * GetPolicyByName takes a relationId and a policyName, returns RowSecurityPolicy
 * object which identifies the policy with name "policyName" on the relation
 * with relationId. If there does not exist such a policy, then this function
 * returns NULL.
 */
static RowSecurityPolicy* GetPolicyByName(Oid relationId, const char* policyName)
{
    List* policyList = GetPolicyListForRelation(relationId);

    RowSecurityPolicy* policy = NULL;
    foreach_declared_ptr(policy, policyList)
    {
        if (strncmp(policy->policy_name, policyName, NAMEDATALEN) == 0) {
            return policy;
        }
    }

    return NULL;
}

/*
 * PreprocessAlterPolicyStmt determines whether a given ALTER POLICY statement involves a
 * distributed table. If so, it creates DDLJobs to encapsulate information needed during
 * the worker node portion of DDL execution before returning the DDLJobs in a list. If no
 * distributed table is involved this returns NIL.
 */
List* PreprocessAlterPolicyStmt(Node* node, const char* queryString,
                                ProcessUtilityContext processUtilityContext)
{
    AlterPolicyStmt* stmt = castNode(AlterPolicyStmt, node);
    StringInfoData ddlString;
    ListCell* roleCell = NULL;

    /* load relation information */
    RangeVar* relvar = stmt->relation;
    Oid relOid = RangeVarGetRelid(relvar, NoLock, false);
    if (!IsCitusTable(relOid)) {
        return NIL;
    }

    initStringInfo(&ddlString);

    Relation relation = relation_open(relOid, AccessShareLock);
    char* relationName = generate_relation_name(relOid, NIL);

    appendStringInfo(&ddlString, "ALTER POLICY %s ON %s",
                     quote_identifier(stmt->policyName), relationName);

    if (stmt->roleList) {
        appendStringInfoString(&ddlString, " TO ");
        foreach (roleCell, stmt->roleList) {
            const char* roleSpec = strVal(lfirst(roleCell));

            appendStringInfoString(&ddlString, RoleSpecString(roleSpec, true));

            if (lnext(roleCell) != NULL) {
                appendStringInfoString(&ddlString, ", ");
            }
        }
    }

    List* relationContext = deparse_context_for(relationName, relOid);

    ParseState* qual_pstate = make_parsestate(NULL);
    AddRangeTableEntryToQueryCompat(qual_pstate, relation);
    Node* qual =
        transformWhereClause(qual_pstate, static_cast<Node*>(copyObject(stmt->usingQual)),
                             EXPR_KIND_POLICY, "POLICY");
    if (qual) {
        ErrorIfUnsupportedPolicyExpr(qual);

        char* qualString = deparse_expression(qual, relationContext, false, false);
        appendStringInfo(&ddlString, " USING (%s)", qualString);
    }

    ParseState* with_check_pstate = make_parsestate(NULL);
    AddRangeTableEntryToQueryCompat(with_check_pstate, relation);
    Node* with_check_qual =
        transformWhereClause(with_check_pstate, NULL, EXPR_KIND_POLICY, "POLICY");
    if (with_check_qual) {
        ErrorIfUnsupportedPolicyExpr(with_check_qual);

        char* withCheckString =
            deparse_expression(with_check_qual, relationContext, false, false);
        appendStringInfo(&ddlString, " WITH CHECK (%s)", withCheckString);
    }

    /*
     * create the DDLJob that needs to be executed both on the local relation  and all its
     * placements.
     */
    DDLJob* ddlJob = static_cast<DDLJob*>(palloc0(sizeof(DDLJob)));
    ObjectAddressSet(ddlJob->targetObjectAddress, RelationRelationId, relOid);
    ddlJob->metadataSyncCommand = pstrdup(ddlString.data);
    ddlJob->taskList = DDLTaskList(relOid, ddlString.data);

    relation_close(relation, NoLock);

    return list_make1(ddlJob);
}

/*
 * ErrorIfUnsupportedPolicy runs checks related to a Relation their Policies and errors
 * out if it is not possible to create one of the policies in a distributed environment.
 *
 * To support policies we require that:
 * - Policy expressions do not contain subqueries.
 */
void ErrorIfUnsupportedPolicy(Relation relation)
{
    ListCell* policyCell = NULL;

    if (!RelationHasRlspolicy(relation->rd_id)) {
        return;
    }

    /*
     * even if a relation has policies they might not be loaded on the Relation yet. This
     * happens if policies are on a Relation without Row Level Security enabled. We need
     * to make sure the policies installed are valid for distribution if RLS gets enabled
     * after the table has been distributed. Therefore we force a build of the policies on
     * the cached Relation
     */
    if (relation->rd_rlsdesc == NULL) {
        RelationBuildRlsPolicies(relation);
    }

    foreach (policyCell, relation->rd_rlsdesc->rlsPolicies) {
        RowSecurityPolicy* policy = (RowSecurityPolicy*)lfirst(policyCell);

        ErrorIfUnsupportedPolicyExpr((Node*)policy->qual);
        ErrorIfUnsupportedPolicyExpr((Node*)policy->with_check_qual);
    }
}

/*
 * ErrorIfUnsupportedPolicyExpr tests if the provided expression for a policy is
 * supported on a distributed table.
 */
void ErrorIfUnsupportedPolicyExpr(Node* expr)
{
    /*
     * We do not allow any sublink to prevent expressions with subqueries to be used as an
     * expression in policies on distributed tables.
     */
    if (checkExprHasSubLink(expr)) {
        ereport(ERROR,
                (errcode(ERRCODE_FEATURE_NOT_SUPPORTED), errmsg("cannot create policy"),
                 errdetail("Subqueries are not supported in policies on distributed "
                           "tables")));
    }
}

/*
 * PreprocessDropPolicyStmt determines whether a given DROP POLICY statement involves a
 * distributed table. If so it creates DDLJobs to encapsulate information needed during
 * the worker node portion of DDL execution before returning the DDLJobs in a List. If no
 * distributed table is involved this returns NIL.
 */
List* PreprocessDropPolicyStmt(Node* node, const char* queryString,
                               ProcessUtilityContext processUtilityContext)
{
    DropStmt* stmt = castNode(DropStmt, node);
    List* ddlJobs = NIL;
    ListCell* dropObjectCell = NULL;

    Assert(stmt->removeType == OBJECT_RLSPOLICY);

    foreach (dropObjectCell, stmt->objects) {
        List* names = (List*)lfirst(dropObjectCell);

        /*
         * the last element in the list of names is the name of the policy. The ones
         * before are describing the relation. By removing the last item from the list we
         * can use makeRangeVarFromNameList to get to the relation. As list_truncate
         * changes the list in place we make a copy before.
         */
        names = list_copy(names);
        names = list_truncate(names, list_length(names) - 1);
        RangeVar* relation = makeRangeVarFromNameList(names);

        Oid relOid = RangeVarGetRelid(relation, NoLock, false);
        if (!IsCitusTable(relOid)) {
            continue;
        }

        DDLJob* ddlJob = static_cast<DDLJob*>(palloc0(sizeof(DDLJob)));
        ObjectAddressSet(ddlJob->targetObjectAddress, RelationRelationId, relOid);
        ddlJob->metadataSyncCommand = queryString;
        ddlJob->taskList = DDLTaskList(relOid, queryString);

        ddlJobs = lappend(ddlJobs, ddlJob);
    }

    return ddlJobs;
}

/*
 * IsPolicyRenameStmt returns wherher the passed-in RenameStmt is one of the following
 * forms:
 *
 *   - ALTER POLICY ... ON ... RENAME TO ...
 */
bool IsPolicyRenameStmt(RenameStmt* stmt)
{
    return stmt->renameType == OBJECT_RLSPOLICY;
}

/*
 * CreatePolicyEventExtendNames extends relation names in the given CreatePolicyStmt tree.
 * This function has side effects on the tree as the names are replaced inplace.
 */
void CreatePolicyEventExtendNames(CreatePolicyStmt* stmt, const char* schemaName,
                                  uint64 shardId)
{
    RangeVar* relation = stmt->relation;
    char** relationName = &(relation->relname);
    char** relationSchemaName = &(relation->schemaname);

    /* prefix with schema name if it is not added already */
    SetSchemaNameIfNotExist(relationSchemaName, schemaName);

    AppendShardIdToName(relationName, shardId);
}

/*
 * AlterPolicyEventExtendNames extends relation names in the given AlterPolicyStatement
 * tree. This function has side effects on the tree as the names are replaced inplace.
 */
void AlterPolicyEventExtendNames(AlterPolicyStmt* stmt, const char* schemaName,
                                 uint64 shardId)
{
    RangeVar* relation = stmt->relation;
    char** relationName = &(relation->relname);
    char** relationSchemaName = &(relation->schemaname);

    /* prefix with schema name if it is not added already */
    SetSchemaNameIfNotExist(relationSchemaName, schemaName);

    AppendShardIdToName(relationName, shardId);
}

/*
 * RenamePolicyEventExtendNames extends relation names in the given RenameStmt tree. This
 * function has side effects on the tree as the names are replaced inline.
 */
void RenamePolicyEventExtendNames(RenameStmt* stmt, const char* schemaName,
                                  uint64 shardId)
{
    char** relationName = &(stmt->relation->relname);
    char** objectSchemaName = &(stmt->relation->schemaname);

    /* prefix with schema name if it is not added already */
    SetSchemaNameIfNotExist(objectSchemaName, schemaName);

    AppendShardIdToName(relationName, shardId);
}

/*
 * DropPolicyEventExtendNames extends relation names in the given DropStmt tree specific
 * to policies. This function has side effects on the tree as the names are replaced
 * inplace.
 */
void DropPolicyEventExtendNames(DropStmt* dropStmt, const char* schemaName,
                                uint64 shardId)
{
    List* relationSchemaNameValue = NULL;
    List* relationNameValue = NULL;

    uint32 dropCount = list_length(dropStmt->objects);
    if (dropCount > 1) {
        ereport(ERROR, (errmsg("cannot extend name for multiple drop objects")));
    }

    List* relationNameList = (List*)linitial(dropStmt->objects);
    int relationNameListLength = list_length(relationNameList);

    switch (relationNameListLength) {
        case 2: {
            relationNameValue = static_cast<List*>(linitial(relationNameList));
            break;
        }

        case 3: {
            relationSchemaNameValue = static_cast<List*>(linitial(relationNameList));
            relationNameValue = static_cast<List*>(lsecond(relationNameList));
            break;
        }

        default: {
            ereport(ERROR, (errcode(ERRCODE_SYNTAX_ERROR),
                            errmsg("improper policy name: \"%s\"",
                                   NameListToString(relationNameList))));
            break;
        }
    }

    /* prefix with schema name if it is not added already */
    if (relationSchemaNameValue == NULL) {
        relationNameList = lcons(makeString(pstrdup(schemaName)), relationNameList);
    }

    char** relationName = &(strVal(relationNameValue));
    AppendShardIdToName(relationName, shardId);
}

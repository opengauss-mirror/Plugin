CREATE FUNCTION pg_catalog.show_object_grants(IN role_name TEXT,
                                              OUT relname NAME,
                                              OUT grantee OID,
                                              OUT grantor OID,
                                              OUT privs INT4,
                                              OUT grant_type TEXT,
                                              OUT grant_sql TEXT)
    RETURNS SETOF RECORD AS '$libdir/dolphin','ShowObjectGrants' 
 LANGUAGE C;

CREATE FUNCTION pg_catalog.show_any_privileges(IN role_name TEXT,
                                               OUT roleid OID,
                                               OUT privilege_type TEXT,
                                               OUT admin_option BOOL,
                                               OUT grant_sql TEXT)
    RETURNS SETOF RECORD AS '$libdir/dolphin','ShowAnyPrivileges' 
 LANGUAGE C;

CREATE FUNCTION pg_catalog.show_role_privilege(IN role_name TEXT,
                                               OUT role_name NAME,
                                               OUT grant_sql TEXT)
    RETURNS SETOF RECORD AS '$libdir/dolphin','ShowRolePrivilege' 
 LANGUAGE C;

CREATE FUNCTION pg_catalog.show_function_status(IN functionOrProcedure CHARACTER,
                                                OUT "Db" NAME,
                                                OUT "Name" NAME,
                                                OUT "Type" TEXT,
                                                OUT "Definer" NAME,
                                                OUT "Modified" tIMESTAMP WITH TIME ZONE,
                                                OUT "Created" tIMESTAMP WITH TIME ZONE,
                                                OUT "Security_type" TEXT,
                                                OUT "Comment" TEXT,
                                                OUT "character_set_client" TEXT,
                                                OUT "collation_connection" TEXT,
                                                OUT "Database Collation" NAME
)
    RETURNS SETOF RECORD AS '$libdir/dolphin','ShowFunctionStatus' 
 LANGUAGE C;

CREATE FUNCTION pg_catalog.show_triggers(IN schema_name NAME,
                                         OUT "Trigger" VARCHAR,
                                         OUT "Event" VARCHAR,
                                         OUT "Table" VARCHAR,
                                         OUT "Statement" VARCHAR,
                                         OUT "Timing" VARCHAR,
                                         OUT "Created" tIMESTAMP WITH TIME ZONE,
                                         OUT "sql_mode" VARCHAR,
                                         OUT "Definer" NAME,
                                         OUT "character_set_client" VARCHAR,
                                         OUT "collation_connection" VARCHAR,
                                         OUT "Database Collation" NAME
) RETURNS SETOF RECORD AS '$libdir/dolphin','ShowTriggers' 
 LANGUAGE C;

CREATE FUNCTION pg_catalog.show_character_set(OUT "charset" NAME,
                                         OUT "description" TEXT,
                                         OUT "default collation" TEXT,
                                         OUT "maxlen" INT4,
                                         OUT "server" BOOL
) RETURNS SETOF RECORD AS '$libdir/dolphin','ShowCharset' 
 LANGUAGE C;

CREATE FUNCTION pg_catalog.show_collation(OUT "collation" NAME,
                                         OUT "charset" NAME,
                                         OUT "id" OID,
                                         OUT "default" TEXT,
                                         OUT "compiled" TEXT,
                                         OUT "sortlen" INT4
) RETURNS SETOF RECORD AS '$libdir/dolphin','ShowCollation' 
 LANGUAGE C;
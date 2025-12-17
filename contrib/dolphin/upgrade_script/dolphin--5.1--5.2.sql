DROP CAST IF EXISTS (varchar AS bytea);
DROP FUNCTION IF EXISTS pg_catalog.varchar_bytea(varchar);
CREATE FUNCTION pg_catalog.varchar_bytea(varchar) RETURNS bytea LANGUAGE INTERNAL IMMUTABLE STRICT AS 'texttoraw';
CREATE CAST (varchar AS bytea) WITH FUNCTION pg_catalog.varchar_bytea(varchar) AS ASSIGNMENT;


CREATE OR REPLACE VIEW performance_schema.threads AS
SELECT
    CAST(pid AS bigint) AS THREAD_ID,
    CAST(application_name AS VARCHAR(128)) AS NAME,
    CAST('BACKGROUND' AS varchar(10)) AS TYPE,
    CAST(pid AS bigint) AS PROCESSLIST_ID,
    CAST(usename AS varchar(32)) AS PROCESSLIST_USER,
    CAST(client_addr::TEXT || ':' || client_port AS varchar(255)) AS PROCESSLIST_HOST,
    CAST(datname AS varchar(64)) AS PROCESSLIST_DB,
    CAST(NULL AS varchar(16)) AS PROCESSLIST_COMMAND,
    CAST(EXTRACT(EPOCH FROM (now() - backend_start)) AS bigint) AS PROCESSLIST_TIME,
    CAST(state AS varchar(64)) AS PROCESSLIST_STATE,
    CAST(query AS text) AS PROCESSLIST_INFO,
    CAST(NULL AS bigint) AS PARENT_THREAD_ID,
    CAST(NULL AS varchar(64)) AS ROLE,
    CAST('YES' AS varchar(3)) AS INSTRUMENTED,
    CAST('YES' AS varchar(3)) AS HISTORY,
    CAST(NULL AS varchar(16)) AS CONNECTION_TYPE,
    CAST(pid AS bigint) AS THREAD_OS_ID,
    CAST(resource_pool AS varchar(64)) AS RESOURCE_GROUP,
    CAST('PRIMARY' AS varchar(16)) AS EXECUTION_ENGINE,
    CAST(NULL as bigint) AS CONTROLLED_MEMORY,
    CAST(NULL as bigint) AS MAX_CONTROLLED_MEMORY,
    CAST(NULL as bigint) AS TOTAL_MEMORY,
    CAST(NULL as bigint) AS MAX_TOTAL_MEMORY,
    CAST('NO' as varchar(3)) AS TELEMETRY_ACTIVE
FROM
    pg_stat_activity
ORDER BY
    pid;

GRANT SELECT ON performance_schema.threads TO PUBLIC;

do $$
begin
update pg_proc left join pg_depend on oid = objid set proconfig = '{dolphin.sql_mode=}' where refobjid = (select oid from pg_extension where extname = 'dolphin') and classid = 1255;
end
$$
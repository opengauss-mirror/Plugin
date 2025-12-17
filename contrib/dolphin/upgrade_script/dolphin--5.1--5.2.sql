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

CREATE OR REPLACE FUNCTION pg_catalog.db_b_format(number, name) RETURNS text LANGUAGE SQL IMMUTABLE STRICT as 'select db_b_format($1::"any", $2::bigint)';

CREATE OR REPLACE FUNCTION pg_catalog.db_b_format(unknown, unknown) RETURNS text LANGUAGE C IMMUTABLE as '$libdir/dolphin', 'db_b_format_cstring';

CREATE OR REPLACE FUNCTION pg_catalog.yearweek(timestamp without time zone, int8) RETURNS int8 LANGUAGE C STABLE CALLED ON NULL INPUT as '$libdir/dolphin', 'yearweek_timestamp';
CREATE OR REPLACE FUNCTION pg_catalog.yearweek(timestamp with time zone, int8) RETURNS int8 LANGUAGE C STABLE CALLED ON NULL INPUT as '$libdir/dolphin', 'yearweek_timestamptz';
CREATE OR REPLACE FUNCTION pg_catalog.yearweek(timestamp without time zone) RETURNS int8 AS $$ SELECT pg_catalog.yearweek($1, null) $$ LANGUAGE SQL;
CREATE OR REPLACE FUNCTION pg_catalog.yearweek(timestamp with time zone) RETURNS int8 AS $$ SELECT pg_catalog.yearweek($1, null) $$ LANGUAGE SQL;

create or replace function pg_catalog.extractvalue(text,text) RETURNS text LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'extractvalue';
create or replace function pg_catalog.updatexml(text,text,text) RETURNS text LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'updatexml';

CREATE OR REPLACE FUNCTION pg_catalog.boolean_tinyblob_eq(boolean, tinyblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::float8 = $2::float8)';
CREATE OR REPLACE FUNCTION pg_catalog.tinyblob_boolean_eq(tinyblob, boolean) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::float8 = $2::float8)';
CREATE OR REPLACE FUNCTION pg_catalog.boolean_tinyblob_ne(boolean, tinyblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::float8 != $2::float8)';
CREATE OR REPLACE FUNCTION pg_catalog.tinyblob_boolean_ne(tinyblob, boolean) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::float8 != $2::float8)';
CREATE OR REPLACE FUNCTION pg_catalog.boolean_tinyblob_gt(boolean, tinyblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::float8 > $2::float8)';
CREATE OR REPLACE FUNCTION pg_catalog.tinyblob_boolean_gt(tinyblob, boolean) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::float8 > $2::float8)';
CREATE OR REPLACE FUNCTION pg_catalog.boolean_tinyblob_lt(boolean, tinyblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::float8 < $2::float8)';
CREATE OR REPLACE FUNCTION pg_catalog.tinyblob_boolean_lt(tinyblob, boolean) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::float8 < $2::float8)';
CREATE OR REPLACE FUNCTION pg_catalog.boolean_tinyblob_ge(boolean, tinyblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::float8 >= $2::float8)';
CREATE OR REPLACE FUNCTION pg_catalog.tinyblob_boolean_ge(tinyblob, boolean) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::float8 >= $2::float8)';
CREATE OR REPLACE FUNCTION pg_catalog.boolean_tinyblob_le(boolean, tinyblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::float8 <= $2::float8)';
CREATE OR REPLACE FUNCTION pg_catalog.tinyblob_boolean_le(tinyblob, boolean) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::float8 <= $2::float8)';

CREATE OR REPLACE FUNCTION pg_catalog.boolean_mediumblob_eq(boolean, mediumblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::float8 = $2::float8)';
CREATE OR REPLACE FUNCTION pg_catalog.mediumblob_boolean_eq(mediumblob, boolean) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::float8 = $2::float8)';
CREATE OR REPLACE FUNCTION pg_catalog.boolean_mediumblob_ne(boolean, mediumblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::float8 != $2::float8)';
CREATE OR REPLACE FUNCTION pg_catalog.mediumblob_boolean_ne(mediumblob, boolean) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::float8 != $2::float8)';
CREATE OR REPLACE FUNCTION pg_catalog.boolean_mediumblob_gt(boolean, mediumblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::float8 > $2::float8)';
CREATE OR REPLACE FUNCTION pg_catalog.mediumblob_boolean_gt(mediumblob, boolean) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::float8 > $2::float8)';
CREATE OR REPLACE FUNCTION pg_catalog.boolean_mediumblob_lt(boolean, mediumblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::float8 < $2::float8)';
CREATE OR REPLACE FUNCTION pg_catalog.mediumblob_boolean_lt(mediumblob, boolean) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::float8 < $2::float8)';
CREATE OR REPLACE FUNCTION pg_catalog.boolean_mediumblob_ge(boolean, mediumblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::float8 >= $2::float8)';
CREATE OR REPLACE FUNCTION pg_catalog.mediumblob_boolean_ge(mediumblob, boolean) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::float8 >= $2::float8)';
CREATE OR REPLACE FUNCTION pg_catalog.boolean_mediumblob_le(boolean, mediumblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::float8 <= $2::float8)';
CREATE OR REPLACE FUNCTION pg_catalog.mediumblob_boolean_le(mediumblob, boolean) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::float8 <= $2::float8)';

CREATE OR REPLACE FUNCTION pg_catalog.boolean_blob_eq(boolean, blob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::float8 = $2::float8)';
CREATE OR REPLACE FUNCTION pg_catalog.blob_boolean_eq(blob, boolean) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::float8 = $2::float8)';
CREATE OR REPLACE FUNCTION pg_catalog.boolean_blob_ne(boolean, blob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::float8 != $2::float8)';
CREATE OR REPLACE FUNCTION pg_catalog.blob_boolean_ne(blob, boolean) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::float8 != $2::float8)';
CREATE OR REPLACE FUNCTION pg_catalog.boolean_blob_gt(boolean, blob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::float8 > $2::float8)';
CREATE OR REPLACE FUNCTION pg_catalog.blob_boolean_gt(blob, boolean) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::float8 > $2::float8)';
CREATE OR REPLACE FUNCTION pg_catalog.boolean_blob_lt(boolean, blob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::float8 < $2::float8)';
CREATE OR REPLACE FUNCTION pg_catalog.blob_boolean_lt(blob, boolean) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::float8 < $2::float8)';
CREATE OR REPLACE FUNCTION pg_catalog.boolean_blob_ge(boolean, blob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::float8 >= $2::float8)';
CREATE OR REPLACE FUNCTION pg_catalog.blob_boolean_ge(blob, boolean) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::float8 >= $2::float8)';
CREATE OR REPLACE FUNCTION pg_catalog.boolean_blob_le(boolean, blob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::float8 <= $2::float8)';
CREATE OR REPLACE FUNCTION pg_catalog.blob_boolean_le(blob, boolean) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::float8 <= $2::float8)';

CREATE OR REPLACE FUNCTION pg_catalog.boolean_longblob_eq(boolean, longblob) returns bool LANGUAGE SQL IMMUTABLE STRICT as 'select ($1::float8 = $2::float8)';
CREATE OR REPLACE FUNCTION pg_catalog.longblob_boolean_eq(longblob, boolean) returns bool LANGUAGE SQL IMMUTABLE STRICT as 'select ($1::float8 = $2::float8)';
CREATE OR REPLACE FUNCTION pg_catalog.boolean_longblob_ne(boolean, longblob) returns bool LANGUAGE SQL IMMUTABLE STRICT as 'select ($1::float8 != $2::float8)';
CREATE OR REPLACE FUNCTION pg_catalog.longblob_boolean_ne(longblob, boolean) returns bool LANGUAGE SQL IMMUTABLE STRICT as 'select ($1::float8 != $2::float8)';
CREATE OR REPLACE FUNCTION pg_catalog.boolean_longblob_gt(boolean, longblob) returns bool LANGUAGE SQL IMMUTABLE STRICT as 'select ($1::float8 > $2::float8)';
CREATE OR REPLACE FUNCTION pg_catalog.longblob_boolean_gt(longblob, boolean) returns bool LANGUAGE SQL IMMUTABLE STRICT as 'select ($1::float8 > $2::float8)';
CREATE OR REPLACE FUNCTION pg_catalog.boolean_longblob_lt(boolean, longblob) returns bool LANGUAGE SQL IMMUTABLE STRICT as 'select ($1::float8 < $2::float8)';
CREATE OR REPLACE FUNCTION pg_catalog.longblob_boolean_lt(longblob, boolean) returns bool LANGUAGE SQL IMMUTABLE STRICT as 'select ($1::float8 < $2::float8)';
CREATE OR REPLACE FUNCTION pg_catalog.boolean_longblob_ge(boolean, longblob) returns bool LANGUAGE SQL IMMUTABLE STRICT as 'select ($1::float8 >= $2::float8)';
CREATE OR REPLACE FUNCTION pg_catalog.longblob_boolean_ge(longblob, boolean) returns bool LANGUAGE SQL IMMUTABLE STRICT as 'select ($1::float8 >= $2::float8)';
CREATE OR REPLACE FUNCTION pg_catalog.boolean_longblob_le(boolean, longblob) returns bool LANGUAGE SQL IMMUTABLE STRICT as 'select ($1::float8 <= $2::float8)';
CREATE OR REPLACE FUNCTION pg_catalog.longblob_boolean_le(longblob, boolean) returns bool LANGUAGE SQL IMMUTABLE STRICT as 'select ($1::float8 <= $2::float8)';

DROP FUNCTION IF EXISTS pg_catalog.date_format (date, text);
CREATE OR REPLACE FUNCTION pg_catalog.date_format (date, text) RETURNS TEXT LANGUAGE SQL STABLE STRICT as  $$ SELECT pg_catalog.date_format($1::text, $2); $$ ;
DROP FUNCTION IF EXISTS pg_catalog.date_format (timestamp without time zone, text);
CREATE OR REPLACE FUNCTION pg_catalog.date_format (timestamp without time zone, text) RETURNS TEXT LANGUAGE SQL STABLE STRICT as  $$ SELECT pg_catalog.date_format($1::text, $2); $$ ;
DROP FUNCTION IF EXISTS pg_catalog.date_format (timestamp with time zone, text);
CREATE OR REPLACE FUNCTION pg_catalog.date_format (timestamp with time zone, text) RETURNS TEXT LANGUAGE SQL STABLE STRICT as  $$ SELECT pg_catalog.date_format($1::text, $2); $$ ;


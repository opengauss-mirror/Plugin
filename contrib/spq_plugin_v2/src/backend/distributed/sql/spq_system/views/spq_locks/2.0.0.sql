-- spq_locks combines the pg_locks views from all nodes and adds global_pid, nodeid, and
-- relation_name. The columns of spq_locks don't change based on the Postgres version,
-- however the pg_locks's columns do. Postgres 14 added one more column to pg_locks
-- (waitstart timestamptz). spq_locks has the most expansive column set, including the
-- newly added column. If spq_locks is queried in a Postgres version where pg_locks
-- doesn't have some columns, the values for those columns in spq_locks will be NULL
CREATE OR REPLACE FUNCTION pg_catalog.spq_locks (
    OUT global_pid bigint,
    OUT nodeid int,
    OUT locktype text,
    OUT database oid,
    OUT relation oid,
    OUT relation_name text,
    OUT page integer,
    OUT tuple smallint,
    OUT virtualxid text,
    OUT transactionid xid,
    OUT classid oid,
    OUT objid oid,
    OUT objsubid smallint,
    OUT virtualtransaction text,
    OUT pid bigint,
    OUT sessionid bigint,
    OUT mode text,
    OUT granted boolean,
    OUT fastpath boolean,
    OUT locktag text,
    OUT global_sessionid text
)
    RETURNS SETOF record
    LANGUAGE plpgsql
    AS $function$
BEGIN
    RETURN QUERY
    SELECT *
    FROM json_to_recordset((
        SELECT
            json_agg(all_spq_locks_rows_as_jsonb.spq_locks_row_as_json)::json
        FROM (
            SELECT
                jsonb_set(json_array_elements(run_command_on_all_nodes.result::json)::jsonb, '{nodeid}',
                    run_command_on_all_nodes.nodeid::text::jsonb, true)::json AS spq_locks_row_as_json
            FROM
                run_command_on_all_nodes (
                    $$
                        SELECT
                            coalesce(to_json(array_agg(spq_locks_from_one_node.*)), '[{}]'::json)
                        FROM (
                            SELECT
                                global_pid, pg_locks.relation::regclass::text AS relation_name, pg_locks.*
                            FROM pg_locks
                        LEFT JOIN get_all_active_transactions () ON process_id = pid) AS spq_locks_from_one_node;
                    $$,
                    parallel:= TRUE,
                    give_warning_for_connection_errors:= TRUE)
            WHERE
                success = 't')
        AS all_spq_locks_rows_as_jsonb), true)
AS (
    global_pid bigint,
    nodeid int,
    locktype text,
    database oid,
    relation oid,
    relation_name text,
    page integer,
    tuple smallint,
    virtualxid text,
    transactionid xid,
    classid oid,
    objid oid,
    objsubid smallint,
    virtualtransaction text,
    pid bigint,
    sessionid bigint,
    mode text,
    granted boolean,
    fastpath boolean,
    locktag text,
    global_sessionid text
);
END;
$function$;

CREATE OR REPLACE VIEW __$spq$__.spq_locks AS
SELECT * FROM pg_catalog.spq_locks();

ALTER VIEW __$spq$__.spq_locks SET SCHEMA pg_catalog;

GRANT SELECT ON pg_catalog.spq_locks TO PUBLIC;

-- spq_stat_activity combines the pg_stat_activity views from all nodes and adds global_pid, nodeid and is_worker_query columns.
-- The columns of spq_stat_activity don't change based on the Postgres version, however the pg_stat_activity's columns do.
-- Both Postgres 13 and 14 added one more column to pg_stat_activity (leader_pid and query_id).
-- spq_stat_activity has the most expansive column set, including the newly added columns.
-- If spq_stat_activity is queried in a Postgres version where pg_stat_activity doesn't have some columns spq_stat_activity has
-- the values for those columns will be NULL

CREATE OR REPLACE FUNCTION pg_catalog.spq_stat_activity(OUT global_pid bigint, OUT nodeid int, OUT is_worker_query boolean, OUT datid oid, OUT datname name, OUT pid bigint,
                                                          OUT sessionid bigint, OUT usesysid oid, OUT usename name, OUT application_name text, OUT client_addr inet, OUT client_hostname text,
                                                          OUT client_port integer, OUT backend_start timestamp with time zone, OUT xact_start timestamp with time zone,
                                                          OUT query_start timestamp with time zone, OUT state_change timestamp with time zone, OUT waiting boolean, OUT enqueue text,
                                                          OUT state text, OUT resource_pool name, OUT query_id bigint, OUT query text, OUT connection_info text, OUT unique_sql_id bigint, OUT trace_id text)
    RETURNS SETOF record
    LANGUAGE plpgsql
    AS $function$
BEGIN
    RETURN QUERY SELECT * FROM json_to_recordset((
        SELECT json_agg(all_csa_rows_as_json.csa_row_as_json)::json FROM (
            SELECT jsonb_set(json_array_elements(run_command_on_all_nodes.result::json)::jsonb, '{nodeid}', run_command_on_all_nodes.nodeid::text::jsonb, true)::json AS csa_row_as_json
            FROM run_command_on_all_nodes($$
                SELECT coalesce(to_json(array_agg(csa_from_one_node.*)), '[{}]'::json)
                FROM (
                    SELECT global_pid, worker_query AS is_worker_query, pg_stat_activity.* FROM
                    pg_stat_activity LEFT JOIN get_all_active_transactions() ON process_id = pid
                ) AS csa_from_one_node;
            $$, parallel:=true, give_warning_for_connection_errors:=true)
            WHERE success = 't'
        ) AS all_csa_rows_as_json
    ), true)
    AS (global_pid bigint, nodeid int, is_worker_query boolean, datid oid, datname name, pid bigint,
        sessionid bigint, usesysid oid, usename name, application_name text, client_addr inet, client_hostname text,
        client_port integer, backend_start timestamp with time zone, xact_start timestamp with time zone,
        query_start timestamp with time zone, state_change timestamp with time zone, waiting boolean, enqueue text,
        state text, resource_pool name, query_id bigint, query text, connection_info text, unique_sql_id bigint, trace_id text);
END;
$function$;

CREATE OR REPLACE VIEW __$spq$__.spq_stat_activity AS
    SELECT * FROM pg_catalog.spq_stat_activity();

ALTER VIEW __$spq$__.spq_stat_activity SET SCHEMA pg_catalog;
GRANT SELECT ON pg_catalog.spq_stat_activity TO PUBLIC;

-- spq_stat_statements
CREATE FUNCTION pg_catalog.spq_query_stats(OUT unique_sql_id bigint,
            OUT user_id oid,
            OUT node_id int,
            OUT executor bigint,
            OUT partition_key text,
            OUT calls bigint)
RETURNS SETOF record
LANGUAGE C STRICT
AS 'MODULE_PATHNAME', $$spq_query_stats$$;

CREATE FUNCTION spq_get_active_worker_nodes(OUT node_name text, OUT node_port bigint)
    RETURNS SETOF record
    LANGUAGE C STRICT ROWS 100
    AS 'MODULE_PATHNAME', $$spq_get_active_worker_nodes$$;
COMMENT ON FUNCTION spq_get_active_worker_nodes() IS 'fetch set of active worker nodes';


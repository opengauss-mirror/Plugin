CREATE FUNCTION pg_catalog.spq_check_cluster_nodes (
    OUT from_nodename text,
    OUT from_nodeport int,
    OUT to_nodename text,
    OUT to_nodeport int,
    OUT result bool )
    RETURNS SETOF RECORD
    LANGUAGE C
    STRICT
    AS 'MODULE_PATHNAME', $$spq_check_cluster_nodes$$;

COMMENT ON FUNCTION pg_catalog.spq_check_cluster_nodes ()
    IS 'checks connections between all nodes in the cluster';

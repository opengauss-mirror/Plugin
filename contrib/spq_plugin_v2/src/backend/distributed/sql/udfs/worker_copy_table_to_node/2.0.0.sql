CREATE OR REPLACE FUNCTION pg_catalog.worker_copy_table_to_node(
    source_table regclass,
    target_node_name text,
    target_node_port integer,
    local_copy boolean)
RETURNS void
LANGUAGE C STRICT
AS 'MODULE_PATHNAME', $$worker_copy_table_to_node_with_nodename$$;
COMMENT ON FUNCTION pg_catalog.worker_copy_table_to_node(
    source_table regclass, 
    target_node_name text,
    target_node_port integer,
    local_copy boolean)
    IS 'Perform copy of a shard';
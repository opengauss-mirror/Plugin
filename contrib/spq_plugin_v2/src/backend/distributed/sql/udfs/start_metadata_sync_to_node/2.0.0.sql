CREATE FUNCTION start_metadata_sync_to_node(nodename text, nodeport integer)
 RETURNS VOID
 LANGUAGE C STRICT
 AS 'MODULE_PATHNAME', $$start_metadata_sync_to_node$$;
COMMENT ON FUNCTION start_metadata_sync_to_node(nodename text, nodeport integer)
    IS 'sync metadata to node';
REVOKE ALL ON FUNCTION start_metadata_sync_to_node(text, integer) FROM PUBLIC;

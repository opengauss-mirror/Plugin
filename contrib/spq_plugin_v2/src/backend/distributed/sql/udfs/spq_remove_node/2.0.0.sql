CREATE FUNCTION spq_remove_node(nodename text, nodeport integer)
 RETURNS void
 LANGUAGE C STRICT
 AS 'MODULE_PATHNAME', $$spq_remove_node$$;

COMMENT ON FUNCTION spq_remove_node(nodename text, nodeport integer) IS 'remove node from the cluster';
REVOKE ALL ON FUNCTION spq_remove_node(text,int) FROM PUBLIC;

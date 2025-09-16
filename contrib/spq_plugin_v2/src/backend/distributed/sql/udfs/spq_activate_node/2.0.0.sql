CREATE FUNCTION spq_activate_node(nodename text,
                                     nodeport integer)
    RETURNS INTEGER
    LANGUAGE C STRICT
    AS 'MODULE_PATHNAME',$$spq_activate_node$$;
COMMENT ON FUNCTION spq_activate_node(nodename text, nodeport integer)
    IS 'activate a node which is in the cluster';

REVOKE ALL ON FUNCTION spq_activate_node(text,int) FROM PUBLIC;

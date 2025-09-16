CREATE FUNCTION pg_catalog.spq_calculate_gpid(nodeid integer,
                                                tid integer)
    RETURNS BIGINT
    LANGUAGE C STRICT
    AS 'MODULE_PATHNAME',$$spq_calculate_gpid$$;
COMMENT ON FUNCTION pg_catalog.spq_calculate_gpid(nodeid integer, tid integer)
    IS 'calculate gpid of a backend running on any node';

GRANT EXECUTE ON FUNCTION pg_catalog.spq_calculate_gpid(integer, integer) TO PUBLIC;

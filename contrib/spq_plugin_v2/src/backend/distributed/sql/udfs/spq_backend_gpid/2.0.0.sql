CREATE FUNCTION pg_catalog.spq_backend_gpid()
    RETURNS BIGINT
    LANGUAGE C STRICT
    AS 'MODULE_PATHNAME',$$spq_backend_gpid$$;
COMMENT ON FUNCTION pg_catalog.spq_backend_gpid()
    IS 'returns gpid of the current backend';

GRANT EXECUTE ON FUNCTION pg_catalog.spq_backend_gpid() TO PUBLIC;

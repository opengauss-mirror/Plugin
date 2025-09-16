CREATE OR REPLACE FUNCTION pg_catalog.spq_version()
    RETURNS text
    LANGUAGE C STABLE STRICT
    AS 'MODULE_PATHNAME', $$spq_version$$;
COMMENT ON FUNCTION pg_catalog.spq_version()
    IS 'Spq version string';

CREATE FUNCTION pg_catalog.spq_server_id()
    RETURNS uuid
    LANGUAGE C STRICT
    AS 'MODULE_PATHNAME', $$spq_server_id$$;
COMMENT ON FUNCTION spq_server_id()
    IS 'generates a random UUID to be used as server identifier';

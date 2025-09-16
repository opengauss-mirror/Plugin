CREATE FUNCTION pg_catalog.spq_is_coordinator()
 RETURNS bool
 LANGUAGE c
 STRICT
AS 'MODULE_PATHNAME', $$spq_is_coordinator$$;
COMMENT ON FUNCTION pg_catalog.spq_is_coordinator()
 IS 'returns whether the current node is a coordinator';

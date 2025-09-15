CREATE FUNCTION pg_catalog.spq_stat_statements_reset()
RETURNS VOID
LANGUAGE C STRICT
AS 'MODULE_PATHNAME', $$spq_stat_statements_reset$$;

REVOKE ALL ON FUNCTION pg_catalog.spq_stat_statements_reset() FROM PUBLIC;

CREATE OR REPLACE FUNCTION pg_catalog.spq_executor_name(executor_type int)
RETURNS text
LANGUAGE C STRICT
AS 'MODULE_PATHNAME', $$spq_executor_name$$;

COMMENT ON FUNCTION pg_catalog.spq_executor_name(int)
IS 'return the name of the external based for the value in spq_stat_statements() output';

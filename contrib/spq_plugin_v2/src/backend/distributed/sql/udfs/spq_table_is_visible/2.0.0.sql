CREATE OR REPLACE FUNCTION pg_catalog.spq_table_is_visible(oid)
  RETURNS bool
LANGUAGE C STRICT
STABLE
AS 'MODULE_PATHNAME', $$spq_table_is_visible$$;
COMMENT ON FUNCTION spq_table_is_visible(oid)
	IS 'wrapper on pg_table_is_visible, filtering out tables (and indexes) that are known to be shards';

DROP FUNCTION pg_catalog.replicate_reference_tables;
CREATE FUNCTION pg_catalog.replicate_reference_tables(shard_transfer_mode __$spq$__.shard_transfer_mode default 'auto')
  RETURNS VOID
  LANGUAGE C STRICT
  AS 'MODULE_PATHNAME', $$replicate_reference_tables$$;
COMMENT ON FUNCTION pg_catalog.replicate_reference_tables(__$spq$__.shard_transfer_mode)
  IS 'replicate reference tables to all nodes';
REVOKE ALL ON FUNCTION pg_catalog.replicate_reference_tables(__$spq$__.shard_transfer_mode) FROM PUBLIC;

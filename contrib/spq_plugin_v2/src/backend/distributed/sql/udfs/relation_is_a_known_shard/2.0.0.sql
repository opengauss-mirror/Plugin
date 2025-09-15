CREATE OR REPLACE FUNCTION pg_catalog.relation_is_a_known_shard(regclass)
  RETURNS bool
LANGUAGE C STRICT
AS 'MODULE_PATHNAME', $$relation_is_a_known_shard$$;
COMMENT ON FUNCTION relation_is_a_known_shard(regclass)
    IS 'returns true if the given relation is a known shard';


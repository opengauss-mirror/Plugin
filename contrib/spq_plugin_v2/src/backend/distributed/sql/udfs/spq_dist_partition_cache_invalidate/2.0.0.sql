CREATE OR REPLACE FUNCTION pg_catalog.spq_dist_partition_cache_invalidate()
    RETURNS trigger
    LANGUAGE C
    AS 'MODULE_PATHNAME', $$spq_dist_partition_cache_invalidate$$;
COMMENT ON FUNCTION pg_catalog.spq_dist_partition_cache_invalidate()
    IS 'register relcache invalidation for changed rows';

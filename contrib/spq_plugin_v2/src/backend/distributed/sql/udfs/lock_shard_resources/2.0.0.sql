CREATE FUNCTION lock_shard_resources(lock_mode int, shard_id bigint[])
    RETURNS VOID
    LANGUAGE C STRICT
    AS 'MODULE_PATHNAME', $$lock_shard_resources$$;
COMMENT ON FUNCTION lock_shard_resources(lock_mode int, shard_id bigint[])
    IS 'lock shard resource to serialise non-commutative writes';

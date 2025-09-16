CREATE FUNCTION pg_catalog.shard_name(object_name regclass, shard_id bigint)
    RETURNS text
    LANGUAGE C STABLE STRICT
    AS 'MODULE_PATHNAME', $$shard_name$$;
COMMENT ON FUNCTION pg_catalog.shard_name(object_name regclass, shard_id bigint)
    IS 'returns schema-qualified, shard-extended identifier of object name';

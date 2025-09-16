CREATE FUNCTION pg_catalog.spq_drop_all_shards(logicalrelid regclass,
                                                 schema_name text,
                                                 table_name text,
                                                 drop_shards_metadata_only boolean default false)
    RETURNS integer
    LANGUAGE C STRICT
    AS 'MODULE_PATHNAME', $$spq_drop_all_shards$$;
COMMENT ON FUNCTION pg_catalog.spq_drop_all_shards(regclass, text, text, boolean)
    IS 'drop all shards in a relation and update metadata';

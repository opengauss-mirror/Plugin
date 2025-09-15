CREATE FUNCTION master_remove_partition_metadata(logicalrelid regclass,
                                                   schema_name text,
                                                   table_name text)
    RETURNS void
    LANGUAGE C STRICT
    AS 'MODULE_PATHNAME', $$master_remove_partition_metadata$$;
COMMENT ON FUNCTION master_remove_partition_metadata(logicalrelid regclass,
                                                       schema_name text,
                                                       table_name text)
    IS 'deletes the partition metadata of a distributed table';

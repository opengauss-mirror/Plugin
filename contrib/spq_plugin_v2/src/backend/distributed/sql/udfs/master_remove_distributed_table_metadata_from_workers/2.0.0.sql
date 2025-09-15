CREATE OR REPLACE FUNCTION master_remove_distributed_table_metadata_from_workers(logicalrelid regclass,
                                                   schema_name text,
                                                   table_name text)
    RETURNS void
    LANGUAGE C STRICT
    AS 'MODULE_PATHNAME', $$master_remove_distributed_table_metadata_from_workers$$;
COMMENT ON FUNCTION master_remove_distributed_table_metadata_from_workers(logicalrelid regclass,
                                                       schema_name text,
                                                       table_name text)
    IS 'drops the table and removes all the metadata belonging the distributed table in the worker nodes with metadata.';

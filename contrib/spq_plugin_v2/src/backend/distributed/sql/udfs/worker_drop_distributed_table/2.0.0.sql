CREATE FUNCTION worker_drop_distributed_table(table_name text)
    RETURNS VOID
    LANGUAGE C STRICT
    AS 'MODULE_PATHNAME', $$worker_drop_distributed_table$$;

COMMENT ON FUNCTION worker_drop_distributed_table(table_name text)
    IS 'drop the distributed table and its reference from metadata tables';

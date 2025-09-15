-- table size functions
CREATE FUNCTION spq_table_size(logicalrelid regclass)
    RETURNS bigint
    LANGUAGE C STRICT
    AS 'MODULE_PATHNAME', $$spq_table_size$$;
COMMENT ON FUNCTION spq_table_size(logicalrelid regclass)
    IS 'get disk space used by the specified table, excluding indexes';
CREATE FUNCTION spq_relation_size(logicalrelid regclass)
    RETURNS bigint
    LANGUAGE C STRICT
    AS 'MODULE_PATHNAME', $$spq_relation_size$$;
COMMENT ON FUNCTION spq_relation_size(logicalrelid regclass)
    IS 'get disk space used by the ''main'' fork';
CREATE FUNCTION pg_catalog.spq_total_relation_size(logicalrelid regclass, fail_on_error boolean default true)
    RETURNS bigint
    LANGUAGE C STRICT
    AS 'MODULE_PATHNAME', $$spq_total_relation_size$$;
COMMENT ON FUNCTION pg_catalog.spq_total_relation_size(logicalrelid regclass, boolean)
    IS 'get total disk space used by the specified table';
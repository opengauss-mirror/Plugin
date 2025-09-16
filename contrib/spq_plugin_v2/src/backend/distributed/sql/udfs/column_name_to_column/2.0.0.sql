CREATE FUNCTION pg_catalog.column_name_to_column(table_name regclass, column_name text)
    RETURNS text
    LANGUAGE C STRICT
    AS 'MODULE_PATHNAME', $$column_name_to_column$$;
COMMENT ON FUNCTION pg_catalog.column_name_to_column(table_name regclass, column_name text)
    IS 'convert a column name to its textual Var representation';


CREATE OR REPLACE FUNCTION pg_catalog.create_intermediate_result(result_id text, query text)
    RETURNS bigint
    LANGUAGE C STRICT VOLATILE
    AS 'MODULE_PATHNAME', $$create_intermediate_result$$;
COMMENT ON FUNCTION pg_catalog.create_intermediate_result(text,text)
    IS 'execute a query and write its results to local result file';

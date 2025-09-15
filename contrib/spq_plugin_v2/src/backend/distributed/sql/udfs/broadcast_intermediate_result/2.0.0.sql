CREATE OR REPLACE FUNCTION pg_catalog.broadcast_intermediate_result(result_id text, query text)
    RETURNS bigint
    LANGUAGE C STRICT VOLATILE
    AS 'MODULE_PATHNAME', $$broadcast_intermediate_result$$;
COMMENT ON FUNCTION pg_catalog.broadcast_intermediate_result(text,text)
    IS 'execute a query and write its results to an result file on all workers';


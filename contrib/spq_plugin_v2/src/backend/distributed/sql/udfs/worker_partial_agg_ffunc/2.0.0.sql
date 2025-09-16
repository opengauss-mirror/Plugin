CREATE FUNCTION pg_catalog.worker_partial_agg_ffunc(internal)
RETURNS cstring
AS 'MODULE_PATHNAME'
LANGUAGE C;
COMMENT ON FUNCTION pg_catalog.worker_partial_agg_ffunc(internal)
    IS 'finalizer for worker_partial_agg';
REVOKE ALL ON FUNCTION pg_catalog.worker_partial_agg_ffunc(internal) FROM PUBLIC;
GRANT EXECUTE ON FUNCTION pg_catalog.worker_partial_agg_ffunc(internal) TO PUBLIC;

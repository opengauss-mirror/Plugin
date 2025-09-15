CREATE FUNCTION pg_catalog.worker_partial_agg_sfunc(internal, oid, anyelement)
RETURNS internal
AS 'MODULE_PATHNAME'
LANGUAGE C;
COMMENT ON FUNCTION pg_catalog.worker_partial_agg_sfunc(internal, oid, anyelement)
    IS 'transition function for worker_partial_agg';
REVOKE ALL ON FUNCTION pg_catalog.worker_partial_agg_sfunc(internal, oid, anyelement) FROM PUBLIC;
GRANT EXECUTE ON FUNCTION pg_catalog.worker_partial_agg_sfunc(internal, oid, anyelement) TO PUBLIC;

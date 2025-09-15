CREATE FUNCTION pg_catalog.coord_combine_agg_ffunc(internal, oid, cstring, anyelement)
RETURNS anyelement
AS 'MODULE_PATHNAME'
LANGUAGE C;
COMMENT ON FUNCTION pg_catalog.coord_combine_agg_ffunc(internal, oid, cstring, anyelement)
    IS 'finalizer for coord_combine_agg';
REVOKE ALL ON FUNCTION pg_catalog.coord_combine_agg_ffunc(internal, oid, cstring, anyelement) FROM PUBLIC;
GRANT EXECUTE ON FUNCTION pg_catalog.coord_combine_agg_ffunc(internal, oid, cstring, anyelement) TO PUBLIC;

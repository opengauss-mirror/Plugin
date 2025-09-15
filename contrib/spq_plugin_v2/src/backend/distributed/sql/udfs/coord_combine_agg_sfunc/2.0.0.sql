CREATE FUNCTION pg_catalog.coord_combine_agg_sfunc(internal, oid, cstring, anyelement)
RETURNS internal
AS 'MODULE_PATHNAME'
LANGUAGE C;
COMMENT ON FUNCTION pg_catalog.coord_combine_agg_sfunc(internal, oid, cstring, anyelement)
    IS 'transition function for coord_combine_agg';
REVOKE ALL ON FUNCTION pg_catalog.coord_combine_agg_sfunc(internal, oid, cstring, anyelement) FROM PUBLIC;
GRANT EXECUTE ON FUNCTION pg_catalog.coord_combine_agg_sfunc(internal, oid, cstring, anyelement) TO PUBLIC;

#include "udfs/spq_jsonb_concatenate_final/2.0.0.sql"
-- Spq json aggregate helpers @FIXME syntax error
--CREATE FUNCTION pg_catalog.citus_jsonb_concatenate(state jsonb, val jsonb)
-- RETURNS jsonb
-- LANGUAGE SQL
--AS $function$
-- SELECT CASE
--  WHEN val IS NULL THEN state
--  WHEN jsonb_typeof(state) = 'null' THEN val
--  ELSE state || val
-- END;
--$function$;

--CREATE AGGREGATE pg_catalog.jsonb_cat_agg(jsonb) (
--    SFUNC = citus_jsonb_concatenate,
--    FINALFUNC = spq_jsonb_concatenate_final,
--    STYPE = jsonb,
--    INITCOND = 'null'
--);
--COMMENT ON AGGREGATE pg_catalog.jsonb_cat_agg(jsonb)
--    IS 'concatenate input jsonbs into a single jsonb';

-- Support infrastructure for distributing aggregation
#include "udfs/worker_partial_agg_sfunc/2.0.0.sql"
#include "udfs/worker_partial_agg_ffunc/2.0.0.sql"
#include "udfs/coord_combine_agg_sfunc/2.0.0.sql"
#include "udfs/coord_combine_agg_ffunc/2.0.0.sql"

--@FIXME
-- select worker_partial_agg(agg, ...)
-- equivalent to
-- select to_cstring(agg_without_ffunc(...))

--CREATE AGGREGATE pg_catalog.worker_partial_agg(oid, anyelement) (
--    STYPE = internal,
--    SFUNC = pg_catalog.worker_partial_agg_sfunc,
--    FINALFUNC = pg_catalog.worker_partial_agg_ffunc
--);
--COMMENT ON AGGREGATE pg_catalog.worker_partial_agg(oid, anyelement)
--    IS 'support aggregate for implementing partial aggregation on workers';

-- select coord_combine_agg(agg, col)
-- equivalent to
-- select agg_ffunc(agg_combine(from_cstring(col)))

--CREATE AGGREGATE pg_catalog.coord_combine_agg(oid, cstring, anyelement) (
--    STYPE = internal,
--    SFUNC = pg_catalog.coord_combine_agg_sfunc,
--    FINALFUNC = pg_catalog.coord_combine_agg_ffunc,
--    FINALFUNC_EXTRA
--);
--COMMENT ON AGGREGATE pg_catalog.coord_combine_agg(oid, cstring, anyelement)
--    IS 'support aggregate for implementing combining partial aggregate results from workers';

--REVOKE ALL ON FUNCTION pg_catalog.worker_partial_agg(oid, anyelement) FROM PUBLIC;
--REVOKE ALL ON FUNCTION pg_catalog.coord_combine_agg(oid, cstring, anyelement) FROM PUBLIC;

--GRANT EXECUTE ON FUNCTION pg_catalog.worker_partial_agg(oid, anyelement) TO PUBLIC;
--GRANT EXECUTE ON FUNCTION pg_catalog.coord_combine_agg(oid, cstring, anyelement) TO PUBLIC;

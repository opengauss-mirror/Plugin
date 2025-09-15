-- pg_dist_poolinfo
CREATE FUNCTION pg_catalog.poolinfo_valid(text)
 RETURNS boolean
    LANGUAGE C STRICT
    AS 'MODULE_PATHNAME', $$poolinfo_valid$$;
COMMENT ON FUNCTION pg_catalog.poolinfo_valid(text) IS 'returns whether a poolinfo is valid';

CREATE TABLE __$spq$__.pg_dist_poolinfo (
    nodeid integer PRIMARY KEY,
    poolinfo text NOT NULL CONSTRAINT poolinfo_valid CHECK (poolinfo_valid(poolinfo))
);

ALTER TABLE __$spq$__.pg_dist_poolinfo SET SCHEMA pg_catalog;
GRANT SELECT ON pg_catalog.pg_dist_poolinfo TO public;

CREATE TRIGGER dist_poolinfo_cache_invalidate
    AFTER INSERT OR UPDATE OR DELETE OR TRUNCATE
    ON pg_catalog.pg_dist_poolinfo
    FOR EACH STATEMENT EXECUTE PROCEDURE spq_conninfo_cache_invalidate();

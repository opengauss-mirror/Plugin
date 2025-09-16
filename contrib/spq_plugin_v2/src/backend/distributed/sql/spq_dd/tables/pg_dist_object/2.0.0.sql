CREATE TABLE __$spq$__.pg_dist_object (
    -- fields used for composite primary key
    classid oid NOT NULL,
    objid oid NOT NULL,
    objsubid integer NOT NULL,

    -- fields used for upgrades
    type text DEFAULT NULL,
    object_names text[] DEFAULT NULL,
    object_args text[] DEFAULT NULL,

    -- fields that are only valid for distributed
    -- functions/procedures
    distribution_argument_index int,
    colocationid int,

    force_delegation bool DEFAULT NULL,

    CONSTRAINT pg_dist_object_pkey PRIMARY KEY (classid, objid, objsubid)
);

#include "../../../udfs/spq_dist_object_cache_invalidate/2.0.0.sql"

CREATE TRIGGER dist_object_cache_invalidate
    AFTER INSERT OR UPDATE OR DELETE
    ON __$spq$__.pg_dist_object
    FOR EACH ROW EXECUTE PROCEDURE spq_dist_object_cache_invalidate();

-- add citus extension owner as a distributed object, if not already in there
INSERT INTO __$spq$__.pg_dist_object SELECT
  (SELECT oid FROM pg_class WHERE relname = 'pg_authid') AS oid,
  (SELECT oid FROM pg_authid WHERE rolname = current_user) as objid,
  0 as objsubid;

INSERT INTO __$spq$__.pg_dist_object SELECT
  'pg_catalog.pg_database'::regclass::oid AS oid,
  (SELECT oid FROM pg_database WHERE datname = current_database()) as objid,
  0 as objsubid;

ALTER TABLE __$spq$__.pg_dist_object SET SCHEMA pg_catalog;
GRANT SELECT ON pg_catalog.pg_dist_object TO public;

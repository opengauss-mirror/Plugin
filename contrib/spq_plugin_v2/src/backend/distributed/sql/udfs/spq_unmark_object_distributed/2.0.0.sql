CREATE FUNCTION pg_catalog.spq_unmark_object_distributed(classid oid, objid oid, objsubid int)
    RETURNS void
    LANGUAGE C STRICT
    AS 'MODULE_PATHNAME', $$spq_unmark_object_distributed$$;
COMMENT ON FUNCTION pg_catalog.spq_unmark_object_distributed(classid oid, objid oid, objsubid int)
    IS 'remove an object address from __$spq$__.pg_dist_object once the object has been deleted';

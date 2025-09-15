CREATE FUNCTION pg_catalog.spq_truncate_trigger()
    RETURNS trigger
    LANGUAGE C STRICT
    AS 'MODULE_PATHNAME', $$spq_truncate_trigger$$;
COMMENT ON FUNCTION pg_catalog.spq_truncate_trigger()
    IS 'trigger function called when truncating the distributed table';

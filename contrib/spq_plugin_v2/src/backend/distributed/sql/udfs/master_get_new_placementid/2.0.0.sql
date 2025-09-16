CREATE FUNCTION pg_catalog.master_get_new_placementid()
    RETURNS bigint
    LANGUAGE C STRICT
    AS 'MODULE_PATHNAME', $$master_get_new_placementid$$;
COMMENT ON FUNCTION pg_catalog.master_get_new_placementid()
    IS 'fetch unique placementid';


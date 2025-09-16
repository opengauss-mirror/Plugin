CREATE FUNCTION pg_catalog.recover_prepared_transactions()
    RETURNS int
    LANGUAGE C STRICT
    AS 'MODULE_PATHNAME', $$recover_prepared_transactions$$;
COMMENT ON FUNCTION pg_catalog.recover_prepared_transactions()
    IS 'recover prepared transactions started by this node';
REVOKE ALL ON FUNCTION recover_prepared_transactions() FROM PUBLIC;


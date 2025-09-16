CREATE FUNCTION worker_hash(value "any")
 RETURNS integer
    LANGUAGE C STRICT
    AS 'MODULE_PATHNAME', $$worker_hash$$;
COMMENT ON FUNCTION worker_hash(value "any")
    IS 'calculate hashed value and return it';

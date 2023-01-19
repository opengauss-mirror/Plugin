DROP FUNCTION IF EXISTS pg_catalog.connection_id() CASCADE;

CREATE OR REPLACE FUNCTION pg_catalog.connection_id()
RETURNS int8 AS
$$
BEGIN
    RETURN (select pg_backend_pid());
END;
$$
LANGUAGE plpgsql;

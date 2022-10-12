DROP FUNCTION IF EXISTS pg_catalog.oct(text) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.oct(boolean) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.oct(bit) CASCADE;

CREATE OR REPLACE FUNCTION pg_catalog.oct(t1 text) RETURNS text AS
$$
BEGIN
    RETURN (SELECT conv(t1, 10, 8));
END;
$$
LANGUAGE plpgsql;

CREATE OR REPLACE FUNCTION pg_catalog.oct(t1 bit) RETURNS text AS
$$
BEGIN
    RETURN (SELECT conv(t1, 10, 8));
END;
$$
LANGUAGE plpgsql;

CREATE OR REPLACE FUNCTION pg_catalog.oct(t1 boolean) RETURNS text AS
$$
BEGIN
    RETURN int4(t1);
END;
$$
LANGUAGE plpgsql;

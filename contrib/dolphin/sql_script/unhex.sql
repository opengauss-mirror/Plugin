DROP FUNCTION IF EXISTS pg_catalog.unhex (text) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.unhex (numeric) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.unhex (bool) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.unhex (bytea) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.unhex ("timestamp") CASCADE;

CREATE OR REPLACE FUNCTION pg_catalog.unhex (text)  RETURNS text LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'unhex';

CREATE OR REPLACE FUNCTION pg_catalog.unhex (num1 numeric)
RETURNS text
AS
$$
BEGIN
    RETURN (SELECT unhex(to_char(num1)));
END;
$$
LANGUAGE plpgsql;

CREATE OR REPLACE FUNCTION pg_catalog.unhex (bool1 bool)
RETURNS text
AS
$$
BEGIN
    RETURN (SELECT unhex(cast(bool1 AS numeric)));
END;
$$
LANGUAGE plpgsql;

CREATE OR REPLACE FUNCTION pg_catalog.unhex (bin1 bytea)
RETURNS text
AS
$$
BEGIN
    RETURN NULL;
END;
$$
LANGUAGE plpgsql;

CREATE OR REPLACE FUNCTION pg_catalog.unhex (datetime1 "timestamp")
RETURNS text
AS
$$
BEGIN
    RETURN NULL;
END;
$$
LANGUAGE plpgsql;
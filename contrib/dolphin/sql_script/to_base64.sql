DROP FUNCTION IF EXISTS pg_catalog.to_base64 (bytea) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.to_base64 (numeric) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.to_base64 (text) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.to_base64 (bit) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.to_base64 (bytea)  RETURNS text LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'base64_encode';

CREATE OR REPLACE FUNCTION pg_catalog.to_base64 (num1 numeric)
RETURNS text
AS
$$
BEGIN
    RETURN (SELECT to_base64(cast(to_char(num1) AS bytea)));
END;
$$
LANGUAGE plpgsql;

CREATE OR REPLACE FUNCTION pg_catalog.to_base64 (t1 text)
RETURNS text
AS
$$
BEGIN
    RETURN (SELECT to_base64(cast(t1 AS bytea)));
END;
$$
LANGUAGE plpgsql;

CREATE OR REPLACE FUNCTION pg_catalog.to_base64(bit1 bit)
RETURNS text
AS
$$
BEGIN
    RETURN (SELECT to_base64((decode(hex(bit1), 'hex'))));
END;
$$
LANGUAGE plpgsql;
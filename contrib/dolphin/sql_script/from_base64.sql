DROP FUNCTION IF EXISTS pg_catalog.from_base64 (text) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.from_base64 (bool) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.from_base64 (bit) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.from_base64 (text)  RETURNS text LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'base64_decode';

CREATE OR REPLACE FUNCTION pg_catalog.from_base64 (bool1 bool)
RETURNS text
AS
$$
BEGIN
    RETURN (SELECT from_base64(''));
END;
$$
LANGUAGE plpgsql;

CREATE OR REPLACE FUNCTION pg_catalog.from_base64(bit1 bit)
RETURNS text
AS
$$
BEGIN
    RETURN (SELECT from_base64(encode((decode(hex(bit1), 'hex')), 'escape')));
END;
$$
LANGUAGE plpgsql;
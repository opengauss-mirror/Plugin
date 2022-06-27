DROP FUNCTION IF EXISTS pg_catalog.ltrim(bytea, bytea) cascade;
CREATE OR REPLACE FUNCTION pg_catalog.ltrim(bytea, bytea) RETURNS bytea LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'byteatrim_leading';

DROP FUNCTION IF EXISTS pg_catalog.rtrim(bytea, bytea) cascade;
CREATE OR REPLACE FUNCTION pg_catalog.rtrim(bytea, bytea) RETURNS bytea LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'byteatrim_trailing';

DROP FUNCTION IF EXISTS pg_catalog.btrim(bytea) cascade;
CREATE OR REPLACE FUNCTION pg_catalog.btrim(bytea) RETURNS bytea AS
$$
BEGIN
    RETURN (SELECT btrim($1, ' '::bytea));
END;
$$
LANGUAGE plpgsql;

DROP FUNCTION IF EXISTS pg_catalog.ltrim(bytea) cascade;
CREATE OR REPLACE FUNCTION pg_catalog.ltrim(bytea) RETURNS bytea AS
$$
BEGIN
    RETURN (SELECT ltrim($1, ' '::bytea));
END;
$$
LANGUAGE plpgsql;

DROP FUNCTION IF EXISTS pg_catalog.rtrim(bytea) cascade;
CREATE OR REPLACE FUNCTION pg_catalog.rtrim(bytea) RETURNS bytea AS
$$
BEGIN
    RETURN (SELECT rtrim($1, ' '::bytea));
END;
$$
LANGUAGE plpgsql;
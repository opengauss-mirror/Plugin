DROP FUNCTION IF EXISTS pg_catalog.left(bytea, int) cascade;
CREATE OR REPLACE FUNCTION pg_catalog.left(bytea, int) RETURNS bytea LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'bytea_left';

DROP FUNCTION IF EXISTS pg_catalog.right(bytea, int) cascade;
CREATE OR REPLACE FUNCTION pg_catalog.right(bytea, int) RETURNS bytea LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'bytea_right';

DROP FUNCTION IF EXISTS pg_catalog.quote(text) CASCADE;

CREATE OR REPLACE FUNCTION pg_catalog.quote(t text)
returns text
as 
$$
begin
  return (select quote_literal(t));
end;
$$
language plpgsql;

DROP FUNCTION IF EXISTS pg_catalog.quote(anyelement) CASCADE;

CREATE OR REPLACE FUNCTION pg_catalog.quote(t anyelement)
returns text
as 
$$
begin
  return (select quote_literal(t));
end;
$$
language plpgsql;

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

DROP FUNCTION IF EXISTS pg_catalog.mid(text, int, int) cascade;
CREATE OR REPLACE FUNCTION pg_catalog.mid(text, int, int) RETURNS text AS
$$
BEGIN
    RETURN (SELECT substr($1, $2, $3));
END;
$$
LANGUAGE plpgsql;

DROP FUNCTION IF EXISTS pg_catalog.mid(text, int) cascade;
CREATE OR REPLACE FUNCTION pg_catalog.mid(text, int) RETURNS text AS
$$
BEGIN
    RETURN (SELECT substr($1, $2));
END;
$$
LANGUAGE plpgsql;

DROP FUNCTION IF EXISTS pg_catalog.mid(bytea, int, int) cascade;
CREATE OR REPLACE FUNCTION pg_catalog.mid(bytea, int, int) RETURNS bytea AS
$$
BEGIN
    RETURN (SELECT substr($1, $2, $3));
END;
$$
LANGUAGE plpgsql;

DROP FUNCTION IF EXISTS pg_catalog.mid(bytea, int) cascade;
CREATE OR REPLACE FUNCTION pg_catalog.mid(bytea, int) RETURNS bytea AS
$$
BEGIN
    RETURN (SELECT substr($1, $2));
END;
$$
LANGUAGE plpgsql;

DROP FUNCTION IF EXISTS pg_catalog.convert (text, name) CASCADE;
CREATE FUNCTION pg_catalog.convert (
text,
name
) RETURNS text LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'pg_convert_to_text';



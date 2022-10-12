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

create function pg_catalog.textxor (
text,
text
) RETURNS int LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'textxor';

create operator pg_catalog.^(leftarg = text, rightarg = text, procedure = pg_catalog.textxor);

create function pg_catalog.boolxor (
boolean,
boolean
) RETURNS int LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'boolxor';
create operator pg_catalog.^(leftarg = boolean, rightarg = boolean, procedure = pg_catalog.boolxor);

CREATE OR REPLACE FUNCTION pg_catalog.ord (text) RETURNS int16 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'ord_text';

CREATE OR REPLACE FUNCTION pg_catalog.ord (numeric) RETURNS int16 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'ord_numeric';

DROP FUNCTION IF EXISTS pg_catalog.substring_index (text, text, numeric) CASCADE;
CREATE FUNCTION pg_catalog.substring_index (
text,
text,
numeric
) RETURNS text LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'substring_index';

DROP FUNCTION IF EXISTS pg_catalog.substring_index (boolean, text, numeric) CASCADE;
CREATE FUNCTION pg_catalog.substring_index (
boolean,
text,
numeric
) RETURNS text LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'substring_index_bool_1';

DROP FUNCTION IF EXISTS pg_catalog.substring_index (text, boolean, numeric) CASCADE;
CREATE FUNCTION pg_catalog.substring_index (
text,
boolean,
numeric
) RETURNS text LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'substring_index_bool_2';

DROP FUNCTION IF EXISTS pg_catalog.substring_index (boolean, boolean, numeric) CASCADE;
CREATE FUNCTION pg_catalog.substring_index (
boolean,
boolean,
numeric
) RETURNS text LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'substring_index_2bool';

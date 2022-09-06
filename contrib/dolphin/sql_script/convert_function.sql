DROP FUNCTION IF EXISTS pg_catalog.convert (text, name) CASCADE;
CREATE FUNCTION pg_catalog.convert (
text,
name
) RETURNS text LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'pg_convert_to_text';

DROP FUNCTION IF EXISTS pg_catalog.set_native_password(text, text) cascade;
CREATE OR REPLACE FUNCTION pg_catalog.set_native_password(text, text) RETURNS text LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'set_native_password';

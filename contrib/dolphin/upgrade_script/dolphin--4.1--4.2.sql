CREATE or replace FUNCTION pg_catalog.replace(json, text, text) RETURNS text LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.replace($1::text, $2, $3)';

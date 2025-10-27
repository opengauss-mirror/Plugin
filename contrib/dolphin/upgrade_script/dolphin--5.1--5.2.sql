DROP CAST IF EXISTS (varchar AS bytea);
DROP FUNCTION IF EXISTS pg_catalog.varchar_bytea(varchar);
CREATE FUNCTION pg_catalog.varchar_bytea(varchar) RETURNS bytea LANGUAGE INTERNAL IMMUTABLE STRICT AS 'texttoraw';
CREATE CAST (varchar AS bytea) WITH FUNCTION pg_catalog.varchar_bytea(varchar) AS ASSIGNMENT;

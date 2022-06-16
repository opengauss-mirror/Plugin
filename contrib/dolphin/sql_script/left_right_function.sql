DROP FUNCTION IF EXISTS pg_catalog.left(bytea, int) cascade;
CREATE OR REPLACE FUNCTION pg_catalog.left(bytea, int) RETURNS bytea LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'bytea_left';

DROP FUNCTION IF EXISTS pg_catalog.right(bytea, int) cascade;
CREATE OR REPLACE FUNCTION pg_catalog.right(bytea, int) RETURNS bytea LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'bytea_right';

DROP FUNCTION IF EXISTS pg_catalog.uuid_short() CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.uuid_short()
RETURNS uint8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'uuid_short';

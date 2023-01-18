DROP FUNCTION IF EXISTS pg_catalog.enum2float8(anyenum) cascade;
CREATE OR REPLACE FUNCTION pg_catalog.enum2float8(anyenum) RETURNS float8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'Enum2Float8';

INSERT INTO pg_catalog.pg_cast
-- castsource is anyenum(3500), casttarget(701) is float8, castowner is 10(superuser)
SELECT 3500, 701, oid, 'i', 'f', 10
FROM pg_proc WHERE 
proname = 'enum2float8' AND
-- namespace is pg_catalog
pronamespace = 11 AND
-- input arg is anyenum
proargtypes='3500' AND
-- return type is float8
prorettype = 701;
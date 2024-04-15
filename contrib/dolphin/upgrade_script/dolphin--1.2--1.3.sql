-- bool cast bit/float/double
DROP FUNCTION IF EXISTS pg_catalog.booltobit(bool, int4) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.booltofloat4(bool) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.booltofloat8(bool) CASCADE;

DROP CAST IF EXISTS (bool AS bit) CASCADE;
DROP CAST IF EXISTS (bool AS float4) CASCADE;
DROP CAST IF EXISTS (bool AS float8) CASCADE;

CREATE OR REPLACE FUNCTION pg_catalog.booltobit(bool, int4) RETURNS bit LANGUAGE C STRICT AS  '$libdir/dolphin',  'bool_bit';
CREATE CAST (bool AS bit) WITH FUNCTION pg_catalog.booltobit(bool, int4) AS ASSIGNMENT;

CREATE OR REPLACE FUNCTION pg_catalog.booltofloat4(bool) RETURNS float4 LANGUAGE C STRICT AS  '$libdir/dolphin',  'bool_float4';
CREATE CAST (bool AS float4) WITH FUNCTION pg_catalog.booltofloat4(bool) AS ASSIGNMENT;

CREATE OR REPLACE FUNCTION pg_catalog.booltofloat8(bool) RETURNS float8 LANGUAGE C STRICT AS  '$libdir/dolphin',  'bool_float8';
CREATE CAST (bool AS float8) WITH FUNCTION pg_catalog.booltofloat8(bool) AS ASSIGNMENT;

DROP FUNCTION IF EXISTS pg_catalog.atan2 (boolean, boolean);
CREATE OR REPLACE FUNCTION pg_catalog.atan2 (boolean, boolean)  RETURNS float8 AS $$ SELECT pg_catalog.atan2($1::int4, $2::int4) $$ LANGUAGE SQL;
DROP FUNCTION IF EXISTS pg_catalog.atan2 (boolean, float8);
CREATE OR REPLACE FUNCTION pg_catalog.atan2 (boolean, float8)  RETURNS float8 AS $$ SELECT pg_catalog.atan2($1::int4, $2) $$ LANGUAGE SQL;
DROP FUNCTION IF EXISTS pg_catalog.atan2 (float8, boolean);
CREATE OR REPLACE FUNCTION pg_catalog.atan2 (float8, boolean)  RETURNS float8 AS $$ SELECT pg_catalog.atan2($1, $2::int4) $$ LANGUAGE SQL;
DROP FUNCTION IF EXISTS pg_catalog.atan (float8, float8);
CREATE OR REPLACE FUNCTION pg_catalog.atan (float8, float8)  RETURNS float8 AS $$ SELECT pg_catalog.atan2($1, $2) $$ LANGUAGE SQL;
DROP FUNCTION IF EXISTS pg_catalog.atan (boolean, boolean);
CREATE OR REPLACE FUNCTION pg_catalog.atan (boolean, boolean)  RETURNS float8 AS $$ SELECT pg_catalog.atan2($1::int4, $2::int4) $$ LANGUAGE SQL;
DROP FUNCTION IF EXISTS pg_catalog.atan (boolean, float8);
CREATE OR REPLACE FUNCTION pg_catalog.atan (boolean, float8)  RETURNS float8 AS $$ SELECT pg_catalog.atan2($1::int4, $2) $$ LANGUAGE SQL;
DROP FUNCTION IF EXISTS pg_catalog.atan (float8, boolean);
CREATE OR REPLACE FUNCTION pg_catalog.atan (float8, boolean)  RETURNS float8 AS $$ SELECT pg_catalog.atan2($1, $2::int4) $$ LANGUAGE SQL;
DROP FUNCTION IF EXISTS pg_catalog.atan (boolean);
CREATE OR REPLACE FUNCTION pg_catalog.atan (boolean)  RETURNS float8 AS $$ SELECT pg_catalog.atan($1::int4) $$ LANGUAGE SQL;

drop CAST IF EXISTS (timestamptz as boolean);
drop CAST IF EXISTS (timestamp(0) without time zone as boolean);
DROP FUNCTION IF EXISTS pg_catalog.timestamptz_bool(timestamptz);
DROP FUNCTION IF EXISTS pg_catalog.timestamp_bool(timestamp(0) without time zone); 
CREATE OR REPLACE FUNCTION pg_catalog.timestamptz_bool(timestamptz) returns boolean LANGUAGE C immutable strict as '$libdir/dolphin', 'timestamptz_bool';
CREATE CAST (timestamptz as boolean) WITH FUNCTION timestamptz_bool(timestamptz) AS ASSIGNMENT;

CREATE OR REPLACE FUNCTION pg_catalog.timestamp_bool(timestamp(0) without time zone) returns boolean LANGUAGE C immutable strict as '$libdir/dolphin', 'timestamp_bool';
CREATE CAST (timestamp(0) without time zone as boolean) WITH FUNCTION timestamp_bool(timestamp(0) without time zone) AS ASSIGNMENT;

--change bit -> int/bigint castcontext from 'e' to 'a'
do $$
begin
    update pg_cast set castcontext='a', castowner=10 where castsource=1560 and casttarget=20 and castcontext='e';
    update pg_cast set castcontext='a', castowner=10 where castsource=1560 and casttarget=23 and castcontext='e';
    update pg_cast set castcontext='a', castowner=10 where castsource=20 and casttarget=1560 and castcontext='e';
    update pg_cast set castcontext='a', castowner=10 where castsource=23 and casttarget=1560 and castcontext='e';
end
$$;
drop CAST IF EXISTS (uint4 AS bit);
drop CAST IF EXISTS (uint8 AS bit);
CREATE CAST (uint4 AS bit) WITH FUNCTION bitfromuint4(uint4, int4) AS ASSIGNMENT;
CREATE CAST (uint8 AS bit) WITH FUNCTION bitfromuint8(uint8, int4) AS ASSIGNMENT;

CREATE OR REPLACE FUNCTION pg_catalog.bit_length(boolean) RETURNS integer LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.bit_length(cast($1 as TEXT))';
CREATE OR REPLACE FUNCTION pg_catalog.bit_length(year) RETURNS integer LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.bit_length(cast($1 as TEXT))';
CREATE OR REPLACE FUNCTION pg_catalog.bit_length(blob) RETURNS integer LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.bit_length(cast($1 as TEXT))';
CREATE OR REPLACE FUNCTION pg_catalog.bit_length(anyenum) RETURNS integer LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.bit_length(cast($1 as TEXT))';
CREATE OR REPLACE FUNCTION pg_catalog.bit_length(json) RETURNS integer LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.bit_length(cast($1 as TEXT))';
CREATE OR REPLACE FUNCTION pg_catalog.bit_length(binary) RETURNS integer LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'mp_bit_length_binary';

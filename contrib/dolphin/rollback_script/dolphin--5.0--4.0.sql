-- for dolphin-4.1 to dolphin-4.0 begin
CREATE OR REPLACE FUNCTION pg_catalog.float8_sum(float8, float8) RETURNS float8 LANGUAGE C AS  '$libdir/dolphin',  'float8_sum';
CREATE OR REPLACE FUNCTION pg_catalog.float_sum(double precision, float4) RETURNS double precision LANGUAGE SQL IMMUTABLE as $$ SELECT pg_catalog.float8_sum($1, $2::float8) $$;
CREATE OR REPLACE FUNCTION pg_catalog.tinyint_sum(numeric, tinyint) RETURNS numeric LANGUAGE SQL IMMUTABLE as $$ SELECT pg_catalog.int8_sum($1, $2::int8) $$;
CREATE OR REPLACE FUNCTION pg_catalog.smallint_sum_ext(numeric, smallint) RETURNS numeric LANGUAGE SQL IMMUTABLE as $$ SELECT pg_catalog.int8_sum($1, $2::int8) $$;
CREATE OR REPLACE FUNCTION pg_catalog.int_sum_ext(numeric, int) RETURNS numeric LANGUAGE SQL IMMUTABLE as $$ SELECT pg_catalog.int8_sum($1, $2::int8) $$;
CREATE OR REPLACE FUNCTION pg_catalog.tinyint_sum(numeric, year) RETURNS numeric LANGUAGE SQL IMMUTABLE as $$ SELECT pg_catalog.int8_sum($1, $2::int8) $$;
CREATE OR REPLACE FUNCTION pg_catalog.text_sum(double precision, text) RETURNS double precision LANGUAGE SQL IMMUTABLE as $$ SELECT pg_catalog.float8_sum($1, $2::float8) $$;
CREATE OR REPLACE FUNCTION pg_catalog.anyset_sum(double precision, anyset) RETURNS double precision LANGUAGE SQL IMMUTABLE as $$ SELECT pg_catalog.float8_sum($1, $2::float8) $$;

drop aggregate pg_catalog.sum(year);
create aggregate pg_catalog.sum(year) (SFUNC=tinyint_sum, cFUNC=numeric_add, STYPE= numeric);
drop FUNCTION IF EXISTS pg_catalog.year_sum(numeric, year);

-- for dolphin-4.1 to dolphin-4.0 end

DROP FUNCTION IF EXISTS pg_catalog.replace(json, text, text);

DROP CAST IF EXISTS (UINT1 AS VARCHAR);
DROP FUNCTION IF EXISTS pg_catalog.TO_VARCHAR(UINT1);

DROP CAST IF EXISTS (UINT2 AS VARCHAR);
DROP FUNCTION IF EXISTS pg_catalog.TO_VARCHAR(UINT2);

DROP CAST IF EXISTS (UINT4 AS VARCHAR);
DROP FUNCTION IF EXISTS pg_catalog.TO_VARCHAR(UINT4);

DROP CAST IF EXISTS (UINT8 AS VARCHAR);
DROP FUNCTION IF EXISTS pg_catalog.TO_VARCHAR(UINT8);

-- left/right for text type
DROP FUNCTION IF EXISTS pg_catalog.left(text, text);
DROP FUNCTION IF EXISTS pg_catalog.right(text, text);

DROP FUNCTION IF EXISTS pg_catalog.left(varbinary, integer);
DROP FUNCTION IF EXISTS pg_catalog.left(binary, integer);
DROP FUNCTION IF EXISTS pg_catalog.left(bytea, integer);

-- max/min for bool type
DROP aggregate pg_catalog.max(boolean);
DROP aggregate pg_catalog.min(boolean);
DROP FUNCTION pg_catalog.bool_larger(boolean, boolean);
DROP FUNCTION pg_catalog.bool_smaller(boolean, boolean);
-- max/min for varchar type
DROP aggregate pg_catalog.max(varchar);
DROP aggregate pg_catalog.min(varchar);
DROP FUNCTION pg_catalog.varchar_larger(varchar, varchar);
DROP FUNCTION pg_catalog.varchar_smaller(varchar, varchar);
-- max/min for binary type
DROP aggregate pg_catalog.max(binary);
DROP aggregate pg_catalog.min(binary);
DROP FUNCTION pg_catalog.binary_larger(binary, binary);
DROP FUNCTION pg_catalog.binary_smaller(binary, binary);
-- max/min for varbinary type
DROP aggregate pg_catalog.max(varbinary);
DROP aggregate pg_catalog.min(varbinary);
DROP FUNCTION pg_catalog.varbinary_larger(varbinary, varbinary);
DROP FUNCTION pg_catalog.varbinary_smaller(varbinary, varbinary);

-- bool to set and enum
DROP CAST IF EXISTS (bool AS anyset) CASCADE;
DROP FUNCTION pg_catalog.set(bool, int4);
DROP FUNCTION pg_catalog.bool_enum(bool, int4, anyelement);

-- xor between bool and bit,time
DROP OPERATOR IF EXISTS pg_catalog.^(bool, bit);
DROP OPERATOR IF EXISTS pg_catalog.^(bit, bool);
DROP OPERATOR IF EXISTS pg_catalog.^(time without time zone, bool);
DROP OPERATOR IF EXISTS pg_catalog.^(bool, time without time zone);
DROP FUNCTION IF EXISTS pg_catalog.op_bool_bit_xor(bool, bit);
DROP FUNCTION IF EXISTS pg_catalog.op_bit_bool_xor(bit, bool);
DROP FUNCTION IF EXISTS pg_catalog.op_time_bool_xor(time, bool);
DROP FUNCTION IF EXISTS pg_catalog.op_bool_time_xor(bool, time);

-- reverse
DROP FUNCTION IF EXISTS pg_catalog.reverse(bool);

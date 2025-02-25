-- for dolphin-4.0 to dolphin-4.1 begin
CREATE OR REPLACE FUNCTION pg_catalog.float8_sum(float8, float8) RETURNS float8 LANGUAGE C IMMUTABLE AS '$libdir/dolphin',  'float8_sum';
CREATE OR REPLACE FUNCTION pg_catalog.float_sum(double precision, float4) RETURNS double precision LANGUAGE C IMMUTABLE AS '$libdir/dolphin',  'float_sum_ext';
CREATE OR REPLACE FUNCTION pg_catalog.tinyint_sum(numeric, tinyint) RETURNS numeric LANGUAGE C IMMUTABLE AS '$libdir/dolphin',  'tinyint_sum_ext';
CREATE OR REPLACE FUNCTION pg_catalog.smallint_sum_ext(numeric, smallint) RETURNS numeric LANGUAGE C IMMUTABLE AS '$libdir/dolphin',  'smallint_sum_ext';
CREATE OR REPLACE FUNCTION pg_catalog.int_sum_ext(numeric, int) RETURNS numeric LANGUAGE C IMMUTABLE AS '$libdir/dolphin',  'int_sum_ext';
CREATE OR REPLACE FUNCTION pg_catalog.year_sum(numeric, year) RETURNS numeric LANGUAGE C IMMUTABLE AS '$libdir/dolphin',  'year_sum_ext';
CREATE OR REPLACE FUNCTION pg_catalog.text_sum(double precision, text) RETURNS double precision LANGUAGE C IMMUTABLE AS '$libdir/dolphin',  'text_sum_ext';
CREATE OR REPLACE FUNCTION pg_catalog.anyset_sum(double precision, anyset) RETURNS double precision LANGUAGE C IMMUTABLE AS '$libdir/dolphin',  'set_sum_ext';

drop aggregate pg_catalog.sum(year);
drop FUNCTION IF EXISTS pg_catalog.tinyint_sum(numeric, year);
CREATE OR REPLACE FUNCTION pg_catalog.year_sum(numeric, year) RETURNS numeric LANGUAGE C IMMUTABLE AS '$libdir/dolphin',  'year_sum_ext';
create aggregate pg_catalog.sum(year) (SFUNC=year_sum, cFUNC=numeric_add, STYPE= numeric);

-- for dolphin-4.0 to dolphin-4.1 end

CREATE or replace FUNCTION pg_catalog.replace(json, text, text) RETURNS text LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.replace($1::text, $2, $3)';

CREATE OR REPLACE FUNCTION pg_catalog.TO_VARCHAR(UINT1)
RETURNS VARCHAR
AS $$ select CAST(uint1out($1) AS VARCHAR)  $$
LANGUAGE SQL  STRICT IMMUTABLE;
CREATE CAST (UINT1 AS VARCHAR) WITH FUNCTION TO_VARCHAR(UINT1) AS IMPLICIT;

CREATE OR REPLACE FUNCTION pg_catalog.TO_VARCHAR(uint2)
RETURNS VARCHAR
AS $$ select CAST(uint2out($1) AS VARCHAR)  $$
LANGUAGE SQL  STRICT IMMUTABLE;
CREATE CAST (uint2 AS VARCHAR) WITH FUNCTION TO_VARCHAR(uint2) AS IMPLICIT;

CREATE OR REPLACE FUNCTION pg_catalog.TO_VARCHAR(uint4)
RETURNS VARCHAR
AS $$ select CAST(uint4out($1) AS VARCHAR)  $$
LANGUAGE SQL  STRICT IMMUTABLE;
CREATE CAST (uint4 AS VARCHAR) WITH FUNCTION TO_VARCHAR(uint4) AS IMPLICIT;

CREATE OR REPLACE FUNCTION pg_catalog.TO_VARCHAR(uint8)
RETURNS VARCHAR
AS $$ select CAST(uint8out($1) AS VARCHAR)  $$
LANGUAGE SQL  STRICT IMMUTABLE;
CREATE CAST (uint8 AS VARCHAR) WITH FUNCTION TO_VARCHAR(uint8) AS IMPLICIT;

-- left/right for text type
DROP FUNCTION IF EXISTS pg_catalog.left(text, text);
CREATE OR REPLACE FUNCTION pg_catalog.left(text, text) RETURNS text LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'text_left_text';
DROP FUNCTION IF EXISTS pg_catalog.right(text, text);
CREATE OR REPLACE FUNCTION pg_catalog.right(text, text) RETURNS text LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'text_right_text';

CREATE OR REPLACE FUNCTION pg_catalog.left(bytea, integer) RETURNS bytea LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'bytea_left';
CREATE OR REPLACE FUNCTION pg_catalog.left(binary, integer) RETURNS varbinary(65535) LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.left($1::bytea, $2)::varbinary(65535)';
CREATE OR REPLACE FUNCTION pg_catalog.left(varbinary, integer) RETURNS varbinary(65535) LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.left($1::bytea, $2)::varbinary(65535)';

CREATE OR REPLACE FUNCTION pg_catalog.left(bit, boolean) RETURNS bytea LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.left($1, $2::integer)';

-- max/min for bool type
CREATE OR REPLACE FUNCTION pg_catalog.bool_larger(boolean, boolean) RETURNS boolean LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin','bool_larger';
CREATE OR REPLACE FUNCTION pg_catalog.bool_smaller(boolean, boolean) RETURNS boolean LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin','bool_smaller';
create aggregate pg_catalog.max(boolean) (SFUNC=pg_catalog.bool_larger, STYPE=boolean);
create aggregate pg_catalog.min(boolean) (SFUNC=pg_catalog.bool_smaller, STYPE=boolean);

-- max/min for varchar type
CREATE OR REPLACE FUNCTION pg_catalog.varchar_larger(varchar, varchar) RETURNS varchar LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin','varchar_larger';
CREATE OR REPLACE FUNCTION pg_catalog.varchar_smaller(varchar, varchar) RETURNS varchar LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin','varchar_smaller';
create aggregate pg_catalog.max(varchar) (SFUNC=pg_catalog.varchar_larger, STYPE=varchar);
create aggregate pg_catalog.min(varchar) (SFUNC=pg_catalog.varchar_smaller, STYPE=varchar);

-- max/min for binary/varbinary type
CREATE OR REPLACE FUNCTION pg_catalog.binary_larger(binary, binary) RETURNS binary LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin','varlena_larger';
CREATE OR REPLACE FUNCTION pg_catalog.binary_smaller(binary, binary) RETURNS binary LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin','varlena_smaller';
create aggregate pg_catalog.max(binary) (SFUNC=pg_catalog.binary_larger, STYPE=binary);
create aggregate pg_catalog.min(binary) (SFUNC=pg_catalog.binary_smaller, STYPE=binary);
CREATE OR REPLACE FUNCTION pg_catalog.varbinary_larger(varbinary, varbinary) RETURNS varbinary LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin','varlena_larger';
CREATE OR REPLACE FUNCTION pg_catalog.varbinary_smaller(varbinary, varbinary) RETURNS varbinary LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin','varlena_smaller';
create aggregate pg_catalog.max(varbinary) (SFUNC=pg_catalog.varbinary_larger, STYPE=varbinary);
create aggregate pg_catalog.min(varbinary) (SFUNC=pg_catalog.varbinary_smaller, STYPE=varbinary);


-- bool to set and enum
CREATE OR REPLACE FUNCTION pg_catalog.bool_enum(bool, int4, anyelement) RETURNS anyenum LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'bool_enum';

CREATE OR REPLACE FUNCTION pg_catalog.set(bool, int4) RETURNS anyset LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'booltoset';
CREATE CAST (bool AS anyset) WITH FUNCTION pg_catalog.set(bool, int4) AS ASSIGNMENT;

-- xor between bool and bit,time
CREATE OR REPLACE FUNCTION pg_catalog.op_bool_bit_xor(bool, bit) returns uint8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.int8xor($1::int8, $2::int8)::uint8';
CREATE OR REPLACE FUNCTION pg_catalog.op_bit_bool_xor(bit, bool) returns uint8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.int8xor($1::int8, $2::int8)::uint8';
CREATE OPERATOR pg_catalog.^ (leftarg = bool, rightarg = bit, procedure = pg_catalog.op_bool_bit_xor);
CREATE OPERATOR pg_catalog.^ (leftarg = bit, rightarg = bool, procedure = pg_catalog.op_bit_bool_xor);

CREATE OR REPLACE FUNCTION pg_catalog.op_time_bool_xor(time, bool) returns uint8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.time_int8_xor($1, $2::uint8)::uint8';
CREATE OR REPLACE FUNCTION pg_catalog.op_bool_time_xor(bool, time) returns uint8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.int8_time_xor($1::uint8, $2)::uint8';
CREATE OPERATOR pg_catalog.^ (leftarg = time without time zone, rightarg = bool, procedure = pg_catalog.op_time_bool_xor);
CREATE OPERATOR pg_catalog.^ (leftarg = bool, rightarg = time without time zone, procedure = pg_catalog.op_bool_time_xor);

-- reverse
CREATE OR REPLACE FUNCTION pg_catalog.reverse(bool) RETURNS varchar LANGUAGE SQL IMMUTABLE STRICT as $$ SELECT pg_catalog.reverse($1::text)::varchar $$;
 
CREATE FUNCTION pg_catalog.int16_text(int16) RETURNS text LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int16_text';
CREATE CAST(int16 AS text) WITH FUNCTION pg_catalog.int16_text(int16)   AS IMPLICIT;

-- nvchar to uint
CREATE OR REPLACE FUNCTION pg_catalog.nvarchar2_to_uint1(nvarchar2) RETURNS uint1 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'nvarchar2_to_uint1';
CREATE CAST (nvarchar2 AS uint1) WITH FUNCTION pg_catalog.nvarchar2_to_uint1(nvarchar2) AS ASSIGNMENT;

CREATE OR REPLACE FUNCTION pg_catalog.nvarchar2_to_uint2(nvarchar2) RETURNS uint2 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'nvarchar2_to_uint2';
CREATE CAST (nvarchar2 AS uint2) WITH FUNCTION pg_catalog.nvarchar2_to_uint2(nvarchar2) AS ASSIGNMENT;

CREATE OR REPLACE FUNCTION pg_catalog.nvarchar2_to_uint4(nvarchar2) RETURNS uint4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'nvarchar2_to_uint4';
CREATE CAST (nvarchar2 AS uint4) WITH FUNCTION pg_catalog.nvarchar2_to_uint4(nvarchar2) AS ASSIGNMENT;

CREATE OR REPLACE FUNCTION pg_catalog.nvarchar2_to_uint8(nvarchar2) RETURNS uint8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'nvarchar2_to_uint8';
CREATE CAST (nvarchar2 AS uint8) WITH FUNCTION pg_catalog.nvarchar2_to_uint8(nvarchar2) AS ASSIGNMENT;


--date function
DROP FUNCTION IF EXISTS pg_catalog.subdate(time, int8);
DROP FUNCTION IF EXISTS pg_catalog.adddate(time, int8);
CREATE OR REPLACE FUNCTION pg_catalog.subdate (time, int8) RETURNS text AS $$ SELECT pg_catalog.date_sub($1, $2::text::interval)  $$ LANGUAGE SQL;
CREATE OR REPLACE FUNCTION pg_catalog.adddate (time, int8) RETURNS text AS $$ SELECT pg_catalog.date_add($1, $2::text::interval)  $$ LANGUAGE SQL;

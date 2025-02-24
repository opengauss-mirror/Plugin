CREATE OR REPLACE FUNCTION pg_catalog.float8_sum(float8, float8) RETURNS float8 LANGUAGE C IMMUTABLE AS '$libdir/dolphin',  'float8_sum';
CREATE OR REPLACE FUNCTION pg_catalog.float_sum(double precision, float4) RETURNS double precision LANGUAGE C IMMUTABLE AS '$libdir/dolphin',  'float_sum_ext';
CREATE OR REPLACE FUNCTION pg_catalog.tinyint_sum(numeric, tinyint) RETURNS numeric LANGUAGE C IMMUTABLE AS '$libdir/dolphin',  'tinyint_sum_ext';
CREATE OR REPLACE FUNCTION pg_catalog.smallint_sum_ext(numeric, smallint) RETURNS numeric LANGUAGE C IMMUTABLE AS '$libdir/dolphin',  'smallint_sum_ext';
CREATE OR REPLACE FUNCTION pg_catalog.int_sum_ext(numeric, int) RETURNS numeric LANGUAGE C IMMUTABLE AS '$libdir/dolphin',  'int_sum_ext';
CREATE OR REPLACE FUNCTION pg_catalog.year_sum(numeric, year) RETURNS numeric LANGUAGE C IMMUTABLE AS '$libdir/dolphin',  'year_sum_ext';
CREATE OR REPLACE FUNCTION pg_catalog.text_sum(double precision, text) RETURNS double precision LANGUAGE C IMMUTABLE AS '$libdir/dolphin',  'text_sum_ext';
CREATE OR REPLACE FUNCTION pg_catalog.anyset_sum(double precision, anyset) RETURNS double precision LANGUAGE C IMMUTABLE AS '$libdir/dolphin',  'set_sum_ext';

drop aggregate pg_catalog.sum(year);
create aggregate pg_catalog.sum(year) (SFUNC=year_sum, cFUNC=numeric_add, STYPE= numeric);
drop FUNCTION IF EXISTS pg_catalog.tinyint_sum(numeric, year);

-- xor between bool and bit,time
CREATE OR REPLACE FUNCTION pg_catalog.op_bool_bit_xor(bool, bit) returns uint8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.int8xor($1::int8, $2::int8)::uint8';
CREATE OR REPLACE FUNCTION pg_catalog.op_bit_bool_xor(bit, bool) returns uint8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.int8xor($1::int8, $2::int8)::uint8';
CREATE OPERATOR pg_catalog.^ (leftarg = bool, rightarg = bit, procedure = pg_catalog.op_bool_bit_xor);
CREATE OPERATOR pg_catalog.^ (leftarg = bit, rightarg = bool, procedure = pg_catalog.op_bit_bool_xor);

CREATE OR REPLACE FUNCTION pg_catalog.op_time_bool_xor(time, bool) returns uint8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.time_int8_xor($1, $2::uint8)::uint8';
CREATE OR REPLACE FUNCTION pg_catalog.op_bool_time_xor(bool, time) returns uint8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.int8_time_xor($1::uint8, $2)::uint8';
CREATE OPERATOR pg_catalog.^ (leftarg = time without time zone, rightarg = bool, procedure = pg_catalog.op_time_bool_xor);
CREATE OPERATOR pg_catalog.^ (leftarg = bool, rightarg = time without time zone, procedure = pg_catalog.op_bool_time_xor);

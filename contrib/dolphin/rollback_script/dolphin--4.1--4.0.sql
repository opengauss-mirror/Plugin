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

-- xor between bool and bit,time
DROP OPERATOR IF EXISTS pg_catalog.^(bool, bit);
DROP OPERATOR IF EXISTS pg_catalog.^(bit, bool);
DROP OPERATOR IF EXISTS pg_catalog.^(time without time zone, bool);
DROP OPERATOR IF EXISTS pg_catalog.^(bool, time without time zone);
DROP FUNCTION IF EXISTS pg_catalog.op_bool_bit_xor(bool, bit);
DROP FUNCTION IF EXISTS pg_catalog.op_bit_bool_xor(bit, bool);
DROP FUNCTION IF EXISTS pg_catalog.op_time_bool_xor(time, bool);
DROP FUNCTION IF EXISTS pg_catalog.op_bool_time_xor(bool, time);

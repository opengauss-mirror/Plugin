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

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
create aggregate pg_catalog.sum(year) (SFUNC=year_sum, cFUNC=numeric_add, STYPE= numeric);
drop FUNCTION IF EXISTS pg_catalog.tinyint_sum(numeric, year);

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

--CREATE TYPE
DROP TYPE IF EXISTS pg_catalog.year CASCADE;

DROP TYPE IF EXISTS pg_catalog._year CASCADE;

CREATE TYPE pg_catalog.year;

--CREATE YEAR'S BASIC FUNCTION

CREATE OR REPLACE FUNCTION pg_catalog.year_in (cstring) RETURNS year LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'year_in';

CREATE OR REPLACE FUNCTION pg_catalog.year_out (year) RETURNS cstring LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'year_out';

CREATE OR REPLACE FUNCTION pg_catalog.year_send (year) RETURNS bytea LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'year_send';

CREATE OR REPLACE FUNCTION pg_catalog.year_recv (bytea) RETURNS year LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'year_recv';

CREATE OR REPLACE FUNCTION pg_catalog.yeartypmodin (cstring[]) RETURNS integer LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'yeartypmodin';

CREATE OR REPLACE FUNCTION pg_catalog.yeartypmodout (integer) RETURNS cstring LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'yeartypmodout';

--COMPLETE YEAR'S TYPE

CREATE TYPE pg_catalog.year (input=year_in, output=year_out, internallength=2, passedbyvalue, alignment=int2, TYPMOD_IN=yeartypmodin, TYPMOD_OUT=yeartypmodout);

--CREATE YEAR'S COMPARATION FUNCTION

CREATE OR REPLACE FUNCTION pg_catalog.year_eq (year, year) RETURNS boolean LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'year_eq';

CREATE OR REPLACE FUNCTION pg_catalog.year_ne (year, year) RETURNS boolean LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'year_ne';

CREATE OR REPLACE FUNCTION pg_catalog.year_le (year, year) RETURNS boolean LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'year_le';

CREATE OR REPLACE FUNCTION pg_catalog.year_lt (year, year) RETURNS boolean LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'year_lt';

CREATE OR REPLACE FUNCTION pg_catalog.year_ge (year, year) RETURNS boolean LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'year_ge';

CREATE OR REPLACE FUNCTION pg_catalog.year_gt (year, year) RETURNS boolean LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'year_gt';

--CREATE YEAR'S CALCULATION FUNCTION

CREATE OR REPLACE FUNCTION pg_catalog.year_pl_interval (year, interval) RETURNS year LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'year_pl_interval';

CREATE OR REPLACE FUNCTION pg_catalog.year_mi_interval (year, interval) RETURNS year LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'year_mi_interval';

CREATE OR REPLACE FUNCTION pg_catalog.year_mi (year, year) RETURNS interval LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'year_mi';

CREATE OPERATOR - (
   leftarg = year,
   rightarg = year,
   procedure = year_mi,
   commutator = -
);

-- YEAR AND INTERVAL 

CREATE OPERATOR + (
   leftarg = year,
   rightarg = interval,
   procedure = year_pl_interval,
   commutator = +
);

CREATE OPERATOR - (
   leftarg = year,
   rightarg = interval,
   procedure = year_mi_interval,
   commutator = -
);

CREATE OR REPLACE FUNCTION pg_catalog.interval_pl_year (interval, year) RETURNS year AS $$ SELECT $2 + $1  $$ LANGUAGE SQL;

CREATE OPERATOR + (
   leftarg = interval,
   rightarg = year,
   procedure = interval_pl_year,
   commutator = +
);

--CREATE YEAR'S B-TREE SUPPORT FUNCTION

CREATE OR REPLACE FUNCTION pg_catalog.year_cmp (year, year) RETURNS integer LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'year_cmp';

CREATE OR REPLACE FUNCTION pg_catalog.year_sortsupport (internal) RETURNS void LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'year_sortsupport';

--CREATE YEAR'S CAST FUNCTION

CREATE OR REPLACE FUNCTION pg_catalog.int32_year (integer) RETURNS year LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'int32_year';

CREATE OR REPLACE FUNCTION pg_catalog.year_integer (year) RETURNS integer LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'year_integer';

CREATE FUNCTION pg_catalog.year (year, integer) RETURNS year LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'year_scale';

DROP CAST IF EXISTS (year AS year) CASCADE;

CREATE CAST(year AS year) WITH FUNCTION year(year, integer) AS IMPLICIT;

DROP CAST IF EXISTS (integer AS year) CASCADE;

CREATE CAST(integer AS year) WITH FUNCTION int32_year(integer) AS IMPLICIT;

DROP CAST IF EXISTS (year AS integer) CASCADE;

CREATE CAST(year AS integer) WITH FUNCTION year_integer(year) AS IMPLICIT;

--CREATE OPERATOR

CREATE OPERATOR pg_catalog.=(leftarg = year, rightarg = year, procedure = year_eq, restrict = eqsel, join = eqjoinsel, MERGES);

CREATE OPERATOR pg_catalog.<>(leftarg = year, rightarg = year, procedure = year_ne, restrict = neqsel, join = neqjoinsel);

CREATE OPERATOR pg_catalog.<=(leftarg = year, rightarg = year, procedure = year_le, COMMUTATOR  = >=, NEGATOR  = >, restrict = scalarltsel, join = scalarltjoinsel);

CREATE OPERATOR pg_catalog.<(leftarg = year, rightarg = year, procedure = year_lt, COMMUTATOR  = >, NEGATOR  = >=, restrict = scalarltsel, join = scalarltjoinsel);

CREATE OPERATOR pg_catalog.>=(leftarg = year, rightarg = year, procedure = year_ge, COMMUTATOR  = <=, NEGATOR  = <, restrict = scalarltsel, join = scalarltjoinsel);

CREATE OPERATOR pg_catalog.>(leftarg = year, rightarg = year, procedure = year_gt, COMMUTATOR  = <, NEGATOR  = <=, restrict = scalarltsel, join = scalarltjoinsel);

CREATE OPERATOR CLASS year_ops
    DEFAULT FOR TYPE year USING btree AS
        OPERATOR        1       < ,
        OPERATOR        2       <= ,
        OPERATOR        3       = ,
        OPERATOR        4       >= ,
        OPERATOR        5       > ,
        FUNCTION        1       year_cmp(year, year),
        FUNCTION        2       year_sortsupport(internal);

--CREATE YEAR'S MAX,MIN FUNCTION

CREATE OR REPLACE FUNCTION pg_catalog.year_larger (year, year) RETURNS year LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'year_larger';

CREATE OR REPLACE FUNCTION pg_catalog.year_smaller (year, year) RETURNS year LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'year_smaller';

CREATE AGGREGATE pg_catalog.max(year) (

SFUNC = year_larger,

STYPE = year,

SORTOP = >

);

CREATE AGGREGATE pg_catalog.min(year) (

SFUNC = year_smaller,

STYPE = year,

SORTOP = <

);

--CREATE DATE'S CAST FUNCTION

CREATE OR REPLACE FUNCTION pg_catalog.int32_b_format_date (int4) RETURNS date LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'int32_b_format_date';

DROP CAST IF EXISTS (int4 AS date) CASCADE;

CREATE CAST(int4 AS date) WITH FUNCTION int32_b_format_date(int4);

--CREATE TIME'S CAST FUNCTION

CREATE OR REPLACE FUNCTION pg_catalog.int32_b_format_time (int4) RETURNS time LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'int32_b_format_time';

CREATE OR REPLACE FUNCTION pg_catalog.numeric_b_format_time (numeric) RETURNS time LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'numeric_b_format_time';

DROP CAST IF EXISTS (int4 AS time) CASCADE;

CREATE CAST(int4 AS time) WITH FUNCTION int32_b_format_time(int4);

DROP CAST IF EXISTS (numeric AS time) CASCADE;

CREATE CAST(numeric AS time) WITH FUNCTION numeric_b_format_time(numeric);

--CREATE DATETIME'S CAST FUNCTION

CREATE OR REPLACE FUNCTION pg_catalog.int32_b_format_datetime (int4) RETURNS datetime LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'int32_b_format_datetime';

CREATE OR REPLACE FUNCTION pg_catalog.int64_b_format_datetime (int8) RETURNS datetime LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'int64_b_format_datetime';

CREATE OR REPLACE FUNCTION pg_catalog.numeric_b_format_datetime (numeric) RETURNS datetime LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'numeric_b_format_datetime';

DROP CAST IF EXISTS (int4 AS datetime) CASCADE;

CREATE CAST(int4 AS datetime) WITH FUNCTION int32_b_format_datetime(int4);

DROP CAST IF EXISTS (int8 AS datetime) CASCADE;

CREATE CAST(int8 AS datetime) WITH FUNCTION int64_b_format_datetime(int8);

DROP CAST IF EXISTS (numeric AS datetime) CASCADE;

CREATE CAST(numeric AS datetime) WITH FUNCTION numeric_b_format_datetime(numeric);

--CREATE DATETIME'S YEAR PART FUNCTION

CREATE FUNCTION pg_catalog.year (datetime) RETURNS float8 LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'datetime_year_part';

--CREATE TIMESTAMP'S CAST FUNCTION

CREATE OR REPLACE FUNCTION pg_catalog.int32_b_format_timestamp (int4) RETURNS timestamptz LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'int32_b_format_timestamp';

CREATE OR REPLACE FUNCTION pg_catalog.int64_b_format_timestamp (int8) RETURNS timestamptz LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'int64_b_format_timestamp';

CREATE OR REPLACE FUNCTION pg_catalog.numeric_b_format_timestamp (numeric) RETURNS timestamptz LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'numeric_b_format_timestamp';

DROP CAST IF EXISTS (int4 AS timestamptz) CASCADE;

CREATE CAST(int4 AS timestamptz) WITH FUNCTION int32_b_format_timestamp(int4);

DROP CAST IF EXISTS (int8 AS timestamptz) CASCADE;

CREATE CAST(int8 AS timestamptz) WITH FUNCTION int64_b_format_timestamp(int8);

DROP CAST IF EXISTS (numeric AS timestamptz) CASCADE;

CREATE CAST(numeric AS timestamptz) WITH FUNCTION numeric_b_format_timestamp(numeric);


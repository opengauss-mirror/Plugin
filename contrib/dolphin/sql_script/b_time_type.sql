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

CREATE TYPE pg_catalog.datetime;

--CREATE datetime'S BASIC FUNCTION

CREATE OR REPLACE FUNCTION pg_catalog.datetime_in (cstring) RETURNS datetime LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'datetime_in';

CREATE OR REPLACE FUNCTION pg_catalog.datetime_out (datetime) RETURNS cstring LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'datetime_out';

CREATE OR REPLACE FUNCTION pg_catalog.datetime_send (datetime) RETURNS bytea LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'datetime_send';

CREATE OR REPLACE FUNCTION pg_catalog.datetime_recv (bytea) RETURNS datetime LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'datetime_recv';

CREATE OR REPLACE FUNCTION pg_catalog.datetimetypmodin (cstring[]) RETURNS integer LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'datetimetypmodin';

CREATE OR REPLACE FUNCTION pg_catalog.datetimetypmodout (integer) RETURNS cstring LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'datetimetypmodout';

CREATE TYPE pg_catalog.datetime (input=datetime_in, output=datetime_out, internallength=8, passedbyvalue, alignment=double, TYPMOD_IN=datetimetypmodin, TYPMOD_OUT=datetimetypmodout);

--CREATE DATETIME'S COMPARATION FUNCTION

CREATE OR REPLACE FUNCTION pg_catalog.datetime_eq (datetime, datetime) RETURNS boolean LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'datetime_eq';

CREATE OR REPLACE FUNCTION pg_catalog.datetime_ne (datetime, datetime) RETURNS boolean LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'datetime_ne';

CREATE OR REPLACE FUNCTION pg_catalog.datetime_le (datetime, datetime) RETURNS boolean LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'datetime_le';

CREATE OR REPLACE FUNCTION pg_catalog.datetime_lt (datetime, datetime) RETURNS boolean LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'datetime_lt';

CREATE OR REPLACE FUNCTION pg_catalog.datetime_ge (datetime, datetime) RETURNS boolean LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'datetime_ge';

CREATE OR REPLACE FUNCTION pg_catalog.datetime_gt (datetime, datetime) RETURNS boolean LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'datetime_gt';

--CREATE DATETIME'S B-TREE SUPPORT FUNCTION

CREATE OR REPLACE FUNCTION pg_catalog.datetime_cmp (datetime, datetime) RETURNS integer LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'datetime_cmp';

CREATE OR REPLACE FUNCTION pg_catalog.datetime_sortsupport (internal) RETURNS void LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'datetime_sortsupport';

--CREATE OPERATOR

CREATE OPERATOR pg_catalog.=(leftarg = datetime, rightarg = datetime, procedure = datetime_eq, restrict = eqsel, join = eqjoinsel, MERGES);

CREATE OPERATOR pg_catalog.<>(leftarg = datetime, rightarg = datetime, procedure = datetime_ne, restrict = neqsel, join = neqjoinsel);

CREATE OPERATOR pg_catalog.<=(leftarg = datetime, rightarg = datetime, procedure = datetime_le, COMMUTATOR  = >=, NEGATOR  = >, restrict = scalarltsel, join = scalarltjoinsel);

CREATE OPERATOR pg_catalog.<(leftarg = datetime, rightarg = datetime, procedure = datetime_lt, COMMUTATOR  = >, NEGATOR  = >=, restrict = scalarltsel, join = scalarltjoinsel);

CREATE OPERATOR pg_catalog.>=(leftarg = datetime, rightarg = datetime, procedure = datetime_ge, COMMUTATOR  = <=, NEGATOR  = <, restrict = scalarltsel, join = scalarltjoinsel);

CREATE OPERATOR pg_catalog.>(leftarg = datetime, rightarg = datetime, procedure = datetime_gt, COMMUTATOR  = <, NEGATOR  = <=, restrict = scalarltsel, join = scalarltjoinsel);

CREATE OPERATOR CLASS datetime_ops
    DEFAULT FOR TYPE datetime USING btree AS
        OPERATOR        1       < ,
        OPERATOR        2       <= ,
        OPERATOR        3       = ,
        OPERATOR        4       >= ,
        OPERATOR        5       > ,
        FUNCTION        1       datetime_cmp(datetime, datetime),
        FUNCTION        2       datetime_sortsupport(internal);

--CREATE DATETIME'S MAX,MIN FUNCTION

CREATE OR REPLACE FUNCTION pg_catalog.datetime_larger (datetime, datetime) RETURNS datetime LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'datetime_larger';

CREATE OR REPLACE FUNCTION pg_catalog.datetime_smaller (datetime, datetime) RETURNS datetime LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'datetime_smaller';

CREATE AGGREGATE pg_catalog.max(datetime) (

SFUNC = datetime_larger,

STYPE = datetime,

SORTOP = >

);

CREATE AGGREGATE pg_catalog.min(datetime) (

SFUNC = datetime_smaller,

STYPE = datetime,

SORTOP = <

);

--CREATE DATETIME'S CAST FUNCTION

CREATE OR REPLACE FUNCTION pg_catalog.datetime (datetime, integer) RETURNS datetime LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'datetime_scale';

DROP CAST IF EXISTS (datetime AS datetime) CASCADE;

CREATE CAST(datetime AS datetime) WITH FUNCTION datetime(datetime, integer) AS IMPLICIT;

CREATE OR REPLACE FUNCTION pg_catalog.int32_b_format_datetime (int4) RETURNS datetime LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'int32_b_format_datetime';

CREATE OR REPLACE FUNCTION pg_catalog.int64_b_format_datetime (int8) RETURNS datetime LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'int64_b_format_datetime';

CREATE OR REPLACE FUNCTION pg_catalog.numeric_b_format_datetime (numeric) RETURNS datetime LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'numeric_b_format_datetime';

CREATE OR REPLACE FUNCTION pg_catalog.timestamptz_datetime (timestamptz) RETURNS datetime LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'timestamptz_datetime';

CREATE OR REPLACE FUNCTION pg_catalog.timestamp_datetime (timestamp) RETURNS datetime LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'timestamp_datetime';

DROP CAST IF EXISTS (int4 AS datetime) CASCADE;

CREATE CAST(int4 AS datetime) WITH FUNCTION int32_b_format_datetime(int4);

DROP CAST IF EXISTS (int8 AS datetime) CASCADE;

CREATE CAST(int8 AS datetime) WITH FUNCTION int64_b_format_datetime(int8);

DROP CAST IF EXISTS (numeric AS datetime) CASCADE;

CREATE CAST(numeric AS datetime) WITH FUNCTION numeric_b_format_datetime(numeric);

DROP CAST IF EXISTS (timestamptz AS datetime) CASCADE;

CREATE CAST(timestamptz AS datetime) WITH FUNCTION timestamptz_datetime(timestamptz) AS IMPLICIT;

DROP CAST IF EXISTS (timestamp AS datetime) CASCADE;

CREATE CAST(timestamp AS datetime) WITH FUNCTION timestamp_datetime(timestamp) AS IMPLICIT;


--CREATE DATETIME'S DATE_PART FUNCTION

CREATE FUNCTION pg_catalog.date_part (text, datetime) RETURNS float8 LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'datetime_part';

--CREATE YEAR'S DATE_PART FUNCTION

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


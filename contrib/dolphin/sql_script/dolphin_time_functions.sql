DROP FUNCTION IF EXISTS pg_catalog.b_db_sys_real_timestamp(integer) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.b_db_sys_real_timestamp(integer) returns datetime LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'b_db_sys_real_timestamp';

DROP FUNCTION IF EXISTS pg_catalog.b_db_statement_start_time(integer) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.b_db_statement_start_time(integer) returns time LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'b_db_statement_start_time';

DROP FUNCTION IF EXISTS pg_catalog.b_db_statement_start_timestamp(integer) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.b_db_statement_start_timestamp(integer) returns datetime LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'b_db_statement_start_timestamp';

DROP FUNCTION IF EXISTS pg_catalog.curdate() CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.curdate() returns date LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'curdate';

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

CREATE CAST(int4 AS date) WITH FUNCTION int32_b_format_date(int4) AS IMPLICIT;

--CREATE TIME'S CAST FUNCTION

CREATE OR REPLACE FUNCTION pg_catalog.int32_b_format_time (int4) RETURNS time LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'int32_b_format_time';

CREATE OR REPLACE FUNCTION pg_catalog.numeric_b_format_time (numeric) RETURNS time LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'numeric_b_format_time';

DROP CAST IF EXISTS (int4 AS time) CASCADE;

CREATE CAST(int4 AS time) WITH FUNCTION int32_b_format_time(int4) AS IMPLICIT;

DROP CAST IF EXISTS (numeric AS time) CASCADE;

CREATE CAST(numeric AS time) WITH FUNCTION numeric_b_format_time(numeric) AS IMPLICIT;

--CREATE DATETIME'S CAST FUNCTION

CREATE OR REPLACE FUNCTION pg_catalog.int32_b_format_datetime (int4) RETURNS datetime LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'int32_b_format_datetime';

CREATE OR REPLACE FUNCTION pg_catalog.int64_b_format_datetime (int8) RETURNS datetime LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'int64_b_format_datetime';

CREATE OR REPLACE FUNCTION pg_catalog.numeric_b_format_datetime (numeric) RETURNS datetime LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'numeric_b_format_datetime';

DROP CAST IF EXISTS (int4 AS datetime) CASCADE;

CREATE CAST(int4 AS datetime) WITH FUNCTION int32_b_format_datetime(int4) AS IMPLICIT;

DROP CAST IF EXISTS (int8 AS datetime) CASCADE;

CREATE CAST(int8 AS datetime) WITH FUNCTION int64_b_format_datetime(int8) AS IMPLICIT;

DROP CAST IF EXISTS (numeric AS datetime) CASCADE;

CREATE CAST(numeric AS datetime) WITH FUNCTION numeric_b_format_datetime(numeric) AS IMPLICIT;

--CREATE DATETIME'S YEAR PART FUNCTION

CREATE FUNCTION pg_catalog.year (datetime) RETURNS float8 LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'datetime_year_part';
CREATE FUNCTION pg_catalog.year (text) RETURNS float8 LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'text_year_part';

--CREATE TIMESTAMP'S CAST FUNCTION

CREATE OR REPLACE FUNCTION pg_catalog.int32_b_format_timestamp (int4) RETURNS timestamptz LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'int32_b_format_timestamp';

CREATE OR REPLACE FUNCTION pg_catalog.int64_b_format_timestamp (int8) RETURNS timestamptz LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'int64_b_format_timestamp';

CREATE OR REPLACE FUNCTION pg_catalog.numeric_b_format_timestamp (numeric) RETURNS timestamptz LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'numeric_b_format_timestamp';

DROP CAST IF EXISTS (int4 AS timestamptz) CASCADE;

CREATE CAST(int4 AS timestamptz) WITH FUNCTION int32_b_format_timestamp(int4) AS IMPLICIT;

DROP CAST IF EXISTS (int8 AS timestamptz) CASCADE;

CREATE CAST(int8 AS timestamptz) WITH FUNCTION int64_b_format_timestamp(int8) AS IMPLICIT;

DROP CAST IF EXISTS (numeric AS timestamptz) CASCADE;

CREATE CAST(numeric AS timestamptz) WITH FUNCTION numeric_b_format_timestamp(numeric) AS IMPLICIT;

--CREATE TIME'S NEGETIVE UNRAY OPERATOR

CREATE OR REPLACE FUNCTION pg_catalog.negetive_time (time) RETURNS time LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'negetive_time';

--OTHER TIME FUNCTIONS

-- CREATE b compatibility time function
CREATE OR REPLACE FUNCTION pg_catalog.makedate (int8, int8) RETURNS date LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'makedate';
CREATE OR REPLACE FUNCTION pg_catalog.makedate (bit, bit) RETURNS date AS $$ SELECT pg_catalog.makedate(cast($1 as int8), cast($2 as int8)) $$ LANGUAGE SQL;
CREATE OR REPLACE FUNCTION pg_catalog.makedate (bit, int8) RETURNS date AS $$ SELECT pg_catalog.makedate(cast($1 as int8), cast($2 as int8)) $$ LANGUAGE SQL;
CREATE OR REPLACE FUNCTION pg_catalog.makedate (int8, bit) RETURNS date AS $$ SELECT pg_catalog.makedate(cast($1 as int8), cast($2 as int8)) $$ LANGUAGE SQL;

CREATE OR REPLACE FUNCTION pg_catalog.maketime (int8, int8, numeric) RETURNS time LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'maketime';
CREATE OR REPLACE FUNCTION pg_catalog.maketime (bit, bit, numeric) RETURNS time AS $$ SELECT pg_catalog.maketime(cast($1 as int8), cast($2 as int8), $3) $$ LANGUAGE SQL;
CREATE OR REPLACE FUNCTION pg_catalog.maketime (bit, int8, numeric) RETURNS time AS $$ SELECT pg_catalog.maketime(cast($1 as int8), cast($2 as int8), $3) $$ LANGUAGE SQL;
CREATE OR REPLACE FUNCTION pg_catalog.maketime (int8, bit, numeric) RETURNS time AS $$ SELECT pg_catalog.maketime(cast($1 as int8), cast($2 as int8), $3) $$ LANGUAGE SQL;

CREATE OR REPLACE FUNCTION pg_catalog.sec_to_time (numeric) RETURNS time LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'sec_to_time';
CREATE OR REPLACE FUNCTION pg_catalog.sec_to_time (text) RETURNS time LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'sec_to_time_str';

CREATE OR REPLACE FUNCTION pg_catalog.subdate (text, int8) RETURNS text LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'subdate_datetime_days_text';
CREATE OR REPLACE FUNCTION pg_catalog.subdate (text, text) RETURNS text AS $$ SELECT pg_catalog.subdate($1, cast($2 as int8))  $$ LANGUAGE SQL;
CREATE OR REPLACE FUNCTION pg_catalog.subdate (text, bit) RETURNS text AS $$ SELECT pg_catalog.subdate($1, cast($2 as int8))  $$ LANGUAGE SQL;
CREATE OR REPLACE FUNCTION pg_catalog.subdate (text, interval) RETURNS text LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'subdate_datetime_interval_text';
CREATE OR REPLACE FUNCTION pg_catalog.subdate (numeric(24,6), int8) RETURNS text AS $$ SELECT pg_catalog.subdate(cast($1 as text), cast($2 as int8))  $$ LANGUAGE SQL;
CREATE OR REPLACE FUNCTION pg_catalog.subdate (numeric(24,6), interval) RETURNS text AS $$ SELECT pg_catalog.subdate(cast($1 as text), $2)  $$ LANGUAGE SQL;
CREATE OR REPLACE FUNCTION pg_catalog.subdate (time, int8) RETURNS time LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'subdate_time_days';
CREATE OR REPLACE FUNCTION pg_catalog.subdate (time, interval) RETURNS time LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'subdate_time_interval';

CREATE OR REPLACE FUNCTION pg_catalog.period_add (int8, int8) RETURNS int8 LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'period_add';
CREATE OR REPLACE FUNCTION pg_catalog.period_add (bit, bit) RETURNS int8 AS $$ SELECT pg_catalog.period_add(cast($1 as int8), cast($2 as int8)) $$ LANGUAGE SQL;
CREATE OR REPLACE FUNCTION pg_catalog.period_add (bit, int8) RETURNS int8 AS $$ SELECT pg_catalog.period_add(cast($1 as int8), cast($2 as int8)) $$ LANGUAGE SQL;
CREATE OR REPLACE FUNCTION pg_catalog.period_add (int8, bit) RETURNS int8 AS $$ SELECT pg_catalog.period_add(cast($1 as int8), cast($2 as int8)) $$ LANGUAGE SQL;

CREATE OR REPLACE FUNCTION pg_catalog.period_diff (int8, int8) RETURNS int8 LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'period_diff';
CREATE OR REPLACE FUNCTION pg_catalog.period_diff (bit, bit) RETURNS int8 AS $$ SELECT pg_catalog.period_diff(cast($1 as int8), cast($2 as int8)) $$ LANGUAGE SQL;
CREATE OR REPLACE FUNCTION pg_catalog.period_diff (bit, int8) RETURNS int8 AS $$ SELECT pg_catalog.period_diff(cast($1 as int8), cast($2 as int8)) $$ LANGUAGE SQL;
CREATE OR REPLACE FUNCTION pg_catalog.period_diff (int8, bit) RETURNS int8 AS $$ SELECT pg_catalog.period_diff(cast($1 as int8), cast($2 as int8)) $$ LANGUAGE SQL;

CREATE OR REPLACE FUNCTION pg_catalog.subtime (text, text) RETURNS TEXT LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'subtime_text';
CREATE OR REPLACE FUNCTION pg_catalog.subtime (date, text) RETURNS TEXT LANGUAGE SQL STABLE STRICT as 'select pg_catalog.subtime(cast(0 as time), $2)';
CREATE OR REPLACE FUNCTION pg_catalog.subtime (date, datetime) RETURNS TEXT LANGUAGE SQL STABLE STRICT as 'select pg_catalog.subtime(cast(0 as time), cast($2 as time))';
CREATE OR REPLACE FUNCTION pg_catalog.subtime (date, date) RETURNS TEXT LANGUAGE SQL STABLE STRICT as 'select pg_catalog.subtime(cast(0 as time), cast(0 as time))';
CREATE OR REPLACE FUNCTION pg_catalog.subtime (text, datetime) RETURNS TEXT LANGUAGE SQL STABLE STRICT as 'select pg_catalog.subtime($1, cast($2 as time))';
CREATE OR REPLACE FUNCTION pg_catalog.subtime (text, date) RETURNS TEXT LANGUAGE SQL STABLE STRICT as 'select pg_catalog.subtime($1, cast(0 as time))';
CREATE OR REPLACE FUNCTION pg_catalog.subtime (numeric, numeric) RETURNS TEXT LANGUAGE SQL STABLE STRICT as 'select pg_catalog.subtime(cast($1 as text), cast($2 as text))';
CREATE OR REPLACE FUNCTION pg_catalog.subtime (text, numeric) RETURNS TEXT LANGUAGE SQL STABLE STRICT as 'select pg_catalog.subtime($1, cast($2 as text))';
CREATE OR REPLACE FUNCTION pg_catalog.subtime (numeric, text) RETURNS TEXT LANGUAGE SQL STABLE STRICT as 'select pg_catalog.subtime(cast($1 as text), $2)';

CREATE OR REPLACE FUNCTION pg_catalog.timediff (text, text) RETURNS time LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'timediff_text';
CREATE OR REPLACE FUNCTION pg_catalog.timediff (datetime, text) RETURNS time LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'timediff_datetime_text';
CREATE OR REPLACE FUNCTION pg_catalog.timediff (time, text) RETURNS time LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'timediff_time_text';
CREATE OR REPLACE FUNCTION pg_catalog.timediff (datetime, time) RETURNS time LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'func_return_null';
CREATE OR REPLACE FUNCTION pg_catalog.timediff (time, date) RETURNS time LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'func_return_null';
CREATE OR REPLACE FUNCTION pg_catalog.timediff (date, text) RETURNS time LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'timediff_date_text';
CREATE OR REPLACE FUNCTION pg_catalog.timediff (date, time) RETURNS time LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'func_return_null';
CREATE OR REPLACE FUNCTION pg_catalog.timediff (numeric, numeric) RETURNS time LANGUAGE SQL STABLE STRICT as 'select pg_catalog.timediff(cast($1 as text), cast($2 as text))';
CREATE OR REPLACE FUNCTION pg_catalog.timediff (text, numeric) RETURNS time LANGUAGE SQL STABLE STRICT as 'select pg_catalog.timediff($1, cast($2 as text))';
CREATE OR REPLACE FUNCTION pg_catalog.timediff (numeric, text) RETURNS time LANGUAGE SQL STABLE STRICT as 'select pg_catalog.timediff(cast($1 as text), $2)';

CREATE OR REPLACE FUNCTION pg_catalog.time_format (text, text) RETURNS text LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'time_format';
CREATE OR REPLACE FUNCTION pg_catalog.time_format (date, text) RETURNS TEXT AS $$ SELECT pg_catalog.time_format('00:00:00', $2) $$ LANGUAGE SQL;
CREATE OR REPLACE FUNCTION pg_catalog.time_format (numeric, text) RETURNS TEXT AS $$ SELECT pg_catalog.time_format(cast($1 as text), $2)  $$ LANGUAGE SQL;

CREATE OR REPLACE FUNCTION pg_catalog.time_mysql (text) RETURNS time LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'time_mysql';
CREATE OR REPLACE FUNCTION pg_catalog.time_mysql (numeric) RETURNS time AS $$ SELECT pg_catalog.time_mysql(cast($1 as text))  $$ LANGUAGE SQL;
CREATE OR REPLACE FUNCTION pg_catalog.time_mysql (date) RETURNS time AS $$ SELECT pg_catalog.time_mysql('00:00:00')  $$ LANGUAGE SQL;

CREATE OR REPLACE FUNCTION pg_catalog.timestamp_mysql ("any") RETURNS datetime LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'timestamp_param1';
CREATE OR REPLACE FUNCTION pg_catalog.timestamp_mysql ("any", "any") RETURNS datetime LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'timestamp_param2';

CREATE OR REPLACE FUNCTION pg_catalog.timestamp_add (text, numeric, text) RETURNS text LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'timestamp_add_text';
CREATE OR REPLACE FUNCTION pg_catalog.timestamp_add (text, numeric, time) RETURNS time LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'timestamp_add_time';
CREATE OR REPLACE FUNCTION pg_catalog.timestamp_add (text, numeric, numeric) RETURNS text AS $$ SELECT pg_catalog.timestamp_add($1, $2, CAST($3 AS text)) $$ LANGUAGE SQL;

CREATE OR REPLACE FUNCTION pg_catalog.to_days (datetime) RETURNS int8 LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'to_days';
CREATE OR REPLACE FUNCTION pg_catalog.to_days (time) RETURNS int8 AS $$ SELECT pg_catalog.to_days(text_date('now') + $1)  $$ LANGUAGE SQL;

CREATE OR REPLACE FUNCTION pg_catalog.to_seconds ("any") RETURNS numeric LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'to_seconds';

CREATE OR REPLACE FUNCTION pg_catalog.unix_timestamp () RETURNS numeric LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'unix_timestamp_no_args';
CREATE OR REPLACE FUNCTION pg_catalog.unix_timestamp ("any") RETURNS numeric LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'unix_timestamp';

CREATE OR REPLACE FUNCTION pg_catalog.utc_timestamp_func (int4) RETURNS datetime LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'utc_timestamp_func';
CREATE OR REPLACE FUNCTION pg_catalog.utc_date_func () RETURNS date LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'utc_date_func';
CREATE OR REPLACE FUNCTION pg_catalog.utc_time_func (int4) RETURNS time LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'utc_time_func';

CREATE OPERATOR pg_catalog.- (
   rightarg = time,
   procedure = negetive_time
);

CREATE OR REPLACE FUNCTION pg_catalog.dayname(text) RETURNS text LANGUAGE C STABLE RETURNS NULL ON NULL INPUT as '$libdir/dolphin', 'dayname_text';
CREATE OR REPLACE FUNCTION pg_catalog.dayname(numeric) RETURNS text LANGUAGE C STABLE RETURNS NULL ON NULL INPUT as '$libdir/dolphin', 'dayname_numeric';

CREATE OR REPLACE FUNCTION pg_catalog.monthname(text) RETURNS text LANGUAGE C STABLE RETURNS NULL ON NULL INPUT as '$libdir/dolphin', 'monthname_text';
CREATE OR REPLACE FUNCTION pg_catalog.monthname(numeric) RETURNS text LANGUAGE C STABLE RETURNS NULL ON NULL INPUT as '$libdir/dolphin', 'monthname_numeric';

CREATE OR REPLACE FUNCTION pg_catalog.time_to_sec(text) RETURNS int4 LANGUAGE C STABLE RETURNS NULL ON NULL INPUT as '$libdir/dolphin', 'time_to_sec';
CREATE OR REPLACE FUNCTION pg_catalog.time_to_sec(date) RETURNS int4 AS $$ SELECT pg_catalog.time_to_sec(cast('00:00:00' as text)) $$ LANGUAGE SQL;
CREATE OR REPLACE FUNCTION pg_catalog.time_to_sec(numeric) RETURNS int4 AS $$ SELECT pg_catalog.time_to_sec(cast($1 as text)) $$ LANGUAGE SQL;

CREATE OR REPLACE FUNCTION pg_catalog.month(text) RETURNS int4 LANGUAGE C STABLE RETURNS NULL ON NULL INPUT as '$libdir/dolphin', 'month_text';
CREATE OR REPLACE FUNCTION pg_catalog.month(numeric) RETURNS int4 LANGUAGE C STABLE RETURNS NULL ON NULL INPUT as '$libdir/dolphin', 'month_numeric';

CREATE OR REPLACE FUNCTION pg_catalog.b_db_last_day(text) RETURNS date LANGUAGE C STABLE RETURNS NULL ON NULL INPUT as '$libdir/dolphin', 'last_day_text';
CREATE OR REPLACE FUNCTION pg_catalog.b_db_last_day(numeric) RETURNS date LANGUAGE C STABLE RETURNS NULL ON NULL INPUT as '$libdir/dolphin', 'last_day_numeric';

CREATE OR REPLACE FUNCTION pg_catalog.b_db_date(text) RETURNS text LANGUAGE C STABLE RETURNS NULL ON NULL INPUT as '$libdir/dolphin', 'b_db_date_text';
CREATE OR REPLACE FUNCTION pg_catalog.b_db_date(numeric) RETURNS text LANGUAGE C STABLE RETURNS NULL ON NULL INPUT as '$libdir/dolphin', 'b_db_date_numeric';

CREATE OR REPLACE FUNCTION pg_catalog.day(text) RETURNS int4 LANGUAGE C STABLE RETURNS NULL ON NULL INPUT as '$libdir/dolphin', 'dayofmonth_text';
CREATE OR REPLACE FUNCTION pg_catalog.day(numeric) RETURNS int4 LANGUAGE C STABLE RETURNS NULL ON NULL INPUT as '$libdir/dolphin', 'dayofmonth_numeric';

CREATE OR REPLACE FUNCTION pg_catalog.week(text, int8) RETURNS int4 LANGUAGE C STABLE CALLED ON NULL INPUT as '$libdir/dolphin', 'week_text';
CREATE OR REPLACE FUNCTION pg_catalog.week(numeric, int8) RETURNS int4 LANGUAGE C STABLE CALLED ON NULL INPUT as '$libdir/dolphin', 'week_numeric';
CREATE OR REPLACE FUNCTION pg_catalog.week(text) RETURNS int4 AS $$ SELECT pg_catalog.week($1, null) $$ LANGUAGE SQL;
CREATE OR REPLACE FUNCTION pg_catalog.week(numeric) RETURNS int4 AS $$ SELECT pg_catalog.week($1, null) $$ LANGUAGE SQL;

CREATE OR REPLACE FUNCTION pg_catalog.yearweek(text, int8) RETURNS int8 LANGUAGE C STABLE CALLED ON NULL INPUT as '$libdir/dolphin', 'yearweek_text';
CREATE OR REPLACE FUNCTION pg_catalog.yearweek(numeric, int8) RETURNS int8 LANGUAGE C STABLE CALLED ON NULL INPUT as '$libdir/dolphin', 'yearweek_numeric';
CREATE OR REPLACE FUNCTION pg_catalog.yearweek(text) RETURNS int8 AS $$ SELECT pg_catalog.yearweek($1, null) $$ LANGUAGE SQL;
CREATE OR REPLACE FUNCTION pg_catalog.yearweek(numeric) RETURNS int8 AS $$ SELECT pg_catalog.yearweek($1, null) $$ LANGUAGE SQL;

CREATE OR REPLACE FUNCTION pg_catalog.datediff(text, text) RETURNS int4 LANGUAGE C STABLE RETURNS NULL ON NULL INPUT as '$libdir/dolphin', 'datediff';
CREATE OR REPLACE FUNCTION pg_catalog.datediff(text, numeric) RETURNS int4 AS $$ SELECT pg_catalog.datediff($1, cast($2 as text)) $$ LANGUAGE SQL;
CREATE OR REPLACE FUNCTION pg_catalog.datediff(numeric, text) RETURNS int4 AS $$ SELECT pg_catalog.datediff(cast($1 as text), $2) $$ LANGUAGE SQL;
CREATE OR REPLACE FUNCTION pg_catalog.datediff(numeric, numeric) RETURNS int4 AS $$ SELECT pg_catalog.datediff(cast($1 as text), cast($2 as text)) $$ LANGUAGE SQL;

CREATE OR REPLACE FUNCTION pg_catalog.from_days(numeric) RETURNS text LANGUAGE C STABLE RETURNS NULL ON NULL INPUT as '$libdir/dolphin', 'from_days_numeric';
CREATE OR REPLACE FUNCTION pg_catalog.from_days(text) RETURNS text LANGUAGE C STABLE RETURNS NULL ON NULL INPUT as '$libdir/dolphin', 'from_days_text';

CREATE OR REPLACE FUNCTION pg_catalog.b_timestampdiff(text,text,text) RETURNS int8 LANGUAGE C STABLE RETURNS NULL ON NULL INPUT as '$libdir/dolphin', 'timestampdiff_datetime';
CREATE OR REPLACE FUNCTION pg_catalog.b_timestampdiff(text,time,time) RETURNS int8 LANGUAGE C STABLE RETURNS NULL ON NULL INPUT as '$libdir/dolphin', 'timestampdiff_time';
CREATE OR REPLACE FUNCTION pg_catalog.b_timestampdiff(text,time,text) RETURNS int8 LANGUAGE C STABLE RETURNS NULL ON NULL INPUT as '$libdir/dolphin', 'timestampdiff_time_before';
CREATE OR REPLACE FUNCTION pg_catalog.b_timestampdiff(text,text,time) RETURNS int8 LANGUAGE C STABLE RETURNS NULL ON NULL INPUT as '$libdir/dolphin', 'timestampdiff_time_after';

CREATE OR REPLACE FUNCTION pg_catalog.convert_tz(text,text,text) RETURNS datetime LANGUAGE C STABLE RETURNS NULL ON NULL INPUT as '$libdir/dolphin', 'convert_tz';
CREATE OR REPLACE FUNCTION pg_catalog.convert_tz(numeric,text,text) RETURNS datetime AS $$ SELECT pg_catalog.convert_tz(cast($1 as text), $2, $3)  $$ LANGUAGE SQL;

CREATE OR REPLACE FUNCTION pg_catalog.adddate (text, int8) RETURNS text LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'adddate_datetime_days_text';
CREATE OR REPLACE FUNCTION pg_catalog.adddate (text, text) RETURNS text AS $$ SELECT pg_catalog.adddate($1, cast($2 as int8))  $$ LANGUAGE SQL;
CREATE OR REPLACE FUNCTION pg_catalog.adddate (text, bit) RETURNS text AS $$ SELECT pg_catalog.adddate($1, cast($2 as int8))  $$ LANGUAGE SQL;
CREATE OR REPLACE FUNCTION pg_catalog.adddate (text, interval) RETURNS text LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'adddate_datetime_interval_text';
CREATE OR REPLACE FUNCTION pg_catalog.adddate (numeric, int8) RETURNS text AS $$ SELECT pg_catalog.adddate(cast($1 as text), cast($2 as int8))  $$ LANGUAGE SQL;
CREATE OR REPLACE FUNCTION pg_catalog.adddate (numeric, interval) RETURNS text AS $$ SELECT pg_catalog.adddate(cast($1 as text), $2)  $$ LANGUAGE SQL;
CREATE OR REPLACE FUNCTION pg_catalog.adddate (time, int8) RETURNS time LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'adddate_time_days';
CREATE OR REPLACE FUNCTION pg_catalog.adddate (time, interval) RETURNS time LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'adddate_time_interval';

CREATE OR REPLACE FUNCTION pg_catalog.date_sub (text, interval) RETURNS text LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'subdate_datetime_interval_text';
CREATE OR REPLACE FUNCTION pg_catalog.date_sub (numeric, interval) RETURNS text AS $$ SELECT pg_catalog.date_sub(cast($1 as text), $2)  $$ LANGUAGE SQL;
CREATE OR REPLACE FUNCTION pg_catalog.date_sub (time, interval) RETURNS time LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'subdate_time_interval';

CREATE OR REPLACE FUNCTION pg_catalog.date_add (text, interval) RETURNS text AS $$ SELECT pg_catalog.adddate($1, $2)  $$ LANGUAGE SQL;
CREATE OR REPLACE FUNCTION pg_catalog.date_add (numeric, interval) RETURNS text AS $$ SELECT pg_catalog.adddate(cast($1 as text), $2)  $$ LANGUAGE SQL;
CREATE OR REPLACE FUNCTION pg_catalog.date_add (time, interval) RETURNS time AS $$ SELECT pg_catalog.adddate($1, $2)  $$ LANGUAGE SQL;

CREATE OR REPLACE FUNCTION pg_catalog.addtime (text, text) RETURNS TEXT LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'addtime_text';
CREATE OR REPLACE FUNCTION pg_catalog.addtime (date, text) RETURNS TEXT LANGUAGE SQL STABLE STRICT as 'select pg_catalog.addtime(cast(0 as time), $2)';
CREATE OR REPLACE FUNCTION pg_catalog.addtime (date, datetime) RETURNS TEXT LANGUAGE SQL STABLE STRICT as 'select pg_catalog.addtime(cast(0 as time), cast($2 as time))';
CREATE OR REPLACE FUNCTION pg_catalog.addtime (date, date) RETURNS TEXT LANGUAGE SQL STABLE STRICT as 'select pg_catalog.addtime(cast(0 as time), cast(0 as time))';
CREATE OR REPLACE FUNCTION pg_catalog.addtime (text, datetime) RETURNS TEXT LANGUAGE SQL STABLE STRICT as 'select pg_catalog.addtime($1, cast($2 as time))';
CREATE OR REPLACE FUNCTION pg_catalog.addtime (text, date) RETURNS TEXT LANGUAGE SQL STABLE STRICT as 'select pg_catalog.addtime($1, cast(0 as time))';
CREATE OR REPLACE FUNCTION pg_catalog.addtime (numeric, numeric) RETURNS TEXT LANGUAGE SQL STABLE STRICT as 'select pg_catalog.addtime(cast($1 as text), cast($2 as text))';
CREATE OR REPLACE FUNCTION pg_catalog.addtime (text, numeric) RETURNS TEXT LANGUAGE SQL STABLE STRICT as 'select pg_catalog.addtime($1, cast($2 as text))';
CREATE OR REPLACE FUNCTION pg_catalog.addtime (numeric, text) RETURNS TEXT LANGUAGE SQL STABLE STRICT as 'select pg_catalog.addtime(cast($1 as text), $2)';

CREATE OR REPLACE FUNCTION pg_catalog.hour (text) RETURNS float8 LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'GetHour';
CREATE OR REPLACE FUNCTION pg_catalog.microsecond (text) RETURNS float8 LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'GetMicrosecond';
CREATE OR REPLACE FUNCTION pg_catalog.minute (text) RETURNS float8 LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'GetMinute';
CREATE OR REPLACE FUNCTION pg_catalog.second (text) RETURNS float8 LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'GetSecond';

CREATE OR REPLACE FUNCTION pg_catalog.dayofmonth (timestamptz) RETURNS float8 LANGUAGE SQL STABLE STRICT as 'select pg_catalog.date_part(''day'', $1)';
CREATE OR REPLACE FUNCTION pg_catalog.dayofmonth (timetz) RETURNS float8 LANGUAGE SQL STABLE STRICT as 'select pg_catalog.date_part(''day'', $1)';
CREATE OR REPLACE FUNCTION pg_catalog.dayofmonth (abstime) RETURNS float8 LANGUAGE SQL STABLE STRICT as 'select pg_catalog.date_part(''day'', $1)';
CREATE OR REPLACE FUNCTION pg_catalog.dayofmonth (date) RETURNS float8 LANGUAGE SQL STABLE STRICT as 'select pg_catalog.date_part(''day'', $1)';
CREATE OR REPLACE FUNCTION pg_catalog.dayofmonth (time) RETURNS float8 LANGUAGE SQL STABLE STRICT as 'select pg_catalog.date_part(''day'', $1)';
CREATE OR REPLACE FUNCTION pg_catalog.dayofmonth (timestamp) RETURNS float8 LANGUAGE SQL STABLE STRICT as 'select pg_catalog.date_part(''day'', $1)';

CREATE OR REPLACE FUNCTION pg_catalog.dayofweek (timestamptz) RETURNS float8 LANGUAGE SQL STABLE STRICT as 'select 1 + pg_catalog.date_part(''dow'', $1)';
CREATE OR REPLACE FUNCTION pg_catalog.dayofweek (timetz) RETURNS float8 LANGUAGE SQL STABLE STRICT as 'select 1 + pg_catalog.date_part(''dow'', $1)';
CREATE OR REPLACE FUNCTION pg_catalog.dayofweek (abstime) RETURNS float8 LANGUAGE SQL STABLE STRICT as 'select 1 + pg_catalog.date_part(''dow'', $1)';
CREATE OR REPLACE FUNCTION pg_catalog.dayofweek (date) RETURNS float8 LANGUAGE SQL STABLE STRICT as 'select 1 + pg_catalog.date_part(''dow'', $1)';
CREATE OR REPLACE FUNCTION pg_catalog.dayofweek (time) RETURNS float8 LANGUAGE SQL STABLE STRICT as 'select 1 + pg_catalog.date_part(''dow'', $1)';
CREATE OR REPLACE FUNCTION pg_catalog.dayofweek (timestamp) RETURNS float8 LANGUAGE SQL STABLE STRICT as 'select 1 + pg_catalog.date_part(''dow'', $1)';

CREATE OR REPLACE FUNCTION pg_catalog.dayofyear (timestamptz) RETURNS float8 LANGUAGE SQL STABLE STRICT as 'select pg_catalog.date_part(''doy'', $1)';
CREATE OR REPLACE FUNCTION pg_catalog.dayofyear (timetz) RETURNS float8 LANGUAGE SQL STABLE STRICT as 'select pg_catalog.date_part(''doy'', $1)';
CREATE OR REPLACE FUNCTION pg_catalog.dayofyear (abstime) RETURNS float8 LANGUAGE SQL STABLE STRICT as 'select pg_catalog.date_part(''doy'', $1)';
CREATE OR REPLACE FUNCTION pg_catalog.dayofyear (date) RETURNS float8 LANGUAGE SQL STABLE STRICT as 'select pg_catalog.date_part(''doy'', $1)';
CREATE OR REPLACE FUNCTION pg_catalog.dayofyear (time) RETURNS float8 LANGUAGE SQL STABLE STRICT as 'select pg_catalog.date_part(''doy'', $1)';
CREATE OR REPLACE FUNCTION pg_catalog.dayofyear (timestamp) RETURNS float8 LANGUAGE SQL STABLE STRICT as 'select pg_catalog.date_part(''doy'', $1)';

CREATE OR REPLACE FUNCTION pg_catalog.quarter (timestamptz) RETURNS float8 LANGUAGE SQL STABLE STRICT as 'select pg_catalog.date_part(''quarter'', $1)';
CREATE OR REPLACE FUNCTION pg_catalog.quarter (timetz) RETURNS float8 LANGUAGE SQL STABLE STRICT as 'select pg_catalog.date_part(''quarter'', $1)';
CREATE OR REPLACE FUNCTION pg_catalog.quarter (abstime) RETURNS float8 LANGUAGE SQL STABLE STRICT as 'select pg_catalog.date_part(''quarter'', $1)';
CREATE OR REPLACE FUNCTION pg_catalog.quarter (date) RETURNS float8 LANGUAGE SQL STABLE STRICT as 'select pg_catalog.date_part(''quarter'', $1)';
CREATE OR REPLACE FUNCTION pg_catalog.quarter (time) RETURNS float8 LANGUAGE SQL STABLE STRICT as 'select pg_catalog.date_part(''quarter'', $1)';
CREATE OR REPLACE FUNCTION pg_catalog.quarter (timestamp) RETURNS float8 LANGUAGE SQL STABLE STRICT as 'select pg_catalog.date_part(''quarter'', $1)';

CREATE OR REPLACE FUNCTION pg_catalog.weekday (timestamptz) RETURNS float8 LANGUAGE SQL STABLE STRICT as 'select pg_catalog.date_part(''isodow'', $1) - 1';
CREATE OR REPLACE FUNCTION pg_catalog.weekday (timetz) RETURNS float8 LANGUAGE SQL STABLE STRICT as 'select pg_catalog.date_part(''isodow'', $1) - 1';
CREATE OR REPLACE FUNCTION pg_catalog.weekday (abstime) RETURNS float8 LANGUAGE SQL STABLE STRICT as 'select pg_catalog.date_part(''isodow'', $1) - 1';
CREATE OR REPLACE FUNCTION pg_catalog.weekday (date) RETURNS float8 LANGUAGE SQL STABLE STRICT as 'select pg_catalog.date_part(''isodow'', $1) - 1';
CREATE OR REPLACE FUNCTION pg_catalog.weekday (time) RETURNS float8 LANGUAGE SQL STABLE STRICT as 'select pg_catalog.date_part(''isodow'', $1) - 1';
CREATE OR REPLACE FUNCTION pg_catalog.weekday (timestamp) RETURNS float8 LANGUAGE SQL STABLE STRICT as 'select pg_catalog.date_part(''isodow'', $1) - 1';

CREATE OR REPLACE FUNCTION pg_catalog.weekofyear (timestamptz) RETURNS float8 LANGUAGE SQL STABLE STRICT as 'select pg_catalog.date_part(''week'', $1)';
CREATE OR REPLACE FUNCTION pg_catalog.weekofyear (timetz) RETURNS float8 LANGUAGE SQL STABLE STRICT as 'select pg_catalog.date_part(''week'', $1)';
CREATE OR REPLACE FUNCTION pg_catalog.weekofyear (abstime) RETURNS float8 LANGUAGE SQL STABLE STRICT as 'select pg_catalog.date_part(''week'', $1)';
CREATE OR REPLACE FUNCTION pg_catalog.weekofyear (date) RETURNS float8 LANGUAGE SQL STABLE STRICT as 'select pg_catalog.date_part(''week'', $1)';
CREATE OR REPLACE FUNCTION pg_catalog.weekofyear (time) RETURNS float8 LANGUAGE SQL STABLE STRICT as 'select pg_catalog.date_part(''week'', $1)';
CREATE OR REPLACE FUNCTION pg_catalog.weekofyear (timestamp) RETURNS float8 LANGUAGE SQL STABLE STRICT as 'select pg_catalog.date_part(''week'', $1)';

CREATE OR REPLACE FUNCTION pg_catalog.get_format (int4, text) RETURNS TEXT LANGUAGE C STABLE RETURNS NULL ON NULL INPUT as '$libdir/dolphin', 'get_format';

CREATE OR REPLACE FUNCTION pg_catalog.date_format (text, text) RETURNS TEXT LANGUAGE C STABLE RETURNS NULL ON NULL INPUT as '$libdir/dolphin', 'date_format_text';

CREATE OR REPLACE FUNCTION pg_catalog.b_extract (text, text) RETURNS int8 LANGUAGE C STABLE RETURNS NULL ON NULL INPUT as '$libdir/dolphin', 'b_extract';

CREATE OR REPLACE FUNCTION pg_catalog.str_to_date (text, text) RETURNS text LANGUAGE C STABLE CALLED ON NULL INPUT as '$libdir/dolphin', 'str_to_date';

CREATE OR REPLACE FUNCTION pg_catalog.from_unixtime (numeric) RETURNS datetime LANGUAGE C STABLE RETURNS NULL ON NULL INPUT as '$libdir/dolphin', 'from_unixtime_with_one_arg';
CREATE OR REPLACE FUNCTION pg_catalog.from_unixtime (numeric, text) RETURNS TEXT LANGUAGE C STABLE RETURNS NULL ON NULL INPUT as '$libdir/dolphin', 'from_unixtime_with_two_arg';

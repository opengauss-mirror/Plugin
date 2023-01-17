DROP FUNCTION IF EXISTS pg_catalog.b_db_sys_real_timestamp(integer) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.b_db_sys_real_timestamp(integer) returns timestamp(0) without time zone LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'b_db_sys_real_timestamp';

DROP FUNCTION IF EXISTS pg_catalog.b_db_statement_start_time(integer) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.b_db_statement_start_time(integer) returns time LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'b_db_statement_start_time';

DROP FUNCTION IF EXISTS pg_catalog.b_db_statement_start_timestamp(integer) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.b_db_statement_start_timestamp(integer) returns timestamp(0) without time zone LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'b_db_statement_start_timestamp';

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

CREATE OR REPLACE FUNCTION pg_catalog.int64_b_format_time (int8) RETURNS time LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'int64_b_format_time';

CREATE OR REPLACE FUNCTION pg_catalog.numeric_b_format_time (numeric) RETURNS time LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'numeric_b_format_time';

DROP CAST IF EXISTS (int4 AS time) CASCADE;

CREATE CAST(int4 AS time) WITH FUNCTION int32_b_format_time(int4) AS IMPLICIT;

DROP CAST IF EXISTS (int8 AS time) CASCADE;

CREATE CAST(int8 AS time) WITH FUNCTION int64_b_format_time(int8) AS IMPLICIT;

DROP CAST IF EXISTS (numeric AS time) CASCADE;

CREATE CAST(numeric AS time) WITH FUNCTION numeric_b_format_time(numeric) AS IMPLICIT;

--CREATE timestamp(0) without time zone'S CAST FUNCTION

CREATE OR REPLACE FUNCTION pg_catalog.int32_b_format_datetime (int4) RETURNS timestamp(0) without time zone LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'int32_b_format_datetime';

CREATE OR REPLACE FUNCTION pg_catalog.int64_b_format_datetime (int8) RETURNS timestamp(0) without time zone LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'int64_b_format_datetime';

CREATE OR REPLACE FUNCTION pg_catalog.numeric_b_format_datetime (numeric) RETURNS timestamp(0) without time zone LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'numeric_b_format_datetime';

DROP CAST IF EXISTS (int4 AS timestamp(0) without time zone) CASCADE;

CREATE CAST(int4 AS timestamp(0) without time zone) WITH FUNCTION int32_b_format_datetime(int4) AS IMPLICIT;

DROP CAST IF EXISTS (int8 AS timestamp(0) without time zone) CASCADE;

CREATE CAST(int8 AS timestamp(0) without time zone) WITH FUNCTION int64_b_format_datetime(int8) AS IMPLICIT;

DROP CAST IF EXISTS (numeric AS timestamp(0) without time zone) CASCADE;

CREATE CAST(numeric AS timestamp(0) without time zone) WITH FUNCTION numeric_b_format_datetime(numeric) AS IMPLICIT;

--CREATE timestamp(0) without time zone'S YEAR PART FUNCTION

CREATE FUNCTION pg_catalog.year (timestamp(0) without time zone) RETURNS float8 LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'datetime_year_part';
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

CREATE OR REPLACE FUNCTION pg_catalog.subtime ("any", "any") RETURNS TEXT LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'subtime';

CREATE OR REPLACE FUNCTION pg_catalog.timediff ("any", "any") RETURNS time LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'timediff';

CREATE OR REPLACE FUNCTION pg_catalog.time_format (text, text) RETURNS text LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'time_format';
CREATE OR REPLACE FUNCTION pg_catalog.time_format (date, text) RETURNS text LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'time_format_date';
CREATE OR REPLACE FUNCTION pg_catalog.time_format (numeric, text) RETURNS TEXT AS $$ SELECT pg_catalog.time_format(cast($1 as text), $2)  $$ LANGUAGE SQL;

CREATE OR REPLACE FUNCTION pg_catalog.time_mysql ("any") RETURNS time LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'time_mysql';

CREATE OR REPLACE FUNCTION pg_catalog.timestamp_mysql ("any") RETURNS timestamp(0) without time zone LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'timestamp_param1';
CREATE OR REPLACE FUNCTION pg_catalog.timestamp_mysql ("any", "any") RETURNS timestamp(0) without time zone LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'timestamp_param2';

CREATE OR REPLACE FUNCTION pg_catalog.timestamp_add (text, numeric, "any") RETURNS text LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'timestamp_add_numeric';
CREATE OR REPLACE FUNCTION pg_catalog.timestamp_add (text, text, "any") RETURNS text LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'timestamp_add_text';

CREATE OR REPLACE FUNCTION pg_catalog.to_days (timestamp(0) without time zone) RETURNS int8 LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'to_days';
CREATE OR REPLACE FUNCTION pg_catalog.to_days (time) RETURNS int8 AS $$ SELECT pg_catalog.to_days(text_date('now') + $1)  $$ LANGUAGE SQL;

CREATE OR REPLACE FUNCTION pg_catalog.to_seconds ("any") RETURNS numeric LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'to_seconds';

CREATE OR REPLACE FUNCTION pg_catalog.unix_timestamp () RETURNS numeric LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'unix_timestamp_no_args';
CREATE OR REPLACE FUNCTION pg_catalog.unix_timestamp ("any") RETURNS numeric LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'unix_timestamp';

CREATE OR REPLACE FUNCTION pg_catalog.utc_timestamp_func (int4) RETURNS timestamp(0) without time zone LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'utc_timestamp_func';
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

CREATE OR REPLACE FUNCTION pg_catalog.datediff(text, text) RETURNS int4 LANGUAGE C STABLE RETURNS NULL ON NULL INPUT as '$libdir/dolphin', 'datediff_t_t';
CREATE OR REPLACE FUNCTION pg_catalog.datediff(text, numeric) RETURNS int4 LANGUAGE C STABLE RETURNS NULL ON NULL INPUT as '$libdir/dolphin', 'datediff_t_n';
CREATE OR REPLACE FUNCTION pg_catalog.datediff(numeric, text) RETURNS int4 AS $$ SELECT -pg_catalog.datediff($2, $1) $$ LANGUAGE SQL;
CREATE OR REPLACE FUNCTION pg_catalog.datediff(numeric, numeric) RETURNS int4 LANGUAGE C STABLE RETURNS NULL ON NULL INPUT as '$libdir/dolphin', 'datediff_n_n';
CREATE OR REPLACE FUNCTION pg_catalog.datediff(time, text) RETURNS int4 LANGUAGE C STABLE RETURNS NULL ON NULL INPUT as '$libdir/dolphin', 'datediff_time_t';
CREATE OR REPLACE FUNCTION pg_catalog.datediff(time, numeric) RETURNS int4 LANGUAGE C STABLE RETURNS NULL ON NULL INPUT as '$libdir/dolphin', 'datediff_time_n';
CREATE OR REPLACE FUNCTION pg_catalog.datediff(time, time) RETURNS int4 LANGUAGE C STABLE RETURNS NULL ON NULL INPUT as '$libdir/dolphin', 'datediff_time_time';
CREATE OR REPLACE FUNCTION pg_catalog.datediff(text, time) RETURNS int4 AS $$ SELECT -pg_catalog.datediff($2, $1) $$ LANGUAGE SQL;
CREATE OR REPLACE FUNCTION pg_catalog.datediff(numeric, time) RETURNS int4 AS $$ SELECT -pg_catalog.datediff($2, $1) $$ LANGUAGE SQL;

CREATE OR REPLACE FUNCTION pg_catalog.from_days(numeric) RETURNS text LANGUAGE C STABLE RETURNS NULL ON NULL INPUT as '$libdir/dolphin', 'from_days_numeric';
CREATE OR REPLACE FUNCTION pg_catalog.from_days(text) RETURNS text LANGUAGE C STABLE RETURNS NULL ON NULL INPUT as '$libdir/dolphin', 'from_days_text';

CREATE OR REPLACE FUNCTION pg_catalog.b_timestampdiff(text,text,text) RETURNS int8 LANGUAGE C STABLE RETURNS NULL ON NULL INPUT as '$libdir/dolphin', 'timestampdiff_datetime_tt';
CREATE OR REPLACE FUNCTION pg_catalog.b_timestampdiff(text,numeric,numeric) RETURNS int8 LANGUAGE C STABLE RETURNS NULL ON NULL INPUT as '$libdir/dolphin', 'timestampdiff_datetime_nn';
CREATE OR REPLACE FUNCTION pg_catalog.b_timestampdiff(text,text,numeric) RETURNS int8 LANGUAGE C STABLE RETURNS NULL ON NULL INPUT as '$libdir/dolphin', 'timestampdiff_datetime_tn';
CREATE OR REPLACE FUNCTION pg_catalog.b_timestampdiff(text,numeric,text) RETURNS int8 AS $$ SELECT -pg_catalog.b_timestampdiff($1, $3, $2) $$ LANGUAGE SQL;
CREATE OR REPLACE FUNCTION pg_catalog.b_timestampdiff(text,time,time) RETURNS int8 LANGUAGE C STABLE RETURNS NULL ON NULL INPUT as '$libdir/dolphin', 'timestampdiff_time';
CREATE OR REPLACE FUNCTION pg_catalog.b_timestampdiff(text,time,text) RETURNS int8 LANGUAGE C STABLE RETURNS NULL ON NULL INPUT as '$libdir/dolphin', 'timestampdiff_time_before_t';
CREATE OR REPLACE FUNCTION pg_catalog.b_timestampdiff(text,time,numeric) RETURNS int8 LANGUAGE C STABLE RETURNS NULL ON NULL INPUT as '$libdir/dolphin', 'timestampdiff_time_before_n';
CREATE OR REPLACE FUNCTION pg_catalog.b_timestampdiff(text,text,time) RETURNS int8 AS $$ SELECT -pg_catalog.b_timestampdiff($1, $3, $2) $$ LANGUAGE SQL;
CREATE OR REPLACE FUNCTION pg_catalog.b_timestampdiff(text,numeric,time) RETURNS int8 AS $$ SELECT -pg_catalog.b_timestampdiff($1, $3, $2) $$ LANGUAGE SQL;

CREATE OR REPLACE FUNCTION pg_catalog.convert_tz(text,text,text) RETURNS timestamp(0) without time zone LANGUAGE C STABLE RETURNS NULL ON NULL INPUT as '$libdir/dolphin', 'convert_tz_t';
CREATE OR REPLACE FUNCTION pg_catalog.convert_tz(numeric,text,text) RETURNS timestamp(0) without time zone LANGUAGE C STABLE RETURNS NULL ON NULL INPUT as '$libdir/dolphin', 'convert_tz_n';
CREATE OR REPLACE FUNCTION pg_catalog.convert_tz(time,text,text) RETURNS timestamp(0) without time zone LANGUAGE C STABLE RETURNS NULL ON NULL INPUT as '$libdir/dolphin', 'convert_tz_time';

CREATE OR REPLACE FUNCTION pg_catalog.adddate (text, int8) RETURNS text LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'adddate_datetime_days_t';
CREATE OR REPLACE FUNCTION pg_catalog.adddate (text, text) RETURNS text AS $$ SELECT pg_catalog.adddate($1, cast($2 as int8))  $$ LANGUAGE SQL;
CREATE OR REPLACE FUNCTION pg_catalog.adddate (text, bit) RETURNS text AS $$ SELECT pg_catalog.adddate($1, cast($2 as int8))  $$ LANGUAGE SQL;
CREATE OR REPLACE FUNCTION pg_catalog.adddate (text, interval) RETURNS text LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'adddate_datetime_interval_t';
CREATE OR REPLACE FUNCTION pg_catalog.adddate (numeric, int8) RETURNS text LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'adddate_datetime_days_n';
CREATE OR REPLACE FUNCTION pg_catalog.adddate (numeric, interval) RETURNS text LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'adddate_datetime_interval_n';
CREATE OR REPLACE FUNCTION pg_catalog.adddate (time, int8) RETURNS time LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'adddate_time_days';
CREATE OR REPLACE FUNCTION pg_catalog.adddate (time, interval) RETURNS time LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'adddate_time_interval';

CREATE OR REPLACE FUNCTION pg_catalog.date_sub (text, interval) RETURNS text AS $$ SELECT pg_catalog.adddate($1, -$2)  $$ LANGUAGE SQL;
CREATE OR REPLACE FUNCTION pg_catalog.date_sub (numeric, interval) RETURNS text AS $$ SELECT pg_catalog.adddate($1, -$2)  $$ LANGUAGE SQL;
CREATE OR REPLACE FUNCTION pg_catalog.date_sub (time, interval) RETURNS time AS $$ SELECT pg_catalog.adddate($1, -$2)  $$ LANGUAGE SQL;

CREATE OR REPLACE FUNCTION pg_catalog.date_add (text, interval) RETURNS text AS $$ SELECT pg_catalog.adddate($1, $2)  $$ LANGUAGE SQL;
CREATE OR REPLACE FUNCTION pg_catalog.date_add (numeric, interval) RETURNS text AS $$ SELECT pg_catalog.adddate($1, $2)  $$ LANGUAGE SQL;
CREATE OR REPLACE FUNCTION pg_catalog.date_add (time, interval) RETURNS time AS $$ SELECT pg_catalog.adddate($1, $2)  $$ LANGUAGE SQL;

CREATE OR REPLACE FUNCTION pg_catalog.addtime (text, text) RETURNS TEXT LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'addtime_text';
CREATE OR REPLACE FUNCTION pg_catalog.addtime (date, text) RETURNS TEXT LANGUAGE SQL STABLE STRICT as 'select pg_catalog.addtime(cast(0 as time), $2)';
CREATE OR REPLACE FUNCTION pg_catalog.addtime (date, timestamp(0) without time zone) RETURNS TEXT LANGUAGE SQL STABLE STRICT as 'select pg_catalog.addtime(cast(0 as time), cast($2 as time))';
CREATE OR REPLACE FUNCTION pg_catalog.addtime (date, date) RETURNS TEXT LANGUAGE SQL STABLE STRICT as 'select pg_catalog.addtime(cast(0 as time), cast(0 as time))';
CREATE OR REPLACE FUNCTION pg_catalog.addtime (text, timestamp(0) without time zone) RETURNS TEXT LANGUAGE SQL STABLE STRICT as 'select pg_catalog.addtime($1, cast($2 as time))';
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
CREATE OR REPLACE FUNCTION pg_catalog.dayofmonth (timestamp(0) with time zone) RETURNS float8 LANGUAGE SQL STABLE STRICT as 'select pg_catalog.date_part(''day'', $1)';

CREATE OR REPLACE FUNCTION pg_catalog.dayofweek (timestamptz) RETURNS float8 LANGUAGE SQL STABLE STRICT as 'select 1 + pg_catalog.date_part(''dow'', $1)';
CREATE OR REPLACE FUNCTION pg_catalog.dayofweek (timetz) RETURNS float8 LANGUAGE SQL STABLE STRICT as 'select 1 + pg_catalog.date_part(''dow'', $1)';
CREATE OR REPLACE FUNCTION pg_catalog.dayofweek (abstime) RETURNS float8 LANGUAGE SQL STABLE STRICT as 'select 1 + pg_catalog.date_part(''dow'', $1)';
CREATE OR REPLACE FUNCTION pg_catalog.dayofweek (date) RETURNS float8 LANGUAGE SQL STABLE STRICT as 'select 1 + pg_catalog.date_part(''dow'', $1)';
CREATE OR REPLACE FUNCTION pg_catalog.dayofweek (time) RETURNS float8 LANGUAGE SQL STABLE STRICT as 'select 1 + pg_catalog.date_part(''dow'', $1)';
CREATE OR REPLACE FUNCTION pg_catalog.dayofweek (timestamp(0) with time zone) RETURNS float8 LANGUAGE SQL STABLE STRICT as 'select 1 + pg_catalog.date_part(''dow'', $1)';

CREATE OR REPLACE FUNCTION pg_catalog.dayofyear (timestamptz) RETURNS float8 LANGUAGE SQL STABLE STRICT as 'select pg_catalog.date_part(''doy'', $1)';
CREATE OR REPLACE FUNCTION pg_catalog.dayofyear (timetz) RETURNS float8 LANGUAGE SQL STABLE STRICT as 'select pg_catalog.date_part(''doy'', $1)';
CREATE OR REPLACE FUNCTION pg_catalog.dayofyear (abstime) RETURNS float8 LANGUAGE SQL STABLE STRICT as 'select pg_catalog.date_part(''doy'', $1)';
CREATE OR REPLACE FUNCTION pg_catalog.dayofyear (date) RETURNS float8 LANGUAGE SQL STABLE STRICT as 'select pg_catalog.date_part(''doy'', $1)';
CREATE OR REPLACE FUNCTION pg_catalog.dayofyear (time) RETURNS float8 LANGUAGE SQL STABLE STRICT as 'select pg_catalog.date_part(''doy'', $1)';
CREATE OR REPLACE FUNCTION pg_catalog.dayofyear (timestamp(0) with time zone) RETURNS float8 LANGUAGE SQL STABLE STRICT as 'select pg_catalog.date_part(''doy'', $1)';

CREATE OR REPLACE FUNCTION pg_catalog.quarter (timestamptz) RETURNS float8 LANGUAGE SQL STABLE STRICT as 'select pg_catalog.date_part(''quarter'', $1)';
CREATE OR REPLACE FUNCTION pg_catalog.quarter (timetz) RETURNS float8 LANGUAGE SQL STABLE STRICT as 'select pg_catalog.date_part(''quarter'', $1)';
CREATE OR REPLACE FUNCTION pg_catalog.quarter (abstime) RETURNS float8 LANGUAGE SQL STABLE STRICT as 'select pg_catalog.date_part(''quarter'', $1)';
CREATE OR REPLACE FUNCTION pg_catalog.quarter (date) RETURNS float8 LANGUAGE SQL STABLE STRICT as 'select pg_catalog.date_part(''quarter'', $1)';
CREATE OR REPLACE FUNCTION pg_catalog.quarter (time) RETURNS float8 LANGUAGE SQL STABLE STRICT as 'select pg_catalog.date_part(''quarter'', $1)';
CREATE OR REPLACE FUNCTION pg_catalog.quarter (timestamp(0) with time zone) RETURNS float8 LANGUAGE SQL STABLE STRICT as 'select pg_catalog.date_part(''quarter'', $1)';

CREATE OR REPLACE FUNCTION pg_catalog.weekday (timestamptz) RETURNS float8 LANGUAGE SQL STABLE STRICT as 'select pg_catalog.date_part(''isodow'', $1) - 1';
CREATE OR REPLACE FUNCTION pg_catalog.weekday (timetz) RETURNS float8 LANGUAGE SQL STABLE STRICT as 'select pg_catalog.date_part(''isodow'', $1) - 1';
CREATE OR REPLACE FUNCTION pg_catalog.weekday (abstime) RETURNS float8 LANGUAGE SQL STABLE STRICT as 'select pg_catalog.date_part(''isodow'', $1) - 1';
CREATE OR REPLACE FUNCTION pg_catalog.weekday (date) RETURNS float8 LANGUAGE SQL STABLE STRICT as 'select pg_catalog.date_part(''isodow'', $1) - 1';
CREATE OR REPLACE FUNCTION pg_catalog.weekday (time) RETURNS float8 LANGUAGE SQL STABLE STRICT as 'select pg_catalog.date_part(''isodow'', $1) - 1';
CREATE OR REPLACE FUNCTION pg_catalog.weekday (timestamp(0) with time zone) RETURNS float8 LANGUAGE SQL STABLE STRICT as 'select pg_catalog.date_part(''isodow'', $1) - 1';

CREATE OR REPLACE FUNCTION pg_catalog.weekofyear (timestamptz) RETURNS float8 LANGUAGE SQL STABLE STRICT as 'select pg_catalog.date_part(''week'', $1)';
CREATE OR REPLACE FUNCTION pg_catalog.weekofyear (timetz) RETURNS float8 LANGUAGE SQL STABLE STRICT as 'select pg_catalog.date_part(''week'', $1)';
CREATE OR REPLACE FUNCTION pg_catalog.weekofyear (abstime) RETURNS float8 LANGUAGE SQL STABLE STRICT as 'select pg_catalog.date_part(''week'', $1)';
CREATE OR REPLACE FUNCTION pg_catalog.weekofyear (date) RETURNS float8 LANGUAGE SQL STABLE STRICT as 'select pg_catalog.date_part(''week'', $1)';
CREATE OR REPLACE FUNCTION pg_catalog.weekofyear (time) RETURNS float8 LANGUAGE SQL STABLE STRICT as 'select pg_catalog.date_part(''week'', $1)';
CREATE OR REPLACE FUNCTION pg_catalog.weekofyear (timestamp(0) with time zone) RETURNS float8 LANGUAGE SQL STABLE STRICT as 'select pg_catalog.date_part(''week'', $1)';

CREATE OR REPLACE FUNCTION pg_catalog.get_format (int4, text) RETURNS TEXT LANGUAGE C STABLE RETURNS NULL ON NULL INPUT as '$libdir/dolphin', 'get_format';

CREATE OR REPLACE FUNCTION pg_catalog.date_format (text, text) RETURNS TEXT LANGUAGE C STABLE RETURNS NULL ON NULL INPUT as '$libdir/dolphin', 'date_format_text';
CREATE OR REPLACE FUNCTION pg_catalog.date_format (numeric, text) RETURNS TEXT LANGUAGE C STABLE RETURNS NULL ON NULL INPUT as '$libdir/dolphin', 'date_format_numeric';

CREATE OR REPLACE FUNCTION pg_catalog.b_extract (text, text) RETURNS int8 LANGUAGE C STABLE RETURNS NULL ON NULL INPUT as '$libdir/dolphin', 'b_extract_text';
CREATE OR REPLACE FUNCTION pg_catalog.b_extract (text, numeric) RETURNS int8 LANGUAGE C STABLE RETURNS NULL ON NULL INPUT as '$libdir/dolphin', 'b_extract_numeric';

CREATE OR REPLACE FUNCTION pg_catalog.str_to_date (text, text) RETURNS text LANGUAGE C STABLE CALLED ON NULL INPUT as '$libdir/dolphin', 'str_to_date';

CREATE OR REPLACE FUNCTION pg_catalog.from_unixtime (numeric) RETURNS timestamp(0) without time zone LANGUAGE C STABLE RETURNS NULL ON NULL INPUT as '$libdir/dolphin', 'from_unixtime_with_one_arg';
CREATE OR REPLACE FUNCTION pg_catalog.from_unixtime (numeric, text) RETURNS TEXT LANGUAGE C STABLE RETURNS NULL ON NULL INPUT as '$libdir/dolphin', 'from_unixtime_with_two_arg';


-- support bit_xor for date

DROP FUNCTION IF EXISTS pg_catalog.date_xor_transfn(int16, date) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.date_xor_transfn (
int16, date
) RETURNS int16 LANGUAGE C as '$libdir/dolphin',  'date_xor_transfn';

DROP FUNCTION IF EXISTS pg_catalog.date_agg_finalfn(int16) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.date_agg_finalfn (
int16
) RETURNS int16 LANGUAGE C as '$libdir/dolphin',  'date_agg_finalfn';

drop aggregate if exists pg_catalog.bit_xor(date);
create aggregate pg_catalog.bit_xor(date) (SFUNC=date_xor_transfn, finalfunc = date_agg_finalfn,  STYPE= int16);

-- support bit_xor aggregate for year

DROP FUNCTION IF EXISTS pg_catalog.year_xor_transfn(integer, year) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.year_xor_transfn (
integer, year
) RETURNS integer LANGUAGE C as '$libdir/dolphin',  'year_xor_transfn';

drop aggregate if exists pg_catalog.bit_xor(year);
create aggregate pg_catalog.bit_xor(year) (SFUNC=year_xor_transfn, STYPE= integer);

-- support bit_xor for datetime and timestamp
DROP FUNCTION IF EXISTS pg_catalog.timestamp_xor_transfn(int16, timestamp(0) with time zone) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.timestamp_xor_transfn (
int16, timestamp(0) with time zone
) RETURNS int16 LANGUAGE C as '$libdir/dolphin',  'timestamp_xor_transfn';

DROP FUNCTION IF EXISTS pg_catalog.timestamp_agg_finalfn(int16) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.timestamp_agg_finalfn (
int16
) RETURNS int16 LANGUAGE C as '$libdir/dolphin',  'timestamp_agg_finalfn';

drop aggregate if exists pg_catalog.bit_xor(timestamp(0) with time zone);
create aggregate pg_catalog.bit_xor(timestamp(0) with time zone) (SFUNC=timestamp_xor_transfn, finalfunc=timestamp_agg_finalfn, STYPE=int16);

-- support bit_xor aggregate for time

DROP FUNCTION IF EXISTS pg_catalog.time_xor_transfn(int16, time) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.time_xor_transfn (
int16, time
) RETURNS int16 LANGUAGE C as '$libdir/dolphin',  'time_xor_transfn';

create aggregate pg_catalog.bit_xor(time) (SFUNC=time_xor_transfn, STYPE= int16);

DROP FUNCTION IF EXISTS pg_catalog.timetz_xor_transfn(int16, time with time zone) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.timetz_xor_transfn (
int16, time with time zone
) RETURNS int16 LANGUAGE C as '$libdir/dolphin',  'timetz_xor_transfn';

create aggregate pg_catalog.bit_xor(time with time zone) (SFUNC=timetz_xor_transfn, STYPE= int16);


DROP FUNCTION IF EXISTS pg_catalog.year_any_value (year, year) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.year_any_value (year, year) RETURNS year LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'year_any_value';

drop aggregate if exists pg_catalog.any_value(year);
CREATE AGGREGATE pg_catalog.any_value(year) (
        sfunc = year_any_value,
        stype = year
);


CREATE OR REPLACE FUNCTION pg_catalog.time_float (time) RETURNS float8 LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'time_float';
DROP CAST IF EXISTS (time AS float8) CASCADE;
CREATE CAST(time AS float8) WITH FUNCTION time_float(time) AS IMPLICIT;

CREATE OR REPLACE FUNCTION pg_catalog.time_pl_float (time, float8) RETURNS float8 LANGUAGE SQL STABLE STRICT as 'select pg_catalog.time_float($1) + $2';
-- DROP OPERATOR IF EXISTS + (time, float8);
CREATE OPERATOR + (
    PROCEDURE = time_pl_float,
    LEFTARG = time,
    RIGHTARG = float8
);

CREATE OR REPLACE FUNCTION pg_catalog.time_mi_float (time, float8) RETURNS float8 LANGUAGE SQL STABLE STRICT as 'select pg_catalog.time_float($1) - $2';
-- DROP OPERATOR IF EXISTS - (time, float8);
CREATE OPERATOR - (
    PROCEDURE = time_mi_float,
    LEFTARG = time,
    RIGHTARG = float8
);

CREATE OR REPLACE FUNCTION pg_catalog.datetime_float (timestamp(0) without time zone) RETURNS float8 LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'datetime_float';
DROP CAST IF EXISTS (timestamp(0) without time zone AS float8) CASCADE;
CREATE CAST(timestamp(0) without time zone AS float8) WITH FUNCTION datetime_float(timestamp(0) without time zone) AS IMPLICIT;

CREATE OR REPLACE FUNCTION pg_catalog.datetime_pl_float (timestamp(0) without time zone, float8) RETURNS float8 LANGUAGE SQL STABLE STRICT as 'select pg_catalog.datetime_float($1) + $2';
-- DROP OPERATOR IF EXISTS + (timestamp(0) without time zone, float8);
CREATE OPERATOR + (
    PROCEDURE = datetime_pl_float,
    LEFTARG = timestamp(0) without time zone,
    RIGHTARG = float8
);

CREATE OR REPLACE FUNCTION pg_catalog.datetime_mi_float (timestamp(0) without time zone, float8) RETURNS float8 LANGUAGE SQL STABLE STRICT as 'select pg_catalog.datetime_float($1) - $2';
-- DROP OPERATOR IF EXISTS - (timestamp(0) without time zone, float8);
CREATE OPERATOR - (
    PROCEDURE = datetime_mi_float,
    LEFTARG = timestamp(0) without time zone,
    RIGHTARG = float8
);

-- CREATE OR REPLACE FUNCTION pg_catalog.date_int (date) RETURNS int4 LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'date_int';
-- DROP CAST IF EXISTS (date AS integer) CASCADE;
-- CREATE CAST(date AS integer) WITH FUNCTION date_int(date) AS IMPLICIT;

-- CREATE OR REPLACE FUNCTION pg_catalog.date_pl_int (date, integer) RETURNS integer LANGUAGE SQL STABLE STRICT as 'select pg_catalog.date_int($1) + $2';
-- -- DROP OPERATOR + (date, integer);
-- CREATE OPERATOR + (
--     PROCEDURE = date_pl_int,
--     LEFTARG = date,
--     RIGHTARG = integer
-- );

-- CREATE OR REPLACE FUNCTION pg_catalog.date_mi_int (date, integer) RETURNS integer LANGUAGE SQL STABLE STRICT as 'select pg_catalog.date_int($1) - $2';
-- -- DROP OPERATOR - (date, integer);
-- CREATE OPERATOR - (
--     PROCEDURE = date_mi_int,
--     LEFTARG = date,
--     RIGHTARG = integer
-- );

create function pg_catalog.datexor(
date,
date
) RETURNS int16 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'datexor';
create operator pg_catalog.^(leftarg = date, rightarg = date, procedure = pg_catalog.datexor);

create function pg_catalog.timexor(
time,
time
) RETURNS int16 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'timexor';
create operator pg_catalog.^(leftarg = time, rightarg = time, procedure = pg_catalog.timexor);

create function pg_catalog.date_time_xor(
date,
time
) RETURNS int16 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'date_time_xor';
create operator pg_catalog.^(leftarg = date, rightarg = time, procedure = pg_catalog.date_time_xor);

create function pg_catalog.time_date_xor(
time,
date
) RETURNS int16 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'time_date_xor';
create operator pg_catalog.^(leftarg = time, rightarg = date, procedure = pg_catalog.time_date_xor);

create function pg_catalog.time_text_xor(
time,
text
) RETURNS int16 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'time_text_xor';
create operator pg_catalog.^(leftarg = time, rightarg = text, procedure = pg_catalog.time_text_xor);

create function pg_catalog.text_time_xor(
text,
time
) RETURNS int16 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'text_time_xor';
create operator pg_catalog.^(leftarg = text, rightarg = time, procedure = pg_catalog.text_time_xor);

create function pg_catalog.date_text_xor(
date,
text
) RETURNS int16 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'date_text_xor';
create operator pg_catalog.^(leftarg = date, rightarg = text, procedure = pg_catalog.date_text_xor);

create function pg_catalog.text_date_xor(
text,
date
) RETURNS int16 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'text_date_xor';
create operator pg_catalog.^(leftarg = text, rightarg = date, procedure = pg_catalog.text_date_xor);

create function pg_catalog.date_int8_xor(
date,
int8
) RETURNS int16 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'date_int8_xor';
create operator pg_catalog.^(leftarg = date, rightarg = int8, procedure = pg_catalog.date_int8_xor);

create function pg_catalog.int8_date_xor(
int8,
date
) RETURNS int16 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'int8_date_xor';
create operator pg_catalog.^(leftarg = int8, rightarg = date, procedure = pg_catalog.int8_date_xor);

create function pg_catalog.time_int8_xor(
time,
int8
) RETURNS int16 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'time_int8_xor';
create operator pg_catalog.^(leftarg = time, rightarg = int8, procedure = pg_catalog.time_int8_xor);

create function pg_catalog.int8_time_xor(
int8,
time
) RETURNS int16 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'int8_time_xor';
create operator pg_catalog.^(leftarg = int8, rightarg = time, procedure = pg_catalog.int8_time_xor);

create function pg_catalog.date_float8_xor(
date,
float8
) RETURNS int16 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'date_float8_xor';
create operator pg_catalog.^(leftarg = date, rightarg = float8, procedure = pg_catalog.date_float8_xor);

create function pg_catalog.float8_date_xor(
float8,
date
) RETURNS int16 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'float8_date_xor';
create operator pg_catalog.^(leftarg = float8, rightarg = date, procedure = pg_catalog.float8_date_xor);

create function pg_catalog.timestampxor(
timestamp,
timestamp
) RETURNS int16 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'timestampxor';
create operator pg_catalog.^(leftarg = timestamp, rightarg = timestamp, procedure = pg_catalog.timestampxor);

create function pg_catalog.timestamp_int8_xor(
timestamp,
int8
) RETURNS int16 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'timestamp_int8_xor';
create operator pg_catalog.^(leftarg = timestamp, rightarg = int8, procedure = pg_catalog.timestamp_int8_xor);

create function pg_catalog.int8_timestamp_xor(
int8,
timestamp
) RETURNS int16 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'int8_timestamp_xor';
create operator pg_catalog.^(leftarg = int8, rightarg = timestamp, procedure = pg_catalog.int8_timestamp_xor);

create function pg_catalog.timestamp_float8_xor(
timestamp,
float8
) RETURNS int16 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'timestamp_float8_xor';
create operator pg_catalog.^(leftarg = timestamp, rightarg = float8, procedure = pg_catalog.timestamp_float8_xor);

create function pg_catalog.float8_timestamp_xor(
float8,
timestamp
) RETURNS int16 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'float8_timestamp_xor';
create operator pg_catalog.^(leftarg = float8, rightarg = timestamp, procedure = pg_catalog.float8_timestamp_xor);

create function pg_catalog.timestamp_text_xor(
timestamp,
text
) RETURNS int16 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'timestamp_text_xor';
create operator pg_catalog.^(leftarg = timestamp, rightarg = text, procedure = pg_catalog.timestamp_text_xor);

create function pg_catalog.text_timestamp_xor(
text,
timestamp
) RETURNS int16 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'text_timestamp_xor';
create operator pg_catalog.^(leftarg = text, rightarg = timestamp, procedure = pg_catalog.text_timestamp_xor);

create function pg_catalog.timestamptzxor(
timestampTz,
timestampTz
) RETURNS int16 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'timestamptzxor';
create operator pg_catalog.^(leftarg = timestampTz, rightarg = timestampTz, procedure = pg_catalog.timestamptzxor);

create function pg_catalog.timestamptz_int8_xor(
timestampTz,
int8
) RETURNS int16 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'timestamptz_int8_xor';
create operator pg_catalog.^(leftarg = timestampTz, rightarg = int8, procedure = pg_catalog.timestamptz_int8_xor);

create function pg_catalog.int8_timestamptz_xor(
int8,
timestampTz
) RETURNS int16 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'int8_timestamptz_xor';
create operator pg_catalog.^(leftarg = int8, rightarg = timestampTz, procedure = pg_catalog.int8_timestamptz_xor);

create function pg_catalog.timestamptz_float8_xor(
timestampTz,
float8
) RETURNS int16 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'timestamptz_float8_xor';
create operator pg_catalog.^(leftarg = timestampTz, rightarg = float8, procedure = pg_catalog.timestamptz_float8_xor);

create function pg_catalog.float8_timestamptz_xor(
float8,
timestampTz
) RETURNS int16 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'float8_timestamptz_xor';
create operator pg_catalog.^(leftarg = float8, rightarg = timestampTz, procedure = pg_catalog.float8_timestamptz_xor);

create function pg_catalog.timestamptz_text_xor(
timestampTz,
text
) RETURNS int16 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'timestamptz_text_xor';
create operator pg_catalog.^(leftarg = timestampTz, rightarg = text, procedure = pg_catalog.timestamptz_text_xor);

create function pg_catalog.text_timestamptz_xor(
text,
timestampTz
) RETURNS int16 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'text_timestamptz_xor';
create operator pg_catalog.^(leftarg = text, rightarg = timestampTz, procedure = pg_catalog.text_timestamptz_xor);

DROP FUNCTION IF EXISTS pg_catalog.sleep(float8) CASCADE;
CREATE FUNCTION pg_catalog.sleep (float8)
RETURNS int LANGUAGE C STABLE CALLED ON NULL INPUT as '$libdir/dolphin', 'db_b_sleep';
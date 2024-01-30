DROP FUNCTION IF EXISTS pg_catalog.dolphin_invoke();
CREATE FUNCTION pg_catalog.dolphin_invoke()
    RETURNS VOID AS '$libdir/dolphin','dolphin_invoke' LANGUAGE C STRICT;

DROP FUNCTION IF EXISTS pg_catalog.date_cast(cstring, boolean);
CREATE OR REPLACE FUNCTION pg_catalog.date_cast(cstring, boolean) RETURNS date LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'date_cast';

DROP FUNCTION IF EXISTS pg_catalog.timestamp_cast(cstring, oid, integer, boolean);
CREATE OR REPLACE FUNCTION pg_catalog.timestamp_cast(cstring, oid, integer, boolean) RETURNS timestamp without time zone LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'timestamp_cast';

DROP FUNCTION IF EXISTS pg_catalog.time_cast(cstring, boolean);
CREATE OR REPLACE FUNCTION pg_catalog.time_cast(cstring, boolean) RETURNS time without time zone LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'time_cast';

DROP CAST IF EXISTS (TEXT AS time);
DROP FUNCTION IF EXISTS pg_catalog.time_cast_implicit(TEXT);
CREATE OR REPLACE FUNCTION pg_catalog.time_cast_implicit(TEXT) RETURNS time without time zone LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'time_cast_implicit';

DROP FUNCTION IF EXISTS pg_catalog.text_time_explicit(TEXT);
CREATE OR REPLACE FUNCTION pg_catalog.text_time_explicit(TEXT) RETURNS time without time zone LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'text_time_explicit';

CREATE CAST(TEXT AS time) WITH FUNCTION time_cast_implicit(TEXT) AS ASSIGNMENT;

DROP FUNCTION IF EXISTS pg_catalog.day(time without time zone);
CREATE OR REPLACE FUNCTION pg_catalog.day(time without time zone) RETURNS int4 LANGUAGE C STABLE RETURNS NULL ON NULL INPUT as '$libdir/dolphin', 'dayofmonth_time';

DROP FUNCTION IF EXISTS pg_catalog.time_to_sec(int8);
CREATE OR REPLACE FUNCTION pg_catalog.time_to_sec(int8) RETURNS int4 LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'int64_time_to_sec';

DROP FUNCTION IF EXISTS pg_catalog.time_to_sec(numeric);
CREATE OR REPLACE FUNCTION pg_catalog.time_to_sec(numeric) RETURNS int4 LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'numeric_time_to_sec';

--CREATE TIME_TIMESTAMP'S COMPARATION FUNCTION
DROP FUNCTION IF EXISTS pg_catalog.time_eq_timestamp (time, timestamp without time zone) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.time_eq_timestamp (time, timestamp without time zone) RETURNS boolean LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'time_eq_timestamp';
DROP FUNCTION IF EXISTS pg_catalog.time_ne_timestamp (time, timestamp without time zone) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.time_ne_timestamp (time, timestamp without time zone) RETURNS boolean LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'time_ne_timestamp';
DROP FUNCTION IF EXISTS pg_catalog.time_le_timestamp (time, timestamp without time zone) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.time_le_timestamp (time, timestamp without time zone) RETURNS boolean LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'time_le_timestamp';
DROP FUNCTION IF EXISTS pg_catalog.time_lt_timestamp (time, timestamp without time zone) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.time_lt_timestamp (time, timestamp without time zone) RETURNS boolean LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'time_lt_timestamp';
DROP FUNCTION IF EXISTS pg_catalog.time_ge_timestamp (time, timestamp without time zone) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.time_ge_timestamp (time, timestamp without time zone) RETURNS boolean LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'time_ge_timestamp';
DROP FUNCTION IF EXISTS pg_catalog.time_gt_timestamp (time, timestamp without time zone) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.time_gt_timestamp (time, timestamp without time zone) RETURNS boolean LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'time_gt_timestamp';
--CREATE TIME_TIMESTAMP CMP OPERATOR

CREATE OPERATOR pg_catalog.=(leftarg = time, rightarg = timestamp without time zone, procedure = time_eq_timestamp, restrict = eqsel, join = eqjoinsel, MERGES);

CREATE OPERATOR pg_catalog.<>(leftarg = time, rightarg = timestamp without time zone, procedure = time_ne_timestamp, restrict = neqsel, join = neqjoinsel);

CREATE OPERATOR pg_catalog.<=(leftarg = time, rightarg = timestamp without time zone, procedure = time_le_timestamp, COMMUTATOR  = >=, NEGATOR  = >, restrict = scalarltsel, join = scalarltjoinsel);

CREATE OPERATOR pg_catalog.<(leftarg = time, rightarg = timestamp without time zone, procedure = time_lt_timestamp, COMMUTATOR  = >, NEGATOR  = >=, restrict = scalarltsel, join = scalarltjoinsel);

CREATE OPERATOR pg_catalog.>=(leftarg = time, rightarg = timestamp without time zone, procedure = time_ge_timestamp, COMMUTATOR  = <=, NEGATOR  = <, restrict = scalarltsel, join = scalarltjoinsel);

CREATE OPERATOR pg_catalog.>(leftarg = time, rightarg = timestamp without time zone, procedure = time_gt_timestamp, COMMUTATOR  = <, NEGATOR  = <=, restrict = scalarltsel, join = scalarltjoinsel);
--CREATE TIMESTAMP_TIME'S COMPARATION FUNCTION
DROP FUNCTION IF EXISTS pg_catalog.timestamp_eq_time (timestamp without time zone, time) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.timestamp_eq_time (timestamp without time zone, time) RETURNS boolean LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'timestamp_eq_time';
DROP FUNCTION IF EXISTS pg_catalog.timestamp_ne_time (timestamp without time zone, time) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.timestamp_ne_time (timestamp without time zone, time) RETURNS boolean LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'timestamp_ne_time';
DROP FUNCTION IF EXISTS pg_catalog.timestamp_le_time (timestamp without time zone, time) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.timestamp_le_time (timestamp without time zone, time) RETURNS boolean LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'timestamp_le_time';
DROP FUNCTION IF EXISTS pg_catalog.timestamp_lt_time (timestamp without time zone, time) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.timestamp_lt_time (timestamp without time zone, time) RETURNS boolean LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'timestamp_lt_time';
DROP FUNCTION IF EXISTS pg_catalog.timestamp_ge_time (timestamp without time zone, time) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.timestamp_ge_time (timestamp without time zone, time) RETURNS boolean LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'timestamp_ge_time';
DROP FUNCTION IF EXISTS pg_catalog.timestamp_gt_time (timestamp without time zone, time) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.timestamp_gt_time (timestamp without time zone, time) RETURNS boolean LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'timestamp_gt_time';
--CREATE TIMESTAMP_TIME CMP OPERATOR

CREATE OPERATOR pg_catalog.=(leftarg = timestamp without time zone, rightarg = time, procedure = timestamp_eq_time, restrict = eqsel, join = eqjoinsel, MERGES);

CREATE OPERATOR pg_catalog.<>(leftarg = timestamp without time zone, rightarg = time, procedure = timestamp_ne_time, restrict = neqsel, join = neqjoinsel);

CREATE OPERATOR pg_catalog.<=(leftarg = timestamp without time zone, rightarg = time, procedure = timestamp_le_time, COMMUTATOR  = >=, NEGATOR  = >, restrict = scalarltsel, join = scalarltjoinsel);

CREATE OPERATOR pg_catalog.<(leftarg = timestamp without time zone, rightarg = time, procedure = timestamp_lt_time, COMMUTATOR  = >, NEGATOR  = >=, restrict = scalarltsel, join = scalarltjoinsel);

CREATE OPERATOR pg_catalog.>=(leftarg = timestamp without time zone, rightarg = time, procedure = timestamp_ge_time, COMMUTATOR  = <=, NEGATOR  = <, restrict = scalarltsel, join = scalarltjoinsel);

CREATE OPERATOR pg_catalog.>(leftarg = timestamp without time zone, rightarg = time, procedure = timestamp_gt_time, COMMUTATOR  = <, NEGATOR  = <=, restrict = scalarltsel, join = scalarltjoinsel);

--CREATE TIME_TIMESTAMPTZ'S COMPARATION FUNCTION
DROP FUNCTION IF EXISTS pg_catalog.time_eq_timestamptz (time, timestamp with time zone) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.time_eq_timestamptz (time, timestamp with time zone) RETURNS boolean LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'time_eq_timestamptz';
DROP FUNCTION IF EXISTS pg_catalog.time_ne_timestamptz (time, timestamp with time zone) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.time_ne_timestamptz (time, timestamp with time zone) RETURNS boolean LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'time_ne_timestamptz';
DROP FUNCTION IF EXISTS pg_catalog.time_le_timestamptz (time, timestamp with time zone) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.time_le_timestamptz (time, timestamp with time zone) RETURNS boolean LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'time_le_timestamptz';
DROP FUNCTION IF EXISTS pg_catalog.time_lt_timestamptz (time, timestamp with time zone) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.time_lt_timestamptz (time, timestamp with time zone) RETURNS boolean LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'time_lt_timestamptz';
DROP FUNCTION IF EXISTS pg_catalog.time_ge_timestamptz (time, timestamp with time zone) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.time_ge_timestamptz (time, timestamp with time zone) RETURNS boolean LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'time_ge_timestamptz';
DROP FUNCTION IF EXISTS pg_catalog.time_gt_timestamptz (time, timestamp with time zone) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.time_gt_timestamptz (time, timestamp with time zone) RETURNS boolean LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'time_gt_timestamptz';
--CREATE TIME_TIMESTAMPTZ CMP OPERATOR

CREATE OPERATOR pg_catalog.=(leftarg = time, rightarg = timestamp with time zone, procedure = time_eq_timestamptz, restrict = eqsel, join = eqjoinsel, MERGES);

CREATE OPERATOR pg_catalog.<>(leftarg = time, rightarg = timestamp with time zone, procedure = time_ne_timestamptz, restrict = neqsel, join = neqjoinsel);

CREATE OPERATOR pg_catalog.<=(leftarg = time, rightarg = timestamp with time zone, procedure = time_le_timestamptz, COMMUTATOR  = >=, NEGATOR  = >, restrict = scalarltsel, join = scalarltjoinsel);

CREATE OPERATOR pg_catalog.<(leftarg = time, rightarg = timestamp with time zone, procedure = time_lt_timestamptz, COMMUTATOR  = >, NEGATOR  = >=, restrict = scalarltsel, join = scalarltjoinsel);

CREATE OPERATOR pg_catalog.>=(leftarg = time, rightarg = timestamp with time zone, procedure = time_ge_timestamptz, COMMUTATOR  = <=, NEGATOR  = <, restrict = scalarltsel, join = scalarltjoinsel);

CREATE OPERATOR pg_catalog.>(leftarg = time, rightarg = timestamp with time zone, procedure = time_gt_timestamptz, COMMUTATOR  = <, NEGATOR  = <=, restrict = scalarltsel, join = scalarltjoinsel);
--CREATE TIMESTAMPTZ_TIME'S COMPARATION FUNCTION
DROP FUNCTION IF EXISTS pg_catalog.timestamptz_eq_time (timestamp with time zone, time) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.timestamptz_eq_time (timestamp with time zone, time) RETURNS boolean LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'timestamptz_eq_time';
DROP FUNCTION IF EXISTS pg_catalog.timestamptz_ne_time (timestamp with time zone, time) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.timestamptz_ne_time (timestamp with time zone, time) RETURNS boolean LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'timestamptz_ne_time';
DROP FUNCTION IF EXISTS pg_catalog.timestamptz_le_time (timestamp with time zone, time) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.timestamptz_le_time (timestamp with time zone, time) RETURNS boolean LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'timestamptz_le_time';
DROP FUNCTION IF EXISTS pg_catalog.timestamptz_lt_time (timestamp with time zone, time) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.timestamptz_lt_time (timestamp with time zone, time) RETURNS boolean LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'timestamptz_lt_time';
DROP FUNCTION IF EXISTS pg_catalog.timestamptz_ge_time (timestamp with time zone, time) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.timestamptz_ge_time (timestamp with time zone, time) RETURNS boolean LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'timestamptz_ge_time';
DROP FUNCTION IF EXISTS pg_catalog.timestamptz_gt_time (timestamp with time zone, time) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.timestamptz_gt_time (timestamp with time zone, time) RETURNS boolean LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'timestamptz_gt_time';
--CREATE TIMESTAMPTZ_TIME CMP OPERATOR

CREATE OPERATOR pg_catalog.=(leftarg = timestamp with time zone, rightarg = time, procedure = timestamptz_eq_time, restrict = eqsel, join = eqjoinsel, MERGES);

CREATE OPERATOR pg_catalog.<>(leftarg = timestamp with time zone, rightarg = time, procedure = timestamptz_ne_time, restrict = neqsel, join = neqjoinsel);

CREATE OPERATOR pg_catalog.<=(leftarg = timestamp with time zone, rightarg = time, procedure = timestamptz_le_time, COMMUTATOR  = >=, NEGATOR  = >, restrict = scalarltsel, join = scalarltjoinsel);

CREATE OPERATOR pg_catalog.<(leftarg = timestamp with time zone, rightarg = time, procedure = timestamptz_lt_time, COMMUTATOR  = >, NEGATOR  = >=, restrict = scalarltsel, join = scalarltjoinsel);

CREATE OPERATOR pg_catalog.>=(leftarg = timestamp with time zone, rightarg = time, procedure = timestamptz_ge_time, COMMUTATOR  = <=, NEGATOR  = <, restrict = scalarltsel, join = scalarltjoinsel);

CREATE OPERATOR pg_catalog.>(leftarg = timestamp with time zone, rightarg = time, procedure = timestamptz_gt_time, COMMUTATOR  = <, NEGATOR  = <=, restrict = scalarltsel, join = scalarltjoinsel);

--change bit -> int/bigint castcontext from 'e' to 'a'
do $$
begin
    update pg_cast set castcontext='a', castowner=10 where castsource=1560 and casttarget=20 and castcontext='e';
    update pg_cast set castcontext='a', castowner=10 where castsource=1560 and casttarget=23 and castcontext='e';
    update pg_cast set castcontext='a', castowner=10 where castsource=20 and casttarget=1560 and castcontext='e';
    update pg_cast set castcontext='a', castowner=10 where castsource=23 and casttarget=1560 and castcontext='e';
end
$$;
drop CAST IF EXISTS (uint4 AS bit);
drop CAST IF EXISTS (uint8 AS bit);
CREATE CAST (uint4 AS bit) WITH FUNCTION bitfromuint4(uint4, int4) AS ASSIGNMENT;
CREATE CAST (uint8 AS bit) WITH FUNCTION bitfromuint8(uint8, int4) AS ASSIGNMENT;

-- non-strict, accept null input
CREATE OR REPLACE FUNCTION pg_catalog.rand(int16) returns double precision LANGUAGE C volatile as '$libdir/dolphin', 'rand_seed';
CREATE OR REPLACE FUNCTION pg_catalog.rand(uint4) returns double precision LANGUAGE SQL volatile as 'select rand($1::int16)';
CREATE OR REPLACE FUNCTION pg_catalog.rand(timestamp with time zone) returns double precision LANGUAGE SQL volatile as 'select rand($1::int8)';
CREATE OR REPLACE FUNCTION pg_catalog.rand(date) returns double precision LANGUAGE SQL volatile as 'select rand($1::int8)';
CREATE OR REPLACE FUNCTION pg_catalog.rand(year) returns double precision LANGUAGE SQL volatile as 'select rand($1::int8)';
CREATE OR REPLACE FUNCTION pg_catalog.rand(binary) returns double precision LANGUAGE SQL volatile as 'select rand($1::int8)';
CREATE OR REPLACE FUNCTION pg_catalog.rand(blob) returns double precision LANGUAGE SQL volatile as 'select rand($1::int8)';
CREATE OR REPLACE FUNCTION pg_catalog.rand(anyenum) returns double precision LANGUAGE SQL volatile as 'select rand($1::int8)';
CREATE OR REPLACE FUNCTION pg_catalog.rand(anyset) returns double precision LANGUAGE SQL volatile as 'select rand($1::int8)';
CREATE OR REPLACE FUNCTION pg_catalog.rand(json) returns double precision LANGUAGE SQL volatile as 'select rand($1::int8)';

CREATE OR REPLACE FUNCTION pg_catalog.random_bytes(int4) returns blob LANGUAGE C volatile STRICT as '$libdir/dolphin', 'random_bytes';
CREATE OR REPLACE FUNCTION pg_catalog.random_bytes(bit) returns blob LANGUAGE SQL volatile as 'select random_bytes($1::int4)';
CREATE OR REPLACE FUNCTION pg_catalog.random_bytes(timestamp with time zone) returns blob LANGUAGE SQL volatile as 'select random_bytes($1::int4)';
CREATE OR REPLACE FUNCTION pg_catalog.random_bytes(date) returns blob LANGUAGE SQL volatile as 'select random_bytes($1::int4)';
CREATE OR REPLACE FUNCTION pg_catalog.random_bytes(year) returns blob LANGUAGE SQL volatile as 'select random_bytes($1::int4)';
CREATE OR REPLACE FUNCTION pg_catalog.random_bytes(binary) returns blob LANGUAGE SQL volatile as 'select random_bytes($1::int4)';
CREATE OR REPLACE FUNCTION pg_catalog.random_bytes(blob) returns blob LANGUAGE SQL volatile as 'select random_bytes($1::int4)';
CREATE OR REPLACE FUNCTION pg_catalog.random_bytes(anyenum) returns blob LANGUAGE SQL volatile as 'select random_bytes($1::int4)';
CREATE OR REPLACE FUNCTION pg_catalog.random_bytes(anyset) returns blob LANGUAGE SQL volatile as 'select random_bytes($1::int4)';
CREATE OR REPLACE FUNCTION pg_catalog.random_bytes(json) returns blob LANGUAGE SQL volatile as 'select random_bytes($1::int4)';

-- The reason for using replace is because we don't want to change the OID
CREATE OR REPLACE FUNCTION pg_catalog.tinyblob_rawout (
tinyblob
) RETURNS cstring LANGUAGE  C IMMUTABLE STRICT as '$libdir/dolphin',  'dolphin_blob_rawout';

CREATE OR REPLACE FUNCTION pg_catalog.mediumblob_rawout (
mediumblob
) RETURNS cstring LANGUAGE  C IMMUTABLE STRICT as '$libdir/dolphin',  'dolphin_blob_rawout';

CREATE OR REPLACE FUNCTION pg_catalog.longblob_rawout (
longblob
) RETURNS cstring LANGUAGE  C IMMUTABLE STRICT as '$libdir/dolphin',  'dolphin_blob_rawout';

-- Make the result of oct(bit) and conv(bit) identical to Mysql
DROP FUNCTION IF EXISTS pg_catalog.conv(bit, int4, int4) CASCADE;

CREATE OR REPLACE FUNCTION pg_catalog.conv(bit, int4, int4) 
RETURNS text AS $$ SELECT pg_catalog.conv($1::int8, 10, $3) $$ LANGUAGE SQL;

DROP FUNCTION IF EXISTS pg_catalog.oct(bit);

CREATE OR REPLACE FUNCTION pg_catalog.oct(t1 bit)
RETURNS text AS $$ SELECT pg_catalog.conv(t1, 10, 8) $$ LANGUAGE SQL;

-- Supplement function dayofmonth to make the function dayofmonth() the same as function day()
DROP FUNCTION IF EXISTS pg_catalog.dayofmonth(text);
DROP FUNCTION IF EXISTS pg_catalog.dayofmonth(numeric);
CREATE OR REPLACE FUNCTION pg_catalog.dayofmonth(text) RETURNS int4 AS $$ SELECT pg_catalog.day($1); $$ LANGUAGE SQL;
CREATE OR REPLACE FUNCTION pg_catalog.dayofmonth(numeric) RETURNS int4 AS $$ SELECT pg_catalog.day($1); $$ LANGUAGE SQL;

drop CAST IF EXISTS (timestamptz as boolean);
drop CAST IF EXISTS (timestamp(0) without time zone as boolean);
DROP FUNCTION IF EXISTS pg_catalog.timestamptz_bool(timestamptz);
DROP FUNCTION IF EXISTS pg_catalog.timestamp_bool(timestamp(0) without time zone); 
CREATE OR REPLACE FUNCTION pg_catalog.timestamptz_bool(timestamptz) returns boolean LANGUAGE C immutable strict as '$libdir/dolphin', 'timestamptz_bool';
CREATE CAST (timestamptz as boolean) WITH FUNCTION timestamptz_bool(timestamptz) AS ASSIGNMENT;

drop function if EXISTS pg_catalog.length(binary);
drop function if EXISTS pg_catalog.length(varbinary);
CREATE OR REPLACE FUNCTION pg_catalog.length(binary) returns int4 LANGUAGE C immutable strict as '$libdir/dolphin', 'binary_length';
CREATE OR REPLACE FUNCTION pg_catalog.length(varbinary) returns int4 LANGUAGE C immutable strict as '$libdir/dolphin', 'binary_length';

CREATE OR REPLACE FUNCTION pg_catalog.timestamp_bool(timestamp(0) without time zone) returns boolean LANGUAGE C immutable strict as '$libdir/dolphin', 'timestamp_bool';
CREATE CAST (timestamp(0) without time zone as boolean) WITH FUNCTION timestamp_bool(timestamp(0) without time zone) AS ASSIGNMENT;

DROP FUNCTION IF EXISTS pg_catalog.date_format (time without time zone, text);
CREATE OR REPLACE FUNCTION pg_catalog.date_format (time without time zone, text) RETURNS TEXT LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'date_format_time';

DROP FUNCTION IF EXISTS pg_catalog.to_char(time without time zone, text);
CREATE OR REPLACE FUNCTION pg_catalog.to_char(time without time zone, text) RETURNS TEXT LANGUAGE SQL IMMUTABLE STRICT as $$ SELECT pg_catalog.to_char($1::interval, $2) $$;

CREATE OR REPLACE FUNCTION pg_catalog.bit_cast_date(bit) 
RETURNS date LANGUAGE SQL IMMUTABLE STRICT as 
'select cast(cast($1 as text) as date)';

CREATE OR REPLACE FUNCTION pg_catalog.bit_cast_datetime(bit) 
RETURNS timestamp without time zone LANGUAGE SQL IMMUTABLE STRICT as 
'select cast(cast($1 as text) as timestamp without time zone)';

CREATE OR REPLACE FUNCTION pg_catalog.bit_cast_timestamp(bit) 
RETURNS timestamptz LANGUAGE SQL IMMUTABLE STRICT as 
'select cast(cast($1 as text) as timestamptz)';

CREATE OR REPLACE FUNCTION pg_catalog.bit_cast_time(bit) 
RETURNS time without time zone LANGUAGE SQL IMMUTABLE STRICT as 
'select cast(cast($1 as text) as time without time zone)';

CREATE OR REPLACE FUNCTION pg_catalog.int8_cast_time(int1)
RETURNS time without time zone LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'int8_cast_time';
CREATE OR REPLACE FUNCTION pg_catalog.int16_cast_time(int2)
RETURNS time without time zone LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'int16_cast_time';
CREATE OR REPLACE FUNCTION pg_catalog.int32_cast_time(int4)
RETURNS time without time zone LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'int32_cast_time';
CREATE OR REPLACE FUNCTION pg_catalog.int64_cast_time(int8)
RETURNS time without time zone LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'int64_cast_time';

CREATE OR REPLACE FUNCTION pg_catalog.uint8_cast_time(uint1)
RETURNS time without time zone LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'uint8_cast_time';
CREATE OR REPLACE FUNCTION pg_catalog.uint16_cast_time(uint2)
RETURNS time without time zone LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'uint16_cast_time';
CREATE OR REPLACE FUNCTION pg_catalog.uint32_cast_time(uint4)
RETURNS time without time zone LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'uint32_cast_time';
CREATE OR REPLACE FUNCTION pg_catalog.uint64_cast_time(uint8)
RETURNS time without time zone LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'uint64_cast_time';

CREATE OR REPLACE FUNCTION pg_catalog.float4_cast_time(float4)
RETURNS time without time zone LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'float4_cast_time';
CREATE OR REPLACE FUNCTION pg_catalog.float8_cast_time(float8)
RETURNS time without time zone LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'float8_cast_time';
CREATE OR REPLACE FUNCTION pg_catalog.numeric_cast_time(numeric)
RETURNS time without time zone LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'numeric_cast_time';
create or replace function pg_catalog."user"() returns name as 'select current_user' LANGUAGE 'sql' IMMUTABLE;
create or replace function pg_catalog."user"() returns name as 'select current_user' LANGUAGE 'sql' IMMUTABLE;

CREATE OR REPLACE FUNCTION pg_catalog.hour (date) RETURNS float8 LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'GetHourFromDate';
CREATE OR REPLACE FUNCTION pg_catalog.microsecond (date) RETURNS float8 LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'GetMicrosecondFromDate';
CREATE OR REPLACE FUNCTION pg_catalog.minute (date) RETURNS float8 LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'GetMinuteFromDate';
CREATE OR REPLACE FUNCTION pg_catalog.second (date) RETURNS float8 LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'GetSecondFromDate';

CREATE OR REPLACE FUNCTION pg_catalog.hour (timetz) RETURNS float8 LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'GetHourFromTimeTz';
CREATE OR REPLACE FUNCTION pg_catalog.microsecond (timetz) RETURNS float8 LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'GetMicrosecondFromTimeTz';
CREATE OR REPLACE FUNCTION pg_catalog.minute (timetz) RETURNS float8 LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'GetMinuteFromTimeTz';
CREATE OR REPLACE FUNCTION pg_catalog.second (timetz) RETURNS float8 LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'GetSecondFromTimeTz';

CREATE OR REPLACE FUNCTION pg_catalog.hour (timestamptz) RETURNS float8 LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'GetHourFromTimestampTz';
CREATE OR REPLACE FUNCTION pg_catalog.microsecond (timestamptz) RETURNS float8 LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'GetMicrosecondFromTimestampTz';
CREATE OR REPLACE FUNCTION pg_catalog.minute (timestamptz) RETURNS float8 LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'GetMinuteFromTimestampTz';
CREATE OR REPLACE FUNCTION pg_catalog.second (timestamptz) RETURNS float8 LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'GetSecondFromTimestampTz';

CREATE OR REPLACE FUNCTION pg_catalog.convert_datetime_double(double precision) RETURNS timestamp without time zone LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'convert_datetime_double';
CREATE OR REPLACE FUNCTION pg_catalog.convert_timestamptz_double(double precision) RETURNS timestamp LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'convert_timestamptz_double';
CREATE OR REPLACE FUNCTION pg_catalog.convert_datetime_uint8(uint8) RETURNS timestamp without time zone LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'convert_datetime_uint64';
CREATE OR REPLACE FUNCTION pg_catalog.convert_timestamptz_uint8(uint8) RETURNS timestamp LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'convert_timestamptz_uint64';

create or replace function pg_catalog.datetime_double_gt(
    timestamp without time zone,
    double precision
) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT as $$ (SELECT $1 > convert_datetime_double($2)) $$;
create operator pg_catalog.>(leftarg = timestamp without time zone, rightarg = double precision, procedure = pg_catalog.datetime_double_gt);
 
create or replace function pg_catalog.datetime_double_ge(
    timestamp without time zone,
    double precision
) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT as $$ (SELECT $1 >= convert_datetime_double($2)) $$;
create operator pg_catalog.>=(leftarg = timestamp without time zone, rightarg = double precision, procedure = pg_catalog.datetime_double_ge);

create or replace function pg_catalog.datetime_double_lt(
    timestamp without time zone,
    double precision
) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT as $$ (SELECT $1 < convert_datetime_double($2)) $$;
create operator pg_catalog.<(leftarg = timestamp without time zone, rightarg = double precision, procedure = pg_catalog.datetime_double_lt);

create or replace function pg_catalog.datetime_double_le(
    timestamp without time zone,
    double precision
) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT as $$ (SELECT $1 <= convert_datetime_double($2)) $$;
create operator pg_catalog.<=(leftarg = timestamp without time zone, rightarg = double precision, procedure = pg_catalog.datetime_double_le);

create or replace function pg_catalog.datetime_double_eq(
    timestamp without time zone,
    double precision
) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT as $$ (SELECT $1 = convert_datetime_double($2)) $$;
create operator pg_catalog.=(leftarg = timestamp without time zone, rightarg = double precision, procedure = pg_catalog.datetime_double_eq);

create or replace function pg_catalog.datetime_double_ne(
    timestamp without time zone,
    double precision
) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT as $$ (SELECT $1 <> convert_datetime_double($2)) $$;
create operator pg_catalog.<>(leftarg = timestamp without time zone, rightarg = double precision, procedure = pg_catalog.datetime_double_ne);


create or replace function pg_catalog.timestamp_double_gt(
    timestamptz,
    double precision
) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT as $$ (SELECT $1 > convert_timestamptz_double($2)) $$;
create operator pg_catalog.>(leftarg = timestamptz, rightarg = double precision, procedure = pg_catalog.timestamp_double_gt);

create or replace function pg_catalog.timestamp_double_ge(
    timestamptz,
    double precision
) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT as $$ (SELECT $1 >= convert_timestamptz_double($2)) $$;
create operator pg_catalog.>=(leftarg = timestamptz, rightarg = double precision, procedure = pg_catalog.timestamp_double_ge);

create or replace function pg_catalog.timestamp_double_lt(
    timestamptz,
    double precision
) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT as $$ (SELECT $1 < convert_timestamptz_double($2)) $$;
create operator pg_catalog.<(leftarg = timestamptz, rightarg = double precision, procedure = pg_catalog.timestamp_double_lt);

create or replace function pg_catalog.timestamp_double_le(
    timestamptz,
    double precision
) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT as $$ (SELECT $1 <= convert_timestamptz_double($2)) $$;
create operator pg_catalog.<=(leftarg = timestamptz, rightarg = double precision, procedure = pg_catalog.timestamp_double_le);

create or replace function pg_catalog.timestamp_double_eq(
    timestamptz,
    double precision
) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT as $$ (SELECT $1 = convert_timestamptz_double($2)) $$;
create operator pg_catalog.=(leftarg = timestamptz, rightarg = double precision, procedure = pg_catalog.timestamp_double_eq);

create or replace function pg_catalog.timestamp_double_ne(
    timestamptz,
    double precision
) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT as $$ (SELECT $1 <> convert_timestamptz_double($2)) $$;
create operator pg_catalog.<>(leftarg = timestamptz, rightarg = double precision, procedure = pg_catalog.timestamp_double_ne);

create or replace function pg_catalog.datetime_uint8_gt(
    timestamp without time zone,
    uint8
) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT as $$ (SELECT $1 > convert_datetime_uint8($2)) $$;
create operator pg_catalog.>(leftarg = timestamp without time zone, rightarg = uint8, procedure = pg_catalog.datetime_uint8_gt);
 
create or replace function pg_catalog.datetime_uint8_ge(
    timestamp without time zone,
    uint8
) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT as $$ (SELECT $1 >= convert_datetime_uint8($2)) $$;
create operator pg_catalog.>=(leftarg = timestamp without time zone, rightarg = uint8, procedure = pg_catalog.datetime_uint8_ge);

create or replace function pg_catalog.datetime_uint8_lt(
    timestamp without time zone,
    uint8
) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT as $$ (SELECT $1 < convert_datetime_uint8($2)) $$;
create operator pg_catalog.<(leftarg = timestamp without time zone, rightarg = uint8, procedure = pg_catalog.datetime_uint8_lt);

create or replace function pg_catalog.datetime_uint8_le(
    timestamp without time zone,
    uint8
) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT as $$ (SELECT $1 <= convert_datetime_uint8($2)) $$;
create operator pg_catalog.<=(leftarg = timestamp without time zone, rightarg = uint8, procedure = pg_catalog.datetime_uint8_le);

create or replace function pg_catalog.datetime_uint8_eq(
    timestamp without time zone,
    uint8
) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT as $$ (SELECT $1 = convert_datetime_uint8($2)) $$;
create operator pg_catalog.=(leftarg = timestamp without time zone, rightarg = uint8, procedure = pg_catalog.datetime_uint8_eq);

create or replace function pg_catalog.datetime_uint8_ne(
    timestamp without time zone,
    uint8
) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT as $$ (SELECT $1 <> convert_datetime_uint8($2)) $$;
create operator pg_catalog.<>(leftarg = timestamp without time zone, rightarg = uint8, procedure = pg_catalog.datetime_uint8_ne);


create or replace function pg_catalog.timestamp_uint8_gt(
    timestamptz,
    uint8
) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT as $$ (SELECT $1 > convert_timestamptz_uint8($2)) $$;
create operator pg_catalog.>(leftarg = timestamptz, rightarg = uint8, procedure = pg_catalog.timestamp_uint8_gt);

create or replace function pg_catalog.timestamp_uint8_ge(
    timestamptz,
    uint8
) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT as $$ (SELECT $1 >= convert_timestamptz_uint8($2)) $$;
create operator pg_catalog.>=(leftarg = timestamptz, rightarg = uint8, procedure = pg_catalog.timestamp_uint8_ge);

create or replace function pg_catalog.timestamp_uint8_lt(
    timestamptz,
    uint8
) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT as $$ (SELECT $1 < convert_timestamptz_uint8($2)) $$;
create operator pg_catalog.<(leftarg = timestamptz, rightarg = uint8, procedure = pg_catalog.timestamp_uint8_lt);

create or replace function pg_catalog.timestamp_uint8_le(
    timestamptz,
    uint8
) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT as $$ (SELECT $1 <= convert_timestamptz_uint8($2)) $$;
create operator pg_catalog.<=(leftarg = timestamptz, rightarg = uint8, procedure = pg_catalog.timestamp_uint8_le);

create or replace function pg_catalog.timestamp_uint8_eq(
    timestamptz,
    uint8
) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT as $$ (SELECT $1 = convert_timestamptz_uint8($2)) $$;
create operator pg_catalog.=(leftarg = timestamptz, rightarg = uint8, procedure = pg_catalog.timestamp_uint8_eq);

create or replace function pg_catalog.timestamp_uint8_ne(
    timestamptz,
    uint8
) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT as $$ (SELECT $1 <> convert_timestamptz_uint8($2)) $$;
create operator pg_catalog.<>(leftarg = timestamptz, rightarg = uint8, procedure = pg_catalog.timestamp_uint8_ne);

CREATE OR REPLACE FUNCTION pg_catalog.hex(tinyblob) RETURNS text LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'bytea_to_hex';
CREATE OR REPLACE FUNCTION pg_catalog.hex(blob) RETURNS text LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'bytea_to_hex';
CREATE OR REPLACE FUNCTION pg_catalog.hex(mediumblob) RETURNS text LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'bytea_to_hex';
CREATE OR REPLACE FUNCTION pg_catalog.hex(longblob) RETURNS text LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'bytea_to_hex';

CREATE OR REPLACE FUNCTION pg_catalog.binary_out (binary) RETURNS cstring LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'dolphin_binaryout';
CREATE OR REPLACE FUNCTION pg_catalog.varbinary_out (varbinary) RETURNS cstring LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'dolphin_binaryout';

DROP FUNCTION IF EXISTS pg_catalog.double_or_binary(double precision, binary) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.double_or_binary(double precision, binary) RETURNS bigint LANGUAGE SQL IMMUTABLE STRICT as $$ SELECT $1 | $2::numeric $$;
CREATE OPERATOR pg_catalog.|(leftarg = double precision, rightarg = binary, procedure = pg_catalog.double_or_binary);
DROP FUNCTION IF EXISTS pg_catalog.binary_or_double(binary, double precision) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.binary_or_double(binary, double precision) RETURNS bigint LANGUAGE SQL IMMUTABLE STRICT as $$ SELECT $1::numeric | $2 $$;
CREATE OPERATOR pg_catalog.|(leftarg = binary, rightarg = double precision, procedure = pg_catalog.binary_or_double);
DROP FUNCTION IF EXISTS pg_catalog.uint8_or_binary(uint8, binary) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.uint8_or_binary(uint8, binary) RETURNS uint8 LANGUAGE SQL IMMUTABLE STRICT as $$ SELECT $1 | $2::numeric $$;
CREATE OPERATOR pg_catalog.|(leftarg = uint8, rightarg = binary, procedure = pg_catalog.uint8_or_binary);
DROP FUNCTION IF EXISTS pg_catalog.binary_or_uint8(binary, uint8) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.binary_or_uint8(binary, uint8) RETURNS uint8 LANGUAGE SQL IMMUTABLE STRICT as $$ SELECT $1::numeric | $2 $$;
CREATE OPERATOR pg_catalog.|(leftarg = binary, rightarg = uint8, procedure = pg_catalog.binary_or_uint8);
DROP FUNCTION IF EXISTS pg_catalog.binary_or_binary(binary, binary) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.binary_or_binary(binary, binary) RETURNS bigint LANGUAGE SQL IMMUTABLE STRICT as $$ SELECT $1::numeric | $2::numeric $$;
CREATE OPERATOR pg_catalog.|(leftarg = binary, rightarg = binary, procedure = pg_catalog.binary_or_binary);

DROP FUNCTION IF EXISTS pg_catalog.div(binary, double precision);
CREATE OR REPLACE FUNCTION pg_catalog.div(binary, double precision)
returns numeric language plpgsql as
$$
declare
    quotient double precision := $1::numeric / $2;
begin
    return sign(quotient) * floor(abs(quotient));
end;
$$;
DROP FUNCTION IF EXISTS pg_catalog.div(double precision, binary);
CREATE OR REPLACE FUNCTION pg_catalog.div(double precision, binary)
returns numeric language plpgsql as
$$
declare
    quotient double precision := $1 / $2::numeric;
begin
    return sign(quotient) * floor(abs(quotient));
end;
$$;
DROP FUNCTION IF EXISTS pg_catalog.div(binary, binary);
CREATE OR REPLACE FUNCTION pg_catalog.div(binary, binary)
returns numeric language plpgsql as
$$
declare
    quotient double precision := $1::numeric / $2::numeric;
begin
    return sign(quotient) * floor(abs(quotient));
end;
$$;

DROP CAST IF EXISTS (year AS boolean);
DROP FUNCTION IF EXISTS pg_catalog.year_to_bool(year);
CREATE OR REPLACE FUNCTION pg_catalog.year_to_bool(year)
RETURNS boolean LANGUAGE SQL IMMUTABLE STRICT as
'select cast(cast($1 as text) as boolean)';
CREATE CAST (year AS boolean) WITH FUNCTION year_to_bool(year) AS IMPLICIT;

-- not operator
DROP FUNCTION IF EXISTS dolphin_catalog.dolphin_int1not(int1) CASCADE;
CREATE OR REPLACE FUNCTION dolphin_catalog.dolphin_int1not(int1) RETURNS uint8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'dolphin_int1not';
CREATE OPERATOR dolphin_catalog.~(rightarg = int1, procedure = dolphin_catalog.dolphin_int1not);

DROP FUNCTION IF EXISTS dolphin_catalog.dolphin_int2not(int2) CASCADE;
CREATE OR REPLACE FUNCTION dolphin_catalog.dolphin_int2not(int2) RETURNS uint8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'dolphin_int2not';
CREATE OPERATOR dolphin_catalog.~(rightarg = int2, procedure = dolphin_catalog.dolphin_int2not);

DROP FUNCTION IF EXISTS dolphin_catalog.dolphin_int4not(int4) CASCADE;
CREATE OR REPLACE FUNCTION dolphin_catalog.dolphin_int4not(int4) RETURNS uint8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'dolphin_int4not';
CREATE OPERATOR dolphin_catalog.~(rightarg = int4, procedure = dolphin_catalog.dolphin_int4not);

DROP FUNCTION IF EXISTS dolphin_catalog.dolphin_int8not(int8) CASCADE;
CREATE OR REPLACE FUNCTION dolphin_catalog.dolphin_int8not(int8) RETURNS uint8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'dolphin_int8not';
CREATE OPERATOR dolphin_catalog.~(rightarg = int8, procedure = dolphin_catalog.dolphin_int8not);

DROP FUNCTION IF EXISTS dolphin_catalog.dolphin_uint1not(uint1) CASCADE;
CREATE OR REPLACE FUNCTION dolphin_catalog.dolphin_uint1not(uint1) RETURNS uint8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'dolphin_uint1not';
CREATE OPERATOR dolphin_catalog.~(rightarg = uint1, procedure = dolphin_catalog.dolphin_uint1not);

DROP FUNCTION IF EXISTS dolphin_catalog.dolphin_uint2not(uint2) CASCADE;
CREATE OR REPLACE FUNCTION dolphin_catalog.dolphin_uint2not(uint2) RETURNS uint8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'dolphin_uint2not';
CREATE OPERATOR dolphin_catalog.~(rightarg = uint2, procedure = dolphin_catalog.dolphin_uint2not);

DROP FUNCTION IF EXISTS dolphin_catalog.dolphin_uint4not(uint4) CASCADE;
CREATE OR REPLACE FUNCTION dolphin_catalog.dolphin_uint4not(uint4) RETURNS uint8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'dolphin_uint4not';
CREATE OPERATOR dolphin_catalog.~(rightarg = uint4, procedure = dolphin_catalog.dolphin_uint4not);

DROP FUNCTION IF EXISTS dolphin_catalog.dolphin_uint8not(uint8) CASCADE;
CREATE OR REPLACE FUNCTION dolphin_catalog.dolphin_uint8not(uint8) RETURNS uint8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'dolphin_uint8not';
CREATE OPERATOR dolphin_catalog.~(rightarg = uint8, procedure = dolphin_catalog.dolphin_uint8not);

DROP FUNCTION IF EXISTS dolphin_catalog.dolphin_float4not(float4) CASCADE;
CREATE OR REPLACE FUNCTION dolphin_catalog.dolphin_float4not(float4) RETURNS uint8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'dolphin_float4not';
CREATE OPERATOR dolphin_catalog.~(rightarg = float4, procedure = dolphin_catalog.dolphin_float4not);

DROP FUNCTION IF EXISTS dolphin_catalog.dolphin_float8not(float8) CASCADE;
CREATE OR REPLACE FUNCTION dolphin_catalog.dolphin_float8not(float8) RETURNS uint8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'dolphin_float8not';
CREATE OPERATOR dolphin_catalog.~(rightarg = float8, procedure = dolphin_catalog.dolphin_float8not);

DROP FUNCTION IF EXISTS dolphin_catalog.dolphin_numericnot(numeric) CASCADE;
CREATE OR REPLACE FUNCTION dolphin_catalog.dolphin_numericnot(numeric) RETURNS uint8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'dolphin_numericnot';
CREATE OPERATOR dolphin_catalog.~(rightarg = numeric, procedure = dolphin_catalog.dolphin_numericnot);

DROP FUNCTION IF EXISTS dolphin_catalog.dolphin_bitnot(bit) CASCADE;
CREATE OR REPLACE FUNCTION dolphin_catalog.dolphin_bitnot(bit) RETURNS uint8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'dolphin_bitnot';
CREATE OPERATOR dolphin_catalog.~(rightarg = bit, procedure = dolphin_catalog.dolphin_bitnot);

DROP FUNCTION IF EXISTS dolphin_catalog.dolphin_boolnot(boolean) CASCADE;
CREATE OR REPLACE FUNCTION dolphin_catalog.dolphin_boolnot(boolean) RETURNS uint8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'dolphin_boolnot';
CREATE OPERATOR dolphin_catalog.~(rightarg = boolean, procedure = dolphin_catalog.dolphin_boolnot);

DROP FUNCTION IF EXISTS dolphin_catalog.dolphin_datenot(date) CASCADE;
CREATE OR REPLACE FUNCTION dolphin_catalog.dolphin_datenot(date) RETURNS uint8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'dolphin_datenot';
CREATE OPERATOR dolphin_catalog.~(rightarg = date, procedure = dolphin_catalog.dolphin_datenot);

DROP FUNCTION IF EXISTS dolphin_catalog.dolphin_timenot(time) CASCADE;
CREATE OR REPLACE FUNCTION dolphin_catalog.dolphin_timenot(time) RETURNS uint8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'dolphin_timenot';
CREATE OPERATOR dolphin_catalog.~(rightarg = time, procedure = dolphin_catalog.dolphin_timenot);

DROP FUNCTION IF EXISTS dolphin_catalog.dolphin_timestampnot(timestamp without time zone) CASCADE;
CREATE OR REPLACE FUNCTION dolphin_catalog.dolphin_timestampnot(timestamp without time zone) RETURNS uint8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'dolphin_timestampnot';
CREATE OPERATOR dolphin_catalog.~(rightarg = timestamp without time zone, procedure = dolphin_catalog.dolphin_timestampnot);

DROP FUNCTION IF EXISTS dolphin_catalog.dolphin_timestamptznot(timestamp with time zone) CASCADE;
CREATE OR REPLACE FUNCTION dolphin_catalog.dolphin_timestamptznot(timestamp with time zone) RETURNS uint8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'dolphin_timestamptznot';
CREATE OPERATOR dolphin_catalog.~(rightarg = timestamp with time zone, procedure = dolphin_catalog.dolphin_timestamptznot);

DROP FUNCTION IF EXISTS dolphin_catalog.dolphin_yearnot(year) CASCADE;
CREATE OR REPLACE FUNCTION dolphin_catalog.dolphin_yearnot(year) RETURNS uint8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'dolphin_yearnot';
CREATE OPERATOR dolphin_catalog.~(rightarg = year, procedure = dolphin_catalog.dolphin_yearnot);

DROP FUNCTION IF EXISTS dolphin_catalog.dolphin_charnot(char) CASCADE;
CREATE OR REPLACE FUNCTION dolphin_catalog.dolphin_charnot(char) RETURNS uint8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'dolphin_charnot';
CREATE OPERATOR dolphin_catalog.~(rightarg = char, procedure = dolphin_catalog.dolphin_charnot);

DROP FUNCTION IF EXISTS dolphin_catalog.dolphin_varcharnot(varchar) CASCADE;
CREATE OR REPLACE FUNCTION dolphin_catalog.dolphin_varcharnot(varchar) RETURNS uint8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'dolphin_varcharnot';
CREATE OPERATOR dolphin_catalog.~(rightarg = varchar, procedure = dolphin_catalog.dolphin_varcharnot);

DROP FUNCTION IF EXISTS dolphin_catalog.dolphin_textnot(text) CASCADE;
CREATE OR REPLACE FUNCTION dolphin_catalog.dolphin_textnot(text) RETURNS uint8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'dolphin_textnot';
CREATE OPERATOR dolphin_catalog.~(rightarg = text, procedure = dolphin_catalog.dolphin_textnot);

DROP FUNCTION IF EXISTS dolphin_catalog.dolphin_varlenanot(anyelement) CASCADE;
CREATE OR REPLACE FUNCTION dolphin_catalog.dolphin_varlenanot(anyelement) RETURNS uint8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'dolphin_varlenanot';
CREATE OPERATOR dolphin_catalog.~(rightarg = anyelement, procedure = dolphin_catalog.dolphin_varlenanot);

DROP FUNCTION IF EXISTS dolphin_catalog.dolphin_enumnot(anyenum) CASCADE;
CREATE OR REPLACE FUNCTION dolphin_catalog.dolphin_enumnot(anyenum) RETURNS uint8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'dolphin_enumnot';
CREATE OPERATOR dolphin_catalog.~(rightarg = anyenum, procedure = dolphin_catalog.dolphin_enumnot);

DROP FUNCTION IF EXISTS dolphin_catalog.dolphin_setnot(anyset) CASCADE;
CREATE OR REPLACE FUNCTION dolphin_catalog.dolphin_setnot(anyset) RETURNS uint8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'dolphin_setnot';
CREATE OPERATOR dolphin_catalog.~(rightarg = anyset, procedure = dolphin_catalog.dolphin_setnot);

CREATE OR REPLACE FUNCTION pg_catalog.binary_cmp(binary, binary) RETURNS integer LANGUAGE INTERNAL IMMUTABLE STRICT as 'byteacmp';

CREATE OPERATOR FAMILY pg_catalog.binary_ops USING BTREE;
CREATE OPERATOR FAMILY pg_catalog.binary_ops USING HASH;

CREATE OPERATOR CLASS pg_catalog.binary_ops DEFAULT
   FOR TYPE binary USING BTREE FAMILY pg_catalog.binary_ops as
   OPERATOR 1 pg_catalog.<(binary, binary),
   OPERATOR 2 pg_catalog.<=(binary, binary),
   OPERATOR 3 pg_catalog.=(binary, binary),
   OPERATOR 4 pg_catalog.>=(binary, binary),
   OPERATOR 5 pg_catalog.>(binary, binary),
   FUNCTION 1 pg_catalog.binary_cmp(binary, binary),
   FUNCTION 2 pg_catalog.bytea_sortsupport(internal);

CREATE OPERATOR CLASS pg_catalog.binary_ops DEFAULT
   FOR TYPE binary USING HASH FAMILY binary_ops as
   OPERATOR 1 pg_catalog.=(binary, binary),
   FUNCTION 1 (binary, binary) pg_catalog.hashvarlena(internal);

CREATE OR REPLACE FUNCTION pg_catalog.varbinary_cmp(varbinary, varbinary) RETURNS integer LANGUAGE INTERNAL IMMUTABLE STRICT as 'byteacmp';
CREATE OPERATOR FAMILY pg_catalog.varbinary_ops USING BTREE;
CREATE OPERATOR FAMILY pg_catalog.varbinary_ops USING HASH;

CREATE OPERATOR CLASS pg_catalog.varbinary_ops DEFAULT
   FOR TYPE varbinary USING BTREE FAMILY pg_catalog.varbinary_ops as
   OPERATOR 1 pg_catalog.<(varbinary, varbinary),
   OPERATOR 2 pg_catalog.<=(varbinary, varbinary),
   OPERATOR 3 pg_catalog.=(varbinary, varbinary),
   OPERATOR 4 pg_catalog.>=(varbinary, varbinary),
   OPERATOR 5 pg_catalog.>(varbinary, varbinary),
   FUNCTION 1 pg_catalog.varbinary_cmp(varbinary, varbinary),
   FUNCTION 2 pg_catalog.bytea_sortsupport(internal);

CREATE OPERATOR CLASS pg_catalog.varbinary_ops DEFAULT
   FOR TYPE varbinary USING HASH FAMILY pg_catalog.varbinary_ops as
   OPERATOR 1 pg_catalog.=(varbinary, varbinary),
   FUNCTION 1 (varbinary, varbinary) pg_catalog.hashvarlena(internal);

CREATE OR REPLACE FUNCTION pg_catalog.blob_cmp(blob, blob) RETURNS integer LANGUAGE INTERNAL IMMUTABLE STRICT as 'byteacmp';
CREATE OPERATOR FAMILY pg_catalog.blob_ops USING BTREE;
CREATE OPERATOR FAMILY pg_catalog.blob_ops USING HASH;

-- about blob op family
CREATE OPERATOR CLASS pg_catalog.blob_ops DEFAULT
   FOR TYPE blob USING BTREE FAMILY pg_catalog.blob_ops as
   OPERATOR 1 pg_catalog.<(blob, blob),
   OPERATOR 2 pg_catalog.<=(blob, blob),
   OPERATOR 3 pg_catalog.=(blob, blob),
   OPERATOR 4 pg_catalog.>=(blob, blob),
   OPERATOR 5 pg_catalog.>(blob, blob),
   FUNCTION 1 pg_catalog.blob_cmp(blob, blob),
   FUNCTION 2 pg_catalog.bytea_sortsupport(internal);

CREATE OPERATOR CLASS pg_catalog.blob_ops DEFAULT
   FOR TYPE blob USING HASH FAMILY blob_ops as
   OPERATOR 1 pg_catalog.=(blob, blob),
   FUNCTION 1 (blob, blob) pg_catalog.hashvarlena(internal);

-- about tinyblob op family
CREATE OR REPLACE FUNCTION pg_catalog.tinyblob_eq(arg1 tinyblob, arg2 tinyblob) RETURNS bool LANGUAGE INTERNAL STRICT AS 'byteaeq';
CREATE OR REPLACE FUNCTION pg_catalog.tinyblob_ne(arg1 tinyblob, arg2 tinyblob) RETURNS bool LANGUAGE INTERNAL STRICT AS 'byteane';
CREATE OR REPLACE FUNCTION pg_catalog.tinyblob_lt(arg1 tinyblob, arg2 tinyblob) RETURNS bool LANGUAGE INTERNAL STRICT AS 'bytealt';
CREATE OR REPLACE FUNCTION pg_catalog.tinyblob_le(arg1 tinyblob, arg2 tinyblob) RETURNS bool LANGUAGE INTERNAL STRICT AS 'byteale';
CREATE OR REPLACE FUNCTION pg_catalog.tinyblob_gt(arg1 tinyblob, arg2 tinyblob) RETURNS bool LANGUAGE INTERNAL STRICT AS 'byteagt';
CREATE OR REPLACE FUNCTION pg_catalog.tinyblob_ge(arg1 tinyblob, arg2 tinyblob) RETURNS bool LANGUAGE INTERNAL STRICT AS 'byteage';
CREATE OR REPLACE FUNCTION pg_catalog.tinyblob_cmp(tinyblob, tinyblob) RETURNS integer LANGUAGE INTERNAL IMMUTABLE STRICT as 'byteacmp';
CREATE OPERATOR pg_catalog.=(leftarg = tinyblob, rightarg = tinyblob, procedure = tinyblob_eq, restrict = eqsel, join = eqjoinsel);
CREATE OPERATOR pg_catalog.<>(leftarg = tinyblob, rightarg = tinyblob, procedure = tinyblob_ne, restrict = neqsel, join = neqjoinsel);
CREATE OPERATOR pg_catalog.<(leftarg = tinyblob, rightarg = tinyblob, procedure = tinyblob_lt, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.<=(leftarg = tinyblob, rightarg = tinyblob, procedure = tinyblob_le, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.>(leftarg = tinyblob, rightarg = tinyblob, procedure = tinyblob_gt, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.>=(leftarg = tinyblob, rightarg = tinyblob, procedure = tinyblob_ge, restrict = scalarltsel, join = scalarltjoinsel);

CREATE OR REPLACE FUNCTION pg_catalog.tinyblob_eq_text(arg1 tinyblob, arg2 text) RETURNS bool LANGUAGE SQL STRICT AS $$ SELECT tinyblob_eq($1, $2::tinyblob) $$;
CREATE OR REPLACE FUNCTION pg_catalog.tinyblob_ne_text(arg1 tinyblob, arg2 text) RETURNS bool LANGUAGE SQL STRICT AS $$ SELECT tinyblob_ne($1, $2::tinyblob) $$;
CREATE OR REPLACE FUNCTION pg_catalog.tinyblob_lt_text(arg1 tinyblob, arg2 text) RETURNS bool LANGUAGE SQL STRICT AS $$ SELECT tinyblob_lt($1, $2::tinyblob) $$;
CREATE OR REPLACE FUNCTION pg_catalog.tinyblob_le_text(arg1 tinyblob, arg2 text) RETURNS bool LANGUAGE SQL STRICT AS $$ SELECT tinyblob_le($1, $2::tinyblob) $$;
CREATE OR REPLACE FUNCTION pg_catalog.tinyblob_gt_text(arg1 tinyblob, arg2 text) RETURNS bool LANGUAGE SQL STRICT AS $$ SELECT tinyblob_gt($1, $2::tinyblob) $$;
CREATE OR REPLACE FUNCTION pg_catalog.tinyblob_ge_text(arg1 tinyblob, arg2 text) RETURNS bool LANGUAGE SQL STRICT AS $$ SELECT tinyblob_ge($1, $2::tinyblob) $$;
CREATE OPERATOR pg_catalog.=(leftarg = tinyblob, rightarg = text, procedure = tinyblob_eq_text, restrict = eqsel, join = eqjoinsel);
CREATE OPERATOR pg_catalog.<>(leftarg = tinyblob, rightarg = text, procedure = tinyblob_ne_text, restrict = neqsel, join = neqjoinsel);
CREATE OPERATOR pg_catalog.<(leftarg = tinyblob, rightarg = text, procedure = tinyblob_lt_text, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.<=(leftarg = tinyblob, rightarg = text, procedure = tinyblob_le_text, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.>(leftarg = tinyblob, rightarg = text, procedure = tinyblob_gt_text, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.>=(leftarg = tinyblob, rightarg = text, procedure = tinyblob_ge_text, restrict = scalarltsel, join = scalarltjoinsel);

CREATE OR REPLACE FUNCTION pg_catalog.text_eq_tinyblob(arg1 text, arg2 tinyblob) RETURNS bool LANGUAGE SQL STRICT AS $$ SELECT tinyblob_eq($1::tinyblob, $2) $$;
CREATE OR REPLACE FUNCTION pg_catalog.text_ne_tinyblob(arg1 text, arg2 tinyblob) RETURNS bool LANGUAGE SQL STRICT AS $$ SELECT tinyblob_ne($1::tinyblob, $2) $$;
CREATE OR REPLACE FUNCTION pg_catalog.text_lt_tinyblob(arg1 text, arg2 tinyblob) RETURNS bool LANGUAGE SQL STRICT AS $$ SELECT tinyblob_lt($1::tinyblob, $2) $$;
CREATE OR REPLACE FUNCTION pg_catalog.text_le_tinyblob(arg1 text, arg2 tinyblob) RETURNS bool LANGUAGE SQL STRICT AS $$ SELECT tinyblob_le($1::tinyblob, $2) $$;
CREATE OR REPLACE FUNCTION pg_catalog.test_gt_tinyblob(arg1 text, arg2 tinyblob) RETURNS bool LANGUAGE SQL STRICT AS $$ SELECT tinyblob_gt($1::tinyblob, $2) $$;
CREATE OR REPLACE FUNCTION pg_catalog.test_ge_tinyblob(arg1 text, arg2 tinyblob) RETURNS bool LANGUAGE SQL STRICT AS $$ SELECT tinyblob_ge($1::tinyblob, $2) $$;
CREATE OPERATOR pg_catalog.=(leftarg = text, rightarg = tinyblob, procedure = text_eq_tinyblob, restrict = eqsel, join = eqjoinsel);
CREATE OPERATOR pg_catalog.<>(leftarg = text, rightarg = tinyblob, procedure = text_ne_tinyblob, restrict = neqsel, join = neqjoinsel);
CREATE OPERATOR pg_catalog.<(leftarg = text, rightarg = tinyblob, procedure = text_lt_tinyblob, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.<=(leftarg = text, rightarg = tinyblob, procedure = text_le_tinyblob, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.>(leftarg = text, rightarg = tinyblob, procedure = test_gt_tinyblob, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.>=(leftarg = text, rightarg = tinyblob, procedure = test_ge_tinyblob, restrict = scalarltsel, join = scalarltjoinsel);

CREATE OPERATOR FAMILY pg_catalog.tinyblob_ops USING BTREE;
CREATE OPERATOR FAMILY pg_catalog.tinyblob_ops USING HASH;
CREATE OPERATOR CLASS pg_catalog.tinyblob_ops DEFAULT
   FOR TYPE tinyblob USING BTREE FAMILY pg_catalog.tinyblob_ops as
   OPERATOR 1 pg_catalog.<(tinyblob, tinyblob),
   OPERATOR 2 pg_catalog.<=(tinyblob, tinyblob),
   OPERATOR 3 pg_catalog.=(tinyblob, tinyblob),
   OPERATOR 4 pg_catalog.>=(tinyblob, tinyblob),
   OPERATOR 5 pg_catalog.>(tinyblob, tinyblob),
   FUNCTION 1 pg_catalog.tinyblob_cmp(tinyblob, tinyblob),
   FUNCTION 2 pg_catalog.bytea_sortsupport(internal);

CREATE OPERATOR CLASS pg_catalog.tinyblob_ops DEFAULT
   FOR TYPE tinyblob USING HASH FAMILY tinyblob_ops as
   OPERATOR 1 pg_catalog.=(tinyblob, tinyblob),
   FUNCTION 1 (tinyblob, tinyblob) pg_catalog.hashvarlena(internal);

-- about mediumblob operator family
CREATE OR REPLACE FUNCTION pg_catalog.mediumblob_eq(arg1 mediumblob, arg2 mediumblob) RETURNS bool LANGUAGE INTERNAL STRICT AS 'byteaeq';
CREATE OR REPLACE FUNCTION pg_catalog.mediumblob_ne(arg1 mediumblob, arg2 mediumblob) RETURNS bool LANGUAGE INTERNAL STRICT AS 'byteane';
CREATE OR REPLACE FUNCTION pg_catalog.mediumblob_lt(arg1 mediumblob, arg2 mediumblob) RETURNS bool LANGUAGE INTERNAL STRICT AS 'bytealt';
CREATE OR REPLACE FUNCTION pg_catalog.mediumblob_le(arg1 mediumblob, arg2 mediumblob) RETURNS bool LANGUAGE INTERNAL STRICT AS 'byteale';
CREATE OR REPLACE FUNCTION pg_catalog.mediumblob_gt(arg1 mediumblob, arg2 mediumblob) RETURNS bool LANGUAGE INTERNAL STRICT AS 'byteagt';
CREATE OR REPLACE FUNCTION pg_catalog.mediumblob_ge(arg1 mediumblob, arg2 mediumblob) RETURNS bool LANGUAGE INTERNAL STRICT AS 'byteage';
CREATE OR REPLACE FUNCTION pg_catalog.mediumblob_cmp(mediumblob, mediumblob) RETURNS integer LANGUAGE INTERNAL IMMUTABLE STRICT as 'byteacmp';
CREATE OPERATOR pg_catalog.=(leftarg = mediumblob, rightarg = mediumblob, procedure = mediumblob_eq, restrict = eqsel, join = eqjoinsel);
CREATE OPERATOR pg_catalog.<>(leftarg = mediumblob, rightarg = mediumblob, procedure = mediumblob_ne, restrict = neqsel, join = neqjoinsel);
CREATE OPERATOR pg_catalog.<(leftarg = mediumblob, rightarg = mediumblob, procedure = mediumblob_lt, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.<=(leftarg = mediumblob, rightarg = mediumblob, procedure = mediumblob_le, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.>(leftarg = mediumblob, rightarg = mediumblob, procedure = mediumblob_gt, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.>=(leftarg = mediumblob, rightarg = mediumblob, procedure = mediumblob_ge, restrict = scalarltsel, join = scalarltjoinsel);

CREATE OR REPLACE FUNCTION pg_catalog.mediumblob_eq_text(arg1 mediumblob, arg2 text) RETURNS bool LANGUAGE SQL STRICT AS $$ SELECT mediumblob_eq($1, $2::mediumblob) $$;
CREATE OR REPLACE FUNCTION pg_catalog.mediumblob_ne_text(arg1 mediumblob, arg2 text) RETURNS bool LANGUAGE SQL STRICT AS $$ SELECT mediumblob_ne($1, $2::mediumblob) $$;
CREATE OR REPLACE FUNCTION pg_catalog.mediumblob_lt_text(arg1 mediumblob, arg2 text) RETURNS bool LANGUAGE SQL STRICT AS $$ SELECT mediumblob_lt($1, $2::mediumblob) $$;
CREATE OR REPLACE FUNCTION pg_catalog.mediumblob_le_text(arg1 mediumblob, arg2 text) RETURNS bool LANGUAGE SQL STRICT AS $$ SELECT mediumblob_le($1, $2::mediumblob) $$;
CREATE OR REPLACE FUNCTION pg_catalog.mediumblob_gt_text(arg1 mediumblob, arg2 text) RETURNS bool LANGUAGE SQL STRICT AS $$ SELECT mediumblob_gt($1, $2::mediumblob) $$;
CREATE OR REPLACE FUNCTION pg_catalog.mediumblob_ge_text(arg1 mediumblob, arg2 text) RETURNS bool LANGUAGE SQL STRICT AS $$ SELECT mediumblob_ge($1, $2::mediumblob) $$;
CREATE OPERATOR pg_catalog.=(leftarg = mediumblob, rightarg = text, procedure = mediumblob_eq_text, restrict = eqsel, join = eqjoinsel);
CREATE OPERATOR pg_catalog.<>(leftarg = mediumblob, rightarg = text, procedure = mediumblob_ne_text, restrict = neqsel, join = neqjoinsel);
CREATE OPERATOR pg_catalog.<(leftarg = mediumblob, rightarg = text, procedure = mediumblob_lt_text, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.<=(leftarg = mediumblob, rightarg = text, procedure = mediumblob_le_text, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.>(leftarg = mediumblob, rightarg = text, procedure = mediumblob_gt_text, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.>=(leftarg = mediumblob, rightarg = text, procedure = mediumblob_ge_text, restrict = scalarltsel, join = scalarltjoinsel);

CREATE OR REPLACE FUNCTION pg_catalog.text_eq_mediumblob(arg1 text, arg2 mediumblob) RETURNS bool LANGUAGE SQL STRICT AS $$ SELECT mediumblob_eq($1::mediumblob, $2) $$;
CREATE OR REPLACE FUNCTION pg_catalog.text_ne_mediumblob(arg1 text, arg2 mediumblob) RETURNS bool LANGUAGE SQL STRICT AS $$ SELECT mediumblob_ne($1::mediumblob, $2) $$;
CREATE OR REPLACE FUNCTION pg_catalog.text_lt_mediumblob(arg1 text, arg2 mediumblob) RETURNS bool LANGUAGE SQL STRICT AS $$ SELECT mediumblob_lt($1::mediumblob, $2) $$;
CREATE OR REPLACE FUNCTION pg_catalog.text_le_mediumblob(arg1 text, arg2 mediumblob) RETURNS bool LANGUAGE SQL STRICT AS $$ SELECT mediumblob_le($1::mediumblob, $2) $$;
CREATE OR REPLACE FUNCTION pg_catalog.test_gt_mediumblob(arg1 text, arg2 mediumblob) RETURNS bool LANGUAGE SQL STRICT AS $$ SELECT mediumblob_gt($1::mediumblob, $2) $$;
CREATE OR REPLACE FUNCTION pg_catalog.test_ge_mediumblob(arg1 text, arg2 mediumblob) RETURNS bool LANGUAGE SQL STRICT AS $$ SELECT mediumblob_ge($1::mediumblob, $2) $$;
CREATE OPERATOR pg_catalog.=(leftarg = text, rightarg = mediumblob, procedure = text_eq_mediumblob, restrict = eqsel, join = eqjoinsel);
CREATE OPERATOR pg_catalog.<>(leftarg = text, rightarg = mediumblob, procedure = text_ne_mediumblob, restrict = neqsel, join = neqjoinsel);
CREATE OPERATOR pg_catalog.<(leftarg = text, rightarg = mediumblob, procedure = text_lt_mediumblob, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.<=(leftarg = text, rightarg = mediumblob, procedure = text_le_mediumblob, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.>(leftarg = text, rightarg = mediumblob, procedure = test_gt_mediumblob, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.>=(leftarg = text, rightarg = mediumblob, procedure = test_ge_mediumblob, restrict = scalarltsel, join = scalarltjoinsel);

CREATE OPERATOR FAMILY pg_catalog.mediumblob_ops USING BTREE;
CREATE OPERATOR FAMILY pg_catalog.mediumblob_ops USING HASH;
CREATE OPERATOR CLASS pg_catalog.mediumblob_ops DEFAULT
   FOR TYPE mediumblob USING BTREE FAMILY pg_catalog.mediumblob_ops as
   OPERATOR 1 pg_catalog.<(mediumblob, mediumblob),
   OPERATOR 2 pg_catalog.<=(mediumblob, mediumblob),
   OPERATOR 3 pg_catalog.=(mediumblob, mediumblob),
   OPERATOR 4 pg_catalog.>=(mediumblob, mediumblob),
   OPERATOR 5 pg_catalog.>(mediumblob, mediumblob),
   FUNCTION 1 pg_catalog.mediumblob_cmp(mediumblob, mediumblob),
   FUNCTION 2 pg_catalog.bytea_sortsupport(internal);

CREATE OPERATOR CLASS pg_catalog.mediumblob_ops DEFAULT
   FOR TYPE mediumblob USING HASH FAMILY mediumblob_ops as
   OPERATOR 1 pg_catalog.=(mediumblob, mediumblob),
   FUNCTION 1 (mediumblob, mediumblob) pg_catalog.hashvarlena(internal);

-- about longblob operator family
CREATE OR REPLACE FUNCTION pg_catalog.longblob_eq(arg1 longblob, arg2 longblob) RETURNS bool LANGUAGE INTERNAL STRICT AS 'byteaeq';
CREATE OR REPLACE FUNCTION pg_catalog.longblob_ne(arg1 longblob, arg2 longblob) RETURNS bool LANGUAGE INTERNAL STRICT AS 'byteane';
CREATE OR REPLACE FUNCTION pg_catalog.longblob_lt(arg1 longblob, arg2 longblob) RETURNS bool LANGUAGE INTERNAL STRICT AS 'bytealt';
CREATE OR REPLACE FUNCTION pg_catalog.longblob_le(arg1 longblob, arg2 longblob) RETURNS bool LANGUAGE INTERNAL STRICT AS 'byteale';
CREATE OR REPLACE FUNCTION pg_catalog.longblob_gt(arg1 longblob, arg2 longblob) RETURNS bool LANGUAGE INTERNAL STRICT AS 'byteagt';
CREATE OR REPLACE FUNCTION pg_catalog.longblob_ge(arg1 longblob, arg2 longblob) RETURNS bool LANGUAGE INTERNAL STRICT AS 'byteage';
CREATE OR REPLACE FUNCTION pg_catalog.longblob_cmp(longblob, longblob) RETURNS integer LANGUAGE INTERNAL IMMUTABLE STRICT as 'byteacmp';
CREATE OPERATOR pg_catalog.=(leftarg = longblob, rightarg = longblob, procedure = longblob_eq, restrict = eqsel, join = eqjoinsel);
CREATE OPERATOR pg_catalog.<>(leftarg = longblob, rightarg = longblob, procedure = longblob_ne, restrict = neqsel, join = neqjoinsel);
CREATE OPERATOR pg_catalog.<(leftarg = longblob, rightarg = longblob, procedure = longblob_lt, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.<=(leftarg = longblob, rightarg = longblob, procedure = longblob_le, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.>(leftarg = longblob, rightarg = longblob, procedure = longblob_gt, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.>=(leftarg = longblob, rightarg = longblob, procedure = longblob_ge, restrict = scalarltsel, join = scalarltjoinsel);

CREATE OR REPLACE FUNCTION pg_catalog.longblob_eq_text(arg1 longblob, arg2 text) RETURNS bool LANGUAGE SQL STRICT AS $$ SELECT longblob_eq($1, $2::longblob) $$;
CREATE OR REPLACE FUNCTION pg_catalog.longblob_ne_text(arg1 longblob, arg2 text) RETURNS bool LANGUAGE SQL STRICT AS $$ SELECT longblob_ne($1, $2::longblob) $$;
CREATE OR REPLACE FUNCTION pg_catalog.longblob_lt_text(arg1 longblob, arg2 text) RETURNS bool LANGUAGE SQL STRICT AS $$ SELECT longblob_lt($1, $2::longblob) $$;
CREATE OR REPLACE FUNCTION pg_catalog.longblob_le_text(arg1 longblob, arg2 text) RETURNS bool LANGUAGE SQL STRICT AS $$ SELECT longblob_le($1, $2::longblob) $$;
CREATE OR REPLACE FUNCTION pg_catalog.longblob_gt_text(arg1 longblob, arg2 text) RETURNS bool LANGUAGE SQL STRICT AS $$ SELECT longblob_gt($1, $2::longblob) $$;
CREATE OR REPLACE FUNCTION pg_catalog.longblob_ge_text(arg1 longblob, arg2 text) RETURNS bool LANGUAGE SQL STRICT AS $$ SELECT longblob_ge($1, $2::longblob) $$;
CREATE OPERATOR pg_catalog.=(leftarg = longblob, rightarg = text, procedure = longblob_eq_text, restrict = eqsel, join = eqjoinsel);
CREATE OPERATOR pg_catalog.<>(leftarg = longblob, rightarg = text, procedure = longblob_ne_text, restrict = neqsel, join = neqjoinsel);
CREATE OPERATOR pg_catalog.<(leftarg = longblob, rightarg = text, procedure = longblob_lt_text, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.<=(leftarg = longblob, rightarg = text, procedure = longblob_le_text, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.>(leftarg = longblob, rightarg = text, procedure = longblob_gt_text, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.>=(leftarg = longblob, rightarg = text, procedure = longblob_ge_text, restrict = scalarltsel, join = scalarltjoinsel);

CREATE OR REPLACE FUNCTION pg_catalog.text_eq_longblob(arg1 text, arg2 longblob) RETURNS bool LANGUAGE SQL STRICT AS $$ SELECT longblob_eq($1::longblob, $2) $$;
CREATE OR REPLACE FUNCTION pg_catalog.text_ne_longblob(arg1 text, arg2 longblob) RETURNS bool LANGUAGE SQL STRICT AS $$ SELECT longblob_ne($1::longblob, $2) $$;
CREATE OR REPLACE FUNCTION pg_catalog.text_lt_longblob(arg1 text, arg2 longblob) RETURNS bool LANGUAGE SQL STRICT AS $$ SELECT longblob_lt($1::longblob, $2) $$;
CREATE OR REPLACE FUNCTION pg_catalog.text_le_longblob(arg1 text, arg2 longblob) RETURNS bool LANGUAGE SQL STRICT AS $$ SELECT longblob_le($1::longblob, $2) $$;
CREATE OR REPLACE FUNCTION pg_catalog.test_gt_longblob(arg1 text, arg2 longblob) RETURNS bool LANGUAGE SQL STRICT AS $$ SELECT longblob_gt($1::longblob, $2) $$;
CREATE OR REPLACE FUNCTION pg_catalog.test_ge_longblob(arg1 text, arg2 longblob) RETURNS bool LANGUAGE SQL STRICT AS $$ SELECT longblob_ge($1::longblob, $2) $$;
CREATE OPERATOR pg_catalog.=(leftarg = text, rightarg = longblob, procedure = text_eq_longblob, restrict = eqsel, join = eqjoinsel);
CREATE OPERATOR pg_catalog.<>(leftarg = text, rightarg = longblob, procedure = text_ne_longblob, restrict = neqsel, join = neqjoinsel);
CREATE OPERATOR pg_catalog.<(leftarg = text, rightarg = longblob, procedure = text_lt_longblob, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.<=(leftarg = text, rightarg = longblob, procedure = text_le_longblob, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.>(leftarg = text, rightarg = longblob, procedure = test_gt_longblob, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.>=(leftarg = text, rightarg = longblob, procedure = test_ge_longblob, restrict = scalarltsel, join = scalarltjoinsel);

CREATE OPERATOR FAMILY pg_catalog.longblob_ops USING BTREE;
CREATE OPERATOR FAMILY pg_catalog.longblob_ops USING HASH;
CREATE OPERATOR CLASS pg_catalog.longblob_ops DEFAULT
   FOR TYPE longblob USING BTREE FAMILY pg_catalog.longblob_ops as
   OPERATOR 1 pg_catalog.<(longblob, longblob),
   OPERATOR 2 pg_catalog.<=(longblob, longblob),
   OPERATOR 3 pg_catalog.=(longblob, longblob),
   OPERATOR 4 pg_catalog.>=(longblob, longblob),
   OPERATOR 5 pg_catalog.>(longblob, longblob),
   FUNCTION 1 pg_catalog.longblob_cmp(longblob, longblob),
   FUNCTION 2 pg_catalog.bytea_sortsupport(internal);

CREATE OPERATOR CLASS pg_catalog.longblob_ops DEFAULT
   FOR TYPE longblob USING HASH FAMILY longblob_ops as
   OPERATOR 1 pg_catalog.=(longblob, longblob),
   FUNCTION 1 (longblob, longblob) pg_catalog.hashvarlena(internal);

-- about tinyblob op othersBlob
-- tinyblob op blob
CREATE OR REPLACE FUNCTION pg_catalog.tinyblob_blob_eq(arg1 tinyblob, arg2 blob) RETURNS bool LANGUAGE INTERNAL STRICT AS 'byteaeq';
CREATE OR REPLACE FUNCTION pg_catalog.tinyblob_blob_ne(arg1 tinyblob, arg2 blob) RETURNS bool LANGUAGE INTERNAL STRICT AS 'byteane';
CREATE OR REPLACE FUNCTION pg_catalog.tinyblob_blob_lt(arg1 tinyblob, arg2 blob) RETURNS bool LANGUAGE INTERNAL STRICT AS 'bytealt';
CREATE OR REPLACE FUNCTION pg_catalog.tinyblob_blob_le(arg1 tinyblob, arg2 blob) RETURNS bool LANGUAGE INTERNAL STRICT AS 'byteale';
CREATE OR REPLACE FUNCTION pg_catalog.tinyblob_blob_gt(arg1 tinyblob, arg2 blob) RETURNS bool LANGUAGE INTERNAL STRICT AS 'byteagt';
CREATE OR REPLACE FUNCTION pg_catalog.tinyblob_blob_ge(arg1 tinyblob, arg2 blob) RETURNS bool LANGUAGE INTERNAL STRICT AS 'byteage';
CREATE OR REPLACE FUNCTION pg_catalog.tinyblob_blob_cmp(tinyblob, blob) RETURNS integer LANGUAGE INTERNAL IMMUTABLE STRICT as 'byteacmp';
CREATE OPERATOR pg_catalog.=(leftarg = tinyblob, rightarg = blob, procedure = tinyblob_blob_eq, restrict = eqsel, join = eqjoinsel);
CREATE OPERATOR pg_catalog.<>(leftarg = tinyblob, rightarg = blob, procedure = tinyblob_blob_ne, restrict = neqsel, join = neqjoinsel);
CREATE OPERATOR pg_catalog.<(leftarg = tinyblob, rightarg = blob, procedure = tinyblob_blob_lt, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.<=(leftarg = tinyblob, rightarg = blob, procedure = tinyblob_blob_le, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.>(leftarg = tinyblob, rightarg = blob, procedure = tinyblob_blob_gt, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.>=(leftarg = tinyblob, rightarg = blob, procedure = tinyblob_blob_ge, restrict = scalarltsel, join = scalarltjoinsel);
-- tinyblob op mediumblob
CREATE OR REPLACE FUNCTION pg_catalog.tinyblob_mediumblob_eq(arg1 tinyblob, arg2 mediumblob) RETURNS bool LANGUAGE INTERNAL STRICT AS 'byteaeq';
CREATE OR REPLACE FUNCTION pg_catalog.tinyblob_mediumblob_ne(arg1 tinyblob, arg2 mediumblob) RETURNS bool LANGUAGE INTERNAL STRICT AS 'byteane';
CREATE OR REPLACE FUNCTION pg_catalog.tinyblob_mediumblob_lt(arg1 tinyblob, arg2 mediumblob) RETURNS bool LANGUAGE INTERNAL STRICT AS 'bytealt';
CREATE OR REPLACE FUNCTION pg_catalog.tinyblob_mediumblob_le(arg1 tinyblob, arg2 mediumblob) RETURNS bool LANGUAGE INTERNAL STRICT AS 'byteale';
CREATE OR REPLACE FUNCTION pg_catalog.tinyblob_mediumblob_gt(arg1 tinyblob, arg2 mediumblob) RETURNS bool LANGUAGE INTERNAL STRICT AS 'byteagt';
CREATE OR REPLACE FUNCTION pg_catalog.tinyblob_mediumblob_ge(arg1 tinyblob, arg2 mediumblob) RETURNS bool LANGUAGE INTERNAL STRICT AS 'byteage';
CREATE OR REPLACE FUNCTION pg_catalog.tinyblob_mediumblob_cmp(tinyblob, mediumblob) RETURNS integer LANGUAGE INTERNAL IMMUTABLE STRICT as 'byteacmp';
CREATE OPERATOR pg_catalog.=(leftarg = tinyblob, rightarg = mediumblob, procedure = tinyblob_mediumblob_eq, restrict = eqsel, join = eqjoinsel);
CREATE OPERATOR pg_catalog.<>(leftarg = tinyblob, rightarg = mediumblob, procedure = tinyblob_mediumblob_ne, restrict = neqsel, join = neqjoinsel);
CREATE OPERATOR pg_catalog.<(leftarg = tinyblob, rightarg = mediumblob, procedure = tinyblob_mediumblob_lt, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.<=(leftarg = tinyblob, rightarg = mediumblob, procedure = tinyblob_mediumblob_le, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.>(leftarg = tinyblob, rightarg = mediumblob, procedure = tinyblob_mediumblob_gt, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.>=(leftarg = tinyblob, rightarg = mediumblob, procedure = tinyblob_mediumblob_ge, restrict = scalarltsel, join = scalarltjoinsel);
-- tinyblob op longblob
CREATE OR REPLACE FUNCTION pg_catalog.tinyblob_longblob_eq(arg1 tinyblob, arg2 longblob) RETURNS bool LANGUAGE INTERNAL STRICT AS 'byteaeq';
CREATE OR REPLACE FUNCTION pg_catalog.tinyblob_longblob_ne(arg1 tinyblob, arg2 longblob) RETURNS bool LANGUAGE INTERNAL STRICT AS 'byteane';
CREATE OR REPLACE FUNCTION pg_catalog.tinyblob_longblob_lt(arg1 tinyblob, arg2 longblob) RETURNS bool LANGUAGE INTERNAL STRICT AS 'bytealt';
CREATE OR REPLACE FUNCTION pg_catalog.tinyblob_longblob_le(arg1 tinyblob, arg2 longblob) RETURNS bool LANGUAGE INTERNAL STRICT AS 'byteale';
CREATE OR REPLACE FUNCTION pg_catalog.tinyblob_longblob_gt(arg1 tinyblob, arg2 longblob) RETURNS bool LANGUAGE INTERNAL STRICT AS 'byteagt';
CREATE OR REPLACE FUNCTION pg_catalog.tinyblob_longblob_ge(arg1 tinyblob, arg2 longblob) RETURNS bool LANGUAGE INTERNAL STRICT AS 'byteage';
CREATE OR REPLACE FUNCTION pg_catalog.tinyblob_longblob_cmp(tinyblob, longblob) RETURNS integer LANGUAGE INTERNAL IMMUTABLE STRICT as 'byteacmp';
CREATE OPERATOR pg_catalog.=(leftarg = tinyblob, rightarg = longblob, procedure = tinyblob_longblob_eq, restrict = eqsel, join = eqjoinsel);
CREATE OPERATOR pg_catalog.<>(leftarg = tinyblob, rightarg = longblob, procedure = tinyblob_longblob_ne, restrict = neqsel, join = neqjoinsel);
CREATE OPERATOR pg_catalog.<(leftarg = tinyblob, rightarg = longblob, procedure = tinyblob_longblob_lt, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.<=(leftarg = tinyblob, rightarg = longblob, procedure = tinyblob_longblob_le, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.>(leftarg = tinyblob, rightarg = longblob, procedure = tinyblob_longblob_gt, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.>=(leftarg = tinyblob, rightarg = longblob, procedure = tinyblob_longblob_ge, restrict = scalarltsel, join = scalarltjoinsel);

-- about blob op othersBlob
-- blob op tinyblob
CREATE OR REPLACE FUNCTION pg_catalog.blob_tinyblob_eq(arg1 blob, arg2 tinyblob) RETURNS bool LANGUAGE INTERNAL STRICT AS 'byteaeq';
CREATE OR REPLACE FUNCTION pg_catalog.blob_tinyblob_ne(arg1 blob, arg2 tinyblob) RETURNS bool LANGUAGE INTERNAL STRICT AS 'byteane';
CREATE OR REPLACE FUNCTION pg_catalog.blob_tinyblob_lt(arg1 blob, arg2 tinyblob) RETURNS bool LANGUAGE INTERNAL STRICT AS 'bytealt';
CREATE OR REPLACE FUNCTION pg_catalog.blob_tinyblob_le(arg1 blob, arg2 tinyblob) RETURNS bool LANGUAGE INTERNAL STRICT AS 'byteale';
CREATE OR REPLACE FUNCTION pg_catalog.blob_tinyblob_gt(arg1 blob, arg2 tinyblob) RETURNS bool LANGUAGE INTERNAL STRICT AS 'byteagt';
CREATE OR REPLACE FUNCTION pg_catalog.blob_tinyblob_ge(arg1 blob, arg2 tinyblob) RETURNS bool LANGUAGE INTERNAL STRICT AS 'byteage';
CREATE OR REPLACE FUNCTION pg_catalog.blob_tinyblob_cmp(blob, tinyblob) RETURNS integer LANGUAGE INTERNAL IMMUTABLE STRICT as 'byteacmp';
CREATE OPERATOR pg_catalog.=(leftarg = blob, rightarg = tinyblob, procedure = blob_tinyblob_eq, restrict = eqsel, join = eqjoinsel);
CREATE OPERATOR pg_catalog.<>(leftarg = blob, rightarg = tinyblob, procedure = blob_tinyblob_ne, restrict = neqsel, join = neqjoinsel);
CREATE OPERATOR pg_catalog.<(leftarg = blob, rightarg = tinyblob, procedure = blob_tinyblob_lt, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.<=(leftarg = blob, rightarg = tinyblob, procedure = blob_tinyblob_le, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.>(leftarg = blob, rightarg = tinyblob, procedure = blob_tinyblob_gt, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.>=(leftarg = blob, rightarg = tinyblob, procedure = blob_tinyblob_ge, restrict = scalarltsel, join = scalarltjoinsel);
--- blob op mediumblob
CREATE OR REPLACE FUNCTION pg_catalog.blob_mediumblob_eq(arg1 blob, arg2 mediumblob) RETURNS bool LANGUAGE INTERNAL STRICT AS 'byteaeq';
CREATE OR REPLACE FUNCTION pg_catalog.blob_mediumblob_ne(arg1 blob, arg2 mediumblob) RETURNS bool LANGUAGE INTERNAL STRICT AS 'byteane';
CREATE OR REPLACE FUNCTION pg_catalog.blob_mediumblob_lt(arg1 blob, arg2 mediumblob) RETURNS bool LANGUAGE INTERNAL STRICT AS 'bytealt';
CREATE OR REPLACE FUNCTION pg_catalog.blob_mediumblob_le(arg1 blob, arg2 mediumblob) RETURNS bool LANGUAGE INTERNAL STRICT AS 'byteale';
CREATE OR REPLACE FUNCTION pg_catalog.blob_mediumblob_gt(arg1 blob, arg2 mediumblob) RETURNS bool LANGUAGE INTERNAL STRICT AS 'byteagt';
CREATE OR REPLACE FUNCTION pg_catalog.blob_mediumblob_ge(arg1 blob, arg2 mediumblob) RETURNS bool LANGUAGE INTERNAL STRICT AS 'byteage';
CREATE OR REPLACE FUNCTION pg_catalog.blob_mediumblob_cmp(blob, mediumblob) RETURNS integer LANGUAGE INTERNAL IMMUTABLE STRICT as 'byteacmp';
CREATE OPERATOR pg_catalog.=(leftarg = blob, rightarg = mediumblob, procedure = blob_mediumblob_eq, restrict = eqsel, join = eqjoinsel);
CREATE OPERATOR pg_catalog.<>(leftarg = blob, rightarg = mediumblob, procedure = blob_mediumblob_ne, restrict = neqsel, join = neqjoinsel);
CREATE OPERATOR pg_catalog.<(leftarg = blob, rightarg = mediumblob, procedure = blob_mediumblob_lt, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.<=(leftarg = blob, rightarg = mediumblob, procedure = blob_mediumblob_le, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.>(leftarg = blob, rightarg = mediumblob, procedure = blob_mediumblob_gt, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.>=(leftarg = blob, rightarg = mediumblob, procedure = blob_mediumblob_ge, restrict = scalarltsel, join = scalarltjoinsel);
-- blob op longblob
CREATE OR REPLACE FUNCTION pg_catalog.blob_longblob_eq(arg1 blob, arg2 longblob) RETURNS bool LANGUAGE INTERNAL STRICT AS 'byteaeq';
CREATE OR REPLACE FUNCTION pg_catalog.blob_longblob_ne(arg1 blob, arg2 longblob) RETURNS bool LANGUAGE INTERNAL STRICT AS 'byteane';
CREATE OR REPLACE FUNCTION pg_catalog.blob_longblob_lt(arg1 blob, arg2 longblob) RETURNS bool LANGUAGE INTERNAL STRICT AS 'bytealt';
CREATE OR REPLACE FUNCTION pg_catalog.blob_longblob_le(arg1 blob, arg2 longblob) RETURNS bool LANGUAGE INTERNAL STRICT AS 'byteale';
CREATE OR REPLACE FUNCTION pg_catalog.blob_longblob_gt(arg1 blob, arg2 longblob) RETURNS bool LANGUAGE INTERNAL STRICT AS 'byteagt';
CREATE OR REPLACE FUNCTION pg_catalog.blob_longblob_ge(arg1 blob, arg2 longblob) RETURNS bool LANGUAGE INTERNAL STRICT AS 'byteage';
CREATE OR REPLACE FUNCTION pg_catalog.blob_longblob_cmp(longblob, longblob) RETURNS integer LANGUAGE INTERNAL IMMUTABLE STRICT as 'byteacmp';
CREATE OPERATOR pg_catalog.=(leftarg = blob, rightarg = longblob, procedure = blob_longblob_eq, restrict = eqsel, join = eqjoinsel);
CREATE OPERATOR pg_catalog.<>(leftarg = blob, rightarg = longblob, procedure = blob_longblob_ne, restrict = neqsel, join = neqjoinsel);
CREATE OPERATOR pg_catalog.<(leftarg = blob, rightarg = longblob, procedure = blob_longblob_lt, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.<=(leftarg = blob, rightarg = longblob, procedure = blob_longblob_le, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.>(leftarg = blob, rightarg = longblob, procedure = blob_longblob_gt, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.>=(leftarg = blob, rightarg = longblob, procedure = blob_longblob_ge, restrict = scalarltsel, join = scalarltjoinsel);

-- about mediumblob op othersBlob
-- mediumblob op tinyblob
CREATE OR REPLACE FUNCTION pg_catalog.mediumblob_tinyblob_eq(arg1 mediumblob, arg2 tinyblob) RETURNS bool LANGUAGE INTERNAL STRICT AS 'byteaeq';
CREATE OR REPLACE FUNCTION pg_catalog.mediumblob_tinyblob_ne(arg1 mediumblob, arg2 tinyblob) RETURNS bool LANGUAGE INTERNAL STRICT AS 'byteane';
CREATE OR REPLACE FUNCTION pg_catalog.mediumblob_tinyblob_lt(arg1 mediumblob, arg2 tinyblob) RETURNS bool LANGUAGE INTERNAL STRICT AS 'bytealt';
CREATE OR REPLACE FUNCTION pg_catalog.mediumblob_tinyblob_le(arg1 mediumblob, arg2 tinyblob) RETURNS bool LANGUAGE INTERNAL STRICT AS 'byteale';
CREATE OR REPLACE FUNCTION pg_catalog.mediumblob_tinyblob_gt(arg1 mediumblob, arg2 tinyblob) RETURNS bool LANGUAGE INTERNAL STRICT AS 'byteagt';
CREATE OR REPLACE FUNCTION pg_catalog.mediumblob_tinyblob_ge(arg1 mediumblob, arg2 tinyblob) RETURNS bool LANGUAGE INTERNAL STRICT AS 'byteage';
CREATE OR REPLACE FUNCTION pg_catalog.mediumblob_tinyblob_cmp(mediumblob, tinyblob) RETURNS integer LANGUAGE INTERNAL IMMUTABLE STRICT as 'byteacmp';
CREATE OPERATOR pg_catalog.=(leftarg = mediumblob, rightarg = tinyblob, procedure = mediumblob_tinyblob_eq, restrict = eqsel, join = eqjoinsel);
CREATE OPERATOR pg_catalog.<>(leftarg = mediumblob, rightarg = tinyblob, procedure = mediumblob_tinyblob_ne, restrict = neqsel, join = neqjoinsel);
CREATE OPERATOR pg_catalog.<(leftarg = mediumblob, rightarg = tinyblob, procedure = mediumblob_tinyblob_lt, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.<=(leftarg = mediumblob, rightarg = tinyblob, procedure = mediumblob_tinyblob_le, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.>(leftarg = mediumblob, rightarg = tinyblob, procedure = mediumblob_tinyblob_gt, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.>=(leftarg = mediumblob, rightarg = tinyblob, procedure = mediumblob_tinyblob_ge, restrict = scalarltsel, join = scalarltjoinsel);
-- mediumblob op blob
CREATE OR REPLACE FUNCTION pg_catalog.mediumblob_blob_eq(arg1 mediumblob, arg2 blob) RETURNS bool LANGUAGE INTERNAL STRICT AS 'byteaeq';
CREATE OR REPLACE FUNCTION pg_catalog.mediumblob_blob_ne(arg1 mediumblob, arg2 blob) RETURNS bool LANGUAGE INTERNAL STRICT AS 'byteane';
CREATE OR REPLACE FUNCTION pg_catalog.mediumblob_blob_lt(arg1 mediumblob, arg2 blob) RETURNS bool LANGUAGE INTERNAL STRICT AS 'bytealt';
CREATE OR REPLACE FUNCTION pg_catalog.mediumblob_blob_le(arg1 mediumblob, arg2 blob) RETURNS bool LANGUAGE INTERNAL STRICT AS 'byteale';
CREATE OR REPLACE FUNCTION pg_catalog.mediumblob_blob_gt(arg1 mediumblob, arg2 blob) RETURNS bool LANGUAGE INTERNAL STRICT AS 'byteagt';
CREATE OR REPLACE FUNCTION pg_catalog.mediumblob_blob_ge(arg1 mediumblob, arg2 blob) RETURNS bool LANGUAGE INTERNAL STRICT AS 'byteage';
CREATE OR REPLACE FUNCTION pg_catalog.mediumblob_blob_cmp(mediumblob, blob) RETURNS integer LANGUAGE INTERNAL IMMUTABLE STRICT as 'byteacmp';
CREATE OPERATOR pg_catalog.=(leftarg = mediumblob, rightarg = blob, procedure = mediumblob_blob_eq, restrict = eqsel, join = eqjoinsel);
CREATE OPERATOR pg_catalog.<>(leftarg = mediumblob, rightarg = blob, procedure = mediumblob_blob_ne, restrict = neqsel, join = neqjoinsel);
CREATE OPERATOR pg_catalog.<(leftarg = mediumblob, rightarg = blob, procedure = mediumblob_blob_lt, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.<=(leftarg = mediumblob, rightarg = blob, procedure = mediumblob_blob_le, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.>(leftarg = mediumblob, rightarg = blob, procedure = mediumblob_blob_gt, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.>=(leftarg = mediumblob, rightarg = blob, procedure = mediumblob_blob_ge, restrict = scalarltsel, join = scalarltjoinsel);
-- mediumblob op longblob
CREATE OR REPLACE FUNCTION pg_catalog.mediumblob_longblob_eq(arg1 mediumblob, arg2 longblob) RETURNS bool LANGUAGE INTERNAL STRICT AS 'byteaeq';
CREATE OR REPLACE FUNCTION pg_catalog.mediumblob_longblob_ne(arg1 mediumblob, arg2 longblob) RETURNS bool LANGUAGE INTERNAL STRICT AS 'byteane';
CREATE OR REPLACE FUNCTION pg_catalog.mediumblob_longblob_lt(arg1 mediumblob, arg2 longblob) RETURNS bool LANGUAGE INTERNAL STRICT AS 'bytealt';
CREATE OR REPLACE FUNCTION pg_catalog.mediumblob_longblob_le(arg1 mediumblob, arg2 longblob) RETURNS bool LANGUAGE INTERNAL STRICT AS 'byteale';
CREATE OR REPLACE FUNCTION pg_catalog.mediumblob_longblob_gt(arg1 mediumblob, arg2 longblob) RETURNS bool LANGUAGE INTERNAL STRICT AS 'byteagt';
CREATE OR REPLACE FUNCTION pg_catalog.mediumblob_longblob_ge(arg1 mediumblob, arg2 longblob) RETURNS bool LANGUAGE INTERNAL STRICT AS 'byteage';
CREATE OR REPLACE FUNCTION pg_catalog.mediumblob_longblob_cmp(longblob, longblob) RETURNS integer LANGUAGE INTERNAL IMMUTABLE STRICT as 'byteacmp';
CREATE OPERATOR pg_catalog.=(leftarg = mediumblob, rightarg = longblob, procedure = mediumblob_longblob_eq, restrict = eqsel, join = eqjoinsel);
CREATE OPERATOR pg_catalog.<>(leftarg = mediumblob, rightarg = longblob, procedure = mediumblob_longblob_ne, restrict = neqsel, join = neqjoinsel);
CREATE OPERATOR pg_catalog.<(leftarg = mediumblob, rightarg = longblob, procedure = mediumblob_longblob_lt, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.<=(leftarg = mediumblob, rightarg = longblob, procedure = mediumblob_longblob_le, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.>(leftarg = mediumblob, rightarg = longblob, procedure = mediumblob_longblob_gt, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.>=(leftarg = mediumblob, rightarg = longblob, procedure = mediumblob_longblob_ge, restrict = scalarltsel, join = scalarltjoinsel);

-- about mediumblob op othersBlob
-- longblob op tinyblob
CREATE OR REPLACE FUNCTION pg_catalog.longblob_tinyblob_eq(arg1 longblob, arg2 tinyblob) RETURNS bool LANGUAGE INTERNAL STRICT AS 'byteaeq';
CREATE OR REPLACE FUNCTION pg_catalog.longblob_tinyblob_ne(arg1 longblob, arg2 tinyblob) RETURNS bool LANGUAGE INTERNAL STRICT AS 'byteane';
CREATE OR REPLACE FUNCTION pg_catalog.longblob_tinyblob_lt(arg1 longblob, arg2 tinyblob) RETURNS bool LANGUAGE INTERNAL STRICT AS 'bytealt';
CREATE OR REPLACE FUNCTION pg_catalog.longblob_tinyblob_le(arg1 longblob, arg2 tinyblob) RETURNS bool LANGUAGE INTERNAL STRICT AS 'byteale';
CREATE OR REPLACE FUNCTION pg_catalog.longblob_tinyblob_gt(arg1 longblob, arg2 tinyblob) RETURNS bool LANGUAGE INTERNAL STRICT AS 'byteagt';
CREATE OR REPLACE FUNCTION pg_catalog.longblob_tinyblob_ge(arg1 longblob, arg2 tinyblob) RETURNS bool LANGUAGE INTERNAL STRICT AS 'byteage';
CREATE OR REPLACE FUNCTION pg_catalog.longblob_tinyblob_cmp(longblob, tinyblob) RETURNS integer LANGUAGE INTERNAL IMMUTABLE STRICT as 'byteacmp';
CREATE OPERATOR pg_catalog.=(leftarg = longblob, rightarg = tinyblob, procedure = longblob_tinyblob_eq, restrict = eqsel, join = eqjoinsel);
CREATE OPERATOR pg_catalog.<>(leftarg = longblob, rightarg = tinyblob, procedure = longblob_tinyblob_ne, restrict = neqsel, join = neqjoinsel);
CREATE OPERATOR pg_catalog.<(leftarg = longblob, rightarg = tinyblob, procedure = longblob_tinyblob_lt, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.<=(leftarg = longblob, rightarg = tinyblob, procedure = longblob_tinyblob_le, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.>(leftarg = longblob, rightarg = tinyblob, procedure = longblob_tinyblob_gt, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.>=(leftarg = longblob, rightarg = tinyblob, procedure = longblob_tinyblob_ge, restrict = scalarltsel, join = scalarltjoinsel);
-- longblob op blob
CREATE OR REPLACE FUNCTION pg_catalog.longblob_blob_eq(arg1 longblob, arg2 blob) RETURNS bool LANGUAGE INTERNAL STRICT AS 'byteaeq';
CREATE OR REPLACE FUNCTION pg_catalog.longblob_blob_ne(arg1 longblob, arg2 blob) RETURNS bool LANGUAGE INTERNAL STRICT AS 'byteane';
CREATE OR REPLACE FUNCTION pg_catalog.longblob_blob_lt(arg1 longblob, arg2 blob) RETURNS bool LANGUAGE INTERNAL STRICT AS 'bytealt';
CREATE OR REPLACE FUNCTION pg_catalog.longblob_blob_le(arg1 longblob, arg2 blob) RETURNS bool LANGUAGE INTERNAL STRICT AS 'byteale';
CREATE OR REPLACE FUNCTION pg_catalog.longblob_blob_gt(arg1 longblob, arg2 blob) RETURNS bool LANGUAGE INTERNAL STRICT AS 'byteagt';
CREATE OR REPLACE FUNCTION pg_catalog.longblob_blob_ge(arg1 longblob, arg2 blob) RETURNS bool LANGUAGE INTERNAL STRICT AS 'byteage';
CREATE OR REPLACE FUNCTION pg_catalog.longblob_blob_cmp(longblob, blob) RETURNS integer LANGUAGE INTERNAL IMMUTABLE STRICT as 'byteacmp';
CREATE OPERATOR pg_catalog.=(leftarg = longblob, rightarg = blob, procedure = longblob_blob_eq, restrict = eqsel, join = eqjoinsel);
CREATE OPERATOR pg_catalog.<>(leftarg = longblob, rightarg = blob, procedure = longblob_blob_ne, restrict = neqsel, join = neqjoinsel);
CREATE OPERATOR pg_catalog.<(leftarg = longblob, rightarg = blob, procedure = longblob_blob_lt, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.<=(leftarg = longblob, rightarg = blob, procedure = longblob_blob_le, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.>(leftarg = longblob, rightarg = blob, procedure = longblob_blob_gt, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.>=(leftarg = longblob, rightarg = blob, procedure = longblob_blob_ge, restrict = scalarltsel, join = scalarltjoinsel);
-- longblobg op mediumblob
CREATE OR REPLACE FUNCTION pg_catalog.longblob_mediumblob_eq(arg1 longblob, arg2 mediumblob) RETURNS bool LANGUAGE INTERNAL STRICT AS 'byteaeq';
CREATE OR REPLACE FUNCTION pg_catalog.longblob_mediumblob_ne(arg1 longblob, arg2 mediumblob) RETURNS bool LANGUAGE INTERNAL STRICT AS 'byteane';
CREATE OR REPLACE FUNCTION pg_catalog.longblob_mediumblob_lt(arg1 longblob, arg2 mediumblob) RETURNS bool LANGUAGE INTERNAL STRICT AS 'bytealt';
CREATE OR REPLACE FUNCTION pg_catalog.longblob_mediumblob_le(arg1 longblob, arg2 mediumblob) RETURNS bool LANGUAGE INTERNAL STRICT AS 'byteale';
CREATE OR REPLACE FUNCTION pg_catalog.longblob_mediumblob_gt(arg1 longblob, arg2 mediumblob) RETURNS bool LANGUAGE INTERNAL STRICT AS 'byteagt';
CREATE OR REPLACE FUNCTION pg_catalog.longblob_mediumblob_ge(arg1 longblob, arg2 mediumblob) RETURNS bool LANGUAGE INTERNAL STRICT AS 'byteage';
CREATE OR REPLACE FUNCTION pg_catalog.longblob_mediumblob_cmp(longblob, longblob) RETURNS integer LANGUAGE INTERNAL IMMUTABLE STRICT as 'byteacmp';
CREATE OPERATOR pg_catalog.=(leftarg = longblob, rightarg = mediumblob, procedure = longblob_mediumblob_eq, restrict = eqsel, join = eqjoinsel);
CREATE OPERATOR pg_catalog.<>(leftarg = longblob, rightarg = mediumblob, procedure = longblob_mediumblob_ne, restrict = neqsel, join = neqjoinsel);
CREATE OPERATOR pg_catalog.<(leftarg = longblob, rightarg = mediumblob, procedure = longblob_mediumblob_lt, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.<=(leftarg = longblob, rightarg = mediumblob, procedure = longblob_mediumblob_le, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.>(leftarg = longblob, rightarg = mediumblob, procedure = longblob_mediumblob_gt, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.>=(leftarg = longblob, rightarg = mediumblob, procedure = longblob_mediumblob_ge, restrict = scalarltsel, join = scalarltjoinsel);

DROP FUNCTION IF EXISTS pg_catalog.degrees(boolean);
DROP FUNCTION IF EXISTS pg_catalog.degrees(year);
DROP FUNCTION IF EXISTS pg_catalog.degrees(json);
DROP FUNCTION IF EXISTS pg_catalog.acos(boolean);
DROP FUNCTION IF EXISTS pg_catalog.acos(year);
DROP FUNCTION IF EXISTS pg_catalog.acos(json);
DROP FUNCTION IF EXISTS pg_catalog.exp(year);
DROP FUNCTION IF EXISTS pg_catalog.exp(json);
CREATE OR REPLACE FUNCTION pg_catalog.degrees(boolean) RETURNS double precision LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.degrees(cast($1 as double precision))';
CREATE OR REPLACE FUNCTION pg_catalog.degrees(year) RETURNS double precision LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.degrees(cast($1 as double precision))';
CREATE OR REPLACE FUNCTION pg_catalog.degrees(json) RETURNS double precision LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.degrees(cast($1 as double precision))';
CREATE OR REPLACE FUNCTION pg_catalog.acos(boolean) RETURNS double precision LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.acos(cast($1 as double precision))';
CREATE OR REPLACE FUNCTION pg_catalog.acos(year) RETURNS double precision LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.acos(cast($1 as double precision))';
CREATE OR REPLACE FUNCTION pg_catalog.acos(json) RETURNS double precision LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.acos(cast($1 as double precision))';

DROP FUNCTION IF EXISTS pg_catalog.convert(boolean, name);
DROP FUNCTION IF EXISTS pg_catalog.convert(longblob, name);
DROP FUNCTION IF EXISTS pg_catalog.convert(anyenum, name);
DROP FUNCTION IF EXISTS pg_catalog.convert(json, name);

CREATE FUNCTION pg_catalog.convert(boolean,name) RETURNS text LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.convert(cast($1 as TEXT), $2)';
CREATE FUNCTION pg_catalog.convert(longblob,name) RETURNS text LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.convert(cast($1 as TEXT), $2)';
CREATE FUNCTION pg_catalog.convert(anyenum,name) RETURNS text LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.convert(cast($1 as TEXT), $2)';
CREATE FUNCTION pg_catalog.convert(json,name) RETURNS text LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.convert(cast($1 as TEXT), $2)';

DROP FUNCTION IF EXISTS pg_catalog.inet_ntoa(bit) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.inet_ntoa(binary) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.inet_ntoa(tinyblob) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.inet_ntoa(nvarchar2) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.inet_ntoa (bit) RETURNS text LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.inet_ntoa(cast($1 as int8))';
CREATE OR REPLACE FUNCTION pg_catalog.inet_ntoa (binary) RETURNS text LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.inet_ntoa(cast($1 as int8))';
CREATE OR REPLACE FUNCTION pg_catalog.inet_ntoa (tinyblob) RETURNS text LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.inet_ntoa(cast($1 as int8))';
CREATE OR REPLACE FUNCTION pg_catalog.inet_ntoa (nvarchar2) RETURNS text LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.inet_ntoa(cast($1 as varchar))';

CREATE OR REPLACE FUNCTION pg_catalog.exp(year) RETURNS numeric LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.exp(cast($1 as numeric))';
CREATE OR REPLACE FUNCTION pg_catalog.exp(json) RETURNS numeric LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.exp(cast($1 as numeric))';

CREATE OR REPLACE FUNCTION pg_catalog.int8_cast_date(int1)
RETURNS date LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'int8_cast_date';
CREATE OR REPLACE FUNCTION pg_catalog.int16_cast_date(int2)
RETURNS date LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'int16_cast_date';
CREATE OR REPLACE FUNCTION pg_catalog.int32_cast_date(int4)
RETURNS date LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'int32_cast_date';
CREATE OR REPLACE FUNCTION pg_catalog.int64_cast_date(int8)
RETURNS date LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'int64_cast_date';

CREATE OR REPLACE FUNCTION pg_catalog.uint8_cast_date(uint1)
RETURNS date LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'uint8_cast_date';
CREATE OR REPLACE FUNCTION pg_catalog.uint16_cast_date(uint2)
RETURNS date LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'uint16_cast_date';
CREATE OR REPLACE FUNCTION pg_catalog.uint32_cast_date(uint4)
RETURNS date LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'uint32_cast_date';
CREATE OR REPLACE FUNCTION pg_catalog.uint64_cast_date(uint8)
RETURNS date LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'uint64_cast_date';

CREATE OR REPLACE FUNCTION pg_catalog.float4_cast_date(float4)
RETURNS date LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'float4_cast_date';
CREATE OR REPLACE FUNCTION pg_catalog.float8_cast_date(float8)
RETURNS date LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'float8_cast_date';
CREATE OR REPLACE FUNCTION pg_catalog.numeric_cast_date(numeric)
RETURNS date LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'numeric_cast_date';
CREATE OR REPLACE FUNCTION pg_catalog.text_date_explicit(TEXT)
RETURNS date LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'text_date_explicit';

CREATE OR REPLACE FUNCTION pg_catalog.int8_cast_datetime(int1)
RETURNS timestamp without time zone LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'int8_cast_datetime';
CREATE OR REPLACE FUNCTION pg_catalog.int16_cast_datetime(int2)
RETURNS timestamp without time zone LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'int16_cast_datetime';
CREATE OR REPLACE FUNCTION pg_catalog.int32_cast_datetime(int4)
RETURNS timestamp without time zone LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'int32_cast_datetime';
CREATE OR REPLACE FUNCTION pg_catalog.int64_cast_datetime(int8)
RETURNS timestamp without time zone LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'int64_cast_datetime';

CREATE OR REPLACE FUNCTION pg_catalog.uint8_cast_datetime(uint1)
RETURNS timestamp without time zone LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'uint8_cast_datetime';
CREATE OR REPLACE FUNCTION pg_catalog.uint16_cast_datetime(uint2)
RETURNS timestamp without time zone LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'uint16_cast_datetime';
CREATE OR REPLACE FUNCTION pg_catalog.uint32_cast_datetime(uint4)
RETURNS timestamp without time zone LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'uint32_cast_datetime';
CREATE OR REPLACE FUNCTION pg_catalog.uint64_cast_datetime(uint8)
RETURNS timestamp without time zone LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'uint64_cast_datetime';

CREATE OR REPLACE FUNCTION pg_catalog.float4_cast_datetime(float4)
RETURNS timestamp without time zone LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'float4_cast_datetime';
CREATE OR REPLACE FUNCTION pg_catalog.float8_cast_datetime(float8)
RETURNS timestamp without time zone LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'float8_cast_datetime';
CREATE OR REPLACE FUNCTION pg_catalog.numeric_cast_datetime(numeric)
RETURNS timestamp without time zone LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'numeric_cast_datetime';
CREATE OR REPLACE FUNCTION pg_catalog.timestamp_explicit(TEXT)
RETURNS timestamp without time zone LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'timestamp_explicit';

CREATE OR REPLACE FUNCTION pg_catalog.int8_cast_timestamptz(int1)
RETURNS timestamp with time zone LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'int8_cast_timestamptz';
CREATE OR REPLACE FUNCTION pg_catalog.int16_cast_timestamptz(int2)
RETURNS timestamp with time zone LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'int16_cast_timestamptz';
CREATE OR REPLACE FUNCTION pg_catalog.int32_cast_timestamptz(int4)
RETURNS timestamp with time zone LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'int32_cast_timestamptz';
CREATE OR REPLACE FUNCTION pg_catalog.int64_cast_timestamptz(int8)
RETURNS timestamp with time zone LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'int64_cast_timestamptz';

CREATE OR REPLACE FUNCTION pg_catalog.uint8_cast_timestamptz(uint1)
RETURNS timestamp with time zone LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'uint8_cast_timestamptz';
CREATE OR REPLACE FUNCTION pg_catalog.uint16_cast_timestamptz(uint2)
RETURNS timestamp with time zone LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'uint16_cast_timestamptz';
CREATE OR REPLACE FUNCTION pg_catalog.uint32_cast_timestamptz(uint4)
RETURNS timestamp with time zone LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'uint32_cast_timestamptz';
CREATE OR REPLACE FUNCTION pg_catalog.uint64_cast_timestamptz(uint8)
RETURNS timestamp with time zone LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'uint64_cast_timestamptz';

CREATE OR REPLACE FUNCTION pg_catalog.float4_cast_timestamptz(float4)
RETURNS timestamp with time zone LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'float4_cast_timestamptz';
CREATE OR REPLACE FUNCTION pg_catalog.float8_cast_timestamptz(float8)
RETURNS timestamp with time zone LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'float8_cast_timestamptz';
CREATE OR REPLACE FUNCTION pg_catalog.numeric_cast_timestamptz(numeric)
RETURNS timestamp with time zone LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'numeric_cast_timestamptz';
CREATE OR REPLACE FUNCTION pg_catalog.timestamp_timestamptz(TEXT)
RETURNS timestamp with time zone LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'timestamptz_explicit';

--reset pg_cast content, change the castfunc in code(find_coercion_pathway)
do $$
begin
update pg_catalog.pg_cast set castfunc = 401, castowner = 10 where castsource = 1042 and casttarget = 25;
update pg_catalog.pg_cast set castfunc = 401, castowner = 10 where castsource = 1042 and casttarget = 1043;
update pg_catalog.pg_cast set castfunc = 401, castowner = 10 where castsource = 1042 and casttarget = 3969;
end
$$;

CREATE OR REPLACE FUNCTION pg_catalog.bool_date(boolean) RETURNS date LANGUAGE SQL IMMUTABLE STRICT as 'select $1::integer::date';
CREATE CAST (boolean AS date) WITH FUNCTION bool_date(boolean) AS ASSIGNMENT;

DROP FUNCTION IF EXISTS pg_catalog.ln(year);
CREATE OR REPLACE FUNCTION pg_catalog.ln(year) 
RETURNS double precision LANGUAGE SQL IMMUTABLE STRICT as
'select pg_catalog.ln(cast($1 as double precision))';

DROP FUNCTION IF EXISTS pg_catalog.ln(json);
CREATE OR REPLACE FUNCTION pg_catalog.ln(json) 
RETURNS double precision LANGUAGE SQL IMMUTABLE STRICT as
'select pg_catalog.ln(cast($1 as double precision))';

DROP FUNCTION IF EXISTS pg_catalog.varlena_cast_int1(anyelement) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.varlena_cast_int1 (
anyelement
) RETURNS int1 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'varlena_cast_int1';

DROP FUNCTION IF EXISTS pg_catalog.varlena_cast_int2(anyelement) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.varlena_cast_int2 (
anyelement
) RETURNS int2 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'varlena_cast_int2';

DROP FUNCTION IF EXISTS pg_catalog.varlena_cast_int4(anyelement) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.varlena_cast_int4 (
anyelement
) RETURNS int4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'varlena_cast_int4';

CREATE OR REPLACE FUNCTION pg_catalog.str_to_date(boolean, TEXT) RETURNS TEXT LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.str_to_date(cast($1 as TEXT), $2)';
CREATE OR REPLACE FUNCTION pg_catalog.str_to_date(longblob, TEXT) RETURNS TEXT LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.str_to_date(cast($1 as TEXT), $2)';
CREATE OR REPLACE FUNCTION pg_catalog.str_to_date(anyenum, TEXT) RETURNS TEXT LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.str_to_date(cast($1 as TEXT), $2)';
CREATE OR REPLACE FUNCTION pg_catalog.str_to_date(json, TEXT) RETURNS TEXT LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.str_to_date(cast($1 as TEXT), $2)';

DROP FUNCTION IF EXISTS pg_catalog.floor(year);
CREATE OR REPLACE FUNCTION pg_catalog.floor(year) 
RETURNS double precision LANGUAGE SQL IMMUTABLE STRICT as
'select pg_catalog.floor(cast($1 as double precision))';

DROP FUNCTION IF EXISTS pg_catalog.floor(json);
CREATE OR REPLACE FUNCTION pg_catalog.floor(json) 
RETURNS double precision LANGUAGE SQL IMMUTABLE STRICT as
'select pg_catalog.floor(cast($1 as double precision))';
       
CREATE OR REPLACE FUNCTION pg_catalog.hour (year) RETURNS float8 LANGUAGE SQL STABLE STRICT as 'SELECT hour($1::time)';
CREATE OR REPLACE FUNCTION pg_catalog.minute (year) RETURNS float8 LANGUAGE SQL STABLE STRICT as 'SELECT minute($1::time)';
CREATE OR REPLACE FUNCTION pg_catalog.second (year) RETURNS float8 LANGUAGE SQL STABLE STRICT as 'SELECT second($1::time)';
CREATE OR REPLACE FUNCTION pg_catalog.year (year) RETURNS float8 LANGUAGE SQL STABLE STRICT as 'SELECT year($1::time)';
-- repeat function support
CREATE OR REPLACE FUNCTION pg_catalog.repeat(anyenum, integer) RETURNS text LANGUAGE SQL STRICT IMMUTABLE AS 'select repeat($1::text, $2)';
CREATE OR REPLACE FUNCTION pg_catalog.repeat(boolean, integer) RETURNS text LANGUAGE SQL STRICT IMMUTABLE AS 'select repeat($1::text, $2)';
CREATE OR REPLACE FUNCTION pg_catalog.repeat(tinyblob, integer) RETURNS text LANGUAGE SQL STRICT IMMUTABLE AS 'select repeat($1::text, $2)';
CREATE OR REPLACE FUNCTION pg_catalog.repeat(json, integer) RETURNS text LANGUAGE SQL STRICT IMMUTABLE AS 'select repeat($1::text, $2)';
CREATE OR REPLACE FUNCTION pg_catalog.repeat(year, integer) RETURNS text LANGUAGE SQL STRICT IMMUTABLE AS 'select repeat($1::text, $2)';
CREATE OR REPLACE FUNCTION pg_catalog.repeat(binary, integer) RETURNS bytea LANGUAGE C STRICT IMMUTABLE AS '$libdir/dolphin', 'repeat_binary';
CREATE OR REPLACE FUNCTION pg_catalog.repeat(bit, integer) RETURNS bytea LANGUAGE C STRICT IMMUTABLE AS '$libdir/dolphin', 'repeat_bit';

DROP FUNCTION IF EXISTS pg_catalog.any2boolean(anyelement) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.enum_boolean(anyenum) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.set_boolean(anyset) CASCADE;
DROP CAST IF EXISTS ("binary" AS boolean) CASCADE;
DROP CAST IF EXISTS ("varbinary" AS boolean) CASCADE;
DROP CAST IF EXISTS (blob AS boolean) CASCADE;
DROP CAST IF EXISTS (tinyblob AS boolean) CASCADE;
DROP CAST IF EXISTS (mediumblob AS boolean) CASCADE;
DROP CAST IF EXISTS (longblob AS boolean) CASCADE;
DROP CAST IF EXISTS (anyset AS boolean) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.any2boolean(anyelement) RETURNS boolean LANGUAGE SQL IMMUTABLE STRICT as 'select cast(cast($1 as text) as boolean)';
CREATE OR REPLACE FUNCTION pg_catalog.enum_boolean(anyenum) RETURNS boolean LANGUAGE SQL IMMUTABLE STRICT as 'select cast(cast($1 as int) as boolean)';
CREATE OR REPLACE FUNCTION pg_catalog.set_boolean(anyset) RETURNS boolean LANGUAGE SQL IMMUTABLE STRICT as 'select cast(cast($1 as int) as boolean)';
CREATE CAST ("binary" AS boolean) WITH FUNCTION pg_catalog.any2boolean(anyelement) AS ASSIGNMENT;
CREATE CAST ("varbinary" AS boolean) WITH FUNCTION pg_catalog.any2boolean(anyelement) AS ASSIGNMENT;
CREATE CAST (blob AS boolean) WITH FUNCTION pg_catalog.any2boolean(anyelement) AS ASSIGNMENT;
CREATE CAST (tinyblob AS boolean) WITH FUNCTION pg_catalog.any2boolean(anyelement) AS ASSIGNMENT;
CREATE CAST (mediumblob AS boolean) WITH FUNCTION pg_catalog.any2boolean(anyelement) AS ASSIGNMENT;
CREATE CAST (longblob AS boolean) WITH FUNCTION pg_catalog.any2boolean(anyelement) AS ASSIGNMENT;
CREATE CAST (anyset AS boolean) WITH FUNCTION pg_catalog.set_boolean(anyset) AS ASSIGNMENT;

-- nvarchar2
DROP FUNCTION IF EXISTS pg_catalog.nvarchar2_cast_ui1(nvarchar2) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.nvarchar2_cast_ui1 (
nvarchar2
) RETURNS uint1 LANGUAGE SQL IMMUTABLE STRICT as
'select cast(cast($1 as float8) as uint1)';

DROP FUNCTION IF EXISTS pg_catalog.nvarchar2_cast_ui2(nvarchar2) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.nvarchar2_cast_ui2 (
nvarchar2
) RETURNS uint2 LANGUAGE SQL IMMUTABLE STRICT as
'select cast(cast($1 as float8) as uint2)';

DROP FUNCTION IF EXISTS pg_catalog.nvarchar2_cast_ui4(nvarchar2) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.nvarchar2_cast_ui4 (
nvarchar2
) RETURNS uint4 LANGUAGE SQL IMMUTABLE STRICT as
'select cast(cast($1 as float8) as uint4)';

DROP FUNCTION IF EXISTS pg_catalog.nvarchar2_cast_ui8(nvarchar2) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.nvarchar2_cast_ui8 (
nvarchar2
) RETURNS uint8 LANGUAGE SQL IMMUTABLE STRICT as
'select cast(cast($1 as float8) as uint8)';

DROP FUNCTION IF EXISTS pg_catalog.nvarchar2_enum(nvarchar2, int4, anyelement) cascade;
CREATE OR REPLACE FUNCTION pg_catalog.nvarchar2_enum(
nvarchar2, int4, anyelement
) RETURNS anyenum LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'nvarchar2_enum';

DROP FUNCTION IF EXISTS pg_catalog.nvarchar2_cast_int8(nvarchar2) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.nvarchar2_cast_int8 (
nvarchar2
) RETURNS int8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'nvarchar2_cast_int8';

CREATE OR REPLACE FUNCTION pg_catalog.ascii(blob) RETURNS integer LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.ascii(cast($1 as TEXT))';
CREATE OR REPLACE FUNCTION pg_catalog.ascii(year) RETURNS integer LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.ascii(cast($1 as TEXT))';
CREATE OR REPLACE FUNCTION pg_catalog.ascii(json) RETURNS integer LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.ascii(cast($1 as TEXT))';
CREATE OR REPLACE FUNCTION pg_catalog.ascii(boolean) RETURNS integer LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.ascii(cast($1 as integer))';
CREATE OR REPLACE FUNCTION pg_catalog.ascii(anyenum) RETURNS integer LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.ascii(cast($1 as TEXT))';
CREATE OR REPLACE FUNCTION pg_catalog.ascii(bit) RETURNS integer LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'bit_to_ascii';
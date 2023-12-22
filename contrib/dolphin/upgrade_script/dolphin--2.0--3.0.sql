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
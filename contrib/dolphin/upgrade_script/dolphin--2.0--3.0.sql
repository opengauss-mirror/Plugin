DROP FUNCTION IF EXISTS pg_catalog.dolphin_invoke();
CREATE FUNCTION pg_catalog.dolphin_invoke()
    RETURNS VOID AS '$libdir/dolphin','dolphin_invoke' LANGUAGE C STRICT;

DROP FUNCTION IF EXISTS pg_catalog.date_cast(cstring, boolean);
CREATE OR REPLACE FUNCTION pg_catalog.date_cast(cstring, boolean) RETURNS date LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'date_cast';

DROP FUNCTION IF EXISTS pg_catalog.timestamp_cast(cstring, oid, integer, boolean);
CREATE OR REPLACE FUNCTION pg_catalog.timestamp_cast(cstring, oid, integer, boolean) RETURNS timestamp without time zone LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'timestamp_cast';

DROP FUNCTION IF EXISTS pg_catalog.timestamptz_cast(cstring, oid, integer, boolean);
CREATE OR REPLACE FUNCTION pg_catalog.timestamptz_cast(cstring, oid, integer, boolean) RETURNS timestamp with time zone LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'timestamptz_cast';

DROP FUNCTION IF EXISTS pg_catalog.time_cast(cstring, boolean);
CREATE OR REPLACE FUNCTION pg_catalog.time_cast(cstring, boolean) RETURNS time without time zone LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'time_cast';

DROP CAST IF EXISTS (TEXT AS time);
DROP FUNCTION IF EXISTS pg_catalog.time_cast_implicit(TEXT);
CREATE OR REPLACE FUNCTION pg_catalog.time_cast_implicit(TEXT) RETURNS time without time zone LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'time_cast_implicit';

DROP FUNCTION IF EXISTS pg_catalog.text_time_explicit(TEXT);
CREATE OR REPLACE FUNCTION pg_catalog.text_time_explicit(TEXT) RETURNS time without time zone LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'text_time_explicit';

CREATE CAST(TEXT AS time) WITH FUNCTION time_cast_implicit(TEXT) AS ASSIGNMENT;

CREATE OR REPLACE FUNCTION pg_catalog.date_cast_datetime(date) RETURNS timestamp without time zone LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'date_cast_datetime';
CREATE OR REPLACE FUNCTION pg_catalog.date_cast_timestamptz(date) RETURNS timestamp LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'date_cast_timestamptz';

DROP FUNCTION IF EXISTS pg_catalog.day(time without time zone);
CREATE OR REPLACE FUNCTION pg_catalog.day(time without time zone) RETURNS int4 LANGUAGE C STABLE RETURNS NULL ON NULL INPUT as '$libdir/dolphin', 'dayofmonth_time';

DROP FUNCTION IF EXISTS pg_catalog.time_to_sec(text);
CREATE OR REPLACE FUNCTION pg_catalog.time_to_sec(text) RETURNS int8 LANGUAGE C STABLE RETURNS NULL ON NULL INPUT as '$libdir/dolphin', 'time_to_sec';

DROP FUNCTION IF EXISTS pg_catalog.time_to_sec(int8);
CREATE OR REPLACE FUNCTION pg_catalog.time_to_sec(int8) RETURNS int8 LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'int64_time_to_sec';

DROP FUNCTION IF EXISTS pg_catalog.time_to_sec(numeric);
CREATE OR REPLACE FUNCTION pg_catalog.time_to_sec(numeric) RETURNS int8 LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'numeric_time_to_sec';

DROP FUNCTION IF EXISTS pg_catalog.time_to_sec(timestamp without time zone);
CREATE OR REPLACE FUNCTION pg_catalog.time_to_sec(timestamp without time zone) RETURNS int8 LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'timestamp_time_to_sec';

DROP FUNCTION IF EXISTS pg_catalog.time_to_sec(timestamp with time zone);
CREATE OR REPLACE FUNCTION pg_catalog.time_to_sec(timestamp with time zone) RETURNS int8 LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'timestamptz_time_to_sec';

DROP FUNCTION IF EXISTS pg_catalog.time_to_sec(date);
CREATE OR REPLACE FUNCTION pg_catalog.time_to_sec(date) RETURNS int8 AS $$ SELECT pg_catalog.time_to_sec(cast($1 as timestamp without time zone)) $$ LANGUAGE SQL;

DROP FUNCTION IF EXISTS pg_catalog.time_to_sec(year);
CREATE OR REPLACE FUNCTION pg_catalog.time_to_sec(year) RETURNS int8 AS $$ SELECT pg_catalog.time_to_sec(cast($1 as int8)) $$ LANGUAGE SQL;

DROP FUNCTION IF EXISTS pg_catalog.time_to_sec(longblob);
CREATE OR REPLACE FUNCTION pg_catalog.time_to_sec(longblob) RETURNS int8 AS $$ SELECT pg_catalog.time_to_sec(cast($1 as text)) $$ LANGUAGE SQL;

DROP FUNCTION IF EXISTS pg_catalog.time_to_sec(bit);
CREATE OR REPLACE FUNCTION pg_catalog.time_to_sec(bit) RETURNS int8 AS $$ SELECT pg_catalog.time_to_sec(cast($1 as text)) $$ LANGUAGE SQL;

DROP FUNCTION IF EXISTS pg_catalog.time_to_sec(anyset);
CREATE OR REPLACE FUNCTION pg_catalog.time_to_sec(anyset) RETURNS int8 AS $$ SELECT pg_catalog.time_to_sec(cast($1 as text)) $$ LANGUAGE SQL;

DROP FUNCTION IF EXISTS pg_catalog.time_to_sec(anyenum);
CREATE OR REPLACE FUNCTION pg_catalog.time_to_sec(anyenum) RETURNS int8 AS $$ SELECT pg_catalog.time_to_sec(cast($1 as text)) $$ LANGUAGE SQL;

DROP FUNCTION IF EXISTS pg_catalog.time_to_sec(json);
CREATE OR REPLACE FUNCTION pg_catalog.time_to_sec(json) RETURNS int8 AS $$ SELECT pg_catalog.time_to_sec(cast($1 as timestamp without time zone)) $$ LANGUAGE SQL;

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

DROP FUNCTION IF EXISTS pg_catalog.hour (text);
DROP FUNCTION IF EXISTS pg_catalog.microsecond (text);
DROP FUNCTION IF EXISTS pg_catalog.minute (text);
DROP FUNCTION IF EXISTS pg_catalog.second (text);

CREATE OR REPLACE FUNCTION pg_catalog.hour (text) RETURNS int8 LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'GetHour';
CREATE OR REPLACE FUNCTION pg_catalog.microsecond (text) RETURNS int8 LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'GetMicrosecond';
CREATE OR REPLACE FUNCTION pg_catalog.minute (text) RETURNS int8 LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'GetMinute';
CREATE OR REPLACE FUNCTION pg_catalog.second (text) RETURNS int8 LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'GetSecond';


CREATE OR REPLACE FUNCTION pg_catalog.hour (date) RETURNS int8 LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'GetHourFromDate';
CREATE OR REPLACE FUNCTION pg_catalog.microsecond (date) RETURNS int8 LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'GetMicrosecondFromDate';
CREATE OR REPLACE FUNCTION pg_catalog.minute (date) RETURNS int8 LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'GetMinuteFromDate';
CREATE OR REPLACE FUNCTION pg_catalog.second (date) RETURNS int8 LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'GetSecondFromDate';

CREATE OR REPLACE FUNCTION pg_catalog.hour (timetz) RETURNS int8 LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'GetHourFromTimeTz';
CREATE OR REPLACE FUNCTION pg_catalog.microsecond (timetz) RETURNS int8 LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'GetMicrosecondFromTimeTz';
CREATE OR REPLACE FUNCTION pg_catalog.minute (timetz) RETURNS int8 LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'GetMinuteFromTimeTz';
CREATE OR REPLACE FUNCTION pg_catalog.second (timetz) RETURNS int8 LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'GetSecondFromTimeTz';

CREATE OR REPLACE FUNCTION pg_catalog.hour (timestamptz) RETURNS int8 LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'GetHourFromTimestampTz';
CREATE OR REPLACE FUNCTION pg_catalog.microsecond (timestamptz) RETURNS int8 LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'GetMicrosecondFromTimestampTz';
CREATE OR REPLACE FUNCTION pg_catalog.minute (timestamptz) RETURNS int8 LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'GetMinuteFromTimestampTz';
CREATE OR REPLACE FUNCTION pg_catalog.second (timestamptz) RETURNS int8 LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'GetSecondFromTimestampTz';


CREATE OR REPLACE FUNCTION pg_catalog.hour (year) RETURNS int8 LANGUAGE SQL STABLE STRICT as 'SELECT hour($1::time)';
CREATE OR REPLACE FUNCTION pg_catalog.minute (year) RETURNS int8 LANGUAGE SQL STABLE STRICT as 'SELECT minute($1::time)';
CREATE OR REPLACE FUNCTION pg_catalog.second (year) RETURNS int8 LANGUAGE SQL STABLE STRICT as 'SELECT second($1::time)';

DROP FUNCTION IF EXISTS pg_catalog.year(timestamp(0) without time zone);
DROP FUNCTION IF EXISTS pg_catalog.year(text);
CREATE FUNCTION pg_catalog.year (timestamp(0) without time zone) RETURNS int8 LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'datetime_year_part';
CREATE FUNCTION pg_catalog.year (text) RETURNS int8 LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'text_year_part';
CREATE OR REPLACE FUNCTION pg_catalog.year(year) RETURNS int8 LANGUAGE SQL STABLE STRICT as 'SELECT year($1::text)';

CREATE OR REPLACE FUNCTION pg_catalog.year(bit) RETURNS int8 LANGUAGE SQL STABLE STRICT as 'SELECT year($1::text)';
CREATE OR REPLACE FUNCTION pg_catalog.year(boolean) RETURNS int8 LANGUAGE SQL STABLE STRICT as 'SELECT year($1::text)';
CREATE OR REPLACE FUNCTION pg_catalog.year(int4) RETURNS int8 LANGUAGE SQL STABLE STRICT as 'SELECT year($1::text)';
CREATE OR REPLACE FUNCTION pg_catalog.year(longblob) RETURNS int8 LANGUAGE SQL STABLE STRICT as 'SELECT year($1::text)';
CREATE OR REPLACE FUNCTION pg_catalog.year(anyenum) RETURNS int8 LANGUAGE SQL STABLE STRICT as 'SELECT year($1::text)';
CREATE OR REPLACE FUNCTION pg_catalog.year(json) RETURNS int8 LANGUAGE SQL STABLE STRICT as 'SELECT year($1::text)';
CREATE OR REPLACE FUNCTION pg_catalog.year(time) RETURNS int8 LANGUAGE SQL STABLE STRICT as 'SELECT year($1::timestamp(0) without time zone)';


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
DROP FUNCTION IF EXISTS pg_catalog.inet_ntoa(year) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.inet_ntoa(json) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.inet_ntoa (bit) RETURNS text LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.inet_ntoa(cast($1 as int8))';
CREATE OR REPLACE FUNCTION pg_catalog.inet_ntoa (binary) RETURNS text LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.inet_ntoa(cast($1 as int8))';
CREATE OR REPLACE FUNCTION pg_catalog.inet_ntoa (tinyblob) RETURNS text LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.inet_ntoa(cast($1 as int8))';
CREATE OR REPLACE FUNCTION pg_catalog.inet_ntoa (nvarchar2) RETURNS text LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.inet_ntoa(cast($1 as varchar))';
CREATE OR REPLACE FUNCTION pg_catalog.inet_ntoa (year) RETURNS text LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.inet_ntoa(cast($1 as int8))';
CREATE OR REPLACE FUNCTION pg_catalog.inet_ntoa (json) RETURNS text LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.inet_ntoa(cast($1 as int8))';

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
CREATE OR REPLACE FUNCTION pg_catalog.timestamptz_explicit(TEXT)
RETURNS timestamp with time zone LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'timestamptz_explicit';

CREATE CAST(TEXT AS timestamp with time zone) WITH FUNCTION pg_catalog.timestamptz_explicit(TEXT) AS ASSIGNMENT;

--reset pg_cast content, change the castfunc in code(find_coercion_pathway)
do $$
begin
update pg_catalog.pg_cast set castfunc = 401, castowner = 10 where castsource = 1042 and casttarget = 25;
update pg_catalog.pg_cast set castfunc = 401, castowner = 10 where castsource = 1042 and casttarget = 1043;
update pg_catalog.pg_cast set castfunc = 401, castowner = 10 where castsource = 1042 and casttarget = 3969;
end
$$;

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

CREATE OR REPLACE FUNCTION pg_catalog.log(anyelement,anyelement) RETURNS number LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.log(cast($1 as number), cast($2 as number))';

DROP FUNCTION IF EXISTS pg_catalog.log(year);
DROP FUNCTION IF EXISTS pg_catalog.log(json);
CREATE OR REPLACE FUNCTION pg_catalog.log(year) RETURNS double precision LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.log(cast($1 as double precision))';
CREATE OR REPLACE FUNCTION pg_catalog.log(json) RETURNS double precision LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.log(cast($1 as double precision))';
CREATE OR REPLACE FUNCTION pg_catalog.log(uint1) RETURNS double precision LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.log(cast($1 as double precision))';
CREATE OR REPLACE FUNCTION pg_catalog.log(uint2) RETURNS double precision LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.log(cast($1 as double precision))';
CREATE OR REPLACE FUNCTION pg_catalog.log(uint4) RETURNS double precision LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.log(cast($1 as double precision))';
CREATE OR REPLACE FUNCTION pg_catalog.log(uint8) RETURNS double precision LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.log(cast($1 as double precision))';
CREATE OR REPLACE FUNCTION pg_catalog.log(bit) RETURNS double precision LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.log(cast($1 as double precision))';
CREATE OR REPLACE FUNCTION pg_catalog.log(boolean) RETURNS double precision LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.log(cast($1 as double precision))';
CREATE OR REPLACE FUNCTION pg_catalog.log(text) RETURNS double precision LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.log(cast($1 as double precision))';
CREATE OR REPLACE FUNCTION pg_catalog.log(char) RETURNS double precision LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.log(cast($1 as double precision))';
CREATE OR REPLACE FUNCTION pg_catalog.log(varchar) RETURNS double precision LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.log(cast($1 as double precision))';

CREATE OR REPLACE FUNCTION pg_catalog.substr(arg1 longblob, start int, the_end int) RETURNS longblob LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.substrb(arg1::text, start, the_end)::longblob';
CREATE OR REPLACE FUNCTION pg_catalog.substr(arg1 longblob, start int) RETURNS longblob LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.substrb(arg1::text, start)::longblob';

CREATE CAST (tinyblob AS binary) WITHOUT FUNCTION AS IMPLICIT;
CREATE CAST (tinyblob AS varbinary) WITHOUT FUNCTION AS IMPLICIT;
CREATE CAST (blob AS binary) WITHOUT FUNCTION AS IMPLICIT;
CREATE CAST (blob AS varbinary) WITHOUT FUNCTION AS IMPLICIT;
CREATE CAST (mediumblob AS binary) WITHOUT FUNCTION AS IMPLICIT;
CREATE CAST (mediumblob AS varbinary) WITHOUT FUNCTION AS IMPLICIT;
CREATE CAST (longblob AS binary) WITHOUT FUNCTION AS IMPLICIT;
CREATE CAST (longblob AS varbinary) WITHOUT FUNCTION AS IMPLICIT;

DROP FUNCTION IF EXISTS pg_catalog.inet_ntoa(blob) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.inet_ntoa(mediumblob) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.inet_ntoa(longblob) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.inet_ntoa (blob) RETURNS text LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.inet_ntoa(cast($1 as int8))';
CREATE OR REPLACE FUNCTION pg_catalog.inet_ntoa (mediumblob) RETURNS text LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.inet_ntoa(cast($1 as int8))';
CREATE OR REPLACE FUNCTION pg_catalog.inet_ntoa (longblob) RETURNS text LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.inet_ntoa(cast($1 as int8))';

DROP FUNCTION IF EXISTS pg_catalog.unhex (text);
DROP FUNCTION IF EXISTS pg_catalog.unhex (boolean);
DROP FUNCTION IF EXISTS pg_catalog.unhex (bytea);
DROP FUNCTION IF EXISTS pg_catalog.unhex (bit);
CREATE OR REPLACE FUNCTION pg_catalog.unhex (text)  RETURNS longblob LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'hex_decode_text';
CREATE OR REPLACE FUNCTION pg_catalog.unhex (boolean)  RETURNS longblob LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'hex_decode_bool';
CREATE OR REPLACE FUNCTION pg_catalog.unhex (bytea)  RETURNS longblob LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'hex_decode_bytea';
CREATE OR REPLACE FUNCTION pg_catalog.unhex (bit)  RETURNS longblob LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'hex_decode_bit';

-- repeat function support
CREATE OR REPLACE FUNCTION pg_catalog.repeat(anyenum, integer) RETURNS text LANGUAGE SQL STRICT IMMUTABLE AS 'select repeat($1::text, $2)';
CREATE OR REPLACE FUNCTION pg_catalog.repeat(boolean, integer) RETURNS text LANGUAGE SQL STRICT IMMUTABLE AS 'select repeat($1::text, $2)';
CREATE OR REPLACE FUNCTION pg_catalog.repeat(tinyblob, integer) RETURNS longblob LANGUAGE SQL STRICT IMMUTABLE AS 'select repeat($1::text, $2)::longblob';
CREATE OR REPLACE FUNCTION pg_catalog.repeat(blob, integer) RETURNS longblob LANGUAGE SQL STRICT IMMUTABLE AS 'select repeat($1::text, $2)::longblob';
CREATE OR REPLACE FUNCTION pg_catalog.repeat(mediumblob, integer) RETURNS longblob LANGUAGE SQL STRICT IMMUTABLE AS 'select repeat($1::text, $2)::longblob';
CREATE OR REPLACE FUNCTION pg_catalog.repeat(longblob, integer) RETURNS longblob LANGUAGE SQL STRICT IMMUTABLE AS 'select repeat($1::text, $2)::longblob';
CREATE OR REPLACE FUNCTION pg_catalog.repeat(json, integer) RETURNS text LANGUAGE SQL STRICT IMMUTABLE AS 'select repeat($1::text, $2)';
CREATE OR REPLACE FUNCTION pg_catalog.repeat(year, integer) RETURNS text LANGUAGE SQL STRICT IMMUTABLE AS 'select repeat($1::text, $2)';
CREATE OR REPLACE FUNCTION pg_catalog.repeat(binary, integer) RETURNS bytea LANGUAGE C STRICT IMMUTABLE AS '$libdir/dolphin', 'repeat_binary';
CREATE OR REPLACE FUNCTION pg_catalog.repeat(bit, integer) RETURNS bytea LANGUAGE C STRICT IMMUTABLE AS '$libdir/dolphin', 'repeat_bit';

DROP CAST IF EXISTS ("binary" AS boolean) CASCADE;
DROP CAST IF EXISTS ("varbinary" AS boolean) CASCADE;
DROP CAST IF EXISTS (blob AS boolean) CASCADE;
DROP CAST IF EXISTS (tinyblob AS boolean) CASCADE;
DROP CAST IF EXISTS (mediumblob AS boolean) CASCADE;
DROP CAST IF EXISTS (longblob AS boolean) CASCADE;
DROP CAST IF EXISTS (anyset AS boolean) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.any2boolean(anyelement) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.enum_boolean(anyenum) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.set_boolean(anyset) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.binary2boolean(binary) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.varbinary2boolean(varbinary) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.any2boolean(anyelement) RETURNS boolean LANGUAGE SQL IMMUTABLE STRICT as 'select cast(cast($1 as text) as boolean)';
CREATE OR REPLACE FUNCTION pg_catalog.enum_boolean(anyenum) RETURNS boolean LANGUAGE SQL IMMUTABLE STRICT as 'select cast(cast($1 as int) as boolean)';
CREATE OR REPLACE FUNCTION pg_catalog.set_boolean(anyset) RETURNS boolean LANGUAGE SQL IMMUTABLE STRICT as 'select cast(cast($1 as int) as boolean)';
CREATE OR REPLACE FUNCTION pg_catalog.binary2boolean(binary) RETURNS boolean LANGUAGE SQL IMMUTABLE STRICT as 'select char_bool($1::char)';
CREATE OR REPLACE FUNCTION pg_catalog.varbinary2boolean(varbinary) RETURNS boolean LANGUAGE SQL IMMUTABLE STRICT as 'select varchar_bool($1::varchar)';
CREATE CAST ("binary" AS boolean) WITH FUNCTION pg_catalog.binary2boolean(binary) AS ASSIGNMENT;
CREATE CAST ("varbinary" AS boolean) WITH FUNCTION pg_catalog.varbinary2boolean(varbinary) AS ASSIGNMENT;
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
DROP FUNCTION IF EXISTS pg_catalog.boolean_time(boolean) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.boolean_date(boolean) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.boolean_datetime(boolean) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.boolean_timestamptz(boolean) CASCADE;
DROP CAST IF EXISTS (boolean as time without time zone) CASCADE;
DROP CAST IF EXISTS (boolean as date) CASCADE;
DROP CAST IF EXISTS (boolean as timestamp without time zone) CASCADE;
DROP CAST IF EXISTS (boolean as timestamptz) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.boolean_time(boolean) RETURNS time without time zone
LANGUAGE SQL IMMUTABLE STRICT as 'select cast(cast($1 as int) as time without time zone)';
CREATE OR REPLACE FUNCTION pg_catalog.boolean_date(boolean) RETURNS date
LANGUAGE C STRICT IMMUTABLE AS '$libdir/dolphin', 'bool_b_format_date';
CREATE OR REPLACE FUNCTION pg_catalog.boolean_datetime(boolean) RETURNS timestamp without time zone
LANGUAGE C STRICT IMMUTABLE AS '$libdir/dolphin', 'bool_b_format_datetime';
CREATE OR REPLACE FUNCTION pg_catalog.boolean_timestamptz(boolean) RETURNS timestamptz
LANGUAGE C STRICT IMMUTABLE AS '$libdir/dolphin', 'bool_b_format_timestamp';
CREATE CAST (boolean as time without time zone) WITH FUNCTION pg_catalog.boolean_time(boolean) AS ASSIGNMENT;
CREATE CAST (boolean as date) WITH FUNCTION pg_catalog.boolean_date(boolean) AS ASSIGNMENT;
CREATE CAST (boolean as timestamp without time zone) WITH FUNCTION pg_catalog.boolean_datetime(boolean) AS ASSIGNMENT;
CREATE CAST (boolean as timestamptz) WITH FUNCTION pg_catalog.boolean_timestamptz(boolean) AS ASSIGNMENT;

DROP FUNCTION IF EXISTS pg_catalog.bool_cast_time(boolean) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.bool_cast_date(boolean) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.bool_cast_datetime(boolean) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.bool_cast_timestamptz(boolean) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.bool_cast_time(boolean)
RETURNS time without time zone LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'bool_cast_time';
CREATE OR REPLACE FUNCTION pg_catalog.bool_cast_date(boolean)
RETURNS date LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'bool_cast_date';
CREATE OR REPLACE FUNCTION pg_catalog.bool_cast_datetime(boolean)
RETURNS timestamp without time zone LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'bool_cast_datetime';
CREATE OR REPLACE FUNCTION pg_catalog.bool_cast_timestamptz(boolean)
RETURNS timestamp with time zone LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'bool_cast_timestamptz';

CREATE OR REPLACE FUNCTION pg_catalog.ascii(blob) RETURNS integer LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.ascii(cast($1 as TEXT))';
CREATE OR REPLACE FUNCTION pg_catalog.ascii(year) RETURNS integer LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.ascii(cast($1 as TEXT))';
CREATE OR REPLACE FUNCTION pg_catalog.ascii(json) RETURNS integer LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.ascii(cast($1 as TEXT))';
CREATE OR REPLACE FUNCTION pg_catalog.ascii(boolean) RETURNS integer LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.ascii(cast($1 as integer))';
CREATE OR REPLACE FUNCTION pg_catalog.ascii(anyenum) RETURNS integer LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.ascii(cast($1 as TEXT))';
CREATE OR REPLACE FUNCTION pg_catalog.ascii(bit) RETURNS integer LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'bit_to_ascii';

CREATE OR REPLACE FUNCTION pg_catalog.bit_length(boolean) RETURNS integer LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.bit_length(cast($1 as TEXT))';
CREATE OR REPLACE FUNCTION pg_catalog.bit_length(year) RETURNS integer LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.bit_length(cast($1 as TEXT))';
CREATE OR REPLACE FUNCTION pg_catalog.bit_length(blob) RETURNS integer LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.bit_length(cast($1 as TEXT))';
CREATE OR REPLACE FUNCTION pg_catalog.bit_length(anyenum) RETURNS integer LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.bit_length(cast($1 as TEXT))';
CREATE OR REPLACE FUNCTION pg_catalog.bit_length(json) RETURNS integer LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.bit_length(cast($1 as TEXT))';
CREATE OR REPLACE FUNCTION pg_catalog.bit_length(binary) RETURNS integer LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'mp_bit_length_binary';

DROP CAST IF EXISTS (uint4 AS year) CASCADE;
DROP CAST IF EXISTS (boolean AS year) CASCADE;
DROP CAST IF EXISTS (char AS year) CASCADE;
DROP CAST IF EXISTS (varchar AS year) CASCADE;
DROP CAST IF EXISTS (text AS year) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.uint4_year(uint4) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.boolean_year(boolean) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.char_year(char) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.varchar_year(varchar) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.text_year(text) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.uint4_year(uint4) RETURNS year LANGUAGE SQL IMMUTABLE STRICT as 'select cast(cast($1 as int8) as year)';
CREATE CAST(uint4 AS year) WITH FUNCTION uint4_year(uint4) AS ASSIGNMENT;
CREATE OR REPLACE FUNCTION pg_catalog.boolean_year(boolean) RETURNS year LANGUAGE SQL IMMUTABLE STRICT as 'select cast(cast($1 as int8) as year)';
CREATE CAST(boolean AS year) WITH FUNCTION boolean_year(boolean) AS ASSIGNMENT;
CREATE OR REPLACE FUNCTION pg_catalog.char_year(char) RETURNS year LANGUAGE SQL IMMUTABLE STRICT as 'select cast(cast($1 as int8) as year)';
CREATE CAST(char AS year) WITH FUNCTION char_year(char) AS ASSIGNMENT;
CREATE OR REPLACE FUNCTION pg_catalog.varchar_year(varchar) RETURNS year LANGUAGE SQL IMMUTABLE STRICT as 'select cast(cast($1 as int8) as year)';
CREATE CAST(varchar AS year) WITH FUNCTION varchar_year(varchar) AS ASSIGNMENT;
CREATE OR REPLACE FUNCTION pg_catalog.text_year (text) RETURNS year LANGUAGE SQL IMMUTABLE STRICT as 'select cast(cast($1 as int8) as year)';
CREATE CAST(text AS year) WITH FUNCTION text_year(text) AS ASSIGNMENT;

create or replace function pg_catalog.varbinarylike(varbinary, varbinary) returns bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'binarylike';
CREATE OPERATOR pg_catalog.~~(leftarg = varbinary, rightarg = varbinary, procedure = pg_catalog.varbinarylike);

create or replace function pg_catalog.varbinarytextlike(varbinary, text) returns bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'binarylike';
CREATE OPERATOR pg_catalog.~~(leftarg = varbinary, rightarg = text, procedure = pg_catalog.varbinarytextlike);
CREATE OPERATOR pg_catalog.~~*(leftarg = varbinary, rightarg = text, procedure = pg_catalog.varbinarytextlike);

create or replace function pg_catalog.textvarbinarylike(text, varbinary) returns bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'binarylike';
CREATE OPERATOR pg_catalog.~~(leftarg = text, rightarg = varbinary, procedure = pg_catalog.textvarbinarylike);
CREATE OPERATOR pg_catalog.~~*(leftarg = text, rightarg = varbinary, procedure = pg_catalog.textvarbinarylike);

create or replace function pg_catalog.bloblike(blob, blob) returns bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'binarylike';
CREATE OPERATOR pg_catalog.~~(leftarg = blob, rightarg = blob, procedure = pg_catalog.bloblike);
CREATE OPERATOR pg_catalog.~~*(leftarg = blob, rightarg = blob, procedure = pg_catalog.bloblike);

create or replace function pg_catalog.binarylike(binary, binary) returns bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'binarylike';
create or replace function pg_catalog.binarytextlike(binary, text) returns bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'binarylike';
create or replace function pg_catalog.textbinarylike(text, binary) returns bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'binarylike';

create or replace function pg_catalog.boolean_binary_eq(boolean, binary) returns bool LANGUAGE SQL IMMUTABLE STRICT as 'select ($1::int = $2::float)';
CREATE OPERATOR pg_catalog.=(leftarg = boolean, rightarg = binary, COMMUTATOR = operator(pg_catalog.=), procedure = pg_catalog.boolean_binary_eq, restrict = eqsel, join = eqjoinsel);
create or replace function pg_catalog.binary_boolean_eq(binary, boolean) returns bool LANGUAGE SQL IMMUTABLE STRICT as 'select ($1::float = $2::int)';
CREATE OPERATOR pg_catalog.=(leftarg = binary, rightarg = boolean, COMMUTATOR = operator(pg_catalog.=), procedure = pg_catalog.binary_boolean_eq, restrict = eqsel, join = eqjoinsel);
CREATE OR REPLACE FUNCTION pg_catalog.boolean_binary_ne(boolean, binary) returns bool LANGUAGE SQL IMMUTABLE STRICT as 'select ($1::int != $2::float)';
CREATE OPERATOR pg_catalog.<>(leftarg = boolean, rightarg = binary, COMMUTATOR = operator(pg_catalog.<>), procedure = pg_catalog.boolean_binary_ne,restrict = neqsel, join = neqjoinsel);
CREATE OR REPLACE FUNCTION pg_catalog.binary_boolean_ne(binary, boolean) returns bool LANGUAGE SQL IMMUTABLE STRICT as 'select ($1::float != $2::int)';
CREATE OPERATOR pg_catalog.<>(leftarg = binary, rightarg = boolean, COMMUTATOR = operator(pg_catalog.<>), procedure = pg_catalog.binary_boolean_ne,restrict = neqsel, join = neqjoinsel);
CREATE OR REPLACE FUNCTION pg_catalog.boolean_binary_gt(boolean, binary) returns bool LANGUAGE SQL IMMUTABLE STRICT as 'select ($1::int > $2::float)';
CREATE OPERATOR pg_catalog.>(leftarg = boolean, rightarg = binary, COMMUTATOR = operator(pg_catalog.<), procedure = pg_catalog.boolean_binary_gt,restrict = scalargtsel, join = scalargtjoinsel);
CREATE OR REPLACE FUNCTION pg_catalog.binary_boolean_gt(binary, boolean) returns bool LANGUAGE SQL IMMUTABLE STRICT as 'select ($1::float > $2::int)';
CREATE OPERATOR pg_catalog.>(leftarg = binary, rightarg = boolean, COMMUTATOR = operator(pg_catalog.<), procedure = pg_catalog.binary_boolean_gt,restrict = scalargtsel, join = scalargtjoinsel);
CREATE OR REPLACE FUNCTION pg_catalog.boolean_binary_lt(boolean, binary) returns bool LANGUAGE SQL IMMUTABLE STRICT as 'select ($1::int < $2::float)';
CREATE OPERATOR pg_catalog.<(leftarg = boolean, rightarg = binary, COMMUTATOR = operator(pg_catalog.>), procedure = pg_catalog.boolean_binary_lt,restrict = scalarltsel, join = scalarltjoinsel);
CREATE OR REPLACE FUNCTION pg_catalog.binary_boolean_lt(binary, boolean) returns bool LANGUAGE SQL IMMUTABLE STRICT as 'select ($1::float < $2::int)';
CREATE OPERATOR pg_catalog.<(leftarg = binary, rightarg = boolean, COMMUTATOR = operator(pg_catalog.>), procedure = pg_catalog.binary_boolean_lt,restrict = scalarltsel, join = scalarltjoinsel);
CREATE OR REPLACE FUNCTION pg_catalog.boolean_binary_ge(boolean, binary) returns bool LANGUAGE SQL IMMUTABLE STRICT as 'select ($1::int >= $2::float)';
CREATE OPERATOR pg_catalog.>=(leftarg = boolean, rightarg = binary, COMMUTATOR = operator(pg_catalog.<=), procedure = pg_catalog.boolean_binary_ge,restrict = scalargtsel, join = scalargtjoinsel);
CREATE OR REPLACE FUNCTION pg_catalog.binary_boolean_ge(binary, boolean) returns bool LANGUAGE SQL IMMUTABLE STRICT as 'select ($1::float >= $2::int)';
CREATE OPERATOR pg_catalog.>=(leftarg = binary, rightarg = boolean, COMMUTATOR = operator(pg_catalog.<=), procedure = pg_catalog.binary_boolean_ge,restrict = scalargtsel, join = scalargtjoinsel);
CREATE OR REPLACE FUNCTION pg_catalog.boolean_binary_le(boolean, binary) returns bool LANGUAGE SQL IMMUTABLE STRICT as 'select ($1::int <= $2::float)';
CREATE OPERATOR pg_catalog.<=(leftarg = boolean, rightarg = binary, COMMUTATOR = operator(pg_catalog.>=), procedure = pg_catalog.boolean_binary_le,restrict = scalarltsel, join = scalarltjoinsel);
CREATE OR REPLACE FUNCTION pg_catalog.binary_boolean_le(binary, boolean) returns bool LANGUAGE SQL IMMUTABLE STRICT as 'select ($1::float <= $2::int)';
CREATE OPERATOR pg_catalog.<=(leftarg = binary, rightarg = boolean, COMMUTATOR = operator(pg_catalog.>=), procedure = pg_catalog.binary_boolean_le,restrict = scalarltsel, join = scalarltjoinsel);

CREATE OR REPLACE FUNCTION pg_catalog.boolean_tinyblob_eq(boolean, tinyblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::text = $2::text)';
CREATE OPERATOR pg_catalog.=(LEFTARG = boolean, RIGHTARG = tinyblob, COMMUTATOR = OPERATOR(pg_catalog.=), PROCEDURE = pg_catalog.boolean_tinyblob_eq, RESTRICT = eqsel, JOIN = eqjoinsel);
CREATE OR REPLACE FUNCTION pg_catalog.tinyblob_boolean_eq(tinyblob, boolean) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::text = $2::text)';
CREATE OPERATOR pg_catalog.=(LEFTARG = tinyblob, RIGHTARG = boolean, COMMUTATOR = OPERATOR(pg_catalog.=), PROCEDURE = pg_catalog.tinyblob_boolean_eq, RESTRICT = eqsel, JOIN = eqjoinsel);
CREATE OR REPLACE FUNCTION pg_catalog.boolean_tinyblob_ne(boolean, tinyblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::text != $2::text)';
CREATE OPERATOR pg_catalog.<>(LEFTARG = boolean, RIGHTARG = tinyblob, COMMUTATOR = OPERATOR(pg_catalog.<>), PROCEDURE = pg_catalog.boolean_tinyblob_ne, RESTRICT = neqsel, JOIN = neqjoinsel);
CREATE OR REPLACE FUNCTION pg_catalog.tinyblob_boolean_ne(tinyblob, boolean) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::text != $2::text)';
CREATE OPERATOR pg_catalog.<>(LEFTARG = tinyblob, RIGHTARG = boolean, COMMUTATOR = OPERATOR(pg_catalog.<>), PROCEDURE = pg_catalog.tinyblob_boolean_ne, RESTRICT = neqsel, JOIN = neqjoinsel);
CREATE OR REPLACE FUNCTION pg_catalog.boolean_tinyblob_gt(boolean, tinyblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::text > $2::text)';
CREATE OPERATOR pg_catalog.>(LEFTARG = boolean, RIGHTARG = tinyblob, COMMUTATOR = OPERATOR(pg_catalog.<), PROCEDURE = pg_catalog.boolean_tinyblob_gt, RESTRICT = scalargtsel, JOIN = scalargtjoinsel);
CREATE OR REPLACE FUNCTION pg_catalog.tinyblob_boolean_gt(tinyblob, boolean) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::text > $2::text)';
CREATE OPERATOR pg_catalog.>(LEFTARG = tinyblob, RIGHTARG = boolean, COMMUTATOR = OPERATOR(pg_catalog.<), PROCEDURE = pg_catalog.tinyblob_boolean_gt, RESTRICT = scalargtsel, JOIN = scalargtjoinsel);
CREATE OR REPLACE FUNCTION pg_catalog.boolean_tinyblob_lt(boolean, tinyblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::text < $2::text)';
CREATE OPERATOR pg_catalog.<(LEFTARG = boolean, RIGHTARG = tinyblob, COMMUTATOR = OPERATOR(pg_catalog.>), PROCEDURE = pg_catalog.boolean_tinyblob_lt, RESTRICT = scalarltsel, JOIN = scalarltjoinsel);
CREATE OR REPLACE FUNCTION pg_catalog.tinyblob_boolean_lt(tinyblob, boolean) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::text < $2::text)';
CREATE OPERATOR pg_catalog.<(LEFTARG = tinyblob, RIGHTARG = boolean, COMMUTATOR = OPERATOR(pg_catalog.>), PROCEDURE = pg_catalog.tinyblob_boolean_lt, RESTRICT = scalarltsel, JOIN = scalarltjoinsel);
CREATE OR REPLACE FUNCTION pg_catalog.boolean_tinyblob_ge(boolean, tinyblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::text >= $2::text)';
CREATE OPERATOR pg_catalog.>=(LEFTARG = boolean, RIGHTARG = tinyblob, COMMUTATOR = OPERATOR(pg_catalog.<=), PROCEDURE = pg_catalog.boolean_tinyblob_ge, RESTRICT = scalargtsel, JOIN = scalargtjoinsel);
CREATE OR REPLACE FUNCTION pg_catalog.tinyblob_boolean_ge(tinyblob, boolean) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::text >= $2::text)';
CREATE OPERATOR pg_catalog.>=(LEFTARG = tinyblob, RIGHTARG = boolean, COMMUTATOR = OPERATOR(pg_catalog.<=), PROCEDURE = pg_catalog.tinyblob_boolean_ge, RESTRICT = scalargtsel, JOIN = scalargtjoinsel);
CREATE OR REPLACE FUNCTION pg_catalog.boolean_tinyblob_le(boolean, tinyblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::text <= $2::text)';
CREATE OPERATOR pg_catalog.<=(LEFTARG = boolean, RIGHTARG = tinyblob, COMMUTATOR = OPERATOR(pg_catalog.>=), PROCEDURE = pg_catalog.boolean_tinyblob_le, RESTRICT = scalarltsel, JOIN = scalarltjoinsel);
CREATE OR REPLACE FUNCTION pg_catalog.tinyblob_boolean_le(tinyblob, boolean) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::text <= $2::text)';
CREATE OPERATOR pg_catalog.<=(LEFTARG = tinyblob, RIGHTARG = boolean, COMMUTATOR = OPERATOR(pg_catalog.>=), PROCEDURE = pg_catalog.tinyblob_boolean_le, RESTRICT = scalarltsel, JOIN = scalarltjoinsel);

CREATE OR REPLACE FUNCTION pg_catalog.boolean_mediumblob_eq(boolean, mediumblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::text = $2::text)';
CREATE OPERATOR pg_catalog.=(LEFTARG = boolean, RIGHTARG = mediumblob, COMMUTATOR = OPERATOR(pg_catalog.=), PROCEDURE = pg_catalog.boolean_mediumblob_eq, RESTRICT = eqsel, JOIN = eqjoinsel);
CREATE OR REPLACE FUNCTION pg_catalog.mediumblob_boolean_eq(mediumblob, boolean) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::text = $2::text)';
CREATE OPERATOR pg_catalog.=(LEFTARG = mediumblob, RIGHTARG = boolean, COMMUTATOR = OPERATOR(pg_catalog.=), PROCEDURE = pg_catalog.mediumblob_boolean_eq, RESTRICT = eqsel, JOIN = eqjoinsel);
CREATE OR REPLACE FUNCTION pg_catalog.boolean_mediumblob_ne(boolean, mediumblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::text != $2::text)';
CREATE OPERATOR pg_catalog.<>(LEFTARG = boolean, RIGHTARG = mediumblob, COMMUTATOR = OPERATOR(pg_catalog.<>), PROCEDURE = pg_catalog.boolean_mediumblob_ne, RESTRICT = neqsel, JOIN = neqjoinsel);
CREATE OR REPLACE FUNCTION pg_catalog.mediumblob_boolean_ne(mediumblob, boolean) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::text != $2::text)';
CREATE OPERATOR pg_catalog.<>(LEFTARG = mediumblob, RIGHTARG = boolean, COMMUTATOR = OPERATOR(pg_catalog.<>), PROCEDURE = pg_catalog.mediumblob_boolean_ne, RESTRICT = neqsel, JOIN = neqjoinsel);
CREATE OR REPLACE FUNCTION pg_catalog.boolean_mediumblob_gt(boolean, mediumblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::text > $2::text)';
CREATE OPERATOR pg_catalog.>(LEFTARG = boolean, RIGHTARG = mediumblob, COMMUTATOR = OPERATOR(pg_catalog.<), PROCEDURE = pg_catalog.boolean_mediumblob_gt, RESTRICT = scalargtsel, JOIN = scalargtjoinsel);
CREATE OR REPLACE FUNCTION pg_catalog.mediumblob_boolean_gt(mediumblob, boolean) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::text > $2::text)';
CREATE OPERATOR pg_catalog.>(LEFTARG = mediumblob, RIGHTARG = boolean, COMMUTATOR = OPERATOR(pg_catalog.<), PROCEDURE = pg_catalog.mediumblob_boolean_gt, RESTRICT = scalargtsel, JOIN = scalargtjoinsel);
CREATE OR REPLACE FUNCTION pg_catalog.boolean_mediumblob_lt(boolean, mediumblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::text < $2::text)';
CREATE OPERATOR pg_catalog.<(LEFTARG = boolean, RIGHTARG = mediumblob, COMMUTATOR = OPERATOR(pg_catalog.>), PROCEDURE = pg_catalog.boolean_mediumblob_lt, RESTRICT = scalarltsel, JOIN = scalarltjoinsel);
CREATE OR REPLACE FUNCTION pg_catalog.mediumblob_boolean_lt(mediumblob, boolean) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::text < $2::text)';
CREATE OPERATOR pg_catalog.<(LEFTARG = mediumblob, RIGHTARG = boolean, COMMUTATOR = OPERATOR(pg_catalog.>), PROCEDURE = pg_catalog.mediumblob_boolean_lt, RESTRICT = scalarltsel, JOIN = scalarltjoinsel);
CREATE OR REPLACE FUNCTION pg_catalog.boolean_mediumblob_ge(boolean, mediumblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::text >= $2::text)';
CREATE OPERATOR pg_catalog.>=(LEFTARG = boolean, RIGHTARG = mediumblob, COMMUTATOR = OPERATOR(pg_catalog.<=), PROCEDURE = pg_catalog.boolean_mediumblob_ge, RESTRICT = scalargtsel, JOIN = scalargtjoinsel);
CREATE OR REPLACE FUNCTION pg_catalog.mediumblob_boolean_ge(mediumblob, boolean) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::text >= $2::text)';
CREATE OPERATOR pg_catalog.>=(LEFTARG = mediumblob, RIGHTARG = boolean, COMMUTATOR = OPERATOR(pg_catalog.<=), PROCEDURE = pg_catalog.mediumblob_boolean_ge, RESTRICT = scalargtsel, JOIN = scalargtjoinsel);
CREATE OR REPLACE FUNCTION pg_catalog.boolean_mediumblob_le(boolean, mediumblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::text <= $2::text)';
CREATE OPERATOR pg_catalog.<=(LEFTARG = boolean, RIGHTARG = mediumblob, COMMUTATOR = OPERATOR(pg_catalog.>=), PROCEDURE = pg_catalog.boolean_mediumblob_le, RESTRICT = scalarltsel, JOIN = scalarltjoinsel);
CREATE OR REPLACE FUNCTION pg_catalog.mediumblob_boolean_le(mediumblob, boolean) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::text <= $2::text)';
CREATE OPERATOR pg_catalog.<=(LEFTARG = mediumblob, RIGHTARG = boolean, COMMUTATOR = OPERATOR(pg_catalog.>=), PROCEDURE = pg_catalog.mediumblob_boolean_le, RESTRICT = scalarltsel, JOIN = scalarltjoinsel);

CREATE OR REPLACE FUNCTION pg_catalog.boolean_blob_eq(boolean, blob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::text = $2::text)';
CREATE OPERATOR pg_catalog.=(LEFTARG = boolean, RIGHTARG = blob, COMMUTATOR = OPERATOR(pg_catalog.=), PROCEDURE = pg_catalog.boolean_blob_eq, RESTRICT = eqsel, JOIN = eqjoinsel);
CREATE OR REPLACE FUNCTION pg_catalog.blob_boolean_eq(blob, boolean) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::text = $2::text)';
CREATE OPERATOR pg_catalog.=(LEFTARG = blob, RIGHTARG = boolean, COMMUTATOR = OPERATOR(pg_catalog.=), PROCEDURE = pg_catalog.blob_boolean_eq, RESTRICT = eqsel, JOIN = eqjoinsel);
CREATE OR REPLACE FUNCTION pg_catalog.boolean_blob_ne(boolean, blob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::text != $2::text)';
CREATE OPERATOR pg_catalog.<>(LEFTARG = boolean, RIGHTARG = blob, COMMUTATOR = OPERATOR(pg_catalog.<>), PROCEDURE = pg_catalog.boolean_blob_ne, RESTRICT = neqsel, JOIN = neqjoinsel);
CREATE OR REPLACE FUNCTION pg_catalog.blob_boolean_ne(blob, boolean) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::text != $2::text)';
CREATE OPERATOR pg_catalog.<>(LEFTARG = blob, RIGHTARG = boolean, COMMUTATOR = OPERATOR(pg_catalog.<>), PROCEDURE = pg_catalog.blob_boolean_ne, RESTRICT = neqsel, JOIN = neqjoinsel);
CREATE OR REPLACE FUNCTION pg_catalog.boolean_blob_gt(boolean, blob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::text > $2::text)';
CREATE OPERATOR pg_catalog.>(LEFTARG = boolean, RIGHTARG = blob, COMMUTATOR = OPERATOR(pg_catalog.<), PROCEDURE = pg_catalog.boolean_blob_gt, RESTRICT = scalargtsel, JOIN = scalargtjoinsel);
CREATE OR REPLACE FUNCTION pg_catalog.blob_boolean_gt(blob, boolean) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::text > $2::text)';
CREATE OPERATOR pg_catalog.>(LEFTARG = blob, RIGHTARG = boolean, COMMUTATOR = OPERATOR(pg_catalog.<), PROCEDURE = pg_catalog.blob_boolean_gt, RESTRICT = scalargtsel, JOIN = scalargtjoinsel);
CREATE OR REPLACE FUNCTION pg_catalog.boolean_blob_lt(boolean, blob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::text < $2::text)';
CREATE OPERATOR pg_catalog.<(LEFTARG = boolean, RIGHTARG = blob, COMMUTATOR = OPERATOR(pg_catalog.>), PROCEDURE = pg_catalog.boolean_blob_lt, RESTRICT = scalarltsel, JOIN = scalarltjoinsel);
CREATE OR REPLACE FUNCTION pg_catalog.blob_boolean_lt(blob, boolean) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::text < $2::text)';
CREATE OPERATOR pg_catalog.<(LEFTARG = blob, RIGHTARG = boolean, COMMUTATOR = OPERATOR(pg_catalog.>), PROCEDURE = pg_catalog.blob_boolean_lt, RESTRICT = scalarltsel, JOIN = scalarltjoinsel);
CREATE OR REPLACE FUNCTION pg_catalog.boolean_blob_ge(boolean, blob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::text >= $2::text)';
CREATE OPERATOR pg_catalog.>=(LEFTARG = boolean, RIGHTARG = blob, COMMUTATOR = OPERATOR(pg_catalog.<=), PROCEDURE = pg_catalog.boolean_blob_ge, RESTRICT = scalargtsel, JOIN = scalargtjoinsel);
CREATE OR REPLACE FUNCTION pg_catalog.blob_boolean_ge(blob, boolean) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::text >= $2::text)';
CREATE OPERATOR pg_catalog.>=(LEFTARG = blob, RIGHTARG = boolean, COMMUTATOR = OPERATOR(pg_catalog.<=), PROCEDURE = pg_catalog.blob_boolean_ge, RESTRICT = scalargtsel, JOIN = scalargtjoinsel);
CREATE OR REPLACE FUNCTION pg_catalog.boolean_blob_le(boolean, blob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::text <= $2::text)';
CREATE OPERATOR pg_catalog.<=(LEFTARG = boolean, RIGHTARG = blob, COMMUTATOR = OPERATOR(pg_catalog.>=), PROCEDURE = pg_catalog.boolean_blob_le, RESTRICT = scalarltsel, JOIN = scalarltjoinsel);
CREATE OR REPLACE FUNCTION pg_catalog.blob_boolean_le(blob, boolean) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::text <= $2::text)';
CREATE OPERATOR pg_catalog.<=(LEFTARG = blob, RIGHTARG = boolean, COMMUTATOR = OPERATOR(pg_catalog.>=), PROCEDURE = pg_catalog.blob_boolean_le, RESTRICT = scalarltsel, JOIN = scalarltjoinsel);

CREATE OR REPLACE FUNCTION pg_catalog.boolean_longblob_eq(boolean, longblob) returns bool LANGUAGE SQL IMMUTABLE STRICT as 'select ($1::text = $2::text)';
CREATE OPERATOR pg_catalog.=(leftarg = boolean, rightarg = longblob, COMMUTATOR = operator(pg_catalog.=), procedure = pg_catalog.boolean_longblob_eq, restrict = eqsel, join = eqjoinsel);
CREATE OR REPLACE FUNCTION pg_catalog.longblob_boolean_eq(longblob, boolean) returns bool LANGUAGE SQL IMMUTABLE STRICT as 'select ($1::text = $2::text)';
CREATE OPERATOR pg_catalog.=(leftarg = longblob, rightarg = boolean, COMMUTATOR = operator(pg_catalog.=), procedure = pg_catalog.longblob_boolean_eq, restrict = eqsel, join = eqjoinsel);
CREATE OR REPLACE FUNCTION pg_catalog.boolean_longblob_ne(boolean, longblob) returns bool LANGUAGE SQL IMMUTABLE STRICT as 'select ($1::text != $2::text)';
CREATE OPERATOR pg_catalog.<>(leftarg = boolean, rightarg = longblob, COMMUTATOR = operator(pg_catalog.<>), procedure = pg_catalog.boolean_longblob_ne,restrict = neqsel, join = neqjoinsel);
CREATE OR REPLACE FUNCTION pg_catalog.longblob_boolean_ne(longblob, boolean) returns bool LANGUAGE SQL IMMUTABLE STRICT as 'select ($1::text != $2::text)';
CREATE OPERATOR pg_catalog.<>(leftarg = longblob, rightarg = boolean, COMMUTATOR = operator(pg_catalog.<>), procedure = pg_catalog.longblob_boolean_ne,restrict = neqsel, join = neqjoinsel);
CREATE OR REPLACE FUNCTION pg_catalog.boolean_longblob_gt(boolean, longblob) returns bool LANGUAGE SQL IMMUTABLE STRICT as 'select ($1::text > $2::text)';
CREATE OPERATOR pg_catalog.>(leftarg = boolean, rightarg = longblob, COMMUTATOR = operator(pg_catalog.<), procedure = pg_catalog.boolean_longblob_gt,restrict = scalargtsel, join = scalargtjoinsel);
CREATE OR REPLACE FUNCTION pg_catalog.longblob_boolean_gt(longblob, boolean) returns bool LANGUAGE SQL IMMUTABLE STRICT as 'select ($1::text > $2::text)';
CREATE OPERATOR pg_catalog.>(leftarg = longblob, rightarg = boolean, COMMUTATOR = operator(pg_catalog.<), procedure = pg_catalog.longblob_boolean_gt,restrict = scalargtsel, join = scalargtjoinsel);
CREATE OR REPLACE FUNCTION pg_catalog.boolean_longblob_lt(boolean, longblob) returns bool LANGUAGE SQL IMMUTABLE STRICT as 'select ($1::text < $2::text)';
CREATE OPERATOR pg_catalog.<(leftarg = boolean, rightarg = longblob, COMMUTATOR = operator(pg_catalog.>), procedure = pg_catalog.boolean_longblob_lt,restrict = scalarltsel, join = scalarltjoinsel);
CREATE OR REPLACE FUNCTION pg_catalog.longblob_boolean_lt(longblob, boolean) returns bool LANGUAGE SQL IMMUTABLE STRICT as 'select ($1::text < $2::text)';
CREATE OPERATOR pg_catalog.<(leftarg = longblob, rightarg = boolean, COMMUTATOR = operator(pg_catalog.>), procedure = pg_catalog.longblob_boolean_lt,restrict = scalarltsel, join = scalarltjoinsel);
CREATE OR REPLACE FUNCTION pg_catalog.boolean_longblob_ge(boolean, longblob) returns bool LANGUAGE SQL IMMUTABLE STRICT as 'select ($1::text >= $2::text)';
CREATE OPERATOR pg_catalog.>=(leftarg = boolean, rightarg = longblob, COMMUTATOR = operator(pg_catalog.<=), procedure = pg_catalog.boolean_longblob_ge,restrict = scalargtsel, join = scalargtjoinsel);
CREATE OR REPLACE FUNCTION pg_catalog.longblob_boolean_ge(longblob, boolean) returns bool LANGUAGE SQL IMMUTABLE STRICT as 'select ($1::text >= $2::text)';
CREATE OPERATOR pg_catalog.>=(leftarg = longblob, rightarg = boolean, COMMUTATOR = operator(pg_catalog.<=), procedure = pg_catalog.longblob_boolean_ge,restrict = scalargtsel, join = scalargtjoinsel);
CREATE OR REPLACE FUNCTION pg_catalog.boolean_longblob_le(boolean, longblob) returns bool LANGUAGE SQL IMMUTABLE STRICT as 'select ($1::text <= $2::text)';
CREATE OPERATOR pg_catalog.<=(leftarg = boolean, rightarg = longblob, COMMUTATOR = operator(pg_catalog.>=), procedure = pg_catalog.boolean_longblob_le,restrict = scalarltsel, join = scalarltjoinsel);
CREATE OR REPLACE FUNCTION pg_catalog.longblob_boolean_le(longblob, boolean) returns bool LANGUAGE SQL IMMUTABLE STRICT as 'select ($1::text <= $2::text)';
CREATE OPERATOR pg_catalog.<=(leftarg = longblob, rightarg = boolean, COMMUTATOR = operator(pg_catalog.>=), procedure = pg_catalog.longblob_boolean_le,restrict = scalarltsel, join = scalarltjoinsel);

CREATE OR REPLACE FUNCTION pg_catalog.year_binary_eq(year, binary) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::int = $2::float)';
CREATE OPERATOR pg_catalog.=(LEFTARG = year, RIGHTARG = binary, COMMUTATOR = OPERATOR(pg_catalog.=), PROCEDURE = pg_catalog.year_binary_eq, RESTRICT = eqsel, JOIN = eqjoinsel);
CREATE OR REPLACE FUNCTION pg_catalog.binary_year_eq(binary, year) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::float = $2::int)';
CREATE OPERATOR pg_catalog.=(LEFTARG = binary, RIGHTARG = year, COMMUTATOR = OPERATOR(pg_catalog.=), PROCEDURE = pg_catalog.binary_year_eq, RESTRICT = eqsel, JOIN = eqjoinsel);
CREATE OR REPLACE FUNCTION pg_catalog.year_binary_ne(year, binary) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::int != $2::float)';
CREATE OPERATOR pg_catalog.<>(LEFTARG = year, RIGHTARG = binary, COMMUTATOR = OPERATOR(pg_catalog.<>), PROCEDURE = pg_catalog.year_binary_ne, RESTRICT = neqsel, JOIN = neqjoinsel);
CREATE OR REPLACE FUNCTION pg_catalog.binary_year_ne(binary, year) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::float != $2::int)';
CREATE OPERATOR pg_catalog.<>(LEFTARG = binary, RIGHTARG = year, COMMUTATOR = OPERATOR(pg_catalog.<>), PROCEDURE = pg_catalog.binary_year_ne, RESTRICT = neqsel, JOIN = neqjoinsel);
CREATE OR REPLACE FUNCTION pg_catalog.year_binary_gt(year, binary) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::int > $2::float)';
CREATE OPERATOR pg_catalog.>(LEFTARG = year, RIGHTARG = binary, COMMUTATOR = OPERATOR(pg_catalog.<), PROCEDURE = pg_catalog.year_binary_gt, RESTRICT = scalargtsel, JOIN = scalargtjoinsel);
CREATE OR REPLACE FUNCTION pg_catalog.binary_year_gt(binary, year) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::float > $2::int)';
CREATE OPERATOR pg_catalog.>(LEFTARG = binary, RIGHTARG = year, COMMUTATOR = OPERATOR(pg_catalog.<), PROCEDURE = pg_catalog.binary_year_gt, RESTRICT = scalargtsel, JOIN = scalargtjoinsel);
CREATE OR REPLACE FUNCTION pg_catalog.year_binary_lt(year, binary) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::int < $2::float)';
CREATE OPERATOR pg_catalog.<(LEFTARG = year, RIGHTARG = binary, COMMUTATOR = OPERATOR(pg_catalog.>), PROCEDURE = pg_catalog.year_binary_lt, RESTRICT = scalarltsel, JOIN = scalarltjoinsel);
CREATE OR REPLACE FUNCTION pg_catalog.binary_year_lt(binary, year) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::float < $2::int)';
CREATE OPERATOR pg_catalog.<(LEFTARG = binary, RIGHTARG = year, COMMUTATOR = OPERATOR(pg_catalog.>), PROCEDURE = pg_catalog.binary_year_lt, RESTRICT = scalarltsel, JOIN = scalarltjoinsel);
CREATE OR REPLACE FUNCTION pg_catalog.year_binary_ge(year, binary) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::int >= $2::float)';
CREATE OPERATOR pg_catalog.>=(LEFTARG = year, RIGHTARG = binary, COMMUTATOR = OPERATOR(pg_catalog.<=), PROCEDURE = pg_catalog.year_binary_ge, RESTRICT = scalargtsel, JOIN = scalargtjoinsel);
CREATE OR REPLACE FUNCTION pg_catalog.binary_year_ge(binary, year) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::float >= $2::int)';
CREATE OPERATOR pg_catalog.>=(LEFTARG = binary, RIGHTARG = year, COMMUTATOR = OPERATOR(pg_catalog.<=), PROCEDURE = pg_catalog.binary_year_ge, RESTRICT = scalargtsel, JOIN = scalargtjoinsel);
CREATE OR REPLACE FUNCTION pg_catalog.year_binary_le(year, binary) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::int <= $2::float)';
CREATE OPERATOR pg_catalog.<=(LEFTARG = year, RIGHTARG = binary, COMMUTATOR = OPERATOR(pg_catalog.>=), PROCEDURE = pg_catalog.year_binary_le, RESTRICT = scalarltsel, JOIN = scalarltjoinsel);
CREATE OR REPLACE FUNCTION pg_catalog.binary_year_le(binary, year) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::float <= $2::int)';
CREATE OPERATOR pg_catalog.<=(LEFTARG = binary, RIGHTARG = year, COMMUTATOR = OPERATOR(pg_catalog.>=), PROCEDURE = pg_catalog.binary_year_le, RESTRICT = scalarltsel, JOIN = scalarltjoinsel);

CREATE OR REPLACE FUNCTION pg_catalog.year_tinyblob_eq(year, tinyblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::text = $2::text)';
CREATE OPERATOR pg_catalog.=(LEFTARG = year, RIGHTARG = tinyblob, COMMUTATOR = OPERATOR(pg_catalog.=), PROCEDURE = pg_catalog.year_tinyblob_eq, RESTRICT = eqsel, JOIN = eqjoinsel);
CREATE OR REPLACE FUNCTION pg_catalog.tinyblob_year_eq(tinyblob, year) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::text = $2::text)';
CREATE OPERATOR pg_catalog.=(LEFTARG = tinyblob, RIGHTARG = year, COMMUTATOR = OPERATOR(pg_catalog.=), PROCEDURE = pg_catalog.tinyblob_year_eq, RESTRICT = eqsel, JOIN = eqjoinsel);
CREATE OR REPLACE FUNCTION pg_catalog.year_tinyblob_ne(year, tinyblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::text != $2::text)';
CREATE OPERATOR pg_catalog.<>(LEFTARG = year, RIGHTARG = tinyblob, COMMUTATOR = OPERATOR(pg_catalog.<>), PROCEDURE = pg_catalog.year_tinyblob_ne, RESTRICT = neqsel, JOIN = neqjoinsel);
CREATE OR REPLACE FUNCTION pg_catalog.tinyblob_year_ne(tinyblob, year) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::text != $2::text)';
CREATE OPERATOR pg_catalog.<>(LEFTARG = tinyblob, RIGHTARG = year, COMMUTATOR = OPERATOR(pg_catalog.<>), PROCEDURE = pg_catalog.tinyblob_year_ne, RESTRICT = neqsel, JOIN = neqjoinsel);
CREATE OR REPLACE FUNCTION pg_catalog.year_tinyblob_gt(year, tinyblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::text > $2::text)';
CREATE OPERATOR pg_catalog.>(LEFTARG = year, RIGHTARG = tinyblob, COMMUTATOR = OPERATOR(pg_catalog.<), PROCEDURE = pg_catalog.year_tinyblob_gt, RESTRICT = scalargtsel, JOIN = scalargtjoinsel);
CREATE OR REPLACE FUNCTION pg_catalog.tinyblob_year_gt(tinyblob, year) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::text > $2::text)';
CREATE OPERATOR pg_catalog.>(LEFTARG = tinyblob, RIGHTARG = year, COMMUTATOR = OPERATOR(pg_catalog.<), PROCEDURE = pg_catalog.tinyblob_year_gt, RESTRICT = scalargtsel, JOIN = scalargtjoinsel);
CREATE OR REPLACE FUNCTION pg_catalog.year_tinyblob_lt(year, tinyblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::text < $2::text)';
CREATE OPERATOR pg_catalog.<(LEFTARG = year, RIGHTARG = tinyblob, COMMUTATOR = OPERATOR(pg_catalog.>), PROCEDURE = pg_catalog.year_tinyblob_lt, RESTRICT = scalarltsel, JOIN = scalarltjoinsel);
CREATE OR REPLACE FUNCTION pg_catalog.tinyblob_year_lt(tinyblob, year) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::text < $2::text)';
CREATE OPERATOR pg_catalog.<(LEFTARG = tinyblob, RIGHTARG = year, COMMUTATOR = OPERATOR(pg_catalog.>), PROCEDURE = pg_catalog.tinyblob_year_lt, RESTRICT = scalarltsel, JOIN = scalarltjoinsel);
CREATE OR REPLACE FUNCTION pg_catalog.year_tinyblob_ge(year, tinyblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::text >= $2::text)';
CREATE OPERATOR pg_catalog.>=(LEFTARG = year, RIGHTARG = tinyblob, COMMUTATOR = OPERATOR(pg_catalog.<=), PROCEDURE = pg_catalog.year_tinyblob_ge, RESTRICT = scalargtsel, JOIN = scalargtjoinsel);
CREATE OR REPLACE FUNCTION pg_catalog.tinyblob_year_ge(tinyblob, year) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::text >= $2::text)';
CREATE OPERATOR pg_catalog.>=(LEFTARG = tinyblob, RIGHTARG = year, COMMUTATOR = OPERATOR(pg_catalog.<=), PROCEDURE = pg_catalog.tinyblob_year_ge, RESTRICT = scalargtsel, JOIN = scalargtjoinsel);
CREATE OR REPLACE FUNCTION pg_catalog.year_tinyblob_le(year, tinyblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::text <= $2::text)';
CREATE OPERATOR pg_catalog.<=(LEFTARG = year, RIGHTARG = tinyblob, COMMUTATOR = OPERATOR(pg_catalog.>=), PROCEDURE = pg_catalog.year_tinyblob_le, RESTRICT = scalarltsel, JOIN = scalarltjoinsel);
CREATE OR REPLACE FUNCTION pg_catalog.tinyblob_year_le(tinyblob, year) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::text <= $2::text)';
CREATE OPERATOR pg_catalog.<=(LEFTARG = tinyblob, RIGHTARG = year, COMMUTATOR = OPERATOR(pg_catalog.>=), PROCEDURE = pg_catalog.tinyblob_year_le, RESTRICT = scalarltsel, JOIN = scalarltjoinsel);

CREATE OR REPLACE FUNCTION pg_catalog.year_mediumblob_eq(year, mediumblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::text = $2::text)';
CREATE OPERATOR pg_catalog.=(LEFTARG = year, RIGHTARG = mediumblob, COMMUTATOR = OPERATOR(pg_catalog.=), PROCEDURE = pg_catalog.year_mediumblob_eq, RESTRICT = eqsel, JOIN = eqjoinsel);
CREATE OR REPLACE FUNCTION pg_catalog.mediumblob_year_eq(mediumblob, year) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::text = $2::text)';
CREATE OPERATOR pg_catalog.=(LEFTARG = mediumblob, RIGHTARG = year, COMMUTATOR = OPERATOR(pg_catalog.=), PROCEDURE = pg_catalog.mediumblob_year_eq, RESTRICT = eqsel, JOIN = eqjoinsel);
CREATE OR REPLACE FUNCTION pg_catalog.year_mediumblob_ne(year, mediumblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::text != $2::text)';
CREATE OPERATOR pg_catalog.<>(LEFTARG = year, RIGHTARG = mediumblob, COMMUTATOR = OPERATOR(pg_catalog.<>), PROCEDURE = pg_catalog.year_mediumblob_ne, RESTRICT = neqsel, JOIN = neqjoinsel);
CREATE OR REPLACE FUNCTION pg_catalog.mediumblob_year_ne(mediumblob, year) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::text != $2::text)';
CREATE OPERATOR pg_catalog.<>(LEFTARG = mediumblob, RIGHTARG = year, COMMUTATOR = OPERATOR(pg_catalog.<>), PROCEDURE = pg_catalog.mediumblob_year_ne, RESTRICT = neqsel, JOIN = neqjoinsel);
CREATE OR REPLACE FUNCTION pg_catalog.year_mediumblob_gt(year, mediumblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::text > $2::text)';
CREATE OPERATOR pg_catalog.>(LEFTARG = year, RIGHTARG = mediumblob, COMMUTATOR = OPERATOR(pg_catalog.<), PROCEDURE = pg_catalog.year_mediumblob_gt, RESTRICT = scalargtsel, JOIN = scalargtjoinsel);
CREATE OR REPLACE FUNCTION pg_catalog.mediumblob_year_gt(mediumblob, year) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::text > $2::text)';
CREATE OPERATOR pg_catalog.>(LEFTARG = mediumblob, RIGHTARG = year, COMMUTATOR = OPERATOR(pg_catalog.<), PROCEDURE = pg_catalog.mediumblob_year_gt, RESTRICT = scalargtsel, JOIN = scalargtjoinsel);
CREATE OR REPLACE FUNCTION pg_catalog.year_mediumblob_lt(year, mediumblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::text < $2::text)';
CREATE OPERATOR pg_catalog.<(LEFTARG = year, RIGHTARG = mediumblob, COMMUTATOR = OPERATOR(pg_catalog.>), PROCEDURE = pg_catalog.year_mediumblob_lt, RESTRICT = scalarltsel, JOIN = scalarltjoinsel);
CREATE OR REPLACE FUNCTION pg_catalog.mediumblob_year_lt(mediumblob, year) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::text < $2::text)';
CREATE OPERATOR pg_catalog.<(LEFTARG = mediumblob, RIGHTARG = year, COMMUTATOR = OPERATOR(pg_catalog.>), PROCEDURE = pg_catalog.mediumblob_year_lt, RESTRICT = scalarltsel, JOIN = scalarltjoinsel);
CREATE OR REPLACE FUNCTION pg_catalog.year_mediumblob_ge(year, mediumblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::text >= $2::text)';
CREATE OPERATOR pg_catalog.>=(LEFTARG = year, RIGHTARG = mediumblob, COMMUTATOR = OPERATOR(pg_catalog.<=), PROCEDURE = pg_catalog.year_mediumblob_ge, RESTRICT = scalargtsel, JOIN = scalargtjoinsel);
CREATE OR REPLACE FUNCTION pg_catalog.mediumblob_year_ge(mediumblob, year) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::text >= $2::text)';
CREATE OPERATOR pg_catalog.>=(LEFTARG = mediumblob, RIGHTARG = year, COMMUTATOR = OPERATOR(pg_catalog.<=), PROCEDURE = pg_catalog.mediumblob_year_ge, RESTRICT = scalargtsel, JOIN = scalargtjoinsel);
CREATE OR REPLACE FUNCTION pg_catalog.year_mediumblob_le(year, mediumblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::text <= $2::text)';
CREATE OPERATOR pg_catalog.<=(LEFTARG = year, RIGHTARG = mediumblob, COMMUTATOR = OPERATOR(pg_catalog.>=), PROCEDURE = pg_catalog.year_mediumblob_le, RESTRICT = scalarltsel, JOIN = scalarltjoinsel);
CREATE OR REPLACE FUNCTION pg_catalog.mediumblob_year_le(mediumblob, year) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::text <= $2::text)';
CREATE OPERATOR pg_catalog.<=(LEFTARG = mediumblob, RIGHTARG = year, COMMUTATOR = OPERATOR(pg_catalog.>=), PROCEDURE = pg_catalog.mediumblob_year_le, RESTRICT = scalarltsel, JOIN = scalarltjoinsel);

CREATE OR REPLACE FUNCTION pg_catalog.year_blob_eq(year, blob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::text = $2::text)';
CREATE OPERATOR pg_catalog.=(LEFTARG = year, RIGHTARG = blob, COMMUTATOR = OPERATOR(pg_catalog.=), PROCEDURE = pg_catalog.year_blob_eq, RESTRICT = eqsel, JOIN = eqjoinsel);
CREATE OR REPLACE FUNCTION pg_catalog.blob_year_eq(blob, year) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::text = $2::text)';
CREATE OPERATOR pg_catalog.=(LEFTARG = blob, RIGHTARG = year, COMMUTATOR = OPERATOR(pg_catalog.=), PROCEDURE = pg_catalog.blob_year_eq, RESTRICT = eqsel, JOIN = eqjoinsel);
CREATE OR REPLACE FUNCTION pg_catalog.year_blob_ne(year, blob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::text != $2::text)';
CREATE OPERATOR pg_catalog.<>(LEFTARG = year, RIGHTARG = blob, COMMUTATOR = OPERATOR(pg_catalog.<>), PROCEDURE = pg_catalog.year_blob_ne, RESTRICT = neqsel, JOIN = neqjoinsel);
CREATE OR REPLACE FUNCTION pg_catalog.blob_year_ne(blob, year) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::text != $2::text)';
CREATE OPERATOR pg_catalog.<>(LEFTARG = blob, RIGHTARG = year, COMMUTATOR = OPERATOR(pg_catalog.<>), PROCEDURE = pg_catalog.blob_year_ne, RESTRICT = neqsel, JOIN = neqjoinsel);
CREATE OR REPLACE FUNCTION pg_catalog.year_blob_gt(year, blob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::text > $2::text)';
CREATE OPERATOR pg_catalog.>(LEFTARG = year, RIGHTARG = blob, COMMUTATOR = OPERATOR(pg_catalog.<), PROCEDURE = pg_catalog.year_blob_gt, RESTRICT = scalargtsel, JOIN = scalargtjoinsel);
CREATE OR REPLACE FUNCTION pg_catalog.blob_year_gt(blob, year) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::text > $2::text)';
CREATE OPERATOR pg_catalog.>(LEFTARG = blob, RIGHTARG = year, COMMUTATOR = OPERATOR(pg_catalog.<), PROCEDURE = pg_catalog.blob_year_gt, RESTRICT = scalargtsel, JOIN = scalargtjoinsel);
CREATE OR REPLACE FUNCTION pg_catalog.year_blob_lt(year, blob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::text < $2::text)';
CREATE OPERATOR pg_catalog.<(LEFTARG = year, RIGHTARG = blob, COMMUTATOR = OPERATOR(pg_catalog.>), PROCEDURE = pg_catalog.year_blob_lt, RESTRICT = scalarltsel, JOIN = scalarltjoinsel);
CREATE OR REPLACE FUNCTION pg_catalog.blob_year_lt(blob, year) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::text < $2::text)';
CREATE OPERATOR pg_catalog.<(LEFTARG = blob, RIGHTARG = year, COMMUTATOR = OPERATOR(pg_catalog.>), PROCEDURE = pg_catalog.blob_year_lt, RESTRICT = scalarltsel, JOIN = scalarltjoinsel);
CREATE OR REPLACE FUNCTION pg_catalog.year_blob_ge(year, blob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::text >= $2::text)';
CREATE OPERATOR pg_catalog.>=(LEFTARG = year, RIGHTARG = blob, COMMUTATOR = OPERATOR(pg_catalog.<=), PROCEDURE = pg_catalog.year_blob_ge, RESTRICT = scalargtsel, JOIN = scalargtjoinsel);
CREATE OR REPLACE FUNCTION pg_catalog.blob_year_ge(blob, year) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::text >= $2::text)';
CREATE OPERATOR pg_catalog.>=(LEFTARG = blob, RIGHTARG = year, COMMUTATOR = OPERATOR(pg_catalog.<=), PROCEDURE = pg_catalog.blob_year_ge, RESTRICT = scalargtsel, JOIN = scalargtjoinsel);
CREATE OR REPLACE FUNCTION pg_catalog.year_blob_le(year, blob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::text <= $2::text)';
CREATE OPERATOR pg_catalog.<=(LEFTARG = year, RIGHTARG = blob, COMMUTATOR = OPERATOR(pg_catalog.>=), PROCEDURE = pg_catalog.year_blob_le, RESTRICT = scalarltsel, JOIN = scalarltjoinsel);
CREATE OR REPLACE FUNCTION pg_catalog.blob_year_le(blob, year) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::text <= $2::text)';
CREATE OPERATOR pg_catalog.<=(LEFTARG = blob, RIGHTARG = year, COMMUTATOR = OPERATOR(pg_catalog.>=), PROCEDURE = pg_catalog.blob_year_le, RESTRICT = scalarltsel, JOIN = scalarltjoinsel);

CREATE OR REPLACE FUNCTION pg_catalog.year_longblob_eq(year, longblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::text = $2::text)';
CREATE OPERATOR pg_catalog.=(LEFTARG = year, RIGHTARG = longblob, COMMUTATOR = OPERATOR(pg_catalog.=), PROCEDURE = pg_catalog.year_longblob_eq, RESTRICT = eqsel, JOIN = eqjoinsel);
CREATE OR REPLACE FUNCTION pg_catalog.longblob_year_eq(longblob, year) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::text = $2::text)';
CREATE OPERATOR pg_catalog.=(LEFTARG = longblob, RIGHTARG = year, COMMUTATOR = OPERATOR(pg_catalog.=), PROCEDURE = pg_catalog.longblob_year_eq, RESTRICT = eqsel, JOIN = eqjoinsel);
CREATE OR REPLACE FUNCTION pg_catalog.year_longblob_ne(year, longblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::text != $2::text)';
CREATE OPERATOR pg_catalog.<>(LEFTARG = year, RIGHTARG = longblob, COMMUTATOR = OPERATOR(pg_catalog.<>), PROCEDURE = pg_catalog.year_longblob_ne, RESTRICT = neqsel, JOIN = neqjoinsel);
CREATE OR REPLACE FUNCTION pg_catalog.longblob_year_ne(longblob, year) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::text != $2::text)';
CREATE OPERATOR pg_catalog.<>(LEFTARG = longblob, RIGHTARG = year, COMMUTATOR = OPERATOR(pg_catalog.<>), PROCEDURE = pg_catalog.longblob_year_ne, RESTRICT = neqsel, JOIN = neqjoinsel);
CREATE OR REPLACE FUNCTION pg_catalog.year_longblob_gt(year, longblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::text > $2::text)';
CREATE OPERATOR pg_catalog.>(LEFTARG = year, RIGHTARG = longblob, COMMUTATOR = OPERATOR(pg_catalog.<), PROCEDURE = pg_catalog.year_longblob_gt, RESTRICT = scalargtsel, JOIN = scalargtjoinsel);
CREATE OR REPLACE FUNCTION pg_catalog.longblob_year_gt(longblob, year) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::text > $2::text)';
CREATE OPERATOR pg_catalog.>(LEFTARG = longblob, RIGHTARG = year, COMMUTATOR = OPERATOR(pg_catalog.<), PROCEDURE = pg_catalog.longblob_year_gt, RESTRICT = scalargtsel, JOIN = scalargtjoinsel);
CREATE OR REPLACE FUNCTION pg_catalog.year_longblob_lt(year, longblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::text < $2::text)';
CREATE OPERATOR pg_catalog.<(LEFTARG = year, RIGHTARG = longblob, COMMUTATOR = OPERATOR(pg_catalog.>), PROCEDURE = pg_catalog.year_longblob_lt, RESTRICT = scalarltsel, JOIN = scalarltjoinsel);
CREATE OR REPLACE FUNCTION pg_catalog.longblob_year_lt(longblob, year) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::text < $2::text)';
CREATE OPERATOR pg_catalog.<(LEFTARG = longblob, RIGHTARG = year, COMMUTATOR = OPERATOR(pg_catalog.>), PROCEDURE = pg_catalog.longblob_year_lt, RESTRICT = scalarltsel, JOIN = scalarltjoinsel);
CREATE OR REPLACE FUNCTION pg_catalog.year_longblob_ge(year, longblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::text >= $2::text)';
CREATE OPERATOR pg_catalog.>=(LEFTARG = year, RIGHTARG = longblob, COMMUTATOR = OPERATOR(pg_catalog.<=), PROCEDURE = pg_catalog.year_longblob_ge, RESTRICT = scalargtsel, JOIN = scalargtjoinsel);
CREATE OR REPLACE FUNCTION pg_catalog.longblob_year_ge(longblob, year) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::text >= $2::text)';
CREATE OPERATOR pg_catalog.>=(LEFTARG = longblob, RIGHTARG = year, COMMUTATOR = OPERATOR(pg_catalog.<=), PROCEDURE = pg_catalog.longblob_year_ge, RESTRICT = scalargtsel, JOIN = scalargtjoinsel);
CREATE OR REPLACE FUNCTION pg_catalog.year_longblob_le(year, longblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::text <= $2::text)';
CREATE OPERATOR pg_catalog.<=(LEFTARG = year, RIGHTARG = longblob, COMMUTATOR = OPERATOR(pg_catalog.>=), PROCEDURE = pg_catalog.year_longblob_le, RESTRICT = scalarltsel, JOIN = scalarltjoinsel);
CREATE OR REPLACE FUNCTION pg_catalog.longblob_year_le(longblob, year) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::text <= $2::text)';
CREATE OPERATOR pg_catalog.<=(LEFTARG = longblob, RIGHTARG = year, COMMUTATOR = OPERATOR(pg_catalog.>=), PROCEDURE = pg_catalog.longblob_year_le, RESTRICT = scalarltsel, JOIN = scalarltjoinsel);

CREATE OR REPLACE FUNCTION pg_catalog.int1_binary_eq(int1, binary) returns bool LANGUAGE SQL IMMUTABLE STRICT as 'select ($1 = $2::float)';
CREATE OPERATOR pg_catalog.=(leftarg = int1, rightarg = binary, COMMUTATOR = operator(pg_catalog.=), procedure = pg_catalog.int1_binary_eq, restrict = eqsel, join = eqjoinsel);
CREATE OR REPLACE FUNCTION pg_catalog.binary_int1_eq(binary, int1) returns bool LANGUAGE SQL IMMUTABLE STRICT as 'select ($1::float = $2)';
CREATE OPERATOR pg_catalog.=(leftarg = binary, rightarg = int1, COMMUTATOR = operator(pg_catalog.=), procedure = pg_catalog.binary_int1_eq, restrict = eqsel, join = eqjoinsel);
CREATE OR REPLACE FUNCTION pg_catalog.int1_binary_ne(int1, binary) returns bool LANGUAGE SQL IMMUTABLE STRICT as 'select ($1::float != $2)';
CREATE OPERATOR pg_catalog.<>(leftarg = int1, rightarg = binary, COMMUTATOR = operator(pg_catalog.<>), procedure = pg_catalog.int1_binary_ne,restrict = neqsel, join = neqjoinsel);
CREATE OR REPLACE FUNCTION pg_catalog.binary_int1_ne(binary, int1) returns bool LANGUAGE SQL IMMUTABLE STRICT as 'select ($1 != $2::float)';
CREATE OPERATOR pg_catalog.<>(leftarg = binary, rightarg = int1, COMMUTATOR = operator(pg_catalog.<>), procedure = pg_catalog.binary_int1_ne,restrict = neqsel, join = neqjoinsel);

CREATE OR REPLACE FUNCTION pg_catalog.text_binary_eq(text, binary) returns bool LANGUAGE SQL IMMUTABLE STRICT as 'select ($1::binary = $2)';
CREATE OPERATOR pg_catalog.=(leftarg = text, rightarg = binary, COMMUTATOR = operator(pg_catalog.=), procedure = pg_catalog.text_binary_eq, restrict = eqsel, join = eqjoinsel);
CREATE OR REPLACE FUNCTION pg_catalog.binary_text_eq(binary, text) returns bool LANGUAGE SQL IMMUTABLE STRICT as 'select ($1 = $2::binary)';
CREATE OPERATOR pg_catalog.=(leftarg = binary, rightarg = text, COMMUTATOR = operator(pg_catalog.=), procedure = pg_catalog.binary_text_eq, restrict = eqsel, join = eqjoinsel);

DROP CAST (timestamptz AS year);
DROP CAST (timestamp(0) without time zone AS year);
DROP CAST (date AS year);

DROP FUNCTION IF EXISTS pg_catalog.timestamp_year(timestamptz) cascade;
DROP FUNCTION IF EXISTS pg_catalog.datetime_year(timestamp(0) without time zone) cascade;
DROP FUNCTION IF EXISTS pg_catalog.date_year(date) cascade;

CREATE OR REPLACE FUNCTION pg_catalog.timestamp_year(timestamptz) RETURNS year LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'timestamptz_year';
CREATE OR REPLACE FUNCTION pg_catalog.datetime_year(timestamp(0) without time zone) RETURNS year LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'datetime_year';
CREATE OR REPLACE FUNCTION pg_catalog.date_year(date) RETURNS year LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'date_year';

CREATE CAST (timestamptz AS year) with function pg_catalog.timestamp_year(timestamptz) AS ASSIGNMENT;
CREATE CAST (timestamp(0) without time zone AS year) with function pg_catalog.datetime_year(timestamp(0) without time zone) AS ASSIGNMENT;
CREATE CAST (date as year) with function pg_catalog.date_year(date) AS ASSIGNMENT;

DROP CAST IF EXISTS (FLOAT8 AS NVARCHAR2);
CREATE OR REPLACE FUNCTION pg_catalog.float8_nvarchar2(FLOAT8) RETURNS NVARCHAR2 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'float8_nvarchar2';
CREATE CAST (FLOAT8 AS NVARCHAR2) WITH FUNCTION pg_catalog.float8_nvarchar2(FLOAT8) AS IMPLICIT;

DROP CAST IF EXISTS (FLOAT4 AS NVARCHAR2);
CREATE OR REPLACE FUNCTION pg_catalog.float4_nvarchar2(FLOAT4) RETURNS NVARCHAR2 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'float4_nvarchar2';
CREATE CAST (FLOAT4 AS NVARCHAR2) WITH FUNCTION pg_catalog.float4_nvarchar2(FLOAT4) AS IMPLICIT;

DROP FUNCTION IF EXISTS pg_catalog.json_contains("any", "any", text);
DROP FUNCTION IF EXISTS pg_catalog.json_contains("any", "any");
DROP FUNCTION IF EXISTS pg_catalog.json_valid("any");
CREATE OR REPLACE FUNCTION pg_catalog.json_contains("any", "any", text) RETURNS int8 LANGUAGE C IMMUTABLE as '$libdir/dolphin', 'json_contains';
CREATE OR REPLACE FUNCTION pg_catalog.json_contains("any", "any") RETURNS int8 LANGUAGE C IMMUTABLE as '$libdir/dolphin', 'json_contains';
CREATE OR REPLACE FUNCTION pg_catalog.json_valid("any") RETURNS int8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'json_valid';

DROP FUNCTION pg_catalog.json_length("any");
DROP FUNCTION pg_catalog.json_length("any",text);
DROP FUNCTION pg_catalog.json_depth("any");
DROP FUNCTION pg_catalog.json_storage_size("any");
CREATE OR REPLACE FUNCTION pg_catalog.json_length("any") RETURNS int8 LANGUAGE C IMMUTABLE as '$libdir/dolphin', 'json_length';
CREATE OR REPLACE FUNCTION pg_catalog.json_length("any",text) RETURNS int8 LANGUAGE C IMMUTABLE as '$libdir/dolphin', 'json_length';
CREATE OR REPLACE FUNCTION pg_catalog.json_depth("any") RETURNS int8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'json_depth';
CREATE OR REPLACE FUNCTION pg_catalog.json_storage_size("any") RETURNS int8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'json_storage_size';

DROP FUNCTION IF EXISTS pg_catalog.b_extract (text, year);
CREATE OR REPLACE FUNCTION pg_catalog.b_extract (text, year) RETURNS int8 LANGUAGE SQL STABLE STRICT as $$ SELECT pg_catalog.b_extract($1, $2::text) $$;
DROP FUNCTION IF EXISTS pg_catalog.yearweek (year);
CREATE OR REPLACE FUNCTION pg_catalog.yearweek (year) RETURNS int8 LANGUAGE SQL STABLE STRICT as $$ SELECT pg_catalog.yearweek($1::text) $$;
DROP FUNCTION IF EXISTS pg_catalog.makedate (year, int8);
CREATE OR REPLACE FUNCTION pg_catalog.makedate (year, int8) RETURNS date AS $$ SELECT pg_catalog.makedate(cast($1 as int8), cast($2 as int8)) $$ LANGUAGE SQL;
DROP FUNCTION IF EXISTS pg_catalog.b_timestampdiff(text,year,year);
DROP FUNCTION IF EXISTS pg_catalog.b_timestampdiff(text,text,year);
DROP FUNCTION IF EXISTS pg_catalog.b_timestampdiff(text,year,text);
CREATE OR REPLACE FUNCTION pg_catalog.b_timestampdiff(text,year,year) RETURNS int8 AS $$ SELECT pg_catalog.b_timestampdiff($1, $3::text, $2::text) $$ LANGUAGE SQL;
CREATE OR REPLACE FUNCTION pg_catalog.b_timestampdiff(text,text,year) RETURNS int8 AS $$ SELECT -pg_catalog.b_timestampdiff($1, $3::text, $2) $$ LANGUAGE SQL;
CREATE OR REPLACE FUNCTION pg_catalog.b_timestampdiff(text,year,text) RETURNS int8 AS $$ SELECT -pg_catalog.b_timestampdiff($1, $3, $2::text) $$ LANGUAGE SQL;
DROP FUNCTION IF EXISTS pg_catalog.date_add (year, interval);
CREATE OR REPLACE FUNCTION pg_catalog.date_add (year, interval) RETURNS text AS $$ SELECT pg_catalog.adddate($1::text, $2)  $$ LANGUAGE SQL;
DROP FUNCTION IF EXISTS pg_catalog.date_sub (year, interval);
CREATE OR REPLACE FUNCTION pg_catalog.date_sub (year, interval) RETURNS text AS $$ SELECT pg_catalog.adddate($1::text, -$2)  $$ LANGUAGE SQL;

DROP FUNCTION IF EXISTS pg_catalog.is_ipv4(bit) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.is_ipv4(boolean) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.is_ipv4(year) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.is_ipv4(blob) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.is_ipv4(anyenum) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.is_ipv4(json) CASCADE;

CREATE OR REPLACE FUNCTION pg_catalog.is_ipv4(bit) RETURNS int4 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.is_ipv4(cast($1 as text))';
CREATE OR REPLACE FUNCTION pg_catalog.is_ipv4(boolean) RETURNS int4 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.is_ipv4(cast($1 as text))';
CREATE OR REPLACE FUNCTION pg_catalog.is_ipv4(year) RETURNS int4 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.is_ipv4(cast($1 as text))';
CREATE OR REPLACE FUNCTION pg_catalog.is_ipv4(blob) RETURNS int4 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.is_ipv4(cast($1 as text))';
CREATE OR REPLACE FUNCTION pg_catalog.is_ipv4(anyenum) RETURNS int4 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.is_ipv4(cast($1 as text))';
CREATE OR REPLACE FUNCTION pg_catalog.is_ipv4(json) RETURNS int4 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.is_ipv4(cast($1 as text))';

DROP AGGREGATE IF EXISTS pg_catalog.bit_and(char);
DROP AGGREGATE IF EXISTS pg_catalog.bit_and(varchar);
DROP AGGREGATE IF EXISTS pg_catalog.bit_and(binary);
DROP AGGREGATE IF EXISTS pg_catalog.bit_and(tinyblob);
DROP AGGREGATE IF EXISTS pg_catalog.bit_and(blob);
DROP AGGREGATE IF EXISTS pg_catalog.bit_and(mediumblob);
DROP AGGREGATE IF EXISTS pg_catalog.bit_and(longblob);
DROP AGGREGATE IF EXISTS pg_catalog.bit_and(text);
DROP AGGREGATE IF EXISTS pg_catalog.bit_and(json);
DROP FUNCTION IF EXISTS pg_catalog.binaryand(binary, binary) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.varbinary_and_binary(varbinary, binary) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.varbinaryand(varbinary, varbinary) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.bloband(blob, blob) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.varbinary_and_tinyblob(varbinary, tinyblob) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.varbinary_and_blob(varbinary, blob) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.varbinary_and_mediumblob(varbinary, mediumblob) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.varbinary_and_longblob(varbinary, longblob) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.text_and_uint8(uint8, text) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.uint8and(uint8, char) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.uint8and(uint8, varchar) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.uint8and(uint8, json) CASCADE;

CREATE OR REPLACE FUNCTION pg_catalog.binaryand(binary, binary) RETURNS varbinary LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'binaryand';
CREATE OR REPLACE FUNCTION pg_catalog.varbinary_and_binary(varbinary, binary) RETURNS varbinary LANGUAGE C IMMUTABLE as '$libdir/dolphin', 'varbinary_and_binary';
CREATE OR REPLACE FUNCTION pg_catalog.varbinaryand(varbinary, varbinary) RETURNS varbinary LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'varbinaryand';
CREATE OR REPLACE FUNCTION pg_catalog.bloband(blob, blob) RETURNS blob LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'bloband';
CREATE OR REPLACE FUNCTION pg_catalog.varbinary_and_tinyblob(varbinary, tinyblob) RETURNS varbinary LANGUAGE C IMMUTABLE as '$libdir/dolphin', 'varbinary_and_tinyblob';
CREATE OR REPLACE FUNCTION pg_catalog.varbinary_and_blob(varbinary, blob) RETURNS varbinary LANGUAGE C IMMUTABLE as '$libdir/dolphin', 'varbinary_and_blob';
CREATE OR REPLACE FUNCTION pg_catalog.varbinary_and_mediumblob(varbinary, mediumblob) RETURNS varbinary LANGUAGE C IMMUTABLE as '$libdir/dolphin', 'varbinary_and_mediumblob';
CREATE OR REPLACE FUNCTION pg_catalog.varbinary_and_longblob(varbinary, longblob) RETURNS varbinary LANGUAGE C IMMUTABLE as '$libdir/dolphin', 'varbinary_and_longblob';
CREATE OR REPLACE FUNCTION pg_catalog.text_and_uint8(uint8, text) RETURNS uint8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'text_and_uint8';
CREATE OR REPLACE FUNCTION pg_catalog.uint8and(uint8, char) RETURNS uint8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.uint8and($1, cast($2 as uint8))';
CREATE OR REPLACE FUNCTION pg_catalog.uint8and(uint8, varchar) RETURNS uint8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.uint8and($1, cast($2 as uint8))';
CREATE OR REPLACE FUNCTION pg_catalog.uint8and(uint8, json) RETURNS uint8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.uint8and($1, cast($2 as uint8))';
CREATE OPERATOR pg_catalog.&(leftarg = binary, rightarg = binary, procedure = pg_catalog.binaryand);
CREATE OPERATOR pg_catalog.&(leftarg = blob, rightarg = blob, procedure = pg_catalog.bloband);
CREATE AGGREGATE pg_catalog.bit_and(char) (SFUNC = pg_catalog.uint8and, cFUNC = pg_catalog.uint8and, STYPE = uint8, initcond = '18446744073709551615');
CREATE AGGREGATE pg_catalog.bit_and(varchar) (SFUNC = pg_catalog.uint8and, cFUNC = pg_catalog.uint8and, STYPE = uint8, initcond = '18446744073709551615');
CREATE AGGREGATE pg_catalog.bit_and(binary) (SFUNC = pg_catalog.varbinary_and_binary, STYPE = varbinary);
CREATE AGGREGATE pg_catalog.bit_and(varbinary) (SFUNC = pg_catalog.varbinaryand, STYPE = varbinary);
CREATE AGGREGATE pg_catalog.bit_and(tinyblob) (SFUNC = pg_catalog.varbinary_and_tinyblob, STYPE = varbinary);
CREATE AGGREGATE pg_catalog.bit_and(blob) (SFUNC = pg_catalog.varbinary_and_blob, STYPE = varbinary);
CREATE AGGREGATE pg_catalog.bit_and(mediumblob) (SFUNC = pg_catalog.varbinary_and_mediumblob, STYPE = varbinary);
CREATE AGGREGATE pg_catalog.bit_and(longblob) (SFUNC = pg_catalog.varbinary_and_longblob, STYPE = varbinary);
CREATE AGGREGATE pg_catalog.bit_and(text) (SFUNC = pg_catalog.text_and_uint8, STYPE = uint8, initcond = '18446744073709551615');
CREATE AGGREGATE pg_catalog.bit_and(json) (SFUNC = pg_catalog.uint8and, cFUNC = pg_catalog.uint8and, STYPE = uint8, initcond = '18446744073709551615');

DROP FUNCTION IF EXISTS pg_catalog.inet6_ntoa(bit) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.inet6_ntoa(boolean) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.inet6_ntoa(year) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.inet6_ntoa(blob) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.inet6_ntoa(anyenum) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.inet6_ntoa(json) CASCADE;

CREATE OR REPLACE FUNCTION pg_catalog.inet6_ntoa(bit) RETURNS varchar LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.inet6_ntoa(cast($1 as text))';
CREATE OR REPLACE FUNCTION pg_catalog.inet6_ntoa(boolean) RETURNS varchar LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.inet6_ntoa(cast($1 as text))';
CREATE OR REPLACE FUNCTION pg_catalog.inet6_ntoa(year) RETURNS varchar LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'inetntop';
CREATE OR REPLACE FUNCTION pg_catalog.inet6_ntoa(blob) RETURNS varchar LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.inet6_ntoa(cast($1 as text))';
CREATE OR REPLACE FUNCTION pg_catalog.inet6_ntoa(anyenum) RETURNS varchar LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.inet6_ntoa(cast($1 as text))';
CREATE OR REPLACE FUNCTION pg_catalog.inet6_ntoa(json) RETURNS varchar LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'inetntop';

DROP FUNCTION IF EXISTS pg_catalog.md5(bit) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.md5(boolean) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.md5(year) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.md5(blob) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.md5(anyenum) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.md5(json) CASCADE;

CREATE OR REPLACE FUNCTION pg_catalog.md5(bit) RETURNS varchar LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.md5(cast($1 as text))';
CREATE OR REPLACE FUNCTION pg_catalog.md5(boolean) RETURNS varchar LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.md5(cast($1 as text))';
CREATE OR REPLACE FUNCTION pg_catalog.md5(year) RETURNS varchar LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.md5(cast($1 as text))';
CREATE OR REPLACE FUNCTION pg_catalog.md5(blob) RETURNS varchar LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.md5(cast($1 as text))';
CREATE OR REPLACE FUNCTION pg_catalog.md5(anyenum) RETURNS varchar LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.md5(cast($1 as text))';
CREATE OR REPLACE FUNCTION pg_catalog.md5(json) RETURNS varchar LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.md5(cast($1 as text))';
DROP OPERATOR CLASS IF EXISTS uint2_ops USING hash;
DROP OPERATOR CLASS IF EXISTS uint4_ops USING hash;
DROP OPERATOR CLASS IF EXISTS uint2_ops USING btree;
DROP OPERATOR CLASS IF EXISTS uint4_ops USING btree;


DO $for_upgrade_only$
DECLARE
  ans boolean;
  v_isinplaceupgrade boolean;
BEGIN
    select case when count(*)=1 then true else false end as ans from (select setting from pg_settings where name = 'upgrade_mode' and setting != '0') into ans;
    show isinplaceupgrade into v_isinplaceupgrade;
    -- we can do drop operator only during upgrade
    if ans = true and v_isinplaceupgrade = true then
        drop operator IF EXISTS pg_catalog.=(uint2, int8);
        CREATE OPERATOR pg_catalog.=(
        leftarg = uint2, rightarg = int8, procedure = uint2_int8_eq,
        restrict = eqsel, join = eqjoinsel, commutator = operator(pg_catalog.=),
        HASHES, MERGES
        );
        drop operator IF EXISTS pg_catalog.=(uint4, int8);
        CREATE OPERATOR pg_catalog.=(
        leftarg = uint4, rightarg = int8, procedure = uint4_int8_eq,
        restrict = eqsel, join = eqjoinsel, commutator=operator(pg_catalog.=),
        HASHES, MERGES
        );
    end if;
END
$for_upgrade_only$;

CREATE FUNCTION pg_catalog.int8_uint4_eq (
int8,uint4
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int8_uint4_eq';

CREATE OPERATOR pg_catalog.=(
leftarg = int8, rightarg = uint4, procedure = int8_uint4_eq, commutator = operator(pg_catalog.=),
restrict = eqsel, join = eqjoinsel, HASHES, MERGES
);

CREATE or replace FUNCTION pg_catalog.int8_uint2_eq (
int8,uint2
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int8_uint2_eq';

CREATE OPERATOR pg_catalog.=(
leftarg = int8, rightarg = uint2, procedure = int8_uint2_eq, commutator = operator(pg_catalog.=),
restrict = eqsel, join = eqjoinsel, HASHES, MERGES
);

CREATE OPERATOR CLASS uint2_ops
    DEFAULT FOR TYPE uint2 USING btree family integer_ops AS
        OPERATOR        1       < ,
        OPERATOR        1       <(uint2, uint4),
        OPERATOR        1       <(uint2, uint8),
        OPERATOR        1       <(uint2, int2),
        OPERATOR        1       <(uint2, int4),
        OPERATOR        1       <(uint2, int8),
        OPERATOR        2       <= ,
        OPERATOR        2       <=(uint2, uint4),
        OPERATOR        2       <=(uint2, uint8),
        OPERATOR        2       <=(uint2, int2),
        OPERATOR        2       <=(uint2, int4),
        OPERATOR        2       <=(uint2, int8),
        OPERATOR        3       = ,
        OPERATOR        3       =(uint2, uint4),
        OPERATOR        3       =(uint2, uint8),
        OPERATOR        3       =(uint2, int2),
        OPERATOR        3       =(uint2, int4),
        OPERATOR        3       =(uint2, int8),
        OPERATOR        4       >= ,
        OPERATOR        4       >=(uint2, uint4),
        OPERATOR        4       >=(uint2, uint8),
        OPERATOR        4       >=(uint2, int2),
        OPERATOR        4       >=(uint2, int4),
        OPERATOR        4       >=(uint2, int8),
        OPERATOR        5       > ,
        OPERATOR        5       >(uint2, uint4),
        OPERATOR        5       >(uint2, uint8),
        OPERATOR        5       >(uint2, int2),
        OPERATOR        5       >(uint2, int4),
        OPERATOR        5       >(uint2, int8),
        FUNCTION        1       uint2cmp(uint2, uint2),
        FUNCTION        1       uint24cmp(uint2, uint4),
        FUNCTION        1       uint28cmp(uint2, uint8),
        FUNCTION        1       uint2_int2cmp(uint2, int2),
        FUNCTION        1       uint2_int4cmp(uint2, int4),
        FUNCTION        1       uint2_int8cmp(uint2, int8),
        FUNCTION        2       uint2_sortsupport(internal);

CREATE OPERATOR CLASS uint4_ops
    DEFAULT FOR TYPE uint4 USING btree family integer_ops AS
        OPERATOR        1       < ,
        OPERATOR        1       <(uint4, uint8),
        OPERATOR        1       <(uint4, int4),
        OPERATOR        1       <(uint4, int8),
        OPERATOR        2       <= ,
        OPERATOR        2       <=(uint4, uint8),
        OPERATOR        2       <=(uint4, int4),
        OPERATOR        2       <=(uint4, int8),
        OPERATOR        3       = ,
        OPERATOR        3       =(uint4, uint8),
        OPERATOR        3       =(uint4, int4),
        OPERATOR        3       =(uint4, int8),
        OPERATOR        4       >= ,
        OPERATOR        4       >=(uint4, uint8),
        OPERATOR        4       >=(uint4, int4),
        OPERATOR        4       >=(uint4, int8),
        OPERATOR        5       > ,
        OPERATOR        5       >(uint4, uint8),
        OPERATOR        5       >(uint4, int4),
        OPERATOR        5       >(uint4, int8),
        FUNCTION        1       uint4cmp(uint4, uint4),
        FUNCTION        1       uint48cmp(uint4, uint8),
        FUNCTION        1       uint4_int4cmp(uint4, int4),
        FUNCTION        1       uint4_int8cmp(uint4, int8),
        FUNCTION        2       uint4_sortsupport(internal);

CREATE OPERATOR CLASS uint2_ops
    DEFAULT FOR TYPE uint2 USING hash family integer_ops AS
        OPERATOR        1       = ,
        OPERATOR        1       =(uint2, uint4),
        OPERATOR        1       =(uint2, uint8),
        OPERATOR        1       =(uint2, int2),
        OPERATOR        1       =(uint2, int4),
        OPERATOR        1       =(uint2, int8),
        OPERATOR        1       =(int8, uint2),
        OPERATOR        1       =(int2, uint2),
        FUNCTION        1       hashuint2(uint2);

CREATE OPERATOR CLASS uint4_ops
    DEFAULT FOR TYPE uint4 USING hash family integer_ops AS
        OPERATOR        1       = ,
        OPERATOR        1       =(uint4, uint8),
        OPERATOR        1       =(uint4, int4),
        OPERATOR        1       =(uint4, int8),
        OPERATOR        1       =(int8, uint4),
        OPERATOR        1       =(int4, uint4),
        FUNCTION        1       hashuint4(uint4);

DROP FUNCTION IF EXISTS pg_catalog.xor(integer, integer);
CREATE OR REPLACE FUNCTION pg_catalog.xor(int8, int8) RETURNS integer LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'dolphin_int8xor';
CREATE OR REPLACE FUNCTION pg_catalog.xor(int8, bit) RETURNS integer LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.xor($1, $2::int8)';
CREATE OR REPLACE FUNCTION pg_catalog.xor(bit, int8) RETURNS integer LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.xor($1::int8, $2)';
CREATE OR REPLACE FUNCTION pg_catalog.xor(bit, bit) RETURNS integer LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.xor($1::int8, $2::int8)';
CREATE OR REPLACE FUNCTION pg_catalog.xor(int8, text) RETURNS integer LANGUAGE SQL IMMUTABLE as 'select pg_catalog.xor($1::int8, $2::bool::int8)';
CREATE OR REPLACE FUNCTION pg_catalog.xor(text, int8) RETURNS integer LANGUAGE SQL IMMUTABLE as 'select pg_catalog.xor($1::bool::int8, $2::int8)';
CREATE OR REPLACE FUNCTION pg_catalog.xor(text, text) RETURNS integer LANGUAGE SQL IMMUTABLE as 'select pg_catalog.xor($1::bool::int8, $2::bool::int8)';
CREATE OR REPLACE FUNCTION pg_catalog.xor(text, bit) RETURNS integer LANGUAGE SQL IMMUTABLE as 'select pg_catalog.xor($1::bool::int8, $2::int8)';
CREATE OR REPLACE FUNCTION pg_catalog.xor(bit, text) RETURNS integer LANGUAGE SQL IMMUTABLE as 'select pg_catalog.xor($1::int8, $2::bool::int8)';
CREATE OR REPLACE FUNCTION pg_catalog.xor(varbinary, int8) RETURNS integer LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.xor($1::int8, $2)';
CREATE OR REPLACE FUNCTION pg_catalog.xor(int8, varbinary) RETURNS integer LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.xor($1, $2::int8)';
CREATE OR REPLACE FUNCTION pg_catalog.xor(varbinary, varbinary) RETURNS integer LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.xor($1::int8, $2::int8)';
CREATE OR REPLACE FUNCTION pg_catalog.xor(varbinary, bit) RETURNS integer LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.xor($1::int8, $2::int8)';
CREATE OR REPLACE FUNCTION pg_catalog.xor(bit, varbinary) RETURNS integer LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.xor($1::int8, $2::int8)';
CREATE OR REPLACE FUNCTION pg_catalog.xor(varbinary, text) RETURNS integer LANGUAGE SQL IMMUTABLE as 'select pg_catalog.xor($1::int8, $2::bool::int8)';
CREATE OR REPLACE FUNCTION pg_catalog.xor(text, varbinary) RETURNS integer LANGUAGE SQL IMMUTABLE as 'select pg_catalog.xor($1::bool::int8, $2::int8)';
CREATE OR REPLACE FUNCTION pg_catalog.xor(binary, int8) RETURNS integer LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.xor($1::int8, $2)';
CREATE OR REPLACE FUNCTION pg_catalog.xor(int8, binary) RETURNS integer LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.xor($1, $2::int8)';
CREATE OR REPLACE FUNCTION pg_catalog.xor(binary, binary) RETURNS integer LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.xor($1::int8, $2::int8)';
CREATE OR REPLACE FUNCTION pg_catalog.xor(binary, bit) RETURNS integer LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.xor($1::int8, $2::int8)';
CREATE OR REPLACE FUNCTION pg_catalog.xor(bit, binary) RETURNS integer LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.xor($1::int8, $2::int8)';
CREATE OR REPLACE FUNCTION pg_catalog.xor(varbinary, binary) RETURNS integer LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.xor($1::int8, $2::int8)';
CREATE OR REPLACE FUNCTION pg_catalog.xor(binary, varbinary) RETURNS integer LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.xor($1::int8, $2::int8)';
CREATE OR REPLACE FUNCTION pg_catalog.xor(binary, text) RETURNS integer LANGUAGE SQL IMMUTABLE as 'select pg_catalog.xor($1::int8, $2::bool::int8)';
CREATE OR REPLACE FUNCTION pg_catalog.xor(text, binary) RETURNS integer LANGUAGE SQL IMMUTABLE as 'select pg_catalog.xor($1::bool::int8, $2::int8)';
CREATE OR REPLACE FUNCTION pg_catalog.xor(varchar, int8) RETURNS integer LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.xor($1::int8, $2)';
CREATE OR REPLACE FUNCTION pg_catalog.xor(int8, varchar) RETURNS integer LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.xor($1, $2::int8)';
CREATE OR REPLACE FUNCTION pg_catalog.xor(varchar, varchar) RETURNS integer LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.xor($1::int8, $2::int8)';
CREATE OR REPLACE FUNCTION pg_catalog.xor(varchar, bit) RETURNS integer LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.xor($1::int8, $2::int8)';
CREATE OR REPLACE FUNCTION pg_catalog.xor(bit, varchar) RETURNS integer LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.xor($1::int8, $2::int8)';
CREATE OR REPLACE FUNCTION pg_catalog.xor(varbinary, varchar) RETURNS integer LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.xor($1::int8, $2::int8)';
CREATE OR REPLACE FUNCTION pg_catalog.xor(varchar, varbinary) RETURNS integer LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.xor($1::int8, $2::int8)';
CREATE OR REPLACE FUNCTION pg_catalog.xor(varchar, text) RETURNS integer LANGUAGE SQL IMMUTABLE as 'select pg_catalog.xor($1::int8, $2::bool::int8)';
CREATE OR REPLACE FUNCTION pg_catalog.xor(text, varchar) RETURNS integer LANGUAGE SQL IMMUTABLE as 'select pg_catalog.xor($1::bool::int8, $2::int8)';
CREATE OR REPLACE FUNCTION pg_catalog.xor(binary, varchar) RETURNS integer LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.xor($1::int8, $2::int8)';
CREATE OR REPLACE FUNCTION pg_catalog.xor(varchar, binary) RETURNS integer LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.xor($1::int8, $2::int8)';
CREATE OR REPLACE FUNCTION pg_catalog.xor(char, int8) RETURNS integer LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.xor($1::int8, $2)';
CREATE OR REPLACE FUNCTION pg_catalog.xor(int8, char) RETURNS integer LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.xor($1, $2::int8)';
CREATE OR REPLACE FUNCTION pg_catalog.xor(char, char) RETURNS integer LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.xor($1::int8, $2::int8)';
CREATE OR REPLACE FUNCTION pg_catalog.xor(char, bit) RETURNS integer LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.xor($1::int8, $2::int8)';
CREATE OR REPLACE FUNCTION pg_catalog.xor(bit, char) RETURNS integer LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.xor($1::int8, $2::int8)';
CREATE OR REPLACE FUNCTION pg_catalog.xor(varbinary, char) RETURNS integer LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.xor($1::int8, $2::int8)';
CREATE OR REPLACE FUNCTION pg_catalog.xor(char, varbinary) RETURNS integer LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.xor($1::int8, $2::int8)';
CREATE OR REPLACE FUNCTION pg_catalog.xor(char, text) RETURNS integer LANGUAGE SQL IMMUTABLE as 'select pg_catalog.xor($1::int8, $2::bool::int8)';
CREATE OR REPLACE FUNCTION pg_catalog.xor(text, char) RETURNS integer LANGUAGE SQL IMMUTABLE as 'select pg_catalog.xor($1::bool::int8, $2::int8)';
CREATE OR REPLACE FUNCTION pg_catalog.xor(binary, char) RETURNS integer LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.xor($1::int8, $2::int8)';
CREATE OR REPLACE FUNCTION pg_catalog.xor(char, binary) RETURNS integer LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.xor($1::int8, $2::int8)';
CREATE OR REPLACE FUNCTION pg_catalog.xor(varchar, char) RETURNS integer LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.xor($1::int8, $2::int8)';
CREATE OR REPLACE FUNCTION pg_catalog.xor(char, varchar) RETURNS integer LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.xor($1::int8, $2::int8)';
CREATE OR REPLACE FUNCTION pg_catalog.xor(unknown,unknown) RETURNS integer LANGUAGE SQL IMMUTABLE as 'select pg_catalog.xor($1::text::int8, $2::text::int8)';
CREATE OR REPLACE FUNCTION pg_catalog.xor(unknown, int8) RETURNS integer LANGUAGE SQL IMMUTABLE as 'select pg_catalog.xor($1::text::int8, $2)';
CREATE OR REPLACE FUNCTION pg_catalog.xor(int8, unknown) RETURNS integer LANGUAGE SQL IMMUTABLE as 'select pg_catalog.xor($1, $2::text::int8)';
CREATE OR REPLACE FUNCTION pg_catalog.xor(integer, unknown) RETURNS integer LANGUAGE SQL IMMUTABLE as 'select pg_catalog.xor($1::int8, $2::text::int8)';
CREATE OR REPLACE FUNCTION pg_catalog.xor(unknown, integer) RETURNS integer LANGUAGE SQL IMMUTABLE as 'select pg_catalog.xor($1::text::int8, $2::int8)';
CREATE OR REPLACE FUNCTION pg_catalog.xor(unknown, float8) RETURNS integer LANGUAGE SQL IMMUTABLE as 'select pg_catalog.xor($1::text::int8, $2::int8)';
CREATE OR REPLACE FUNCTION pg_catalog.xor(float8, unknown) RETURNS integer LANGUAGE SQL IMMUTABLE as 'select pg_catalog.xor($1::int8, $2::text::int8)';
CREATE OR REPLACE FUNCTION pg_catalog.xor(unknown, boolean) RETURNS integer LANGUAGE SQL IMMUTABLE as 'select pg_catalog.xor($1::text::int8, $2::int8)';
CREATE OR REPLACE FUNCTION pg_catalog.xor(boolean, unknown) RETURNS integer LANGUAGE SQL IMMUTABLE as 'select pg_catalog.xor($1::int8, $2::text::int8)';
CREATE OR REPLACE FUNCTION pg_catalog.xor(unknown, bit) RETURNS integer LANGUAGE SQL IMMUTABLE as 'select pg_catalog.xor($1::text::int8, $2::int8)';
CREATE OR REPLACE FUNCTION pg_catalog.xor(bit, unknown) RETURNS integer LANGUAGE SQL IMMUTABLE as 'select pg_catalog.xor($1::int8, $2::text::int8)';
CREATE OR REPLACE FUNCTION pg_catalog.xor(unknown, text) RETURNS integer LANGUAGE SQL IMMUTABLE as 'select pg_catalog.xor($1::text::int8, $2::bool::int8)';
CREATE OR REPLACE FUNCTION pg_catalog.xor(text, unknown) RETURNS integer LANGUAGE SQL IMMUTABLE as 'select pg_catalog.xor($1::bool::int8, $2::text::int8)';
CREATE OR REPLACE FUNCTION pg_catalog.xor(unknown, binary) RETURNS integer LANGUAGE SQL IMMUTABLE as 'select pg_catalog.xor($1::text::int8, $2::int8)';
CREATE OR REPLACE FUNCTION pg_catalog.xor(binary, unknown) RETURNS integer LANGUAGE SQL IMMUTABLE as 'select pg_catalog.xor($1::int8, $2::text::int8)';
CREATE OR REPLACE FUNCTION pg_catalog.xor(varbinary, unknown) RETURNS integer LANGUAGE SQL IMMUTABLE as 'select pg_catalog.xor($1::int8, $2::text::int8)';
CREATE OR REPLACE FUNCTION pg_catalog.xor(unknown, varbinary) RETURNS integer LANGUAGE SQL IMMUTABLE as 'select pg_catalog.xor($1::text::int8, $2::int8)';
CREATE OR REPLACE FUNCTION pg_catalog.xor(varchar, unknown) RETURNS integer LANGUAGE SQL IMMUTABLE as 'select pg_catalog.xor($1::int8, $2::text::int8)';
CREATE OR REPLACE FUNCTION pg_catalog.xor(unknown, varchar) RETURNS integer LANGUAGE SQL IMMUTABLE as 'select pg_catalog.xor($1::text::int8, $2::int8)';

CREATE OR REPLACE FUNCTION pg_catalog.round(int1) RETURNS int8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.round(cast($1 as number))::int8';
CREATE OR REPLACE FUNCTION pg_catalog.round(uint1) RETURNS int8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.round(cast($1 as number))::int8';
CREATE OR REPLACE FUNCTION pg_catalog.round(int2) RETURNS int8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.round(cast($1 as number))::int8';
CREATE OR REPLACE FUNCTION pg_catalog.round(uint2) RETURNS int8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.round(cast($1 as number))::int8';
CREATE OR REPLACE FUNCTION pg_catalog.round(int4) RETURNS int8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.round(cast($1 as number))::int8';
CREATE OR REPLACE FUNCTION pg_catalog.round(uint4) RETURNS int8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.round(cast($1 as number))::int8';
CREATE OR REPLACE FUNCTION pg_catalog.round(int8) RETURNS int8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.round(cast($1 as number))::int8';
CREATE OR REPLACE FUNCTION pg_catalog.round(uint8) RETURNS int8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.round(cast($1 as number))::int8';
CREATE OR REPLACE FUNCTION pg_catalog.round(boolean) RETURNS int8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.round(cast($1 as number))::int8';
CREATE OR REPLACE FUNCTION pg_catalog.round(year) RETURNS int8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.round(cast($1 as number))::int8';

CREATE OR REPLACE FUNCTION pg_catalog.round(int1, int4) RETURNS int8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.round(cast($1 as number), $2)::int8';
CREATE OR REPLACE FUNCTION pg_catalog.round(uint1, int4) RETURNS int8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.round(cast($1 as number), $2)::int8';
CREATE OR REPLACE FUNCTION pg_catalog.round(int2, int4) RETURNS int8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.round(cast($1 as number), $2)::int8';
CREATE OR REPLACE FUNCTION pg_catalog.round(uint2, int4) RETURNS int8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.round(cast($1 as number), $2)::int8';
CREATE OR REPLACE FUNCTION pg_catalog.round(int4, int4) RETURNS int8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.round(cast($1 as number), $2)::int8';
CREATE OR REPLACE FUNCTION pg_catalog.round(uint4, int4) RETURNS int8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.round(cast($1 as number), $2)::int8';
CREATE OR REPLACE FUNCTION pg_catalog.round(int8, int4) RETURNS int8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.round(cast($1 as number), $2)::int8';
CREATE OR REPLACE FUNCTION pg_catalog.round(uint8, int4) RETURNS int8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.round(cast($1 as number), $2)::int8';
CREATE OR REPLACE FUNCTION pg_catalog.round(boolean, int4) RETURNS int8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.round(cast($1 as number), $2)::int8';
CREATE OR REPLACE FUNCTION pg_catalog.round(year, int4) RETURNS int8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.round(cast($1 as number), $2)::int8';
CREATE OR REPLACE FUNCTION pg_catalog.round(binary, int4) RETURNS number LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.round(cast($1 as number), $2)';
CREATE OR REPLACE FUNCTION pg_catalog.round(json, int4) RETURNS number LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.round(cast($1 as number), $2)';

CREATE OR REPLACE FUNCTION pg_catalog.round(int1, uint4) RETURNS int8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.round(cast($1 as number), cast($2 as int4))::int8';
CREATE OR REPLACE FUNCTION pg_catalog.round(uint1, uint4) RETURNS int8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.round(cast($1 as number), cast($2 as int4))::int8';
CREATE OR REPLACE FUNCTION pg_catalog.round(int2, uint4) RETURNS int8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.round(cast($1 as number), cast($2 as int4))::int8';
CREATE OR REPLACE FUNCTION pg_catalog.round(uint2, uint4) RETURNS int8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.round(cast($1 as number), cast($2 as int4))::int8';
CREATE OR REPLACE FUNCTION pg_catalog.round(int4, uint4) RETURNS int8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.round(cast($1 as number), cast($2 as int4))::int8';
CREATE OR REPLACE FUNCTION pg_catalog.round(uint4, uint4) RETURNS int8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.round(cast($1 as number), cast($2 as int4))::int8';
CREATE OR REPLACE FUNCTION pg_catalog.round(int8, uint4) RETURNS int8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.round(cast($1 as number), cast($2 as int4))::int8';
CREATE OR REPLACE FUNCTION pg_catalog.round(uint8, uint4) RETURNS int8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.round(cast($1 as number), cast($2 as int4))::int8';
CREATE OR REPLACE FUNCTION pg_catalog.round(boolean, uint4) RETURNS int8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.round(cast($1 as number), cast($2 as int4))::int8';
CREATE OR REPLACE FUNCTION pg_catalog.round(year, uint4) RETURNS int8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.round(cast($1 as number), cast($2 as int4))::int8';
CREATE OR REPLACE FUNCTION pg_catalog.round(binary, uint4) RETURNS number LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.round(cast($1 as number), cast($2 as int4))';
CREATE OR REPLACE FUNCTION pg_catalog.round(json, uint4) RETURNS number LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.round(cast($1 as number), cast($2 as int4))';

CREATE OR REPLACE FUNCTION pg_catalog.truncate(int1, int4) RETURNS int8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.truncate(cast($1 as number), $2)::int8';
CREATE OR REPLACE FUNCTION pg_catalog.truncate(uint1, int4) RETURNS int8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.truncate(cast($1 as number), $2)::int8';
CREATE OR REPLACE FUNCTION pg_catalog.truncate(int2, int4) RETURNS int8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.truncate(cast($1 as number), $2)::int8';
CREATE OR REPLACE FUNCTION pg_catalog.truncate(uint2, int4) RETURNS int8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.truncate(cast($1 as number), $2)::int8';
CREATE OR REPLACE FUNCTION pg_catalog.truncate(int4, int4) RETURNS int8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.truncate(cast($1 as number), $2)::int8';
CREATE OR REPLACE FUNCTION pg_catalog.truncate(uint4, int4) RETURNS int8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.truncate(cast($1 as number), $2)::int8';
CREATE OR REPLACE FUNCTION pg_catalog.truncate(int8, int4) RETURNS int8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.truncate(cast($1 as number), $2)::int8';
CREATE OR REPLACE FUNCTION pg_catalog.truncate(uint8, int4) RETURNS int8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.truncate(cast($1 as number), $2)::int8';
CREATE OR REPLACE FUNCTION pg_catalog.truncate(boolean, int4) RETURNS int8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.truncate(cast($1 as number), $2)::int8';
CREATE OR REPLACE FUNCTION pg_catalog.truncate(year, int4) RETURNS int8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.truncate(cast($1 as number), $2)::int8';
CREATE OR REPLACE FUNCTION pg_catalog.truncate(binary, int4) RETURNS number LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.truncate(cast($1 as number), $2)';
CREATE OR REPLACE FUNCTION pg_catalog.truncate(json, int4) RETURNS number LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.truncate(cast($1 as number), $2)';

CREATE OR REPLACE FUNCTION pg_catalog.truncate(int1, uint4) RETURNS int8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.truncate(cast($1 as number), cast($2 as int4))::int8';
CREATE OR REPLACE FUNCTION pg_catalog.truncate(uint1, uint4) RETURNS int8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.truncate(cast($1 as number), cast($2 as int4))::int8';
CREATE OR REPLACE FUNCTION pg_catalog.truncate(int2, uint4) RETURNS int8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.truncate(cast($1 as number), cast($2 as int4))::int8';
CREATE OR REPLACE FUNCTION pg_catalog.truncate(uint2, uint4) RETURNS int8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.truncate(cast($1 as number), cast($2 as int4))::int8';
CREATE OR REPLACE FUNCTION pg_catalog.truncate(int4, uint4) RETURNS int8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.truncate(cast($1 as number), cast($2 as int4))::int8';
CREATE OR REPLACE FUNCTION pg_catalog.truncate(uint4, uint4) RETURNS int8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.truncate(cast($1 as number), cast($2 as int4))::int8';
CREATE OR REPLACE FUNCTION pg_catalog.truncate(int8, uint4) RETURNS int8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.truncate(cast($1 as number), cast($2 as int4))::int8';
CREATE OR REPLACE FUNCTION pg_catalog.truncate(uint8, uint4) RETURNS int8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.truncate(cast($1 as number), cast($2 as int4))::int8';
CREATE OR REPLACE FUNCTION pg_catalog.truncate(boolean, uint4) RETURNS int8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.truncate(cast($1 as number), cast($2 as int4))::int8';
CREATE OR REPLACE FUNCTION pg_catalog.truncate(year, uint4) RETURNS int8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.truncate(cast($1 as number), cast($2 as int4))::int8';
CREATE OR REPLACE FUNCTION pg_catalog.truncate(binary, uint4) RETURNS number LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.truncate(cast($1 as number), cast($2 as int4))';
CREATE OR REPLACE FUNCTION pg_catalog.truncate(json, uint4) RETURNS number LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.truncate(cast($1 as number), cast($2 as int4))';

CREATE FUNCTION pg_catalog.bit_concat(bit, bit) RETURNS blob LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'concat_blob';
CREATE FUNCTION pg_catalog.bit_bool_concat(bit, boolean) RETURNS blob LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'concat_blob';
CREATE FUNCTION pg_catalog.bool_bit_concat(boolean, bit) RETURNS blob LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'concat_blob';
CREATE FUNCTION pg_catalog.bool_concat(boolean, boolean) RETURNS text LANGUAGE SQL IMMUTABLE STRICT as 'select concat($1, $2)';
CREATE FUNCTION pg_catalog.text_bool_concat(text, boolean) RETURNS text LANGUAGE SQL IMMUTABLE STRICT as 'select concat($1, $2)';
CREATE FUNCTION pg_catalog.bool_text_concat(boolean, text) RETURNS text LANGUAGE SQL IMMUTABLE STRICT as 'select concat($1, $2)';
CREATE FUNCTION pg_catalog.text_bit_concat(text, bit) RETURNS blob LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'concat_blob';
CREATE FUNCTION pg_catalog.bit_text_concat(bit, text) RETURNS blob LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'concat_blob';

CREATE FUNCTION pg_catalog.bin_concat(binary,binary) RETURNS blob LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'concat_blob';
CREATE FUNCTION pg_catalog.bin_int_concat(binary,uint8) RETURNS blob LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'concat_blob';
CREATE FUNCTION pg_catalog.bin_int8_concat(binary,int8) RETURNS blob LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'concat_blob';
CREATE FUNCTION pg_catalog.bin_float4_concat(binary,float4) RETURNS blob LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'concat_blob';
CREATE FUNCTION pg_catalog.bin_float8_concat(binary,float8) RETURNS blob LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'concat_blob';
CREATE FUNCTION pg_catalog.bin_num_concat(binary,numeric) RETURNS blob LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'concat_blob';
CREATE FUNCTION pg_catalog.bin_bit_concat(binary,bit) RETURNS blob LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'concat_blob';
CREATE FUNCTION pg_catalog.bin_text_concat(binary,text) RETURNS blob LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'concat_blob';
CREATE FUNCTION pg_catalog.bin_bool_concat(binary,boolean) RETURNS blob LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'concat_blob';
CREATE FUNCTION pg_catalog.int_bin_concat(uint8,binary) RETURNS blob LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'concat_blob';
CREATE FUNCTION pg_catalog.int8_bin_concat(int8,binary) RETURNS blob LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'concat_blob';
CREATE FUNCTION pg_catalog.float4_bin_concat(float4,binary) RETURNS blob LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'concat_blob';
CREATE FUNCTION pg_catalog.float8_bin_concat(float8,binary) RETURNS blob LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'concat_blob';
CREATE FUNCTION pg_catalog.num_bin_concat(numeric,binary) RETURNS blob LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'concat_blob';
CREATE FUNCTION pg_catalog.bit_bin_concat(bit,binary) RETURNS blob LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'concat_blob';
CREATE FUNCTION pg_catalog.text_bin_concat(text,binary) RETURNS blob LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'concat_blob';
CREATE FUNCTION pg_catalog.bool_bin_concat(boolean,binary) RETURNS blob LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'concat_blob';

CREATE FUNCTION pg_catalog.varbin_concat(varbinary,varbinary) RETURNS blob LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'concat_blob';
CREATE FUNCTION pg_catalog.varbin_int_concat(varbinary,uint8) RETURNS blob LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'concat_blob';
CREATE FUNCTION pg_catalog.varbin_int8_concat(varbinary,int8) RETURNS blob LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'concat_blob';
CREATE FUNCTION pg_catalog.varbin_float4_concat(varbinary,float4) RETURNS blob LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'concat_blob';
CREATE FUNCTION pg_catalog.varbin_float8_concat(varbinary,float8) RETURNS blob LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'concat_blob';
CREATE FUNCTION pg_catalog.varbin_num_concat(varbinary,numeric) RETURNS blob LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'concat_blob';
CREATE FUNCTION pg_catalog.varbin_bit_concat(varbinary,bit) RETURNS blob LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'concat_blob';
CREATE FUNCTION pg_catalog.varbin_text_concat(varbinary,text) RETURNS blob LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'concat_blob';
CREATE FUNCTION pg_catalog.varbin_bool_concat(varbinary,boolean) RETURNS blob LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'concat_blob';
CREATE FUNCTION pg_catalog.varbin_bin_concat(varbinary,binary) RETURNS blob LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'concat_blob';
CREATE FUNCTION pg_catalog.int_varbin_concat(uint8,varbinary) RETURNS blob LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'concat_blob';
CREATE FUNCTION pg_catalog.int8_varbin_concat(int8,varbinary) RETURNS blob LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'concat_blob';
CREATE FUNCTION pg_catalog.float4_varbin_concat(float4,varbinary) RETURNS blob LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'concat_blob';
CREATE FUNCTION pg_catalog.float8_varbin_concat(float8,varbinary) RETURNS blob LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'concat_blob';
CREATE FUNCTION pg_catalog.num_varbin_concat(numeric,varbinary) RETURNS blob LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'concat_blob';
CREATE FUNCTION pg_catalog.bit_varbin_concat(bit,varbinary) RETURNS blob LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'concat_blob';
CREATE FUNCTION pg_catalog.text_varbin_concat(text,varbinary) RETURNS blob LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'concat_blob';
CREATE FUNCTION pg_catalog.bool_varbin_concat(boolean,varbinary) RETURNS blob LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'concat_blob';
CREATE FUNCTION pg_catalog.bin_varbin_concat(binary,varbinary) RETURNS blob LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'concat_blob';

CREATE FUNCTION pg_catalog.unknown_concat(unknown, unknown) RETURNS text LANGUAGE SQL IMMUTABLE STRICT as 'select concat($1, $2)';
CREATE FUNCTION pg_catalog.unknown_int_concat(unknown, integer) RETURNS text LANGUAGE SQL IMMUTABLE STRICT as 'select concat($1, $2)';
CREATE FUNCTION pg_catalog.int_unknown_concat(integer, unknown) RETURNS text LANGUAGE SQL IMMUTABLE STRICT as 'select concat($1, $2)';

CREATE OPERATOR pg_catalog.||(leftarg=bit, rightarg=bit, procedure=pg_catalog.bit_concat);
CREATE OPERATOR pg_catalog.||(leftarg=bit, rightarg=boolean, procedure=pg_catalog.bit_bool_concat);
CREATE OPERATOR pg_catalog.||(leftarg=boolean, rightarg=bit, procedure=pg_catalog.bool_bit_concat);
CREATE OPERATOR pg_catalog.||(leftarg=boolean, rightarg=boolean, procedure=pg_catalog.bool_concat);
CREATE OPERATOR pg_catalog.||(leftarg=text, rightarg=boolean, procedure=pg_catalog.text_bool_concat);
CREATE OPERATOR pg_catalog.||(leftarg=boolean, rightarg=text, procedure=pg_catalog.bool_text_concat);
CREATE OPERATOR pg_catalog.||(leftarg=bit, rightarg=text, procedure=pg_catalog.bit_text_concat);
CREATE OPERATOR pg_catalog.||(leftarg=text, rightarg=bit, procedure=pg_catalog.text_bit_concat);
CREATE OPERATOR pg_catalog.||(leftarg=bit, rightarg=binary, procedure=pg_catalog.bit_bin_concat);
CREATE OPERATOR pg_catalog.||(leftarg=binary, rightarg=bit, procedure=pg_catalog.bin_bit_concat);
CREATE OPERATOR pg_catalog.||(leftarg=uint8, rightarg=binary, procedure=pg_catalog.int_bin_concat);
CREATE OPERATOR pg_catalog.||(leftarg=binary, rightarg=uint8, procedure=pg_catalog.bin_int_concat);
CREATE OPERATOR pg_catalog.||(leftarg=int8, rightarg=binary, procedure=pg_catalog.int8_bin_concat);
CREATE OPERATOR pg_catalog.||(leftarg=binary, rightarg=int8, procedure=pg_catalog.bin_int8_concat);
CREATE OPERATOR pg_catalog.||(leftarg=numeric, rightarg=binary, procedure=pg_catalog.num_bin_concat);
CREATE OPERATOR pg_catalog.||(leftarg=binary, rightarg=numeric, procedure=pg_catalog.bin_num_concat);
CREATE OPERATOR pg_catalog.||(leftarg=float4, rightarg=binary, procedure=pg_catalog.float4_bin_concat);
CREATE OPERATOR pg_catalog.||(leftarg=binary, rightarg=float4, procedure=pg_catalog.bin_float4_concat);
CREATE OPERATOR pg_catalog.||(leftarg=float8, rightarg=binary, procedure=pg_catalog.float8_bin_concat);
CREATE OPERATOR pg_catalog.||(leftarg=binary, rightarg=float8, procedure=pg_catalog.bin_float8_concat);
CREATE OPERATOR pg_catalog.||(leftarg=text, rightarg=binary, procedure=pg_catalog.text_bin_concat);
CREATE OPERATOR pg_catalog.||(leftarg=binary, rightarg=text, procedure=pg_catalog.bin_text_concat);
CREATE OPERATOR pg_catalog.||(leftarg=boolean, rightarg=binary, procedure=pg_catalog.bool_bin_concat);
CREATE OPERATOR pg_catalog.||(leftarg=binary, rightarg=boolean, procedure=pg_catalog.bin_bool_concat);
CREATE OPERATOR pg_catalog.||(leftarg=binary, rightarg=binary, procedure=pg_catalog.bin_concat);
CREATE OPERATOR pg_catalog.||(leftarg=bit, rightarg=varbinary, procedure=pg_catalog.bit_varbin_concat);
CREATE OPERATOR pg_catalog.||(leftarg=varbinary, rightarg=bit, procedure=pg_catalog.varbin_bit_concat);
CREATE OPERATOR pg_catalog.||(leftarg=uint8, rightarg=varbinary, procedure=pg_catalog.int_varbin_concat);
CREATE OPERATOR pg_catalog.||(leftarg=varbinary, rightarg=uint8, procedure=pg_catalog.varbin_int_concat);
CREATE OPERATOR pg_catalog.||(leftarg=int8, rightarg=varbinary, procedure=pg_catalog.int8_varbin_concat);
CREATE OPERATOR pg_catalog.||(leftarg=varbinary, rightarg=int8, procedure=pg_catalog.varbin_int8_concat);
CREATE OPERATOR pg_catalog.||(leftarg=numeric, rightarg=varbinary, procedure=pg_catalog.num_varbin_concat);
CREATE OPERATOR pg_catalog.||(leftarg=varbinary, rightarg=numeric, procedure=pg_catalog.varbin_num_concat);
CREATE OPERATOR pg_catalog.||(leftarg=float4, rightarg=varbinary, procedure=pg_catalog.float4_varbin_concat);
CREATE OPERATOR pg_catalog.||(leftarg=varbinary, rightarg=float4, procedure=pg_catalog.varbin_float4_concat);
CREATE OPERATOR pg_catalog.||(leftarg=float8, rightarg=varbinary, procedure=pg_catalog.float8_varbin_concat);
CREATE OPERATOR pg_catalog.||(leftarg=varbinary, rightarg=float8, procedure=pg_catalog.varbin_float8_concat);
CREATE OPERATOR pg_catalog.||(leftarg=text, rightarg=varbinary, procedure=pg_catalog.text_varbin_concat);
CREATE OPERATOR pg_catalog.||(leftarg=varbinary, rightarg=text, procedure=pg_catalog.varbin_text_concat);
CREATE OPERATOR pg_catalog.||(leftarg=boolean, rightarg=varbinary, procedure=pg_catalog.bool_varbin_concat);
CREATE OPERATOR pg_catalog.||(leftarg=varbinary, rightarg=boolean, procedure=pg_catalog.varbin_bool_concat);
CREATE OPERATOR pg_catalog.||(leftarg=binary, rightarg=varbinary, procedure=pg_catalog.bin_varbin_concat);
CREATE OPERATOR pg_catalog.||(leftarg=varbinary, rightarg=binary, procedure=pg_catalog.varbin_bin_concat);
CREATE OPERATOR pg_catalog.||(leftarg=varbinary, rightarg=varbinary, procedure=pg_catalog.varbin_concat);
CREATE OPERATOR pg_catalog.||(leftarg=unknown, rightarg=unknown, procedure=pg_catalog.unknown_concat);
CREATE OPERATOR pg_catalog.||(leftarg=unknown, rightarg=integer, procedure=pg_catalog.unknown_int_concat);
CREATE OPERATOR pg_catalog.||(leftarg=integer, rightarg=unknown, procedure=pg_catalog.int_unknown_concat);

CREATE OR REPLACE FUNCTION pg_catalog.left(bit, integer) RETURNS varbinary LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'bit_left';
CREATE OR REPLACE FUNCTION pg_catalog.left(blob, integer) RETURNS varbinary LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'blob_left';
CREATE OR REPLACE FUNCTION pg_catalog.left(boolean, integer) RETURNS varchar LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.left(cast($1 as text), $2)::varchar';

CREATE OR REPLACE FUNCTION pg_catalog.int_uint2_eq(int, uint2) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1 = $2::int4)';
CREATE OPERATOR pg_catalog.=(LEFTARG = int, RIGHTARG = uint2, COMMUTATOR = operator(pg_catalog.=), PROCEDURE = pg_catalog.int_uint2_eq, RESTRICT = eqsel, JOIN = eqjoinsel);

CREATE OR REPLACE FUNCTION pg_catalog.json_eq(tinyblob, json) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin','json_eq';
CREATE OPERATOR pg_catalog.=(leftarg = tinyblob, rightarg = json, PROCEDURE = pg_catalog.json_eq, RESTRICT = eqsel, JOIN = eqjoinsel);
CREATE OR REPLACE FUNCTION pg_catalog.json_eq(blob, json) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin','json_eq';
CREATE OPERATOR pg_catalog.=(leftarg = blob, rightarg = json, PROCEDURE = pg_catalog.json_eq, RESTRICT = eqsel, JOIN = eqjoinsel);
CREATE OR REPLACE FUNCTION pg_catalog.json_eq(mediumblob, json) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin','json_eq';
CREATE OPERATOR pg_catalog.=(leftarg = mediumblob, rightarg = json, PROCEDURE = pg_catalog.json_eq, RESTRICT = eqsel, JOIN = eqjoinsel);
CREATE OR REPLACE FUNCTION pg_catalog.json_eq(longblob, json) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin','json_eq';
CREATE OPERATOR pg_catalog.=(leftarg = longblob, rightarg = json, PROCEDURE = pg_catalog.json_eq, RESTRICT = eqsel, JOIN = eqjoinsel);
CREATE OR REPLACE FUNCTION pg_catalog.json_eq(anyenum, json) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin','json_eq';
CREATE OPERATOR pg_catalog.=(leftarg = anyenum, rightarg = json, PROCEDURE = pg_catalog.json_eq, RESTRICT = eqsel, JOIN = eqjoinsel);

CREATE OR REPLACE FUNCTION pg_catalog.hex(anyenum) RETURNS varchar LANGUAGE SQL IMMUTABLE STRICT as 'SELECT hex($1::text)';

CREATE OR REPLACE FUNCTION pg_catalog.left(bit, integer) RETURNS varbinary LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'bit_left';
CREATE OR REPLACE FUNCTION pg_catalog.left(blob, integer) RETURNS varbinary LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'blob_left';
CREATE OR REPLACE FUNCTION pg_catalog.left(boolean, integer) RETURNS varchar LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.left(cast($1 as text), $2)::varchar';
--blob op numeric
CREATE OR REPLACE FUNCTION pg_catalog.tinyblob_numeric_eq(arg1 tinyblob, arg2 numeric) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT as 'select $1::float8=$2';
CREATE OR REPLACE FUNCTION pg_catalog.tinyblob_numeric_ne(arg1 tinyblob, arg2 numeric) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1::float8<>$2';
CREATE OR REPLACE FUNCTION pg_catalog.tinyblob_numeric_lt(arg1 tinyblob, arg2 numeric) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1::float8<$2';
CREATE OR REPLACE FUNCTION pg_catalog.tinyblob_numeric_le(arg1 tinyblob, arg2 numeric) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1::float8<=$2';
CREATE OR REPLACE FUNCTION pg_catalog.tinyblob_numeric_gt(arg1 tinyblob, arg2 numeric) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1::float8>$2';
CREATE OR REPLACE FUNCTION pg_catalog.tinyblob_numeric_ge(arg1 tinyblob, arg2 numeric) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1::float8>=$2';
CREATE OR REPLACE FUNCTION pg_catalog.tinyblob_numeric_cmp(tinyblob, numeric) RETURNS integer LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.btfloat8cmp($1::float8, $2)';
CREATE OPERATOR pg_catalog.=(leftarg = tinyblob, rightarg = numeric, COMMUTATOR = operator(pg_catalog.=), procedure = pg_catalog.tinyblob_numeric_eq, restrict = eqsel, join = eqjoinsel);
CREATE OPERATOR pg_catalog.<>(leftarg = tinyblob, rightarg = numeric, COMMUTATOR = operator(pg_catalog.<>), procedure = pg_catalog.tinyblob_numeric_ne, restrict = neqsel, join = neqjoinsel);
CREATE OPERATOR pg_catalog.<(leftarg = tinyblob, rightarg = numeric, COMMUTATOR = operator(pg_catalog.<), procedure = pg_catalog.tinyblob_numeric_lt, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.<=(leftarg = tinyblob, rightarg = numeric, COMMUTATOR = operator(pg_catalog.<=), procedure = pg_catalog.tinyblob_numeric_le, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.>(leftarg = tinyblob, rightarg = numeric, COMMUTATOR = operator(pg_catalog.>), procedure = pg_catalog.tinyblob_numeric_gt, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.>=(leftarg = tinyblob, rightarg = numeric, COMMUTATOR = operator(pg_catalog.>=), procedure = pg_catalog.tinyblob_numeric_ge, restrict = scalarltsel, join = scalarltjoinsel);

CREATE OR REPLACE FUNCTION pg_catalog.numeric_tinyblob_eq(arg1 numeric, arg2 tinyblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1=$2::float8';
CREATE OR REPLACE FUNCTION pg_catalog.numeric_tinyblob_ne(arg1 numeric, arg2 tinyblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1<>$2::float8';
CREATE OR REPLACE FUNCTION pg_catalog.numeric_tinyblob_lt(arg1 numeric, arg2 tinyblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1<$2::float8';
CREATE OR REPLACE FUNCTION pg_catalog.numeric_tinyblob_le(arg1 numeric, arg2 tinyblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1<=$2::float8';
CREATE OR REPLACE FUNCTION pg_catalog.numeric_tinyblob_gt(arg1 numeric, arg2 tinyblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1>$2::float8';
CREATE OR REPLACE FUNCTION pg_catalog.numeric_tinyblob_ge(arg1 numeric, arg2 tinyblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1>=$2::float8';
CREATE OR REPLACE FUNCTION pg_catalog.numeric_tinyblob_cmp(numeric, tinyblob) RETURNS integer LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.btfloat8cmp($1, $2::float8)';
CREATE OPERATOR pg_catalog.=(leftarg = numeric, rightarg = tinyblob, COMMUTATOR = operator(pg_catalog.=), procedure = pg_catalog.numeric_tinyblob_eq, restrict = eqsel, join = eqjoinsel);
CREATE OPERATOR pg_catalog.<>(leftarg = numeric, rightarg = tinyblob, COMMUTATOR = operator(pg_catalog.<>), procedure = pg_catalog.numeric_tinyblob_ne, restrict = neqsel, join = neqjoinsel);
CREATE OPERATOR pg_catalog.<(leftarg = numeric, rightarg = tinyblob, COMMUTATOR = operator(pg_catalog.<), procedure = pg_catalog.numeric_tinyblob_lt, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.<=(leftarg = numeric, rightarg = tinyblob, COMMUTATOR = operator(pg_catalog.<=), procedure = pg_catalog.numeric_tinyblob_le, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.>(leftarg = numeric, rightarg = tinyblob, COMMUTATOR = operator(pg_catalog.>), procedure = pg_catalog.numeric_tinyblob_gt, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.>=(leftarg = numeric, rightarg = tinyblob, COMMUTATOR = operator(pg_catalog.>=), procedure = pg_catalog.numeric_tinyblob_ge, restrict = scalarltsel, join = scalarltjoinsel);

CREATE OR REPLACE FUNCTION pg_catalog.blob_numeric_eq(arg1 blob, arg2 numeric) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT as 'select $1::float8=$2';
CREATE OR REPLACE FUNCTION pg_catalog.blob_numeric_ne(arg1 blob, arg2 numeric) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1::float8<>$2';
CREATE OR REPLACE FUNCTION pg_catalog.blob_numeric_lt(arg1 blob, arg2 numeric) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1::float8<$2';
CREATE OR REPLACE FUNCTION pg_catalog.blob_numeric_le(arg1 blob, arg2 numeric) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1::float8<=$2';
CREATE OR REPLACE FUNCTION pg_catalog.blob_numeric_gt(arg1 blob, arg2 numeric) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1::float8>$2';
CREATE OR REPLACE FUNCTION pg_catalog.blob_numeric_ge(arg1 blob, arg2 numeric) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1::float8>=$2';
CREATE OR REPLACE FUNCTION pg_catalog.blob_numeric_cmp(blob, numeric) RETURNS integer LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.btfloat8cmp($1::float8, $2)';
CREATE OPERATOR pg_catalog.=(leftarg = blob, rightarg = numeric, COMMUTATOR = operator(pg_catalog.=), procedure = pg_catalog.blob_numeric_eq, restrict = eqsel, join = eqjoinsel);
CREATE OPERATOR pg_catalog.<>(leftarg = blob, rightarg = numeric, COMMUTATOR = operator(pg_catalog.<>), procedure = pg_catalog.blob_numeric_ne, restrict = neqsel, join = neqjoinsel);
CREATE OPERATOR pg_catalog.<(leftarg = blob, rightarg = numeric, COMMUTATOR = operator(pg_catalog.<), procedure = pg_catalog.blob_numeric_lt, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.<=(leftarg = blob, rightarg = numeric, COMMUTATOR = operator(pg_catalog.<=), procedure = pg_catalog.blob_numeric_le, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.>(leftarg = blob, rightarg = numeric, COMMUTATOR = operator(pg_catalog.>), procedure = pg_catalog.blob_numeric_gt, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.>=(leftarg = blob, rightarg = numeric, COMMUTATOR = operator(pg_catalog.>=), procedure = pg_catalog.blob_numeric_ge, restrict = scalarltsel, join = scalarltjoinsel);

CREATE OR REPLACE FUNCTION pg_catalog.numeric_blob_eq(arg1 numeric, arg2 blob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1=$2::float8';
CREATE OR REPLACE FUNCTION pg_catalog.numeric_blob_ne(arg1 numeric, arg2 blob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1<>$2::float8';
CREATE OR REPLACE FUNCTION pg_catalog.numeric_blob_lt(arg1 numeric, arg2 blob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1<$2::float8';
CREATE OR REPLACE FUNCTION pg_catalog.numeric_blob_le(arg1 numeric, arg2 blob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1<=$2::float8';
CREATE OR REPLACE FUNCTION pg_catalog.numeric_blob_gt(arg1 numeric, arg2 blob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1>$2::float8';
CREATE OR REPLACE FUNCTION pg_catalog.numeric_blob_ge(arg1 numeric, arg2 blob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1>=$2::float8';
CREATE OR REPLACE FUNCTION pg_catalog.numeric_blob_cmp(numeric, blob) RETURNS integer LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.btfloat8cmp($1, $2::float8)';
CREATE OPERATOR pg_catalog.=(leftarg = numeric, rightarg = blob, COMMUTATOR = operator(pg_catalog.=), procedure = pg_catalog.numeric_blob_eq, restrict = eqsel, join = eqjoinsel);
CREATE OPERATOR pg_catalog.<>(leftarg = numeric, rightarg = blob, COMMUTATOR = operator(pg_catalog.<>), procedure = pg_catalog.numeric_blob_ne, restrict = neqsel, join = neqjoinsel);
CREATE OPERATOR pg_catalog.<(leftarg = numeric, rightarg = blob, COMMUTATOR = operator(pg_catalog.<), procedure = pg_catalog.numeric_blob_lt, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.<=(leftarg = numeric, rightarg = blob, COMMUTATOR = operator(pg_catalog.<=), procedure = pg_catalog.numeric_blob_le, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.>(leftarg = numeric, rightarg = blob, COMMUTATOR = operator(pg_catalog.>), procedure = pg_catalog.numeric_blob_gt, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.>=(leftarg = numeric, rightarg = blob, COMMUTATOR = operator(pg_catalog.>=), procedure = pg_catalog.numeric_blob_ge, restrict = scalarltsel, join = scalarltjoinsel);

CREATE OR REPLACE FUNCTION pg_catalog.mediumblob_numeric_eq(arg1 mediumblob, arg2 numeric) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT as 'select $1::float8=$2';
CREATE OR REPLACE FUNCTION pg_catalog.mediumblob_numeric_ne(arg1 mediumblob, arg2 numeric) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1::float8<>$2';
CREATE OR REPLACE FUNCTION pg_catalog.mediumblob_numeric_lt(arg1 mediumblob, arg2 numeric) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1::float8<$2';
CREATE OR REPLACE FUNCTION pg_catalog.mediumblob_numeric_le(arg1 mediumblob, arg2 numeric) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1::float8<=$2';
CREATE OR REPLACE FUNCTION pg_catalog.mediumblob_numeric_gt(arg1 mediumblob, arg2 numeric) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1::float8>$2';
CREATE OR REPLACE FUNCTION pg_catalog.mediumblob_numeric_ge(arg1 mediumblob, arg2 numeric) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1::float8>=$2';
CREATE OR REPLACE FUNCTION pg_catalog.mediumblob_numeric_cmp(mediumblob, numeric) RETURNS integer LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.btfloat8cmp($1::float8, $2)';
CREATE OPERATOR pg_catalog.=(leftarg = mediumblob, rightarg = numeric, COMMUTATOR = operator(pg_catalog.=), procedure = pg_catalog.mediumblob_numeric_eq, restrict = eqsel, join = eqjoinsel);
CREATE OPERATOR pg_catalog.<>(leftarg = mediumblob, rightarg = numeric, COMMUTATOR = operator(pg_catalog.<>), procedure = pg_catalog.mediumblob_numeric_ne, restrict = neqsel, join = neqjoinsel);
CREATE OPERATOR pg_catalog.<(leftarg = mediumblob, rightarg = numeric, COMMUTATOR = operator(pg_catalog.<), procedure = pg_catalog.mediumblob_numeric_lt, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.<=(leftarg = mediumblob, rightarg = numeric, COMMUTATOR = operator(pg_catalog.<=), procedure = pg_catalog.mediumblob_numeric_le, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.>(leftarg = mediumblob, rightarg = numeric, COMMUTATOR = operator(pg_catalog.>), procedure = pg_catalog.mediumblob_numeric_gt, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.>=(leftarg = mediumblob, rightarg = numeric, COMMUTATOR = operator(pg_catalog.>=), procedure = pg_catalog.mediumblob_numeric_ge, restrict = scalarltsel, join = scalarltjoinsel);

CREATE OR REPLACE FUNCTION pg_catalog.numeric_mediumblob_eq(arg1 numeric, arg2 mediumblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1=$2::float8';
CREATE OR REPLACE FUNCTION pg_catalog.numeric_mediumblob_ne(arg1 numeric, arg2 mediumblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1<>$2::float8';
CREATE OR REPLACE FUNCTION pg_catalog.numeric_mediumblob_lt(arg1 numeric, arg2 mediumblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1<$2::float8';
CREATE OR REPLACE FUNCTION pg_catalog.numeric_mediumblob_le(arg1 numeric, arg2 mediumblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1<=$2::float8';
CREATE OR REPLACE FUNCTION pg_catalog.numeric_mediumblob_gt(arg1 numeric, arg2 mediumblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1>$2::float8';
CREATE OR REPLACE FUNCTION pg_catalog.numeric_mediumblob_ge(arg1 numeric, arg2 mediumblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1>=$2::float8';
CREATE OR REPLACE FUNCTION pg_catalog.numeric_mediumblob_cmp(numeric, mediumblob) RETURNS integer LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.btfloat8cmp($1, $2::float8)';
CREATE OPERATOR pg_catalog.=(leftarg = numeric, rightarg = mediumblob, COMMUTATOR = operator(pg_catalog.=), procedure = pg_catalog.numeric_mediumblob_eq, restrict = eqsel, join = eqjoinsel);
CREATE OPERATOR pg_catalog.<>(leftarg = numeric, rightarg = mediumblob, COMMUTATOR = operator(pg_catalog.<>), procedure = pg_catalog.numeric_mediumblob_ne, restrict = neqsel, join = neqjoinsel);
CREATE OPERATOR pg_catalog.<(leftarg = numeric, rightarg = mediumblob, COMMUTATOR = operator(pg_catalog.<), procedure = pg_catalog.numeric_mediumblob_lt, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.<=(leftarg = numeric, rightarg = mediumblob, COMMUTATOR = operator(pg_catalog.<=), procedure = pg_catalog.numeric_mediumblob_le, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.>(leftarg = numeric, rightarg = mediumblob, COMMUTATOR = operator(pg_catalog.>), procedure = pg_catalog.numeric_mediumblob_gt, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.>=(leftarg = numeric, rightarg = mediumblob, COMMUTATOR = operator(pg_catalog.>=), procedure = pg_catalog.numeric_mediumblob_ge, restrict = scalarltsel, join = scalarltjoinsel);

CREATE OR REPLACE FUNCTION pg_catalog.longblob_numeric_eq(arg1 longblob, arg2 numeric) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT as 'select $1::float8=$2';
CREATE OR REPLACE FUNCTION pg_catalog.longblob_numeric_ne(arg1 longblob, arg2 numeric) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1::float8<>$2';
CREATE OR REPLACE FUNCTION pg_catalog.longblob_numeric_lt(arg1 longblob, arg2 numeric) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1::float8<$2';
CREATE OR REPLACE FUNCTION pg_catalog.longblob_numeric_le(arg1 longblob, arg2 numeric) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1::float8<=$2';
CREATE OR REPLACE FUNCTION pg_catalog.longblob_numeric_gt(arg1 longblob, arg2 numeric) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1::float8>$2';
CREATE OR REPLACE FUNCTION pg_catalog.longblob_numeric_ge(arg1 longblob, arg2 numeric) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1::float8>=$2';
CREATE OR REPLACE FUNCTION pg_catalog.longblob_numeric_cmp(longblob, numeric) RETURNS integer LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.btfloat8cmp($1::float8, $2)';
CREATE OPERATOR pg_catalog.=(leftarg = longblob, rightarg = numeric, COMMUTATOR = operator(pg_catalog.=), procedure = pg_catalog.longblob_numeric_eq, restrict = eqsel, join = eqjoinsel);
CREATE OPERATOR pg_catalog.<>(leftarg = longblob, rightarg = numeric, COMMUTATOR = operator(pg_catalog.<>), procedure = pg_catalog.longblob_numeric_ne, restrict = neqsel, join = neqjoinsel);
CREATE OPERATOR pg_catalog.<(leftarg = longblob, rightarg = numeric, COMMUTATOR = operator(pg_catalog.<), procedure = pg_catalog.longblob_numeric_lt, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.<=(leftarg = longblob, rightarg = numeric, COMMUTATOR = operator(pg_catalog.<=), procedure = pg_catalog.longblob_numeric_le, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.>(leftarg = longblob, rightarg = numeric, COMMUTATOR = operator(pg_catalog.>), procedure = pg_catalog.longblob_numeric_gt, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.>=(leftarg = longblob, rightarg = numeric, COMMUTATOR = operator(pg_catalog.>=), procedure = pg_catalog.longblob_numeric_ge, restrict = scalarltsel, join = scalarltjoinsel);

CREATE OR REPLACE FUNCTION pg_catalog.numeric_longblob_eq(arg1 numeric, arg2 longblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1=$2::float8';
CREATE OR REPLACE FUNCTION pg_catalog.numeric_longblob_ne(arg1 numeric, arg2 longblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1<>$2::float8';
CREATE OR REPLACE FUNCTION pg_catalog.numeric_longblob_lt(arg1 numeric, arg2 longblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1<$2::float8';
CREATE OR REPLACE FUNCTION pg_catalog.numeric_longblob_le(arg1 numeric, arg2 longblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1<=$2::float8';
CREATE OR REPLACE FUNCTION pg_catalog.numeric_longblob_gt(arg1 numeric, arg2 longblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1>$2::float8';
CREATE OR REPLACE FUNCTION pg_catalog.numeric_longblob_ge(arg1 numeric, arg2 longblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1>=$2::float8';
CREATE OR REPLACE FUNCTION pg_catalog.numeric_longblob_cmp(numeric, longblob) RETURNS integer LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.btfloat8cmp($1, $2::float8)';
CREATE OPERATOR pg_catalog.=(leftarg = numeric, rightarg = longblob, COMMUTATOR = operator(pg_catalog.=), procedure = pg_catalog.numeric_longblob_eq, restrict = eqsel, join = eqjoinsel);
CREATE OPERATOR pg_catalog.<>(leftarg = numeric, rightarg = longblob, COMMUTATOR = operator(pg_catalog.<>), procedure = pg_catalog.numeric_longblob_ne, restrict = neqsel, join = neqjoinsel);
CREATE OPERATOR pg_catalog.<(leftarg = numeric, rightarg = longblob, COMMUTATOR = operator(pg_catalog.<), procedure = pg_catalog.numeric_longblob_lt, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.<=(leftarg = numeric, rightarg = longblob, COMMUTATOR = operator(pg_catalog.<=), procedure = pg_catalog.numeric_longblob_le, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.>(leftarg = numeric, rightarg = longblob, COMMUTATOR = operator(pg_catalog.>), procedure = pg_catalog.numeric_longblob_gt, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.>=(leftarg = numeric, rightarg = longblob, COMMUTATOR = operator(pg_catalog.>=), procedure = pg_catalog.numeric_longblob_ge, restrict = scalarltsel, join = scalarltjoinsel);

CREATE OR REPLACE FUNCTION pg_catalog.tinyblob_int8_eq(arg1 tinyblob, arg2 int8) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT as 'select $1::float8=$2';
CREATE OR REPLACE FUNCTION pg_catalog.tinyblob_int8_ne(arg1 tinyblob, arg2 int8) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1::float8<>$2';
CREATE OR REPLACE FUNCTION pg_catalog.tinyblob_int8_lt(arg1 tinyblob, arg2 int8) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1::float8<$2';
CREATE OR REPLACE FUNCTION pg_catalog.tinyblob_int8_le(arg1 tinyblob, arg2 int8) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1::float8<=$2';
CREATE OR REPLACE FUNCTION pg_catalog.tinyblob_int8_gt(arg1 tinyblob, arg2 int8) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1::float8>$2';
CREATE OR REPLACE FUNCTION pg_catalog.tinyblob_int8_ge(arg1 tinyblob, arg2 int8) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1::float8>=$2';
CREATE OR REPLACE FUNCTION pg_catalog.tinyblob_int8_cmp(tinyblob, int8) RETURNS integer LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.btfloat8cmp($1::float8, $2)';
CREATE OPERATOR pg_catalog.=(leftarg = tinyblob, rightarg = int8, COMMUTATOR = operator(pg_catalog.=), procedure = pg_catalog.tinyblob_int8_eq, restrict = eqsel, join = eqjoinsel);
CREATE OPERATOR pg_catalog.<>(leftarg = tinyblob, rightarg = int8, COMMUTATOR = operator(pg_catalog.<>), procedure = pg_catalog.tinyblob_int8_ne, restrict = neqsel, join = neqjoinsel);
CREATE OPERATOR pg_catalog.<(leftarg = tinyblob, rightarg = int8, COMMUTATOR = operator(pg_catalog.<), procedure = pg_catalog.tinyblob_int8_lt, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.<=(leftarg = tinyblob, rightarg = int8, COMMUTATOR = operator(pg_catalog.<=), procedure = pg_catalog.tinyblob_int8_le, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.>(leftarg = tinyblob, rightarg = int8, COMMUTATOR = operator(pg_catalog.>), procedure = pg_catalog.tinyblob_int8_gt, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.>=(leftarg = tinyblob, rightarg = int8, COMMUTATOR = operator(pg_catalog.>=), procedure = pg_catalog.tinyblob_int8_ge, restrict = scalarltsel, join = scalarltjoinsel);

CREATE OR REPLACE FUNCTION pg_catalog.int8_tinyblob_eq(arg1 int8, arg2 tinyblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1=$2::float8';
CREATE OR REPLACE FUNCTION pg_catalog.int8_tinyblob_ne(arg1 int8, arg2 tinyblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1<>$2::float8';
CREATE OR REPLACE FUNCTION pg_catalog.int8_tinyblob_lt(arg1 int8, arg2 tinyblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1<$2::float8';
CREATE OR REPLACE FUNCTION pg_catalog.int8_tinyblob_le(arg1 int8, arg2 tinyblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1<=$2::float8';
CREATE OR REPLACE FUNCTION pg_catalog.int8_tinyblob_gt(arg1 int8, arg2 tinyblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1>$2::float8';
CREATE OR REPLACE FUNCTION pg_catalog.int8_tinyblob_ge(arg1 int8, arg2 tinyblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1>=$2::float8';
CREATE OR REPLACE FUNCTION pg_catalog.int8_tinyblob_cmp(int8, tinyblob) RETURNS integer LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.btfloat8cmp($1, $2::float8)';
CREATE OPERATOR pg_catalog.=(leftarg = int8, rightarg = tinyblob, COMMUTATOR = operator(pg_catalog.=), procedure = pg_catalog.int8_tinyblob_eq, restrict = eqsel, join = eqjoinsel);
CREATE OPERATOR pg_catalog.<>(leftarg = int8, rightarg = tinyblob, COMMUTATOR = operator(pg_catalog.<>), procedure = pg_catalog.int8_tinyblob_ne, restrict = neqsel, join = neqjoinsel);
CREATE OPERATOR pg_catalog.<(leftarg = int8, rightarg = tinyblob, COMMUTATOR = operator(pg_catalog.<), procedure = pg_catalog.int8_tinyblob_lt, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.<=(leftarg = int8, rightarg = tinyblob, COMMUTATOR = operator(pg_catalog.<=), procedure = pg_catalog.int8_tinyblob_le, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.>(leftarg = int8, rightarg = tinyblob, COMMUTATOR = operator(pg_catalog.>), procedure = pg_catalog.int8_tinyblob_gt, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.>=(leftarg = int8, rightarg = tinyblob, COMMUTATOR = operator(pg_catalog.>=), procedure = pg_catalog.int8_tinyblob_ge, restrict = scalarltsel, join = scalarltjoinsel);

CREATE OR REPLACE FUNCTION pg_catalog.blob_int8_eq(arg1 blob, arg2 int8) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT as 'select $1::float8=$2';
CREATE OR REPLACE FUNCTION pg_catalog.blob_int8_ne(arg1 blob, arg2 int8) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1::float8<>$2';
CREATE OR REPLACE FUNCTION pg_catalog.blob_int8_lt(arg1 blob, arg2 int8) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1::float8<$2';
CREATE OR REPLACE FUNCTION pg_catalog.blob_int8_le(arg1 blob, arg2 int8) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1::float8<=$2';
CREATE OR REPLACE FUNCTION pg_catalog.blob_int8_gt(arg1 blob, arg2 int8) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1::float8>$2';
CREATE OR REPLACE FUNCTION pg_catalog.blob_int8_ge(arg1 blob, arg2 int8) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1::float8>=$2';
CREATE OR REPLACE FUNCTION pg_catalog.blob_int8_cmp(blob, int8) RETURNS integer LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.btfloat8cmp($1::float8, $2)';
CREATE OPERATOR pg_catalog.=(leftarg = blob, rightarg = int8, COMMUTATOR = operator(pg_catalog.=), procedure = pg_catalog.blob_int8_eq, restrict = eqsel, join = eqjoinsel);
CREATE OPERATOR pg_catalog.<>(leftarg = blob, rightarg = int8, COMMUTATOR = operator(pg_catalog.<>), procedure = pg_catalog.blob_int8_ne, restrict = neqsel, join = neqjoinsel);
CREATE OPERATOR pg_catalog.<(leftarg = blob, rightarg = int8, COMMUTATOR = operator(pg_catalog.<), procedure = pg_catalog.blob_int8_lt, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.<=(leftarg = blob, rightarg = int8, COMMUTATOR = operator(pg_catalog.<=), procedure = pg_catalog.blob_int8_le, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.>(leftarg = blob, rightarg = int8, COMMUTATOR = operator(pg_catalog.>), procedure = pg_catalog.blob_int8_gt, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.>=(leftarg = blob, rightarg = int8, COMMUTATOR = operator(pg_catalog.>=),procedure = pg_catalog.blob_int8_ge, restrict = scalarltsel, join = scalarltjoinsel);

CREATE OR REPLACE FUNCTION pg_catalog.int8_blob_eq(arg1 int8, arg2 blob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1=$2::float8';
CREATE OR REPLACE FUNCTION pg_catalog.int8_blob_ne(arg1 int8, arg2 blob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1<>$2::float8';
CREATE OR REPLACE FUNCTION pg_catalog.int8_blob_lt(arg1 int8, arg2 blob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1<$2::float8';
CREATE OR REPLACE FUNCTION pg_catalog.int8_blob_le(arg1 int8, arg2 blob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1<=$2::float8';
CREATE OR REPLACE FUNCTION pg_catalog.int8_blob_gt(arg1 int8, arg2 blob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1>$2::float8';
CREATE OR REPLACE FUNCTION pg_catalog.int8_blob_ge(arg1 int8, arg2 blob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1>=$2::float8';
CREATE OR REPLACE FUNCTION pg_catalog.int8_blob_cmp(int8, blob) RETURNS integer LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.btfloat8cmp($1, $2::float8)';
CREATE OPERATOR pg_catalog.=(leftarg = int8, rightarg = blob, COMMUTATOR = operator(pg_catalog.=), procedure = pg_catalog.int8_blob_eq, restrict = eqsel, join = eqjoinsel);
CREATE OPERATOR pg_catalog.<>(leftarg = int8, rightarg = blob, COMMUTATOR = operator(pg_catalog.<>), procedure = pg_catalog.int8_blob_ne, restrict = neqsel, join = neqjoinsel);
CREATE OPERATOR pg_catalog.<(leftarg = int8, rightarg = blob, COMMUTATOR = operator(pg_catalog.<), procedure = pg_catalog.int8_blob_lt, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.<=(leftarg = int8, rightarg = blob, COMMUTATOR = operator(pg_catalog.<=), procedure = pg_catalog.int8_blob_le, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.>(leftarg = int8, rightarg = blob, COMMUTATOR = operator(pg_catalog.>), procedure = pg_catalog.int8_blob_gt, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.>=(leftarg = int8, rightarg = blob, COMMUTATOR = operator(pg_catalog.>=), procedure = pg_catalog.int8_blob_ge, restrict = scalarltsel, join = scalarltjoinsel);

CREATE OR REPLACE FUNCTION pg_catalog.mediumblob_int8_eq(arg1 mediumblob, arg2 int8) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT as 'select $1::float8=$2';
CREATE OR REPLACE FUNCTION pg_catalog.mediumblob_int8_ne(arg1 mediumblob, arg2 int8) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1::float8<>$2';
CREATE OR REPLACE FUNCTION pg_catalog.mediumblob_int8_lt(arg1 mediumblob, arg2 int8) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1::float8<$2';
CREATE OR REPLACE FUNCTION pg_catalog.mediumblob_int8_le(arg1 mediumblob, arg2 int8) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1::float8<=$2';
CREATE OR REPLACE FUNCTION pg_catalog.mediumblob_int8_gt(arg1 mediumblob, arg2 int8) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1::float8>$2';
CREATE OR REPLACE FUNCTION pg_catalog.mediumblob_int8_ge(arg1 mediumblob, arg2 int8) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1::float8>=$2';
CREATE OR REPLACE FUNCTION pg_catalog.mediumblob_int8_cmp(mediumblob, int8) RETURNS integer LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.btfloat8cmp($1::float8, $2)';
CREATE OPERATOR pg_catalog.=(leftarg = mediumblob, rightarg = int8, COMMUTATOR = operator(pg_catalog.=), procedure = pg_catalog.mediumblob_int8_eq, restrict = eqsel, join = eqjoinsel);
CREATE OPERATOR pg_catalog.<>(leftarg = mediumblob, rightarg = int8, COMMUTATOR = operator(pg_catalog.<>), procedure = pg_catalog.mediumblob_int8_ne, restrict = neqsel, join = neqjoinsel);
CREATE OPERATOR pg_catalog.<(leftarg = mediumblob, rightarg = int8, COMMUTATOR = operator(pg_catalog.<), procedure = pg_catalog.mediumblob_int8_lt, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.<=(leftarg = mediumblob, rightarg = int8, COMMUTATOR = operator(pg_catalog.<=), procedure = pg_catalog.mediumblob_int8_le, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.>(leftarg = mediumblob, rightarg = int8, COMMUTATOR = operator(pg_catalog.>), procedure = pg_catalog.mediumblob_int8_gt, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.>=(leftarg = mediumblob, rightarg = int8, COMMUTATOR = operator(pg_catalog.>=), procedure = pg_catalog.mediumblob_int8_ge, restrict = scalarltsel, join = scalarltjoinsel);

CREATE OR REPLACE FUNCTION pg_catalog.int8_mediumblob_eq(arg1 int8, arg2 mediumblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1=$2::float8';
CREATE OR REPLACE FUNCTION pg_catalog.int8_mediumblob_ne(arg1 int8, arg2 mediumblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1<>$2::float8';
CREATE OR REPLACE FUNCTION pg_catalog.int8_mediumblob_lt(arg1 int8, arg2 mediumblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1<$2::float8';
CREATE OR REPLACE FUNCTION pg_catalog.int8_mediumblob_le(arg1 int8, arg2 mediumblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1<=$2::float8';
CREATE OR REPLACE FUNCTION pg_catalog.int8_mediumblob_gt(arg1 int8, arg2 mediumblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1>$2::float8';
CREATE OR REPLACE FUNCTION pg_catalog.int8_mediumblob_ge(arg1 int8, arg2 mediumblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1>=$2::float8';
CREATE OR REPLACE FUNCTION pg_catalog.int8_mediumblob_cmp(int8, mediumblob) RETURNS integer LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.btfloat8cmp($1, $2::float8)';
CREATE OPERATOR pg_catalog.=(leftarg = int8, rightarg = mediumblob, COMMUTATOR = operator(pg_catalog.=), procedure = pg_catalog.int8_mediumblob_eq, restrict = eqsel, join = eqjoinsel);
CREATE OPERATOR pg_catalog.<>(leftarg = int8, rightarg = mediumblob, COMMUTATOR = operator(pg_catalog.<>), procedure = pg_catalog.int8_mediumblob_ne, restrict = neqsel, join = neqjoinsel);
CREATE OPERATOR pg_catalog.<(leftarg = int8, rightarg = mediumblob, COMMUTATOR = operator(pg_catalog.<), procedure = pg_catalog.int8_mediumblob_lt, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.<=(leftarg = int8, rightarg = mediumblob, COMMUTATOR = operator(pg_catalog.<=), procedure = pg_catalog.int8_mediumblob_le, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.>(leftarg = int8, rightarg = mediumblob, COMMUTATOR = operator(pg_catalog.>), procedure = pg_catalog.int8_mediumblob_gt, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.>=(leftarg = int8, rightarg = mediumblob, COMMUTATOR = operator(pg_catalog.>=), procedure = pg_catalog.int8_mediumblob_ge, restrict = scalarltsel, join = scalarltjoinsel);

CREATE OR REPLACE FUNCTION pg_catalog.longblob_int8_eq(arg1 longblob, arg2 int8) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT as 'select $1::float8=$2';
CREATE OR REPLACE FUNCTION pg_catalog.longblob_int8_ne(arg1 longblob, arg2 int8) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1::float8<>$2';
CREATE OR REPLACE FUNCTION pg_catalog.longblob_int8_lt(arg1 longblob, arg2 int8) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1::float8<$2';
CREATE OR REPLACE FUNCTION pg_catalog.longblob_int8_le(arg1 longblob, arg2 int8) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1::float8<=$2';
CREATE OR REPLACE FUNCTION pg_catalog.longblob_int8_gt(arg1 longblob, arg2 int8) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1::float8>$2';
CREATE OR REPLACE FUNCTION pg_catalog.longblob_int8_ge(arg1 longblob, arg2 int8) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1::float8>=$2';
CREATE OR REPLACE FUNCTION pg_catalog.longblob_int8_cmp(longblob, int8) RETURNS integer LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.btfloat8cmp($1::float8, $2)';
CREATE OPERATOR pg_catalog.=(leftarg = longblob, rightarg = int8, COMMUTATOR = operator(pg_catalog.=), procedure = pg_catalog.longblob_int8_eq, restrict = eqsel, join = eqjoinsel);
CREATE OPERATOR pg_catalog.<>(leftarg = longblob, rightarg = int8, COMMUTATOR = operator(pg_catalog.<>), procedure = pg_catalog.longblob_int8_ne, restrict = neqsel, join = neqjoinsel);
CREATE OPERATOR pg_catalog.<(leftarg = longblob, rightarg = int8, COMMUTATOR = operator(pg_catalog.<), procedure = pg_catalog.longblob_int8_lt, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.<=(leftarg = longblob, rightarg = int8, COMMUTATOR = operator(pg_catalog.<=), procedure = pg_catalog.longblob_int8_le, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.>(leftarg = longblob, rightarg = int8, COMMUTATOR = operator(pg_catalog.>), procedure = pg_catalog.longblob_int8_gt, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.>=(leftarg = longblob, rightarg = int8, COMMUTATOR = operator(pg_catalog.>=), procedure = pg_catalog.longblob_int8_ge, restrict = scalarltsel, join = scalarltjoinsel);

CREATE OR REPLACE FUNCTION pg_catalog.int8_longblob_eq(arg1 int8, arg2 longblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1=$2::float8';
CREATE OR REPLACE FUNCTION pg_catalog.int8_longblob_ne(arg1 int8, arg2 longblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1<>$2::float8';
CREATE OR REPLACE FUNCTION pg_catalog.int8_longblob_lt(arg1 int8, arg2 longblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1<$2::float8';
CREATE OR REPLACE FUNCTION pg_catalog.int8_longblob_le(arg1 int8, arg2 longblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1<=$2::float8';
CREATE OR REPLACE FUNCTION pg_catalog.int8_longblob_gt(arg1 int8, arg2 longblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1>$2::float8';
CREATE OR REPLACE FUNCTION pg_catalog.int8_longblob_ge(arg1 int8, arg2 longblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1>=$2::float8';
CREATE OR REPLACE FUNCTION pg_catalog.int8_longblob_cmp(int8, longblob) RETURNS integer LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.btfloat8cmp($1, $2::float8)';
CREATE OPERATOR pg_catalog.=(leftarg = int8, rightarg = longblob, COMMUTATOR = operator(pg_catalog.=), procedure = pg_catalog.int8_longblob_eq, restrict = eqsel, join = eqjoinsel);
CREATE OPERATOR pg_catalog.<>(leftarg = int8, rightarg = longblob, COMMUTATOR = operator(pg_catalog.<>), procedure = pg_catalog.int8_longblob_ne, restrict = neqsel, join = neqjoinsel);
CREATE OPERATOR pg_catalog.<(leftarg = int8, rightarg = longblob, COMMUTATOR = operator(pg_catalog.<), procedure = pg_catalog.int8_longblob_lt, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.<=(leftarg = int8, rightarg = longblob, COMMUTATOR = operator(pg_catalog.<=), procedure = pg_catalog.int8_longblob_le, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.>(leftarg = int8, rightarg = longblob, COMMUTATOR = operator(pg_catalog.>), procedure = pg_catalog.int8_longblob_gt, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.>=(leftarg = int8, rightarg = longblob, COMMUTATOR = operator(pg_catalog.>=), procedure = pg_catalog.int8_longblob_ge, restrict = scalarltsel, join = scalarltjoinsel);

CREATE OR REPLACE FUNCTION pg_catalog.tinyblob_uint8_eq(arg1 tinyblob, arg2 uint8) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT as 'select $1::float8=$2';
CREATE OR REPLACE FUNCTION pg_catalog.tinyblob_uint8_ne(arg1 tinyblob, arg2 uint8) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1::float8<>$2';
CREATE OR REPLACE FUNCTION pg_catalog.tinyblob_uint8_lt(arg1 tinyblob, arg2 uint8) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1::float8<$2';
CREATE OR REPLACE FUNCTION pg_catalog.tinyblob_uint8_le(arg1 tinyblob, arg2 uint8) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1::float8<=$2';
CREATE OR REPLACE FUNCTION pg_catalog.tinyblob_uint8_gt(arg1 tinyblob, arg2 uint8) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1::float8>$2';
CREATE OR REPLACE FUNCTION pg_catalog.tinyblob_uint8_ge(arg1 tinyblob, arg2 uint8) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1::float8>=$2';
CREATE OR REPLACE FUNCTION pg_catalog.tinyblob_uint8_cmp(tinyblob, uint8) RETURNS integer LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.btfloat8cmp($1::float8, $2)';
CREATE OPERATOR pg_catalog.=(leftarg = tinyblob, rightarg = uint8, COMMUTATOR = operator(pg_catalog.=), procedure = pg_catalog.tinyblob_uint8_eq, restrict = eqsel, join = eqjoinsel);
CREATE OPERATOR pg_catalog.<>(leftarg = tinyblob, rightarg = uint8, COMMUTATOR = operator(pg_catalog.<>), procedure = pg_catalog.tinyblob_uint8_ne, restrict = neqsel, join = neqjoinsel);
CREATE OPERATOR pg_catalog.<(leftarg = tinyblob, rightarg = uint8, COMMUTATOR = operator(pg_catalog.<), procedure = pg_catalog.tinyblob_uint8_lt, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.<=(leftarg = tinyblob, rightarg = uint8, COMMUTATOR = operator(pg_catalog.<=), procedure = pg_catalog.tinyblob_uint8_le, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.>(leftarg = tinyblob, rightarg = uint8, COMMUTATOR = operator(pg_catalog.>), procedure = pg_catalog.tinyblob_uint8_gt, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.>=(leftarg = tinyblob, rightarg = uint8, COMMUTATOR = operator(pg_catalog.>=), procedure = pg_catalog.tinyblob_uint8_ge, restrict = scalarltsel, join = scalarltjoinsel);

CREATE OR REPLACE FUNCTION pg_catalog.uint8_tinyblob_eq(arg1 uint8, arg2 tinyblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1=$2::float8';
CREATE OR REPLACE FUNCTION pg_catalog.uint8_tinyblob_ne(arg1 uint8, arg2 tinyblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1<>$2::float8';
CREATE OR REPLACE FUNCTION pg_catalog.uint8_tinyblob_lt(arg1 uint8, arg2 tinyblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1<$2::float8';
CREATE OR REPLACE FUNCTION pg_catalog.uint8_tinyblob_le(arg1 uint8, arg2 tinyblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1<=$2::float8';
CREATE OR REPLACE FUNCTION pg_catalog.uint8_tinyblob_gt(arg1 uint8, arg2 tinyblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1>$2::float8';
CREATE OR REPLACE FUNCTION pg_catalog.uint8_tinyblob_ge(arg1 uint8, arg2 tinyblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1>=$2::float8';
CREATE OR REPLACE FUNCTION pg_catalog.uint8_tinyblob_cmp(uint8, tinyblob) RETURNS integer LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.btfloat8cmp($1, $2::float8)';
CREATE OPERATOR pg_catalog.=(leftarg = uint8, rightarg = tinyblob, COMMUTATOR = operator(pg_catalog.=), procedure = pg_catalog.uint8_tinyblob_eq, restrict = eqsel, join = eqjoinsel);
CREATE OPERATOR pg_catalog.<>(leftarg = uint8, rightarg = tinyblob, COMMUTATOR = operator(pg_catalog.<>), procedure = pg_catalog.uint8_tinyblob_ne, restrict = neqsel, join = neqjoinsel);
CREATE OPERATOR pg_catalog.<(leftarg = uint8, rightarg = tinyblob, COMMUTATOR = operator(pg_catalog.<), procedure = pg_catalog.uint8_tinyblob_lt, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.<=(leftarg = uint8, rightarg = tinyblob, COMMUTATOR = operator(pg_catalog.<=), procedure = pg_catalog.uint8_tinyblob_le, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.>(leftarg = uint8, rightarg = tinyblob, COMMUTATOR = operator(pg_catalog.>), procedure = pg_catalog.uint8_tinyblob_gt, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.>=(leftarg = uint8, rightarg = tinyblob, COMMUTATOR = operator(pg_catalog.>=), procedure = pg_catalog.uint8_tinyblob_ge, restrict = scalarltsel, join = scalarltjoinsel);

CREATE OR REPLACE FUNCTION pg_catalog.blob_uint8_eq(arg1 blob, arg2 uint8) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT as 'select $1::float8=$2';
CREATE OR REPLACE FUNCTION pg_catalog.blob_uint8_ne(arg1 blob, arg2 uint8) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1::float8<>$2';
CREATE OR REPLACE FUNCTION pg_catalog.blob_uint8_lt(arg1 blob, arg2 uint8) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1::float8<$2';
CREATE OR REPLACE FUNCTION pg_catalog.blob_uint8_le(arg1 blob, arg2 uint8) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1::float8<=$2';
CREATE OR REPLACE FUNCTION pg_catalog.blob_uint8_gt(arg1 blob, arg2 uint8) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1::float8>$2';
CREATE OR REPLACE FUNCTION pg_catalog.blob_uint8_ge(arg1 blob, arg2 uint8) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1::float8>=$2';
CREATE OR REPLACE FUNCTION pg_catalog.blob_uint8_cmp(blob, uint8) RETURNS integer LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.btfloat8cmp($1::float8, $2)';
CREATE OPERATOR pg_catalog.=(leftarg = blob, rightarg = uint8, COMMUTATOR = operator(pg_catalog.=), procedure = pg_catalog.blob_uint8_eq, restrict = eqsel, join = eqjoinsel);
CREATE OPERATOR pg_catalog.<>(leftarg = blob, rightarg = uint8, COMMUTATOR = operator(pg_catalog.<>), procedure = pg_catalog.blob_uint8_ne, restrict = neqsel, join = neqjoinsel);
CREATE OPERATOR pg_catalog.<(leftarg = blob, rightarg = uint8, COMMUTATOR = operator(pg_catalog.<), procedure = pg_catalog.blob_uint8_lt, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.<=(leftarg = blob, rightarg = uint8, COMMUTATOR = operator(pg_catalog.<=), procedure = pg_catalog.blob_uint8_le, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.>(leftarg = blob, rightarg = uint8, COMMUTATOR = operator(pg_catalog.>), procedure = pg_catalog.blob_uint8_gt, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.>=(leftarg = blob, rightarg = uint8, COMMUTATOR = operator(pg_catalog.>=), procedure = pg_catalog.blob_uint8_ge, restrict = scalarltsel, join = scalarltjoinsel);

CREATE OR REPLACE FUNCTION pg_catalog.uint8_blob_eq(arg1 uint8, arg2 blob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1=$2::float8';
CREATE OR REPLACE FUNCTION pg_catalog.uint8_blob_ne(arg1 uint8, arg2 blob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1<>$2::float8';
CREATE OR REPLACE FUNCTION pg_catalog.uint8_blob_lt(arg1 uint8, arg2 blob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1<$2::float8';
CREATE OR REPLACE FUNCTION pg_catalog.uint8_blob_le(arg1 uint8, arg2 blob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1<=$2::float8';
CREATE OR REPLACE FUNCTION pg_catalog.uint8_blob_gt(arg1 uint8, arg2 blob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1>$2::float8';
CREATE OR REPLACE FUNCTION pg_catalog.uint8_blob_ge(arg1 uint8, arg2 blob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1>=$2::float8';
CREATE OR REPLACE FUNCTION pg_catalog.uint8_blob_cmp(uint8, blob) RETURNS integer LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.btfloat8cmp($1, $2::float8)';
CREATE OPERATOR pg_catalog.=(leftarg = uint8, rightarg = blob, COMMUTATOR = operator(pg_catalog.=), procedure = pg_catalog.uint8_blob_eq, restrict = eqsel, join = eqjoinsel);
CREATE OPERATOR pg_catalog.<>(leftarg = uint8, rightarg = blob, COMMUTATOR = operator(pg_catalog.<>), procedure = pg_catalog.uint8_blob_ne, restrict = neqsel, join = neqjoinsel);
CREATE OPERATOR pg_catalog.<(leftarg = uint8, rightarg = blob, COMMUTATOR = operator(pg_catalog.<), procedure = pg_catalog.uint8_blob_lt, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.<=(leftarg = uint8, rightarg = blob, COMMUTATOR = operator(pg_catalog.<=), procedure = pg_catalog.uint8_blob_le, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.>(leftarg = uint8, rightarg = blob, COMMUTATOR = operator(pg_catalog.>), procedure = pg_catalog.uint8_blob_gt, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.>=(leftarg = uint8, rightarg = blob, COMMUTATOR = operator(pg_catalog.>=), procedure = pg_catalog.uint8_blob_ge, restrict = scalarltsel, join = scalarltjoinsel);

CREATE OR REPLACE FUNCTION pg_catalog.mediumblob_uint8_eq(arg1 mediumblob, arg2 uint8) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT as 'select $1::float8=$2';
CREATE OR REPLACE FUNCTION pg_catalog.mediumblob_uint8_ne(arg1 mediumblob, arg2 uint8) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1::float8<>$2';
CREATE OR REPLACE FUNCTION pg_catalog.mediumblob_uint8_lt(arg1 mediumblob, arg2 uint8) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1::float8<$2';
CREATE OR REPLACE FUNCTION pg_catalog.mediumblob_uint8_le(arg1 mediumblob, arg2 uint8) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1::float8<=$2';
CREATE OR REPLACE FUNCTION pg_catalog.mediumblob_uint8_gt(arg1 mediumblob, arg2 uint8) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1::float8>$2';
CREATE OR REPLACE FUNCTION pg_catalog.mediumblob_uint8_ge(arg1 mediumblob, arg2 uint8) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1::float8>=$2';
CREATE OR REPLACE FUNCTION pg_catalog.mediumblob_uint8_cmp(mediumblob, uint8) RETURNS integer LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.btfloat8cmp($1::float8, $2)';
CREATE OPERATOR pg_catalog.=(leftarg = mediumblob, rightarg = uint8, COMMUTATOR = operator(pg_catalog.=), procedure = pg_catalog.mediumblob_uint8_eq, restrict = eqsel, join = eqjoinsel);
CREATE OPERATOR pg_catalog.<>(leftarg = mediumblob, rightarg = uint8, COMMUTATOR = operator(pg_catalog.<>), procedure = pg_catalog.mediumblob_uint8_ne, restrict = neqsel, join = neqjoinsel);
CREATE OPERATOR pg_catalog.<(leftarg = mediumblob, rightarg = uint8, COMMUTATOR = operator(pg_catalog.<), procedure = pg_catalog.mediumblob_uint8_lt, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.<=(leftarg = mediumblob, rightarg = uint8, COMMUTATOR = operator(pg_catalog.<=), procedure = pg_catalog.mediumblob_uint8_le, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.>(leftarg = mediumblob, rightarg = uint8, COMMUTATOR = operator(pg_catalog.>), procedure = pg_catalog.mediumblob_uint8_gt, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.>=(leftarg = mediumblob, rightarg = uint8, COMMUTATOR = operator(pg_catalog.>=), procedure = pg_catalog.mediumblob_uint8_ge, restrict = scalarltsel, join = scalarltjoinsel);

CREATE OR REPLACE FUNCTION pg_catalog.uint8_mediumblob_eq(arg1 uint8, arg2 mediumblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1=$2::float8';
CREATE OR REPLACE FUNCTION pg_catalog.uint8_mediumblob_ne(arg1 uint8, arg2 mediumblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1<>$2::float8';
CREATE OR REPLACE FUNCTION pg_catalog.uint8_mediumblob_lt(arg1 uint8, arg2 mediumblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1<$2::float8';
CREATE OR REPLACE FUNCTION pg_catalog.uint8_mediumblob_le(arg1 uint8, arg2 mediumblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1<=$2::float8';
CREATE OR REPLACE FUNCTION pg_catalog.uint8_mediumblob_gt(arg1 uint8, arg2 mediumblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1>$2::float8';
CREATE OR REPLACE FUNCTION pg_catalog.uint8_mediumblob_ge(arg1 uint8, arg2 mediumblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1>=$2::float8';
CREATE OR REPLACE FUNCTION pg_catalog.uint8_mediumblob_cmp(uint8, mediumblob) RETURNS integer LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.btfloat8cmp($1, $2::float8)';
CREATE OPERATOR pg_catalog.=(leftarg = uint8, rightarg = mediumblob, COMMUTATOR = operator(pg_catalog.=), procedure = pg_catalog.uint8_mediumblob_eq, restrict = eqsel, join = eqjoinsel);
CREATE OPERATOR pg_catalog.<>(leftarg = uint8, rightarg = mediumblob, COMMUTATOR = operator(pg_catalog.<>), procedure = pg_catalog.uint8_mediumblob_ne, restrict = neqsel, join = neqjoinsel);
CREATE OPERATOR pg_catalog.<(leftarg = uint8, rightarg = mediumblob, COMMUTATOR = operator(pg_catalog.<), procedure = pg_catalog.uint8_mediumblob_lt, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.<=(leftarg = uint8, rightarg = mediumblob, COMMUTATOR = operator(pg_catalog.<=), procedure = pg_catalog.uint8_mediumblob_le, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.>(leftarg = uint8, rightarg = mediumblob, COMMUTATOR = operator(pg_catalog.>), procedure = pg_catalog.uint8_mediumblob_gt, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.>=(leftarg = uint8, rightarg = mediumblob, COMMUTATOR = operator(pg_catalog.>=), procedure = pg_catalog.uint8_mediumblob_ge, restrict = scalarltsel, join = scalarltjoinsel);

CREATE OR REPLACE FUNCTION pg_catalog.longblob_uint8_eq(arg1 longblob, arg2 uint8) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT as 'select $1::float8=$2';
CREATE OR REPLACE FUNCTION pg_catalog.longblob_uint8_ne(arg1 longblob, arg2 uint8) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1::float8<>$2';
CREATE OR REPLACE FUNCTION pg_catalog.longblob_uint8_lt(arg1 longblob, arg2 uint8) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1::float8<$2';
CREATE OR REPLACE FUNCTION pg_catalog.longblob_uint8_le(arg1 longblob, arg2 uint8) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1::float8<=$2';
CREATE OR REPLACE FUNCTION pg_catalog.longblob_uint8_gt(arg1 longblob, arg2 uint8) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1::float8>$2';
CREATE OR REPLACE FUNCTION pg_catalog.longblob_uint8_ge(arg1 longblob, arg2 uint8) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1::float8>=$2';
CREATE OR REPLACE FUNCTION pg_catalog.longblob_uint8_cmp(longblob, uint8) RETURNS integer LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.btfloat8cmp($1::float8, $2)';
CREATE OPERATOR pg_catalog.=(leftarg = longblob, rightarg = uint8, COMMUTATOR = operator(pg_catalog.=), procedure = pg_catalog.longblob_uint8_eq, restrict = eqsel, join = eqjoinsel);
CREATE OPERATOR pg_catalog.<>(leftarg = longblob, rightarg = uint8, COMMUTATOR = operator(pg_catalog.<>), procedure = pg_catalog.longblob_uint8_ne, restrict = neqsel, join = neqjoinsel);
CREATE OPERATOR pg_catalog.<(leftarg = longblob, rightarg = uint8, COMMUTATOR = operator(pg_catalog.<), procedure = pg_catalog.longblob_uint8_lt, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.<=(leftarg = longblob, rightarg = uint8, COMMUTATOR = operator(pg_catalog.<=), procedure = pg_catalog.longblob_uint8_le, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.>(leftarg = longblob, rightarg = uint8, COMMUTATOR = operator(pg_catalog.>), procedure = pg_catalog.longblob_uint8_gt, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.>=(leftarg = longblob, rightarg = uint8, COMMUTATOR = operator(pg_catalog.>=), procedure = pg_catalog.longblob_uint8_ge, restrict = scalarltsel, join = scalarltjoinsel);

CREATE OR REPLACE FUNCTION pg_catalog.uint8_longblob_eq(arg1 uint8, arg2 longblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1=$2::float8';
CREATE OR REPLACE FUNCTION pg_catalog.uint8_longblob_ne(arg1 uint8, arg2 longblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1<>$2::float8';
CREATE OR REPLACE FUNCTION pg_catalog.uint8_longblob_lt(arg1 uint8, arg2 longblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1<$2::float8';
CREATE OR REPLACE FUNCTION pg_catalog.uint8_longblob_le(arg1 uint8, arg2 longblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1<=$2::float8';
CREATE OR REPLACE FUNCTION pg_catalog.uint8_longblob_gt(arg1 uint8, arg2 longblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1>$2::float8';
CREATE OR REPLACE FUNCTION pg_catalog.uint8_longblob_ge(arg1 uint8, arg2 longblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1>=$2::float8';
CREATE OR REPLACE FUNCTION pg_catalog.uint8_longblob_cmp(uint8, longblob) RETURNS integer LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.btfloat8cmp($1, $2::float8)';
CREATE OPERATOR pg_catalog.=(leftarg = uint8, rightarg = longblob, COMMUTATOR = operator(pg_catalog.=), procedure = pg_catalog.uint8_longblob_eq, restrict = eqsel, join = eqjoinsel);
CREATE OPERATOR pg_catalog.<>(leftarg = uint8, rightarg = longblob, COMMUTATOR = operator(pg_catalog.<>),  procedure = pg_catalog.uint8_longblob_ne, restrict = neqsel, join = neqjoinsel);
CREATE OPERATOR pg_catalog.<(leftarg = uint8, rightarg = longblob, COMMUTATOR = operator(pg_catalog.<),  procedure = pg_catalog.uint8_longblob_lt, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.<=(leftarg = uint8, rightarg = longblob, COMMUTATOR = operator(pg_catalog.<=),  procedure = pg_catalog.uint8_longblob_le, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.>(leftarg = uint8, rightarg = longblob, COMMUTATOR = operator(pg_catalog.>),  procedure = pg_catalog.uint8_longblob_gt, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.>=(leftarg = uint8, rightarg = longblob, COMMUTATOR = operator(pg_catalog.>=),  procedure = pg_catalog.uint8_longblob_ge, restrict = scalarltsel, join = scalarltjoinsel);

CREATE OR REPLACE FUNCTION pg_catalog.tinyblob_float8_eq(arg1 tinyblob, arg2 float8) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT as 'select $1::float8=$2';
CREATE OR REPLACE FUNCTION pg_catalog.tinyblob_float8_ne(arg1 tinyblob, arg2 float8) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1::float8<>$2';
CREATE OR REPLACE FUNCTION pg_catalog.tinyblob_float8_lt(arg1 tinyblob, arg2 float8) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1::float8<$2';
CREATE OR REPLACE FUNCTION pg_catalog.tinyblob_float8_le(arg1 tinyblob, arg2 float8) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1::float8<=$2';
CREATE OR REPLACE FUNCTION pg_catalog.tinyblob_float8_gt(arg1 tinyblob, arg2 float8) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1::float8>$2';
CREATE OR REPLACE FUNCTION pg_catalog.tinyblob_float8_ge(arg1 tinyblob, arg2 float8) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1::float8>=$2';
CREATE OR REPLACE FUNCTION pg_catalog.tinyblob_float8_cmp(tinyblob, float8) RETURNS integer LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.btfloat8cmp($1::float8, $2)';
CREATE OPERATOR pg_catalog.=(leftarg = tinyblob, rightarg = float8, COMMUTATOR = operator(pg_catalog.=), procedure = pg_catalog.tinyblob_float8_eq, restrict = eqsel, join = eqjoinsel);
CREATE OPERATOR pg_catalog.<>(leftarg = tinyblob, rightarg = float8, COMMUTATOR = operator(pg_catalog.<>), procedure = pg_catalog.tinyblob_float8_ne, restrict = neqsel, join = neqjoinsel);
CREATE OPERATOR pg_catalog.<(leftarg = tinyblob, rightarg = float8, COMMUTATOR = operator(pg_catalog.<), procedure = pg_catalog.tinyblob_float8_lt, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.<=(leftarg = tinyblob, rightarg = float8, COMMUTATOR = operator(pg_catalog.<=), procedure = pg_catalog.tinyblob_float8_le, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.>(leftarg = tinyblob, rightarg = float8, COMMUTATOR = operator(pg_catalog.>), procedure = pg_catalog.tinyblob_float8_gt, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.>=(leftarg = tinyblob, rightarg = float8, COMMUTATOR = operator(pg_catalog.>=), procedure = pg_catalog.tinyblob_float8_ge, restrict = scalarltsel, join = scalarltjoinsel);

CREATE OR REPLACE FUNCTION pg_catalog.float8_tinyblob_eq(arg1 float8, arg2 tinyblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1=$2::float8';
CREATE OR REPLACE FUNCTION pg_catalog.float8_tinyblob_ne(arg1 float8, arg2 tinyblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1<>$2::float8';
CREATE OR REPLACE FUNCTION pg_catalog.float8_tinyblob_lt(arg1 float8, arg2 tinyblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1<$2::float8';
CREATE OR REPLACE FUNCTION pg_catalog.float8_tinyblob_le(arg1 float8, arg2 tinyblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1<=$2::float8';
CREATE OR REPLACE FUNCTION pg_catalog.float8_tinyblob_gt(arg1 float8, arg2 tinyblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1>$2::float8';
CREATE OR REPLACE FUNCTION pg_catalog.float8_tinyblob_ge(arg1 float8, arg2 tinyblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1>=$2::float8';
CREATE OR REPLACE FUNCTION pg_catalog.float8_tinyblob_cmp(float8, tinyblob) RETURNS integer LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.btfloat8cmp($1, $2::float8)';
CREATE OPERATOR pg_catalog.=(leftarg = float8, rightarg = tinyblob, COMMUTATOR = operator(pg_catalog.=), procedure = pg_catalog.float8_tinyblob_eq, restrict = eqsel, join = eqjoinsel);
CREATE OPERATOR pg_catalog.<>(leftarg = float8, rightarg = tinyblob, COMMUTATOR = operator(pg_catalog.<>), procedure = pg_catalog.float8_tinyblob_ne, restrict = neqsel, join = neqjoinsel);
CREATE OPERATOR pg_catalog.<(leftarg = float8, rightarg = tinyblob, COMMUTATOR = operator(pg_catalog.<), procedure = pg_catalog.float8_tinyblob_lt, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.<=(leftarg = float8, rightarg = tinyblob, COMMUTATOR = operator(pg_catalog.<=), procedure = pg_catalog.float8_tinyblob_le, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.>(leftarg = float8, rightarg = tinyblob, COMMUTATOR = operator(pg_catalog.>), procedure = pg_catalog.float8_tinyblob_gt, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.>=(leftarg = float8, rightarg = tinyblob, COMMUTATOR = operator(pg_catalog.>=), procedure = pg_catalog.float8_tinyblob_ge, restrict = scalarltsel, join = scalarltjoinsel);

CREATE OR REPLACE FUNCTION pg_catalog.blob_float8_eq(arg1 blob, arg2 float8) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT as 'select $1::float8=$2';
CREATE OR REPLACE FUNCTION pg_catalog.blob_float8_ne(arg1 blob, arg2 float8) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1::float8<>$2';
CREATE OR REPLACE FUNCTION pg_catalog.blob_float8_lt(arg1 blob, arg2 float8) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1::float8<$2';
CREATE OR REPLACE FUNCTION pg_catalog.blob_float8_le(arg1 blob, arg2 float8) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1::float8<=$2';
CREATE OR REPLACE FUNCTION pg_catalog.blob_float8_gt(arg1 blob, arg2 float8) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1::float8>$2';
CREATE OR REPLACE FUNCTION pg_catalog.blob_float8_ge(arg1 blob, arg2 float8) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1::float8>=$2';
CREATE OR REPLACE FUNCTION pg_catalog.blob_float8_cmp(blob, float8) RETURNS integer LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.btfloat8cmp($1::float8, $2)';
CREATE OPERATOR pg_catalog.=(leftarg = blob, rightarg = float8, COMMUTATOR = operator(pg_catalog.=), procedure = pg_catalog.blob_float8_eq, restrict = eqsel, join = eqjoinsel);
CREATE OPERATOR pg_catalog.<>(leftarg = blob, rightarg = float8, COMMUTATOR = operator(pg_catalog.<>), procedure = pg_catalog.blob_float8_ne, restrict = neqsel, join = neqjoinsel);
CREATE OPERATOR pg_catalog.<(leftarg = blob, rightarg = float8, COMMUTATOR = operator(pg_catalog.<), procedure = pg_catalog.blob_float8_lt, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.<=(leftarg = blob, rightarg = float8, COMMUTATOR = operator(pg_catalog.<=), procedure = pg_catalog.blob_float8_le, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.>(leftarg = blob, rightarg = float8, COMMUTATOR = operator(pg_catalog.>), procedure = pg_catalog.blob_float8_gt, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.>=(leftarg = blob, rightarg = float8, COMMUTATOR = operator(pg_catalog.>=), procedure = pg_catalog.blob_float8_ge, restrict = scalarltsel, join = scalarltjoinsel);

CREATE OR REPLACE FUNCTION pg_catalog.float8_blob_eq(arg1 float8, arg2 blob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1=$2::float8';
CREATE OR REPLACE FUNCTION pg_catalog.float8_blob_ne(arg1 float8, arg2 blob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1<>$2::float8';
CREATE OR REPLACE FUNCTION pg_catalog.float8_blob_lt(arg1 float8, arg2 blob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1<$2::float8';
CREATE OR REPLACE FUNCTION pg_catalog.float8_blob_le(arg1 float8, arg2 blob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1<=$2::float8';
CREATE OR REPLACE FUNCTION pg_catalog.float8_blob_gt(arg1 float8, arg2 blob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1>$2::float8';
CREATE OR REPLACE FUNCTION pg_catalog.float8_blob_ge(arg1 float8, arg2 blob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1>=$2::float8';
CREATE OR REPLACE FUNCTION pg_catalog.float8_blob_cmp(float8, blob) RETURNS integer LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.btfloat8cmp($1, $2::float8)';
CREATE OPERATOR pg_catalog.=(leftarg = float8, rightarg = blob, COMMUTATOR = operator(pg_catalog.=), procedure = pg_catalog.float8_blob_eq, restrict = eqsel, join = eqjoinsel);
CREATE OPERATOR pg_catalog.<>(leftarg = float8, rightarg = blob, COMMUTATOR = operator(pg_catalog.<>), procedure = pg_catalog.float8_blob_ne, restrict = neqsel, join = neqjoinsel);
CREATE OPERATOR pg_catalog.<(leftarg = float8, rightarg = blob, COMMUTATOR = operator(pg_catalog.<), procedure = pg_catalog.float8_blob_lt, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.<=(leftarg = float8, rightarg = blob, COMMUTATOR = operator(pg_catalog.<=), procedure = pg_catalog.float8_blob_le, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.>(leftarg = float8, rightarg = blob, COMMUTATOR = operator(pg_catalog.>), procedure = pg_catalog.float8_blob_gt, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.>=(leftarg = float8, rightarg = blob, COMMUTATOR = operator(pg_catalog.>=), procedure = pg_catalog.float8_blob_ge, restrict = scalarltsel, join = scalarltjoinsel);

CREATE OR REPLACE FUNCTION pg_catalog.mediumblob_float8_eq(arg1 mediumblob, arg2 float8) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT as 'select $1::float8=$2';
CREATE OR REPLACE FUNCTION pg_catalog.mediumblob_float8_ne(arg1 mediumblob, arg2 float8) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1::float8<>$2';
CREATE OR REPLACE FUNCTION pg_catalog.mediumblob_float8_lt(arg1 mediumblob, arg2 float8) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1::float8<$2';
CREATE OR REPLACE FUNCTION pg_catalog.mediumblob_float8_le(arg1 mediumblob, arg2 float8) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1::float8<=$2';
CREATE OR REPLACE FUNCTION pg_catalog.mediumblob_float8_gt(arg1 mediumblob, arg2 float8) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1::float8>$2';
CREATE OR REPLACE FUNCTION pg_catalog.mediumblob_float8_ge(arg1 mediumblob, arg2 float8) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1::float8>=$2';
CREATE OR REPLACE FUNCTION pg_catalog.mediumblob_float8_cmp(mediumblob, float8) RETURNS integer LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.btfloat8cmp($1::float8, $2)';
CREATE OPERATOR pg_catalog.=(leftarg = mediumblob, rightarg = float8, COMMUTATOR = operator(pg_catalog.=), procedure = pg_catalog.mediumblob_float8_eq, restrict = eqsel, join = eqjoinsel);
CREATE OPERATOR pg_catalog.<>(leftarg = mediumblob, rightarg = float8, COMMUTATOR = operator(pg_catalog.<>), procedure = pg_catalog.mediumblob_float8_ne, restrict = neqsel, join = neqjoinsel);
CREATE OPERATOR pg_catalog.<(leftarg = mediumblob, rightarg = float8, COMMUTATOR = operator(pg_catalog.<), procedure = pg_catalog.mediumblob_float8_lt, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.<=(leftarg = mediumblob, rightarg = float8, COMMUTATOR = operator(pg_catalog.<=), procedure = pg_catalog.mediumblob_float8_le, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.>(leftarg = mediumblob, rightarg = float8, COMMUTATOR = operator(pg_catalog.>), procedure = pg_catalog.mediumblob_float8_gt, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.>=(leftarg = mediumblob, rightarg = float8, COMMUTATOR = operator(pg_catalog.>=), procedure = pg_catalog.mediumblob_float8_ge, restrict = scalarltsel, join = scalarltjoinsel);

CREATE OR REPLACE FUNCTION pg_catalog.float8_mediumblob_eq(arg1 float8, arg2 mediumblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1=$2::float8';
CREATE OR REPLACE FUNCTION pg_catalog.float8_mediumblob_ne(arg1 float8, arg2 mediumblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1<>$2::float8';
CREATE OR REPLACE FUNCTION pg_catalog.float8_mediumblob_lt(arg1 float8, arg2 mediumblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1<$2::float8';
CREATE OR REPLACE FUNCTION pg_catalog.float8_mediumblob_le(arg1 float8, arg2 mediumblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1<=$2::float8';
CREATE OR REPLACE FUNCTION pg_catalog.float8_mediumblob_gt(arg1 float8, arg2 mediumblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1>$2::float8';
CREATE OR REPLACE FUNCTION pg_catalog.float8_mediumblob_ge(arg1 float8, arg2 mediumblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1>=$2::float8';
CREATE OR REPLACE FUNCTION pg_catalog.float8_mediumblob_cmp(float8, mediumblob) RETURNS integer LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.btfloat8cmp($1, $2::float8)';
CREATE OPERATOR pg_catalog.=(leftarg = float8, rightarg = mediumblob, COMMUTATOR = operator(pg_catalog.=), procedure = pg_catalog.float8_mediumblob_eq, restrict = eqsel, join = eqjoinsel);
CREATE OPERATOR pg_catalog.<>(leftarg = float8, rightarg = mediumblob, COMMUTATOR = operator(pg_catalog.<>), procedure = pg_catalog.float8_mediumblob_ne, restrict = neqsel, join = neqjoinsel);
CREATE OPERATOR pg_catalog.<(leftarg = float8, rightarg = mediumblob, COMMUTATOR = operator(pg_catalog.<), procedure = pg_catalog.float8_mediumblob_lt, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.<=(leftarg = float8, rightarg = mediumblob, COMMUTATOR = operator(pg_catalog.<=), procedure = pg_catalog.float8_mediumblob_le, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.>(leftarg = float8, rightarg = mediumblob, COMMUTATOR = operator(pg_catalog.>), procedure = pg_catalog.float8_mediumblob_gt, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.>=(leftarg = float8, rightarg = mediumblob, COMMUTATOR = operator(pg_catalog.>=), procedure = pg_catalog.float8_mediumblob_ge, restrict = scalarltsel, join = scalarltjoinsel);

CREATE OR REPLACE FUNCTION pg_catalog.longblob_float8_eq(arg1 longblob, arg2 float8) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT as 'select $1::float8=$2';
CREATE OR REPLACE FUNCTION pg_catalog.longblob_float8_ne(arg1 longblob, arg2 float8) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1::float8<>$2';
CREATE OR REPLACE FUNCTION pg_catalog.longblob_float8_lt(arg1 longblob, arg2 float8) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1::float8<$2';
CREATE OR REPLACE FUNCTION pg_catalog.longblob_float8_le(arg1 longblob, arg2 float8) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1::float8<=$2';
CREATE OR REPLACE FUNCTION pg_catalog.longblob_float8_gt(arg1 longblob, arg2 float8) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1::float8>$2';
CREATE OR REPLACE FUNCTION pg_catalog.longblob_float8_ge(arg1 longblob, arg2 float8) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1::float8>=$2';
CREATE OR REPLACE FUNCTION pg_catalog.longblob_float8_cmp(longblob, float8) RETURNS integer LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.btfloat8cmp($1::float8, $2)';
CREATE OPERATOR pg_catalog.=(leftarg = longblob, rightarg = float8, COMMUTATOR = operator(pg_catalog.=), procedure = pg_catalog.longblob_float8_eq, restrict = eqsel, join = eqjoinsel);
CREATE OPERATOR pg_catalog.<>(leftarg = longblob, rightarg = float8, COMMUTATOR = operator(pg_catalog.<>), procedure = pg_catalog.longblob_float8_ne, restrict = neqsel, join = neqjoinsel);
CREATE OPERATOR pg_catalog.<(leftarg = longblob, rightarg = float8, COMMUTATOR = operator(pg_catalog.<), procedure = pg_catalog.longblob_float8_lt, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.<=(leftarg = longblob, rightarg = float8, COMMUTATOR = operator(pg_catalog.<=), procedure = pg_catalog.longblob_float8_le, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.>(leftarg = longblob, rightarg = float8, COMMUTATOR = operator(pg_catalog.>), procedure = pg_catalog.longblob_float8_gt, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.>=(leftarg = longblob, rightarg = float8, COMMUTATOR = operator(pg_catalog.>=), procedure = pg_catalog.longblob_float8_ge, restrict = scalarltsel, join = scalarltjoinsel);

CREATE OR REPLACE FUNCTION pg_catalog.float8_longblob_eq(arg1 float8, arg2 longblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1=$2::float8';
CREATE OR REPLACE FUNCTION pg_catalog.float8_longblob_ne(arg1 float8, arg2 longblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1<>$2::float8';
CREATE OR REPLACE FUNCTION pg_catalog.float8_longblob_lt(arg1 float8, arg2 longblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1<$2::float8';
CREATE OR REPLACE FUNCTION pg_catalog.float8_longblob_le(arg1 float8, arg2 longblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1<=$2::float8';
CREATE OR REPLACE FUNCTION pg_catalog.float8_longblob_gt(arg1 float8, arg2 longblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1>$2::float8';
CREATE OR REPLACE FUNCTION pg_catalog.float8_longblob_ge(arg1 float8, arg2 longblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1>=$2::float8';
CREATE OR REPLACE FUNCTION pg_catalog.float8_longblob_cmp(float8, longblob) RETURNS integer LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.btfloat8cmp($1, $2::float8)';
CREATE OPERATOR pg_catalog.=(leftarg = float8, rightarg = longblob, COMMUTATOR = operator(pg_catalog.=), procedure = pg_catalog.float8_longblob_eq, restrict = eqsel, join = eqjoinsel);
CREATE OPERATOR pg_catalog.<>(leftarg = float8, rightarg = longblob, COMMUTATOR = operator(pg_catalog.<>), procedure = pg_catalog.float8_longblob_ne, restrict = neqsel, join = neqjoinsel);
CREATE OPERATOR pg_catalog.<(leftarg = float8, rightarg = longblob, COMMUTATOR = operator(pg_catalog.<), procedure = pg_catalog.float8_longblob_lt, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.<=(leftarg = float8, rightarg = longblob, COMMUTATOR = operator(pg_catalog.<=), procedure = pg_catalog.float8_longblob_le, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.>(leftarg = float8, rightarg = longblob, COMMUTATOR = operator(pg_catalog.>), procedure = pg_catalog.float8_longblob_gt, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.>=(leftarg = float8, rightarg = longblob, COMMUTATOR = operator(pg_catalog.>=), procedure = pg_catalog.float8_longblob_ge, restrict = scalarltsel, join = scalarltjoinsel);

CREATE OR REPLACE FUNCTION pg_catalog.tinyblob_datetime_eq(arg1 tinyblob, arg2 timestamp without time zone) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT as 'select $1::text=$2';
CREATE OR REPLACE FUNCTION pg_catalog.tinyblob_datetime_ne(arg1 tinyblob, arg2 timestamp without time zone) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1::text<>$2';
CREATE OR REPLACE FUNCTION pg_catalog.tinyblob_datetime_lt(arg1 tinyblob, arg2 timestamp without time zone) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1::text<$2';
CREATE OR REPLACE FUNCTION pg_catalog.tinyblob_datetime_le(arg1 tinyblob, arg2 timestamp without time zone) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1::text<=$2';
CREATE OR REPLACE FUNCTION pg_catalog.tinyblob_datetime_gt(arg1 tinyblob, arg2 timestamp without time zone) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1::text>$2';
CREATE OR REPLACE FUNCTION pg_catalog.tinyblob_datetime_ge(arg1 tinyblob, arg2 timestamp without time zone) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1::text>=$2';
CREATE OR REPLACE FUNCTION pg_catalog.tinyblob_datetime_cmp(tinyblob, timestamp without time zone) RETURNS integer LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.btfloat8cmp($1::text, $2)';
CREATE OPERATOR pg_catalog.=(leftarg = tinyblob, rightarg = timestamp without time zone, COMMUTATOR = operator(pg_catalog.=), procedure = pg_catalog.tinyblob_datetime_eq, restrict = eqsel, join = eqjoinsel);
CREATE OPERATOR pg_catalog.<>(leftarg = tinyblob, rightarg = timestamp without time zone, COMMUTATOR = operator(pg_catalog.<>), procedure = pg_catalog.tinyblob_datetime_ne, restrict = neqsel, join = neqjoinsel);
CREATE OPERATOR pg_catalog.<(leftarg = tinyblob, rightarg = timestamp without time zone, COMMUTATOR = operator(pg_catalog.<), procedure = pg_catalog.tinyblob_datetime_lt, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.<=(leftarg = tinyblob, rightarg = timestamp without time zone, COMMUTATOR = operator(pg_catalog.<=), procedure = pg_catalog.tinyblob_datetime_le, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.>(leftarg = tinyblob, rightarg = timestamp without time zone, COMMUTATOR = operator(pg_catalog.>), procedure = pg_catalog.tinyblob_datetime_gt, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.>=(leftarg = tinyblob, rightarg = timestamp without time zone, COMMUTATOR = operator(pg_catalog.>=), procedure = pg_catalog.tinyblob_datetime_ge, restrict = scalarltsel, join = scalarltjoinsel);

CREATE OR REPLACE FUNCTION pg_catalog.datetime_tinyblob_eq(arg1 timestamp without time zone, arg2 tinyblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1=$2::text';
CREATE OR REPLACE FUNCTION pg_catalog.datetime_tinyblob_ne(arg1 timestamp without time zone, arg2 tinyblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1<>$2::text';
CREATE OR REPLACE FUNCTION pg_catalog.datetime_tinyblob_lt(arg1 timestamp without time zone, arg2 tinyblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1<$2::text';
CREATE OR REPLACE FUNCTION pg_catalog.datetime_tinyblob_le(arg1 timestamp without time zone, arg2 tinyblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1<=$2::text';
CREATE OR REPLACE FUNCTION pg_catalog.datetime_tinyblob_gt(arg1 timestamp without time zone, arg2 tinyblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1>$2::text';
CREATE OR REPLACE FUNCTION pg_catalog.datetime_tinyblob_ge(arg1 timestamp without time zone, arg2 tinyblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1>=$2::text';
CREATE OR REPLACE FUNCTION pg_catalog.datetime_tinyblob_cmp(timestamp without time zone, tinyblob) RETURNS integer LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.btfloat8cmp($1, $2::text)';
CREATE OPERATOR pg_catalog.=(leftarg = timestamp without time zone, rightarg = tinyblob, COMMUTATOR = operator(pg_catalog.=), procedure = pg_catalog.datetime_tinyblob_eq, restrict = eqsel, join = eqjoinsel);
CREATE OPERATOR pg_catalog.<>(leftarg = timestamp without time zone, rightarg = tinyblob, COMMUTATOR = operator(pg_catalog.<>), procedure = pg_catalog.datetime_tinyblob_ne, restrict = neqsel, join = neqjoinsel);
CREATE OPERATOR pg_catalog.<(leftarg = timestamp without time zone, rightarg = tinyblob, COMMUTATOR = operator(pg_catalog.<), procedure = pg_catalog.datetime_tinyblob_lt, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.<=(leftarg = timestamp without time zone, rightarg = tinyblob, COMMUTATOR = operator(pg_catalog.<=), procedure = pg_catalog.datetime_tinyblob_le, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.>(leftarg = timestamp without time zone, rightarg = tinyblob, COMMUTATOR = operator(pg_catalog.>), procedure = pg_catalog.datetime_tinyblob_gt, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.>=(leftarg = timestamp without time zone, rightarg = tinyblob, COMMUTATOR = operator(pg_catalog.>=), procedure = pg_catalog.datetime_tinyblob_ge, restrict = scalarltsel, join = scalarltjoinsel);

CREATE OR REPLACE FUNCTION pg_catalog.blob_datetime_eq(arg1 blob, arg2 timestamp without time zone) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT as 'select $1::text=$2';
CREATE OR REPLACE FUNCTION pg_catalog.blob_datetime_ne(arg1 blob, arg2 timestamp without time zone) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1::text<>$2';
CREATE OR REPLACE FUNCTION pg_catalog.blob_datetime_lt(arg1 blob, arg2 timestamp without time zone) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1::text<$2';
CREATE OR REPLACE FUNCTION pg_catalog.blob_datetime_le(arg1 blob, arg2 timestamp without time zone) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1::text<=$2';
CREATE OR REPLACE FUNCTION pg_catalog.blob_datetime_gt(arg1 blob, arg2 timestamp without time zone) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1::text>$2';
CREATE OR REPLACE FUNCTION pg_catalog.blob_datetime_ge(arg1 blob, arg2 timestamp without time zone) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1::text>=$2';
CREATE OR REPLACE FUNCTION pg_catalog.blob_datetime_cmp(blob, timestamp without time zone) RETURNS integer LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.btfloat8cmp($1::text, $2)';
CREATE OPERATOR pg_catalog.=(leftarg = blob, rightarg = timestamp without time zone, COMMUTATOR = operator(pg_catalog.=), procedure = pg_catalog.blob_datetime_eq, restrict = eqsel, join = eqjoinsel);
CREATE OPERATOR pg_catalog.<>(leftarg = blob, rightarg = timestamp without time zone, COMMUTATOR = operator(pg_catalog.<>), procedure = pg_catalog.blob_datetime_ne, restrict = neqsel, join = neqjoinsel);
CREATE OPERATOR pg_catalog.<(leftarg = blob, rightarg = timestamp without time zone, COMMUTATOR = operator(pg_catalog.<), procedure = pg_catalog.blob_datetime_lt, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.<=(leftarg = blob, rightarg = timestamp without time zone, COMMUTATOR = operator(pg_catalog.<=), procedure = pg_catalog.blob_datetime_le, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.>(leftarg = blob, rightarg = timestamp without time zone, COMMUTATOR = operator(pg_catalog.>), procedure = pg_catalog.blob_datetime_gt, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.>=(leftarg = blob, rightarg = timestamp without time zone, COMMUTATOR = operator(pg_catalog.>=), procedure = pg_catalog.blob_datetime_ge, restrict = scalarltsel, join = scalarltjoinsel);

CREATE OR REPLACE FUNCTION pg_catalog.datetime_blob_eq(arg1 timestamp without time zone, arg2 blob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1=$2::text';
CREATE OR REPLACE FUNCTION pg_catalog.datetime_blob_ne(arg1 timestamp without time zone, arg2 blob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1<>$2::text';
CREATE OR REPLACE FUNCTION pg_catalog.datetime_blob_lt(arg1 timestamp without time zone, arg2 blob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1<$2::text';
CREATE OR REPLACE FUNCTION pg_catalog.datetime_blob_le(arg1 timestamp without time zone, arg2 blob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1<=$2::text';
CREATE OR REPLACE FUNCTION pg_catalog.datetime_blob_gt(arg1 timestamp without time zone, arg2 blob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1>$2::text';
CREATE OR REPLACE FUNCTION pg_catalog.datetime_blob_ge(arg1 timestamp without time zone, arg2 blob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1>=$2::text';
CREATE OR REPLACE FUNCTION pg_catalog.datetime_blob_cmp(timestamp without time zone, blob) RETURNS integer LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.btfloat8cmp($1, $2::text)';
CREATE OPERATOR pg_catalog.=(leftarg = timestamp without time zone, rightarg = blob, COMMUTATOR = operator(pg_catalog.=), procedure = pg_catalog.datetime_blob_eq, restrict = eqsel, join = eqjoinsel);
CREATE OPERATOR pg_catalog.<>(leftarg = timestamp without time zone, rightarg = blob, COMMUTATOR = operator(pg_catalog.<>), procedure = pg_catalog.datetime_blob_ne, restrict = neqsel, join = neqjoinsel);
CREATE OPERATOR pg_catalog.<(leftarg = timestamp without time zone, rightarg = blob, COMMUTATOR = operator(pg_catalog.<), procedure = pg_catalog.datetime_blob_lt, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.<=(leftarg = timestamp without time zone, rightarg = blob, COMMUTATOR = operator(pg_catalog.<=), procedure = pg_catalog.datetime_blob_le, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.>(leftarg = timestamp without time zone, rightarg = blob, COMMUTATOR = operator(pg_catalog.>), procedure = pg_catalog.datetime_blob_gt, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.>=(leftarg = timestamp without time zone, rightarg = blob, COMMUTATOR = operator(pg_catalog.>=), procedure = pg_catalog.datetime_blob_ge, restrict = scalarltsel, join = scalarltjoinsel);

CREATE OR REPLACE FUNCTION pg_catalog.mediumblob_datetime_eq(arg1 mediumblob, arg2 timestamp without time zone) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT as 'select $1::text=$2';
CREATE OR REPLACE FUNCTION pg_catalog.mediumblob_datetime_ne(arg1 mediumblob, arg2 timestamp without time zone) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1::text<>$2';
CREATE OR REPLACE FUNCTION pg_catalog.mediumblob_datetime_lt(arg1 mediumblob, arg2 timestamp without time zone) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1::text<$2';
CREATE OR REPLACE FUNCTION pg_catalog.mediumblob_datetime_le(arg1 mediumblob, arg2 timestamp without time zone) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1::text<=$2';
CREATE OR REPLACE FUNCTION pg_catalog.mediumblob_datetime_gt(arg1 mediumblob, arg2 timestamp without time zone) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1::text>$2';
CREATE OR REPLACE FUNCTION pg_catalog.mediumblob_datetime_ge(arg1 mediumblob, arg2 timestamp without time zone) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1::text>=$2';
CREATE OR REPLACE FUNCTION pg_catalog.mediumblob_datetime_cmp(mediumblob, timestamp without time zone) RETURNS integer LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.btfloat8cmp($1::text, $2)';
CREATE OPERATOR pg_catalog.=(leftarg = mediumblob, rightarg = timestamp without time zone, COMMUTATOR = operator(pg_catalog.=), procedure = pg_catalog.mediumblob_datetime_eq, restrict = eqsel, join = eqjoinsel);
CREATE OPERATOR pg_catalog.<>(leftarg = mediumblob, rightarg = timestamp without time zone, COMMUTATOR = operator(pg_catalog.<>), procedure = pg_catalog.mediumblob_datetime_ne, restrict = neqsel, join = neqjoinsel);
CREATE OPERATOR pg_catalog.<(leftarg = mediumblob, rightarg = timestamp without time zone, COMMUTATOR = operator(pg_catalog.<), procedure = pg_catalog.mediumblob_datetime_lt, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.<=(leftarg = mediumblob, rightarg = timestamp without time zone, COMMUTATOR = operator(pg_catalog.<=), procedure = pg_catalog.mediumblob_datetime_le, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.>(leftarg = mediumblob, rightarg = timestamp without time zone, COMMUTATOR = operator(pg_catalog.>), procedure = pg_catalog.mediumblob_datetime_gt, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.>=(leftarg = mediumblob, rightarg = timestamp without time zone, COMMUTATOR = operator(pg_catalog.>=), procedure = pg_catalog.mediumblob_datetime_ge, restrict = scalarltsel, join = scalarltjoinsel);

CREATE OR REPLACE FUNCTION pg_catalog.datetime_mediumblob_eq(arg1 timestamp without time zone, arg2 mediumblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1=$2::text';
CREATE OR REPLACE FUNCTION pg_catalog.datetime_mediumblob_ne(arg1 timestamp without time zone, arg2 mediumblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1<>$2::text';
CREATE OR REPLACE FUNCTION pg_catalog.datetime_mediumblob_lt(arg1 timestamp without time zone, arg2 mediumblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1<$2::text';
CREATE OR REPLACE FUNCTION pg_catalog.datetime_mediumblob_le(arg1 timestamp without time zone, arg2 mediumblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1<=$2::text';
CREATE OR REPLACE FUNCTION pg_catalog.datetime_mediumblob_gt(arg1 timestamp without time zone, arg2 mediumblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1>$2::text';
CREATE OR REPLACE FUNCTION pg_catalog.datetime_mediumblob_ge(arg1 timestamp without time zone, arg2 mediumblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1>=$2::text';
CREATE OR REPLACE FUNCTION pg_catalog.datetime_mediumblob_cmp(timestamp without time zone, mediumblob) RETURNS integer LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.btfloat8cmp($1, $2::text)';
CREATE OPERATOR pg_catalog.=(leftarg = timestamp without time zone, rightarg = mediumblob, COMMUTATOR = operator(pg_catalog.=), procedure = pg_catalog.datetime_mediumblob_eq, restrict = eqsel, join = eqjoinsel);
CREATE OPERATOR pg_catalog.<>(leftarg = timestamp without time zone, rightarg = mediumblob, COMMUTATOR = operator(pg_catalog.<>), procedure = pg_catalog.datetime_mediumblob_ne, restrict = neqsel, join = neqjoinsel);
CREATE OPERATOR pg_catalog.<(leftarg = timestamp without time zone, rightarg = mediumblob, COMMUTATOR = operator(pg_catalog.<), procedure = pg_catalog.datetime_mediumblob_lt, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.<=(leftarg = timestamp without time zone, rightarg = mediumblob, COMMUTATOR = operator(pg_catalog.<=), procedure = pg_catalog.datetime_mediumblob_le, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.>(leftarg = timestamp without time zone, rightarg = mediumblob, COMMUTATOR = operator(pg_catalog.>), procedure = pg_catalog.datetime_mediumblob_gt, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.>=(leftarg = timestamp without time zone, rightarg = mediumblob, COMMUTATOR = operator(pg_catalog.>=), procedure = pg_catalog.datetime_mediumblob_ge, restrict = scalarltsel, join = scalarltjoinsel);

CREATE OR REPLACE FUNCTION pg_catalog.longblob_datetime_eq(arg1 longblob, arg2 timestamp without time zone) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT as 'select $1::text=$2';
CREATE OR REPLACE FUNCTION pg_catalog.longblob_datetime_ne(arg1 longblob, arg2 timestamp without time zone) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1::text<>$2';
CREATE OR REPLACE FUNCTION pg_catalog.longblob_datetime_lt(arg1 longblob, arg2 timestamp without time zone) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1::text<$2';
CREATE OR REPLACE FUNCTION pg_catalog.longblob_datetime_le(arg1 longblob, arg2 timestamp without time zone) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1::text<=$2';
CREATE OR REPLACE FUNCTION pg_catalog.longblob_datetime_gt(arg1 longblob, arg2 timestamp without time zone) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1::text>$2';
CREATE OR REPLACE FUNCTION pg_catalog.longblob_datetime_ge(arg1 longblob, arg2 timestamp without time zone) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1::text>=$2';
CREATE OR REPLACE FUNCTION pg_catalog.longblob_datetime_cmp(longblob, timestamp without time zone) RETURNS integer LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.btfloat8cmp($1::text, $2)';
CREATE OPERATOR pg_catalog.=(leftarg = longblob, rightarg = timestamp without time zone, COMMUTATOR = operator(pg_catalog.=), procedure = pg_catalog.longblob_datetime_eq, restrict = eqsel, join = eqjoinsel);
CREATE OPERATOR pg_catalog.<>(leftarg = longblob, rightarg = timestamp without time zone, COMMUTATOR = operator(pg_catalog.<>), procedure = pg_catalog.longblob_datetime_ne, restrict = neqsel, join = neqjoinsel);
CREATE OPERATOR pg_catalog.<(leftarg = longblob, rightarg = timestamp without time zone, COMMUTATOR = operator(pg_catalog.<), procedure = pg_catalog.longblob_datetime_lt, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.<=(leftarg = longblob, rightarg = timestamp without time zone, COMMUTATOR = operator(pg_catalog.<=), procedure = pg_catalog.longblob_datetime_le, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.>(leftarg = longblob, rightarg = timestamp without time zone, COMMUTATOR = operator(pg_catalog.>), procedure = pg_catalog.longblob_datetime_gt, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.>=(leftarg = longblob, rightarg = timestamp without time zone, COMMUTATOR = operator(pg_catalog.>=), procedure = pg_catalog.longblob_datetime_ge, restrict = scalarltsel, join = scalarltjoinsel);

CREATE OR REPLACE FUNCTION pg_catalog.datetime_longblob_eq(arg1 timestamp without time zone, arg2 longblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1=$2::text';
CREATE OR REPLACE FUNCTION pg_catalog.datetime_longblob_ne(arg1 timestamp without time zone, arg2 longblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1<>$2::text';
CREATE OR REPLACE FUNCTION pg_catalog.datetime_longblob_lt(arg1 timestamp without time zone, arg2 longblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1<$2::text';
CREATE OR REPLACE FUNCTION pg_catalog.datetime_longblob_le(arg1 timestamp without time zone, arg2 longblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1<=$2::text';
CREATE OR REPLACE FUNCTION pg_catalog.datetime_longblob_gt(arg1 timestamp without time zone, arg2 longblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1>$2::text';
CREATE OR REPLACE FUNCTION pg_catalog.datetime_longblob_ge(arg1 timestamp without time zone, arg2 longblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1>=$2::text';
CREATE OR REPLACE FUNCTION pg_catalog.datetime_longblob_cmp(timestamp without time zone, longblob) RETURNS integer LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.btfloat8cmp($1, $2::text)';
CREATE OPERATOR pg_catalog.=(leftarg = timestamp without time zone, rightarg = longblob, COMMUTATOR = operator(pg_catalog.=), procedure = pg_catalog.datetime_longblob_eq, restrict = eqsel, join = eqjoinsel);
CREATE OPERATOR pg_catalog.<>(leftarg = timestamp without time zone, rightarg = longblob, COMMUTATOR = operator(pg_catalog.<>), procedure = pg_catalog.datetime_longblob_ne, restrict = neqsel, join = neqjoinsel);
CREATE OPERATOR pg_catalog.<(leftarg = timestamp without time zone, rightarg = longblob, COMMUTATOR = operator(pg_catalog.<), procedure = pg_catalog.datetime_longblob_lt, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.<=(leftarg = timestamp without time zone, rightarg = longblob, COMMUTATOR = operator(pg_catalog.<=), procedure = pg_catalog.datetime_longblob_le, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.>(leftarg = timestamp without time zone, rightarg = longblob, COMMUTATOR = operator(pg_catalog.>), procedure = pg_catalog.datetime_longblob_gt, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.>=(leftarg = timestamp without time zone, rightarg = longblob, COMMUTATOR = operator(pg_catalog.>=), procedure = pg_catalog.datetime_longblob_ge, restrict = scalarltsel, join = scalarltjoinsel);

CREATE OR REPLACE FUNCTION pg_catalog.tinyblob_timestamp_eq(arg1 tinyblob, arg2 timestamptz) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT as 'select $1::text=$2';
CREATE OR REPLACE FUNCTION pg_catalog.tinyblob_timestamp_ne(arg1 tinyblob, arg2 timestamptz) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1::text<>$2';
CREATE OR REPLACE FUNCTION pg_catalog.tinyblob_timestamp_lt(arg1 tinyblob, arg2 timestamptz) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1::text<$2';
CREATE OR REPLACE FUNCTION pg_catalog.tinyblob_timestamp_le(arg1 tinyblob, arg2 timestamptz) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1::text<=$2';
CREATE OR REPLACE FUNCTION pg_catalog.tinyblob_timestamp_gt(arg1 tinyblob, arg2 timestamptz) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1::text>$2';
CREATE OR REPLACE FUNCTION pg_catalog.tinyblob_timestamp_ge(arg1 tinyblob, arg2 timestamptz) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1::text>=$2';
CREATE OR REPLACE FUNCTION pg_catalog.tinyblob_timestamp_cmp(tinyblob, timestamptz) RETURNS integer LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.btfloat8cmp($1::text, $2)';
CREATE OPERATOR pg_catalog.=(leftarg = tinyblob, rightarg = timestamptz, COMMUTATOR = operator(pg_catalog.=), procedure = pg_catalog.tinyblob_timestamp_eq, restrict = eqsel, join = eqjoinsel);
CREATE OPERATOR pg_catalog.<>(leftarg = tinyblob, rightarg = timestamptz, COMMUTATOR = operator(pg_catalog.<>), procedure = pg_catalog.tinyblob_timestamp_ne, restrict = neqsel, join = neqjoinsel);
CREATE OPERATOR pg_catalog.<(leftarg = tinyblob, rightarg = timestamptz, COMMUTATOR = operator(pg_catalog.<), procedure = pg_catalog.tinyblob_timestamp_lt, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.<=(leftarg = tinyblob, rightarg = timestamptz, COMMUTATOR = operator(pg_catalog.<=), procedure = pg_catalog.tinyblob_timestamp_le, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.>(leftarg = tinyblob, rightarg = timestamptz, COMMUTATOR = operator(pg_catalog.>), procedure = pg_catalog.tinyblob_timestamp_gt, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.>=(leftarg = tinyblob, rightarg = timestamptz, COMMUTATOR = operator(pg_catalog.>=), procedure = pg_catalog.tinyblob_timestamp_ge, restrict = scalarltsel, join = scalarltjoinsel);

CREATE OR REPLACE FUNCTION pg_catalog.timestamp_tinyblob_eq(arg1 timestamptz, arg2 tinyblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1=$2::text';
CREATE OR REPLACE FUNCTION pg_catalog.timestamp_tinyblob_ne(arg1 timestamptz, arg2 tinyblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1<>$2::text';
CREATE OR REPLACE FUNCTION pg_catalog.timestamp_tinyblob_lt(arg1 timestamptz, arg2 tinyblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1<$2::text';
CREATE OR REPLACE FUNCTION pg_catalog.timestamp_tinyblob_le(arg1 timestamptz, arg2 tinyblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1<=$2::text';
CREATE OR REPLACE FUNCTION pg_catalog.timestamp_tinyblob_gt(arg1 timestamptz, arg2 tinyblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1>$2::text';
CREATE OR REPLACE FUNCTION pg_catalog.timestamp_tinyblob_ge(arg1 timestamptz, arg2 tinyblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1>=$2::text';
CREATE OR REPLACE FUNCTION pg_catalog.timestamp_tinyblob_cmp(timestamptz, tinyblob) RETURNS integer LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.btfloat8cmp($1, $2::text)';
CREATE OPERATOR pg_catalog.=(leftarg = timestamptz, rightarg = tinyblob, COMMUTATOR = operator(pg_catalog.=), procedure = pg_catalog.timestamp_tinyblob_eq, restrict = eqsel, join = eqjoinsel);
CREATE OPERATOR pg_catalog.<>(leftarg = timestamptz, rightarg = tinyblob, COMMUTATOR = operator(pg_catalog.<>), procedure = pg_catalog.timestamp_tinyblob_ne, restrict = neqsel, join = neqjoinsel);
CREATE OPERATOR pg_catalog.<(leftarg = timestamptz, rightarg = tinyblob, COMMUTATOR = operator(pg_catalog.<), procedure = pg_catalog.timestamp_tinyblob_lt, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.<=(leftarg = timestamptz, rightarg = tinyblob, COMMUTATOR = operator(pg_catalog.<=), procedure = pg_catalog.timestamp_tinyblob_le, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.>(leftarg = timestamptz, rightarg = tinyblob, COMMUTATOR = operator(pg_catalog.>), procedure = pg_catalog.timestamp_tinyblob_gt, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.>=(leftarg = timestamptz, rightarg = tinyblob, COMMUTATOR = operator(pg_catalog.>=), procedure = pg_catalog.timestamp_tinyblob_ge, restrict = scalarltsel, join = scalarltjoinsel);

CREATE OR REPLACE FUNCTION pg_catalog.blob_timestamp_eq(arg1 blob, arg2 timestamptz) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT as 'select $1::text=$2';
CREATE OR REPLACE FUNCTION pg_catalog.blob_timestamp_ne(arg1 blob, arg2 timestamptz) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1::text<>$2';
CREATE OR REPLACE FUNCTION pg_catalog.blob_timestamp_lt(arg1 blob, arg2 timestamptz) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1::text<$2';
CREATE OR REPLACE FUNCTION pg_catalog.blob_timestamp_le(arg1 blob, arg2 timestamptz) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1::text<=$2';
CREATE OR REPLACE FUNCTION pg_catalog.blob_timestamp_gt(arg1 blob, arg2 timestamptz) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1::text>$2';
CREATE OR REPLACE FUNCTION pg_catalog.blob_timestamp_ge(arg1 blob, arg2 timestamptz) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1::text>=$2';
CREATE OR REPLACE FUNCTION pg_catalog.blob_timestamp_cmp(blob, timestamptz) RETURNS integer LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.btfloat8cmp($1::text, $2)';
CREATE OPERATOR pg_catalog.=(leftarg = blob, rightarg = timestamptz, COMMUTATOR = operator(pg_catalog.=), procedure = pg_catalog.blob_timestamp_eq, restrict = eqsel, join = eqjoinsel);
CREATE OPERATOR pg_catalog.<>(leftarg = blob, rightarg = timestamptz, COMMUTATOR = operator(pg_catalog.<>), procedure = pg_catalog.blob_timestamp_ne, restrict = neqsel, join = neqjoinsel);
CREATE OPERATOR pg_catalog.<(leftarg = blob, rightarg = timestamptz, COMMUTATOR = operator(pg_catalog.<), procedure = pg_catalog.blob_timestamp_lt, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.<=(leftarg = blob, rightarg = timestamptz, COMMUTATOR = operator(pg_catalog.<=), procedure = pg_catalog.blob_timestamp_le, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.>(leftarg = blob, rightarg = timestamptz, COMMUTATOR = operator(pg_catalog.>), procedure = pg_catalog.blob_timestamp_gt, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.>=(leftarg = blob, rightarg = timestamptz, COMMUTATOR = operator(pg_catalog.>=), procedure = pg_catalog.blob_timestamp_ge, restrict = scalarltsel, join = scalarltjoinsel);

CREATE OR REPLACE FUNCTION pg_catalog.timestamp_blob_eq(arg1 timestamptz, arg2 blob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1=$2::text';
CREATE OR REPLACE FUNCTION pg_catalog.timestamp_blob_ne(arg1 timestamptz, arg2 blob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1<>$2::text';
CREATE OR REPLACE FUNCTION pg_catalog.timestamp_blob_lt(arg1 timestamptz, arg2 blob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1<$2::text';
CREATE OR REPLACE FUNCTION pg_catalog.timestamp_blob_le(arg1 timestamptz, arg2 blob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1<=$2::text';
CREATE OR REPLACE FUNCTION pg_catalog.timestamp_blob_gt(arg1 timestamptz, arg2 blob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1>$2::text';
CREATE OR REPLACE FUNCTION pg_catalog.timestamp_blob_ge(arg1 timestamptz, arg2 blob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1>=$2::text';
CREATE OR REPLACE FUNCTION pg_catalog.timestamp_blob_cmp(timestamptz, blob) RETURNS integer LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.btfloat8cmp($1, $2::text)';
CREATE OPERATOR pg_catalog.=(leftarg = timestamptz, rightarg = blob, COMMUTATOR = operator(pg_catalog.=), procedure = pg_catalog.timestamp_blob_eq, restrict = eqsel, join = eqjoinsel);
CREATE OPERATOR pg_catalog.<>(leftarg = timestamptz, rightarg = blob, COMMUTATOR = operator(pg_catalog.<>), procedure = pg_catalog.timestamp_blob_ne, restrict = neqsel, join = neqjoinsel);
CREATE OPERATOR pg_catalog.<(leftarg = timestamptz, rightarg = blob, COMMUTATOR = operator(pg_catalog.<), procedure = pg_catalog.timestamp_blob_lt, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.<=(leftarg = timestamptz, rightarg = blob, COMMUTATOR = operator(pg_catalog.<=), procedure = pg_catalog.timestamp_blob_le, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.>(leftarg = timestamptz, rightarg = blob, COMMUTATOR = operator(pg_catalog.>), procedure = pg_catalog.timestamp_blob_gt, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.>=(leftarg = timestamptz, rightarg = blob, COMMUTATOR = operator(pg_catalog.>=), procedure = pg_catalog.timestamp_blob_ge, restrict = scalarltsel, join = scalarltjoinsel);

CREATE OR REPLACE FUNCTION pg_catalog.mediumblob_timestamp_eq(arg1 mediumblob, arg2 timestamptz) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT as 'select $1::text=$2';
CREATE OR REPLACE FUNCTION pg_catalog.mediumblob_timestamp_ne(arg1 mediumblob, arg2 timestamptz) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1::text<>$2';
CREATE OR REPLACE FUNCTION pg_catalog.mediumblob_timestamp_lt(arg1 mediumblob, arg2 timestamptz) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1::text<$2';
CREATE OR REPLACE FUNCTION pg_catalog.mediumblob_timestamp_le(arg1 mediumblob, arg2 timestamptz) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1::text<=$2';
CREATE OR REPLACE FUNCTION pg_catalog.mediumblob_timestamp_gt(arg1 mediumblob, arg2 timestamptz) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1::text>$2';
CREATE OR REPLACE FUNCTION pg_catalog.mediumblob_timestamp_ge(arg1 mediumblob, arg2 timestamptz) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1::text>=$2';
CREATE OR REPLACE FUNCTION pg_catalog.mediumblob_timestamp_cmp(mediumblob, timestamptz) RETURNS integer LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.btfloat8cmp($1::text, $2)';
CREATE OPERATOR pg_catalog.=(leftarg = mediumblob, rightarg = timestamptz, COMMUTATOR = operator(pg_catalog.=), procedure = pg_catalog.mediumblob_timestamp_eq, restrict = eqsel, join = eqjoinsel);
CREATE OPERATOR pg_catalog.<>(leftarg = mediumblob, rightarg = timestamptz, COMMUTATOR = operator(pg_catalog.<>), procedure = pg_catalog.mediumblob_timestamp_ne, restrict = neqsel, join = neqjoinsel);
CREATE OPERATOR pg_catalog.<(leftarg = mediumblob, rightarg = timestamptz, COMMUTATOR = operator(pg_catalog.<), procedure = pg_catalog.mediumblob_timestamp_lt, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.<=(leftarg = mediumblob, rightarg = timestamptz, COMMUTATOR = operator(pg_catalog.<=), procedure = pg_catalog.mediumblob_timestamp_le, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.>(leftarg = mediumblob, rightarg = timestamptz, COMMUTATOR = operator(pg_catalog.>), procedure = pg_catalog.mediumblob_timestamp_gt, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.>=(leftarg = mediumblob, rightarg = timestamptz, COMMUTATOR = operator(pg_catalog.>=), procedure = pg_catalog.mediumblob_timestamp_ge, restrict = scalarltsel, join = scalarltjoinsel);

CREATE OR REPLACE FUNCTION pg_catalog.timestamp_mediumblob_eq(arg1 timestamptz, arg2 mediumblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1=$2::text';
CREATE OR REPLACE FUNCTION pg_catalog.timestamp_mediumblob_ne(arg1 timestamptz, arg2 mediumblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1<>$2::text';
CREATE OR REPLACE FUNCTION pg_catalog.timestamp_mediumblob_lt(arg1 timestamptz, arg2 mediumblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1<$2::text';
CREATE OR REPLACE FUNCTION pg_catalog.timestamp_mediumblob_le(arg1 timestamptz, arg2 mediumblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1<=$2::text';
CREATE OR REPLACE FUNCTION pg_catalog.timestamp_mediumblob_gt(arg1 timestamptz, arg2 mediumblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1>$2::text';
CREATE OR REPLACE FUNCTION pg_catalog.timestamp_mediumblob_ge(arg1 timestamptz, arg2 mediumblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1>=$2::text';
CREATE OR REPLACE FUNCTION pg_catalog.timestamp_mediumblob_cmp(timestamptz, mediumblob) RETURNS integer LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.btfloat8cmp($1, $2::text)';
CREATE OPERATOR pg_catalog.=(leftarg = timestamptz, rightarg = mediumblob, COMMUTATOR = operator(pg_catalog.=), procedure = pg_catalog.timestamp_mediumblob_eq, restrict = eqsel, join = eqjoinsel);
CREATE OPERATOR pg_catalog.<>(leftarg = timestamptz, rightarg = mediumblob, COMMUTATOR = operator(pg_catalog.<>), procedure = pg_catalog.timestamp_mediumblob_ne, restrict = neqsel, join = neqjoinsel);
CREATE OPERATOR pg_catalog.<(leftarg = timestamptz, rightarg = mediumblob, COMMUTATOR = operator(pg_catalog.<), procedure = pg_catalog.timestamp_mediumblob_lt, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.<=(leftarg = timestamptz, rightarg = mediumblob, COMMUTATOR = operator(pg_catalog.<=), procedure = pg_catalog.timestamp_mediumblob_le, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.>(leftarg = timestamptz, rightarg = mediumblob, COMMUTATOR = operator(pg_catalog.>), procedure = pg_catalog.timestamp_mediumblob_gt, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.>=(leftarg = timestamptz, rightarg = mediumblob, COMMUTATOR = operator(pg_catalog.>=), procedure = pg_catalog.timestamp_mediumblob_ge, restrict = scalarltsel, join = scalarltjoinsel);

CREATE OR REPLACE FUNCTION pg_catalog.longblob_timestamp_eq(arg1 longblob, arg2 timestamptz) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT as 'select $1::text=$2';
CREATE OR REPLACE FUNCTION pg_catalog.longblob_timestamp_ne(arg1 longblob, arg2 timestamptz) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1::text<>$2';
CREATE OR REPLACE FUNCTION pg_catalog.longblob_timestamp_lt(arg1 longblob, arg2 timestamptz) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1::text<$2';
CREATE OR REPLACE FUNCTION pg_catalog.longblob_timestamp_le(arg1 longblob, arg2 timestamptz) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1::text<=$2';
CREATE OR REPLACE FUNCTION pg_catalog.longblob_timestamp_gt(arg1 longblob, arg2 timestamptz) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1::text>$2';
CREATE OR REPLACE FUNCTION pg_catalog.longblob_timestamp_ge(arg1 longblob, arg2 timestamptz) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1::text>=$2';
CREATE OR REPLACE FUNCTION pg_catalog.longblob_timestamp_cmp(longblob, timestamptz) RETURNS integer LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.btfloat8cmp($1::text, $2)';
CREATE OPERATOR pg_catalog.=(leftarg = longblob, rightarg = timestamptz, COMMUTATOR = operator(pg_catalog.=), procedure = pg_catalog.longblob_timestamp_eq, restrict = eqsel, join = eqjoinsel);
CREATE OPERATOR pg_catalog.<>(leftarg = longblob, rightarg = timestamptz, COMMUTATOR = operator(pg_catalog.<>), procedure = pg_catalog.longblob_timestamp_ne, restrict = neqsel, join = neqjoinsel);
CREATE OPERATOR pg_catalog.<(leftarg = longblob, rightarg = timestamptz, COMMUTATOR = operator(pg_catalog.<), procedure = pg_catalog.longblob_timestamp_lt, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.<=(leftarg = longblob, rightarg = timestamptz, COMMUTATOR = operator(pg_catalog.<=), procedure = pg_catalog.longblob_timestamp_le, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.>(leftarg = longblob, rightarg = timestamptz, COMMUTATOR = operator(pg_catalog.>), procedure = pg_catalog.longblob_timestamp_gt, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.>=(leftarg = longblob, rightarg = timestamptz, COMMUTATOR = operator(pg_catalog.>=), procedure = pg_catalog.longblob_timestamp_ge, restrict = scalarltsel, join = scalarltjoinsel);

CREATE OR REPLACE FUNCTION pg_catalog.timestamp_longblob_eq(arg1 timestamptz, arg2 longblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1=$2::text';
CREATE OR REPLACE FUNCTION pg_catalog.timestamp_longblob_ne(arg1 timestamptz, arg2 longblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1<>$2::text';
CREATE OR REPLACE FUNCTION pg_catalog.timestamp_longblob_lt(arg1 timestamptz, arg2 longblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1<$2::text';
CREATE OR REPLACE FUNCTION pg_catalog.timestamp_longblob_le(arg1 timestamptz, arg2 longblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1<=$2::text';
CREATE OR REPLACE FUNCTION pg_catalog.timestamp_longblob_gt(arg1 timestamptz, arg2 longblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1>$2::text';
CREATE OR REPLACE FUNCTION pg_catalog.timestamp_longblob_ge(arg1 timestamptz, arg2 longblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1>=$2::text';
CREATE OR REPLACE FUNCTION pg_catalog.timestamp_longblob_cmp(timestamptz, longblob) RETURNS integer LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.btfloat8cmp($1, $2::text)';
CREATE OPERATOR pg_catalog.=(leftarg = timestamptz, rightarg = longblob, COMMUTATOR = operator(pg_catalog.=), procedure = pg_catalog.timestamp_longblob_eq, restrict = eqsel, join = eqjoinsel);
CREATE OPERATOR pg_catalog.<>(leftarg = timestamptz, rightarg = longblob, COMMUTATOR = operator(pg_catalog.<>), procedure = pg_catalog.timestamp_longblob_ne, restrict = neqsel, join = neqjoinsel);
CREATE OPERATOR pg_catalog.<(leftarg = timestamptz, rightarg = longblob, COMMUTATOR = operator(pg_catalog.<), procedure = pg_catalog.timestamp_longblob_lt, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.<=(leftarg = timestamptz, rightarg = longblob, COMMUTATOR = operator(pg_catalog.<=), procedure = pg_catalog.timestamp_longblob_le, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.>(leftarg = timestamptz, rightarg = longblob, COMMUTATOR = operator(pg_catalog.>), procedure = pg_catalog.timestamp_longblob_gt, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OPERATOR pg_catalog.>=(leftarg = timestamptz, rightarg = longblob, COMMUTATOR = operator(pg_catalog.>=), procedure = pg_catalog.timestamp_longblob_ge, restrict = scalarltsel, join = scalarltjoinsel);
CREATE OR REPLACE FUNCTION pg_catalog.any_accum(numeric[], anyelement) RETURNS numeric[] LANGUAGE C STRICT AS  '$libdir/dolphin',  'any_accum';
CREATE AGGREGATE pg_catalog.stddev_pop(json) (SFUNC = any_accum, cFUNC = numeric_collect, STYPE = numeric[], finalfunc = numeric_stddev_pop, initcond = '{0,0,0}', initcollect = '{0,0,0}');

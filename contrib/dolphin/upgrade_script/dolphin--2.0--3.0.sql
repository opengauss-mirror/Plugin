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
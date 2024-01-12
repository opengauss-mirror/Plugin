DROP FUNCTION IF EXISTS pg_catalog.int8_cast_time(int1);
DROP FUNCTION IF EXISTS pg_catalog.int16_cast_time(int2);
DROP FUNCTION IF EXISTS pg_catalog.int32_cast_time(int4);
DROP FUNCTION IF EXISTS pg_catalog.int64_cast_time(int8);

DROP FUNCTION IF EXISTS pg_catalog.uint8_cast_time(uint1);
DROP FUNCTION IF EXISTS pg_catalog.uint16_cast_time(uint2);
DROP FUNCTION IF EXISTS pg_catalog.uint32_cast_time(uint4);
DROP FUNCTION IF EXISTS pg_catalog.uint64_cast_time(uint8);

DROP FUNCTION IF EXISTS pg_catalog.float4_cast_time(float4);
DROP FUNCTION IF EXISTS pg_catalog.float8_cast_time(float8);
DROP FUNCTION IF EXISTS pg_catalog.numeric_cast_time(numeric);

DROP FUNCTION IF EXISTS pg_catalog.bit_cast_time(bit);
DROP FUNCTION IF EXISTS pg_catalog.bit_cast_timestamp(bit);
DROP FUNCTION IF EXISTS pg_catalog.bit_cast_datetime(bit);
DROP FUNCTION IF EXISTS pg_catalog.bit_cast_date(bit);

DROP FUNCTION IF EXISTS pg_catalog.dolphin_invoke();
CREATE FUNCTION pg_catalog.dolphin_invoke()
    RETURNS VOID AS '$libdir/dolphin','dolphin_invoke' LANGUAGE C STRICT;

DROP FUNCTION IF EXISTS pg_catalog.date_cast(cstring, boolean);
DROP FUNCTION IF EXISTS pg_catalog.timestamp_cast(cstring, oid, integer, boolean);

DROP FUNCTION IF EXISTS pg_catalog.time_cast(cstring, boolean);
DROP CAST IF EXISTS (TEXT AS time);
DROP FUNCTION IF EXISTS pg_catalog.time_cast_implicit(TEXT);
DROP FUNCTION IF EXISTS pg_catalog.text_time_explicit(TEXT);

DROP FUNCTION IF EXISTS pg_catalog.day(time without time zone);

DROP FUNCTION IF EXISTS pg_catalog.time_to_sec(int8);

DROP FUNCTION IF EXISTS pg_catalog.time_to_sec(numeric);
CREATE OR REPLACE FUNCTION pg_catalog.time_to_sec(numeric) RETURNS int4 AS $$ SELECT pg_catalog.time_to_sec(cast($1 as text)) $$ LANGUAGE SQL;

DROP OPERATOR IF EXISTS pg_catalog.=(time, timestamp without time zone);
DROP OPERATOR IF EXISTS pg_catalog.<>(time, timestamp without time zone);
DROP OPERATOR IF EXISTS pg_catalog.<=(time, timestamp without time zone);
DROP OPERATOR IF EXISTS pg_catalog.<(time, timestamp without time zone);
DROP OPERATOR IF EXISTS pg_catalog.>=(time, timestamp without time zone);
DROP OPERATOR IF EXISTS pg_catalog.>(time, timestamp without time zone);
DROP FUNCTION IF EXISTS pg_catalog.time_eq_timestamp (time, timestamp without time zone);
DROP FUNCTION IF EXISTS pg_catalog.time_ne_timestamp (time, timestamp without time zone);
DROP FUNCTION IF EXISTS pg_catalog.time_le_timestamp (time, timestamp without time zone);
DROP FUNCTION IF EXISTS pg_catalog.time_lt_timestamp (time, timestamp without time zone);
DROP FUNCTION IF EXISTS pg_catalog.time_ge_timestamp (time, timestamp without time zone);
DROP FUNCTION IF EXISTS pg_catalog.time_gt_timestamp (time, timestamp without time zone);
DROP OPERATOR IF EXISTS pg_catalog.=(timestamp without time zone, time);
DROP OPERATOR IF EXISTS pg_catalog.<>(timestamp without time zone, time);
DROP OPERATOR IF EXISTS pg_catalog.<=(timestamp without time zone, time);
DROP OPERATOR IF EXISTS pg_catalog.<(timestamp without time zone, time);
DROP OPERATOR IF EXISTS pg_catalog.>=(timestamp without time zone, time);
DROP OPERATOR IF EXISTS pg_catalog.>(timestamp without time zone, time);
DROP FUNCTION IF EXISTS pg_catalog.timestamp_eq_time (timestamp without time zone, time);
DROP FUNCTION IF EXISTS pg_catalog.timestamp_ne_time (timestamp without time zone, time);
DROP FUNCTION IF EXISTS pg_catalog.timestamp_le_time (timestamp without time zone, time);
DROP FUNCTION IF EXISTS pg_catalog.timestamp_lt_time (timestamp without time zone, time);
DROP FUNCTION IF EXISTS pg_catalog.timestamp_ge_time (timestamp without time zone, time);
DROP FUNCTION IF EXISTS pg_catalog.timestamp_gt_time (timestamp without time zone, time);

DROP OPERATOR IF EXISTS pg_catalog.=(time, timestamp with time zone);
DROP OPERATOR IF EXISTS pg_catalog.<>(time, timestamp with time zone);
DROP OPERATOR IF EXISTS pg_catalog.<=(time, timestamp with time zone);
DROP OPERATOR IF EXISTS pg_catalog.<(time, timestamp with time zone);
DROP OPERATOR IF EXISTS pg_catalog.>=(time, timestamp with time zone);
DROP OPERATOR IF EXISTS pg_catalog.>(time, timestamp with time zone);
DROP FUNCTION IF EXISTS pg_catalog.time_eq_timestamptz (time, timestamp with time zone);
DROP FUNCTION IF EXISTS pg_catalog.time_ne_timestamptz (time, timestamp with time zone);
DROP FUNCTION IF EXISTS pg_catalog.time_le_timestamptz (time, timestamp with time zone);
DROP FUNCTION IF EXISTS pg_catalog.time_lt_timestamptz (time, timestamp with time zone);
DROP FUNCTION IF EXISTS pg_catalog.time_ge_timestamptz (time, timestamp with time zone);
DROP FUNCTION IF EXISTS pg_catalog.time_gt_timestamptz (time, timestamp with time zone);
DROP OPERATOR IF EXISTS pg_catalog.=(timestamp with time zone, time);
DROP OPERATOR IF EXISTS pg_catalog.<>(timestamp with time zone, time);
DROP OPERATOR IF EXISTS pg_catalog.<=(timestamp with time zone, time);
DROP OPERATOR IF EXISTS pg_catalog.<(timestamp with time zone, time);
DROP OPERATOR IF EXISTS pg_catalog.>=(timestamp with time zone, time);
DROP OPERATOR IF EXISTS pg_catalog.>(timestamp with time zone, time);
DROP FUNCTION IF EXISTS pg_catalog.timestamptz_eq_time (timestamp with time zone, time);
DROP FUNCTION IF EXISTS pg_catalog.timestamptz_ne_time (timestamp with time zone, time);
DROP FUNCTION IF EXISTS pg_catalog.timestamptz_le_time (timestamp with time zone, time);
DROP FUNCTION IF EXISTS pg_catalog.timestamptz_lt_time (timestamp with time zone, time);
DROP FUNCTION IF EXISTS pg_catalog.timestamptz_ge_time (timestamp with time zone, time);
DROP FUNCTION IF EXISTS pg_catalog.timestamptz_gt_time (timestamp with time zone, time);

do $$
begin
    update pg_cast set castcontext='e', castowner=10 where castsource=1560 and casttarget=20 and castcontext='a';
    update pg_cast set castcontext='e', castowner=10 where castsource=1560 and casttarget=23 and castcontext='a';
    update pg_cast set castcontext='e', castowner=10 where castsource=20 and casttarget=1560 and castcontext='a';
    update pg_cast set castcontext='e', castowner=10 where castsource=23 and casttarget=1560 and castcontext='a';
end
$$;
drop CAST IF EXISTS (uint4 AS bit);
drop CAST IF EXISTS (uint8 AS bit);
CREATE CAST (uint4 AS bit) WITH FUNCTION bitfromuint4(uint4, int4);
CREATE CAST (uint8 AS bit) WITH FUNCTION bitfromuint8(uint8, int4);

DROP FUNCTION IF EXISTS pg_catalog.rand(int16);
DROP FUNCTION IF EXISTS pg_catalog.rand(uint4);
DROP FUNCTION IF EXISTS pg_catalog.rand(timestamp with time zone);
DROP FUNCTION IF EXISTS pg_catalog.rand(date);
DROP FUNCTION IF EXISTS pg_catalog.rand(year);
DROP FUNCTION IF EXISTS pg_catalog.rand(binary);
DROP FUNCTION IF EXISTS pg_catalog.rand(blob);
DROP FUNCTION IF EXISTS pg_catalog.rand(anyenum);
DROP FUNCTION IF EXISTS pg_catalog.rand(anyset);
DROP FUNCTION IF EXISTS pg_catalog.rand(json);

DROP FUNCTION IF EXISTS pg_catalog.random_bytes(int4);
DROP FUNCTION IF EXISTS pg_catalog.random_bytes(bit);
DROP FUNCTION IF EXISTS pg_catalog.random_bytes(timestamp with time zone);
DROP FUNCTION IF EXISTS pg_catalog.random_bytes(date);
DROP FUNCTION IF EXISTS pg_catalog.random_bytes(year);
DROP FUNCTION IF EXISTS pg_catalog.random_bytes(binary);
DROP FUNCTION IF EXISTS pg_catalog.random_bytes(blob);
DROP FUNCTION IF EXISTS pg_catalog.random_bytes(anyenum);
DROP FUNCTION IF EXISTS pg_catalog.random_bytes(anyset);
DROP FUNCTION IF EXISTS pg_catalog.random_bytes(json);

CREATE OR REPLACE FUNCTION pg_catalog.tinyblob_rawout (
tinyblob
) RETURNS cstring LANGUAGE INTERNAL IMMUTABLE STRICT as 'byteaout';

CREATE OR REPLACE FUNCTION pg_catalog.mediumblob_rawout (
mediumblob
) RETURNS cstring LANGUAGE INTERNAL IMMUTABLE STRICT as 'byteaout';

CREATE OR REPLACE FUNCTION pg_catalog.longblob_rawout (
longblob
) RETURNS cstring LANGUAGE INTERNAL IMMUTABLE STRICT as 'byteaout';

-- Make the result of oct(bit) and conv(bit) identical to Mysql
DROP FUNCTION IF EXISTS pg_catalog.conv(bit, int4, int4) CASCADE;

DROP FUNCTION IF EXISTS pg_catalog.oct(bit);
CREATE OR REPLACE FUNCTION pg_catalog.oct(bit) RETURNS text AS
$$
BEGIN
    RETURN 0;
END;
$$
LANGUAGE plpgsql;

DROP FUNCTION IF EXISTS pg_catalog.dayofmonth(text);
DROP FUNCTION IF EXISTS pg_catalog.dayofmonth(numeric);

drop function if EXISTS pg_catalog.length(binary);
drop function if EXISTS pg_catalog.length(varbinary);

drop CAST IF EXISTS (timestamptz as boolean);
drop CAST IF EXISTS (timestamp(0) without time zone as boolean);
DROP FUNCTION IF EXISTS pg_catalog.timestamptz_bool(timestamptz);
DROP FUNCTION IF EXISTS pg_catalog.timestamp_bool(timestamp(0) without time zone);

DROP FUNCTION IF EXISTS pg_catalog.date_format (time without time zone, text);
DROP FUNCTION IF EXISTS pg_catalog.to_char(time without time zone, text);

drop function IF EXISTS pg_catalog."user"();

DROP FUNCTION IF EXISTS pg_catalog.hour(date);
DROP FUNCTION IF EXISTS pg_catalog.microsecond(date);
DROP FUNCTION IF EXISTS pg_catalog.minute(date);
DROP FUNCTION IF EXISTS pg_catalog.second(date);

DROP FUNCTION IF EXISTS pg_catalog.hour(timetz);
DROP FUNCTION IF EXISTS pg_catalog.microsecond(timetz);
DROP FUNCTION IF EXISTS pg_catalog.minute(timetz);
DROP FUNCTION IF EXISTS pg_catalog.second(timetz);

DROP FUNCTION IF EXISTS pg_catalog.hour(timestamptz);
DROP FUNCTION IF EXISTS pg_catalog.microsecond(timestamptz);
DROP FUNCTION IF EXISTS pg_catalog.minute(timestamptz);
DROP FUNCTION IF EXISTS pg_catalog.second(timestamptz);


DROP OPERATOR IF EXISTS pg_catalog.>(timestamp without time zone, double precision);
drop function if exists pg_catalog.datetime_double_gt(timestamp without time zone, double precision);

DROP OPERATOR IF EXISTS pg_catalog.>=(timestamp without time zone, double precision);
drop function if exists pg_catalog.datetime_double_ge(timestamp without time zone, double precision);

DROP OPERATOR IF EXISTS pg_catalog.<(timestamp without time zone, double precision);
drop function if exists pg_catalog.datetime_double_lt(timestamp without time zone, double precision);

DROP OPERATOR IF EXISTS pg_catalog.<=(timestamp without time zone, double precision);
drop function if exists pg_catalog.datetime_double_le(timestamp without time zone, double precision);

DROP OPERATOR IF EXISTS pg_catalog.=(timestamp without time zone, double precision);
drop function if exists pg_catalog.datetime_double_eq(timestamp without time zone, double precision);

DROP OPERATOR IF EXISTS pg_catalog.<>(timestamp without time zone, double precision);
drop function if exists pg_catalog.datetime_double_ne(timestamp without time zone, double precision);

DROP OPERATOR IF EXISTS pg_catalog.>(timestamptz, double precision);
drop function if exists pg_catalog.timestamp_double_gt(timestamptz, double precision);

DROP OPERATOR IF EXISTS pg_catalog.>=(timestamp, double);
drop function if exists pg_catalog.timestamp_double_ge(timestamptz, double precision);

DROP OPERATOR IF EXISTS pg_catalog.<(timestamp, double);
drop function if exists pg_catalog.timestamp_double_lt(timestamptz, double precision);

DROP OPERATOR IF EXISTS pg_catalog.<=(timestamp, double);
drop function if exists pg_catalog.timestamp_double_le(timestamptz, double precision);

DROP OPERATOR IF EXISTS pg_catalog.=(timestamp, double);
drop function if exists pg_catalog.timestamp_double_eq(timestamptz, double precision);

DROP OPERATOR IF EXISTS pg_catalog.<>(timestamp, double);
drop function if exists pg_catalog.timestamp_double_ne(timestamptz, double precision);

DROP OPERATOR IF EXISTS pg_catalog.>(timestamp without time zone, uint8);
drop function if exists pg_catalog.datetime_uint8_gt(timestamp without time zone, uint8);

DROP OPERATOR IF EXISTS pg_catalog.>=(timestamp without time zone, uint8);
drop function if exists pg_catalog.datetime_uint8_ge(timestamp without time zone, uint8);

DROP OPERATOR IF EXISTS pg_catalog.<(timestamp without time zone, uint8);
drop function if exists pg_catalog.datetime_uint8_lt(timestamp without time zone, uint8);

DROP OPERATOR IF EXISTS pg_catalog.<=(timestamp without time zone, uint8);
drop function if exists pg_catalog.datetime_uint8_le(timestamp without time zone, uint8);

DROP OPERATOR IF EXISTS pg_catalog.=(timestamp without time zone, uint8);
drop function if exists pg_catalog.datetime_uint8_eq(timestamp without time zone, uint8);

DROP OPERATOR IF EXISTS pg_catalog.<>(timestamp without time zone, uint8);
drop function if exists pg_catalog.datetime_uint8_ne(timestamp without time zone, uint8);

DROP OPERATOR IF EXISTS pg_catalog.>(timestamptz, uint8);
drop function if exists pg_catalog.timestamp_uint8_gt(timestamptz, uint8);

DROP OPERATOR IF EXISTS pg_catalog.>=(timestamptz, uint8);
drop function if exists pg_catalog.timestamp_uint8_ge(timestamptz, uint8);

DROP OPERATOR IF EXISTS pg_catalog.<(timestamptz, uint8);
drop function if exists pg_catalog.timestamp_uint8_lt(timestamptz, uint8);

DROP OPERATOR IF EXISTS pg_catalog.<=(timestamptz, uint8);
drop function if exists pg_catalog.timestamp_uint8_le(timestamptz, uint8);

DROP OPERATOR IF EXISTS pg_catalog.=(timestamptz, uint8);
drop function if exists pg_catalog.timestamp_uint8_eq(timestamptz, uint8);

DROP OPERATOR IF EXISTS pg_catalog.<>(timestamptz, uint8);
drop function if exists pg_catalog.timestamp_uint8_ne(timestamptz, uint8);

drop function if exists pg_catalog.convert_datetime_double(double precision);
drop function if exists pg_catalog.convert_timestamptz_double(double precision);
drop function if exists pg_catalog.convert_datetime_uint8(uint8);
drop function if exists pg_catalog.convert_timestamptz_uint8(uint8);

DROP FUNCTION IF EXISTS pg_catalog.hex(tinyblob);
DROP FUNCTION IF EXISTS pg_catalog.hex(blob);
DROP FUNCTION IF EXISTS pg_catalog.hex(mediumblob);
DROP FUNCTION IF EXISTS pg_catalog.hex(longblob);

CREATE OR REPLACE FUNCTION pg_catalog.binary_out (binary) RETURNS cstring LANGUAGE INTERNAL IMMUTABLE STRICT as 'byteaout';

CREATE OR REPLACE FUNCTION pg_catalog.varbinary_out (varbinary) RETURNS cstring LANGUAGE INTERNAL IMMUTABLE STRICT as 'byteaout';

DROP OPERATOR IF EXISTS pg_catalog.|(double precision, binary) CASCADE;
DROP OPERATOR IF EXISTS pg_catalog.|(binary, double precision) CASCADE;
DROP OPERATOR IF EXISTS pg_catalog.|(uint8, binary) CASCADE;
DROP OPERATOR IF EXISTS pg_catalog.|(binary, uint8) CASCADE;
DROP OPERATOR IF EXISTS pg_catalog.|(binary, binary) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.double_or_binary(double precision, binary) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.binary_or_double(binary, double precision) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.uint8_or_binary(uint8, binary) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.binary_or_uint8(binary, uint8) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.binary_or_binary(binary, binary) CASCADE;

DROP FUNCTION IF EXISTS pg_catalog.div(binary, double precision);
DROP FUNCTION IF EXISTS pg_catalog.div(double precision, binary);
DROP FUNCTION IF EXISTS pg_catalog.div(binary, binary);

DROP CAST IF EXISTS (year AS boolean);
DROP FUNCTION IF EXISTS pg_catalog.year_to_bool(year);

-- not operator
DROP OPERATOR IF EXISTS dolphin_catalog.~(rightarg = int1);
DROP FUNCTION IF EXISTS dolphin_catalog.dolphin_int1not(int1);

DROP OPERATOR IF EXISTS dolphin_catalog.~(rightarg = int2);
DROP FUNCTION IF EXISTS dolphin_catalog.dolphin_int2not(int2);

DROP OPERATOR IF EXISTS dolphin_catalog.~(rightarg = int4);
DROP FUNCTION IF EXISTS dolphin_catalog.dolphin_int4not(int4);

DROP OPERATOR IF EXISTS dolphin_catalog.~(rightarg = int8);
DROP FUNCTION IF EXISTS dolphin_catalog.dolphin_int8not(int8);

DROP OPERATOR IF EXISTS dolphin_catalog.~(rightarg = uint1);
DROP FUNCTION IF EXISTS dolphin_catalog.dolphin_uint1not(uint1);

DROP OPERATOR IF EXISTS dolphin_catalog.~(rightarg = uint2);
DROP FUNCTION IF EXISTS dolphin_catalog.dolphin_uint2not(uint2);

DROP OPERATOR IF EXISTS dolphin_catalog.~(rightarg = uint4);
DROP FUNCTION IF EXISTS dolphin_catalog.dolphin_uint4not(uint4);

DROP OPERATOR IF EXISTS dolphin_catalog.~(rightarg = uint8);
DROP FUNCTION IF EXISTS dolphin_catalog.dolphin_uint8not(uint8);

DROP OPERATOR IF EXISTS dolphin_catalog.~(rightarg = float4);
DROP FUNCTION IF EXISTS dolphin_catalog.dolphin_float4not(float4);

DROP OPERATOR IF EXISTS dolphin_catalog.~(rightarg = float8);
DROP FUNCTION IF EXISTS dolphin_catalog.dolphin_float8not(float8);

DROP OPERATOR IF EXISTS dolphin_catalog.~(rightarg = numeric);
DROP FUNCTION IF EXISTS dolphin_catalog.dolphin_numericnot(numeric);

DROP OPERATOR IF EXISTS dolphin_catalog.~(rightarg = bit);
DROP FUNCTION IF EXISTS dolphin_catalog.dolphin_bitnot(bit);

DROP OPERATOR IF EXISTS dolphin_catalog.~(rightarg = boolean);
DROP FUNCTION IF EXISTS dolphin_catalog.dolphin_boolnot(boolean);

DROP OPERATOR IF EXISTS dolphin_catalog.~(rightarg = date);
DROP FUNCTION IF EXISTS dolphin_catalog.dolphin_datenot(date);

DROP OPERATOR IF EXISTS dolphin_catalog.~(rightarg = time);
DROP FUNCTION IF EXISTS dolphin_catalog.dolphin_timenot(time);

DROP OPERATOR IF EXISTS dolphin_catalog.~(rightarg = timestamp);
DROP FUNCTION IF EXISTS dolphin_catalog.dolphin_timestampnot(timestamp without time zone);

DROP OPERATOR IF EXISTS dolphin_catalog.~(rightarg = timestamp);
DROP FUNCTION IF EXISTS dolphin_catalog.dolphin_timestamptznot(timestamp with time zone);

DROP OPERATOR IF EXISTS dolphin_catalog.~(rightarg = year);
DROP FUNCTION IF EXISTS dolphin_catalog.dolphin_yearnot(year);

DROP OPERATOR IF EXISTS dolphin_catalog.~(rightarg = char);
DROP FUNCTION IF EXISTS dolphin_catalog.dolphin_charnot(char);

DROP OPERATOR IF EXISTS dolphin_catalog.~(rightarg = varchar);
DROP FUNCTION IF EXISTS dolphin_catalog.dolphin_varcharnot(varchar);

DROP OPERATOR IF EXISTS dolphin_catalog.~(rightarg = text);
DROP FUNCTION IF EXISTS dolphin_catalog.dolphin_textnot(text);

DROP OPERATOR IF EXISTS dolphin_catalog.~(rightarg = anyelement);
DROP FUNCTION IF EXISTS dolphin_catalog.dolphin_varlenanot(anyelement);

DROP OPERATOR IF EXISTS dolphin_catalog.~(rightarg = anyenum);
DROP FUNCTION IF EXISTS dolphin_catalog.dolphin_enumnot(anyenum);

DROP OPERATOR IF EXISTS dolphin_catalog.~(rightarg = anyset);
DROP FUNCTION IF EXISTS dolphin_catalog.dolphin_setnot(anyset);

DROP FUNCTION IF EXISTS pg_catalog.degrees(boolean);
DROP FUNCTION IF EXISTS pg_catalog.degrees(year);
DROP FUNCTION IF EXISTS pg_catalog.degrees(json);
DROP FUNCTION IF EXISTS pg_catalog.exp(year);
DROP FUNCTION IF EXISTS pg_catalog.exp(json);

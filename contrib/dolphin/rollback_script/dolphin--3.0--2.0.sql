DROP OPERATOR CLASS IF EXISTS pg_catalog.enumtext_ops USING hash;

DROP FUNCTION IF EXISTS pg_catalog.nvarchar2_cast_int8(nvarchar2);
DROP FUNCTION IF EXISTS pg_catalog.nvarchar2_enum(nvarchar2, int4, anyelement);
DROP FUNCTION IF EXISTS pg_catalog.nvarchar2_cast_ui8(nvarchar2);
DROP FUNCTION IF EXISTS pg_catalog.nvarchar2_cast_ui4(nvarchar2);
DROP FUNCTION IF EXISTS pg_catalog.nvarchar2_cast_ui2(nvarchar2);
DROP FUNCTION IF EXISTS pg_catalog.nvarchar2_cast_ui1(nvarchar2);

DROP CAST IF EXISTS (boolean as time without time zone);
DROP CAST IF EXISTS (boolean as date);
DROP CAST IF EXISTS (boolean as timestamp without time zone);
DROP CAST IF EXISTS (boolean as timestamptz);
DROP FUNCTION IF EXISTS pg_catalog.boolean_time(boolean);
DROP FUNCTION IF EXISTS pg_catalog.boolean_date(boolean);
DROP FUNCTION IF EXISTS pg_catalog.boolean_datetime(boolean);
DROP FUNCTION IF EXISTS pg_catalog.boolean_timestamptz(boolean);

DROP FUNCTION IF EXISTS pg_catalog.bool_cast_time(boolean);
DROP FUNCTION IF EXISTS pg_catalog.bool_cast_date(boolean);
DROP FUNCTION IF EXISTS pg_catalog.bool_cast_datetime(boolean);
DROP FUNCTION IF EXISTS pg_catalog.bool_cast_timestamptz(boolean);

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
DROP FUNCTION IF EXISTS pg_catalog.timestamptz_cast(cstring, oid, integer, boolean);

DROP FUNCTION IF EXISTS pg_catalog.time_cast(cstring, boolean);
DROP CAST IF EXISTS (TEXT AS time);
DROP FUNCTION IF EXISTS pg_catalog.time_cast_implicit(TEXT);
DROP FUNCTION IF EXISTS pg_catalog.text_time_explicit(TEXT);

DROP FUNCTION IF EXISTS pg_catalog.day(time without time zone);

DROP FUNCTION IF EXISTS pg_catalog.time_to_sec(json);
DROP FUNCTION IF EXISTS pg_catalog.time_to_sec(anyenum);
DROP FUNCTION IF EXISTS pg_catalog.time_to_sec(anyset);
DROP FUNCTION IF EXISTS pg_catalog.time_to_sec(bit);
DROP FUNCTION IF EXISTS pg_catalog.time_to_sec(longblob);
DROP FUNCTION IF EXISTS pg_catalog.time_to_sec(year);
DROP FUNCTION IF EXISTS pg_catalog.time_to_sec(date);
DROP FUNCTION IF EXISTS pg_catalog.time_to_sec(timestamp with time zone);
DROP FUNCTION IF EXISTS pg_catalog.time_to_sec(timestamp without time zone);
DROP FUNCTION IF EXISTS pg_catalog.time_to_sec(numeric);
DROP FUNCTION IF EXISTS pg_catalog.time_to_sec(int8);
DROP FUNCTION IF EXISTS pg_catalog.time_to_sec(text);

CREATE OR REPLACE FUNCTION pg_catalog.time_to_sec(text) RETURNS int4 LANGUAGE C STABLE RETURNS NULL ON NULL INPUT as '$libdir/dolphin', 'time_to_sec';
CREATE OR REPLACE FUNCTION pg_catalog.time_to_sec(date) RETURNS int4 AS $$ SELECT pg_catalog.time_to_sec(cast('00:00:00' as text)) $$ LANGUAGE SQL;
CREATE OR REPLACE FUNCTION pg_catalog.time_to_sec(numeric) RETURNS int4 AS $$ SELECT pg_catalog.time_to_sec(cast($1 as text)) $$ LANGUAGE SQL;

DROP FUNCTION IF EXISTS pg_catalog.date_cast_timestamptz(date);
DROP FUNCTION IF EXISTS pg_catalog.date_cast_datetime(date);

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
DROP OPERATOR IF EXISTS dolphin_catalog.~(NONE, int1);
DROP FUNCTION IF EXISTS dolphin_catalog.dolphin_int1not(int1);

DROP OPERATOR IF EXISTS dolphin_catalog.~(NONE, int2);
DROP FUNCTION IF EXISTS dolphin_catalog.dolphin_int2not(int2);

DROP OPERATOR IF EXISTS dolphin_catalog.~(NONE, int4);
DROP FUNCTION IF EXISTS dolphin_catalog.dolphin_int4not(int4);

DROP OPERATOR IF EXISTS dolphin_catalog.~(NONE, int8);
DROP FUNCTION IF EXISTS dolphin_catalog.dolphin_int8not(int8);

DROP OPERATOR IF EXISTS dolphin_catalog.~(NONE, uint1);
DROP FUNCTION IF EXISTS dolphin_catalog.dolphin_uint1not(uint1);

DROP OPERATOR IF EXISTS dolphin_catalog.~(NONE, uint2);
DROP FUNCTION IF EXISTS dolphin_catalog.dolphin_uint2not(uint2);

DROP OPERATOR IF EXISTS dolphin_catalog.~(NONE, uint4);
DROP FUNCTION IF EXISTS dolphin_catalog.dolphin_uint4not(uint4);

DROP OPERATOR IF EXISTS dolphin_catalog.~(NONE, uint8);
DROP FUNCTION IF EXISTS dolphin_catalog.dolphin_uint8not(uint8);

DROP OPERATOR IF EXISTS dolphin_catalog.~(NONE, float4);
DROP FUNCTION IF EXISTS dolphin_catalog.dolphin_float4not(float4);

DROP OPERATOR IF EXISTS dolphin_catalog.~(NONE, float8);
DROP FUNCTION IF EXISTS dolphin_catalog.dolphin_float8not(float8);

DROP OPERATOR IF EXISTS dolphin_catalog.~(NONE, numeric);
DROP FUNCTION IF EXISTS dolphin_catalog.dolphin_numericnot(numeric);

DROP OPERATOR IF EXISTS dolphin_catalog.~(NONE, bit);
DROP FUNCTION IF EXISTS dolphin_catalog.dolphin_bitnot(bit);

DROP OPERATOR IF EXISTS dolphin_catalog.~(NONE, boolean);
DROP FUNCTION IF EXISTS dolphin_catalog.dolphin_boolnot(boolean);

DROP OPERATOR IF EXISTS dolphin_catalog.~(NONE, date);
DROP FUNCTION IF EXISTS dolphin_catalog.dolphin_datenot(date);

DROP OPERATOR IF EXISTS dolphin_catalog.~(NONE, time);
DROP FUNCTION IF EXISTS dolphin_catalog.dolphin_timenot(time);

DROP OPERATOR IF EXISTS dolphin_catalog.~(NONE, timestamp without time zone);
DROP FUNCTION IF EXISTS dolphin_catalog.dolphin_timestampnot(timestamp without time zone);

DROP OPERATOR IF EXISTS dolphin_catalog.~(NONE, timestamp with time zone);
DROP FUNCTION IF EXISTS dolphin_catalog.dolphin_timestamptznot(timestamp with time zone);

DROP OPERATOR IF EXISTS dolphin_catalog.~(NONE, year);
DROP FUNCTION IF EXISTS dolphin_catalog.dolphin_yearnot(year);

DROP OPERATOR IF EXISTS dolphin_catalog.~(NONE, char);
DROP FUNCTION IF EXISTS dolphin_catalog.dolphin_charnot(char);

DROP OPERATOR IF EXISTS dolphin_catalog.~(NONE, varchar);
DROP FUNCTION IF EXISTS dolphin_catalog.dolphin_varcharnot(varchar);

DROP OPERATOR IF EXISTS dolphin_catalog.~(NONE, text);
DROP FUNCTION IF EXISTS dolphin_catalog.dolphin_textnot(text);

DROP OPERATOR IF EXISTS dolphin_catalog.~(NONE, anyelement);
DROP FUNCTION IF EXISTS dolphin_catalog.dolphin_varlenanot(anyelement);

DROP OPERATOR IF EXISTS dolphin_catalog.~(NONE, anyenum);
DROP FUNCTION IF EXISTS dolphin_catalog.dolphin_enumnot(anyenum);

DROP OPERATOR IF EXISTS dolphin_catalog.~(NONE, anyset);
DROP FUNCTION IF EXISTS dolphin_catalog.dolphin_setnot(anyset);

DROP FUNCTION IF EXISTS pg_catalog.convert(boolean, name);
DROP FUNCTION IF EXISTS pg_catalog.convert(longblob, name);
DROP FUNCTION IF EXISTS pg_catalog.convert(anyenum, name);
DROP FUNCTION IF EXISTS pg_catalog.convert(json, name);

DROP OPERATOR CLASS IF EXISTS pg_catalog.varbinary_ops USING BTREE;
DROP OPERATOR CLASS IF EXISTS pg_catalog.varbinary_ops USING HASH;
DROP OPERATOR CLASS IF EXISTS pg_catalog.binary_ops USING BTREE;
DROP OPERATOR CLASS IF EXISTS pg_catalog.binary_ops USING HASH;
DROP OPERATOR FAMILY IF EXISTS pg_catalog.varbinary_ops USING BTREE;
DROP OPERATOR FAMILY IF EXISTS pg_catalog.varbinary_ops USING HASH;
DROP OPERATOR FAMILY IF EXISTS pg_catalog.binary_ops USING BTREE;
DROP OPERATOR FAMILY IF EXISTS pg_catalog.binary_ops USING HASH;
DROP FUNCTION IF EXISTS pg_catalog.varbinary_cmp(varbinary, varbinary);
DROP FUNCTION IF EXISTS pg_catalog.binary_cmp(binary, binary);

DROP FUNCTION IF EXISTS pg_catalog.degrees(boolean);
DROP FUNCTION IF EXISTS pg_catalog.degrees(year);
DROP FUNCTION IF EXISTS pg_catalog.degrees(json);
DROP FUNCTION IF EXISTS pg_catalog.acos(boolean);
DROP FUNCTION IF EXISTS pg_catalog.acos(year);
DROP FUNCTION IF EXISTS pg_catalog.acos(json);

DROP FUNCTION IF EXISTS pg_catalog.inet_ntoa(bit);
DROP FUNCTION IF EXISTS pg_catalog.inet_ntoa(binary);
DROP FUNCTION IF EXISTS pg_catalog.inet_ntoa(tinyblob);
DROP FUNCTION IF EXISTS pg_catalog.inet_ntoa(nvarchar2);
DROP FUNCTION IF EXISTS pg_catalog.inet_ntoa(year);
DROP FUNCTION IF EXISTS pg_catalog.inet_ntoa(json);

DROP FUNCTION IF EXISTS pg_catalog.exp(year);
DROP FUNCTION IF EXISTS pg_catalog.exp(json);

DROP FUNCTION IF EXISTS pg_catalog.int8_cast_date(int1);
DROP FUNCTION IF EXISTS pg_catalog.int16_cast_date(int2);
DROP FUNCTION IF EXISTS pg_catalog.int32_cast_date(int4);
DROP FUNCTION IF EXISTS pg_catalog.int64_cast_date(int8);
DROP FUNCTION IF EXISTS pg_catalog.uint8_cast_date(uint1);
DROP FUNCTION IF EXISTS pg_catalog.uint16_cast_date(uint2);
DROP FUNCTION IF EXISTS pg_catalog.uint32_cast_date(uint4);
DROP FUNCTION IF EXISTS pg_catalog.uint64_cast_date(uint8);
DROP FUNCTION IF EXISTS pg_catalog.float4_cast_date(float4);
DROP FUNCTION IF EXISTS pg_catalog.float8_cast_date(float8);
DROP FUNCTION IF EXISTS pg_catalog.numeric_cast_date(numeric);
DROP FUNCTION IF EXISTS pg_catalog.text_date_explicit(TEXT);

DROP FUNCTION IF EXISTS pg_catalog.int8_cast_datetime(int1);
DROP FUNCTION IF EXISTS pg_catalog.int16_cast_datetime(int2);
DROP FUNCTION IF EXISTS pg_catalog.int32_cast_datetime(int4);
DROP FUNCTION IF EXISTS pg_catalog.int64_cast_datetime(int8);
DROP FUNCTION IF EXISTS pg_catalog.uint8_cast_datetime(uint1);
DROP FUNCTION IF EXISTS pg_catalog.uint16_cast_datetime(uint2);
DROP FUNCTION IF EXISTS pg_catalog.uint32_cast_datetime(uint4);
DROP FUNCTION IF EXISTS pg_catalog.uint64_cast_datetime(uint8);
DROP FUNCTION IF EXISTS pg_catalog.float4_cast_datetime(float4);
DROP FUNCTION IF EXISTS pg_catalog.float8_cast_datetime(float8);
DROP FUNCTION IF EXISTS pg_catalog.numeric_cast_datetime(numeric);
DROP FUNCTION IF EXISTS pg_catalog.timestamp_explicit(TEXT);

DROP FUNCTION IF EXISTS pg_catalog.int8_cast_timestamptz(int1);
DROP FUNCTION IF EXISTS pg_catalog.int16_cast_timestamptz(int2);
DROP FUNCTION IF EXISTS pg_catalog.int32_cast_timestamptz(int4);
DROP FUNCTION IF EXISTS pg_catalog.int64_cast_timestamptz(int8);
DROP FUNCTION IF EXISTS pg_catalog.uint8_cast_timestamptz(uint1);
DROP FUNCTION IF EXISTS pg_catalog.uint16_cast_timestamptz(uint2);
DROP FUNCTION IF EXISTS pg_catalog.uint32_cast_timestamptz(uint4);
DROP FUNCTION IF EXISTS pg_catalog.uint64_cast_timestamptz(uint8);
DROP FUNCTION IF EXISTS pg_catalog.float4_cast_timestamptz(float4);
DROP FUNCTION IF EXISTS pg_catalog.float8_cast_timestamptz(float8);
DROP FUNCTION IF EXISTS pg_catalog.numeric_cast_timestamptz(numeric);
DROP CAST IF EXISTS (TEXT AS timestamp with time zone);
DROP FUNCTION IF EXISTS pg_catalog.timestamptz_explicit(TEXT);

DROP FUNCTION IF EXISTS pg_catalog.ln(year);
DROP FUNCTION IF EXISTS pg_catalog.ln(json);

DROP FUNCTION IF EXISTS pg_catalog.log(anyelement,anyelement);

DROP FUNCTION IF EXISTS pg_catalog.log(year);
DROP FUNCTION IF EXISTS pg_catalog.log(json);

DROP FUNCTION IF EXISTS pg_catalog.varlena_cast_int1(anyelement) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.varlena_cast_int2(anyelement) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.varlena_cast_int4(anyelement) CASCADE;

DROP FUNCTION IF EXISTS pg_catalog.str_to_date(boolean, TEXT);
DROP FUNCTION IF EXISTS pg_catalog.str_to_date(longblob, TEXT);
DROP FUNCTION IF EXISTS pg_catalog.str_to_date(anyenum, TEXT);
DROP FUNCTION IF EXISTS pg_catalog.str_to_date(json, TEXT);
DROP OPERATOR CLASS IF EXISTS pg_catalog.tinyblob_ops USING BTREE;
DROP OPERATOR CLASS IF EXISTS pg_catalog.tinyblob_ops USING HASH;
DROP OPERATOR CLASS IF EXISTS pg_catalog.blob_ops USING BTREE;
DROP OPERATOR CLASS IF EXISTS pg_catalog.blob_ops USING HASH;
DROP OPERATOR CLASS IF EXISTS pg_catalog.mediumblob_ops USING BTREE;
DROP OPERATOR CLASS IF EXISTS pg_catalog.mediumblob_ops USING HASH;
DROP OPERATOR CLASS IF EXISTS pg_catalog.longblob_ops USING BTREE;
DROP OPERATOR CLASS IF EXISTS pg_catalog.longblob_ops USING HASH;
DROP OPERATOR FAMILY IF EXISTS pg_catalog.tinyblob_ops USING BTREE;
DROP OPERATOR FAMILY IF EXISTS pg_catalog.tinyblob_ops USING HASH;
DROP OPERATOR FAMILY IF EXISTS pg_catalog.blob_ops USING BTREE;
DROP OPERATOR FAMILY IF EXISTS pg_catalog.blob_ops USING HASH;
DROP OPERATOR FAMILY IF EXISTS pg_catalog.mediumblob_ops USING BTREE;
DROP OPERATOR FAMILY IF EXISTS pg_catalog.mediumblob_ops USING HASH;
DROP OPERATOR FAMILY IF EXISTS pg_catalog.longblob_ops USING BTREE;
DROP OPERATOR FAMILY IF EXISTS pg_catalog.longblob_ops USING HASH;

DROP OPERATOR IF EXISTS pg_catalog.=(tinyblob, tinyblob);
DROP OPERATOR IF EXISTS pg_catalog.<>(tinyblob, tinyblob);
DROP OPERATOR IF EXISTS pg_catalog.>(tinyblob, tinyblob);
DROP OPERATOR IF EXISTS pg_catalog.>=(tinyblob, tinyblob);
DROP OPERATOR IF EXISTS pg_catalog.<(tinyblob, tinyblob);
DROP OPERATOR IF EXISTS pg_catalog.<=(tinyblob, tinyblob);
DROP OPERATOR IF EXISTS pg_catalog.=(tinyblob, blob);
DROP OPERATOR IF EXISTS pg_catalog.<>(tinyblob, blob);
DROP OPERATOR IF EXISTS pg_catalog.>(tinyblob, blob);
DROP OPERATOR IF EXISTS pg_catalog.>=(tinyblob, blob);
DROP OPERATOR IF EXISTS pg_catalog.<(tinyblob, blob);
DROP OPERATOR IF EXISTS pg_catalog.<=(tinyblob, blob);
DROP OPERATOR IF EXISTS pg_catalog.=(tinyblob, mediumblob);
DROP OPERATOR IF EXISTS pg_catalog.<>(tinyblob, mediumblob);
DROP OPERATOR IF EXISTS pg_catalog.>(tinyblob, mediumblob);
DROP OPERATOR IF EXISTS pg_catalog.>=(tinyblob, mediumblob);
DROP OPERATOR IF EXISTS pg_catalog.<(tinyblob, mediumblob);
DROP OPERATOR IF EXISTS pg_catalog.<=(tinyblob, mediumblob);
DROP OPERATOR IF EXISTS pg_catalog.=(tinyblob, longblob);
DROP OPERATOR IF EXISTS pg_catalog.<>(tinyblob, longblob);
DROP OPERATOR IF EXISTS pg_catalog.>(tinyblob, longblob);
DROP OPERATOR IF EXISTS pg_catalog.>=(tinyblob, longblob);
DROP OPERATOR IF EXISTS pg_catalog.<(tinyblob, longblob);
DROP OPERATOR IF EXISTS pg_catalog.<=(tinyblob, longblob);
DROP OPERATOR IF EXISTS pg_catalog.=(tinyblob, text);
DROP OPERATOR IF EXISTS pg_catalog.<>(tinyblob, text);
DROP OPERATOR IF EXISTS pg_catalog.>(tinyblob, text);
DROP OPERATOR IF EXISTS pg_catalog.>=(tinyblob, text);
DROP OPERATOR IF EXISTS pg_catalog.<(tinyblob, text);
DROP OPERATOR IF EXISTS pg_catalog.<=(tinyblob, text);
DROP OPERATOR IF EXISTS pg_catalog.=(text, tinyblob);
DROP OPERATOR IF EXISTS pg_catalog.<>(text, tinyblob);
DROP OPERATOR IF EXISTS pg_catalog.>(text, tinyblob);
DROP OPERATOR IF EXISTS pg_catalog.>=(text, tinyblob);
DROP OPERATOR IF EXISTS pg_catalog.<(text, tinyblob);
DROP OPERATOR IF EXISTS pg_catalog.<=(text, tinyblob);

DROP OPERATOR IF EXISTS pg_catalog.=(blob, tinyblob);
DROP OPERATOR IF EXISTS pg_catalog.<>(blob, tinyblob);
DROP OPERATOR IF EXISTS pg_catalog.>(blob, tinyblob);
DROP OPERATOR IF EXISTS pg_catalog.>=(blob, tinyblob);
DROP OPERATOR IF EXISTS pg_catalog.<(blob, tinyblob);
DROP OPERATOR IF EXISTS pg_catalog.<=(blob, tinyblob);
DROP OPERATOR IF EXISTS pg_catalog.=(blob, mediumblob);
DROP OPERATOR IF EXISTS pg_catalog.<>(blob, mediumblob);
DROP OPERATOR IF EXISTS pg_catalog.>(blob, mediumblob);
DROP OPERATOR IF EXISTS pg_catalog.>=(blob, mediumblob);
DROP OPERATOR IF EXISTS pg_catalog.<(blob, mediumblob);
DROP OPERATOR IF EXISTS pg_catalog.<=(blob, mediumblob);
DROP OPERATOR IF EXISTS pg_catalog.=(blob, longblob);
DROP OPERATOR IF EXISTS pg_catalog.<>(blob, longblob);
DROP OPERATOR IF EXISTS pg_catalog.>(blob, longblob);
DROP OPERATOR IF EXISTS pg_catalog.>=(blob, longblob);
DROP OPERATOR IF EXISTS pg_catalog.<(blob, longblob);
DROP OPERATOR IF EXISTS pg_catalog.<=(blob, longblob);

DROP OPERATOR IF EXISTS pg_catalog.=(mediumblob, tinyblob);
DROP OPERATOR IF EXISTS pg_catalog.<>(mediumblob, tinyblob);
DROP OPERATOR IF EXISTS pg_catalog.>(mediumblob, tinyblob);
DROP OPERATOR IF EXISTS pg_catalog.>=(mediumblob, tinyblob);
DROP OPERATOR IF EXISTS pg_catalog.<(mediumblob, tinyblob);
DROP OPERATOR IF EXISTS pg_catalog.<=(mediumblob, tinyblob);
DROP OPERATOR IF EXISTS pg_catalog.=(mediumblob, blob);
DROP OPERATOR IF EXISTS pg_catalog.<>(mediumblob, blob);
DROP OPERATOR IF EXISTS pg_catalog.>(mediumblob, blob);
DROP OPERATOR IF EXISTS pg_catalog.>=(mediumblob, blob);
DROP OPERATOR IF EXISTS pg_catalog.<(mediumblob, blob);
DROP OPERATOR IF EXISTS pg_catalog.<=(mediumblob, blob);
DROP OPERATOR IF EXISTS pg_catalog.=(mediumblob, mediumblob);
DROP OPERATOR IF EXISTS pg_catalog.<>(mediumblob, mediumblob);
DROP OPERATOR IF EXISTS pg_catalog.>(mediumblob, mediumblob);
DROP OPERATOR IF EXISTS pg_catalog.>=(mediumblob, mediumblob);
DROP OPERATOR IF EXISTS pg_catalog.<(mediumblob, mediumblob);
DROP OPERATOR IF EXISTS pg_catalog.<=(mediumblob, mediumblob);
DROP OPERATOR IF EXISTS pg_catalog.=(mediumblob, longblob);
DROP OPERATOR IF EXISTS pg_catalog.<>(mediumblob, longblob);
DROP OPERATOR IF EXISTS pg_catalog.>(mediumblob, longblob);
DROP OPERATOR IF EXISTS pg_catalog.>=(mediumblob, longblob);
DROP OPERATOR IF EXISTS pg_catalog.<(mediumblob, longblob);
DROP OPERATOR IF EXISTS pg_catalog.<=(mediumblob, longblob);
DROP OPERATOR IF EXISTS pg_catalog.=(mediumblob, text);
DROP OPERATOR IF EXISTS pg_catalog.<>(mediumblob, text);
DROP OPERATOR IF EXISTS pg_catalog.>(mediumblob, text);
DROP OPERATOR IF EXISTS pg_catalog.>=(mediumblob, text);
DROP OPERATOR IF EXISTS pg_catalog.<(mediumblob, text);
DROP OPERATOR IF EXISTS pg_catalog.<=(mediumblob, text);
DROP OPERATOR IF EXISTS pg_catalog.=(text, mediumblob);
DROP OPERATOR IF EXISTS pg_catalog.<>(text, mediumblob);
DROP OPERATOR IF EXISTS pg_catalog.>(text, mediumblob);
DROP OPERATOR IF EXISTS pg_catalog.>=(text, mediumblob);
DROP OPERATOR IF EXISTS pg_catalog.<(text, mediumblob);
DROP OPERATOR IF EXISTS pg_catalog.<=(text, mediumblob);

DROP OPERATOR IF EXISTS pg_catalog.=(longblob, tinyblob);
DROP OPERATOR IF EXISTS pg_catalog.<>(longblob, tinyblob);
DROP OPERATOR IF EXISTS pg_catalog.>(longblob, tinyblob);
DROP OPERATOR IF EXISTS pg_catalog.>=(longblob, tinyblob);
DROP OPERATOR IF EXISTS pg_catalog.<(longblob, tinyblob);
DROP OPERATOR IF EXISTS pg_catalog.<=(longblob, tinyblob);
DROP OPERATOR IF EXISTS pg_catalog.=(longblob, blob);
DROP OPERATOR IF EXISTS pg_catalog.<>(longblob, blob);
DROP OPERATOR IF EXISTS pg_catalog.>(longblob, blob);
DROP OPERATOR IF EXISTS pg_catalog.>=(longblob, blob);
DROP OPERATOR IF EXISTS pg_catalog.<(longblob, blob);
DROP OPERATOR IF EXISTS pg_catalog.<=(longblob, blob);
DROP OPERATOR IF EXISTS pg_catalog.=(longblob, mediumblob);
DROP OPERATOR IF EXISTS pg_catalog.<>(longblob, mediumblob);
DROP OPERATOR IF EXISTS pg_catalog.>(longblob, mediumblob);
DROP OPERATOR IF EXISTS pg_catalog.>=(longblob, mediumblob);
DROP OPERATOR IF EXISTS pg_catalog.<(longblob, mediumblob);
DROP OPERATOR IF EXISTS pg_catalog.<=(longblob, mediumblob);
DROP OPERATOR IF EXISTS pg_catalog.=(longblob, longblob);
DROP OPERATOR IF EXISTS pg_catalog.<>(longblob, longblob);
DROP OPERATOR IF EXISTS pg_catalog.>(longblob, longblob);
DROP OPERATOR IF EXISTS pg_catalog.>=(longblob, longblob);
DROP OPERATOR IF EXISTS pg_catalog.<(longblob, longblob);
DROP OPERATOR IF EXISTS pg_catalog.<=(longblob, longblob);
DROP OPERATOR IF EXISTS pg_catalog.=(longblob, text);
DROP OPERATOR IF EXISTS pg_catalog.<>(longblob, text);
DROP OPERATOR IF EXISTS pg_catalog.>(longblob, text);
DROP OPERATOR IF EXISTS pg_catalog.>=(longblob, text);
DROP OPERATOR IF EXISTS pg_catalog.<(longblob, text);
DROP OPERATOR IF EXISTS pg_catalog.<=(longblob, text);
DROP OPERATOR IF EXISTS pg_catalog.=(text, longblob);
DROP OPERATOR IF EXISTS pg_catalog.<>(text, longblob);
DROP OPERATOR IF EXISTS pg_catalog.>(text, longblob);
DROP OPERATOR IF EXISTS pg_catalog.>=(text, longblob);
DROP OPERATOR IF EXISTS pg_catalog.<(text, longblob);
DROP OPERATOR IF EXISTS pg_catalog.<=(text, longblob);

DROP FUNCTION IF EXISTS pg_catalog.blob_cmp(blob, blob);

DROP FUNCTION IF EXISTS pg_catalog.tinyblob_eq(arg1 tinyblob, arg2 tinyblob);
DROP FUNCTION IF EXISTS pg_catalog.tinyblob_ne(arg1 tinyblob, arg2 tinyblob);
DROP FUNCTION IF EXISTS pg_catalog.tinyblob_lt(arg1 tinyblob, arg2 tinyblob);
DROP FUNCTION IF EXISTS pg_catalog.tinyblob_le(arg1 tinyblob, arg2 tinyblob);
DROP FUNCTION IF EXISTS pg_catalog.tinyblob_gt(arg1 tinyblob, arg2 tinyblob);
DROP FUNCTION IF EXISTS pg_catalog.tinyblob_ge(arg1 tinyblob, arg2 tinyblob);
DROP FUNCTION IF EXISTS pg_catalog.tinyblob_cmp(tinyblob, tinyblob);

DROP FUNCTION IF EXISTS pg_catalog.tinyblob_eq_text(arg1 tinyblob, arg2 text);
DROP FUNCTION IF EXISTS pg_catalog.tinyblob_ne_text(arg1 tinyblob, arg2 text);
DROP FUNCTION IF EXISTS pg_catalog.tinyblob_lt_text(arg1 tinyblob, arg2 text);
DROP FUNCTION IF EXISTS pg_catalog.tinyblob_le_text(arg1 tinyblob, arg2 text);
DROP FUNCTION IF EXISTS pg_catalog.tinyblob_gt_text(arg1 tinyblob, arg2 text);
DROP FUNCTION IF EXISTS pg_catalog.tinyblob_ge_text(arg1 tinyblob, arg2 text);

DROP FUNCTION IF EXISTS pg_catalog.text_eq_tinyblob(arg1 text, arg2 tinyblob);
DROP FUNCTION IF EXISTS pg_catalog.text_ne_tinyblob(arg1 text, arg2 tinyblob);
DROP FUNCTION IF EXISTS pg_catalog.text_lt_tinyblob(arg1 text, arg2 tinyblob);
DROP FUNCTION IF EXISTS pg_catalog.text_le_tinyblob(arg1 text, arg2 tinyblob);
DROP FUNCTION IF EXISTS pg_catalog.test_gt_tinyblob(arg1 text, arg2 tinyblob);
DROP FUNCTION IF EXISTS pg_catalog.test_ge_tinyblob(arg1 text, arg2 tinyblob);

DROP FUNCTION IF EXISTS pg_catalog.mediumblob_eq(arg1 mediumblob, arg2 mediumblob);
DROP FUNCTION IF EXISTS pg_catalog.mediumblob_ne(arg1 mediumblob, arg2 mediumblob);
DROP FUNCTION IF EXISTS pg_catalog.mediumblob_lt(arg1 mediumblob, arg2 mediumblob);
DROP FUNCTION IF EXISTS pg_catalog.mediumblob_le(arg1 mediumblob, arg2 mediumblob);
DROP FUNCTION IF EXISTS pg_catalog.mediumblob_gt(arg1 mediumblob, arg2 mediumblob);
DROP FUNCTION IF EXISTS pg_catalog.mediumblob_ge(arg1 mediumblob, arg2 mediumblob);
DROP FUNCTION IF EXISTS pg_catalog.mediumblob_cmp(mediumblob, mediumblob);

DROP FUNCTION IF EXISTS pg_catalog.tinyblob_eq_text(arg1 tinyblob, arg2 text);
DROP FUNCTION IF EXISTS pg_catalog.tinyblob_ne_text(arg1 tinyblob, arg2 text);
DROP FUNCTION IF EXISTS pg_catalog.tinyblob_lt_text(arg1 tinyblob, arg2 text);
DROP FUNCTION IF EXISTS pg_catalog.tinyblob_le_text(arg1 tinyblob, arg2 text);
DROP FUNCTION IF EXISTS pg_catalog.tinyblob_gt_text(arg1 tinyblob, arg2 text);
DROP FUNCTION IF EXISTS pg_catalog.tinyblob_ge_text(arg1 tinyblob, arg2 text);

DROP FUNCTION IF EXISTS pg_catalog.text_eq_tinyblob(arg1 text, arg2 tinyblob);
DROP FUNCTION IF EXISTS pg_catalog.text_ne_tinyblob(arg1 text, arg2 tinyblob);
DROP FUNCTION IF EXISTS pg_catalog.text_lt_tinyblob(arg1 text, arg2 tinyblob);
DROP FUNCTION IF EXISTS pg_catalog.text_le_tinyblob(arg1 text, arg2 tinyblob);
DROP FUNCTION IF EXISTS pg_catalog.test_gt_tinyblob(arg1 text, arg2 tinyblob);
DROP FUNCTION IF EXISTS pg_catalog.test_ge_tinyblob(arg1 text, arg2 tinyblob);

DROP FUNCTION IF EXISTS pg_catalog.mediumblob_eq(arg1 mediumblob, arg2 mediumblob);
DROP FUNCTION IF EXISTS pg_catalog.mediumblob_ne(arg1 mediumblob, arg2 mediumblob);
DROP FUNCTION IF EXISTS pg_catalog.mediumblob_lt(arg1 mediumblob, arg2 mediumblob);
DROP FUNCTION IF EXISTS pg_catalog.mediumblob_le(arg1 mediumblob, arg2 mediumblob);
DROP FUNCTION IF EXISTS pg_catalog.mediumblob_gt(arg1 mediumblob, arg2 mediumblob);
DROP FUNCTION IF EXISTS pg_catalog.mediumblob_ge(arg1 mediumblob, arg2 mediumblob);
DROP FUNCTION IF EXISTS pg_catalog.mediumblob_cmp(mediumblob, mediumblob);

DROP FUNCTION IF EXISTS pg_catalog.mediumblob_eq_text(arg1 mediumblob, arg2 text);
DROP FUNCTION IF EXISTS pg_catalog.mediumblob_ne_text(arg1 mediumblob, arg2 text);
DROP FUNCTION IF EXISTS pg_catalog.mediumblob_lt_text(arg1 mediumblob, arg2 text);
DROP FUNCTION IF EXISTS pg_catalog.mediumblob_le_text(arg1 mediumblob, arg2 text);
DROP FUNCTION IF EXISTS pg_catalog.mediumblob_gt_text(arg1 mediumblob, arg2 text);
DROP FUNCTION IF EXISTS pg_catalog.mediumblob_ge_text(arg1 mediumblob, arg2 text);

DROP FUNCTION IF EXISTS pg_catalog.text_eq_mediumblob(arg1 text, arg2 mediumblob);
DROP FUNCTION IF EXISTS pg_catalog.text_ne_mediumblob(arg1 text, arg2 mediumblob);
DROP FUNCTION IF EXISTS pg_catalog.text_lt_mediumblob(arg1 text, arg2 mediumblob);
DROP FUNCTION IF EXISTS pg_catalog.text_le_mediumblob(arg1 text, arg2 mediumblob);
DROP FUNCTION IF EXISTS pg_catalog.test_gt_mediumblob(arg1 text, arg2 mediumblob);
DROP FUNCTION IF EXISTS pg_catalog.test_ge_mediumblob(arg1 text, arg2 mediumblob);

DROP FUNCTION IF EXISTS pg_catalog.longblob_eq(arg1 longblob, arg2 longblob);
DROP FUNCTION IF EXISTS pg_catalog.longblob_ne(arg1 longblob, arg2 longblob);
DROP FUNCTION IF EXISTS pg_catalog.longblob_lt(arg1 longblob, arg2 longblob);
DROP FUNCTION IF EXISTS pg_catalog.longblob_le(arg1 longblob, arg2 longblob);
DROP FUNCTION IF EXISTS pg_catalog.longblob_gt(arg1 longblob, arg2 longblob);
DROP FUNCTION IF EXISTS pg_catalog.longblob_ge(arg1 longblob, arg2 longblob);
DROP FUNCTION IF EXISTS pg_catalog.longblob_cmp(longblob, longblob);

DROP FUNCTION IF EXISTS pg_catalog.longblob_eq_text(arg1 longblob, arg2 text);
DROP FUNCTION IF EXISTS pg_catalog.longblob_ne_text(arg1 longblob, arg2 text);
DROP FUNCTION IF EXISTS pg_catalog.longblob_lt_text(arg1 longblob, arg2 text);
DROP FUNCTION IF EXISTS pg_catalog.longblob_le_text(arg1 longblob, arg2 text);
DROP FUNCTION IF EXISTS pg_catalog.longblob_gt_text(arg1 longblob, arg2 text);
DROP FUNCTION IF EXISTS pg_catalog.longblob_ge_text(arg1 longblob, arg2 text);

DROP FUNCTION IF EXISTS pg_catalog.text_eq_longblob(arg1 text, arg2 longblob);
DROP FUNCTION IF EXISTS pg_catalog.text_ne_longblob(arg1 text, arg2 longblob);
DROP FUNCTION IF EXISTS pg_catalog.text_lt_longblob(arg1 text, arg2 longblob);
DROP FUNCTION IF EXISTS pg_catalog.text_le_longblob(arg1 text, arg2 longblob);
DROP FUNCTION IF EXISTS pg_catalog.test_gt_longblob(arg1 text, arg2 longblob);
DROP FUNCTION IF EXISTS pg_catalog.test_ge_longblob(arg1 text, arg2 longblob);

DROP FUNCTION IF EXISTS pg_catalog.tinyblob_blob_eq(arg1 tinyblob, arg2 blob);
DROP FUNCTION IF EXISTS pg_catalog.tinyblob_blob_ne(arg1 tinyblob, arg2 blob);
DROP FUNCTION IF EXISTS pg_catalog.tinyblob_blob_lt(arg1 tinyblob, arg2 blob);
DROP FUNCTION IF EXISTS pg_catalog.tinyblob_blob_le(arg1 tinyblob, arg2 blob);
DROP FUNCTION IF EXISTS pg_catalog.tinyblob_blob_gt(arg1 tinyblob, arg2 blob);
DROP FUNCTION IF EXISTS pg_catalog.tinyblob_blob_ge(arg1 tinyblob, arg2 blob);
DROP FUNCTION IF EXISTS pg_catalog.tinyblob_blob_cmp(tinyblob, blob);

DROP FUNCTION IF EXISTS pg_catalog.tinyblob_mediumblob_eq(arg1 tinyblob, arg2 mediumblob);
DROP FUNCTION IF EXISTS pg_catalog.tinyblob_mediumblob_ne(arg1 tinyblob, arg2 mediumblob);
DROP FUNCTION IF EXISTS pg_catalog.tinyblob_mediumblob_lt(arg1 tinyblob, arg2 mediumblob);
DROP FUNCTION IF EXISTS pg_catalog.tinyblob_mediumblob_le(arg1 tinyblob, arg2 mediumblob);
DROP FUNCTION IF EXISTS pg_catalog.tinyblob_mediumblob_gt(arg1 tinyblob, arg2 mediumblob);
DROP FUNCTION IF EXISTS pg_catalog.tinyblob_mediumblob_ge(arg1 tinyblob, arg2 mediumblob);
DROP FUNCTION IF EXISTS pg_catalog.tinyblob_mediumblob_cmp(tinyblob, mediumblob);

DROP FUNCTION IF EXISTS pg_catalog.tinyblob_longblob_eq(arg1 tinyblob, arg2 longblob);
DROP FUNCTION IF EXISTS pg_catalog.tinyblob_longblob_ne(arg1 tinyblob, arg2 longblob);
DROP FUNCTION IF EXISTS pg_catalog.tinyblob_longblob_lt(arg1 tinyblob, arg2 longblob);
DROP FUNCTION IF EXISTS pg_catalog.tinyblob_longblob_le(arg1 tinyblob, arg2 longblob);
DROP FUNCTION IF EXISTS pg_catalog.tinyblob_longblob_gt(arg1 tinyblob, arg2 longblob);
DROP FUNCTION IF EXISTS pg_catalog.tinyblob_longblob_ge(arg1 tinyblob, arg2 longblob);
DROP FUNCTION IF EXISTS pg_catalog.tinyblob_longblob_cmp(tinyblob, longblob);

DROP FUNCTION IF EXISTS pg_catalog.blob_tinyblob_eq(arg1 blob, arg2 tinyblob);
DROP FUNCTION IF EXISTS pg_catalog.blob_tinyblob_ne(arg1 blob, arg2 tinyblob);
DROP FUNCTION IF EXISTS pg_catalog.blob_tinyblob_lt(arg1 blob, arg2 tinyblob);
DROP FUNCTION IF EXISTS pg_catalog.blob_tinyblob_le(arg1 blob, arg2 tinyblob);
DROP FUNCTION IF EXISTS pg_catalog.blob_tinyblob_gt(arg1 blob, arg2 tinyblob);
DROP FUNCTION IF EXISTS pg_catalog.blob_tinyblob_ge(arg1 blob, arg2 tinyblob);
DROP FUNCTION IF EXISTS pg_catalog.blob_tinyblob_cmp(blob, tinyblob);

DROP FUNCTION IF EXISTS pg_catalog.blob_mediumblob_eq(arg1 blob, arg2 mediumblob);
DROP FUNCTION IF EXISTS pg_catalog.blob_mediumblob_ne(arg1 blob, arg2 mediumblob);
DROP FUNCTION IF EXISTS pg_catalog.blob_mediumblob_lt(arg1 blob, arg2 mediumblob);
DROP FUNCTION IF EXISTS pg_catalog.blob_mediumblob_le(arg1 blob, arg2 mediumblob);
DROP FUNCTION IF EXISTS pg_catalog.blob_mediumblob_gt(arg1 blob, arg2 mediumblob);
DROP FUNCTION IF EXISTS pg_catalog.blob_mediumblob_ge(arg1 blob, arg2 mediumblob);
DROP FUNCTION IF EXISTS pg_catalog.blob_mediumblob_cmp(blob, mediumblob);

DROP FUNCTION IF EXISTS pg_catalog.blob_longblob_eq(arg1 blob, arg2 longblob);
DROP FUNCTION IF EXISTS pg_catalog.blob_longblob_ne(arg1 blob, arg2 longblob);
DROP FUNCTION IF EXISTS pg_catalog.blob_longblob_lt(arg1 blob, arg2 longblob);
DROP FUNCTION IF EXISTS pg_catalog.blob_longblob_le(arg1 blob, arg2 longblob);
DROP FUNCTION IF EXISTS pg_catalog.blob_longblob_gt(arg1 blob, arg2 longblob);
DROP FUNCTION IF EXISTS pg_catalog.blob_longblob_ge(arg1 blob, arg2 longblob);
DROP FUNCTION IF EXISTS pg_catalog.blob_longblob_cmp(longblob, longblob);

DROP FUNCTION IF EXISTS pg_catalog.mediumblob_tinyblob_eq(arg1 mediumblob, arg2 tinyblob);
DROP FUNCTION IF EXISTS pg_catalog.mediumblob_tinyblob_ne(arg1 mediumblob, arg2 tinyblob);
DROP FUNCTION IF EXISTS pg_catalog.mediumblob_tinyblob_lt(arg1 mediumblob, arg2 tinyblob);
DROP FUNCTION IF EXISTS pg_catalog.mediumblob_tinyblob_le(arg1 mediumblob, arg2 tinyblob);
DROP FUNCTION IF EXISTS pg_catalog.mediumblob_tinyblob_gt(arg1 mediumblob, arg2 tinyblob);
DROP FUNCTION IF EXISTS pg_catalog.mediumblob_tinyblob_ge(arg1 mediumblob, arg2 tinyblob);
DROP FUNCTION IF EXISTS pg_catalog.mediumblob_tinyblob_cmp(mediumblob, tinyblob);

DROP FUNCTION IF EXISTS pg_catalog.mediumblob_blob_eq(arg1 mediumblob, arg2 blob);
DROP FUNCTION IF EXISTS pg_catalog.mediumblob_blob_ne(arg1 mediumblob, arg2 blob);
DROP FUNCTION IF EXISTS pg_catalog.mediumblob_blob_lt(arg1 mediumblob, arg2 blob);
DROP FUNCTION IF EXISTS pg_catalog.mediumblob_blob_le(arg1 mediumblob, arg2 blob);
DROP FUNCTION IF EXISTS pg_catalog.mediumblob_blob_gt(arg1 mediumblob, arg2 blob);
DROP FUNCTION IF EXISTS pg_catalog.mediumblob_blob_ge(arg1 mediumblob, arg2 blob);
DROP FUNCTION IF EXISTS pg_catalog.mediumblob_blob_cmp(mediumblob, blob);

DROP FUNCTION IF EXISTS pg_catalog.mediumblob_longblob_eq(arg1 mediumblob, arg2 longblob);
DROP FUNCTION IF EXISTS pg_catalog.mediumblob_longblob_ne(arg1 mediumblob, arg2 longblob);
DROP FUNCTION IF EXISTS pg_catalog.mediumblob_longblob_lt(arg1 mediumblob, arg2 longblob);
DROP FUNCTION IF EXISTS pg_catalog.mediumblob_longblob_le(arg1 mediumblob, arg2 longblob);
DROP FUNCTION IF EXISTS pg_catalog.mediumblob_longblob_gt(arg1 mediumblob, arg2 longblob);
DROP FUNCTION IF EXISTS pg_catalog.mediumblob_longblob_ge(arg1 mediumblob, arg2 longblob);
DROP FUNCTION IF EXISTS pg_catalog.mediumblob_longblob_cmp(longblob, longblob);

DROP FUNCTION IF EXISTS pg_catalog.longblob_tinyblob_eq(arg1 longblob, arg2 tinyblob);
DROP FUNCTION IF EXISTS pg_catalog.longblob_tinyblob_ne(arg1 longblob, arg2 tinyblob);
DROP FUNCTION IF EXISTS pg_catalog.longblob_tinyblob_lt(arg1 longblob, arg2 tinyblob);
DROP FUNCTION IF EXISTS pg_catalog.longblob_tinyblob_le(arg1 longblob, arg2 tinyblob);
DROP FUNCTION IF EXISTS pg_catalog.longblob_tinyblob_gt(arg1 longblob, arg2 tinyblob);
DROP FUNCTION IF EXISTS pg_catalog.longblob_tinyblob_ge(arg1 longblob, arg2 tinyblob);
DROP FUNCTION IF EXISTS pg_catalog.longblob_tinyblob_cmp(longblob, tinyblob);

DROP FUNCTION IF EXISTS pg_catalog.longblob_blob_eq(arg1 longblob, arg2 blob);
DROP FUNCTION IF EXISTS pg_catalog.longblob_blob_ne(arg1 longblob, arg2 blob);
DROP FUNCTION IF EXISTS pg_catalog.longblob_blob_lt(arg1 longblob, arg2 blob);
DROP FUNCTION IF EXISTS pg_catalog.longblob_blob_le(arg1 longblob, arg2 blob);
DROP FUNCTION IF EXISTS pg_catalog.longblob_blob_gt(arg1 longblob, arg2 blob);
DROP FUNCTION IF EXISTS pg_catalog.longblob_blob_ge(arg1 longblob, arg2 blob);
DROP FUNCTION IF EXISTS pg_catalog.longblob_blob_cmp(longblob, blob);

DROP FUNCTION IF EXISTS pg_catalog.longblob_mediumblob_eq(arg1 longblob, arg2 mediumblob);
DROP FUNCTION IF EXISTS pg_catalog.longblob_mediumblob_ne(arg1 longblob, arg2 mediumblob);
DROP FUNCTION IF EXISTS pg_catalog.longblob_mediumblob_lt(arg1 longblob, arg2 mediumblob);
DROP FUNCTION IF EXISTS pg_catalog.longblob_mediumblob_le(arg1 longblob, arg2 mediumblob);
DROP FUNCTION IF EXISTS pg_catalog.longblob_mediumblob_gt(arg1 longblob, arg2 mediumblob);
DROP FUNCTION IF EXISTS pg_catalog.longblob_mediumblob_ge(arg1 longblob, arg2 mediumblob);
DROP FUNCTION IF EXISTS pg_catalog.longblob_mediumblob_cmp(longblob, longblob);

DROP FUNCTION IF EXISTS pg_catalog.floor(year);
DROP FUNCTION IF EXISTS pg_catalog.floor(json);
DROP FUNCTION IF EXISTS pg_catalog.substr(longblob, int, int);
DROP FUNCTION IF EXISTS pg_catalog.substr(longblob, int);
DROP FUNCTION IF EXISTS pg_catalog.repeat(longblob, int);

DROP CAST IF EXISTS (tinyblob AS binary) ;
DROP CAST IF EXISTS (tinyblob AS varbinary) ;
DROP CAST IF EXISTS (blob AS binary) ;
DROP CAST IF EXISTS (blob AS varbinary) ;
DROP CAST IF EXISTS (mediumblob AS binary) ;
DROP CAST IF EXISTS (mediumblob AS varbinary) ;
DROP CAST IF EXISTS (longblob AS binary) ;
DROP CAST IF EXISTS (longblob AS varbinary) ;
DROP FUNCTION IF EXISTS pg_catalog.inet_ntoa(blob) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.inet_ntoa(mediumblob) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.inet_ntoa(longblob) CASCADE;

DROP FUNCTION IF EXISTS pg_catalog.unhex (text);
DROP FUNCTION IF EXISTS pg_catalog.unhex (boolean);
DROP FUNCTION IF EXISTS pg_catalog.unhex (bytea);
DROP FUNCTION IF EXISTS pg_catalog.unhex (bit);
CREATE OR REPLACE FUNCTION pg_catalog.unhex (text)  RETURNS text LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'hex_decode_text';
CREATE OR REPLACE FUNCTION pg_catalog.unhex (boolean)  RETURNS text LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'hex_decode_bool';
CREATE OR REPLACE FUNCTION pg_catalog.unhex (bytea)  RETURNS text LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'hex_decode_bytea';
CREATE OR REPLACE FUNCTION pg_catalog.unhex (bit)  RETURNS text LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'hex_decode_bit';


DROP FUNCTION IF EXISTS pg_catalog.hour(YEAR);
DROP FUNCTION IF EXISTS pg_catalog.hour(date);
DROP FUNCTION IF EXISTS pg_catalog.hour(timetz);
DROP FUNCTION IF EXISTS pg_catalog.hour(timestamptz);
DROP FUNCTION IF EXISTS pg_catalog.hour (text);
CREATE OR REPLACE FUNCTION pg_catalog.hour (text) RETURNS float8 LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'GetHour';

DROP FUNCTION IF EXISTS pg_catalog.minute(YEAR);
DROP FUNCTION IF EXISTS pg_catalog.minute(date);
DROP FUNCTION IF EXISTS pg_catalog.minute(timetz);
DROP FUNCTION IF EXISTS pg_catalog.minute(timestamptz);
DROP FUNCTION IF EXISTS pg_catalog.minute (text);
CREATE OR REPLACE FUNCTION pg_catalog.minute (text) RETURNS float8 LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'GetMinute';

DROP FUNCTION IF EXISTS pg_catalog.second(YEAR);
DROP FUNCTION IF EXISTS pg_catalog.second(date);
DROP FUNCTION IF EXISTS pg_catalog.second(timetz);
DROP FUNCTION IF EXISTS pg_catalog.second(timestamptz);
DROP FUNCTION IF EXISTS pg_catalog.second (text);
CREATE OR REPLACE FUNCTION pg_catalog.second (text) RETURNS float8 LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'GetSecond';


DROP FUNCTION IF EXISTS pg_catalog.microsecond(date);
DROP FUNCTION IF EXISTS pg_catalog.microsecond(timetz);
DROP FUNCTION IF EXISTS pg_catalog.microsecond(timestamptz);
DROP FUNCTION IF EXISTS pg_catalog.microsecond (text);
CREATE OR REPLACE FUNCTION pg_catalog.microsecond (text) RETURNS float8 LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'GetMicrosecond';

DROP FUNCTION IF EXISTS pg_catalog.year(YEAR);
DROP FUNCTION IF EXISTS pg_catalog.year(timestamp(0) without time zone);
DROP FUNCTION IF EXISTS pg_catalog.year(text);
DROP FUNCTION IF EXISTS pg_catalog.year(bit);
DROP FUNCTION IF EXISTS pg_catalog.year(boolean);
DROP FUNCTION IF EXISTS pg_catalog.year(int4);
DROP FUNCTION IF EXISTS pg_catalog.year(longblob);
DROP FUNCTION IF EXISTS pg_catalog.year(anyenum);
DROP FUNCTION IF EXISTS pg_catalog.year(json);
DROP FUNCTION IF EXISTS pg_catalog.year(time);
CREATE FUNCTION pg_catalog.year (timestamp(0) without time zone) RETURNS float8 LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'datetime_year_part';
CREATE FUNCTION pg_catalog.year (text) RETURNS float8 LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'text_year_part';

-- repeat function support
DROP FUNCTION IF EXISTS pg_catalog.repeat(anyenum, integer);
DROP FUNCTION IF EXISTS pg_catalog.repeat(boolean, integer);
DROP FUNCTION IF EXISTS pg_catalog.repeat(tinyblob, integer);
DROP FUNCTION IF EXISTS pg_catalog.repeat(json, integer);
DROP FUNCTION IF EXISTS pg_catalog.repeat(year, integer);
DROP FUNCTION IF EXISTS pg_catalog.repeat(binary, integer);
DROP FUNCTION IF EXISTS pg_catalog.repeat(bit, integer);

do $$
begin
update pg_catalog.pg_cast set castfunc = (select oid from pg_proc where proname = 'bpchar_text'), castowner = 10 where castsource = 1042 and casttarget = 25;
update pg_catalog.pg_cast set castfunc = (select oid from pg_proc where proname = 'bpchar_text'), castowner = 10 where castsource = 1042 and casttarget = 1043;
update pg_catalog.pg_cast set castfunc = (select oid from pg_proc where proname = 'bpchar_text'), castowner = 10 where castsource = 1042 and casttarget = 3969;
end
$$;

DROP CAST IF EXISTS ("binary" AS boolean);
DROP CAST IF EXISTS ("varbinary" AS boolean);
DROP CAST IF EXISTS (blob AS boolean);
DROP CAST IF EXISTS (tinyblob AS boolean);
DROP CAST IF EXISTS (mediumblob AS boolean);
DROP CAST IF EXISTS (longblob AS boolean);
DROP CAST IF EXISTS (anyset AS boolean);
DROP FUNCTION IF EXISTS pg_catalog.any2boolean(anyelement) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.enum_boolean(anyenum) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.set_boolean(anyset) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.binary2boolean(binary) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.varbinary2boolean(varbinary) CASCADE;

DROP FUNCTION IF EXISTS pg_catalog.ascii(blob);
DROP FUNCTION IF EXISTS pg_catalog.ascii(year);
DROP FUNCTION IF EXISTS pg_catalog.ascii(json);
DROP FUNCTION IF EXISTS pg_catalog.ascii(boolean);
DROP FUNCTION IF EXISTS pg_catalog.ascii(anyenum);
DROP FUNCTION IF EXISTS pg_catalog.ascii(bit);

DROP FUNCTION IF EXISTS pg_catalog.bit_length(boolean);
DROP FUNCTION IF EXISTS pg_catalog.bit_length(year);
DROP FUNCTION IF EXISTS pg_catalog.bit_length(blob);
DROP FUNCTION IF EXISTS pg_catalog.bit_length(anyenum);
DROP FUNCTION IF EXISTS pg_catalog.bit_length(json);
DROP FUNCTION IF EXISTS pg_catalog.bit_length(binary);

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

DROP OPERATOR IF EXISTS pg_catalog.~~(varbinary, varbinary);
DROP OPERATOR IF EXISTS pg_catalog.~~(varbinary, text);
DROP OPERATOR IF EXISTS pg_catalog.~~*(varbinary, text);
DROP OPERATOR IF EXISTS pg_catalog.~~(text, varbinary);
DROP OPERATOR IF EXISTS pg_catalog.~~*(text, varbinary);
DROP FUNCTION IF EXISTS pg_catalog.varbinarylike(varbinary, varbinary);
DROP FUNCTION IF EXISTS pg_catalog.textvarbinarylike(text, varbinary);
DROP FUNCTION IF EXISTS pg_catalog.varbinarytextlike(varbinary, text);
DROP OPERATOR IF EXISTS pg_catalog.~~(blob, blob);
DROP OPERATOR IF EXISTS pg_catalog.~~*(blob, blob);
DROP FUNCTION IF EXISTS pg_catalog.bloblike(blob, blob);

DROP OPERATOR IF EXISTS pg_catalog.=(boolean, binary);
DROP OPERATOR IF EXISTS pg_catalog.=(binary, boolean);
DROP OPERATOR IF EXISTS pg_catalog.<>(boolean, binary);
DROP OPERATOR IF EXISTS pg_catalog.<>(binary, boolean);
DROP OPERATOR IF EXISTS pg_catalog.>(boolean, binary);
DROP OPERATOR IF EXISTS pg_catalog.>(binary, boolean);
DROP OPERATOR IF EXISTS pg_catalog.<(boolean, binary);
DROP OPERATOR IF EXISTS pg_catalog.<(binary, boolean);
DROP OPERATOR IF EXISTS pg_catalog.>=(boolean, binary);
DROP OPERATOR IF EXISTS pg_catalog.>=(binary, boolean);
DROP OPERATOR IF EXISTS pg_catalog.<=(boolean, binary);
DROP OPERATOR IF EXISTS pg_catalog.<=(binary, boolean);
DROP function IF EXISTS pg_catalog.boolean_binary_eq(boolean, binary);
DROP function IF EXISTS pg_catalog.binary_boolean_eq(binary, boolean);
DROP FUNCTION IF EXISTS pg_catalog.boolean_binary_ne(boolean, binary);
DROP FUNCTION IF EXISTS pg_catalog.binary_boolean_ne(binary, boolean);
DROP FUNCTION IF EXISTS pg_catalog.boolean_binary_gt(boolean, binary);
DROP FUNCTION IF EXISTS pg_catalog.binary_boolean_gt(binary, boolean);
DROP FUNCTION IF EXISTS pg_catalog.boolean_binary_lt(boolean, binary);
DROP FUNCTION IF EXISTS pg_catalog.binary_boolean_lt(binary, boolean);
DROP FUNCTION IF EXISTS pg_catalog.boolean_binary_ge(boolean, binary);
DROP FUNCTION IF EXISTS pg_catalog.binary_boolean_ge(binary, boolean);
DROP FUNCTION IF EXISTS pg_catalog.boolean_binary_le(boolean, binary);
DROP FUNCTION IF EXISTS pg_catalog.binary_boolean_le(binary, boolean);

DROP OPERATOR IF EXISTS pg_catalog.=(boolean, tinyblob);
DROP OPERATOR IF EXISTS pg_catalog.=(tinyblob, boolean);
DROP OPERATOR IF EXISTS pg_catalog.<>(boolean, tinyblob);
DROP OPERATOR IF EXISTS pg_catalog.<>(tinyblob, boolean);
DROP OPERATOR IF EXISTS pg_catalog.>(boolean, tinyblob);
DROP OPERATOR IF EXISTS pg_catalog.>(tinyblob, boolean);
DROP OPERATOR IF EXISTS pg_catalog.<(boolean, tinyblob);
DROP OPERATOR IF EXISTS pg_catalog.<(tinyblob, boolean);
DROP OPERATOR IF EXISTS pg_catalog.>=(boolean, tinyblob);
DROP OPERATOR IF EXISTS pg_catalog.>=(tinyblob, boolean);
DROP OPERATOR IF EXISTS pg_catalog.<=(boolean, tinyblob);
DROP OPERATOR IF EXISTS pg_catalog.<=(tinyblob, boolean);
DROP function IF EXISTS pg_catalog.boolean_tinyblob_eq(boolean, tinyblob);
DROP function IF EXISTS pg_catalog.tinyblob_boolean_eq(tinyblob, boolean);
DROP FUNCTION IF EXISTS pg_catalog.boolean_tinyblob_ne(boolean, tinyblob);
DROP FUNCTION IF EXISTS pg_catalog.tinyblob_boolean_ne(tinyblob, boolean);
DROP FUNCTION IF EXISTS pg_catalog.boolean_tinyblob_gt(boolean, tinyblob);
DROP FUNCTION IF EXISTS pg_catalog.tinyblob_boolean_gt(tinyblob, boolean);
DROP FUNCTION IF EXISTS pg_catalog.boolean_tinyblob_lt(boolean, tinyblob);
DROP FUNCTION IF EXISTS pg_catalog.tinyblob_boolean_lt(tinyblob, boolean);
DROP FUNCTION IF EXISTS pg_catalog.boolean_tinyblob_ge(boolean, tinyblob);
DROP FUNCTION IF EXISTS pg_catalog.tinyblob_boolean_ge(tinyblob, boolean);
DROP FUNCTION IF EXISTS pg_catalog.boolean_tinyblob_le(boolean, tinyblob);
DROP FUNCTION IF EXISTS pg_catalog.tinyblob_boolean_le(tinyblob, boolean);

DROP OPERATOR IF EXISTS pg_catalog.=(boolean, mediumblob);
DROP OPERATOR IF EXISTS pg_catalog.=(mediumblob, boolean);
DROP OPERATOR IF EXISTS pg_catalog.<>(boolean, mediumblob);
DROP OPERATOR IF EXISTS pg_catalog.<>(mediumblob, boolean);
DROP OPERATOR IF EXISTS pg_catalog.>(boolean, mediumblob);
DROP OPERATOR IF EXISTS pg_catalog.>(mediumblob, boolean);
DROP OPERATOR IF EXISTS pg_catalog.<(boolean, mediumblob);
DROP OPERATOR IF EXISTS pg_catalog.<(mediumblob, boolean);
DROP OPERATOR IF EXISTS pg_catalog.>=(boolean, mediumblob);
DROP OPERATOR IF EXISTS pg_catalog.>=(mediumblob, boolean);
DROP OPERATOR IF EXISTS pg_catalog.<=(boolean, mediumblob);
DROP OPERATOR IF EXISTS pg_catalog.<=(mediumblob, boolean);
DROP function IF EXISTS pg_catalog.boolean_mediumblob_eq(boolean, mediumblob);
DROP function IF EXISTS pg_catalog.mediumblob_boolean_eq(mediumblob, boolean);
DROP FUNCTION IF EXISTS pg_catalog.boolean_mediumblob_ne(boolean, mediumblob);
DROP FUNCTION IF EXISTS pg_catalog.mediumblob_boolean_ne(mediumblob, boolean);
DROP FUNCTION IF EXISTS pg_catalog.boolean_mediumblob_gt(boolean, mediumblob);
DROP FUNCTION IF EXISTS pg_catalog.mediumblob_boolean_gt(mediumblob, boolean);
DROP FUNCTION IF EXISTS pg_catalog.boolean_mediumblob_lt(boolean, mediumblob);
DROP FUNCTION IF EXISTS pg_catalog.mediumblob_boolean_lt(mediumblob, boolean);
DROP FUNCTION IF EXISTS pg_catalog.boolean_mediumblob_ge(boolean, mediumblob);
DROP FUNCTION IF EXISTS pg_catalog.mediumblob_boolean_ge(mediumblob, boolean);
DROP FUNCTION IF EXISTS pg_catalog.boolean_mediumblob_le(boolean, mediumblob);
DROP FUNCTION IF EXISTS pg_catalog.mediumblob_boolean_le(mediumblob, boolean);

DROP OPERATOR IF EXISTS pg_catalog.=(boolean, blob);
DROP OPERATOR IF EXISTS pg_catalog.=(blob, boolean);
DROP OPERATOR IF EXISTS pg_catalog.<>(boolean, blob);
DROP OPERATOR IF EXISTS pg_catalog.<>(blob, boolean);
DROP OPERATOR IF EXISTS pg_catalog.>(boolean, blob);
DROP OPERATOR IF EXISTS pg_catalog.>(blob, boolean);
DROP OPERATOR IF EXISTS pg_catalog.<(boolean, blob);
DROP OPERATOR IF EXISTS pg_catalog.<(blob, boolean);
DROP OPERATOR IF EXISTS pg_catalog.>=(boolean, blob);
DROP OPERATOR IF EXISTS pg_catalog.>=(blob, boolean);
DROP OPERATOR IF EXISTS pg_catalog.<=(boolean, blob);
DROP OPERATOR IF EXISTS pg_catalog.<=(blob, boolean);
DROP function IF EXISTS pg_catalog.boolean_blob_eq(boolean, blob);
DROP function IF EXISTS pg_catalog.blob_boolean_eq(blob, boolean);
DROP FUNCTION IF EXISTS pg_catalog.boolean_blob_ne(boolean, blob);
DROP FUNCTION IF EXISTS pg_catalog.blob_boolean_ne(blob, boolean);
DROP FUNCTION IF EXISTS pg_catalog.boolean_blob_gt(boolean, blob);
DROP FUNCTION IF EXISTS pg_catalog.blob_boolean_gt(blob, boolean);
DROP FUNCTION IF EXISTS pg_catalog.boolean_blob_lt(boolean, blob);
DROP FUNCTION IF EXISTS pg_catalog.blob_boolean_lt(blob, boolean);
DROP FUNCTION IF EXISTS pg_catalog.boolean_blob_ge(boolean, blob);
DROP FUNCTION IF EXISTS pg_catalog.blob_boolean_ge(blob, boolean);
DROP FUNCTION IF EXISTS pg_catalog.boolean_blob_le(boolean, blob);
DROP FUNCTION IF EXISTS pg_catalog.blob_boolean_le(blob, boolean);

DROP OPERATOR IF EXISTS pg_catalog.=(boolean, longblob);
DROP OPERATOR IF EXISTS pg_catalog.=(longblob, boolean);
DROP OPERATOR IF EXISTS pg_catalog.<>(boolean, longblob);
DROP OPERATOR IF EXISTS pg_catalog.<>(longblob, boolean);
DROP OPERATOR IF EXISTS pg_catalog.>(boolean, longblob);
DROP OPERATOR IF EXISTS pg_catalog.>(longblob, boolean);
DROP OPERATOR IF EXISTS pg_catalog.<(boolean, longblob);
DROP OPERATOR IF EXISTS pg_catalog.<(longblob, boolean);
DROP OPERATOR IF EXISTS pg_catalog.>=(boolean, longblob);
DROP OPERATOR IF EXISTS pg_catalog.>=(longblob, boolean);
DROP OPERATOR IF EXISTS pg_catalog.<=(boolean, longblob);
DROP OPERATOR IF EXISTS pg_catalog.<=(longblob, boolean);
DROP function IF EXISTS pg_catalog.boolean_longblob_eq(boolean, longblob);
DROP function IF EXISTS pg_catalog.longblob_boolean_eq(longblob, boolean);
DROP FUNCTION IF EXISTS pg_catalog.boolean_longblob_ne(boolean, longblob);
DROP FUNCTION IF EXISTS pg_catalog.longblob_boolean_ne(longblob, boolean);
DROP FUNCTION IF EXISTS pg_catalog.boolean_longblob_gt(boolean, longblob);
DROP FUNCTION IF EXISTS pg_catalog.longblob_boolean_gt(longblob, boolean);
DROP FUNCTION IF EXISTS pg_catalog.boolean_longblob_lt(boolean, longblob);
DROP FUNCTION IF EXISTS pg_catalog.longblob_boolean_lt(longblob, boolean);
DROP FUNCTION IF EXISTS pg_catalog.boolean_longblob_ge(boolean, longblob);
DROP FUNCTION IF EXISTS pg_catalog.longblob_boolean_ge(longblob, boolean);
DROP FUNCTION IF EXISTS pg_catalog.boolean_longblob_le(boolean, longblob);
DROP FUNCTION IF EXISTS pg_catalog.longblob_boolean_le(longblob, boolean);

DROP OPERATOR IF EXISTS pg_catalog.=(year, binary);
DROP OPERATOR IF EXISTS pg_catalog.=(binary, year);
DROP OPERATOR IF EXISTS pg_catalog.<>(year, binary);
DROP OPERATOR IF EXISTS pg_catalog.<>(binary, year);
DROP OPERATOR IF EXISTS pg_catalog.>(year, binary);
DROP OPERATOR IF EXISTS pg_catalog.>(binary, year);
DROP OPERATOR IF EXISTS pg_catalog.<(year, binary);
DROP OPERATOR IF EXISTS pg_catalog.<(binary, year);
DROP OPERATOR IF EXISTS pg_catalog.>=(year, binary);
DROP OPERATOR IF EXISTS pg_catalog.>=(binary, year);
DROP OPERATOR IF EXISTS pg_catalog.<=(year, binary);
DROP OPERATOR IF EXISTS pg_catalog.<=(binary, year);
DROP FUNCTION IF EXISTS pg_catalog.year_binary_eq(year, binary);
DROP FUNCTION IF EXISTS pg_catalog.binary_year_eq(binary, year);
DROP FUNCTION IF EXISTS pg_catalog.year_binary_ne(year, binary);
DROP FUNCTION IF EXISTS pg_catalog.binary_year_ne(binary, year);
DROP FUNCTION IF EXISTS pg_catalog.year_binary_gt(year, binary);
DROP FUNCTION IF EXISTS pg_catalog.binary_year_gt(binary, year);
DROP FUNCTION IF EXISTS pg_catalog.year_binary_lt(year, binary);
DROP FUNCTION IF EXISTS pg_catalog.binary_year_lt(binary, year);
DROP FUNCTION IF EXISTS pg_catalog.year_binary_ge(year, binary);
DROP FUNCTION IF EXISTS pg_catalog.binary_year_ge(binary, year);
DROP FUNCTION IF EXISTS pg_catalog.year_binary_le(year, binary);
DROP FUNCTION IF EXISTS pg_catalog.binary_year_le(binary, year);

DROP OPERATOR IF EXISTS pg_catalog.=(year, tinyblob);
DROP OPERATOR IF EXISTS pg_catalog.=(tinyblob, year);
DROP OPERATOR IF EXISTS pg_catalog.<>(year, tinyblob);
DROP OPERATOR IF EXISTS pg_catalog.<>(tinyblob, year);
DROP OPERATOR IF EXISTS pg_catalog.>(year, tinyblob);
DROP OPERATOR IF EXISTS pg_catalog.>(tinyblob, year);
DROP OPERATOR IF EXISTS pg_catalog.<(year, tinyblob);
DROP OPERATOR IF EXISTS pg_catalog.<(tinyblob, year);
DROP OPERATOR IF EXISTS pg_catalog.>=(year, tinyblob);
DROP OPERATOR IF EXISTS pg_catalog.>=(tinyblob, year);
DROP OPERATOR IF EXISTS pg_catalog.<=(year, tinyblob);
DROP OPERATOR IF EXISTS pg_catalog.<=(tinyblob, year);
DROP FUNCTION IF EXISTS pg_catalog.year_tinyblob_eq(year, tinyblob);
DROP FUNCTION IF EXISTS pg_catalog.tinyblob_year_eq(tinyblob, year);
DROP FUNCTION IF EXISTS pg_catalog.year_tinyblob_ne(year, tinyblob);
DROP FUNCTION IF EXISTS pg_catalog.tinyblob_year_ne(tinyblob, year);
DROP FUNCTION IF EXISTS pg_catalog.year_tinyblob_gt(year, tinyblob);
DROP FUNCTION IF EXISTS pg_catalog.tinyblob_year_gt(tinyblob, year);
DROP FUNCTION IF EXISTS pg_catalog.year_tinyblob_lt(year, tinyblob);
DROP FUNCTION IF EXISTS pg_catalog.tinyblob_year_lt(tinyblob, year);
DROP FUNCTION IF EXISTS pg_catalog.year_tinyblob_ge(year, tinyblob);
DROP FUNCTION IF EXISTS pg_catalog.tinyblob_year_ge(tinyblob, year);
DROP FUNCTION IF EXISTS pg_catalog.year_tinyblob_le(year, tinyblob);
DROP FUNCTION IF EXISTS pg_catalog.tinyblob_year_le(tinyblob, year);

DROP OPERATOR IF EXISTS pg_catalog.=(year, mediumblob);
DROP OPERATOR IF EXISTS pg_catalog.=(mediumblob, year);
DROP OPERATOR IF EXISTS pg_catalog.<>(year, mediumblob);
DROP OPERATOR IF EXISTS pg_catalog.<>(mediumblob, year);
DROP OPERATOR IF EXISTS pg_catalog.>(year, mediumblob);
DROP OPERATOR IF EXISTS pg_catalog.>(mediumblob, year);
DROP OPERATOR IF EXISTS pg_catalog.<(year, mediumblob);
DROP OPERATOR IF EXISTS pg_catalog.<(mediumblob, year);
DROP OPERATOR IF EXISTS pg_catalog.>=(year, mediumblob);
DROP OPERATOR IF EXISTS pg_catalog.>=(mediumblob, year);
DROP OPERATOR IF EXISTS pg_catalog.<=(year, mediumblob);
DROP OPERATOR IF EXISTS pg_catalog.<=(mediumblob, year);
DROP FUNCTION IF EXISTS pg_catalog.year_mediumblob_eq(year, mediumblob);
DROP FUNCTION IF EXISTS pg_catalog.mediumblob_year_eq(mediumblob, year);
DROP FUNCTION IF EXISTS pg_catalog.year_mediumblob_ne(year, mediumblob);
DROP FUNCTION IF EXISTS pg_catalog.mediumblob_year_ne(mediumblob, year);
DROP FUNCTION IF EXISTS pg_catalog.year_mediumblob_gt(year, mediumblob);
DROP FUNCTION IF EXISTS pg_catalog.mediumblob_year_gt(mediumblob, year);
DROP FUNCTION IF EXISTS pg_catalog.year_mediumblob_lt(year, mediumblob);
DROP FUNCTION IF EXISTS pg_catalog.mediumblob_year_lt(mediumblob, year);
DROP FUNCTION IF EXISTS pg_catalog.year_mediumblob_ge(year, mediumblob);
DROP FUNCTION IF EXISTS pg_catalog.mediumblob_year_ge(mediumblob, year);
DROP FUNCTION IF EXISTS pg_catalog.year_mediumblob_le(year, mediumblob);
DROP FUNCTION IF EXISTS pg_catalog.mediumblob_year_le(mediumblob, year);

DROP OPERATOR IF EXISTS pg_catalog.=(year, blob);
DROP OPERATOR IF EXISTS pg_catalog.=(blob, year);
DROP OPERATOR IF EXISTS pg_catalog.<>(year, blob);
DROP OPERATOR IF EXISTS pg_catalog.<>(blob, year);
DROP OPERATOR IF EXISTS pg_catalog.>(year, blob);
DROP OPERATOR IF EXISTS pg_catalog.>(blob, year);
DROP OPERATOR IF EXISTS pg_catalog.<(year, blob);
DROP OPERATOR IF EXISTS pg_catalog.<(blob, year);
DROP OPERATOR IF EXISTS pg_catalog.>=(year, blob);
DROP OPERATOR IF EXISTS pg_catalog.>=(blob, year);
DROP OPERATOR IF EXISTS pg_catalog.<=(year, blob);
DROP OPERATOR IF EXISTS pg_catalog.<=(blob, year);
DROP FUNCTION IF EXISTS pg_catalog.year_blob_eq(year, blob);
DROP FUNCTION IF EXISTS pg_catalog.blob_year_eq(blob, year);
DROP FUNCTION IF EXISTS pg_catalog.year_blob_ne(year, blob);
DROP FUNCTION IF EXISTS pg_catalog.blob_year_ne(blob, year);
DROP FUNCTION IF EXISTS pg_catalog.year_blob_gt(year, blob);
DROP FUNCTION IF EXISTS pg_catalog.blob_year_gt(blob, year);
DROP FUNCTION IF EXISTS pg_catalog.year_blob_lt(year, blob);
DROP FUNCTION IF EXISTS pg_catalog.blob_year_lt(blob, year);
DROP FUNCTION IF EXISTS pg_catalog.year_blob_ge(year, blob);
DROP FUNCTION IF EXISTS pg_catalog.blob_year_ge(blob, year);
DROP FUNCTION IF EXISTS pg_catalog.year_blob_le(year, blob);
DROP FUNCTION IF EXISTS pg_catalog.blob_year_le(blob, year);

DROP OPERATOR IF EXISTS pg_catalog.=(year, longblob);
DROP OPERATOR IF EXISTS pg_catalog.=(longblob, year);
DROP OPERATOR IF EXISTS pg_catalog.<>(year, longblob);
DROP OPERATOR IF EXISTS pg_catalog.<>(longblob, year);
DROP OPERATOR IF EXISTS pg_catalog.>(year, longblob);
DROP OPERATOR IF EXISTS pg_catalog.>(longblob, year);
DROP OPERATOR IF EXISTS pg_catalog.<(year, longblob);
DROP OPERATOR IF EXISTS pg_catalog.<(longblob, year);
DROP OPERATOR IF EXISTS pg_catalog.>=(year, longblob);
DROP OPERATOR IF EXISTS pg_catalog.>=(longblob, year);
DROP OPERATOR IF EXISTS pg_catalog.<=(year, longblob);
DROP OPERATOR IF EXISTS pg_catalog.<=(longblob, year);
DROP FUNCTION IF EXISTS pg_catalog.year_longblob_eq(year, longblob);
DROP FUNCTION IF EXISTS pg_catalog.longblob_year_eq(longblob, year);
DROP FUNCTION IF EXISTS pg_catalog.year_longblob_ne(year, longblob);
DROP FUNCTION IF EXISTS pg_catalog.longblob_year_ne(longblob, year);
DROP FUNCTION IF EXISTS pg_catalog.year_longblob_gt(year, longblob);
DROP FUNCTION IF EXISTS pg_catalog.longblob_year_gt(longblob, year);
DROP FUNCTION IF EXISTS pg_catalog.year_longblob_lt(year, longblob);
DROP FUNCTION IF EXISTS pg_catalog.longblob_year_lt(longblob, year);
DROP FUNCTION IF EXISTS pg_catalog.year_longblob_ge(year, longblob);
DROP FUNCTION IF EXISTS pg_catalog.longblob_year_ge(longblob, year);
DROP FUNCTION IF EXISTS pg_catalog.year_longblob_le(year, longblob);
DROP FUNCTION IF EXISTS pg_catalog.longblob_year_le(longblob, year);

DROP OPERATOR IF EXISTS pg_catalog.=(int1, binary);
DROP FUNCTION IF EXISTS pg_catalog.int1_binary_eq(int1, binary);
DROP OPERATOR IF EXISTS pg_catalog.=(binary, int1);
DROP FUNCTION IF EXISTS pg_catalog.binary_int1_eq(binary, int1);
DROP OPERATOR IF EXISTS pg_catalog.<>(int1, binary);
DROP FUNCTION IF EXISTS pg_catalog.int1_binary_ne(int1, binary);
DROP OPERATOR IF EXISTS pg_catalog.<>(binary, int1);
DROP FUNCTION IF EXISTS pg_catalog.binary_int1_ne(binary, int1);

DROP OPERATOR IF EXISTS pg_catalog.=(int1, tinyblob);
DROP FUNCTION IF EXISTS pg_catalog.int1_tinyblob_eq(int1, tinyblob);
DROP OPERATOR IF EXISTS pg_catalog.=(tinyblob, int1);
DROP FUNCTION IF EXISTS pg_catalog.tinyblob_int1_eq(tinyblob, int1);
DROP OPERATOR IF EXISTS pg_catalog.<>(int1, tinyblob);
DROP FUNCTION IF EXISTS pg_catalog.int1_tinyblob_ne(int1, tinyblob);
DROP OPERATOR IF EXISTS pg_catalog.<>(tinyblob, int1);
DROP FUNCTION IF EXISTS pg_catalog.tinyblob_int1_ne(tinyblob, int1);

DROP OPERATOR IF EXISTS pg_catalog.=(int1, mediumblob);
DROP FUNCTION IF EXISTS pg_catalog.int1_mediumblob_eq(int1, mediumblob);
DROP OPERATOR IF EXISTS pg_catalog.=(mediumblob, int1);
DROP FUNCTION IF EXISTS pg_catalog.mediumblob_int1_eq(mediumblob, int1);
DROP OPERATOR IF EXISTS pg_catalog.<>(int1, mediumblob);
DROP FUNCTION IF EXISTS pg_catalog.int1_mediumblob_ne(int1, mediumblob);
DROP OPERATOR IF EXISTS pg_catalog.<>(mediumblob, int1);
DROP FUNCTION IF EXISTS pg_catalog.mediumblob_int1_ne(mediumblob, int1);

DROP OPERATOR IF EXISTS pg_catalog.=(int1, blob);
DROP FUNCTION IF EXISTS pg_catalog.int1_blob_eq(int1, blob);
DROP OPERATOR IF EXISTS pg_catalog.=(blob, int1);
DROP FUNCTION IF EXISTS pg_catalog.blob_int1_eq(blob, int1);
DROP OPERATOR IF EXISTS pg_catalog.<>(int1, blob);
DROP FUNCTION IF EXISTS pg_catalog.int1_blob_ne(int1, blob);
DROP OPERATOR IF EXISTS pg_catalog.<>(blob, int1);
DROP FUNCTION IF EXISTS pg_catalog.blob_int1_ne(blob, int1);

DROP OPERATOR IF EXISTS pg_catalog.=(int1, longblob);
DROP FUNCTION IF EXISTS pg_catalog.int1_longblob_eq(int1, longblob);
DROP OPERATOR IF EXISTS pg_catalog.=(longblob, int1);
DROP FUNCTION IF EXISTS pg_catalog.longblob_int1_eq(longblob, int1);
DROP OPERATOR IF EXISTS pg_catalog.<>(int1, longblob);
DROP FUNCTION IF EXISTS pg_catalog.int1_longblob_ne(int1, longblob);
DROP OPERATOR IF EXISTS pg_catalog.<>(longblob, int1);
DROP FUNCTION IF EXISTS pg_catalog.longblob_int1_ne(longblob, int1);

DROP OPERATOR IF EXISTS pg_catalog.=(text, binary);
DROP FUNCTION IF EXISTS pg_catalog.text_binary_eq(text, binary);
DROP OPERATOR IF EXISTS pg_catalog.=(binary, text);
DROP FUNCTION IF EXISTS pg_catalog.binary_text_eq(binary, text);

DROP CAST (timestamptz AS year);
DROP CAST (timestamp(0) without time zone AS year);
DROP CAST (date AS year);

DROP FUNCTION IF EXISTS pg_catalog.timestamp_year(timestamptz) cascade;
DROP FUNCTION IF EXISTS pg_catalog.datetime_year(timestamp(0) without time zone) cascade;
DROP FUNCTION IF EXISTS pg_catalog.date_year(date) cascade;

CREATE OR REPLACE FUNCTION pg_catalog.timestamp_year(timestamptz) RETURNS year LANGUAGE SQL STABLE STRICT as 'select cast(cast($1 as int8) as year)';
CREATE OR REPLACE FUNCTION pg_catalog.datetime_year(timestamp(0) without time zone) RETURNS year LANGUAGE SQL STABLE STRICT as 'select cast(cast($1 as int8) as year)';
CREATE OR REPLACE FUNCTION pg_catalog.date_year(date) RETURNS year LANGUAGE SQL STABLE STRICT as 'select cast(cast($1 as int8) as year)';

CREATE CAST (timestamptz AS year) with function pg_catalog.timestamp_year(timestamptz) AS ASSIGNMENT;
CREATE CAST (timestamp(0) without time zone AS year) with function pg_catalog.datetime_year(timestamp(0) without time zone) AS ASSIGNMENT;
CREATE CAST (date as year) with function pg_catalog.date_year(date) AS ASSIGNMENT;

DROP CAST IF EXISTS (FLOAT8 AS NVARCHAR2);
DROP CAST IF EXISTS (FLOAT4 AS NVARCHAR2);
DROP FUNCTION IF EXISTS pg_catalog.float8_nvarchar2(FLOAT8);
DROP FUNCTION IF EXISTS pg_catalog.float4_nvarchar2(FLOAT4);
CREATE CAST (FLOAT4 AS NVARCHAR2) WITH FUNCTION pg_catalog.TO_NVARCHAR2(FLOAT4) AS IMPLICIT;
CREATE CAST (FLOAT8 AS NVARCHAR2) WITH FUNCTION pg_catalog.TO_NVARCHAR2(FLOAT8) AS IMPLICIT;

DROP FUNCTION IF EXISTS pg_catalog.json_contains("any", "any", text);
DROP FUNCTION IF EXISTS pg_catalog.json_contains("any", "any");
DROP FUNCTION IF EXISTS pg_catalog.json_valid("any");
CREATE OR REPLACE FUNCTION pg_catalog.json_contains("any", "any", text) RETURNS boolean LANGUAGE C IMMUTABLE as '$libdir/dolphin', 'json_contains';
CREATE OR REPLACE FUNCTION pg_catalog.json_contains("any", "any") RETURNS boolean LANGUAGE C IMMUTABLE as '$libdir/dolphin', 'json_contains';
CREATE OR REPLACE FUNCTION pg_catalog.json_valid("any") RETURNS boolean LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'json_valid';

DROP FUNCTION pg_catalog.json_length("any");
DROP FUNCTION pg_catalog.json_length("any",text);
DROP FUNCTION pg_catalog.json_depth("any");
DROP FUNCTION pg_catalog.json_storage_size("any");
CREATE OR REPLACE FUNCTION pg_catalog.json_length("any") RETURNS int LANGUAGE C IMMUTABLE as '$libdir/dolphin', 'json_length';
CREATE OR REPLACE FUNCTION pg_catalog.json_length("any",text) RETURNS int LANGUAGE C IMMUTABLE as '$libdir/dolphin', 'json_length';
CREATE OR REPLACE FUNCTION pg_catalog.json_depth("any") RETURNS int LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'json_depth';
CREATE OR REPLACE FUNCTION pg_catalog.json_storage_size("any") RETURNS int LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'json_storage_size';

DROP FUNCTION IF EXISTS pg_catalog.b_extract (text, year);
DROP FUNCTION IF EXISTS pg_catalog.yearweek (year);
DROP FUNCTION IF EXISTS pg_catalog.makedate (year, int8);
DROP FUNCTION IF EXISTS pg_catalog.b_timestampdiff(text,year,year);
DROP FUNCTION IF EXISTS pg_catalog.b_timestampdiff(text,text,year);
DROP FUNCTION IF EXISTS pg_catalog.b_timestampdiff(text,year,text);
DROP FUNCTION IF EXISTS pg_catalog.date_add (year, interval);
DROP FUNCTION IF EXISTS pg_catalog.date_sub (year, interval);

DROP FUNCTION IF EXISTS pg_catalog.is_ipv4(bit) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.is_ipv4(boolean) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.is_ipv4(year) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.is_ipv4(blob) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.is_ipv4(anyenum) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.is_ipv4(json) CASCADE;

DROP OPERATOR IF EXISTS pg_catalog.&(binary, binary);
DROP OPERATOR IF EXISTS pg_catalog.&(blob, blob);
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

DROP FUNCTION IF EXISTS pg_catalog.inet6_ntoa(bit) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.inet6_ntoa(boolean) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.inet6_ntoa(year) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.inet6_ntoa(blob) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.inet6_ntoa(anyenum) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.inet6_ntoa(json) CASCADE;

DROP FUNCTION IF EXISTS pg_catalog.md5(bit) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.md5(boolean) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.md5(year) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.md5(blob) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.md5(anyenum) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.md5(json) CASCADE;
DROP OPERATOR CLASS IF EXISTS uint2_ops USING hash;
DROP OPERATOR CLASS IF EXISTS uint4_ops USING hash;
DROP OPERATOR CLASS IF EXISTS uint2_ops USING btree;
DROP OPERATOR CLASS IF EXISTS uint4_ops USING btree;

DROP OPERATOR IF EXISTS pg_catalog.=(int8, uint4);
DROP OPERATOR IF EXISTS pg_catalog.=(int8, uint2);
DROP FUNCTION IF EXISTS pg_catalog.int8_uint4_eq(int8, uint4);
DROP FUNCTION IF EXISTS pg_catalog.int8_uint2_eq(int8, uint2);

drop operator IF EXISTS pg_catalog.=(uint2, int8);
CREATE OPERATOR pg_catalog.=(
leftarg = uint2, rightarg = int8, procedure = uint2_int8_eq,
restrict = eqsel, join = eqjoinsel,
HASHES, MERGES
);
drop operator IF EXISTS pg_catalog.=(uint4, int8);
CREATE OPERATOR pg_catalog.=(
leftarg = uint4, rightarg = int8, procedure = uint4_int8_eq,
restrict = eqsel, join = eqjoinsel,
HASHES, MERGES
);

CREATE OPERATOR CLASS uint2_ops
    DEFAULT FOR TYPE uint2 USING hash family integer_ops AS
        OPERATOR        1       = ,
        OPERATOR        1       =(uint2, uint4),
        OPERATOR        1       =(uint2, uint8),
        OPERATOR        1       =(uint2, int2),
        OPERATOR        1       =(uint2, int4),
        OPERATOR        1       =(uint2, int8),
        OPERATOR        1       =(int2, uint2),
        FUNCTION        1       hashuint2(uint2);
CREATE OPERATOR CLASS uint4_ops
    DEFAULT FOR TYPE uint4 USING hash family integer_ops AS
        OPERATOR        1       = ,
        OPERATOR        1       =(uint4, uint8),
        OPERATOR        1       =(uint4, int4),
        OPERATOR        1       =(uint4, int8),
        OPERATOR        1       =(int4, uint4),
        FUNCTION        1       hashuint4(uint4);

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

DROP FUNCTION pg_catalog.xor(int8, int8);
DROP FUNCTION pg_catalog.xor(int8, bit);
DROP FUNCTION pg_catalog.xor(bit, int8);
DROP FUNCTION pg_catalog.xor(bit, bit);
DROP FUNCTION pg_catalog.xor(int8, text);
DROP FUNCTION pg_catalog.xor(text, int8);
DROP FUNCTION pg_catalog.xor(text, text);
DROP FUNCTION pg_catalog.xor(text, bit);
DROP FUNCTION pg_catalog.xor(bit, text);
DROP FUNCTION pg_catalog.xor(varbinary, int8);
DROP FUNCTION pg_catalog.xor(int8, varbinary);
DROP FUNCTION pg_catalog.xor(varbinary, varbinary);
DROP FUNCTION pg_catalog.xor(varbinary, bit);
DROP FUNCTION pg_catalog.xor(bit, varbinary);
DROP FUNCTION pg_catalog.xor(varbinary, text);
DROP FUNCTION pg_catalog.xor(text, varbinary);
DROP FUNCTION pg_catalog.xor(binary, int8);
DROP FUNCTION pg_catalog.xor(int8, binary);
DROP FUNCTION pg_catalog.xor(binary, binary);
DROP FUNCTION pg_catalog.xor(binary, bit);
DROP FUNCTION pg_catalog.xor(bit, binary);
DROP FUNCTION pg_catalog.xor(varbinary, binary);
DROP FUNCTION pg_catalog.xor(binary, varbinary);
DROP FUNCTION pg_catalog.xor(binary, text);
DROP FUNCTION pg_catalog.xor(text, binary);
DROP FUNCTION pg_catalog.xor(varchar, int8);
DROP FUNCTION pg_catalog.xor(int8, varchar);
DROP FUNCTION pg_catalog.xor(varchar, varchar);
DROP FUNCTION pg_catalog.xor(varchar, bit);
DROP FUNCTION pg_catalog.xor(bit, varchar);
DROP FUNCTION pg_catalog.xor(varbinary, varchar);
DROP FUNCTION pg_catalog.xor(varchar, varbinary);
DROP FUNCTION pg_catalog.xor(varchar, text);
DROP FUNCTION pg_catalog.xor(text, varchar);
DROP FUNCTION pg_catalog.xor(binary, varchar);
DROP FUNCTION pg_catalog.xor(varchar, binary);
DROP FUNCTION pg_catalog.xor(char, int8);
DROP FUNCTION pg_catalog.xor(int8, char);
DROP FUNCTION pg_catalog.xor(char, char);
DROP FUNCTION pg_catalog.xor(char, bit);
DROP FUNCTION pg_catalog.xor(bit, char);
DROP FUNCTION pg_catalog.xor(varbinary, char);
DROP FUNCTION pg_catalog.xor(char, varbinary);
DROP FUNCTION pg_catalog.xor(char, text);
DROP FUNCTION pg_catalog.xor(text, char) ;
DROP FUNCTION pg_catalog.xor(binary, char);
DROP FUNCTION pg_catalog.xor(char, binary);
DROP FUNCTION pg_catalog.xor(varchar, char);
DROP FUNCTION pg_catalog.xor(char, varchar);
DROP FUNCTION pg_catalog.xor(unknown,unknown);
DROP FUNCTION pg_catalog.xor(unknown, int8);
DROP FUNCTION pg_catalog.xor(int8, unknown);
DROP FUNCTION pg_catalog.xor(unknown, integer);
DROP FUNCTION pg_catalog.xor(integer, unknown);
DROP FUNCTION pg_catalog.xor(unknown, float8);
DROP FUNCTION pg_catalog.xor(float8, unknown);
DROP FUNCTION pg_catalog.xor(unknown, boolean);
DROP FUNCTION pg_catalog.xor(boolean, unknown);
DROP FUNCTION pg_catalog.xor(unknown, bit);
DROP FUNCTION pg_catalog.xor(bit, unknown);
DROP FUNCTION pg_catalog.xor(unknown, text);
DROP FUNCTION pg_catalog.xor(text, unknown);
DROP FUNCTION pg_catalog.xor(unknown, binary);
DROP FUNCTION pg_catalog.xor(binary, unknown);
DROP FUNCTION pg_catalog.xor(varbinary, unknown);
DROP FUNCTION pg_catalog.xor(unknown, varbinary);
DROP FUNCTION pg_catalog.xor(varchar, unknown);
DROP FUNCTION pg_catalog.xor(unknown, varchar);

CREATE OR REPLACE FUNCTION pg_catalog.xor(a integer, b integer)
returns integer
as
$$
begin
    return (select int4xor(a::bool::integer, b::bool::integer));
end;
$$
language plpgsql;

DROP FUNCTION pg_catalog.round(int1);
DROP FUNCTION pg_catalog.round(uint1);
DROP FUNCTION pg_catalog.round(int2);
DROP FUNCTION pg_catalog.round(uint2);
DROP FUNCTION pg_catalog.round(int4);
DROP FUNCTION pg_catalog.round(uint4);
DROP FUNCTION pg_catalog.round(int8);
DROP FUNCTION pg_catalog.round(uint8);
DROP FUNCTION pg_catalog.round(boolean);
DROP FUNCTION pg_catalog.round(year);

DROP FUNCTION pg_catalog.round(int1, int4);
DROP FUNCTION pg_catalog.round(uint1, int4);
DROP FUNCTION pg_catalog.round(int2, int4);
DROP FUNCTION pg_catalog.round(uint2, int4);
DROP FUNCTION pg_catalog.round(int4, int4);
DROP FUNCTION pg_catalog.round(uint4, int4);
DROP FUNCTION pg_catalog.round(int8, int4);
DROP FUNCTION pg_catalog.round(uint8, int4);
DROP FUNCTION pg_catalog.round(boolean, int4);
DROP FUNCTION pg_catalog.round(year, int4);
DROP FUNCTION pg_catalog.round(binary, int4);
DROP FUNCTION pg_catalog.round(json, int4);

DROP FUNCTION pg_catalog.round(int1, uint4);
DROP FUNCTION pg_catalog.round(uint1, uint4);
DROP FUNCTION pg_catalog.round(int2, uint4);
DROP FUNCTION pg_catalog.round(uint2, uint4);
DROP FUNCTION pg_catalog.round(int4, uint4);
DROP FUNCTION pg_catalog.round(uint4, uint4);
DROP FUNCTION pg_catalog.round(int8, uint4);
DROP FUNCTION pg_catalog.round(uint8, uint4);
DROP FUNCTION pg_catalog.round(boolean, uint4);
DROP FUNCTION pg_catalog.round(year, uint4);
DROP FUNCTION pg_catalog.round(binary, uint4);
DROP FUNCTION pg_catalog.round(json, uint4);

DROP FUNCTION pg_catalog.truncate(int1, int4);
DROP FUNCTION pg_catalog.truncate(uint1, int4);
DROP FUNCTION pg_catalog.truncate(int2, int4);
DROP FUNCTION pg_catalog.truncate(uint2, int4);
DROP FUNCTION pg_catalog.truncate(int4, int4);
DROP FUNCTION pg_catalog.truncate(uint4, int4);
DROP FUNCTION pg_catalog.truncate(int8, int4);
DROP FUNCTION pg_catalog.truncate(uint8, int4);
DROP FUNCTION pg_catalog.truncate(boolean, int4);
DROP FUNCTION pg_catalog.truncate(year, int4);
DROP FUNCTION pg_catalog.truncate(binary, int4);
DROP FUNCTION pg_catalog.truncate(json, int4);

DROP FUNCTION pg_catalog.truncate(int1, uint4);
DROP FUNCTION pg_catalog.truncate(uint1, uint4);
DROP FUNCTION pg_catalog.truncate(int2, uint4);
DROP FUNCTION pg_catalog.truncate(uint2, uint4);
DROP FUNCTION pg_catalog.truncate(int4, uint4);
DROP FUNCTION pg_catalog.truncate(uint4, uint4);
DROP FUNCTION pg_catalog.truncate(int8, uint4);
DROP FUNCTION pg_catalog.truncate(uint8, uint4);
DROP FUNCTION pg_catalog.truncate(boolean, uint4);
DROP FUNCTION pg_catalog.truncate(year, uint4);
DROP FUNCTION pg_catalog.truncate(binary, uint4);
DROP FUNCTION pg_catalog.truncate(json, uint4);

DROP FUNCTION IF EXISTS pg_catalog.left(bit, integer);
DROP FUNCTION IF EXISTS pg_catalog.left(blob, integer);
DROP FUNCTION IF EXISTS pg_catalog.left(boolean, integer);

DROP OPERATOR IF EXISTS pg_catalog.=(int, uint2);
DROP FUNCTION IF EXISTS pg_catalog.int_uint2_eq(int, uint2);

DROP OPERATOR IF EXISTS pg_catalog.=(tinyblob, json);
DROP FUNCTION IF EXISTS pg_catalog.json_eq(tinyblob, json);
DROP OPERATOR IF EXISTS pg_catalog.=(blob, json);
DROP FUNCTION IF EXISTS pg_catalog.json_eq(blob, json);
DROP OPERATOR IF EXISTS pg_catalog.=(mediumblob, json);
DROP FUNCTION IF EXISTS pg_catalog.json_eq(mediumblob, json);
DROP OPERATOR IF EXISTS pg_catalog.=(longblob, json);
DROP FUNCTION IF EXISTS pg_catalog.json_eq(longblob, json);
DROP OPERATOR IF EXISTS pg_catalog.=(anyenum, json);
DROP FUNCTION IF EXISTS pg_catalog.json_eq(anyenum, json);

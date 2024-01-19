DROP CAST IF EXISTS (bool AS bit) CASCADE;
DROP CAST IF EXISTS (bool AS float4) CASCADE;
DROP CAST IF EXISTS (bool AS float8) CASCADE;

DROP FUNCTION IF EXISTS pg_catalog.booltobit(bool, int4) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.booltofloat4(bool) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.booltofloat8(bool) CASCADE;

DROP FUNCTION IF EXISTS pg_catalog.atan2 (boolean, boolean);
DROP FUNCTION IF EXISTS pg_catalog.atan2 (boolean, float8);
DROP FUNCTION IF EXISTS pg_catalog.atan2 (float8, boolean);
DROP FUNCTION IF EXISTS pg_catalog.atan (float8, float8);
DROP FUNCTION IF EXISTS pg_catalog.atan (boolean, boolean);
DROP FUNCTION IF EXISTS pg_catalog.atan (boolean, float8);
DROP FUNCTION IF EXISTS pg_catalog.atan (float8, boolean);
DROP FUNCTION IF EXISTS pg_catalog.atan (boolean);

drop CAST IF EXISTS (timestamptz as boolean);
drop CAST IF EXISTS (timestamp(0) without time zone as boolean);
DROP FUNCTION IF EXISTS pg_catalog.timestamptz_bool(timestamptz);
DROP FUNCTION IF EXISTS pg_catalog.timestamp_bool(timestamp(0) without time zone);

DROP FUNCTION IF EXISTS pg_catalog.ord(varbit);
DROP FUNCTION IF EXISTS pg_catalog.oct(bit);
DROP FUNCTION IF EXISTS pg_catalog.substring_index ("any", "any", text);
DROP FUNCTION IF EXISTS pg_catalog.substring_index ("any", "any", numeric);

DO $for_og_310$
BEGIN
    if working_version_num() > 92780 then
        DROP FUNCTION IF EXISTS pg_catalog.substring_index (text, text, numeric) CASCADE;
        CREATE FUNCTION pg_catalog.substring_index (
        text,
        text,
        numeric
        ) RETURNS text LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'substring_index';
        
        DROP FUNCTION IF EXISTS pg_catalog.substring_index (boolean, text, numeric) CASCADE;
        CREATE FUNCTION pg_catalog.substring_index (
        boolean,
        text,
        numeric
        ) RETURNS text LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'substring_index_bool_1';
        
        DROP FUNCTION IF EXISTS pg_catalog.substring_index (text, boolean, numeric) CASCADE;
        CREATE FUNCTION pg_catalog.substring_index (
        text,
        boolean,
        numeric
        ) RETURNS text LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'substring_index_bool_2';
        
        DROP FUNCTION IF EXISTS pg_catalog.substring_index (boolean, boolean, numeric) CASCADE;
        CREATE FUNCTION pg_catalog.substring_index (
        boolean,
        boolean,
        numeric
        ) RETURNS text LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'substring_index_2bool';
    end if;
END
$for_og_310$;

-- below from 3.0
DROP FUNCTION IF EXISTS pg_catalog.date_cast(cstring, boolean);
DROP FUNCTION IF EXISTS pg_catalog.timestamp_cast(cstring, oid, integer, boolean);

DROP FUNCTION IF EXISTS pg_catalog.bit_cast_int8(bit) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.float4_cast_int8(float4) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.float8_cast_int8(float8) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.numeric_cast_int8(numeric) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.date_cast_int8(date) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.timestamp_cast_int8(timestamp without time zone) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.timestamptz_cast_int8(timestamptz) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.time_cast_date(time) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.timetz_cast_date(timetz) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.set_cast_int8(anyset) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.uint8_cast_int8(uint8) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.year_cast_int8(year) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.bpchar_cast_int8(char) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.varchar_cast_int8(varchar) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.text_cast_int8(text) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.varlena_cast_int8(anyelement) CASCADE;

DROP FUNCTION IF EXISTS pg_catalog.time_cast(cstring, boolean);
DROP CAST IF EXISTS (TEXT AS time);
DROP FUNCTION IF EXISTS pg_catalog.time_cast_implicit(TEXT);
DROP FUNCTION IF EXISTS pg_catalog.text_time_explicit(TEXT);

DROP FUNCTION IF EXISTS pg_catalog.day(time without time zone);

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

DROP FUNCTION IF EXISTS pg_catalog.bit_length(boolean);
DROP FUNCTION IF EXISTS pg_catalog.bit_length(year);
DROP FUNCTION IF EXISTS pg_catalog.bit_length(blob);
DROP FUNCTION IF EXISTS pg_catalog.bit_length(anyenum);
DROP FUNCTION IF EXISTS pg_catalog.bit_length(json);
DROP FUNCTION IF EXISTS pg_catalog.bit_length(binary);

drop function IF EXISTS pg_catalog."user"();

DROP OPERATOR IF EXISTS pg_catalog.~~(varbinary, varbinary);
DROP OPERATOR IF EXISTS pg_catalog.~~(varbinary, text);
DROP OPERATOR IF EXISTS pg_catalog.~~(text, varbinary);
DROP FUNCTION IF EXISTS pg_catalog.varbinarylike(varbinary, varbinary);
DROP FUNCTION IF EXISTS pg_catalog.textvarbinarylike(text, varbinary);
DROP FUNCTION IF EXISTS pg_catalog.varbinarytextlike(varbinary, text);
DROP OPERATOR IF EXISTS pg_catalog.~~(blob, blob);
DROP FUNCTION IF EXISTS pg_catalog.bloblike(blob, blob);

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

DROP FUNCTION IF EXISTS pg_catalog.time_to_sec(numeric);
DROP FUNCTION IF EXISTS pg_catalog.time_to_sec(int8);
CREATE OR REPLACE FUNCTION pg_catalog.time_to_sec(numeric) RETURNS int4 AS $$ SELECT pg_catalog.time_to_sec(cast($1 as text)) $$ LANGUAGE SQL;

DROP FUNCTION IF EXISTS pg_catalog.dayofmonth(text);
DROP FUNCTION IF EXISTS pg_catalog.dayofmonth(numeric);

DROP FUNCTION IF EXISTS pg_catalog.date_format (time without time zone, text);
DROP FUNCTION IF EXISTS pg_catalog.to_char(time without time zone, text);

DROP FUNCTION IF EXISTS pg_catalog.bit_cast_time(bit);
DROP FUNCTION IF EXISTS pg_catalog.bit_cast_timestamp(bit);
DROP FUNCTION IF EXISTS pg_catalog.bit_cast_datetime(bit);
DROP FUNCTION IF EXISTS pg_catalog.bit_cast_date(bit);

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

drop function if EXISTS pg_catalog.length(binary);
drop function if EXISTS pg_catalog.length(varbinary);

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

DROP FUNCTION IF EXISTS pg_catalog.degrees(boolean);
DROP FUNCTION IF EXISTS pg_catalog.degrees(year);
DROP FUNCTION IF EXISTS pg_catalog.degrees(json);

DROP FUNCTION IF EXISTS pg_catalog.exp(year);
DROP FUNCTION IF EXISTS pg_catalog.exp(json);

DROP FUNCTION IF EXISTS pg_catalog.inet_ntoa(bit);
DROP FUNCTION IF EXISTS pg_catalog.inet_ntoa(binary);
DROP FUNCTION IF EXISTS pg_catalog.inet_ntoa(tinyblob);
DROP FUNCTION IF EXISTS pg_catalog.inet_ntoa(nvarchar2);
DROP FUNCTION IF EXISTS pg_catalog.inet_ntoa(year);
DROP FUNCTION IF EXISTS pg_catalog.inet_ntoa(json);

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

DROP FUNCTION IF EXISTS pg_catalog.ln(year);
DROP FUNCTION IF EXISTS pg_catalog.ln(json);

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

DROP FUNCTION IF EXISTS pg_catalog.acos(boolean);
DROP FUNCTION IF EXISTS pg_catalog.acos(year);
DROP FUNCTION IF EXISTS pg_catalog.acos(json);

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

DROP FUNCTION IF EXISTS pg_catalog.floor(year);
DROP FUNCTION IF EXISTS pg_catalog.floor(json);

-- repeat function support
DROP FUNCTION IF EXISTS pg_catalog.repeat(anyenum, integer);
DROP FUNCTION IF EXISTS pg_catalog.repeat(boolean, integer);
DROP FUNCTION IF EXISTS pg_catalog.repeat(tinyblob, integer);
DROP FUNCTION IF EXISTS pg_catalog.repeat(json, integer);
DROP FUNCTION IF EXISTS pg_catalog.repeat(year, integer);
DROP FUNCTION IF EXISTS pg_catalog.repeat(binary, integer);
DROP FUNCTION IF EXISTS pg_catalog.repeat(bit, integer);
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
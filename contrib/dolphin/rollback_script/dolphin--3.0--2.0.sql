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

DROP FUNCTION IF EXISTS pg_catalog.dolphin_invoke();
CREATE FUNCTION pg_catalog.dolphin_invoke()
    RETURNS VOID AS '$libdir/dolphin','dolphin_invoke' LANGUAGE C STRICT;

DROP FUNCTION IF EXISTS pg_catalog.timestamptz_cast(cstring, oid, integer, boolean);

DROP FUNCTION IF EXISTS pg_catalog.time_to_sec(json);
DROP FUNCTION IF EXISTS pg_catalog.time_to_sec(anyenum);
DROP FUNCTION IF EXISTS pg_catalog.time_to_sec(anyset);
DROP FUNCTION IF EXISTS pg_catalog.time_to_sec(bit);
DROP FUNCTION IF EXISTS pg_catalog.time_to_sec(longblob);
DROP FUNCTION IF EXISTS pg_catalog.time_to_sec(year);
DROP FUNCTION IF EXISTS pg_catalog.time_to_sec(date);
DROP FUNCTION IF EXISTS pg_catalog.time_to_sec(timestamp with time zone);
DROP FUNCTION IF EXISTS pg_catalog.time_to_sec(timestamp without time zone);
DROP FUNCTION IF EXISTS pg_catalog.time_to_sec(text);

CREATE OR REPLACE FUNCTION pg_catalog.time_to_sec(text) RETURNS int4 LANGUAGE C STABLE RETURNS NULL ON NULL INPUT as '$libdir/dolphin', 'time_to_sec';
CREATE OR REPLACE FUNCTION pg_catalog.time_to_sec(date) RETURNS int4 AS $$ SELECT pg_catalog.time_to_sec(cast('00:00:00' as text)) $$ LANGUAGE SQL;

DROP FUNCTION IF EXISTS pg_catalog.date_cast_timestamptz(date);
DROP FUNCTION IF EXISTS pg_catalog.date_cast_datetime(date);

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

DROP FUNCTION IF EXISTS pg_catalog.dayofmonth(text);
DROP FUNCTION IF EXISTS pg_catalog.dayofmonth(numeric);

drop function IF EXISTS pg_catalog."user"();

DROP FUNCTION IF EXISTS pg_catalog.convert(boolean, name);
DROP FUNCTION IF EXISTS pg_catalog.convert(longblob, name);
DROP FUNCTION IF EXISTS pg_catalog.convert(anyenum, name);
DROP FUNCTION IF EXISTS pg_catalog.convert(json, name);

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

DROP FUNCTION IF EXISTS pg_catalog.log(anyelement,anyelement);

DROP FUNCTION IF EXISTS pg_catalog.log(year);
DROP FUNCTION IF EXISTS pg_catalog.log(json);
DROP FUNCTION IF EXISTS pg_catalog.log(uint1);
DROP FUNCTION IF EXISTS pg_catalog.log(uint2);
DROP FUNCTION IF EXISTS pg_catalog.log(uint4);
DROP FUNCTION IF EXISTS pg_catalog.log(uint8);
DROP FUNCTION IF EXISTS pg_catalog.log(bit);
DROP FUNCTION IF EXISTS pg_catalog.log(boolean);
DROP FUNCTION IF EXISTS pg_catalog.log(text);
DROP FUNCTION IF EXISTS pg_catalog.log(char);
DROP FUNCTION IF EXISTS pg_catalog.log(varchar);

DROP FUNCTION IF EXISTS pg_catalog.log2(uint1);
DROP FUNCTION IF EXISTS pg_catalog.log2(uint2);
DROP FUNCTION IF EXISTS pg_catalog.log2(uint4);
DROP FUNCTION IF EXISTS pg_catalog.log2(uint8);
DROP FUNCTION IF EXISTS pg_catalog.log2(numeric);
CREATE OR REPLACE FUNCTION pg_catalog.log2(numeric)
RETURNS numeric
AS
$$
BEGIN
    IF $1 <= 0 THEN
        RETURN NULL;
    end if;
    RETURN (SELECT log(2, $1));
END;
$$
LANGUAGE plpgsql;
DROP FUNCTION IF EXISTS pg_catalog.log2(bit);
DROP FUNCTION IF EXISTS pg_catalog.log2(boolean);
DROP FUNCTION IF EXISTS pg_catalog.log2(text);
DROP FUNCTION IF EXISTS pg_catalog.log2(char);
DROP FUNCTION IF EXISTS pg_catalog.log2(varchar);

DROP FUNCTION IF EXISTS pg_catalog.log10(uint1);
DROP FUNCTION IF EXISTS pg_catalog.log10(uint2);
DROP FUNCTION IF EXISTS pg_catalog.log10(uint4);
DROP FUNCTION IF EXISTS pg_catalog.log10(uint8);
DROP FUNCTION IF EXISTS pg_catalog.log10(numeric);
CREATE OR REPLACE FUNCTION pg_catalog.log10(numeric)
RETURNS numeric
AS
$$
BEGIN
    IF $1 <= 0 THEN
        RETURN NULL;
    end if;
    RETURN (SELECT log(10, $1));
END;
$$
LANGUAGE plpgsql;
DROP FUNCTION IF EXISTS pg_catalog.log10(bit);
DROP FUNCTION IF EXISTS pg_catalog.log10(boolean);
DROP FUNCTION IF EXISTS pg_catalog.log10(text);
DROP FUNCTION IF EXISTS pg_catalog.log10(char);
DROP FUNCTION IF EXISTS pg_catalog.log10(varchar);

DROP FUNCTION IF EXISTS pg_catalog.varlena_cast_int1(anyelement) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.varlena_cast_int2(anyelement) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.varlena_cast_int4(anyelement) CASCADE;

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

drop function pg_catalog.year_recv(internal);
CREATE OR REPLACE FUNCTION pg_catalog.year_recv (bytea) RETURNS year LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'year_recv';
do $$
begin
update pg_catalog.pg_type set typreceive = '-', typsend = '-'  where oid = 'year'::regtype;
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
DROP AGGREGATE IF EXISTS pg_catalog.bit_and(varbinary);
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

DROP FUNCTION IF EXISTS pg_catalog.to_days(bit);
DROP FUNCTION IF EXISTS pg_catalog.to_days(boolean);
DROP FUNCTION IF EXISTS pg_catalog.to_days(time);
DROP FUNCTION IF EXISTS pg_catalog.to_days(year);
DROP FUNCTION IF EXISTS pg_catalog.to_days(binary);
DROP FUNCTION IF EXISTS pg_catalog.to_days(blob);
DROP FUNCTION IF EXISTS pg_catalog.to_days(text);
DROP FUNCTION IF EXISTS pg_catalog.to_days(anyenum);
DROP FUNCTION IF EXISTS pg_catalog.to_days(anyset);
DROP FUNCTION IF EXISTS pg_catalog.to_days(json);

CREATE OR REPLACE FUNCTION pg_catalog.to_days(time) RETURNS int8 AS $$ SELECT pg_catalog.to_days(text_date('now') + $1)  $$ LANGUAGE SQL;

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

DROP OPERATOR IF EXISTS pg_catalog.||(bit, bit);
DROP OPERATOR IF EXISTS pg_catalog.||(bit, boolean);
DROP OPERATOR IF EXISTS pg_catalog.||(boolean, bit);
DROP OPERATOR IF EXISTS pg_catalog.||(boolean, boolean);
DROP OPERATOR IF EXISTS pg_catalog.||(text, boolean);
DROP OPERATOR IF EXISTS pg_catalog.||(boolean, text);
DROP OPERATOR IF EXISTS pg_catalog.||(bit, text);
DROP OPERATOR IF EXISTS pg_catalog.||(text, bit);
DROP OPERATOR IF EXISTS pg_catalog.||(bit, binary);
DROP OPERATOR IF EXISTS pg_catalog.||(binary, bit);
DROP OPERATOR IF EXISTS pg_catalog.||(uint8, binary);
DROP OPERATOR IF EXISTS pg_catalog.||(binary, uint8);
DROP OPERATOR IF EXISTS pg_catalog.||(int8, binary);
DROP OPERATOR IF EXISTS pg_catalog.||(binary, int8);
DROP OPERATOR IF EXISTS pg_catalog.||(numeric, binary);
DROP OPERATOR IF EXISTS pg_catalog.||(binary, numeric);
DROP OPERATOR IF EXISTS pg_catalog.||(float4, binary);
DROP OPERATOR IF EXISTS pg_catalog.||(binary, float4);
DROP OPERATOR IF EXISTS pg_catalog.||(float8, binary);
DROP OPERATOR IF EXISTS pg_catalog.||(binary, float8);
DROP OPERATOR IF EXISTS pg_catalog.||(text, binary);
DROP OPERATOR IF EXISTS pg_catalog.||(binary, text);
DROP OPERATOR IF EXISTS pg_catalog.||(boolean, binary);
DROP OPERATOR IF EXISTS pg_catalog.||(binary, boolean);
DROP OPERATOR IF EXISTS pg_catalog.||(binary, binary);
DROP OPERATOR IF EXISTS pg_catalog.||(bit, varbinary);
DROP OPERATOR IF EXISTS pg_catalog.||(varbinary, bit);
DROP OPERATOR IF EXISTS pg_catalog.||(uint8, varbinary);
DROP OPERATOR IF EXISTS pg_catalog.||(varbinary, uint8);
DROP OPERATOR IF EXISTS pg_catalog.||(int8, varbinary);
DROP OPERATOR IF EXISTS pg_catalog.||(varbinary, int8);
DROP OPERATOR IF EXISTS pg_catalog.||(numeric, varbinary);
DROP OPERATOR IF EXISTS pg_catalog.||(varbinary, numeric);
DROP OPERATOR IF EXISTS pg_catalog.||(float4, varbinary);
DROP OPERATOR IF EXISTS pg_catalog.||(varbinary, float4);
DROP OPERATOR IF EXISTS pg_catalog.||(float8, varbinary);
DROP OPERATOR IF EXISTS pg_catalog.||(varbinary, float8);
DROP OPERATOR IF EXISTS pg_catalog.||(text, varbinary);
DROP OPERATOR IF EXISTS pg_catalog.||(varbinary, text);
DROP OPERATOR IF EXISTS pg_catalog.||(boolean, varbinary);
DROP OPERATOR IF EXISTS pg_catalog.||(varbinary, boolean);
DROP OPERATOR IF EXISTS pg_catalog.||(binary, varbinary);
DROP OPERATOR IF EXISTS pg_catalog.||(varbinary, binary);
DROP OPERATOR IF EXISTS pg_catalog.||(varbinary, varbinary);
DROP OPERATOR IF EXISTS pg_catalog.||(unknown, unknown);
DROP OPERATOR IF EXISTS pg_catalog.||(unknown, integer);
DROP OPERATOR IF EXISTS pg_catalog.||(integer, unknown);

DROP FUNCTION pg_catalog.bit_concat(bit, bit);
DROP FUNCTION pg_catalog.bit_bool_concat(bit, boolean);
DROP FUNCTION pg_catalog.bool_bit_concat(boolean, bit);
DROP FUNCTION pg_catalog.bool_concat(boolean, boolean);
DROP FUNCTION pg_catalog.text_bool_concat(text, boolean);
DROP FUNCTION pg_catalog.bool_text_concat(boolean, text);
DROP FUNCTION pg_catalog.text_bit_concat(text, bit);
DROP FUNCTION pg_catalog.bit_text_concat(bit, text);
DROP FUNCTION pg_catalog.bin_concat(binary,binary);
DROP FUNCTION pg_catalog.bin_int_concat(binary,uint8);
DROP FUNCTION pg_catalog.bin_int8_concat(binary,int8);
DROP FUNCTION pg_catalog.bin_float4_concat(binary,float4);
DROP FUNCTION pg_catalog.bin_float8_concat(binary,float8);
DROP FUNCTION pg_catalog.bin_num_concat(binary,numeric);
DROP FUNCTION pg_catalog.bin_bit_concat(binary,bit);
DROP FUNCTION pg_catalog.bin_text_concat(binary,text);
DROP FUNCTION pg_catalog.bin_bool_concat(binary,boolean);
DROP FUNCTION pg_catalog.int_bin_concat(uint8,binary);
DROP FUNCTION pg_catalog.int8_bin_concat(int8,binary);
DROP FUNCTION pg_catalog.float4_bin_concat(float4,binary);
DROP FUNCTION pg_catalog.float8_bin_concat(float8,binary);
DROP FUNCTION pg_catalog.num_bin_concat(numeric,binary);
DROP FUNCTION pg_catalog.bit_bin_concat(bit,binary);
DROP FUNCTION pg_catalog.text_bin_concat(text,binary);
DROP FUNCTION pg_catalog.bool_bin_concat(boolean,binary);
DROP FUNCTION pg_catalog.varbin_concat(varbinary,varbinary);
DROP FUNCTION pg_catalog.varbin_int_concat(varbinary,uint8);
DROP FUNCTION pg_catalog.varbin_int8_concat(varbinary,int8);
DROP FUNCTION pg_catalog.varbin_float4_concat(varbinary,float4);
DROP FUNCTION pg_catalog.varbin_float8_concat(varbinary,float8);
DROP FUNCTION pg_catalog.varbin_num_concat(varbinary,numeric);
DROP FUNCTION pg_catalog.varbin_bit_concat(varbinary,bit);
DROP FUNCTION pg_catalog.varbin_text_concat(varbinary,text);
DROP FUNCTION pg_catalog.varbin_bool_concat(varbinary,boolean);
DROP FUNCTION pg_catalog.varbin_bin_concat(varbinary,binary);
DROP FUNCTION pg_catalog.int_varbin_concat(uint8,varbinary);
DROP FUNCTION pg_catalog.int8_varbin_concat(int8,varbinary);
DROP FUNCTION pg_catalog.float4_varbin_concat(float4,varbinary);
DROP FUNCTION pg_catalog.float8_varbin_concat(float8,varbinary);
DROP FUNCTION pg_catalog.num_varbin_concat(numeric,varbinary);
DROP FUNCTION pg_catalog.bit_varbin_concat(bit,varbinary);
DROP FUNCTION pg_catalog.text_varbin_concat(text,varbinary);
DROP FUNCTION pg_catalog.bool_varbin_concat(boolean,varbinary);
DROP FUNCTION pg_catalog.bin_varbin_concat(binary,varbinary);
DROP FUNCTION pg_catalog.unknown_concat(unknown, unknown);
DROP FUNCTION pg_catalog.unknown_int_concat(unknown, integer);
DROP FUNCTION pg_catalog.int_unknown_concat(integer, unknown);

DROP FUNCTION IF EXISTS pg_catalog.left(bit, integer);
DROP FUNCTION IF EXISTS pg_catalog.left(blob, integer);
DROP FUNCTION IF EXISTS pg_catalog.left(boolean, integer);

DROP FUNCTION IF EXISTS pg_catalog.lower(boolean);
DROP FUNCTION IF EXISTS pg_catalog.lower(bit);
DROP FUNCTION IF EXISTS pg_catalog.lower(tinyblob);
DROP FUNCTION IF EXISTS pg_catalog.lower(blob);
DROP FUNCTION IF EXISTS pg_catalog.lower(mediumblob);
DROP FUNCTION IF EXISTS pg_catalog.lower(longblob);
DROP FUNCTION IF EXISTS pg_catalog.lower(binary);
DROP FUNCTION IF EXISTS pg_catalog.lower(varbinary);
DROP FUNCTION IF EXISTS pg_catalog.lower(integer);
DROP FUNCTION IF EXISTS pg_catalog.lower(float);
DROP FUNCTION IF EXISTS pg_catalog.lower(char);
DROP FUNCTION IF EXISTS pg_catalog.lower(varchar);

DROP FUNCTION IF EXISTS pg_catalog.lcase(boolean);
DROP FUNCTION IF EXISTS pg_catalog.lcase(tinyblob);
DROP FUNCTION IF EXISTS pg_catalog.lcase(blob);
DROP FUNCTION IF EXISTS pg_catalog.lcase(mediumblob);
DROP FUNCTION IF EXISTS pg_catalog.lcase(longblob);
DROP FUNCTION IF EXISTS pg_catalog.lcase(bit);
DROP FUNCTION IF EXISTS pg_catalog.lcase(binary);
DROP FUNCTION IF EXISTS pg_catalog.lcase(varbinary);
DROP FUNCTION IF EXISTS pg_catalog.lcase(integer);
DROP FUNCTION IF EXISTS pg_catalog.lcase(float);
DROP FUNCTION IF EXISTS pg_catalog.lcase(char);
DROP FUNCTION IF EXISTS pg_catalog.lcase(varchar);

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

DROP FUNCTION IF EXISTS pg_catalog.hex(anyenum);

DROP OPERATOR IF EXISTS pg_catalog.=(tinyblob, numeric);
DROP OPERATOR IF EXISTS pg_catalog.=(numeric, tinyblob);
DROP OPERATOR IF EXISTS pg_catalog.<>(tinyblob, numeric);
DROP OPERATOR IF EXISTS pg_catalog.<>(numeric, tinyblob);
DROP OPERATOR IF EXISTS pg_catalog.<(tinyblob, numeric);
DROP OPERATOR IF EXISTS pg_catalog.<(numeric, tinyblob);
DROP OPERATOR IF EXISTS pg_catalog.<=(tinyblob, numeric);
DROP OPERATOR IF EXISTS pg_catalog.<=(numeric, tinyblob);
DROP OPERATOR IF EXISTS pg_catalog.>(tinyblob, numeric);
DROP OPERATOR IF EXISTS pg_catalog.>(numeric, tinyblob);
DROP OPERATOR IF EXISTS pg_catalog.>=(tinyblob, numeric);
DROP OPERATOR IF EXISTS pg_catalog.>=(numeric, tinyblob);
DROP FUNCTION IF EXISTS pg_catalog.tinyblob_numeric_eq(tinyblob, numeric);
DROP FUNCTION IF EXISTS pg_catalog.numeric_tinyblob_eq(numeric, tinyblob);
DROP FUNCTION IF EXISTS pg_catalog.tinyblob_numeric_ne(tinyblob, numeric);
DROP FUNCTION IF EXISTS pg_catalog.numeric_tinyblob_ne(numeric, tinyblob);
DROP FUNCTION IF EXISTS pg_catalog.tinyblob_numeric_lt(tinyblob, numeric);
DROP FUNCTION IF EXISTS pg_catalog.numeric_tinyblob_lt(numeric, tinyblob);
DROP FUNCTION IF EXISTS pg_catalog.tinyblob_numeric_le(tinyblob, numeric);
DROP FUNCTION IF EXISTS pg_catalog.numeric_tinyblob_le(numeric, tinyblob);
DROP FUNCTION IF EXISTS pg_catalog.tinyblob_numeric_gt(tinyblob, numeric);
DROP FUNCTION IF EXISTS pg_catalog.numeric_tinyblob_gt(numeric, tinyblob);
DROP FUNCTION IF EXISTS pg_catalog.tinyblob_numeric_ge(tinyblob, numeric);
DROP FUNCTION IF EXISTS pg_catalog.numeric_tinyblob_ge(numeric, tinyblob);
DROP FUNCTION IF EXISTS pg_catalog.tinyblob_numeric_cmp(tinyblob, numeric);
DROP FUNCTION IF EXISTS pg_catalog.numeric_tinyblob_cmp(numeric, tinyblob);
DROP OPERATOR IF EXISTS pg_catalog.=(blob, numeric);
DROP OPERATOR IF EXISTS pg_catalog.=(numeric, blob);
DROP OPERATOR IF EXISTS pg_catalog.<>(blob, numeric);
DROP OPERATOR IF EXISTS pg_catalog.<>(numeric, blob);
DROP OPERATOR IF EXISTS pg_catalog.<(blob, numeric);
DROP OPERATOR IF EXISTS pg_catalog.<(numeric, blob);
DROP OPERATOR IF EXISTS pg_catalog.<=(blob, numeric);
DROP OPERATOR IF EXISTS pg_catalog.<=(numeric, blob);
DROP OPERATOR IF EXISTS pg_catalog.>(blob, numeric);
DROP OPERATOR IF EXISTS pg_catalog.>(numeric, blob);
DROP OPERATOR IF EXISTS pg_catalog.>=(blob, numeric);
DROP OPERATOR IF EXISTS pg_catalog.>=(numeric, blob);
DROP FUNCTION IF EXISTS pg_catalog.blob_numeric_eq(blob, numeric);
DROP FUNCTION IF EXISTS pg_catalog.numeric_blob_eq(numeric, blob);
DROP FUNCTION IF EXISTS pg_catalog.blob_numeric_ne(blob, numeric);
DROP FUNCTION IF EXISTS pg_catalog.numeric_blob_ne(numeric, blob);
DROP FUNCTION IF EXISTS pg_catalog.blob_numeric_lt(blob, numeric);
DROP FUNCTION IF EXISTS pg_catalog.numeric_blob_lt(numeric, blob);
DROP FUNCTION IF EXISTS pg_catalog.blob_numeric_le(blob, numeric);
DROP FUNCTION IF EXISTS pg_catalog.numeric_blob_le(numeric, blob);
DROP FUNCTION IF EXISTS pg_catalog.blob_numeric_gt(blob, numeric);
DROP FUNCTION IF EXISTS pg_catalog.numeric_blob_gt(numeric, blob);
DROP FUNCTION IF EXISTS pg_catalog.blob_numeric_ge(blob, numeric);
DROP FUNCTION IF EXISTS pg_catalog.numeric_blob_ge(numeric, blob);
DROP FUNCTION IF EXISTS pg_catalog.blob_numeric_cmp(blob, numeric);
DROP FUNCTION IF EXISTS pg_catalog.numeric_blob_cmp(numeric, blob);
DROP OPERATOR IF EXISTS pg_catalog.=(mediumblob, numeric);
DROP OPERATOR IF EXISTS pg_catalog.=(numeric, mediumblob);
DROP OPERATOR IF EXISTS pg_catalog.<>(mediumblob, numeric);
DROP OPERATOR IF EXISTS pg_catalog.<>(numeric, mediumblob);
DROP OPERATOR IF EXISTS pg_catalog.<(mediumblob, numeric);
DROP OPERATOR IF EXISTS pg_catalog.<(numeric, mediumblob);
DROP OPERATOR IF EXISTS pg_catalog.<=(mediumblob, numeric);
DROP OPERATOR IF EXISTS pg_catalog.<=(numeric, mediumblob);
DROP OPERATOR IF EXISTS pg_catalog.>(mediumblob, numeric);
DROP OPERATOR IF EXISTS pg_catalog.>(numeric, mediumblob);
DROP OPERATOR IF EXISTS pg_catalog.>=(mediumblob, numeric);
DROP OPERATOR IF EXISTS pg_catalog.>=(numeric, mediumblob);
DROP FUNCTION IF EXISTS pg_catalog.mediumblob_numeric_eq(mediumblob, numeric);
DROP FUNCTION IF EXISTS pg_catalog.numeric_mediumblob_eq(numeric, mediumblob);
DROP FUNCTION IF EXISTS pg_catalog.mediumblob_numeric_ne(mediumblob, numeric);
DROP FUNCTION IF EXISTS pg_catalog.numeric_mediumblob_ne(numeric, mediumblob);
DROP FUNCTION IF EXISTS pg_catalog.mediumblob_numeric_lt(mediumblob, numeric);
DROP FUNCTION IF EXISTS pg_catalog.numeric_mediumblob_lt(numeric, mediumblob);
DROP FUNCTION IF EXISTS pg_catalog.mediumblob_numeric_le(mediumblob, numeric);
DROP FUNCTION IF EXISTS pg_catalog.numeric_mediumblob_le(numeric, mediumblob);
DROP FUNCTION IF EXISTS pg_catalog.mediumblob_numeric_gt(mediumblob, numeric);
DROP FUNCTION IF EXISTS pg_catalog.numeric_mediumblob_gt(numeric, mediumblob);
DROP FUNCTION IF EXISTS pg_catalog.mediumblob_numeric_ge(mediumblob, numeric);
DROP FUNCTION IF EXISTS pg_catalog.numeric_mediumblob_ge(numeric, mediumblob);
DROP FUNCTION IF EXISTS pg_catalog.mediumblob_numeric_cmp(mediumblob, numeric);
DROP FUNCTION IF EXISTS pg_catalog.numeric_mediumblob_cmp(numeric, mediumblob);
DROP OPERATOR IF EXISTS pg_catalog.=(longblob, numeric);
DROP OPERATOR IF EXISTS pg_catalog.=(numeric, longblob);
DROP OPERATOR IF EXISTS pg_catalog.<>(longblob, numeric);
DROP OPERATOR IF EXISTS pg_catalog.<>(numeric, longblob);
DROP OPERATOR IF EXISTS pg_catalog.<(longblob, numeric);
DROP OPERATOR IF EXISTS pg_catalog.<(numeric, longblob);
DROP OPERATOR IF EXISTS pg_catalog.<=(longblob, numeric);
DROP OPERATOR IF EXISTS pg_catalog.<=(numeric, longblob);
DROP OPERATOR IF EXISTS pg_catalog.>(longblob, numeric);
DROP OPERATOR IF EXISTS pg_catalog.>(numeric, longblob);
DROP OPERATOR IF EXISTS pg_catalog.>=(longblob, numeric);
DROP OPERATOR IF EXISTS pg_catalog.>=(numeric, longblob);
DROP FUNCTION IF EXISTS pg_catalog.longblob_numeric_eq(longblob, numeric);
DROP FUNCTION IF EXISTS pg_catalog.numeric_longblob_eq(numeric, longblob);
DROP FUNCTION IF EXISTS pg_catalog.longblob_numeric_ne(longblob, numeric);
DROP FUNCTION IF EXISTS pg_catalog.numeric_longblob_ne(numeric, longblob);
DROP FUNCTION IF EXISTS pg_catalog.longblob_numeric_lt(longblob, numeric);
DROP FUNCTION IF EXISTS pg_catalog.numeric_longblob_lt(numeric, longblob);
DROP FUNCTION IF EXISTS pg_catalog.longblob_numeric_le(longblob, numeric);
DROP FUNCTION IF EXISTS pg_catalog.numeric_longblob_le(numeric, longblob);
DROP FUNCTION IF EXISTS pg_catalog.longblob_numeric_gt(longblob, numeric);
DROP FUNCTION IF EXISTS pg_catalog.numeric_longblob_gt(numeric, longblob);
DROP FUNCTION IF EXISTS pg_catalog.longblob_numeric_ge(longblob, numeric);
DROP FUNCTION IF EXISTS pg_catalog.numeric_longblob_ge(numeric, longblob);
DROP FUNCTION IF EXISTS pg_catalog.longblob_numeric_cmp(longblob, numeric);
DROP FUNCTION IF EXISTS pg_catalog.numeric_longblob_cmp(numeric, longblob);
DROP OPERATOR IF EXISTS pg_catalog.=(tinyblob, int8);
DROP OPERATOR IF EXISTS pg_catalog.=(int8, tinyblob);
DROP OPERATOR IF EXISTS pg_catalog.<>(tinyblob, int8);
DROP OPERATOR IF EXISTS pg_catalog.<>(int8, tinyblob);
DROP OPERATOR IF EXISTS pg_catalog.<(tinyblob, int8);
DROP OPERATOR IF EXISTS pg_catalog.<(int8, tinyblob);
DROP OPERATOR IF EXISTS pg_catalog.<=(tinyblob, int8);
DROP OPERATOR IF EXISTS pg_catalog.<=(int8, tinyblob);
DROP OPERATOR IF EXISTS pg_catalog.>(tinyblob, int8);
DROP OPERATOR IF EXISTS pg_catalog.>(int8, tinyblob);
DROP OPERATOR IF EXISTS pg_catalog.>=(tinyblob, int8);
DROP OPERATOR IF EXISTS pg_catalog.>=(int8, tinyblob);
DROP FUNCTION IF EXISTS pg_catalog.tinyblob_int8_eq(tinyblob, int8);
DROP FUNCTION IF EXISTS pg_catalog.int8_tinyblob_eq(int8, tinyblob);
DROP FUNCTION IF EXISTS pg_catalog.tinyblob_int8_ne(tinyblob, int8);
DROP FUNCTION IF EXISTS pg_catalog.int8_tinyblob_ne(int8, tinyblob);
DROP FUNCTION IF EXISTS pg_catalog.tinyblob_int8_lt(tinyblob, int8);
DROP FUNCTION IF EXISTS pg_catalog.int8_tinyblob_lt(int8, tinyblob);
DROP FUNCTION IF EXISTS pg_catalog.tinyblob_int8_le(tinyblob, int8);
DROP FUNCTION IF EXISTS pg_catalog.int8_tinyblob_le(int8, tinyblob);
DROP FUNCTION IF EXISTS pg_catalog.tinyblob_int8_gt(tinyblob, int8);
DROP FUNCTION IF EXISTS pg_catalog.int8_tinyblob_gt(int8, tinyblob);
DROP FUNCTION IF EXISTS pg_catalog.tinyblob_int8_ge(tinyblob, int8);
DROP FUNCTION IF EXISTS pg_catalog.int8_tinyblob_ge(int8, tinyblob);
DROP FUNCTION IF EXISTS pg_catalog.tinyblob_int8_cmp(tinyblob, int8);
DROP FUNCTION IF EXISTS pg_catalog.int8_tinyblob_cmp(int8, tinyblob);
DROP OPERATOR IF EXISTS pg_catalog.=(blob, int8);
DROP OPERATOR IF EXISTS pg_catalog.=(int8, blob);
DROP OPERATOR IF EXISTS pg_catalog.<>(blob, int8);
DROP OPERATOR IF EXISTS pg_catalog.<>(int8, blob);
DROP OPERATOR IF EXISTS pg_catalog.<(blob, int8);
DROP OPERATOR IF EXISTS pg_catalog.<(int8, blob);
DROP OPERATOR IF EXISTS pg_catalog.<=(blob, int8);
DROP OPERATOR IF EXISTS pg_catalog.<=(int8, blob);
DROP OPERATOR IF EXISTS pg_catalog.>(blob, int8);
DROP OPERATOR IF EXISTS pg_catalog.>(int8, blob);
DROP OPERATOR IF EXISTS pg_catalog.>=(blob, int8);
DROP OPERATOR IF EXISTS pg_catalog.>=(int8, blob);
DROP FUNCTION IF EXISTS pg_catalog.blob_int8_eq(blob, int8);
DROP FUNCTION IF EXISTS pg_catalog.int8_blob_eq(int8, blob);
DROP FUNCTION IF EXISTS pg_catalog.blob_int8_ne(blob, int8);
DROP FUNCTION IF EXISTS pg_catalog.int8_blob_ne(int8, blob);
DROP FUNCTION IF EXISTS pg_catalog.blob_int8_lt(blob, int8);
DROP FUNCTION IF EXISTS pg_catalog.int8_blob_lt(int8, blob);
DROP FUNCTION IF EXISTS pg_catalog.blob_int8_le(blob, int8);
DROP FUNCTION IF EXISTS pg_catalog.int8_blob_le(int8, blob);
DROP FUNCTION IF EXISTS pg_catalog.blob_int8_gt(blob, int8);
DROP FUNCTION IF EXISTS pg_catalog.int8_blob_gt(int8, blob);
DROP FUNCTION IF EXISTS pg_catalog.blob_int8_ge(blob, int8);
DROP FUNCTION IF EXISTS pg_catalog.int8_blob_ge(int8, blob);
DROP FUNCTION IF EXISTS pg_catalog.blob_int8_cmp(blob, int8);
DROP FUNCTION IF EXISTS pg_catalog.int8_blob_cmp(int8, blob);
DROP OPERATOR IF EXISTS pg_catalog.=(mediumblob, int8);
DROP OPERATOR IF EXISTS pg_catalog.=(int8, mediumblob);
DROP OPERATOR IF EXISTS pg_catalog.<>(mediumblob, int8);
DROP OPERATOR IF EXISTS pg_catalog.<>(int8, mediumblob);
DROP OPERATOR IF EXISTS pg_catalog.<(mediumblob, int8);
DROP OPERATOR IF EXISTS pg_catalog.<(int8, mediumblob);
DROP OPERATOR IF EXISTS pg_catalog.<=(mediumblob, int8);
DROP OPERATOR IF EXISTS pg_catalog.<=(int8, mediumblob);
DROP OPERATOR IF EXISTS pg_catalog.>(mediumblob, int8);
DROP OPERATOR IF EXISTS pg_catalog.>(int8, mediumblob);
DROP OPERATOR IF EXISTS pg_catalog.>=(mediumblob, int8);
DROP OPERATOR IF EXISTS pg_catalog.>=(int8, mediumblob);
DROP FUNCTION IF EXISTS pg_catalog.mediumblob_int8_eq(mediumblob, int8);
DROP FUNCTION IF EXISTS pg_catalog.int8_mediumblob_eq(int8, mediumblob);
DROP FUNCTION IF EXISTS pg_catalog.mediumblob_int8_ne(mediumblob, int8);
DROP FUNCTION IF EXISTS pg_catalog.int8_mediumblob_ne(int8, mediumblob);
DROP FUNCTION IF EXISTS pg_catalog.mediumblob_int8_lt(mediumblob, int8);
DROP FUNCTION IF EXISTS pg_catalog.int8_mediumblob_lt(int8, mediumblob);
DROP FUNCTION IF EXISTS pg_catalog.mediumblob_int8_le(mediumblob, int8);
DROP FUNCTION IF EXISTS pg_catalog.int8_mediumblob_le(int8, mediumblob);
DROP FUNCTION IF EXISTS pg_catalog.mediumblob_int8_gt(mediumblob, int8);
DROP FUNCTION IF EXISTS pg_catalog.int8_mediumblob_gt(int8, mediumblob);
DROP FUNCTION IF EXISTS pg_catalog.mediumblob_int8_ge(mediumblob, int8);
DROP FUNCTION IF EXISTS pg_catalog.int8_mediumblob_ge(int8, mediumblob);
DROP FUNCTION IF EXISTS pg_catalog.mediumblob_int8_cmp(mediumblob, int8);
DROP FUNCTION IF EXISTS pg_catalog.int8_mediumblob_cmp(int8, mediumblob);
DROP OPERATOR IF EXISTS pg_catalog.=(longblob, int8);
DROP OPERATOR IF EXISTS pg_catalog.=(int8, longblob);
DROP OPERATOR IF EXISTS pg_catalog.<>(longblob, int8);
DROP OPERATOR IF EXISTS pg_catalog.<>(int8, longblob);
DROP OPERATOR IF EXISTS pg_catalog.<(longblob, int8);
DROP OPERATOR IF EXISTS pg_catalog.<(int8, longblob);
DROP OPERATOR IF EXISTS pg_catalog.<=(longblob, int8);
DROP OPERATOR IF EXISTS pg_catalog.<=(int8, longblob);
DROP OPERATOR IF EXISTS pg_catalog.>(longblob, int8);
DROP OPERATOR IF EXISTS pg_catalog.>(int8, longblob);
DROP OPERATOR IF EXISTS pg_catalog.>=(longblob, int8);
DROP OPERATOR IF EXISTS pg_catalog.>=(int8, longblob);
DROP FUNCTION IF EXISTS pg_catalog.longblob_int8_eq(longblob, int8);
DROP FUNCTION IF EXISTS pg_catalog.int8_longblob_eq(int8, longblob);
DROP FUNCTION IF EXISTS pg_catalog.longblob_int8_ne(longblob, int8);
DROP FUNCTION IF EXISTS pg_catalog.int8_longblob_ne(int8, longblob);
DROP FUNCTION IF EXISTS pg_catalog.longblob_int8_lt(longblob, int8);
DROP FUNCTION IF EXISTS pg_catalog.int8_longblob_lt(int8, longblob);
DROP FUNCTION IF EXISTS pg_catalog.longblob_int8_le(longblob, int8);
DROP FUNCTION IF EXISTS pg_catalog.int8_longblob_le(int8, longblob);
DROP FUNCTION IF EXISTS pg_catalog.longblob_int8_gt(longblob, int8);
DROP FUNCTION IF EXISTS pg_catalog.int8_longblob_gt(int8, longblob);
DROP FUNCTION IF EXISTS pg_catalog.longblob_int8_ge(longblob, int8);
DROP FUNCTION IF EXISTS pg_catalog.int8_longblob_ge(int8, longblob);
DROP FUNCTION IF EXISTS pg_catalog.longblob_int8_cmp(longblob, int8);
DROP FUNCTION IF EXISTS pg_catalog.int8_longblob_cmp(int8, longblob);
DROP OPERATOR IF EXISTS pg_catalog.=(tinyblob, uint8);
DROP OPERATOR IF EXISTS pg_catalog.=(uint8, tinyblob);
DROP OPERATOR IF EXISTS pg_catalog.<>(tinyblob, uint8);
DROP OPERATOR IF EXISTS pg_catalog.<>(uint8, tinyblob);
DROP OPERATOR IF EXISTS pg_catalog.<(tinyblob, uint8);
DROP OPERATOR IF EXISTS pg_catalog.<(uint8, tinyblob);
DROP OPERATOR IF EXISTS pg_catalog.<=(tinyblob, uint8);
DROP OPERATOR IF EXISTS pg_catalog.<=(uint8, tinyblob);
DROP OPERATOR IF EXISTS pg_catalog.>(tinyblob, uint8);
DROP OPERATOR IF EXISTS pg_catalog.>(uint8, tinyblob);
DROP OPERATOR IF EXISTS pg_catalog.>=(tinyblob, uint8);
DROP OPERATOR IF EXISTS pg_catalog.>=(uint8, tinyblob);
DROP FUNCTION IF EXISTS pg_catalog.tinyblob_uint8_eq(tinyblob, uint8);
DROP FUNCTION IF EXISTS pg_catalog.uint8_tinyblob_eq(uint8, tinyblob);
DROP FUNCTION IF EXISTS pg_catalog.tinyblob_uint8_ne(tinyblob, uint8);
DROP FUNCTION IF EXISTS pg_catalog.uint8_tinyblob_ne(uint8, tinyblob);
DROP FUNCTION IF EXISTS pg_catalog.tinyblob_uint8_lt(tinyblob, uint8);
DROP FUNCTION IF EXISTS pg_catalog.uint8_tinyblob_lt(uint8, tinyblob);
DROP FUNCTION IF EXISTS pg_catalog.tinyblob_uint8_le(tinyblob, uint8);
DROP FUNCTION IF EXISTS pg_catalog.uint8_tinyblob_le(uint8, tinyblob);
DROP FUNCTION IF EXISTS pg_catalog.tinyblob_uint8_gt(tinyblob, uint8);
DROP FUNCTION IF EXISTS pg_catalog.uint8_tinyblob_gt(uint8, tinyblob);
DROP FUNCTION IF EXISTS pg_catalog.tinyblob_uint8_ge(tinyblob, uint8);
DROP FUNCTION IF EXISTS pg_catalog.uint8_tinyblob_ge(uint8, tinyblob);
DROP FUNCTION IF EXISTS pg_catalog.tinyblob_uint8_cmp(tinyblob, uint8);
DROP FUNCTION IF EXISTS pg_catalog.uint8_tinyblob_cmp(uint8, tinyblob);
DROP OPERATOR IF EXISTS pg_catalog.=(blob, uint8);
DROP OPERATOR IF EXISTS pg_catalog.=(uint8, blob);
DROP OPERATOR IF EXISTS pg_catalog.<>(blob, uint8);
DROP OPERATOR IF EXISTS pg_catalog.<>(uint8, blob);
DROP OPERATOR IF EXISTS pg_catalog.<(blob, uint8);
DROP OPERATOR IF EXISTS pg_catalog.<(uint8, blob);
DROP OPERATOR IF EXISTS pg_catalog.<=(blob, uint8);
DROP OPERATOR IF EXISTS pg_catalog.<=(uint8, blob);
DROP OPERATOR IF EXISTS pg_catalog.>(blob, uint8);
DROP OPERATOR IF EXISTS pg_catalog.>(uint8, blob);
DROP OPERATOR IF EXISTS pg_catalog.>=(blob, uint8);
DROP OPERATOR IF EXISTS pg_catalog.>=(uint8, blob);
DROP FUNCTION IF EXISTS pg_catalog.blob_uint8_eq(blob, uint8);
DROP FUNCTION IF EXISTS pg_catalog.uint8_blob_eq(uint8, blob);
DROP FUNCTION IF EXISTS pg_catalog.blob_uint8_ne(blob, uint8);
DROP FUNCTION IF EXISTS pg_catalog.uint8_blob_ne(uint8, blob);
DROP FUNCTION IF EXISTS pg_catalog.blob_uint8_lt(blob, uint8);
DROP FUNCTION IF EXISTS pg_catalog.uint8_blob_lt(uint8, blob);
DROP FUNCTION IF EXISTS pg_catalog.blob_uint8_le(blob, uint8);
DROP FUNCTION IF EXISTS pg_catalog.uint8_blob_le(uint8, blob);
DROP FUNCTION IF EXISTS pg_catalog.blob_uint8_gt(blob, uint8);
DROP FUNCTION IF EXISTS pg_catalog.uint8_blob_gt(uint8, blob);
DROP FUNCTION IF EXISTS pg_catalog.blob_uint8_ge(blob, uint8);
DROP FUNCTION IF EXISTS pg_catalog.uint8_blob_ge(uint8, blob);
DROP FUNCTION IF EXISTS pg_catalog.blob_uint8_cmp(blob, uint8);
DROP FUNCTION IF EXISTS pg_catalog.uint8_blob_cmp(uint8, blob);
DROP OPERATOR IF EXISTS pg_catalog.=(mediumblob, uint8);
DROP OPERATOR IF EXISTS pg_catalog.=(uint8, mediumblob);
DROP OPERATOR IF EXISTS pg_catalog.<>(mediumblob, uint8);
DROP OPERATOR IF EXISTS pg_catalog.<>(uint8, mediumblob);
DROP OPERATOR IF EXISTS pg_catalog.<(mediumblob, uint8);
DROP OPERATOR IF EXISTS pg_catalog.<(uint8, mediumblob);
DROP OPERATOR IF EXISTS pg_catalog.<=(mediumblob, uint8);
DROP OPERATOR IF EXISTS pg_catalog.<=(uint8, mediumblob);
DROP OPERATOR IF EXISTS pg_catalog.>(mediumblob, uint8);
DROP OPERATOR IF EXISTS pg_catalog.>(uint8, mediumblob);
DROP OPERATOR IF EXISTS pg_catalog.>=(mediumblob, uint8);
DROP OPERATOR IF EXISTS pg_catalog.>=(uint8, mediumblob);
DROP FUNCTION IF EXISTS pg_catalog.mediumblob_uint8_eq(mediumblob, uint8);
DROP FUNCTION IF EXISTS pg_catalog.uint8_mediumblob_eq(uint8, mediumblob);
DROP FUNCTION IF EXISTS pg_catalog.mediumblob_uint8_ne(mediumblob, uint8);
DROP FUNCTION IF EXISTS pg_catalog.uint8_mediumblob_ne(uint8, mediumblob);
DROP FUNCTION IF EXISTS pg_catalog.mediumblob_uint8_lt(mediumblob, uint8);
DROP FUNCTION IF EXISTS pg_catalog.uint8_mediumblob_lt(uint8, mediumblob);
DROP FUNCTION IF EXISTS pg_catalog.mediumblob_uint8_le(mediumblob, uint8);
DROP FUNCTION IF EXISTS pg_catalog.uint8_mediumblob_le(uint8, mediumblob);
DROP FUNCTION IF EXISTS pg_catalog.mediumblob_uint8_gt(mediumblob, uint8);
DROP FUNCTION IF EXISTS pg_catalog.uint8_mediumblob_gt(uint8, mediumblob);
DROP FUNCTION IF EXISTS pg_catalog.mediumblob_uint8_ge(mediumblob, uint8);
DROP FUNCTION IF EXISTS pg_catalog.uint8_mediumblob_ge(uint8, mediumblob);
DROP FUNCTION IF EXISTS pg_catalog.mediumblob_uint8_cmp(mediumblob, uint8);
DROP FUNCTION IF EXISTS pg_catalog.uint8_mediumblob_cmp(uint8, mediumblob);
DROP OPERATOR IF EXISTS pg_catalog.=(longblob, uint8);
DROP OPERATOR IF EXISTS pg_catalog.=(uint8, longblob);
DROP OPERATOR IF EXISTS pg_catalog.<>(longblob, uint8);
DROP OPERATOR IF EXISTS pg_catalog.<>(uint8, longblob);
DROP OPERATOR IF EXISTS pg_catalog.<(longblob, uint8);
DROP OPERATOR IF EXISTS pg_catalog.<(uint8, longblob);
DROP OPERATOR IF EXISTS pg_catalog.<=(longblob, uint8);
DROP OPERATOR IF EXISTS pg_catalog.<=(uint8, longblob);
DROP OPERATOR IF EXISTS pg_catalog.>(longblob, uint8);
DROP OPERATOR IF EXISTS pg_catalog.>(uint8, longblob);
DROP OPERATOR IF EXISTS pg_catalog.>=(longblob, uint8);
DROP OPERATOR IF EXISTS pg_catalog.>=(uint8, longblob);
DROP FUNCTION IF EXISTS pg_catalog.longblob_uint8_eq(longblob, uint8);
DROP FUNCTION IF EXISTS pg_catalog.uint8_longblob_eq(uint8, longblob);
DROP FUNCTION IF EXISTS pg_catalog.longblob_uint8_ne(longblob, uint8);
DROP FUNCTION IF EXISTS pg_catalog.uint8_longblob_ne(uint8, longblob);
DROP FUNCTION IF EXISTS pg_catalog.longblob_uint8_lt(longblob, uint8);
DROP FUNCTION IF EXISTS pg_catalog.uint8_longblob_lt(uint8, longblob);
DROP FUNCTION IF EXISTS pg_catalog.longblob_uint8_le(longblob, uint8);
DROP FUNCTION IF EXISTS pg_catalog.uint8_longblob_le(uint8, longblob);
DROP FUNCTION IF EXISTS pg_catalog.longblob_uint8_gt(longblob, uint8);
DROP FUNCTION IF EXISTS pg_catalog.uint8_longblob_gt(uint8, longblob);
DROP FUNCTION IF EXISTS pg_catalog.longblob_uint8_ge(longblob, uint8);
DROP FUNCTION IF EXISTS pg_catalog.uint8_longblob_ge(uint8, longblob);
DROP FUNCTION IF EXISTS pg_catalog.longblob_uint8_cmp(longblob, uint8);
DROP FUNCTION IF EXISTS pg_catalog.uint8_longblob_cmp(uint8, longblob);
DROP OPERATOR IF EXISTS pg_catalog.=(tinyblob, float8);
DROP OPERATOR IF EXISTS pg_catalog.=(float8, tinyblob);
DROP OPERATOR IF EXISTS pg_catalog.<>(tinyblob, float8);
DROP OPERATOR IF EXISTS pg_catalog.<>(float8, tinyblob);
DROP OPERATOR IF EXISTS pg_catalog.<(tinyblob, float8);
DROP OPERATOR IF EXISTS pg_catalog.<(float8, tinyblob);
DROP OPERATOR IF EXISTS pg_catalog.<=(tinyblob, float8);
DROP OPERATOR IF EXISTS pg_catalog.<=(float8, tinyblob);
DROP OPERATOR IF EXISTS pg_catalog.>(tinyblob, float8);
DROP OPERATOR IF EXISTS pg_catalog.>(float8, tinyblob);
DROP OPERATOR IF EXISTS pg_catalog.>=(tinyblob, float8);
DROP OPERATOR IF EXISTS pg_catalog.>=(float8, tinyblob);
DROP FUNCTION IF EXISTS pg_catalog.tinyblob_float8_eq(tinyblob, float8);
DROP FUNCTION IF EXISTS pg_catalog.float8_tinyblob_eq(float8, tinyblob);
DROP FUNCTION IF EXISTS pg_catalog.tinyblob_float8_ne(tinyblob, float8);
DROP FUNCTION IF EXISTS pg_catalog.float8_tinyblob_ne(float8, tinyblob);
DROP FUNCTION IF EXISTS pg_catalog.tinyblob_float8_lt(tinyblob, float8);
DROP FUNCTION IF EXISTS pg_catalog.float8_tinyblob_lt(float8, tinyblob);
DROP FUNCTION IF EXISTS pg_catalog.tinyblob_float8_le(tinyblob, float8);
DROP FUNCTION IF EXISTS pg_catalog.float8_tinyblob_le(float8, tinyblob);
DROP FUNCTION IF EXISTS pg_catalog.tinyblob_float8_gt(tinyblob, float8);
DROP FUNCTION IF EXISTS pg_catalog.float8_tinyblob_gt(float8, tinyblob);
DROP FUNCTION IF EXISTS pg_catalog.tinyblob_float8_ge(tinyblob, float8);
DROP FUNCTION IF EXISTS pg_catalog.float8_tinyblob_ge(float8, tinyblob);
DROP FUNCTION IF EXISTS pg_catalog.tinyblob_float8_cmp(tinyblob, float8);
DROP FUNCTION IF EXISTS pg_catalog.float8_tinyblob_cmp(float8, tinyblob);
DROP OPERATOR IF EXISTS pg_catalog.=(blob, float8);
DROP OPERATOR IF EXISTS pg_catalog.=(float8, blob);
DROP OPERATOR IF EXISTS pg_catalog.<>(blob, float8);
DROP OPERATOR IF EXISTS pg_catalog.<>(float8, blob);
DROP OPERATOR IF EXISTS pg_catalog.<(blob, float8);
DROP OPERATOR IF EXISTS pg_catalog.<(float8, blob);
DROP OPERATOR IF EXISTS pg_catalog.<=(blob, float8);
DROP OPERATOR IF EXISTS pg_catalog.<=(float8, blob);
DROP OPERATOR IF EXISTS pg_catalog.>(blob, float8);
DROP OPERATOR IF EXISTS pg_catalog.>(float8, blob);
DROP OPERATOR IF EXISTS pg_catalog.>=(blob, float8);
DROP OPERATOR IF EXISTS pg_catalog.>=(float8, blob);
DROP FUNCTION IF EXISTS pg_catalog.blob_float8_eq(blob, float8);
DROP FUNCTION IF EXISTS pg_catalog.float8_blob_eq(float8, blob);
DROP FUNCTION IF EXISTS pg_catalog.blob_float8_ne(blob, float8);
DROP FUNCTION IF EXISTS pg_catalog.float8_blob_ne(float8, blob);
DROP FUNCTION IF EXISTS pg_catalog.blob_float8_lt(blob, float8);
DROP FUNCTION IF EXISTS pg_catalog.float8_blob_lt(float8, blob);
DROP FUNCTION IF EXISTS pg_catalog.blob_float8_le(blob, float8);
DROP FUNCTION IF EXISTS pg_catalog.float8_blob_le(float8, blob);
DROP FUNCTION IF EXISTS pg_catalog.blob_float8_gt(blob, float8);
DROP FUNCTION IF EXISTS pg_catalog.float8_blob_gt(float8, blob);
DROP FUNCTION IF EXISTS pg_catalog.blob_float8_ge(blob, float8);
DROP FUNCTION IF EXISTS pg_catalog.float8_blob_ge(float8, blob);
DROP FUNCTION IF EXISTS pg_catalog.blob_float8_cmp(blob, float8);
DROP FUNCTION IF EXISTS pg_catalog.float8_blob_cmp(float8, blob);
DROP OPERATOR IF EXISTS pg_catalog.=(mediumblob, float8);
DROP OPERATOR IF EXISTS pg_catalog.=(float8, mediumblob);
DROP OPERATOR IF EXISTS pg_catalog.<>(mediumblob, float8);
DROP OPERATOR IF EXISTS pg_catalog.<>(float8, mediumblob);
DROP OPERATOR IF EXISTS pg_catalog.<(mediumblob, float8);
DROP OPERATOR IF EXISTS pg_catalog.<(float8, mediumblob);
DROP OPERATOR IF EXISTS pg_catalog.<=(mediumblob, float8);
DROP OPERATOR IF EXISTS pg_catalog.<=(float8, mediumblob);
DROP OPERATOR IF EXISTS pg_catalog.>(mediumblob, float8);
DROP OPERATOR IF EXISTS pg_catalog.>(float8, mediumblob);
DROP OPERATOR IF EXISTS pg_catalog.>=(mediumblob, float8);
DROP OPERATOR IF EXISTS pg_catalog.>=(float8, mediumblob);
DROP FUNCTION IF EXISTS pg_catalog.mediumblob_float8_eq(mediumblob, float8);
DROP FUNCTION IF EXISTS pg_catalog.float8_mediumblob_eq(float8, mediumblob);
DROP FUNCTION IF EXISTS pg_catalog.mediumblob_float8_ne(mediumblob, float8);
DROP FUNCTION IF EXISTS pg_catalog.float8_mediumblob_ne(float8, mediumblob);
DROP FUNCTION IF EXISTS pg_catalog.mediumblob_float8_lt(mediumblob, float8);
DROP FUNCTION IF EXISTS pg_catalog.float8_mediumblob_lt(float8, mediumblob);
DROP FUNCTION IF EXISTS pg_catalog.mediumblob_float8_le(mediumblob, float8);
DROP FUNCTION IF EXISTS pg_catalog.float8_mediumblob_le(float8, mediumblob);
DROP FUNCTION IF EXISTS pg_catalog.mediumblob_float8_gt(mediumblob, float8);
DROP FUNCTION IF EXISTS pg_catalog.float8_mediumblob_gt(float8, mediumblob);
DROP FUNCTION IF EXISTS pg_catalog.mediumblob_float8_ge(mediumblob, float8);
DROP FUNCTION IF EXISTS pg_catalog.float8_mediumblob_ge(float8, mediumblob);
DROP FUNCTION IF EXISTS pg_catalog.mediumblob_float8_cmp(mediumblob, float8);
DROP FUNCTION IF EXISTS pg_catalog.float8_mediumblob_cmp(float8, mediumblob);
DROP OPERATOR IF EXISTS pg_catalog.=(longblob, float8);
DROP OPERATOR IF EXISTS pg_catalog.=(float8, longblob);
DROP OPERATOR IF EXISTS pg_catalog.<>(longblob, float8);
DROP OPERATOR IF EXISTS pg_catalog.<>(float8, longblob);
DROP OPERATOR IF EXISTS pg_catalog.<(longblob, float8);
DROP OPERATOR IF EXISTS pg_catalog.<(float8, longblob);
DROP OPERATOR IF EXISTS pg_catalog.<=(longblob, float8);
DROP OPERATOR IF EXISTS pg_catalog.<=(float8, longblob);
DROP OPERATOR IF EXISTS pg_catalog.>(longblob, float8);
DROP OPERATOR IF EXISTS pg_catalog.>(float8, longblob);
DROP OPERATOR IF EXISTS pg_catalog.>=(longblob, float8);
DROP OPERATOR IF EXISTS pg_catalog.>=(float8, longblob);
DROP FUNCTION IF EXISTS pg_catalog.longblob_float8_eq(longblob, float8);
DROP FUNCTION IF EXISTS pg_catalog.float8_longblob_eq(float8, longblob);
DROP FUNCTION IF EXISTS pg_catalog.longblob_float8_ne(longblob, float8);
DROP FUNCTION IF EXISTS pg_catalog.float8_longblob_ne(float8, longblob);
DROP FUNCTION IF EXISTS pg_catalog.longblob_float8_lt(longblob, float8);
DROP FUNCTION IF EXISTS pg_catalog.float8_longblob_lt(float8, longblob);
DROP FUNCTION IF EXISTS pg_catalog.longblob_float8_le(longblob, float8);
DROP FUNCTION IF EXISTS pg_catalog.float8_longblob_le(float8, longblob);
DROP FUNCTION IF EXISTS pg_catalog.longblob_float8_gt(longblob, float8);
DROP FUNCTION IF EXISTS pg_catalog.float8_longblob_gt(float8, longblob);
DROP FUNCTION IF EXISTS pg_catalog.longblob_float8_ge(longblob, float8);
DROP FUNCTION IF EXISTS pg_catalog.float8_longblob_ge(float8, longblob);
DROP FUNCTION IF EXISTS pg_catalog.longblob_float8_cmp(longblob, float8);
DROP FUNCTION IF EXISTS pg_catalog.float8_longblob_cmp(float8, longblob);

DROP OPERATOR IF EXISTS pg_catalog.=(tinyblob, timestamp without time zone);
DROP OPERATOR IF EXISTS pg_catalog.=(timestamp without time zone, tinyblob);
DROP OPERATOR IF EXISTS pg_catalog.<>(tinyblob, timestamp without time zone);
DROP OPERATOR IF EXISTS pg_catalog.<>(timestamp without time zone, tinyblob);
DROP OPERATOR IF EXISTS pg_catalog.<(tinyblob, timestamp without time zone);
DROP OPERATOR IF EXISTS pg_catalog.<(timestamp without time zone, tinyblob);
DROP OPERATOR IF EXISTS pg_catalog.<=(tinyblob, timestamp without time zone);
DROP OPERATOR IF EXISTS pg_catalog.<=(timestamp without time zone, tinyblob);
DROP OPERATOR IF EXISTS pg_catalog.>(tinyblob, timestamp without time zone);
DROP OPERATOR IF EXISTS pg_catalog.>(timestamp without time zone, tinyblob);
DROP OPERATOR IF EXISTS pg_catalog.>=(tinyblob, timestamp without time zone);
DROP OPERATOR IF EXISTS pg_catalog.>=(timestamp without time zone, tinyblob);
DROP FUNCTION IF EXISTS pg_catalog.tinyblob_datetime_eq(tinyblob, timestamp without time zone);
DROP FUNCTION IF EXISTS pg_catalog.datetime_tinyblob_eq(timestamp without time zone, tinyblob);
DROP FUNCTION IF EXISTS pg_catalog.tinyblob_datetime_ne(tinyblob, timestamp without time zone);
DROP FUNCTION IF EXISTS pg_catalog.datetime_tinyblob_ne(timestamp without time zone, tinyblob);
DROP FUNCTION IF EXISTS pg_catalog.tinyblob_datetime_lt(tinyblob, timestamp without time zone);
DROP FUNCTION IF EXISTS pg_catalog.datetime_tinyblob_lt(timestamp without time zone, tinyblob);
DROP FUNCTION IF EXISTS pg_catalog.tinyblob_datetime_le(tinyblob, timestamp without time zone);
DROP FUNCTION IF EXISTS pg_catalog.datetime_tinyblob_le(timestamp without time zone, tinyblob);
DROP FUNCTION IF EXISTS pg_catalog.tinyblob_datetime_gt(tinyblob, timestamp without time zone);
DROP FUNCTION IF EXISTS pg_catalog.datetime_tinyblob_gt(timestamp without time zone, tinyblob);
DROP FUNCTION IF EXISTS pg_catalog.tinyblob_datetime_ge(tinyblob, timestamp without time zone);
DROP FUNCTION IF EXISTS pg_catalog.datetime_tinyblob_ge(timestamp without time zone, tinyblob);
DROP FUNCTION IF EXISTS pg_catalog.tinyblob_datetime_cmp(tinyblob, timestamp without time zone);
DROP FUNCTION IF EXISTS pg_catalog.datetime_tinyblob_cmp(timestamp without time zone, tinyblob);
DROP OPERATOR IF EXISTS pg_catalog.=(blob, timestamp without time zone);
DROP OPERATOR IF EXISTS pg_catalog.=(timestamp without time zone, blob);
DROP OPERATOR IF EXISTS pg_catalog.<>(blob, timestamp without time zone);
DROP OPERATOR IF EXISTS pg_catalog.<>(timestamp without time zone, blob);
DROP OPERATOR IF EXISTS pg_catalog.<(blob, timestamp without time zone);
DROP OPERATOR IF EXISTS pg_catalog.<(timestamp without time zone, blob);
DROP OPERATOR IF EXISTS pg_catalog.<=(blob, timestamp without time zone);
DROP OPERATOR IF EXISTS pg_catalog.<=(timestamp without time zone, blob);
DROP OPERATOR IF EXISTS pg_catalog.>(blob, timestamp without time zone);
DROP OPERATOR IF EXISTS pg_catalog.>(timestamp without time zone, blob);
DROP OPERATOR IF EXISTS pg_catalog.>=(blob, timestamp without time zone);
DROP OPERATOR IF EXISTS pg_catalog.>=(timestamp without time zone, blob);
DROP FUNCTION IF EXISTS pg_catalog.blob_datetime_eq(blob, timestamp without time zone);
DROP FUNCTION IF EXISTS pg_catalog.datetime_blob_eq(timestamp without time zone, blob);
DROP FUNCTION IF EXISTS pg_catalog.blob_datetime_ne(blob, timestamp without time zone);
DROP FUNCTION IF EXISTS pg_catalog.datetime_blob_ne(timestamp without time zone, blob);
DROP FUNCTION IF EXISTS pg_catalog.blob_datetime_lt(blob, timestamp without time zone);
DROP FUNCTION IF EXISTS pg_catalog.datetime_blob_lt(timestamp without time zone, blob);
DROP FUNCTION IF EXISTS pg_catalog.blob_datetime_le(blob, timestamp without time zone);
DROP FUNCTION IF EXISTS pg_catalog.datetime_blob_le(timestamp without time zone, blob);
DROP FUNCTION IF EXISTS pg_catalog.blob_datetime_gt(blob, timestamp without time zone);
DROP FUNCTION IF EXISTS pg_catalog.datetime_blob_gt(timestamp without time zone, blob);
DROP FUNCTION IF EXISTS pg_catalog.blob_datetime_ge(blob, timestamp without time zone);
DROP FUNCTION IF EXISTS pg_catalog.datetime_blob_ge(timestamp without time zone, blob);
DROP FUNCTION IF EXISTS pg_catalog.blob_datetime_cmp(blob, timestamp without time zone);
DROP FUNCTION IF EXISTS pg_catalog.datetime_blob_cmp(timestamp without time zone, blob);
DROP OPERATOR IF EXISTS pg_catalog.=(mediumblob, timestamp without time zone);
DROP OPERATOR IF EXISTS pg_catalog.=(timestamp without time zone, mediumblob);
DROP OPERATOR IF EXISTS pg_catalog.<>(mediumblob, timestamp without time zone);
DROP OPERATOR IF EXISTS pg_catalog.<>(timestamp without time zone, mediumblob);
DROP OPERATOR IF EXISTS pg_catalog.<(mediumblob, timestamp without time zone);
DROP OPERATOR IF EXISTS pg_catalog.<(timestamp without time zone, mediumblob);
DROP OPERATOR IF EXISTS pg_catalog.<=(mediumblob, timestamp without time zone);
DROP OPERATOR IF EXISTS pg_catalog.<=(timestamp without time zone, mediumblob);
DROP OPERATOR IF EXISTS pg_catalog.>(mediumblob, timestamp without time zone);
DROP OPERATOR IF EXISTS pg_catalog.>(timestamp without time zone, mediumblob);
DROP OPERATOR IF EXISTS pg_catalog.>=(mediumblob, timestamp without time zone);
DROP OPERATOR IF EXISTS pg_catalog.>=(timestamp without time zone, mediumblob);
DROP FUNCTION IF EXISTS pg_catalog.mediumblob_datetime_eq(mediumblob, timestamp without time zone);
DROP FUNCTION IF EXISTS pg_catalog.datetime_mediumblob_eq(timestamp without time zone, mediumblob);
DROP FUNCTION IF EXISTS pg_catalog.mediumblob_datetime_ne(mediumblob, timestamp without time zone);
DROP FUNCTION IF EXISTS pg_catalog.datetime_mediumblob_ne(timestamp without time zone, mediumblob);
DROP FUNCTION IF EXISTS pg_catalog.mediumblob_datetime_lt(mediumblob, timestamp without time zone);
DROP FUNCTION IF EXISTS pg_catalog.datetime_mediumblob_lt(timestamp without time zone, mediumblob);
DROP FUNCTION IF EXISTS pg_catalog.mediumblob_datetime_le(mediumblob, timestamp without time zone);
DROP FUNCTION IF EXISTS pg_catalog.datetime_mediumblob_le(timestamp without time zone, mediumblob);
DROP FUNCTION IF EXISTS pg_catalog.mediumblob_datetime_gt(mediumblob, timestamp without time zone);
DROP FUNCTION IF EXISTS pg_catalog.datetime_mediumblob_gt(timestamp without time zone, mediumblob);
DROP FUNCTION IF EXISTS pg_catalog.mediumblob_datetime_ge(mediumblob, timestamp without time zone);
DROP FUNCTION IF EXISTS pg_catalog.datetime_mediumblob_ge(timestamp without time zone, mediumblob);
DROP FUNCTION IF EXISTS pg_catalog.mediumblob_datetime_cmp(mediumblob, timestamp without time zone);
DROP FUNCTION IF EXISTS pg_catalog.datetime_mediumblob_cmp(timestamp without time zone, mediumblob);
DROP OPERATOR IF EXISTS pg_catalog.=(longblob, timestamp without time zone);
DROP OPERATOR IF EXISTS pg_catalog.=(timestamp without time zone, longblob);
DROP OPERATOR IF EXISTS pg_catalog.<>(longblob, timestamp without time zone);
DROP OPERATOR IF EXISTS pg_catalog.<>(timestamp without time zone, longblob);
DROP OPERATOR IF EXISTS pg_catalog.<(longblob, timestamp without time zone);
DROP OPERATOR IF EXISTS pg_catalog.<(timestamp without time zone, longblob);
DROP OPERATOR IF EXISTS pg_catalog.<=(longblob, timestamp without time zone);
DROP OPERATOR IF EXISTS pg_catalog.<=(timestamp without time zone, longblob);
DROP OPERATOR IF EXISTS pg_catalog.>(longblob, timestamp without time zone);
DROP OPERATOR IF EXISTS pg_catalog.>(timestamp without time zone, longblob);
DROP OPERATOR IF EXISTS pg_catalog.>=(longblob, timestamp without time zone);
DROP OPERATOR IF EXISTS pg_catalog.>=(timestamp without time zone, longblob);
DROP FUNCTION IF EXISTS pg_catalog.longblob_datetime_eq(longblob, timestamp without time zone);
DROP FUNCTION IF EXISTS pg_catalog.datetime_longblob_eq(timestamp without time zone, longblob);
DROP FUNCTION IF EXISTS pg_catalog.longblob_datetime_ne(longblob, timestamp without time zone);
DROP FUNCTION IF EXISTS pg_catalog.datetime_longblob_ne(timestamp without time zone, longblob);
DROP FUNCTION IF EXISTS pg_catalog.longblob_datetime_lt(longblob, timestamp without time zone);
DROP FUNCTION IF EXISTS pg_catalog.datetime_longblob_lt(timestamp without time zone, longblob);
DROP FUNCTION IF EXISTS pg_catalog.longblob_datetime_le(longblob, timestamp without time zone);
DROP FUNCTION IF EXISTS pg_catalog.datetime_longblob_le(timestamp without time zone, longblob);
DROP FUNCTION IF EXISTS pg_catalog.longblob_datetime_gt(longblob, timestamp without time zone);
DROP FUNCTION IF EXISTS pg_catalog.datetime_longblob_gt(timestamp without time zone, longblob);
DROP FUNCTION IF EXISTS pg_catalog.longblob_datetime_ge(longblob, timestamp without time zone);
DROP FUNCTION IF EXISTS pg_catalog.datetime_longblob_ge(timestamp without time zone, longblob);
DROP FUNCTION IF EXISTS pg_catalog.longblob_datetime_cmp(longblob, timestamp without time zone);
DROP FUNCTION IF EXISTS pg_catalog.datetime_longblob_cmp(timestamp without time zone, longblob);

DROP OPERATOR IF EXISTS pg_catalog.=(tinyblob, timestamptz);
DROP OPERATOR IF EXISTS pg_catalog.=(timestamptz, tinyblob);
DROP OPERATOR IF EXISTS pg_catalog.<>(tinyblob, timestamptz);
DROP OPERATOR IF EXISTS pg_catalog.<>(timestamptz, tinyblob);
DROP OPERATOR IF EXISTS pg_catalog.<(tinyblob, timestamptz);
DROP OPERATOR IF EXISTS pg_catalog.<(timestamptz, tinyblob);
DROP OPERATOR IF EXISTS pg_catalog.<=(tinyblob, timestamptz);
DROP OPERATOR IF EXISTS pg_catalog.<=(timestamptz, tinyblob);
DROP OPERATOR IF EXISTS pg_catalog.>(tinyblob, timestamptz);
DROP OPERATOR IF EXISTS pg_catalog.>(timestamptz, tinyblob);
DROP OPERATOR IF EXISTS pg_catalog.>=(tinyblob, timestamptz);
DROP OPERATOR IF EXISTS pg_catalog.>=(timestamptz, tinyblob);
DROP FUNCTION IF EXISTS pg_catalog.tinyblob_timestamp_eq(tinyblob, timestamptz);
DROP FUNCTION IF EXISTS pg_catalog.timestamp_tinyblob_eq(timestamptz, tinyblob);
DROP FUNCTION IF EXISTS pg_catalog.tinyblob_timestamp_ne(tinyblob, timestamptz);
DROP FUNCTION IF EXISTS pg_catalog.timestamp_tinyblob_ne(timestamptz, tinyblob);
DROP FUNCTION IF EXISTS pg_catalog.tinyblob_timestamp_lt(tinyblob, timestamptz);
DROP FUNCTION IF EXISTS pg_catalog.timestamp_tinyblob_lt(timestamptz, tinyblob);
DROP FUNCTION IF EXISTS pg_catalog.tinyblob_timestamp_le(tinyblob, timestamptz);
DROP FUNCTION IF EXISTS pg_catalog.timestamp_tinyblob_le(timestamptz, tinyblob);
DROP FUNCTION IF EXISTS pg_catalog.tinyblob_timestamp_gt(tinyblob, timestamptz);
DROP FUNCTION IF EXISTS pg_catalog.timestamp_tinyblob_gt(timestamptz, tinyblob);
DROP FUNCTION IF EXISTS pg_catalog.tinyblob_timestamp_ge(tinyblob, timestamptz);
DROP FUNCTION IF EXISTS pg_catalog.timestamp_tinyblob_ge(timestamptz, tinyblob);
DROP FUNCTION IF EXISTS pg_catalog.tinyblob_timestamp_cmp(tinyblob, timestamptz);
DROP FUNCTION IF EXISTS pg_catalog.timestamp_tinyblob_cmp(timestamptz, tinyblob);
DROP OPERATOR IF EXISTS pg_catalog.=(blob, timestamptz);
DROP OPERATOR IF EXISTS pg_catalog.=(timestamptz, blob);
DROP OPERATOR IF EXISTS pg_catalog.<>(blob, timestamptz);
DROP OPERATOR IF EXISTS pg_catalog.<>(timestamptz, blob);
DROP OPERATOR IF EXISTS pg_catalog.<(blob, timestamptz);
DROP OPERATOR IF EXISTS pg_catalog.<(timestamptz, blob);
DROP OPERATOR IF EXISTS pg_catalog.<=(blob, timestamptz);
DROP OPERATOR IF EXISTS pg_catalog.<=(timestamptz, blob);
DROP OPERATOR IF EXISTS pg_catalog.>(blob, timestamptz);
DROP OPERATOR IF EXISTS pg_catalog.>(timestamptz, blob);
DROP OPERATOR IF EXISTS pg_catalog.>=(blob, timestamptz);
DROP OPERATOR IF EXISTS pg_catalog.>=(timestamptz, blob);
DROP FUNCTION IF EXISTS pg_catalog.blob_timestamp_eq(blob, timestamptz);
DROP FUNCTION IF EXISTS pg_catalog.timestamp_blob_eq(timestamptz, blob);
DROP FUNCTION IF EXISTS pg_catalog.blob_timestamp_ne(blob, timestamptz);
DROP FUNCTION IF EXISTS pg_catalog.timestamp_blob_ne(timestamptz, blob);
DROP FUNCTION IF EXISTS pg_catalog.blob_timestamp_lt(blob, timestamptz);
DROP FUNCTION IF EXISTS pg_catalog.timestamp_blob_lt(timestamptz, blob);
DROP FUNCTION IF EXISTS pg_catalog.blob_timestamp_le(blob, timestamptz);
DROP FUNCTION IF EXISTS pg_catalog.timestamp_blob_le(timestamptz, blob);
DROP FUNCTION IF EXISTS pg_catalog.blob_timestamp_gt(blob, timestamptz);
DROP FUNCTION IF EXISTS pg_catalog.timestamp_blob_gt(timestamptz, blob);
DROP FUNCTION IF EXISTS pg_catalog.blob_timestamp_ge(blob, timestamptz);
DROP FUNCTION IF EXISTS pg_catalog.timestamp_blob_ge(timestamptz, blob);
DROP FUNCTION IF EXISTS pg_catalog.blob_timestamp_cmp(blob, timestamptz);
DROP FUNCTION IF EXISTS pg_catalog.timestamp_blob_cmp(timestamptz, blob);
DROP OPERATOR IF EXISTS pg_catalog.=(mediumblob, timestamptz);
DROP OPERATOR IF EXISTS pg_catalog.=(timestamptz, mediumblob);
DROP OPERATOR IF EXISTS pg_catalog.<>(mediumblob, timestamptz);
DROP OPERATOR IF EXISTS pg_catalog.<>(timestamptz, mediumblob);
DROP OPERATOR IF EXISTS pg_catalog.<(mediumblob, timestamptz);
DROP OPERATOR IF EXISTS pg_catalog.<(timestamptz, mediumblob);
DROP OPERATOR IF EXISTS pg_catalog.<=(mediumblob, timestamptz);
DROP OPERATOR IF EXISTS pg_catalog.<=(timestamptz, mediumblob);
DROP OPERATOR IF EXISTS pg_catalog.>(mediumblob, timestamptz);
DROP OPERATOR IF EXISTS pg_catalog.>(timestamptz, mediumblob);
DROP OPERATOR IF EXISTS pg_catalog.>=(mediumblob, timestamptz);
DROP OPERATOR IF EXISTS pg_catalog.>=(timestamptz, mediumblob);
DROP FUNCTION IF EXISTS pg_catalog.mediumblob_timestamp_eq(mediumblob, timestamptz);
DROP FUNCTION IF EXISTS pg_catalog.timestamp_mediumblob_eq(timestamptz, mediumblob);
DROP FUNCTION IF EXISTS pg_catalog.mediumblob_timestamp_ne(mediumblob, timestamptz);
DROP FUNCTION IF EXISTS pg_catalog.timestamp_mediumblob_ne(timestamptz, mediumblob);
DROP FUNCTION IF EXISTS pg_catalog.mediumblob_timestamp_lt(mediumblob, timestamptz);
DROP FUNCTION IF EXISTS pg_catalog.timestamp_mediumblob_lt(timestamptz, mediumblob);
DROP FUNCTION IF EXISTS pg_catalog.mediumblob_timestamp_le(mediumblob, timestamptz);
DROP FUNCTION IF EXISTS pg_catalog.timestamp_mediumblob_le(timestamptz, mediumblob);
DROP FUNCTION IF EXISTS pg_catalog.mediumblob_timestamp_gt(mediumblob, timestamptz);
DROP FUNCTION IF EXISTS pg_catalog.timestamp_mediumblob_gt(timestamptz, mediumblob);
DROP FUNCTION IF EXISTS pg_catalog.mediumblob_timestamp_ge(mediumblob, timestamptz);
DROP FUNCTION IF EXISTS pg_catalog.timestamp_mediumblob_ge(timestamptz, mediumblob);
DROP FUNCTION IF EXISTS pg_catalog.mediumblob_timestamp_cmp(mediumblob, timestamptz);
DROP FUNCTION IF EXISTS pg_catalog.timestamp_mediumblob_cmp(timestamptz, mediumblob);
DROP OPERATOR IF EXISTS pg_catalog.=(longblob, timestamptz);
DROP OPERATOR IF EXISTS pg_catalog.=(timestamptz, longblob);
DROP OPERATOR IF EXISTS pg_catalog.<>(longblob, timestamptz);
DROP OPERATOR IF EXISTS pg_catalog.<>(timestamptz, longblob);
DROP OPERATOR IF EXISTS pg_catalog.<(longblob, timestamptz);
DROP OPERATOR IF EXISTS pg_catalog.<(timestamptz, longblob);
DROP OPERATOR IF EXISTS pg_catalog.<=(longblob, timestamptz);
DROP OPERATOR IF EXISTS pg_catalog.<=(timestamptz, longblob);
DROP OPERATOR IF EXISTS pg_catalog.>(longblob, timestamptz);
DROP OPERATOR IF EXISTS pg_catalog.>(timestamptz, longblob);
DROP OPERATOR IF EXISTS pg_catalog.>=(longblob, timestamptz);
DROP OPERATOR IF EXISTS pg_catalog.>=(timestamptz, longblob);
DROP FUNCTION IF EXISTS pg_catalog.longblob_timestamp_eq(longblob, timestamptz);
DROP FUNCTION IF EXISTS pg_catalog.timestamp_longblob_eq(timestamptz, longblob);
DROP FUNCTION IF EXISTS pg_catalog.longblob_timestamp_ne(longblob, timestamptz);
DROP FUNCTION IF EXISTS pg_catalog.timestamp_longblob_ne(timestamptz, longblob);
DROP FUNCTION IF EXISTS pg_catalog.longblob_timestamp_lt(longblob, timestamptz);
DROP FUNCTION IF EXISTS pg_catalog.timestamp_longblob_lt(timestamptz, longblob);
DROP FUNCTION IF EXISTS pg_catalog.longblob_timestamp_le(longblob, timestamptz);
DROP FUNCTION IF EXISTS pg_catalog.timestamp_longblob_le(timestamptz, longblob);
DROP FUNCTION IF EXISTS pg_catalog.longblob_timestamp_gt(longblob, timestamptz);
DROP FUNCTION IF EXISTS pg_catalog.timestamp_longblob_gt(timestamptz, longblob);
DROP FUNCTION IF EXISTS pg_catalog.longblob_timestamp_ge(longblob, timestamptz);
DROP FUNCTION IF EXISTS pg_catalog.timestamp_longblob_ge(timestamptz, longblob);
DROP FUNCTION IF EXISTS pg_catalog.longblob_timestamp_cmp(longblob, timestamptz);
DROP FUNCTION IF EXISTS pg_catalog.timestamp_longblob_cmp(timestamptz, longblob);

DROP AGGREGATE IF EXISTS pg_catalog.stddev_pop(json);
DROP AGGREGATE IF EXISTS pg_catalog.var_pop(json);
DROP FUNCTION IF EXISTS pg_catalog.any_accum(numeric[], anyelement);

DROP FUNCTION IF EXISTS pg_catalog.upper(int8);
DROP FUNCTION IF EXISTS pg_catalog.upper(numeric);
DROP FUNCTION IF EXISTS pg_catalog.upper(date);
DROP FUNCTION IF EXISTS pg_catalog.upper(time);
DROP FUNCTION IF EXISTS pg_catalog.upper(timestamp(0) without time zone);
DROP FUNCTION IF EXISTS pg_catalog.upper(timestamp(0) with time zone);
DROP FUNCTION IF EXISTS pg_catalog.upper(bit);
DROP FUNCTION IF EXISTS pg_catalog.upper(boolean);
DROP FUNCTION IF EXISTS pg_catalog.upper(year);
DROP FUNCTION IF EXISTS pg_catalog.upper(char);
DROP FUNCTION IF EXISTS pg_catalog.upper(varchar);
DROP FUNCTION IF EXISTS pg_catalog.upper(tinyblob);
DROP FUNCTION IF EXISTS pg_catalog.upper(binary);
DROP FUNCTION IF EXISTS pg_catalog.upper(varbinary);
DROP FUNCTION IF EXISTS pg_catalog.upper(blob);
DROP FUNCTION IF EXISTS pg_catalog.upper(mediumblob);
DROP FUNCTION IF EXISTS pg_catalog.upper(longblob);
DROP FUNCTION IF EXISTS pg_catalog.upper(anyenum);
DROP FUNCTION IF EXISTS pg_catalog.upper(anyset);
DROP FUNCTION IF EXISTS pg_catalog.upper(json);

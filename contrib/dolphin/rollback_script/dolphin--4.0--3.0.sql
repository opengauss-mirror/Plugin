DROP FUNCTION IF EXISTS pg_catalog.length(boolean);
DROP FUNCTION IF EXISTS pg_catalog.length(tinyblob);
DROP FUNCTION IF EXISTS pg_catalog.length(blob);
DROP FUNCTION IF EXISTS pg_catalog.length(mediumblob);
DROP FUNCTION IF EXISTS pg_catalog.length(longblob);
DROP FUNCTION IF EXISTS pg_catalog.length(anyenum);
DROP FUNCTION IF EXISTS pg_catalog.length(json);

DROP FUNCTION IF EXISTS pg_catalog.octet_length(boolean);
DROP FUNCTION IF EXISTS pg_catalog.octet_length(year);
DROP FUNCTION IF EXISTS pg_catalog.octet_length(blob);
DROP FUNCTION IF EXISTS pg_catalog.octet_length(anyenum);
DROP FUNCTION IF EXISTS pg_catalog.octet_length(json);

DROP FUNCTION IF EXISTS pg_catalog.position(boolean, text);
DROP FUNCTION IF EXISTS pg_catalog.position(binary, text);
DROP FUNCTION IF EXISTS pg_catalog.position(varbinary, text);

DROP FUNCTION IF EXISTS pg_catalog.instr(binary, text);
DROP FUNCTION IF EXISTS pg_catalog.instr(varbinary, text);
DROP FUNCTION IF EXISTS pg_catalog.instr(boolean, text, integer);
DROP FUNCTION IF EXISTS pg_catalog.instr(bit, bit, integer);
DROP FUNCTION IF EXISTS pg_catalog.instr(bytea, bytea, integer);
DROP FUNCTION IF EXISTS pg_catalog.instr(binary, text, integer);
DROP FUNCTION IF EXISTS pg_catalog.instr(varbinary, text, integer);

DROP FUNCTION IF EXISTS pg_catalog.locate(boolean, text);
DROP FUNCTION IF EXISTS pg_catalog.locate(bit, bit);
DROP FUNCTION IF EXISTS pg_catalog.locate(binary, text);
DROP FUNCTION IF EXISTS pg_catalog.locate(varbinary, text);
DROP FUNCTION IF EXISTS pg_catalog.locate(boolean, text, integer);
DROP FUNCTION IF EXISTS pg_catalog.locate(bit, bit, integer);
DROP FUNCTION IF EXISTS pg_catalog.locate(bytea, bytea, integer);
DROP FUNCTION IF EXISTS pg_catalog.locate(binary, text, integer);
DROP FUNCTION IF EXISTS pg_catalog.locate(varbinary, text, integer);

DROP FUNCTION IF EXISTS pg_catalog.mid(boolean, int);
DROP FUNCTION IF EXISTS pg_catalog.mid(boolean, int, int);
DROP FUNCTION IF EXISTS pg_catalog.mid(binary, int);
DROP FUNCTION IF EXISTS pg_catalog.mid(binary, int, int);
DROP FUNCTION IF EXISTS pg_catalog.mid(varbinary, int);
DROP FUNCTION IF EXISTS pg_catalog.mid(varbinary, int, int);
DROP FUNCTION IF EXISTS pg_catalog.mid(bit, int);
DROP FUNCTION IF EXISTS pg_catalog.mid(bit, int, int);

DROP FUNCTION IF EXISTS pg_catalog.log10(float8);
CREATE OR REPLACE FUNCTION pg_catalog.log10(float8)
RETURNS float8
AS
$$
BEGIN
    IF $1 <= 0 THEN
        RETURN NULL;
    end if;
    RETURN (SELECT dlog10($1));
END;
$$
LANGUAGE plpgsql;

DROP FUNCTION IF EXISTS pg_catalog.mod(binary,int8);
DROP FUNCTION IF EXISTS pg_catalog.mod(binary,bit);
DROP FUNCTION IF EXISTS pg_catalog.mod(binary,year);
DROP FUNCTION IF EXISTS pg_catalog.mod(binary,binary);
DROP FUNCTION IF EXISTS pg_catalog.mod(binary,varbinary);
DROP FUNCTION IF EXISTS pg_catalog.mod(int8,binary);
DROP FUNCTION IF EXISTS pg_catalog.mod(bit,binary);
DROP FUNCTION IF EXISTS pg_catalog.mod(year,binary);
DROP FUNCTION IF EXISTS pg_catalog.mod(varbinary,varbinary);
DROP FUNCTION IF EXISTS pg_catalog.mod(varbinary,binary);
DROP FUNCTION IF EXISTS pg_catalog.mod(varbinary,int8);
DROP FUNCTION IF EXISTS pg_catalog.mod(varbinary,bit);
DROP FUNCTION IF EXISTS pg_catalog.mod(varbinary,year);
DROP FUNCTION IF EXISTS pg_catalog.mod(int8,varbinary);
DROP FUNCTION IF EXISTS pg_catalog.mod(bit,varbinary);
DROP FUNCTION IF EXISTS pg_catalog.mod(year,varbinary);

DROP FUNCTION IF EXISTS pg_catalog.lower(int8);
DROP FUNCTION IF EXISTS pg_catalog.lower(uint8);
DROP FUNCTION IF EXISTS pg_catalog.lower(float4);
DROP FUNCTION IF EXISTS pg_catalog.lower(numeric);

DROP FUNCTION IF EXISTS pg_catalog.lcase(int8);
DROP FUNCTION IF EXISTS pg_catalog.lcase(uint8);
DROP FUNCTION IF EXISTS pg_catalog.lcase(float4);
DROP FUNCTION IF EXISTS pg_catalog.lcase(numeric);

DROP FUNCTION IF EXISTS pg_catalog.cot(bit);
DROP FUNCTION IF EXISTS pg_catalog.cot(boolean);
DROP FUNCTION IF EXISTS pg_catalog.acos(bit);
DROP FUNCTION IF EXISTS pg_catalog.cos(bit);
DROP FUNCTION IF EXISTS pg_catalog.cos(boolean);
DROP FUNCTION IF EXISTS pg_catalog.asin(bit);
DROP FUNCTION IF EXISTS pg_catalog.asin(boolean);
DROP FUNCTION IF EXISTS pg_catalog.atan(bit);

DROP FUNCTION IF EXISTS pg_catalog.year(integer);
DROP FUNCTION IF EXISTS pg_catalog.year(anyset);

DROP FUNCTION IF EXISTS pg_catalog.hour(bit);
DROP FUNCTION IF EXISTS pg_catalog.hour(blob);
DROP FUNCTION IF EXISTS pg_catalog.hour(boolean);
DROP FUNCTION IF EXISTS pg_catalog.hour(json);
DROP FUNCTION IF EXISTS pg_catalog.hour(integer);

DROP FUNCTION IF EXISTS pg_catalog.minute(bit);
DROP FUNCTION IF EXISTS pg_catalog.minute(blob);
DROP FUNCTION IF EXISTS pg_catalog.minute(boolean);
DROP FUNCTION IF EXISTS pg_catalog.minute(json);
DROP FUNCTION IF EXISTS pg_catalog.minute(integer);

DROP FUNCTION IF EXISTS pg_catalog.second(bit);
DROP FUNCTION IF EXISTS pg_catalog.second(blob);
DROP FUNCTION IF EXISTS pg_catalog.second(boolean);
DROP FUNCTION IF EXISTS pg_catalog.second(json);
DROP FUNCTION IF EXISTS pg_catalog.second(integer);

DROP FUNCTION IF EXISTS pg_catalog.microsecond(year);
DROP FUNCTION IF EXISTS pg_catalog.microsecond(bit);
DROP FUNCTION IF EXISTS pg_catalog.microsecond(blob);
DROP FUNCTION IF EXISTS pg_catalog.microsecond(boolean);
DROP FUNCTION IF EXISTS pg_catalog.microsecond(json);
DROP FUNCTION IF EXISTS pg_catalog.microsecond(integer);
DROP FUNCTION IF EXISTS pg_catalog.microsecond(float);

CREATE OR REPLACE FUNCTION pg_catalog.year(int4) RETURNS int8 LANGUAGE SQL STABLE STRICT as 'SELECT year($1::text)';

CREATE FUNCTION pg_catalog.unknown_concat(unknown, unknown) RETURNS text LANGUAGE SQL IMMUTABLE STRICT as 'select concat($1, $2)';
CREATE FUNCTION pg_catalog.unknown_int_concat(unknown, integer) RETURNS text LANGUAGE SQL IMMUTABLE STRICT as 'select concat($1, $2)';
CREATE FUNCTION pg_catalog.int_unknown_concat(integer, unknown) RETURNS text LANGUAGE SQL IMMUTABLE STRICT as 'select concat($1, $2)';

CREATE OPERATOR pg_catalog.||(leftarg=unknown, rightarg=unknown, procedure=pg_catalog.unknown_concat);
CREATE OPERATOR pg_catalog.||(leftarg=unknown, rightarg=integer, procedure=pg_catalog.unknown_int_concat);
CREATE OPERATOR pg_catalog.||(leftarg=integer, rightarg=unknown, procedure=pg_catalog.int_unknown_concat);

CREATE OR REPLACE FUNCTION pg_catalog.datetime_text_eq(arg1 timestamp without time zone, arg2 text) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1=$2::timestamp without time zone';
CREATE OR REPLACE FUNCTION pg_catalog.datetime_text_ne(arg1 timestamp without time zone, arg2 text) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1<>$2::timestamp without time zone';
CREATE OR REPLACE FUNCTION pg_catalog.datetime_text_lt(arg1 timestamp without time zone, arg2 text) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1<$2::timestamp without time zone';
CREATE OR REPLACE FUNCTION pg_catalog.datetime_text_le(arg1 timestamp without time zone, arg2 text) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1<=$2::timestamp without time zone';
CREATE OR REPLACE FUNCTION pg_catalog.datetime_text_gt(arg1 timestamp without time zone, arg2 text) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1>$2::timestamp without time zone';
CREATE OR REPLACE FUNCTION pg_catalog.datetime_text_ge(arg1 timestamp without time zone, arg2 text) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1>=$2::timestamp without time zone';

CREATE OR REPLACE FUNCTION pg_catalog.text_datetime_eq(arg1 text, arg2 timestamp without time zone) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT as 'select $1::timestamp without time zone=$2';
CREATE OR REPLACE FUNCTION pg_catalog.text_datetime_ne(arg1 text, arg2 timestamp without time zone) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1::timestamp without time zone<>$2';
CREATE OR REPLACE FUNCTION pg_catalog.text_datetime_lt(arg1 text, arg2 timestamp without time zone) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1::timestamp without time zone<$2';
CREATE OR REPLACE FUNCTION pg_catalog.text_datetime_le(arg1 text, arg2 timestamp without time zone) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1::timestamp without time zone<=$2';
CREATE OR REPLACE FUNCTION pg_catalog.text_datetime_gt(arg1 text, arg2 timestamp without time zone) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1::timestamp without time zone>$2';
CREATE OR REPLACE FUNCTION pg_catalog.text_datetime_ge(arg1 text, arg2 timestamp without time zone) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1::timestamp without time zone>=$2';

CREATE OR REPLACE FUNCTION pg_catalog.timestamp_text_eq(arg1 timestamptz, arg2 text) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1=$2::timestamptz';
CREATE OR REPLACE FUNCTION pg_catalog.timestamp_text_ne(arg1 timestamptz, arg2 text) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1<>$2::timestamptz';
CREATE OR REPLACE FUNCTION pg_catalog.timestamp_text_lt(arg1 timestamptz, arg2 text) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1<$2::timestamptz';
CREATE OR REPLACE FUNCTION pg_catalog.timestamp_text_le(arg1 timestamptz, arg2 text) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1<=$2::timestamptz';
CREATE OR REPLACE FUNCTION pg_catalog.timestamp_text_gt(arg1 timestamptz, arg2 text) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1>$2::timestamptz';
CREATE OR REPLACE FUNCTION pg_catalog.timestamp_text_ge(arg1 timestamptz, arg2 text) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1>=$2::timestamptz';

CREATE OR REPLACE FUNCTION pg_catalog.text_timestamp_eq(arg1 text, arg2 timestamptz) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT as 'select $1::timestamptz=$2';
CREATE OR REPLACE FUNCTION pg_catalog.text_timestamp_ne(arg1 text, arg2 timestamptz) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1::timestamptz<>$2';
CREATE OR REPLACE FUNCTION pg_catalog.text_timestamp_lt(arg1 text, arg2 timestamptz) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1::timestamptz<$2';
CREATE OR REPLACE FUNCTION pg_catalog.text_timestamp_le(arg1 text, arg2 timestamptz) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1::timestamptz<=$2';
CREATE OR REPLACE FUNCTION pg_catalog.text_timestamp_gt(arg1 text, arg2 timestamptz) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1::timestamptz>$2';
CREATE OR REPLACE FUNCTION pg_catalog.text_timestamp_ge(arg1 text, arg2 timestamptz) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1::timestamptz>=$2';

DO $for_og_502$
BEGIN
    -- workering_version_num of openGauss5.0.1 is 92854, openGauss5.1.0 is 92913.
    if working_version_num() < 92855 or working_version_num() >= 92913 then
        DROP OPERATOR IF EXISTS pg_catalog.=(timestamp without time zone, text);
        DROP OPERATOR IF EXISTS pg_catalog.<(timestamp without time zone, text);
        DROP OPERATOR IF EXISTS pg_catalog.<=(timestamp without time zone, text);
        DROP OPERATOR IF EXISTS pg_catalog.>(timestamp without time zone, text);
        DROP OPERATOR IF EXISTS pg_catalog.>=(timestamp without time zone, text);
        DROP OPERATOR IF EXISTS pg_catalog.<>(timestamp without time zone, text);
        DROP OPERATOR IF EXISTS pg_catalog.=(text, timestamp without time zone);
        DROP OPERATOR IF EXISTS pg_catalog.<(text, timestamp without time zone);
        DROP OPERATOR IF EXISTS pg_catalog.<=(text, timestamp without time zone);
        DROP OPERATOR IF EXISTS pg_catalog.>(text, timestamp without time zone);
        DROP OPERATOR IF EXISTS pg_catalog.>=(text, timestamp without time zone);
        DROP OPERATOR IF EXISTS pg_catalog.<>(text, timestamp without time zone);
        DROP OPERATOR IF EXISTS pg_catalog.=(timestamptz, text);
        DROP OPERATOR IF EXISTS pg_catalog.<(timestamptz, text);
        DROP OPERATOR IF EXISTS pg_catalog.<=(timestamptz, text);
        DROP OPERATOR IF EXISTS pg_catalog.>(timestamptz, text);
        DROP OPERATOR IF EXISTS pg_catalog.>=(timestamptz, text);
        DROP OPERATOR IF EXISTS pg_catalog.<>(timestamptz, text);
        DROP OPERATOR IF EXISTS pg_catalog.=(text, timestamptz);
        DROP OPERATOR IF EXISTS pg_catalog.<(text, timestamptz);
        DROP OPERATOR IF EXISTS pg_catalog.<=(text, timestamptz);
        DROP OPERATOR IF EXISTS pg_catalog.>(text, timestamptz);
        DROP OPERATOR IF EXISTS pg_catalog.>=(text, timestamptz);
        DROP OPERATOR IF EXISTS pg_catalog.<>(text, timestamptz);

        DROP FUNCTION IF EXISTS pg_catalog.datetime_text_eq(timestamp without time zone, text);
        DROP FUNCTION IF EXISTS pg_catalog.datetime_text_ne(timestamp without time zone, text);
        DROP FUNCTION IF EXISTS pg_catalog.datetime_text_lt(timestamp without time zone, text);
        DROP FUNCTION IF EXISTS pg_catalog.datetime_text_le(timestamp without time zone, text);
        DROP FUNCTION IF EXISTS pg_catalog.datetime_text_gt(timestamp without time zone, text);
        DROP FUNCTION IF EXISTS pg_catalog.datetime_text_ge(timestamp without time zone, text);
        DROP FUNCTION IF EXISTS pg_catalog.text_datetime_eq(text, timestamp without time zone);
        DROP FUNCTION IF EXISTS pg_catalog.text_datetime_ne(text, timestamp without time zone);
        DROP FUNCTION IF EXISTS pg_catalog.text_datetime_lt(text, timestamp without time zone);
        DROP FUNCTION IF EXISTS pg_catalog.text_datetime_le(text, timestamp without time zone);
        DROP FUNCTION IF EXISTS pg_catalog.text_datetime_gt(text, timestamp without time zone);
        DROP FUNCTION IF EXISTS pg_catalog.text_datetime_ge(text, timestamp without time zone);
        DROP FUNCTION IF EXISTS pg_catalog.timestamp_text_eq(timestamptz, text);
        DROP FUNCTION IF EXISTS pg_catalog.timestamp_text_ne(timestamptz, text);
        DROP FUNCTION IF EXISTS pg_catalog.timestamp_text_lt(timestamptz, text);
        DROP FUNCTION IF EXISTS pg_catalog.timestamp_text_le(timestamptz, text);
        DROP FUNCTION IF EXISTS pg_catalog.timestamp_text_gt(timestamptz, text);
        DROP FUNCTION IF EXISTS pg_catalog.timestamp_text_ge(timestamptz, text);
        DROP FUNCTION IF EXISTS pg_catalog.text_timestamp_eq(text, timestamptz);
        DROP FUNCTION IF EXISTS pg_catalog.text_timestamp_ne(text, timestamptz);
        DROP FUNCTION IF EXISTS pg_catalog.text_timestamp_lt(text, timestamptz);
        DROP FUNCTION IF EXISTS pg_catalog.text_timestamp_le(text, timestamptz);
        DROP FUNCTION IF EXISTS pg_catalog.text_timestamp_gt(text, timestamptz);
        DROP FUNCTION IF EXISTS pg_catalog.text_timestamp_ge(text, timestamptz);
    end if;
END
$for_og_502$;

DROP FUNCTION IF EXISTS pg_catalog.convert_text_datetime(text);
DROP FUNCTION IF EXISTS pg_catalog.convert_text_timestamptz(text);

DROP OPERATOR IF EXISTS pg_catalog.~~(anyenum, text);
DROP OPERATOR IF EXISTS pg_catalog.~~(text, anyenum);
DROP OPERATOR IF EXISTS pg_catalog.!~~(anyenum, text);
DROP OPERATOR IF EXISTS pg_catalog.!~~(text, anyenum);
DROP FUNCTION IF EXISTS pg_catalog.enumtext_like(anyenum, text);
DROP FUNCTION IF EXISTS pg_catalog.textenum_like(text, anyenum);
DROP FUNCTION IF EXISTS pg_catalog.enumtext_nlike(anyenum, text);
DROP FUNCTION IF EXISTS pg_catalog.textenum_nlike(text, anyenum);

DROP FUNCTION IF EXISTS pg_catalog.blob_to_float8(blob);
CREATE OR REPLACE FUNCTION pg_catalog.tinyblob_float8_eq(arg1 tinyblob, arg2 float8) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT $1::float8 = $2';
CREATE OR REPLACE FUNCTION pg_catalog.float8_tinyblob_eq(arg1 float8, arg2 tinyblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT $1 = $2::float8';
CREATE OR REPLACE FUNCTION pg_catalog.mediumblob_float8_eq(arg1 mediumblob, arg2 float8) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT $1::float8 = $2';
CREATE OR REPLACE FUNCTION pg_catalog.float8_mediumblob_eq(arg1 float8, arg2 mediumblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT $1 = $2::float8';
CREATE OR REPLACE FUNCTION pg_catalog.blob_float8_eq(arg1 blob, arg2 float8) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT $1::float8 = $2';
CREATE OR REPLACE FUNCTION pg_catalog.float8_blob_eq(arg1 float8, arg2 blob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT $1 = $2::float8';
CREATE OR REPLACE FUNCTION pg_catalog.longblob_float8_eq(arg1 longblob, arg2 float8) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT $1::float8 = $2';
CREATE OR REPLACE FUNCTION pg_catalog.float8_longblob_eq(arg1 float8, arg2 longblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT $1 = $2::float8';

CREATE OR REPLACE FUNCTION pg_catalog.tinyblob_numeric_eq(arg1 tinyblob, arg2 numeric) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT $1::float8 = $2';
CREATE OR REPLACE FUNCTION pg_catalog.numeric_tinyblob_eq(arg1 numeric, arg2 tinyblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT $1 = $2::float8';
CREATE OR REPLACE FUNCTION pg_catalog.mediumblob_numeric_eq(arg1 mediumblob, arg2 numeric) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT $1::float8 = $2';
CREATE OR REPLACE FUNCTION pg_catalog.numeric_mediumblob_eq(arg1 numeric, arg2 mediumblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT $1 = $2::float8';
CREATE OR REPLACE FUNCTION pg_catalog.blob_numeric_eq(arg1 blob, arg2 numeric) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT $1::float8 = $2';
CREATE OR REPLACE FUNCTION pg_catalog.numeric_blob_eq(arg1 numeric, arg2 blob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT $1 = $2::float8';
CREATE OR REPLACE FUNCTION pg_catalog.longblob_numeric_eq(arg1 longblob, arg2 numeric) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT $1::float8 = $2';
CREATE OR REPLACE FUNCTION pg_catalog.numeric_longblob_eq(arg1 numeric, arg2 longblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT $1 = $2::float8';

CREATE OR REPLACE FUNCTION pg_catalog.tinyblob_uint8_eq(arg1 tinyblob, arg2 uint8) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT $1::float8 = $2';
CREATE OR REPLACE FUNCTION pg_catalog.uint8_tinyblob_eq(arg1 uint8, arg2 tinyblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT $1 = $2::float8';
CREATE OR REPLACE FUNCTION pg_catalog.mediumblob_uint8_eq(arg1 mediumblob, arg2 uint8) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT $1::float8 = $2';
CREATE OR REPLACE FUNCTION pg_catalog.uint8_mediumblob_eq(arg1 uint8, arg2 mediumblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT $1 = $2::float8';
CREATE OR REPLACE FUNCTION pg_catalog.blob_uint8_eq(arg1 blob, arg2 uint8) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT $1::float8 = $2';
CREATE OR REPLACE FUNCTION pg_catalog.uint8_blob_eq(arg1 uint8, arg2 blob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT $1 = $2::float8';
CREATE OR REPLACE FUNCTION pg_catalog.longblob_uint8_eq(arg1 longblob, arg2 uint8) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT $1::float8 = $2';
CREATE OR REPLACE FUNCTION pg_catalog.uint8_longblob_eq(arg1 uint8, arg2 longblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT $1 = $2::float8';

CREATE OR REPLACE FUNCTION pg_catalog.tinyblob_int8_eq(arg1 tinyblob, arg2 int8) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT $1::float8 = $2';
CREATE OR REPLACE FUNCTION pg_catalog.int8_tinyblob_eq(arg1 int8, arg2 tinyblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT $1 = $2::float8';
CREATE OR REPLACE FUNCTION pg_catalog.mediumblob_int8_eq(arg1 mediumblob, arg2 int8) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT $1::float8 = $2';
CREATE OR REPLACE FUNCTION pg_catalog.int8_mediumblob_eq(arg1 int8, arg2 mediumblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT $1 = $2::float8';
CREATE OR REPLACE FUNCTION pg_catalog.blob_int8_eq(arg1 blob, arg2 int8) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT $1::float8 = $2';
CREATE OR REPLACE FUNCTION pg_catalog.int8_blob_eq(arg1 int8, arg2 blob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT $1 = $2::float8';
CREATE OR REPLACE FUNCTION pg_catalog.longblob_int8_eq(arg1 longblob, arg2 int8) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT $1::float8 = $2';
CREATE OR REPLACE FUNCTION pg_catalog.int8_longblob_eq(arg1 int8, arg2 longblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT $1 = $2::float8';

DROP CAST IF EXISTS (text AS tinyblob) CASCADE;
DROP CAST IF EXISTS (text AS blob) CASCADE;
DROP CAST IF EXISTS (text AS mediumblob) CASCADE;
DROP CAST IF EXISTS (text AS longblob) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.text_to_tinyblob(text);
DROP FUNCTION IF EXISTS pg_catalog.text_to_blob(text);
DROP FUNCTION IF EXISTS pg_catalog.text_to_mediumblob(text);
DROP FUNCTION IF EXISTS pg_catalog.text_to_longblob(text);

DROP OPERATOR CLASS IF EXISTS pg_catalog.uint1_ops USING ubtree;
DROP OPERATOR CLASS IF EXISTS pg_catalog.uint2_ops USING ubtree;
DROP OPERATOR CLASS IF EXISTS pg_catalog.uint4_ops USING ubtree;
DROP OPERATOR CLASS IF EXISTS pg_catalog.uint8_ops USING ubtree;

DROP FUNCTION IF EXISTS pg_catalog.chara(variadic arr "any") cascade;
CREATE OR REPLACE FUNCTION pg_catalog.chara(variadic arr "any") returns text LANGUAGE C IMMUTABLE as '$libdir/dolphin', 'm_char';

DROP FUNCTION IF EXISTS pg_catalog.lpad(boolean, integer, text);
DROP FUNCTION IF EXISTS pg_catalog.lpad(bit, integer, text);
DROP FUNCTION IF EXISTS pg_catalog.lpad(binary, integer, text);
DROP FUNCTION IF EXISTS pg_catalog.lpad(varbinary, integer, text);

DROP FUNCTION IF EXISTS pg_catalog.quarter (timestamptz);
DROP FUNCTION IF EXISTS pg_catalog.quarter (timetz);
DROP FUNCTION IF EXISTS pg_catalog.quarter (abstime);
DROP FUNCTION IF EXISTS pg_catalog.quarter (date);
DROP FUNCTION IF EXISTS pg_catalog.quarter (time);
DROP FUNCTION IF EXISTS pg_catalog.quarter (timestamp(0) with time zone);
DROP FUNCTION IF EXISTS pg_catalog.quarter (year);
DROP FUNCTION IF EXISTS pg_catalog.quarter (binary);
DROP FUNCTION IF EXISTS pg_catalog.quarter (text);
DROP CAST IF EXISTS (binary as timestamp without time zone);
DROP CAST IF EXISTS (binary as timestamp with time zone);
DROP FUNCTION IF EXISTS pg_catalog.binary_timestamp (binary);
DROP FUNCTION IF EXISTS pg_catalog.binary_timestamptz (binary);
CREATE CAST ("binary" AS timestamp without time zone) WITH FUNCTION pg_catalog.Varlena2Datetime(anyelement) AS ASSIGNMENT;
CREATE CAST ("binary" AS timestamp with time zone) WITH FUNCTION pg_catalog.Varlena2Timestamptz(anyelement) AS ASSIGNMENT;
CREATE OR REPLACE FUNCTION pg_catalog.quarter (timestamptz) RETURNS float8 LANGUAGE SQL STABLE STRICT as 'select pg_catalog.date_part(''quarter'', $1)';
CREATE OR REPLACE FUNCTION pg_catalog.quarter (timetz) RETURNS float8 LANGUAGE SQL STABLE STRICT as 'select pg_catalog.date_part(''quarter'', $1)';
CREATE OR REPLACE FUNCTION pg_catalog.quarter (abstime) RETURNS float8 LANGUAGE SQL STABLE STRICT as 'select pg_catalog.date_part(''quarter'', $1)';
CREATE OR REPLACE FUNCTION pg_catalog.quarter (date) RETURNS float8 LANGUAGE SQL STABLE STRICT as 'select pg_catalog.date_part(''quarter'', $1)';
CREATE OR REPLACE FUNCTION pg_catalog.quarter (time) RETURNS float8 LANGUAGE SQL STABLE STRICT as 'select pg_catalog.date_part(''quarter'', $1)';
CREATE OR REPLACE FUNCTION pg_catalog.quarter (timestamp(0) with time zone) RETURNS float8 LANGUAGE SQL STABLE STRICT as 'select pg_catalog.date_part(''quarter'', $1)';

DROP FUNCTION IF EXISTS pg_catalog.weekday (timestamptz);
DROP FUNCTION IF EXISTS pg_catalog.weekday (timetz);
DROP FUNCTION IF EXISTS pg_catalog.weekday (abstime);
DROP FUNCTION IF EXISTS pg_catalog.weekday (date);
DROP FUNCTION IF EXISTS pg_catalog.weekday (time);
DROP FUNCTION IF EXISTS pg_catalog.weekday (timestamp(0) with time zone);
DROP FUNCTION IF EXISTS pg_catalog.weekday (year);
DROP FUNCTION IF EXISTS pg_catalog.weekday (binary);
DROP FUNCTION IF EXISTS pg_catalog.weekday (text);
CREATE OR REPLACE FUNCTION pg_catalog.weekday (timestamptz) RETURNS float8 LANGUAGE SQL STABLE STRICT as 'select pg_catalog.date_part(''isodow'', $1) - 1';
CREATE OR REPLACE FUNCTION pg_catalog.weekday (timetz) RETURNS float8 LANGUAGE SQL STABLE STRICT as 'select pg_catalog.date_part(''isodow'', $1) - 1';
CREATE OR REPLACE FUNCTION pg_catalog.weekday (abstime) RETURNS float8 LANGUAGE SQL STABLE STRICT as 'select pg_catalog.date_part(''isodow'', $1) - 1';
CREATE OR REPLACE FUNCTION pg_catalog.weekday (date) RETURNS float8 LANGUAGE SQL STABLE STRICT as 'select pg_catalog.date_part(''isodow'', $1) - 1';
CREATE OR REPLACE FUNCTION pg_catalog.weekday (time) RETURNS float8 LANGUAGE SQL STABLE STRICT as 'select pg_catalog.date_part(''isodow'', $1) - 1';
CREATE OR REPLACE FUNCTION pg_catalog.weekday (timestamp(0) with time zone) RETURNS float8 LANGUAGE SQL STABLE STRICT as 'select pg_catalog.date_part(''isodow'', $1) - 1';

DROP FUNCTION IF EXISTS pg_catalog.weekofyear (timestamptz);
DROP FUNCTION IF EXISTS pg_catalog.weekofyear (timetz);
DROP FUNCTION IF EXISTS pg_catalog.weekofyear (abstime);
DROP FUNCTION IF EXISTS pg_catalog.weekofyear (date);
DROP FUNCTION IF EXISTS pg_catalog.weekofyear (time);
DROP FUNCTION IF EXISTS pg_catalog.weekofyear (timestamp(0) with time zone);
DROP FUNCTION IF EXISTS pg_catalog.weekofyear (year);
DROP FUNCTION IF EXISTS pg_catalog.weekofyear (binary);
DROP FUNCTION IF EXISTS pg_catalog.weekofyear (text);
CREATE OR REPLACE FUNCTION pg_catalog.weekofyear (timestamptz) RETURNS float8 LANGUAGE SQL STABLE STRICT as 'select pg_catalog.date_part(''week'', $1)';
CREATE OR REPLACE FUNCTION pg_catalog.weekofyear (timetz) RETURNS float8 LANGUAGE SQL STABLE STRICT as 'select pg_catalog.date_part(''week'', $1)';
CREATE OR REPLACE FUNCTION pg_catalog.weekofyear (abstime) RETURNS float8 LANGUAGE SQL STABLE STRICT as 'select pg_catalog.date_part(''week'', $1)';
CREATE OR REPLACE FUNCTION pg_catalog.weekofyear (date) RETURNS float8 LANGUAGE SQL STABLE STRICT as 'select pg_catalog.date_part(''week'', $1)';
CREATE OR REPLACE FUNCTION pg_catalog.weekofyear (time) RETURNS float8 LANGUAGE SQL STABLE STRICT as 'select pg_catalog.date_part(''week'', $1)';
CREATE OR REPLACE FUNCTION pg_catalog.weekofyear (timestamp(0) with time zone) RETURNS float8 LANGUAGE SQL STABLE STRICT as 'select pg_catalog.date_part(''week'', $1)';

-- json
DROP aggregate IF EXISTS pg_catalog.max(json);
DROP aggregate IF EXISTS pg_catalog.min(json);
DROP FUNCTION IF EXISTS pg_catalog.json_larger(json, json);
DROP FUNCTION IF EXISTS pg_catalog.json_smaller(json, json);
-- tinyblob
DROP aggregate pg_catalog.max(tinyblob);
DROP aggregate pg_catalog.min(tinyblob);
DROP FUNCTION pg_catalog.tinyblob_larger(tinyblob, tinyblob);
DROP FUNCTION pg_catalog.tinyblob_smaller(tinyblob, tinyblob);
-- blob
DROP aggregate pg_catalog.max(blob);
DROP aggregate pg_catalog.min(blob);
DROP FUNCTION pg_catalog.blob_larger(blob, blob);
DROP FUNCTION pg_catalog.blob_smaller(blob, blob);
-- mediumblob
DROP aggregate pg_catalog.max(mediumblob);
DROP aggregate pg_catalog.min(mediumblob);
DROP FUNCTION pg_catalog.mediumblob_larger(mediumblob, mediumblob);
DROP FUNCTION pg_catalog.mediumblob_smaller(mediumblob, mediumblob);
-- longblob
DROP aggregate pg_catalog.max(longblob);
DROP aggregate pg_catalog.min(longblob);
DROP FUNCTION pg_catalog.longblob_larger(longblob, longblob);
DROP FUNCTION pg_catalog.longblob_smaller(longblob, longblob);
-- bit
DROP aggregate pg_catalog.max(bit);
DROP aggregate pg_catalog.min(bit);
DROP FUNCTION pg_catalog.bit_larger(bit, bit);
DROP FUNCTION pg_catalog.bit_smaller(bit, bit);
-- varbit
DROP aggregate pg_catalog.max(varbit);
DROP aggregate pg_catalog.min(varbit);
DROP FUNCTION pg_catalog.varbit_larger(varbit, varbit);
DROP FUNCTION pg_catalog.varbit_smaller(varbit, varbit);

-- date +/- interval expr unit
DROP OPERATOR IF EXISTS pg_catalog.+(text, interval);
DROP OPERATOR IF EXISTS pg_catalog.+(interval, text);
DROP OPERATOR IF EXISTS pg_catalog.+(number, interval);
DROP OPERATOR IF EXISTS pg_catalog.+(interval, number);
DROP OPERATOR IF EXISTS pg_catalog.-(text, interval);
DROP OPERATOR IF EXISTS pg_catalog.-(number, interval);
do $$
begin
        update pg_catalog.pg_operator set oprresult = 'timestamp'::regtype, oprcode = 'date_pl_interval'::regproc
            where oprname = '+' and oprleft = 'date'::regtype and oprright = 'interval'::regtype;
        update pg_catalog.pg_operator set oprresult = 'timestamp'::regtype, oprcode = 'timestamp_pl_interval'::regproc
            where oprname = '+' and oprleft = 'timestamp without time zone'::regtype and oprright = 'interval'::regtype;
        update pg_catalog.pg_operator set oprresult = 'timestamptz'::regtype, oprcode = 'timestamptz_pl_interval'::regproc
            where oprname = '+' and oprleft = 'timestamptz'::regtype and oprright = 'interval'::regtype;
        update pg_catalog.pg_operator set oprresult = 'time'::regtype, oprcode = 'time_pl_interval'::regproc
            where oprname = '+' and oprleft = 'time'::regtype and oprright = 'interval'::regtype;
    
        update pg_catalog.pg_operator set oprresult = 'timestamp'::regtype, oprcode = 'interval_pl_date'::regproc
            where oprname = '+' and oprleft = 'interval'::regtype and oprright = 'date'::regtype;
        update pg_catalog.pg_operator set oprresult = 'timestamp'::regtype, oprcode = 'interval_pl_timestamp'::regproc
            where oprname = '+' and oprleft = 'interval'::regtype and oprright = 'timestamp without time zone'::regtype;
        update pg_catalog.pg_operator set oprresult = 'timestamptz'::regtype, oprcode = 'interval_pl_timestamptz'::regproc
            where oprname = '+' and oprleft = 'interval'::regtype and oprright = 'timestamptz'::regtype;
        update pg_catalog.pg_operator set oprresult = 'time'::regtype, oprcode = 'interval_pl_time'::regproc
            where oprname = '+' and oprleft = 'interval'::regtype and oprright = 'time'::regtype;
        
        update pg_catalog.pg_operator set oprresult = 'timestamp'::regtype, oprcode = 'date_mi_interval'::regproc
            where oprname = '-' and oprleft = 'date'::regtype and oprright = 'interval'::regtype;
        update pg_catalog.pg_operator set oprresult = 'timestamp'::regtype, oprcode = 'timestamp_mi_interval'::regproc
            where oprname = '-' and oprleft = 'timestamp without time zone'::regtype and oprright = 'interval'::regtype;
        update pg_catalog.pg_operator set oprresult = 'timestamptz'::regtype, oprcode = 'timestamptz_mi_interval'::regproc
            where oprname = '-' and oprleft = 'timestamptz'::regtype and oprright = 'interval'::regtype;
        update pg_catalog.pg_operator set oprresult = 'time'::regtype, oprcode = 'time_mi_interval'::regproc
            where oprname = '-' and oprleft = 'time'::regtype and oprright = 'interval'::regtype;
end
$$;
DROP FUNCTION IF EXISTS pg_catalog.op_num_add_intr (numeric, interval);
DROP FUNCTION IF EXISTS pg_catalog.op_text_add_intr (text, interval);
DROP FUNCTION IF EXISTS pg_catalog.op_date_add_intr (date, interval);
DROP FUNCTION IF EXISTS pg_catalog.op_dttm_add_intr (timestamp without time zone, interval);
DROP FUNCTION IF EXISTS pg_catalog.op_tmsp_add_intr (timestamptz, interval);
DROP FUNCTION IF EXISTS pg_catalog.op_intr_add_num (interval, numeric);
DROP FUNCTION IF EXISTS pg_catalog.op_intr_add_text (interval, text);
DROP FUNCTION IF EXISTS pg_catalog.op_intr_add_date (interval, date);
DROP FUNCTION IF EXISTS pg_catalog.op_intr_add_dttm (interval, timestamp without time zone);
DROP FUNCTION IF EXISTS pg_catalog.op_intr_add_tmsp (interval, timestamptz);
DROP FUNCTION IF EXISTS pg_catalog.op_num_sub_intr (numeric, interval);
DROP FUNCTION IF EXISTS pg_catalog.op_text_sub_intr (text, interval);
DROP FUNCTION IF EXISTS pg_catalog.op_date_sub_intr (date, interval);
DROP FUNCTION IF EXISTS pg_catalog.op_dttm_sub_intr (timestamp without time zone, interval);
DROP FUNCTION IF EXISTS pg_catalog.op_tmsp_sub_intr (timestamptz, interval);
DROP FUNCTION IF EXISTS pg_catalog.op_intr_add_time (interval, time);
DROP FUNCTION IF EXISTS pg_catalog.op_time_add_intr (time, interval);
DROP FUNCTION IF EXISTS pg_catalog.op_time_sub_intr (time, interval);
DROP FUNCTION IF EXISTS pg_catalog.date_sub (time, interval);
DROP FUNCTION IF EXISTS pg_catalog.date_add (time, interval);
CREATE OR REPLACE FUNCTION pg_catalog.date_add (time, interval) RETURNS time AS $$ SELECT pg_catalog.adddate($1, $2)  $$ LANGUAGE SQL;
CREATE OR REPLACE FUNCTION pg_catalog.date_sub (time, interval) RETURNS time AS $$ SELECT pg_catalog.adddate($1, -$2)  $$ LANGUAGE SQL;

DROP OPERATOR IF EXISTS pg_catalog.^(date, bit);
DROP OPERATOR IF EXISTS pg_catalog.^(bit, date);
DROP OPERATOR IF EXISTS pg_catalog.^(time, bit);
DROP OPERATOR IF EXISTS pg_catalog.^(bit, time);
DROP OPERATOR IF EXISTS pg_catalog.^(timestamp without time zone, bit);
DROP OPERATOR IF EXISTS pg_catalog.^(bit, timestamp without time zone);
DROP OPERATOR IF EXISTS pg_catalog.^(timestampTz, bit);
DROP OPERATOR IF EXISTS pg_catalog.^(bit, timestampTz);
DROP OPERATOR IF EXISTS pg_catalog.^(uint8, bit);
DROP OPERATOR IF EXISTS pg_catalog.^(bit, uint8);
DROP OPERATOR IF EXISTS pg_catalog.^(numeric, bit);
DROP OPERATOR IF EXISTS pg_catalog.^(bit, numeric);
DROP OPERATOR IF EXISTS pg_catalog.^(int1, int1);
create operator pg_catalog.^(leftarg = int1, rightarg = int1, procedure = pg_catalog.int1xor);
DROP OPERATOR IF EXISTS pg_catalog.^(int2, int2);
create operator pg_catalog.^(leftarg = int2, rightarg = int2, procedure = pg_catalog.int2xor);
DROP OPERATOR IF EXISTS pg_catalog.^(int4, int4);
create operator pg_catalog.^(leftarg = int4, rightarg = int4, procedure = pg_catalog.int4xor);
DROP OPERATOR IF EXISTS pg_catalog.^(int8, int8);
create operator pg_catalog.^(leftarg = int8, rightarg = int8, procedure = pg_catalog.int8xor);
DROP OPERATOR IF EXISTS pg_catalog.^(uint1, uint1);
CREATE OPERATOR pg_catalog.^(
leftarg = uint1, rightarg = uint1, procedure = uint1xor,
commutator=operator(pg_catalog.^)
);
COMMENT ON OPERATOR pg_catalog.^(uint1, uint1) IS 'uint1xor';
DROP OPERATOR IF EXISTS pg_catalog.^(uint2, uint2);
CREATE OPERATOR pg_catalog.^(
leftarg = uint2, rightarg = uint2, procedure = uint2xor,
commutator=operator(pg_catalog.^)
);
COMMENT ON OPERATOR pg_catalog.^(uint2, uint2) IS 'uint2xor';
DROP OPERATOR IF EXISTS pg_catalog.^(uint4, uint4);
CREATE OPERATOR pg_catalog.^(
leftarg = uint4, rightarg = uint4, procedure = uint4xor,
commutator=operator(pg_catalog.^)
);
COMMENT ON OPERATOR pg_catalog.^(uint4, uint4) IS 'uint4xor';
DROP OPERATOR IF EXISTS pg_catalog.^(uint8, uint8);
CREATE OPERATOR pg_catalog.^(
leftarg = uint8, rightarg = uint8, procedure = uint8xor,
commutator=operator(pg_catalog.^)
);
COMMENT ON OPERATOR pg_catalog.^(uint8, uint8) IS 'uint8xor';
DROP OPERATOR IF EXISTS pg_catalog.^(uint1, int1);
CREATE OPERATOR pg_catalog.^(
leftarg = uint1, rightarg = int1, procedure = uint1_xor_int1
);
COMMENT ON OPERATOR pg_catalog.^(uint1, int1) IS 'uint1_xor_int1';
DROP OPERATOR IF EXISTS pg_catalog.^(int1, uint1);
CREATE OPERATOR pg_catalog.^(
leftarg = int1, rightarg = uint1, procedure = int1_xor_uint1
);
COMMENT ON OPERATOR pg_catalog.^(int1, uint1) IS 'int1_xor_uint1';
DROP OPERATOR IF EXISTS pg_catalog.^(uint2, int2);
CREATE OPERATOR pg_catalog.^(
leftarg = uint2, rightarg = int2, procedure = uint2_xor_int2,
commutator=operator(pg_catalog.^)
);
COMMENT ON OPERATOR pg_catalog.^(uint2, int2) IS 'uint2_xor_int2';
DROP OPERATOR IF EXISTS pg_catalog.^(int2, uint2);
CREATE OPERATOR pg_catalog.^(
leftarg = int2, rightarg = uint2, procedure = int2_xor_uint2,
commutator=operator(pg_catalog.^)
);
COMMENT ON OPERATOR pg_catalog.^(int2, uint2) IS 'int2_xor_uint2';
DROP OPERATOR IF EXISTS pg_catalog.^(uint4, int4);
CREATE OPERATOR pg_catalog.^(
leftarg = uint4, rightarg = int4, procedure = uint4_xor_int4,
commutator=operator(pg_catalog.^)
);
COMMENT ON OPERATOR pg_catalog.^(uint4, int4) IS 'uint4_xor_int4';
DROP OPERATOR IF EXISTS pg_catalog.^(int4, uint4);
CREATE OPERATOR pg_catalog.^(
leftarg = int4, rightarg = uint4, procedure = int4_xor_uint4,
commutator=operator(pg_catalog.^)
);
COMMENT ON OPERATOR pg_catalog.^(int4, uint4) IS 'int4_xor_uint4';
DROP OPERATOR IF EXISTS pg_catalog.^(uint8, int8);
CREATE OPERATOR pg_catalog.^(
leftarg = uint8, rightarg = int8, procedure = uint8_xor_int8,
commutator=operator(pg_catalog.^)
);
COMMENT ON OPERATOR pg_catalog.^(uint8, int8) IS 'uint8_xor_int8';
DROP OPERATOR IF EXISTS pg_catalog.^(int8, uint8);
CREATE OPERATOR pg_catalog.^(
leftarg = int8, rightarg = uint8, procedure = int8_xor_uint8,
commutator=operator(pg_catalog.^)
);
COMMENT ON OPERATOR pg_catalog.^(int8, uint8) IS 'int8_xor_uint8';
DROP OPERATOR IF EXISTS pg_catalog.^(blob, blob);
create operator pg_catalog.^(leftarg = blob, rightarg = blob, procedure = pg_catalog.blobxor);
DROP OPERATOR IF EXISTS pg_catalog.^(mediumblob, mediumblob);
DROP OPERATOR IF EXISTS pg_catalog.^(longblob, longblob);
DROP OPERATOR IF EXISTS pg_catalog.^(binary, binary);
DROP OPERATOR IF EXISTS pg_catalog.^(varbinary, varbinary);
DROP OPERATOR IF EXISTS pg_catalog.^(tinyblob, tinyblob);
DROP OPERATOR IF EXISTS pg_catalog.^(blob, integer);
create operator pg_catalog.^(leftarg = blob, rightarg = integer, procedure = pg_catalog.blobxor);
DROP OPERATOR IF EXISTS pg_catalog.^(integer, blob);
create operator pg_catalog.^(leftarg = integer, rightarg = blob, procedure = pg_catalog.blobxor);
DROP OPERATOR IF EXISTS pg_catalog.^(int8, blob);
create operator pg_catalog.^(leftarg = int8, rightarg = blob, procedure = pg_catalog.blobxor);
DROP OPERATOR IF EXISTS pg_catalog.^(blob, int8);
create operator pg_catalog.^(leftarg = blob, rightarg = int8, procedure = pg_catalog.blobxor);
DROP OPERATOR IF EXISTS pg_catalog.^(float8, blob);
create operator pg_catalog.^(leftarg = float8, rightarg = blob, procedure = pg_catalog.blobxor);
DROP OPERATOR IF EXISTS pg_catalog.^(blob, float8);
create operator pg_catalog.^(leftarg = blob, rightarg = float8, procedure = pg_catalog.blobxor);
DROP OPERATOR IF EXISTS pg_catalog.^(boolean, boolean);
create operator pg_catalog.^(leftarg = boolean, rightarg = boolean, procedure = pg_catalog.boolxor);
DROP OPERATOR IF EXISTS pg_catalog.^(boolean, float8);
create operator pg_catalog.^(leftarg = boolean, rightarg = float8, procedure = pg_catalog.bool_float8_xor);
DROP OPERATOR IF EXISTS pg_catalog.^(float8, boolean);
create operator pg_catalog.^(leftarg = float8, rightarg = boolean, procedure = pg_catalog.float8_bool_xor);
DROP OPERATOR IF EXISTS pg_catalog.^(date, date);
CREATE OPERATOR pg_catalog.^(leftarg = date, rightarg = date, procedure = pg_catalog.datexor);
DROP OPERATOR IF EXISTS pg_catalog.^(time, time);
CREATE OPERATOR pg_catalog.^(leftarg = time, rightarg = time, procedure = pg_catalog.timexor);
DROP OPERATOR IF EXISTS pg_catalog.^(date, time);
CREATE OPERATOR pg_catalog.^(leftarg = date, rightarg = time, procedure = pg_catalog.date_time_xor);
DROP OPERATOR IF EXISTS pg_catalog.^(time, date);
CREATE OPERATOR pg_catalog.^(leftarg = time, rightarg = date, procedure = pg_catalog.time_date_xor);
DROP OPERATOR IF EXISTS pg_catalog.^(time, text);
CREATE OPERATOR pg_catalog.^(leftarg = time, rightarg = text, procedure = pg_catalog.time_text_xor);
DROP OPERATOR IF EXISTS pg_catalog.^(text, time);
CREATE OPERATOR pg_catalog.^(leftarg = text, rightarg = time, procedure = pg_catalog.text_time_xor);
DROP OPERATOR IF EXISTS pg_catalog.^(date, text);
CREATE OPERATOR pg_catalog.^(leftarg = date, rightarg = text, procedure = pg_catalog.date_text_xor);
DROP OPERATOR IF EXISTS pg_catalog.^(text, date);
CREATE OPERATOR pg_catalog.^(leftarg = text, rightarg = date, procedure = pg_catalog.text_date_xor);
DROP OPERATOR IF EXISTS pg_catalog.^(date, int8);
CREATE OPERATOR pg_catalog.^(leftarg = date, rightarg = int8, procedure = pg_catalog.date_int8_xor);
DROP OPERATOR IF EXISTS pg_catalog.^(int8, date);
CREATE OPERATOR pg_catalog.^(leftarg = int8, rightarg = date, procedure = pg_catalog.int8_date_xor);
DROP OPERATOR IF EXISTS pg_catalog.^(time, int8);
CREATE OPERATOR pg_catalog.^(leftarg = time, rightarg = int8, procedure = pg_catalog.time_int8_xor);
DROP OPERATOR IF EXISTS pg_catalog.^(int8, time);
CREATE OPERATOR pg_catalog.^(leftarg = int8, rightarg = time, procedure = pg_catalog.int8_time_xor);
DROP OPERATOR IF EXISTS pg_catalog.^(date, float8);
CREATE OPERATOR pg_catalog.^(leftarg = date, rightarg = float8, procedure = pg_catalog.date_float8_xor);
DROP OPERATOR IF EXISTS pg_catalog.^(float8, date);
CREATE OPERATOR pg_catalog.^(leftarg = float8, rightarg = date, procedure = pg_catalog.float8_date_xor);
DROP OPERATOR IF EXISTS pg_catalog.^(timestamp without time zone, timestamp without time zone);
CREATE OPERATOR pg_catalog.^(leftarg = timestamp without time zone, rightarg = timestamp without time zone, procedure = pg_catalog.timestampxor);
DROP OPERATOR IF EXISTS pg_catalog.^(timestamp without time zone, int8);
CREATE OPERATOR pg_catalog.^(leftarg = timestamp without time zone, rightarg = int8, procedure = pg_catalog.timestamp_int8_xor);
DROP OPERATOR IF EXISTS pg_catalog.^(int8, timestamp without time zone);
CREATE OPERATOR pg_catalog.^(leftarg = int8, rightarg = timestamp without time zone, procedure = pg_catalog.int8_timestamp_xor);
DROP OPERATOR IF EXISTS pg_catalog.^(timestamp without time zone, float8);
CREATE OPERATOR pg_catalog.^(leftarg = timestamp without time zone, rightarg = float8, procedure = pg_catalog.timestamp_float8_xor);
DROP OPERATOR IF EXISTS pg_catalog.^(float8, timestamp without time zone);
CREATE OPERATOR pg_catalog.^(leftarg = float8, rightarg = timestamp without time zone, procedure = pg_catalog.float8_timestamp_xor);
DROP OPERATOR IF EXISTS pg_catalog.^(timestamp without time zone, text);
CREATE OPERATOR pg_catalog.^(leftarg = timestamp without time zone, rightarg = text, procedure = pg_catalog.timestamp_text_xor);
DROP OPERATOR IF EXISTS pg_catalog.^(text, timestamp without time zone);
CREATE OPERATOR pg_catalog.^(leftarg = text, rightarg = timestamp without time zone, procedure = pg_catalog.text_timestamp_xor);
DROP OPERATOR IF EXISTS pg_catalog.^(timestampTz, timestampTz);
CREATE OPERATOR pg_catalog.^(leftarg = timestampTz, rightarg = timestampTz, procedure = pg_catalog.timestamptzxor);
DROP OPERATOR IF EXISTS pg_catalog.^(timestampTz, int8);
CREATE OPERATOR pg_catalog.^(leftarg = timestampTz, rightarg = int8, procedure = pg_catalog.timestamptz_int8_xor);
DROP OPERATOR IF EXISTS pg_catalog.^(int8, timestampTz);
CREATE OPERATOR pg_catalog.^(leftarg = int8, rightarg = timestampTz, procedure = pg_catalog.int8_timestamptz_xor);
DROP OPERATOR IF EXISTS pg_catalog.^(timestampTz, float8);
CREATE OPERATOR pg_catalog.^(leftarg = timestampTz, rightarg = float8, procedure = pg_catalog.timestamptz_float8_xor);
DROP OPERATOR IF EXISTS pg_catalog.^(float8, timestampTz);
CREATE OPERATOR pg_catalog.^(leftarg = float8, rightarg = timestampTz, procedure = pg_catalog.float8_timestamptz_xor);
DROP OPERATOR IF EXISTS pg_catalog.^(timestampTz, text);
CREATE OPERATOR pg_catalog.^(leftarg = timestampTz, rightarg = text, procedure = pg_catalog.timestamptz_text_xor);
DROP OPERATOR IF EXISTS pg_catalog.^(text, timestampTz);
CREATE OPERATOR pg_catalog.^(leftarg = text, rightarg = timestampTz, procedure = pg_catalog.text_timestamptz_xor);
DROP OPERATOR IF EXISTS pg_catalog.^(uint8, boolean);
CREATE OPERATOR pg_catalog.^(
leftarg = uint8, rightarg = boolean, procedure = uint8_xor_bool,
commutator=operator(pg_catalog.^)
);
DROP OPERATOR IF EXISTS pg_catalog.^(uint4, boolean);
CREATE OPERATOR pg_catalog.^(leftarg = uint4, rightarg = boolean, procedure = pg_catalog.uint4_xor_bool, commutator=operator(pg_catalog.^));
DROP OPERATOR IF EXISTS pg_catalog.^(uint2, boolean);
CREATE OPERATOR pg_catalog.^(leftarg = uint2, rightarg = boolean, procedure = pg_catalog.uint2_xor_bool, commutator=operator(pg_catalog.^));
DROP OPERATOR IF EXISTS pg_catalog.^(uint1, boolean);
CREATE OPERATOR pg_catalog.^(leftarg = uint1, rightarg = boolean, procedure = pg_catalog.uint1_xor_bool, commutator=operator(pg_catalog.^));
DROP OPERATOR IF EXISTS pg_catalog.^(boolean, uint1);
CREATE OPERATOR pg_catalog.^(leftarg = boolean, rightarg = uint1, procedure = pg_catalog.bool_xor_uint1, commutator=operator(pg_catalog.^));
DROP OPERATOR IF EXISTS pg_catalog.^(boolean, uint2);
CREATE OPERATOR pg_catalog.^(leftarg = boolean, rightarg = uint2, procedure = pg_catalog.bool_xor_uint2, commutator=operator(pg_catalog.^));
DROP OPERATOR IF EXISTS pg_catalog.^(boolean, uint4);
CREATE OPERATOR pg_catalog.^(leftarg = boolean, rightarg = uint4, procedure = pg_catalog.bool_xor_uint4, commutator=operator(pg_catalog.^));
DROP OPERATOR IF EXISTS pg_catalog.^(boolean, uint8);
CREATE OPERATOR pg_catalog.^(leftarg = boolean, rightarg = uint8, procedure = pg_catalog.bool_xor_uint8, commutator=operator(pg_catalog.^));
DROP OPERATOR IF EXISTS pg_catalog.^(bit, bit);
CREATE OPERATOR pg_catalog.^ (leftarg = bit, rightarg = bit, procedure = pg_catalog.bitxor);
DROP OPERATOR IF EXISTS pg_catalog.^(text, text);
CREATE OPERATOR pg_catalog.^ (leftarg = text, rightarg = text, procedure = pg_catalog.textxor);
UPDATE pg_catalog.pg_operator SET oprresult = 'float8'::regtype, oprcode = 'dpow'::regproc
    WHERE oprname = '^' AND oprleft = 'float8'::regtype AND oprright = 'float8'::regtype;
UPDATE pg_catalog.pg_operator SET oprresult = 'numeric'::regtype, oprcode = 'numeric_power'::regproc
    WHERE oprname = '^' AND oprleft = 'numeric'::regtype AND oprright = 'numeric'::regtype;

DROP OPERATOR IF EXISTS pg_catalog.^(boolean, int1);
DROP OPERATOR IF EXISTS pg_catalog.^(boolean, int2);
DROP OPERATOR IF EXISTS pg_catalog.^(boolean, int4);
DROP OPERATOR IF EXISTS pg_catalog.^(boolean, int8);
DROP OPERATOR IF EXISTS pg_catalog.^(int8, boolean);
DROP OPERATOR IF EXISTS pg_catalog.^(int4, boolean);
DROP OPERATOR IF EXISTS pg_catalog.^(int2, boolean);
DROP OPERATOR IF EXISTS pg_catalog.^(int1, boolean);
DROP FUNCTION IF EXISTS pg_catalog.op_int1xor (int1, int1);
DROP FUNCTION IF EXISTS pg_catalog.op_int2xor (int2, int2);
DROP FUNCTION IF EXISTS pg_catalog.op_int4xor (int4, int4);
DROP FUNCTION IF EXISTS pg_catalog.op_int8xor (int8, int8);
DROP FUNCTION IF EXISTS pg_catalog.op_uint1xor (uint1, uint1);
DROP FUNCTION IF EXISTS pg_catalog.op_uint2xor (uint2, uint2);
DROP FUNCTION IF EXISTS pg_catalog.op_uint4xor (uint4, uint4);
DROP FUNCTION IF EXISTS pg_catalog.op_uint8xor (uint8, uint8);
DROP FUNCTION IF EXISTS pg_catalog.op_uint1_xor_int1 (uint1, int1);
DROP FUNCTION IF EXISTS pg_catalog.op_int1_xor_uint1 (int1, uint1);
DROP FUNCTION IF EXISTS pg_catalog.op_uint2_xor_int2 (uint2, int2);
DROP FUNCTION IF EXISTS pg_catalog.op_int2_xor_uint2 (int2, uint2);
DROP FUNCTION IF EXISTS pg_catalog.op_uint4_xor_int4 (uint4, int4);
DROP FUNCTION IF EXISTS pg_catalog.op_int4_xor_uint4 (int4, uint4);
DROP FUNCTION IF EXISTS pg_catalog.op_uint8_xor_int8 (uint8, int8);
DROP FUNCTION IF EXISTS pg_catalog.op_int8_xor_uint8 (int8, uint8);
DROP FUNCTION IF EXISTS pg_catalog.blob_xor_blob (blob, blob);
DROP FUNCTION IF EXISTS pg_catalog.mblob_xor_mblob (mediumblob, mediumblob);
DROP FUNCTION IF EXISTS pg_catalog.lblob_xor_lblob (longblob, longblob);
DROP FUNCTION IF EXISTS pg_catalog.binary_xor_binary (binary, binary);
DROP FUNCTION IF EXISTS pg_catalog.varbinary_xor_varbinary (varbinary, varbinary);
DROP FUNCTION IF EXISTS pg_catalog.tinyblob_xor_tinyblob (tinyblob, tinyblob);
DROP FUNCTION IF EXISTS pg_catalog.op_blob_int_xor(blob, integer);
DROP FUNCTION IF EXISTS pg_catalog.op_int_blob_xor(integer, blob);
DROP FUNCTION IF EXISTS pg_catalog.op_int8_blob_xor(int8, blob);
DROP FUNCTION IF EXISTS pg_catalog.op_blob_int8_xor(blob, int8);
DROP FUNCTION IF EXISTS pg_catalog.op_float8_blob_xor(float8, blob);
DROP FUNCTION IF EXISTS pg_catalog.op_blob_float8_xor(blob, float8);
DROP FUNCTION IF EXISTS pg_catalog.op_boolxor(boolean, boolean);
DROP FUNCTION IF EXISTS pg_catalog.op_bool_float8_xor(boolean, float8);
DROP FUNCTION IF EXISTS pg_catalog.op_float8_bool_xor(float8, boolean);
DROP FUNCTION IF EXISTS pg_catalog.op_datexor(date, date);
DROP FUNCTION IF EXISTS pg_catalog.op_timexor(time, time);
DROP FUNCTION IF EXISTS pg_catalog.op_date_time_xor(date, time);
DROP FUNCTION IF EXISTS pg_catalog.op_time_date_xor(time, date);
DROP FUNCTION IF EXISTS pg_catalog.op_time_text_xor(time, text);
DROP FUNCTION IF EXISTS pg_catalog.op_text_time_xor(text, time);
DROP FUNCTION IF EXISTS pg_catalog.op_date_text_xor(date, text);
DROP FUNCTION IF EXISTS pg_catalog.op_text_date_xor(text, date);
DROP FUNCTION IF EXISTS pg_catalog.op_date_int8_xor(date, int8);
DROP FUNCTION IF EXISTS pg_catalog.op_int8_date_xor(int8, date);
DROP FUNCTION IF EXISTS pg_catalog.op_time_int8_xor(time, int8);
DROP FUNCTION IF EXISTS pg_catalog.op_int8_time_xor(int8, time);
DROP FUNCTION IF EXISTS pg_catalog.op_date_float8_xor(date, float8);
DROP FUNCTION IF EXISTS pg_catalog.op_float8_date_xor(float8, date);
DROP FUNCTION IF EXISTS pg_catalog.op_timestampxor(timestamp without time zone, timestamp without time zone);
DROP FUNCTION IF EXISTS pg_catalog.op_timestamp_int8_xor(timestamp without time zone, int8);
DROP FUNCTION IF EXISTS pg_catalog.op_int8_timestamp_xor(int8, timestamp without time zone);
DROP FUNCTION IF EXISTS pg_catalog.op_timestamp_float8_xor(timestamp without time zone, float8);
DROP FUNCTION IF EXISTS pg_catalog.op_float8_timestamp_xor(float8, timestamp without time zone);
DROP FUNCTION IF EXISTS pg_catalog.op_timestamp_text_xor(timestamp without time zone, text);
DROP FUNCTION IF EXISTS pg_catalog.op_text_timestamp_xor(text, timestamp without time zone);
DROP FUNCTION IF EXISTS pg_catalog.op_timestamptzxor(timestampTz, timestampTz);
DROP FUNCTION IF EXISTS pg_catalog.op_timestamptz_int8_xor(timestampTz, int8);
DROP FUNCTION IF EXISTS pg_catalog.op_int8_timestamptz_xor(int8, timestampTz);
DROP FUNCTION IF EXISTS pg_catalog.op_timestamptz_float8_xor(timestampTz, float8);
DROP FUNCTION IF EXISTS pg_catalog.op_float8_timestamptz_xor(float8, timestampTz);
DROP FUNCTION IF EXISTS pg_catalog.op_timestamptz_text_xor(timestampTz, text);
DROP FUNCTION IF EXISTS pg_catalog.op_text_timestamptz_xor(text, timestampTz);
DROP FUNCTION IF EXISTS pg_catalog.op_uint8_xor_bool(uint8, boolean);
DROP FUNCTION IF EXISTS pg_catalog.op_uint4_xor_bool(uint4, boolean);
DROP FUNCTION IF EXISTS pg_catalog.op_uint2_xor_bool(uint2, boolean);
DROP FUNCTION IF EXISTS pg_catalog.op_uint1_xor_bool(uint1, boolean);
DROP FUNCTION IF EXISTS pg_catalog.op_bool_xor_uint1(boolean, uint1);
DROP FUNCTION IF EXISTS pg_catalog.op_bool_xor_uint2(boolean, uint2);
DROP FUNCTION IF EXISTS pg_catalog.op_bool_xor_uint4(boolean, uint4);
DROP FUNCTION IF EXISTS pg_catalog.op_bool_xor_uint8(boolean, uint8);
DROP FUNCTION IF EXISTS pg_catalog.op_bitxor (bit, bit);
DROP FUNCTION IF EXISTS pg_catalog.op_dpow (float8, float8);
DROP FUNCTION IF EXISTS pg_catalog.op_numeric_power(numeric, numeric);
DROP FUNCTION IF EXISTS pg_catalog.op_textxor(text, text);
DROP FUNCTION IF EXISTS pg_catalog.op_bool_xor_int1(boolean, int1);
DROP FUNCTION IF EXISTS pg_catalog.op_bool_xor_int2(boolean, int2);
DROP FUNCTION IF EXISTS pg_catalog.op_bool_xor_int4(boolean, int4);
DROP FUNCTION IF EXISTS pg_catalog.op_bool_xor_int8(boolean, int8);
DROP FUNCTION IF EXISTS pg_catalog.op_int1_xor_bool(int1, boolean);
DROP FUNCTION IF EXISTS pg_catalog.op_int2_xor_bool(int2, boolean);
DROP FUNCTION IF EXISTS pg_catalog.op_int4_xor_bool(int4, boolean);
DROP FUNCTION IF EXISTS pg_catalog.op_int8_xor_bool(int8, boolean);
DROP FUNCTION IF EXISTS pg_catalog.op_num_xor_bit(numeric, bit);
DROP FUNCTION IF EXISTS pg_catalog.op_bit_xor_num(bit, numeric);
DROP FUNCTION IF EXISTS pg_catalog.op_uint8_xor_bit(uint8, bit);
DROP FUNCTION IF EXISTS pg_catalog.op_bit_xor_uint8(bit, uint8);
DROP FUNCTION IF EXISTS pg_catalog.op_date_bit_xor(date, bit);
DROP FUNCTION IF EXISTS pg_catalog.op_bit_date_xor(bit, date);
DROP FUNCTION IF EXISTS pg_catalog.op_timestamp_bit_xor(timestamp without time zone, bit);
DROP FUNCTION IF EXISTS pg_catalog.op_bit_timestamp_xor(bit, timestamp without time zone);
DROP FUNCTION IF EXISTS pg_catalog.op_timestamptz_bit_xor(timestampTz, bit);
DROP FUNCTION IF EXISTS pg_catalog.op_bit_timestamptz_xor(bit, timestampTz);
DROP FUNCTION IF EXISTS pg_catalog.numeric_xor(numeric, numeric);

do $$
begin
update pg_catalog.pg_type set typmodin = '-', typmodout = '-' where oid in('int1'::regtype, 'uint1'::regtype, 'int2'::regtype, 'uint2'::regtype, 'int4'::regtype, 'uint4'::regtype, 'int8'::regtype, 'uint8'::regtype);
end
$$;

DROP FUNCTION IF EXISTS pg_catalog.int1_typmodin (_cstring) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.int1_typmodout (int) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.int2_typmodin (_cstring) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.int2_typmodout (int) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.int4_typmodin (_cstring) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.int4_typmodout (int) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.int8_typmodin (_cstring) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.int8_typmodout (int) CASCADE;


DROP CAST IF EXISTS (text AS bit);
DROP FUNCTION IF EXISTS pg_catalog.text_to_bit(text);

DROP FUNCTION IF EXISTS pg_catalog.inet_ntoa(bit) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.inet_ntoa(binary) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.inet_ntoa(tinyblob) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.inet_ntoa(blob) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.inet_ntoa(mediumblob) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.inet_ntoa(longblob) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.inet_ntoa(nvarchar2) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.inet_ntoa(year) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.inet_ntoa(json) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.inet_ntoa(int8) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.inet_ntoa(int8) RETURNS text LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'inetntoa';
CREATE OR REPLACE FUNCTION pg_catalog.inet_ntoa(bit) RETURNS text LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.inet_ntoa(cast($1 as int8))';
CREATE OR REPLACE FUNCTION pg_catalog.inet_ntoa(binary) RETURNS text LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.inet_ntoa(cast($1 as int8))';
CREATE OR REPLACE FUNCTION pg_catalog.inet_ntoa(tinyblob) RETURNS text LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.inet_ntoa(cast($1 as int8))';
CREATE OR REPLACE FUNCTION pg_catalog.inet_ntoa(blob) RETURNS text LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.inet_ntoa(cast($1 as int8))';
CREATE OR REPLACE FUNCTION pg_catalog.inet_ntoa(mediumblob) RETURNS text LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.inet_ntoa(cast($1 as int8))';
CREATE OR REPLACE FUNCTION pg_catalog.inet_ntoa(longblob) RETURNS text LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.inet_ntoa(cast($1 as int8))';
CREATE OR REPLACE FUNCTION pg_catalog.inet_ntoa(nvarchar2) RETURNS text LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.inet_ntoa(cast($1 as varchar))';
CREATE OR REPLACE FUNCTION pg_catalog.inet_ntoa(year) RETURNS text LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.inet_ntoa(cast($1 as int8))';
CREATE OR REPLACE FUNCTION pg_catalog.inet_ntoa(json) RETURNS text LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.inet_ntoa(cast($1 as int8))';
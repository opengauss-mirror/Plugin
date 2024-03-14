DROP FUNCTION IF EXISTS pg_catalog.dolphin_invoke();
CREATE FUNCTION pg_catalog.dolphin_invoke()
    RETURNS VOID AS '$libdir/dolphin','dolphin_invoke' LANGUAGE C STRICT;


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

create or replace function pg_catalog."user"() returns name as 'select current_user' LANGUAGE 'sql' IMMUTABLE;

drop function pg_catalog.year_recv(bytea);
CREATE OR REPLACE FUNCTION pg_catalog.year_recv (internal) RETURNS year LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'year_recv';
do $$
begin
update pg_catalog.pg_type set typreceive = 'year_recv'::regproc, typsend = 'year_send'::regproc where oid = 'year'::regtype;
end
$$;

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

CREATE OR REPLACE FUNCTION pg_catalog.log2(uint1) RETURNS double precision LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.log2(cast($1 as double precision))';
CREATE OR REPLACE FUNCTION pg_catalog.log2(uint2) RETURNS double precision LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.log2(cast($1 as double precision))';
CREATE OR REPLACE FUNCTION pg_catalog.log2(uint4) RETURNS double precision LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.log2(cast($1 as double precision))';
CREATE OR REPLACE FUNCTION pg_catalog.log2(uint8) RETURNS double precision LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.log2(cast($1 as double precision))';
DROP FUNCTION IF EXISTS pg_catalog.log2(numeric);
CREATE OR REPLACE FUNCTION pg_catalog.log2(numeric) RETURNS double precision LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.log2(cast($1 as double precision))';
CREATE OR REPLACE FUNCTION pg_catalog.log2(bit) RETURNS double precision LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.log2(cast($1 as double precision))';
CREATE OR REPLACE FUNCTION pg_catalog.log2(boolean) RETURNS double precision LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.log2(cast($1 as double precision))';
CREATE OR REPLACE FUNCTION pg_catalog.log2(text) RETURNS double precision LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.log2(cast($1 as double precision))';
CREATE OR REPLACE FUNCTION pg_catalog.log2(char) RETURNS double precision LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.log2(cast($1 as double precision))';
CREATE OR REPLACE FUNCTION pg_catalog.log2(varchar) RETURNS double precision LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.log2(cast($1 as double precision))';

CREATE OR REPLACE FUNCTION pg_catalog.log10(uint1) RETURNS double precision LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.log10(cast($1 as double precision))';
CREATE OR REPLACE FUNCTION pg_catalog.log10(uint2) RETURNS double precision LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.log10(cast($1 as double precision))';
CREATE OR REPLACE FUNCTION pg_catalog.log10(uint4) RETURNS double precision LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.log10(cast($1 as double precision))';
CREATE OR REPLACE FUNCTION pg_catalog.log10(uint8) RETURNS double precision LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.log10(cast($1 as double precision))';
DROP FUNCTION IF EXISTS pg_catalog.log10(numeric);
CREATE OR REPLACE FUNCTION pg_catalog.log10(numeric) RETURNS double precision LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.log10(cast($1 as double precision))';
CREATE OR REPLACE FUNCTION pg_catalog.log10(bit) RETURNS double precision LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.log10(cast($1 as double precision))';
CREATE OR REPLACE FUNCTION pg_catalog.log10(boolean) RETURNS double precision LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.log10(cast($1 as double precision))';
CREATE OR REPLACE FUNCTION pg_catalog.log10(text) RETURNS double precision LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.log10(cast($1 as double precision))';
CREATE OR REPLACE FUNCTION pg_catalog.log10(char) RETURNS double precision LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.log10(cast($1 as double precision))';
CREATE OR REPLACE FUNCTION pg_catalog.log10(varchar) RETURNS double precision LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.log10(cast($1 as double precision))';

DROP CAST IF EXISTS (blob AS boolean) CASCADE;
DROP CAST IF EXISTS (tinyblob AS boolean) CASCADE;
DROP CAST IF EXISTS (mediumblob AS boolean) CASCADE;
DROP CAST IF EXISTS (longblob AS boolean) CASCADE;
DROP CAST IF EXISTS (anyset AS boolean) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.any2boolean(anyelement) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.enum_boolean(anyenum) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.set_boolean(anyset) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.any2boolean(anyelement) RETURNS boolean LANGUAGE SQL IMMUTABLE STRICT as 'select cast(cast($1 as text) as boolean)';
CREATE OR REPLACE FUNCTION pg_catalog.enum_boolean(anyenum) RETURNS boolean LANGUAGE SQL IMMUTABLE STRICT as 'select cast(cast($1 as int) as boolean)';
CREATE OR REPLACE FUNCTION pg_catalog.set_boolean(anyset) RETURNS boolean LANGUAGE SQL IMMUTABLE STRICT as 'select cast(cast($1 as int) as boolean)';
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

CREATE OPERATOR CLASS pg_catalog.enumtext_ops
     FOR TYPE anyenum USING hash AS
        OPERATOR        1       = ,
        OPERATOR        1       =(anyenum, text),
        OPERATOR        1       =(text, anyenum),
        FUNCTION        1       hashenum(anyenum),
        FUNCTION        1       hashtext(text);

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

CREATE OR REPLACE FUNCTION pg_catalog.to_days(bit) RETURNS int8 LANGUAGE SQL STABLE STRICT as 'select pg_catalog.to_days(cast(cast($1 as int8) as timestamp(0) without time zone))';
CREATE OR REPLACE FUNCTION pg_catalog.to_days(boolean) RETURNS int8 LANGUAGE SQL STABLE STRICT as 'select pg_catalog.to_days(cast(cast($1 as int8) as timestamp(0) without time zone))';
CREATE OR REPLACE FUNCTION pg_catalog.to_days(time) RETURNS int8 LANGUAGE SQL STABLE STRICT as 'select pg_catalog.to_days(cast($1 as timestamp(0) without time zone))';
CREATE OR REPLACE FUNCTION pg_catalog.to_days(year) RETURNS int8 LANGUAGE SQL STABLE STRICT as 'select pg_catalog.to_days(cast(cast($1 as int8) as timestamp(0) without time zone))';
CREATE OR REPLACE FUNCTION pg_catalog.to_days(binary) RETURNS int8 LANGUAGE SQL STABLE STRICT as 'select pg_catalog.to_days(cast($1 as timestamp(0) without time zone))';
CREATE OR REPLACE FUNCTION pg_catalog.to_days(blob) RETURNS int8 LANGUAGE SQL STABLE STRICT as 'select pg_catalog.to_days(cast($1 as timestamp(0) without time zone))';
CREATE OR REPLACE FUNCTION pg_catalog.to_days(text) RETURNS int8 LANGUAGE SQL STABLE STRICT as 'select pg_catalog.to_days(text_timestamp($1))';
CREATE OR REPLACE FUNCTION pg_catalog.to_days(anyenum) RETURNS int8 LANGUAGE SQL STABLE STRICT as 'select pg_catalog.to_days(cast($1 as timestamp(0) without time zone))';
CREATE OR REPLACE FUNCTION pg_catalog.to_days(anyset) RETURNS int8 LANGUAGE SQL STABLE STRICT as 'select pg_catalog.to_days(cast($1 as timestamp(0) without time zone))';
CREATE OR REPLACE FUNCTION pg_catalog.to_days(json) RETURNS int8 LANGUAGE SQL STABLE STRICT as 'select pg_catalog.to_days(cast($1 as timestamp(0) without time zone))';

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

CREATE OR REPLACE FUNCTION pg_catalog.lower(boolean) RETURNS varchar LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.lower(cast($1 as TEXT))::varchar';
CREATE OR REPLACE FUNCTION pg_catalog.lower(tinyblob) RETURNS varbinary LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'lower_blob';
CREATE OR REPLACE FUNCTION pg_catalog.lower(blob) RETURNS blob LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'lower_blob';
CREATE OR REPLACE FUNCTION pg_catalog.lower(mediumblob) RETURNS blob LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'lower_blob';
CREATE OR REPLACE FUNCTION pg_catalog.lower(longblob) RETURNS blob LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'lower_blob';
CREATE OR REPLACE FUNCTION pg_catalog.lower(bit) RETURNS varbinary LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'lower_bit';
CREATE OR REPLACE FUNCTION pg_catalog.lower(binary) RETURNS varbinary LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'lower_blob';
CREATE OR REPLACE FUNCTION pg_catalog.lower(varbinary) RETURNS varbinary LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'lower_blob';
CREATE OR REPLACE FUNCTION pg_catalog.lower(integer) RETURNS varchar LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.lower(cast($1 as TEXT))::varchar';
CREATE OR REPLACE FUNCTION pg_catalog.lower(float) RETURNS varchar LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.lower(cast($1 as TEXT))::varchar';
CREATE OR REPLACE FUNCTION pg_catalog.lower(char) RETURNS varchar LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.lower(cast($1 as TEXT))::varchar';
CREATE OR REPLACE FUNCTION pg_catalog.lower(varchar) RETURNS varchar LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.lower(cast($1 as TEXT))::varchar';

CREATE OR REPLACE FUNCTION pg_catalog.lcase(boolean) RETURNS varchar LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.lower($1)';
CREATE OR REPLACE FUNCTION pg_catalog.lcase(tinyblob) RETURNS varbinary LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.lower($1)';
CREATE OR REPLACE FUNCTION pg_catalog.lcase(blob) RETURNS blob LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.lower($1)';
CREATE OR REPLACE FUNCTION pg_catalog.lcase(mediumblob) RETURNS blob LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.lower($1)';
CREATE OR REPLACE FUNCTION pg_catalog.lcase(longblob) RETURNS blob LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.lower($1)';
CREATE OR REPLACE FUNCTION pg_catalog.lcase(bit) RETURNS varbinary LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.lower($1)';
CREATE OR REPLACE FUNCTION pg_catalog.lcase(binary) RETURNS varbinary LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.lower($1)';
CREATE OR REPLACE FUNCTION pg_catalog.lcase(varbinary) RETURNS varbinary LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.lower($1)';
CREATE OR REPLACE FUNCTION pg_catalog.lcase(integer) RETURNS varchar LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.lower($1)';
CREATE OR REPLACE FUNCTION pg_catalog.lcase(float) RETURNS varchar LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.lower($1)';
CREATE OR REPLACE FUNCTION pg_catalog.lcase(char) RETURNS varchar LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.lower($1)';
CREATE OR REPLACE FUNCTION pg_catalog.lcase(varchar) RETURNS varchar LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.lower($1)';

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
CREATE AGGREGATE pg_catalog.var_pop(json) (SFUNC = any_accum, cFUNC = numeric_collect, STYPE = numeric[], finalfunc = numeric_var_pop, initcond = '{0,0,0}', initcollect = '{0,0,0}');

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

CREATE OR REPLACE FUNCTION pg_catalog.upper(int8) RETURNS varchar LANGUAGE SQL IMMUTABLE STRICT AS 'select pg_catalog.upper($1::text)';
CREATE OR REPLACE FUNCTION pg_catalog.upper(numeric) RETURNS varchar LANGUAGE SQL IMMUTABLE STRICT AS 'select pg_catalog.upper($1::text)';
CREATE OR REPLACE FUNCTION pg_catalog.upper(date) RETURNS varchar LANGUAGE SQL IMMUTABLE STRICT AS 'select pg_catalog.upper($1::text)';
CREATE OR REPLACE FUNCTION pg_catalog.upper(time) RETURNS varchar LANGUAGE SQL IMMUTABLE STRICT AS 'select pg_catalog.upper($1::text)';
CREATE OR REPLACE FUNCTION pg_catalog.upper(timestamp(0) without time zone) RETURNS varchar LANGUAGE SQL IMMUTABLE STRICT AS 'select pg_catalog.upper($1::text)';
CREATE OR REPLACE FUNCTION pg_catalog.upper(timestamp(0) with time zone) RETURNS varchar LANGUAGE SQL IMMUTABLE STRICT AS 'select pg_catalog.upper($1::text)';
CREATE OR REPLACE FUNCTION pg_catalog.upper(binary) RETURNS varbinary LANGUAGE C IMMUTABLE STRICT AS '$libdir/dolphin',  'binary_varbinary';
CREATE OR REPLACE FUNCTION pg_catalog.upper(bit) RETURNS varbinary LANGUAGE SQL IMMUTABLE STRICT AS 'select pg_catalog.upper($1::text::binary)';
CREATE OR REPLACE FUNCTION pg_catalog.upper(boolean) RETURNS varchar LANGUAGE SQL IMMUTABLE STRICT AS 'select pg_catalog.upper($1::text)';
CREATE OR REPLACE FUNCTION pg_catalog.upper(year) RETURNS varchar LANGUAGE SQL IMMUTABLE STRICT AS 'select pg_catalog.upper($1::text)';
CREATE OR REPLACE FUNCTION pg_catalog.upper(char) RETURNS varchar LANGUAGE SQL IMMUTABLE STRICT AS 'select pg_catalog.upper($1::text)';
CREATE OR REPLACE FUNCTION pg_catalog.upper(varchar) RETURNS varchar LANGUAGE SQL IMMUTABLE STRICT AS 'select pg_catalog.upper($1::text)';
CREATE OR REPLACE FUNCTION pg_catalog.upper(varbinary) RETURNS varbinary LANGUAGE SQL IMMUTABLE STRICT AS 'select $1';
CREATE OR REPLACE FUNCTION pg_catalog.upper(tinyblob) RETURNS varbinary LANGUAGE SQL IMMUTABLE STRICT AS 'select pg_catalog.upper($1::binary)';
CREATE OR REPLACE FUNCTION pg_catalog.upper(blob) RETURNS blob LANGUAGE SQL IMMUTABLE STRICT AS 'select $1';
CREATE OR REPLACE FUNCTION pg_catalog.upper(mediumblob) RETURNS mediumblob LANGUAGE SQL IMMUTABLE STRICT AS 'select $1';
CREATE OR REPLACE FUNCTION pg_catalog.upper(longblob) RETURNS longblob LANGUAGE SQL IMMUTABLE STRICT AS 'select $1';
CREATE OR REPLACE FUNCTION pg_catalog.upper(anyenum) RETURNS varchar LANGUAGE SQL IMMUTABLE STRICT AS 'select pg_catalog.upper($1::text)';
CREATE OR REPLACE FUNCTION pg_catalog.upper(anyset) RETURNS varchar LANGUAGE SQL IMMUTABLE STRICT AS 'select pg_catalog.upper($1::text)';
CREATE OR REPLACE FUNCTION pg_catalog.upper(json) RETURNS text LANGUAGE SQL IMMUTABLE STRICT AS 'select pg_catalog.upper($1::text)';

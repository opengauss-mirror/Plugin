DO $for_og_310$
BEGIN
    -- add special script for version before 3.1.0, cause 3.1.0 is dolphin 1.0 and 5.0.0 is dolphin 1.0 too, but they are different.
    if working_version_num() <= 92780 then
        DROP CAST IF EXISTS (float8 as boolean);
        DROP FUNCTION IF EXISTS pg_catalog.float8_bool(float8);
        DROP CAST IF EXISTS (float4 as boolean);
        DROP FUNCTION IF EXISTS pg_catalog.float4_bool(float4);
        CREATE OR REPLACE FUNCTION pg_catalog.float8_bool(float) returns boolean LANGUAGE C immutable as '$libdir/dolphin', 'float8_bool';
        CREATE CAST (float as boolean) WITH FUNCTION float8_bool(float) AS IMPLICIT;

        DROP CAST IF EXISTS (date as boolean);
        DROP FUNCTION IF EXISTS pg_catalog.date_bool(float8);
        CREATE OR REPLACE FUNCTION pg_catalog.date_bool(date) returns boolean LANGUAGE C immutable as '$libdir/dolphin', 'date_bool';
        CREATE CAST (date as boolean) WITH FUNCTION date_bool(date) AS IMPLICIT;
        
        DROP CAST IF EXISTS (time as boolean);
        DROP FUNCTION IF EXISTS pg_catalog.time_bool(time);
        CREATE OR REPLACE FUNCTION pg_catalog.time_bool(time) returns boolean LANGUAGE C immutable as '$libdir/dolphin', 'time_bool';
        CREATE CAST (time as boolean) WITH FUNCTION time_bool(time) AS IMPLICIT;
        
        DROP CAST IF EXISTS (bit as boolean);
        DROP FUNCTION IF EXISTS pg_catalog.bit_bool(bit);
        CREATE OR REPLACE FUNCTION pg_catalog.bit_bool(bit) returns boolean LANGUAGE C immutable as '$libdir/dolphin', 'bit_bool';
        CREATE CAST (bit as boolean) WITH FUNCTION bit_bool(bit) AS IMPLICIT;
        DROP CAST IF EXISTS (text as boolean);
        DROP FUNCTION IF EXISTS pg_catalog.text_bool(text);
        DROP CAST IF EXISTS (varchar as boolean);
        DROP FUNCTION IF EXISTS pg_catalog.varchar_bool(varchar);
        DROP CAST IF EXISTS (char as boolean);
        DROP FUNCTION IF EXISTS pg_catalog.char_bool(char);
        DROP OPERATOR IF EXISTS pg_catalog.~~(bool, bool);
        DROP OPERATOR IF EXISTS pg_catalog.~~*(bool, bool);
        DROP OPERATOR IF EXISTS pg_catalog.~~(bool, text);
        DROP OPERATOR IF EXISTS pg_catalog.~~*(bool, text);
        DROP OPERATOR IF EXISTS pg_catalog.~~(text, bool);
        DROP OPERATOR IF EXISTS pg_catalog.~~*(text, bool);
        DROP OPERATOR IF EXISTS pg_catalog.!~~(bool, bool);
        DROP OPERATOR IF EXISTS pg_catalog.!~~*(bool, bool);
        DROP OPERATOR IF EXISTS pg_catalog.!~~(bool, text);
        DROP OPERATOR IF EXISTS pg_catalog.!~~*(bool, text);
        DROP OPERATOR IF EXISTS pg_catalog.!~~(text, bool);
        DROP OPERATOR IF EXISTS pg_catalog.!~~*(text, bool);
        DROP OPERATOR IF EXISTS pg_catalog.!~~(VarBit, VarBit);
        DROP OPERATOR IF EXISTS pg_catalog.!~~*(VarBit, VarBit);
        DROP OPERATOR IF EXISTS pg_catalog.~~(VarBit, VarBit);
        DROP OPERATOR IF EXISTS pg_catalog.~~*(VarBit, VarBit);
        DROP OPERATOR IF EXISTS pg_catalog.!~~(VarBit, bool);
        DROP OPERATOR IF EXISTS pg_catalog.!~~*(VarBit, bool);
        DROP OPERATOR IF EXISTS pg_catalog.!~~(bool, VarBit);
        DROP OPERATOR IF EXISTS pg_catalog.!~~*(bool, VarBit);
        DROP OPERATOR IF EXISTS pg_catalog.!~~(VarBit, text);
        DROP OPERATOR IF EXISTS pg_catalog.!~~*(VarBit, text);
        DROP OPERATOR IF EXISTS pg_catalog.!~~(text, VarBit);
        DROP OPERATOR IF EXISTS pg_catalog.!~~*(text, VarBit);
        DROP OPERATOR IF EXISTS pg_catalog.!~~(VarBit, bytea);
        DROP OPERATOR IF EXISTS pg_catalog.!~~*(VarBit, bytea);
        DROP OPERATOR IF EXISTS pg_catalog.!~~(bytea, VarBit);
        DROP OPERATOR IF EXISTS pg_catalog.!~~*(bytea, VarBit);
        DROP OPERATOR IF EXISTS pg_catalog.!~~(VarBit, blob);
        DROP OPERATOR IF EXISTS pg_catalog.!~~*(VarBit, blob);
        DROP OPERATOR IF EXISTS pg_catalog.!~~(blob, VarBit);
        DROP OPERATOR IF EXISTS pg_catalog.!~~*(blob, VarBit);

        DROP OPERATOR IF EXISTS pg_catalog.~~(VarBit, bool);
        DROP OPERATOR IF EXISTS pg_catalog.~~*(VarBit, bool);
        DROP OPERATOR IF EXISTS pg_catalog.~~(bool, VarBit);
        DROP OPERATOR IF EXISTS pg_catalog.~~*(bool, VarBit);
        DROP OPERATOR IF EXISTS pg_catalog.~~(VarBit, text);
        DROP OPERATOR IF EXISTS pg_catalog.~~*(VarBit, text);
        DROP OPERATOR IF EXISTS pg_catalog.~~(text, VarBit);
        DROP OPERATOR IF EXISTS pg_catalog.~~*(text, VarBit);
        DROP OPERATOR IF EXISTS pg_catalog.~~(VarBit, bytea);
        DROP OPERATOR IF EXISTS pg_catalog.~~*(VarBit, bytea);
        DROP OPERATOR IF EXISTS pg_catalog.~~(bytea, VarBit);
        DROP OPERATOR IF EXISTS pg_catalog.~~*(bytea, VarBit);
        DROP OPERATOR IF EXISTS pg_catalog.~~(VarBit, blob);
        DROP OPERATOR IF EXISTS pg_catalog.~~*(VarBit, blob);
        DROP OPERATOR IF EXISTS pg_catalog.~~(blob, VarBit);
        DROP OPERATOR IF EXISTS pg_catalog.~~*(blob, VarBit);

        DROP OPERATOR IF EXISTS pg_catalog.~~*(bytea, bytea);
        DROP OPERATOR IF EXISTS pg_catalog.!~~*(bytea, bytea);

        DROP OPERATOR IF EXISTS pg_catalog.~~*(raw, raw);
        DROP OPERATOR IF EXISTS pg_catalog.!~~*(raw, raw);
        DROP FUNCTION IF EXISTS pg_catalog.boolboollike(
        bool,
        bool
        );
        DROP FUNCTION IF EXISTS pg_catalog.booltextlike(
        bool,
        text
        );
        DROP FUNCTION IF EXISTS pg_catalog.textboollike(
        text,
        bool
        );
        DROP FUNCTION IF EXISTS pg_catalog.boolboolnlike(
        bool,
        bool
        );

        DROP FUNCTION IF EXISTS pg_catalog.booltextnlike(
        bool,
        text
        );
        DROP FUNCTION IF EXISTS pg_catalog.textboolnlike(
        text,
        bool
        );
        DROP FUNCTION IF EXISTS pg_catalog.bitlike(
        VarBit,
        VarBit
        );
        DROP FUNCTION IF EXISTS pg_catalog.bitnlike(
        VarBit,
        VarBit
        );

        DROP FUNCTION IF EXISTS pg_catalog.bitothernlike(
        VarBit,
        bool
        );

        DROP FUNCTION IF EXISTS pg_catalog.bitothern2like(
        bool,
        VarBit
        );
        DROP FUNCTION IF EXISTS pg_catalog.bitothern3like(
        VarBit,
        text
        );
        DROP FUNCTION IF EXISTS pg_catalog.bitothern4like(
        text,
        VarBit
        );
        DROP FUNCTION IF EXISTS pg_catalog.bitothern5like(
        VarBit,
        bytea
        );
        DROP FUNCTION IF EXISTS pg_catalog.bitothern6like(
        bytea,
        VarBit
        );
        DROP FUNCTION IF EXISTS pg_catalog.bitothern7like(
        VarBit,
        blob
        );
        DROP FUNCTION IF EXISTS pg_catalog.bitothern8like(
        blob,
        VarBit
        );
        DROP FUNCTION IF EXISTS pg_catalog.bitotherlike(
        VarBit,
        bool
        );
        DROP FUNCTION IF EXISTS pg_catalog.bitother2like(
        bool,
        VarBit
        );
        DROP FUNCTION IF EXISTS pg_catalog.bitother3like(
        VarBit,
        text
        );
        DROP FUNCTION IF EXISTS pg_catalog.bitother4like(
        text,
        VarBit
        );
        DROP FUNCTION IF EXISTS pg_catalog.bitother5like(
        VarBit,
        bytea
        );
        DROP FUNCTION IF EXISTS pg_catalog.bitother6like(
        bytea,
        VarBit
        );
        DROP FUNCTION IF EXISTS pg_catalog.bitother7like(
        VarBit,
        blob
        );
        DROP FUNCTION IF EXISTS pg_catalog.bitother8like(
        blob,
        VarBit
        );

        DROP FUNCTION IF EXISTS pg_catalog.b_between_and("any","any","any");

        DROP FUNCTION IF EXISTS pg_catalog.b_sym_between_and("any","any","any");

        DROP FUNCTION IF EXISTS pg_catalog.b_not_between_and("any","any","any");

        DROP FUNCTION IF EXISTS pg_catalog.b_not_sym_between_and("any","any","any");

        DROP FUNCTION IF EXISTS pg_catalog.dolphin_version();
        update pg_catalog.pg_cast set castfunc = 401, castowner = 10 where castsource = 1042 and casttarget = 25;
        update pg_catalog.pg_cast set castfunc = 401, castowner = 10 where castsource = 1042 and casttarget = 1043;
        update pg_catalog.pg_cast set castfunc = 401, castowner = 10 where castsource = 1042 and casttarget = 3969;
        DROP FUNCTION IF EXISTS pg_catalog.bpchar_text(bpchar);
        
        drop aggregate if exists pg_catalog.any_value(bigint);
        drop aggregate if exists pg_catalog.any_value(numeric);
        drop aggregate if exists pg_catalog.any_value(double precision);
        drop aggregate if exists pg_catalog.any_value(float4);
        drop aggregate if exists pg_catalog.any_value(text);
        drop aggregate if exists pg_catalog.any_value(bytea);
        drop aggregate if exists pg_catalog.any_value(blob);

        DROP FUNCTION IF EXISTS pg_catalog.bigint_any_value (bigint, bigint);
        DROP FUNCTION IF EXISTS pg_catalog.numeric_any_value (numeric, numeric);
        DROP FUNCTION IF EXISTS pg_catalog.double_any_value (double precision, double precision);
        DROP FUNCTION IF EXISTS pg_catalog.float_any_value (float4, float4);
        DROP FUNCTION IF EXISTS pg_catalog.text_any_value (text, text);
        DROP FUNCTION IF EXISTS pg_catalog.bytea_any_value (bytea, bytea);
        DROP FUNCTION IF EXISTS pg_catalog.blob_any_value (blob, blob);
        DROP FUNCTION IF EXISTS  pg_catalog.dolphin_attname_eq(name, name);

        DROP OPERATOR IF EXISTS pg_catalog.~~(binary, text);
        DROP OPERATOR IF EXISTS pg_catalog.~~*(binary, text);
        DROP OPERATOR IF EXISTS pg_catalog.~~(text, binary);
        DROP OPERATOR IF EXISTS pg_catalog.~~*(text, binary);
        DROP OPERATOR IF EXISTS pg_catalog.!~~(binary, text);
        DROP OPERATOR IF EXISTS pg_catalog.!~~*(binary, text);
        DROP OPERATOR IF EXISTS pg_catalog.!~~(text, binary);
        DROP OPERATOR IF EXISTS pg_catalog.!~~*(text, binary);
        DROP OPERATOR IF EXISTS pg_catalog.=(binary, binary);
        DROP OPERATOR IF EXISTS pg_catalog.<>(binary, binary);
        DROP OPERATOR IF EXISTS pg_catalog.>(binary, binary);
        DROP OPERATOR IF EXISTS pg_catalog.<(binary, binary);
        DROP OPERATOR IF EXISTS pg_catalog.>=(binary, binary);
        DROP OPERATOR IF EXISTS pg_catalog.<=(binary, binary);
        DROP OPERATOR IF EXISTS pg_catalog.=(text, varbinary);
        DROP OPERATOR IF EXISTS pg_catalog.<>(text, varbinary);
        DROP OPERATOR IF EXISTS pg_catalog.>(text, varbinary);
        DROP OPERATOR IF EXISTS pg_catalog.<(text, varbinary);
        DROP OPERATOR IF EXISTS pg_catalog.>=(text, varbinary);
        DROP OPERATOR IF EXISTS pg_catalog.<=(text, varbinary);
        DROP OPERATOR IF EXISTS pg_catalog.=(varbinary, text);

        DROP OPERATOR IF EXISTS pg_catalog.<>(varbinary, text);
        DROP OPERATOR IF EXISTS pg_catalog.>(varbinary, text);
        DROP OPERATOR IF EXISTS pg_catalog.<(varbinary, text);
        DROP OPERATOR IF EXISTS pg_catalog.>=(varbinary, text);
        DROP OPERATOR IF EXISTS pg_catalog.<=(varbinary, text);
        DROP OPERATOR IF EXISTS pg_catalog.=(binary, varbinary);
        DROP OPERATOR IF EXISTS pg_catalog.<>(binary, varbinary);
        DROP OPERATOR IF EXISTS pg_catalog.>(binary, varbinary);
        DROP OPERATOR IF EXISTS pg_catalog.<(binary, varbinary);
        DROP OPERATOR IF EXISTS pg_catalog.>=(binary, varbinary);
        DROP OPERATOR IF EXISTS pg_catalog.<=(binary, varbinary);
        DROP OPERATOR IF EXISTS pg_catalog.=(varbinary, binary);
        DROP OPERATOR IF EXISTS pg_catalog.<>(varbinary, binary);
        DROP OPERATOR IF EXISTS pg_catalog.>(varbinary, binary);
        DROP OPERATOR IF EXISTS pg_catalog.<(varbinary, binary);
        DROP OPERATOR IF EXISTS pg_catalog.>=(varbinary, binary);
        DROP OPERATOR IF EXISTS pg_catalog.<=(varbinary, binary);
        DROP OPERATOR IF EXISTS pg_catalog.=(varbinary, varbinary);
        DROP OPERATOR IF EXISTS pg_catalog.<>(varbinary, varbinary);
        DROP OPERATOR IF EXISTS pg_catalog.>(varbinary, varbinary);
        DROP OPERATOR IF EXISTS pg_catalog.<(varbinary, varbinary);
        DROP OPERATOR IF EXISTS pg_catalog.>=(varbinary, varbinary);
        DROP OPERATOR IF EXISTS pg_catalog.<=(varbinary, varbinary);

        CREATE OR REPLACE FUNCTION pg_catalog.binary_in (
        cstring
        ) RETURNS binary LANGUAGE INTERNAL IMMUTABLE STRICT as 'byteain';
        
        DROP FUNCTION IF EXISTS pg_catalog.binary_in (
        text
        );

        DROP FUNCTION IF EXISTS pg_catalog.binarytextlike(
        binary,
        text
        );

        DROP FUNCTION IF EXISTS pg_catalog.textbinarylike(
        text,
        binary
        );
        DROP FUNCTION IF EXISTS pg_catalog.binarytextnlike(
        binary,
        text
        );
        DROP FUNCTION IF EXISTS pg_catalog.textbinarynlike(
        text,
        binary
        );
        DROP FUNCTION IF EXISTS pg_catalog.like_escape(
        binary,
        text
        );
        DROP FUNCTION IF EXISTS pg_catalog.binaryeq(
        binary,
        binary
        );

        DROP FUNCTION IF EXISTS pg_catalog.binaryne(
        binary,
        binary
        );

        DROP FUNCTION IF EXISTS pg_catalog.binarygt(
        binary,
        binary
        );
        DROP FUNCTION IF EXISTS pg_catalog.binarylt(
        binary,
        binary
        );
        DROP FUNCTION IF EXISTS pg_catalog.binaryge(
        binary,
        binary
        );
        DROP FUNCTION IF EXISTS pg_catalog.binaryle(
        binary,
        binary
        );
        
        DROP FUNCTION IF EXISTS pg_catalog.text_varbinary_eq(
        text,
        varbinary
        );

        DROP FUNCTION IF EXISTS pg_catalog.text_varbinary_ne(
        text,
        varbinary
        );


        DROP FUNCTION IF EXISTS pg_catalog.text_varbinary_gt(
        text,
        varbinary
        );

        DROP FUNCTION IF EXISTS pg_catalog.text_varbinary_lt(
        text,
        varbinary
        );


        DROP FUNCTION IF EXISTS pg_catalog.text_varbinary_ge(
        text,
        varbinary
        );


        DROP FUNCTION IF EXISTS pg_catalog.text_varbinary_le(
        text,
        varbinary
        );


        DROP FUNCTION IF EXISTS pg_catalog.varbinary_text_eq(
        varbinary,
        text
        );


        DROP FUNCTION IF EXISTS pg_catalog.varbinary_text_ne(
        varbinary,
        text
        );

        DROP FUNCTION IF EXISTS pg_catalog.varbinary_text_gt(
        varbinary,
        text
        );
        DROP FUNCTION IF EXISTS pg_catalog.varbinary_text_lt(
        varbinary,
        text
        );
        DROP FUNCTION IF EXISTS pg_catalog.varbinary_text_ge(
        varbinary,
        text
        );
        DROP FUNCTION IF EXISTS pg_catalog.varbinary_text_le(
        varbinary,
        text
        );
        DROP FUNCTION IF EXISTS pg_catalog.binary_varbinary_eq(
        binary,
        varbinary
        );
        DROP FUNCTION IF EXISTS pg_catalog.binary_varbinary_ne(
        binary,
        varbinary
        );
        DROP FUNCTION IF EXISTS pg_catalog.binary_varbinary_gt(
        binary,
        varbinary
        );
        DROP FUNCTION IF EXISTS pg_catalog.binary_varbinary_lt(
        binary,
        varbinary
        );
        DROP FUNCTION IF EXISTS pg_catalog.binary_varbinary_ge(
        binary,
        varbinary
        );

        DROP FUNCTION IF EXISTS pg_catalog.binary_varbinary_le(
        binary,
        varbinary
        );
        DROP FUNCTION IF EXISTS pg_catalog.varbinary_binary_eq(
        varbinary,
        binary
        );
        DROP FUNCTION IF EXISTS pg_catalog.varbinary_binary_ne(
        varbinary,
        binary
        );
        DROP FUNCTION IF EXISTS pg_catalog.varbinary_binary_gt(
        varbinary,
        binary
        );
        DROP FUNCTION IF EXISTS pg_catalog.varbinary_binary_lt(
        varbinary,
        binary
        );
        DROP FUNCTION IF EXISTS pg_catalog.varbinary_binary_ge(
        varbinary,
        binary
        );
        DROP FUNCTION IF EXISTS pg_catalog.varbinary_binary_le(
        varbinary,
        binary
        );
        DROP FUNCTION IF EXISTS pg_catalog.varbinary_varbinary_eq(
        varbinary,
        varbinary
        );
        DROP FUNCTION IF EXISTS pg_catalog.varbinary_varbinary_ne(
        varbinary,
        varbinary
        );
        DROP FUNCTION IF EXISTS pg_catalog.varbinary_varbinary_gt(
        varbinary,
        varbinary
        );
        DROP FUNCTION IF EXISTS pg_catalog.varbinary_varbinary_lt(
        varbinary,
        varbinary
        );
        DROP FUNCTION IF EXISTS pg_catalog.varbinary_varbinary_ge(
        varbinary,
        varbinary
        );
        DROP FUNCTION IF EXISTS pg_catalog.varbinary_varbinary_le(
        varbinary,
        varbinary
        );

        CREATE OR REPLACE FUNCTION pg_catalog.bit2numeric (bit) RETURNS numeric AS
        $$
        BEGIN
            RETURN (SELECT int8($1));
        END;
        $$
        LANGUAGE plpgsql;

        CREATE OR REPLACE FUNCTION pg_catalog.tinyblob_rawout (
        tinyblob
        ) RETURNS cstring LANGUAGE INTERNAL IMMUTABLE STRICT as 'rawout';
        

        CREATE OR REPLACE FUNCTION pg_catalog.mediumblob_rawout (
        mediumblob
        ) RETURNS cstring LANGUAGE INTERNAL IMMUTABLE STRICT as 'rawout';
        

        CREATE OR REPLACE FUNCTION pg_catalog.longblob_rawout (
        longblob
        ) RETURNS cstring LANGUAGE INTERNAL IMMUTABLE STRICT as 'rawout';
        
        drop operator IF EXISTS pg_catalog.^(int1, int1);
        
        drop operator if exists pg_catalog.^(blob, blob);
        drop operator if exists pg_catalog.^(blob, integer);
        drop operator if exists pg_catalog.^(integer, blob);
        drop operator if exists pg_catalog.^(int8, blob);
        drop operator if exists pg_catalog.^(blob, int8);
        drop operator if exists pg_catalog.^(float8, blob);
        drop operator if exists pg_catalog.^(blob, float8);

        drop operator if exists pg_catalog.=(blob, blob);
        drop operator if exists pg_catalog.<>(blob, blob);
        drop operator if exists pg_catalog.<(blob, blob);
        drop operator if exists pg_catalog.<=(blob, blob);
        drop operator if exists pg_catalog.>(blob, blob);
        drop operator if exists pg_catalog.>=(blob, blob);

        drop operator if exists pg_catalog.=(blob, text);
        drop operator if exists pg_catalog.<>(blob, text);
        drop operator if exists pg_catalog.<(blob, text);
        drop operator if exists pg_catalog.<=(blob, text);
        drop operator if exists pg_catalog.>(blob, text);
        drop operator if exists pg_catalog.>=(blob, text);

        drop operator if exists pg_catalog.=(text, blob);
        drop operator if exists pg_catalog.<>(text, blob);
        drop operator if exists pg_catalog.<(text, blob);
        drop operator if exists pg_catalog.<=(text, blob);
        drop operator if exists pg_catalog.>(text, blob);
        drop operator if exists pg_catalog.>=(text, blob);

        DROP CAST IF EXISTS (bytea AS float8);
        DROP CAST IF EXISTS ("binary" AS float8);
        DROP CAST IF EXISTS ("varbinary" AS float8);
        DROP CAST IF EXISTS (blob AS float8);
        DROP CAST IF EXISTS (tinyblob AS float8);
        DROP CAST IF EXISTS (mediumblob AS float8);
        DROP CAST IF EXISTS (longblob AS float8);
        DROP CAST IF EXISTS (json AS float8);

        drop function if exists pg_catalog.blobxor(
        blob,
        blob
        );
        drop function if exists pg_catalog.blobxor(
        blob,
        int
        );
        drop function if exists pg_catalog.blobxor(
        int,
        blob
        );
        drop function if exists pg_catalog.blobxor(
        int8,
        blob
        );
        drop function if exists pg_catalog.blobxor(
        blob,
        int8
        );
        drop function if exists pg_catalog.blobxor(
        float8,
        blob
        );
        drop function if exists pg_catalog.blobxor(
        blob,
        float8
        );
        drop aggregate if exists pg_catalog.bit_xor(longblob);
        drop aggregate if exists pg_catalog.bit_xor(blob);

        DROP FUNCTION IF EXISTS pg_catalog.bit_longblob(uint8,longblob);
        DROP FUNCTION IF EXISTS pg_catalog.bit_blob(uint8,blob);
        DROP FUNCTION IF EXISTS pg_catalog.varlena2float8(anyelement);

        DROP FUNCTION IF EXISTS pg_catalog.blob_eq(blob, blob);
        DROP FUNCTION IF EXISTS pg_catalog.blob_ne(blob, blob);
        DROP FUNCTION IF EXISTS pg_catalog.blob_lt(blob, blob);
        DROP FUNCTION IF EXISTS pg_catalog.blob_le(blob, blob);
        DROP FUNCTION IF EXISTS pg_catalog.blob_gt(blob, blob);
        DROP FUNCTION IF EXISTS pg_catalog.blob_ge(blob, blob);

        DROP FUNCTION IF EXISTS pg_catalog.blob_eq_text(blob, text);
        DROP FUNCTION IF EXISTS pg_catalog.blob_ne_text(blob, text);
        DROP FUNCTION IF EXISTS pg_catalog.blob_lt_text(blob, text);
        DROP FUNCTION IF EXISTS pg_catalog.blob_le_text(blob, text);
        DROP FUNCTION IF EXISTS pg_catalog.blob_gt_text(blob, text);
        DROP FUNCTION IF EXISTS pg_catalog.blob_ge_text(blob, text);

        DROP FUNCTION IF EXISTS pg_catalog.text_eq_blob(text, blob);
        DROP FUNCTION IF EXISTS pg_catalog.text_ne_blob(text, blob);
        DROP FUNCTION IF EXISTS pg_catalog.text_lt_blob(text, blob);
        DROP FUNCTION IF EXISTS pg_catalog.text_le_blob(text, blob);
        DROP FUNCTION IF EXISTS pg_catalog.test_gt_blob(text, blob);
        DROP FUNCTION IF EXISTS pg_catalog.test_ge_blob(text, blob);

        DROP FUNCTION IF EXISTS pg_catalog.show_character_set(OUT NAME,OUT TEXT,OUT TEXT,OUT INT4,OUT BOOL);
        DROP FUNCTION IF EXISTS pg_catalog.show_collation(OUT NAME, OUT NAME,OUT OID,OUT TEXT,OUT TEXT,OUT  INT4);
        
        DROP FUNCTION IF EXISTS pg_catalog.dolphin_types();
        DROP FUNCTION IF EXISTS pg_catalog.gs_get_viewdef_oid(integer);
        DROP FUNCTION IF EXISTS pg_catalog.system_user();
        DROP FUNCTION IF EXISTS pg_catalog.database();
        DROP operator IF EXISTS pg_catalog.^(boolean, boolean);
        DROP operator IF EXISTS pg_catalog.^(float8, boolean);
        DROP operator IF EXISTS pg_catalog.^(boolean, float8);

        DROP FUNCTION IF EXISTS pg_catalog.boolxor (
        boolean,
        boolean
        );

        DROP FUNCTION IF EXISTS pg_catalog.ord (text);
        DROP FUNCTION IF EXISTS pg_catalog.ord (numeric);
        DROP FUNCTION IF EXISTS pg_catalog.substring_index (text, text, numeric);
        DROP FUNCTION IF EXISTS pg_catalog.substring_index (boolean, text, numeric);
        DROP FUNCTION IF EXISTS pg_catalog.substring_index (text, boolean, numeric);
        DROP FUNCTION IF EXISTS pg_catalog.substring_index (boolean, boolean, numeric);
        DROP FUNCTION IF EXISTS pg_catalog.bool_float8_xor(
        boolean,
        float8
        );
        DROP FUNCTION IF EXISTS pg_catalog.float8_bool_xor(
        float8,
        boolean
        );

        CREATE OR REPLACE FUNCTION pg_catalog.b_db_sys_real_timestamp(integer) returns datetime LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'b_db_sys_real_timestamp';
        CREATE OR REPLACE FUNCTION pg_catalog.b_db_statement_start_timestamp(integer) returns datetime LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'b_db_statement_start_timestamp';

        DROP CAST IF EXISTS (int8 AS time);
        DROP FUNCTION IF EXISTS pg_catalog.int64_b_format_time (int8);

        DROP FUNCTION IF EXISTS pg_catalog.year (text);
        DROP FUNCTION IF EXISTS pg_catalog.sec_to_time (text);
        
        DROP FUNCTION IF EXISTS pg_catalog.subtime ("any", "any");
        DROP FUNCTION IF EXISTS pg_catalog.timediff ("any", "any");
        CREATE OR REPLACE FUNCTION pg_catalog.subtime (text, text) RETURNS TEXT LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'subtime_text';
        CREATE OR REPLACE FUNCTION pg_catalog.subtime (date, text) RETURNS TEXT LANGUAGE SQL STABLE STRICT as 'select pg_catalog.subtime(cast(0 as time), $2)';
        CREATE OR REPLACE FUNCTION pg_catalog.subtime (date, datetime) RETURNS TEXT LANGUAGE SQL STABLE STRICT as 'select pg_catalog.subtime(cast(0 as time), cast($2 as time))';
        CREATE OR REPLACE FUNCTION pg_catalog.subtime (date, date) RETURNS TEXT LANGUAGE SQL STABLE STRICT as 'select pg_catalog.subtime(cast(0 as time), cast(0 as time))';
        CREATE OR REPLACE FUNCTION pg_catalog.subtime (text, datetime) RETURNS TEXT LANGUAGE SQL STABLE STRICT as 'select pg_catalog.subtime($1, cast($2 as time))';
        CREATE OR REPLACE FUNCTION pg_catalog.subtime (text, date) RETURNS TEXT LANGUAGE SQL STABLE STRICT as 'select pg_catalog.subtime($1, cast(0 as time))';
        CREATE OR REPLACE FUNCTION pg_catalog.subtime (numeric, numeric) RETURNS TEXT LANGUAGE SQL STABLE STRICT as 'select pg_catalog.subtime(cast($1 as text), cast($2 as text))';
        CREATE OR REPLACE FUNCTION pg_catalog.subtime (text, numeric) RETURNS TEXT LANGUAGE SQL STABLE STRICT as 'select pg_catalog.subtime($1, cast($2 as text))';
        CREATE OR REPLACE FUNCTION pg_catalog.subtime (numeric, text) RETURNS TEXT LANGUAGE SQL STABLE STRICT as 'select pg_catalog.subtime(cast($1 as text), $2)';

        CREATE OR REPLACE FUNCTION pg_catalog.timediff (text, text) RETURNS time LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'timediff_text';
        CREATE OR REPLACE FUNCTION pg_catalog.timediff (datetime, text) RETURNS time LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'timediff_datetime_text';
        CREATE OR REPLACE FUNCTION pg_catalog.timediff (time, text) RETURNS time LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'timediff_time_text';
        CREATE OR REPLACE FUNCTION pg_catalog.timediff (datetime, time) RETURNS time LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'func_return_null';
        CREATE OR REPLACE FUNCTION pg_catalog.timediff (time, date) RETURNS time LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'func_return_null';
        CREATE OR REPLACE FUNCTION pg_catalog.timediff (date, text) RETURNS time LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'timediff_date_text';
        CREATE OR REPLACE FUNCTION pg_catalog.timediff (date, time) RETURNS time LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'func_return_null';
        CREATE OR REPLACE FUNCTION pg_catalog.timediff (numeric, numeric) RETURNS time LANGUAGE SQL STABLE STRICT as 'select pg_catalog.timediff(cast($1 as text), cast($2 as text))';
        CREATE OR REPLACE FUNCTION pg_catalog.timediff (text, numeric) RETURNS time LANGUAGE SQL STABLE STRICT as 'select pg_catalog.timediff($1, cast($2 as text))';
        CREATE OR REPLACE FUNCTION pg_catalog.timediff (numeric, text) RETURNS time LANGUAGE SQL STABLE STRICT as 'select pg_catalog.timediff(cast($1 as text), $2)';

        DROP FUNCTION IF EXISTS pg_catalog.time_format (date, text);
        CREATE OR REPLACE FUNCTION pg_catalog.time_format (date, text) RETURNS TEXT AS $$ SELECT pg_catalog.time_format('00:00:00', $2) $$ LANGUAGE SQL;

        DROP FUNCTION IF EXISTS pg_catalog.time_mysql ("any");
        CREATE OR REPLACE FUNCTION pg_catalog.time_mysql (text) RETURNS text LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'time_mysql';
        CREATE OR REPLACE FUNCTION pg_catalog.time_mysql (numeric) RETURNS text AS $$ SELECT pg_catalog.time_mysql(cast($1 as text))  $$ LANGUAGE SQL;
        CREATE OR REPLACE FUNCTION pg_catalog.time_mysql (date) RETURNS text AS $$ SELECT pg_catalog.time_mysql('00:00:00')  $$ LANGUAGE SQL;

        DROP FUNCTION IF EXISTS pg_catalog.timestamp_mysql ("any");
        DROP FUNCTION IF EXISTS pg_catalog.timestamp_mysql ("any", "any");
        CREATE OR REPLACE FUNCTION pg_catalog.timestamp_mysql (datetime) RETURNS datetime LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'timestamp_param1';
        CREATE OR REPLACE FUNCTION pg_catalog.timestamp_mysql (datetime, text) RETURNS datetime LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'timestamp_param2';
        CREATE OR REPLACE FUNCTION pg_catalog.timestamp_mysql (text, text) RETURNS datetime AS $$ SELECT pg_catalog.timestamp_mysql(pg_catalog.timestamp_mysql($1), $2)  $$ LANGUAGE SQL;
        CREATE OR REPLACE FUNCTION pg_catalog.timestamp_mysql (time, text) RETURNS datetime AS $$ SELECT pg_catalog.timestamp_mysql(pg_catalog.timestamp_mysql((clock_timestamp()::date), $1), $2)  $$ LANGUAGE SQL;
        CREATE OR REPLACE FUNCTION pg_catalog.timestamp_mysql (time) RETURNS datetime AS $$ SELECT pg_catalog.timestamp_mysql((clock_timestamp()::date), $1)  $$ LANGUAGE SQL;
        
        DROP FUNCTION IF EXISTS pg_catalog.timestamp_add (text, numeric, "any");
        DROP FUNCTION IF EXISTS pg_catalog.timestamp_add (text, text, "any");
        CREATE OR REPLACE FUNCTION pg_catalog.timestamp_add (text, numeric, text) RETURNS text LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'timestamp_add_text';
        CREATE OR REPLACE FUNCTION pg_catalog.timestamp_add (text, numeric, time) RETURNS time LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'timestamp_add_time';
        CREATE OR REPLACE FUNCTION pg_catalog.timestamp_add (text, numeric, numeric) RETURNS text AS $$ SELECT pg_catalog.timestamp_add($1, $2, CAST($3 AS text)) $$ LANGUAGE SQL;

        DROP FUNCTION IF EXISTS pg_catalog.to_seconds ("any");
        CREATE OR REPLACE FUNCTION pg_catalog.to_seconds (text) RETURNS numeric LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'to_seconds_text';
        CREATE OR REPLACE FUNCTION pg_catalog.to_seconds (time) RETURNS numeric LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'to_seconds_time';
        CREATE OR REPLACE FUNCTION pg_catalog.to_seconds (numeric) RETURNS numeric LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'to_seconds_numeric';
    
        DROP FUNCTION IF EXISTS pg_catalog.unix_timestamp ("any");
        CREATE OR REPLACE FUNCTION pg_catalog.unix_timestamp (text) RETURNS numeric LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'unix_timestamp_text';
        CREATE OR REPLACE FUNCTION pg_catalog.unix_timestamp (time) RETURNS numeric LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'unix_timestamp_time';
        CREATE OR REPLACE FUNCTION pg_catalog.unix_timestamp (numeric) RETURNS numeric LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'unix_timestamp_numeric';

        DROP FUNCTION IF EXISTS pg_catalog.b_db_date(text);
        CREATE OR REPLACE FUNCTION pg_catalog.b_db_date(text) RETURNS text LANGUAGE C STABLE RETURNS NULL ON NULL INPUT as '$libdir/dolphin', 'b_db_date_text';
        DROP FUNCTION IF EXISTS pg_catalog.b_db_date(numeric);
        CREATE OR REPLACE FUNCTION pg_catalog.b_db_date(numeric) RETURNS text LANGUAGE C STABLE RETURNS NULL ON NULL INPUT as '$libdir/dolphin', 'b_db_date_numeric';
        
        DROP FUNCTION IF EXISTS pg_catalog.datediff(text, text);
        DROP FUNCTION IF EXISTS pg_catalog.datediff(text, numeric);
        DROP FUNCTION IF EXISTS pg_catalog.datediff(numeric, text);
        DROP FUNCTION IF EXISTS pg_catalog.datediff(numeric, numeric);
        DROP FUNCTION IF EXISTS pg_catalog.datediff(time, text);
        DROP FUNCTION IF EXISTS pg_catalog.datediff(time, numeric);
        DROP FUNCTION IF EXISTS pg_catalog.datediff(time, time);
        DROP FUNCTION IF EXISTS pg_catalog.datediff(text, time);
        DROP FUNCTION IF EXISTS pg_catalog.datediff(numeric, time);

        CREATE OR REPLACE FUNCTION pg_catalog.datediff(text, text) RETURNS int4 LANGUAGE C STABLE RETURNS NULL ON NULL INPUT as '$libdir/dolphin', 'datediff';
        CREATE OR REPLACE FUNCTION pg_catalog.datediff(text, numeric) RETURNS int4 AS $$ SELECT pg_catalog.datediff($1, cast($2 as text)) $$ LANGUAGE SQL;
        CREATE OR REPLACE FUNCTION pg_catalog.datediff(numeric, text) RETURNS int4 AS $$ SELECT pg_catalog.datediff(cast($1 as text), $2) $$ LANGUAGE SQL;
        CREATE OR REPLACE FUNCTION pg_catalog.datediff(numeric, numeric) RETURNS int4 AS $$ SELECT pg_catalog.datediff(cast($1 as text), cast($2 as text)) $$ LANGUAGE SQL;

        DROP FUNCTION IF EXISTS pg_catalog.b_timestampdiff(text,text,text);
        DROP FUNCTION IF EXISTS pg_catalog.b_timestampdiff(text,numeric,numeric);
        DROP FUNCTION IF EXISTS pg_catalog.b_timestampdiff(text,text,numeric);
        DROP FUNCTION IF EXISTS pg_catalog.b_timestampdiff(text,numeric,text);
        DROP FUNCTION IF EXISTS pg_catalog.b_timestampdiff(text,time,text);
        DROP FUNCTION IF EXISTS pg_catalog.b_timestampdiff(text,time,numeric);
        DROP FUNCTION IF EXISTS pg_catalog.b_timestampdiff(text,text,time);
        DROP FUNCTION IF EXISTS pg_catalog.b_timestampdiff(text,numeric,time);
        CREATE OR REPLACE FUNCTION pg_catalog.b_timestampdiff(text,text,text) RETURNS int8 LANGUAGE C STABLE RETURNS NULL ON NULL INPUT as '$libdir/dolphin', 'timestampdiff_datetime';
        CREATE OR REPLACE FUNCTION pg_catalog.b_timestampdiff(text,time,text) RETURNS int8 LANGUAGE C STABLE RETURNS NULL ON NULL INPUT as '$libdir/dolphin', 'timestampdiff_time_before';
        CREATE OR REPLACE FUNCTION pg_catalog.b_timestampdiff(text,text,time) RETURNS int8 LANGUAGE C STABLE RETURNS NULL ON NULL INPUT as '$libdir/dolphin', 'timestampdiff_time_after';
        
        
        DROP FUNCTION IF EXISTS pg_catalog.convert_tz(text,text,text);
        DROP FUNCTION IF EXISTS pg_catalog.convert_tz(numeric,text,text);
        DROP FUNCTION IF EXISTS pg_catalog.convert_tz(time,text,text);
        CREATE OR REPLACE FUNCTION pg_catalog.convert_tz(text,text,text) RETURNS datetime LANGUAGE C STABLE RETURNS NULL ON NULL INPUT as '$libdir/dolphin', 'convert_tz';
        CREATE OR REPLACE FUNCTION pg_catalog.convert_tz(numeric,text,text) RETURNS datetime AS $$ SELECT pg_catalog.convert_tz(cast($1 as text), $2, $3)  $$ LANGUAGE SQL;

        DROP FUNCTION IF EXISTS pg_catalog.adddate(text, int8);
        CREATE OR REPLACE FUNCTION pg_catalog.adddate (text, int8) RETURNS text LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'adddate_datetime_days_text';

        DROP FUNCTION IF EXISTS pg_catalog.adddate(text, interval);
        DROP FUNCTION IF EXISTS pg_catalog.adddate(numeric, int8);
        DROP FUNCTION IF EXISTS pg_catalog.adddate(numeric, interval);
        CREATE OR REPLACE FUNCTION pg_catalog.adddate (text, interval) RETURNS text LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'adddate_datetime_interval_text';
        CREATE OR REPLACE FUNCTION pg_catalog.adddate (numeric, int8) RETURNS text AS $$ SELECT pg_catalog.adddate(cast($1 as text), cast($2 as int8))  $$ LANGUAGE SQL;
        CREATE OR REPLACE FUNCTION pg_catalog.adddate (numeric, interval) RETURNS text AS $$ SELECT pg_catalog.adddate(cast($1 as text), $2)  $$ LANGUAGE SQL;

        DROP FUNCTION IF EXISTS pg_catalog.date_sub (text, interval);
        DROP FUNCTION IF EXISTS pg_catalog.date_sub (numeric, interval);
        DROP FUNCTION IF EXISTS pg_catalog.date_sub (time, interval);
        CREATE OR REPLACE FUNCTION pg_catalog.date_sub (text, interval) RETURNS text LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'subdate_datetime_interval_text';
        CREATE OR REPLACE FUNCTION pg_catalog.date_sub (numeric, interval) RETURNS text AS $$ SELECT pg_catalog.date_sub(cast($1 as text), $2)  $$ LANGUAGE SQL;
        CREATE OR REPLACE FUNCTION pg_catalog.date_sub (time, interval) RETURNS time LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'subdate_time_interval';

        DROP FUNCTION IF EXISTS pg_catalog.date_add (numeric, interval);
        CREATE OR REPLACE FUNCTION pg_catalog.date_add (numeric, interval) RETURNS text AS $$ SELECT pg_catalog.adddate(cast($1 as text), $2)  $$ LANGUAGE SQL;

        DROP FUNCTION IF EXISTS pg_catalog.hour (text);
        DROP FUNCTION IF EXISTS pg_catalog.microsecond (text);
        DROP FUNCTION IF EXISTS pg_catalog.minute (text);
        DROP FUNCTION IF EXISTS pg_catalog.second (text);

        DROP FUNCTION IF EXISTS pg_catalog.dayofmonth (timestamptz);
        DROP FUNCTION IF EXISTS pg_catalog.dayofmonth (timetz);
        DROP FUNCTION IF EXISTS pg_catalog.dayofmonth (abstime);
        DROP FUNCTION IF EXISTS pg_catalog.dayofmonth (date);
        DROP FUNCTION IF EXISTS pg_catalog.dayofmonth (time);
        DROP FUNCTION IF EXISTS pg_catalog.dayofmonth (timestamp(0) with time zone);

        DROP FUNCTION IF EXISTS pg_catalog.dayofweek (timestamptz);
        DROP FUNCTION IF EXISTS pg_catalog.dayofweek (timetz);
        DROP FUNCTION IF EXISTS pg_catalog.dayofweek (abstime);
        DROP FUNCTION IF EXISTS pg_catalog.dayofweek (date);
        DROP FUNCTION IF EXISTS pg_catalog.dayofweek (time);
        DROP FUNCTION IF EXISTS pg_catalog.dayofweek (timestamp(0) with time zone);

        DROP FUNCTION IF EXISTS pg_catalog.dayofyear (timestamptz);
        DROP FUNCTION IF EXISTS pg_catalog.dayofyear (timetz);
        DROP FUNCTION IF EXISTS pg_catalog.dayofyear (abstime);
        DROP FUNCTION IF EXISTS pg_catalog.dayofyear (date);
        DROP FUNCTION IF EXISTS pg_catalog.dayofyear (time);
        DROP FUNCTION IF EXISTS pg_catalog.dayofyear (timestamp(0) with time zone);

        DROP FUNCTION IF EXISTS pg_catalog.quarter (timestamptz);
        DROP FUNCTION IF EXISTS pg_catalog.quarter (timetz);
        DROP FUNCTION IF EXISTS pg_catalog.quarter (abstime);
        DROP FUNCTION IF EXISTS pg_catalog.quarter (date);
        DROP FUNCTION IF EXISTS pg_catalog.quarter (time);
        DROP FUNCTION IF EXISTS pg_catalog.quarter (timestamp(0) with time zone);

        DROP FUNCTION IF EXISTS pg_catalog.weekday (timestamptz);
        DROP FUNCTION IF EXISTS pg_catalog.weekday (timetz);
        DROP FUNCTION IF EXISTS pg_catalog.weekday (abstime);
        DROP FUNCTION IF EXISTS pg_catalog.weekday (date);
        DROP FUNCTION IF EXISTS pg_catalog.weekday (time);
        DROP FUNCTION IF EXISTS pg_catalog.weekday (timestamp(0) with time zone);

        DROP FUNCTION IF EXISTS pg_catalog.weekofyear (timestamptz);
        DROP FUNCTION IF EXISTS pg_catalog.weekofyear (timetz);
        DROP FUNCTION IF EXISTS pg_catalog.weekofyear (abstime);
        DROP FUNCTION IF EXISTS pg_catalog.weekofyear (date);
        DROP FUNCTION IF EXISTS pg_catalog.weekofyear (time);
        DROP FUNCTION IF EXISTS pg_catalog.weekofyear (timestamp(0) with time zone);

        DROP FUNCTION IF EXISTS pg_catalog.get_format (int4, text);

        DROP FUNCTION IF EXISTS pg_catalog.date_format (text, text);
        DROP FUNCTION IF EXISTS pg_catalog.date_format (numeric, text);

        DROP FUNCTION IF EXISTS pg_catalog.b_extract (text, text);
        DROP FUNCTION IF EXISTS pg_catalog.b_extract (text, numeric);

        DROP FUNCTION IF EXISTS pg_catalog.str_to_date (text, text);

        DROP FUNCTION IF EXISTS pg_catalog.from_unixtime (numeric);
        DROP FUNCTION IF EXISTS pg_catalog.from_unixtime (numeric, text);


        -- support bit_xor for date
        drop aggregate if exists pg_catalog.bit_xor(date);
        drop aggregate if exists pg_catalog.bit_xor(year);
        drop aggregate if exists pg_catalog.bit_xor(timestamp(0) with time zone);
        drop aggregate if exists pg_catalog.bit_xor(time);
        drop aggregate if exists pg_catalog.bit_xor(time with time zone);
        drop aggregate if exists pg_catalog.any_value(year);

        DROP FUNCTION IF EXISTS pg_catalog.date_xor_transfn(int16, date);
        DROP FUNCTION IF EXISTS pg_catalog.date_agg_finalfn(int16);
        -- support bit_xor aggregate for year
        DROP FUNCTION IF EXISTS pg_catalog.year_xor_transfn(integer, year);
        -- support bit_xor for datetime and timestamp
        DROP FUNCTION IF EXISTS pg_catalog.timestamp_xor_transfn(int16, timestamp(0) with time zone);
        DROP FUNCTION IF EXISTS pg_catalog.timestamp_agg_finalfn(int16);
        -- support bit_xor aggregate for time
        DROP FUNCTION IF EXISTS pg_catalog.time_xor_transfn(int16, time);
        DROP FUNCTION IF EXISTS pg_catalog.timetz_xor_transfn(int16, time with time zone);
        DROP FUNCTION IF EXISTS pg_catalog.year_any_value (year, year);

        drop CAST if exists (time as int8);
        DROP CAST IF EXISTS (time AS float8);
        drop CAST if exists (time as numeric);
        DROP CAST IF EXISTS (time AS integer);
        DROP CAST IF EXISTS (timestamp(0) without time zone AS float8);
        DROP CAST IF EXISTS (timestamp(0) without time zone AS bigint);
        DROP CAST IF EXISTS (date AS integer);
        drop OPERATOR if exists pg_catalog.+ (time, float8);
        drop OPERATOR if exists pg_catalog.- (time, float8);
        drop OPERATOR if exists pg_catalog.+ (timestamp(0) without time zone, float8);
        drop OPERATOR if exists pg_catalog.- (timestamp(0) without time zone,float8);
        drop OPERATOR if exists pg_catalog.^(date,date);
        drop OPERATOR if exists pg_catalog.^(time,time);
        drop OPERATOR if exists pg_catalog.^(date,time);
        drop OPERATOR if exists pg_catalog.^(time,date);
        drop OPERATOR if exists pg_catalog.^(time,text);
        drop OPERATOR if exists pg_catalog.^(text,time);
        drop OPERATOR if exists pg_catalog.^(date,text);
        drop OPERATOR if exists pg_catalog.^(text,date);
        drop OPERATOR if exists pg_catalog.^(date,int8);
        drop OPERATOR if exists pg_catalog.^(int8,date);
        drop OPERATOR if exists pg_catalog.^(time,int8);
        drop OPERATOR if exists pg_catalog.^(int8,time);
        drop OPERATOR if exists pg_catalog.^(int8,time);
        drop OPERATOR if exists pg_catalog.^(date,float8);
        drop OPERATOR if exists pg_catalog.^(float8,date);
        drop OPERATOR if exists pg_catalog.^(timestamp without time zone,timestamp without time zone);
        drop OPERATOR if exists pg_catalog.^(timestamp without time zone,int8);
        drop OPERATOR if exists pg_catalog.^(int8,timestamp without time zone);
        drop OPERATOR if exists pg_catalog.^(timestamp without time zone,float8);
        drop OPERATOR if exists pg_catalog.^(float8,timestamp without time zone);
        drop OPERATOR if exists pg_catalog.^(timestamp without time zone,text);
        drop OPERATOR if exists pg_catalog.^(text,timestamp without time zone);
        drop OPERATOR if exists pg_catalog.^(timestampTz,timestampTz);
        drop OPERATOR if exists pg_catalog.^(timestampTz,int8);
        drop OPERATOR if exists pg_catalog.^(int8,timestampTz);
        drop OPERATOR if exists pg_catalog.^(timestampTz,float8);
        drop OPERATOR if exists pg_catalog.^(float8,timestampTz);
        drop OPERATOR if exists pg_catalog.^(timestampTz,text);
        drop OPERATOR if exists pg_catalog.^(text,timestampTz);
        drop CAST if exists (timetz as int8) ;
        drop CAST if exists (timetz as float8);
        drop CAST if exists (timetz as numeric) ;
        drop CAST if exists ("timestamptz" as int8) ;
        drop CAST if exists (timestamp without time zone as numeric) ;
        drop CAST if exists ("timestamptz" as float8);
        drop CAST if exists ("timestamptz" as numeric) ;
        drop CAST if exists ("date" as int8);
        drop CAST if exists ("date" as float8) ;
        drop CAST if exists ("date" as numeric);
        drop CAST if exists ("year" as float8) ;
        drop CAST IF EXISTS (year AS numeric);

        DROP FUNCTION IF EXISTS pg_catalog.time_int8(time);
        DROP FUNCTION IF EXISTS pg_catalog.time_float (time);
        DROP FUNCTION IF EXISTS pg_catalog.time_numeric(time);

        DROP FUNCTION IF EXISTS pg_catalog.time_integer (time);

        DROP FUNCTION IF EXISTS pg_catalog.time_pl_float (time, float8);

        DROP FUNCTION IF EXISTS pg_catalog.time_mi_float (time, float8);

        DROP FUNCTION IF EXISTS pg_catalog.datetime_float (timestamp(0) without time zone);

        DROP FUNCTION IF EXISTS pg_catalog.datetime_bigint (timestamp(0) without time zone);

        DROP FUNCTION IF EXISTS pg_catalog.datetime_pl_float (timestamp(0) without time zone, float8);

        DROP FUNCTION IF EXISTS pg_catalog.datetime_mi_float (timestamp(0) without time zone, float8);

        DROP FUNCTION IF EXISTS pg_catalog.date_int (date);

        drop function IF EXISTS pg_catalog.datexor(
        date,
        date
        );

        drop function IF EXISTS pg_catalog.timexor(
        time,
        time
        );

        drop function IF EXISTS pg_catalog.date_time_xor(
        date,
        time
        );

        drop function IF EXISTS pg_catalog.time_date_xor(
        time,
        date
        );

        drop function IF EXISTS pg_catalog.time_text_xor(
        time,
        text
        );

        drop function IF EXISTS pg_catalog.text_time_xor(
        text,
        time
        );

        drop function IF EXISTS pg_catalog.date_text_xor(
        date,
        text
        );

        drop function IF EXISTS pg_catalog.text_date_xor(
        text,
        date
        );

        drop function IF EXISTS pg_catalog.date_int8_xor(
        date,
        int8
        );

        drop function IF EXISTS pg_catalog.int8_date_xor(
        int8,
        date
        );

        drop function IF EXISTS pg_catalog.time_int8_xor(
        time,
        int8
        );

        drop function IF EXISTS pg_catalog.int8_time_xor(
        int8,
        time
        );

        drop function IF EXISTS pg_catalog.date_float8_xor(
        date,
        float8
        );

        drop function IF EXISTS pg_catalog.float8_date_xor(
        float8,
        date
        ) ;

        drop function IF EXISTS pg_catalog.timestampxor(
        timestamp without time zone,
        timestamp without time zone
        ) ;

        drop function IF EXISTS pg_catalog.timestamp_int8_xor(
        timestamp without time zone,
        int8
        ) ;

        drop function IF EXISTS pg_catalog.int8_timestamp_xor(
        int8,
        timestamp without time zone
        );

        drop function IF EXISTS pg_catalog.timestamp_float8_xor(
        timestamp without time zone,
        float8
        ) ;


        drop function IF EXISTS pg_catalog.float8_timestamp_xor(
        float8,
        timestamp without time zone
        );

        drop function IF EXISTS pg_catalog.timestamp_text_xor(
        timestamp without time zone,
        text
        ) ;

        drop function IF EXISTS pg_catalog.text_timestamp_xor(
        text,
        timestamp without time zone
        ) ;

        drop function IF EXISTS pg_catalog.timestamptzxor(
        timestampTz,
        timestampTz
        ) ;

        drop function IF EXISTS pg_catalog.timestamptz_int8_xor(
        timestampTz,
        int8
        ) ;

        drop function IF EXISTS pg_catalog.int8_timestamptz_xor(
        int8,
        timestampTz
        );

        drop function IF EXISTS pg_catalog.timestamptz_float8_xor(
        timestampTz,
        float8
        ) ;


        drop function IF EXISTS pg_catalog.float8_timestamptz_xor(
        float8,
        timestampTz
        ) ;

        drop function IF EXISTS pg_catalog.timestamptz_text_xor(
        timestampTz,
        text
        ) ;

        drop function IF EXISTS pg_catalog.text_timestamptz_xor(
        text,
        timestampTz
        ) ;

        DROP FUNCTION IF EXISTS pg_catalog.sleep(float8);

        DROP FUNCTION IF EXISTS pg_catalog.timetz_int8(timetz);

        DROP FUNCTION IF EXISTS pg_catalog.timetz_float8(timetz);

        DROP FUNCTION IF EXISTS pg_catalog.timetz_numeric(timetz);

        DROP FUNCTION IF EXISTS pg_catalog.timestamp_numeric(timestamp without time zone);

        DROP FUNCTION IF EXISTS pg_catalog.timestamptz_int8("timestamptz");

        DROP FUNCTION IF EXISTS pg_catalog.timestamptz_float8("timestamptz");

        DROP FUNCTION IF EXISTS pg_catalog.timestamptz_numeric("timestamptz");

        DROP FUNCTION IF EXISTS pg_catalog.date_int8("date");

        DROP FUNCTION IF EXISTS pg_catalog.date2float8("date");

        DROP FUNCTION IF EXISTS pg_catalog.date_numeric("date");

        drop aggregate if exists pg_catalog.any_value(date);
        drop aggregate if exists pg_catalog.any_value(time);
        drop aggregate if exists pg_catalog.any_value(timetz);
        drop aggregate if exists pg_catalog.any_value(timestamp without time zone);
        drop aggregate if exists pg_catalog.any_value(timestamptz);
        DROP aggregate IF EXISTS pg_catalog.json_arrayagg("any");
        drop aggregate IF EXISTS pg_catalog.json_objectagg("any", "any");

        DROP FUNCTION IF EXISTS pg_catalog.date_any_value (date, date);

        DROP FUNCTION IF EXISTS pg_catalog.year_float8("year");

        DROP FUNCTION IF EXISTS pg_catalog.year_numeric("year");

        DROP FUNCTION IF EXISTS pg_catalog.time_any_value (time, time);
        DROP FUNCTION IF EXISTS pg_catalog.timetz_any_value (timetz, timetz);
        DROP FUNCTION IF EXISTS pg_catalog.timestamp_any_value (timestamp without time zone, timestamp without time zone);
        DROP FUNCTION IF EXISTS pg_catalog.timestamptz_any_value (timestamptz, timestamptz);
        DROP FUNCTION IF EXISTS pg_catalog.json_array();
        
        DROP FUNCTION IF EXISTS pg_catalog.json_contains("any", "any", text);
        DROP FUNCTION IF EXISTS pg_catalog.json_contains("any", "any");
        CREATE OR REPLACE FUNCTION pg_catalog.json_contains(json, json, text) RETURNS boolean LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'json_contains';
        CREATE OR REPLACE FUNCTION pg_catalog.json_contains(json, json) RETURNS boolean LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'json_contains';

        DROP FUNCTION IF EXISTS pg_catalog.json_contains_path("any", text, variadic text[]);
        CREATE OR REPLACE FUNCTION pg_catalog.json_contains_path(json, text, variadic text[]) RETURNS boolean LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'json_contains_path';
        
        DROP FUNCTION IF EXISTS pg_catalog.json_extract("any", variadic text[]);

        DROP FUNCTION IF EXISTS pg_catalog.json_keys("any");
        DROP FUNCTION IF EXISTS pg_catalog.json_keys("any",text);

        DROP FUNCTION IF EXISTS pg_catalog.json_search("any",text,"any");
        DROP FUNCTION IF EXISTS pg_catalog.json_search("any",text,"any","any");
        DROP FUNCTION IF EXISTS pg_catalog.json_search("any",text,"any","any",variadic text[]);

        DROP FUNCTION IF EXISTS pg_catalog.json_array_append("any", VARIADIC "any");

        DROP FUNCTION IF EXISTS pg_catalog.json_append("any", VARIADIC "any");

        DROP FUNCTION IF EXISTS pg_catalog.json_unquote("any");

        DROP FUNCTION IF EXISTS pg_catalog.json_merge_preserve(variadic arr "any");

        DROP FUNCTION IF EXISTS pg_catalog.json_merge(variadic arr "any");

        DROP FUNCTION IF EXISTS pg_catalog.json_merge_patch(variadic arr "any");

        DROP FUNCTION IF EXISTS pg_catalog.json_insert(variadic "any");

        DROP FUNCTION IF EXISTS pg_catalog.json_depth("any");

        DROP FUNCTION IF EXISTS pg_catalog.json_replace(variadic arr "any");

        DROP FUNCTION IF EXISTS pg_catalog.json_remove(variadic arr "any");

        DROP FUNCTION IF EXISTS pg_catalog.json_array_insert(variadic arr "any");

        DROP FUNCTION IF EXISTS pg_catalog.json_set(variadic "any");

        DROP FUNCTION IF EXISTS pg_catalog.json_length("any");
        DROP FUNCTION IF EXISTS pg_catalog.json_length("any",text);

        DROP FUNCTION IF EXISTS pg_catalog.json_objectagg_mysql_transfn (
            internal, "any", "any"
        );

        DROP FUNCTION IF EXISTS pg_catalog.json_objectagg_finalfn(internal);


        DROP FUNCTION IF EXISTS pg_catalog.json_valid("any");

        DROP FUNCTION IF EXISTS pg_catalog.json_storage_size("any");

        DROP FUNCTION IF EXISTS pg_catalog.json_pretty("any");

        DROP FUNCTION IF EXISTS pg_catalog.json_type("any");

        
        DROP FUNCTION IF EXISTS pg_catalog.json_extract(json, variadic text[]);

        CREATE FUNCTION pg_catalog.json_extract(json, variadic text[]) RETURNS json LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'json_extract';

        CREATE OR REPLACE FUNCTION pg_catalog.json_keys(json) RETURNS json LANGUAGE C STABLE STRICT as '$libdir/dolphin','json_keys';
        CREATE OR REPLACE FUNCTION pg_catalog.json_keys(json,text) RETURNS json LANGUAGE C STABLE STRICT as '$libdir/dolphin','json_keys';

        CREATE OR REPLACE FUNCTION pg_catalog.json_search(json,text,"any") RETURNS text LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'json_search';
        CREATE OR REPLACE FUNCTION pg_catalog.json_search(json,text,"any","any") RETURNS text LANGUAGE C STABLE as '$libdir/dolphin', 'json_search';
        CREATE OR REPLACE FUNCTION pg_catalog.json_search(json,text,"any","any",variadic text[]) RETURNS text LANGUAGE C STABLE as '$libdir/dolphin', 'json_search';

        CREATE OR REPLACE FUNCTION pg_catalog.json_array_append(json, VARIADIC "any") RETURNS json LANGUAGE C STABLE STRICT as '$libdir/dolphin','json_array_append';

        CREATE OR REPLACE FUNCTION pg_catalog.json_append(json, VARIADIC "any") RETURNS json LANGUAGE C STABLE STRICT as '$libdir/dolphin','json_append';

        CREATE OR REPLACE FUNCTION pg_catalog.json_unquote(text) RETURNS text LANGUAGE C STABLE STRICT as '$libdir/dolphin','json_unquote';
        CREATE OR REPLACE FUNCTION pg_catalog.json_unquote(json) RETURNS text LANGUAGE C STABLE STRICT as '$libdir/dolphin','json_unquote';

        CREATE OR REPLACE FUNCTION pg_catalog.json_merge_preserve(variadic json[]) RETURNS json LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'json_merge_preserve';

        CREATE OR REPLACE FUNCTION pg_catalog.json_merge(variadic json[]) RETURNS json LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'json_merge';

        CREATE OR REPLACE FUNCTION pg_catalog.json_merge_patch(variadic json[]) RETURNS json LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'json_merge_patch';

        CREATE OR REPLACE FUNCTION pg_catalog.json_insert(variadic "any") RETURNS json LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'json_insert';

        CREATE OR REPLACE FUNCTION pg_catalog.json_depth(json) RETURNS int LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'json_depth';

        CREATE OR REPLACE FUNCTION pg_catalog.json_replace(variadic arr "any") RETURNS json LANGUAGE C STABLE as '$libdir/dolphin', 'json_replace';

        CREATE OR REPLACE FUNCTION pg_catalog.json_remove(json, variadic "any") RETURNS json LANGUAGE C STABLE as '$libdir/dolphin', 'json_remove';

        CREATE OR REPLACE FUNCTION pg_catalog.json_array_insert(json, variadic "any") RETURNS json LANGUAGE C STABLE as '$libdir/dolphin', 'json_array_insert';

        CREATE OR REPLACE FUNCTION pg_catalog.json_set( variadic "any") RETURNS json LANGUAGE C STABLE as '$libdir/dolphin', 'json_set';

        DROP OPERATOR CLASS IF EXISTS uint8_ops USING hash;
        DROP OPERATOR IF EXISTS pg_catalog.=(uint8,uint4);
        CREATE OPERATOR CLASS uint8_ops
            DEFAULT FOR TYPE uint8 USING hash family integer_ops AS
        OPERATOR        1       = ,
        OPERATOR        1       =(uint8, int8),
        OPERATOR        1       =(int8, uint8),
        FUNCTION        1       hashuint8(uint8);

        DROP OPERATOR IF EXISTS pg_catalog.^(
        uint8, boolean);

        DROP OPERATOR IF EXISTS pg_catalog.^(
        uint4, boolean);
        DROP OPERATOR IF EXISTS pg_catalog.^(
        uint2, boolean
        );
        DROP OPERATOR IF EXISTS pg_catalog.^(
        uint1, boolean
        );
        DROP OPERATOR IF EXISTS pg_catalog.^(
        boolean, uint1
        );
        DROP OPERATOR IF EXISTS pg_catalog.^(
        boolean, uint2
        );
        DROP OPERATOR IF EXISTS pg_catalog.^(
        boolean, uint4
        );
        DROP OPERATOR IF EXISTS pg_catalog.^(
        boolean, uint8
        );
        drop CAST IF EXISTS (year AS uint8);

        DROP FUNCTION IF EXISTS pg_catalog.uint8_uint4_eq(uint8,uint4);
        DROP FUNCTION IF EXISTS pg_catalog.i1_cast_ui1(int1);
        DROP FUNCTION IF EXISTS pg_catalog.i1_cast_ui2(int1);
        DROP FUNCTION IF EXISTS pg_catalog.i1_cast_ui4(int1);
        DROP FUNCTION IF EXISTS pg_catalog.i1_cast_ui8(int1);
        DROP FUNCTION IF EXISTS pg_catalog.year_uint8("year");

        drop CAST IF EXISTS (bit AS uint4);
        drop CAST IF EXISTS (bit AS uint8);
        CREATE CAST (bit AS uint4) WITH FUNCTION bittouint4(bit);
        CREATE CAST (bit AS uint8) WITH FUNCTION bittouint8(bit);

        drop aggregate if exists pg_catalog.bit_xor(int8);
        drop aggregate if exists pg_catalog.bit_xor(text);
        drop aggregate if exists pg_catalog.any_value(uint8);
        DROP FUNCTION IF EXISTS pg_catalog.uint2_xor_bool(uint2, boolean) ;

        DROP FUNCTION IF EXISTS pg_catalog.uint1_xor_bool(uint1, boolean) ;

        DROP FUNCTION IF EXISTS pg_catalog.uint4_xor_bool(uint4, boolean) ;

        DROP FUNCTION IF EXISTS pg_catalog.uint8_xor_bool(uint8, boolean) ;

        DROP FUNCTION IF EXISTS pg_catalog.bool_xor_uint1(boolean, uint1) ;

        DROP FUNCTION IF EXISTS pg_catalog.bool_xor_uint2(boolean, uint2) ;

        DROP FUNCTION IF EXISTS pg_catalog.bool_xor_uint4(boolean, uint4) ;

        DROP FUNCTION IF EXISTS pg_catalog.bool_xor_uint8(boolean, uint8) ;

        drop CAST IF EXISTS (timestamp(0) with time zone AS uint8) ;
        drop CAST IF EXISTS (money AS uint8) ;

        DROP FUNCTION IF EXISTS pg_catalog.timestamp_uint8(timestamp(0) with time zone) ;

        DROP FUNCTION IF EXISTS pg_catalog.cash_uint(money) ;

        DROP FUNCTION IF EXISTS pg_catalog.int8_xor(uint8,int8) ;

        DROP FUNCTION IF EXISTS pg_catalog.uint8_xor(uint8,int8) ;

        DROP FUNCTION IF EXISTS pg_catalog.text_xor(uint8,text) ;

        DROP FUNCTION IF EXISTS pg_catalog.uint_any_value (uint8, uint8) ;
        
        DROP FUNCTION IF EXISTS pg_catalog.bit_count(numeric) ;
        DROP FUNCTION IF EXISTS pg_catalog.bit_count(text) ;
        DROP FUNCTION IF EXISTS pg_catalog.bit_count_bit(text) ;
        DROP FUNCTION IF EXISTS pg_catalog.bit_count(bit) ;

        drop aggregate if exists pg_catalog.bit_xor(varbit);
        DROP FUNCTION IF EXISTS pg_catalog.varbit_bit_xor_transfn(varbit, varbit) ;
        DROP FUNCTION IF EXISTS pg_catalog.varbit_bit_xor_finalfn(varbit) ;
        drop aggregate if exists pg_catalog.bit_xor(varbit);
        DROP FUNCTION IF EXISTS pg_catalog.connection_id();
        DROP FUNCTION IF EXISTS pg_catalog.set_native_password(text, text);

        DROP FUNCTION IF EXISTS pg_catalog.b_plpgsql_call_handler();
        DROP FUNCTION IF EXISTS pg_catalog.b_plpgsql_inline_handler(internal);
        DROP FUNCTION IF EXISTS pg_catalog.b_plpgsql_validator(oid);
        
        DROP CAST IF EXISTS (anyenum as float8);
        DROP FUNCTION IF EXISTS pg_catalog.enum2float8(anyenum);


        DROP OPERATOR IF EXISTS pg_catalog.=( 
        anyenum, text);

        DROP OPERATOR IF EXISTS pg_catalog.<>( 
        anyenum, text
        );

        DROP OPERATOR IF EXISTS pg_catalog.<( 
          anyenum, text
        );

        DROP OPERATOR IF EXISTS pg_catalog.>( 
          anyenum, text
        );

        DROP OPERATOR IF EXISTS pg_catalog.<=(
         anyenum, text
        );

        DROP OPERATOR IF EXISTS pg_catalog.>=(
         anyenum, text
        );

        DROP OPERATOR IF EXISTS pg_catalog.=( 
         text, anyenum
        );

        DROP OPERATOR IF EXISTS pg_catalog.<>(
         text, anyenum
        );

        DROP OPERATOR IF EXISTS pg_catalog.<( 
          text, anyenum
        );

        DROP OPERATOR IF EXISTS pg_catalog.>( 
         text, anyenum
        );

        DROP OPERATOR IF EXISTS pg_catalog.<=( 
          text, anyenum
        );

        DROP OPERATOR IF EXISTS pg_catalog.>=( 
          text, anyenum
        );


        DROP FUNCTION IF EXISTS pg_catalog.enumtextlt(anyenum, text); 

        DROP FUNCTION IF EXISTS pg_catalog.enumtextle(anyenum, text); 

        DROP FUNCTION IF EXISTS pg_catalog.enumtextne(anyenum, text); 

        DROP FUNCTION IF EXISTS pg_catalog.enumtexteq(anyenum, text); 

        DROP FUNCTION IF EXISTS pg_catalog.enumtextgt(anyenum, text); 

        DROP FUNCTION IF EXISTS pg_catalog.enumtextge(anyenum, text); 

        DROP FUNCTION IF EXISTS pg_catalog.textenumlt(text, anyenum); 

        DROP FUNCTION IF EXISTS pg_catalog.textenumle(text, anyenum); 

        DROP FUNCTION IF EXISTS pg_catalog.textenumne(text, anyenum); 

        DROP FUNCTION IF EXISTS pg_catalog.textenumeq(text, anyenum); 

        DROP FUNCTION IF EXISTS pg_catalog.textenumgt(text, anyenum); 

        DROP FUNCTION IF EXISTS pg_catalog.textenumge(text, anyenum); 
        
        DROP FUNCTION IF EXISTS pg_catalog.from_base64 (text);
        DROP FUNCTION IF EXISTS pg_catalog.from_base64 (bool);
        DROP FUNCTION IF EXISTS pg_catalog.from_base64 (bit);
        DROP FUNCTION IF EXISTS pg_catalog.oct(text);
        DROP FUNCTION IF EXISTS pg_catalog.oct(boolean);
        DROP FUNCTION IF EXISTS pg_catalog.oct(bit);
        DROP FUNCTION IF EXISTS pg_catalog.schema ();
        DROP FUNCTION IF EXISTS pg_catalog.show_status(boolean);

        DROP OPERATOR CLASS IF EXISTS public.int1_ops USING btree ;
        DROP OPERATOR CLASS IF EXISTS public.int1_ops USING cbtree ;
        DROP OPERATOR CLASS IF EXISTS public.int1_ops USING ubtree;
        DROP OPERATOR CLASS IF EXISTS public.int1_ops USING hash;

        DROP OPERATOR IF EXISTS pg_catalog.+(
        int1, int1
        );

        DROP OPERATOR IF EXISTS pg_catalog.-(
        int1, int1
        );

        DROP OPERATOR IF EXISTS pg_catalog.*(
        int1, int1
        );

        DROP OPERATOR IF EXISTS pg_catalog./(
        int1, int1
        );

        DROP OPERATOR IF EXISTS pg_catalog.%(
        int1, int1
        );

        DROP OPERATOR IF EXISTS pg_catalog.&(
        int1, int1
        );

        DROP OPERATOR IF EXISTS pg_catalog.|(
        int1, int1
        );

        DROP OPERATOR IF EXISTS pg_catalog.#(
        int1, int1
        );

        DROP OPERATOR IF EXISTS pg_catalog.>>(
        int1, int4
        );

        DROP OPERATOR IF EXISTS pg_catalog.<<(
        int1, int4
        );

        DROP OPERATOR IF EXISTS pg_catalog.~(
        NONE,int1
        );

        DROP OPERATOR IF EXISTS pg_catalog.+(
        NONE,int1
        );

        DROP OPERATOR IF EXISTS pg_catalog.-(
        NONE,int1
        );

        DROP OPERATOR IF EXISTS pg_catalog.@(
        NONE,int1
        );


        DROP OPERATOR IF EXISTS pg_catalog.=(
        int1, int2
        );

        DROP OPERATOR IF EXISTS pg_catalog.<(
        int1, int2
        );

        DROP OPERATOR IF EXISTS pg_catalog.<=(
        int1, int2
        );

        DROP OPERATOR IF EXISTS pg_catalog.>(
        int1, int2
        );

        DROP OPERATOR IF EXISTS pg_catalog.>=(
        int1, int2
        );

        DROP OPERATOR IF EXISTS pg_catalog.=(
        int1, int4
        );

        DROP OPERATOR IF EXISTS pg_catalog.<(
        int1, int4
        );

        DROP OPERATOR IF EXISTS pg_catalog.<=(
        int1, int4
        );

        DROP OPERATOR IF EXISTS pg_catalog.>(
        int1, int4
        );

        DROP OPERATOR IF EXISTS pg_catalog.>=(
        int1, int4
        );

        DROP OPERATOR IF EXISTS pg_catalog.=(
        int1, int8
        );

        DROP OPERATOR IF EXISTS pg_catalog.<(
        int1, int8
        );

        DROP OPERATOR IF EXISTS pg_catalog.<=(
        int1, int8
        );

        DROP OPERATOR IF EXISTS pg_catalog.>(
        int1, int8
        );

        DROP OPERATOR IF EXISTS pg_catalog.>=(
        int1, int8
        );

        drop aggregate if exists pg_catalog.stddev_pop(int1);

        drop aggregate if exists pg_catalog.listagg(int1,text);

        drop aggregate if exists pg_catalog.listagg(int1);

        drop aggregate if exists pg_catalog.var_pop(int1);

        drop aggregate if exists pg_catalog.var_samp(int1);

        drop aggregate if exists pg_catalog.variance(int1);

        DROP FUNCTION IF EXISTS pg_catalog.int1_accum(numeric[], int1);

        DROP FUNCTION IF EXISTS pg_catalog.int1_list_agg_transfn(internal, int1, text);

        DROP FUNCTION IF EXISTS pg_catalog.int1_list_agg_noarg2_transfn(internal, int1);

        DROP FUNCTION IF EXISTS pg_catalog.int12eq (
        int1,int2
        );

        DROP FUNCTION IF EXISTS pg_catalog.int12lt (
        int1,int2
        );

        DROP FUNCTION IF EXISTS pg_catalog.int12le (
        int1,int2
        );

        DROP FUNCTION IF EXISTS pg_catalog.int12gt (
        int1,int2
        ) ;

        DROP FUNCTION IF EXISTS pg_catalog.int12ge (
        int1,int2
        ) ;

        DROP FUNCTION IF EXISTS pg_catalog.int14eq (
        int1,int4
        ) ;

        DROP FUNCTION IF EXISTS pg_catalog.int14lt (
        int1,int4
        );

        DROP FUNCTION IF EXISTS pg_catalog.int14le (
        int1,int4
        ) ;

        DROP FUNCTION IF EXISTS pg_catalog.int14gt (
        int1,int4
        ) ;

        DROP FUNCTION IF EXISTS pg_catalog.int14ge (
        int1,int4
        ) ;

        DROP FUNCTION IF EXISTS pg_catalog.int18eq (
        int1,int8
        ) ;

        DROP FUNCTION IF EXISTS pg_catalog.int18lt (
        int1,int8
        ) ;

        DROP FUNCTION IF EXISTS pg_catalog.int18le (
        int1,int8
        ) ;

        DROP FUNCTION IF EXISTS pg_catalog.int18gt (
        int1,int8
        ) ;

        DROP FUNCTION IF EXISTS pg_catalog.int18ge (
        int1,int8
        ) ;


        DROP FUNCTION IF EXISTS pg_catalog.int12cmp (
        int1,int2
        ) ;

        DROP FUNCTION IF EXISTS pg_catalog.int14cmp (
        int1,int4
        ) ;

        DROP FUNCTION IF EXISTS pg_catalog.int18cmp (
        int1,int8
        ) ;
        
        DROP FUNCTION IF EXISTS pg_catalog.to_base64 (bytea);
        DROP FUNCTION IF EXISTS pg_catalog.to_base64 (numeric);
        DROP FUNCTION IF EXISTS pg_catalog.to_base64 (text);
        DROP FUNCTION IF EXISTS pg_catalog.to_base64 (bit);

        DROP FUNCTION IF EXISTS pg_catalog.unhex (text) ;
        DROP FUNCTION IF EXISTS pg_catalog.unhex (numeric) ;
        DROP FUNCTION IF EXISTS pg_catalog.unhex (bool) ;
        DROP FUNCTION IF EXISTS pg_catalog.unhex (bytea) ;
        DROP FUNCTION IF EXISTS pg_catalog.unhex ("timestamp") ;
        
        DROP operator IF EXISTS dolphin_catalog.+(int4, int4);
        DROP operator IF EXISTS dolphin_catalog.-(int4, int4);
        DROP operator IF EXISTS dolphin_catalog.*(int4, int4);
        DROP operator IF EXISTS dolphin_catalog./(int4, int4);
        DROP operator IF EXISTS dolphin_catalog.+(int2, int2);
        DROP operator IF EXISTS dolphin_catalog.-(int2, int2);
        DROP operator IF EXISTS dolphin_catalog.*(int2, int2);
        DROP operator IF EXISTS dolphin_catalog./(int2, int2);
        DROP operator IF EXISTS dolphin_catalog.+(int1, int1);
        DROP operator IF EXISTS dolphin_catalog.-(int1, int1);
        DROP operator IF EXISTS dolphin_catalog.*(int1, int1);
        DROP operator IF EXISTS dolphin_catalog./(int1, int1);
        DROP operator IF EXISTS dolphin_catalog.+(int8, int8);
        DROP operator IF EXISTS dolphin_catalog.-(int8, int8);
        DROP operator IF EXISTS dolphin_catalog.*(int8, int8);
        DROP operator IF EXISTS dolphin_catalog./(int8, int8);
        DROP operator IF EXISTS dolphin_catalog.*(uint1, uint1);
        DROP operator IF EXISTS dolphin_catalog.+(int1, uint1);
        DROP operator IF EXISTS dolphin_catalog.-(int1, uint1);
        DROP operator IF EXISTS dolphin_catalog.*(int1, uint1);
        DROP operator IF EXISTS dolphin_catalog./(int1, uint1);
        DROP operator IF EXISTS dolphin_catalog.+(uint1, uint1);
        DROP operator IF EXISTS dolphin_catalog.-(uint1, uint1);
        DROP operator IF EXISTS dolphin_catalog.*(uint1, int1);
        DROP operator IF EXISTS dolphin_catalog./(uint1, int1);
        DROP operator IF EXISTS dolphin_catalog.+(int2, int4);
        DROP operator IF EXISTS dolphin_catalog.-(int2, int4);
        DROP operator IF EXISTS dolphin_catalog.*(int2, int4);
        DROP operator IF EXISTS dolphin_catalog.+(uint1, int1);
        DROP operator IF EXISTS dolphin_catalog.-(uint1, int1);
        DROP operator IF EXISTS dolphin_catalog./(int2, int4);
        DROP operator IF EXISTS dolphin_catalog.+(int2, int8);
        DROP operator IF EXISTS dolphin_catalog.-(int2, int8);
        DROP operator IF EXISTS dolphin_catalog.*(int2, int8);
        DROP operator IF EXISTS dolphin_catalog./(int2, int8);
        DROP operator IF EXISTS dolphin_catalog.+(int2, uint2);
        DROP operator IF EXISTS dolphin_catalog.-(int2, uint2);
        DROP operator IF EXISTS dolphin_catalog.*(int2, uint2);
        DROP operator IF EXISTS dolphin_catalog./(int2, uint2);
        DROP operator IF EXISTS dolphin_catalog.+(int4, int2);
        DROP operator IF EXISTS dolphin_catalog.-(int4, int2);
        DROP operator IF EXISTS dolphin_catalog.*(int4, int2);
        DROP operator IF EXISTS dolphin_catalog./(int4, int2);
        DROP operator IF EXISTS dolphin_catalog.+(int4, int8);
        DROP operator IF EXISTS dolphin_catalog.-(int4, int8);
        DROP operator IF EXISTS dolphin_catalog.*(int4, int8);
        DROP operator IF EXISTS dolphin_catalog./(int4, int8);
        DROP operator IF EXISTS dolphin_catalog.+(int4, uint4);
        DROP operator IF EXISTS dolphin_catalog.-(int4, uint4);
        DROP operator IF EXISTS dolphin_catalog.*(int4, uint4);
        DROP operator IF EXISTS dolphin_catalog./(int4, uint4);
        DROP operator IF EXISTS dolphin_catalog.+(int8, int2);
        DROP operator IF EXISTS dolphin_catalog.-(int8, int2);
        DROP operator IF EXISTS dolphin_catalog.*(int8, int2);
        DROP operator IF EXISTS dolphin_catalog./(int8, int2);
        DROP operator IF EXISTS dolphin_catalog.-(int8, int4);
        DROP operator IF EXISTS dolphin_catalog.+(int8, int4);
        DROP operator IF EXISTS dolphin_catalog.*(int8, int4);
        DROP operator IF EXISTS dolphin_catalog./(int8, int4);
        DROP operator IF EXISTS dolphin_catalog.+(uint2, int2);
        DROP operator IF EXISTS dolphin_catalog.-(uint2, int2);
        DROP operator IF EXISTS dolphin_catalog.*(uint2, int2);
        DROP operator IF EXISTS dolphin_catalog./(uint2, int2);
        DROP operator IF EXISTS dolphin_catalog.+(uint2, uint2);
        DROP operator IF EXISTS dolphin_catalog.-(uint2, uint2);
        DROP operator IF EXISTS dolphin_catalog.*(uint2, uint2);
        DROP operator IF EXISTS dolphin_catalog./(uint2, uint2);
        DROP operator IF EXISTS dolphin_catalog.+(uint4, uint4);
        DROP operator IF EXISTS dolphin_catalog.-(uint4, uint4);
        DROP operator IF EXISTS dolphin_catalog.*(uint4, uint4);
        DROP operator IF EXISTS dolphin_catalog./(uint4, uint4);
        DROP operator IF EXISTS dolphin_catalog.+(uint4, int4);
        DROP operator IF EXISTS dolphin_catalog.-(uint4, int4);
        DROP operator IF EXISTS dolphin_catalog.*(uint4, int4);
        DROP operator IF EXISTS dolphin_catalog./(uint4, int4);
        DROP operator IF EXISTS dolphin_catalog.+(float4, float4);
        DROP operator IF EXISTS dolphin_catalog.-(float4, float4);
        DROP operator IF EXISTS dolphin_catalog.*(float4, float4);
        DROP operator IF EXISTS dolphin_catalog./(float4, float4);
        DROP operator IF EXISTS dolphin_catalog.+(float4, float8);
        DROP operator IF EXISTS dolphin_catalog.-(float4, float8);
        DROP operator IF EXISTS dolphin_catalog.*(float4, float8);
        DROP operator IF EXISTS dolphin_catalog./(float4, float8);
        DROP operator IF EXISTS dolphin_catalog.+(float8, float4);
        DROP operator IF EXISTS dolphin_catalog.-(float8, float4);
        DROP operator IF EXISTS dolphin_catalog.*(float8, float4);
        DROP operator IF EXISTS dolphin_catalog./(float8, float4);
        DROP operator IF EXISTS dolphin_catalog.+(uint8, int8);
        DROP operator IF EXISTS dolphin_catalog.-(uint8, int8);
        DROP operator IF EXISTS dolphin_catalog.*(uint8, int8);
        DROP operator IF EXISTS dolphin_catalog./(uint8, int8);
        DROP operator IF EXISTS dolphin_catalog.+(int8, uint8);
        DROP operator IF EXISTS dolphin_catalog.-(int8, uint8);
        DROP operator IF EXISTS dolphin_catalog.*(int8, uint8);
        DROP operator IF EXISTS dolphin_catalog./(int8, uint8);
        DROP operator IF EXISTS dolphin_catalog.+(float8, float8);
        DROP operator IF EXISTS dolphin_catalog.-(float8, float8);
        DROP operator IF EXISTS dolphin_catalog.*(float8, float8);
        DROP operator IF EXISTS dolphin_catalog./(float8, float8);
        DROP operator IF EXISTS dolphin_catalog./(uint1, uint1);

        /* uint8 */
        DROP operator IF EXISTS dolphin_catalog.+(uint8, uint8);
        DROP operator IF EXISTS dolphin_catalog.-(uint8, uint8);
        DROP operator IF EXISTS dolphin_catalog.*(uint8, uint8);
        DROP operator IF EXISTS dolphin_catalog./(uint8, uint8);

        /* numeric */
        DROP operator IF EXISTS dolphin_catalog.+(numeric, numeric);
        DROP operator IF EXISTS dolphin_catalog.-(numeric, numeric);
        DROP operator IF EXISTS dolphin_catalog.*(numeric, numeric);
        DROP operator IF EXISTS dolphin_catalog./(numeric, numeric);

        /* int4 */
        DROP FUNCTION IF EXISTS dolphin_catalog.dolphin_int4pl (
        int4,
        int4
        );

        DROP FUNCTION IF EXISTS dolphin_catalog.dolphin_int4mi (
        int4,
        int4
        ) ;

        DROP FUNCTION IF EXISTS dolphin_catalog.dolphin_int4mul (
        int4,
        int4
        );

        DROP FUNCTION IF EXISTS dolphin_catalog.dolphin_int4div (
        int4,
        int4
        );

        /* int2 */
        DROP FUNCTION IF EXISTS dolphin_catalog.dolphin_int2pl (
        int2,
        int2
        );

        DROP FUNCTION IF EXISTS dolphin_catalog.dolphin_int2mi (
        int2,
        int2
        );

        DROP FUNCTION IF EXISTS dolphin_catalog.dolphin_int2mul (
        int2,
        int2
        ) ;

        DROP FUNCTION IF EXISTS dolphin_catalog.dolphin_int2div (
        int2,
        int2
        ) ;

        /* int1 */
        DROP FUNCTION IF EXISTS dolphin_catalog.dolphin_int1pl (
        int1,
        int1
        );

        DROP FUNCTION IF EXISTS dolphin_catalog.dolphin_int1mi (
        int1,
        int1
        ) ;

        DROP FUNCTION IF EXISTS dolphin_catalog.dolphin_int1mul (
        int1,
        int1
        ) ;

        DROP FUNCTION IF EXISTS dolphin_catalog.dolphin_int1div (
        int1,
        int1
        ) ;

        /* int8 */
        DROP FUNCTION IF EXISTS dolphin_catalog.dolphin_int8pl (
        int8,
        int8
        ) ;

        DROP FUNCTION IF EXISTS dolphin_catalog.dolphin_int8mi (
        int8,
        int8
        ) ;

        DROP FUNCTION IF EXISTS dolphin_catalog.dolphin_int8mul (
        int8,
        int8
        );

        DROP FUNCTION IF EXISTS dolphin_catalog.dolphin_int8div (
        int8,
        int8
        ) ;

        /* uint1 */
        DROP FUNCTION IF EXISTS dolphin_catalog.dolphin_uint1pl (
        uint1,
        uint1
        ) ;


        DROP FUNCTION IF EXISTS dolphin_catalog.dolphin_uint1mi (
        uint1,
        uint1
        ) ;

        DROP FUNCTION IF EXISTS dolphin_catalog.dolphin_uint1mul (
        uint1,
        uint1
        );

        DROP FUNCTION IF EXISTS dolphin_catalog.dolphin_uint1div (
        uint1,
        uint1
        );

        /* int1_uint1 */
        DROP FUNCTION IF EXISTS dolphin_catalog.dolphin_int1_pl_uint1 (
        int1,
        uint1
        );

        DROP FUNCTION IF EXISTS dolphin_catalog.dolphin_int1_mi_uint1 (
        int1,
        uint1
        ) ;

        DROP FUNCTION IF EXISTS dolphin_catalog.dolphin_int1_mul_uint1 (
        int1,
        uint1
        );

        DROP FUNCTION IF EXISTS dolphin_catalog.dolphin_int1_div_uint1 (
        int1,
        uint1
        );

        /* uint1_int1 */
        DROP FUNCTION IF EXISTS dolphin_catalog.dolphin_uint1_pl_int1 (
        uint1,
        int1
        );


        DROP FUNCTION IF EXISTS dolphin_catalog.dolphin_uint1_mi_int1 (
        uint1,
        int1
        ) ;

        DROP FUNCTION IF EXISTS dolphin_catalog.dolphin_uint1_mul_int1 (
        uint1,
        int1
        );


        DROP FUNCTION IF EXISTS dolphin_catalog.dolphin_uint1_div_int1 (
        uint1,
        int1
        );

        /* int2_int4 */
        DROP FUNCTION IF EXISTS dolphin_catalog.dolphin_int24pl (
        int2,
        int4
        ) ;

        DROP FUNCTION IF EXISTS dolphin_catalog.dolphin_int24mi (
        int2,
        int4
        ) ;

        DROP FUNCTION IF EXISTS dolphin_catalog.dolphin_int24mul (
        int2,
        int4
        ) ;

        DROP FUNCTION IF EXISTS dolphin_catalog.dolphin_int24div (
        int2,
        int4
        );

        /* int2_int8 */
        DROP FUNCTION IF EXISTS dolphin_catalog.dolphin_int28pl (
        int2,
        int8
        );


        DROP FUNCTION IF EXISTS dolphin_catalog.dolphin_int28mi (
        int2,
        int8
        );

        DROP FUNCTION IF EXISTS dolphin_catalog.dolphin_int28mul (
        int2,
        int8
        );

        DROP FUNCTION IF EXISTS dolphin_catalog.dolphin_int28div (
        int2,
        int8
        );

        /* int2_uint2 */
        DROP FUNCTION IF EXISTS dolphin_catalog.dolphin_int2_pl_uint2 (
        int2,
        uint2
        ) ;

        DROP FUNCTION IF EXISTS dolphin_catalog.dolphin_int2_mi_uint2 (
        int2,
        uint2
        ) ;

        DROP FUNCTION IF EXISTS dolphin_catalog.dolphin_int2_mul_uint2 (
        int2,
        uint2
        );


        DROP FUNCTION IF EXISTS dolphin_catalog.dolphin_int2_div_uint2 (
        int2,
        uint2
        ) ;

        /* int4_int2 */
        DROP FUNCTION IF EXISTS dolphin_catalog.dolphin_int42pl (
        int4,
        int2
        );

        DROP FUNCTION IF EXISTS dolphin_catalog.dolphin_int42mi (
        int4,
        int2
        );

        DROP FUNCTION IF EXISTS dolphin_catalog.dolphin_int42mul (
        int4,
        int2
        );

        DROP FUNCTION IF EXISTS dolphin_catalog.dolphin_int42div (
        int4,
        int2
        );

        /* int4_int8 */
        DROP FUNCTION IF EXISTS dolphin_catalog.dolphin_int48pl (
        int4,
        int8
        );


        DROP FUNCTION IF EXISTS dolphin_catalog.dolphin_int48mi (
        int4,
        int8
        );

        DROP FUNCTION IF EXISTS dolphin_catalog.dolphin_int48mul (
        int4,
        int8
        );

        DROP FUNCTION IF EXISTS dolphin_catalog.dolphin_int48div (
        int4,
        int8
        );

        /* int4_uint4 */
        DROP FUNCTION IF EXISTS dolphin_catalog.dolphin_int4_pl_uint4 (
        int4,
        uint4
        ) ;

        DROP FUNCTION IF EXISTS dolphin_catalog.dolphin_int4_mi_uint4 (
        int4,
        uint4
        );

        DROP FUNCTION IF EXISTS dolphin_catalog.dolphin_int4_mul_uint4 (
        int4,
        uint4
        ) ;


        DROP FUNCTION IF EXISTS dolphin_catalog.dolphin_int4_div_uint4 (
        int4,
        uint4
        );

        /* int8_int2 */
        DROP FUNCTION IF EXISTS dolphin_catalog.dolphin_int82pl (
        int8,
        int2
        );

        DROP FUNCTION IF EXISTS dolphin_catalog.dolphin_int82mi (
        int8,
        int2
        );

        DROP FUNCTION IF EXISTS dolphin_catalog.dolphin_int82mul (
        int8,
        int2
        ) ;

        DROP FUNCTION IF EXISTS dolphin_catalog.dolphin_int82div (
        int8,
        int2
        ) ;

        /* int8_int4 */
        DROP FUNCTION IF EXISTS dolphin_catalog.dolphin_int84pl (
        int8,
        int4
        );

        DROP FUNCTION IF EXISTS dolphin_catalog.dolphin_int84mi (
        int8,
        int4
        ) ;

        DROP FUNCTION IF EXISTS dolphin_catalog.dolphin_int84mul (
        int8,
        int4
        );

        DROP FUNCTION IF EXISTS dolphin_catalog.dolphin_int84div (
        int8,
        int4
        ) ;

        /* uint2_int2 */
        DROP FUNCTION IF EXISTS dolphin_catalog.dolphin_uint2_pl_int2 (
        uint2,
        int2
        ) ;

        DROP FUNCTION IF EXISTS dolphin_catalog.dolphin_uint2_mi_int2 (
        uint2,
        int2
        ) ;

        DROP FUNCTION IF EXISTS dolphin_catalog.dolphin_uint2_mul_int2 (
        uint2,
        int2
        ) ;

        DROP FUNCTION IF EXISTS dolphin_catalog.dolphin_uint2_div_int2 (
        uint2,
        int2
        ) ;

        /* uint2 */
        DROP FUNCTION IF EXISTS dolphin_catalog.dolphin_uint2pl (
        uint2,
        uint2
        );

        DROP FUNCTION IF EXISTS dolphin_catalog.dolphin_uint2mi (
        uint2,
        uint2
        );

        DROP FUNCTION IF EXISTS dolphin_catalog.dolphin_uint2mul (
        uint2,
        uint2
        ) ;

        DROP FUNCTION IF EXISTS dolphin_catalog.dolphin_uint2div (
        uint2,
        uint2
        ) ;

        /* uint4 */
        DROP FUNCTION IF EXISTS dolphin_catalog.dolphin_uint4pl (
        uint4,
        uint4
        );

        DROP FUNCTION IF EXISTS dolphin_catalog.dolphin_uint4mi (
        uint4,
        uint4
        );

        DROP FUNCTION IF EXISTS dolphin_catalog.dolphin_uint4mul (
        uint4,
        uint4
        );

        DROP FUNCTION IF EXISTS dolphin_catalog.dolphin_uint4div (
        uint4,
        uint4
        ) ;

        /* uint4_int4 */
        DROP FUNCTION IF EXISTS dolphin_catalog.dolphin_uint4_pl_int4 (
        uint4,
        int4
        ) ;

        DROP FUNCTION IF EXISTS dolphin_catalog.dolphin_uint4_mi_int4 (
        uint4,
        int4
        ) ;

        DROP FUNCTION IF EXISTS dolphin_catalog.dolphin_uint4_mul_int4 (
        uint4,
        int4
        ) ;

        DROP FUNCTION IF EXISTS dolphin_catalog.dolphin_uint4_div_int4 (
        uint4,
        int4
        ) ;

        /* float4 */
        DROP FUNCTION IF EXISTS dolphin_catalog.dolphin_float4pl (
        float4,
        float4
        );

        DROP FUNCTION IF EXISTS dolphin_catalog.dolphin_float4mi (
        float4,
        float4
        ) ;

        DROP FUNCTION IF EXISTS dolphin_catalog.dolphin_float4mul (
        float4,
        float4
        ) ;

        DROP FUNCTION IF EXISTS dolphin_catalog.dolphin_float4div (
        float4,
        float4
        ) ;

        /* float4_float8 */
        DROP FUNCTION IF EXISTS dolphin_catalog.dolphin_float48pl (
        float4,
        float8
        ) ;

        DROP FUNCTION IF EXISTS dolphin_catalog.dolphin_float48mi (
        float4,
        float8
        ) ;

        DROP FUNCTION IF EXISTS dolphin_catalog.dolphin_float48mul (
        float4,
        float8
        ) ;

        DROP FUNCTION IF EXISTS dolphin_catalog.dolphin_float48div (
        float4,
        float8
        ) ;

        /* float8_float4 */
        DROP FUNCTION IF EXISTS dolphin_catalog.dolphin_float84pl (
        float8,
        float4
        );

        DROP FUNCTION IF EXISTS dolphin_catalog.dolphin_float84mi (
        float8,
        float4
        ) ;

        DROP FUNCTION IF EXISTS dolphin_catalog.dolphin_float84mul (
        float8,
        float4
        ) ;

        DROP FUNCTION IF EXISTS dolphin_catalog.dolphin_float84div (
        float8,
        float4
        ) ;

        /* uint8_int8 */
        DROP FUNCTION IF EXISTS dolphin_catalog.dolphin_uint8_pl_int8 (
        uint8,
        int8
        ) ;

        DROP FUNCTION IF EXISTS dolphin_catalog.dolphin_uint8_mi_int8 (
        uint8,
        int8
        ) ;

        DROP FUNCTION IF EXISTS dolphin_catalog.dolphin_uint8_mul_int8 (
        uint8,
        int8
        ) ;

        DROP FUNCTION IF EXISTS dolphin_catalog.dolphin_uint8_div_int8 (
        uint8,
        int8
        ) ;

        /* int8_uint8 */
        DROP FUNCTION IF EXISTS dolphin_catalog.dolphin_int8_pl_uint8 (
        int8,
        uint8
        ) ;

        DROP FUNCTION IF EXISTS dolphin_catalog.dolphin_int8_mi_uint8 (
        int8,
        uint8
        ) ;

        DROP FUNCTION IF EXISTS dolphin_catalog.dolphin_int8_mul_uint8 (
        int8,
        uint8
        );

        DROP FUNCTION IF EXISTS dolphin_catalog.dolphin_int8_div_uint8 (
        int8,
        uint8
        ) ;

        /* float8 */

        DROP FUNCTION IF EXISTS dolphin_catalog.dolphin_uint8div (
        uint8,
        uint8
        );

        DROP SCHEMA IF EXISTS dolphin_catalog;
        DROP FUNCTION IF EXISTS pg_catalog.uuid_short();

    end if;
END
$for_og_310$;

--rollback binary/varbinary -> text implicit
DROP OPERATOR IF EXISTS pg_catalog.>=(binary, time) CASCADE;
DROP OPERATOR IF EXISTS pg_catalog.>=(binary, numeric) CASCADE;
DROP OPERATOR IF EXISTS pg_catalog.>=(binary, uint1) CASCADE;
DROP OPERATOR IF EXISTS pg_catalog.>=(binary, uint2) CASCADE;
DROP OPERATOR IF EXISTS pg_catalog.>=(binary, uint4) CASCADE;
DROP OPERATOR IF EXISTS pg_catalog.>=(binary, uint8) CASCADE;

DROP FUNCTION IF EXISTS pg_catalog.binary_time_ge(binary, time) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.binary_numeric_ge(binary, numeric) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.binary_uint1_ge(binary, uint1) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.binary_uint2_ge(binary, uint2) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.binary_uint4_ge(binary, uint4) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.binary_uint8_ge(binary, uint8) CASCADE;

DROP OPERATOR IF EXISTS pg_catalog.>=(time, binary) CASCADE;
DROP OPERATOR IF EXISTS pg_catalog.>=(numeric, binary) CASCADE;
DROP OPERATOR IF EXISTS pg_catalog.>=(uint1, binary) CASCADE;
DROP OPERATOR IF EXISTS pg_catalog.>=(uint2, binary) CASCADE;
DROP OPERATOR IF EXISTS pg_catalog.>=(uint4, binary) CASCADE;
DROP OPERATOR IF EXISTS pg_catalog.>=(uint8, binary) CASCADE;

DROP FUNCTION IF EXISTS pg_catalog.time_binary_ge(time, binary) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.numeric_binary_ge(numeric, binary) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.uint1_binary_ge(uint1, binary) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.uint2_binary_ge(uint2, binary) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.uint4_binary_ge(uint4, binary) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.uint8_binary_ge(uint8, binary) CASCADE;

DROP OPERATOR IF EXISTS pg_catalog.<=(binary, time) CASCADE;
DROP OPERATOR IF EXISTS pg_catalog.<=(binary, numeric) CASCADE;
DROP OPERATOR IF EXISTS pg_catalog.<=(binary, uint1) CASCADE;
DROP OPERATOR IF EXISTS pg_catalog.<=(binary, uint2) CASCADE;
DROP OPERATOR IF EXISTS pg_catalog.<=(binary, uint4) CASCADE;
DROP OPERATOR IF EXISTS pg_catalog.<=(binary, uint8) CASCADE;

DROP FUNCTION IF EXISTS pg_catalog.binary_time_le(binary, time) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.binary_numeric_le(binary, numeric) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.binary_uint1_le(binary, uint1) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.binary_uint2_le(binary, uint2) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.binary_uint4_le(binary, uint4) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.binary_uint8_le(binary, uint8) CASCADE;

DROP OPERATOR IF EXISTS pg_catalog.<=(time, binary) CASCADE;
DROP OPERATOR IF EXISTS pg_catalog.<=(numeric, binary) CASCADE;
DROP OPERATOR IF EXISTS pg_catalog.<=(uint1, binary) CASCADE;
DROP OPERATOR IF EXISTS pg_catalog.<=(uint2, binary) CASCADE;
DROP OPERATOR IF EXISTS pg_catalog.<=(uint4, binary) CASCADE;
DROP OPERATOR IF EXISTS pg_catalog.<=(uint8, binary) CASCADE;

DROP FUNCTION IF EXISTS pg_catalog.time_binary_le(time, binary) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.numeric_binary_le(numeric, binary) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.uint1_binary_le(uint1, binary) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.uint2_binary_le(uint2, binary) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.uint4_binary_le(uint4, binary) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.uint8_binary_le(uint8, binary) CASCADE;

DROP OPERATOR IF EXISTS pg_catalog.>(binary, time) CASCADE;
DROP OPERATOR IF EXISTS pg_catalog.>(binary, numeric) CASCADE;
DROP OPERATOR IF EXISTS pg_catalog.>(binary, uint1) CASCADE;
DROP OPERATOR IF EXISTS pg_catalog.>(binary, uint2) CASCADE;
DROP OPERATOR IF EXISTS pg_catalog.>(binary, uint4) CASCADE;
DROP OPERATOR IF EXISTS pg_catalog.>(binary, uint8) CASCADE;

DROP FUNCTION IF EXISTS pg_catalog.binary_time_gt(binary, time) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.binary_numeric_gt(binary, numeric) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.binary_uint1_gt(binary, uint1) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.binary_uint2_gt(binary, uint2) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.binary_uint4_gt(binary, uint4) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.binary_uint8_gt(binary, uint8) CASCADE;

DROP OPERATOR IF EXISTS pg_catalog.>(time, binary) CASCADE;
DROP OPERATOR IF EXISTS pg_catalog.>(numeric, binary) CASCADE;
DROP OPERATOR IF EXISTS pg_catalog.>(uint1, binary) CASCADE;
DROP OPERATOR IF EXISTS pg_catalog.>(uint2, binary) CASCADE;
DROP OPERATOR IF EXISTS pg_catalog.>(uint4, binary) CASCADE;
DROP OPERATOR IF EXISTS pg_catalog.>(uint8, binary) CASCADE;

DROP FUNCTION IF EXISTS pg_catalog.time_binary_gt(time, binary) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.numeric_binary_gt(numeric, binary) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.uint1_binary_gt(uint1, binary) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.uint2_binary_gt(uint2, binary) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.uint4_binary_gt(uint4, binary) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.uint8_binary_gt(uint8, binary) CASCADE;

DROP OPERATOR IF EXISTS pg_catalog.<(binary, time) CASCADE;
DROP OPERATOR IF EXISTS pg_catalog.<(binary, numeric) CASCADE;
DROP OPERATOR IF EXISTS pg_catalog.<(binary, uint1) CASCADE;
DROP OPERATOR IF EXISTS pg_catalog.<(binary, uint2) CASCADE;
DROP OPERATOR IF EXISTS pg_catalog.<(binary, uint4) CASCADE;
DROP OPERATOR IF EXISTS pg_catalog.<(binary, uint8) CASCADE;

DROP FUNCTION IF EXISTS pg_catalog.binary_time_lt(binary, time) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.binary_numeric_lt(binary, numeric) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.binary_uint1_lt(binary, uint1) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.binary_uint2_lt(binary, uint2) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.binary_uint4_lt(binary, uint4) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.binary_uint8_lt(binary, uint8) CASCADE;

DROP OPERATOR IF EXISTS pg_catalog.<(time, binary) CASCADE;
DROP OPERATOR IF EXISTS pg_catalog.<(numeric, binary) CASCADE;
DROP OPERATOR IF EXISTS pg_catalog.<(uint1, binary) CASCADE;
DROP OPERATOR IF EXISTS pg_catalog.<(uint2, binary) CASCADE;
DROP OPERATOR IF EXISTS pg_catalog.<(uint4, binary) CASCADE;
DROP OPERATOR IF EXISTS pg_catalog.<(uint8, binary) CASCADE;

DROP FUNCTION IF EXISTS pg_catalog.time_binary_lt(time, binary) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.numeric_binary_lt(numeric, binary) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.uint1_binary_lt(uint1, binary) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.uint2_binary_lt(uint2, binary) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.uint4_binary_lt(uint4, binary) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.uint8_binary_lt(uint8, binary) CASCADE;

DROP OPERATOR IF EXISTS pg_catalog.!~~(binary, binary) CASCADE;
DROP OPERATOR IF EXISTS pg_catalog.!~~*(binary, binary) CASCADE;
DROP OPERATOR IF EXISTS pg_catalog.~~(binary, binary) CASCADE;
DROP OPERATOR IF EXISTS pg_catalog.~~*(binary, binary) CASCADE;
DROP OPERATOR IF EXISTS pg_catalog.!~~(name, binary) CASCADE;
DROP OPERATOR IF EXISTS pg_catalog.!~~*(name, binary) CASCADE;
DROP OPERATOR IF EXISTS pg_catalog.~~(name, binary) CASCADE;
DROP OPERATOR IF EXISTS pg_catalog.~~*(name, binary) CASCADE;
DROP OPERATOR IF EXISTS pg_catalog.!~~(char, binary) CASCADE;
DROP OPERATOR IF EXISTS pg_catalog.!~~*(char, binary) CASCADE;
DROP OPERATOR IF EXISTS pg_catalog.~~(char, binary) CASCADE;
DROP OPERATOR IF EXISTS pg_catalog.~~*(char, binary) CASCADE;

DROP FUNCTION IF EXISTS pg_catalog.bpcharbinarylike(char, binary) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.bpcharbinarynlike(char, binary) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.namebinarylike(name, binary) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.namebinarynlike(name, binary) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.binarylike(binary, binary) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.binarynlike(binary, binary) CASCADE;

DROP FUNCTION IF EXISTS pg_catalog.varlena_cast_ui8(anyelement) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.varlena_cast_ui4(anyelement) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.varlena_cast_ui2(anyelement) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.varlena_cast_ui1(anyelement) CASCADE;

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


DROP CAST IF EXISTS ("binary" AS uint1) CASCADE;
DROP CAST IF EXISTS ("varbinary" AS uint1) CASCADE;
DROP CAST IF EXISTS (blob AS uint1) CASCADE;
DROP CAST IF EXISTS (tinyblob AS uint1) CASCADE;
DROP CAST IF EXISTS (mediumblob AS uint1) CASCADE;
DROP CAST IF EXISTS (longblob AS uint1) CASCADE;
DROP CAST IF EXISTS (json AS uint1) CASCADE;
DROP CAST IF EXISTS ("binary" AS uint2) CASCADE;
DROP CAST IF EXISTS ("varbinary" AS uint2) CASCADE;
DROP CAST IF EXISTS (blob AS uint2) CASCADE;
DROP CAST IF EXISTS (tinyblob AS uint2) CASCADE;
DROP CAST IF EXISTS (mediumblob AS uint2) CASCADE;
DROP CAST IF EXISTS (longblob AS uint2) CASCADE;
DROP CAST IF EXISTS (json AS uint2) CASCADE;
DROP CAST IF EXISTS ("binary" AS uint4) CASCADE;
DROP CAST IF EXISTS ("varbinary" AS uint4) CASCADE;
DROP CAST IF EXISTS (blob AS uint4) CASCADE;
DROP CAST IF EXISTS (tinyblob AS uint4) CASCADE;
DROP CAST IF EXISTS (mediumblob AS uint4) CASCADE;
DROP CAST IF EXISTS (longblob AS uint4) CASCADE;
DROP CAST IF EXISTS (json AS uint4) CASCADE;
DROP CAST IF EXISTS ("binary" AS uint8) CASCADE;
DROP CAST IF EXISTS ("varbinary" AS uint8) CASCADE;
DROP CAST IF EXISTS (blob AS uint8) CASCADE;
DROP CAST IF EXISTS (tinyblob AS uint8) CASCADE;
DROP CAST IF EXISTS (mediumblob AS uint8) CASCADE;
DROP CAST IF EXISTS (longblob AS uint8) CASCADE;
DROP CAST IF EXISTS (json AS uint8) CASCADE;

DROP FUNCTION IF EXISTS pg_catalog.varlena2ui1(anyelement) cascade;
DROP FUNCTION IF EXISTS pg_catalog.varlena2ui2(anyelement) cascade;
DROP FUNCTION IF EXISTS pg_catalog.varlena2ui4(anyelement) cascade;
DROP FUNCTION IF EXISTS pg_catalog.varlena2ui8(anyelement) cascade;

DROP FUNCTION IF EXISTS pg_catalog.varchar_cast_ui8(char) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.varchar_cast_ui4(char) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.varchar_cast_ui2(char) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.varchar_cast_ui1(char) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.char_cast_ui8(char) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.char_cast_ui4(char) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.char_cast_ui2(char) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.char_cast_ui1(char) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.time_cast_ui8(time) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.time_cast_ui4(time) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.time_cast_ui2(time) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.time_cast_ui1(time) CASCADE;
DROP OPERATOR IF EXISTS pg_catalog.-(timestamptz, int4) CASCADE;
DROP OPERATOR IF EXISTS pg_catalog.+(timestamptz, int4) CASCADE;
DROP OPERATOR IF EXISTS pg_catalog.-(timestamp without time zone, int4) CASCADE;
DROP OPERATOR IF EXISTS pg_catalog.+(timestamp without time zone, int4) CASCADE;

DROP FUNCTION IF EXISTS pg_catalog.timestamp_add (text, timestamptz, "any") CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.time_format(timestamp without time zone, text) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.timestamp_mi_int4(timestamptz, int4) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.timestamp_pl_int4(timestamptz, int4) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.datetime_mi_int4(timestamp without time zone, int4) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.datetime_pl_int4(timestamp without time zone, int4) CASCADE;

DROP FUNCTION IF EXISTS pg_catalog.atan2 (boolean, boolean);
DROP FUNCTION IF EXISTS pg_catalog.atan2 (boolean, float8);
DROP FUNCTION IF EXISTS pg_catalog.atan2 (float8, boolean);
DROP FUNCTION IF EXISTS pg_catalog.atan (float8, float8);
DROP FUNCTION IF EXISTS pg_catalog.atan (boolean, boolean);
DROP FUNCTION IF EXISTS pg_catalog.atan (boolean, float8);
DROP FUNCTION IF EXISTS pg_catalog.atan (float8, boolean);
DROP FUNCTION IF EXISTS pg_catalog.atan (boolean);

DROP FUNCTION IF EXISTS pg_catalog.dolphin_invoke();
CREATE FUNCTION pg_catalog.dolphin_invoke()
    RETURNS VOID AS '$libdir/dolphin','dolphin_invoke' LANGUAGE C STRICT;

DROP CAST IF EXISTS (bool AS bit) CASCADE;
DROP CAST IF EXISTS (bool AS float4) CASCADE;
DROP CAST IF EXISTS (bool AS float8) CASCADE;

DROP FUNCTION IF EXISTS pg_catalog.booltobit(bool, int4) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.booltofloat4(bool) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.booltofloat8(bool) CASCADE;

--rollback castcontext
CREATE FUNCTION pg_catalog.rollback_castcontext(varchar, varchar, varchar) RETURNS varchar AS
$$
DECLARE
    s_type ALIAS FOR $1;
    t_type ALIAS FOR $2;
    context ALIAS FOR $3;
BEGIN
    update pg_cast set castcontext=context, castowner=10 where castsource=(select oid from pg_type where typname=s_type) and casttarget=(select oid from pg_type where typname=t_type);
    RETURN 'SUCCESS';
END;
$$
LANGUAGE plpgsql;

DO
$$
BEGIN
    PERFORM pg_catalog.rollback_castcontext('int8', 'int1', 'a');
    PERFORM pg_catalog.rollback_castcontext('int8', 'int2', 'a');
    PERFORM pg_catalog.rollback_castcontext('int8', 'int4', 'a');

    PERFORM pg_catalog.rollback_castcontext('date', 'int4', 'e');
    PERFORM pg_catalog.rollback_castcontext('date', 'int8', 'e');
    PERFORM pg_catalog.rollback_castcontext('date', 'uint4', 'e');
    PERFORM pg_catalog.rollback_castcontext('date', 'uint8', 'e');
    PERFORM pg_catalog.rollback_castcontext('date', 'float4', 'e');
    PERFORM pg_catalog.rollback_castcontext('date', 'float8', 'e');
    PERFORM pg_catalog.rollback_castcontext('date', 'numeric', 'e');
    PERFORM pg_catalog.rollback_castcontext('timestamp', 'int8', 'e');
    PERFORM pg_catalog.rollback_castcontext('timestamp', 'uint8', 'e');
    PERFORM pg_catalog.rollback_castcontext('timestamp', 'float4', 'e');
    PERFORM pg_catalog.rollback_castcontext('timestamp', 'numeric', 'e');
    PERFORM pg_catalog.rollback_castcontext('timestamptz', 'int8', 'e');
    PERFORM pg_catalog.rollback_castcontext('timestamptz', 'uint8', 'e');
    PERFORM pg_catalog.rollback_castcontext('timestamptz', 'float4', 'e');
    PERFORM pg_catalog.rollback_castcontext('timestamptz', 'float8', 'e');
    PERFORM pg_catalog.rollback_castcontext('timestamptz', 'numeric', 'e');
    PERFORM pg_catalog.rollback_castcontext('time', 'int4', 'e');
    PERFORM pg_catalog.rollback_castcontext('time', 'int8', 'e');
    PERFORM pg_catalog.rollback_castcontext('time', 'uint4', 'e');
    PERFORM pg_catalog.rollback_castcontext('time', 'uint8', 'e');
    PERFORM pg_catalog.rollback_castcontext('time', 'float4', 'e');
    PERFORM pg_catalog.rollback_castcontext('time', 'numeric', 'e');
END
$$
LANGUAGE plpgsql;

DROP FUNCTION IF EXISTS pg_catalog.rollback_castcontext(varchar, varchar, varchar) CASCADE;

--dolphin_binary_function.sql rollback
DROP OPERATOR IF EXISTS pg_catalog.^(blob, date) CASCADE;
DROP OPERATOR IF EXISTS pg_catalog.^(date, blob) CASCADE;
DROP OPERATOR IF EXISTS pg_catalog.^(blob, timestamptz) CASCADE;
DROP OPERATOR IF EXISTS pg_catalog.^(timestamptz, blob) CASCADE;
DROP OPERATOR IF EXISTS pg_catalog.^(time, bit) CASCADE;
DROP OPERATOR IF EXISTS pg_catalog.^(bit, time) CASCADE;

DROP FUNCTION IF EXISTS pg_catalog.blob_date_xor(
    blob,
    date
) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.date_blob_xor(
    date,
    blob
) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.blob_timestamptz_xor(
    blob,
    timestamptz
) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.timestamptz_blob_xor(
    timestamptz,
    blob
) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.time_xor_bit(
    time,
    bit
) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.bit_xor_time(
    bit,
    time
) CASCADE;


DROP CAST IF EXISTS (bit AS binary) CASCADE;
DROP CAST IF EXISTS (bit AS varbinary) CASCADE;
DROP CAST IF EXISTS (bit AS tinyblob) CASCADE;
DROP CAST IF EXISTS (bit AS mediumblob) CASCADE;
DROP CAST IF EXISTS (bit AS blob) CASCADE;
DROP CAST IF EXISTS (bit AS longblob) CASCADE;

DROP FUNCTION IF EXISTS pg_catalog.bittobinary(bit) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.bittovarbinary(bit) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.bittotinyblob(bit) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.bittomediumblob(bit) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.bittoblob(bit) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.bittolongblob(bit) CASCADE;


DROP CAST IF EXISTS (binary as json) CASCADE;
DROP CAST IF EXISTS (varbinary as json) CASCADE;
DROP CAST IF EXISTS (tinyblob as json) CASCADE;
DROP CAST IF EXISTS (blob as json) CASCADE;
DROP CAST IF EXISTS (mediumblob as json) CASCADE;
DROP CAST IF EXISTS (longblob as json) CASCADE;

DROP FUNCTION IF EXISTS pg_catalog.binary_json(binary) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.varbinary_json(varbinary) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.tinyblob_json(tinyblob) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.blob_json(blob) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.mediumblob_json(mediumblob) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.longblob_json(longblob) CASCADE;

DROP FUNCTION IF EXISTS pg_catalog.to_base64 (tinyblob) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.to_base64 (blob) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.to_base64 (mediumblob) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.to_base64 (longblob) CASCADE;

DROP FUNCTION IF EXISTS pg_catalog.varlena_json(anyelement) cascade;

DROP CAST IF EXISTS (raw AS text);
CREATE CAST (raw AS text) WITH FUNCTION pg_catalog.rawtohex(raw) AS IMPLICIT;


DROP CAST IF EXISTS ("binary" AS int1) CASCADE;
DROP CAST IF EXISTS ("varbinary" AS int1) CASCADE;
DROP CAST IF EXISTS (blob AS int1) CASCADE;
DROP CAST IF EXISTS (tinyblob AS int1) CASCADE;
DROP CAST IF EXISTS (mediumblob AS int1) CASCADE;
DROP CAST IF EXISTS (longblob AS int1) CASCADE;
DROP CAST IF EXISTS (json AS int1) CASCADE;
DROP CAST IF EXISTS ("binary" AS int2) CASCADE;
DROP CAST IF EXISTS ("varbinary" AS int2) CASCADE;
DROP CAST IF EXISTS (blob AS int2) CASCADE;
DROP CAST IF EXISTS (tinyblob AS int2) CASCADE;
DROP CAST IF EXISTS (mediumblob AS int2) CASCADE;
DROP CAST IF EXISTS (longblob AS int2) CASCADE;
DROP CAST IF EXISTS (json AS int2) CASCADE;
DROP CAST IF EXISTS ("binary" AS int4) CASCADE;
DROP CAST IF EXISTS ("varbinary" AS int4) CASCADE;
DROP CAST IF EXISTS (blob AS int4) CASCADE;
DROP CAST IF EXISTS (tinyblob AS int4) CASCADE;
DROP CAST IF EXISTS (mediumblob AS int4) CASCADE;
DROP CAST IF EXISTS (longblob AS int4) CASCADE;
DROP CAST IF EXISTS (json AS int4) CASCADE;
DROP CAST IF EXISTS ("binary" AS int8) CASCADE;
DROP CAST IF EXISTS ("varbinary" AS int8) CASCADE;
DROP CAST IF EXISTS (blob AS int8) CASCADE;
DROP CAST IF EXISTS (tinyblob AS int8) CASCADE;
DROP CAST IF EXISTS (mediumblob AS int8) CASCADE;
DROP CAST IF EXISTS (longblob AS int8) CASCADE;
DROP CAST IF EXISTS (json AS int8) CASCADE;
DROP CAST IF EXISTS ("binary" AS float4) CASCADE;
DROP CAST IF EXISTS ("varbinary" AS float4) CASCADE;
DROP CAST IF EXISTS (blob AS float4) CASCADE;
DROP CAST IF EXISTS (tinyblob AS float4) CASCADE;
DROP CAST IF EXISTS (mediumblob AS float4) CASCADE;
DROP CAST IF EXISTS (longblob AS float4) CASCADE;
DROP CAST IF EXISTS (json AS float4) CASCADE;
DROP CAST IF EXISTS ("binary" AS numeric) CASCADE;
DROP CAST IF EXISTS ("varbinary" AS numeric) CASCADE;
DROP CAST IF EXISTS (blob AS numeric) CASCADE;
DROP CAST IF EXISTS (tinyblob AS numeric) CASCADE;
DROP CAST IF EXISTS (mediumblob AS numeric) CASCADE;
DROP CAST IF EXISTS (longblob AS numeric) CASCADE;
DROP CAST IF EXISTS (json AS numeric) CASCADE;
DROP CAST IF EXISTS ("binary" AS char) CASCADE;
DROP CAST IF EXISTS ("varbinary" AS char) CASCADE;
DROP CAST IF EXISTS (blob AS char) CASCADE;
DROP CAST IF EXISTS (tinyblob AS char) CASCADE;
DROP CAST IF EXISTS (mediumblob AS char) CASCADE;
DROP CAST IF EXISTS (longblob AS char) CASCADE;
DROP CAST IF EXISTS (json AS char) CASCADE;
DROP CAST IF EXISTS ("binary" AS varchar) CASCADE;
DROP CAST IF EXISTS ("varbinary" AS varchar) CASCADE;
DROP CAST IF EXISTS (blob AS varchar) CASCADE;
DROP CAST IF EXISTS (tinyblob AS varchar) CASCADE;
DROP CAST IF EXISTS (mediumblob AS varchar) CASCADE;
DROP CAST IF EXISTS (longblob AS varchar) CASCADE;
DROP CAST IF EXISTS (json AS varchar) CASCADE;
DROP CAST IF EXISTS ("binary" AS text) CASCADE;
DROP CAST IF EXISTS ("varbinary" AS text) CASCADE;
DROP CAST IF EXISTS (blob AS text) CASCADE;
DROP CAST IF EXISTS (tinyblob AS text) CASCADE;
DROP CAST IF EXISTS (mediumblob AS text) CASCADE;
DROP CAST IF EXISTS (longblob AS text) CASCADE;
DROP CAST IF EXISTS (json AS text) CASCADE;
DROP CAST IF EXISTS ("binary" AS date) CASCADE;
DROP CAST IF EXISTS ("varbinary" AS date) CASCADE;
DROP CAST IF EXISTS (blob AS date) CASCADE;
DROP CAST IF EXISTS (tinyblob AS date) CASCADE;
DROP CAST IF EXISTS (mediumblob AS date) CASCADE;
DROP CAST IF EXISTS (longblob AS date) CASCADE;
DROP CAST IF EXISTS (json AS date) CASCADE;
DROP CAST IF EXISTS ("binary" AS timestamptz) CASCADE;
DROP CAST IF EXISTS ("varbinary" AS timestamptz) CASCADE;
DROP CAST IF EXISTS (blob AS timestamptz) CASCADE;
DROP CAST IF EXISTS (tinyblob AS timestamptz) CASCADE;
DROP CAST IF EXISTS (mediumblob AS timestamptz) CASCADE;
DROP CAST IF EXISTS (longblob AS timestamptz) CASCADE;
DROP CAST IF EXISTS (json AS timestamptz) CASCADE;
DROP CAST IF EXISTS ("binary" AS timestamp without time zone) CASCADE;
DROP CAST IF EXISTS ("varbinary" AS timestamp without time zone) CASCADE;
DROP CAST IF EXISTS (blob AS timestamp without time zone) CASCADE;
DROP CAST IF EXISTS (tinyblob AS timestamp without time zone) CASCADE;
DROP CAST IF EXISTS (mediumblob AS timestamp without time zone) CASCADE;
DROP CAST IF EXISTS (longblob AS timestamp without time zone) CASCADE;
DROP CAST IF EXISTS (json AS timestamp without time zone)CASCADE;
DROP CAST IF EXISTS ("binary" AS time) CASCADE;
DROP CAST IF EXISTS ("varbinary" AS time) CASCADE;
DROP CAST IF EXISTS (blob AS time) CASCADE;
DROP CAST IF EXISTS (tinyblob AS time) CASCADE;
DROP CAST IF EXISTS (mediumblob AS time) CASCADE;
DROP CAST IF EXISTS (longblob AS time) CASCADE;
DROP CAST IF EXISTS (json AS time) CASCADE;
DROP CAST IF EXISTS ("binary" AS year) CASCADE;
DROP CAST IF EXISTS ("varbinary" AS year) CASCADE;
DROP CAST IF EXISTS (blob AS year) CASCADE;
DROP CAST IF EXISTS (tinyblob AS year) CASCADE;
DROP CAST IF EXISTS (mediumblob AS year) CASCADE;
DROP CAST IF EXISTS (longblob AS year) CASCADE;
DROP CAST IF EXISTS (json AS year) CASCADE;
DROP CAST IF EXISTS ("binary" AS bit) CASCADE;
DROP CAST IF EXISTS ("varbinary" AS bit) CASCADE;
DROP CAST IF EXISTS (blob AS bit) CASCADE;
DROP CAST IF EXISTS (tinyblob AS bit) CASCADE;
DROP CAST IF EXISTS (mediumblob AS bit) CASCADE;
DROP CAST IF EXISTS (longblob AS bit) CASCADE;
DROP CAST IF EXISTS (json AS bit) CASCADE;
DROP CAST IF EXISTS ("binary" AS anyset) CASCADE;
DROP CAST IF EXISTS ("varbinary" AS anyset) CASCADE;
DROP CAST IF EXISTS (blob AS anyset) CASCADE;
DROP CAST IF EXISTS (tinyblob AS anyset) CASCADE;
DROP CAST IF EXISTS (mediumblob AS anyset) CASCADE;
DROP CAST IF EXISTS (longblob AS anyset) CASCADE;
DROP CAST IF EXISTS (json AS anyset) CASCADE;

DROP FUNCTION IF EXISTS pg_catalog.varlenatoset(anyelement, int4) cascade;
DROP FUNCTION IF EXISTS pg_catalog.Varlena2Bit(anyelement, int4) cascade;
DROP FUNCTION IF EXISTS pg_catalog.Varlena2Year(anyelement) cascade;
DROP FUNCTION IF EXISTS pg_catalog.Varlena2Time(anyelement) cascade;
DROP FUNCTION IF EXISTS pg_catalog.Varlena2Datetime(anyelement) cascade;
DROP FUNCTION IF EXISTS pg_catalog.Varlena2Timestamptz(anyelement) cascade;
DROP FUNCTION IF EXISTS pg_catalog.Varlena2Date(anyelement) cascade;
DROP FUNCTION IF EXISTS pg_catalog.Varlena2Text(anyelement) cascade;
DROP FUNCTION IF EXISTS pg_catalog.Varlena2Varchar(anyelement) cascade;
DROP FUNCTION IF EXISTS pg_catalog.Varlena2Bpchar(anyelement) cascade;
DROP FUNCTION IF EXISTS pg_catalog.varlena2numeric(anyelement) cascade;
DROP FUNCTION IF EXISTS pg_catalog.varlena2float4(anyelement) cascade;
DROP FUNCTION IF EXISTS pg_catalog.varlena2int8(anyelement) cascade;
DROP FUNCTION IF EXISTS pg_catalog.varlena2int4(anyelement) cascade;
DROP FUNCTION IF EXISTS pg_catalog.varlena2int2(anyelement) cascade;
DROP FUNCTION IF EXISTS pg_catalog.varlena2int1(anyelement) cascade;

DROP CAST IF EXISTS (json AS float8) CASCADE;

--unsigned.sql rollback
DO $for_og_310$
BEGIN
    if working_version_num() > 92780 then
        CREATE CAST (json AS float8) WITH FUNCTION pg_catalog.varlena2float8(anyelement);
        --DROP FUNCTION IF EXISTS pg_catalog.bit_longblob(uint8,longblob) CASCADE;
        CREATE OR REPLACE FUNCTION pg_catalog.bit_longblob (t1 uint8, t2 longblob) RETURNS uint8  AS
        $$
        DECLARE num NUMBER := to_number(unhex(substring(cast(t2 as text), 3)));
        BEGIN
            IF num > 9223372036854775807 then
                num = 9223372036854775807;
            ELSEIF num < -9223372036854775808 then
                num = 9223372036854775808;
            END IF;
            RETURN (SELECT uint8_xor(t1, num));
        END;
        $$
        LANGUAGE plpgsql;
    end if;
END
$for_og_310$;

--DROP FUNCTION IF EXISTS pg_catalog.bit2float4(bit) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.bit2float4 (bit) RETURNS float4 AS
$$
BEGIN
    RETURN (SELECT int4($1));
END;
$$
LANGUAGE plpgsql;

--DROP FUNCTION IF EXISTS pg_catalog.bit2float8(bit) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.bit2float8 (bit) RETURNS float8 AS
$$
BEGIN
    RETURN (SELECT int8($1));
END;
$$
LANGUAGE plpgsql;

--DROP FUNCTION IF EXISTS pg_catalog.varbinary_in(cstring) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.varbinary_in (
cstring
) RETURNS varbinary LANGUAGE INTERNAL IMMUTABLE STRICT as 'byteain';

DROP OPERATOR IF EXISTS pg_catalog.&(uint4, year) CASCADE;
DROP OPERATOR IF EXISTS pg_catalog.&(year, uint4) CASCADE;
DROP OPERATOR IF EXISTS pg_catalog.&(uint4, nvarchar2) CASCADE;
DROP OPERATOR IF EXISTS pg_catalog.&(nvarchar2, uint4) CASCADE;
DROP OPERATOR IF EXISTS pg_catalog.^(uint4, year) CASCADE;
DROP OPERATOR IF EXISTS pg_catalog.^(year, uint4) CASCADE;
DROP OPERATOR IF EXISTS pg_catalog.|(uint4, year) CASCADE;
DROP OPERATOR IF EXISTS pg_catalog.|(year, uint4) CASCADE;
DROP OPERATOR IF EXISTS pg_catalog.|(uint4, nvarchar2) CASCADE;
DROP OPERATOR IF EXISTS pg_catalog.|(nvarchar2, uint4) CASCADE;
DROP OPERATOR IF EXISTS pg_catalog.%(uint4, year) CASCADE;
DROP OPERATOR IF EXISTS pg_catalog.&(year, int8) CASCADE;
DROP OPERATOR IF EXISTS pg_catalog.&(nvarchar2, int8) CASCADE;
DROP OPERATOR IF EXISTS pg_catalog.&(int8, year) CASCADE;
DROP OPERATOR IF EXISTS pg_catalog.&(int8, nvarchar2) CASCADE;
DROP OPERATOR IF EXISTS pg_catalog.#(year, int8) CASCADE;
DROP OPERATOR IF EXISTS pg_catalog.#(int8, year) CASCADE;
DROP OPERATOR IF EXISTS pg_catalog.^(year, int8) CASCADE;
DROP OPERATOR IF EXISTS pg_catalog.^(int8, year) CASCADE;
DROP OPERATOR IF EXISTS pg_catalog.#(nvarchar2, int8) CASCADE;
DROP OPERATOR IF EXISTS pg_catalog.#(int8, nvarchar2) CASCADE;
DROP OPERATOR IF EXISTS pg_catalog.|(year, int8) CASCADE;
DROP OPERATOR IF EXISTS pg_catalog.|(nvarchar2, int8) CASCADE;
DROP OPERATOR IF EXISTS pg_catalog.|(int8, year) CASCADE;
DROP OPERATOR IF EXISTS pg_catalog.|(int8, nvarchar2) CASCADE;
DROP OPERATOR IF EXISTS pg_catalog.%(int8, year) CASCADE;
DROP OPERATOR IF EXISTS pg_catalog.>=(time, uint2) CASCADE;
DROP OPERATOR IF EXISTS pg_catalog.>=(time, uint1) CASCADE;
DROP OPERATOR IF EXISTS pg_catalog.>=(date, uint2) CASCADE;
DROP OPERATOR IF EXISTS pg_catalog.>=(date, uint1) CASCADE;
DROP OPERATOR IF EXISTS pg_catalog.<=(time, uint2) CASCADE;
DROP OPERATOR IF EXISTS pg_catalog.<=(time, uint1) CASCADE;
DROP OPERATOR IF EXISTS pg_catalog.<=(date, uint2) CASCADE;
DROP OPERATOR IF EXISTS pg_catalog.<=(date, uint1) CASCADE;
DROP OPERATOR IF EXISTS pg_catalog.>(time, uint2) CASCADE;
DROP OPERATOR IF EXISTS pg_catalog.>(time, uint1) CASCADE;
DROP OPERATOR IF EXISTS pg_catalog.>(date, uint2) CASCADE;
DROP OPERATOR IF EXISTS pg_catalog.>(date, uint1) CASCADE;
DROP OPERATOR IF EXISTS pg_catalog.<(time, uint2) CASCADE;
DROP OPERATOR IF EXISTS pg_catalog.<(time, uint1) CASCADE;
DROP OPERATOR IF EXISTS pg_catalog.<(date, uint2) CASCADE;
DROP OPERATOR IF EXISTS pg_catalog.<(date, uint1) CASCADE;
DROP OPERATOR IF EXISTS pg_catalog.<>(uint2, time) CASCADE;
DROP OPERATOR IF EXISTS pg_catalog.<>(uint1, time) CASCADE;
DROP OPERATOR IF EXISTS pg_catalog.<>(uint2, date) CASCADE;
DROP OPERATOR IF EXISTS pg_catalog.<>(uint1, date) CASCADE;
DROP OPERATOR IF EXISTS pg_catalog.<>(time, uint2) CASCADE;
DROP OPERATOR IF EXISTS pg_catalog.<>(time, uint1) CASCADE;
DROP OPERATOR IF EXISTS pg_catalog.<>(date, uint2) CASCADE;
DROP OPERATOR IF EXISTS pg_catalog.<>(date, uint1) CASCADE;
DROP OPERATOR IF EXISTS pg_catalog.=(time, uint2) CASCADE;
DROP OPERATOR IF EXISTS pg_catalog.=(time, uint1) CASCADE;
DROP OPERATOR IF EXISTS pg_catalog.=(date, uint2) CASCADE;
DROP OPERATOR IF EXISTS pg_catalog.=(date, uint1) CASCADE;
DROP OPERATOR IF EXISTS pg_catalog.^(timestampTz, uint8) CASCADE;
DROP OPERATOR IF EXISTS pg_catalog.^(uint8, timestampTz) CASCADE;
DROP OPERATOR IF EXISTS pg_catalog.^(timestamp without time zone, uint8) CASCADE;
DROP OPERATOR IF EXISTS pg_catalog.^(uint8, timestamp without time zone) CASCADE;
DROP OPERATOR IF EXISTS pg_catalog.^(time, uint8) CASCADE;
DROP OPERATOR IF EXISTS pg_catalog.^(uint8, time) CASCADE;
DROP OPERATOR IF EXISTS pg_catalog.^(date, uint8) CASCADE;
DROP OPERATOR IF EXISTS pg_catalog.^(uint8, date) CASCADE;

DROP FUNCTION IF EXISTS pg_catalog.date_int8_xor(
    date,
    uint8
) CASCADE;

DROP FUNCTION IF EXISTS pg_catalog.int8_date_xor(
    uint8,
    date
) CASCADE;

DROP FUNCTION IF EXISTS pg_catalog.time_int8_xor(
    time,
    uint8
) CASCADE;

DROP FUNCTION IF EXISTS pg_catalog.int8_time_xor(
    uint8,
    time
) CASCADE;

DROP FUNCTION IF EXISTS pg_catalog.datetime_int8_xor(
    timestamp without time zone,
    uint8
) CASCADE;

DROP FUNCTION IF EXISTS pg_catalog.int8_datetime_xor(
    uint8,
    timestamp without time zone
) CASCADE;

DROP FUNCTION IF EXISTS pg_catalog.timestamptz_int8_xor(
    timestampTz,
    uint8
) CASCADE;

DROP FUNCTION IF EXISTS pg_catalog.int8_timestamptz_xor(
    uint8,
    timestampTz
) CASCADE;

DROP FUNCTION IF EXISTS pg_catalog.date_uint1_eq(
    date,
    uint1
) CASCADE;

DROP FUNCTION IF EXISTS pg_catalog.date_uint2_eq(
    date,
    uint2
) CASCADE;

DROP FUNCTION IF EXISTS pg_catalog.time_uint1_eq(
    time,
    uint1
) CASCADE;

DROP FUNCTION IF EXISTS pg_catalog.time_uint2_eq(
    time,
    uint2
) CASCADE;

DROP FUNCTION IF EXISTS pg_catalog.date_uint1_ne(
    date,
    uint1
) CASCADE;

DROP FUNCTION IF EXISTS pg_catalog.date_uint2_ne(
    date,
    uint2
) CASCADE;

DROP FUNCTION IF EXISTS pg_catalog.time_uint1_ne(
    time,
    uint1
) CASCADE;

DROP FUNCTION IF EXISTS pg_catalog.time_uint2_ne(
    time,
    uint2
) CASCADE;

DROP FUNCTION IF EXISTS pg_catalog.uint1_date_ne(
    uint1,
    date
) CASCADE;

DROP FUNCTION IF EXISTS pg_catalog.uint2_date_ne(
    uint2,
    date
) CASCADE;

DROP FUNCTION IF EXISTS pg_catalog.uint1_time_ne(
    uint1,
    time
) CASCADE;

DROP FUNCTION IF EXISTS pg_catalog.uint2_time_ne(
    uint2,
    time
) CASCADE;

DROP FUNCTION IF EXISTS pg_catalog.date_uint1_lt(
    date,
    uint1
) CASCADE;

DROP FUNCTION IF EXISTS pg_catalog.date_uint2_lt(
    date,
    uint2
) CASCADE;

DROP FUNCTION IF EXISTS pg_catalog.time_uint1_lt(
    time,
    uint1
) CASCADE;

DROP FUNCTION IF EXISTS pg_catalog.time_uint2_lt(
    time,
    uint2
) CASCADE;

DROP FUNCTION IF EXISTS pg_catalog.date_uint1_gt(
    date,
    uint1
) CASCADE;

DROP FUNCTION IF EXISTS pg_catalog.date_uint2_gt(
    date,
    uint2
) CASCADE;

DROP FUNCTION IF EXISTS pg_catalog.time_uint1_gt(
    time,
    uint1
) CASCADE;

DROP FUNCTION IF EXISTS pg_catalog.time_uint2_gt(
    time,
    uint2
) CASCADE;

DROP FUNCTION IF EXISTS pg_catalog.date_uint1_le(
    date,
    uint1
) CASCADE;

DROP FUNCTION IF EXISTS pg_catalog.date_uint2_le(
    date,
    uint2
) CASCADE;

DROP FUNCTION IF EXISTS pg_catalog.time_uint1_le(
    time,
    uint1
) CASCADE;

DROP FUNCTION IF EXISTS pg_catalog.time_uint2_le(
    time,
    uint2
) CASCADE;

DROP FUNCTION IF EXISTS pg_catalog.date_uint1_ge(
    date,
    uint1
) CASCADE;

DROP FUNCTION IF EXISTS pg_catalog.date_uint2_ge(
    date,
    uint2
) CASCADE;

DROP FUNCTION IF EXISTS pg_catalog.time_uint1_ge(
    time,
    uint1
) CASCADE;

DROP FUNCTION IF EXISTS pg_catalog.time_uint2_ge(
    time,
    uint2
) CASCADE;

DROP FUNCTION IF EXISTS pg_catalog.int8_year_mod(
    int8,
    year
) CASCADE;

DROP FUNCTION IF EXISTS pg_catalog.int8_or_nvarchar2(
    int8,
    nvarchar2
) CASCADE;

DROP FUNCTION IF EXISTS pg_catalog.int8_or_year(
    int8,
    year
) CASCADE;

DROP FUNCTION IF EXISTS pg_catalog.nvarchar2_or_int8(
    nvarchar2,
    int8
) CASCADE;

DROP FUNCTION IF EXISTS pg_catalog.year_or_int8(
    year,
    int8
) CASCADE;

DROP FUNCTION IF EXISTS pg_catalog.int8_xor_nvarchar2(
    int8,
    nvarchar2
) CASCADE;

DROP FUNCTION IF EXISTS pg_catalog.int8_xor_year(
    int8,
    year
) CASCADE;


DROP FUNCTION IF EXISTS pg_catalog.nvarchar2_xor_int8(
    nvarchar2,
    int8
) CASCADE;

DROP FUNCTION IF EXISTS pg_catalog.year_xor_int8(
    year,
    int8
) CASCADE;


DROP FUNCTION IF EXISTS pg_catalog.int8_and_nvarchar2(
    int8,
    nvarchar2
) CASCADE;

DROP FUNCTION IF EXISTS pg_catalog.int8_and_year(
    int8,
    year
) CASCADE;

DROP FUNCTION IF EXISTS pg_catalog.nvarchar2_and_int8(
    nvarchar2,
    int8
) CASCADE;

DROP FUNCTION IF EXISTS pg_catalog.year_and_int8(
    year,
    int8
) CASCADE;

DROP FUNCTION IF EXISTS pg_catalog.uint4_mod_year(
    uint4,
    year
) CASCADE;

DROP FUNCTION IF EXISTS pg_catalog.nvarchar2_or_uint4(
    nvarchar2,
    uint4
) CASCADE;

DROP FUNCTION IF EXISTS pg_catalog.uint4_or_nvarchar2(
    uint4,
    nvarchar2
) CASCADE;

DROP FUNCTION IF EXISTS pg_catalog.year_or_uint4(
    year,
    uint4
) CASCADE;

DROP FUNCTION IF EXISTS pg_catalog.uint4_or_year(
    uint4,
    year
) CASCADE;

DROP FUNCTION IF EXISTS pg_catalog.year_xor_uint4(
    year,
    uint4
) CASCADE;

DROP FUNCTION IF EXISTS pg_catalog.uint4_xor_year(
    uint4,
    year
) CASCADE;

DROP FUNCTION IF EXISTS pg_catalog.nvarchar2_and_uint4(
    nvarchar2,
    uint4
) CASCADE;

DROP FUNCTION IF EXISTS pg_catalog.uint4_and_nvarchar2(
    uint4,
    nvarchar2
) CASCADE;

DROP FUNCTION IF EXISTS pg_catalog.year_and_uint4(
    year,
    uint4
) CASCADE;

DROP FUNCTION IF EXISTS pg_catalog.uint4_and_year(
    uint4,
    year
) CASCADE;


DROP FUNCTION IF EXISTS pg_catalog.enum_uint1(anyenum) cascade;
DROP FUNCTION IF EXISTS pg_catalog.enum_uint2(anyenum) cascade;
DROP FUNCTION IF EXISTS pg_catalog.enum_uint4(anyenum) cascade;
DROP FUNCTION IF EXISTS pg_catalog.enum_uint8(anyenum) cascade;

DROP FUNCTION IF EXISTS pg_catalog.uint1_enum(uint1, int4, anyelement) cascade;
DROP FUNCTION IF EXISTS pg_catalog.uint2_enum(uint2, int4, anyelement) cascade;
DROP FUNCTION IF EXISTS pg_catalog.uint4_enum(uint4, int4, anyelement) cascade;
DROP FUNCTION IF EXISTS pg_catalog.uint8_enum(uint8, int4, anyelement) cascade;

DROP CAST IF EXISTS (int1 AS anyset) CASCADE;
DROP CAST IF EXISTS (uint1 AS anyset) CASCADE;
DROP CAST IF EXISTS (uint2 AS anyset) CASCADE;
DROP CAST IF EXISTS (uint4 AS anyset) CASCADE;
DROP CAST IF EXISTS (uint8 AS anyset) CASCADE;

DROP FUNCTION IF EXISTS pg_catalog.set(int1, int4) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.set(uint1, int4) CASCADE; 
DROP FUNCTION IF EXISTS pg_catalog.set(uint2, int4) CASCADE; 
DROP FUNCTION IF EXISTS pg_catalog.set(uint4, int4) CASCADE; 
DROP FUNCTION IF EXISTS pg_catalog.set(uint8, int4) CASCADE; 

DROP CAST IF EXISTS (anyset AS bit) CASCADE;
DROP CAST IF EXISTS (anyset AS int1) CASCADE;
DROP CAST IF EXISTS (anyset AS uint1) CASCADE;
DROP CAST IF EXISTS (anyset AS uint2) CASCADE;
DROP CAST IF EXISTS (anyset AS uint4) CASCADE;
DROP CAST IF EXISTS (anyset AS uint8) CASCADE;

DROP FUNCTION IF EXISTS pg_catalog.settobit(anyset, int4) cascade;
DROP FUNCTION IF EXISTS pg_catalog.settoint1(anyset) cascade;
DROP FUNCTION IF EXISTS pg_catalog.settouint1(anyset) cascade;
DROP FUNCTION IF EXISTS pg_catalog.settouint2(anyset) cascade;
DROP FUNCTION IF EXISTS pg_catalog.settouint4(anyset) cascade;
DROP FUNCTION IF EXISTS pg_catalog.settouint8(anyset) cascade;

DROP CAST IF EXISTS (char AS uint1) CASCADE;
DROP CAST IF EXISTS (char AS uint2) CASCADE;
DROP CAST IF EXISTS (char AS uint4) CASCADE;
DROP CAST IF EXISTS (char AS uint8) CASCADE;
DROP CAST IF EXISTS (varchar AS uint1) CASCADE;
DROP CAST IF EXISTS (varchar AS uint2) CASCADE;
DROP CAST IF EXISTS (varchar AS uint4) CASCADE;
DROP CAST IF EXISTS (varchar AS uint8) CASCADE;

DROP FUNCTION IF EXISTS pg_catalog.bpchar_uint1 (char) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.bpchar_uint2 (char) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.bpchar_uint4 (char) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.bpchar_uint8 (char) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.varchar_uint1 (varchar) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.varchar_uint2 (varchar) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.varchar_uint4 (varchar) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.varchar_uint8 (varchar) CASCADE;

DROP CAST IF EXISTS (bit AS uint1) CASCADE;
DROP CAST IF EXISTS (bit AS uint2) CASCADE;
DROP CAST IF EXISTS (uint1 AS bit) CASCADE;
DROP CAST IF EXISTS (uint2 AS bit) CASCADE;

DROP FUNCTION IF EXISTS pg_catalog.bittouint1(bit) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.bittouint2(bit) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.bitfromuint1(uint1, int4) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.bitfromuint2(uint2, int4) CASCADE;

DROP CAST IF EXISTS (uint1 as json) cascade;
DROP CAST IF EXISTS (uint2 as json) cascade;
DROP CAST IF EXISTS (uint4 as json) cascade;
DROP CAST IF EXISTS (uint8 as json) cascade;

DROP FUNCTION IF EXISTS pg_catalog.uint1_json(uint1) cascade;
DROP FUNCTION IF EXISTS pg_catalog.uint2_json(uint2) cascade;
DROP FUNCTION IF EXISTS pg_catalog.uint4_json(uint4) cascade;
DROP FUNCTION IF EXISTS pg_catalog.uint8_json(uint8) cascade;

DROP CAST IF EXISTS (uint8 AS year) CASCADE;
DROP CAST IF EXISTS (year AS uint1) CASCADE;
DROP CAST IF EXISTS (year AS uint2) CASCADE;

DROP FUNCTION IF EXISTS pg_catalog.int64_year(uint8) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.year_uint1(year) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.year_uint2(year) CASCADE;

DROP CAST IF EXISTS ("timestamptz" AS uint1) CASCADE;
DROP CAST IF EXISTS ("timestamptz" AS uint2) CASCADE;
DROP CAST IF EXISTS ("timestamptz" AS uint4) CASCADE;
DROP CAST IF EXISTS (uint1 AS year) CASCADE;
DROP CAST IF EXISTS (uint2 AS year) CASCADE;

DROP FUNCTION IF EXISTS pg_catalog.timestamptz_uint1 ("timestamptz") CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.timestamptz_uint2 ("timestamptz") CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.timestamptz_uint4 ("timestamptz") CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.int8_year(uint1) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.int16_year(uint2) CASCADE;

DROP CAST IF EXISTS (timestamp(0) without time zone AS uint1) CASCADE;
DROP CAST IF EXISTS (timestamp(0) without time zone AS uint2) CASCADE;
DROP CAST IF EXISTS (timestamp(0) without time zone AS uint4) CASCADE;
DROP CAST IF EXISTS (timestamp(0) without time zone AS uint8) CASCADE;
DROP CAST IF EXISTS (uint1 AS timestamptz) CASCADE;
DROP CAST IF EXISTS (uint2 AS timestamptz) CASCADE;

DROP FUNCTION IF EXISTS pg_catalog.datetime_uint1 (timestamp(0) without time zone) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.datetime_uint2 (timestamp(0) without time zone) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.datetime_uint4 (timestamp(0) without time zone) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.datetime_uint8 (timestamp(0) without time zone) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.int8_b_format_timestamp(uint1) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.int16_b_format_timestamp(uint2) CASCADE;

DROP CAST IF EXISTS (uint8 AS time) CASCADE;
DROP CAST IF EXISTS (time AS uint1) CASCADE;
DROP CAST IF EXISTS (time AS uint2) CASCADE;
DROP CAST IF EXISTS (time AS uint4) CASCADE;
DROP CAST IF EXISTS (time AS uint8) CASCADE;
DROP CAST IF EXISTS (uint1 AS timestamp(0) without time zone) CASCADE;
DROP CAST IF EXISTS (uint2 AS timestamp(0) without time zone) CASCADE;

CREATE OR REPLACE FUNCTION pg_catalog.int32_b_format_time (
uint4
) RETURNS time LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int32_b_format_time';

DROP FUNCTION IF EXISTS pg_catalog.int64_b_format_time(uint8) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.time_uint1 (time) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.time_uint2 (time) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.time_uint4 (time) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.time_uint8 (time) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.int8_b_format_datetime(uint1) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.int16_b_format_datetime(uint2) CASCADE;

DROP CAST IF EXISTS (uint8 AS date) CASCADE;
DROP CAST IF EXISTS ("date" as uint1) CASCADE;
DROP CAST IF EXISTS ("date" as uint2) CASCADE;
DROP CAST IF EXISTS ("date" as uint4) CASCADE;
DROP CAST IF EXISTS ("date" as uint8) CASCADE;
DROP CAST IF EXISTS (uint1 AS time) CASCADE;
DROP CAST IF EXISTS (uint2 AS time) CASCADE;

DROP FUNCTION IF EXISTS pg_catalog.int64_b_format_date (uint8) cascade;
DROP FUNCTION IF EXISTS pg_catalog.date2uint1("date") cascade;
DROP FUNCTION IF EXISTS pg_catalog.date2uint2("date") cascade;
DROP FUNCTION IF EXISTS pg_catalog.date2uint4("date") cascade;
DROP FUNCTION IF EXISTS pg_catalog.date2uint8("date") cascade;
DROP FUNCTION IF EXISTS pg_catalog.int8_b_format_time(uint1) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.int16_b_format_time(uint2) CASCADE;

DROP CAST IF EXISTS (uint1 AS date) CASCADE;
DROP CAST IF EXISTS (uint2 AS date) CASCADE;

DROP FUNCTION IF EXISTS pg_catalog.int8_b_format_date (uint1) cascade;
DROP FUNCTION IF EXISTS pg_catalog.int16_b_format_date (uint2) cascade;

--to_base64.sql rollback
DROP CAST IF EXISTS (bit as json);
DROP FUNCTION IF EXISTS pg_catalog.bit_json (bit) CASCADE;

--json.sql rollback
DROP CAST IF EXISTS (year as json) cascade;
DROP FUNCTION IF EXISTS pg_catalog.year_json(year) cascade;

DROP CAST IF EXISTS (int1 as json) cascade;
DROP CAST IF EXISTS (int2 as json) cascade;
DROP CAST IF EXISTS (int4 as json) cascade;
DROP CAST IF EXISTS (int8 as json) cascade;
DROP CAST IF EXISTS (float4 as json) cascade;
DROP CAST IF EXISTS (float8 as json) cascade;
DROP CAST IF EXISTS (numeric as json) cascade;
DROP CAST IF EXISTS (date as json) cascade;
DROP CAST IF EXISTS (timestamp without time zone as json) cascade;
DROP CAST IF EXISTS (timestamptz as json) cascade;
DROP CAST IF EXISTS (time as json) cascade;
DROP CAST IF EXISTS (anyset as json) cascade;

DROP FUNCTION IF EXISTS pg_catalog.int1_json(int1) cascade;
DROP FUNCTION IF EXISTS pg_catalog.int2_json(int2) cascade;
DROP FUNCTION IF EXISTS pg_catalog.int4_json(int4) cascade;
DROP FUNCTION IF EXISTS pg_catalog.int8_json(int8) cascade;
DROP FUNCTION IF EXISTS pg_catalog.float4_json(float4) cascade;
DROP FUNCTION IF EXISTS pg_catalog.float8_json(float8) cascade;
DROP FUNCTION IF EXISTS pg_catalog.numeric_json(numeric) cascade;
DROP FUNCTION IF EXISTS pg_catalog.date_json(date) cascade;
DROP FUNCTION IF EXISTS pg_catalog.datetime_json(timestamp without time zone) cascade;
DROP FUNCTION IF EXISTS pg_catalog.timestamp_json(timestamptz) cascade;
DROP FUNCTION IF EXISTS pg_catalog.time_json(time) cascade;
DROP FUNCTION IF EXISTS pg_catalog.set_json(anyset) cascade;
DROP FUNCTION IF EXISTS pg_catalog.enum_varlena(anyenum) cascade;

--enum.sql rollback
DROP FUNCTION IF EXISTS pg_catalog.year_enum(year, int4, anyelement) cascade;
DROP FUNCTION IF EXISTS pg_catalog.bit_enum(bit, int4, anyelement) cascade;
DROP FUNCTION IF EXISTS pg_catalog.numeric_enum(numeric, int4, anyelement) cascade;
DROP FUNCTION IF EXISTS pg_catalog.float8_enum(float8, int4, anyelement) cascade;
DROP FUNCTION IF EXISTS pg_catalog.float4_enum(float4, int4, anyelement) cascade;
DROP FUNCTION IF EXISTS pg_catalog.int8_enum(int8, int4, anyelement) cascade;
DROP FUNCTION IF EXISTS pg_catalog.int4_enum(int4, int4, anyelement) cascade;
DROP FUNCTION IF EXISTS pg_catalog.int2_enum(int2, int4, anyelement) cascade;
DROP FUNCTION IF EXISTS pg_catalog.int1_enum(int1, int4, anyelement) cascade;
DROP FUNCTION IF EXISTS pg_catalog.time_enum(time, int4, anyelement) cascade;
DROP FUNCTION IF EXISTS pg_catalog.timestamp_enum(timestamptz, int4, anyelement) cascade;
DROP FUNCTION IF EXISTS pg_catalog.datetime_enum(timestamp without time zone, int4, anyelement) cascade;
DROP FUNCTION IF EXISTS pg_catalog.date_enum(date, int4, anyelement) cascade;
DROP FUNCTION IF EXISTS pg_catalog.set_enum(anyset, int4, anyelement) cascade;
DROP FUNCTION IF EXISTS pg_catalog.varlena_enum(anyelement, int4) cascade;
DROP FUNCTION IF EXISTS pg_catalog.varchar_enum(varchar, int4, anyelement) cascade;
DROP FUNCTION IF EXISTS pg_catalog.bpchar_enum(char, int4, anyelement) cascade;
DROP FUNCTION IF EXISTS pg_catalog.text_enum(text, int4, anyelement) cascade;
DROP FUNCTION IF EXISTS pg_catalog.enum_json(anyenum) cascade;
DROP FUNCTION IF EXISTS pg_catalog.enum_set(anyenum, int4) cascade;
DROP FUNCTION IF EXISTS pg_catalog.enum_year(anyenum) cascade;
DROP FUNCTION IF EXISTS pg_catalog.enum_time(anyenum) cascade;
DROP FUNCTION IF EXISTS pg_catalog.enum_timestamptz(anyenum) cascade;
DROP FUNCTION IF EXISTS pg_catalog.enum_timestamp(anyenum) cascade;
DROP FUNCTION IF EXISTS pg_catalog.enum_date(anyenum) cascade;
DROP FUNCTION IF EXISTS pg_catalog.enum_bit(anyenum, int4) cascade;
DROP FUNCTION IF EXISTS pg_catalog.enum_float4(anyenum) cascade;
DROP FUNCTION IF EXISTS pg_catalog.enum_numeric(anyenum) cascade;
DROP FUNCTION IF EXISTS pg_catalog.enum_int8(anyenum) cascade;
DROP FUNCTION IF EXISTS pg_catalog.enum_int4(anyenum) cascade;
DROP FUNCTION IF EXISTS pg_catalog.enum_int2(anyenum) cascade;
DROP FUNCTION IF EXISTS pg_catalog.enum_int1(anyenum) cascade;
DROP FUNCTION IF EXISTS pg_catalog.enum_float8(anyenum) cascade;

DO $for_og_310$
BEGIN
    if working_version_num() > 92780 then
        DROP FUNCTION IF EXISTS pg_catalog.enum2float8(anyenum) cascade;
        CREATE OR REPLACE FUNCTION pg_catalog.enum2float8(anyenum) RETURNS float8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'Enum2Float8';

        INSERT INTO pg_catalog.pg_cast
        -- castsource is anyenum(3500), casttarget(701) is float8, castowner is 10(superuser)
        SELECT 3500, 701, oid, 'i', 'f', 10
        FROM pg_proc WHERE 
        proname = 'enum2float8' AND
        -- namespace is pg_catalog
        pronamespace = 11 AND
        -- input arg is anyenum
        proargtypes='3500' AND
        -- return type is float8
        prorettype = 701;
    end if;
END
$for_og_310$;

--dolphin_time_functions.sql rollback
DROP OPERATOR IF EXISTS pg_catalog.^(time, timestampTz) CASCADE;
DROP OPERATOR IF EXISTS pg_catalog.^(timestampTz, time) CASCADE;
DROP OPERATOR IF EXISTS pg_catalog.^(boolean, date) CASCADE;
DROP OPERATOR IF EXISTS pg_catalog.^(boolean, timestamptz) CASCADE;
DROP OPERATOR IF EXISTS pg_catalog.^(date, boolean) CASCADE;
DROP OPERATOR IF EXISTS pg_catalog.^(timestamptz, boolean) CASCADE;
DROP OPERATOR IF EXISTS pg_catalog.-(nvarchar2, time) CASCADE;
DROP OPERATOR IF EXISTS pg_catalog.+(nvarchar2, time) CASCADE;
DROP OPERATOR IF EXISTS pg_catalog.+(date, numeric) CASCADE;
DROP OPERATOR IF EXISTS pg_catalog.-(date, numeric) CASCADE;
DROP OPERATOR IF EXISTS pg_catalog.*(time, numeric) CASCADE;
DROP OPERATOR IF EXISTS pg_catalog./(time, numeric) CASCADE;
DROP OPERATOR IF EXISTS pg_catalog.*(timestamp without time zone, numeric) CASCADE;
DROP OPERATOR IF EXISTS pg_catalog./(timestamp without time zone, numeric) CASCADE;
DROP OPERATOR IF EXISTS pg_catalog.*(timestamp without time zone, int4) CASCADE;

DROP FUNCTION IF EXISTS pg_catalog.b_db_date(date) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.dayname(date) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.b_db_last_day(date) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.week(date) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.yearweek(date) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.adddate(date, interval) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.date_add(date, interval) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.date_sub(date, interval) CASCADE;

DROP FUNCTION IF EXISTS pg_catalog.time_timestamptz_xor(time, timestamptz) CASCADE;

DROP FUNCTION IF EXISTS pg_catalog.timestamptz_time_xor(timestamptz, time) CASCADE;

DROP FUNCTION IF EXISTS pg_catalog.bool_date_xor(boolean, date) CASCADE;

DROP FUNCTION IF EXISTS pg_catalog.bool_timestamptz_xor(boolean, timestamptz) CASCADE;

DROP FUNCTION IF EXISTS pg_catalog.date_bool_xor(date, boolean) CASCADE;

DROP FUNCTION IF EXISTS pg_catalog.timestamptz_bool_xor(timestamptz, boolean) CASCADE;

DROP FUNCTION IF EXISTS pg_catalog.nvarchar2_mi_time(nvarchar2, time) CASCADE;

DROP FUNCTION IF EXISTS pg_catalog.nvarchar2_pl_time(nvarchar2, time) CASCADE;

DROP FUNCTION IF EXISTS pg_catalog.date_add_numeric(date, numeric) CASCADE;

DROP FUNCTION IF EXISTS pg_catalog.date_sub_numeric(date, numeric) CASCADE;

DROP FUNCTION IF EXISTS pg_catalog.time_mul_numeric(time, numeric) CASCADE;

DROP FUNCTION IF EXISTS pg_catalog.time_div_numeric(time, numeric) CASCADE;

DROP FUNCTION IF EXISTS pg_catalog.datetime_mul_numeric(timestamp without time zone, numeric) CASCADE;

DROP FUNCTION IF EXISTS pg_catalog.datetime_div_numeric(timestamp without time zone, numeric) CASCADE;

DROP FUNCTION IF EXISTS pg_catalog.datetime_mul_int4(timestamp without time zone, int4) CASCADE;

DROP CAST IF EXISTS (year AS anyset) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.set(year, int4) cascade;

DROP CAST IF EXISTS (time AS anyset) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.set(time, int4) cascade;

DROP CAST IF EXISTS (timestamptz AS anyset) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.set(timestamptz, int4) cascade;

DROP CAST IF EXISTS (timestamp without time zone AS anyset) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.set(timestamp without time zone, int4) cascade;

DROP CAST IF EXISTS (date AS anyset) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.set (date, int4) cascade;

DROP CAST IF EXISTS (anyset as year) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.set_year (anyset) cascade;

DROP CAST IF EXISTS (anyset as time) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.set_time (anyset) cascade;

DROP CAST IF EXISTS (anyset as timestamptz) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.set_timestamp (anyset) cascade;

DROP CAST IF EXISTS (anyset as timestamp without time zone) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.set_datetime (anyset) cascade;

DROP CAST IF EXISTS (anyset as date) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.set_date (anyset) cascade;

DROP CAST IF EXISTS (year AS bit) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.bitfromyear(year, int4) cascade;

DROP CAST IF EXISTS (time AS char) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.time_bpchar (time) cascade;

DROP CAST IF EXISTS (time AS varchar) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.time_varchar (time) cascade;

DROP CAST IF EXISTS (timestamptz AS char) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.timestamptz_bpchar (timestamptz) cascade;

DROP CAST IF EXISTS (timestamptz AS varchar) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.timestamptz_varchar (timestamptz) cascade;

DROP CAST IF EXISTS (timestamp without time zone AS char) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.timestamp_bpchar (timestamp without time zone) cascade;

DROP CAST IF EXISTS (varchar as time) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.varchar_time (varchar) cascade;

DROP CAST IF EXISTS (char as time) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.bpchar_time (char) cascade;

DROP CAST IF EXISTS (varchar AS timestamptz) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.varchar_timestamptz (varchar) cascade;

DROP CAST IF EXISTS (char AS timestamptz) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.bpchar_timestamptz (char) cascade;

DROP CAST IF EXISTS (time AS timestamptz) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.time_timestamp (time) cascade;

DROP CAST IF EXISTS (time AS timestamp without time zone) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.time_datetime (time) cascade;

DROP CAST IF EXISTS (time AS date) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.time_date (time) cascade;

DROP CAST IF EXISTS (date AS time) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.date_time (date) cascade;

DROP CAST IF EXISTS (date AS year) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.date_year(date) cascade;

DROP CAST IF EXISTS (time AS year) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.time_year(time) cascade;

DROP CAST IF EXISTS (timestamptz AS year) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.timestamp_year(timestamptz) cascade;

DROP CAST IF EXISTS (timestamp(0) without time zone AS year) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.datetime_year(timestamp(0) without time zone) cascade;

DROP CAST IF EXISTS (year AS time) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.year_time("year") cascade;

DROP CAST IF EXISTS (year AS timestamptz) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.year_timestamp("year") cascade;

DROP CAST IF EXISTS (year AS timestamp(0) without time zone) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.year_datetime("year") cascade;

DROP CAST IF EXISTS (year AS date) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.year_date("year") cascade;


DROP CAST IF EXISTS ("date" as int1) CASCADE;
DROP CAST IF EXISTS ("date" as int2) CASCADE;
DROP CAST IF EXISTS ("date" as float4) CASCADE;
DROP CAST IF EXISTS (year AS float4) CASCADE;

DROP FUNCTION IF EXISTS pg_catalog.date2int1("date") cascade;
DROP FUNCTION IF EXISTS pg_catalog.date2int2("date") cascade;
DROP FUNCTION IF EXISTS pg_catalog.date2float4("date") cascade;
DROP FUNCTION IF EXISTS pg_catalog.year_float4("year") cascade;

DROP CAST IF EXISTS ("timestamptz" AS int1) CASCADE;
DROP CAST IF EXISTS ("timestamptz" AS int2) CASCADE;
DROP CAST IF EXISTS ("timestamptz" AS int4) CASCADE;
DROP CAST IF EXISTS ("timestamptz" AS float4) CASCADE;

DROP FUNCTION IF EXISTS pg_catalog.timestamptz_int1 ("timestamptz") CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.timestamptz_int2 ("timestamptz") CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.timestamptz_int4 ("timestamptz") CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.timestamptz_float4 ("timestamptz") CASCADE;

DROP CAST IF EXISTS (timestamp(0) without time zone AS int1) CASCADE;
DROP CAST IF EXISTS (timestamp(0) without time zone AS int2) CASCADE;
DROP CAST IF EXISTS (timestamp(0) without time zone AS int4) CASCADE;
DROP CAST IF EXISTS (timestamp(0) without time zone AS float4) CASCADE;

DROP FUNCTION IF EXISTS pg_catalog.datetime_int1 (timestamp(0) without time zone) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.datetime_int2 (timestamp(0) without time zone) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.datetime_int4 (timestamp(0) without time zone) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.datetime_float4 (timestamp(0) without time zone) CASCADE;

DROP CAST IF EXISTS (time AS int1) CASCADE;
DROP CAST IF EXISTS (time AS int2) CASCADE;
DROP CAST IF EXISTS (time AS float4) CASCADE;

DROP FUNCTION IF EXISTS pg_catalog.time_int1 (time) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.time_int2 (time) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.time_float4 (time) CASCADE;

DROP CAST IF EXISTS (float4 AS timestamptz) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.float4_b_format_timestamp (float4) CASCADE;

DROP CAST IF EXISTS (int1 AS timestamptz) CASCADE;
DROP CAST IF EXISTS (int2 AS timestamptz) CASCADE;
DROP CAST IF EXISTS (float8 AS timestamptz) CASCADE;

DROP FUNCTION IF EXISTS pg_catalog.int8_b_format_timestamp (int1) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.int16_b_format_timestamp (int2) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.float8_b_format_timestamp (float8) CASCADE;

DROP CAST IF EXISTS (float4 AS timestamp(0) without time zone) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.float4_b_format_datetime (float4) CASCADE;

DROP CAST IF EXISTS (int1 AS timestamp(0) without time zone) CASCADE;
DROP CAST IF EXISTS (int2 AS timestamp(0) without time zone) CASCADE;
DROP CAST IF EXISTS (float8 AS timestamp(0) without time zone) CASCADE;

DROP FUNCTION IF EXISTS pg_catalog.int8_b_format_datetime (int1) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.int16_b_format_datetime (int2) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.float8_b_format_datetime (float8) CASCADE;

DROP CAST IF EXISTS (float4 AS time) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.float4_b_format_time (float4) CASCADE;

DROP CAST IF EXISTS (int1 AS time) CASCADE;
DROP CAST IF EXISTS (int2 AS time) CASCADE;
DROP CAST IF EXISTS (float8 AS time) CASCADE;

DROP FUNCTION IF EXISTS pg_catalog.int8_b_format_time (int1) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.int16_b_format_time (int2) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.float8_b_format_time (float8) CASCADE;

DROP CAST IF EXISTS (float4 AS date) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.float4_b_format_date (float4) CASCADE;

DROP CAST IF EXISTS (int1 AS date) CASCADE;
DROP CAST IF EXISTS (int2 AS date) CASCADE;
DROP CAST IF EXISTS (int8 AS date) CASCADE;
DROP CAST IF EXISTS (numeric AS date) CASCADE;
DROP CAST IF EXISTS (float8 AS date) CASCADE;

DROP FUNCTION IF EXISTS pg_catalog.int8_b_format_date (int1) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.int16_b_format_date (int2) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.int64_b_format_date (int8) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.numeric_b_format_date (numeric) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.float8_b_format_date (float8) CASCADE;

DROP CAST IF EXISTS (bit AS year) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.bit_year (bit) CASCADE;

DROP CAST IF EXISTS (float8 AS year) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.float8_year (float8) CASCADE;

DROP CAST IF EXISTS (float4 AS year) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.float4_year (float4) CASCADE;

DROP CAST IF EXISTS (numeric AS year) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.numeric_year (numeric) CASCADE;

DROP CAST IF EXISTS (year AS int8) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.year_int64 (year) CASCADE;

DROP CAST IF EXISTS (year AS int2) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.year_int16 (year) CASCADE;

DROP CAST IF EXISTS (year AS int1) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.year_int8 (year) CASCADE;

DROP CAST IF EXISTS (int8 AS year) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.int64_year (int8) CASCADE;

DROP CAST IF EXISTS (int2 AS year) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.int16_year (int2) CASCADE;

DROP CAST IF EXISTS (int1 AS year) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.int8_year (int1) CASCADE;

--bit_functions.sql rollback
DROP CAST IF EXISTS (bit AS anyset) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.set(bit, int4) CASCADE; 

DROP CAST IF EXISTS (time as bit) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.bitfromtime(time, int4) CASCADE;

DROP CAST IF EXISTS (timestamptz as bit) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.bitfromtimestamp(timestamptz, int4) CASCADE;

DROP CAST IF EXISTS (timestamp without time zone as bit) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.bitfromdatetime(timestamp without time zone, int4) CASCADE;

DROP CAST IF EXISTS (date as bit) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.bitfromdate(date, int4) CASCADE;

DROP CAST IF EXISTS (float8 as bit) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.bitfromfloat8(float8, int4) CASCADE;

DROP CAST IF EXISTS (float4 as bit) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.bitfromfloat4(float4, int4) CASCADE;

DROP CAST IF EXISTS (numeric as bit) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.bitfromnumeric(numeric, int4) CASCADE;

DROP CAST IF EXISTS (bit AS time) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.bittotime(bit) CASCADE;

DROP CAST IF EXISTS (bit AS timestamptz) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.bittotimestamp(bit) CASCADE;

DROP CAST IF EXISTS (bit AS timestamp without time zone) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.bittodatetime(bit) CASCADE;

DROP CAST IF EXISTS (bit AS date) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.bittodate(bit) CASCADE;

DROP CAST IF EXISTS (bit AS varchar) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.bittovarchar(bit, int4) CASCADE;

DROP CAST IF EXISTS (bit AS char) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.bittochar(bit, int4) CASCADE;

DROP CAST IF EXISTS (bit AS text) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.bittotext(bit) CASCADE;

drop CAST IF EXISTS (bit AS int1) CASCADE;
drop CAST IF EXISTS (bit AS int2) CASCADE;
drop CAST IF EXISTS (int1 AS bit) CASCADE;
drop CAST IF EXISTS (int2 AS bit) CASCADE;

DROP FUNCTION IF EXISTS pg_catalog.bittoint1(bit) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.bittoint2(bit) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.bitfromint1(int1, int4) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.bitfromint2(int2, int4) CASCADE;

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
        DROP FUNCTION IF EXISTS pg_catalog.bit_count(numeric) CASCADE;
        CREATE OR REPLACE FUNCTION pg_catalog.bit_count (numeric)  RETURNS int8 LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'bit_count_numeric';

        DROP FUNCTION IF EXISTS pg_catalog.bit_count(text) CASCADE;
        CREATE OR REPLACE FUNCTION pg_catalog.bit_count (text)  RETURNS int8 LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'bit_count_text';

        DROP FUNCTION IF EXISTS pg_catalog.bit_count_bit(text) CASCADE;
        CREATE OR REPLACE FUNCTION pg_catalog.bit_count_bit (text) RETURNS int8 LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'bit_count_bit';

        DROP FUNCTION IF EXISTS pg_catalog.bit_count(bit) CASCADE;
        CREATE OR REPLACE FUNCTION pg_catalog.bit_count (bit) RETURNS int8 AS $$ SELECT pg_catalog.bit_count_bit(cast($1 as text)) $$ LANGUAGE SQL;

        DROP FUNCTION IF EXISTS pg_catalog.connection_id() CASCADE;
        CREATE OR REPLACE FUNCTION pg_catalog.connection_id()
        RETURNS int8 AS
        $$
        BEGIN
            RETURN (select pg_backend_pid());
        END;
        $$
        LANGUAGE plpgsql;
    end if;
END
$for_og_310$;

DROP FUNCTION IF EXISTS pg_catalog.conv(date, int4, int4) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.conv(timestamp without time zone, int4, int4) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.conv(timestamptz, int4, int4) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.conv(time, int4, int4) CASCADE;

DROP FUNCTION IF EXISTS pg_catalog.oct(date) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.oct(timestamp without time zone) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.oct(timestamptz) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.oct(time) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.oct(text) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.oct(boolean) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.oct(bit) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.oct(numeric) CASCADE;

DROP FUNCTION IF EXISTS pg_catalog.bit_count(date) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.to_base64 (boolean) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.unhex (bit) CASCADE;

DO $for_og_310$
BEGIN
    if working_version_num() > 92780 then
        DROP FUNCTION IF EXISTS pg_catalog.system_user() CASCADE;

        CREATE OR REPLACE FUNCTION pg_catalog.system_user ()
        RETURNS text
        AS
        $$
        BEGIN
            RETURN (SELECT session_user);
        END;
        $$
        LANGUAGE plpgsql;
        DROP FUNCTION IF EXISTS pg_catalog.schema() CASCADE;
        CREATE OR REPLACE FUNCTION pg_catalog.schema ()
        RETURNS text
        AS
        $$
        BEGIN
            RETURN (SELECT CURRENT_SCHEMA());
        END;
        $$
        LANGUAGE plpgsql;
        DROP FUNCTION IF EXISTS pg_catalog.from_base64 (text) CASCADE;
        DROP FUNCTION IF EXISTS pg_catalog.from_base64 (bool) CASCADE;
        DROP FUNCTION IF EXISTS pg_catalog.from_base64 (bit) CASCADE;
        CREATE OR REPLACE FUNCTION pg_catalog.from_base64 (text)  RETURNS text LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'base64_decode';
        
        CREATE OR REPLACE FUNCTION pg_catalog.from_base64 (bool1 bool)
        RETURNS text
        AS
        $$
        BEGIN
            RETURN (SELECT from_base64(''));
        END;
        $$
        LANGUAGE plpgsql;
        
        CREATE OR REPLACE FUNCTION pg_catalog.from_base64(bit1 bit)
        RETURNS text
        AS
        $$
        BEGIN
            RETURN (SELECT from_base64(encode((decode(hex(bit1), 'hex')), 'escape')));
        END;
        $$
        LANGUAGE plpgsql;
        
        DROP FUNCTION IF EXISTS pg_catalog.oct(text) CASCADE;
        DROP FUNCTION IF EXISTS pg_catalog.oct(boolean) CASCADE;
        DROP FUNCTION IF EXISTS pg_catalog.oct(bit) CASCADE;

        CREATE OR REPLACE FUNCTION pg_catalog.oct(t1 text) RETURNS text AS
        $$
        BEGIN
            RETURN (SELECT conv(t1, 10, 8));
        END;
        $$
        LANGUAGE plpgsql;
        
        CREATE OR REPLACE FUNCTION pg_catalog.oct(t1 bit) RETURNS text AS
        $$
        BEGIN
            RETURN (SELECT conv(t1, 10, 8));
        END;
        $$
        LANGUAGE plpgsql;
        
        CREATE OR REPLACE FUNCTION pg_catalog.oct(t1 boolean) RETURNS text AS
        $$
        BEGIN
            RETURN int4(t1);
        END;
        $$
        LANGUAGE plpgsql;
        DROP FUNCTION IF EXISTS pg_catalog.to_base64 (bytea) CASCADE;
        DROP FUNCTION IF EXISTS pg_catalog.to_base64 (numeric) CASCADE;
        DROP FUNCTION IF EXISTS pg_catalog.to_base64 (text) CASCADE;
        DROP FUNCTION IF EXISTS pg_catalog.to_base64 (bit) CASCADE;
        CREATE OR REPLACE FUNCTION pg_catalog.to_base64 (bytea)  RETURNS text LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'base64_encode';

        CREATE OR REPLACE FUNCTION pg_catalog.to_base64 (num1 numeric)
        RETURNS text
        AS
        $$
        BEGIN
            RETURN (SELECT to_base64(cast(to_char(num1) AS bytea)));
        END;
        $$
        LANGUAGE plpgsql;

        CREATE OR REPLACE FUNCTION pg_catalog.to_base64 (t1 text)
        RETURNS text
        AS
        $$
        BEGIN
            RETURN (SELECT to_base64(cast(t1 AS bytea)));
        END;
        $$
        LANGUAGE plpgsql;

        CREATE OR REPLACE FUNCTION pg_catalog.to_base64(bit1 bit)
        RETURNS text
        AS
        $$
        BEGIN
            RETURN (SELECT to_base64((decode(hex(bit1), 'hex'))));
        END;
        $$
        LANGUAGE plpgsql;

        DROP FUNCTION IF EXISTS pg_catalog.unhex (text) CASCADE;
        DROP FUNCTION IF EXISTS pg_catalog.unhex (numeric) CASCADE;
        DROP FUNCTION IF EXISTS pg_catalog.unhex (bool) CASCADE;
        DROP FUNCTION IF EXISTS pg_catalog.unhex (boolean) CASCADE;
        DROP FUNCTION IF EXISTS pg_catalog.unhex (bytea) CASCADE;
        DROP FUNCTION IF EXISTS pg_catalog.unhex ("timestamp") CASCADE;

        CREATE OR REPLACE FUNCTION pg_catalog.unhex (text)  RETURNS text LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'unhex';

        CREATE OR REPLACE FUNCTION pg_catalog.unhex (num1 numeric)
        RETURNS text
        AS
        $$
        BEGIN
            RETURN (SELECT unhex(to_char(num1)));
        END;
        $$
        LANGUAGE plpgsql;

        CREATE OR REPLACE FUNCTION pg_catalog.unhex (bool1 bool)
        RETURNS text
        AS
        $$
        BEGIN
            RETURN (SELECT unhex(cast(bool1 AS numeric)));
        END;
        $$
        LANGUAGE plpgsql;

        CREATE OR REPLACE FUNCTION pg_catalog.unhex (bin1 bytea)
        RETURNS text
        AS
        $$
        BEGIN
            RETURN NULL;
        END;
        $$
        LANGUAGE plpgsql;

        CREATE OR REPLACE FUNCTION pg_catalog.unhex (datetime1 "timestamp")
        RETURNS text
        AS
        $$
        BEGIN
            RETURN NULL;
        END;
        $$
        LANGUAGE plpgsql;
    end if;
END
$for_og_310$;

DROP FUNCTION IF EXISTS pg_catalog.export_set (bit, "any", "any") CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.export_set (bit, "any", "any", "any") CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.export_set (int8, "any", "any", "any", bit) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.export_set (bit, "any", "any", "any", int8) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.export_set (bit, "any", "any", "any", bit) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.export_set (int8, "any", "any", "any", int8) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.export_set (int8, "any", "any", "any") CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.export_set (int8, "any", "any") CASCADE;

DROP FUNCTION IF EXISTS pg_catalog.export_set (numeric, text, text, text, numeric) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.export_set (numeric, text, text, text) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.export_set (numeric, text, text) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.export_set (numeric, text, text, text, numeric) RETURNS text LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'export_set_5args';
CREATE OR REPLACE FUNCTION pg_catalog.export_set (numeric, text, text, text) RETURNS text LANGUAGE C STABLE STRICT  as '$libdir/dolphin', 'export_set_4args';
CREATE OR REPLACE FUNCTION pg_catalog.export_set (numeric, text, text)  RETURNS text LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'export_set_3args';
DROP FUNCTION IF EXISTS pg_catalog.sleep(d date) CASCADE;

DROP FUNCTION IF EXISTS pg_catalog.pg_open_tables() CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.pg_open_tables(TEXT) CASCADE;

DROP FUNCTION IF EXISTS pg_catalog.compress (text) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.compress (bytea) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.compress (boolean) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.compress (bit) CASCADE;

DROP FUNCTION IF EXISTS pg_catalog.uncompress (text) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.uncompress (bytea) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.uncompress (boolean) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.uncompress (bit) CASCADE;

DROP FUNCTION IF EXISTS pg_catalog.uncompressed_length (text) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.uncompressed_length (bytea) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.uncompressed_length (boolean) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.uncompressed_length (bit) CASCADE;

DROP FUNCTION IF EXISTS pg_catalog.weight_string (TEXT, uint4) cascade;
DROP FUNCTION IF EXISTS pg_catalog.weight_string (TEXT, TEXT, uint4) cascade;
DROP FUNCTION IF EXISTS pg_catalog.weight_string (TEXT, TEXT, uint4, uint4) cascade;
DROP FUNCTION IF EXISTS pg_catalog.weight_string (bytea, uint4) cascade;
DROP FUNCTION IF EXISTS pg_catalog.weight_string (bytea, text, uint4) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.weight_string (bytea, text, uint4, uint4) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.weight_string (boolean, uint4) cascade;
DROP FUNCTION IF EXISTS pg_catalog.weight_string (boolean, TEXT, uint4) cascade;
DROP FUNCTION IF EXISTS pg_catalog.weight_string (boolean, TEXT, uint4, uint4) cascade;
DROP FUNCTION IF EXISTS pg_catalog.weight_string (numeric, uint4) cascade;
DROP FUNCTION IF EXISTS pg_catalog.weight_string (numeric, TEXT, uint4) cascade;
DROP FUNCTION IF EXISTS pg_catalog.weight_string (numeric, TEXT, uint4, uint4) cascade;
DROP FUNCTION IF EXISTS pg_catalog.weight_string (numeric, TEXT, TEXT, uint4);
DROP FUNCTION IF EXISTS pg_catalog.weight_string (date, uint4) cascade;
DROP FUNCTION IF EXISTS pg_catalog.weight_string (date, TEXT, uint4) cascade;
DROP FUNCTION IF EXISTS pg_catalog.weight_string (date, TEXT, uint4, uint4) cascade;
DROP FUNCTION IF EXISTS pg_catalog.weight_string (time, uint4) cascade;
DROP FUNCTION IF EXISTS pg_catalog.weight_string (time, TEXT, uint4) cascade;
DROP FUNCTION IF EXISTS pg_catalog.weight_string (time, TEXT, uint4, uint4) cascade;
DROP FUNCTION IF EXISTS pg_catalog.weight_string (timestamp without time zone, uint4) cascade;
DROP FUNCTION IF EXISTS pg_catalog.weight_string (timestamp without time zone, TEXT, uint4) cascade;
DROP FUNCTION IF EXISTS pg_catalog.weight_string (timestamp without time zone, TEXT, uint4, uint4) cascade;
DROP FUNCTION IF EXISTS pg_catalog.weight_string (timestamptz, uint4) cascade;
DROP FUNCTION IF EXISTS pg_catalog.weight_string (timestamptz, TEXT, uint4) cascade;
DROP FUNCTION IF EXISTS pg_catalog.weight_string (timestamptz, TEXT, uint4, uint4) cascade;
DROP FUNCTION IF EXISTS pg_catalog.weight_string (interval, uint4) cascade;
DROP FUNCTION IF EXISTS pg_catalog.weight_string (interval, TEXT, uint4) cascade;
DROP FUNCTION IF EXISTS pg_catalog.weight_string (interval, TEXT, uint4, uint4) cascade;

DROP FUNCTION IF EXISTS pg_catalog.sleep(d date) CASCADE;

DO $for_og_310$
BEGIN
    if working_version_num() > 92780 then
        CREATE OR REPLACE FUNCTION pg_catalog.text_xor (t1 uint8,t2 text) RETURNS uint8 AS
        $$
        DECLARE num NUMBER := to_number(t2);
        BEGIN
            IF num > 9223372036854775807 then
                num = 9223372036854775807;
            ELSEIF num < -9223372036854775808 then
                num = 9223372036854775808;
            END IF;
            RETURN (SELECT uint8_xor(t1, num));
        END;
        $$
        LANGUAGE plpgsql;
        drop aggregate if exists pg_catalog.bit_xor(text);
        create aggregate pg_catalog.bit_xor(text) (SFUNC=text_xor, STYPE= uint8);
    end if;
END
$for_og_310$;

drop view performance_schema.events_statements_current;
drop view performance_schema.events_statements_history;
drop view performance_schema.events_statements_summary_by_digest;
drop view performance_schema.statement_history;
drop view performance_schema.events_waits_current;
drop view performance_schema.events_waits_summary_global_by_event_name;
drop view performance_schema.file_summary_by_instance;
drop view performance_schema.table_io_waits_summary_by_table;
drop view performance_schema.table_io_waits_summary_by_index_usage;
drop schema if exists performance_schema CASCADE;
drop function if exists pg_catalog.get_statement_history();

drop operator IF EXISTS pg_catalog./(int1, int1);
drop operator IF EXISTS pg_catalog.-(NONE, int1);
DO $for_og_310$
BEGIN
    if working_version_num() > 92780 then
        create operator pg_catalog./(leftarg = int1, rightarg = int1, procedure = pg_catalog.int1div);
        COMMENT ON OPERATOR pg_catalog./(int1, int1) IS 'int1div';

        CREATE OPERATOR pg_catalog.-(rightarg = int1, procedure = pg_catalog.int1um);
    end if;
END
$for_og_310$;

-- left operator
DROP FUNCTION IF EXISTS dolphin_catalog.json_uplus(json);
DROP OPERATOR IF EXISTS dolphin_catalog.-(json);

-- binary cmp operator
DROP FUNCTION IF EXISTS dolphin_catalog.json_null_save_eq(json, "any");
DROP OPERATOR IF EXISTS dolphin_catalog.<=>(json, "any");
DROP FUNCTION IF EXISTS dolphin_catalog.json_null_save_eq(text, json);
DROP OPERATOR IF EXISTS dolphin_catalog.<=>(text, json);

DROP FUNCTION IF EXISTS dolphin_catalog.json_eq(json, "any");
DROP OPERATOR IF EXISTS dolphin_catalog.=(json, "any");
DROP FUNCTION IF EXISTS dolphin_catalog.json_eq(text, json);
DROP OPERATOR IF EXISTS dolphin_catalog.=(text, json);
DROP FUNCTION IF EXISTS dolphin_catalog.json_eq(bit, json);
DROP OPERATOR IF EXISTS dolphin_catalog.=(bit, json);
DROP FUNCTION IF EXISTS dolphin_catalog.json_eq(year, json);
DROP OPERATOR IF EXISTS dolphin_catalog.=(year, json);

DROP FUNCTION IF EXISTS dolphin_catalog.json_ne(json, "any");
DROP OPERATOR IF EXISTS dolphin_catalog.!=(json, "any");
DROP FUNCTION IF EXISTS dolphin_catalog.json_ne(text, json);
DROP OPERATOR IF EXISTS dolphin_catalog.!=(text, json);
DROP FUNCTION IF EXISTS dolphin_catalog.json_ne(bit, json);
DROP OPERATOR IF EXISTS dolphin_catalog.!=(bit, json);
DROP FUNCTION IF EXISTS dolphin_catalog.json_ne(year, json);
DROP OPERATOR IF EXISTS dolphin_catalog.!=(year, json);

DROP FUNCTION IF EXISTS dolphin_catalog.json_gt(json, "any");
DROP OPERATOR IF EXISTS dolphin_catalog.>(json, "any");
DROP FUNCTION IF EXISTS dolphin_catalog.json_gt(text, json);
DROP OPERATOR IF EXISTS dolphin_catalog.>(text, json);
DROP FUNCTION IF EXISTS dolphin_catalog.json_gt(bit, json);
DROP OPERATOR IF EXISTS dolphin_catalog.>(bit, json);
DROP FUNCTION IF EXISTS dolphin_catalog.json_gt(year, json);
DROP OPERATOR IF EXISTS dolphin_catalog.>(year, json);

DROP FUNCTION IF EXISTS dolphin_catalog.json_ge(json, "any");
DROP OPERATOR IF EXISTS dolphin_catalog.>=(json, "any");
DROP FUNCTION IF EXISTS dolphin_catalog.json_ge(text, json);
DROP OPERATOR IF EXISTS dolphin_catalog.>=(text, json);
DROP FUNCTION IF EXISTS dolphin_catalog.json_ge(bit, json);
DROP OPERATOR IF EXISTS dolphin_catalog.>=(bit, json);
DROP FUNCTION IF EXISTS dolphin_catalog.json_ge(year, json);
DROP OPERATOR IF EXISTS dolphin_catalog.>=(year, json);

DROP FUNCTION IF EXISTS dolphin_catalog.json_lt(json, "any");
DROP OPERATOR IF EXISTS dolphin_catalog.<(json, "any");
DROP FUNCTION IF EXISTS dolphin_catalog.json_lt(text, json);
DROP OPERATOR IF EXISTS dolphin_catalog.<(text, json);
DROP FUNCTION IF EXISTS dolphin_catalog.json_lt(bit, json);
DROP OPERATOR IF EXISTS dolphin_catalog.<(bit, json);
DROP FUNCTION IF EXISTS dolphin_catalog.json_lt(year, json);
DROP OPERATOR IF EXISTS dolphin_catalog.<(year, json);

DROP FUNCTION IF EXISTS dolphin_catalog.json_le(json, "any");
DROP OPERATOR IF EXISTS dolphin_catalog.<=(json, "any");
DROP FUNCTION IF EXISTS dolphin_catalog.json_le(text, json);
DROP OPERATOR IF EXISTS dolphin_catalog.<=(text, json);
DROP FUNCTION IF EXISTS dolphin_catalog.json_le(bit, json);
DROP OPERATOR IF EXISTS dolphin_catalog.<=(bit, json);
DROP FUNCTION IF EXISTS dolphin_catalog.json_le(year, json);
DROP OPERATOR IF EXISTS dolphin_catalog.<=(year, json);

DROP FUNCTION IF EXISTS pg_catalog.b_mod(json, anyelement);
DROP FUNCTION IF EXISTS pg_catalog.b_mod(anyelement, json);
DROP FUNCTION IF EXISTS pg_catalog.b_mod(json, json);

DROP FUNCTION IF EXISTS pg_catalog.div(json, anyelement);
DROP FUNCTION IF EXISTS pg_catalog.div(anyelement, json);
DROP FUNCTION IF EXISTS pg_catalog.div(json, json);

DROP FUNCTION IF EXISTS pg_catalog.xor(json, anyelement);
DROP FUNCTION IF EXISTS pg_catalog.xor(anyelement, json);
DROP FUNCTION IF EXISTS pg_catalog.xor(json, json);
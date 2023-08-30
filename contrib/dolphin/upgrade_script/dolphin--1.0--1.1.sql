DO $for_og_310$
DECLARE
  ans boolean;
  v_isinplaceupgrade boolean;
BEGIN
    -- add special script for version before 3.1.0, cause 3.1.0 is dolphin 1.0 and 5.0.0 is dolphin 1.0 too, but they are different.
    select case when count(*)=1 then true else false end as ans from (select setting from pg_settings where name = 'upgrade_mode' and setting != '0') into ans;
    show isinplaceupgrade into v_isinplaceupgrade;
    if working_version_num() <= 92780 and ans and v_isinplaceupgrade then
        DROP CAST IF EXISTS (float8 as boolean);
        DROP CAST IF EXISTS (float as boolean);
        DROP FUNCTION IF EXISTS pg_catalog.float8_bool(float8);
        DROP FUNCTION IF EXISTS pg_catalog.float8_bool(float);

        CREATE OR REPLACE FUNCTION pg_catalog.float8_bool(float8) returns boolean LANGUAGE C immutable strict as '$libdir/dolphin', 'float8_bool';
        CREATE CAST (float8 as boolean) WITH FUNCTION float8_bool(float8) AS IMPLICIT;

        DROP CAST IF EXISTS (float4 as boolean);
        DROP FUNCTION IF EXISTS pg_catalog.float4_bool(float4);
        CREATE OR REPLACE FUNCTION pg_catalog.float4_bool(float4) returns boolean LANGUAGE C immutable strict as '$libdir/dolphin', 'float4_bool';
        CREATE CAST (float4 as boolean) WITH FUNCTION float4_bool(float4) AS IMPLICIT;
        
        DROP CAST IF EXISTS (date as boolean);
        DROP FUNCTION IF EXISTS pg_catalog.date_bool(float8);
        CREATE OR REPLACE FUNCTION pg_catalog.date_bool(date) returns boolean LANGUAGE C immutable strict as '$libdir/dolphin', 'date_bool';
        CREATE CAST (date as boolean) WITH FUNCTION date_bool(date) AS IMPLICIT;
        
        DROP CAST IF EXISTS (time as boolean);
        DROP FUNCTION IF EXISTS pg_catalog.time_bool(time);
        CREATE OR REPLACE FUNCTION pg_catalog.time_bool(time) returns boolean LANGUAGE C immutable strict as '$libdir/dolphin', 'time_bool';
        CREATE CAST (time as boolean) WITH FUNCTION time_bool(time) AS IMPLICIT;
        
        DROP CAST IF EXISTS (bit as boolean);
        DROP FUNCTION IF EXISTS pg_catalog.bit_bool(bit);
        CREATE OR REPLACE FUNCTION pg_catalog.bit_bool(bit) returns boolean LANGUAGE C immutable strict as '$libdir/dolphin', 'bit_bool';
        CREATE CAST (bit as boolean) WITH FUNCTION bit_bool(bit) AS IMPLICIT;

        DROP CAST IF EXISTS (text as boolean);
        DROP FUNCTION IF EXISTS pg_catalog.text_bool(text);
        CREATE OR REPLACE FUNCTION pg_catalog.text_bool(text) returns boolean LANGUAGE C immutable strict as '$libdir/dolphin', 'text_bool';
        CREATE CAST (text as boolean) WITH FUNCTION text_bool(text) AS IMPLICIT;

        DROP CAST IF EXISTS (varchar as boolean);
        DROP FUNCTION IF EXISTS pg_catalog.varchar_bool(varchar);
        CREATE OR REPLACE FUNCTION pg_catalog.varchar_bool(varchar) returns boolean LANGUAGE C immutable strict as '$libdir/dolphin', 'varchar_bool';
        CREATE CAST (varchar as boolean) WITH FUNCTION varchar_bool(varchar) AS IMPLICIT;

        DROP CAST IF EXISTS (char as boolean);
        DROP FUNCTION IF EXISTS pg_catalog.char_bool(char);
        CREATE OR REPLACE FUNCTION pg_catalog.char_bool(char) returns boolean LANGUAGE C immutable strict as '$libdir/dolphin', 'char_bool';
        CREATE CAST (char as boolean) WITH FUNCTION char_bool(char) AS IMPLICIT;

        CREATE OR REPLACE FUNCTION pg_catalog.boolboollike(
        bool,
        bool
        ) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'boolboollike';

        CREATE OPERATOR pg_catalog.~~(leftarg = bool, rightarg = bool, procedure = pg_catalog.boolboollike);
        CREATE OPERATOR pg_catalog.~~*(leftarg = bool, rightarg = bool, procedure = pg_catalog.boolboollike);

        CREATE OR REPLACE FUNCTION pg_catalog.booltextlike(
        bool,
        text
        ) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'booltextlike';

        CREATE OPERATOR pg_catalog.~~(leftarg = bool, rightarg = text, procedure = pg_catalog.booltextlike);
        CREATE OPERATOR pg_catalog.~~*(leftarg = bool, rightarg = text, procedure = pg_catalog.booltextlike);

        CREATE OR REPLACE FUNCTION pg_catalog.textboollike(
        text,
        bool
        ) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'textboollike';

        CREATE OPERATOR pg_catalog.~~(leftarg = text, rightarg = bool, procedure = pg_catalog.textboollike);
        CREATE OPERATOR pg_catalog.~~*(leftarg = text, rightarg = bool, procedure = pg_catalog.textboollike);

        CREATE OR REPLACE FUNCTION pg_catalog.boolboolnlike(
        bool,
        bool
        ) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'boolboolnlike';

        CREATE OPERATOR pg_catalog.!~~(leftarg = bool, rightarg = bool, procedure = pg_catalog.boolboolnlike);
        CREATE OPERATOR pg_catalog.!~~*(leftarg = bool, rightarg = bool, procedure = pg_catalog.boolboolnlike);

        CREATE OR REPLACE FUNCTION pg_catalog.booltextnlike(
        bool,
        text
        ) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'booltextnlike';

        CREATE OPERATOR pg_catalog.!~~(leftarg = bool, rightarg = text, procedure = pg_catalog.booltextnlike);
        CREATE OPERATOR pg_catalog.!~~*(leftarg = bool, rightarg = text, procedure = pg_catalog.booltextnlike);

        CREATE OR REPLACE FUNCTION pg_catalog.textboolnlike(
        text,
        bool
        ) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'textboolnlike';

        CREATE OPERATOR pg_catalog.!~~(leftarg = text, rightarg = bool, procedure = pg_catalog.textboolnlike);
        CREATE OPERATOR pg_catalog.!~~*(leftarg = text, rightarg = bool, procedure = pg_catalog.textboolnlike);

        CREATE OR REPLACE FUNCTION pg_catalog.bitlike(
        VarBit,
        VarBit
        ) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'bitlike';

        CREATE OR REPLACE FUNCTION pg_catalog.bitnlike(
        VarBit,
        VarBit
        ) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'bitnlike';

        CREATE OPERATOR pg_catalog.!~~(leftarg=VarBit, rightarg=VarBit, procedure=pg_catalog.bitnlike);
        CREATE OPERATOR pg_catalog.!~~*(leftarg=VarBit, rightarg=VarBit, procedure=pg_catalog.bitnlike);

        CREATE OPERATOR pg_catalog.~~(leftarg=VarBit, rightarg=VarBit, procedure=pg_catalog.bitlike);
        CREATE OPERATOR pg_catalog.~~*(leftarg=VarBit, rightarg=VarBit, procedure=pg_catalog.bitlike);


        CREATE OR REPLACE FUNCTION pg_catalog.bitothernlike(
        VarBit,
        bool
        ) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'bitothernlike';

        CREATE OPERATOR pg_catalog.!~~(leftarg=VarBit, rightarg=bool, procedure=pg_catalog.bitothernlike);
        CREATE OPERATOR pg_catalog.!~~*(leftarg=VarBit, rightarg=bool, procedure=pg_catalog.bitothernlike);

        CREATE OR REPLACE FUNCTION pg_catalog.bitothern2like(
        bool,
        VarBit
        ) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'bitothernlike';

        CREATE OPERATOR pg_catalog.!~~(leftarg=bool, rightarg=VarBit, procedure=pg_catalog.bitothern2like);
        CREATE OPERATOR pg_catalog.!~~*(leftarg=bool, rightarg=VarBit, procedure=pg_catalog.bitothern2like);

        CREATE OR REPLACE FUNCTION pg_catalog.bitothern3like(
        VarBit,
        text
        ) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'bitothernlike';

        CREATE OPERATOR pg_catalog.!~~(leftarg=VarBit, rightarg=text, procedure=pg_catalog.bitothern3like);
        CREATE OPERATOR pg_catalog.!~~*(leftarg=VarBit, rightarg=text, procedure=pg_catalog.bitothern3like);

        CREATE OR REPLACE FUNCTION pg_catalog.bitothern4like(
        text,
        VarBit
        ) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'bitothernlike';

        CREATE OPERATOR pg_catalog.!~~(leftarg=text, rightarg=VarBit, procedure=pg_catalog.bitothern4like);
        CREATE OPERATOR pg_catalog.!~~*(leftarg=text, rightarg=VarBit, procedure=pg_catalog.bitothern4like);

        CREATE OR REPLACE FUNCTION pg_catalog.bitothern5like(
        VarBit,
        bytea
        ) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'bitothernlike';

        CREATE OPERATOR pg_catalog.!~~(leftarg=VarBit, rightarg=bytea, procedure=pg_catalog.bitothern5like);
        CREATE OPERATOR pg_catalog.!~~*(leftarg=VarBit, rightarg=bytea, procedure=pg_catalog.bitothern5like);

        CREATE OR REPLACE FUNCTION pg_catalog.bitothern6like(
        bytea,
        VarBit
        ) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'bitothernlike';

        CREATE OPERATOR pg_catalog.!~~(leftarg=bytea, rightarg=VarBit, procedure=pg_catalog.bitothern6like);
        CREATE OPERATOR pg_catalog.!~~*(leftarg=bytea, rightarg=VarBit, procedure=pg_catalog.bitothern6like);

        CREATE OR REPLACE FUNCTION pg_catalog.bitothern7like(
        VarBit,
        blob
        ) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'bitothernlike';

        CREATE OPERATOR pg_catalog.!~~(leftarg=VarBit, rightarg=blob, procedure=pg_catalog.bitothern7like);
        CREATE OPERATOR pg_catalog.!~~*(leftarg=VarBit, rightarg=blob, procedure=pg_catalog.bitothern7like);

        CREATE OR REPLACE FUNCTION pg_catalog.bitothern8like(
        blob,
        VarBit
        ) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'bitothernlike';

        CREATE OPERATOR pg_catalog.!~~(leftarg=blob, rightarg=VarBit, procedure=pg_catalog.bitothern8like);
        CREATE OPERATOR pg_catalog.!~~*(leftarg=blob, rightarg=VarBit, procedure=pg_catalog.bitothern8like);

        CREATE OR REPLACE FUNCTION pg_catalog.bitotherlike(
        VarBit,
        bool
        ) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'bitotherlike';

        CREATE OPERATOR pg_catalog.~~(leftarg=VarBit, rightarg=bool, procedure=pg_catalog.bitotherlike);
        CREATE OPERATOR pg_catalog.~~*(leftarg=VarBit, rightarg=bool, procedure=pg_catalog.bitotherlike);

        CREATE OR REPLACE FUNCTION pg_catalog.bitother2like(
        bool,
        VarBit
        ) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'bitotherlike';

        CREATE OPERATOR pg_catalog.~~(leftarg=bool, rightarg=VarBit, procedure=pg_catalog.bitother2like);
        CREATE OPERATOR pg_catalog.~~*(leftarg=bool, rightarg=VarBit, procedure=pg_catalog.bitother2like);

        CREATE OR REPLACE FUNCTION pg_catalog.bitother3like(
        VarBit,
        text
        ) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'bitotherlike';

        CREATE OPERATOR pg_catalog.~~(leftarg=VarBit, rightarg=text, procedure=pg_catalog.bitother3like);
        CREATE OPERATOR pg_catalog.~~*(leftarg=VarBit, rightarg=text, procedure=pg_catalog.bitother3like);

        CREATE OR REPLACE FUNCTION pg_catalog.bitother4like(
        text,
        VarBit
        ) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'bitotherlike';

        CREATE OPERATOR pg_catalog.~~(leftarg=text, rightarg=VarBit, procedure=pg_catalog.bitother4like);
        CREATE OPERATOR pg_catalog.~~*(leftarg=text, rightarg=VarBit, procedure=pg_catalog.bitother4like);

        CREATE OR REPLACE FUNCTION pg_catalog.bitother5like(
        VarBit,
        bytea
        ) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'bitotherlike';

        CREATE OPERATOR pg_catalog.~~(leftarg=VarBit, rightarg=bytea, procedure=pg_catalog.bitother5like);
        CREATE OPERATOR pg_catalog.~~*(leftarg=VarBit, rightarg=bytea, procedure=pg_catalog.bitother5like);

        CREATE OR REPLACE FUNCTION pg_catalog.bitother6like(
        bytea,
        VarBit
        ) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'bitotherlike';

        CREATE OPERATOR pg_catalog.~~(leftarg=bytea, rightarg=VarBit, procedure=pg_catalog.bitother6like);
        CREATE OPERATOR pg_catalog.~~*(leftarg=bytea, rightarg=VarBit, procedure=pg_catalog.bitother6like);

        CREATE OR REPLACE FUNCTION pg_catalog.bitother7like(
        VarBit,
        blob
        ) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'bitotherlike';

        CREATE OPERATOR pg_catalog.~~(leftarg=VarBit, rightarg=blob, procedure=pg_catalog.bitother7like);
        CREATE OPERATOR pg_catalog.~~*(leftarg=VarBit, rightarg=blob, procedure=pg_catalog.bitother7like);

        CREATE OR REPLACE FUNCTION pg_catalog.bitother8like(
        blob,
        VarBit
        ) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'bitotherlike';

        CREATE OPERATOR pg_catalog.~~(leftarg=blob, rightarg=VarBit, procedure=pg_catalog.bitother8like);
        CREATE OPERATOR pg_catalog.~~*(leftarg=blob, rightarg=VarBit, procedure=pg_catalog.bitother8like);

        CREATE OPERATOR pg_catalog.~~*(leftarg = bytea, rightarg = bytea, procedure = pg_catalog.bytealike);
        CREATE OPERATOR pg_catalog.!~~*(leftarg = bytea, rightarg = bytea, procedure = pg_catalog.byteanlike);

        CREATE OPERATOR pg_catalog.~~*(leftarg=raw, rightarg=raw, procedure=pg_catalog.rawlike);
        CREATE OPERATOR pg_catalog.!~~*(leftarg=raw, rightarg=raw, procedure=pg_catalog.rawnlike);

        DROP FUNCTION IF EXISTS pg_catalog.b_between_and("any","any","any");
        CREATE OR REPLACE FUNCTION pg_catalog.b_between_and("any","any","any") returns boolean LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'between_and';

        DROP FUNCTION IF EXISTS pg_catalog.b_sym_between_and("any","any","any");
        CREATE OR REPLACE FUNCTION pg_catalog.b_sym_between_and("any","any","any") returns boolean LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'sym_between_and';

        DROP FUNCTION IF EXISTS pg_catalog.b_not_between_and("any","any","any");
        CREATE OR REPLACE FUNCTION pg_catalog.b_not_between_and("any","any","any") returns boolean LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'not_between_and';

        DROP FUNCTION IF EXISTS pg_catalog.b_not_sym_between_and("any","any","any");
        CREATE OR REPLACE FUNCTION pg_catalog.b_not_sym_between_and("any","any","any") returns boolean LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'not_sym_between_and';

        CREATE OR REPLACE FUNCTION pg_catalog.dolphin_version() RETURNS text AS '$libdir/dolphin','dolphin_version' LANGUAGE C IMMUTABLE STRICT;
        
        DROP FUNCTION IF EXISTS pg_catalog.xor(a integer, b integer);
        CREATE OR REPLACE FUNCTION pg_catalog.xor(a integer, b integer)
        returns integer
        as
        $$
        begin
          return (select int4xor(a::bool::integer, b::bool::integer));
        end;
        $$
        language plpgsql;
        
        DROP FUNCTION IF EXISTS pg_catalog.bpchar_text(bpchar);
        CREATE OR REPLACE FUNCTION pg_catalog.bpchar_text (
        bpchar
        ) RETURNS text LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'bpchar_text';
        
        update pg_catalog.pg_cast set castfunc = (select oid from pg_proc where proname = 'bpchar_text'), castowner = 10 where castsource = 1042 and casttarget = 25;
        update pg_catalog.pg_cast set castfunc = (select oid from pg_proc where proname = 'bpchar_text'), castowner = 10 where castsource = 1042 and casttarget = 1043;
        update pg_catalog.pg_cast set castfunc = (select oid from pg_proc where proname = 'bpchar_text'), castowner = 10 where castsource = 1042 and casttarget = 3969;
        
        DROP FUNCTION IF EXISTS pg_catalog.bigint_any_value (bigint, bigint);
        CREATE OR REPLACE FUNCTION pg_catalog.bigint_any_value (bigint, bigint) RETURNS bigint LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'bigint_any_value';

        drop aggregate if exists pg_catalog.any_value(bigint);
        CREATE AGGREGATE pg_catalog.any_value(bigint) (
                sfunc = bigint_any_value,
                stype = int8
        );

        DROP FUNCTION IF EXISTS pg_catalog.numeric_any_value (numeric, numeric);
        CREATE OR REPLACE FUNCTION pg_catalog.numeric_any_value (numeric, numeric) RETURNS numeric LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'numeric_any_value';

        drop aggregate if exists pg_catalog.any_value(numeric);
        CREATE AGGREGATE pg_catalog.any_value(numeric) (
                sfunc = numeric_any_value,
                stype = numeric
        );

        DROP FUNCTION IF EXISTS pg_catalog.double_any_value (double precision, double precision);
        CREATE OR REPLACE FUNCTION pg_catalog.double_any_value (double precision, double precision) RETURNS double precision LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'double_any_value';

        drop aggregate if exists pg_catalog.any_value(double precision);
        CREATE AGGREGATE pg_catalog.any_value(double precision) (
                sfunc = double_any_value,
                stype = double precision
        );

        DROP FUNCTION IF EXISTS pg_catalog.float_any_value (float4, float4);
        CREATE OR REPLACE FUNCTION pg_catalog.float_any_value (float4, float4) RETURNS float4 LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'float_any_value';

        drop aggregate if exists pg_catalog.any_value(float4);
        CREATE AGGREGATE pg_catalog.any_value(float4) (
                sfunc = float_any_value,
                stype = float4
        );

        DROP FUNCTION IF EXISTS pg_catalog.text_any_value (text, text);
        CREATE OR REPLACE FUNCTION pg_catalog.text_any_value (text, text) RETURNS text LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'text_any_value';

        drop aggregate if exists pg_catalog.any_value(text);
        CREATE AGGREGATE pg_catalog.any_value(text) (
                sfunc = text_any_value,
                stype = text
        );

        DROP FUNCTION IF EXISTS pg_catalog.bytea_any_value (bytea, bytea);
        CREATE OR REPLACE FUNCTION pg_catalog.bytea_any_value (bytea, bytea) RETURNS bytea LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'bytea_any_value';

        drop aggregate if exists pg_catalog.any_value(bytea);
        CREATE AGGREGATE pg_catalog.any_value(bytea) (
                sfunc = bytea_any_value,
                stype = bytea
        );

        DROP FUNCTION IF EXISTS pg_catalog.blob_any_value (blob, blob);
        CREATE OR REPLACE FUNCTION pg_catalog.blob_any_value (blob, blob) RETURNS blob LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'blob_any_value';

        drop aggregate if exists pg_catalog.any_value(blob);
        CREATE AGGREGATE pg_catalog.any_value(blob) (
                sfunc = blob_any_value,
                stype = blob
        );

        DROP FUNCTION IF EXISTS pg_catalog.dolphin_attname_eq(name, name);
        CREATE OR REPLACE FUNCTION pg_catalog.dolphin_attname_eq(name, name) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'dolphin_attname_eq';

        CREATE OR REPLACE FUNCTION pg_catalog.binary_in (
        cstring
        ) RETURNS binary  LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'dolphin_binaryin';
        
        CREATE OR REPLACE FUNCTION pg_catalog.binary_in (
        text
        ) RETURNS binary   AS
        $$
        BEGIN
            RETURN (SELECT binary_in($1::cstring));
        END;
        $$
        LANGUAGE plpgsql;

        CREATE OR REPLACE FUNCTION pg_catalog.binarytextlike(
        binary,
        text
        ) RETURNS bool AS
        $$
        BEGIN
            RETURN (SELECT bytealike($1::bytea,$2::binary::bytea));
        END;
        $$
        LANGUAGE plpgsql;

        CREATE OPERATOR pg_catalog.~~(leftarg = binary, rightarg = text, procedure = pg_catalog.binarytextlike);
        CREATE OPERATOR pg_catalog.~~*(leftarg = binary, rightarg = text, procedure = pg_catalog.binarytextlike);

        CREATE OR REPLACE FUNCTION pg_catalog.textbinarylike(
        text,
        binary
        ) RETURNS bool AS
        $$
        BEGIN
            RETURN (SELECT bytealike($1::binary::bytea,$2::bytea));
        END;
        $$
        LANGUAGE plpgsql;

        CREATE OPERATOR pg_catalog.~~(leftarg = text, rightarg = binary, procedure = pg_catalog.textbinarylike);
        CREATE OPERATOR pg_catalog.~~*(leftarg = text, rightarg = binary, procedure = pg_catalog.textbinarylike);

        CREATE OR REPLACE FUNCTION pg_catalog.binarytextnlike(
        binary,
        text
        ) RETURNS bool AS
        $$
        BEGIN
            RETURN (SELECT byteanlike($1::bytea,$2::binary::bytea));
        END;
        $$
        LANGUAGE plpgsql;

        CREATE OPERATOR pg_catalog.!~~(leftarg = binary, rightarg = text, procedure = pg_catalog.binarytextnlike);
        CREATE OPERATOR pg_catalog.!~~*(leftarg = binary, rightarg = text, procedure = pg_catalog.binarytextnlike);

        CREATE OR REPLACE FUNCTION pg_catalog.textbinarynlike(
        text,
        binary
        ) RETURNS bool AS
        $$
        BEGIN
            RETURN (SELECT byteanlike($1::binary::bytea,$2::bytea));
        END;
        $$
        LANGUAGE plpgsql;

        CREATE OPERATOR pg_catalog.!~~(leftarg = text, rightarg = binary, procedure = pg_catalog.textbinarynlike);
        CREATE OPERATOR pg_catalog.!~~*(leftarg = text, rightarg = binary, procedure = pg_catalog.textbinarynlike);

        CREATE OR REPLACE FUNCTION pg_catalog.like_escape(
        binary,
        text
        ) RETURNS bytea AS
        $$
        BEGIN
            RETURN (SELECT like_escape($1::bytea,$2::binary::bytea));
        END;
        $$
        LANGUAGE plpgsql;

        CREATE OR REPLACE FUNCTION pg_catalog.binaryeq(
        binary,
        binary
        ) RETURNS bool AS
        $$
        BEGIN
            RETURN (SELECT byteaeq($1::bytea,$2::bytea));
        END;
        $$
        LANGUAGE plpgsql;

        CREATE OPERATOR pg_catalog.=(leftarg = binary, rightarg = binary, procedure = pg_catalog.binaryeq,restrict = eqsel, join = eqjoinsel,
        HASHES, MERGES);

        CREATE OR REPLACE FUNCTION pg_catalog.binaryne(
        binary,
        binary
        ) RETURNS bool AS
        $$
        BEGIN
            RETURN (SELECT byteane($1::bytea,$2::bytea));
        END;
        $$
        LANGUAGE plpgsql;

        CREATE OPERATOR pg_catalog.<>(leftarg = binary, rightarg = binary, procedure = pg_catalog.binaryne,restrict = neqsel, join = neqjoinsel);


        CREATE OR REPLACE FUNCTION pg_catalog.binarygt(
        binary,
        binary
        ) RETURNS bool AS
        $$
        BEGIN
            RETURN (SELECT byteagt($1::bytea,$2::bytea));
        END;
        $$
        LANGUAGE plpgsql;

        CREATE OPERATOR pg_catalog.>(leftarg = binary, rightarg = binary, procedure = pg_catalog.binarygt,restrict = scalargtsel, join = scalargtjoinsel);

        CREATE OR REPLACE FUNCTION pg_catalog.binarylt(
        binary,
        binary
        ) RETURNS bool AS
        $$
        BEGIN
            RETURN (SELECT bytealt($1::bytea,$2::bytea));
        END;
        $$
        LANGUAGE plpgsql;

        CREATE OPERATOR pg_catalog.<(leftarg = binary, rightarg = binary, procedure = pg_catalog.binarylt,restrict = scalarltsel, join = scalarltjoinsel);

        CREATE OR REPLACE FUNCTION pg_catalog.binaryge(
        binary,
        binary
        ) RETURNS bool AS
        $$
        BEGIN
            RETURN (SELECT byteage($1::bytea,$2::bytea));
        END;
        $$
        LANGUAGE plpgsql;

        CREATE OPERATOR pg_catalog.>=(leftarg = binary, rightarg = binary, procedure = pg_catalog.binaryge,restrict = scalargtsel, join = scalargtjoinsel);

        CREATE OR REPLACE FUNCTION pg_catalog.binaryle(
        binary,
        binary
        ) RETURNS bool AS
        $$
        BEGIN
            RETURN (SELECT byteale($1::bytea,$2::bytea));
        END;
        $$
        LANGUAGE plpgsql;

        CREATE OPERATOR pg_catalog.<=(leftarg = binary, rightarg = binary, procedure = pg_catalog.binaryle,restrict = scalarltsel, join = scalarltjoinsel);
        
        CREATE OR REPLACE FUNCTION pg_catalog.text_varbinary_eq(
        text,
        varbinary
        ) RETURNS bool AS
        $$
        BEGIN
            RETURN (SELECT byteaeq($1::bytea,$2::bytea));
        END;
        $$
        LANGUAGE plpgsql;

        CREATE OPERATOR pg_catalog.=(leftarg = text, rightarg = varbinary, procedure = pg_catalog.text_varbinary_eq,restrict = eqsel, join = eqjoinsel,
        HASHES, MERGES);

        CREATE OR REPLACE FUNCTION pg_catalog.text_varbinary_ne(
        text,
        varbinary
        ) RETURNS bool AS
        $$
        BEGIN
            RETURN (SELECT byteane($1::bytea,$2::bytea));
        END;
        $$
        LANGUAGE plpgsql;

        CREATE OPERATOR pg_catalog.<>(leftarg = text, rightarg = varbinary, procedure = pg_catalog.text_varbinary_ne,restrict = neqsel, join = neqjoinsel);

        CREATE OR REPLACE FUNCTION pg_catalog.text_varbinary_gt(
        text,
        varbinary
        ) RETURNS bool AS
        $$
        BEGIN
            RETURN (SELECT byteagt($1::bytea,$2::bytea));
        END;
        $$
        LANGUAGE plpgsql;

        CREATE OPERATOR pg_catalog.>(leftarg = text, rightarg = varbinary, procedure = pg_catalog.text_varbinary_gt,restrict = scalargtsel, join = scalargtjoinsel);


        CREATE OR REPLACE FUNCTION pg_catalog.text_varbinary_lt(
        text,
        varbinary
        ) RETURNS bool AS
        $$
        BEGIN
            RETURN (SELECT bytealt($1::bytea,$2::bytea));
        END;
        $$
        LANGUAGE plpgsql;

        CREATE OPERATOR pg_catalog.<(leftarg = text, rightarg = varbinary, procedure = pg_catalog.text_varbinary_lt,restrict = scalarltsel, join = scalarltjoinsel);

        CREATE OR REPLACE FUNCTION pg_catalog.text_varbinary_ge(
        text,
        varbinary
        ) RETURNS bool AS
        $$
        BEGIN
            RETURN (SELECT byteage($1::bytea,$2::bytea));
        END;
        $$
        LANGUAGE plpgsql;

        CREATE OPERATOR pg_catalog.>=(leftarg = text, rightarg = varbinary, procedure = pg_catalog.text_varbinary_ge,restrict = scalargtsel, join = scalargtjoinsel);

        CREATE OR REPLACE FUNCTION pg_catalog.text_varbinary_le(
        text,
        varbinary
        ) RETURNS bool AS
        $$
        BEGIN
            RETURN (SELECT byteale($1::bytea,$2::bytea));
        END;
        $$
        LANGUAGE plpgsql;

        CREATE OPERATOR pg_catalog.<=(leftarg = text, rightarg = varbinary, procedure = pg_catalog.text_varbinary_le,restrict = scalarltsel, join = scalarltjoinsel);

        CREATE OR REPLACE FUNCTION pg_catalog.varbinary_text_eq(
        varbinary,
        text
        ) RETURNS bool AS
        $$
        BEGIN
            RETURN (SELECT byteaeq($1::bytea,$2::bytea));
        END;
        $$
        LANGUAGE plpgsql;

        CREATE OPERATOR pg_catalog.=(leftarg = varbinary, rightarg = text, procedure = pg_catalog.varbinary_text_eq,restrict = eqsel, join = eqjoinsel,
        HASHES, MERGES);

        CREATE OR REPLACE FUNCTION pg_catalog.varbinary_text_ne(
        varbinary,
        text
        ) RETURNS bool AS
        $$
        BEGIN
            RETURN (SELECT byteane($1::bytea,$2::bytea));
        END;
        $$
        LANGUAGE plpgsql;

        CREATE OPERATOR pg_catalog.<>(leftarg = varbinary, rightarg = text, procedure = pg_catalog.varbinary_text_ne,restrict = neqsel, join = neqjoinsel);

        CREATE OR REPLACE FUNCTION pg_catalog.varbinary_text_gt(
        varbinary,
        text
        ) RETURNS bool AS
        $$
        BEGIN
            RETURN (SELECT byteagt($1::bytea,$2::bytea));
        END;
        $$
        LANGUAGE plpgsql;

        CREATE OPERATOR pg_catalog.>(leftarg = varbinary, rightarg = text, procedure = pg_catalog.varbinary_text_gt,restrict = scalargtsel, join = scalargtjoinsel);


        CREATE OR REPLACE FUNCTION pg_catalog.varbinary_text_lt(
        varbinary,
        text
        ) RETURNS bool AS
        $$
        BEGIN
            RETURN (SELECT bytealt($1::bytea,$2::bytea));
        END;
        $$
        LANGUAGE plpgsql;

        CREATE OPERATOR pg_catalog.<(leftarg = varbinary, rightarg = text, procedure = pg_catalog.varbinary_text_lt,restrict = scalarltsel, join = scalarltjoinsel);

        CREATE OR REPLACE FUNCTION pg_catalog.varbinary_text_ge(
        varbinary,
        text
        ) RETURNS bool AS
        $$
        BEGIN
            RETURN (SELECT byteage($1::bytea,$2::bytea));
        END;
        $$
        LANGUAGE plpgsql;

        CREATE OPERATOR pg_catalog.>=(leftarg = varbinary, rightarg = text, procedure = pg_catalog.varbinary_text_ge,restrict = scalargtsel, join = scalargtjoinsel);

        CREATE OR REPLACE FUNCTION pg_catalog.varbinary_text_le(
        varbinary,
        text
        ) RETURNS bool AS
        $$
        BEGIN
            RETURN (SELECT byteale($1::bytea,$2::bytea));
        END;
        $$
        LANGUAGE plpgsql;

        CREATE OPERATOR pg_catalog.<=(leftarg = varbinary, rightarg = text, procedure = pg_catalog.varbinary_text_le,restrict = scalarltsel, join = scalarltjoinsel);

        CREATE OR REPLACE FUNCTION pg_catalog.binary_varbinary_eq(
        binary,
        varbinary
        ) RETURNS bool AS
        $$
        BEGIN
            RETURN (SELECT byteaeq($1::bytea,$2::bytea));
        END;
        $$
        LANGUAGE plpgsql;

        CREATE OPERATOR pg_catalog.=(leftarg = binary, rightarg = varbinary, procedure = pg_catalog.binary_varbinary_eq,restrict = eqsel, join = eqjoinsel,
        HASHES, MERGES);

        CREATE OR REPLACE FUNCTION pg_catalog.binary_varbinary_ne(
        binary,
        varbinary
        ) RETURNS bool AS
        $$
        BEGIN
            RETURN (SELECT byteane($1::bytea,$2::bytea));
        END;
        $$
        LANGUAGE plpgsql;

        CREATE OPERATOR pg_catalog.<>(leftarg = binary, rightarg = varbinary, procedure = pg_catalog.binary_varbinary_ne,restrict = neqsel, join = neqjoinsel);

        CREATE OR REPLACE FUNCTION pg_catalog.binary_varbinary_gt(
        binary,
        varbinary
        ) RETURNS bool AS
        $$
        BEGIN
            RETURN (SELECT byteagt($1::bytea,$2::bytea));
        END;
        $$
        LANGUAGE plpgsql;

        CREATE OPERATOR pg_catalog.>(leftarg = binary, rightarg = varbinary, procedure = pg_catalog.binary_varbinary_gt,restrict = scalargtsel, join = scalargtjoinsel);

        CREATE OR REPLACE FUNCTION pg_catalog.binary_varbinary_lt(
        binary,
        varbinary
        ) RETURNS bool AS
        $$
        BEGIN
            RETURN (SELECT bytealt($1::bytea,$2::bytea));
        END;
        $$
        LANGUAGE plpgsql;

        CREATE OPERATOR pg_catalog.<(leftarg = binary, rightarg = varbinary, procedure = pg_catalog.binary_varbinary_lt,restrict = scalarltsel, join = scalarltjoinsel);

        CREATE OR REPLACE FUNCTION pg_catalog.binary_varbinary_ge(
        binary,
        varbinary
        ) RETURNS bool AS
        $$
        BEGIN
            RETURN (SELECT byteage($1::bytea,$2::bytea));
        END;
        $$
        LANGUAGE plpgsql;

        CREATE OPERATOR pg_catalog.>=(leftarg = binary, rightarg = varbinary, procedure = pg_catalog.binary_varbinary_ge,restrict = scalargtsel, join = scalargtjoinsel);

        CREATE OR REPLACE FUNCTION pg_catalog.binary_varbinary_le(
        binary,
        varbinary
        ) RETURNS bool AS
        $$
        BEGIN
            RETURN (SELECT byteale($1::bytea,$2::bytea));
        END;
        $$
        LANGUAGE plpgsql;

        CREATE OPERATOR pg_catalog.<=(leftarg = binary, rightarg = varbinary, procedure = pg_catalog.binary_varbinary_le,restrict = scalarltsel, join = scalarltjoinsel);



        CREATE OR REPLACE FUNCTION pg_catalog.varbinary_binary_eq(
        varbinary,
        binary
        ) RETURNS bool AS
        $$
        BEGIN
            RETURN (SELECT byteaeq($1::bytea,$2::bytea));
        END;
        $$
        LANGUAGE plpgsql;

        CREATE OPERATOR pg_catalog.=(leftarg = varbinary, rightarg = binary, procedure = pg_catalog.varbinary_binary_eq,restrict = eqsel, join = eqjoinsel,
        HASHES, MERGES);

        CREATE OR REPLACE FUNCTION pg_catalog.varbinary_binary_ne(
        varbinary,
        binary
        ) RETURNS bool AS
        $$
        BEGIN
            RETURN (SELECT byteane($1::bytea,$2::bytea));
        END;
        $$
        LANGUAGE plpgsql;

        CREATE OPERATOR pg_catalog.<>(leftarg = varbinary, rightarg = binary, procedure = pg_catalog.varbinary_binary_ne,restrict = neqsel, join = neqjoinsel);

        CREATE OR REPLACE FUNCTION pg_catalog.varbinary_binary_gt(
        varbinary,
        binary
        ) RETURNS bool AS
        $$
        BEGIN
            RETURN (SELECT byteagt($1::bytea,$2::bytea));
        END;
        $$
        LANGUAGE plpgsql;

        CREATE OPERATOR pg_catalog.>(leftarg = varbinary, rightarg = binary, procedure = pg_catalog.varbinary_binary_gt,restrict = scalargtsel, join = scalargtjoinsel);


        CREATE OR REPLACE FUNCTION pg_catalog.varbinary_binary_lt(
        varbinary,
        binary
        ) RETURNS bool AS
        $$
        BEGIN
            RETURN (SELECT bytealt($1::bytea,$2::bytea));
        END;
        $$
        LANGUAGE plpgsql;

        CREATE OPERATOR pg_catalog.<(leftarg = varbinary, rightarg = binary, procedure = pg_catalog.varbinary_binary_lt,restrict = scalarltsel, join = scalarltjoinsel);

        CREATE OR REPLACE FUNCTION pg_catalog.varbinary_binary_ge(
        varbinary,
        binary
        ) RETURNS bool AS
        $$
        BEGIN
            RETURN (SELECT byteage($1::bytea,$2::bytea));
        END;
        $$
        LANGUAGE plpgsql;

        CREATE OPERATOR pg_catalog.>=(leftarg = varbinary, rightarg = binary, procedure = pg_catalog.varbinary_binary_ge,restrict = scalargtsel, join = scalargtjoinsel);

        CREATE OR REPLACE FUNCTION pg_catalog.varbinary_binary_le(
        varbinary,
        binary
        ) RETURNS bool AS
        $$
        BEGIN
            RETURN (SELECT byteale($1::bytea,$2::bytea));
        END;
        $$
        LANGUAGE plpgsql;

        CREATE OPERATOR pg_catalog.<=(leftarg = varbinary, rightarg = binary, procedure = pg_catalog.varbinary_binary_le,restrict = scalarltsel, join = scalarltjoinsel);

        CREATE OR REPLACE FUNCTION pg_catalog.varbinary_varbinary_eq(
        varbinary,
        varbinary
        ) RETURNS bool AS
        $$
        BEGIN
            RETURN (SELECT byteaeq($1::bytea,$2::bytea));
        END;
        $$
        LANGUAGE plpgsql;

        CREATE OPERATOR pg_catalog.=(leftarg = varbinary, rightarg = varbinary, procedure = pg_catalog.varbinary_varbinary_eq,restrict = eqsel, join = eqjoinsel,
        HASHES, MERGES);

        CREATE OR REPLACE FUNCTION pg_catalog.varbinary_varbinary_ne(
        varbinary,
        varbinary
        ) RETURNS bool AS
        $$
        BEGIN
            RETURN (SELECT byteane($1::bytea,$2::bytea));
        END;
        $$
        LANGUAGE plpgsql;

        CREATE OPERATOR pg_catalog.<>(leftarg = varbinary, rightarg = varbinary, procedure = pg_catalog.varbinary_varbinary_ne,restrict = neqsel, join = neqjoinsel);

        CREATE OR REPLACE FUNCTION pg_catalog.varbinary_varbinary_gt(
        varbinary,
        varbinary
        ) RETURNS bool AS
        $$
        BEGIN
            RETURN (SELECT byteagt($1::bytea,$2::bytea));
        END;
        $$
        LANGUAGE plpgsql;

        CREATE OPERATOR pg_catalog.>(leftarg = varbinary, rightarg = varbinary, procedure = pg_catalog.varbinary_varbinary_gt,restrict = scalargtsel, join = scalargtjoinsel);


        CREATE OR REPLACE FUNCTION pg_catalog.varbinary_varbinary_lt(
        varbinary,
        varbinary
        ) RETURNS bool AS
        $$
        BEGIN
            RETURN (SELECT bytealt($1::bytea,$2::bytea));
        END;
        $$
        LANGUAGE plpgsql;

        CREATE OPERATOR pg_catalog.<(leftarg = varbinary, rightarg = varbinary, procedure = pg_catalog.varbinary_varbinary_lt,restrict = scalarltsel, join = scalarltjoinsel);

        CREATE OR REPLACE FUNCTION pg_catalog.varbinary_varbinary_ge(
        varbinary,
        varbinary
        ) RETURNS bool AS
        $$
        BEGIN
            RETURN (SELECT byteage($1::bytea,$2::bytea));
        END;
        $$
        LANGUAGE plpgsql;

        CREATE OPERATOR pg_catalog.>=(leftarg = varbinary, rightarg = varbinary, procedure = pg_catalog.varbinary_varbinary_ge,restrict = scalargtsel, join = scalargtjoinsel);

        CREATE OR REPLACE FUNCTION pg_catalog.varbinary_varbinary_le(
        varbinary,
        varbinary
        ) RETURNS bool AS
        $$
        BEGIN
            RETURN (SELECT byteale($1::bytea,$2::bytea));
        END;
        $$
        LANGUAGE plpgsql;

        CREATE OPERATOR pg_catalog.<=(leftarg = varbinary, rightarg = varbinary, procedure = pg_catalog.varbinary_varbinary_le,restrict = scalarltsel, join = scalarltjoinsel);

        CREATE OR REPLACE FUNCTION pg_catalog.bit2numeric (bit) RETURNS numeric AS
        $$
        BEGIN
            RETURN (SELECT bittouint8($1));
        END;
        $$
        LANGUAGE plpgsql;

        CREATE OR REPLACE FUNCTION pg_catalog.tinyblob_rawout (
        tinyblob
        ) RETURNS cstring LANGUAGE INTERNAL IMMUTABLE STRICT as 'byteaout';
        
        CREATE OR REPLACE FUNCTION pg_catalog.mediumblob_rawout (
        mediumblob
        ) RETURNS cstring LANGUAGE INTERNAL IMMUTABLE STRICT as 'byteaout';
        

        CREATE OR REPLACE FUNCTION pg_catalog.longblob_rawout (
        longblob
        ) RETURNS cstring LANGUAGE INTERNAL IMMUTABLE STRICT as 'byteaout';
        
        create operator pg_catalog.^(leftarg = int1, rightarg = int1, procedure = pg_catalog.int1xor);

        CREATE OR REPLACE FUNCTION pg_catalog.blobxor(
        blob,
        blob
        )RETURNS int LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'blobxor';
        CREATE OR REPLACE FUNCTION pg_catalog.blobxor(
        blob,
        int
        )RETURNS int LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'blobxor';
        CREATE OR REPLACE FUNCTION pg_catalog.blobxor(
        int,
        blob
        )RETURNS int LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'blobxor';
        CREATE OR REPLACE FUNCTION pg_catalog.blobxor(
        int8,
        blob
        )RETURNS int LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'blobxor';
        CREATE OR REPLACE FUNCTION pg_catalog.blobxor(
        blob,
        int8
        )RETURNS int LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'blobxor';
        CREATE OR REPLACE FUNCTION pg_catalog.blobxor(
        float8,
        blob
        )RETURNS int LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'blobxor';
        CREATE OR REPLACE FUNCTION pg_catalog.blobxor(
        blob,
        float8
        )RETURNS int LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'blobxor';
        create operator pg_catalog.^(leftarg = blob, rightarg = blob, procedure = pg_catalog.blobxor);
        create operator pg_catalog.^(leftarg = blob, rightarg = integer, procedure = pg_catalog.blobxor);
        create operator pg_catalog.^(leftarg = integer, rightarg = blob, procedure = pg_catalog.blobxor);
        create operator pg_catalog.^(leftarg = int8, rightarg = blob, procedure = pg_catalog.blobxor);
        create operator pg_catalog.^(leftarg = blob, rightarg = int8, procedure = pg_catalog.blobxor);
        create operator pg_catalog.^(leftarg = float8, rightarg = blob, procedure = pg_catalog.blobxor);
        create operator pg_catalog.^(leftarg = blob, rightarg = float8, procedure = pg_catalog.blobxor);


        DROP FUNCTION IF EXISTS pg_catalog.bit_longblob(uint8,longblob);
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
        drop aggregate if exists pg_catalog.bit_xor(longblob);
        create aggregate pg_catalog.bit_xor(longblob) (SFUNC=bit_longblob, STYPE= uint8);

        DROP FUNCTION IF EXISTS pg_catalog.bit_blob(uint8,blob);
        CREATE OR REPLACE FUNCTION pg_catalog.bit_blob (t1 uint8, t2 blob) RETURNS uint8  AS
        $$
        DECLARE num NUMBER := to_number((cast(t2 as text)));
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
        drop aggregate if exists pg_catalog.bit_xor(blob);
        create aggregate pg_catalog.bit_xor(blob) (SFUNC=bit_blob, STYPE= uint8);

        DROP FUNCTION IF EXISTS pg_catalog.varlena2float8(anyelement);
        CREATE OR REPLACE FUNCTION pg_catalog.varlena2float8(anyelement) RETURNS float8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'Varlena2Float8';

        CREATE CAST (bytea AS float8) WITH FUNCTION pg_catalog.varlena2float8(anyelement) AS IMPLICIT;
        CREATE CAST ("binary" AS float8) WITH FUNCTION pg_catalog.varlena2float8(anyelement) AS IMPLICIT;
        CREATE CAST ("varbinary" AS float8) WITH FUNCTION pg_catalog.varlena2float8(anyelement) AS IMPLICIT;
        CREATE CAST (blob AS float8) WITH FUNCTION pg_catalog.varlena2float8(anyelement) AS IMPLICIT;
        CREATE CAST (tinyblob AS float8) WITH FUNCTION pg_catalog.varlena2float8(anyelement) AS IMPLICIT;
        CREATE CAST (mediumblob AS float8) WITH FUNCTION pg_catalog.varlena2float8(anyelement) AS IMPLICIT;
        CREATE CAST (longblob AS float8) WITH FUNCTION pg_catalog.varlena2float8(anyelement) AS IMPLICIT;
        CREATE CAST (json AS float8) WITH FUNCTION pg_catalog.varlena2float8(anyelement);

        DROP FUNCTION IF EXISTS pg_catalog.blob_eq(blob, blob);
        CREATE OR REPLACE FUNCTION pg_catalog.blob_eq(arg1 blob, arg2 blob) RETURNS bool LANGUAGE INTERNAL STRICT AS 'byteaeq';
        DROP FUNCTION IF EXISTS pg_catalog.blob_ne(blob, blob);
        CREATE OR REPLACE FUNCTION pg_catalog.blob_ne(arg1 blob, arg2 blob) RETURNS bool LANGUAGE INTERNAL STRICT AS 'byteane';
        DROP FUNCTION IF EXISTS pg_catalog.blob_lt(blob, blob);
        CREATE OR REPLACE FUNCTION pg_catalog.blob_lt(arg1 blob, arg2 blob) RETURNS bool LANGUAGE INTERNAL STRICT AS 'bytealt';
        DROP FUNCTION IF EXISTS pg_catalog.blob_le(blob, blob);
        CREATE OR REPLACE FUNCTION pg_catalog.blob_le(arg1 blob, arg2 blob) RETURNS bool LANGUAGE INTERNAL STRICT AS 'byteale';
        DROP FUNCTION IF EXISTS pg_catalog.blob_gt(blob, blob);
        CREATE OR REPLACE FUNCTION pg_catalog.blob_gt(arg1 blob, arg2 blob) RETURNS bool LANGUAGE INTERNAL STRICT AS 'byteagt';
        DROP FUNCTION IF EXISTS pg_catalog.blob_ge(blob, blob);
        CREATE OR REPLACE FUNCTION pg_catalog.blob_ge(arg1 blob, arg2 blob) RETURNS bool LANGUAGE INTERNAL STRICT AS 'byteage';

        DROP FUNCTION IF EXISTS pg_catalog.blob_eq_text(blob, text);
        CREATE OR REPLACE FUNCTION pg_catalog.blob_eq_text(arg1 blob, arg2 text) RETURNS bool LANGUAGE SQL STRICT AS $$ SELECT blob_eq($1, $2::blob) $$;
        DROP FUNCTION IF EXISTS pg_catalog.blob_ne_text(blob, text);
        CREATE OR REPLACE FUNCTION pg_catalog.blob_ne_text(arg1 blob, arg2 text) RETURNS bool LANGUAGE SQL STRICT AS $$ SELECT blob_ne($1, $2::blob) $$;
        DROP FUNCTION IF EXISTS pg_catalog.blob_lt_text(blob, text);
        CREATE OR REPLACE FUNCTION pg_catalog.blob_lt_text(arg1 blob, arg2 text) RETURNS bool LANGUAGE SQL STRICT AS $$ SELECT blob_lt($1, $2::blob) $$;
        DROP FUNCTION IF EXISTS pg_catalog.blob_le_text(blob, text);
        CREATE OR REPLACE FUNCTION pg_catalog.blob_le_text(arg1 blob, arg2 text) RETURNS bool LANGUAGE SQL STRICT AS $$ SELECT blob_le($1, $2::blob) $$;
        DROP FUNCTION IF EXISTS pg_catalog.blob_gt_text(blob, text);
        CREATE OR REPLACE FUNCTION pg_catalog.blob_gt_text(arg1 blob, arg2 text) RETURNS bool LANGUAGE SQL STRICT AS $$ SELECT blob_gt($1, $2::blob) $$;
        DROP FUNCTION IF EXISTS pg_catalog.blob_ge_text(blob, text);
        CREATE OR REPLACE FUNCTION pg_catalog.blob_ge_text(arg1 blob, arg2 text) RETURNS bool LANGUAGE SQL STRICT AS $$ SELECT blob_ge($1, $2::blob) $$;

        DROP FUNCTION IF EXISTS pg_catalog.text_eq_blob(text, blob);
        CREATE OR REPLACE FUNCTION pg_catalog.text_eq_blob(arg1 text, arg2 blob) RETURNS bool LANGUAGE SQL STRICT AS $$ SELECT blob_eq($1::blob, $2) $$;
        DROP FUNCTION IF EXISTS pg_catalog.text_ne_blob(text, blob);
        CREATE OR REPLACE FUNCTION pg_catalog.text_ne_blob(arg1 text, arg2 blob) RETURNS bool LANGUAGE SQL STRICT AS $$ SELECT blob_ne($1::blob, $2) $$;
        DROP FUNCTION IF EXISTS pg_catalog.text_lt_blob(text, blob);
        CREATE OR REPLACE FUNCTION pg_catalog.text_lt_blob(arg1 text, arg2 blob) RETURNS bool LANGUAGE SQL STRICT AS $$ SELECT blob_lt($1::blob, $2) $$;
        DROP FUNCTION IF EXISTS pg_catalog.text_le_blob(text, blob);
        CREATE OR REPLACE FUNCTION pg_catalog.text_le_blob(arg1 text, arg2 blob) RETURNS bool LANGUAGE SQL STRICT AS $$ SELECT blob_le($1::blob, $2) $$;
        DROP FUNCTION IF EXISTS pg_catalog.test_gt_blob(text, blob);
        CREATE OR REPLACE FUNCTION pg_catalog.test_gt_blob(arg1 text, arg2 blob) RETURNS bool LANGUAGE SQL STRICT AS $$ SELECT blob_gt($1::blob, $2) $$;
        DROP FUNCTION IF EXISTS pg_catalog.test_ge_blob(text, blob);
        CREATE OR REPLACE FUNCTION pg_catalog.test_ge_blob(arg1 text, arg2 blob) RETURNS bool LANGUAGE SQL STRICT AS $$ SELECT blob_ge($1::blob, $2) $$;

        CREATE OPERATOR pg_catalog.=(leftarg = blob, rightarg = blob, procedure = blob_eq, restrict = eqsel, join = eqjoinsel);
        CREATE OPERATOR pg_catalog.<>(leftarg = blob, rightarg = blob, procedure = blob_ne, restrict = neqsel, join = neqjoinsel);
        CREATE OPERATOR pg_catalog.<(leftarg = blob, rightarg = blob, procedure = blob_lt, restrict = scalarltsel, join = scalarltjoinsel);
        CREATE OPERATOR pg_catalog.<=(leftarg = blob, rightarg = blob, procedure = blob_le, restrict = scalarltsel, join = scalarltjoinsel);
        CREATE OPERATOR pg_catalog.>(leftarg = blob, rightarg = blob, procedure = blob_gt, restrict = scalarltsel, join = scalarltjoinsel);
        CREATE OPERATOR pg_catalog.>=(leftarg = blob, rightarg = blob, procedure = blob_ge, restrict = scalarltsel, join = scalarltjoinsel);

        CREATE OPERATOR pg_catalog.=(leftarg = blob, rightarg = text, procedure = blob_eq_text, restrict = eqsel, join = eqjoinsel);
        CREATE OPERATOR pg_catalog.<>(leftarg = blob, rightarg = text, procedure = blob_ne_text, restrict = neqsel, join = neqjoinsel);
        CREATE OPERATOR pg_catalog.<(leftarg = blob, rightarg = text, procedure = blob_lt_text, restrict = scalarltsel, join = scalarltjoinsel);
        CREATE OPERATOR pg_catalog.<=(leftarg = blob, rightarg = text, procedure = blob_le_text, restrict = scalarltsel, join = scalarltjoinsel);
        CREATE OPERATOR pg_catalog.>(leftarg = blob, rightarg = text, procedure = blob_gt_text, restrict = scalarltsel, join = scalarltjoinsel);
        CREATE OPERATOR pg_catalog.>=(leftarg = blob, rightarg = text, procedure = blob_ge_text, restrict = scalarltsel, join = scalarltjoinsel);

        CREATE OPERATOR pg_catalog.=(leftarg = text, rightarg = blob, procedure = text_eq_blob, restrict = eqsel, join = eqjoinsel);
        CREATE OPERATOR pg_catalog.<>(leftarg = text, rightarg = blob, procedure = text_ne_blob, restrict = neqsel, join = neqjoinsel);
        CREATE OPERATOR pg_catalog.<(leftarg = text, rightarg = blob, procedure = text_lt_blob, restrict = scalarltsel, join = scalarltjoinsel);
        CREATE OPERATOR pg_catalog.<=(leftarg = text, rightarg = blob, procedure = text_le_blob, restrict = scalarltsel, join = scalarltjoinsel);
        CREATE OPERATOR pg_catalog.>(leftarg = text, rightarg = blob, procedure = test_gt_blob, restrict = scalarltsel, join = scalarltjoinsel);
        CREATE OPERATOR pg_catalog.>=(leftarg = text, rightarg = blob, procedure = test_ge_blob, restrict = scalarltsel, join = scalarltjoinsel);

        CREATE OR REPLACE FUNCTION pg_catalog.show_character_set(OUT "charset" NAME,
                                                 OUT "description" TEXT,
                                                 OUT "default collation" TEXT,
                                                 OUT "maxlen" INT4,
                                                 OUT "server" BOOL
        ) RETURNS SETOF RECORD AS '$libdir/dolphin','ShowCharset' 
         LANGUAGE C;

        CREATE OR REPLACE FUNCTION pg_catalog.show_collation(OUT "collation" NAME,
                                                 OUT "charset" NAME,
                                                 OUT "id" OID,
                                                 OUT "default" TEXT,
                                                 OUT "compiled" TEXT,
                                                 OUT "sortlen" INT4
        ) RETURNS SETOF RECORD AS '$libdir/dolphin','ShowCollation' 
         LANGUAGE C;

        CREATE OR REPLACE FUNCTION pg_catalog.dolphin_types()
        RETURNS text[][] LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'dolphin_types';
        
        DROP FUNCTION IF EXISTS pg_catalog.gs_get_viewdef_oid(integer);

        CREATE OR REPLACE FUNCTION pg_catalog.gs_get_viewdef_oid (
        integer
        ) RETURNS text LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'gs_get_viewdef_oid';

        DROP FUNCTION IF EXISTS pg_catalog.system_user();

        CREATE OR REPLACE FUNCTION pg_catalog.system_user ()
        RETURNS text
        AS
        $$
        BEGIN
            RETURN (SELECT session_user);
        END;
        $$
        LANGUAGE plpgsql;
        DROP FUNCTION IF EXISTS pg_catalog.database();
        CREATE OR REPLACE FUNCTION pg_catalog.database(
        ) RETURNS name LANGUAGE C STABLE STRICT AS '$libdir/dolphin','get_b_database';

        CREATE OR REPLACE FUNCTION pg_catalog.boolxor (
        boolean,
        boolean
        ) RETURNS int LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'boolxor';
        create operator pg_catalog.^(leftarg = boolean, rightarg = boolean, procedure = pg_catalog.boolxor);

        CREATE OR REPLACE FUNCTION pg_catalog.ord (text) RETURNS int16 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'ord_text';

        CREATE OR REPLACE FUNCTION pg_catalog.ord (numeric) RETURNS int16 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'ord_numeric';

        DROP FUNCTION IF EXISTS pg_catalog.substring_index (text, text, numeric);
        CREATE OR REPLACE FUNCTION pg_catalog.substring_index (
        text,
        text,
        numeric
        ) RETURNS text LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'substring_index';

        DROP FUNCTION IF EXISTS pg_catalog.substring_index (boolean, text, numeric);
        CREATE OR REPLACE FUNCTION pg_catalog.substring_index (
        boolean,
        text,
        numeric
        ) RETURNS text LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'substring_index_bool_1';

        DROP FUNCTION IF EXISTS pg_catalog.substring_index (text, boolean, numeric);
        CREATE OR REPLACE FUNCTION pg_catalog.substring_index (
        text,
        boolean,
        numeric
        ) RETURNS text LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'substring_index_bool_2';

        DROP FUNCTION IF EXISTS pg_catalog.substring_index (boolean, boolean, numeric);
        CREATE OR REPLACE FUNCTION pg_catalog.substring_index (
        boolean,
        boolean,
        numeric
        ) RETURNS text LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'substring_index_2bool';

        CREATE OR REPLACE FUNCTION pg_catalog.bool_float8_xor(
        boolean,
        float8
        ) RETURNS int LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'bool_float8_xor';
        create operator pg_catalog.^(leftarg = boolean, rightarg = float8, procedure = pg_catalog.bool_float8_xor);

        CREATE OR REPLACE FUNCTION pg_catalog.float8_bool_xor(
        float8,
        boolean
        ) RETURNS int LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'float8_bool_xor';
        create operator pg_catalog.^(leftarg = float8, rightarg = boolean, procedure = pg_catalog.float8_bool_xor);

        DROP FUNCTION IF EXISTS pg_catalog.b_db_sys_real_timestamp(integer);
        CREATE OR REPLACE FUNCTION pg_catalog.b_db_sys_real_timestamp(integer) returns timestamp(0) without time zone LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'b_db_sys_real_timestamp';

        DROP FUNCTION IF EXISTS pg_catalog.b_db_statement_start_timestamp(integer);
        CREATE OR REPLACE FUNCTION pg_catalog.b_db_statement_start_timestamp(integer) returns timestamp(0) without time zone LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'b_db_statement_start_timestamp';

        DROP OPERATOR IF EXISTS public.+(year, interval);
        DROP OPERATOR IF EXISTS pg_catalog.+(year, interval);
        CREATE OPERATOR pg_catalog.+ (
           leftarg = year,
           rightarg = interval,
           procedure = year_pl_interval,
           commutator = operator(pg_catalog.+)
        );

        DROP OPERATOR IF EXISTS public.-(year, interval);
        DROP OPERATOR IF EXISTS pg_catalog.-(year, interval);
        CREATE OPERATOR pg_catalog.- (
           leftarg = year,
           rightarg = interval,
           procedure = year_mi_interval,
           commutator = operator(pg_catalog.-)
        );

        DROP OPERATOR IF EXISTS public.+(interval, year);
        DROP OPERATOR IF EXISTS pg_catalog.+(interval, year);
        CREATE OPERATOR pg_catalog.+ (
           leftarg = interval,
           rightarg = year,
           procedure = interval_pl_year,
           commutator = operator(pg_catalog.+)
        );

        CREATE OR REPLACE FUNCTION pg_catalog.int64_b_format_time (int8) RETURNS time LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'int64_b_format_time';
        DROP CAST IF EXISTS (int8 AS time);

        CREATE CAST(int8 AS time) WITH FUNCTION int64_b_format_time(int8) AS IMPLICIT;

        CREATE OR REPLACE FUNCTION pg_catalog.int32_b_format_datetime (int4) RETURNS timestamp(0) without time zone LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'int32_b_format_datetime';

        CREATE OR REPLACE FUNCTION pg_catalog.int64_b_format_datetime (int8) RETURNS timestamp(0) without time zone LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'int64_b_format_datetime';

        CREATE OR REPLACE FUNCTION pg_catalog.numeric_b_format_datetime (numeric) RETURNS timestamp(0) without time zone LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'numeric_b_format_datetime';

        DROP CAST IF EXISTS (int4 AS datetime);

        CREATE CAST(int4 AS timestamp(0) without time zone) WITH FUNCTION int32_b_format_datetime(int4) AS IMPLICIT;

        DROP CAST IF EXISTS (int8 AS datetime);

        CREATE CAST(int8 AS timestamp(0) without time zone) WITH FUNCTION int64_b_format_datetime(int8) AS IMPLICIT;

        DROP CAST IF EXISTS (numeric AS datetime);

        CREATE CAST(numeric AS timestamp(0) without time zone) WITH FUNCTION numeric_b_format_datetime(numeric) AS IMPLICIT;

        DROP FUNCTION IF EXISTS pg_catalog.year (datetime);
        CREATE OR REPLACE FUNCTION pg_catalog.year (timestamp(0) without time zone) RETURNS float8 LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'datetime_year_part';
        CREATE OR REPLACE FUNCTION pg_catalog.year (text) RETURNS float8 LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'text_year_part';
        CREATE OR REPLACE FUNCTION pg_catalog.sec_to_time (text) RETURNS time LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'sec_to_time_str';

        DROP FUNCTION IF EXISTS pg_catalog.subtime (text, text);
        DROP FUNCTION IF EXISTS pg_catalog.subtime (date, text);
        DROP FUNCTION IF EXISTS pg_catalog.subtime (date, datetime);
        DROP FUNCTION IF EXISTS pg_catalog.subtime (date, date);
        DROP FUNCTION IF EXISTS pg_catalog.subtime (text, datetime);
        DROP FUNCTION IF EXISTS pg_catalog.subtime (text, date);
        DROP FUNCTION IF EXISTS pg_catalog.subtime (numeric, numeric);
        DROP FUNCTION IF EXISTS pg_catalog.subtime (text, numeric);
        DROP FUNCTION IF EXISTS pg_catalog.subtime (numeric, text);

        DROP FUNCTION IF EXISTS pg_catalog.timediff (text, text);
        DROP FUNCTION IF EXISTS pg_catalog.timediff (datetime, text);
        DROP FUNCTION IF EXISTS pg_catalog.timediff (time, text);
        DROP FUNCTION IF EXISTS pg_catalog.timediff (datetime, time);
        DROP FUNCTION IF EXISTS pg_catalog.timediff (time, date);
        DROP FUNCTION IF EXISTS pg_catalog.timediff (date, text);
        DROP FUNCTION IF EXISTS pg_catalog.timediff (date, time);
        DROP FUNCTION IF EXISTS pg_catalog.timediff (numeric, numeric);
        DROP FUNCTION IF EXISTS pg_catalog.timediff (text, numeric);
        DROP FUNCTION IF EXISTS pg_catalog.timediff (numeric, text);

        CREATE OR REPLACE FUNCTION pg_catalog.subtime ("any", "any") RETURNS TEXT LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'subtime';
        CREATE OR REPLACE FUNCTION pg_catalog.timediff ("any", "any") RETURNS time LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'timediff';

        DROP FUNCTION IF EXISTS pg_catalog.time_format (date, text);
        CREATE OR REPLACE FUNCTION pg_catalog.time_format (date, text) RETURNS text LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'time_format_date';
        
        DROP FUNCTION IF EXISTS pg_catalog.time_mysql (text);
        DROP FUNCTION IF EXISTS pg_catalog.time_mysql (numeric);
        DROP FUNCTION IF EXISTS pg_catalog.time_mysql (date);
        CREATE OR REPLACE FUNCTION pg_catalog.time_mysql ("any") RETURNS time LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'time_mysql';

        DROP FUNCTION IF EXISTS pg_catalog.timestamp_mysql (datetime);
        DROP FUNCTION IF EXISTS pg_catalog.timestamp_mysql (datetime, text);
        DROP FUNCTION IF EXISTS pg_catalog.timestamp_mysql (text, text);
        DROP FUNCTION IF EXISTS pg_catalog.timestamp_mysql (time, text);
        DROP FUNCTION IF EXISTS pg_catalog.timestamp_mysql (time);
        
        CREATE OR REPLACE FUNCTION pg_catalog.timestamp_mysql ("any") RETURNS timestamp(0) without time zone LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'timestamp_param1';
        CREATE OR REPLACE FUNCTION pg_catalog.timestamp_mysql ("any", "any") RETURNS timestamp(0) without time zone LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'timestamp_param2';

        DROP FUNCTION IF EXISTS pg_catalog.timestamp_add (text, numeric, text);
        DROP FUNCTION IF EXISTS pg_catalog.timestamp_add (text, numeric, time);
        DROP FUNCTION IF EXISTS pg_catalog.timestamp_add (text, numeric, numeric);

        CREATE OR REPLACE FUNCTION pg_catalog.timestamp_add (text, numeric, "any") RETURNS text LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'timestamp_add_numeric';
        CREATE OR REPLACE FUNCTION pg_catalog.timestamp_add (text, text, "any") RETURNS text LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'timestamp_add_text';

        DROP FUNCTION IF EXISTS pg_catalog.to_days (datetime);
        CREATE OR REPLACE FUNCTION pg_catalog.to_days (timestamp(0) without time zone) RETURNS int8 LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'to_days';

        DROP FUNCTION IF EXISTS pg_catalog.to_seconds (text);
        DROP FUNCTION IF EXISTS pg_catalog.to_seconds (time);
        DROP FUNCTION IF EXISTS pg_catalog.to_seconds (numeric);
        CREATE OR REPLACE FUNCTION pg_catalog.to_seconds ("any") RETURNS numeric LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'to_seconds';

        DROP FUNCTION IF EXISTS pg_catalog.unix_timestamp (text);
        DROP FUNCTION IF EXISTS pg_catalog.unix_timestamp (time);
        DROP FUNCTION IF EXISTS pg_catalog.unix_timestamp (numeric);
        CREATE OR REPLACE FUNCTION pg_catalog.unix_timestamp ("any") RETURNS numeric LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'unix_timestamp';

        DROP FUNCTION IF EXISTS pg_catalog.utc_timestamp_func (int4);
        CREATE OR REPLACE FUNCTION pg_catalog.utc_timestamp_func (int4) RETURNS timestamp(0) without time zone LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'utc_timestamp_func';
        
        DROP FUNCTION IF EXISTS pg_catalog.b_db_date(text);
        CREATE OR REPLACE FUNCTION pg_catalog.b_db_date(text) RETURNS date LANGUAGE C STABLE RETURNS NULL ON NULL INPUT as '$libdir/dolphin', 'b_db_date_text';
        DROP FUNCTION IF EXISTS pg_catalog.b_db_date(numeric);
        CREATE OR REPLACE FUNCTION pg_catalog.b_db_date(numeric) RETURNS date LANGUAGE C STABLE RETURNS NULL ON NULL INPUT as '$libdir/dolphin', 'b_db_date_numeric';

        DROP FUNCTION IF EXISTS pg_catalog.datediff(text, text);
        DROP FUNCTION IF EXISTS pg_catalog.datediff(text, numeric);
        DROP FUNCTION IF EXISTS pg_catalog.datediff(numeric, text);
        DROP FUNCTION IF EXISTS pg_catalog.datediff(numeric, numeric);
        CREATE OR REPLACE FUNCTION pg_catalog.datediff(text, text) RETURNS int4 LANGUAGE C STABLE RETURNS NULL ON NULL INPUT as '$libdir/dolphin', 'datediff_t_t';
        CREATE OR REPLACE FUNCTION pg_catalog.datediff(text, numeric) RETURNS int4 LANGUAGE C STABLE RETURNS NULL ON NULL INPUT as '$libdir/dolphin', 'datediff_t_n';
        CREATE OR REPLACE FUNCTION pg_catalog.datediff(numeric, text) RETURNS int4 AS $$ SELECT -pg_catalog.datediff($2, $1) $$ LANGUAGE SQL;
        CREATE OR REPLACE FUNCTION pg_catalog.datediff(numeric, numeric) RETURNS int4 LANGUAGE C STABLE RETURNS NULL ON NULL INPUT as '$libdir/dolphin', 'datediff_n_n';
        CREATE OR REPLACE FUNCTION pg_catalog.datediff(time, text) RETURNS int4 LANGUAGE C STABLE RETURNS NULL ON NULL INPUT as '$libdir/dolphin', 'datediff_time_t';
        CREATE OR REPLACE FUNCTION pg_catalog.datediff(time, numeric) RETURNS int4 LANGUAGE C STABLE RETURNS NULL ON NULL INPUT as '$libdir/dolphin', 'datediff_time_n';
        CREATE OR REPLACE FUNCTION pg_catalog.datediff(time, time) RETURNS int4 LANGUAGE C STABLE RETURNS NULL ON NULL INPUT as '$libdir/dolphin', 'datediff_time_time';
        CREATE OR REPLACE FUNCTION pg_catalog.datediff(text, time) RETURNS int4 AS $$ SELECT -pg_catalog.datediff($2, $1) $$ LANGUAGE SQL;
        CREATE OR REPLACE FUNCTION pg_catalog.datediff(numeric, time) RETURNS int4 AS $$ SELECT -pg_catalog.datediff($2, $1) $$ LANGUAGE SQL;

        DROP FUNCTION IF EXISTS pg_catalog.b_timestampdiff(text,text,text);
        DROP FUNCTION IF EXISTS pg_catalog.b_timestampdiff(text,time,text);
        DROP FUNCTION IF EXISTS pg_catalog.b_timestampdiff(text,text,time);
        CREATE OR REPLACE FUNCTION pg_catalog.b_timestampdiff(text,text,text) RETURNS int8 LANGUAGE C STABLE RETURNS NULL ON NULL INPUT as '$libdir/dolphin', 'timestampdiff_datetime_tt';
        CREATE OR REPLACE FUNCTION pg_catalog.b_timestampdiff(text,numeric,numeric) RETURNS int8 LANGUAGE C STABLE RETURNS NULL ON NULL INPUT as '$libdir/dolphin', 'timestampdiff_datetime_nn';
        CREATE OR REPLACE FUNCTION pg_catalog.b_timestampdiff(text,text,numeric) RETURNS int8 LANGUAGE C STABLE RETURNS NULL ON NULL INPUT as '$libdir/dolphin', 'timestampdiff_datetime_tn';
        CREATE OR REPLACE FUNCTION pg_catalog.b_timestampdiff(text,numeric,text) RETURNS int8 AS $$ SELECT -pg_catalog.b_timestampdiff($1, $3, $2) $$ LANGUAGE SQL;
        CREATE OR REPLACE FUNCTION pg_catalog.b_timestampdiff(text,time,text) RETURNS int8 LANGUAGE C STABLE RETURNS NULL ON NULL INPUT as '$libdir/dolphin', 'timestampdiff_time_before_t';
        CREATE OR REPLACE FUNCTION pg_catalog.b_timestampdiff(text,time,numeric) RETURNS int8 LANGUAGE C STABLE RETURNS NULL ON NULL INPUT as '$libdir/dolphin', 'timestampdiff_time_before_n';
        CREATE OR REPLACE FUNCTION pg_catalog.b_timestampdiff(text,text,time) RETURNS int8 AS $$ SELECT -pg_catalog.b_timestampdiff($1, $3, $2) $$ LANGUAGE SQL;
        CREATE OR REPLACE FUNCTION pg_catalog.b_timestampdiff(text,numeric,time) RETURNS int8 AS $$ SELECT -pg_catalog.b_timestampdiff($1, $3, $2) $$ LANGUAGE SQL;
        
        DROP FUNCTION IF EXISTS pg_catalog.convert_tz(text,text,text);
        DROP FUNCTION IF EXISTS pg_catalog.convert_tz(numeric,text,text);
        CREATE OR REPLACE FUNCTION pg_catalog.convert_tz(text,text,text) RETURNS timestamp(0) without time zone LANGUAGE C STABLE RETURNS NULL ON NULL INPUT as '$libdir/dolphin', 'convert_tz_t';
        CREATE OR REPLACE FUNCTION pg_catalog.convert_tz(numeric,text,text) RETURNS timestamp(0) without time zone LANGUAGE C STABLE RETURNS NULL ON NULL INPUT as '$libdir/dolphin', 'convert_tz_n';
        CREATE OR REPLACE FUNCTION pg_catalog.convert_tz(time,text,text) RETURNS timestamp(0) without time zone LANGUAGE C STABLE RETURNS NULL ON NULL INPUT as '$libdir/dolphin', 'convert_tz_time';

        DROP FUNCTION IF EXISTS pg_catalog.adddate(text, int8);
        CREATE OR REPLACE FUNCTION pg_catalog.adddate (text, int8) RETURNS text LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'adddate_datetime_days_t';

        DROP FUNCTION IF EXISTS pg_catalog.adddate(text, interval);
        DROP FUNCTION IF EXISTS pg_catalog.adddate(numeric, int8);
        DROP FUNCTION IF EXISTS pg_catalog.adddate(numeric, interval);
        CREATE OR REPLACE FUNCTION pg_catalog.adddate (text, interval) RETURNS text LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'adddate_datetime_interval_t';
        CREATE OR REPLACE FUNCTION pg_catalog.adddate (numeric, int8) RETURNS text LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'adddate_datetime_days_n';
        CREATE OR REPLACE FUNCTION pg_catalog.adddate (numeric, interval) RETURNS text LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'adddate_datetime_interval_n';
        
        DROP FUNCTION IF EXISTS pg_catalog.date_sub (text, interval);
        DROP FUNCTION IF EXISTS pg_catalog.date_sub (numeric, interval);
        DROP FUNCTION IF EXISTS pg_catalog.date_sub (time, interval);
        CREATE OR REPLACE FUNCTION pg_catalog.date_sub (text, interval) RETURNS text AS $$ SELECT pg_catalog.adddate($1, -$2)  $$ LANGUAGE SQL;
        CREATE OR REPLACE FUNCTION pg_catalog.date_sub (numeric, interval) RETURNS text AS $$ SELECT pg_catalog.adddate($1, -$2)  $$ LANGUAGE SQL;
        CREATE OR REPLACE FUNCTION pg_catalog.date_sub (time, interval) RETURNS time AS $$ SELECT pg_catalog.adddate($1, -$2)  $$ LANGUAGE SQL;

        DROP FUNCTION IF EXISTS pg_catalog.date_add (numeric, interval);
        CREATE OR REPLACE FUNCTION pg_catalog.date_add (numeric, interval) RETURNS text AS $$ SELECT pg_catalog.adddate($1, $2)  $$ LANGUAGE SQL;

        DROP FUNCTION IF EXISTS pg_catalog.addtime (date, datetime);
        DROP FUNCTION IF EXISTS pg_catalog.addtime (text, datetime);
        CREATE OR REPLACE FUNCTION pg_catalog.addtime (date, timestamp(0) without time zone) RETURNS TEXT LANGUAGE SQL STABLE STRICT as 'select pg_catalog.addtime(cast(0 as time), cast($2 as time))';
        CREATE OR REPLACE FUNCTION pg_catalog.addtime (text, timestamp(0) without time zone) RETURNS TEXT LANGUAGE SQL STABLE STRICT as 'select pg_catalog.addtime($1, cast($2 as time))';

        CREATE OR REPLACE FUNCTION pg_catalog.hour (text) RETURNS float8 LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'GetHour';
        CREATE OR REPLACE FUNCTION pg_catalog.microsecond (text) RETURNS float8 LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'GetMicrosecond';
        CREATE OR REPLACE FUNCTION pg_catalog.minute (text) RETURNS float8 LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'GetMinute';
        CREATE OR REPLACE FUNCTION pg_catalog.second (text) RETURNS float8 LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'GetSecond';

        CREATE OR REPLACE FUNCTION pg_catalog.dayofmonth (timestamptz) RETURNS float8 LANGUAGE SQL STABLE STRICT as 'select pg_catalog.date_part(''day'', $1)';
        CREATE OR REPLACE FUNCTION pg_catalog.dayofmonth (timetz) RETURNS float8 LANGUAGE SQL STABLE STRICT as 'select pg_catalog.date_part(''day'', $1)';
        CREATE OR REPLACE FUNCTION pg_catalog.dayofmonth (abstime) RETURNS float8 LANGUAGE SQL STABLE STRICT as 'select pg_catalog.date_part(''day'', $1)';
        CREATE OR REPLACE FUNCTION pg_catalog.dayofmonth (date) RETURNS float8 LANGUAGE SQL STABLE STRICT as 'select pg_catalog.date_part(''day'', $1)';
        CREATE OR REPLACE FUNCTION pg_catalog.dayofmonth (time) RETURNS float8 LANGUAGE SQL STABLE STRICT as 'select pg_catalog.date_part(''day'', $1)';
        CREATE OR REPLACE FUNCTION pg_catalog.dayofmonth (timestamp(0) with time zone) RETURNS float8 LANGUAGE SQL STABLE STRICT as 'select pg_catalog.date_part(''day'', $1)';

        CREATE OR REPLACE FUNCTION pg_catalog.dayofweek (timestamptz) RETURNS float8 LANGUAGE SQL STABLE STRICT as 'select 1 + pg_catalog.date_part(''dow'', $1)';
        CREATE OR REPLACE FUNCTION pg_catalog.dayofweek (timetz) RETURNS float8 LANGUAGE SQL STABLE STRICT as 'select 1 + pg_catalog.date_part(''dow'', $1)';
        CREATE OR REPLACE FUNCTION pg_catalog.dayofweek (abstime) RETURNS float8 LANGUAGE SQL STABLE STRICT as 'select 1 + pg_catalog.date_part(''dow'', $1)';
        CREATE OR REPLACE FUNCTION pg_catalog.dayofweek (date) RETURNS float8 LANGUAGE SQL STABLE STRICT as 'select 1 + pg_catalog.date_part(''dow'', $1)';
        CREATE OR REPLACE FUNCTION pg_catalog.dayofweek (time) RETURNS float8 LANGUAGE SQL STABLE STRICT as 'select 1 + pg_catalog.date_part(''dow'', $1)';
        CREATE OR REPLACE FUNCTION pg_catalog.dayofweek (timestamp(0) with time zone) RETURNS float8 LANGUAGE SQL STABLE STRICT as 'select 1 + pg_catalog.date_part(''dow'', $1)';

        CREATE OR REPLACE FUNCTION pg_catalog.dayofyear (timestamptz) RETURNS float8 LANGUAGE SQL STABLE STRICT as 'select pg_catalog.date_part(''doy'', $1)';
        CREATE OR REPLACE FUNCTION pg_catalog.dayofyear (timetz) RETURNS float8 LANGUAGE SQL STABLE STRICT as 'select pg_catalog.date_part(''doy'', $1)';
        CREATE OR REPLACE FUNCTION pg_catalog.dayofyear (abstime) RETURNS float8 LANGUAGE SQL STABLE STRICT as 'select pg_catalog.date_part(''doy'', $1)';
        CREATE OR REPLACE FUNCTION pg_catalog.dayofyear (date) RETURNS float8 LANGUAGE SQL STABLE STRICT as 'select pg_catalog.date_part(''doy'', $1)';
        CREATE OR REPLACE FUNCTION pg_catalog.dayofyear (time) RETURNS float8 LANGUAGE SQL STABLE STRICT as 'select pg_catalog.date_part(''doy'', $1)';
        CREATE OR REPLACE FUNCTION pg_catalog.dayofyear (timestamp(0) with time zone) RETURNS float8 LANGUAGE SQL STABLE STRICT as 'select pg_catalog.date_part(''doy'', $1)';

        CREATE OR REPLACE FUNCTION pg_catalog.quarter (timestamptz) RETURNS float8 LANGUAGE SQL STABLE STRICT as 'select pg_catalog.date_part(''quarter'', $1)';
        CREATE OR REPLACE FUNCTION pg_catalog.quarter (timetz) RETURNS float8 LANGUAGE SQL STABLE STRICT as 'select pg_catalog.date_part(''quarter'', $1)';
        CREATE OR REPLACE FUNCTION pg_catalog.quarter (abstime) RETURNS float8 LANGUAGE SQL STABLE STRICT as 'select pg_catalog.date_part(''quarter'', $1)';
        CREATE OR REPLACE FUNCTION pg_catalog.quarter (date) RETURNS float8 LANGUAGE SQL STABLE STRICT as 'select pg_catalog.date_part(''quarter'', $1)';
        CREATE OR REPLACE FUNCTION pg_catalog.quarter (time) RETURNS float8 LANGUAGE SQL STABLE STRICT as 'select pg_catalog.date_part(''quarter'', $1)';
        CREATE OR REPLACE FUNCTION pg_catalog.quarter (timestamp(0) with time zone) RETURNS float8 LANGUAGE SQL STABLE STRICT as 'select pg_catalog.date_part(''quarter'', $1)';

        CREATE OR REPLACE FUNCTION pg_catalog.weekday (timestamptz) RETURNS float8 LANGUAGE SQL STABLE STRICT as 'select pg_catalog.date_part(''isodow'', $1) - 1';
        CREATE OR REPLACE FUNCTION pg_catalog.weekday (timetz) RETURNS float8 LANGUAGE SQL STABLE STRICT as 'select pg_catalog.date_part(''isodow'', $1) - 1';
        CREATE OR REPLACE FUNCTION pg_catalog.weekday (abstime) RETURNS float8 LANGUAGE SQL STABLE STRICT as 'select pg_catalog.date_part(''isodow'', $1) - 1';
        CREATE OR REPLACE FUNCTION pg_catalog.weekday (date) RETURNS float8 LANGUAGE SQL STABLE STRICT as 'select pg_catalog.date_part(''isodow'', $1) - 1';
        CREATE OR REPLACE FUNCTION pg_catalog.weekday (time) RETURNS float8 LANGUAGE SQL STABLE STRICT as 'select pg_catalog.date_part(''isodow'', $1) - 1';
        CREATE OR REPLACE FUNCTION pg_catalog.weekday (timestamp(0) with time zone) RETURNS float8 LANGUAGE SQL STABLE STRICT as 'select pg_catalog.date_part(''isodow'', $1) - 1';

        CREATE OR REPLACE FUNCTION pg_catalog.weekofyear (timestamptz) RETURNS float8 LANGUAGE SQL STABLE STRICT as 'select pg_catalog.date_part(''week'', $1)';
        CREATE OR REPLACE FUNCTION pg_catalog.weekofyear (timetz) RETURNS float8 LANGUAGE SQL STABLE STRICT as 'select pg_catalog.date_part(''week'', $1)';
        CREATE OR REPLACE FUNCTION pg_catalog.weekofyear (abstime) RETURNS float8 LANGUAGE SQL STABLE STRICT as 'select pg_catalog.date_part(''week'', $1)';
        CREATE OR REPLACE FUNCTION pg_catalog.weekofyear (date) RETURNS float8 LANGUAGE SQL STABLE STRICT as 'select pg_catalog.date_part(''week'', $1)';
        CREATE OR REPLACE FUNCTION pg_catalog.weekofyear (time) RETURNS float8 LANGUAGE SQL STABLE STRICT as 'select pg_catalog.date_part(''week'', $1)';
        CREATE OR REPLACE FUNCTION pg_catalog.weekofyear (timestamp(0) with time zone) RETURNS float8 LANGUAGE SQL STABLE STRICT as 'select pg_catalog.date_part(''week'', $1)';

        CREATE OR REPLACE FUNCTION pg_catalog.get_format (int4, text) RETURNS TEXT LANGUAGE C STABLE RETURNS NULL ON NULL INPUT as '$libdir/dolphin', 'get_format';

        CREATE OR REPLACE FUNCTION pg_catalog.date_format (text, text) RETURNS TEXT LANGUAGE C STABLE RETURNS NULL ON NULL INPUT as '$libdir/dolphin', 'date_format_text';
        CREATE OR REPLACE FUNCTION pg_catalog.date_format (numeric, text) RETURNS TEXT LANGUAGE C STABLE RETURNS NULL ON NULL INPUT as '$libdir/dolphin', 'date_format_numeric';

        CREATE OR REPLACE FUNCTION pg_catalog.b_extract (text, text) RETURNS int8 LANGUAGE C STABLE RETURNS NULL ON NULL INPUT as '$libdir/dolphin', 'b_extract_text';
        CREATE OR REPLACE FUNCTION pg_catalog.b_extract (text, numeric) RETURNS int8 LANGUAGE C STABLE RETURNS NULL ON NULL INPUT as '$libdir/dolphin', 'b_extract_numeric';

        CREATE OR REPLACE FUNCTION pg_catalog.str_to_date (text, text) RETURNS text LANGUAGE C STABLE CALLED ON NULL INPUT as '$libdir/dolphin', 'str_to_date';

        CREATE OR REPLACE FUNCTION pg_catalog.from_unixtime (numeric) RETURNS timestamp(0) without time zone LANGUAGE C STABLE RETURNS NULL ON NULL INPUT as '$libdir/dolphin', 'from_unixtime_with_one_arg';
        CREATE OR REPLACE FUNCTION pg_catalog.from_unixtime (numeric, text) RETURNS TEXT LANGUAGE C STABLE RETURNS NULL ON NULL INPUT as '$libdir/dolphin', 'from_unixtime_with_two_arg';

        -- support bit_xor for date

        DROP FUNCTION IF EXISTS pg_catalog.date_xor_transfn(int16, date);
        CREATE OR REPLACE FUNCTION pg_catalog.date_xor_transfn (
        int16, date
        ) RETURNS int16 LANGUAGE C as '$libdir/dolphin',  'date_xor_transfn';

        DROP FUNCTION IF EXISTS pg_catalog.date_agg_finalfn(int16);
        CREATE OR REPLACE FUNCTION pg_catalog.date_agg_finalfn (
        int16
        ) RETURNS int16 LANGUAGE C as '$libdir/dolphin',  'date_agg_finalfn';

        drop aggregate if exists pg_catalog.bit_xor(date);
        create aggregate pg_catalog.bit_xor(date) (SFUNC=date_xor_transfn, finalfunc = date_agg_finalfn,  STYPE= int16);

        -- support bit_xor aggregate for year

        DROP FUNCTION IF EXISTS pg_catalog.year_xor_transfn(integer, year) ;
        CREATE OR REPLACE FUNCTION pg_catalog.year_xor_transfn (
        integer, year
        ) RETURNS integer LANGUAGE C as '$libdir/dolphin',  'year_xor_transfn';

        drop aggregate if exists pg_catalog.bit_xor(year);
        create aggregate pg_catalog.bit_xor(year) (SFUNC=year_xor_transfn, STYPE= integer);

        -- support bit_xor for datetime and timestamp
        DROP FUNCTION IF EXISTS pg_catalog.timestamp_xor_transfn(int16, timestamp(0) with time zone) ;
        CREATE OR REPLACE FUNCTION pg_catalog.timestamp_xor_transfn (
        int16, timestamp(0) with time zone
        ) RETURNS int16 LANGUAGE C as '$libdir/dolphin',  'timestamp_xor_transfn';

        DROP FUNCTION IF EXISTS pg_catalog.timestamp_agg_finalfn(int16) ;
        CREATE OR REPLACE FUNCTION pg_catalog.timestamp_agg_finalfn (
        int16
        ) RETURNS int16 LANGUAGE C as '$libdir/dolphin',  'timestamp_agg_finalfn';

        drop aggregate if exists pg_catalog.bit_xor(timestamp(0) with time zone);
        create aggregate pg_catalog.bit_xor(timestamp(0) with time zone) (SFUNC=timestamp_xor_transfn, finalfunc=timestamp_agg_finalfn, STYPE=int16);

        -- support bit_xor aggregate for time

        DROP FUNCTION IF EXISTS pg_catalog.time_xor_transfn(int16, time) ;
        CREATE OR REPLACE FUNCTION pg_catalog.time_xor_transfn (
        int16, time
        ) RETURNS int16 LANGUAGE C as '$libdir/dolphin',  'time_xor_transfn';

        create aggregate pg_catalog.bit_xor(time) (SFUNC=time_xor_transfn, STYPE= int16);

        DROP FUNCTION IF EXISTS pg_catalog.timetz_xor_transfn(int16, time with time zone) ;
        CREATE OR REPLACE FUNCTION pg_catalog.timetz_xor_transfn (
        int16, time with time zone
        ) RETURNS int16 LANGUAGE C as '$libdir/dolphin',  'timetz_xor_transfn';

        create aggregate pg_catalog.bit_xor(time with time zone) (SFUNC=timetz_xor_transfn, STYPE= int16);


        DROP FUNCTION IF EXISTS pg_catalog.year_any_value (year, year) ;
        CREATE OR REPLACE FUNCTION pg_catalog.year_any_value (year, year) RETURNS year LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'year_any_value';

        drop aggregate if exists pg_catalog.any_value(year);
        CREATE AGGREGATE pg_catalog.any_value(year) (
                sfunc = year_any_value,
                stype = year
        );

        DROP FUNCTION IF EXISTS pg_catalog.time_int8(time) ;
        CREATE OR REPLACE FUNCTION pg_catalog.time_int8(time) RETURNS int8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'time_int8';
        CREATE CAST (time as int8) with function pg_catalog.time_int8(time);
        CREATE OR REPLACE FUNCTION pg_catalog.time_float (time) RETURNS float8 LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'time_float';
        DROP CAST IF EXISTS (time AS float8) ;
        CREATE CAST(time AS float8) WITH FUNCTION time_float(time) AS IMPLICIT;
        DROP FUNCTION IF EXISTS pg_catalog.time_numeric(time) ;
        CREATE OR REPLACE FUNCTION pg_catalog.time_numeric(time) RETURNS numeric LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'time_numeric';
        CREATE CAST (time as numeric) with function pg_catalog.time_numeric(time);

        CREATE OR REPLACE FUNCTION pg_catalog.time_integer (time) RETURNS integer LANGUAGE SQL STABLE STRICT as 'select cast(pg_catalog.time_float($1) as integer)';
        DROP CAST IF EXISTS (time AS integer) ;
        CREATE CAST(time AS integer) WITH FUNCTION time_integer(time);

        CREATE OR REPLACE FUNCTION pg_catalog.time_pl_float (time, float8) RETURNS float8 LANGUAGE SQL STABLE STRICT as 'select pg_catalog.time_float($1) + $2';
        -- DROP OPERATOR IF EXISTS + (time, float8);
        CREATE OPERATOR pg_catalog.+ (
            PROCEDURE = time_pl_float,
            LEFTARG = time,
            RIGHTARG = float8
        );

        CREATE OR REPLACE FUNCTION pg_catalog.time_mi_float (time, float8) RETURNS float8 LANGUAGE SQL STABLE STRICT as 'select pg_catalog.time_float($1) - $2';
        -- DROP OPERATOR IF EXISTS - (time, float8);
        CREATE OPERATOR pg_catalog.- (
            PROCEDURE = time_mi_float,
            LEFTARG = time,
            RIGHTARG = float8
        );

        CREATE OR REPLACE FUNCTION pg_catalog.datetime_float (timestamp(0) without time zone) RETURNS float8 LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'datetime_float';
        DROP CAST IF EXISTS (timestamp(0) without time zone AS float8) ;
        CREATE CAST(timestamp(0) without time zone AS float8) WITH FUNCTION datetime_float(timestamp(0) without time zone) AS IMPLICIT;

        CREATE OR REPLACE FUNCTION pg_catalog.datetime_bigint (timestamp(0) without time zone) RETURNS bigint LANGUAGE SQL STABLE STRICT as 'select cast(pg_catalog.datetime_float($1) as bigint)';
        DROP CAST IF EXISTS (timestamp(0) without time zone AS bigint) ;
        CREATE CAST(timestamp(0) without time zone AS bigint) WITH FUNCTION datetime_bigint(timestamp(0) without time zone);

        CREATE OR REPLACE FUNCTION pg_catalog.datetime_pl_float (timestamp(0) without time zone, float8) RETURNS float8 LANGUAGE SQL STABLE STRICT as 'select pg_catalog.datetime_float($1) + $2';
        -- DROP OPERATOR IF EXISTS + (timestamp(0) without time zone, float8);
        CREATE OPERATOR pg_catalog.+ (
            PROCEDURE = datetime_pl_float,
            LEFTARG = timestamp(0) without time zone,
            RIGHTARG = float8
        );

        CREATE OR REPLACE FUNCTION pg_catalog.datetime_mi_float (timestamp(0) without time zone, float8) RETURNS float8 LANGUAGE SQL STABLE STRICT as 'select pg_catalog.datetime_float($1) - $2';
        -- DROP OPERATOR IF EXISTS - (timestamp(0) without time zone, float8);
        CREATE OPERATOR pg_catalog.- (
            PROCEDURE = datetime_mi_float,
            LEFTARG = timestamp(0) without time zone,
            RIGHTARG = float8
        );

        CREATE OR REPLACE FUNCTION pg_catalog.date_int (date) RETURNS int4 LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'date_int';
        DROP CAST IF EXISTS (date AS integer) ;
        CREATE CAST(date AS integer) WITH FUNCTION date_int(date);

        -- CREATE OR REPLACE FUNCTION pg_catalog.date_pl_int (date, integer) RETURNS integer LANGUAGE SQL STABLE STRICT as 'select pg_catalog.date_int($1) + $2';
        -- -- DROP OPERATOR + (date, integer);
        -- CREATE OPERATOR + (
        --     PROCEDURE = date_pl_int,
        --     LEFTARG = date,
        --     RIGHTARG = integer
        -- );

        -- CREATE OR REPLACE FUNCTION pg_catalog.date_mi_int (date, integer) RETURNS integer LANGUAGE SQL STABLE STRICT as 'select pg_catalog.date_int($1) - $2';
        -- -- DROP OPERATOR - (date, integer);
        -- CREATE OPERATOR - (
        --     PROCEDURE = date_mi_int,
        --     LEFTARG = date,
        --     RIGHTARG = integer
        -- );

        CREATE OR REPLACE FUNCTION pg_catalog.datexor(
        date,
        date
        ) RETURNS int16 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'datexor';
        create operator pg_catalog.^(leftarg = date, rightarg = date, procedure = pg_catalog.datexor);

        CREATE OR REPLACE FUNCTION pg_catalog.timexor(
        time,
        time
        ) RETURNS int16 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'timexor';
        create operator pg_catalog.^(leftarg = time, rightarg = time, procedure = pg_catalog.timexor);

        CREATE OR REPLACE FUNCTION pg_catalog.date_time_xor(
        date,
        time
        ) RETURNS int16 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'date_time_xor';
        create operator pg_catalog.^(leftarg = date, rightarg = time, procedure = pg_catalog.date_time_xor);

        CREATE OR REPLACE FUNCTION pg_catalog.time_date_xor(
        time,
        date
        ) RETURNS int16 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'time_date_xor';
        create operator pg_catalog.^(leftarg = time, rightarg = date, procedure = pg_catalog.time_date_xor);

        CREATE OR REPLACE FUNCTION pg_catalog.time_text_xor(
        time,
        text
        ) RETURNS int16 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'time_text_xor';
        create operator pg_catalog.^(leftarg = time, rightarg = text, procedure = pg_catalog.time_text_xor);

        CREATE OR REPLACE FUNCTION pg_catalog.text_time_xor(
        text,
        time
        ) RETURNS int16 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'text_time_xor';
        create operator pg_catalog.^(leftarg = text, rightarg = time, procedure = pg_catalog.text_time_xor);

        CREATE OR REPLACE FUNCTION pg_catalog.date_text_xor(
        date,
        text
        ) RETURNS int16 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'date_text_xor';
        create operator pg_catalog.^(leftarg = date, rightarg = text, procedure = pg_catalog.date_text_xor);

        CREATE OR REPLACE FUNCTION pg_catalog.text_date_xor(
        text,
        date
        ) RETURNS int16 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'text_date_xor';
        create operator pg_catalog.^(leftarg = text, rightarg = date, procedure = pg_catalog.text_date_xor);

        CREATE OR REPLACE FUNCTION pg_catalog.date_int8_xor(
        date,
        int8
        ) RETURNS int16 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'date_int8_xor';
        create operator pg_catalog.^(leftarg = date, rightarg = int8, procedure = pg_catalog.date_int8_xor);

        CREATE OR REPLACE FUNCTION pg_catalog.int8_date_xor(
        int8,
        date
        ) RETURNS int16 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'int8_date_xor';
        create operator pg_catalog.^(leftarg = int8, rightarg = date, procedure = pg_catalog.int8_date_xor);

        CREATE OR REPLACE FUNCTION pg_catalog.time_int8_xor(
        time,
        int8
        ) RETURNS int16 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'time_int8_xor';
        create operator pg_catalog.^(leftarg = time, rightarg = int8, procedure = pg_catalog.time_int8_xor);

        CREATE OR REPLACE FUNCTION pg_catalog.int8_time_xor(
        int8,
        time
        ) RETURNS int16 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'int8_time_xor';
        create operator pg_catalog.^(leftarg = int8, rightarg = time, procedure = pg_catalog.int8_time_xor);

        CREATE OR REPLACE FUNCTION pg_catalog.date_float8_xor(
        date,
        float8
        ) RETURNS int16 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'date_float8_xor';
        create operator pg_catalog.^(leftarg = date, rightarg = float8, procedure = pg_catalog.date_float8_xor);

        CREATE OR REPLACE FUNCTION pg_catalog.float8_date_xor(
        float8,
        date
        ) RETURNS int16 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'float8_date_xor';
        create operator pg_catalog.^(leftarg = float8, rightarg = date, procedure = pg_catalog.float8_date_xor);

        CREATE OR REPLACE FUNCTION pg_catalog.timestampxor(
        timestamp without time zone,
        timestamp without time zone
        ) RETURNS int16 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'timestampxor';
        create operator pg_catalog.^(leftarg = timestamp without time zone, rightarg = timestamp without time zone, procedure = pg_catalog.timestampxor);

        CREATE OR REPLACE FUNCTION pg_catalog.timestamp_int8_xor(
        timestamp without time zone,
        int8
        ) RETURNS int16 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'timestamp_int8_xor';
        create operator pg_catalog.^(leftarg = timestamp without time zone, rightarg = int8, procedure = pg_catalog.timestamp_int8_xor);

        CREATE OR REPLACE FUNCTION pg_catalog.int8_timestamp_xor(
        int8,
        timestamp without time zone
        ) RETURNS int16 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'int8_timestamp_xor';
        create operator pg_catalog.^(leftarg = int8, rightarg = timestamp without time zone, procedure = pg_catalog.int8_timestamp_xor);

        CREATE OR REPLACE FUNCTION pg_catalog.timestamp_float8_xor(
        timestamp without time zone,
        float8
        ) RETURNS int16 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'timestamp_float8_xor';
        create operator pg_catalog.^(leftarg = timestamp without time zone, rightarg = float8, procedure = pg_catalog.timestamp_float8_xor);

        CREATE OR REPLACE FUNCTION pg_catalog.float8_timestamp_xor(
        float8,
        timestamp without time zone
        ) RETURNS int16 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'float8_timestamp_xor';
        create operator pg_catalog.^(leftarg = float8, rightarg = timestamp without time zone, procedure = pg_catalog.float8_timestamp_xor);

        CREATE OR REPLACE FUNCTION pg_catalog.timestamp_text_xor(
        timestamp without time zone,
        text
        ) RETURNS int16 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'timestamp_text_xor';
        create operator pg_catalog.^(leftarg = timestamp without time zone, rightarg = text, procedure = pg_catalog.timestamp_text_xor);

        CREATE OR REPLACE FUNCTION pg_catalog.text_timestamp_xor(
        text,
        timestamp without time zone
        ) RETURNS int16 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'text_timestamp_xor';
        create operator pg_catalog.^(leftarg = text, rightarg = timestamp without time zone, procedure = pg_catalog.text_timestamp_xor);

        CREATE OR REPLACE FUNCTION pg_catalog.timestamptzxor(
        timestampTz,
        timestampTz
        ) RETURNS int16 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'timestamptzxor';
        create operator pg_catalog.^(leftarg = timestampTz, rightarg = timestampTz, procedure = pg_catalog.timestamptzxor);

        CREATE OR REPLACE FUNCTION pg_catalog.timestamptz_int8_xor(
        timestampTz,
        int8
        ) RETURNS int16 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'timestamptz_int8_xor';
        create operator pg_catalog.^(leftarg = timestampTz, rightarg = int8, procedure = pg_catalog.timestamptz_int8_xor);

        CREATE OR REPLACE FUNCTION pg_catalog.int8_timestamptz_xor(
        int8,
        timestampTz
        ) RETURNS int16 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'int8_timestamptz_xor';
        create operator pg_catalog.^(leftarg = int8, rightarg = timestampTz, procedure = pg_catalog.int8_timestamptz_xor);

        CREATE OR REPLACE FUNCTION pg_catalog.timestamptz_float8_xor(
        timestampTz,
        float8
        ) RETURNS int16 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'timestamptz_float8_xor';
        create operator pg_catalog.^(leftarg = timestampTz, rightarg = float8, procedure = pg_catalog.timestamptz_float8_xor);

        CREATE OR REPLACE FUNCTION pg_catalog.float8_timestamptz_xor(
        float8,
        timestampTz
        ) RETURNS int16 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'float8_timestamptz_xor';
        create operator pg_catalog.^(leftarg = float8, rightarg = timestampTz, procedure = pg_catalog.float8_timestamptz_xor);

        CREATE OR REPLACE FUNCTION pg_catalog.timestamptz_text_xor(
        timestampTz,
        text
        ) RETURNS int16 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'timestamptz_text_xor';
        create operator pg_catalog.^(leftarg = timestampTz, rightarg = text, procedure = pg_catalog.timestamptz_text_xor);

        CREATE OR REPLACE FUNCTION pg_catalog.text_timestamptz_xor(
        text,
        timestampTz
        ) RETURNS int16 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'text_timestamptz_xor';
        create operator pg_catalog.^(leftarg = text, rightarg = timestampTz, procedure = pg_catalog.text_timestamptz_xor);

        DROP FUNCTION IF EXISTS pg_catalog.sleep(float8) ;
        CREATE OR REPLACE FUNCTION pg_catalog.sleep (float8)
        RETURNS int LANGUAGE C STABLE CALLED ON NULL INPUT as '$libdir/dolphin', 'db_b_sleep';

        DROP FUNCTION IF EXISTS pg_catalog.timetz_int8(timetz) ;
        CREATE OR REPLACE FUNCTION pg_catalog.timetz_int8(timetz) RETURNS int8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'timetz_int8';
        CREATE CAST (timetz as int8) with function pg_catalog.timetz_int8(timetz);

        DROP FUNCTION IF EXISTS pg_catalog.timetz_float8(timetz) ;
        CREATE OR REPLACE FUNCTION pg_catalog.timetz_float8(timetz) RETURNS float8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'timetz_float8';
        CREATE CAST (timetz as float8) with function pg_catalog.timetz_float8(timetz);

        DROP FUNCTION IF EXISTS pg_catalog.timetz_numeric(timetz) ;
        CREATE OR REPLACE FUNCTION pg_catalog.timetz_numeric(timetz) RETURNS numeric LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'timetz_numeric';
        CREATE CAST (timetz as numeric) with function pg_catalog.timetz_numeric(timetz);

        DROP FUNCTION IF EXISTS pg_catalog.timestamp_numeric(timestamp without time zone) ;
        CREATE OR REPLACE FUNCTION pg_catalog.timestamp_numeric(timestamp without time zone) RETURNS numeric LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'timestamp_numeric';
        CREATE CAST (timestamp without time zone as numeric) with function pg_catalog.timestamp_numeric(timestamp without time zone);

        DROP FUNCTION IF EXISTS pg_catalog.timestamptz_int8("timestamptz") ;
        CREATE OR REPLACE FUNCTION pg_catalog.timestamptz_int8("timestamptz") RETURNS int8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'timestamptz_int8';
        CREATE CAST ("timestamptz" as int8) with function pg_catalog.timestamptz_int8("timestamptz");

        DROP FUNCTION IF EXISTS pg_catalog.timestamptz_float8("timestamptz") ;
        CREATE OR REPLACE FUNCTION pg_catalog.timestamptz_float8("timestamptz") RETURNS float8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'timestamptz_float8';
        CREATE CAST ("timestamptz" as float8) with function pg_catalog.timestamptz_float8("timestamptz");

        DROP FUNCTION IF EXISTS pg_catalog.timestamptz_numeric("timestamptz") ;
        CREATE OR REPLACE FUNCTION pg_catalog.timestamptz_numeric("timestamptz") RETURNS numeric LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'timestamptz_numeric';
        CREATE CAST ("timestamptz" as numeric) with function pg_catalog.timestamptz_numeric("timestamptz");

        DROP FUNCTION IF EXISTS pg_catalog.date_int8("date") ;
        CREATE OR REPLACE FUNCTION pg_catalog.date_int8("date") RETURNS int8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'date_int8';
        CREATE CAST ("date" as int8) with function pg_catalog.date_int8("date");

        DROP FUNCTION IF EXISTS pg_catalog.date2float8("date") ;
        CREATE OR REPLACE FUNCTION pg_catalog.date2float8("date") RETURNS float8 LANGUAGE SQL IMMUTABLE STRICT as $$ SELECT date_int8($1)::float8; $$;
        CREATE CAST ("date" as float8) with function pg_catalog.date2float8("date");

        DROP FUNCTION IF EXISTS pg_catalog.date_numeric("date") ;
        CREATE OR REPLACE FUNCTION pg_catalog.date_numeric("date") RETURNS numeric LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'date_numeric';
        CREATE CAST ("date" as numeric) with function pg_catalog.date_numeric("date");

        DROP FUNCTION IF EXISTS pg_catalog.date_any_value (date, date) ;
        CREATE OR REPLACE FUNCTION pg_catalog.date_any_value (date, date) RETURNS date LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'date_any_value';

        DROP FUNCTION IF EXISTS pg_catalog.year_float8("year") ;
        CREATE OR REPLACE FUNCTION pg_catalog.year_float8("year") RETURNS float8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'year_float8';
        drop CAST IF EXISTS (year AS float8) ;
        CREATE CAST ("year" as float8) with function pg_catalog.year_float8("year");

        DROP FUNCTION IF EXISTS pg_catalog.year_numeric("year") ;
        CREATE OR REPLACE FUNCTION pg_catalog.year_numeric("year") RETURNS numeric LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'year_numeric';
        drop CAST IF EXISTS (year AS numeric) ;
        CREATE CAST ("year" as numeric) with function pg_catalog.year_numeric("year");

        drop aggregate if exists pg_catalog.any_value(date);
        CREATE AGGREGATE pg_catalog.any_value(date) (
                sfunc = date_any_value,
                stype = date
        );

        DROP FUNCTION IF EXISTS pg_catalog.time_any_value (time, time) ;
        CREATE OR REPLACE FUNCTION pg_catalog.time_any_value (time, time) RETURNS time LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'time_any_value';

        drop aggregate if exists pg_catalog.any_value(time);
        CREATE AGGREGATE pg_catalog.any_value(time) (
                sfunc = time_any_value,
                stype = time
        );

        DROP FUNCTION IF EXISTS pg_catalog.timetz_any_value (timetz, timetz) ;
        CREATE OR REPLACE FUNCTION pg_catalog.timetz_any_value (timetz, timetz) RETURNS timetz LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'timetz_any_value';

        drop aggregate if exists pg_catalog.any_value(timetz);
        CREATE AGGREGATE pg_catalog.any_value(timetz) (
                sfunc = timetz_any_value,
                stype = timetz
        );

        DROP FUNCTION IF EXISTS pg_catalog.timestamp_any_value (timestamp without time zone, timestamp without time zone) ;
        CREATE OR REPLACE FUNCTION pg_catalog.timestamp_any_value (timestamp without time zone, timestamp without time zone) RETURNS timestamp without time zone LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'timestamp_any_value';

        drop aggregate if exists pg_catalog.any_value(timestamp without time zone);
        CREATE AGGREGATE pg_catalog.any_value(timestamp without time zone) (
                sfunc = timestamp_any_value,
                stype = timestamp without time zone
        );

        DROP FUNCTION IF EXISTS pg_catalog.timestamptz_any_value (timestamptz, timestamptz) ;
        CREATE OR REPLACE FUNCTION pg_catalog.timestamptz_any_value (timestamptz, timestamptz) RETURNS timestamptz LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'timestamptz_any_value';

        drop aggregate if exists pg_catalog.any_value(timestamptz);
        CREATE AGGREGATE pg_catalog.any_value(timestamptz) (
                sfunc = timestamptz_any_value,
                stype = timestamptz
        );
        
        DROP FUNCTION IF EXISTS pg_catalog.json_quote(text);
        DROP FUNCTION IF EXISTS pg_catalog.json_array(variadic "any");
        CREATE OR REPLACE FUNCTION pg_catalog.json_quote(text) RETURNS json LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'json_quote';
        CREATE OR REPLACE FUNCTION pg_catalog.json_array(variadic "any") RETURNS json LANGUAGE C IMMUTABLE as '$libdir/dolphin', 'json_array';
        CREATE OR REPLACE FUNCTION pg_catalog.json_array() RETURNS json LANGUAGE C IMMUTABLE as '$libdir/dolphin', 'json_array';

        DROP FUNCTION IF EXISTS pg_catalog.json_contains(json, json, text);
        DROP FUNCTION IF EXISTS pg_catalog.json_contains(json, json);
        CREATE OR REPLACE FUNCTION pg_catalog.json_contains("any", "any", text) RETURNS boolean LANGUAGE C IMMUTABLE as '$libdir/dolphin', 'json_contains';
        CREATE OR REPLACE FUNCTION pg_catalog.json_contains("any", "any") RETURNS boolean LANGUAGE C IMMUTABLE as '$libdir/dolphin', 'json_contains';

        DROP FUNCTION IF EXISTS pg_catalog.json_contains_path(json, text, variadic text[]);
        CREATE OR REPLACE FUNCTION pg_catalog.json_contains_path("any", text, variadic text[]) RETURNS boolean LANGUAGE C IMMUTABLE as '$libdir/dolphin', 'json_contains_path';
        
        CREATE OR REPLACE FUNCTION pg_catalog.json_extract("any", variadic text[]) RETURNS json LANGUAGE C IMMUTABLE as '$libdir/dolphin',  'json_extract';

        DROP FUNCTION IF EXISTS pg_catalog.json_extract(json, variadic text[]);
        DROP FUNCTION IF EXISTS pg_catalog.json_keys(json);
        DROP FUNCTION IF EXISTS pg_catalog.json_keys(json,text);

        DROP FUNCTION IF EXISTS pg_catalog.json_search(json,text,"any");
        DROP FUNCTION IF EXISTS pg_catalog.json_search(json,text,"any","any");
        DROP FUNCTION IF EXISTS pg_catalog.json_search(json,text,"any","any",variadic text[]);

        DROP FUNCTION IF EXISTS pg_catalog.json_array_append(json, VARIADIC "any");

        DROP FUNCTION IF EXISTS pg_catalog.json_append(json, VARIADIC "any");

        DROP FUNCTION IF EXISTS pg_catalog.json_unquote(text);
        DROP FUNCTION IF EXISTS pg_catalog.json_unquote(json);

        DROP FUNCTION IF EXISTS pg_catalog.json_merge_preserve(variadic json[]);

        DROP FUNCTION IF EXISTS pg_catalog.json_merge(variadic json[]);

        DROP FUNCTION IF EXISTS pg_catalog.json_merge_patch(variadic json[]);

        DROP FUNCTION IF EXISTS pg_catalog.json_insert(variadic "any");

        DROP FUNCTION IF EXISTS pg_catalog.json_depth(json);

        DROP FUNCTION IF EXISTS pg_catalog.json_replace(variadic arr "any");

        DROP FUNCTION IF EXISTS pg_catalog.json_remove(json, variadic "any");

        DROP FUNCTION IF EXISTS pg_catalog.json_array_insert(json, variadic "any");

        DROP FUNCTION IF EXISTS pg_catalog.json_set( variadic "any");

        CREATE OR REPLACE FUNCTION pg_catalog.json_keys("any") RETURNS json LANGUAGE C IMMUTABLE as '$libdir/dolphin','json_keys';
        CREATE OR REPLACE FUNCTION pg_catalog.json_keys("any",text) RETURNS json LANGUAGE C IMMUTABLE as '$libdir/dolphin','json_keys';

        CREATE OR REPLACE FUNCTION pg_catalog.json_search("any",text,"any") RETURNS text LANGUAGE C IMMUTABLE as '$libdir/dolphin', 'json_search';
        CREATE OR REPLACE FUNCTION pg_catalog.json_search("any",text,"any","any") RETURNS text LANGUAGE C IMMUTABLE as '$libdir/dolphin', 'json_search';
        CREATE OR REPLACE FUNCTION pg_catalog.json_search("any",text,"any","any",variadic text[]) RETURNS text LANGUAGE C IMMUTABLE as '$libdir/dolphin', 'json_search';

        CREATE OR REPLACE FUNCTION pg_catalog.json_array_append("any", VARIADIC "any") RETURNS json LANGUAGE C IMMUTABLE as '$libdir/dolphin','json_array_append';

        CREATE OR REPLACE FUNCTION pg_catalog.json_append("any", VARIADIC "any") RETURNS json LANGUAGE C IMMUTABLE as '$libdir/dolphin','json_append';

        CREATE OR REPLACE FUNCTION pg_catalog.json_unquote("any") RETURNS text LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin','json_unquote';

        CREATE OR REPLACE FUNCTION pg_catalog.json_merge_preserve(variadic arr "any") RETURNS json LANGUAGE C IMMUTABLE as '$libdir/dolphin', 'json_merge_preserve';

        CREATE OR REPLACE FUNCTION pg_catalog.json_merge(variadic arr "any") RETURNS json LANGUAGE C IMMUTABLE as '$libdir/dolphin', 'json_merge';

        CREATE OR REPLACE FUNCTION pg_catalog.json_merge_patch(variadic arr "any") RETURNS json LANGUAGE C IMMUTABLE as '$libdir/dolphin', 'json_merge_patch';

        CREATE OR REPLACE FUNCTION pg_catalog.json_insert(variadic "any") RETURNS json LANGUAGE C IMMUTABLE as '$libdir/dolphin', 'json_insert';

        CREATE OR REPLACE FUNCTION pg_catalog.json_depth("any") RETURNS int LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'json_depth';

        CREATE OR REPLACE FUNCTION pg_catalog.json_replace(variadic arr "any") RETURNS json LANGUAGE C IMMUTABLE as '$libdir/dolphin', 'json_replace';

        CREATE OR REPLACE FUNCTION pg_catalog.json_remove(variadic arr "any") RETURNS json LANGUAGE C IMMUTABLE as '$libdir/dolphin', 'json_remove';

        CREATE OR REPLACE FUNCTION pg_catalog.json_array_insert(variadic arr "any") RETURNS json LANGUAGE C IMMUTABLE as '$libdir/dolphin', 'json_array_insert';

        CREATE OR REPLACE FUNCTION pg_catalog.json_set(variadic "any") RETURNS json LANGUAGE C IMMUTABLE as '$libdir/dolphin', 'json_set';

        CREATE OR REPLACE FUNCTION pg_catalog.json_length("any") RETURNS int LANGUAGE C IMMUTABLE as '$libdir/dolphin', 'json_length';
        CREATE OR REPLACE FUNCTION pg_catalog.json_length("any",text) RETURNS int LANGUAGE C IMMUTABLE as '$libdir/dolphin', 'json_length';
        create aggregate pg_catalog.json_arrayagg("any") (SFUNC = json_agg_transfn, STYPE = internal, finalfunc = json_agg_finalfn);

        CREATE OR REPLACE FUNCTION pg_catalog.json_objectagg_mysql_transfn (
            internal, "any", "any"
        ) RETURNS internal LANGUAGE C as '$libdir/dolphin', 'json_objectagg_mysql_transfn';

        CREATE OR REPLACE FUNCTION pg_catalog.json_objectagg_finalfn(
            internal
        ) RETURNS json LANGUAGE C as '$libdir/dolphin', 'json_objectagg_finalfn';

        create aggregate pg_catalog.json_objectagg("any", "any") (SFUNC = json_objectagg_mysql_transfn, STYPE = internal, finalfunc = json_objectagg_finalfn);

        CREATE OR REPLACE FUNCTION pg_catalog.json_valid("any") RETURNS boolean LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'json_valid';

        CREATE OR REPLACE FUNCTION pg_catalog.json_storage_size("any") RETURNS int LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'json_storage_size';

        CREATE OR REPLACE FUNCTION pg_catalog.json_pretty("any") RETURNS json LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'json_pretty';

        CREATE OR REPLACE FUNCTION pg_catalog.json_type("any") RETURNS text LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'json_type';

        DROP CAST IF EXISTS(UINT1 AS TEXT);
        DROP CAST IF EXISTS(UINT1 AS INTERVAL);
        DROP FUNCTION IF EXISTS NUMTODAY(uint1);
        DROP FUNCTION IF EXISTS TO_TEXT(uint1);
        CREATE OR REPLACE FUNCTION pg_catalog.NUMTODAY(uint1)
        RETURNS INTERVAL
        AS $$ SELECT NUMTODSINTERVAL(uint1_numeric($1),'DAY')$$
        LANGUAGE SQL IMMUTABLE STRICT;

        CREATE OR REPLACE FUNCTION pg_catalog.TO_TEXT(UINT1)
        RETURNS TEXT
        AS $$ select CAST(uint1out($1) AS VARCHAR2)  $$
        LANGUAGE SQL  STRICT IMMUTABLE;
        CREATE CAST (UINT1 AS TEXT) WITH FUNCTION TO_TEXT(UINT1) AS IMPLICIT;
        CREATE CAST (UINT1 AS INTERVAL) WITH FUNCTION NUMTODAY(UINT1) AS IMPLICIT;

        DROP OPERATOR CLASS IF EXISTS uint1_ops USING hash;
        DROP OPERATOR CLASS IF EXISTS uint2_ops USING hash;
        DROP OPERATOR CLASS IF EXISTS uint4_ops USING hash;
        DROP OPERATOR CLASS IF EXISTS uint8_ops USING hash;
        DROP OPERATOR CLASS IF EXISTS uint_ops USING btree;
        DROP OPERATOR CLASS IF EXISTS uint2_ops USING btree;
        DROP OPERATOR CLASS IF EXISTS uint4_ops USING btree;
        DROP OPERATOR CLASS IF EXISTS uint8_ops USING btree;

        DROP OPERATOR IF EXISTS pg_catalog.=(uint1,uint1);
        CREATE OPERATOR pg_catalog.=(
        leftarg = uint1, rightarg = uint1, procedure = uint1eq,
        commutator=operator(pg_catalog.=),
        restrict = eqsel, join = eqjoinsel,
        HASHES, MERGES
        );
        
        DROP CAST IF EXISTS (TEXT AS UINT1);
        DROP FUNCTION IF EXISTS uint1(text);
        CREATE OR REPLACE FUNCTION pg_catalog.uint1(text)
        RETURNS uint1
        LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'text_uint1';
        CREATE CAST (TEXT AS UINT1) WITH FUNCTION UINT1(TEXT) AS IMPLICIT;
        
        DROP OPERATOR IF EXISTS pg_catalog.=(int1,uint1);
        CREATE OPERATOR pg_catalog.=(
        leftarg = int1, rightarg = uint1, procedure = int1_uint1_eq,
        commutator=operator(pg_catalog.=),
        restrict = eqsel, join = eqjoinsel,
        HASHES, MERGES
        );
        
        DROP CAST IF EXISTS(uint2 AS INTERVAL);
        DROP CAST IF EXISTS(uint2 AS TEXT);
        DROP FUNCTION IF EXISTS NUMTODAY(uint2);
        DROP FUNCTION IF EXISTS TO_TEXT(uint2);
        CREATE OR REPLACE FUNCTION pg_catalog.NUMTODAY(uint2)
        RETURNS INTERVAL
        AS $$ SELECT NUMTODSINTERVAL(uint2_numeric($1),'DAY')$$
        LANGUAGE SQL IMMUTABLE STRICT;

        CREATE OR REPLACE FUNCTION pg_catalog.TO_TEXT(uint2)
        RETURNS TEXT
        AS $$ select CAST(uint2out($1) AS VARCHAR2)  $$
        LANGUAGE SQL  STRICT IMMUTABLE;
        CREATE CAST (uint2 AS TEXT) WITH FUNCTION TO_TEXT(uint2) AS IMPLICIT;
        CREATE CAST (uint2 AS INTERVAL) WITH FUNCTION NUMTODAY(uint2) AS IMPLICIT;

        DROP OPERATOR IF EXISTS pg_catalog.=(uint2,uint2);
        CREATE OPERATOR pg_catalog.=(
        leftarg = uint2, rightarg = uint2, procedure = uint2eq,
        commutator=operator(pg_catalog.=),
        restrict = eqsel, join = eqjoinsel,
        HASHES, MERGES
        );
        
        DROP CAST IF EXISTS (TEXT AS uint2);
        DROP FUNCTION IF EXISTS uint2(text);
        CREATE OR REPLACE FUNCTION pg_catalog.uint2(text)
        RETURNS uint2
        LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'text_uint2';
        CREATE CAST (TEXT AS uint2) WITH FUNCTION uint2(TEXT) AS IMPLICIT;

        DROP OPERATOR IF EXISTS pg_catalog.=(int2,uint2);
        CREATE OPERATOR pg_catalog.=(
        leftarg = int2, rightarg = uint2, procedure = int2_uint2_eq,
        commutator=operator(pg_catalog.=),
        restrict = eqsel, join = eqjoinsel,
        HASHES, MERGES
        );
        
        DROP CAST IF EXISTS(uint4 AS TEXT);
        DROP CAST IF EXISTS(uint4 AS INTERVAL);
        DROP FUNCTION IF EXISTS NUMTODAY(uint4);
        DROP FUNCTION IF EXISTS TO_TEXT(uint4);
        CREATE OR REPLACE FUNCTION pg_catalog.NUMTODAY(uint4)
        RETURNS INTERVAL
        AS $$ SELECT NUMTODSINTERVAL(uint4_numeric($1),'DAY')$$
        LANGUAGE SQL IMMUTABLE STRICT;
        CREATE OR REPLACE FUNCTION pg_catalog.TO_TEXT(uint4)
        RETURNS TEXT
        AS $$ select CAST(uint4out($1) AS VARCHAR2)  $$
        LANGUAGE SQL  STRICT IMMUTABLE;
        CREATE CAST (uint4 AS TEXT) WITH FUNCTION TO_TEXT(uint4) AS IMPLICIT;
        CREATE CAST (uint4 AS INTERVAL) WITH FUNCTION NUMTODAY(uint4) AS IMPLICIT;
        
        DROP OPERATOR IF EXISTS pg_catalog.=(uint4,uint4);
        CREATE OPERATOR pg_catalog.=(
        leftarg = uint4, rightarg = uint4, procedure = uint4eq,
        commutator=operator(pg_catalog.=),
        restrict = eqsel, join = eqjoinsel,
        HASHES, MERGES
        );
        
        DROP CAST IF EXISTS (TEXT AS uint4);
        DROP FUNCTION IF EXISTS uint4(text);
        CREATE OR REPLACE FUNCTION pg_catalog.uint4(text)
        RETURNS uint4
        LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'text_uint4';
        CREATE CAST (TEXT AS uint4) WITH FUNCTION uint4(TEXT) AS IMPLICIT;

        DROP OPERATOR IF EXISTS pg_catalog.=(int4,uint4);
        CREATE OPERATOR pg_catalog.=(
        leftarg = int4, rightarg = uint4, procedure = int4_uint4_eq,
        commutator=operator(pg_catalog.=),
        restrict = eqsel, join = eqjoinsel,
        HASHES, MERGES
        );
        
        DROP CAST IF EXISTS (uint8 AS TEXT);
        DROP FUNCTION IF EXISTS TO_TEXT(uint8);
        CREATE OR REPLACE FUNCTION pg_catalog.TO_TEXT(uint8)
        RETURNS TEXT
        AS $$ select CAST(uint8out($1) AS VARCHAR2)  $$
        LANGUAGE SQL  STRICT IMMUTABLE;
        CREATE CAST (uint8 AS TEXT) WITH FUNCTION TO_TEXT(uint8) AS IMPLICIT;

        
        DROP OPERATOR IF EXISTS pg_catalog.=(uint8,uint8);
        CREATE OPERATOR pg_catalog.=(
        leftarg = uint8, rightarg = uint8, procedure = uint8eq,
        commutator=operator(pg_catalog.=),
        restrict = eqsel, join = eqjoinsel,
        HASHES, MERGES
        );
        
        DROP CAST IF EXISTS (TEXT AS uint8);
        DROP FUNCTION IF EXISTS uint8(text);
        CREATE OR REPLACE FUNCTION pg_catalog.uint8(text)
        RETURNS uint8
        LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'text_uint8';
        CREATE CAST (TEXT AS uint8) WITH FUNCTION uint8(TEXT) AS IMPLICIT;
        
        DROP OPERATOR IF EXISTS pg_catalog.=(int8,uint8);
        CREATE OPERATOR pg_catalog.=(
        leftarg = int8, rightarg = uint8, procedure = int8_uint8_eq,
        commutator=operator(pg_catalog.=),
        restrict = eqsel, join = eqjoinsel,
        HASHES, MERGES
        );
        
        DROP FUNCTION IF EXISTS pg_catalog.uint8_uint4_eq(uint8,uint4);
        CREATE OR REPLACE FUNCTION pg_catalog.uint8_uint4_eq (
        uint8,uint4
        ) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint8_uint4_eq';

        DROP OPERATOR IF EXISTS pg_catalog.=(uint4,uint8);
        CREATE OPERATOR pg_catalog.=(
        leftarg = uint4, rightarg = uint8, procedure = uint4_uint8_eq,
        restrict = eqsel, join = eqjoinsel,
        commutator=operator(pg_catalog.=),
        HASHES, MERGES
        );
        
        CREATE OPERATOR pg_catalog.=(
        leftarg = uint8, rightarg = uint4, procedure = uint8_uint4_eq,
        restrict = eqsel, join = eqjoinsel,
        commutator=operator(pg_catalog.=),
        HASHES, MERGES
        );
        COMMENT ON OPERATOR pg_catalog.=(uint8, uint4) IS 'uint8_uint4_eq';

        CREATE OPERATOR CLASS uint_ops
            DEFAULT FOR TYPE uint1 USING btree family integer_ops AS
                OPERATOR        1       < ,
                OPERATOR        1       <(uint1, uint2),
                OPERATOR        1       <(uint1, uint4),
                OPERATOR        1       <(uint1, uint8),
                OPERATOR        1       <(uint1, int1),
                OPERATOR        1       <(uint1, int2),
                OPERATOR        1       <(uint1, int4),
                OPERATOR        1       <(uint1, int8),
                OPERATOR        2       <= ,
                OPERATOR        2       <=(uint1, uint2),
                OPERATOR        2       <=(uint1, uint4),
                OPERATOR        2       <=(uint1, uint8),
                OPERATOR        2       <=(uint1, int1),
                OPERATOR        2       <=(uint1, int2),
                OPERATOR        2       <=(uint1, int4),
                OPERATOR        2       <=(uint1, int8),
                OPERATOR        3       = ,
                OPERATOR        3       =(uint1, uint2),
                OPERATOR        3       =(uint1, uint4),
                OPERATOR        3       =(uint1, uint8),
                OPERATOR        3       =(uint1, int1),
                OPERATOR        3       =(uint1, int2),
                OPERATOR        3       =(uint1, int4),
                OPERATOR        3       =(uint1, int8),
                OPERATOR        4       >= ,
                OPERATOR        4       >=(uint1, uint2),
                OPERATOR        4       >=(uint1, uint4),
                OPERATOR        4       >=(uint1, uint8),
                OPERATOR        4       >=(uint1, int1),
                OPERATOR        4       >=(uint1, int2),
                OPERATOR        4       >=(uint1, int4),
                OPERATOR        4       >=(uint1, int8),
                OPERATOR        5       > ,
                OPERATOR        5       >(uint1, uint2),
                OPERATOR        5       >(uint1, uint4),
                OPERATOR        5       >(uint1, uint8),
                OPERATOR        5       >(uint1, int1),
                OPERATOR        5       >(uint1, int2),
                OPERATOR        5       >(uint1, int4),
                OPERATOR        5       >(uint1, int8),
                FUNCTION        1       uint1cmp(uint1, uint1),
                FUNCTION        1       uint12cmp(uint1, uint2),
                FUNCTION        1       uint14cmp(uint1, uint4),
                FUNCTION        1       uint18cmp(uint1, uint8),
                FUNCTION        1       uint1_int1cmp(uint1, int1),
                FUNCTION        1       uint1_int2cmp(uint1, int2),
                FUNCTION        1       uint1_int4cmp(uint1, int4),
                FUNCTION        1       uint1_int8cmp(uint1, int8),
                FUNCTION        2       uint1_sortsupport(internal);

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

        CREATE OPERATOR CLASS uint8_ops
            DEFAULT FOR TYPE uint8 USING btree family integer_ops AS
                OPERATOR        1       < ,
                OPERATOR        1       <(uint8, int8),
                OPERATOR        2       <= ,
                OPERATOR        2       <=(uint8, int8),
                OPERATOR        3       = ,
                OPERATOR        3       =(uint8, int8),
                OPERATOR        4       >= ,
                OPERATOR        4       >=(uint8, int8),
                OPERATOR        5       > ,
                OPERATOR        5       >(uint8, int8),
                FUNCTION        1       uint8cmp(uint8, uint8),
                FUNCTION        1       uint8_int8cmp(uint8, int8),
                FUNCTION        2       uint8_sortsupport(internal);

        CREATE OPERATOR CLASS uint1_ops
            DEFAULT FOR TYPE uint1 USING hash family integer_ops AS
                OPERATOR        1       = ,
                OPERATOR        1       =(uint1, uint2),
                OPERATOR        1       =(uint1, uint4),
                OPERATOR        1       =(uint1, uint8),
                OPERATOR        1       =(uint1, int1),
                OPERATOR        1       =(uint1, int2),
                OPERATOR        1       =(uint1, int4),
                OPERATOR        1       =(uint1, int8),
                OPERATOR        1       =(int1, uint1),
                FUNCTION        1       hashint1(int1),
                FUNCTION        1       hashuint1(uint1);

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

        CREATE OPERATOR CLASS uint8_ops
            DEFAULT FOR TYPE uint8 USING hash family integer_ops AS
                OPERATOR        1       = ,
                OPERATOR        1       =(uint8, int8),
                OPERATOR        1       =(int8, uint8),
                OPERATOR        1       =(uint8, uint4),
                FUNCTION        1       hashuint8(uint8);

        DROP FUNCTION IF EXISTS pg_catalog.i1_cast_ui1(int1);
        CREATE OR REPLACE FUNCTION pg_catalog.i1_cast_ui1 (
        int1
        ) RETURNS uint1 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'i1_cast_ui1';
        DROP FUNCTION IF EXISTS pg_catalog.i1_cast_ui2(int1);
        CREATE OR REPLACE FUNCTION pg_catalog.i1_cast_ui2 (
        int1
        ) RETURNS uint2 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'i1_cast_ui2';
        DROP FUNCTION IF EXISTS pg_catalog.i1_cast_ui4(int1);
        CREATE OR REPLACE FUNCTION pg_catalog.i1_cast_ui4 (
        int1
        ) RETURNS uint4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'i1_cast_ui4';
        DROP FUNCTION IF EXISTS pg_catalog.i1_cast_ui8(int1);
        CREATE OR REPLACE FUNCTION pg_catalog.i1_cast_ui8 (
        int1
        ) RETURNS uint8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'i1_cast_ui8';

        drop CAST IF EXISTS (uint4 AS timestamp(0) without time zone);
        drop CAST IF EXISTS (uint4 AS datetime);
        DROP FUNCTION IF EXISTS pg_catalog.int32_b_format_datetime(uint4);
        CREATE OR REPLACE FUNCTION pg_catalog.int32_b_format_datetime (
        uint4
        ) RETURNS timestamp(0) without time zone LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int32_b_format_datetime';
        CREATE CAST (uint4 AS timestamp(0) without time zone) WITH FUNCTION int32_b_format_datetime(uint4);

        drop CAST IF EXISTS (uint8 AS timestamp(0) without time zone);
        drop CAST IF EXISTS (uint8 AS datetime);
        DROP FUNCTION IF EXISTS pg_catalog.int64_b_format_datetime(uint8);
        CREATE OR REPLACE FUNCTION pg_catalog.int64_b_format_datetime (
        uint8
        ) RETURNS timestamp(0) without time zone LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int64_b_format_datetime';
        CREATE CAST (uint8 AS timestamp(0) without time zone) WITH FUNCTION int64_b_format_datetime(uint8);

        DROP FUNCTION IF EXISTS pg_catalog.year_uint8("year");
        CREATE OR REPLACE FUNCTION pg_catalog.year_uint8("year") RETURNS uint8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'year_uint8';
        drop CAST IF EXISTS (year AS uint8);
        CREATE CAST ("year" as uint8) with function pg_catalog.year_uint8("year");

        drop CAST IF EXISTS (bit AS uint4);
        drop CAST IF EXISTS (bit AS uint8);
        CREATE CAST (bit AS uint4) WITH FUNCTION bittouint4(bit) AS IMPLICIT;
        CREATE CAST (bit AS uint8) WITH FUNCTION bittouint8(bit) AS IMPLICIT;
        
        DROP FUNCTION IF EXISTS pg_catalog.uint2_xor_bool(uint2, boolean);
        CREATE OR REPLACE FUNCTION pg_catalog.uint2_xor_bool (
        uint2, boolean
        ) RETURNS uint2 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint2_xor_bool';

        DROP FUNCTION IF EXISTS pg_catalog.uint1_xor_bool(uint1, boolean);
        CREATE OR REPLACE FUNCTION pg_catalog.uint1_xor_bool (
        uint1, boolean
        ) RETURNS uint1 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint1_xor_bool';

        DROP FUNCTION IF EXISTS pg_catalog.uint4_xor_bool(uint4, boolean);
        CREATE OR REPLACE FUNCTION pg_catalog.uint4_xor_bool (
        uint4, boolean
        ) RETURNS uint4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint4_xor_bool';

        DROP FUNCTION IF EXISTS pg_catalog.uint8_xor_bool(uint8, boolean);
        CREATE OR REPLACE FUNCTION pg_catalog.uint8_xor_bool (
        uint8, boolean
        ) RETURNS uint8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint8_xor_bool';

        DROP FUNCTION IF EXISTS pg_catalog.bool_xor_uint1(boolean, uint1);
        CREATE OR REPLACE FUNCTION pg_catalog.bool_xor_uint1 (
        boolean, uint1
        ) RETURNS uint1 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'bool_xor_uint1';

        DROP FUNCTION IF EXISTS pg_catalog.bool_xor_uint2(boolean, uint2);
        CREATE OR REPLACE FUNCTION pg_catalog.bool_xor_uint2 (
        boolean, uint2
        ) RETURNS uint2 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'bool_xor_uint2';

        DROP FUNCTION IF EXISTS pg_catalog.bool_xor_uint4(boolean, uint4);
        CREATE OR REPLACE FUNCTION pg_catalog.bool_xor_uint4 (
        boolean, uint4
        ) RETURNS uint4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'bool_xor_uint4';

        DROP FUNCTION IF EXISTS pg_catalog.bool_xor_uint8(boolean, uint8);
        CREATE OR REPLACE FUNCTION pg_catalog.bool_xor_uint8 (
        boolean, uint8
        ) RETURNS uint8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'bool_xor_uint8';

        CREATE OPERATOR pg_catalog.^(
        leftarg = uint8, rightarg = boolean, procedure = uint8_xor_bool,
        commutator=operator(pg_catalog.^)
        );
        COMMENT ON OPERATOR pg_catalog.^(uint8, boolean) IS 'uint8_xor_bool';
        CREATE OPERATOR pg_catalog.^(
        leftarg = uint4, rightarg = boolean, procedure = uint4_xor_bool,
        commutator=operator(pg_catalog.^)
        );
        COMMENT ON OPERATOR pg_catalog.^(uint4, boolean) IS 'uint4_xor_bool';
        CREATE OPERATOR pg_catalog.^(
        leftarg = uint2, rightarg = boolean, procedure = uint2_xor_bool,
        commutator=operator(pg_catalog.^)
        );
        COMMENT ON OPERATOR pg_catalog.^(uint2, boolean) IS 'uint2_xor_bool';
        CREATE OPERATOR pg_catalog.^(
        leftarg = uint1, rightarg = boolean, procedure = uint1_xor_bool,
        commutator=operator(pg_catalog.^)
        );
        COMMENT ON OPERATOR pg_catalog.^(uint1, boolean) IS 'uint1_xor_bool';
        CREATE OPERATOR pg_catalog.^(
        leftarg = boolean, rightarg = uint1, procedure = bool_xor_uint1,
        commutator=operator(pg_catalog.^)
        );
        COMMENT ON OPERATOR pg_catalog.^(boolean, uint1) IS 'bool_xor_uint1';
        CREATE OPERATOR pg_catalog.^(
        leftarg = boolean, rightarg = uint2, procedure = bool_xor_uint2,
        commutator=operator(pg_catalog.^)
        );
        COMMENT ON OPERATOR pg_catalog.^(boolean, uint2) IS 'bool_xor_uint2';
        CREATE OPERATOR pg_catalog.^(
        leftarg = boolean, rightarg = uint4, procedure = bool_xor_uint4,
        commutator=operator(pg_catalog.^)
        );
        COMMENT ON OPERATOR pg_catalog.^(boolean, uint4) IS 'bool_xor_uint4';
        CREATE OPERATOR pg_catalog.^(
        leftarg = boolean, rightarg = uint8, procedure = bool_xor_uint8,
        commutator=operator(pg_catalog.^)
        );
        COMMENT ON OPERATOR pg_catalog.^(boolean, uint8) IS 'bool_xor_uint8';

        DROP FUNCTION IF EXISTS pg_catalog.timestamp_uint8(timestamp(0) with time zone);
        CREATE OR REPLACE FUNCTION pg_catalog.timestamp_uint8(timestamp(0) with time zone)
        RETURNS uint8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'timestamp_uint8';
        drop CAST IF EXISTS (timestamp(0) with time zone AS uint8);
        CREATE CAST (timestamp(0) with time zone AS uint8) WITH FUNCTION timestamp_uint8(timestamp(0) with time zone);

        DROP FUNCTION IF EXISTS pg_catalog.cash_uint(money);
        CREATE OR REPLACE FUNCTION pg_catalog.cash_uint(money)
        RETURNS uint8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'cash_uint';
        drop CAST IF EXISTS (money AS uint8);
        CREATE CAST (money AS uint8) WITH FUNCTION cash_uint(money) AS ASSIGNMENT;

        drop aggregate if exists pg_catalog.bit_xor(text);
        drop aggregate if exists pg_catalog.any_value(uint8);
        drop aggregate if exists pg_catalog.bit_xor(int8);
        drop aggregate if exists pg_catalog.bit_xor(varbit);

        DROP FUNCTION IF EXISTS pg_catalog.int8_xor(uint8,int8);
        CREATE OR REPLACE FUNCTION pg_catalog.int8_xor (
        uint8,int8
        ) RETURNS uint8 LANGUAGE C as '$libdir/dolphin',  'uint8_xor_int8';
        create aggregate pg_catalog.bit_xor(int8) (SFUNC=int8_xor, STYPE= uint8);

        DROP FUNCTION IF EXISTS pg_catalog.uint8_xor(uint8,int8);
        CREATE OR REPLACE FUNCTION pg_catalog.uint8_xor (
        uint8,int8
        ) RETURNS uint8 LANGUAGE C as '$libdir/dolphin',  'uint8xor';

        drop aggregate if exists pg_catalog.bit_xor(text);
        DROP FUNCTION IF EXISTS pg_catalog.text_xor(uint8,text);
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
        create aggregate pg_catalog.bit_xor(text) (SFUNC=text_xor, STYPE= uint8);

        DROP FUNCTION IF EXISTS pg_catalog.uint_any_value (uint8, uint8);
        CREATE OR REPLACE FUNCTION pg_catalog.uint_any_value (uint8, uint8) RETURNS uint8 LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'uint_any_value';
        CREATE AGGREGATE pg_catalog.any_value(uint8) (
                sfunc = uint_any_value,
                stype = uint8
        );

        DROP FUNCTION IF EXISTS pg_catalog.bit_count(numeric) ;
        CREATE OR REPLACE FUNCTION pg_catalog.bit_count (numeric)  RETURNS int8 LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'bit_count_numeric';
        DROP FUNCTION IF EXISTS pg_catalog.bit_count(text) ;
        CREATE OR REPLACE FUNCTION pg_catalog.bit_count (text)  RETURNS int8 LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'bit_count_text';
        DROP FUNCTION IF EXISTS pg_catalog.bit_count_bit(text) ;
        CREATE OR REPLACE FUNCTION pg_catalog.bit_count_bit (text) RETURNS int8 LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'bit_count_bit';
        DROP FUNCTION IF EXISTS pg_catalog.bit_count(bit) ;
        CREATE OR REPLACE FUNCTION pg_catalog.bit_count (bit) RETURNS int8 AS $$ SELECT pg_catalog.bit_count_bit(cast($1 as text)) $$ LANGUAGE SQL;

        -- support bit_xor aggregate for varbit

        DROP FUNCTION IF EXISTS pg_catalog.varbit_bit_xor_transfn(varbit, varbit) ;
        CREATE FUNCTION pg_catalog.varbit_bit_xor_transfn (
        t1 varbit ,t2 varbit
        ) RETURNS varbit AS
        $$
        BEGIN
            IF t1 is null THEN
                RETURN t2;
            ELSEIF t2 is null THEN
                RETURN t1;
            ELSE
                RETURN (SELECT bitxor(t1, t2));
            END IF;
        END;
        $$
        LANGUAGE plpgsql;

        DROP FUNCTION IF EXISTS pg_catalog.varbit_bit_xor_finalfn(varbit) ;
        CREATE FUNCTION pg_catalog.varbit_bit_xor_finalfn (
        t varbit
        ) RETURNS int8 AS
        $$
        BEGIN
            IF t is null THEN
                RETURN 0;
            ELSE
                RETURN (SELECT int8(t));
            END IF;
        END;
        $$
        LANGUAGE plpgsql;

        create aggregate pg_catalog.bit_xor(varbit) (SFUNC=varbit_bit_xor_transfn, finalfunc = varbit_bit_xor_finalfn ,STYPE= varbit);

        DROP FUNCTION IF EXISTS pg_catalog.connection_id();

        CREATE OR REPLACE FUNCTION pg_catalog.connection_id()
        RETURNS int8 AS
        $$
        BEGIN
            RETURN (select pg_backend_pid());
        END;
        $$
        LANGUAGE plpgsql;

        DROP FUNCTION IF EXISTS pg_catalog.set_native_password(text, text);
        CREATE OR REPLACE FUNCTION pg_catalog.set_native_password(text, text) RETURNS text LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'set_native_password';
        create or replace function pg_catalog.b_plpgsql_call_handler() returns language_handler language C volatile as '$libdir/dolphin', 'b_plpgsql_call_handler';
        create or replace function pg_catalog.b_plpgsql_inline_handler(internal) returns void language C strict volatile as '$libdir/dolphin', 'b_plpgsql_inline_handler';
        create or replace function pg_catalog.b_plpgsql_validator(oid) returns void language C strict volatile as '$libdir/dolphin', 'b_plpgsql_validator';

        DROP FUNCTION IF EXISTS pg_catalog.enum2float8(anyenum);
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

        DROP FUNCTION IF EXISTS pg_catalog.enumtextlt(anyenum, text); 
        CREATE FUNCTION pg_catalog.enumtextlt ( 
        anyenum, text 
        ) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'enumtextlt';
        DROP FUNCTION IF EXISTS pg_catalog.enumtextle(anyenum, text); 
        CREATE FUNCTION pg_catalog.enumtextle ( 
        anyenum, text 
        ) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'enumtextle';
        DROP FUNCTION IF EXISTS pg_catalog.enumtextne(anyenum, text); 
        CREATE FUNCTION pg_catalog.enumtextne ( 
        anyenum, text 
        ) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'enumtextne';
        DROP FUNCTION IF EXISTS pg_catalog.enumtexteq(anyenum, text); 
        CREATE FUNCTION pg_catalog.enumtexteq ( 
        anyenum, text 
        ) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'enumtexteq';
        DROP FUNCTION IF EXISTS pg_catalog.enumtextgt(anyenum, text); 
        CREATE FUNCTION pg_catalog.enumtextgt ( 
        anyenum, text 
        ) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'enumtextgt';
        DROP FUNCTION IF EXISTS pg_catalog.enumtextge(anyenum, text); 
        CREATE FUNCTION pg_catalog.enumtextge ( 
        anyenum, text 
        ) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'enumtextge';
        DROP FUNCTION IF EXISTS pg_catalog.textenumlt(text, anyenum); 
        CREATE FUNCTION pg_catalog.textenumlt ( 
        text, anyenum 
        ) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'textenumlt';
        DROP FUNCTION IF EXISTS pg_catalog.textenumle(text, anyenum); 
        CREATE FUNCTION pg_catalog.textenumle ( 
        text, anyenum 
        ) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'textenumle';
        DROP FUNCTION IF EXISTS pg_catalog.textenumne(text, anyenum); 
        CREATE FUNCTION pg_catalog.textenumne ( 
        text, anyenum 
        ) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'textenumne';
        DROP FUNCTION IF EXISTS pg_catalog.textenumeq(text, anyenum); 
        CREATE FUNCTION pg_catalog.textenumeq ( 
        text, anyenum 
        ) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'textenumeq';
        DROP FUNCTION IF EXISTS pg_catalog.textenumgt(text, anyenum); 
        CREATE FUNCTION pg_catalog.textenumgt ( 
        text, anyenum 
        ) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'textenumgt';
        DROP FUNCTION IF EXISTS pg_catalog.textenumge(text, anyenum); 
        CREATE FUNCTION pg_catalog.textenumge ( 
        text, anyenum 
        ) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'textenumge';

        CREATE OPERATOR pg_catalog.=( 
         PROCEDURE = enumtexteq, LEFTARG = anyenum, RIGHTARG = text, COMMUTATOR = OPERATOR(pg_catalog.=), NEGATOR = OPERATOR(pg_catalog.<>), RESTRICT = eqsel, JOIN = eqjoinsel, HASHES
        );

        CREATE OPERATOR pg_catalog.<>( 
         PROCEDURE = enumtextne, LEFTARG = anyenum, RIGHTARG = text, COMMUTATOR = OPERATOR(pg_catalog.<>), NEGATOR = OPERATOR(pg_catalog.=), RESTRICT = neqsel, JOIN = neqjoinsel
        );

        CREATE OPERATOR pg_catalog.<( 
         PROCEDURE = enumtextlt, LEFTARG = anyenum, RIGHTARG = text, COMMUTATOR = OPERATOR(pg_catalog.>), NEGATOR = OPERATOR(pg_catalog.>=), RESTRICT = scalarltsel, JOIN = scalarltjoinsel
        );

        CREATE OPERATOR pg_catalog.>( 
         PROCEDURE = enumtextgt, LEFTARG = anyenum, RIGHTARG = text, COMMUTATOR = OPERATOR(pg_catalog.<), NEGATOR = OPERATOR(pg_catalog.<=), RESTRICT = scalargtsel, JOIN = scalargtjoinsel
        );

        CREATE OPERATOR pg_catalog.<=( 
         PROCEDURE = enumtextle, LEFTARG = anyenum, RIGHTARG = text, COMMUTATOR = OPERATOR(pg_catalog.>=), NEGATOR = OPERATOR(pg_catalog.>), RESTRICT = scalarltsel, JOIN = scalarltjoinsel
        );

        CREATE OPERATOR pg_catalog.>=( 
         PROCEDURE = enumtextge, LEFTARG = anyenum, RIGHTARG = text, COMMUTATOR = OPERATOR(pg_catalog.<=), NEGATOR = OPERATOR(pg_catalog.<), RESTRICT = scalargtsel, JOIN = scalargtjoinsel
        );

        CREATE OPERATOR pg_catalog.=( 
         PROCEDURE = textenumeq, LEFTARG = text, RIGHTARG = anyenum, COMMUTATOR = OPERATOR(pg_catalog.=), NEGATOR = OPERATOR(pg_catalog.<>), RESTRICT = eqsel, JOIN = eqjoinsel, HASHES
        );

        CREATE OPERATOR pg_catalog.<>( 
         PROCEDURE = textenumne, LEFTARG = text, RIGHTARG = anyenum, COMMUTATOR = OPERATOR(pg_catalog.<>), NEGATOR = OPERATOR(pg_catalog.=), RESTRICT = neqsel, JOIN = neqjoinsel
        );

        CREATE OPERATOR pg_catalog.<( 
         PROCEDURE = textenumlt, LEFTARG = text, RIGHTARG = anyenum, COMMUTATOR = OPERATOR(pg_catalog.>), NEGATOR = OPERATOR(pg_catalog.>=), RESTRICT = scalarltsel, JOIN = scalarltjoinsel
        );

        CREATE OPERATOR pg_catalog.>( 
         PROCEDURE = textenumgt, LEFTARG = text, RIGHTARG = anyenum, COMMUTATOR = OPERATOR(pg_catalog.<), NEGATOR = OPERATOR(pg_catalog.<=), RESTRICT = scalargtsel, JOIN = scalargtjoinsel
        );

        CREATE OPERATOR pg_catalog.<=( 
         PROCEDURE = textenumle, LEFTARG = text, RIGHTARG = anyenum, COMMUTATOR = OPERATOR(pg_catalog.>=), NEGATOR = OPERATOR(pg_catalog.>), RESTRICT = scalarltsel, JOIN = scalarltjoinsel
        );

        CREATE OPERATOR pg_catalog.>=( 
         PROCEDURE = textenumge, LEFTARG = text, RIGHTARG = anyenum, COMMUTATOR = OPERATOR(pg_catalog.<=), NEGATOR = OPERATOR(pg_catalog.<), RESTRICT = scalargtsel, JOIN = scalargtjoinsel
        );
        
        DROP FUNCTION IF EXISTS pg_catalog.from_base64 (text);
        DROP FUNCTION IF EXISTS pg_catalog.from_base64 (bool);
        DROP FUNCTION IF EXISTS pg_catalog.from_base64 (bit);
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
        
        DROP FUNCTION IF EXISTS pg_catalog.oct(text);
        DROP FUNCTION IF EXISTS pg_catalog.oct(boolean);
        DROP FUNCTION IF EXISTS pg_catalog.oct(bit);

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

        DROP FUNCTION IF EXISTS pg_catalog.schema ();

        CREATE OR REPLACE FUNCTION pg_catalog.schema ()
        RETURNS text
        AS
        $$
        BEGIN
            RETURN (SELECT CURRENT_SCHEMA());
        END;
        $$
        LANGUAGE plpgsql;
        
        CREATE OR REPLACE FUNCTION pg_catalog.show_status(is_session boolean)
        RETURNS TABLE (VARIABLE_NAME text, VALUE text) AS
        $$
        DECLARE sql_mode text;
        BEGIN
        EXECUTE IMMEDIATE 'show dolphin.sql_mode' into sql_mode;
        set dolphin.sql_mode='ansi_quotes,pipes_as_concat';
        RETURN QUERY
        select 'os_runtime_count' as variable_name , count(*)::text as value from dbe_perf.global_os_runtime
        union
        select concat(nodename, '-', memorytype) as variable_name, memorymbytes::text as value from dbe_perf.global_memory_node_detail
        union
        select unnest(array['p80', 'p95']) as variable_name, unnest(array[p80, p95]::text[]) as value from dbe_perf.statement_responsetime_percentile
        union
        select 'global_instance_time_count' as variable_name, count(*)::text as value from dbe_perf.global_instance_time
        union
        select unnest(array['node_name', 'worker_info', 'session_info', 'stream_info']) as variable_name, unnest(array[node_name, worker_info, session_info, stream_info]) as value from dbe_perf.global_threadpool_status
        union
        select unnest(array['group_id', 'bind_numa_id', 'bind_cpu_number', 'listener']) as variable_name, unnest(array[group_id, bind_numa_id, bind_cpu_number, listener]::text[]) as value from dbe_perf.global_threadpool_status
        union
        select 'os_threads_count' as variable_name,count(*)::text as value from dbe_perf.global_os_threads
        union
        select unnest(array['node_name', 'user_name']) as variable_name, unnest(array[node_name, user_name]) as value from dbe_perf.summary_user_login
        union
        select unnest(array['login_counter', 'logout_counter', 'user_id']) as variable_name, unnest(array[login_counter, logout_counter, user_id]::text[]) as value from dbe_perf.summary_user_login
        union
        select unnest(array['select_count', 'update_count', 'insert_count', 'delete_count', 'ddl_count', 'dml_count', 'dcl_count']) as variable_name, unnest(array[select_count, update_count, insert_count, delete_count, ddl_count, dml_count, dcl_count]::text[]) as value from dbe_perf.summary_workload_sql_count
        union
        select unnest(array['node_name', 'workload']) as variable_name, unnest(array[node_name, workload]::text[]) as value from dbe_perf.global_workload_transaction
        union
        select unnest(array['commit_counter', 'rollback_counter', 'resp_min', 'resp_max', 'resp_avg', 'resp_total', 'bg_commit_counter', 'bg_rollback_counter', 'bg_resp_min', 'bg_resp_max', 'bg_resp_avg', 'bg_resp_total']) as variable_name,
        unnest(array[commit_counter, rollback_counter, resp_min, resp_max, resp_avg, resp_total, bg_commit_counter, bg_rollback_counter, bg_resp_min, bg_resp_max, bg_resp_avg, bg_resp_total]::text[]) as value from dbe_perf.global_workload_transaction
        union
        select 'node_name' as variable_name, node_name::text as value from dbe_perf.global_bgwriter_stat
        union
        select 'stats_reset' as variable_name, stats_reset::text as value from dbe_perf.global_bgwriter_stat
        union
        select unnest(array['checkpoint_write_time', 'checkpoint_sync_time']) as variable_name, unnest(array[checkpoint_write_time, checkpoint_sync_time]::text[]) as value from dbe_perf.global_bgwriter_stat
        union
        select unnest(array['checkpoints_timed', 'checkpoints_req', 'buffers_checkpoint', 'buffers_clean', 'maxwritten_clean', 'buffers_backend', 'buffers_backend_fsync', 'buffers_alloc']) as variable_name, unnest(array[checkpoints_timed, checkpoints_req, buffers_checkpoint, buffers_clean, maxwritten_clean, buffers_backend, buffers_backend_fsync, buffers_alloc]::text[]) as value from dbe_perf.global_bgwriter_stat
        union
        select unnest(array['phyrds', 'phywrts', 'phyblkrd', 'phyblkwrt']) as variable_name, unnest(array[phyrds, phywrts, phyblkrd, phyblkwrt]::text[]) as value from dbe_perf.summary_rel_iostat
        union
        select 'summary_file_iostat_count' as variable_name, count(*)::text as value from dbe_perf.summary_file_iostat
        union
        select unnest(array['phywrts', 'phyblkwrt', 'writetim', 'avgiotim', 'lstiotim', 'miniotim', 'maxiowtm']) as variable_name, unnest(array[phywrts, phyblkwrt, writetim, avgiotim, lstiotim, miniotim, maxiowtm]::text[]) as value from dbe_perf.summary_file_redo_iostat
        union
        select unnest(array['node_name', 'worker_info']) as variable_name, unnest(array[node_name, worker_info]::text[]) as value from dbe_perf.global_redo_status union select unnest(array['redo_start_ptr', 'redo_start_time', 'redo_done_time', 'curr_time', 'min_recovery_point', 'read_ptr', 'last_replayed_read_ptr', 'recovery_done_ptr', 'read_xlog_io_counter', 'read_xlog_io_total_dur', 'read_data_io_counter', 'read_data_io_total_dur', 'write_data_io_counter', 'write_data_io_total_dur', 'process_pending_counter', 'process_pending_total_dur', 'apply_counter', 'apply_total_dur', 'speed', 'local_max_ptr', 'primary_flush_ptr']) as variable_name, unnest(array[redo_start_ptr, redo_start_time, redo_done_time, curr_time, min_recovery_point, read_ptr, last_replayed_read_ptr, recovery_done_ptr, read_xlog_io_counter, read_xlog_io_total_dur, read_data_io_counter, read_data_io_total_dur, write_data_io_counter, write_data_io_total_dur, process_pending_counter, process_pending_total_dur, apply_counter, apply_total_dur, speed, local_max_ptr, primary_flush_ptr]::text[]) as value from dbe_perf.global_redo_status
        union
        select unnest(array['node_name', 'ckpt_redo_point']) as variable_name, unnest(array[node_name, ckpt_redo_point]::text[]) as value from dbe_perf.global_ckpt_status union select unnest(array['ckpt_clog_flush_num', 'ckpt_csnlog_flush_num', 'ckpt_multixact_flush_num', 'ckpt_predicate_flush_num', 'ckpt_twophase_flush_num']) as variable_name, unnest(array[ckpt_clog_flush_num, ckpt_csnlog_flush_num, ckpt_multixact_flush_num, ckpt_predicate_flush_num, ckpt_twophase_flush_num]::text[]) as value from dbe_perf.global_ckpt_status
        union
        select unnest(array['databaseid', 'tablespaceid', 'relfilenode']) as variable_name, unnest(array[databaseid, tablespaceid, relfilenode]::text[]) as value from dbe_perf.summary_stat_bad_block
        union
        select unnest(array['forknum', 'error_count']) as variable_name, unnest(array[forknum, error_count]::text[]) as value from dbe_perf.summary_stat_bad_block
        union
        select unnest(array['first_time', 'last_time']) as variable_name, unnest(array[first_time, last_time]::text[]) as value from dbe_perf.summary_stat_bad_block
        union
        select unnest(array['node_name', 'queue_head_page_rec_lsn', 'queue_rec_lsn', 'current_xlog_insert_lsn', 'ckpt_redo_point']) as variable_name, unnest(array[node_name, queue_head_page_rec_lsn, queue_rec_lsn, current_xlog_insert_lsn, ckpt_redo_point]) as value from dbe_perf.global_pagewriter_status
        union
        select unnest(array['pgwr_actual_flush_total_num', 'pgwr_last_flush_num', 'remain_dirty_page_num']) as variable_name, unnest(array[pgwr_actual_flush_total_num, pgwr_last_flush_num, remain_dirty_page_num]::text[]) as value from dbe_perf.global_pagewriter_status
        union
        select unnest(array['confl_tablespace', 'confl_lock', 'confl_snapshot', 'confl_bufferpin', 'confl_deadlock']) as variable_name, unnest(array[confl_tablespace, confl_lock, confl_snapshot, confl_bufferpin, confl_deadlock]::text[]) as value from dbe_perf.summary_stat_database_conflicts where datname = current_database()
        union
        select 'wait_events_count' as variable_name, count(*)::text as value from dbe_perf.global_wait_events
        union
        select 'locks_count' as variable_name,count(*)::text as value from pg_locks where database in (select oid from pg_database where datname = current_database())
        union
        select 'datname' as variable_name, datname::text as value from pg_stat_database where datname = current_database()
        union
        select unnest(array['numbackends', 'xact_commit', 'xact_rollback', 'blks_read', 'blks_hit', 'tup_returned', 'tup_fetched', 'tup_inserted', 'tup_updated', 'tup_deleted', 'conflicts', 'temp_files', 'temp_bytes', 'deadlocks', 'datid']) as variable_name, unnest(array[numbackends, xact_commit, xact_rollback, blks_read, blks_hit, tup_returned, tup_fetched, tup_inserted, tup_updated, tup_deleted, conflicts, temp_files, temp_bytes, deadlocks, datid]::text[]) as value from pg_stat_database where datname = current_database()
        union
        select unnest(array['blk_read_time', 'blk_write_time']) as variable_name, unnest(array[blk_read_time, blk_write_time]::text[]) as value from pg_stat_database where datname = current_database()
        union
        select 'stats_reset' as variable_name, stats_reset::text as value from pg_stat_database where datname = current_database()
        union
        select unnest(array['curr_dwn', 'curr_start_page', 'file_trunc_num', 'file_reset_num', 'total_writes', 'low_threshold_writes', 'high_threshold_writes', 'total_pages', 'low_threshold_pages', 'high_threshold_pages', 'file_id']) as variable_name, unnest(array[curr_dwn, curr_start_page, file_trunc_num, file_reset_num, total_writes, low_threshold_writes, high_threshold_writes, total_pages, low_threshold_pages, high_threshold_pages, file_id]::text[]) as value from dbe_perf.global_double_write_status
        union
        select unnest(array['active', 'dummy_standby']) as variable_name, unnest(array[active, dummy_standby]::text[]) as value from dbe_perf.global_replication_slots
        union
        select 'datoid' as variable_name, datoid::text as value from dbe_perf.global_replication_slots
        union
        select unnest(array['node_name', 'database']) as variable_name, unnest(array[node_name, database]::text[]) as value from dbe_perf.global_replication_slots
        union
        select unnest(array['slot_name', 'plugin', 'slot_type', 'restart_lsn']) as variable_name, unnest(array[slot_name, plugin, slot_type, restart_lsn]) as value from dbe_perf.global_replication_slots
        union
        select unnest(array['x_min', 'catalog_xmin']) as variable_name, unnest(array[x_min, catalog_xmin]::text[]) as value from dbe_perf.global_replication_slots
        union
        select unnest(array['node_name', 'usename']) as variable_name, unnest(array[node_name, usename]::text[]) as value from dbe_perf.global_replication_stat
        union
        select unnest(array['pid', 'client_port', 'sync_priority']) as variable_name, unnest(array[pid, client_port, sync_priority]) as value from dbe_perf.global_replication_stat
        union
        select 'usesysid' as variable_name, usesysid::text as value from dbe_perf.global_replication_stat
        union
        select 'backend_start' as variable_name, backend_start::text as value from dbe_perf.global_replication_stat
        union
        select 'client_addr' as variable_name, client_addr::text as value from dbe_perf.global_replication_stat
        union
        select unnest(array['application_name', 'client_hostname', 'state', 'sender_sent_location', 'receiver_write_location', 'receiver_flush_location', 'receiver_replay_location', 'sync_state']) as variable_name, unnest(array[application_name, client_hostname, state, sender_sent_location, receiver_write_location, receiver_flush_location, receiver_replay_location, sync_state]) as value from dbe_perf.global_replication_stat
        union
        select unnest(array['owner', 'database']) as variable_name, unnest(array[owner, database]::text[]) as value from dbe_perf.summary_transactions_prepared_xacts
        union
        select 'transaction' as variable_name, transaction::text as value from dbe_perf.summary_transactions_prepared_xacts
        union
        select 'gid' as variable_name, gid as value from dbe_perf.summary_transactions_prepared_xacts
        union
        select 'prepared' as variable_name, prepared::text as value from dbe_perf.summary_transactions_prepared_xacts order by variable_name;
        EXECUTE IMMEDIATE 'set dolphin.sql_mode=''' || sql_mode || '''';
        END;
        $$
        LANGUAGE plpgsql;
        
        CREATE OPERATOR pg_catalog.+(
        leftarg = int1, rightarg = int1, procedure = int1pl,
        commutator=operator(pg_catalog.+)
        );
        COMMENT ON OPERATOR pg_catalog.+(int1, int1) IS 'int1pl';

        CREATE OPERATOR pg_catalog.-(
        leftarg = int1, rightarg = int1, procedure = int1mi,
        commutator=operator(pg_catalog.-)
        );
        COMMENT ON OPERATOR pg_catalog.-(int1, int1) IS 'int1mi';

        CREATE OPERATOR pg_catalog.*(
        leftarg = int1, rightarg = int1, procedure = int1mul,
        commutator=operator(pg_catalog.*)
        );
        COMMENT ON OPERATOR pg_catalog.*(int1, int1) IS 'int1mul';

        CREATE OPERATOR pg_catalog.%(
        leftarg = int1, rightarg = int1, procedure = int1mod,
        commutator=operator(pg_catalog.%)
        );
        COMMENT ON OPERATOR pg_catalog.%(int1, int1) IS 'int1mod';

        CREATE OPERATOR pg_catalog.&(
        leftarg = int1, rightarg = int1, procedure = int1and,
        commutator=operator(pg_catalog.&)
        );
        COMMENT ON OPERATOR pg_catalog.&(int1, int1) IS 'int1and';

        CREATE OPERATOR pg_catalog.|(
        leftarg = int1, rightarg = int1, procedure = int1or,
        commutator=operator(pg_catalog.|)
        );
        COMMENT ON OPERATOR pg_catalog.|(int1, int1) IS 'int1or';

        CREATE OPERATOR pg_catalog.#(
        leftarg = int1, rightarg = int1, procedure = int1xor,
        commutator=operator(pg_catalog.#)
        );
        COMMENT ON OPERATOR pg_catalog.#(int1, int1) IS 'int1xor';

        CREATE OPERATOR pg_catalog.>>(
        leftarg = int1, rightarg = int4, procedure = int1shr
        );
        COMMENT ON OPERATOR pg_catalog.>>(int1, int4) IS 'int1shr';

        CREATE OPERATOR pg_catalog.<<(
        leftarg = int1, rightarg = int4, procedure = int1shl
        );
        COMMENT ON OPERATOR pg_catalog.<<(int1, int4) IS 'int1shl';

        CREATE OPERATOR pg_catalog.~(
        rightarg = int1, procedure = int1not
        );

        CREATE OPERATOR pg_catalog.+(
        rightarg = int1, procedure = int1up
        );

        CREATE OPERATOR pg_catalog.-(
        rightarg = int1, procedure = int1um
        );

        CREATE OPERATOR pg_catalog.@(
        rightarg = int1, procedure = int1abs
        );

        DROP FUNCTION IF EXISTS pg_catalog.int1_accum(numeric[], int1) ;
        CREATE OR REPLACE FUNCTION pg_catalog.int1_accum(numeric[], int1)
         RETURNS numeric[] LANGUAGE C STRICT AS  '$libdir/dolphin',  'int1_accum';

        drop aggregate if exists pg_catalog.stddev_pop(int1);
        create aggregate pg_catalog.stddev_pop(int1) (SFUNC=int1_accum, cFUNC=numeric_collect, STYPE= numeric[], finalfunc = numeric_stddev_pop, initcond = '{0,0,0}', initcollect = '{0,0,0}');

        DROP FUNCTION IF EXISTS pg_catalog.int1_list_agg_transfn(internal, int1, text);

        CREATE OR REPLACE FUNCTION pg_catalog.int1_list_agg_transfn(internal, int1, text)
         RETURNS internal LANGUAGE C AS    '$libdir/dolphin',    'int1_list_agg_transfn';

        DROP FUNCTION IF EXISTS pg_catalog.int1_list_agg_noarg2_transfn(internal, int1);

        CREATE OR REPLACE FUNCTION pg_catalog.int1_list_agg_noarg2_transfn(internal, int1)
         RETURNS internal LANGUAGE C  AS '$libdir/dolphin',    'int1_list_agg_noarg2_transfn';

        drop aggregate if exists pg_catalog.listagg(int1,text);
        create aggregate pg_catalog.listagg(int1,text) (SFUNC=int1_list_agg_transfn, finalfunc = list_agg_finalfn,  STYPE= internal);

        drop aggregate if exists pg_catalog.listagg(int1);
        create aggregate pg_catalog.listagg(int1) (SFUNC=int1_list_agg_noarg2_transfn, finalfunc = list_agg_finalfn,  STYPE= internal);

        drop aggregate if exists pg_catalog.var_pop(int1);
        create aggregate pg_catalog.var_pop(int1) (SFUNC=int1_accum, cFUNC=numeric_collect, STYPE= numeric[], finalfunc = numeric_var_pop, initcond = '{0,0,0}', initcollect = '{0,0,0}');

        drop aggregate if exists pg_catalog.var_samp(int1);
        create aggregate pg_catalog.var_samp(int1) (SFUNC=int1_accum, cFUNC=numeric_collect, STYPE= numeric[], finalfunc = numeric_var_samp, initcond = '{0,0,0}', initcollect = '{0,0,0}');

        drop aggregate if exists pg_catalog.variance(int1);
        create aggregate pg_catalog.variance(int1) (SFUNC=int1_accum, cFUNC=numeric_collect, STYPE= numeric[], finalfunc = numeric_var_samp, initcond = '{0,0,0}', initcollect = '{0,0,0}');

        CREATE OR REPLACE FUNCTION pg_catalog.int12eq (
        int1,int2
        ) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int12eq';

        CREATE OR REPLACE FUNCTION pg_catalog.int12lt (
        int1,int2
        ) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int12lt';

        CREATE OR REPLACE FUNCTION pg_catalog.int12le (
        int1,int2
        ) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int12le';

        CREATE OR REPLACE FUNCTION pg_catalog.int12gt (
        int1,int2
        ) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int12gt';

        CREATE OR REPLACE FUNCTION pg_catalog.int12ge (
        int1,int2
        ) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int12ge';

        CREATE OR REPLACE FUNCTION pg_catalog.int14eq (
        int1,int4
        ) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int14eq';

        CREATE OR REPLACE FUNCTION pg_catalog.int14lt (
        int1,int4
        ) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int14lt';

        CREATE OR REPLACE FUNCTION pg_catalog.int14le (
        int1,int4
        ) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int14le';

        CREATE OR REPLACE FUNCTION pg_catalog.int14gt (
        int1,int4
        ) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int14gt';

        CREATE OR REPLACE FUNCTION pg_catalog.int14ge (
        int1,int4
        ) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int14ge';

        CREATE OR REPLACE FUNCTION pg_catalog.int18eq (
        int1,int8
        ) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int18eq';

        CREATE OR REPLACE FUNCTION pg_catalog.int18lt (
        int1,int8
        ) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int18lt';

        CREATE OR REPLACE FUNCTION pg_catalog.int18le (
        int1,int8
        ) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int18le';

        CREATE OR REPLACE FUNCTION pg_catalog.int18gt (
        int1,int8
        ) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int18gt';

        CREATE OR REPLACE FUNCTION pg_catalog.int18ge (
        int1,int8
        ) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int18ge';

        CREATE OPERATOR pg_catalog.=(
        leftarg = int1, rightarg = int2, procedure = int12eq,
        restrict = eqsel, join = eqjoinsel,
        HASHES, MERGES
        );
        COMMENT ON OPERATOR pg_catalog.=(int1, int2) IS 'int12eq';

        CREATE OPERATOR pg_catalog.<(
        leftarg = int1, rightarg = int2, procedure = int12lt,
        restrict = scalarltsel, join = scalarltjoinsel
        );
        COMMENT ON OPERATOR pg_catalog.<(int1, int2) IS 'int12lt';

        CREATE OPERATOR pg_catalog.<=(
        leftarg = int1, rightarg = int2, procedure = int12le,
        restrict = scalarltsel, join = scalarltjoinsel
        );
        COMMENT ON OPERATOR pg_catalog.<=(int1, int2) IS 'int12le';

        CREATE OPERATOR pg_catalog.>(
        leftarg = int1, rightarg = int2, procedure = int12gt,
        restrict = scalargtsel, join = scalargtjoinsel
        );
        COMMENT ON OPERATOR pg_catalog.>(int1, int2) IS 'int12gt';

        CREATE OPERATOR pg_catalog.>=(
        leftarg = int1, rightarg = int2, procedure = int12ge,
        restrict = scalargtsel, join = scalargtjoinsel
        );
        COMMENT ON OPERATOR pg_catalog.>=(int1, int2) IS 'int12ge';

        CREATE OPERATOR pg_catalog.=(
        leftarg = int1, rightarg = int4, procedure = int14eq,
        restrict = eqsel, join = eqjoinsel,
        HASHES, MERGES
        );
        COMMENT ON OPERATOR pg_catalog.=(int1, int4) IS 'int14eq';

        CREATE OPERATOR pg_catalog.<(
        leftarg = int1, rightarg = int4, procedure = int14lt,
        restrict = scalarltsel, join = scalarltjoinsel
        );
        COMMENT ON OPERATOR pg_catalog.<(int1, int4) IS 'int14lt';

        CREATE OPERATOR pg_catalog.<=(
        leftarg = int1, rightarg = int4, procedure = int14le,
        restrict = scalarltsel, join = scalarltjoinsel
        );
        COMMENT ON OPERATOR pg_catalog.<=(int1, int4) IS 'int14le';

        CREATE OPERATOR pg_catalog.>(
        leftarg = int1, rightarg = int4, procedure = int14gt,
        restrict = scalargtsel, join = scalargtjoinsel
        );
        COMMENT ON OPERATOR pg_catalog.>(int1, int4) IS 'int14gt';

        CREATE OPERATOR pg_catalog.>=(
        leftarg = int1, rightarg = int4, procedure = int14ge,
        restrict = scalargtsel, join = scalargtjoinsel
        );
        COMMENT ON OPERATOR pg_catalog.>=(int1, int4) IS 'int14ge';

        CREATE OPERATOR pg_catalog.=(
        leftarg = int1, rightarg = int8, procedure = int18eq,
        restrict = eqsel, join = eqjoinsel,
        HASHES, MERGES
        );
        COMMENT ON OPERATOR pg_catalog.=(int1, int8) IS 'int18eq';

        CREATE OPERATOR pg_catalog.<(
        leftarg = int1, rightarg = int8, procedure = int18lt,
        restrict = scalarltsel, join = scalarltjoinsel
        );
        COMMENT ON OPERATOR pg_catalog.<(int1, int8) IS 'int18lt';

        CREATE OPERATOR pg_catalog.<=(
        leftarg = int1, rightarg = int8, procedure = int18le,
        restrict = scalarltsel, join = scalarltjoinsel
        );
        COMMENT ON OPERATOR pg_catalog.<=(int1, int8) IS 'int18le';

        CREATE OPERATOR pg_catalog.>(
        leftarg = int1, rightarg = int8, procedure = int18gt,
        restrict = scalargtsel, join = scalargtjoinsel
        );
        COMMENT ON OPERATOR pg_catalog.>(int1, int8) IS 'int18gt';

        CREATE OPERATOR pg_catalog.>=(
        leftarg = int1, rightarg = int8, procedure = int18ge,
        restrict = scalargtsel, join = scalargtjoinsel
        );
        COMMENT ON OPERATOR pg_catalog.>=(int1, int8) IS 'int18ge';

        CREATE OR REPLACE FUNCTION pg_catalog.int12cmp (
        int1,int2
        ) RETURNS int4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int12cmp';

        CREATE OR REPLACE FUNCTION pg_catalog.int14cmp (
        int1,int4
        ) RETURNS int4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int14cmp';

        CREATE OR REPLACE FUNCTION pg_catalog.int18cmp (
        int1,int8
        ) RETURNS int4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int18cmp';

        CREATE OPERATOR CLASS int1_ops
            FOR TYPE int1 USING btree family int1_ops AS
                OPERATOR        1       <(int1, int2),
                OPERATOR        1       <(int1, int4),
                OPERATOR        1       <(int1, int8),
                OPERATOR        2       <=(int1, int2),
                OPERATOR        2       <=(int1, int4),
                OPERATOR        2       <=(int1, int8),
                OPERATOR        3       =(int1, int2),
                OPERATOR        3       =(int1, int4),
                OPERATOR        3       =(int1, int8),
                OPERATOR        4       >=(int1, int2),
                OPERATOR        4       >=(int1, int4),
                OPERATOR        4       >=(int1, int8),
                OPERATOR        5       >(int1, int2),
                OPERATOR        5       >(int1, int4),
                OPERATOR        5       >(int1, int8),
                FUNCTION        1       int12cmp(int1, int2),
                FUNCTION        1       int14cmp(int1, int4),
                FUNCTION        1       int18cmp(int1, int8);

        CREATE OPERATOR CLASS int1_ops
            FOR TYPE int1 USING cbtree family int1_ops AS
                OPERATOR        1       <(int1, int2),
                OPERATOR        1       <(int1, int4),
                OPERATOR        1       <(int1, int8),
                OPERATOR        2       <=(int1, int2),
                OPERATOR        2       <=(int1, int4),
                OPERATOR        2       <=(int1, int8),
                OPERATOR        3       =(int1, int2),
                OPERATOR        3       =(int1, int4),
                OPERATOR        3       =(int1, int8),
                OPERATOR        4       >=(int1, int2),
                OPERATOR        4       >=(int1, int4),
                OPERATOR        4       >=(int1, int8),
                OPERATOR        5       >(int1, int2),
                OPERATOR        5       >(int1, int4),
                OPERATOR        5       >(int1, int8),
                FUNCTION        1       int12cmp(int1, int2),
                FUNCTION        1       int14cmp(int1, int4),
                FUNCTION        1       int18cmp(int1, int8);

        CREATE OPERATOR CLASS int1_ops
            FOR TYPE int1 USING ubtree family int1_ops AS
                OPERATOR        1       <(int1, int2),
                OPERATOR        1       <(int1, int4),
                OPERATOR        1       <(int1, int8),
                OPERATOR        2       <=(int1, int2),
                OPERATOR        2       <=(int1, int4),
                OPERATOR        2       <=(int1, int8),
                OPERATOR        3       =(int1, int2),
                OPERATOR        3       =(int1, int4),
                OPERATOR        3       =(int1, int8),
                OPERATOR        4       >=(int1, int2),
                OPERATOR        4       >=(int1, int4),
                OPERATOR        4       >=(int1, int8),
                OPERATOR        5       >(int1, int2),
                OPERATOR        5       >(int1, int4),
                OPERATOR        5       >(int1, int8),
                FUNCTION        1       int12cmp(int1, int2),
                FUNCTION        1       int14cmp(int1, int4),
                FUNCTION        1       int18cmp(int1, int8);

        CREATE OPERATOR CLASS int1_ops
            FOR TYPE int1 USING hash family int1_ops AS
                OPERATOR        1       =(int1, int2),
                OPERATOR        1       =(int1, int4),
                OPERATOR        1       =(int1, int8),
                FUNCTION        1       hashint2(int2),
                FUNCTION        1       hashint4(int4),
                FUNCTION        1       hashint8(int8);

        DROP FUNCTION IF EXISTS pg_catalog.to_base64 (bytea);
        DROP FUNCTION IF EXISTS pg_catalog.to_base64 (numeric);
        DROP FUNCTION IF EXISTS pg_catalog.to_base64 (text);
        DROP FUNCTION IF EXISTS pg_catalog.to_base64 (bit);
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
        
        DROP FUNCTION IF EXISTS pg_catalog.unhex (text) ;
        DROP FUNCTION IF EXISTS pg_catalog.unhex (numeric) ;
        DROP FUNCTION IF EXISTS pg_catalog.unhex (bool) ;
        DROP FUNCTION IF EXISTS pg_catalog.unhex (bytea) ;
        DROP FUNCTION IF EXISTS pg_catalog.unhex ("timestamp") ;

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
        
        CREATE SCHEMA dolphin_catalog;

        /* int4 */
        create function dolphin_catalog.dolphin_int4pl (
        int4,
        int4
        ) RETURNS int8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'dolphin_int4pl';
        create operator dolphin_catalog.+(leftarg = int4, rightarg = int4, procedure = dolphin_catalog.dolphin_int4pl);

        create function dolphin_catalog.dolphin_int4mi (
        int4,
        int4
        ) RETURNS int8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'dolphin_int4mi';
        create operator dolphin_catalog.-(leftarg = int4, rightarg = int4, procedure = dolphin_catalog.dolphin_int4mi);

        create function dolphin_catalog.dolphin_int4mul (
        int4,
        int4
        ) RETURNS int8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'dolphin_int4mul';
        create operator dolphin_catalog.*(leftarg = int4, rightarg = int4, procedure = dolphin_catalog.dolphin_int4mul);

        create function dolphin_catalog.dolphin_int4div (
        int4,
        int4
        ) RETURNS numeric LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'dolphin_int4div';
        create operator dolphin_catalog./(leftarg = int4, rightarg = int4, procedure = dolphin_catalog.dolphin_int4div);

        /* int2 */
        create function dolphin_catalog.dolphin_int2pl (
        int2,
        int2
        ) RETURNS int4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'dolphin_int2pl';
        create operator dolphin_catalog.+(leftarg = int2, rightarg = int2, procedure = dolphin_catalog.dolphin_int2pl);

        create function dolphin_catalog.dolphin_int2mi (
        int2,
        int2
        ) RETURNS int4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'dolphin_int2mi';
        create operator dolphin_catalog.-(leftarg = int2, rightarg = int2, procedure = dolphin_catalog.dolphin_int2mi);

        create function dolphin_catalog.dolphin_int2mul (
        int2,
        int2
        ) RETURNS int8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'dolphin_int2mul';
        create operator dolphin_catalog.*(leftarg = int2, rightarg = int2, procedure = dolphin_catalog.dolphin_int2mul);

        create function dolphin_catalog.dolphin_int2div (
        int2,
        int2
        ) RETURNS numeric LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'dolphin_int2div';
        create operator dolphin_catalog./(leftarg = int2, rightarg = int2, procedure = dolphin_catalog.dolphin_int2div);

        /* int1 */
        create function dolphin_catalog.dolphin_int1pl (
        int1,
        int1
        ) RETURNS int4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'dolphin_int1pl';
        create operator dolphin_catalog.+(leftarg = int1, rightarg = int1, procedure = dolphin_catalog.dolphin_int1pl);

        create function dolphin_catalog.dolphin_int1mi (
        int1,
        int1
        ) RETURNS int4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'dolphin_int1mi';
        create operator dolphin_catalog.-(leftarg = int1, rightarg = int1, procedure = dolphin_catalog.dolphin_int1mi);

        create function dolphin_catalog.dolphin_int1mul (
        int1,
        int1
        ) RETURNS int4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'dolphin_int1mul';
        create operator dolphin_catalog.*(leftarg = int1, rightarg = int1, procedure = dolphin_catalog.dolphin_int1mul);

        create function dolphin_catalog.dolphin_int1div (
        int1,
        int1
        ) RETURNS numeric LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'dolphin_int1div';
        create operator dolphin_catalog./(leftarg = int1, rightarg = int1, procedure = dolphin_catalog.dolphin_int1div);

        CREATE OPERATOR pg_catalog./(leftarg = int1, rightarg = int1, procedure = dolphin_catalog.dolphin_int1div);
        COMMENT ON OPERATOR pg_catalog./(int1, int1) IS 'dolphin_int1div';

        /* int8 */
        create function dolphin_catalog.dolphin_int8pl (
        int8,
        int8
        ) RETURNS int8 LANGUAGE INTERNAL IMMUTABLE STRICT as 'int8pl';
        create operator dolphin_catalog.+(leftarg = int8, rightarg = int8, procedure = dolphin_catalog.dolphin_int8pl);

        create function dolphin_catalog.dolphin_int8mi (
        int8,
        int8
        ) RETURNS int8 LANGUAGE INTERNAL IMMUTABLE STRICT as 'int8mi';
        create operator dolphin_catalog.-(leftarg = int8, rightarg = int8, procedure = dolphin_catalog.dolphin_int8mi);

        create function dolphin_catalog.dolphin_int8mul (
        int8,
        int8
        ) RETURNS int8 LANGUAGE INTERNAL IMMUTABLE STRICT as 'int8mul';
        create operator dolphin_catalog.*(leftarg = int8, rightarg = int8, procedure = dolphin_catalog.dolphin_int8mul);

        create function dolphin_catalog.dolphin_int8div (
        int8,
        int8
        ) RETURNS numeric LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'dolphin_int8div';
        create operator dolphin_catalog./(leftarg = int8, rightarg = int8, procedure = dolphin_catalog.dolphin_int8div);

        /* uint1 */
        create function dolphin_catalog.dolphin_uint1pl (
        uint1,
        uint1
        ) RETURNS uint4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'dolphin_uint1pl';
        create operator dolphin_catalog.+(leftarg = uint1, rightarg = uint1, procedure = dolphin_catalog.dolphin_uint1pl);

        create function dolphin_catalog.dolphin_uint1mi (
        uint1,
        uint1
        ) RETURNS uint4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'dolphin_uint1mi';
        create operator dolphin_catalog.-(leftarg = uint1, rightarg = uint1, procedure = dolphin_catalog.dolphin_uint1mi);

        create function dolphin_catalog.dolphin_uint1mul (
        uint1,
        uint1
        ) RETURNS uint4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'dolphin_uint1mul';
        create operator dolphin_catalog.*(leftarg = uint1, rightarg = uint1, procedure = dolphin_catalog.dolphin_uint1mul);

        create function dolphin_catalog.dolphin_uint1div (
        uint1,
        uint1
        ) RETURNS numeric LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'dolphin_uint1div';
        create operator dolphin_catalog./(leftarg = uint1, rightarg = uint1, procedure = dolphin_catalog.dolphin_uint1div);

        /* int1_uint1 */
        create function dolphin_catalog.dolphin_int1_pl_uint1 (
        int1,
        uint1
        ) RETURNS uint4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'dolphin_int1_pl_uint1';
        create operator dolphin_catalog.+(leftarg = int1, rightarg = uint1, procedure = dolphin_catalog.dolphin_int1_pl_uint1);

        create function dolphin_catalog.dolphin_int1_mi_uint1 (
        int1,
        uint1
        ) RETURNS uint4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'dolphin_int1_mi_uint1';
        create operator dolphin_catalog.-(leftarg = int1, rightarg = uint1, procedure = dolphin_catalog.dolphin_int1_mi_uint1);

        create function dolphin_catalog.dolphin_int1_mul_uint1 (
        int1,
        uint1
        ) RETURNS uint4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'dolphin_int1_mul_uint1';
        create operator dolphin_catalog.*(leftarg = int1, rightarg = uint1, procedure = dolphin_catalog.dolphin_int1_mul_uint1);

        create function dolphin_catalog.dolphin_int1_div_uint1 (
        int1,
        uint1
        ) RETURNS numeric LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'dolphin_int1_div_uint1';
        create operator dolphin_catalog./(leftarg = int1, rightarg = uint1, procedure = dolphin_catalog.dolphin_int1_div_uint1);

        /* uint1_int1 */
        create function dolphin_catalog.dolphin_uint1_pl_int1 (
        uint1,
        int1
        ) RETURNS uint4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'dolphin_uint1_pl_int1';
        create operator dolphin_catalog.+(leftarg = uint1, rightarg = int1, procedure = dolphin_catalog.dolphin_uint1_pl_int1);

        create function dolphin_catalog.dolphin_uint1_mi_int1 (
        uint1,
        int1
        ) RETURNS uint4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'dolphin_uint1_mi_int1';
        create operator dolphin_catalog.-(leftarg = uint1, rightarg = int1, procedure = dolphin_catalog.dolphin_uint1_mi_int1);

        create function dolphin_catalog.dolphin_uint1_mul_int1 (
        uint1,
        int1
        ) RETURNS uint4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'dolphin_uint1_mul_int1';
        create operator dolphin_catalog.*(leftarg = uint1, rightarg = int1, procedure = dolphin_catalog.dolphin_uint1_mul_int1);

        create function dolphin_catalog.dolphin_uint1_div_int1 (
        uint1,
        int1
        ) RETURNS numeric LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'dolphin_uint1_div_int1';
        create operator dolphin_catalog./(leftarg = uint1, rightarg = int1, procedure = dolphin_catalog.dolphin_uint1_div_int1);

        /* int2_int4 */
        create function dolphin_catalog.dolphin_int24pl (
        int2,
        int4
        ) RETURNS int8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'dolphin_int24pl';
        create operator dolphin_catalog.+(leftarg = int2, rightarg = int4, procedure = dolphin_catalog.dolphin_int24pl);

        create function dolphin_catalog.dolphin_int24mi (
        int2,
        int4
        ) RETURNS int8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'dolphin_int24mi';
        create operator dolphin_catalog.-(leftarg = int2, rightarg = int4, procedure = dolphin_catalog.dolphin_int24mi);

        create function dolphin_catalog.dolphin_int24mul (
        int2,
        int4
        ) RETURNS int8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'dolphin_int24mul';
        create operator dolphin_catalog.*(leftarg = int2, rightarg = int4, procedure = dolphin_catalog.dolphin_int24mul);

        create function dolphin_catalog.dolphin_int24div (
        int2,
        int4
        ) RETURNS numeric LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'dolphin_int24div';
        create operator dolphin_catalog./(leftarg = int2, rightarg = int4, procedure = dolphin_catalog.dolphin_int24div);

        /* int2_int8 */
        create function dolphin_catalog.dolphin_int28pl (
        int2,
        int8
        ) RETURNS int8 LANGUAGE INTERNAL IMMUTABLE STRICT as 'int28pl';
        create operator dolphin_catalog.+(leftarg = int2, rightarg = int8, procedure = dolphin_catalog.dolphin_int28pl);

        create function dolphin_catalog.dolphin_int28mi (
        int2,
        int8
        ) RETURNS int8 LANGUAGE INTERNAL IMMUTABLE STRICT as 'int28mi';
        create operator dolphin_catalog.-(leftarg = int2, rightarg = int8, procedure = dolphin_catalog.dolphin_int28mi);

        create function dolphin_catalog.dolphin_int28mul (
        int2,
        int8
        ) RETURNS int8 LANGUAGE INTERNAL IMMUTABLE STRICT as 'int28mul';
        create operator dolphin_catalog.*(leftarg = int2, rightarg = int8, procedure = dolphin_catalog.dolphin_int28mul);

        create function dolphin_catalog.dolphin_int28div (
        int2,
        int8
        ) RETURNS numeric LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'dolphin_int28div';
        create operator dolphin_catalog./(leftarg = int2, rightarg = int8, procedure = dolphin_catalog.dolphin_int28div);

        /* int2_uint2 */
        create function dolphin_catalog.dolphin_int2_pl_uint2 (
        int2,
        uint2
        ) RETURNS uint4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'dolphin_int2_pl_uint2';
        create operator dolphin_catalog.+(leftarg = int2, rightarg = uint2, procedure = dolphin_catalog.dolphin_int2_pl_uint2);

        create function dolphin_catalog.dolphin_int2_mi_uint2 (
        int2,
        uint2
        ) RETURNS uint4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'dolphin_int2_mi_uint2';
        create operator dolphin_catalog.-(leftarg = int2, rightarg = uint2, procedure = dolphin_catalog.dolphin_int2_mi_uint2);

        create function dolphin_catalog.dolphin_int2_mul_uint2 (
        int2,
        uint2
        ) RETURNS uint8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'dolphin_int2_mul_uint2';
        create operator dolphin_catalog.*(leftarg = int2, rightarg = uint2, procedure = dolphin_catalog.dolphin_int2_mul_uint2);

        create function dolphin_catalog.dolphin_int2_div_uint2 (
        int2,
        uint2
        ) RETURNS numeric LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'dolphin_int2_div_uint2';
        create operator dolphin_catalog./(leftarg = int2, rightarg = uint2, procedure = dolphin_catalog.dolphin_int2_div_uint2);

        /* int4_int2 */
        create function dolphin_catalog.dolphin_int42pl (
        int4,
        int2
        ) RETURNS int8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'dolphin_int42pl';
        create operator dolphin_catalog.+(leftarg = int4, rightarg = int2, procedure = dolphin_catalog.dolphin_int42pl);

        create function dolphin_catalog.dolphin_int42mi (
        int4,
        int2
        ) RETURNS int8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'dolphin_int42mi';
        create operator dolphin_catalog.-(leftarg = int4, rightarg = int2, procedure = dolphin_catalog.dolphin_int42mi);

        create function dolphin_catalog.dolphin_int42mul (
        int4,
        int2
        ) RETURNS int8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'dolphin_int42mul';
        create operator dolphin_catalog.*(leftarg = int4, rightarg = int2, procedure = dolphin_catalog.dolphin_int42mul);

        create function dolphin_catalog.dolphin_int42div (
        int4,
        int2
        ) RETURNS numeric LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'dolphin_int42div';
        create operator dolphin_catalog./(leftarg = int4, rightarg = int2, procedure = dolphin_catalog.dolphin_int42div);

        /* int4_int8 */
        create function dolphin_catalog.dolphin_int48pl (
        int4,
        int8
        ) RETURNS int8 LANGUAGE INTERNAL IMMUTABLE STRICT as 'int48pl';
        create operator dolphin_catalog.+(leftarg = int4, rightarg = int8, procedure = dolphin_catalog.dolphin_int48pl);

        create function dolphin_catalog.dolphin_int48mi (
        int4,
        int8
        ) RETURNS int8 LANGUAGE INTERNAL IMMUTABLE STRICT as 'int48mi';
        create operator dolphin_catalog.-(leftarg = int4, rightarg = int8, procedure = dolphin_catalog.dolphin_int48mi);

        create function dolphin_catalog.dolphin_int48mul (
        int4,
        int8
        ) RETURNS int8 LANGUAGE INTERNAL IMMUTABLE STRICT as 'int48mul';
        create operator dolphin_catalog.*(leftarg = int4, rightarg = int8, procedure = dolphin_catalog.dolphin_int48mul);

        create function dolphin_catalog.dolphin_int48div (
        int4,
        int8
        ) RETURNS numeric LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'dolphin_int48div';
        create operator dolphin_catalog./(leftarg = int4, rightarg = int8, procedure = dolphin_catalog.dolphin_int48div);

        /* int4_uint4 */
        create function dolphin_catalog.dolphin_int4_pl_uint4 (
        int4,
        uint4
        ) RETURNS uint8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'dolphin_int4_pl_uint4';
        create operator dolphin_catalog.+(leftarg = int4, rightarg = uint4, procedure = dolphin_catalog.dolphin_int4_pl_uint4);

        create function dolphin_catalog.dolphin_int4_mi_uint4 (
        int4,
        uint4
        ) RETURNS uint8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'dolphin_int4_mi_uint4';
        create operator dolphin_catalog.-(leftarg = int4, rightarg = uint4, procedure = dolphin_catalog.dolphin_int4_mi_uint4);

        create function dolphin_catalog.dolphin_int4_mul_uint4 (
        int4,
        uint4
        ) RETURNS uint8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'dolphin_int4_mul_uint4';
        create operator dolphin_catalog.*(leftarg = int4, rightarg = uint4, procedure = dolphin_catalog.dolphin_int4_mul_uint4);

        create function dolphin_catalog.dolphin_int4_div_uint4 (
        int4,
        uint4
        ) RETURNS numeric LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'dolphin_int4_div_uint4';
        create operator dolphin_catalog./(leftarg = int4, rightarg = uint4, procedure = dolphin_catalog.dolphin_int4_div_uint4);

        /* int8_int2 */
        create function dolphin_catalog.dolphin_int82pl (
        int8,
        int2
        ) RETURNS int8 LANGUAGE INTERNAL IMMUTABLE STRICT as 'int82pl';
        create operator dolphin_catalog.+(leftarg = int8, rightarg = int2, procedure = dolphin_catalog.dolphin_int82pl);

        create function dolphin_catalog.dolphin_int82mi (
        int8,
        int2
        ) RETURNS int8 LANGUAGE INTERNAL IMMUTABLE STRICT as 'int82mi';
        create operator dolphin_catalog.-(leftarg = int8, rightarg = int2, procedure = dolphin_catalog.dolphin_int82mi);

        create function dolphin_catalog.dolphin_int82mul (
        int8,
        int2
        ) RETURNS int8 LANGUAGE INTERNAL IMMUTABLE STRICT as 'int82mul';
        create operator dolphin_catalog.*(leftarg = int8, rightarg = int2, procedure = dolphin_catalog.dolphin_int82mul);

        create function dolphin_catalog.dolphin_int82div (
        int8,
        int2
        ) RETURNS numeric LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'dolphin_int82div';
        create operator dolphin_catalog./(leftarg = int8, rightarg = int2, procedure = dolphin_catalog.dolphin_int82div);

        /* int8_int4 */
        create function dolphin_catalog.dolphin_int84pl (
        int8,
        int4
        ) RETURNS int8 LANGUAGE INTERNAL IMMUTABLE STRICT as 'int84pl';
        create operator dolphin_catalog.+(leftarg = int8, rightarg = int4, procedure = dolphin_catalog.dolphin_int84pl);

        create function dolphin_catalog.dolphin_int84mi (
        int8,
        int4
        ) RETURNS int8 LANGUAGE INTERNAL IMMUTABLE STRICT as 'int84mi';
        create operator dolphin_catalog.-(leftarg = int8, rightarg = int4, procedure = dolphin_catalog.dolphin_int84mi);

        create function dolphin_catalog.dolphin_int84mul (
        int8,
        int4
        ) RETURNS int8 LANGUAGE INTERNAL IMMUTABLE STRICT as 'int84mul';
        create operator dolphin_catalog.*(leftarg = int8, rightarg = int4, procedure = dolphin_catalog.dolphin_int84mul);

        create function dolphin_catalog.dolphin_int84div (
        int8,
        int4
        ) RETURNS numeric LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'dolphin_int84div';
        create operator dolphin_catalog./(leftarg = int8, rightarg = int4, procedure = dolphin_catalog.dolphin_int84div);

        /* uint2_int2 */
        create function dolphin_catalog.dolphin_uint2_pl_int2 (
        uint2,
        int2
        ) RETURNS uint4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'dolphin_uint2_pl_int2';
        create operator dolphin_catalog.+(leftarg = uint2, rightarg = int2, procedure = dolphin_catalog.dolphin_uint2_pl_int2);

        create function dolphin_catalog.dolphin_uint2_mi_int2 (
        uint2,
        int2
        ) RETURNS uint4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'dolphin_uint2_mi_int2';
        create operator dolphin_catalog.-(leftarg = uint2, rightarg = int2, procedure = dolphin_catalog.dolphin_uint2_mi_int2);

        create function dolphin_catalog.dolphin_uint2_mul_int2 (
        uint2,
        int2
        ) RETURNS uint8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'dolphin_uint2_mul_int2';
        create operator dolphin_catalog.*(leftarg = uint2, rightarg = int2, procedure = dolphin_catalog.dolphin_uint2_mul_int2);

        create function dolphin_catalog.dolphin_uint2_div_int2 (
        uint2,
        int2
        ) RETURNS numeric LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'dolphin_uint2_div_int2';
        create operator dolphin_catalog./(leftarg = uint2, rightarg = int2, procedure = dolphin_catalog.dolphin_uint2_div_int2);

        /* uint2 */
        create function dolphin_catalog.dolphin_uint2pl (
        uint2,
        uint2
        ) RETURNS uint4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'dolphin_uint2pl';
        create operator dolphin_catalog.+(leftarg = uint2, rightarg = uint2, procedure = dolphin_catalog.dolphin_uint2pl);

        create function dolphin_catalog.dolphin_uint2mi (
        uint2,
        uint2
        ) RETURNS uint4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'dolphin_uint2mi';
        create operator dolphin_catalog.-(leftarg = uint2, rightarg = uint2, procedure = dolphin_catalog.dolphin_uint2mi);

        create function dolphin_catalog.dolphin_uint2mul (
        uint2,
        uint2
        ) RETURNS uint8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'dolphin_uint2mul';
        create operator dolphin_catalog.*(leftarg = uint2, rightarg = uint2, procedure = dolphin_catalog.dolphin_uint2mul);

        create function dolphin_catalog.dolphin_uint2div (
        uint2,
        uint2
        ) RETURNS numeric LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'dolphin_uint2div';
        create operator dolphin_catalog./(leftarg = uint2, rightarg = uint2, procedure = dolphin_catalog.dolphin_uint2div);

        /* uint4 */
        create function dolphin_catalog.dolphin_uint4pl (
        uint4,
        uint4
        ) RETURNS uint8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'dolphin_uint4pl';
        create operator dolphin_catalog.+(leftarg = uint4, rightarg = uint4, procedure = dolphin_catalog.dolphin_uint4pl);

        create function dolphin_catalog.dolphin_uint4mi (
        uint4,
        uint4
        ) RETURNS uint8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'dolphin_uint4mi';
        create operator dolphin_catalog.-(leftarg = uint4, rightarg = uint4, procedure = dolphin_catalog.dolphin_uint4mi);

        create function dolphin_catalog.dolphin_uint4mul (
        uint4,
        uint4
        ) RETURNS uint8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'dolphin_uint4mul';
        create operator dolphin_catalog.*(leftarg = uint4, rightarg = uint4, procedure = dolphin_catalog.dolphin_uint4mul);

        create function dolphin_catalog.dolphin_uint4div (
        uint4,
        uint4
        ) RETURNS numeric LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'dolphin_uint4div';
        create operator dolphin_catalog./(leftarg = uint4, rightarg = uint4, procedure = dolphin_catalog.dolphin_uint4div);

        /* uint4_int4 */
        create function dolphin_catalog.dolphin_uint4_pl_int4 (
        uint4,
        int4
        ) RETURNS uint8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'dolphin_uint4_pl_int4';
        create operator dolphin_catalog.+(leftarg = uint4, rightarg = int4, procedure = dolphin_catalog.dolphin_uint4_pl_int4);

        create function dolphin_catalog.dolphin_uint4_mi_int4 (
        uint4,
        int4
        ) RETURNS uint8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'dolphin_uint4_mi_int4';
        create operator dolphin_catalog.-(leftarg = uint4, rightarg = int4, procedure = dolphin_catalog.dolphin_uint4_mi_int4);

        create function dolphin_catalog.dolphin_uint4_mul_int4 (
        uint4,
        int4
        ) RETURNS uint8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'dolphin_uint4_mul_int4';
        create operator dolphin_catalog.*(leftarg = uint4, rightarg = int4, procedure = dolphin_catalog.dolphin_uint4_mul_int4);

        create function dolphin_catalog.dolphin_uint4_div_int4 (
        uint4,
        int4
        ) RETURNS numeric LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'dolphin_uint4_div_int4';
        create operator dolphin_catalog./(leftarg = uint4, rightarg = int4, procedure = dolphin_catalog.dolphin_uint4_div_int4);

        /* float4 */
        create function dolphin_catalog.dolphin_float4pl (
        float4,
        float4
        ) RETURNS float8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'dolphin_float4pl';
        create operator dolphin_catalog.+(leftarg = float4, rightarg = float4, procedure = dolphin_catalog.dolphin_float4pl);

        create function dolphin_catalog.dolphin_float4mi (
        float4,
        float4
        ) RETURNS float8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'dolphin_float4mi';
        create operator dolphin_catalog.-(leftarg = float4, rightarg = float4, procedure = dolphin_catalog.dolphin_float4mi);

        create function dolphin_catalog.dolphin_float4mul (
        float4,
        float4
        ) RETURNS float8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'dolphin_float4mul';
        create operator dolphin_catalog.*(leftarg = float4, rightarg = float4, procedure = dolphin_catalog.dolphin_float4mul);

        create function dolphin_catalog.dolphin_float4div (
        float4,
        float4
        ) RETURNS float8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'dolphin_float4div';
        create operator dolphin_catalog./(leftarg = float4, rightarg = float4, procedure = dolphin_catalog.dolphin_float4div);

        /* float4_float8 */
        create function dolphin_catalog.dolphin_float48pl (
        float4,
        float8
        ) RETURNS float8 LANGUAGE INTERNAL IMMUTABLE STRICT as 'float48pl';
        create operator dolphin_catalog.+(leftarg = float4, rightarg = float8, procedure = dolphin_catalog.dolphin_float48pl);

        create function dolphin_catalog.dolphin_float48mi (
        float4,
        float8
        ) RETURNS float8 LANGUAGE INTERNAL IMMUTABLE STRICT as 'float48mi';
        create operator dolphin_catalog.-(leftarg = float4, rightarg = float8, procedure = dolphin_catalog.dolphin_float48mi);

        create function dolphin_catalog.dolphin_float48mul (
        float4,
        float8
        ) RETURNS float8 LANGUAGE INTERNAL IMMUTABLE STRICT as 'float48mul';
        create operator dolphin_catalog.*(leftarg = float4, rightarg = float8, procedure = dolphin_catalog.dolphin_float48mul);

        create function dolphin_catalog.dolphin_float48div (
        float4,
        float8
        ) RETURNS float8 LANGUAGE INTERNAL IMMUTABLE STRICT as 'float48div';
        create operator dolphin_catalog./(leftarg = float4, rightarg = float8, procedure = dolphin_catalog.dolphin_float48div);

        /* float8_float4 */
        create function dolphin_catalog.dolphin_float84pl (
        float8,
        float4
        ) RETURNS float8 LANGUAGE INTERNAL IMMUTABLE STRICT as 'float84pl';
        create operator dolphin_catalog.+(leftarg = float8, rightarg = float4, procedure = dolphin_catalog.dolphin_float84pl);

        create function dolphin_catalog.dolphin_float84mi (
        float8,
        float4
        ) RETURNS float8 LANGUAGE INTERNAL IMMUTABLE STRICT as 'float84mi';
        create operator dolphin_catalog.-(leftarg = float8, rightarg = float4, procedure = dolphin_catalog.dolphin_float84mi);

        create function dolphin_catalog.dolphin_float84mul (
        float8,
        float4
        ) RETURNS float8 LANGUAGE INTERNAL IMMUTABLE STRICT as 'float84mul';
        create operator dolphin_catalog.*(leftarg = float8, rightarg = float4, procedure = dolphin_catalog.dolphin_float84mul);

        create function dolphin_catalog.dolphin_float84div (
        float8,
        float4
        ) RETURNS float8 LANGUAGE INTERNAL IMMUTABLE STRICT as 'float84div';
        create operator dolphin_catalog./(leftarg = float8, rightarg = float4, procedure = dolphin_catalog.dolphin_float84div);

        /* uint8_int8 */
        CREATE FUNCTION dolphin_catalog.dolphin_uint8_pl_int8 (
        uint8,
        int8
        ) RETURNS uint8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint8_pl_int8';
        CREATE OPERATOR dolphin_catalog.+(leftarg = uint8, rightarg = int8, procedure = dolphin_catalog.dolphin_uint8_pl_int8);

        CREATE FUNCTION dolphin_catalog.dolphin_uint8_mi_int8 (
        uint8,
        int8
        ) RETURNS uint8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint8_mi_int8';
        CREATE OPERATOR dolphin_catalog.-(leftarg = uint8, rightarg = int8, procedure = dolphin_catalog.dolphin_uint8_mi_int8);

        CREATE FUNCTION dolphin_catalog.dolphin_uint8_mul_int8 (
        uint8,
        int8
        ) RETURNS uint8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint8_mul_int8';
        CREATE OPERATOR dolphin_catalog.*(leftarg = uint8, rightarg = int8, procedure = dolphin_catalog.dolphin_uint8_mul_int8);

        CREATE FUNCTION dolphin_catalog.dolphin_uint8_div_int8 (
        uint8,
        int8
        ) RETURNS numeric LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'dolphin_uint8_div_int8';
        CREATE OPERATOR dolphin_catalog./(leftarg = uint8, rightarg = int8, procedure = dolphin_catalog.dolphin_uint8_div_int8);

        /* int8_uint8 */
        CREATE FUNCTION dolphin_catalog.dolphin_int8_pl_uint8 (
        int8,
        uint8
        ) RETURNS uint8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int8_pl_uint8';
        CREATE OPERATOR dolphin_catalog.+(leftarg = int8, rightarg = uint8, procedure = dolphin_catalog.dolphin_int8_pl_uint8);

        CREATE FUNCTION dolphin_catalog.dolphin_int8_mi_uint8 (
        int8,
        uint8
        ) RETURNS uint8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int8_mi_uint8';
        CREATE OPERATOR dolphin_catalog.-(leftarg = int8, rightarg = uint8, procedure = dolphin_catalog.dolphin_int8_mi_uint8);

        CREATE FUNCTION dolphin_catalog.dolphin_int8_mul_uint8 (
        int8,
        uint8
        ) RETURNS uint8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int8_mul_uint8';
        CREATE OPERATOR dolphin_catalog.*(leftarg = int8, rightarg = uint8, procedure = dolphin_catalog.dolphin_int8_mul_uint8);

        create function dolphin_catalog.dolphin_int8_div_uint8 (
        int8,
        uint8
        ) RETURNS numeric LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'dolphin_int8_div_uint8';
        create operator dolphin_catalog./(leftarg = int8, rightarg = uint8, procedure = dolphin_catalog.dolphin_int8_div_uint8);

        /* float8 */
        create operator dolphin_catalog.+(leftarg = float8, rightarg = float8, procedure = float8pl);
        create operator dolphin_catalog.-(leftarg = float8, rightarg = float8, procedure = float8mi);
        create operator dolphin_catalog.*(leftarg = float8, rightarg = float8, procedure = float8mul);
        create operator dolphin_catalog./(leftarg = float8, rightarg = float8, procedure = float8div);

        /* uint8 */
        create operator dolphin_catalog.+(leftarg = uint8, rightarg = uint8, procedure = uint8pl);
        create operator dolphin_catalog.-(leftarg = uint8, rightarg = uint8, procedure = uint8mi);
        create operator dolphin_catalog.*(leftarg = uint8, rightarg = uint8, procedure = uint8mul);
        create function dolphin_catalog.dolphin_uint8div (
        uint8,
        uint8
        ) RETURNS numeric LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'dolphin_uint8div';
        create operator dolphin_catalog./(leftarg = uint8, rightarg = uint8, procedure = dolphin_catalog.dolphin_uint8div);

        /* numeric */
        create operator dolphin_catalog.+(leftarg = numeric, rightarg = numeric, procedure = numeric_add);
        create operator dolphin_catalog.-(leftarg = numeric, rightarg = numeric, procedure = numeric_sub);
        create operator dolphin_catalog.*(leftarg = numeric, rightarg = numeric, procedure = numeric_mul);
        create operator dolphin_catalog./(leftarg = numeric, rightarg = numeric, procedure = numeric_div);

        DROP FUNCTION IF EXISTS pg_catalog.uuid_short();
        CREATE OR REPLACE FUNCTION pg_catalog.uuid_short()
        RETURNS uint8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'uuid_short';

    end if;
END
$for_og_310$;

GRANT USAGE ON schema dolphin_catalog TO PUBLIC;

DO $for_upgrade_only$
DECLARE
  ans boolean;
  v_isinplaceupgrade boolean;
BEGIN
    select case when count(*)=1 then true else false end as ans from (select setting from pg_settings where name = 'upgrade_mode' and setting != '0') into ans;
    show isinplaceupgrade into v_isinplaceupgrade;
    -- we can do drop operator only during upgrade
    if ans = true and v_isinplaceupgrade = true then
        drop operator IF EXISTS pg_catalog./(int1, int1);
        create operator pg_catalog./(leftarg = int1, rightarg = int1, procedure = dolphin_catalog.dolphin_int1div);
        COMMENT ON OPERATOR pg_catalog./(int1, int1) IS 'dolphin_int1div';

        drop operator IF EXISTS pg_catalog.-(NONE, int1);
        CREATE OPERATOR pg_catalog.-(rightarg = int1, procedure = pg_catalog.int1um);
    end if;
END
$for_upgrade_only$;

DROP FUNCTION IF EXISTS pg_catalog.dolphin_invoke();
CREATE FUNCTION pg_catalog.dolphin_invoke()
    RETURNS VOID AS '$libdir/dolphin','dolphin_invoke' LANGUAGE C STRICT;

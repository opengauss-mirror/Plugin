DROP FUNCTION IF EXISTS pg_catalog.length(boolean);
DROP FUNCTION IF EXISTS pg_catalog.length(tinyblob);
DROP FUNCTION IF EXISTS pg_catalog.length(blob);
DROP FUNCTION IF EXISTS pg_catalog.length(mediumblob);
DROP FUNCTION IF EXISTS pg_catalog.length(longblob);
DROP FUNCTION IF EXISTS pg_catalog.length(anyenum);
DROP FUNCTION IF EXISTS pg_catalog.length(json);

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

DROP OPERATOR IF EXISTS pg_catalog.~~(anyenum, text);
DROP OPERATOR IF EXISTS pg_catalog.~~(text, anyenum);
DROP OPERATOR IF EXISTS pg_catalog.!~~(anyenum, text);
DROP OPERATOR IF EXISTS pg_catalog.!~~(text, anyenum);
DROP FUNCTION IF EXISTS pg_catalog.enumtext_like(anyenum, text);
DROP FUNCTION IF EXISTS pg_catalog.textenum_like(text, anyenum);
DROP FUNCTION IF EXISTS pg_catalog.enumtext_nlike(anyenum, text);
DROP FUNCTION IF EXISTS pg_catalog.textenum_nlike(text, anyenum);

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
CREATE OR REPLACE FUNCTION pg_catalog.length(boolean) RETURNS integer LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.length($1::integer)';
CREATE OR REPLACE FUNCTION pg_catalog.length(tinyblob) RETURNS integer LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.length($1::text)';
CREATE OR REPLACE FUNCTION pg_catalog.length(blob) RETURNS integer LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.length($1::text)';
CREATE OR REPLACE FUNCTION pg_catalog.length(mediumblob) RETURNS integer LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.length($1::text)';
CREATE OR REPLACE FUNCTION pg_catalog.length(longblob) RETURNS integer LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.length($1::text)';
CREATE OR REPLACE FUNCTION pg_catalog.length(anyenum) RETURNS integer LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.length($1::text)';
CREATE OR REPLACE FUNCTION pg_catalog.length(json) RETURNS integer LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.length($1::text)';

CREATE OR REPLACE FUNCTION pg_catalog.octet_length(boolean) RETURNS integer LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.octet_length($1::text)';
CREATE OR REPLACE FUNCTION pg_catalog.octet_length(year) RETURNS integer LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.octet_length($1::text)';
CREATE OR REPLACE FUNCTION pg_catalog.octet_length(blob) RETURNS integer LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.octet_length($1::text)';
CREATE OR REPLACE FUNCTION pg_catalog.octet_length(anyenum) RETURNS integer LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.octet_length($1::text)';
CREATE OR REPLACE FUNCTION pg_catalog.octet_length(json) RETURNS integer LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.octet_length($1::text)';

CREATE OR REPLACE FUNCTION pg_catalog.position(boolean, text) RETURNS integer LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.position($1::text, $2)';
CREATE OR REPLACE FUNCTION pg_catalog.position(binary, text) RETURNS integer LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.position($1::bytea, $2::bytea)';
CREATE OR REPLACE FUNCTION pg_catalog.position(varbinary, text) RETURNS integer LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.position($1::bytea, $2::bytea)';

CREATE OR REPLACE FUNCTION pg_catalog.instr(binary, text) RETURNS integer LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.position($1, $2)';
CREATE OR REPLACE FUNCTION pg_catalog.instr(varbinary, text) RETURNS integer LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.position($1, $2)';
CREATE OR REPLACE FUNCTION pg_catalog.instr(boolean, text, integer) RETURNS integer LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.instr($1::text, $2, $3)';
CREATE OR REPLACE FUNCTION pg_catalog.instr(bit, bit, integer) RETURNS integer LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'instr_bit';
CREATE OR REPLACE FUNCTION pg_catalog.instr(bytea, bytea, integer) RETURNS integer LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'instr_binary';
CREATE OR REPLACE FUNCTION pg_catalog.instr(binary, text, integer) RETURNS integer LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.instr($1::bytea, $2::bytea, $3)';
CREATE OR REPLACE FUNCTION pg_catalog.instr(varbinary, text, integer) RETURNS integer LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.instr($1::bytea, $2::bytea, $3)';

CREATE OR REPLACE FUNCTION pg_catalog.locate(boolean, text) RETURNS integer LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.position($1, $2)';
CREATE OR REPLACE FUNCTION pg_catalog.locate(bit, bit) RETURNS integer LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.position($1, $2)';
CREATE OR REPLACE FUNCTION pg_catalog.locate(binary, text) RETURNS integer LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.position($1, $2)';
CREATE OR REPLACE FUNCTION pg_catalog.locate(varbinary, text) RETURNS integer LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.position($1, $2)';
CREATE OR REPLACE FUNCTION pg_catalog.locate(boolean, text, integer) RETURNS integer LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.instr($1, $2, $3)';
CREATE OR REPLACE FUNCTION pg_catalog.locate(bit, bit, integer) RETURNS integer LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.instr($1, $2, $3)';
CREATE OR REPLACE FUNCTION pg_catalog.locate(bytea, bytea, integer) RETURNS integer LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.instr($1, $2, $3)';
CREATE OR REPLACE FUNCTION pg_catalog.locate(binary, text, integer) RETURNS integer LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.instr($1, $2, $3)';
CREATE OR REPLACE FUNCTION pg_catalog.locate(varbinary, text, integer) RETURNS integer LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.instr($1, $2, $3)';

CREATE OR REPLACE FUNCTION pg_catalog.mid(boolean, int) RETURNS varchar LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.mid($1::text, $2)::varchar';
CREATE OR REPLACE FUNCTION pg_catalog.mid(boolean, int, int) RETURNS varchar LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.mid($1::text, $2, $3)::varchar';
CREATE OR REPLACE FUNCTION pg_catalog.mid(binary, int) RETURNS varbinary LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.mid($1::bytea, $2)::varbinary(65535)';
CREATE OR REPLACE FUNCTION pg_catalog.mid(binary, int, int) RETURNS varbinary LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.mid($1::bytea, $2, $3)::varbinary(65535)';
CREATE OR REPLACE FUNCTION pg_catalog.mid(varbinary, int) RETURNS varbinary LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.mid($1::bytea, $2)::varbinary(65535)';
CREATE OR REPLACE FUNCTION pg_catalog.mid(varbinary, int, int) RETURNS varbinary LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.mid($1::bytea, $2, $3)::varbinary(65535)';
CREATE OR REPLACE FUNCTION pg_catalog.mid(bit, int) RETURNS varbinary LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'bit_substr_no_len';
CREATE OR REPLACE FUNCTION pg_catalog.mid(bit, int, int) RETURNS varbinary LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'bit_substr';

CREATE OR REPLACE FUNCTION pg_catalog.log10(float8) RETURNS double precision LANGUAGE INTERNAL IMMUTABLE STRICT as 'dlog10';

CREATE OR REPLACE FUNCTION pg_catalog.lower(int8) RETURNS varchar LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.lower(cast($1 as TEXT))::varchar';
CREATE OR REPLACE FUNCTION pg_catalog.lower(uint8) RETURNS varchar LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.lower(cast($1 as TEXT))::varchar';
CREATE OR REPLACE FUNCTION pg_catalog.lower(float4) RETURNS varchar LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.lower(cast($1 as TEXT))::varchar';
CREATE OR REPLACE FUNCTION pg_catalog.lower(numeric) RETURNS varchar LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.lower(cast($1 as TEXT))::varchar';

CREATE OR REPLACE FUNCTION pg_catalog.lcase(int8) RETURNS varchar LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.lower($1)';
CREATE OR REPLACE FUNCTION pg_catalog.lcase(uint8) RETURNS varchar LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.lower($1)';
CREATE OR REPLACE FUNCTION pg_catalog.lcase(float4) RETURNS varchar LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.lower($1)';
CREATE OR REPLACE FUNCTION pg_catalog.lcase(numeric) RETURNS varchar LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.lower($1)';

DROP FUNCTION IF EXISTS pg_catalog.cot(bit);
CREATE OR REPLACE FUNCTION pg_catalog.cot(bit) RETURNS double precision LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'cot_bit';
DROP FUNCTION IF EXISTS pg_catalog.cot(boolean);
CREATE OR REPLACE FUNCTION pg_catalog.cot(boolean) RETURNS double precision LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.cot(cast($1 as float))';

DROP FUNCTION IF EXISTS pg_catalog.acos(bit);
CREATE OR REPLACE FUNCTION pg_catalog.acos(bit) RETURNS double precision LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'acos_bit';

DROP FUNCTION IF EXISTS pg_catalog.cos(bit);
CREATE OR REPLACE FUNCTION pg_catalog.cos(bit) RETURNS double precision LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'cos_bit';
DROP FUNCTION IF EXISTS pg_catalog.cos(boolean);
CREATE OR REPLACE FUNCTION pg_catalog.cos(boolean) RETURNS double precision LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.cos(cast($1 as float))';

DROP FUNCTION IF EXISTS pg_catalog.asin(bit);
CREATE OR REPLACE FUNCTION pg_catalog.asin(bit) RETURNS double precision LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'asin_bit';
DROP FUNCTION IF EXISTS pg_catalog.asin(boolean);
CREATE OR REPLACE FUNCTION pg_catalog.asin(boolean) RETURNS double precision LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.asin(cast($1 as float))';

DROP FUNCTION IF EXISTS pg_catalog.atan(bit);
CREATE OR REPLACE FUNCTION pg_catalog.atan(bit) RETURNS double precision LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'atan_bit';

DROP FUNCTION IF EXISTS pg_catalog.year(integer);
CREATE OR REPLACE FUNCTION pg_catalog.year(integer) RETURNS int8 LANGUAGE SQL IMMUTABLE STRICT as 'SELECT year($1::timestamp(0) without time zone)';
CREATE OR REPLACE FUNCTION pg_catalog.year(anyset) RETURNS int8 LANGUAGE SQL IMMUTABLE STRICT as 'SELECT year($1::timestamp(0) without time zone)';

CREATE OR REPLACE FUNCTION pg_catalog.hour(bit) RETURNS int8 LANGUAGE C IMMUTABLE STRICT as  '$libdir/dolphin','hour_bit';
CREATE OR REPLACE FUNCTION pg_catalog.hour(blob) RETURNS int8 LANGUAGE SQL IMMUTABLE STRICT as 'SELECT hour($1::text)';
CREATE OR REPLACE FUNCTION pg_catalog.hour(boolean) RETURNS int8 LANGUAGE SQL IMMUTABLE STRICT as 'SELECT hour($1::text)';
CREATE OR REPLACE FUNCTION pg_catalog.hour(json) RETURNS int8 LANGUAGE SQL IMMUTABLE STRICT as 'SELECT hour($1::timestamp(0) without time zone)';
CREATE OR REPLACE FUNCTION pg_catalog.hour(integer) RETURNS int8 LANGUAGE SQL IMMUTABLE STRICT as 'SELECT hour($1::text)';

CREATE OR REPLACE FUNCTION pg_catalog.minute(bit) RETURNS int8 LANGUAGE C IMMUTABLE STRICT as  '$libdir/dolphin','minute_bit';
CREATE OR REPLACE FUNCTION pg_catalog.minute(blob) RETURNS int8 LANGUAGE SQL IMMUTABLE STRICT as 'SELECT minute($1::text)';
CREATE OR REPLACE FUNCTION pg_catalog.minute(boolean) RETURNS int8 LANGUAGE SQL IMMUTABLE STRICT as 'SELECT minute($1::text)';
CREATE OR REPLACE FUNCTION pg_catalog.minute(json) RETURNS int8 LANGUAGE SQL IMMUTABLE STRICT as 'SELECT minute($1::timestamp(0) without time zone)';
CREATE OR REPLACE FUNCTION pg_catalog.minute(integer) RETURNS int8 LANGUAGE SQL IMMUTABLE STRICT as 'SELECT minute($1::text)';

CREATE OR REPLACE FUNCTION pg_catalog.second(bit) RETURNS int8 LANGUAGE C IMMUTABLE STRICT as  '$libdir/dolphin','second_bit';
CREATE OR REPLACE FUNCTION pg_catalog.second(blob) RETURNS int8 LANGUAGE SQL IMMUTABLE STRICT as 'SELECT second($1::text)';
CREATE OR REPLACE FUNCTION pg_catalog.second(boolean) RETURNS int8 LANGUAGE SQL IMMUTABLE STRICT as 'SELECT second($1::text)';
CREATE OR REPLACE FUNCTION pg_catalog.second(json) RETURNS int8 LANGUAGE SQL IMMUTABLE STRICT as 'SELECT second($1::timestamp(0) without time zone)';
CREATE OR REPLACE FUNCTION pg_catalog.second(integer) RETURNS int8 LANGUAGE SQL IMMUTABLE STRICT as 'SELECT second($1::text)';

CREATE OR REPLACE FUNCTION pg_catalog.microsecond (year) RETURNS int8 LANGUAGE SQL STABLE STRICT as 'SELECT microsecond($1::time)';
CREATE OR REPLACE FUNCTION pg_catalog.microsecond (bit) RETURNS int8 LANGUAGE C IMMUTABLE STRICT as  '$libdir/dolphin','microsecond_bit';
CREATE OR REPLACE FUNCTION pg_catalog.microsecond (blob) RETURNS int8 LANGUAGE SQL STABLE STRICT as 'SELECT microsecond($1::text)';
CREATE OR REPLACE FUNCTION pg_catalog.microsecond (boolean) RETURNS int8 LANGUAGE SQL STABLE STRICT as 'SELECT microsecond($1::text)';
CREATE OR REPLACE FUNCTION pg_catalog.microsecond (json) RETURNS int8 LANGUAGE SQL STABLE STRICT as 'SELECT microsecond($1::timestamp(0) without time zone)';
CREATE OR REPLACE FUNCTION pg_catalog.microsecond (integer) RETURNS int8 LANGUAGE SQL STABLE STRICT as 'SELECT microsecond($1::text)';
CREATE OR REPLACE FUNCTION pg_catalog.microsecond (float) RETURNS int8 LANGUAGE SQL STABLE STRICT as 'SELECT microsecond($1::text)';

DO $for_upgrade_only$
DECLARE
  ans boolean;
  v_isinplaceupgrade boolean;
BEGIN
    select case when count(*)=1 then true else false end as ans from (select setting from pg_settings where name = 'upgrade_mode' and setting != '0') into ans;
    show isinplaceupgrade into v_isinplaceupgrade;
    -- we can do drop operator only during upgrade. Just for 6.0.0-RC1
    if ans = true and v_isinplaceupgrade = true then
        DROP OPERATOR IF EXISTS pg_catalog.||(unknown, unknown);
        DROP OPERATOR IF EXISTS pg_catalog.||(unknown, integer);
        DROP OPERATOR IF EXISTS pg_catalog.||(integer, unknown);

        DROP FUNCTION IF EXISTS pg_catalog.unknown_concat(unknown, unknown);
        DROP FUNCTION IF EXISTS pg_catalog.unknown_int_concat(unknown, integer);
        DROP FUNCTION IF EXISTS pg_catalog.int_unknown_concat(integer, unknown);
    end if;
END
$for_upgrade_only$;

CREATE OR REPLACE FUNCTION pg_catalog.mod(binary,int8) RETURNS number LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.mod(cast($1 as number), cast($2 as number))';
CREATE OR REPLACE FUNCTION pg_catalog.mod(binary,bit) RETURNS number LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.mod(cast($1 as number), cast($2 as number))';
CREATE OR REPLACE FUNCTION pg_catalog.mod(binary,year) RETURNS number LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.mod(cast($1 as number), cast($2 as number))';
CREATE OR REPLACE FUNCTION pg_catalog.mod(binary,binary) RETURNS number LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.mod(cast($1 as number), cast($2 as number))';
CREATE OR REPLACE FUNCTION pg_catalog.mod(binary,varbinary) RETURNS number LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.mod(cast($1 as number), cast($2 as number))';
CREATE OR REPLACE FUNCTION pg_catalog.mod(int8,binary) RETURNS number LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.mod(cast($1 as number), cast($2 as number))';
CREATE OR REPLACE FUNCTION pg_catalog.mod(bit,binary) RETURNS number LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.mod(cast($1 as number), cast($2 as number))';
CREATE OR REPLACE FUNCTION pg_catalog.mod(year,binary) RETURNS number LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.mod(cast($1 as number), cast($2 as number))';
CREATE OR REPLACE FUNCTION pg_catalog.mod(varbinary,varbinary) RETURNS number LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.mod(cast($1 as number), cast($2 as number))';
CREATE OR REPLACE FUNCTION pg_catalog.mod(varbinary,binary) RETURNS number LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.mod(cast($1 as number), cast($2 as number))';
CREATE OR REPLACE FUNCTION pg_catalog.mod(varbinary,int8) RETURNS number LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.mod(cast($1 as number), cast($2 as number))';
CREATE OR REPLACE FUNCTION pg_catalog.mod(varbinary,bit) RETURNS number LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.mod(cast($1 as number), cast($2 as number))';
CREATE OR REPLACE FUNCTION pg_catalog.mod(varbinary,year) RETURNS number LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.mod(cast($1 as number), cast($2 as number))';
CREATE OR REPLACE FUNCTION pg_catalog.mod(int8,varbinary) RETURNS number LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.mod(cast($1 as number), cast($2 as number))';
CREATE OR REPLACE FUNCTION pg_catalog.mod(bit,varbinary) RETURNS number LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.mod(cast($1 as number), cast($2 as number))';
CREATE OR REPLACE FUNCTION pg_catalog.mod(year,varbinary) RETURNS number LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.mod(cast($1 as number), cast($2 as number))';

DO $for_og_502$
BEGIN
    -- workering_version_num of openGauss5.0.1 is 92854, openGauss5.1.0 is 92913.
    if working_version_num() < 92855 or working_version_num() >= 92913 then
        CREATE OR REPLACE FUNCTION pg_catalog.datetime_text_eq(arg1 timestamp without time zone, arg2 text) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1=$2::timestamp without time zone';
        CREATE OR REPLACE FUNCTION pg_catalog.datetime_text_ne(arg1 timestamp without time zone, arg2 text) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1<>$2::timestamp without time zone';
        CREATE OR REPLACE FUNCTION pg_catalog.datetime_text_lt(arg1 timestamp without time zone, arg2 text) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1<$2::timestamp without time zone';
        CREATE OR REPLACE FUNCTION pg_catalog.datetime_text_le(arg1 timestamp without time zone, arg2 text) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1<=$2::timestamp without time zone';
        CREATE OR REPLACE FUNCTION pg_catalog.datetime_text_gt(arg1 timestamp without time zone, arg2 text) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1>$2::timestamp without time zone';
        CREATE OR REPLACE FUNCTION pg_catalog.datetime_text_ge(arg1 timestamp without time zone, arg2 text) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1>=$2::timestamp without time zone';
        CREATE OPERATOR pg_catalog.=(leftarg = timestamp without time zone, rightarg = text, COMMUTATOR = operator(pg_catalog.=), procedure = pg_catalog.datetime_text_eq);
        CREATE OPERATOR pg_catalog.<>(leftarg = timestamp without time zone, rightarg = text, COMMUTATOR = operator(pg_catalog.<>), procedure = pg_catalog.datetime_text_ne);
        CREATE OPERATOR pg_catalog.<(leftarg = timestamp without time zone, rightarg = text, COMMUTATOR = operator(pg_catalog.>), procedure = pg_catalog.datetime_text_lt);
        CREATE OPERATOR pg_catalog.<=(leftarg = timestamp without time zone, rightarg = text, COMMUTATOR = operator(pg_catalog.>=), procedure = pg_catalog.datetime_text_le);
        CREATE OPERATOR pg_catalog.>(leftarg = timestamp without time zone, rightarg = text, COMMUTATOR = operator(pg_catalog.<), procedure = pg_catalog.datetime_text_gt);
        CREATE OPERATOR pg_catalog.>=(leftarg = timestamp without time zone, rightarg = text, COMMUTATOR = operator(pg_catalog.<=), procedure = pg_catalog.datetime_text_ge);

        CREATE OR REPLACE FUNCTION pg_catalog.text_datetime_eq(arg1 text, arg2 timestamp without time zone) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT as 'select $1::timestamp without time zone=$2';
        CREATE OR REPLACE FUNCTION pg_catalog.text_datetime_ne(arg1 text, arg2 timestamp without time zone) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1::timestamp without time zone<>$2';
        CREATE OR REPLACE FUNCTION pg_catalog.text_datetime_lt(arg1 text, arg2 timestamp without time zone) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1::timestamp without time zone<$2';
        CREATE OR REPLACE FUNCTION pg_catalog.text_datetime_le(arg1 text, arg2 timestamp without time zone) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1::timestamp without time zone<=$2';
        CREATE OR REPLACE FUNCTION pg_catalog.text_datetime_gt(arg1 text, arg2 timestamp without time zone) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1::timestamp without time zone>$2';
        CREATE OR REPLACE FUNCTION pg_catalog.text_datetime_ge(arg1 text, arg2 timestamp without time zone) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1::timestamp without time zone>=$2';
        CREATE OPERATOR pg_catalog.=(leftarg = text, rightarg = timestamp without time zone, COMMUTATOR = operator(pg_catalog.=), procedure = pg_catalog.text_datetime_eq);
        CREATE OPERATOR pg_catalog.<>(leftarg = text, rightarg = timestamp without time zone, COMMUTATOR = operator(pg_catalog.<>), procedure = pg_catalog.text_datetime_ne);
        CREATE OPERATOR pg_catalog.<(leftarg = text, rightarg = timestamp without time zone, COMMUTATOR = operator(pg_catalog.>), procedure = pg_catalog.text_datetime_lt);
        CREATE OPERATOR pg_catalog.<=(leftarg = text, rightarg = timestamp without time zone, COMMUTATOR = operator(pg_catalog.>=), procedure = pg_catalog.text_datetime_le);
        CREATE OPERATOR pg_catalog.>(leftarg = text, rightarg = timestamp without time zone, COMMUTATOR = operator(pg_catalog.<), procedure = pg_catalog.text_datetime_gt);
        CREATE OPERATOR pg_catalog.>=(leftarg = text, rightarg = timestamp without time zone, COMMUTATOR = operator(pg_catalog.<=), procedure = pg_catalog.text_datetime_ge);

        CREATE OR REPLACE FUNCTION pg_catalog.timestamp_text_eq(arg1 timestamptz, arg2 text) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1=$2::timestamptz';
        CREATE OR REPLACE FUNCTION pg_catalog.timestamp_text_ne(arg1 timestamptz, arg2 text) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1<>$2::timestamptz';
        CREATE OR REPLACE FUNCTION pg_catalog.timestamp_text_lt(arg1 timestamptz, arg2 text) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1<$2::timestamptz';
        CREATE OR REPLACE FUNCTION pg_catalog.timestamp_text_le(arg1 timestamptz, arg2 text) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1<=$2::timestamptz';
        CREATE OR REPLACE FUNCTION pg_catalog.timestamp_text_gt(arg1 timestamptz, arg2 text) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1>$2::timestamptz';
        CREATE OR REPLACE FUNCTION pg_catalog.timestamp_text_ge(arg1 timestamptz, arg2 text) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1>=$2::timestamptz';
        CREATE OPERATOR pg_catalog.=(leftarg = timestamptz, rightarg = text, COMMUTATOR = operator(pg_catalog.=), procedure = pg_catalog.timestamp_text_eq);
        CREATE OPERATOR pg_catalog.<>(leftarg = timestamptz, rightarg = text, COMMUTATOR = operator(pg_catalog.<>), procedure = pg_catalog.timestamp_text_ne);
        CREATE OPERATOR pg_catalog.<(leftarg = timestamptz, rightarg = text, COMMUTATOR = operator(pg_catalog.>), procedure = pg_catalog.timestamp_text_lt);
        CREATE OPERATOR pg_catalog.<=(leftarg = timestamptz, rightarg = text, COMMUTATOR = operator(pg_catalog.>=), procedure = pg_catalog.timestamp_text_le);
        CREATE OPERATOR pg_catalog.>(leftarg = timestamptz, rightarg = text, COMMUTATOR = operator(pg_catalog.<), procedure = pg_catalog.timestamp_text_gt);
        CREATE OPERATOR pg_catalog.>=(leftarg = timestamptz, rightarg = text, COMMUTATOR = operator(pg_catalog.<=), procedure = pg_catalog.timestamp_text_ge);

        CREATE OR REPLACE FUNCTION pg_catalog.text_timestamp_eq(arg1 text, arg2 timestamptz) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT as 'select $1::timestamptz=$2';
        CREATE OR REPLACE FUNCTION pg_catalog.text_timestamp_ne(arg1 text, arg2 timestamptz) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1::timestamptz<>$2';
        CREATE OR REPLACE FUNCTION pg_catalog.text_timestamp_lt(arg1 text, arg2 timestamptz) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1::timestamptz<$2';
        CREATE OR REPLACE FUNCTION pg_catalog.text_timestamp_le(arg1 text, arg2 timestamptz) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1::timestamptz<=$2';
        CREATE OR REPLACE FUNCTION pg_catalog.text_timestamp_gt(arg1 text, arg2 timestamptz) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1::timestamptz>$2';
        CREATE OR REPLACE FUNCTION pg_catalog.text_timestamp_ge(arg1 text, arg2 timestamptz) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1::timestamptz>=$2';
        CREATE OPERATOR pg_catalog.=(leftarg = text, rightarg = timestamptz, COMMUTATOR = operator(pg_catalog.=), procedure = pg_catalog.text_timestamp_eq);
        CREATE OPERATOR pg_catalog.<>(leftarg = text, rightarg = timestamptz, COMMUTATOR = operator(pg_catalog.<>), procedure = pg_catalog.text_timestamp_ne);
        CREATE OPERATOR pg_catalog.<(leftarg = text, rightarg = timestamptz, COMMUTATOR = operator(pg_catalog.>), procedure = pg_catalog.text_timestamp_lt);
        CREATE OPERATOR pg_catalog.<=(leftarg = text, rightarg = timestamptz, COMMUTATOR = operator(pg_catalog.>=), procedure = pg_catalog.text_timestamp_le);
        CREATE OPERATOR pg_catalog.>(leftarg = text, rightarg = timestamptz, COMMUTATOR = operator(pg_catalog.<), procedure = pg_catalog.text_timestamp_gt);
        CREATE OPERATOR pg_catalog.>=(leftarg = text, rightarg = timestamptz, COMMUTATOR = operator(pg_catalog.<=), procedure = pg_catalog.text_timestamp_ge);
    end if;
END
$for_og_502$;

CREATE FUNCTION pg_catalog.enumtext_like(anyenum, text) returns bool LANGUAGE SQL IMMUTABLE as 'select $1::text like $2';
CREATE FUNCTION pg_catalog.textenum_like(text, anyenum) returns bool LANGUAGE SQL IMMUTABLE as 'select $1 like $2::text';
CREATE FUNCTION pg_catalog.enumtext_nlike(anyenum, text) returns bool LANGUAGE SQL IMMUTABLE as 'select $1::text not like $2';
CREATE FUNCTION pg_catalog.textenum_nlike(text, anyenum) returns bool LANGUAGE SQL IMMUTABLE as 'select $1 not like $2::text';
CREATE OPERATOR pg_catalog.~~(leftarg = anyenum, rightarg = text, procedure = pg_catalog.enumtext_like);
CREATE OPERATOR pg_catalog.~~(leftarg = text, rightarg = anyenum, procedure = pg_catalog.textenum_like);
CREATE OPERATOR pg_catalog.!~~(leftarg = anyenum, rightarg = text, procedure = pg_catalog.enumtext_nlike);
CREATE OPERATOR pg_catalog.!~~(leftarg = text, rightarg = anyenum, procedure = pg_catalog.textenum_nlike);

DROP FUNCTION IF EXISTS pg_catalog.chara(variadic arr "any") cascade;
CREATE OR REPLACE FUNCTION pg_catalog.chara(variadic arr "any") returns varbinary LANGUAGE C IMMUTABLE as '$libdir/dolphin', 'm_char';

CREATE OR REPLACE FUNCTION pg_catalog.lpad(boolean, integer, text) RETURNS varchar LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.lpad($1::text, $2, $3)::varchar';
CREATE OR REPLACE FUNCTION pg_catalog.lpad(bit, integer, text) RETURNS varbinary LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'lpad_bit';
CREATE OR REPLACE FUNCTION pg_catalog.lpad(binary, integer, text) RETURNS varbinary LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'lpad_bin';
CREATE OR REPLACE FUNCTION pg_catalog.lpad(varbinary, integer, text) RETURNS varbinary LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'lpad_bin';

DROP FUNCTION IF EXISTS pg_catalog.quarter (timestamptz);
DROP FUNCTION IF EXISTS pg_catalog.quarter (timetz);
DROP FUNCTION IF EXISTS pg_catalog.quarter (abstime);
DROP FUNCTION IF EXISTS pg_catalog.quarter (date);
DROP FUNCTION IF EXISTS pg_catalog.quarter (time);
DROP FUNCTION IF EXISTS pg_catalog.quarter (timestamp(0) with time zone);
CREATE OR REPLACE FUNCTION pg_catalog.quarter (timestamptz) RETURNS integer LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.date_part(''quarter'', $1)::integer';
CREATE OR REPLACE FUNCTION pg_catalog.quarter (timetz) RETURNS integer LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.date_part(''quarter'', $1)::integer';
CREATE OR REPLACE FUNCTION pg_catalog.quarter (abstime) RETURNS integer LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.date_part(''quarter'', $1)::integer';
CREATE OR REPLACE FUNCTION pg_catalog.quarter (date) RETURNS integer LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.date_part(''quarter'', $1)::integer';
CREATE OR REPLACE FUNCTION pg_catalog.quarter (time) RETURNS integer LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.date_part(''quarter'', $1)::integer';
CREATE OR REPLACE FUNCTION pg_catalog.quarter (timestamp(0) with time zone) RETURNS integer LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.date_part(''quarter'', $1)::integer';
CREATE OR REPLACE FUNCTION pg_catalog.quarter (year) RETURNS integer LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.date_part(''quarter'', cast($1 as timestamp(0) with time zone))::integer';
CREATE OR REPLACE FUNCTION pg_catalog.quarter (binary) RETURNS integer LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.date_part(''quarter'', cast($1 as timestamp(0) without time zone))::integer';
CREATE OR REPLACE FUNCTION pg_catalog.quarter (text) RETURNS integer LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.date_part(''quarter'', cast($1 as timestamp(0) without time zone))::integer';

DROP CAST IF EXISTS (binary AS timestamp without time zone);
DROP CAST IF EXISTS (binary AS timestamp with time zone);
CREATE OR REPLACE FUNCTION pg_catalog.binary_timestamp(binary) RETURNS timestamp without time zone LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'binary_timestamp';
CREATE OR REPLACE FUNCTION pg_catalog.binary_timestamptz(binary) RETURNS timestamp with time zone LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'binary_timestamptz';
CREATE CAST (binary AS timestamp without time zone) WITH FUNCTION pg_catalog.binary_timestamp(binary) AS ASSIGNMENT;
CREATE CAST (binary AS timestamp with time zone) WITH FUNCTION pg_catalog.binary_timestamptz(binary) AS ASSIGNMENT;

DROP FUNCTION IF EXISTS pg_catalog.weekday (timestamptz);
DROP FUNCTION IF EXISTS pg_catalog.weekday (timetz);
DROP FUNCTION IF EXISTS pg_catalog.weekday (abstime);
DROP FUNCTION IF EXISTS pg_catalog.weekday (date);
DROP FUNCTION IF EXISTS pg_catalog.weekday (time);
DROP FUNCTION IF EXISTS pg_catalog.weekday (timestamp(0) with time zone);
CREATE OR REPLACE FUNCTION pg_catalog.weekday (timestamptz) RETURNS integer LANGUAGE SQL STABLE STRICT as 'select pg_catalog.date_part(''isodow'', $1)::integer - 1';
CREATE OR REPLACE FUNCTION pg_catalog.weekday (timetz) RETURNS integer LANGUAGE SQL STABLE STRICT as 'select pg_catalog.date_part(''isodow'', $1)::integer - 1';
CREATE OR REPLACE FUNCTION pg_catalog.weekday (abstime) RETURNS integer LANGUAGE SQL STABLE STRICT as 'select pg_catalog.date_part(''isodow'', $1)::integer - 1';
CREATE OR REPLACE FUNCTION pg_catalog.weekday (date) RETURNS integer LANGUAGE SQL STABLE STRICT as 'select pg_catalog.date_part(''isodow'', $1)::integer - 1';
CREATE OR REPLACE FUNCTION pg_catalog.weekday (time) RETURNS integer LANGUAGE SQL STABLE STRICT as 'select pg_catalog.date_part(''isodow'', $1)::integer - 1';
CREATE OR REPLACE FUNCTION pg_catalog.weekday (timestamp(0) with time zone) RETURNS integer LANGUAGE SQL STABLE STRICT as 'select pg_catalog.date_part(''isodow'', $1)::integer - 1';
CREATE OR REPLACE FUNCTION pg_catalog.weekday (year) RETURNS integer LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.date_part(''isodow'', cast($1 as timestamp(0) with time zone))::integer - 1';
CREATE OR REPLACE FUNCTION pg_catalog.weekday (binary) RETURNS integer LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.date_part(''isodow'', cast($1 as timestamp(0) with time zone))::integer - 1';
CREATE OR REPLACE FUNCTION pg_catalog.weekday (text) RETURNS integer LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.date_part(''isodow'', cast($1 as timestamp(0) with time zone))::integer - 1';

DROP FUNCTION IF EXISTS pg_catalog.weekofyear (timestamptz);
DROP FUNCTION IF EXISTS pg_catalog.weekofyear (timetz);
DROP FUNCTION IF EXISTS pg_catalog.weekofyear (abstime);
DROP FUNCTION IF EXISTS pg_catalog.weekofyear (date);
DROP FUNCTION IF EXISTS pg_catalog.weekofyear (time);
DROP FUNCTION IF EXISTS pg_catalog.weekofyear (timestamp(0) with time zone);
CREATE OR REPLACE FUNCTION pg_catalog.weekofyear (timestamptz) RETURNS integer LANGUAGE SQL STABLE STRICT as 'select pg_catalog.date_part(''week'', $1)::integer';
CREATE OR REPLACE FUNCTION pg_catalog.weekofyear (timetz) RETURNS integer LANGUAGE SQL STABLE STRICT as 'select pg_catalog.date_part(''week'', $1)::integer';
CREATE OR REPLACE FUNCTION pg_catalog.weekofyear (abstime) RETURNS integer LANGUAGE SQL STABLE STRICT as 'select pg_catalog.date_part(''week'', $1)::integer';
CREATE OR REPLACE FUNCTION pg_catalog.weekofyear (date) RETURNS integer LANGUAGE SQL STABLE STRICT as 'select pg_catalog.date_part(''week'', $1)::integer';
CREATE OR REPLACE FUNCTION pg_catalog.weekofyear (time) RETURNS integer LANGUAGE SQL STABLE STRICT as 'select pg_catalog.date_part(''week'', cast($1 as timestamp(0) without time zone))::integer';
CREATE OR REPLACE FUNCTION pg_catalog.weekofyear (timestamp(0) with time zone) RETURNS integer LANGUAGE SQL STABLE STRICT as 'select pg_catalog.date_part(''week'', $1)::integer';
CREATE OR REPLACE FUNCTION pg_catalog.weekofyear (year) RETURNS integer LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.date_part(''week'', cast($1 as timestamp(0) without time zone))::integer';
CREATE OR REPLACE FUNCTION pg_catalog.weekofyear (binary) RETURNS integer LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.date_part(''week'', cast($1 as timestamp(0) without time zone))::integer';
CREATE OR REPLACE FUNCTION pg_catalog.weekofyear (text) RETURNS integer LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.date_part(''week'', cast($1 as timestamp(0) without time zone))::integer';

-- json max/min
CREATE OR REPLACE FUNCTION pg_catalog.json_larger(json, json) RETURNS json LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin','json_larger';
CREATE OR REPLACE FUNCTION pg_catalog.json_smaller(json, json) RETURNS json LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin','json_smaller';
create aggregate pg_catalog.max(json) (SFUNC=pg_catalog.json_larger, STYPE=json);
create aggregate pg_catalog.min(json) (SFUNC=pg_catalog.json_smaller, STYPE=json);
-- tinyblob
CREATE OR REPLACE FUNCTION pg_catalog.tinyblob_larger(tinyblob, tinyblob) RETURNS tinyblob LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin','varlena_larger';
CREATE OR REPLACE FUNCTION pg_catalog.tinyblob_smaller(tinyblob, tinyblob) RETURNS tinyblob LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin','varlena_smaller';
create aggregate pg_catalog.max(tinyblob) (SFUNC=pg_catalog.tinyblob_larger, STYPE=tinyblob);
create aggregate pg_catalog.min(tinyblob) (SFUNC=pg_catalog.tinyblob_smaller, STYPE=tinyblob);
-- blob
CREATE OR REPLACE FUNCTION pg_catalog.blob_larger(blob, blob) RETURNS blob LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin','varlena_larger';
CREATE OR REPLACE FUNCTION pg_catalog.blob_smaller(blob, blob) RETURNS blob LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin','varlena_smaller';
create aggregate pg_catalog.max(blob) (SFUNC=pg_catalog.blob_larger, STYPE=blob);
create aggregate pg_catalog.min(blob) (SFUNC=pg_catalog.blob_smaller, STYPE=blob);
-- mediumblob
CREATE OR REPLACE FUNCTION pg_catalog.mediumblob_larger(mediumblob, mediumblob) RETURNS mediumblob LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin','varlena_larger';
CREATE OR REPLACE FUNCTION pg_catalog.mediumblob_smaller(mediumblob, mediumblob) RETURNS mediumblob LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin','varlena_smaller';
create aggregate pg_catalog.max(mediumblob) (SFUNC=pg_catalog.mediumblob_larger, STYPE=mediumblob);
create aggregate pg_catalog.min(mediumblob) (SFUNC=pg_catalog.mediumblob_smaller, STYPE=mediumblob);
-- longblob
CREATE OR REPLACE FUNCTION pg_catalog.longblob_larger(longblob, longblob) RETURNS longblob LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin','varlena_larger';
CREATE OR REPLACE FUNCTION pg_catalog.longblob_smaller(longblob, longblob) RETURNS longblob LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin','varlena_smaller';
create aggregate pg_catalog.max(longblob) (SFUNC=pg_catalog.longblob_larger, STYPE=longblob);
create aggregate pg_catalog.min(longblob) (SFUNC=pg_catalog.longblob_smaller, STYPE=longblob);
-- bit
CREATE OR REPLACE FUNCTION pg_catalog.bit_larger(bit, bit) RETURNS bit LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin','varbit_larger';
CREATE OR REPLACE FUNCTION pg_catalog.bit_smaller(bit, bit) RETURNS bit LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin','varbit_smaller';
create aggregate pg_catalog.max(bit) (SFUNC=pg_catalog.bit_larger, STYPE=bit);
create aggregate pg_catalog.min(bit) (SFUNC=pg_catalog.bit_smaller, STYPE=bit);
-- varbit
CREATE OR REPLACE FUNCTION pg_catalog.varbit_larger(varbit, varbit) RETURNS varbit LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin','varbit_larger';
CREATE OR REPLACE FUNCTION pg_catalog.varbit_smaller(varbit, varbit) RETURNS varbit LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin','varbit_smaller';
create aggregate pg_catalog.max(varbit) (SFUNC=pg_catalog.varbit_larger, STYPE=varbit);
create aggregate pg_catalog.min(varbit) (SFUNC=pg_catalog.varbit_smaller, STYPE=varbit);

-- date +/- interval expr unit
DROP FUNCTION IF EXISTS pg_catalog.date_add (time, interval);
DROP FUNCTION IF EXISTS pg_catalog.date_sub (time, interval);
CREATE OR REPLACE FUNCTION pg_catalog.date_add (time, interval) RETURNS text LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'date_add_time_interval';
CREATE OR REPLACE FUNCTION pg_catalog.op_time_add_intr (time, interval) RETURNS text AS $$ SELECT pg_catalog.date_add($1, $2)  $$ LANGUAGE SQL;
CREATE OR REPLACE FUNCTION pg_catalog.op_intr_add_time (interval, time) RETURNS text AS $$ SELECT pg_catalog.date_add($2, $1)  $$ LANGUAGE SQL;
CREATE OR REPLACE FUNCTION pg_catalog.date_sub (time, interval) RETURNS text AS $$ SELECT pg_catalog.date_add($1, -$2)  $$ LANGUAGE SQL;
CREATE OR REPLACE FUNCTION pg_catalog.op_time_sub_intr (time, interval) RETURNS text AS $$ SELECT pg_catalog.date_sub($1, $2)  $$ LANGUAGE SQL;

CREATE OR REPLACE FUNCTION pg_catalog.op_num_add_intr (numeric, interval) RETURNS text AS $$ SELECT pg_catalog.date_add($1, $2) $$ LANGUAGE SQL;
CREATE OR REPLACE FUNCTION pg_catalog.op_text_add_intr (text, interval) RETURNS text AS $$ SELECT pg_catalog.date_add($1, $2) $$ LANGUAGE SQL;
CREATE OR REPLACE FUNCTION pg_catalog.op_date_add_intr (date, interval) RETURNS text AS $$ SELECT pg_catalog.date_add($1, $2) $$ LANGUAGE SQL;
CREATE OR REPLACE FUNCTION pg_catalog.op_dttm_add_intr (timestamp without time zone, interval) RETURNS text AS $$ SELECT pg_catalog.date_add($1, $2) $$ LANGUAGE SQL;
CREATE OR REPLACE FUNCTION pg_catalog.op_tmsp_add_intr (timestamptz, interval) RETURNS text AS $$ SELECT pg_catalog.date_add($1, $2) $$ LANGUAGE SQL;
CREATE OR REPLACE FUNCTION pg_catalog.op_intr_add_num (interval, numeric) RETURNS text AS $$ SELECT pg_catalog.date_add($2, $1) $$ LANGUAGE SQL;
CREATE OR REPLACE FUNCTION pg_catalog.op_intr_add_text (interval, text) RETURNS text AS $$ SELECT pg_catalog.date_add($2, $1) $$ LANGUAGE SQL;
CREATE OR REPLACE FUNCTION pg_catalog.op_intr_add_date (interval, date) RETURNS text AS $$ SELECT pg_catalog.date_add($2, $1) $$ LANGUAGE SQL;
CREATE OR REPLACE FUNCTION pg_catalog.op_intr_add_dttm (interval, timestamp without time zone) RETURNS text AS $$ SELECT pg_catalog.date_add($2, $1) $$ LANGUAGE SQL;
CREATE OR REPLACE FUNCTION pg_catalog.op_intr_add_tmsp (interval, timestamptz) RETURNS text AS $$ SELECT pg_catalog.date_add($2, $1) $$ LANGUAGE SQL;
CREATE OR REPLACE FUNCTION pg_catalog.op_num_sub_intr (numeric, interval) RETURNS text AS $$ SELECT pg_catalog.date_sub($1, $2) $$ LANGUAGE SQL;
CREATE OR REPLACE FUNCTION pg_catalog.op_text_sub_intr (text, interval) RETURNS text AS $$ SELECT pg_catalog.date_sub($1, $2) $$ LANGUAGE SQL;
CREATE OR REPLACE FUNCTION pg_catalog.op_date_sub_intr (date, interval) RETURNS text AS $$ SELECT pg_catalog.date_sub($1, $2) $$ LANGUAGE SQL;
CREATE OR REPLACE FUNCTION pg_catalog.op_dttm_sub_intr (timestamp without time zone, interval) RETURNS text AS $$ SELECT pg_catalog.date_sub($1, $2) $$ LANGUAGE SQL;
CREATE OR REPLACE FUNCTION pg_catalog.op_tmsp_sub_intr (timestamptz, interval) RETURNS text AS $$ SELECT pg_catalog.date_sub($1, $2) $$ LANGUAGE SQL;

CREATE OPERATOR pg_catalog.+ (leftarg = numeric, rightarg = interval, procedure = op_num_add_intr, commutator = operator(pg_catalog.+));
CREATE OPERATOR pg_catalog.+ (leftarg = text, rightarg = interval, procedure = op_text_add_intr, commutator = operator(pg_catalog.+));
CREATE OPERATOR pg_catalog.+ (leftarg = interval, rightarg = numeric, procedure = op_intr_add_num, commutator = operator(pg_catalog.+));
CREATE OPERATOR pg_catalog.+ (leftarg = interval, rightarg = text, procedure = op_intr_add_text, commutator = operator(pg_catalog.+));
CREATE OPERATOR pg_catalog.- (leftarg = numeric, rightarg = interval, procedure = op_num_sub_intr, commutator = operator(pg_catalog.-));
CREATE OPERATOR pg_catalog.- (leftarg = text, rightarg = interval, procedure = op_text_sub_intr, commutator = operator(pg_catalog.-));
DO $$
BEGIN
        update pg_catalog.pg_operator set oprresult = 'text'::regtype, oprcode = 'op_date_add_intr'::regproc
            where oprname = '+' and oprleft = 'date'::regtype and oprright = 'interval'::regtype;
        update pg_catalog.pg_operator set oprresult = 'text'::regtype, oprcode = 'op_dttm_add_intr'::regproc
            where oprname = '+' and oprleft = 'timestamp without time zone'::regtype and oprright = 'interval'::regtype;
        update pg_catalog.pg_operator set oprresult = 'text'::regtype, oprcode = 'op_tmsp_add_intr'::regproc
            where oprname = '+' and oprleft = 'timestamptz'::regtype and oprright = 'interval'::regtype;
        update pg_catalog.pg_operator set oprresult = 'text'::regtype, oprcode = 'op_time_add_intr'::regproc
            where oprname = '+' and oprleft = 'time'::regtype and oprright = 'interval'::regtype;
    
        update pg_catalog.pg_operator set oprresult = 'text'::regtype, oprcode = 'op_intr_add_date'::regproc
            where oprname = '+' and oprleft = 'interval'::regtype and oprright = 'date'::regtype;
        update pg_catalog.pg_operator set oprresult = 'text'::regtype, oprcode = 'op_intr_add_dttm'::regproc
            where oprname = '+' and oprleft = 'interval'::regtype and oprright = 'timestamp without time zone'::regtype;
        update pg_catalog.pg_operator set oprresult = 'text'::regtype, oprcode = 'op_intr_add_tmsp'::regproc
            where oprname = '+' and oprleft = 'interval'::regtype and oprright = 'timestamptz'::regtype;
        update pg_catalog.pg_operator set oprresult = 'text'::regtype, oprcode = 'op_intr_add_time'::regproc
            where oprname = '+' and oprleft = 'interval'::regtype and oprright = 'time'::regtype;
        
        update pg_catalog.pg_operator set oprresult = 'text'::regtype, oprcode = 'op_date_sub_intr'::regproc
            where oprname = '-' and oprleft = 'date'::regtype and oprright = 'interval'::regtype;
        update pg_catalog.pg_operator set oprresult = 'text'::regtype, oprcode = 'op_dttm_sub_intr'::regproc
            where oprname = '-' and oprleft = 'timestamp without time zone'::regtype and oprright = 'interval'::regtype;
        update pg_catalog.pg_operator set oprresult = 'text'::regtype, oprcode = 'op_tmsp_sub_intr'::regproc
            where oprname = '-' and oprleft = 'timestamptz'::regtype and oprright = 'interval'::regtype;
        update pg_catalog.pg_operator set oprresult = 'text'::regtype, oprcode = 'op_time_sub_intr'::regproc
            where oprname = '-' and oprleft = 'time'::regtype and oprright = 'interval'::regtype;
END
$$;
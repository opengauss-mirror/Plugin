-- bool cast bit/float/double
DROP FUNCTION IF EXISTS pg_catalog.booltobit(bool, int4) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.booltofloat4(bool) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.booltofloat8(bool) CASCADE;

DROP CAST IF EXISTS (bool AS bit) CASCADE;
DROP CAST IF EXISTS (bool AS float4) CASCADE;
DROP CAST IF EXISTS (bool AS float8) CASCADE;

CREATE OR REPLACE FUNCTION pg_catalog.booltobit(bool, int4) RETURNS bit LANGUAGE C STRICT AS  '$libdir/dolphin',  'bool_bit';
CREATE CAST (bool AS bit) WITH FUNCTION pg_catalog.booltobit(bool, int4) AS ASSIGNMENT;

CREATE OR REPLACE FUNCTION pg_catalog.booltofloat4(bool) RETURNS float4 LANGUAGE C STRICT AS  '$libdir/dolphin',  'bool_float4';
CREATE CAST (bool AS float4) WITH FUNCTION pg_catalog.booltofloat4(bool) AS ASSIGNMENT;

CREATE OR REPLACE FUNCTION pg_catalog.booltofloat8(bool) RETURNS float8 LANGUAGE C STRICT AS  '$libdir/dolphin',  'bool_float8';
CREATE CAST (bool AS float8) WITH FUNCTION pg_catalog.booltofloat8(bool) AS ASSIGNMENT;

DROP FUNCTION IF EXISTS pg_catalog.atan2 (boolean, boolean);
CREATE OR REPLACE FUNCTION pg_catalog.atan2 (boolean, boolean)  RETURNS float8 AS $$ SELECT pg_catalog.atan2($1::int4, $2::int4) $$ LANGUAGE SQL;
DROP FUNCTION IF EXISTS pg_catalog.atan2 (boolean, float8);
CREATE OR REPLACE FUNCTION pg_catalog.atan2 (boolean, float8)  RETURNS float8 AS $$ SELECT pg_catalog.atan2($1::int4, $2) $$ LANGUAGE SQL;
DROP FUNCTION IF EXISTS pg_catalog.atan2 (float8, boolean);
CREATE OR REPLACE FUNCTION pg_catalog.atan2 (float8, boolean)  RETURNS float8 AS $$ SELECT pg_catalog.atan2($1, $2::int4) $$ LANGUAGE SQL;
DROP FUNCTION IF EXISTS pg_catalog.atan (float8, float8);
CREATE OR REPLACE FUNCTION pg_catalog.atan (float8, float8)  RETURNS float8 AS $$ SELECT pg_catalog.atan2($1, $2) $$ LANGUAGE SQL;
DROP FUNCTION IF EXISTS pg_catalog.atan (boolean, boolean);
CREATE OR REPLACE FUNCTION pg_catalog.atan (boolean, boolean)  RETURNS float8 AS $$ SELECT pg_catalog.atan2($1::int4, $2::int4) $$ LANGUAGE SQL;
DROP FUNCTION IF EXISTS pg_catalog.atan (boolean, float8);
CREATE OR REPLACE FUNCTION pg_catalog.atan (boolean, float8)  RETURNS float8 AS $$ SELECT pg_catalog.atan2($1::int4, $2) $$ LANGUAGE SQL;
DROP FUNCTION IF EXISTS pg_catalog.atan (float8, boolean);
CREATE OR REPLACE FUNCTION pg_catalog.atan (float8, boolean)  RETURNS float8 AS $$ SELECT pg_catalog.atan2($1, $2::int4) $$ LANGUAGE SQL;
DROP FUNCTION IF EXISTS pg_catalog.atan (boolean);
CREATE OR REPLACE FUNCTION pg_catalog.atan (boolean)  RETURNS float8 AS $$ SELECT pg_catalog.atan($1::int4) $$ LANGUAGE SQL;

drop CAST IF EXISTS (timestamptz as boolean);
drop CAST IF EXISTS (timestamp(0) without time zone as boolean);
DROP FUNCTION IF EXISTS pg_catalog.timestamptz_bool(timestamptz);
DROP FUNCTION IF EXISTS pg_catalog.timestamp_bool(timestamp(0) without time zone); 
CREATE OR REPLACE FUNCTION pg_catalog.timestamptz_bool(timestamptz) returns boolean LANGUAGE C immutable strict as '$libdir/dolphin', 'timestamptz_bool';
CREATE CAST (timestamptz as boolean) WITH FUNCTION timestamptz_bool(timestamptz) AS ASSIGNMENT;

CREATE OR REPLACE FUNCTION pg_catalog.timestamp_bool(timestamp(0) without time zone) returns boolean LANGUAGE C immutable strict as '$libdir/dolphin', 'timestamp_bool';
CREATE CAST (timestamp(0) without time zone as boolean) WITH FUNCTION timestamp_bool(timestamp(0) without time zone) AS ASSIGNMENT;

DROP FUNCTION IF EXISTS pg_catalog.oct(bit);
CREATE OR REPLACE FUNCTION pg_catalog.oct(bit) RETURNS text AS
$$
BEGIN
    RETURN 0;
END;
$$
LANGUAGE plpgsql;

DROP FUNCTION IF EXISTS pg_catalog.ord(varbit);
CREATE FUNCTION pg_catalog.ord (varbit) RETURNS int16 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'ord_bit';

DROP FUNCTION IF EXISTS pg_catalog.substring_index (text, text, numeric) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.substring_index (boolean, text, numeric) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.substring_index (text, boolean, numeric) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.substring_index (boolean, boolean, numeric) CASCADE;

DROP FUNCTION IF EXISTS pg_catalog.substring_index ("any", "any", text) CASCADE;
CREATE FUNCTION pg_catalog.substring_index (
"any",
"any",
text
) RETURNS text LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'substring_index_text';

DROP FUNCTION IF EXISTS pg_catalog.substring_index ("any", "any", numeric) CASCADE;
CREATE FUNCTION pg_catalog.substring_index (
"any",
"any",
numeric
) RETURNS text LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'substring_index_numeric';

-- below from 3.0
DROP FUNCTION IF EXISTS pg_catalog.date_cast(cstring, boolean);
CREATE OR REPLACE FUNCTION pg_catalog.date_cast(cstring, boolean) RETURNS date LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'date_cast';

DROP FUNCTION IF EXISTS pg_catalog.timestamp_cast(cstring, oid, integer, boolean);
CREATE OR REPLACE FUNCTION pg_catalog.timestamp_cast(cstring, oid, integer, boolean) RETURNS timestamp without time zone LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'timestamp_cast';

DROP FUNCTION IF EXISTS pg_catalog.bit_cast_int8(bit) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.bit_cast_int8 (
bit
) RETURNS int8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'bit_cast_int8';

DROP FUNCTION IF EXISTS pg_catalog.float4_cast_int8(float4) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.float4_cast_int8 (
float4
) RETURNS int8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'float4_cast_int8';

DROP FUNCTION IF EXISTS pg_catalog.float8_cast_int8(float8) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.float8_cast_int8 (
float8
) RETURNS int8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'float8_cast_int8';

DROP FUNCTION IF EXISTS pg_catalog.numeric_cast_int8(numeric) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.numeric_cast_int8 (
numeric
) RETURNS int8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'numeric_cast_int8';

DROP FUNCTION IF EXISTS pg_catalog.date_cast_int8(date) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.date_cast_int8 (
date
) RETURNS int8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'date_cast_int8';

DROP FUNCTION IF EXISTS pg_catalog.timestamp_cast_int8(timestamp without time zone) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.timestamp_cast_int8 (
timestamp without time zone
) RETURNS int8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'timestamp_cast_int8';

DROP FUNCTION IF EXISTS pg_catalog.timestamptz_cast_int8(timestamptz) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.timestamptz_cast_int8 (
timestamptz
) RETURNS int8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'timestamptz_cast_int8';

DROP FUNCTION IF EXISTS pg_catalog.time_cast_int8(time) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.time_cast_int8 (
time
) RETURNS int8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'time_cast_int8';

DROP FUNCTION IF EXISTS pg_catalog.timetz_cast_int8(timetz) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.timetz_cast_int8 (
timetz
) RETURNS int8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'timetz_cast_int8';

DROP FUNCTION IF EXISTS pg_catalog.set_cast_int8(anyset) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.set_cast_int8 (
anyset
) RETURNS int8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'set_cast_int8';

DROP FUNCTION IF EXISTS pg_catalog.uint8_cast_int8(uint8) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.uint8_cast_int8 (
uint8
) RETURNS int8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint8_cast_int8';

DROP FUNCTION IF EXISTS pg_catalog.year_cast_int8(year) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.year_cast_int8 (
year
) RETURNS int8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'year_cast_int8';

DROP FUNCTION IF EXISTS pg_catalog.bpchar_cast_int8(char) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.bpchar_cast_int8 (
char
) RETURNS int8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'bpchar_cast_int8';

DROP FUNCTION IF EXISTS pg_catalog.varchar_cast_int8(varchar) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.varchar_cast_int8 (
varchar
) RETURNS int8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'varchar_cast_int8';

DROP FUNCTION IF EXISTS pg_catalog.text_cast_int8(text) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.text_cast_int8 (
text
) RETURNS int8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'text_cast_int8';

DROP FUNCTION IF EXISTS pg_catalog.varlena_cast_int8(anyelement) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.varlena_cast_int8 (
anyelement
) RETURNS int8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'varlena_cast_int8';

DROP FUNCTION IF EXISTS pg_catalog.time_cast(cstring, boolean);
CREATE OR REPLACE FUNCTION pg_catalog.time_cast(cstring, boolean) RETURNS time without time zone LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'time_cast';

DROP CAST IF EXISTS (TEXT AS time);
DROP FUNCTION IF EXISTS pg_catalog.time_cast_implicit(TEXT);
CREATE OR REPLACE FUNCTION pg_catalog.time_cast_implicit(TEXT) RETURNS time without time zone LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'time_cast_implicit';

DROP FUNCTION IF EXISTS pg_catalog.text_time_explicit(TEXT);
CREATE OR REPLACE FUNCTION pg_catalog.text_time_explicit(TEXT) RETURNS time without time zone LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'text_time_explicit';

CREATE CAST(TEXT AS time) WITH FUNCTION time_cast_implicit(TEXT) AS ASSIGNMENT;

DROP FUNCTION IF EXISTS pg_catalog.day(time without time zone);
CREATE OR REPLACE FUNCTION pg_catalog.day(time without time zone) RETURNS int4 LANGUAGE C STABLE RETURNS NULL ON NULL INPUT as '$libdir/dolphin', 'dayofmonth_time';

--change bit -> int/bigint castcontext from 'e' to 'a'
do $$
begin
    update pg_cast set castcontext='a', castowner=10 where castsource=1560 and casttarget=20 and castcontext='e';
    update pg_cast set castcontext='a', castowner=10 where castsource=1560 and casttarget=23 and castcontext='e';
    update pg_cast set castcontext='a', castowner=10 where castsource=20 and casttarget=1560 and castcontext='e';
    update pg_cast set castcontext='a', castowner=10 where castsource=23 and casttarget=1560 and castcontext='e';
end
$$;
drop CAST IF EXISTS (uint4 AS bit);
drop CAST IF EXISTS (uint8 AS bit);
CREATE CAST (uint4 AS bit) WITH FUNCTION bitfromuint4(uint4, int4) AS ASSIGNMENT;
CREATE CAST (uint8 AS bit) WITH FUNCTION bitfromuint8(uint8, int4) AS ASSIGNMENT;

CREATE OR REPLACE FUNCTION pg_catalog.bit_length(boolean) RETURNS integer LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.bit_length(cast($1 as TEXT))';
CREATE OR REPLACE FUNCTION pg_catalog.bit_length(year) RETURNS integer LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.bit_length(cast($1 as TEXT))';
CREATE OR REPLACE FUNCTION pg_catalog.bit_length(blob) RETURNS integer LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.bit_length(cast($1 as TEXT))';
CREATE OR REPLACE FUNCTION pg_catalog.bit_length(anyenum) RETURNS integer LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.bit_length(cast($1 as TEXT))';
CREATE OR REPLACE FUNCTION pg_catalog.bit_length(json) RETURNS integer LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.bit_length(cast($1 as TEXT))';
CREATE OR REPLACE FUNCTION pg_catalog.bit_length(binary) RETURNS integer LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'mp_bit_length_binary';


create or replace function pg_catalog."user"() returns name as 'select current_user' LANGUAGE 'sql' IMMUTABLE;

create or replace function pg_catalog.varbinarylike(varbinary, varbinary) returns bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'binarylike';
CREATE OPERATOR pg_catalog.~~(leftarg = varbinary, rightarg = varbinary, procedure = pg_catalog.varbinarylike);

create or replace function pg_catalog.varbinarytextlike(varbinary, text) returns bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'binarylike';
CREATE OPERATOR pg_catalog.~~(leftarg = varbinary, rightarg = text, procedure = pg_catalog.varbinarytextlike);

create or replace function pg_catalog.textvarbinarylike(text, varbinary) returns bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'binarylike';
CREATE OPERATOR pg_catalog.~~(leftarg = text, rightarg = varbinary, procedure = pg_catalog.textvarbinarylike);

create or replace function pg_catalog.bloblike(blob, blob) returns bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'binarylike';
CREATE OPERATOR pg_catalog.~~(leftarg = blob, rightarg = blob, procedure = pg_catalog.bloblike);

create or replace function pg_catalog.binarylike(binary, binary) returns bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'binarylike';
create or replace function pg_catalog.binarytextlike(binary, text) returns bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'binarylike';
create or replace function pg_catalog.textbinarylike(text, binary) returns bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'binarylike';

CREATE OPERATOR pg_catalog.~~(leftarg = binary, rightarg = binary, procedure = pg_catalog.binarylike);

CREATE OR REPLACE FUNCTION pg_catalog.binary_cmp(binary, binary) RETURNS integer LANGUAGE INTERNAL IMMUTABLE STRICT as 'byteacmp';

CREATE OPERATOR FAMILY pg_catalog.binary_ops USING BTREE;
CREATE OPERATOR FAMILY pg_catalog.binary_ops USING HASH;

CREATE OPERATOR CLASS pg_catalog.binary_ops DEFAULT
   FOR TYPE binary USING BTREE FAMILY pg_catalog.binary_ops as
   OPERATOR 1 pg_catalog.<(binary, binary),
   OPERATOR 2 pg_catalog.<=(binary, binary),
   OPERATOR 3 pg_catalog.=(binary, binary),
   OPERATOR 4 pg_catalog.>=(binary, binary),
   OPERATOR 5 pg_catalog.>(binary, binary),
   FUNCTION 1 pg_catalog.binary_cmp(binary, binary),
   FUNCTION 2 pg_catalog.bytea_sortsupport(internal);

CREATE OPERATOR CLASS pg_catalog.binary_ops DEFAULT
   FOR TYPE binary USING HASH FAMILY binary_ops as
   OPERATOR 1 pg_catalog.=(binary, binary),
   FUNCTION 1 (binary, binary) pg_catalog.hashvarlena(internal);

CREATE OR REPLACE FUNCTION pg_catalog.varbinary_cmp(varbinary, varbinary) RETURNS integer LANGUAGE INTERNAL IMMUTABLE STRICT as 'byteacmp';
CREATE OPERATOR FAMILY pg_catalog.varbinary_ops USING BTREE;
CREATE OPERATOR FAMILY pg_catalog.varbinary_ops USING HASH;

CREATE OPERATOR CLASS pg_catalog.varbinary_ops DEFAULT
   FOR TYPE varbinary USING BTREE FAMILY pg_catalog.varbinary_ops as
   OPERATOR 1 pg_catalog.<(varbinary, varbinary),
   OPERATOR 2 pg_catalog.<=(varbinary, varbinary),
   OPERATOR 3 pg_catalog.=(varbinary, varbinary),
   OPERATOR 4 pg_catalog.>=(varbinary, varbinary),
   OPERATOR 5 pg_catalog.>(varbinary, varbinary),
   FUNCTION 1 pg_catalog.varbinary_cmp(varbinary, varbinary),
   FUNCTION 2 pg_catalog.bytea_sortsupport(internal);

CREATE OPERATOR CLASS pg_catalog.varbinary_ops DEFAULT
   FOR TYPE varbinary USING HASH FAMILY pg_catalog.varbinary_ops as
   OPERATOR 1 pg_catalog.=(varbinary, varbinary),
   FUNCTION 1 (varbinary, varbinary) pg_catalog.hashvarlena(internal);



--CREATE TIME_TIMESTAMP'S COMPARATION FUNCTION
DROP FUNCTION IF EXISTS pg_catalog.time_eq_timestamp (time, timestamp without time zone) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.time_eq_timestamp (time, timestamp without time zone) RETURNS boolean LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'time_eq_timestamp';
DROP FUNCTION IF EXISTS pg_catalog.time_ne_timestamp (time, timestamp without time zone) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.time_ne_timestamp (time, timestamp without time zone) RETURNS boolean LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'time_ne_timestamp';
DROP FUNCTION IF EXISTS pg_catalog.time_le_timestamp (time, timestamp without time zone) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.time_le_timestamp (time, timestamp without time zone) RETURNS boolean LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'time_le_timestamp';
DROP FUNCTION IF EXISTS pg_catalog.time_lt_timestamp (time, timestamp without time zone) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.time_lt_timestamp (time, timestamp without time zone) RETURNS boolean LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'time_lt_timestamp';
DROP FUNCTION IF EXISTS pg_catalog.time_ge_timestamp (time, timestamp without time zone) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.time_ge_timestamp (time, timestamp without time zone) RETURNS boolean LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'time_ge_timestamp';
DROP FUNCTION IF EXISTS pg_catalog.time_gt_timestamp (time, timestamp without time zone) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.time_gt_timestamp (time, timestamp without time zone) RETURNS boolean LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'time_gt_timestamp';
--CREATE TIME_TIMESTAMP CMP OPERATOR

CREATE OPERATOR pg_catalog.=(leftarg = time, rightarg = timestamp without time zone, procedure = time_eq_timestamp, restrict = eqsel, join = eqjoinsel, MERGES);

CREATE OPERATOR pg_catalog.<>(leftarg = time, rightarg = timestamp without time zone, procedure = time_ne_timestamp, restrict = neqsel, join = neqjoinsel);

CREATE OPERATOR pg_catalog.<=(leftarg = time, rightarg = timestamp without time zone, procedure = time_le_timestamp, COMMUTATOR  = >=, NEGATOR  = >, restrict = scalarltsel, join = scalarltjoinsel);

CREATE OPERATOR pg_catalog.<(leftarg = time, rightarg = timestamp without time zone, procedure = time_lt_timestamp, COMMUTATOR  = >, NEGATOR  = >=, restrict = scalarltsel, join = scalarltjoinsel);

CREATE OPERATOR pg_catalog.>=(leftarg = time, rightarg = timestamp without time zone, procedure = time_ge_timestamp, COMMUTATOR  = <=, NEGATOR  = <, restrict = scalarltsel, join = scalarltjoinsel);

CREATE OPERATOR pg_catalog.>(leftarg = time, rightarg = timestamp without time zone, procedure = time_gt_timestamp, COMMUTATOR  = <, NEGATOR  = <=, restrict = scalarltsel, join = scalarltjoinsel);
--CREATE TIMESTAMP_TIME'S COMPARATION FUNCTION
DROP FUNCTION IF EXISTS pg_catalog.timestamp_eq_time (timestamp without time zone, time) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.timestamp_eq_time (timestamp without time zone, time) RETURNS boolean LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'timestamp_eq_time';
DROP FUNCTION IF EXISTS pg_catalog.timestamp_ne_time (timestamp without time zone, time) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.timestamp_ne_time (timestamp without time zone, time) RETURNS boolean LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'timestamp_ne_time';
DROP FUNCTION IF EXISTS pg_catalog.timestamp_le_time (timestamp without time zone, time) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.timestamp_le_time (timestamp without time zone, time) RETURNS boolean LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'timestamp_le_time';
DROP FUNCTION IF EXISTS pg_catalog.timestamp_lt_time (timestamp without time zone, time) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.timestamp_lt_time (timestamp without time zone, time) RETURNS boolean LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'timestamp_lt_time';
DROP FUNCTION IF EXISTS pg_catalog.timestamp_ge_time (timestamp without time zone, time) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.timestamp_ge_time (timestamp without time zone, time) RETURNS boolean LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'timestamp_ge_time';
DROP FUNCTION IF EXISTS pg_catalog.timestamp_gt_time (timestamp without time zone, time) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.timestamp_gt_time (timestamp without time zone, time) RETURNS boolean LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'timestamp_gt_time';
--CREATE TIMESTAMP_TIME CMP OPERATOR

CREATE OPERATOR pg_catalog.=(leftarg = timestamp without time zone, rightarg = time, procedure = timestamp_eq_time, restrict = eqsel, join = eqjoinsel, MERGES);

CREATE OPERATOR pg_catalog.<>(leftarg = timestamp without time zone, rightarg = time, procedure = timestamp_ne_time, restrict = neqsel, join = neqjoinsel);

CREATE OPERATOR pg_catalog.<=(leftarg = timestamp without time zone, rightarg = time, procedure = timestamp_le_time, COMMUTATOR  = >=, NEGATOR  = >, restrict = scalarltsel, join = scalarltjoinsel);

CREATE OPERATOR pg_catalog.<(leftarg = timestamp without time zone, rightarg = time, procedure = timestamp_lt_time, COMMUTATOR  = >, NEGATOR  = >=, restrict = scalarltsel, join = scalarltjoinsel);

CREATE OPERATOR pg_catalog.>=(leftarg = timestamp without time zone, rightarg = time, procedure = timestamp_ge_time, COMMUTATOR  = <=, NEGATOR  = <, restrict = scalarltsel, join = scalarltjoinsel);

CREATE OPERATOR pg_catalog.>(leftarg = timestamp without time zone, rightarg = time, procedure = timestamp_gt_time, COMMUTATOR  = <, NEGATOR  = <=, restrict = scalarltsel, join = scalarltjoinsel);

--CREATE TIME_TIMESTAMPTZ'S COMPARATION FUNCTION
DROP FUNCTION IF EXISTS pg_catalog.time_eq_timestamptz (time, timestamp with time zone) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.time_eq_timestamptz (time, timestamp with time zone) RETURNS boolean LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'time_eq_timestamptz';
DROP FUNCTION IF EXISTS pg_catalog.time_ne_timestamptz (time, timestamp with time zone) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.time_ne_timestamptz (time, timestamp with time zone) RETURNS boolean LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'time_ne_timestamptz';
DROP FUNCTION IF EXISTS pg_catalog.time_le_timestamptz (time, timestamp with time zone) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.time_le_timestamptz (time, timestamp with time zone) RETURNS boolean LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'time_le_timestamptz';
DROP FUNCTION IF EXISTS pg_catalog.time_lt_timestamptz (time, timestamp with time zone) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.time_lt_timestamptz (time, timestamp with time zone) RETURNS boolean LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'time_lt_timestamptz';
DROP FUNCTION IF EXISTS pg_catalog.time_ge_timestamptz (time, timestamp with time zone) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.time_ge_timestamptz (time, timestamp with time zone) RETURNS boolean LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'time_ge_timestamptz';
DROP FUNCTION IF EXISTS pg_catalog.time_gt_timestamptz (time, timestamp with time zone) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.time_gt_timestamptz (time, timestamp with time zone) RETURNS boolean LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'time_gt_timestamptz';
--CREATE TIME_TIMESTAMPTZ CMP OPERATOR

CREATE OPERATOR pg_catalog.=(leftarg = time, rightarg = timestamp with time zone, procedure = time_eq_timestamptz, restrict = eqsel, join = eqjoinsel, MERGES);

CREATE OPERATOR pg_catalog.<>(leftarg = time, rightarg = timestamp with time zone, procedure = time_ne_timestamptz, restrict = neqsel, join = neqjoinsel);

CREATE OPERATOR pg_catalog.<=(leftarg = time, rightarg = timestamp with time zone, procedure = time_le_timestamptz, COMMUTATOR  = >=, NEGATOR  = >, restrict = scalarltsel, join = scalarltjoinsel);

CREATE OPERATOR pg_catalog.<(leftarg = time, rightarg = timestamp with time zone, procedure = time_lt_timestamptz, COMMUTATOR  = >, NEGATOR  = >=, restrict = scalarltsel, join = scalarltjoinsel);

CREATE OPERATOR pg_catalog.>=(leftarg = time, rightarg = timestamp with time zone, procedure = time_ge_timestamptz, COMMUTATOR  = <=, NEGATOR  = <, restrict = scalarltsel, join = scalarltjoinsel);

CREATE OPERATOR pg_catalog.>(leftarg = time, rightarg = timestamp with time zone, procedure = time_gt_timestamptz, COMMUTATOR  = <, NEGATOR  = <=, restrict = scalarltsel, join = scalarltjoinsel);
--CREATE TIMESTAMPTZ_TIME'S COMPARATION FUNCTION
DROP FUNCTION IF EXISTS pg_catalog.timestamptz_eq_time (timestamp with time zone, time) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.timestamptz_eq_time (timestamp with time zone, time) RETURNS boolean LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'timestamptz_eq_time';
DROP FUNCTION IF EXISTS pg_catalog.timestamptz_ne_time (timestamp with time zone, time) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.timestamptz_ne_time (timestamp with time zone, time) RETURNS boolean LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'timestamptz_ne_time';
DROP FUNCTION IF EXISTS pg_catalog.timestamptz_le_time (timestamp with time zone, time) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.timestamptz_le_time (timestamp with time zone, time) RETURNS boolean LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'timestamptz_le_time';
DROP FUNCTION IF EXISTS pg_catalog.timestamptz_lt_time (timestamp with time zone, time) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.timestamptz_lt_time (timestamp with time zone, time) RETURNS boolean LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'timestamptz_lt_time';
DROP FUNCTION IF EXISTS pg_catalog.timestamptz_ge_time (timestamp with time zone, time) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.timestamptz_ge_time (timestamp with time zone, time) RETURNS boolean LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'timestamptz_ge_time';
DROP FUNCTION IF EXISTS pg_catalog.timestamptz_gt_time (timestamp with time zone, time) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.timestamptz_gt_time (timestamp with time zone, time) RETURNS boolean LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'timestamptz_gt_time';
--CREATE TIMESTAMPTZ_TIME CMP OPERATOR

CREATE OPERATOR pg_catalog.=(leftarg = timestamp with time zone, rightarg = time, procedure = timestamptz_eq_time, restrict = eqsel, join = eqjoinsel, MERGES);

CREATE OPERATOR pg_catalog.<>(leftarg = timestamp with time zone, rightarg = time, procedure = timestamptz_ne_time, restrict = neqsel, join = neqjoinsel);

CREATE OPERATOR pg_catalog.<=(leftarg = timestamp with time zone, rightarg = time, procedure = timestamptz_le_time, COMMUTATOR  = >=, NEGATOR  = >, restrict = scalarltsel, join = scalarltjoinsel);

CREATE OPERATOR pg_catalog.<(leftarg = timestamp with time zone, rightarg = time, procedure = timestamptz_lt_time, COMMUTATOR  = >, NEGATOR  = >=, restrict = scalarltsel, join = scalarltjoinsel);

CREATE OPERATOR pg_catalog.>=(leftarg = timestamp with time zone, rightarg = time, procedure = timestamptz_ge_time, COMMUTATOR  = <=, NEGATOR  = <, restrict = scalarltsel, join = scalarltjoinsel);

CREATE OPERATOR pg_catalog.>(leftarg = timestamp with time zone, rightarg = time, procedure = timestamptz_gt_time, COMMUTATOR  = <, NEGATOR  = <=, restrict = scalarltsel, join = scalarltjoinsel);

-- Make the result of oct(bit) and conv(bit) identical to Mysql
DROP FUNCTION IF EXISTS pg_catalog.conv(bit, int4, int4) CASCADE;

CREATE OR REPLACE FUNCTION pg_catalog.conv(bit, int4, int4) 
RETURNS text AS $$ SELECT pg_catalog.conv($1::int8, 10, $3) $$ LANGUAGE SQL;

DROP FUNCTION IF EXISTS pg_catalog.oct(bit);

CREATE OR REPLACE FUNCTION pg_catalog.oct(t1 bit)
RETURNS text AS $$ SELECT pg_catalog.conv(t1, 10, 8) $$ LANGUAGE SQL;

DROP FUNCTION IF EXISTS pg_catalog.time_to_sec(int8);
CREATE OR REPLACE FUNCTION pg_catalog.time_to_sec(int8) RETURNS int8 LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'int64_time_to_sec';

DROP FUNCTION IF EXISTS pg_catalog.time_to_sec(numeric);
CREATE OR REPLACE FUNCTION pg_catalog.time_to_sec(numeric) RETURNS int8 LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'numeric_time_to_sec';

-- Supplement function dayofmonth to make the function dayofmonth() the same as function day()
DROP FUNCTION IF EXISTS pg_catalog.dayofmonth(text);
DROP FUNCTION IF EXISTS pg_catalog.dayofmonth(numeric);
CREATE OR REPLACE FUNCTION pg_catalog.dayofmonth(text) RETURNS int4 AS $$ SELECT pg_catalog.day($1); $$ LANGUAGE SQL;
CREATE OR REPLACE FUNCTION pg_catalog.dayofmonth(numeric) RETURNS int4 AS $$ SELECT pg_catalog.day($1); $$ LANGUAGE SQL;

DROP FUNCTION IF EXISTS pg_catalog.date_format (time without time zone, text);
CREATE OR REPLACE FUNCTION pg_catalog.date_format (time without time zone, text) RETURNS TEXT LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'date_format_time';

DROP FUNCTION IF EXISTS pg_catalog.to_char(time without time zone, text);
CREATE OR REPLACE FUNCTION pg_catalog.to_char(time without time zone, text) RETURNS TEXT LANGUAGE SQL IMMUTABLE STRICT as $$ SELECT pg_catalog.to_char($1::interval, $2) $$;

CREATE OR REPLACE FUNCTION pg_catalog.bit_cast_date(bit) 
RETURNS date LANGUAGE SQL IMMUTABLE STRICT as 
'select cast(cast($1 as text) as date)';

CREATE OR REPLACE FUNCTION pg_catalog.bit_cast_datetime(bit) 
RETURNS timestamp without time zone LANGUAGE SQL IMMUTABLE STRICT as 
'select cast(cast($1 as text) as timestamp without time zone)';

CREATE OR REPLACE FUNCTION pg_catalog.bit_cast_timestamp(bit) 
RETURNS timestamptz LANGUAGE SQL IMMUTABLE STRICT as 
'select cast(cast($1 as text) as timestamptz)';

CREATE OR REPLACE FUNCTION pg_catalog.bit_cast_time(bit) 
RETURNS time without time zone LANGUAGE SQL IMMUTABLE STRICT as 
'select cast(cast($1 as text) as time without time zone)';
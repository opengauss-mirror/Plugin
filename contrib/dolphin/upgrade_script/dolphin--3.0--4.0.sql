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

-- bit bool compare
CREATE OR REPLACE FUNCTION pg_catalog.bool_bit_gt(boolean, bit) returns bool LANGUAGE SQL IMMUTABLE STRICT as 'select ($1::int > $2::int)';
CREATE OPERATOR pg_catalog.>(leftarg = boolean, rightarg = bit, COMMUTATOR = operator(pg_catalog.<), procedure = pg_catalog.bool_bit_gt);
CREATE OR REPLACE FUNCTION pg_catalog.bool_bit_ge(boolean, bit) returns bool LANGUAGE SQL IMMUTABLE STRICT as 'select ($1::int >= $2::int)';
CREATE OPERATOR pg_catalog.>=(leftarg = boolean, rightarg = bit, COMMUTATOR = operator(pg_catalog.<=), procedure = pg_catalog.bool_bit_ge);
CREATE OR REPLACE FUNCTION pg_catalog.bool_bit_lt(boolean, bit) returns bool LANGUAGE SQL IMMUTABLE STRICT as 'select ($1::int < $2::int)';
CREATE OPERATOR pg_catalog.<(leftarg = boolean, rightarg = bit, COMMUTATOR = operator(pg_catalog.>), procedure = pg_catalog.bool_bit_lt);
CREATE OR REPLACE FUNCTION pg_catalog.bool_bit_le(boolean, bit) returns bool LANGUAGE SQL IMMUTABLE STRICT as 'select ($1::int <= $2::int)';
CREATE OPERATOR pg_catalog.<=(leftarg = boolean, rightarg = bit, COMMUTATOR = operator(pg_catalog.>=), procedure = pg_catalog.bool_bit_le);
CREATE OR REPLACE FUNCTION pg_catalog.bit_bool_gt(bit, boolean) returns bool LANGUAGE SQL IMMUTABLE STRICT as 'select ($1::int > $2::int)';
CREATE OPERATOR pg_catalog.>(leftarg = bit, rightarg = boolean, COMMUTATOR = operator(pg_catalog.<), procedure = pg_catalog.bit_bool_gt);
CREATE OR REPLACE FUNCTION pg_catalog.bit_bool_ge(bit, boolean) returns bool LANGUAGE SQL IMMUTABLE STRICT as 'select ($1::int >= $2::int)';
CREATE OPERATOR pg_catalog.>=(leftarg = bit, rightarg = boolean, COMMUTATOR = operator(pg_catalog.<=), procedure = pg_catalog.bit_bool_ge);
CREATE OR REPLACE FUNCTION pg_catalog.bit_bool_lt(bit, boolean) returns bool LANGUAGE SQL IMMUTABLE STRICT as 'select ($1::int < $2::int)';
CREATE OPERATOR pg_catalog.<(leftarg = bit, rightarg = boolean, COMMUTATOR = operator(pg_catalog.>), procedure = pg_catalog.bit_bool_lt);
CREATE OR REPLACE FUNCTION pg_catalog.bit_bool_le(bit, boolean) returns bool LANGUAGE SQL IMMUTABLE STRICT as 'select ($1::int <= $2::int)';
CREATE OPERATOR pg_catalog.<=(leftarg = bit, rightarg = boolean, COMMUTATOR = operator(pg_catalog.>=), procedure = pg_catalog.bit_bool_le);
-- bit date compare
CREATE OR REPLACE FUNCTION pg_catalog.date_bit_gt(date, bit) returns bool LANGUAGE SQL IMMUTABLE STRICT as 'select ($1::float4 > $2::float4)';
CREATE OPERATOR pg_catalog.>(leftarg = date, rightarg = bit, COMMUTATOR = operator(pg_catalog.<), procedure = pg_catalog.date_bit_gt);
CREATE OR REPLACE FUNCTION pg_catalog.date_bit_ge(date, bit) returns bool LANGUAGE SQL IMMUTABLE STRICT as 'select ($1::float4 >= $2::float4)';
CREATE OPERATOR pg_catalog.>=(leftarg = date, rightarg = bit, COMMUTATOR = operator(pg_catalog.<=), procedure = pg_catalog.date_bit_ge);
CREATE OR REPLACE FUNCTION pg_catalog.date_bit_lt(date, bit) returns bool LANGUAGE SQL IMMUTABLE STRICT as 'select ($1::float4 < $2::float4)';
CREATE OPERATOR pg_catalog.<(leftarg = date, rightarg = bit, COMMUTATOR = operator(pg_catalog.>), procedure = pg_catalog.date_bit_lt);
CREATE OR REPLACE FUNCTION pg_catalog.date_bit_le(date, bit) returns bool LANGUAGE SQL IMMUTABLE STRICT as 'select ($1::float4 <= $2::float4)';
CREATE OPERATOR pg_catalog.<=(leftarg = date, rightarg = bit, COMMUTATOR = operator(pg_catalog.>=), procedure = pg_catalog.date_bit_le);
CREATE OR REPLACE FUNCTION pg_catalog.bit_date_gt(bit, date) returns bool LANGUAGE SQL IMMUTABLE STRICT as 'select ($1::float4 > $2::float4)';
CREATE OPERATOR pg_catalog.>(leftarg = bit, rightarg = date, COMMUTATOR = operator(pg_catalog.<), procedure = pg_catalog.bit_date_gt);
CREATE OR REPLACE FUNCTION pg_catalog.bit_date_ge(bit, date) returns bool LANGUAGE SQL IMMUTABLE STRICT as 'select ($1::float4 >= $2::float4)';
CREATE OPERATOR pg_catalog.>=(leftarg = bit, rightarg = date, COMMUTATOR = operator(pg_catalog.<=), procedure = pg_catalog.bit_date_ge);
CREATE OR REPLACE FUNCTION pg_catalog.bit_date_lt(bit, date) returns bool LANGUAGE SQL IMMUTABLE STRICT as 'select ($1::float4 < $2::float4)';
CREATE OPERATOR pg_catalog.<(leftarg = bit, rightarg = date, COMMUTATOR = operator(pg_catalog.>), procedure = pg_catalog.bit_date_lt);
CREATE OR REPLACE FUNCTION pg_catalog.bit_date_le(bit, date) returns bool LANGUAGE SQL IMMUTABLE STRICT as 'select ($1::float4 <= $2::float4)';
CREATE OPERATOR pg_catalog.<=(leftarg = bit, rightarg = date, COMMUTATOR = operator(pg_catalog.>=), procedure = pg_catalog.bit_date_le);
-- bit timestamp without time zone compare
CREATE OR REPLACE FUNCTION pg_catalog.timestamp_bit_gt(timestamp without time zone, bit) returns bool LANGUAGE SQL IMMUTABLE STRICT as 'select ($1::float4 > $2::float4)';
CREATE OPERATOR pg_catalog.>(leftarg = timestamp without time zone, rightarg = bit, COMMUTATOR = operator(pg_catalog.<), procedure = pg_catalog.timestamp_bit_gt);
CREATE OR REPLACE FUNCTION pg_catalog.timestamp_bit_ge(timestamp without time zone, bit) returns bool LANGUAGE SQL IMMUTABLE STRICT as 'select ($1::float4 >= $2::float4)';
CREATE OPERATOR pg_catalog.>=(leftarg = timestamp without time zone, rightarg = bit, COMMUTATOR = operator(pg_catalog.<=), procedure = pg_catalog.timestamp_bit_ge);
CREATE OR REPLACE FUNCTION pg_catalog.timestamp_bit_lt(timestamp without time zone, bit) returns bool LANGUAGE SQL IMMUTABLE STRICT as 'select ($1::float4 < $2::float4)';
CREATE OPERATOR pg_catalog.<(leftarg = timestamp without time zone, rightarg = bit, COMMUTATOR = operator(pg_catalog.>), procedure = pg_catalog.timestamp_bit_lt);
CREATE OR REPLACE FUNCTION pg_catalog.timestamp_bit_le(timestamp without time zone, bit) returns bool LANGUAGE SQL IMMUTABLE STRICT as 'select ($1::float4 <= $2::float4)';
CREATE OPERATOR pg_catalog.<=(leftarg = timestamp without time zone, rightarg = bit, COMMUTATOR = operator(pg_catalog.>=), procedure = pg_catalog.timestamp_bit_le);
CREATE OR REPLACE FUNCTION pg_catalog.bit_timestamp_gt(bit, timestamp without time zone) returns bool LANGUAGE SQL IMMUTABLE STRICT as 'select ($1::float4 > $2::float4)';
CREATE OPERATOR pg_catalog.>(leftarg = bit, rightarg = timestamp without time zone, COMMUTATOR = operator(pg_catalog.<), procedure = pg_catalog.bit_timestamp_gt);
CREATE OR REPLACE FUNCTION pg_catalog.bit_timestamp_ge(bit, timestamp without time zone) returns bool LANGUAGE SQL IMMUTABLE STRICT as 'select ($1::float4 >= $2::float4)';
CREATE OPERATOR pg_catalog.>=(leftarg = bit, rightarg = timestamp without time zone, COMMUTATOR = operator(pg_catalog.<=), procedure = pg_catalog.bit_timestamp_ge);
CREATE OR REPLACE FUNCTION pg_catalog.bit_timestamp_lt(bit, timestamp without time zone) returns bool LANGUAGE SQL IMMUTABLE STRICT as 'select ($1::float4 < $2::float4)';
CREATE OPERATOR pg_catalog.<(leftarg = bit, rightarg = timestamp without time zone, COMMUTATOR = operator(pg_catalog.>), procedure = pg_catalog.bit_timestamp_lt);
CREATE OR REPLACE FUNCTION pg_catalog.bit_timestamp_le(bit, timestamp without time zone) returns bool LANGUAGE SQL IMMUTABLE STRICT as 'select ($1::float4 <= $2::float4)';
CREATE OPERATOR pg_catalog.<=(leftarg = bit, rightarg = timestamp without time zone, COMMUTATOR = operator(pg_catalog.>=), procedure = pg_catalog.bit_timestamp_le);
-- bit timestamp with time zone
CREATE OR REPLACE FUNCTION pg_catalog.timestamptz_bit_gt(timestamp with time zone, bit) returns bool LANGUAGE SQL IMMUTABLE STRICT as 'select ($1::float4 > $2::float4)';
CREATE OPERATOR pg_catalog.>(leftarg = timestamp with time zone, rightarg = bit, COMMUTATOR = operator(pg_catalog.<), procedure = pg_catalog.timestamptz_bit_gt);
CREATE OR REPLACE FUNCTION pg_catalog.timestamptz_bit_ge(timestamp with time zone, bit) returns bool LANGUAGE SQL IMMUTABLE STRICT as 'select ($1::float4 >= $2::float4)';
CREATE OPERATOR pg_catalog.>=(leftarg = timestamp with time zone, rightarg = bit, COMMUTATOR = operator(pg_catalog.<=), procedure = pg_catalog.timestamptz_bit_ge);
CREATE OR REPLACE FUNCTION pg_catalog.timestamptz_bit_lt(timestamp with time zone, bit) returns bool LANGUAGE SQL IMMUTABLE STRICT as 'select ($1::float4 < $2::float4)';
CREATE OPERATOR pg_catalog.<(leftarg = timestamp with time zone, rightarg = bit, COMMUTATOR = operator(pg_catalog.>), procedure = pg_catalog.timestamptz_bit_lt);
CREATE OR REPLACE FUNCTION pg_catalog.timestamptz_bit_le(timestamp with time zone, bit) returns bool LANGUAGE SQL IMMUTABLE STRICT as 'select ($1::float4 <= $2::float4)';
CREATE OPERATOR pg_catalog.<=(leftarg = timestamp with time zone, rightarg = bit, COMMUTATOR = operator(pg_catalog.>=), procedure = pg_catalog.timestamptz_bit_le);
CREATE OR REPLACE FUNCTION pg_catalog.bit_timestamptz_gt(bit, timestamp with time zone) returns bool LANGUAGE SQL IMMUTABLE STRICT as 'select ($1::float4 > $2::float4)';
CREATE OPERATOR pg_catalog.>(leftarg = bit, rightarg = timestamp with time zone, COMMUTATOR = operator(pg_catalog.<), procedure = pg_catalog.bit_timestamptz_gt);
CREATE OR REPLACE FUNCTION pg_catalog.bit_timestamptz_ge(bit, timestamp with time zone) returns bool LANGUAGE SQL IMMUTABLE STRICT as 'select ($1::float4 >= $2::float4)';
CREATE OPERATOR pg_catalog.>=(leftarg = bit, rightarg = timestamp with time zone, COMMUTATOR = operator(pg_catalog.<=), procedure = pg_catalog.bit_timestamptz_ge);
CREATE OR REPLACE FUNCTION pg_catalog.bit_timestamptz_lt(bit, timestamp with time zone) returns bool LANGUAGE SQL IMMUTABLE STRICT as 'select ($1::float4 < $2::float4)';
CREATE OPERATOR pg_catalog.<(leftarg = bit, rightarg = timestamp with time zone, COMMUTATOR = operator(pg_catalog.>), procedure = pg_catalog.bit_timestamptz_lt);
CREATE OR REPLACE FUNCTION pg_catalog.bit_timestamptz_le(bit, timestamp with time zone) returns bool LANGUAGE SQL IMMUTABLE STRICT as 'select ($1::float4 <= $2::float4)';
CREATE OPERATOR pg_catalog.<=(leftarg = bit, rightarg = timestamp with time zone, COMMUTATOR = operator(pg_catalog.>=), procedure = pg_catalog.bit_timestamptz_le);
-- bit year compare
CREATE OR REPLACE FUNCTION pg_catalog.year_bit_gt(year, bit) returns bool LANGUAGE SQL IMMUTABLE STRICT as 'select ($1::float4 > $2::float4)';
CREATE OPERATOR pg_catalog.>(leftarg = year, rightarg = bit, COMMUTATOR = operator(pg_catalog.<), procedure = pg_catalog.year_bit_gt);
CREATE OR REPLACE FUNCTION pg_catalog.year_bit_ge(year, bit) returns bool LANGUAGE SQL IMMUTABLE STRICT as 'select ($1::float4 >= $2::float4)';
CREATE OPERATOR pg_catalog.>=(leftarg = year, rightarg = bit, COMMUTATOR = operator(pg_catalog.<=), procedure = pg_catalog.year_bit_ge);
CREATE OR REPLACE FUNCTION pg_catalog.year_bit_lt(year, bit) returns bool LANGUAGE SQL IMMUTABLE STRICT as 'select ($1::float4 < $2::float4)';
CREATE OPERATOR pg_catalog.<(leftarg = year, rightarg = bit, COMMUTATOR = operator(pg_catalog.>), procedure = pg_catalog.year_bit_lt);
CREATE OR REPLACE FUNCTION pg_catalog.year_bit_le(year, bit) returns bool LANGUAGE SQL IMMUTABLE STRICT as 'select ($1::float4 <= $2::float4)';
CREATE OPERATOR pg_catalog.<=(leftarg = year, rightarg = bit, COMMUTATOR = operator(pg_catalog.>=), procedure = pg_catalog.year_bit_le);
CREATE OR REPLACE FUNCTION pg_catalog.bit_year_gt(bit, year) returns bool LANGUAGE SQL IMMUTABLE STRICT as 'select ($1::float4 > $2::float4)';
CREATE OPERATOR pg_catalog.>(leftarg = bit, rightarg = year, COMMUTATOR = operator(pg_catalog.<), procedure = pg_catalog.bit_year_gt);
CREATE OR REPLACE FUNCTION pg_catalog.bit_year_ge(bit, year) returns bool LANGUAGE SQL IMMUTABLE STRICT as 'select ($1::float4 >= $2::float4)';
CREATE OPERATOR pg_catalog.>=(leftarg = bit, rightarg = year, COMMUTATOR = operator(pg_catalog.<=), procedure = pg_catalog.bit_year_ge);
CREATE OR REPLACE FUNCTION pg_catalog.bit_year_lt(bit, year) returns bool LANGUAGE SQL IMMUTABLE STRICT as 'select ($1::float4 < $2::float4)';
CREATE OPERATOR pg_catalog.<(leftarg = bit, rightarg = year, COMMUTATOR = operator(pg_catalog.>), procedure = pg_catalog.bit_year_lt);
CREATE OR REPLACE FUNCTION pg_catalog.bit_year_le(bit, year) returns bool LANGUAGE SQL IMMUTABLE STRICT as 'select ($1::float4 <= $2::float4)';
CREATE OPERATOR pg_catalog.<=(leftarg = bit, rightarg = year, COMMUTATOR = operator(pg_catalog.>=), procedure = pg_catalog.bit_year_le);
-- bit time compare
CREATE OR REPLACE FUNCTION pg_catalog.time_bit_gt(time, bit) returns bool LANGUAGE SQL IMMUTABLE STRICT as 'select ($1::float4 > $2::float4)';
CREATE OPERATOR pg_catalog.>(leftarg = time, rightarg = bit, COMMUTATOR = operator(pg_catalog.<), procedure = pg_catalog.time_bit_gt);
CREATE OR REPLACE FUNCTION pg_catalog.time_bit_ge(time, bit) returns bool LANGUAGE SQL IMMUTABLE STRICT as 'select ($1::float4 >= $2::float4)';
CREATE OPERATOR pg_catalog.>=(leftarg = time, rightarg = bit, COMMUTATOR = operator(pg_catalog.<=), procedure = pg_catalog.time_bit_ge);
CREATE OR REPLACE FUNCTION pg_catalog.time_bit_lt(time, bit) returns bool LANGUAGE SQL IMMUTABLE STRICT as 'select ($1::float4 < $2::float4)';
CREATE OPERATOR pg_catalog.<(leftarg = time, rightarg = bit, COMMUTATOR = operator(pg_catalog.>), procedure = pg_catalog.time_bit_lt);
CREATE OR REPLACE FUNCTION pg_catalog.time_bit_le(time, bit) returns bool LANGUAGE SQL IMMUTABLE STRICT as 'select ($1::float4 <= $2::float4)';
CREATE OPERATOR pg_catalog.<=(leftarg = time, rightarg = bit, COMMUTATOR = operator(pg_catalog.>=), procedure = pg_catalog.time_bit_le);
CREATE OR REPLACE FUNCTION pg_catalog.bit_time_gt(bit, time) returns bool LANGUAGE SQL IMMUTABLE STRICT as 'select ($1::float4 > $2::float4)';
CREATE OPERATOR pg_catalog.>(leftarg = bit, rightarg = time, COMMUTATOR = operator(pg_catalog.<), procedure = pg_catalog.bit_time_gt);
CREATE OR REPLACE FUNCTION pg_catalog.bit_time_ge(bit, time) returns bool LANGUAGE SQL IMMUTABLE STRICT as 'select ($1::float4 >= $2::float4)';
CREATE OPERATOR pg_catalog.>=(leftarg = bit, rightarg = time, COMMUTATOR = operator(pg_catalog.<=), procedure = pg_catalog.bit_time_ge);
CREATE OR REPLACE FUNCTION pg_catalog.bit_time_lt(bit, time) returns bool LANGUAGE SQL IMMUTABLE STRICT as 'select ($1::float4 < $2::float4)';
CREATE OPERATOR pg_catalog.<(leftarg = bit, rightarg = time, COMMUTATOR = operator(pg_catalog.>), procedure = pg_catalog.bit_time_lt);
CREATE OR REPLACE FUNCTION pg_catalog.bit_time_le(bit, time) returns bool LANGUAGE SQL IMMUTABLE STRICT as 'select ($1::float4 <= $2::float4)';
CREATE OPERATOR pg_catalog.<=(leftarg = bit, rightarg = time, COMMUTATOR = operator(pg_catalog.>=), procedure = pg_catalog.bit_time_le);
-- bit uint8 compare
CREATE OR REPLACE FUNCTION pg_catalog.uint8_bit_gt(uint8, bit) returns bool LANGUAGE SQL IMMUTABLE STRICT as 'select ($1::float4 > $2::float4)';
CREATE OPERATOR pg_catalog.>(leftarg = uint8, rightarg = bit, COMMUTATOR = operator(pg_catalog.<), procedure = pg_catalog.uint8_bit_gt);
CREATE OR REPLACE FUNCTION pg_catalog.uint8_bit_ge(uint8, bit) returns bool LANGUAGE SQL IMMUTABLE STRICT as 'select ($1::float4 >= $2::float4)';
CREATE OPERATOR pg_catalog.>=(leftarg = uint8, rightarg = bit, COMMUTATOR = operator(pg_catalog.<=), procedure = pg_catalog.uint8_bit_ge);
CREATE OR REPLACE FUNCTION pg_catalog.uint8_bit_lt(uint8, bit) returns bool LANGUAGE SQL IMMUTABLE STRICT as 'select ($1::float4 < $2::float4)';
CREATE OPERATOR pg_catalog.<(leftarg = uint8, rightarg = bit, COMMUTATOR = operator(pg_catalog.>), procedure = pg_catalog.uint8_bit_lt);
CREATE OR REPLACE FUNCTION pg_catalog.uint8_bit_le(uint8, bit) returns bool LANGUAGE SQL IMMUTABLE STRICT as 'select ($1::float4 <= $2::float4)';
CREATE OPERATOR pg_catalog.<=(leftarg = uint8, rightarg = bit, COMMUTATOR = operator(pg_catalog.>=), procedure = pg_catalog.uint8_bit_le);
CREATE OR REPLACE FUNCTION pg_catalog.bit_uint8_gt(bit, uint8) returns bool LANGUAGE SQL IMMUTABLE STRICT as 'select ($1::float4 > $2::float4)';
CREATE OPERATOR pg_catalog.>(leftarg = bit, rightarg = uint8, COMMUTATOR = operator(pg_catalog.<), procedure = pg_catalog.bit_uint8_gt);
CREATE OR REPLACE FUNCTION pg_catalog.bit_uint8_ge(bit, uint8) returns bool LANGUAGE SQL IMMUTABLE STRICT as 'select ($1::float4 >= $2::float4)';
CREATE OPERATOR pg_catalog.>=(leftarg = bit, rightarg = uint8, COMMUTATOR = operator(pg_catalog.<=), procedure = pg_catalog.bit_uint8_ge);
CREATE OR REPLACE FUNCTION pg_catalog.bit_uint8_lt(bit, uint8) returns bool LANGUAGE SQL IMMUTABLE STRICT as 'select ($1::float4 < $2::float4)';
CREATE OPERATOR pg_catalog.<(leftarg = bit, rightarg = uint8, COMMUTATOR = operator(pg_catalog.>), procedure = pg_catalog.bit_uint8_lt);
CREATE OR REPLACE FUNCTION pg_catalog.bit_uint8_le(bit, uint8) returns bool LANGUAGE SQL IMMUTABLE STRICT as 'select ($1::float4 <= $2::float4)';
CREATE OPERATOR pg_catalog.<=(leftarg = bit, rightarg = uint8, COMMUTATOR = operator(pg_catalog.>=), procedure = pg_catalog.bit_uint8_le);

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

CREATE OR REPLACE FUNCTION pg_catalog.convert_text_datetime(text) RETURNS timestamp without time zone LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'convert_text_datetime';
CREATE OR REPLACE FUNCTION pg_catalog.convert_text_timestamptz(text) RETURNS timestamptz LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'convert_text_timestamptz';

CREATE OR REPLACE FUNCTION pg_catalog.datetime_text_eq(arg1 timestamp without time zone, arg2 text) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1=convert_text_datetime($2)';
CREATE OR REPLACE FUNCTION pg_catalog.datetime_text_ne(arg1 timestamp without time zone, arg2 text) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1<>convert_text_datetime($2)';
CREATE OR REPLACE FUNCTION pg_catalog.datetime_text_lt(arg1 timestamp without time zone, arg2 text) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1<convert_text_datetime($2)';
CREATE OR REPLACE FUNCTION pg_catalog.datetime_text_le(arg1 timestamp without time zone, arg2 text) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1<=convert_text_datetime($2)';
CREATE OR REPLACE FUNCTION pg_catalog.datetime_text_gt(arg1 timestamp without time zone, arg2 text) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1>convert_text_datetime($2)';
CREATE OR REPLACE FUNCTION pg_catalog.datetime_text_ge(arg1 timestamp without time zone, arg2 text) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1>=convert_text_datetime($2)';

CREATE OR REPLACE FUNCTION pg_catalog.text_datetime_eq(arg1 text, arg2 timestamp without time zone) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select convert_text_datetime($1)=$2';
CREATE OR REPLACE FUNCTION pg_catalog.text_datetime_ne(arg1 text, arg2 timestamp without time zone) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select convert_text_datetime($1)<>$2';
CREATE OR REPLACE FUNCTION pg_catalog.text_datetime_lt(arg1 text, arg2 timestamp without time zone) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select convert_text_datetime($1)<$2';
CREATE OR REPLACE FUNCTION pg_catalog.text_datetime_le(arg1 text, arg2 timestamp without time zone) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select convert_text_datetime($1)<=$2';
CREATE OR REPLACE FUNCTION pg_catalog.text_datetime_gt(arg1 text, arg2 timestamp without time zone) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select convert_text_datetime($1)>$2';
CREATE OR REPLACE FUNCTION pg_catalog.text_datetime_ge(arg1 text, arg2 timestamp without time zone) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select convert_text_datetime($1)>=$2';

CREATE OR REPLACE FUNCTION pg_catalog.timestamp_text_eq(arg1 timestamptz, arg2 text) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1=convert_text_timestamptz($2)';
CREATE OR REPLACE FUNCTION pg_catalog.timestamp_text_ne(arg1 timestamptz, arg2 text) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1<>convert_text_timestamptz($2)';
CREATE OR REPLACE FUNCTION pg_catalog.timestamp_text_lt(arg1 timestamptz, arg2 text) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1<convert_text_timestamptz($2)';
CREATE OR REPLACE FUNCTION pg_catalog.timestamp_text_le(arg1 timestamptz, arg2 text) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1<=convert_text_timestamptz($2)';
CREATE OR REPLACE FUNCTION pg_catalog.timestamp_text_gt(arg1 timestamptz, arg2 text) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1>convert_text_timestamptz($2)';
CREATE OR REPLACE FUNCTION pg_catalog.timestamp_text_ge(arg1 timestamptz, arg2 text) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select $1>=convert_text_timestamptz($2)';

CREATE OR REPLACE FUNCTION pg_catalog.text_timestamp_eq(arg1 text, arg2 timestamptz) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select convert_text_timestamptz($1)=$2';
CREATE OR REPLACE FUNCTION pg_catalog.text_timestamp_ne(arg1 text, arg2 timestamptz) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select convert_text_timestamptz($1)<>$2';
CREATE OR REPLACE FUNCTION pg_catalog.text_timestamp_lt(arg1 text, arg2 timestamptz) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select convert_text_timestamptz($1)<$2';
CREATE OR REPLACE FUNCTION pg_catalog.text_timestamp_le(arg1 text, arg2 timestamptz) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select convert_text_timestamptz($1)<=$2';
CREATE OR REPLACE FUNCTION pg_catalog.text_timestamp_gt(arg1 text, arg2 timestamptz) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select convert_text_timestamptz($1)>$2';
CREATE OR REPLACE FUNCTION pg_catalog.text_timestamp_ge(arg1 text, arg2 timestamptz) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'select convert_text_timestamptz($1)>=$2';

CREATE FUNCTION pg_catalog.enumtext_like(anyenum, text) returns bool LANGUAGE SQL IMMUTABLE as 'select $1::text like $2';
CREATE FUNCTION pg_catalog.textenum_like(text, anyenum) returns bool LANGUAGE SQL IMMUTABLE as 'select $1 like $2::text';
CREATE FUNCTION pg_catalog.enumtext_nlike(anyenum, text) returns bool LANGUAGE SQL IMMUTABLE as 'select $1::text not like $2';
CREATE FUNCTION pg_catalog.textenum_nlike(text, anyenum) returns bool LANGUAGE SQL IMMUTABLE as 'select $1 not like $2::text';
CREATE OPERATOR pg_catalog.~~(leftarg = anyenum, rightarg = text, procedure = pg_catalog.enumtext_like);
CREATE OPERATOR pg_catalog.~~(leftarg = text, rightarg = anyenum, procedure = pg_catalog.textenum_like);
CREATE OPERATOR pg_catalog.!~~(leftarg = anyenum, rightarg = text, procedure = pg_catalog.enumtext_nlike);
CREATE OPERATOR pg_catalog.!~~(leftarg = text, rightarg = anyenum, procedure = pg_catalog.textenum_nlike);

CREATE FUNCTION pg_catalog.blob_to_float8(blob) RETURNS double precision LANGUAGE C IMMUTABLE STRICT AS '$libdir/dolphin', 'blob_to_float8';
CREATE OR REPLACE FUNCTION pg_catalog.tinyblob_float8_eq(arg1 tinyblob, arg2 float8) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT pg_catalog.blob_to_float8($1) = $2';
CREATE OR REPLACE FUNCTION pg_catalog.float8_tinyblob_eq(arg1 float8, arg2 tinyblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT $1 = pg_catalog.blob_to_float8($2)';
CREATE OR REPLACE FUNCTION pg_catalog.mediumblob_float8_eq(arg1 mediumblob, arg2 float8) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT pg_catalog.blob_to_float8($1) = $2';
CREATE OR REPLACE FUNCTION pg_catalog.float8_mediumblob_eq(arg1 float8, arg2 mediumblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT $1 = pg_catalog.blob_to_float8($2)';
CREATE OR REPLACE FUNCTION pg_catalog.blob_float8_eq(arg1 blob, arg2 float8) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT pg_catalog.blob_to_float8($1) = $2';
CREATE OR REPLACE FUNCTION pg_catalog.float8_blob_eq(arg1 float8, arg2 blob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT $1 = pg_catalog.blob_to_float8($2)';
CREATE OR REPLACE FUNCTION pg_catalog.longblob_float8_eq(arg1 longblob, arg2 float8) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT pg_catalog.blob_to_float8($1) = $2';
CREATE OR REPLACE FUNCTION pg_catalog.float8_longblob_eq(arg1 float8, arg2 longblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT $1 = pg_catalog.blob_to_float8($2)';

CREATE OR REPLACE FUNCTION pg_catalog.tinyblob_numeric_eq(arg1 tinyblob, arg2 numeric) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT pg_catalog.blob_to_float8($1) = $2';
CREATE OR REPLACE FUNCTION pg_catalog.numeric_tinyblob_eq(arg1 numeric, arg2 tinyblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT $1 = pg_catalog.blob_to_float8($2)';
CREATE OR REPLACE FUNCTION pg_catalog.mediumblob_numeric_eq(arg1 mediumblob, arg2 numeric) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT pg_catalog.blob_to_float8($1) = $2';
CREATE OR REPLACE FUNCTION pg_catalog.numeric_mediumblob_eq(arg1 numeric, arg2 mediumblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT $1 = pg_catalog.blob_to_float8($2)';
CREATE OR REPLACE FUNCTION pg_catalog.blob_numeric_eq(arg1 blob, arg2 numeric) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT pg_catalog.blob_to_float8($1) = $2';
CREATE OR REPLACE FUNCTION pg_catalog.numeric_blob_eq(arg1 numeric, arg2 blob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT $1 = pg_catalog.blob_to_float8($2)';
CREATE OR REPLACE FUNCTION pg_catalog.longblob_numeric_eq(arg1 longblob, arg2 numeric) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT pg_catalog.blob_to_float8($1) = $2';
CREATE OR REPLACE FUNCTION pg_catalog.numeric_longblob_eq(arg1 numeric, arg2 longblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT $1 = pg_catalog.blob_to_float8($2)';

CREATE OR REPLACE FUNCTION pg_catalog.tinyblob_uint8_eq(arg1 tinyblob, arg2 uint8) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT pg_catalog.blob_to_float8($1) = $2';
CREATE OR REPLACE FUNCTION pg_catalog.uint8_tinyblob_eq(arg1 uint8, arg2 tinyblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT $1 = pg_catalog.blob_to_float8($2)';
CREATE OR REPLACE FUNCTION pg_catalog.mediumblob_uint8_eq(arg1 mediumblob, arg2 uint8) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT pg_catalog.blob_to_float8($1) = $2';
CREATE OR REPLACE FUNCTION pg_catalog.uint8_mediumblob_eq(arg1 uint8, arg2 mediumblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT $1 = pg_catalog.blob_to_float8($2)';
CREATE OR REPLACE FUNCTION pg_catalog.blob_uint8_eq(arg1 blob, arg2 uint8) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT pg_catalog.blob_to_float8($1) = $2';
CREATE OR REPLACE FUNCTION pg_catalog.uint8_blob_eq(arg1 uint8, arg2 blob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT $1 = pg_catalog.blob_to_float8($2)';
CREATE OR REPLACE FUNCTION pg_catalog.longblob_uint8_eq(arg1 longblob, arg2 uint8) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT pg_catalog.blob_to_float8($1) = $2';
CREATE OR REPLACE FUNCTION pg_catalog.uint8_longblob_eq(arg1 uint8, arg2 longblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT $1 = pg_catalog.blob_to_float8($2)';

CREATE OR REPLACE FUNCTION pg_catalog.tinyblob_int8_eq(arg1 tinyblob, arg2 int8) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT pg_catalog.blob_to_float8($1) = $2';
CREATE OR REPLACE FUNCTION pg_catalog.int8_tinyblob_eq(arg1 int8, arg2 tinyblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT $1 = pg_catalog.blob_to_float8($2)';
CREATE OR REPLACE FUNCTION pg_catalog.mediumblob_int8_eq(arg1 mediumblob, arg2 int8) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT pg_catalog.blob_to_float8($1) = $2';
CREATE OR REPLACE FUNCTION pg_catalog.int8_mediumblob_eq(arg1 int8, arg2 mediumblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT $1 = pg_catalog.blob_to_float8($2)';
CREATE OR REPLACE FUNCTION pg_catalog.blob_int8_eq(arg1 blob, arg2 int8) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT pg_catalog.blob_to_float8($1) = $2';
CREATE OR REPLACE FUNCTION pg_catalog.int8_blob_eq(arg1 int8, arg2 blob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT $1 = pg_catalog.blob_to_float8($2)';
CREATE OR REPLACE FUNCTION pg_catalog.longblob_int8_eq(arg1 longblob, arg2 int8) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT pg_catalog.blob_to_float8($1) = $2';
CREATE OR REPLACE FUNCTION pg_catalog.int8_longblob_eq(arg1 int8, arg2 longblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT $1 = pg_catalog.blob_to_float8($2)';

CREATE FUNCTION pg_catalog.text_to_tinyblob(text) RETURNS tinyblob LANGUAGE C IMMUTABLE STRICT AS '$libdir/dolphin', 'text_to_blob';
CREATE FUNCTION pg_catalog.text_to_blob(text) RETURNS blob LANGUAGE C IMMUTABLE STRICT AS '$libdir/dolphin', 'text_to_blob';
CREATE FUNCTION pg_catalog.text_to_mediumblob(text) RETURNS mediumblob LANGUAGE C IMMUTABLE STRICT AS '$libdir/dolphin', 'text_to_blob';
CREATE FUNCTION pg_catalog.text_to_longblob(text) RETURNS longblob LANGUAGE C IMMUTABLE STRICT AS '$libdir/dolphin', 'text_to_blob';
CREATE CAST (text AS tinyblob) WITH FUNCTION pg_catalog.text_to_tinyblob(text);
CREATE CAST (text AS blob) WITH FUNCTION pg_catalog.text_to_blob(text);
CREATE CAST (text AS mediumblob) WITH FUNCTION pg_catalog.text_to_mediumblob(text);
CREATE CAST (text AS longblob) WITH FUNCTION pg_catalog.text_to_longblob(text);

DO $for_upgrade_only$
DECLARE
  ans boolean;
  typ_check boolean;
  v_isinplaceupgrade boolean;
BEGIN
    select case when count(*)=1 then true else false end as ans from (select setting from pg_settings where name = 'upgrade_mode' and setting != '0') into ans;
    select case when a.oid<>b.typarray then true else false end as typ_check from pg_type a, pg_type b where (a.typname='_uint1' and b.typname='uint1') into typ_check;
    show isinplaceupgrade into v_isinplaceupgrade;
    -- we can do drop type only during upgrade
    if ans = true and v_isinplaceupgrade = true and typ_check = true then
        DROP TYPE IF EXISTS pg_catalog._uint1 cascade;
        DROP TYPE IF EXISTS pg_catalog._uint2 cascade;
        DROP TYPE IF EXISTS pg_catalog._uint4 cascade;
        DROP TYPE IF EXISTS pg_catalog._uint8 cascade;
        update pg_catalog.pg_type set typname = '_uint1' where typname = '_uint1_1';
        update pg_catalog.pg_type set typname = '_uint2' where typname = '_uint2_1';
        update pg_catalog.pg_type set typname = '_uint4' where typname = '_uint4_1';
        update pg_catalog.pg_type set typname = '_uint8' where typname = '_uint8_1';
    end if;
END
$for_upgrade_only$;

CREATE OPERATOR CLASS pg_catalog.uint1_ops
    DEFAULT FOR TYPE uint1 USING ubtree family pg_catalog.integer_ops AS
        OPERATOR        1       pg_catalog.< ,
        OPERATOR        1       pg_catalog.<(uint1, uint2),
        OPERATOR        1       pg_catalog.<(uint1, uint4),
        OPERATOR        1       pg_catalog.<(uint1, uint8),
        OPERATOR        1       pg_catalog.<(uint1, int1),
        OPERATOR        1       pg_catalog.<(uint1, int2),
        OPERATOR        1       pg_catalog.<(uint1, int4),
        OPERATOR        1       pg_catalog.<(uint1, int8),
        OPERATOR        2       pg_catalog.<= ,
        OPERATOR        2       pg_catalog.<=(uint1, uint2),
        OPERATOR        2       pg_catalog.<=(uint1, uint4),
        OPERATOR        2       pg_catalog.<=(uint1, uint8),
        OPERATOR        2       pg_catalog.<=(uint1, int1),
        OPERATOR        2       pg_catalog.<=(uint1, int2),
        OPERATOR        2       pg_catalog.<=(uint1, int4),
        OPERATOR        2       pg_catalog.<=(uint1, int8),
        OPERATOR        3       pg_catalog.= ,
        OPERATOR        3       pg_catalog.=(uint1, uint2),
        OPERATOR        3       pg_catalog.=(uint1, uint4),
        OPERATOR        3       pg_catalog.=(uint1, uint8),
        OPERATOR        3       pg_catalog.=(uint1, int1),
        OPERATOR        3       pg_catalog.=(uint1, int2),
        OPERATOR        3       pg_catalog.=(uint1, int4),
        OPERATOR        3       pg_catalog.=(uint1, int8),
        OPERATOR        4       pg_catalog.>= ,
        OPERATOR        4       pg_catalog.>=(uint1, uint2),
        OPERATOR        4       pg_catalog.>=(uint1, uint4),
        OPERATOR        4       pg_catalog.>=(uint1, uint8),
        OPERATOR        4       pg_catalog.>=(uint1, int1),
        OPERATOR        4       pg_catalog.>=(uint1, int2),
        OPERATOR        4       pg_catalog.>=(uint1, int4),
        OPERATOR        4       pg_catalog.>=(uint1, int8),
        OPERATOR        5       pg_catalog.> ,
        OPERATOR        5       pg_catalog.>(uint1, uint2),
        OPERATOR        5       pg_catalog.>(uint1, uint4),
        OPERATOR        5       pg_catalog.>(uint1, uint8),
        OPERATOR        5       pg_catalog.>(uint1, int1),
        OPERATOR        5       pg_catalog.>(uint1, int2),
        OPERATOR        5       pg_catalog.>(uint1, int4),
        OPERATOR        5       pg_catalog.>(uint1, int8),
        FUNCTION        1       pg_catalog.uint1cmp(uint1, uint1),
        FUNCTION        1       pg_catalog.uint12cmp(uint1, uint2),
        FUNCTION        1       pg_catalog.uint14cmp(uint1, uint4),
        FUNCTION        1       pg_catalog.uint18cmp(uint1, uint8),
        FUNCTION        1       pg_catalog.uint1_int1cmp(uint1, int1),
        FUNCTION        1       pg_catalog.uint1_int2cmp(uint1, int2),
        FUNCTION        1       pg_catalog.uint1_int4cmp(uint1, int4),
        FUNCTION        1       pg_catalog.uint1_int8cmp(uint1, int8),
        FUNCTION        2       pg_catalog.uint1_sortsupport(internal);

CREATE OPERATOR CLASS pg_catalog.uint2_ops
    DEFAULT FOR TYPE uint2 USING ubtree family pg_catalog.integer_ops AS
        OPERATOR        1       pg_catalog.< ,
        OPERATOR        1       pg_catalog.<(uint2, uint4),
        OPERATOR        1       pg_catalog.<(uint2, uint8),
        OPERATOR        1       pg_catalog.<(uint2, int2),
        OPERATOR        1       pg_catalog.<(uint2, int4),
        OPERATOR        1       pg_catalog.<(uint2, int8),
        OPERATOR        2       pg_catalog.<= ,
        OPERATOR        2       pg_catalog.<=(uint2, uint4),
        OPERATOR        2       pg_catalog.<=(uint2, uint8),
        OPERATOR        2       pg_catalog.<=(uint2, int2),
        OPERATOR        2       pg_catalog.<=(uint2, int4),
        OPERATOR        2       pg_catalog.<=(uint2, int8),
        OPERATOR        3       pg_catalog.= ,
        OPERATOR        3       pg_catalog.=(uint2, uint4),
        OPERATOR        3       pg_catalog.=(uint2, uint8),
        OPERATOR        3       pg_catalog.=(uint2, int2),
        OPERATOR        3       pg_catalog.=(uint2, int4),
        OPERATOR        3       pg_catalog.=(uint2, int8),
        OPERATOR        4       pg_catalog.>= ,
        OPERATOR        4       pg_catalog.>=(uint2, uint4),
        OPERATOR        4       pg_catalog.>=(uint2, uint8),
        OPERATOR        4       pg_catalog.>=(uint2, int2),
        OPERATOR        4       pg_catalog.>=(uint2, int4),
        OPERATOR        4       pg_catalog.>=(uint2, int8),
        OPERATOR        5       pg_catalog.> ,
        OPERATOR        5       pg_catalog.>(uint2, uint4),
        OPERATOR        5       pg_catalog.>(uint2, uint8),
        OPERATOR        5       pg_catalog.>(uint2, int2),
        OPERATOR        5       pg_catalog.>(uint2, int4),
        OPERATOR        5       pg_catalog.>(uint2, int8),
        FUNCTION        1       pg_catalog.uint2cmp(uint2, uint2),
        FUNCTION        1       pg_catalog.uint24cmp(uint2, uint4),
        FUNCTION        1       pg_catalog.uint28cmp(uint2, uint8),
        FUNCTION        1       pg_catalog.uint2_int2cmp(uint2, int2),
        FUNCTION        1       pg_catalog.uint2_int4cmp(uint2, int4),
        FUNCTION        1       pg_catalog.uint2_int8cmp(uint2, int8),
        FUNCTION        2       pg_catalog.uint2_sortsupport(internal);

CREATE OPERATOR CLASS pg_catalog.uint4_ops
    DEFAULT FOR TYPE uint4 USING ubtree family pg_catalog.integer_ops AS
        OPERATOR        1       pg_catalog.< ,
        OPERATOR        1       pg_catalog.<(uint4, uint8),
        OPERATOR        1       pg_catalog.<(uint4, int4),
        OPERATOR        1       pg_catalog.<(uint4, int8),
        OPERATOR        2       pg_catalog.<= ,
        OPERATOR        2       pg_catalog.<=(uint4, uint8),
        OPERATOR        2       pg_catalog.<=(uint4, int4),
        OPERATOR        2       pg_catalog.<=(uint4, int8),
        OPERATOR        3       pg_catalog.= ,
        OPERATOR        3       pg_catalog.=(uint4, uint8),
        OPERATOR        3       pg_catalog.=(uint4, int4),
        OPERATOR        3       pg_catalog.=(uint4, int8),
        OPERATOR        4       pg_catalog.>= ,
        OPERATOR        4       pg_catalog.>=(uint4, uint8),
        OPERATOR        4       pg_catalog.>=(uint4, int4),
        OPERATOR        4       pg_catalog.>=(uint4, int8),
        OPERATOR        5       pg_catalog.> ,
        OPERATOR        5       pg_catalog.>(uint4, uint8),
        OPERATOR        5       pg_catalog.>(uint4, int4),
        OPERATOR        5       pg_catalog.>(uint4, int8),
        FUNCTION        1       pg_catalog.uint4cmp(uint4, uint4),
        FUNCTION        1       pg_catalog.uint48cmp(uint4, uint8),
        FUNCTION        1       pg_catalog.uint4_int4cmp(uint4, int4),
        FUNCTION        1       pg_catalog.uint4_int8cmp(uint4, int8),
        FUNCTION        2       pg_catalog.uint4_sortsupport(internal);

CREATE OPERATOR CLASS pg_catalog.uint8_ops
    DEFAULT FOR TYPE uint8 USING ubtree family pg_catalog.integer_ops AS
        OPERATOR        1       pg_catalog.< ,
        OPERATOR        1       pg_catalog.<(uint8, int8),
        OPERATOR        2       pg_catalog.<= ,
        OPERATOR        2       pg_catalog.<=(uint8, int8),
        OPERATOR        3       pg_catalog.= ,
        OPERATOR        3       pg_catalog.=(uint8, int8),
        OPERATOR        4       pg_catalog.>= ,
        OPERATOR        4       pg_catalog.>=(uint8, int8),
        OPERATOR        5       pg_catalog.> ,
        OPERATOR        5       pg_catalog.>(uint8, int8),
        FUNCTION        1       pg_catalog.uint8cmp(uint8, uint8),
        FUNCTION        1       pg_catalog.uint8_int8cmp(uint8, int8),
        FUNCTION        2       pg_catalog.uint8_sortsupport(internal);

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
CREATE OR REPLACE FUNCTION pg_catalog.date_add(timestamp without time zone, interval) RETURNS timestamp without time zone LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'date_add_datetime_interval';
CREATE OR REPLACE FUNCTION pg_catalog.date_add(timestamptz, interval) RETURNS timestamptz LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'date_add_timestamp_interval';
CREATE OR REPLACE FUNCTION pg_catalog.date_sub(timestamp without time zone, interval) RETURNS timestamp without time zone AS $$ SELECT pg_catalog.date_add($1, -$2) $$ LANGUAGE SQL;
CREATE OR REPLACE FUNCTION pg_catalog.date_sub(timestamptz, interval) RETURNS timestamptz AS $$ SELECT pg_catalog.date_add($1, -$2) $$ LANGUAGE SQL;

CREATE OR REPLACE FUNCTION pg_catalog.op_num_add_intr (numeric, interval) RETURNS text AS $$ SELECT pg_catalog.date_add($1, $2) $$ LANGUAGE SQL;
CREATE OR REPLACE FUNCTION pg_catalog.op_text_add_intr (text, interval) RETURNS text AS $$ SELECT pg_catalog.date_add($1, $2) $$ LANGUAGE SQL;
CREATE OR REPLACE FUNCTION pg_catalog.op_date_add_intr (date, interval) RETURNS text AS $$ SELECT pg_catalog.date_add($1, $2) $$ LANGUAGE SQL;
CREATE OR REPLACE FUNCTION pg_catalog.op_dttm_add_intr (timestamp without time zone, interval) RETURNS timestamp without time zone AS $$ SELECT pg_catalog.date_add($1, $2) $$ LANGUAGE SQL;
CREATE OR REPLACE FUNCTION pg_catalog.op_tmsp_add_intr (timestamptz, interval) RETURNS timestamptz AS $$ SELECT pg_catalog.date_add($1, $2) $$ LANGUAGE SQL;
CREATE OR REPLACE FUNCTION pg_catalog.op_intr_add_num (interval, numeric) RETURNS text AS $$ SELECT pg_catalog.date_add($2, $1) $$ LANGUAGE SQL;
CREATE OR REPLACE FUNCTION pg_catalog.op_intr_add_text (interval, text) RETURNS text AS $$ SELECT pg_catalog.date_add($2, $1) $$ LANGUAGE SQL;
CREATE OR REPLACE FUNCTION pg_catalog.op_intr_add_date (interval, date) RETURNS text AS $$ SELECT pg_catalog.date_add($2, $1) $$ LANGUAGE SQL;
CREATE OR REPLACE FUNCTION pg_catalog.op_intr_add_dttm (interval, timestamp without time zone) RETURNS timestamp without time zone AS $$ SELECT pg_catalog.date_add($2, $1) $$ LANGUAGE SQL;
CREATE OR REPLACE FUNCTION pg_catalog.op_intr_add_tmsp (interval, timestamptz) RETURNS timestamptz AS $$ SELECT pg_catalog.date_add($2, $1) $$ LANGUAGE SQL;
CREATE OR REPLACE FUNCTION pg_catalog.op_num_sub_intr (numeric, interval) RETURNS text AS $$ SELECT pg_catalog.date_sub($1, $2) $$ LANGUAGE SQL;
CREATE OR REPLACE FUNCTION pg_catalog.op_text_sub_intr (text, interval) RETURNS text AS $$ SELECT pg_catalog.date_sub($1, $2) $$ LANGUAGE SQL;
CREATE OR REPLACE FUNCTION pg_catalog.op_date_sub_intr (date, interval) RETURNS text AS $$ SELECT pg_catalog.date_sub($1, $2) $$ LANGUAGE SQL;
CREATE OR REPLACE FUNCTION pg_catalog.op_dttm_sub_intr (timestamp without time zone, interval) RETURNS timestamp without time zone AS $$ SELECT pg_catalog.date_sub($1, $2) $$ LANGUAGE SQL;
CREATE OR REPLACE FUNCTION pg_catalog.op_tmsp_sub_intr (timestamptz, interval) RETURNS timestamptz AS $$ SELECT pg_catalog.date_sub($1, $2) $$ LANGUAGE SQL;

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
        update pg_catalog.pg_operator set oprresult = 'timestamp without time zone'::regtype, oprcode = 'op_dttm_add_intr'::regproc
            where oprname = '+' and oprleft = 'timestamp without time zone'::regtype and oprright = 'interval'::regtype;
        update pg_catalog.pg_operator set oprresult = 'timestamptz'::regtype, oprcode = 'op_tmsp_add_intr'::regproc
            where oprname = '+' and oprleft = 'timestamptz'::regtype and oprright = 'interval'::regtype;
        update pg_catalog.pg_operator set oprresult = 'text'::regtype, oprcode = 'op_time_add_intr'::regproc
            where oprname = '+' and oprleft = 'time'::regtype and oprright = 'interval'::regtype;
    
        update pg_catalog.pg_operator set oprresult = 'text'::regtype, oprcode = 'op_intr_add_date'::regproc
            where oprname = '+' and oprleft = 'interval'::regtype and oprright = 'date'::regtype;
        update pg_catalog.pg_operator set oprresult = 'timestamp without time zone'::regtype, oprcode = 'op_intr_add_dttm'::regproc
            where oprname = '+' and oprleft = 'interval'::regtype and oprright = 'timestamp without time zone'::regtype;
        update pg_catalog.pg_operator set oprresult = 'timestamptz'::regtype, oprcode = 'op_intr_add_tmsp'::regproc
            where oprname = '+' and oprleft = 'interval'::regtype and oprright = 'timestamptz'::regtype;
        update pg_catalog.pg_operator set oprresult = 'text'::regtype, oprcode = 'op_intr_add_time'::regproc
            where oprname = '+' and oprleft = 'interval'::regtype and oprright = 'time'::regtype;
        
        update pg_catalog.pg_operator set oprresult = 'text'::regtype, oprcode = 'op_date_sub_intr'::regproc
            where oprname = '-' and oprleft = 'date'::regtype and oprright = 'interval'::regtype;
        update pg_catalog.pg_operator set oprresult = 'timestamp without time zone'::regtype, oprcode = 'op_dttm_sub_intr'::regproc
            where oprname = '-' and oprleft = 'timestamp without time zone'::regtype and oprright = 'interval'::regtype;
        update pg_catalog.pg_operator set oprresult = 'timestamptz'::regtype, oprcode = 'op_tmsp_sub_intr'::regproc
            where oprname = '-' and oprleft = 'timestamptz'::regtype and oprright = 'interval'::regtype;
        update pg_catalog.pg_operator set oprresult = 'text'::regtype, oprcode = 'op_time_sub_intr'::regproc
            where oprname = '-' and oprleft = 'time'::regtype and oprright = 'interval'::regtype;
END
$$;

create or replace function pg_catalog.any2interval(anyelement, integer) returns interval LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin','any2interval';
create or replace function pg_catalog.any2interval(anyelement, integer, integer) returns interval LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin','any2interval';

CREATE OR REPLACE FUNCTION pg_catalog.op_int1xor(int1, int1) returns uint8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.int1xor($1, $2)::uint8';
CREATE OR REPLACE FUNCTION pg_catalog.op_int2xor(int2, int2) returns uint8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.int2xor($1, $2)::uint8';
CREATE OR REPLACE FUNCTION pg_catalog.op_int4xor(int4, int4) returns uint8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.int4xor($1, $2)::uint8';
CREATE OR REPLACE FUNCTION pg_catalog.op_int8xor(int8, int8) returns uint8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.int8xor($1, $2)::uint8';
CREATE OR REPLACE FUNCTION pg_catalog.op_uint1xor(uint1, uint1) returns uint8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.uint1xor($1, $2)::uint8';
CREATE OR REPLACE FUNCTION pg_catalog.op_uint2xor(uint2, uint2) returns uint8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.uint2xor($1, $2)::uint8';
CREATE OR REPLACE FUNCTION pg_catalog.op_uint4xor(uint4, uint4) returns uint8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.uint4xor($1, $2)::uint8';
CREATE OR REPLACE FUNCTION pg_catalog.op_uint8xor(uint8, uint8) returns uint8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.uint8xor($1, $2)::uint8';
CREATE OR REPLACE FUNCTION pg_catalog.op_uint1_xor_int1(uint1, int1) returns uint8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.uint1_xor_int1($1, $2)::uint8';
CREATE OR REPLACE FUNCTION pg_catalog.op_int1_xor_uint1(int1, uint1) returns uint8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.int1_xor_uint1($1, $2)::uint8';
CREATE OR REPLACE FUNCTION pg_catalog.op_uint2_xor_int2(uint2, int2) returns uint8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.uint2_xor_int2($1, $2)::uint8';
CREATE OR REPLACE FUNCTION pg_catalog.op_int2_xor_uint2(int2, uint2) returns uint8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.int2_xor_uint2($1, $2)::uint8';
CREATE OR REPLACE FUNCTION pg_catalog.op_uint4_xor_int4(uint4, int4) returns uint8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.uint4_xor_int4($1, $2)::uint8';
CREATE OR REPLACE FUNCTION pg_catalog.op_int4_xor_uint4(int4, uint4) returns uint8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.int4_xor_uint4($1, $2)::uint8';
CREATE OR REPLACE FUNCTION pg_catalog.op_uint8_xor_int8(uint8, int8) returns uint8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.uint8_xor_int8($1, $2)::uint8';
CREATE OR REPLACE FUNCTION pg_catalog.op_int8_xor_uint8(int8, uint8) returns uint8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.int8_xor_uint8($1, $2)::uint8';
CREATE OR REPLACE FUNCTION pg_catalog.blob_xor_blob(blob, blob) returns blob LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'blob_xor_blob';
CREATE OR REPLACE FUNCTION pg_catalog.mblob_xor_mblob(mediumblob, mediumblob) returns mediumblob LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'blob_xor_blob';
CREATE OR REPLACE FUNCTION pg_catalog.lblob_xor_lblob(longblob, longblob) returns longblob LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'blob_xor_blob';
CREATE OR REPLACE FUNCTION pg_catalog.binary_xor_binary(binary, binary) returns varbinary LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'blob_xor_blob';
CREATE OR REPLACE FUNCTION pg_catalog.varbinary_xor_varbinary(varbinary, varbinary) returns varbinary LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'blob_xor_blob';
CREATE OR REPLACE FUNCTION pg_catalog.tinyblob_xor_tinyblob(tinyblob, tinyblob) returns varbinary LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'blob_xor_blob';
CREATE OR REPLACE FUNCTION pg_catalog.op_blob_int_xor(blob, integer) returns uint8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.blobxor($1, $2)::uint8';
CREATE OR REPLACE FUNCTION pg_catalog.op_int_blob_xor(integer, blob) returns uint8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.blobxor($1, $2)::uint8';
CREATE OR REPLACE FUNCTION pg_catalog.op_int8_blob_xor(int8, blob) returns uint8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.blobxor($1, $2)::uint8';
CREATE OR REPLACE FUNCTION pg_catalog.op_blob_int8_xor(blob, int8) returns uint8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.blobxor($1, $2)::uint8';
CREATE OR REPLACE FUNCTION pg_catalog.op_float8_blob_xor(float8, blob) returns uint8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.blobxor($1, $2)::uint8';
CREATE OR REPLACE FUNCTION pg_catalog.op_blob_float8_xor(blob, float8) returns uint8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.blobxor($1, $2)::uint8';
CREATE OR REPLACE FUNCTION pg_catalog.op_boolxor(boolean, boolean) returns uint8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.int1xor($1::int1, $2::int1)::uint8';
CREATE OR REPLACE FUNCTION pg_catalog.op_bool_float8_xor(boolean, float8) returns uint8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.dpow($1::float8, $2)::uint8';
CREATE OR REPLACE FUNCTION pg_catalog.op_float8_bool_xor(float8, boolean) returns uint8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.dpow($1, $2::float8)::uint8';
CREATE OR REPLACE FUNCTION pg_catalog.op_datexor(date, date) returns uint8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.datexor($1, $2)::uint8';
CREATE OR REPLACE FUNCTION pg_catalog.op_timexor(time, time) returns uint8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.timexor($1, $2)::uint8';
CREATE OR REPLACE FUNCTION pg_catalog.op_date_time_xor(date, time) returns uint8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.date_time_xor($1, $2)::uint8';
CREATE OR REPLACE FUNCTION pg_catalog.op_time_date_xor(time, date) returns uint8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.time_date_xor($1, $2)::uint8';
CREATE OR REPLACE FUNCTION pg_catalog.op_time_text_xor(time, text) returns uint8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.time_text_xor($1, $2)::uint8';
CREATE OR REPLACE FUNCTION pg_catalog.op_text_time_xor(text, time) returns uint8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.text_time_xor($1, $2)::uint8';
CREATE OR REPLACE FUNCTION pg_catalog.op_date_text_xor(date, text) returns uint8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.date_text_xor($1, $2)::uint8';
CREATE OR REPLACE FUNCTION pg_catalog.op_text_date_xor(text, date) returns uint8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.text_date_xor($1, $2)::uint8';
CREATE OR REPLACE FUNCTION pg_catalog.op_date_int8_xor(date, int8) returns uint8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.date_int8_xor($1, $2)::uint8';
CREATE OR REPLACE FUNCTION pg_catalog.op_int8_date_xor(int8, date) returns uint8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.int8_date_xor($1, $2)::uint8';
CREATE OR REPLACE FUNCTION pg_catalog.op_time_int8_xor(time, int8) returns uint8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.time_int8_xor($1, $2)::uint8';
CREATE OR REPLACE FUNCTION pg_catalog.op_int8_time_xor(int8, time) returns uint8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.int8_time_xor($1, $2)::uint8';
CREATE OR REPLACE FUNCTION pg_catalog.op_date_float8_xor(date, float8) returns uint8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.date_float8_xor($1, $2)::uint8';
CREATE OR REPLACE FUNCTION pg_catalog.op_float8_date_xor(float8, date) returns uint8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.float8_date_xor($1, $2)::uint8';
CREATE OR REPLACE FUNCTION pg_catalog.op_timestampxor(timestamp without time zone, timestamp without time zone) returns uint8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.timestampxor($1, $2)::uint8';
CREATE OR REPLACE FUNCTION pg_catalog.op_timestamp_int8_xor(timestamp without time zone, int8) returns uint8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.timestamp_int8_xor($1, $2)::uint8';
CREATE OR REPLACE FUNCTION pg_catalog.op_int8_timestamp_xor(int8, timestamp without time zone) returns uint8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.int8_timestamp_xor($1, $2)::uint8';
CREATE OR REPLACE FUNCTION pg_catalog.op_timestamp_float8_xor(timestamp without time zone, float8) returns uint8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.timestamp_float8_xor($1, $2)::uint8';
CREATE OR REPLACE FUNCTION pg_catalog.op_float8_timestamp_xor(float8, timestamp without time zone) returns uint8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.float8_timestamp_xor($1, $2)::uint8';
CREATE OR REPLACE FUNCTION pg_catalog.op_timestamp_text_xor(timestamp without time zone, text) returns uint8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.timestamp_text_xor($1, $2)::uint8';
CREATE OR REPLACE FUNCTION pg_catalog.op_text_timestamp_xor(text, timestamp without time zone) returns uint8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.text_timestamp_xor($1, $2)::uint8';
CREATE OR REPLACE FUNCTION pg_catalog.op_timestamptzxor(timestampTz, timestampTz) returns uint8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.timestamptzxor($1, $2)::uint8';
CREATE OR REPLACE FUNCTION pg_catalog.op_timestamptz_int8_xor(timestampTz, int8) returns uint8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.timestamptz_int8_xor($1, $2)::uint8';
CREATE OR REPLACE FUNCTION pg_catalog.op_int8_timestamptz_xor(int8, timestampTz) returns uint8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.int8_timestamptz_xor($1, $2)::uint8';
CREATE OR REPLACE FUNCTION pg_catalog.op_timestamptz_float8_xor(timestampTz, float8) returns uint8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.timestamptz_float8_xor($1, $2)::uint8';
CREATE OR REPLACE FUNCTION pg_catalog.op_float8_timestamptz_xor(float8, timestampTz) returns uint8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.float8_timestamptz_xor($1, $2)::uint8';
CREATE OR REPLACE FUNCTION pg_catalog.op_timestamptz_text_xor(timestampTz, text) returns uint8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.timestamptz_text_xor($1, $2)::uint8';
CREATE OR REPLACE FUNCTION pg_catalog.op_text_timestamptz_xor(text, timestampTz) returns uint8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.text_timestamptz_xor($1, $2)::uint8';
CREATE OR REPLACE FUNCTION pg_catalog.op_uint8_xor_bool(uint8, boolean) returns uint8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.uint8xor($1, $2::uint8)::uint8';
CREATE OR REPLACE FUNCTION pg_catalog.op_uint4_xor_bool(uint4, boolean) returns uint8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.uint4xor($1, $2::uint8)::uint8';
CREATE OR REPLACE FUNCTION pg_catalog.op_uint2_xor_bool(uint2, boolean) returns uint8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.uint2xor($1, $2::uint8)::uint8';
CREATE OR REPLACE FUNCTION pg_catalog.op_uint1_xor_bool(uint1, boolean) returns uint8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.uint1xor($1, $2::uint8)::uint8';
CREATE OR REPLACE FUNCTION pg_catalog.op_bool_xor_uint1(boolean, uint1) returns uint8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.uint1xor($1::uint8, $2)::uint8';
CREATE OR REPLACE FUNCTION pg_catalog.op_bool_xor_uint2(boolean, uint2) returns uint8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.uint2xor($1::uint8, $2)::uint8';
CREATE OR REPLACE FUNCTION pg_catalog.op_bool_xor_uint4(boolean, uint4) returns uint8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.uint4xor($1::uint8, $2)::uint8';
CREATE OR REPLACE FUNCTION pg_catalog.op_bool_xor_uint8(boolean, uint8) returns uint8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.uint8xor($1::uint8, $2)::uint8';
CREATE OR REPLACE FUNCTION pg_catalog.op_bitxor(bit, bit) returns uint8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.bitxor($1, $2)::uint8';
CREATE OR REPLACE FUNCTION pg_catalog.op_dpow(float8, float8) returns uint8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.dpow($1, $2)::uint8';
CREATE OR REPLACE FUNCTION pg_catalog.numeric_xor(numeric, numeric) returns int8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'numeric_xor';
CREATE OR REPLACE FUNCTION pg_catalog.op_numeric_power(numeric, numeric) returns uint8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.numeric_xor($1, $2)::uint8';
CREATE OR REPLACE FUNCTION pg_catalog.op_textxor(text, text) returns uint8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.textxor($1, $2)::uint8';
CREATE OR REPLACE FUNCTION pg_catalog.op_bool_xor_int1(boolean, int1) returns uint8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.int1xor($1::int1, $2)::uint8';
CREATE OR REPLACE FUNCTION pg_catalog.op_bool_xor_int2(boolean, int2) returns uint8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.int2xor($1::int2, $2)::uint8';
CREATE OR REPLACE FUNCTION pg_catalog.op_bool_xor_int4(boolean, int4) returns uint8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.int4xor($1::int4, $2)::uint8';
CREATE OR REPLACE FUNCTION pg_catalog.op_bool_xor_int8(boolean, int8) returns uint8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.int8xor($1::int8, $2)::uint8';
CREATE OR REPLACE FUNCTION pg_catalog.op_int1_xor_bool(int1, boolean) returns uint8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.int1xor($1, $2::int1)::uint8';
CREATE OR REPLACE FUNCTION pg_catalog.op_int2_xor_bool(int2, boolean) returns uint8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.int2xor($1, $2::int2)::uint8';
CREATE OR REPLACE FUNCTION pg_catalog.op_int4_xor_bool(int4, boolean) returns uint8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.int4xor($1, $2::int4)::uint8';
CREATE OR REPLACE FUNCTION pg_catalog.op_int8_xor_bool(int8, boolean) returns uint8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.int8xor($1, $2::int8)::uint8';
CREATE OR REPLACE FUNCTION pg_catalog.op_num_xor_bit(numeric, bit) returns uint8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.numeric_xor($1, $2::numeric)::uint8';
CREATE OR REPLACE FUNCTION pg_catalog.op_bit_xor_num(bit, numeric) returns uint8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.numeric_xor($1::numeric, $2)::uint8';
CREATE OR REPLACE FUNCTION pg_catalog.op_uint8_xor_bit(uint8, bit) returns uint8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.numeric_xor($1::numeric, $2::numeric)::uint8';
CREATE OR REPLACE FUNCTION pg_catalog.op_bit_xor_uint8(bit, uint8) returns uint8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.numeric_xor($1::numeric, $2::numeric)::uint8';
CREATE OR REPLACE FUNCTION pg_catalog.op_date_bit_xor(date, bit) returns uint8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.date_int8_xor($1, $2::int8)::uint8';
CREATE OR REPLACE FUNCTION pg_catalog.op_bit_date_xor(bit, date) returns uint8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.int8_date_xor($1::int8, $2)::uint8';
CREATE OR REPLACE FUNCTION pg_catalog.op_timestamp_bit_xor(timestamp without time zone, bit) returns uint8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.timestamp_int8_xor($1, $2::int8)::uint8';
CREATE OR REPLACE FUNCTION pg_catalog.op_bit_timestamp_xor(bit, timestamp without time zone) returns uint8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.int8_timestamp_xor($1::int8, $2)::uint8';
CREATE OR REPLACE FUNCTION pg_catalog.op_timestamptz_bit_xor(timestampTz, bit) returns uint8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.timestamptz_int8_xor($1, $2::int8)::uint8';
CREATE OR REPLACE FUNCTION pg_catalog.op_bit_timestamptz_xor(bit, timestampTz) returns uint8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.int8_timestamptz_xor($1::int8, $2)::uint8';
DO $for_upgrade_only$
DECLARE
  ans boolean;
  v_isinplaceupgrade boolean;
BEGIN
    select case when count(*)=1 then true else false end as ans from (select setting from pg_settings where name = 'upgrade_mode' and setting != '0') into ans;
    show isinplaceupgrade into v_isinplaceupgrade;
    -- we can do drop operator only during upgrade
    if ans = true and v_isinplaceupgrade = true then
        DROP OPERATOR IF EXISTS pg_catalog.^(int1, int1);
        CREATE OPERATOR pg_catalog.^ (leftarg = int1, rightarg = int1, procedure = pg_catalog.op_int1xor);
        DROP OPERATOR IF EXISTS pg_catalog.^(int2, int2);
        CREATE OPERATOR pg_catalog.^ (leftarg = int2, rightarg = int2, procedure = pg_catalog.op_int2xor);
        DROP OPERATOR IF EXISTS pg_catalog.^(int4, int4);
        CREATE OPERATOR pg_catalog.^ (leftarg = int4, rightarg = int4, procedure = pg_catalog.op_int4xor);
        DROP OPERATOR IF EXISTS pg_catalog.^(int8, int8);
        CREATE OPERATOR pg_catalog.^ (leftarg = int8, rightarg = int8, procedure = pg_catalog.op_int8xor);
        DROP OPERATOR IF EXISTS pg_catalog.^(uint1, uint1);
        CREATE OPERATOR pg_catalog.^ (leftarg = uint1, rightarg = uint1, procedure = pg_catalog.op_uint1xor);
        DROP OPERATOR IF EXISTS pg_catalog.^(uint2, uint2);
        CREATE OPERATOR pg_catalog.^ (leftarg = uint2, rightarg = uint2, procedure = pg_catalog.op_uint2xor);
        DROP OPERATOR IF EXISTS pg_catalog.^(uint4, uint4);
        CREATE OPERATOR pg_catalog.^ (leftarg = uint4, rightarg = uint4, procedure = pg_catalog.op_uint4xor);
        DROP OPERATOR IF EXISTS pg_catalog.^(uint8, uint8);
        CREATE OPERATOR pg_catalog.^ (leftarg = uint8, rightarg = uint8, procedure = pg_catalog.op_uint8xor);
        DROP OPERATOR IF EXISTS pg_catalog.^(uint1, int1);
        CREATE OPERATOR pg_catalog.^ (leftarg = uint1, rightarg = int1, procedure = pg_catalog.op_uint1_xor_int1);
        DROP OPERATOR IF EXISTS pg_catalog.^(int1, uint1);
        CREATE OPERATOR pg_catalog.^ (leftarg = int1, rightarg = uint1, procedure = pg_catalog.op_int1_xor_uint1);
        DROP OPERATOR IF EXISTS pg_catalog.^(uint2, int2);
        CREATE OPERATOR pg_catalog.^ (leftarg = uint2, rightarg = int2, procedure = pg_catalog.op_uint2_xor_int2);
        DROP OPERATOR IF EXISTS pg_catalog.^(int2, uint2);
        CREATE OPERATOR pg_catalog.^ (leftarg = int2, rightarg = uint2, procedure = pg_catalog.op_int2_xor_uint2);
        DROP OPERATOR IF EXISTS pg_catalog.^(uint4, int4);
        CREATE OPERATOR pg_catalog.^ (leftarg = uint4, rightarg = int4, procedure = pg_catalog.op_uint4_xor_int4);
        DROP OPERATOR IF EXISTS pg_catalog.^(int4, uint4);
        CREATE OPERATOR pg_catalog.^ (leftarg = int4, rightarg = uint4, procedure = pg_catalog.op_int4_xor_uint4);
        DROP OPERATOR IF EXISTS pg_catalog.^(uint8, int8);
        CREATE OPERATOR pg_catalog.^ (leftarg = uint8, rightarg = int8, procedure = pg_catalog.op_uint8_xor_int8);
        DROP OPERATOR IF EXISTS pg_catalog.^(int8, uint8);
        CREATE OPERATOR pg_catalog.^ (leftarg = int8, rightarg = uint8, procedure = pg_catalog.op_int8_xor_uint8);
        DROP OPERATOR IF EXISTS pg_catalog.^(blob, blob);
        CREATE OPERATOR pg_catalog.^(leftarg = blob, rightarg = blob, procedure = pg_catalog.blob_xor_blob);
        DROP OPERATOR IF EXISTS pg_catalog.^(mediumblob, mediumblob);
        CREATE OPERATOR pg_catalog.^(leftarg = mediumblob, rightarg = mediumblob, procedure = pg_catalog.mblob_xor_mblob);
        DROP OPERATOR IF EXISTS pg_catalog.^(longblob, longblob);
        CREATE OPERATOR pg_catalog.^(leftarg = longblob, rightarg = longblob, procedure = pg_catalog.lblob_xor_lblob);
        DROP OPERATOR IF EXISTS pg_catalog.^(binary, binary);
        CREATE OPERATOR pg_catalog.^(leftarg = binary, rightarg = binary, procedure = pg_catalog.binary_xor_binary);
        DROP OPERATOR IF EXISTS pg_catalog.^(varbinary, varbinary);
        CREATE OPERATOR pg_catalog.^(leftarg = varbinary, rightarg = varbinary, procedure = pg_catalog.varbinary_xor_varbinary);
        DROP OPERATOR IF EXISTS pg_catalog.^(tinyblob, tinyblob);
        CREATE OPERATOR pg_catalog.^(leftarg = tinyblob, rightarg = tinyblob, procedure = pg_catalog.tinyblob_xor_tinyblob);
        DROP OPERATOR IF EXISTS pg_catalog.^(blob, integer);
        CREATE OPERATOR pg_catalog.^(leftarg = blob, rightarg = integer, procedure = pg_catalog.op_blob_int_xor);
        DROP OPERATOR IF EXISTS pg_catalog.^(integer, blob);
        CREATE OPERATOR pg_catalog.^(leftarg = integer, rightarg = blob, procedure = pg_catalog.op_int_blob_xor);
        DROP OPERATOR IF EXISTS pg_catalog.^(int8, blob);
        CREATE OPERATOR pg_catalog.^(leftarg = int8, rightarg = blob, procedure = pg_catalog.op_int8_blob_xor);
        DROP OPERATOR IF EXISTS pg_catalog.^(blob, int8);
        CREATE OPERATOR pg_catalog.^(leftarg = blob, rightarg = int8, procedure = pg_catalog.op_blob_int8_xor);
        DROP OPERATOR IF EXISTS pg_catalog.^(float8, blob);
        CREATE OPERATOR pg_catalog.^(leftarg = float8, rightarg = blob, procedure = pg_catalog.op_float8_blob_xor);
        DROP OPERATOR IF EXISTS pg_catalog.^(blob, float8);
        CREATE OPERATOR pg_catalog.^(leftarg = blob, rightarg = float8, procedure = pg_catalog.op_blob_float8_xor);
        DROP OPERATOR IF EXISTS pg_catalog.^(boolean, boolean);
        CREATE OPERATOR pg_catalog.^(leftarg = boolean, rightarg = boolean, procedure = pg_catalog.op_boolxor);
        DROP OPERATOR IF EXISTS pg_catalog.^(boolean, float8);
        CREATE OPERATOR pg_catalog.^(leftarg = boolean, rightarg = float8, procedure = pg_catalog.op_bool_float8_xor);
        DROP OPERATOR IF EXISTS pg_catalog.^(float8, boolean);
        CREATE OPERATOR pg_catalog.^(leftarg = float8, rightarg = boolean, procedure = pg_catalog.op_float8_bool_xor);
        DROP OPERATOR IF EXISTS pg_catalog.^(date, date);
        CREATE OPERATOR pg_catalog.^(leftarg = date, rightarg = date, procedure = pg_catalog.op_datexor);
        DROP OPERATOR IF EXISTS pg_catalog.^(time, time);
        CREATE OPERATOR pg_catalog.^(leftarg = time, rightarg = time, procedure = pg_catalog.op_timexor);
        DROP OPERATOR IF EXISTS pg_catalog.^(date, time);
        CREATE OPERATOR pg_catalog.^(leftarg = date, rightarg = time, procedure = pg_catalog.op_date_time_xor);
        DROP OPERATOR IF EXISTS pg_catalog.^(time, date);
        CREATE OPERATOR pg_catalog.^(leftarg = time, rightarg = date, procedure = pg_catalog.op_time_date_xor);
        DROP OPERATOR IF EXISTS pg_catalog.^(time, text);
        CREATE OPERATOR pg_catalog.^(leftarg = time, rightarg = text, procedure = pg_catalog.op_time_text_xor);
        DROP OPERATOR IF EXISTS pg_catalog.^(text, time);
        CREATE OPERATOR pg_catalog.^(leftarg = text, rightarg = time, procedure = pg_catalog.op_text_time_xor);
        DROP OPERATOR IF EXISTS pg_catalog.^(date, text);
        CREATE OPERATOR pg_catalog.^(leftarg = date, rightarg = text, procedure = pg_catalog.op_date_text_xor);
        DROP OPERATOR IF EXISTS pg_catalog.^(text, date);
        CREATE OPERATOR pg_catalog.^(leftarg = text, rightarg = date, procedure = pg_catalog.op_text_date_xor);
        DROP OPERATOR IF EXISTS pg_catalog.^(date, int8);
        CREATE OPERATOR pg_catalog.^(leftarg = date, rightarg = int8, procedure = pg_catalog.op_date_int8_xor);
        DROP OPERATOR IF EXISTS pg_catalog.^(int8, date);
        CREATE OPERATOR pg_catalog.^(leftarg = int8, rightarg = date, procedure = pg_catalog.op_int8_date_xor);
        DROP OPERATOR IF EXISTS pg_catalog.^(time, int8);
        CREATE OPERATOR pg_catalog.^(leftarg = time, rightarg = int8, procedure = pg_catalog.op_time_int8_xor);
        DROP OPERATOR IF EXISTS pg_catalog.^(int8, time);
        CREATE OPERATOR pg_catalog.^(leftarg = int8, rightarg = time, procedure = pg_catalog.op_int8_time_xor);
        DROP OPERATOR IF EXISTS pg_catalog.^(date, float8);
        CREATE OPERATOR pg_catalog.^(leftarg = date, rightarg = float8, procedure = pg_catalog.op_date_float8_xor);
        DROP OPERATOR IF EXISTS pg_catalog.^(float8, date);
        CREATE OPERATOR pg_catalog.^(leftarg = float8, rightarg = date, procedure = pg_catalog.op_float8_date_xor);
        DROP OPERATOR IF EXISTS pg_catalog.^(timestamp without time zone, timestamp without time zone);
        CREATE OPERATOR pg_catalog.^(leftarg = timestamp without time zone, rightarg = timestamp without time zone, procedure = pg_catalog.op_timestampxor);
        DROP OPERATOR IF EXISTS pg_catalog.^(timestamp without time zone, int8);
        CREATE OPERATOR pg_catalog.^(leftarg = timestamp without time zone, rightarg = int8, procedure = pg_catalog.op_timestamp_int8_xor);
        DROP OPERATOR IF EXISTS pg_catalog.^(int8, timestamp without time zone);
        CREATE OPERATOR pg_catalog.^(leftarg = int8, rightarg = timestamp without time zone, procedure = pg_catalog.op_int8_timestamp_xor);
        DROP OPERATOR IF EXISTS pg_catalog.^(timestamp without time zone, float8);
        CREATE OPERATOR pg_catalog.^(leftarg = timestamp without time zone, rightarg = float8, procedure = pg_catalog.op_timestamp_float8_xor);
        DROP OPERATOR IF EXISTS pg_catalog.^(float8, timestamp without time zone);
        CREATE OPERATOR pg_catalog.^(leftarg = float8, rightarg = timestamp without time zone, procedure = pg_catalog.op_float8_timestamp_xor);
        DROP OPERATOR IF EXISTS pg_catalog.^(timestamp without time zone, text);
        CREATE OPERATOR pg_catalog.^(leftarg = timestamp without time zone, rightarg = text, procedure = pg_catalog.op_timestamp_text_xor);
        DROP OPERATOR IF EXISTS pg_catalog.^(text, timestamp without time zone);
        CREATE OPERATOR pg_catalog.^(leftarg = text, rightarg = timestamp without time zone, procedure = pg_catalog.op_text_timestamp_xor);
        DROP OPERATOR IF EXISTS pg_catalog.^(timestampTz, timestampTz);
        CREATE OPERATOR pg_catalog.^(leftarg = timestampTz, rightarg = timestampTz, procedure = pg_catalog.op_timestamptzxor);
        DROP OPERATOR IF EXISTS pg_catalog.^(timestampTz, int8);
        CREATE OPERATOR pg_catalog.^(leftarg = timestampTz, rightarg = int8, procedure = pg_catalog.op_timestamptz_int8_xor);
        DROP OPERATOR IF EXISTS pg_catalog.^(int8, timestampTz);
        CREATE OPERATOR pg_catalog.^(leftarg = int8, rightarg = timestampTz, procedure = pg_catalog.op_int8_timestamptz_xor);
        DROP OPERATOR IF EXISTS pg_catalog.^(timestampTz, float8);
        CREATE OPERATOR pg_catalog.^(leftarg = timestampTz, rightarg = float8, procedure = pg_catalog.op_timestamptz_float8_xor);
        DROP OPERATOR IF EXISTS pg_catalog.^(float8, timestampTz);
        CREATE OPERATOR pg_catalog.^(leftarg = float8, rightarg = timestampTz, procedure = pg_catalog.op_float8_timestamptz_xor);
        DROP OPERATOR IF EXISTS pg_catalog.^(timestampTz, text);
        CREATE OPERATOR pg_catalog.^(leftarg = timestampTz, rightarg = text, procedure = pg_catalog.op_timestamptz_text_xor);
        DROP OPERATOR IF EXISTS pg_catalog.^(text, timestampTz);
        CREATE OPERATOR pg_catalog.^(leftarg = text, rightarg = timestampTz, procedure = pg_catalog.op_text_timestamptz_xor);
        DROP OPERATOR IF EXISTS pg_catalog.^(uint8, boolean);
        CREATE OPERATOR pg_catalog.^(leftarg = uint8, rightarg = boolean, procedure = pg_catalog.op_uint8_xor_bool, commutator=operator(pg_catalog.^));
        DROP OPERATOR IF EXISTS pg_catalog.^(uint4, boolean);
        CREATE OPERATOR pg_catalog.^(leftarg = uint4, rightarg = boolean, procedure = pg_catalog.op_uint4_xor_bool, commutator=operator(pg_catalog.^));
        DROP OPERATOR IF EXISTS pg_catalog.^(uint2, boolean);
        CREATE OPERATOR pg_catalog.^(leftarg = uint2, rightarg = boolean, procedure = pg_catalog.op_uint2_xor_bool, commutator=operator(pg_catalog.^));
        DROP OPERATOR IF EXISTS pg_catalog.^(uint1, boolean);
        CREATE OPERATOR pg_catalog.^(leftarg = uint1, rightarg = boolean, procedure = pg_catalog.op_uint1_xor_bool, commutator=operator(pg_catalog.^));
        DROP OPERATOR IF EXISTS pg_catalog.^(boolean, uint1);
        CREATE OPERATOR pg_catalog.^(leftarg = boolean, rightarg = uint1, procedure = pg_catalog.op_bool_xor_uint1, commutator=operator(pg_catalog.^));
        DROP OPERATOR IF EXISTS pg_catalog.^(boolean, uint2);
        CREATE OPERATOR pg_catalog.^(leftarg = boolean, rightarg = uint2, procedure = pg_catalog.op_bool_xor_uint2, commutator=operator(pg_catalog.^));
        DROP OPERATOR IF EXISTS pg_catalog.^(boolean, uint4);
        CREATE OPERATOR pg_catalog.^(leftarg = boolean, rightarg = uint4, procedure = pg_catalog.op_bool_xor_uint4, commutator=operator(pg_catalog.^));
        DROP OPERATOR IF EXISTS pg_catalog.^(boolean, uint8);
        CREATE OPERATOR pg_catalog.^(leftarg = boolean, rightarg = uint8, procedure = pg_catalog.op_bool_xor_uint8, commutator=operator(pg_catalog.^));
        DROP OPERATOR IF EXISTS pg_catalog.^(bit, bit);
        CREATE OPERATOR pg_catalog.^ (leftarg = bit, rightarg = bit, procedure = pg_catalog.op_bitxor);
        DROP OPERATOR IF EXISTS pg_catalog.^(text, text);
        CREATE OPERATOR pg_catalog.^ (leftarg = text, rightarg = text, procedure = pg_catalog.op_textxor);
        DROP OPERATOR IF EXISTS pg_catalog.^(boolean, int1);
        CREATE OPERATOR pg_catalog.^(leftarg = boolean, rightarg = int1, procedure = pg_catalog.op_bool_xor_int1, commutator=operator(pg_catalog.^));
        DROP OPERATOR IF EXISTS pg_catalog.^(boolean, int8);
        CREATE OPERATOR pg_catalog.^(leftarg = boolean, rightarg = int2, procedure = pg_catalog.op_bool_xor_int2, commutator=operator(pg_catalog.^));
        DROP OPERATOR IF EXISTS pg_catalog.^(boolean, int8);
        CREATE OPERATOR pg_catalog.^(leftarg = boolean, rightarg = int4, procedure = pg_catalog.op_bool_xor_int4, commutator=operator(pg_catalog.^));
        DROP OPERATOR IF EXISTS pg_catalog.^(boolean, int8);
        CREATE OPERATOR pg_catalog.^(leftarg = boolean, rightarg = int8, procedure = pg_catalog.op_bool_xor_int8, commutator=operator(pg_catalog.^));
        DROP OPERATOR IF EXISTS pg_catalog.^(int8, boolean);
        CREATE OPERATOR pg_catalog.^(leftarg = int8, rightarg = boolean, procedure = pg_catalog.op_int8_xor_bool, commutator=operator(pg_catalog.^));
        DROP OPERATOR IF EXISTS pg_catalog.^(int4, boolean);
        CREATE OPERATOR pg_catalog.^(leftarg = int4, rightarg = boolean, procedure = pg_catalog.op_int4_xor_bool, commutator=operator(pg_catalog.^));
        DROP OPERATOR IF EXISTS pg_catalog.^(int2, boolean);
        CREATE OPERATOR pg_catalog.^(leftarg = int2, rightarg = boolean, procedure = pg_catalog.op_int2_xor_bool, commutator=operator(pg_catalog.^));
        DROP OPERATOR IF EXISTS pg_catalog.^(int1, boolean);
        CREATE OPERATOR pg_catalog.^(leftarg = int1, rightarg = boolean, procedure = pg_catalog.op_int1_xor_bool, commutator=operator(pg_catalog.^));
        DROP OPERATOR IF EXISTS pg_catalog.^(numeric, bit);
        CREATE OPERATOR pg_catalog.^(leftarg = numeric, rightarg = bit, procedure = pg_catalog.op_num_xor_bit);
        DROP OPERATOR IF EXISTS pg_catalog.^(bit, numeric);
        CREATE OPERATOR pg_catalog.^(leftarg = bit, rightarg = numeric, procedure = pg_catalog.op_bit_xor_num);
        DROP OPERATOR IF EXISTS pg_catalog.^(uint8, bit);
        CREATE OPERATOR pg_catalog.^(leftarg = uint8, rightarg = bit, procedure = pg_catalog.op_uint8_xor_bit);
        DROP OPERATOR IF EXISTS pg_catalog.^(bit, uint8);
        CREATE OPERATOR pg_catalog.^(leftarg = bit, rightarg = uint8, procedure = pg_catalog.op_bit_xor_uint8);
        DROP OPERATOR IF EXISTS pg_catalog.^(date, bit);
        CREATE OPERATOR pg_catalog.^(leftarg = date, rightarg = bit, procedure = pg_catalog.op_date_bit_xor);
        DROP OPERATOR IF EXISTS pg_catalog.^(bit, date);
        CREATE OPERATOR pg_catalog.^(leftarg = bit, rightarg = date, procedure = pg_catalog.op_bit_date_xor);
        DROP OPERATOR IF EXISTS pg_catalog.^(timestamp without time zone, bit);
        CREATE OPERATOR pg_catalog.^(leftarg = timestamp without time zone, rightarg = bit, procedure = pg_catalog.op_timestamp_bit_xor);
        DROP OPERATOR IF EXISTS pg_catalog.^(bit, timestamp without time zone);
        CREATE OPERATOR pg_catalog.^(leftarg = bit, rightarg = timestamp without time zone, procedure = pg_catalog.op_bit_timestamp_xor);
        DROP OPERATOR IF EXISTS pg_catalog.^(timestampTz, bit);
        CREATE OPERATOR pg_catalog.^(leftarg = timestampTz, rightarg = bit, procedure = pg_catalog.op_timestamptz_bit_xor);
        DROP OPERATOR IF EXISTS pg_catalog.^(bit, timestampTz);
        CREATE OPERATOR pg_catalog.^(leftarg = bit, rightarg = timestampTz, procedure = pg_catalog.op_bit_timestamptz_xor);
    end if;
END
$for_upgrade_only$;

do $$
begin
    UPDATE pg_catalog.pg_operator SET oprresult = 'uint8'::regtype, oprcode = 'op_dpow'::regproc
        WHERE oprname = '^' AND oprleft = 'float8'::regtype AND oprright = 'float8'::regtype;
    UPDATE pg_catalog.pg_operator SET oprresult = 'uint8'::regtype, oprcode = 'op_numeric_power'::regproc
        WHERE oprname = '^' AND oprleft = 'numeric'::regtype AND oprright = 'numeric'::regtype;
end
$$;

CREATE OR REPLACE FUNCTION pg_catalog.int1_typmodin (_cstring) RETURNS int LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int1_typmodin';
CREATE OR REPLACE FUNCTION pg_catalog.int1_typmodout (int) RETURNS cstring LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int1_typmodout';
CREATE OR REPLACE FUNCTION pg_catalog.int2_typmodin (_cstring) RETURNS int LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int2_typmodin';
CREATE OR REPLACE FUNCTION pg_catalog.int2_typmodout (int) RETURNS cstring LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int2_typmodout';
CREATE OR REPLACE FUNCTION pg_catalog.int4_typmodin (_cstring) RETURNS int LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int4_typmodin';
CREATE OR REPLACE FUNCTION pg_catalog.int4_typmodout (int) RETURNS cstring LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int4_typmodout';
CREATE OR REPLACE FUNCTION pg_catalog.int8_typmodin (_cstring) RETURNS int LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int8_typmodin';
CREATE OR REPLACE FUNCTION pg_catalog.int8_typmodout (int) RETURNS cstring LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int8_typmodout';

do $$
begin
update pg_catalog.pg_type set typmodin = 'int1_typmodin'::regproc, typmodout = 'int1_typmodout'::regproc where oid in('int1'::regtype, 'uint1'::regtype);
update pg_catalog.pg_type set typmodin = 'int2_typmodin'::regproc, typmodout = 'int2_typmodout'::regproc where oid in('int2'::regtype, 'uint2'::regtype);
update pg_catalog.pg_type set typmodin = 'int4_typmodin'::regproc, typmodout = 'int4_typmodout'::regproc where oid in('int4'::regtype, 'uint4'::regtype);
update pg_catalog.pg_type set typmodin = 'int8_typmodin'::regproc, typmodout = 'int8_typmodout'::regproc where oid in('int8'::regtype, 'uint8'::regtype);
end
$$;



CREATE FUNCTION pg_catalog.text_to_bit(text) RETURNS bit LANGUAGE C IMMUTABLE STRICT AS '$libdir/dolphin', 'text_to_bit';
CREATE CAST(text AS bit) WITH FUNCTION pg_catalog.text_to_bit(text) AS ASSIGNMENT;

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
CREATE OR REPLACE FUNCTION pg_catalog.inet_ntoa(int8) RETURNS varchar LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'inetntoa';
CREATE OR REPLACE FUNCTION pg_catalog.inet_ntoa(bit) RETURNS varchar LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.inet_ntoa(cast($1 as int8))';
CREATE OR REPLACE FUNCTION pg_catalog.inet_ntoa(binary) RETURNS varchar LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.inet_ntoa(cast($1 as int8))';
CREATE OR REPLACE FUNCTION pg_catalog.inet_ntoa(tinyblob) RETURNS varchar LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.inet_ntoa(cast($1 as int8))';
CREATE OR REPLACE FUNCTION pg_catalog.inet_ntoa(blob) RETURNS varchar LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.inet_ntoa(cast($1 as int8))';
CREATE OR REPLACE FUNCTION pg_catalog.inet_ntoa(mediumblob) RETURNS varchar LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.inet_ntoa(cast($1 as int8))';
CREATE OR REPLACE FUNCTION pg_catalog.inet_ntoa(longblob) RETURNS varchar LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.inet_ntoa(cast($1 as int8))';
CREATE OR REPLACE FUNCTION pg_catalog.inet_ntoa(nvarchar2) RETURNS varchar LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.inet_ntoa(cast($1 as varchar))';
CREATE OR REPLACE FUNCTION pg_catalog.inet_ntoa(year) RETURNS varchar LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.inet_ntoa(cast($1 as int8))';
CREATE OR REPLACE FUNCTION pg_catalog.inet_ntoa(json) RETURNS varchar LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.inet_ntoa(cast($1 as int8))';

-- sum
drop aggregate if exists pg_catalog.sum(uint1);
drop aggregate if exists pg_catalog.sum(uint2);
drop aggregate if exists pg_catalog.sum(uint4);

DROP FUNCTION IF EXISTS pg_catalog.uint1_sum(int8, uint1) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.uint2_sum(int8, uint2) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.uint4_sum(int8, uint4) CASCADE;

CREATE OR REPLACE FUNCTION pg_catalog.uint1_sum(numeric, uint1) RETURNS numeric LANGUAGE C AS  '$libdir/dolphin',  'uint1_sum';
CREATE OR REPLACE FUNCTION pg_catalog.uint2_sum(numeric, uint2) RETURNS numeric LANGUAGE C AS  '$libdir/dolphin',  'uint2_sum';
CREATE OR REPLACE FUNCTION pg_catalog.uint4_sum(numeric, uint4) RETURNS numeric LANGUAGE C AS  '$libdir/dolphin',  'uint4_sum';

create aggregate pg_catalog.sum(uint1) (SFUNC=uint1_sum, cFUNC=numeric_add, STYPE= numeric );
create aggregate pg_catalog.sum(uint2) (SFUNC=uint2_sum, cFUNC=numeric_add, STYPE= numeric );
create aggregate pg_catalog.sum(uint4) (SFUNC=uint4_sum, cFUNC=numeric_add, STYPE= numeric );

CREATE OR REPLACE FUNCTION pg_catalog.float8_sum(float8, float8) RETURNS float8 LANGUAGE C AS  '$libdir/dolphin',  'float8_sum';

CREATE OR REPLACE FUNCTION pg_catalog.float_sum(double precision, float4) RETURNS double precision LANGUAGE SQL IMMUTABLE as $$ SELECT pg_catalog.float8_sum($1, $2::float8) $$;
drop aggregate if exists pg_catalog.sum_ext(float4);
create aggregate pg_catalog.sum_ext(float4) (SFUNC=float_sum, cFUNC=float8pl, STYPE= double precision, initcond = 0);

CREATE OR REPLACE FUNCTION pg_catalog.tinyint_sum(numeric, tinyint) RETURNS numeric LANGUAGE SQL IMMUTABLE as $$ SELECT pg_catalog.int8_sum($1, $2::int8) $$;
drop aggregate if exists pg_catalog.sum(tinyint);
create aggregate pg_catalog.sum(tinyint) (SFUNC=tinyint_sum, cFUNC=numeric_add, STYPE= numeric, initcond = 0);

CREATE OR REPLACE FUNCTION pg_catalog.smallint_sum_ext(numeric, smallint) RETURNS numeric LANGUAGE SQL IMMUTABLE as $$ SELECT pg_catalog.int8_sum($1, $2::int8) $$;
drop aggregate if exists pg_catalog.sum_ext(smallint);
create aggregate pg_catalog.sum_ext(smallint) (SFUNC=smallint_sum_ext, cFUNC=numeric_add, STYPE= numeric, initcond = 0);

CREATE OR REPLACE FUNCTION pg_catalog.int_sum_ext(numeric, int) RETURNS numeric LANGUAGE SQL IMMUTABLE as $$ SELECT pg_catalog.int8_sum($1, $2::int8) $$;
drop aggregate if exists pg_catalog.sum_ext(int);
create aggregate pg_catalog.sum_ext(int) (SFUNC=int_sum_ext, cFUNC=numeric_add, STYPE= numeric, initcond = 0);

CREATE OR REPLACE FUNCTION pg_catalog.tinyint_sum(numeric, year) RETURNS numeric LANGUAGE SQL IMMUTABLE as $$ SELECT pg_catalog.int8_sum($1, $2::int8) $$;
drop aggregate if exists pg_catalog.sum(year);
create aggregate pg_catalog.sum(year) (SFUNC=tinyint_sum, cFUNC=numeric_add, STYPE= numeric, initcond = 0);

CREATE OR REPLACE FUNCTION pg_catalog.text_sum(double precision, text) RETURNS double precision LANGUAGE SQL IMMUTABLE as $$ SELECT pg_catalog.float8_sum($1, $2::float8) $$;
drop aggregate if exists pg_catalog.sum(text);
create aggregate pg_catalog.sum(text) (SFUNC=text_sum, cFUNC=float8pl, STYPE= double precision, initcond = 0);

CREATE OR REPLACE FUNCTION pg_catalog.anyset_sum(double precision, anyset) RETURNS double precision LANGUAGE SQL IMMUTABLE as $$ SELECT pg_catalog.float8_sum($1, $2::float8) $$;
drop aggregate if exists pg_catalog.sum(anyset);
create aggregate pg_catalog.sum(anyset) (SFUNC=anyset_sum, cFUNC=float8pl, STYPE= double precision, initcond = 0);

CREATE OR REPLACE FUNCTION pg_catalog.anyenum_sum(double precision, anyenum) RETURNS double precision LANGUAGE SQL IMMUTABLE as $$ SELECT pg_catalog.float8_sum($1, $2::float8) $$;
drop aggregate if exists pg_catalog.sum(anyenum);
create aggregate pg_catalog.sum(anyenum) (SFUNC=anyenum_sum, cFUNC=float8pl, STYPE= double precision, initcond = 0);


DROP FUNCTION IF EXISTS pg_catalog.round(text) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.round(time without time zone) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.round(timestamp with time zone) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.round(timestamp without time zone) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.round(anyenum) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.round(anyset) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.round(text) RETURNS double precision LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.round(cast($1 as double precision))::double precision';
CREATE OR REPLACE FUNCTION pg_catalog.round(time without time zone) RETURNS double precision LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.round(cast($1 as double precision))::double precision';
CREATE OR REPLACE FUNCTION pg_catalog.round(timestamp with time zone) RETURNS double precision LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.round(cast($1 as double precision))::double precision';
CREATE OR REPLACE FUNCTION pg_catalog.round(timestamp without time zone) RETURNS double precision LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.round(cast($1 as double precision))::double precision';
CREATE OR REPLACE FUNCTION pg_catalog.round(anyenum) RETURNS double precision LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.round(cast($1 as double precision))::double precision';
CREATE OR REPLACE FUNCTION pg_catalog.round(anyset) RETURNS double precision LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.round(cast($1 as double precision))::double precision';

drop cast if exists ("binary" as varchar);
drop cast if exists ("binary" as char);
CREATE CAST ("binary" AS varchar) WITH FUNCTION pg_catalog.Varlena2Varchar(anyelement) AS IMPLICIT;
CREATE CAST ("binary" AS char) WITH FUNCTION pg_catalog.Varlena2Bpchar(anyelement) AS IMPLICIT;


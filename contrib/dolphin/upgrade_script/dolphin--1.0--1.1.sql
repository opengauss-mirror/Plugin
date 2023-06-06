DROP FUNCTION IF EXISTS pg_catalog.dolphin_invoke();
CREATE FUNCTION pg_catalog.dolphin_invoke()
    RETURNS VOID AS '$libdir/dolphin','dolphin_invoke' LANGUAGE C STRICT;

DROP FUNCTION IF EXISTS pg_catalog.bit_count(numeric) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.bit_count (numeric)  RETURNS int LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'bit_count_numeric';
DROP FUNCTION IF EXISTS pg_catalog.bit_count(text) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.bit_count (text)  RETURNS int LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'bit_count_text';
DROP FUNCTION IF EXISTS pg_catalog.bit_count_bit(text) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.bit_count(date) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.bit_count (date)  RETURNS int LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'bit_count_date';
DROP FUNCTION IF EXISTS pg_catalog.bit_count(bit) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.bit_count(bit) RETURNS int LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'bit_count_bit';

DROP FUNCTION IF EXISTS pg_catalog.connection_id() CASCADE;

CREATE OR REPLACE FUNCTION pg_catalog.connection_id() RETURNS int8 LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'connection_id';

DROP FUNCTION IF EXISTS pg_catalog.system_user() CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.system_user(
) RETURNS name LANGUAGE C STABLE STRICT AS '$libdir/dolphin','get_b_session_user';
DROP FUNCTION IF EXISTS pg_catalog.schema() CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.schema(
) RETURNS name LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'get_b_schema';

DROP FUNCTION IF EXISTS pg_catalog.from_base64 (text) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.from_base64 (bool) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.from_base64 (boolean) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.from_base64 (bit) CASCADE;

CREATE OR REPLACE FUNCTION pg_catalog.from_base64 (text)  RETURNS text LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'base64_decode_text';
CREATE OR REPLACE FUNCTION pg_catalog.from_base64 (boolean)  RETURNS text LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'base64_decode_bool';
CREATE OR REPLACE FUNCTION pg_catalog.from_base64 (bit)  RETURNS text LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'base64_decode_bit';

DROP FUNCTION IF EXISTS pg_catalog.oct(text) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.oct(boolean) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.oct(bit) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.oct(numeric) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.oct(text) RETURNS text LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'oct_str';
CREATE OR REPLACE FUNCTION pg_catalog.oct(numeric) RETURNS text LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'oct_num';

DROP FUNCTION IF EXISTS pg_catalog.to_base64 (bytea) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.to_base64 (numeric) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.to_base64 (text) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.to_base64 (boolean) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.to_base64 (bit) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.to_base64 (bytea)  RETURNS text LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'base64_encode';
CREATE OR REPLACE FUNCTION pg_catalog.to_base64 (text)  RETURNS text LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'base64_encode_text';
CREATE OR REPLACE FUNCTION pg_catalog.to_base64 (boolean)  RETURNS text LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'base64_encode_bool';
CREATE OR REPLACE FUNCTION pg_catalog.to_base64 (bit)  RETURNS text LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'base64_encode_bit';

DROP FUNCTION IF EXISTS pg_catalog.unhex ("timestamp") CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.unhex (numeric) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.unhex (text) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.unhex (boolean) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.unhex (bytea) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.unhex (bit) CASCADE;

CREATE OR REPLACE FUNCTION pg_catalog.unhex (text)  RETURNS text LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'hex_decode_text';
CREATE OR REPLACE FUNCTION pg_catalog.unhex (boolean)  RETURNS text LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'hex_decode_bool';
CREATE OR REPLACE FUNCTION pg_catalog.unhex (bytea)  RETURNS text LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'hex_decode_bytea';
CREATE OR REPLACE FUNCTION pg_catalog.unhex (bit)  RETURNS text LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'hex_decode_bit';

--bit_functions.sql
DROP FUNCTION IF EXISTS pg_catalog.bittoint1(bit) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.bittoint1 (
bit
) RETURNS int1 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'bittoint1';
DROP FUNCTION IF EXISTS pg_catalog.bittoint2(bit) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.bittoint2 (
bit
) RETURNS int2 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'bittoint2';
DROP FUNCTION IF EXISTS pg_catalog.bitfromint1(int1, int4) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.bitfromint1 (
int1, int4
) RETURNS bit LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'bitfromint1';
DROP FUNCTION IF EXISTS pg_catalog.bitfromint2(int2, int4) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.bitfromint2 (
int2, int4
) RETURNS bit LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'bitfromint2';

drop CAST IF EXISTS (bit AS int1) CASCADE;
drop CAST IF EXISTS (bit AS int2) CASCADE;
drop CAST IF EXISTS (int1 AS bit) CASCADE;
drop CAST IF EXISTS (int2 AS bit) CASCADE;

CREATE CAST (bit AS int1) WITH FUNCTION bittoint1(bit) AS ASSIGNMENT;
CREATE CAST (bit AS int2) WITH FUNCTION bittoint2(bit) AS ASSIGNMENT;
CREATE CAST (int1 AS bit) WITH FUNCTION bitfromint1(int1, int4) AS ASSIGNMENT;
CREATE CAST (int2 AS bit) WITH FUNCTION bitfromint2(int2, int4) AS ASSIGNMENT;

DROP FUNCTION IF EXISTS pg_catalog.bittotext(bit) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.bittotext(bit) 
RETURNS text LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'bittotext';
DROP CAST IF EXISTS (bit AS text) CASCADE;
CREATE CAST (bit AS text) WITH FUNCTION bittotext(bit) AS ASSIGNMENT;

DROP FUNCTION IF EXISTS pg_catalog.bittochar(bit, int4) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.bittochar(bit, int4) 
RETURNS text LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'bittobpchar';
DROP CAST IF EXISTS (bit AS char) CASCADE;
CREATE CAST (bit AS char) WITH FUNCTION bittochar(bit, int4) AS ASSIGNMENT;

DROP FUNCTION IF EXISTS pg_catalog.bittovarchar(bit, int4) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.bittovarchar(bit, int4) 
RETURNS text LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'bittovarchar';
DROP CAST IF EXISTS (bit AS varchar) CASCADE;
CREATE CAST (bit AS varchar) WITH FUNCTION bittovarchar(bit, int4) AS ASSIGNMENT;

DROP FUNCTION IF EXISTS pg_catalog.bittodate(bit) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.bittodate(bit) 
RETURNS date LANGUAGE SQL IMMUTABLE STRICT as 
'select cast(cast($1 as text) as date)';
DROP CAST IF EXISTS (bit AS date) CASCADE;
CREATE CAST (bit AS date) WITH FUNCTION bittodate(bit) AS ASSIGNMENT;

DROP FUNCTION IF EXISTS pg_catalog.bittodatetime(bit) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.bittodatetime(bit) 
RETURNS timestamp without time zone LANGUAGE SQL IMMUTABLE STRICT as 
'select cast(cast($1 as text) as timestamp without time zone)';
DROP CAST IF EXISTS (bit AS timestamp without time zone) CASCADE;
CREATE CAST (bit AS timestamp without time zone) WITH FUNCTION bittodatetime(bit) AS ASSIGNMENT;

DROP FUNCTION IF EXISTS pg_catalog.bittotimestamp(bit) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.bittotimestamp(bit) 
RETURNS timestamptz LANGUAGE SQL IMMUTABLE STRICT as 
'select cast(cast($1 as text) as timestamptz)';
DROP CAST IF EXISTS (bit AS timestamptz) CASCADE;
CREATE CAST (bit AS timestamptz) WITH FUNCTION bittotimestamp(bit) AS ASSIGNMENT;

DROP FUNCTION IF EXISTS pg_catalog.bittotime(bit) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.bittotime(bit) 
RETURNS time LANGUAGE SQL IMMUTABLE STRICT as 
'select cast(cast($1 as text) as time)';
DROP CAST IF EXISTS (bit AS time) CASCADE;
CREATE CAST (bit AS time) WITH FUNCTION bittotime(bit) AS ASSIGNMENT;

DROP FUNCTION IF EXISTS pg_catalog.bitfromnumeric(numeric, int4) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.bitfromnumeric(numeric, int4) 
RETURNS bit LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'bitfromnumeric';
DROP CAST IF EXISTS (numeric as bit) CASCADE;
CREATE CAST (numeric as bit) WITH FUNCTION bitfromnumeric(numeric, int4) AS ASSIGNMENT;

DROP FUNCTION IF EXISTS pg_catalog.bitfromfloat4(float4, int4) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.bitfromfloat4(float4, int4) 
RETURNS bit LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'bitfromfloat4';
DROP CAST IF EXISTS (float4 as bit) CASCADE;
CREATE CAST (float4 as bit) WITH FUNCTION bitfromfloat4(float4, int4) AS ASSIGNMENT;

DROP FUNCTION IF EXISTS pg_catalog.bitfromfloat8(float8, int4) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.bitfromfloat8(float8, int4) 
RETURNS bit LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'bitfromfloat8';
DROP CAST IF EXISTS (float8 as bit) CASCADE;
CREATE CAST (float8 as bit) WITH FUNCTION bitfromfloat8(float8, int4) AS ASSIGNMENT;

DROP FUNCTION IF EXISTS pg_catalog.bitfromdate(date, int4) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.bitfromdate(date, int4) 
RETURNS bit LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'bitfromdate';
DROP CAST IF EXISTS (date as bit) CASCADE;
CREATE CAST (date as bit) WITH FUNCTION bitfromdate(date, int4) AS ASSIGNMENT;

DROP FUNCTION IF EXISTS pg_catalog.bitfromdatetime(timestamp without time zone, int4) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.bitfromdatetime(timestamp without time zone, int4) 
RETURNS bit LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'bitfromdatetime';
DROP CAST IF EXISTS (timestamp without time zone as bit) CASCADE;
CREATE CAST (timestamp without time zone as bit) WITH FUNCTION bitfromdatetime(timestamp without time zone, int4) AS ASSIGNMENT;

DROP FUNCTION IF EXISTS pg_catalog.bitfromtimestamp(timestamptz, int4) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.bitfromtimestamp(timestamptz, int4) 
RETURNS bit LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'bitfromtimestamp';
DROP CAST IF EXISTS (timestamptz as bit) CASCADE;
CREATE CAST (timestamptz as bit) WITH FUNCTION bitfromtimestamp(timestamptz, int4) AS ASSIGNMENT;

DROP FUNCTION IF EXISTS pg_catalog.bitfromtime(time, int4) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.bitfromtime(time, int4) 
RETURNS bit LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'bitfromtime';
DROP CAST IF EXISTS (time as bit) CASCADE;
CREATE CAST (time as bit) WITH FUNCTION bitfromtime(time, int4) AS ASSIGNMENT;

DROP FUNCTION IF EXISTS pg_catalog.set(bit, int4) CASCADE; 
CREATE OR REPLACE FUNCTION pg_catalog.set (bit, int4) RETURNS anyset LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'bittoset';
DROP CAST IF EXISTS (bit AS anyset) CASCADE;
CREATE CAST (bit AS anyset) WITH FUNCTION pg_catalog.set(bit, int4) AS ASSIGNMENT;

--dolphin_time_functions.sql
DROP FUNCTION IF EXISTS pg_catalog.int8_year (int1) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.int16_year (int2) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.int64_year (int8) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.year_int8 (year) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.year_int16 (year) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.year_int64 (year) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.numeric_year (numeric) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.float4_year (float4) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.float8_year (float8) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.bit_year (bit) CASCADE;

DROP CAST IF EXISTS (int1 AS year) CASCADE;
DROP CAST IF EXISTS (int2 AS year) CASCADE;
DROP CAST IF EXISTS (int8 AS year) CASCADE;
DROP CAST IF EXISTS (year AS int1) CASCADE;
DROP CAST IF EXISTS (year AS int2) CASCADE;
DROP CAST IF EXISTS (year AS int8) CASCADE;
DROP CAST IF EXISTS (numeric AS year) CASCADE;
DROP CAST IF EXISTS (float4 AS year) CASCADE;
DROP CAST IF EXISTS (float8 AS year) CASCADE;
DROP CAST IF EXISTS (bit AS year) CASCADE;

CREATE OR REPLACE FUNCTION pg_catalog.int8_year (int1) RETURNS year LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'int8_year';
CREATE CAST(int1 AS year) WITH FUNCTION int8_year(int1) AS IMPLICIT;

CREATE OR REPLACE FUNCTION pg_catalog.int16_year (int2) RETURNS year LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'int16_year';
CREATE CAST(int2 AS year) WITH FUNCTION int16_year(int2) AS IMPLICIT;

CREATE OR REPLACE FUNCTION pg_catalog.int64_year (int8) RETURNS year LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'int64_year';
CREATE CAST(int8 AS year) WITH FUNCTION int64_year(int8) AS ASSIGNMENT;

CREATE OR REPLACE FUNCTION pg_catalog.year_int8 (year) RETURNS int1 LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'year_int8';
CREATE CAST(year AS int1) WITH FUNCTION year_int8(year) AS ASSIGNMENT;

CREATE OR REPLACE FUNCTION pg_catalog.year_int16 (year) RETURNS int2 LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'year_int16';
CREATE CAST(year AS int2) WITH FUNCTION year_int16(year) AS ASSIGNMENT;

CREATE OR REPLACE FUNCTION pg_catalog.year_int64 (year) RETURNS int8 LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'year_int64';
CREATE CAST(year AS int8) WITH FUNCTION year_int64(year) AS ASSIGNMENT;

CREATE OR REPLACE FUNCTION pg_catalog.numeric_year (numeric) RETURNS year LANGUAGE SQL STABLE STRICT as 'select cast(cast($1 as int8) as year)';
CREATE CAST(numeric AS year) WITH FUNCTION numeric_year(numeric) AS ASSIGNMENT;

CREATE OR REPLACE FUNCTION pg_catalog.float4_year (float4) RETURNS year LANGUAGE SQL STABLE STRICT as 'select cast(cast($1 as int8) as year)';
CREATE CAST(float4 AS year) WITH FUNCTION float4_year(float4) AS ASSIGNMENT;

CREATE OR REPLACE FUNCTION pg_catalog.float8_year (float8) RETURNS year LANGUAGE SQL STABLE STRICT as 'select cast(cast($1 as int8) as year)';
CREATE CAST(float8 AS year) WITH FUNCTION float8_year(float8) AS ASSIGNMENT;

CREATE OR REPLACE FUNCTION pg_catalog.bit_year (bit) RETURNS year LANGUAGE SQL STABLE STRICT as 'select cast(cast($1 as int8) as year)';
CREATE CAST(bit AS year) WITH FUNCTION bit_year(bit) AS ASSIGNMENT;


DROP FUNCTION IF EXISTS pg_catalog.int8_b_format_date (int1) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.int16_b_format_date (int2) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.int64_b_format_date (int8) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.numeric_b_format_date (numeric) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.float8_b_format_date (float8) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.float4_b_format_date (float4) CASCADE;

DROP CAST IF EXISTS (int1 AS date) CASCADE;
DROP CAST IF EXISTS (int2 AS date) CASCADE;
DROP CAST IF EXISTS (int8 AS date) CASCADE;
DROP CAST IF EXISTS (numeric AS date) CASCADE;
DROP CAST IF EXISTS (float8 AS date) CASCADE;
DROP CAST IF EXISTS (float4 AS date) CASCADE;

CREATE OR REPLACE FUNCTION pg_catalog.int8_b_format_date (int1) RETURNS date LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'int8_b_format_date';
CREATE CAST(int1 AS date) WITH FUNCTION int8_b_format_date(int1) AS IMPLICIT;

CREATE OR REPLACE FUNCTION pg_catalog.int16_b_format_date (int2) RETURNS date LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'int16_b_format_date';
CREATE CAST(int2 AS date) WITH FUNCTION int16_b_format_date(int2) AS IMPLICIT;

CREATE OR REPLACE FUNCTION pg_catalog.int64_b_format_date (int8) RETURNS date LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'int64_b_format_date';
CREATE CAST(int8 AS date) WITH FUNCTION int64_b_format_date(int8) AS ASSIGNMENT;

CREATE OR REPLACE FUNCTION pg_catalog.numeric_b_format_date (numeric) RETURNS date LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'numeric_b_format_date';
CREATE CAST(numeric AS date) WITH FUNCTION numeric_b_format_date(numeric) AS ASSIGNMENT;

CREATE OR REPLACE FUNCTION pg_catalog.float8_b_format_date (float8) RETURNS date LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'float8_b_format_date';
CREATE CAST(float8 AS date) WITH FUNCTION float8_b_format_date(float8) AS ASSIGNMENT;

CREATE OR REPLACE FUNCTION pg_catalog.float4_b_format_date (float4) RETURNS date LANGUAGE SQL STABLE STRICT as 'select cast(cast($1 as float8) as date)';
CREATE CAST(float4 AS date) WITH FUNCTION float4_b_format_date(float4) AS ASSIGNMENT;


DROP FUNCTION IF EXISTS pg_catalog.int8_b_format_time (int1) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.int16_b_format_time (int2) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.float8_b_format_time (float8) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.float4_b_format_time (float4) CASCADE;

DROP CAST IF EXISTS (int1 AS time) CASCADE;
DROP CAST IF EXISTS (int2 AS time) CASCADE;
DROP CAST IF EXISTS (float8 AS time) CASCADE;
DROP CAST IF EXISTS (float4 AS time) CASCADE;

CREATE OR REPLACE FUNCTION pg_catalog.int8_b_format_time (int1) RETURNS time LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'int8_b_format_time';
CREATE CAST(int1 AS time) WITH FUNCTION int8_b_format_time(int1) AS IMPLICIT;

CREATE OR REPLACE FUNCTION pg_catalog.int16_b_format_time (int2) RETURNS time LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'int16_b_format_time';
CREATE CAST(int2 AS time) WITH FUNCTION int16_b_format_time(int2) AS IMPLICIT;

CREATE OR REPLACE FUNCTION pg_catalog.float8_b_format_time (float8) RETURNS time LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'float8_b_format_time';
CREATE CAST(float8 AS time) WITH FUNCTION float8_b_format_time(float8) AS ASSIGNMENT;

CREATE OR REPLACE FUNCTION pg_catalog.float4_b_format_time (float4) RETURNS time LANGUAGE SQL STABLE STRICT as 'select cast(cast($1 as float8) as time)';
CREATE CAST(float4 AS time) WITH FUNCTION float4_b_format_time(float4) AS ASSIGNMENT;


DROP FUNCTION IF EXISTS pg_catalog.int8_b_format_datetime (int1) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.int16_b_format_datetime (int2) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.float8_b_format_datetime (float8) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.float4_b_format_datetime (float4) CASCADE;

DROP CAST IF EXISTS (int1 AS timestamp(0) without time zone) CASCADE;
DROP CAST IF EXISTS (int2 AS timestamp(0) without time zone) CASCADE;
DROP CAST IF EXISTS (float8 AS timestamp(0) without time zone) CASCADE;
DROP CAST IF EXISTS (float4 AS timestamp(0) without time zone) CASCADE;

CREATE OR REPLACE FUNCTION pg_catalog.int8_b_format_datetime (int1) RETURNS timestamp(0) without time zone LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'int8_b_format_datetime';
CREATE CAST(int1 AS timestamp(0) without time zone) WITH FUNCTION int8_b_format_datetime(int1) AS IMPLICIT;

CREATE OR REPLACE FUNCTION pg_catalog.int16_b_format_datetime (int2) RETURNS timestamp(0) without time zone LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'int16_b_format_datetime';
CREATE CAST(int2 AS timestamp(0) without time zone) WITH FUNCTION int16_b_format_datetime(int2) AS IMPLICIT;

CREATE OR REPLACE FUNCTION pg_catalog.float8_b_format_datetime (float8) RETURNS timestamp(0) without time zone LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'float8_b_format_datetime';
CREATE CAST(float8 AS timestamp(0) without time zone) WITH FUNCTION float8_b_format_datetime(float8) AS ASSIGNMENT;

CREATE OR REPLACE FUNCTION pg_catalog.float4_b_format_datetime (float4) RETURNS timestamp(0) without time zone LANGUAGE SQL STABLE STRICT as 'select cast(cast($1 as float8) as timestamp(0) without time zone)';
CREATE CAST(float4 AS timestamp(0) without time zone) WITH FUNCTION float4_b_format_datetime(float4) AS ASSIGNMENT;


DROP FUNCTION IF EXISTS pg_catalog.int8_b_format_timestamp (int1) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.int16_b_format_timestamp (int2) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.float8_b_format_timestamp (float8) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.float4_b_format_timestamp (float4) CASCADE;


DROP CAST IF EXISTS (int1 AS timestamptz) CASCADE;
DROP CAST IF EXISTS (int2 AS timestamptz) CASCADE;
DROP CAST IF EXISTS (float8 AS timestamptz) CASCADE;
DROP CAST IF EXISTS (float4 AS timestamptz) CASCADE;


CREATE OR REPLACE FUNCTION pg_catalog.int8_b_format_timestamp (int1) RETURNS timestamptz LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'int8_b_format_timestamp';
CREATE CAST(int1 AS timestamptz) WITH FUNCTION int8_b_format_timestamp(int1) AS IMPLICIT;

CREATE OR REPLACE FUNCTION pg_catalog.int16_b_format_timestamp (int2) RETURNS timestamptz LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'int16_b_format_timestamp';
CREATE CAST(int2 AS timestamptz) WITH FUNCTION int16_b_format_timestamp(int2) AS IMPLICIT;

CREATE OR REPLACE FUNCTION pg_catalog.float8_b_format_timestamp (float8) RETURNS timestamptz LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'float8_b_format_timestamp';
CREATE CAST(float8 AS timestamptz) WITH FUNCTION float8_b_format_timestamp(float8) AS ASSIGNMENT;

CREATE OR REPLACE FUNCTION pg_catalog.float4_b_format_timestamp (float4) RETURNS timestamptz LANGUAGE SQL STABLE STRICT as 'select cast(cast($1 as float8) as timestamptz)';
CREATE CAST(float4 AS timestamptz) WITH FUNCTION float4_b_format_timestamp(float4) AS ASSIGNMENT;


DROP FUNCTION IF EXISTS pg_catalog.time_int1 (time) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.time_int2 (time) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.time_float4 (time) CASCADE;

DROP CAST IF EXISTS (time AS int1) CASCADE;
DROP CAST IF EXISTS (time AS int2) CASCADE;
DROP CAST IF EXISTS (time AS float4) CASCADE;

CREATE OR REPLACE FUNCTION pg_catalog.time_int1 (time) RETURNS int1 LANGUAGE SQL STABLE STRICT as 'select cast(pg_catalog.time_float($1) as int1)';
CREATE CAST(time AS int1) WITH FUNCTION time_int1(time) AS ASSIGNMENT;

CREATE OR REPLACE FUNCTION pg_catalog.time_int2 (time) RETURNS int2 LANGUAGE SQL STABLE STRICT as 'select cast(pg_catalog.time_float($1) as int2)';
CREATE CAST(time AS int2) WITH FUNCTION time_int2(time) AS ASSIGNMENT;

CREATE OR REPLACE FUNCTION pg_catalog.time_float4 (time) RETURNS float4 LANGUAGE SQL STABLE STRICT as 'select cast(pg_catalog.time_float($1) as float4)';
CREATE CAST(time AS float4) WITH FUNCTION time_float4(time) AS ASSIGNMENT;


DROP FUNCTION IF EXISTS pg_catalog.datetime_int1 (timestamp(0) without time zone) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.datetime_int2 (timestamp(0) without time zone) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.datetime_int4 (timestamp(0) without time zone) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.datetime_float4 (timestamp(0) without time zone) CASCADE;

DROP CAST IF EXISTS (timestamp(0) without time zone AS int1) CASCADE;
DROP CAST IF EXISTS (timestamp(0) without time zone AS int2) CASCADE;
DROP CAST IF EXISTS (timestamp(0) without time zone AS int4) CASCADE;
DROP CAST IF EXISTS (timestamp(0) without time zone AS float4) CASCADE;

CREATE OR REPLACE FUNCTION pg_catalog.datetime_int1 (timestamp(0) without time zone) RETURNS int1 LANGUAGE SQL STABLE STRICT as 'select cast(pg_catalog.datetime_float($1) as int1)';
CREATE CAST(timestamp(0) without time zone AS int1) WITH FUNCTION datetime_int1(timestamp(0) without time zone) AS ASSIGNMENT;

CREATE OR REPLACE FUNCTION pg_catalog.datetime_int2 (timestamp(0) without time zone) RETURNS int2 LANGUAGE SQL STABLE STRICT as 'select cast(pg_catalog.datetime_float($1) as int2)';
CREATE CAST(timestamp(0) without time zone AS int2) WITH FUNCTION datetime_int2(timestamp(0) without time zone) AS ASSIGNMENT;

CREATE OR REPLACE FUNCTION pg_catalog.datetime_int4 (timestamp(0) without time zone) RETURNS int4 LANGUAGE SQL STABLE STRICT as 'select cast(pg_catalog.datetime_float($1) as int4)';
CREATE CAST(timestamp(0) without time zone AS int4) WITH FUNCTION datetime_int4(timestamp(0) without time zone) AS ASSIGNMENT;

CREATE OR REPLACE FUNCTION pg_catalog.datetime_float4 (timestamp(0) without time zone) RETURNS float4 LANGUAGE SQL STABLE STRICT as 'select cast(pg_catalog.datetime_float($1) as float4)';
CREATE CAST(timestamp(0) without time zone AS float4) WITH FUNCTION datetime_float4(timestamp(0) without time zone) AS ASSIGNMENT;


DROP FUNCTION IF EXISTS pg_catalog.timestamptz_int1 ("timestamptz") CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.timestamptz_int2 ("timestamptz") CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.timestamptz_int4 ("timestamptz") CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.timestamptz_float4 ("timestamptz") CASCADE;

DROP CAST IF EXISTS ("timestamptz" AS int1) CASCADE;
DROP CAST IF EXISTS ("timestamptz" AS int2) CASCADE;
DROP CAST IF EXISTS ("timestamptz" AS int4) CASCADE;
DROP CAST IF EXISTS ("timestamptz" AS float4) CASCADE;

CREATE OR REPLACE FUNCTION pg_catalog.timestamptz_int1 ("timestamptz") RETURNS int1 LANGUAGE SQL STABLE STRICT as 'select cast(pg_catalog.timestamptz_float8($1) as int1)';
CREATE CAST("timestamptz" AS int1) WITH FUNCTION timestamptz_int1("timestamptz") AS ASSIGNMENT;

CREATE OR REPLACE FUNCTION pg_catalog.timestamptz_int2 ("timestamptz") RETURNS int2 LANGUAGE SQL STABLE STRICT as 'select cast(pg_catalog.timestamptz_float8($1) as int2)';
CREATE CAST("timestamptz" AS int2) WITH FUNCTION timestamptz_int2("timestamptz") AS ASSIGNMENT;

CREATE OR REPLACE FUNCTION pg_catalog.timestamptz_int4 ("timestamptz") RETURNS int4 LANGUAGE SQL STABLE STRICT as 'select cast(pg_catalog.timestamptz_float8($1) as int4)';
CREATE CAST("timestamptz" AS int4) WITH FUNCTION timestamptz_int4("timestamptz") AS ASSIGNMENT;

CREATE OR REPLACE FUNCTION pg_catalog.timestamptz_float4 ("timestamptz") RETURNS float4 LANGUAGE SQL STABLE STRICT as 'select cast(pg_catalog.timestamptz_float8($1) as float4)';
CREATE CAST("timestamptz" AS float4) WITH FUNCTION timestamptz_float4("timestamptz") AS ASSIGNMENT;


DROP FUNCTION IF EXISTS pg_catalog.date2int1("date") cascade;
DROP FUNCTION IF EXISTS pg_catalog.date2int2("date") cascade;
DROP FUNCTION IF EXISTS pg_catalog.date2float4("date") cascade;
DROP FUNCTION IF EXISTS pg_catalog.year_float4("year") cascade;

DROP CAST IF EXISTS ("date" as int1) CASCADE;
DROP CAST IF EXISTS ("date" as int2) CASCADE;
DROP CAST IF EXISTS ("date" as float4) CASCADE;
DROP CAST IF EXISTS (year AS float4) CASCADE;

CREATE OR REPLACE FUNCTION pg_catalog.date2int1("date") RETURNS int1 LANGUAGE SQL IMMUTABLE STRICT as $$ SELECT date_int8($1)::int1; $$;
CREATE CAST ("date" as int1) with function pg_catalog.date2int1("date") AS ASSIGNMENT;

CREATE OR REPLACE FUNCTION pg_catalog.date2int2("date") RETURNS int2 LANGUAGE SQL IMMUTABLE STRICT as $$ SELECT date_int8($1)::int2; $$;
CREATE CAST ("date" as int2) with function pg_catalog.date2int2("date") AS ASSIGNMENT;

CREATE OR REPLACE FUNCTION pg_catalog.date2float4("date") RETURNS float4 LANGUAGE SQL IMMUTABLE STRICT as $$ SELECT date_int8($1)::float4; $$;
CREATE CAST ("date" as float4) with function pg_catalog.date2float4("date") AS ASSIGNMENT;

CREATE OR REPLACE FUNCTION pg_catalog.year_float4("year") RETURNS float4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'year_float4';
CREATE CAST ("year" as float4) with function pg_catalog.year_float4("year") AS ASSIGNMENT;


DROP FUNCTION IF EXISTS pg_catalog.year_date("year") cascade;
DROP FUNCTION IF EXISTS pg_catalog.year_datetime("year") cascade;
DROP FUNCTION IF EXISTS pg_catalog.year_timestamp("year") cascade;
DROP FUNCTION IF EXISTS pg_catalog.year_time("year") cascade;
DROP FUNCTION IF EXISTS pg_catalog.datetime_year(timestamp(0) without time zone) cascade;
DROP FUNCTION IF EXISTS pg_catalog.timestamp_year(timestamptz) cascade;
DROP FUNCTION IF EXISTS pg_catalog.time_year(time) cascade;
DROP FUNCTION IF EXISTS pg_catalog.date_year(date) cascade;
DROP FUNCTION IF EXISTS pg_catalog.date_time (date) cascade;
DROP FUNCTION IF EXISTS pg_catalog.time_date (time) cascade;
DROP FUNCTION IF EXISTS pg_catalog.time_datetime (time) cascade;
DROP FUNCTION IF EXISTS pg_catalog.time_timestamp (time) cascade;
DROP FUNCTION IF EXISTS pg_catalog.bpchar_timestamptz (char) cascade;
DROP FUNCTION IF EXISTS pg_catalog.varchar_timestamptz (varchar) cascade;
DROP FUNCTION IF EXISTS pg_catalog.bpchar_time (char) cascade;
DROP FUNCTION IF EXISTS pg_catalog.varchar_time (varchar) cascade;
DROP FUNCTION IF EXISTS pg_catalog.timestamp_bpchar (timestamp without time zone) cascade;
DROP FUNCTION IF EXISTS pg_catalog.timestamptz_varchar (timestamptz) cascade;
DROP FUNCTION IF EXISTS pg_catalog.timestamptz_bpchar (timestamptz) cascade;
DROP FUNCTION IF EXISTS pg_catalog.time_varchar (time) cascade;
DROP FUNCTION IF EXISTS pg_catalog.time_bpchar (time) cascade;
DROP FUNCTION IF EXISTS pg_catalog.bitfromyear(year, int4) cascade;
DROP FUNCTION IF EXISTS pg_catalog.set_date (anyset) cascade;
DROP FUNCTION IF EXISTS pg_catalog.set_datetime (anyset) cascade;
DROP FUNCTION IF EXISTS pg_catalog.set_timestamp (anyset) cascade;
DROP FUNCTION IF EXISTS pg_catalog.set_time (anyset) cascade;
DROP FUNCTION IF EXISTS pg_catalog.set_year (anyset) cascade;
DROP FUNCTION IF EXISTS pg_catalog.set (date, int4) cascade; 
DROP FUNCTION IF EXISTS pg_catalog.set(timestamp without time zone, int4) cascade; 
DROP FUNCTION IF EXISTS pg_catalog.set(timestamptz, int4) cascade; 
DROP FUNCTION IF EXISTS pg_catalog.set(time, int4) cascade; 
DROP FUNCTION IF EXISTS pg_catalog.set(year, int4) cascade;

DROP CAST IF EXISTS (year AS date) CASCADE;
DROP CAST IF EXISTS (year AS timestamp(0) without time zone) CASCADE;
DROP CAST IF EXISTS (year AS timestamptz) CASCADE;
DROP CAST IF EXISTS (year AS time) CASCADE;
DROP CAST IF EXISTS (timestamp(0) without time zone AS year) CASCADE;
DROP CAST IF EXISTS (timestamptz AS year) CASCADE;
DROP CAST IF EXISTS (time AS year) CASCADE;
DROP CAST IF EXISTS (date AS year) CASCADE;
DROP CAST IF EXISTS (date AS time) CASCADE;
DROP CAST IF EXISTS (time AS date) CASCADE;
DROP CAST IF EXISTS (time AS timestamp without time zone) CASCADE;
DROP CAST IF EXISTS (time AS timestamptz) CASCADE;
DROP CAST IF EXISTS (char AS timestamptz) CASCADE;
DROP CAST IF EXISTS (varchar AS timestamptz) CASCADE;
DROP CAST IF EXISTS (char as time) CASCADE;
DROP CAST IF EXISTS (varchar as time) CASCADE;
DROP CAST IF EXISTS (timestamp without time zone AS char) CASCADE;
DROP CAST IF EXISTS (timestamptz AS varchar) CASCADE;
DROP CAST IF EXISTS (timestamptz AS char) CASCADE;
DROP CAST IF EXISTS (time AS varchar) CASCADE;
DROP CAST IF EXISTS (time AS char) CASCADE;
DROP CAST IF EXISTS (year AS bit) CASCADE;
DROP CAST IF EXISTS (anyset as date) CASCADE;
DROP CAST IF EXISTS (anyset as timestamp without time zone) CASCADE;
DROP CAST IF EXISTS (anyset as timestamptz) CASCADE;
DROP CAST IF EXISTS (anyset as time) CASCADE;
DROP CAST IF EXISTS (anyset as year) CASCADE;
DROP CAST IF EXISTS (date AS anyset) CASCADE;
DROP CAST IF EXISTS (timestamp without time zone AS anyset) CASCADE;
DROP CAST IF EXISTS (timestamptz AS anyset) CASCADE;
DROP CAST IF EXISTS (time AS anyset) CASCADE;
DROP CAST IF EXISTS (year AS anyset) CASCADE;

CREATE OR REPLACE FUNCTION pg_catalog.year_date("year") RETURNS date LANGUAGE SQL STABLE STRICT as 'select cast(cast($1 as int4) as date)';
CREATE CAST ("year" as date) with function pg_catalog.year_date("year") AS ASSIGNMENT;

CREATE OR REPLACE FUNCTION pg_catalog.year_datetime("year") RETURNS timestamp(0) without time zone LANGUAGE SQL STABLE STRICT as 'select cast(cast($1 as int4) as timestamp(0) without time zone)';
CREATE CAST ("year" as timestamp(0) without time zone) with function pg_catalog.year_datetime("year") AS ASSIGNMENT;

CREATE OR REPLACE FUNCTION pg_catalog.year_timestamp("year") RETURNS timestamptz LANGUAGE SQL STABLE STRICT as 'select cast(cast($1 as int4) as timestamptz)';
CREATE CAST ("year" as timestamptz) with function pg_catalog.year_timestamp("year") AS ASSIGNMENT;

CREATE OR REPLACE FUNCTION pg_catalog.year_time("year") RETURNS time LANGUAGE SQL IMMUTABLE STRICT as 'select cast(cast($1 as int4) as time)';
CREATE CAST ("year" as time) with function pg_catalog.year_time("year") AS ASSIGNMENT;

CREATE OR REPLACE FUNCTION pg_catalog.datetime_year(timestamp(0) without time zone) RETURNS year LANGUAGE SQL STABLE STRICT as 'select cast(cast($1 as int8) as year)';
CREATE CAST (timestamp(0) without time zone AS year) with function pg_catalog.datetime_year(timestamp(0) without time zone) AS ASSIGNMENT;

CREATE OR REPLACE FUNCTION pg_catalog.timestamp_year(timestamptz) RETURNS year LANGUAGE SQL STABLE STRICT as 'select cast(cast($1 as int8) as year)';
CREATE CAST (timestamptz AS year) with function pg_catalog.timestamp_year(timestamptz) AS ASSIGNMENT;

CREATE OR REPLACE FUNCTION pg_catalog.time_year(time) RETURNS year LANGUAGE SQL STABLE STRICT as 'select cast(cast($1 as int8) as year)';
CREATE CAST (time AS year) with function pg_catalog.time_year(time) AS ASSIGNMENT;

CREATE OR REPLACE FUNCTION pg_catalog.date_year(date) RETURNS year LANGUAGE SQL STABLE STRICT as 'select cast(cast($1 as int8) as year)';
CREATE CAST (date AS year) with function pg_catalog.date_year(date) AS ASSIGNMENT;

CREATE OR REPLACE FUNCTION pg_catalog.date_time (date) RETURNS time LANGUAGE SQL IMMUTABLE STRICT as 'select cast(cast($1 as timestamptz) as time)';
CREATE CAST(date AS time) WITH FUNCTION date_time(date) AS ASSIGNMENT;

CREATE OR REPLACE FUNCTION pg_catalog.time_date (time) RETURNS date LANGUAGE SQL IMMUTABLE STRICT as 'select addtime(curdate()::timestamptz, $1)::date';
CREATE CAST(time AS date) WITH FUNCTION time_date(time) AS ASSIGNMENT;

CREATE OR REPLACE FUNCTION pg_catalog.time_datetime (time) RETURNS timestamp without time zone LANGUAGE SQL IMMUTABLE STRICT as 'select addtime(curdate()::timestamp without time zone, $1)::timestamp without time zone';
CREATE CAST(time AS timestamp without time zone) WITH FUNCTION time_datetime(time) AS ASSIGNMENT;

CREATE OR REPLACE FUNCTION pg_catalog.time_timestamp (time) RETURNS timestamptz LANGUAGE SQL IMMUTABLE STRICT as 'select addtime(curdate()::timestamptz, $1)::timestamptz';
CREATE CAST(time AS timestamptz) WITH FUNCTION time_timestamp(time) AS ASSIGNMENT;

CREATE OR REPLACE FUNCTION pg_catalog.bpchar_timestamptz (char) RETURNS timestamptz LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'bpchar_timestamptz';
CREATE CAST(char AS timestamptz) WITH FUNCTION bpchar_timestamptz(char) AS IMPLICIT;

CREATE OR REPLACE FUNCTION pg_catalog.varchar_timestamptz (varchar) RETURNS timestamptz LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'varchar_timestamptz';
CREATE CAST(varchar AS timestamptz) WITH FUNCTION varchar_timestamptz(varchar) AS IMPLICIT;

CREATE OR REPLACE FUNCTION pg_catalog.bpchar_time (char) RETURNS time LANGUAGE SQL IMMUTABLE STRICT as 'select cast(cast($1 as timestamptz) as time)';
CREATE CAST(char as time) WITH FUNCTION bpchar_time(char) AS IMPLICIT;

CREATE OR REPLACE FUNCTION pg_catalog.varchar_time (varchar) RETURNS time LANGUAGE SQL IMMUTABLE STRICT as 'select cast(cast($1 as timestamptz) as time)';
CREATE CAST(varchar as time) WITH FUNCTION varchar_time(varchar) AS IMPLICIT;

CREATE OR REPLACE FUNCTION pg_catalog.timestamp_bpchar (timestamp without time zone) RETURNS char LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'timestamp_bpchar';
CREATE CAST(timestamp without time zone AS char) WITH FUNCTION timestamp_bpchar(timestamp without time zone) AS IMPLICIT;

CREATE OR REPLACE FUNCTION pg_catalog.timestamptz_varchar (timestamptz) RETURNS varchar LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'timestamptz_varchar';
CREATE CAST(timestamptz AS varchar) WITH FUNCTION timestamptz_varchar(timestamptz) AS IMPLICIT;

CREATE OR REPLACE FUNCTION pg_catalog.timestamptz_bpchar (timestamptz) RETURNS char LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'timestamptz_bpchar';
CREATE CAST(timestamptz AS char) WITH FUNCTION timestamptz_bpchar(timestamptz) AS IMPLICIT;

CREATE OR REPLACE FUNCTION pg_catalog.time_varchar (time) RETURNS varchar LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'time_varchar';
CREATE CAST(time AS varchar) WITH FUNCTION time_varchar(time) AS IMPLICIT;

CREATE OR REPLACE FUNCTION pg_catalog.time_bpchar (time) RETURNS char LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'time_bpchar';
CREATE CAST(time AS char) WITH FUNCTION time_bpchar(time) AS IMPLICIT;

CREATE OR REPLACE FUNCTION pg_catalog.bitfromyear(year, int4) RETURNS bit LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'bitfromyear';
CREATE CAST (year as bit) WITH FUNCTION bitfromyear(year, int4) AS ASSIGNMENT;

CREATE OR REPLACE FUNCTION pg_catalog.set_date (anyset) RETURNS date LANGUAGE SQL IMMUTABLE STRICT as 'select cast(cast($1 as text) as date)';
CREATE CAST(anyset as date) WITH FUNCTION set_date(anyset) AS ASSIGNMENT;

CREATE OR REPLACE FUNCTION pg_catalog.set_datetime (anyset) RETURNS timestamp without time zone LANGUAGE SQL IMMUTABLE STRICT as 'select cast(cast($1 as text) as timestamp without time zone)';
CREATE CAST(anyset as timestamp without time zone) WITH FUNCTION set_datetime(anyset) AS ASSIGNMENT;

CREATE OR REPLACE FUNCTION pg_catalog.set_timestamp (anyset) RETURNS timestamptz LANGUAGE SQL IMMUTABLE STRICT as 'select cast(cast($1 as text) as timestamptz)';
CREATE CAST(anyset as timestamptz) WITH FUNCTION set_timestamp(anyset) AS ASSIGNMENT;

CREATE OR REPLACE FUNCTION pg_catalog.set_time (anyset) RETURNS time LANGUAGE SQL IMMUTABLE STRICT as 'select cast(cast($1 as text) as time)';
CREATE CAST(anyset as time) WITH FUNCTION set_time(anyset) AS ASSIGNMENT;

CREATE OR REPLACE FUNCTION pg_catalog.set_year (anyset) RETURNS year LANGUAGE SQL IMMUTABLE STRICT as 'select cast(cast($1 as text) as year)';
CREATE CAST(anyset as year) WITH FUNCTION set_year(anyset) AS ASSIGNMENT;

CREATE OR REPLACE FUNCTION pg_catalog.set (date, int4) RETURNS anyset LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'datetoset';
CREATE CAST (date AS anyset) WITH FUNCTION pg_catalog.set(date, int4) AS ASSIGNMENT;

CREATE OR REPLACE FUNCTION pg_catalog.set (timestamp without time zone, int4) RETURNS anyset LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'datetimetoset';
CREATE CAST (timestamp without time zone AS anyset) WITH FUNCTION pg_catalog.set(timestamp without time zone, int4) AS ASSIGNMENT;

CREATE OR REPLACE FUNCTION pg_catalog.set (timestamptz, int4) RETURNS anyset LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'timestamptoset';
CREATE CAST (timestamptz AS anyset) WITH FUNCTION pg_catalog.set(timestamptz, int4) AS ASSIGNMENT;

CREATE OR REPLACE FUNCTION pg_catalog.set (time, int4) RETURNS anyset LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'timetoset';
CREATE CAST (time AS anyset) WITH FUNCTION pg_catalog.set(time, int4) AS ASSIGNMENT;

CREATE OR REPLACE FUNCTION pg_catalog.set (year, int4) RETURNS anyset LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'yeartoset';
CREATE CAST (year AS anyset) WITH FUNCTION pg_catalog.set(year, int4) AS ASSIGNMENT;


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

create function pg_catalog.time_timestamptz_xor(
    time,
    timestamptz
) RETURNS int16 LANGUAGE SQL IMMUTABLE STRICT as $$ SELECT pg_catalog.int8_timestamptz_xor($1::int8, $2) $$;

create function pg_catalog.timestamptz_time_xor(
    timestamptz,
    time
) RETURNS int16 LANGUAGE SQL IMMUTABLE STRICT as $$ SELECT pg_catalog.int8_timestamptz_xor($2::int8, $1) $$;

create function pg_catalog.bool_date_xor(
    boolean,
    date
) RETURNS int16 LANGUAGE SQL IMMUTABLE STRICT as $$ SELECT pg_catalog.int8_date_xor($1::int8, $2) $$;

create function pg_catalog.bool_timestamptz_xor(
    boolean,
    timestamptz
) RETURNS int16 LANGUAGE SQL IMMUTABLE STRICT as $$ SELECT pg_catalog.int8_timestamptz_xor($1::int8, $2) $$;

create function pg_catalog.date_bool_xor(
    date,
    boolean
) RETURNS int16 LANGUAGE SQL IMMUTABLE STRICT as $$ SELECT pg_catalog.date_int8_xor($1, $2::int8) $$;

create function pg_catalog.timestamptz_bool_xor(
    timestamptz,
    boolean
) RETURNS int16 LANGUAGE SQL IMMUTABLE STRICT as $$ SELECT pg_catalog.timestamptz_int8_xor($1, $2::int8) $$;

create function pg_catalog.nvarchar2_mi_time(
    nvarchar2,
    time
) RETURNS float8 LANGUAGE SQL IMMUTABLE STRICT as $$ SELECT pg_catalog.datetime_mi_float($1::timestamp without time zone, $2::float8) $$;

create function pg_catalog.nvarchar2_pl_time(
    nvarchar2,
    time
) RETURNS float8 LANGUAGE SQL IMMUTABLE STRICT as $$ SELECT pg_catalog.datetime_pl_float($1::timestamp without time zone, $2::float8) $$;

create function pg_catalog.date_add_numeric(
    date,
    numeric
) RETURNS date LANGUAGE SQL IMMUTABLE STRICT as $$ SELECT pg_catalog.date_pli($1, $2::int4) $$;

create function pg_catalog.date_sub_numeric(
    date,
    numeric
) RETURNS date LANGUAGE SQL IMMUTABLE STRICT as $$ SELECT pg_catalog.date_mii($1, $2::int4) $$;

create function pg_catalog.time_mul_numeric(
    time,
    numeric
) RETURNS float8 LANGUAGE SQL IMMUTABLE STRICT as $$ SELECT pg_catalog.float8mul($1::float8, $2::float8) $$;

create function pg_catalog.time_div_numeric(
    time,
    numeric
) RETURNS float8 LANGUAGE SQL IMMUTABLE STRICT as $$ SELECT pg_catalog.float8div($1::float8, $2::float8) $$;

create function pg_catalog.datetime_mul_numeric(
    timestamp without time zone,
    numeric
) RETURNS float8 LANGUAGE SQL IMMUTABLE STRICT as $$ SELECT pg_catalog.float8mul($1::float8, $2::float8) $$;

create function pg_catalog.datetime_div_numeric(
    timestamp without time zone,
    numeric
) RETURNS float8 LANGUAGE SQL IMMUTABLE STRICT as $$ SELECT pg_catalog.float8div($1::float8, $2::float8) $$;

create function pg_catalog.datetime_mul_int4(
    timestamp without time zone,
    int4
) RETURNS float8 LANGUAGE SQL IMMUTABLE STRICT as $$ SELECT pg_catalog.float8mul($1::float8, $2::float8) $$;

create operator pg_catalog.^(leftarg = time, rightarg = timestampTz, procedure = pg_catalog.time_timestamptz_xor);
create operator pg_catalog.^(leftarg = timestampTz, rightarg = time, procedure = pg_catalog.timestamptz_time_xor);
create operator pg_catalog.^(leftarg = boolean, rightarg = date, procedure = pg_catalog.bool_date_xor);
create operator pg_catalog.^(leftarg = boolean, rightarg = timestamptz, procedure = pg_catalog.bool_timestamptz_xor);
create operator pg_catalog.^(leftarg = date, rightarg = boolean, procedure = pg_catalog.date_bool_xor);
create operator pg_catalog.^(leftarg = timestamptz, rightarg = boolean, procedure = pg_catalog.timestamptz_bool_xor);
create operator pg_catalog.-(leftarg = nvarchar2, rightarg = time, procedure = pg_catalog.nvarchar2_mi_time);
create operator pg_catalog.+(leftarg = nvarchar2, rightarg = time, procedure = pg_catalog.nvarchar2_pl_time);
create operator pg_catalog.+(leftarg = date, rightarg = numeric, procedure = pg_catalog.date_add_numeric);
create operator pg_catalog.-(leftarg = date, rightarg = numeric, procedure = pg_catalog.date_sub_numeric);
create operator pg_catalog.*(leftarg = time, rightarg = numeric, procedure = pg_catalog.time_mul_numeric);
create operator pg_catalog./(leftarg = time, rightarg = numeric, procedure = pg_catalog.time_div_numeric);
create operator pg_catalog.*(leftarg = timestamp without time zone, rightarg = numeric, procedure = pg_catalog.datetime_mul_numeric);
create operator pg_catalog./(leftarg = timestamp without time zone, rightarg = numeric, procedure = pg_catalog.datetime_div_numeric);
create operator pg_catalog.*(leftarg = timestamp without time zone, rightarg = int4, procedure = pg_catalog.datetime_mul_int4);

DROP FUNCTION IF EXISTS pg_catalog.b_db_date(date) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.dayname(date) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.b_db_last_day(date) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.week(date) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.yearweek(date) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.adddate(date, interval) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.date_add(date, interval) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.date_sub(date, interval) CASCADE;

CREATE OR REPLACE FUNCTION pg_catalog.b_db_date(date) RETURNS date LANGUAGE SQL STABLE STRICT as 'select pg_catalog.b_db_date($1::text)';
CREATE OR REPLACE FUNCTION pg_catalog.dayname(date) RETURNS text LANGUAGE SQL STABLE STRICT as 'select pg_catalog.dayname($1::text)';
CREATE OR REPLACE FUNCTION pg_catalog.b_db_last_day(date) RETURNS date LANGUAGE SQL STABLE STRICT as 'select pg_catalog.b_db_last_day($1::text)';
CREATE OR REPLACE FUNCTION pg_catalog.week(date) RETURNS int4 LANGUAGE SQL STABLE STRICT as 'select pg_catalog.week($1::text)';
CREATE OR REPLACE FUNCTION pg_catalog.yearweek(date) RETURNS int8 LANGUAGE SQL STABLE STRICT as 'select pg_catalog.yearweek($1::text)';
CREATE OR REPLACE FUNCTION pg_catalog.adddate(date, interval) RETURNS text LANGUAGE SQL STABLE STRICT as 'select pg_catalog.adddate($1::text, $2)';
CREATE OR REPLACE FUNCTION pg_catalog.date_add(date, interval) RETURNS text LANGUAGE SQL STABLE STRICT as 'select pg_catalog.date_add($1::text, $2)';
CREATE OR REPLACE FUNCTION pg_catalog.date_sub(date, interval) RETURNS text LANGUAGE SQL STABLE STRICT as 'select pg_catalog.date_sub($1::text, $2)';

--enum.sql
DROP CAST IF EXISTS (anyenum as float8) cascade;
DROP FUNCTION IF EXISTS pg_catalog.enum2float8(anyenum) cascade;

DROP FUNCTION IF EXISTS pg_catalog.enum_float8(anyenum) cascade;
DROP FUNCTION IF EXISTS pg_catalog.enum_int1(anyenum) cascade;
DROP FUNCTION IF EXISTS pg_catalog.enum_int2(anyenum) cascade;
DROP FUNCTION IF EXISTS pg_catalog.enum_int4(anyenum) cascade;
DROP FUNCTION IF EXISTS pg_catalog.enum_int8(anyenum) cascade;
DROP FUNCTION IF EXISTS pg_catalog.enum_numeric(anyenum) cascade;
DROP FUNCTION IF EXISTS pg_catalog.enum_float4(anyenum) cascade;
DROP FUNCTION IF EXISTS pg_catalog.enum_bit(anyenum, int4) cascade;
DROP FUNCTION IF EXISTS pg_catalog.enum_date(anyenum) cascade;
DROP FUNCTION IF EXISTS pg_catalog.enum_timestamp(anyenum) cascade;
DROP FUNCTION IF EXISTS pg_catalog.enum_timestamptz(anyenum) cascade;
DROP FUNCTION IF EXISTS pg_catalog.enum_time(anyenum) cascade;
DROP FUNCTION IF EXISTS pg_catalog.enum_year(anyenum) cascade;
DROP FUNCTION IF EXISTS pg_catalog.enum_set(anyenum, int4) cascade;
DROP FUNCTION IF EXISTS pg_catalog.enum_json(anyenum) cascade;
DROP FUNCTION IF EXISTS pg_catalog.text_enum(text, int4, anyelement) cascade;
DROP FUNCTION IF EXISTS pg_catalog.bpchar_enum(char, int4, anyelement) cascade;
DROP FUNCTION IF EXISTS pg_catalog.varchar_enum(varchar, int4, anyelement) cascade;
DROP FUNCTION IF EXISTS pg_catalog.varlena_enum(anyelement, int4) cascade;
DROP FUNCTION IF EXISTS pg_catalog.set_enum(anyset, int4, anyelement) cascade;
DROP FUNCTION IF EXISTS pg_catalog.date_enum(date, int4, anyelement) cascade;
DROP FUNCTION IF EXISTS pg_catalog.datetime_enum(timestamp without time zone, int4, anyelement) cascade;
DROP FUNCTION IF EXISTS pg_catalog.timestamp_enum(timestamptz, int4, anyelement) cascade;
DROP FUNCTION IF EXISTS pg_catalog.time_enum(time, int4, anyelement) cascade;
DROP FUNCTION IF EXISTS pg_catalog.int1_enum(int1, int4, anyelement) cascade;
DROP FUNCTION IF EXISTS pg_catalog.int2_enum(int2, int4, anyelement) cascade;
DROP FUNCTION IF EXISTS pg_catalog.int4_enum(int4, int4, anyelement) cascade;
DROP FUNCTION IF EXISTS pg_catalog.int8_enum(int8, int4, anyelement) cascade;
DROP FUNCTION IF EXISTS pg_catalog.float4_enum(float4, int4, anyelement) cascade;
DROP FUNCTION IF EXISTS pg_catalog.float8_enum(float8, int4, anyelement) cascade;
DROP FUNCTION IF EXISTS pg_catalog.numeric_enum(numeric, int4, anyelement) cascade;
DROP FUNCTION IF EXISTS pg_catalog.bit_enum(bit, int4, anyelement) cascade;
DROP FUNCTION IF EXISTS pg_catalog.year_enum(year, int4, anyelement) cascade;

CREATE OR REPLACE FUNCTION pg_catalog.enum_float8(anyenum) RETURNS float8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'Enum2Float8';

CREATE OR REPLACE FUNCTION pg_catalog.enum_int1(anyenum) RETURNS int1 LANGUAGE SQL IMMUTABLE STRICT as 'select cast(cast($1 as float8) as int1)';

CREATE OR REPLACE FUNCTION pg_catalog.enum_int2(anyenum) RETURNS int2 LANGUAGE SQL IMMUTABLE STRICT as 'select cast(cast($1 as float8) as int2)';

CREATE OR REPLACE FUNCTION pg_catalog.enum_int4(anyenum) RETURNS int4 LANGUAGE SQL IMMUTABLE STRICT as 'select cast(cast($1 as float8) as int4)';

CREATE OR REPLACE FUNCTION pg_catalog.enum_int8(anyenum) RETURNS int8 LANGUAGE SQL IMMUTABLE STRICT as 'select cast(cast($1 as float8) as int8)';

CREATE OR REPLACE FUNCTION pg_catalog.enum_numeric(anyenum) RETURNS numeric LANGUAGE SQL IMMUTABLE STRICT as 'select cast(cast($1 as float8) as numeric)';

CREATE OR REPLACE FUNCTION pg_catalog.enum_float4(anyenum) RETURNS float4 LANGUAGE SQL IMMUTABLE STRICT as 'select cast(cast($1 as float8) as float4)';

CREATE OR REPLACE FUNCTION pg_catalog.enum_bit(anyenum, int4) RETURNS bit LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'Enum2Bit';

CREATE OR REPLACE FUNCTION pg_catalog.enum_date(anyenum) RETURNS date LANGUAGE SQL IMMUTABLE STRICT as 'select cast(cast($1 as text) as date)';

CREATE OR REPLACE FUNCTION pg_catalog.enum_timestamp(anyenum) RETURNS timestamp without time zone LANGUAGE SQL IMMUTABLE STRICT as 
'select cast(cast($1 as text) as timestamp without time zone)';

CREATE OR REPLACE FUNCTION pg_catalog.enum_timestamptz(anyenum) RETURNS timestamptz LANGUAGE SQL IMMUTABLE STRICT as 
'select cast(cast($1 as text) as timestaptz)';

CREATE OR REPLACE FUNCTION pg_catalog.enum_time(anyenum) RETURNS time LANGUAGE SQL IMMUTABLE STRICT as 'select cast(cast($1 as text) as time)';

CREATE OR REPLACE FUNCTION pg_catalog.enum_year(anyenum) RETURNS year LANGUAGE SQL IMMUTABLE STRICT as 'select cast(cast($1 as text) as year)';

CREATE OR REPLACE FUNCTION pg_catalog.enum_set(anyenum, int4) RETURNS anyset LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'enumtoset';

CREATE OR REPLACE FUNCTION pg_catalog.enum_json(anyenum) RETURNS json LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'cast_json';


CREATE OR REPLACE FUNCTION pg_catalog.text_enum(text, int4, anyelement) RETURNS anyenum LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'text_enum';

CREATE OR REPLACE FUNCTION pg_catalog.bpchar_enum(char, int4, anyelement) RETURNS anyenum LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'bpchar_enum';

CREATE OR REPLACE FUNCTION pg_catalog.varchar_enum(varchar, int4, anyelement) RETURNS anyenum LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'varchar_enum';

CREATE OR REPLACE FUNCTION pg_catalog.varlena_enum(anyelement, int4) RETURNS anyenum LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'varlena_enum';

CREATE OR REPLACE FUNCTION pg_catalog.set_enum(anyset, int4, anyelement) RETURNS anyenum LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'set_enum';

CREATE OR REPLACE FUNCTION pg_catalog.date_enum(date, int4, anyelement) RETURNS anyenum LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'date_enum';

CREATE OR REPLACE FUNCTION pg_catalog.datetime_enum(timestamp without time zone, int4, anyelement) RETURNS anyenum LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'datetime_enum';

CREATE OR REPLACE FUNCTION pg_catalog.timestamp_enum(timestamptz, int4, anyelement) RETURNS anyenum LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'timestamp_enum';

CREATE OR REPLACE FUNCTION pg_catalog.time_enum(time, int4, anyelement) RETURNS anyenum LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'time_enum';

CREATE OR REPLACE FUNCTION pg_catalog.int1_enum(int1, int4, anyelement) RETURNS anyenum LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int1_enum';

CREATE OR REPLACE FUNCTION pg_catalog.int2_enum(int2, int4, anyelement) RETURNS anyenum LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int2_enum';

CREATE OR REPLACE FUNCTION pg_catalog.int4_enum(int4, int4, anyelement) RETURNS anyenum LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int4_enum';

CREATE OR REPLACE FUNCTION pg_catalog.int8_enum(int8, int4, anyelement) RETURNS anyenum LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int8_enum';

CREATE OR REPLACE FUNCTION pg_catalog.float4_enum(float4, int4, anyelement) RETURNS anyenum LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'float4_enum';

CREATE OR REPLACE FUNCTION pg_catalog.float8_enum(float8, int4, anyelement) RETURNS anyenum LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'float8_enum';

CREATE OR REPLACE FUNCTION pg_catalog.numeric_enum(numeric, int4, anyelement) RETURNS anyenum LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'numeric_enum';

CREATE OR REPLACE FUNCTION pg_catalog.bit_enum(bit, int4, anyelement) RETURNS anyenum LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'bit_enum';

CREATE OR REPLACE FUNCTION pg_catalog.year_enum(year, int4, anyelement) RETURNS anyenum LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'year_enum';

--json.sql
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
DROP FUNCTION IF EXISTS pg_catalog.year_json(year) cascade;
DROP FUNCTION IF EXISTS pg_catalog.set_json(anyset) cascade;
DROP FUNCTION IF EXISTS pg_catalog.enum_varlena(anyenum) cascade;

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
DROP CAST IF EXISTS (year as json) cascade;
DROP CAST IF EXISTS (anyset as json) cascade;



CREATE OR REPLACE FUNCTION pg_catalog.int1_json(int1) RETURNS json LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'cast_json';
CREATE CAST (int1 as json) WITH FUNCTION pg_catalog.int1_json(int1);

CREATE OR REPLACE FUNCTION pg_catalog.int2_json(int2) RETURNS json LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'cast_json';
CREATE CAST (int2 as json) WITH FUNCTION pg_catalog.int2_json(int2);

CREATE OR REPLACE FUNCTION pg_catalog.int4_json(int4) RETURNS json LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'cast_json';
CREATE CAST (int4 as json) WITH FUNCTION pg_catalog.int4_json(int4);

CREATE OR REPLACE FUNCTION pg_catalog.int8_json(int8) RETURNS json LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'cast_json';
CREATE CAST (int8 as json) WITH FUNCTION pg_catalog.int8_json(int8);

CREATE OR REPLACE FUNCTION pg_catalog.float4_json(float4) RETURNS json LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'cast_json';
CREATE CAST (float4 as json) WITH FUNCTION pg_catalog.float4_json(float4);

CREATE OR REPLACE FUNCTION pg_catalog.float8_json(float8) RETURNS json LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'cast_json';
CREATE CAST (float8 as json) WITH FUNCTION pg_catalog.float8_json(float8);

CREATE OR REPLACE FUNCTION pg_catalog.numeric_json(numeric) RETURNS json LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'cast_json';
CREATE CAST (numeric as json) WITH FUNCTION pg_catalog.numeric_json(numeric);

CREATE OR REPLACE FUNCTION pg_catalog.date_json(date) RETURNS json LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'cast_json';
CREATE CAST (date as json) WITH FUNCTION pg_catalog.date_json(date);

CREATE OR REPLACE FUNCTION pg_catalog.datetime_json(timestamp without time zone) RETURNS json LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'cast_json';
CREATE CAST (timestamp without time zone as json) WITH FUNCTION pg_catalog.datetime_json(timestamp without time zone);

CREATE OR REPLACE FUNCTION pg_catalog.timestamp_json(timestamptz) RETURNS json LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'cast_json';
CREATE CAST (timestamptz as json) WITH FUNCTION pg_catalog.timestamp_json(timestamptz);

CREATE OR REPLACE FUNCTION pg_catalog.time_json(time) RETURNS json LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'cast_json';
CREATE CAST (time as json) WITH FUNCTION pg_catalog.time_json(time);

CREATE OR REPLACE FUNCTION pg_catalog.year_json(year) RETURNS json LANGUAGE SQL IMMUTABLE STRICT as 'select cast(cast($1 as int4) as json)';
CREATE CAST (year as json) WITH FUNCTION pg_catalog.year_json(year);

CREATE OR REPLACE FUNCTION pg_catalog.set_json(anyset) RETURNS json LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'cast_json';
CREATE CAST (anyset as json) WITH FUNCTION pg_catalog.set_json(anyset);

CREATE OR REPLACE FUNCTION pg_catalog.enum_varlena(anyenum) RETURNS json LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'cast_json';

--to_base64.sql
DROP FUNCTION IF EXISTS pg_catalog.bit_json (bit) CASCADE;
DROP CAST IF EXISTS (bit as json);

CREATE OR REPLACE FUNCTION pg_catalog.bit_json(bit)
RETURNS json 
AS
$$
BEGIN
    RETURN (SELECT to_json(concat('base64:type16:', to_base64($1))));
END;
$$
LANGUAGE plpgsql;

CREATE CAST (bit as json) WITH FUNCTION pg_catalog.bit_json(bit);

--unsigned_int.sql
DROP FUNCTION IF EXISTS pg_catalog.int8_b_format_date (uint1) cascade;
DROP FUNCTION IF EXISTS pg_catalog.int16_b_format_date (uint2) cascade;

DROP CAST IF EXISTS (uint1 AS date) CASCADE;
DROP CAST IF EXISTS (uint2 AS date) CASCADE;

CREATE OR REPLACE FUNCTION pg_catalog.int8_b_format_date (uint1) 
RETURNS date LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int8_b_format_date';
CREATE CAST(uint1 AS date) WITH FUNCTION int8_b_format_date(uint1) AS ASSIGNMENT;

CREATE OR REPLACE FUNCTION pg_catalog.int16_b_format_date (uint2) 
RETURNS date LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int16_b_format_date';
CREATE CAST(uint2 AS date) WITH FUNCTION int16_b_format_date(uint2) AS ASSIGNMENT;


DROP FUNCTION IF EXISTS pg_catalog.int64_b_format_date (uint8) cascade;
DROP FUNCTION IF EXISTS pg_catalog.date2uint1("date") cascade;
DROP FUNCTION IF EXISTS pg_catalog.date2uint2("date") cascade;
DROP FUNCTION IF EXISTS pg_catalog.date2uint4("date") cascade;
DROP FUNCTION IF EXISTS pg_catalog.date2uint8("date") cascade;
DROP FUNCTION IF EXISTS pg_catalog.int8_b_format_time(uint1) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.int16_b_format_time(uint2) CASCADE;

DROP CAST IF EXISTS (uint8 AS date) CASCADE;
DROP CAST IF EXISTS ("date" as uint1) CASCADE;
DROP CAST IF EXISTS ("date" as uint2) CASCADE;
DROP CAST IF EXISTS ("date" as uint4) CASCADE;
DROP CAST IF EXISTS ("date" as uint8) CASCADE;
DROP CAST IF EXISTS (uint1 AS time) CASCADE;
DROP CAST IF EXISTS (uint2 AS time) CASCADE;

CREATE OR REPLACE FUNCTION pg_catalog.int64_b_format_date (uint8) 
RETURNS date LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int64_b_format_date';
CREATE CAST(uint8 AS date) WITH FUNCTION int64_b_format_date(uint8) AS ASSIGNMENT;

CREATE OR REPLACE FUNCTION pg_catalog.date2uint1("date") RETURNS uint1 LANGUAGE SQL IMMUTABLE STRICT as $$ SELECT date_int8($1)::uint1; $$;
CREATE CAST ("date" as uint1) with function pg_catalog.date2uint1("date") AS ASSIGNMENT;

CREATE OR REPLACE FUNCTION pg_catalog.date2uint2("date") RETURNS uint2 LANGUAGE SQL IMMUTABLE STRICT as $$ SELECT date_int8($1)::uint2; $$;
CREATE CAST ("date" as uint2) with function pg_catalog.date2uint2("date") AS ASSIGNMENT;

CREATE OR REPLACE FUNCTION pg_catalog.date2uint4("date") RETURNS uint4 LANGUAGE SQL IMMUTABLE STRICT as $$ SELECT date_int8($1)::uint4; $$;
CREATE CAST ("date" as uint4) with function pg_catalog.date2uint4("date") AS ASSIGNMENT;

CREATE OR REPLACE FUNCTION pg_catalog.date2uint8("date") RETURNS uint8 LANGUAGE SQL IMMUTABLE STRICT as $$ SELECT date_int8($1)::uint8; $$;
CREATE CAST ("date" as uint8) with function pg_catalog.date2uint8("date") AS ASSIGNMENT;

CREATE OR REPLACE FUNCTION pg_catalog.int8_b_format_time (
uint1
) RETURNS time LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int8_b_format_time';
CREATE CAST (uint1 AS time) WITH FUNCTION int8_b_format_time(uint1) AS ASSIGNMENT;

CREATE OR REPLACE FUNCTION pg_catalog.int16_b_format_time (
uint2
) RETURNS time LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int16_b_format_time';
CREATE CAST (uint2 AS time) WITH FUNCTION int16_b_format_time(uint2) AS ASSIGNMENT;


DROP FUNCTION IF EXISTS pg_catalog.int64_b_format_time(uint8) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.time_uint1 (time) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.time_uint2 (time) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.time_uint4 (time) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.time_uint8 (time) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.int8_b_format_datetime(uint1) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.int16_b_format_datetime(uint2) CASCADE;

DROP CAST IF EXISTS (uint8 AS time) CASCADE;
DROP CAST IF EXISTS (time AS uint1) CASCADE;
DROP CAST IF EXISTS (time AS uint2) CASCADE;
DROP CAST IF EXISTS (time AS uint4) CASCADE;
DROP CAST IF EXISTS (time AS uint8) CASCADE;
DROP CAST IF EXISTS (uint1 AS timestamp(0) without time zone) CASCADE;
DROP CAST IF EXISTS (uint2 AS timestamp(0) without time zone) CASCADE;

CREATE OR REPLACE FUNCTION pg_catalog.int64_b_format_time (
uint8
) RETURNS time LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int64_b_format_time';
CREATE CAST (uint8 AS time) WITH FUNCTION int64_b_format_time(uint8) AS ASSIGNMENT;

CREATE OR REPLACE FUNCTION pg_catalog.time_uint1 (time) RETURNS uint1 LANGUAGE SQL STABLE STRICT as 'select cast(pg_catalog.time_float($1) as uint1)';
CREATE CAST(time AS uint1) WITH FUNCTION time_uint1(time) AS ASSIGNMENT;

CREATE OR REPLACE FUNCTION pg_catalog.time_uint2 (time) RETURNS uint2 LANGUAGE SQL STABLE STRICT as 'select cast(pg_catalog.time_float($1) as uint2)';
CREATE CAST(time AS uint2) WITH FUNCTION time_uint2(time) AS ASSIGNMENT;

CREATE OR REPLACE FUNCTION pg_catalog.time_uint4 (time) RETURNS uint4 LANGUAGE SQL STABLE STRICT as 'select cast(pg_catalog.time_float($1) as uint4)';
CREATE CAST(time AS uint4) WITH FUNCTION time_uint4(time) AS ASSIGNMENT;

CREATE OR REPLACE FUNCTION pg_catalog.time_uint8 (time) RETURNS uint8 LANGUAGE SQL STABLE STRICT as 'select cast(pg_catalog.time_float($1) as uint8)';
CREATE CAST(time AS uint8) WITH FUNCTION time_uint8(time) AS ASSIGNMENT;

CREATE OR REPLACE FUNCTION pg_catalog.int8_b_format_datetime (
uint1
) RETURNS timestamp(0) without time zone LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int8_b_format_datetime';
CREATE CAST (uint1 AS timestamp(0) without time zone) WITH FUNCTION int8_b_format_datetime(uint1) AS ASSIGNMENT;

CREATE OR REPLACE FUNCTION pg_catalog.int16_b_format_datetime (
uint2
) RETURNS timestamp(0) without time zone LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int16_b_format_datetime';
CREATE CAST (uint2 AS timestamp(0) without time zone) WITH FUNCTION int16_b_format_datetime(uint2) AS ASSIGNMENT;


DROP FUNCTION IF EXISTS pg_catalog.datetime_uint1 (timestamp(0) without time zone) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.datetime_uint2 (timestamp(0) without time zone) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.datetime_uint4 (timestamp(0) without time zone) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.datetime_uint8 (timestamp(0) without time zone) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.int8_b_format_timestamp(uint1) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.int16_b_format_timestamp(uint2) CASCADE;

DROP CAST IF EXISTS (timestamp(0) without time zone AS uint1) CASCADE;
DROP CAST IF EXISTS (timestamp(0) without time zone AS uint2) CASCADE;
DROP CAST IF EXISTS (timestamp(0) without time zone AS uint4) CASCADE;
DROP CAST IF EXISTS (timestamp(0) without time zone AS uint8) CASCADE;
DROP CAST IF EXISTS (uint1 AS timestamptz) CASCADE;
DROP CAST IF EXISTS (uint2 AS timestamptz) CASCADE;

CREATE OR REPLACE FUNCTION pg_catalog.datetime_uint1 (timestamp(0) without time zone) RETURNS uint1 LANGUAGE SQL STABLE STRICT as 'select cast(pg_catalog.datetime_float($1) as uint1)';
CREATE CAST(timestamp(0) without time zone AS uint1) WITH FUNCTION datetime_uint1(timestamp(0) without time zone) AS ASSIGNMENT;

CREATE OR REPLACE FUNCTION pg_catalog.datetime_uint2 (timestamp(0) without time zone) RETURNS uint2 LANGUAGE SQL STABLE STRICT as 'select cast(pg_catalog.datetime_float($1) as uint2)';
CREATE CAST(timestamp(0) without time zone AS uint2) WITH FUNCTION datetime_uint2(timestamp(0) without time zone) AS ASSIGNMENT;

CREATE OR REPLACE FUNCTION pg_catalog.datetime_uint4 (timestamp(0) without time zone) RETURNS uint4 LANGUAGE SQL STABLE STRICT as 'select cast(pg_catalog.datetime_float($1) as uint4)';
CREATE CAST(timestamp(0) without time zone AS uint4) WITH FUNCTION datetime_uint4(timestamp(0) without time zone) AS ASSIGNMENT;

CREATE OR REPLACE FUNCTION pg_catalog.datetime_uint8 (timestamp(0) without time zone) RETURNS uint8 LANGUAGE SQL STABLE STRICT as 'select cast(pg_catalog.datetime_float($1) as uint8)';
CREATE CAST(timestamp(0) without time zone AS uint8) WITH FUNCTION datetime_uint8(timestamp(0) without time zone) AS ASSIGNMENT;

CREATE OR REPLACE FUNCTION pg_catalog.int8_b_format_timestamp (
uint1
) RETURNS timestamptz LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int8_b_format_timestamp';
CREATE CAST (uint1 AS timestamptz) WITH FUNCTION int8_b_format_timestamp(uint1) AS ASSIGNMENT;

CREATE OR REPLACE FUNCTION pg_catalog.int16_b_format_timestamp (
uint2
) RETURNS timestamptz LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int16_b_format_timestamp';
CREATE CAST (uint2 AS timestamptz) WITH FUNCTION int16_b_format_timestamp(uint2) AS ASSIGNMENT;


DROP FUNCTION IF EXISTS pg_catalog.timestamptz_uint1 ("timestamptz") CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.timestamptz_uint2 ("timestamptz") CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.timestamptz_uint4 ("timestamptz") CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.int8_year(uint1) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.int16_year(uint2) CASCADE;

DROP CAST IF EXISTS ("timestamptz" AS uint1) CASCADE;
DROP CAST IF EXISTS ("timestamptz" AS uint2) CASCADE;
DROP CAST IF EXISTS ("timestamptz" AS uint4) CASCADE;
DROP CAST IF EXISTS (uint1 AS year) CASCADE;
DROP CAST IF EXISTS (uint2 AS year) CASCADE;

CREATE OR REPLACE FUNCTION pg_catalog.timestamptz_uint1 ("timestamptz") RETURNS uint1 LANGUAGE SQL STABLE STRICT as 'select cast(pg_catalog.timestamptz_float8($1) as uint1)';
CREATE CAST("timestamptz" AS uint1) WITH FUNCTION timestamptz_uint1("timestamptz") AS ASSIGNMENT;

CREATE OR REPLACE FUNCTION pg_catalog.timestamptz_uint2 ("timestamptz") RETURNS uint2 LANGUAGE SQL STABLE STRICT as 'select cast(pg_catalog.timestamptz_float8($1) as uint2)';
CREATE CAST("timestamptz" AS uint2) WITH FUNCTION timestamptz_uint2("timestamptz") AS ASSIGNMENT;

CREATE OR REPLACE FUNCTION pg_catalog.timestamptz_uint4 ("timestamptz") RETURNS uint4 LANGUAGE SQL STABLE STRICT as 'select cast(pg_catalog.timestamptz_float8($1) as uint4)';
CREATE CAST("timestamptz" AS uint4) WITH FUNCTION timestamptz_uint4("timestamptz") AS ASSIGNMENT;

CREATE OR REPLACE FUNCTION pg_catalog.int8_year (
uint1
) RETURNS year LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int8_year';
CREATE CAST (uint1 AS year) WITH FUNCTION int8_year(uint1) AS ASSIGNMENT;

CREATE OR REPLACE FUNCTION pg_catalog.int16_year (
uint2
) RETURNS year LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int16_year';
CREATE CAST (uint2 AS year) WITH FUNCTION int16_year(uint2) AS ASSIGNMENT;


DROP FUNCTION IF EXISTS pg_catalog.int64_year(uint8) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.year_uint1(year) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.year_uint2(year) CASCADE;

DROP CAST IF EXISTS (uint8 AS year) CASCADE;
DROP CAST IF EXISTS (year AS uint1) CASCADE;
DROP CAST IF EXISTS (year AS uint2) CASCADE;

CREATE OR REPLACE FUNCTION pg_catalog.int64_year (
uint8
) RETURNS year LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int64_year';
CREATE CAST (uint8 AS year) WITH FUNCTION int64_year(uint8) AS ASSIGNMENT;

CREATE OR REPLACE FUNCTION pg_catalog.year_uint1 (
year
) RETURNS uint1 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'year_int8';
CREATE CAST (year AS uint1) WITH FUNCTION year_uint1(year) AS ASSIGNMENT;

CREATE OR REPLACE FUNCTION pg_catalog.year_uint2 (
year
) RETURNS uint2 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'year_int16';
CREATE CAST (year AS uint2) WITH FUNCTION year_uint2(year) AS ASSIGNMENT;


DROP FUNCTION IF EXISTS pg_catalog.uint1_json(uint1) cascade;
DROP FUNCTION IF EXISTS pg_catalog.uint2_json(uint2) cascade;
DROP FUNCTION IF EXISTS pg_catalog.uint4_json(uint4) cascade;
DROP FUNCTION IF EXISTS pg_catalog.uint8_json(uint8) cascade;

DROP CAST IF EXISTS (uint1 as json) cascade;
DROP CAST IF EXISTS (uint2 as json) cascade;
DROP CAST IF EXISTS (uint4 as json) cascade;
DROP CAST IF EXISTS (uint8 as json) cascade;

CREATE OR REPLACE FUNCTION pg_catalog.uint1_json(uint1) RETURNS json LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'cast_json';
CREATE CAST (uint1 as json) WITH FUNCTION pg_catalog.uint1_json(uint1);

CREATE OR REPLACE FUNCTION pg_catalog.uint2_json(uint2) RETURNS json LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'cast_json';
CREATE CAST (uint2 as json) WITH FUNCTION pg_catalog.uint2_json(uint2);

CREATE OR REPLACE FUNCTION pg_catalog.uint4_json(uint4) RETURNS json LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'cast_json';
CREATE CAST (uint4 as json) WITH FUNCTION pg_catalog.uint4_json(uint4);

CREATE OR REPLACE FUNCTION pg_catalog.uint8_json(uint8) RETURNS json LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'cast_json';
CREATE CAST (uint8 as json) WITH FUNCTION pg_catalog.uint8_json(uint8);


DROP FUNCTION IF EXISTS pg_catalog.bittouint1(bit) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.bittouint2(bit) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.bitfromuint1(uint1, int4) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.bitfromuint2(uint2, int4) CASCADE;

CREATE OR REPLACE FUNCTION pg_catalog.bittouint1 (
bit
) RETURNS uint1 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'bittouint1';
CREATE OR REPLACE FUNCTION pg_catalog.bittouint2 (
bit
) RETURNS uint2 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'bittouint2';

CREATE OR REPLACE FUNCTION pg_catalog.bitfromuint1 (
uint1, int4
) RETURNS bit LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'bitfromuint1';
CREATE OR REPLACE FUNCTION pg_catalog.bitfromuint2 (
uint2, int4
) RETURNS bit LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'bitfromuint2';

DROP CAST IF EXISTS (bit AS uint1) CASCADE;
DROP CAST IF EXISTS (bit AS uint2) CASCADE;
DROP CAST IF EXISTS (uint1 AS bit) CASCADE;
DROP CAST IF EXISTS (uint2 AS bit) CASCADE;

CREATE CAST (bit AS uint1) WITH FUNCTION bittouint1(bit) AS ASSIGNMENT;
CREATE CAST (bit AS uint2) WITH FUNCTION bittouint2(bit) AS ASSIGNMENT;
CREATE CAST (uint1 AS bit) WITH FUNCTION bitfromuint1(uint1, int4) AS ASSIGNMENT;
CREATE CAST (uint2 AS bit) WITH FUNCTION bitfromuint2(uint2, int4) AS ASSIGNMENT;


DROP FUNCTION IF EXISTS pg_catalog.bpchar_uint1 (char) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.bpchar_uint2 (char) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.bpchar_uint4 (char) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.bpchar_uint8 (char) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.varchar_uint1 (varchar) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.varchar_uint2 (varchar) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.varchar_uint4 (varchar) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.varchar_uint8 (varchar) CASCADE;

DROP CAST IF EXISTS (char AS uint1) CASCADE;
DROP CAST IF EXISTS (char AS uint2) CASCADE;
DROP CAST IF EXISTS (char AS uint4) CASCADE;
DROP CAST IF EXISTS (char AS uint8) CASCADE;
DROP CAST IF EXISTS (varchar AS uint1) CASCADE;
DROP CAST IF EXISTS (varchar AS uint2) CASCADE;
DROP CAST IF EXISTS (varchar AS uint4) CASCADE;
DROP CAST IF EXISTS (varchar AS uint8) CASCADE;

CREATE OR REPLACE FUNCTION pg_catalog.bpchar_uint1 (char) RETURNS uint1 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'bpchar_uint1';
CREATE CAST(char AS uint1) WITH FUNCTION bpchar_uint1(char) AS IMPLICIT;

CREATE OR REPLACE FUNCTION pg_catalog.bpchar_uint2 (char) RETURNS uint2 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'bpchar_uint2';
CREATE CAST(char AS uint2) WITH FUNCTION bpchar_uint2(char) AS IMPLICIT;

CREATE OR REPLACE FUNCTION pg_catalog.bpchar_uint4 (char) RETURNS uint4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'bpchar_uint4';
CREATE CAST(char AS uint4) WITH FUNCTION bpchar_uint4(char) AS IMPLICIT;

CREATE OR REPLACE FUNCTION pg_catalog.bpchar_uint8 (char) RETURNS uint8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'bpchar_uint8';
CREATE CAST(char AS uint8) WITH FUNCTION bpchar_uint8(char) AS IMPLICIT;

CREATE OR REPLACE FUNCTION pg_catalog.varchar_uint1 (varchar) RETURNS uint1 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'varchar_uint1';
CREATE CAST(varchar AS uint1) WITH FUNCTION varchar_uint1(varchar) AS IMPLICIT;

CREATE OR REPLACE FUNCTION pg_catalog.varchar_uint2 (varchar) RETURNS uint2 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'varchar_uint2';
CREATE CAST(varchar AS uint2) WITH FUNCTION varchar_uint2(varchar) AS IMPLICIT;

CREATE OR REPLACE FUNCTION pg_catalog.varchar_uint4 (varchar) RETURNS uint4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'varchar_uint4';
CREATE CAST(varchar AS uint4) WITH FUNCTION varchar_uint4(varchar) AS IMPLICIT;

CREATE OR REPLACE FUNCTION pg_catalog.varchar_uint8 (varchar) RETURNS uint8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'varchar_uint8';
CREATE CAST(varchar AS uint8) WITH FUNCTION varchar_uint8(varchar) AS IMPLICIT;


DROP FUNCTION IF EXISTS pg_catalog.settobit(anyset, int4) cascade;
DROP FUNCTION IF EXISTS pg_catalog.settoint1(anyset) cascade;
DROP FUNCTION IF EXISTS pg_catalog.settouint1(anyset) cascade;
DROP FUNCTION IF EXISTS pg_catalog.settouint2(anyset) cascade;
DROP FUNCTION IF EXISTS pg_catalog.settouint4(anyset) cascade;
DROP FUNCTION IF EXISTS pg_catalog.settouint8(anyset) cascade;

DROP CAST IF EXISTS (anyset AS bit) CASCADE;
DROP CAST IF EXISTS (anyset AS int1) CASCADE;
DROP CAST IF EXISTS (anyset AS uint1) CASCADE;
DROP CAST IF EXISTS (anyset AS uint2) CASCADE;
DROP CAST IF EXISTS (anyset AS uint4) CASCADE;
DROP CAST IF EXISTS (anyset AS uint8) CASCADE;

CREATE OR REPLACE FUNCTION pg_catalog.settobit(anyset, int4) 
RETURNS bit LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'settobit';
CREATE CAST (anyset AS bit) WITH FUNCTION settobit(anyset, int4) AS ASSIGNMENT;

CREATE OR REPLACE FUNCTION pg_catalog.settoint1(anyset) 
RETURNS int1 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'settoint1';
CREATE CAST (anyset AS int1) WITH FUNCTION settoint1(anyset) AS ASSIGNMENT;

CREATE OR REPLACE FUNCTION pg_catalog.settouint1(anyset) 
RETURNS uint1 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'settouint1';
CREATE CAST (anyset AS uint1) WITH FUNCTION settouint1(anyset) AS ASSIGNMENT;

CREATE OR REPLACE FUNCTION pg_catalog.settouint2(anyset) 
RETURNS uint2 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'settouint2';
CREATE CAST (anyset AS uint2) WITH FUNCTION settouint2(anyset) AS ASSIGNMENT;

CREATE OR REPLACE FUNCTION pg_catalog.settouint4(anyset) 
RETURNS uint4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'settouint4';
CREATE CAST (anyset AS uint4) WITH FUNCTION settouint4(anyset) AS ASSIGNMENT;

CREATE OR REPLACE FUNCTION pg_catalog.settouint8(anyset) 
RETURNS uint8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'settouint8';
CREATE CAST (anyset AS uint8) WITH FUNCTION settouint8(anyset) AS ASSIGNMENT;


DROP FUNCTION IF EXISTS pg_catalog.set(int1, int4) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.set(uint1, int4) CASCADE; 
DROP FUNCTION IF EXISTS pg_catalog.set(uint2, int4) CASCADE; 
DROP FUNCTION IF EXISTS pg_catalog.set(uint4, int4) CASCADE; 
DROP FUNCTION IF EXISTS pg_catalog.set(uint8, int4) CASCADE; 

DROP CAST IF EXISTS (int1 AS anyset) CASCADE;
DROP CAST IF EXISTS (uint1 AS anyset) CASCADE;
DROP CAST IF EXISTS (uint2 AS anyset) CASCADE;
DROP CAST IF EXISTS (uint4 AS anyset) CASCADE;
DROP CAST IF EXISTS (uint8 AS anyset) CASCADE;

CREATE OR REPLACE FUNCTION pg_catalog.set (int1, int4) RETURNS anyset LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'i1toset';
CREATE CAST (int1 AS anyset) WITH FUNCTION pg_catalog.set(int1, int4) AS ASSIGNMENT;

CREATE OR REPLACE FUNCTION pg_catalog.set (uint1, int4) RETURNS anyset LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'ui1toset';
CREATE CAST (uint1 AS anyset) WITH FUNCTION pg_catalog.set(uint1, int4) AS ASSIGNMENT;

CREATE OR REPLACE FUNCTION pg_catalog.set (uint2, int4) RETURNS anyset LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'ui2toset';
CREATE CAST (uint2 AS anyset) WITH FUNCTION pg_catalog.set(uint2, int4) AS ASSIGNMENT;

CREATE OR REPLACE FUNCTION pg_catalog.set (uint4, int4) RETURNS anyset LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'ui4toset';
CREATE CAST (uint4 AS anyset) WITH FUNCTION pg_catalog.set(uint4, int4) AS ASSIGNMENT;

CREATE OR REPLACE FUNCTION pg_catalog.set (uint8, int4) RETURNS anyset LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'ui8toset';
CREATE CAST (uint8 AS anyset) WITH FUNCTION pg_catalog.set(uint8, int4) AS ASSIGNMENT;


DROP FUNCTION IF EXISTS pg_catalog.uint1_enum(uint1, int4, anyelement) cascade;
DROP FUNCTION IF EXISTS pg_catalog.uint2_enum(uint2, int4, anyelement) cascade;
DROP FUNCTION IF EXISTS pg_catalog.uint4_enum(uint4, int4, anyelement) cascade;
DROP FUNCTION IF EXISTS pg_catalog.uint8_enum(uint8, int4, anyelement) cascade;

CREATE OR REPLACE FUNCTION pg_catalog.uint1_enum(uint1, int4, anyelement) RETURNS anyenum LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint1_enum';
CREATE OR REPLACE FUNCTION pg_catalog.uint2_enum(uint2, int4, anyelement) RETURNS anyenum LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint2_enum';
CREATE OR REPLACE FUNCTION pg_catalog.uint4_enum(uint4, int4, anyelement) RETURNS anyenum LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint4_enum';
CREATE OR REPLACE FUNCTION pg_catalog.uint8_enum(uint8, int4, anyelement) RETURNS anyenum LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint8_enum';


DROP FUNCTION IF EXISTS pg_catalog.enum_uint1(anyenum) cascade;
DROP FUNCTION IF EXISTS pg_catalog.enum_uint2(anyenum) cascade;
DROP FUNCTION IF EXISTS pg_catalog.enum_uint4(anyenum) cascade;
DROP FUNCTION IF EXISTS pg_catalog.enum_uint8(anyenum) cascade;

CREATE OR REPLACE FUNCTION pg_catalog.enum_uint1(anyenum) RETURNS uint1 LANGUAGE SQL IMMUTABLE STRICT as 'select cast(cast($1 as float8) as uint1)';
CREATE OR REPLACE FUNCTION pg_catalog.enum_uint2(anyenum) RETURNS uint2 LANGUAGE SQL IMMUTABLE STRICT as 'select cast(cast($1 as float8) as uint2)';
CREATE OR REPLACE FUNCTION pg_catalog.enum_uint4(anyenum) RETURNS uint4 LANGUAGE SQL IMMUTABLE STRICT as 'select cast(cast($1 as float8) as uint4)';
CREATE OR REPLACE FUNCTION pg_catalog.enum_uint8(anyenum) RETURNS uint8 LANGUAGE SQL IMMUTABLE STRICT as 'select cast(cast($1 as float8) as uint8)';


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

DROP FUNCTION IF EXISTS pg_catalog.timestamp_int8_xor(
    timestamp,
    uint8
) CASCADE;

DROP FUNCTION IF EXISTS pg_catalog.int8_timestamp_xor(
    uint8,
    timestamp
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


create function pg_catalog.date_int8_xor(
    date,
    uint8
) RETURNS int16 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'date_int8_xor';
create operator pg_catalog.^(leftarg = date, rightarg = uint8, procedure = pg_catalog.date_int8_xor);

create function pg_catalog.int8_date_xor(
    uint8,
    date
) RETURNS int16 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'int8_date_xor';
create operator pg_catalog.^(leftarg = uint8, rightarg = date, procedure = pg_catalog.int8_date_xor);

create function pg_catalog.time_int8_xor(
    time,
    uint8
) RETURNS int16 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'time_int8_xor';
create operator pg_catalog.^(leftarg = time, rightarg = uint8, procedure = pg_catalog.time_int8_xor);

create function pg_catalog.int8_time_xor(
    uint8,
    time
) RETURNS int16 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'int8_time_xor';
create operator pg_catalog.^(leftarg = uint8, rightarg = time, procedure = pg_catalog.int8_time_xor);

create function pg_catalog.timestamp_int8_xor(
    timestamp,
    uint8
) RETURNS int16 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'timestamp_int8_xor';
create operator pg_catalog.^(leftarg = timestamp, rightarg = uint8, procedure = pg_catalog.timestamp_int8_xor);

create function pg_catalog.int8_timestamp_xor(
    uint8,
    timestamp
) RETURNS int16 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'int8_timestamp_xor';
create operator pg_catalog.^(leftarg = uint8, rightarg = timestamp, procedure = pg_catalog.int8_timestamp_xor);

create function pg_catalog.timestamptz_int8_xor(
    timestampTz,
    uint8
) RETURNS int16 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'timestamptz_int8_xor';
create operator pg_catalog.^(leftarg = timestampTz, rightarg = uint8, procedure = pg_catalog.timestamptz_int8_xor);

create function pg_catalog.int8_timestamptz_xor(
    uint8,
    timestampTz
) RETURNS int16 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'int8_timestamptz_xor';
create operator pg_catalog.^(leftarg = uint8, rightarg = timestampTz, procedure = pg_catalog.int8_timestamptz_xor);

create function pg_catalog.date_uint1_eq(
    date,
    uint1
) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT as $$ SELECT pg_catalog.texteq($1::text, $2::text) $$;
create operator pg_catalog.=(leftarg = date, rightarg = uint1, procedure = pg_catalog.date_uint1_eq);

create function pg_catalog.date_uint2_eq(
    date,
    uint2
) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT as $$ SELECT pg_catalog.texteq($1::text, $2::text) $$;
create operator pg_catalog.=(leftarg = date, rightarg = uint2, procedure = pg_catalog.date_uint2_eq);

create function pg_catalog.time_uint1_eq(
    time,
    uint1
) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT as $$ SELECT pg_catalog.float8eq($1::float8, $2::float8) $$;
create operator pg_catalog.=(leftarg = time, rightarg = uint1, procedure = pg_catalog.time_uint1_eq);

create function pg_catalog.time_uint2_eq(
    time,
    uint2
) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT as $$ SELECT pg_catalog.float8eq($1::float8, $2::float8) $$;
create operator pg_catalog.=(leftarg = time, rightarg = uint2, procedure = pg_catalog.time_uint2_eq);

create function pg_catalog.date_uint1_ne(
    date,
    uint1
) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT as $$ SELECT pg_catalog.textne($1::text, $2::text) $$;
create operator pg_catalog.<>(leftarg = date, rightarg = uint1, procedure = pg_catalog.date_uint1_ne);

create function pg_catalog.date_uint2_ne(
    date,
    uint2
) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT as $$ SELECT pg_catalog.textne($1::text, $2::text) $$;
create operator pg_catalog.<>(leftarg = date, rightarg = uint2, procedure = pg_catalog.date_uint2_ne);

create function pg_catalog.time_uint1_ne(
    time,
    uint1
) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT as $$ SELECT pg_catalog.float8ne($1::float8, $2::float8) $$;
create operator pg_catalog.<>(leftarg = time, rightarg = uint1, procedure = pg_catalog.time_uint1_ne);

create function pg_catalog.time_uint2_ne(
    time,
    uint2
) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT as $$ SELECT pg_catalog.float8ne($1::float8, $2::float8) $$;
create operator pg_catalog.<>(leftarg = time, rightarg = uint2, procedure = pg_catalog.time_uint2_ne);

create function pg_catalog.uint1_date_ne(
    uint1,
    date
) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT as $$ SELECT pg_catalog.textne($1::text, $2::text) $$;
create operator pg_catalog.<>(leftarg = uint1, rightarg = date, procedure = pg_catalog.uint1_date_ne);

create function pg_catalog.uint2_date_ne(
    uint2,
    date
) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT as $$ SELECT pg_catalog.textne($1::text, $2::text) $$;
create operator pg_catalog.<>(leftarg = uint2, rightarg = date, procedure = pg_catalog.uint2_date_ne);

create function pg_catalog.uint1_time_ne(
    uint1,
    time
) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT as $$ SELECT pg_catalog.float8ne($1::text, $2::text) $$;
create operator pg_catalog.<>(leftarg = uint1, rightarg = time, procedure = pg_catalog.uint1_time_ne);

create function pg_catalog.uint2_time_ne(
    uint2,
    time
) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT as $$ SELECT pg_catalog.float8ne($1::text, $2::text) $$;
create operator pg_catalog.<>(leftarg = uint2, rightarg = time, procedure = pg_catalog.uint2_time_ne);

create function pg_catalog.date_uint1_lt(
    date,
    uint1
) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT as $$ SELECT pg_catalog.text_lt($1::text, $2::text) $$;
create operator pg_catalog.<(leftarg = date, rightarg = uint1, procedure = pg_catalog.date_uint1_lt);

create function pg_catalog.date_uint2_lt(
    date,
    uint2
) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT as $$ SELECT pg_catalog.text_lt($1::text, $2::text) $$;
create operator pg_catalog.<(leftarg = date, rightarg = uint2, procedure = pg_catalog.date_uint2_lt);

create function pg_catalog.time_uint1_lt(
    time,
    uint1
) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT as $$ SELECT pg_catalog.float8lt($1::float8, $2::float8) $$;
create operator pg_catalog.<(leftarg = time, rightarg = uint1, procedure = pg_catalog.time_uint1_lt);

create function pg_catalog.time_uint2_lt(
    time,
    uint2
) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT as $$ SELECT pg_catalog.float8lt($1::float8, $2::float8) $$;
create operator pg_catalog.<(leftarg = time, rightarg = uint2, procedure = pg_catalog.time_uint2_lt);

create function pg_catalog.date_uint1_gt(
    date,
    uint1
) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT as $$ SELECT pg_catalog.text_gt($1::text, $2::text) $$;
create operator pg_catalog.>(leftarg = date, rightarg = uint1, procedure = pg_catalog.date_uint1_gt);

create function pg_catalog.date_uint2_gt(
    date,
    uint2
) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT as $$ SELECT pg_catalog.text_gt($1::text, $2::text) $$;
create operator pg_catalog.>(leftarg = date, rightarg = uint2, procedure = pg_catalog.date_uint2_gt);

create function pg_catalog.time_uint1_gt(
    time,
    uint1
) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT as $$ SELECT pg_catalog.float8gt($1::float8, $2::float8) $$;
create operator pg_catalog.>(leftarg = time, rightarg = uint1, procedure = pg_catalog.time_uint1_gt);

create function pg_catalog.time_uint2_gt(
    time,
    uint2
) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT as $$ SELECT pg_catalog.float8gt($1::float8, $2::float8) $$;
create operator pg_catalog.>(leftarg = time, rightarg = uint2, procedure = pg_catalog.time_uint2_gt);

create function pg_catalog.date_uint1_le(
    date,
    uint1
) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT as $$ SELECT pg_catalog.text_le($1::text, $2::text) $$;
create operator pg_catalog.<=(leftarg = date, rightarg = uint1, procedure = pg_catalog.date_uint1_le);

create function pg_catalog.date_uint2_le(
    date,
    uint2
) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT as $$ SELECT pg_catalog.text_le($1::text, $2::text) $$;
create operator pg_catalog.<=(leftarg = date, rightarg = uint2, procedure = pg_catalog.date_uint2_le);

create function pg_catalog.time_uint1_le(
    time,
    uint1
) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT as $$ SELECT pg_catalog.float8le($1::float8, $2::float8) $$;
create operator pg_catalog.<=(leftarg = time, rightarg = uint1, procedure = pg_catalog.time_uint1_le);

create function pg_catalog.time_uint2_le(
    time,
    uint2
) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT as $$ SELECT pg_catalog.float8le($1::float8, $2::float8) $$;
create operator pg_catalog.<=(leftarg = time, rightarg = uint2, procedure = pg_catalog.time_uint2_le);

create function pg_catalog.date_uint1_ge(
    date,
    uint1
) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT as $$ SELECT pg_catalog.text_ge($1::text, $2::text) $$;
create operator pg_catalog.>=(leftarg = date, rightarg = uint1, procedure = pg_catalog.date_uint1_ge);

create function pg_catalog.date_uint2_ge(
    date,
    uint2
) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT as $$ SELECT pg_catalog.text_ge($1::text, $2::text) $$;
create operator pg_catalog.>=(leftarg = date, rightarg = uint2, procedure = pg_catalog.date_uint2_ge);

create function pg_catalog.time_uint1_ge(
    time,
    uint1
) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT as $$ SELECT pg_catalog.float8ge($1::float8, $2::float8) $$;
create operator pg_catalog.>=(leftarg = time, rightarg = uint1, procedure = pg_catalog.time_uint1_ge);

create function pg_catalog.time_uint2_ge(
    time,
    uint2
) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT as $$ SELECT pg_catalog.float8ge($1::float8, $2::float8) $$;
create operator pg_catalog.>=(leftarg = time, rightarg = uint2, procedure = pg_catalog.time_uint2_ge);

create function pg_catalog.int8_year_mod(
    int8,
    year
) RETURNS uint4 LANGUAGE SQL IMMUTABLE STRICT as $$ SELECT pg_catalog.uint4_int4_mod($1::uint4, $2::int4) $$;
create operator pg_catalog.%(leftarg = int8, rightarg = year, procedure = pg_catalog.int8_year_mod);

create function pg_catalog.int8_or_nvarchar2(
    int8,
    nvarchar2
) RETURNS uint4 LANGUAGE SQL IMMUTABLE STRICT as $$ SELECT pg_catalog.uint4_or_int4($1::uint4, $2::int4) $$;
create operator pg_catalog.|(leftarg = int8, rightarg = nvarchar2, procedure = pg_catalog.int8_or_nvarchar2);

create function pg_catalog.int8_or_year(
    int8,
    year
) RETURNS uint4 LANGUAGE SQL IMMUTABLE STRICT as $$ SELECT pg_catalog.uint4_or_int4($1::uint4, $2::int4) $$;
create operator pg_catalog.|(leftarg = int8, rightarg = year, procedure = pg_catalog.int8_or_year);

create function pg_catalog.nvarchar2_or_int8(
    nvarchar2,
    int8
) RETURNS uint4 LANGUAGE SQL IMMUTABLE STRICT as $$ SELECT pg_catalog.int4_or_uint4($1::int4, $2::uint4) $$;
create operator pg_catalog.|(leftarg = nvarchar2, rightarg = int8, procedure = pg_catalog.nvarchar2_or_int8);

create function pg_catalog.year_or_int8(
    year,
    int8
) RETURNS uint4 LANGUAGE SQL IMMUTABLE STRICT as $$ SELECT pg_catalog.int4_or_uint4($1::int4, $2::uint4) $$;
create operator pg_catalog.|(leftarg = year, rightarg = int8, procedure = pg_catalog.year_or_int8);

create function pg_catalog.int8_xor_nvarchar2(
    int8,
    nvarchar2
) RETURNS uint4 LANGUAGE SQL IMMUTABLE STRICT as $$ SELECT pg_catalog.uint4_or_int4($1::uint4, $2::int4) $$;
create operator pg_catalog.#(leftarg = int8, rightarg = nvarchar2, procedure = pg_catalog.int8_xor_nvarchar2);

create function pg_catalog.int8_xor_year(
    int8,
    year
) RETURNS uint4 LANGUAGE SQL IMMUTABLE STRICT as $$ SELECT pg_catalog.uint4_xor_int4($1::uint4, $2::int4) $$;
create operator pg_catalog.#(leftarg = int8, rightarg = year, procedure = pg_catalog.int8_xor_year);
create operator pg_catalog.^(leftarg = int8, rightarg = year, procedure = pg_catalog.int8_xor_year);

create function pg_catalog.nvarchar2_xor_int8(
    nvarchar2,
    int8
) RETURNS uint4 LANGUAGE SQL IMMUTABLE STRICT as $$ SELECT pg_catalog.int4_xor_uint4($1::int4, $2::uint4) $$;
create operator pg_catalog.#(leftarg = nvarchar2, rightarg = int8, procedure = pg_catalog.nvarchar2_xor_int8);

create function pg_catalog.year_xor_int8(
    year,
    int8
) RETURNS uint4 LANGUAGE SQL IMMUTABLE STRICT as $$ SELECT pg_catalog.int4_xor_uint4($1::int4, $2::uint4) $$;
create operator pg_catalog.#(leftarg = year, rightarg = int8, procedure = pg_catalog.year_xor_int8);
create operator pg_catalog.^(leftarg = year, rightarg = int8, procedure = pg_catalog.year_xor_int8);

create function pg_catalog.int8_and_nvarchar2(
    int8,
    nvarchar2
) RETURNS uint4 LANGUAGE SQL IMMUTABLE STRICT as $$ SELECT pg_catalog.uint4_and_int4($1::uint4, $2::int4) $$;
create operator pg_catalog.&(leftarg = int8, rightarg = nvarchar2, procedure = pg_catalog.int8_and_nvarchar2);

create function pg_catalog.int8_and_year(
    int8,
    year
) RETURNS uint4 LANGUAGE SQL IMMUTABLE STRICT as $$ SELECT pg_catalog.uint4_and_int4($1::uint4, $2::int4) $$;
create operator pg_catalog.&(leftarg = int8, rightarg = year, procedure = pg_catalog.int8_and_year);

create function pg_catalog.nvarchar2_and_int8(
    nvarchar2,
    int8
) RETURNS uint4 LANGUAGE SQL IMMUTABLE STRICT as $$ SELECT pg_catalog.int4_and_uint4($1::int4, $2::uint4) $$;
create operator pg_catalog.&(leftarg = nvarchar2, rightarg = int8, procedure = pg_catalog.nvarchar2_and_int8);

create function pg_catalog.year_and_int8(
    year,
    int8
) RETURNS uint4 LANGUAGE SQL IMMUTABLE STRICT as $$ SELECT pg_catalog.int4_and_uint4($1::int4, $2::uint4) $$;
create operator pg_catalog.&(leftarg = year, rightarg = int8, procedure = pg_catalog.year_and_int8);

create function pg_catalog.uint4_mod_year(
    uint4,
    year
) RETURNS uint4 LANGUAGE SQL IMMUTABLE STRICT as $$ SELECT pg_catalog.uint4_int4_mod($1::uint4, $2::int4) $$;
create operator pg_catalog.%(leftarg = uint4, rightarg = year, procedure = pg_catalog.uint4_mod_year);

create function pg_catalog.nvarchar2_or_uint4(
    nvarchar2,
    uint4
) RETURNS uint4 LANGUAGE SQL IMMUTABLE STRICT as $$ SELECT pg_catalog.int4_or_uint4($1::int4, $2::uint4) $$;
create operator pg_catalog.|(leftarg = nvarchar2, rightarg = uint4, procedure = pg_catalog.nvarchar2_or_uint4);

create function pg_catalog.uint4_or_nvarchar2(
    uint4,
    nvarchar2
) RETURNS uint4 LANGUAGE SQL IMMUTABLE STRICT as $$ SELECT pg_catalog.uint4_or_int4($1::uint4, $2::int4) $$;
create operator pg_catalog.|(leftarg = uint4, rightarg = nvarchar2, procedure = pg_catalog.uint4_or_nvarchar2);

create function pg_catalog.year_or_uint4(
    year,
    uint4
) RETURNS uint4 LANGUAGE SQL IMMUTABLE STRICT as $$ SELECT pg_catalog.int4_or_uint4($1::int4, $2::uint4) $$;
create operator pg_catalog.|(leftarg = year, rightarg = uint4, procedure = pg_catalog.year_or_uint4);

create function pg_catalog.uint4_or_year(
    uint4,
    year
) RETURNS uint4 LANGUAGE SQL IMMUTABLE STRICT as $$ SELECT pg_catalog.uint4_or_int4($1::uint4, $2::int4) $$;
create operator pg_catalog.|(leftarg = uint4, rightarg = year, procedure = pg_catalog.uint4_or_year);

create function pg_catalog.year_xor_uint4(
    year,
    uint4
) RETURNS uint4 LANGUAGE SQL IMMUTABLE STRICT as $$ SELECT pg_catalog.int4_xor_uint4($1::int4, $2::uint4) $$;
create operator pg_catalog.^(leftarg = year, rightarg = uint4, procedure = pg_catalog.year_xor_uint4);

create function pg_catalog.uint4_xor_year(
    uint4,
    year
) RETURNS uint4 LANGUAGE SQL IMMUTABLE STRICT as $$ SELECT pg_catalog.uint4_xor_int4($1::uint4, $2::int4) $$;
create operator pg_catalog.^(leftarg = uint4, rightarg = year, procedure = pg_catalog.uint4_xor_year);

create function pg_catalog.nvarchar2_and_uint4(
    nvarchar2,
    uint4
) RETURNS uint4 LANGUAGE SQL IMMUTABLE STRICT as $$ SELECT pg_catalog.int4_and_uint4($1::int4, $2::uint4) $$;
create operator pg_catalog.&(leftarg = nvarchar2, rightarg = uint4, procedure = pg_catalog.nvarchar2_and_uint4);

create function pg_catalog.uint4_and_nvarchar2(
    uint4,
    nvarchar2
) RETURNS uint4 LANGUAGE SQL IMMUTABLE STRICT as $$ SELECT pg_catalog.uint4_and_int4($1::uint4, $2::int4) $$;
create operator pg_catalog.&(leftarg = uint4, rightarg = nvarchar2, procedure = pg_catalog.uint4_and_nvarchar2);

create function pg_catalog.year_and_uint4(
    year,
    uint4
) RETURNS uint4 LANGUAGE SQL IMMUTABLE STRICT as $$ SELECT pg_catalog.int4_and_uint4($1::int4, $2::uint4) $$;
create operator pg_catalog.&(leftarg = year, rightarg = uint4, procedure = pg_catalog.year_and_uint4);

create function pg_catalog.uint4_and_year(
    uint4,
    year
) RETURNS uint4 LANGUAGE SQL IMMUTABLE STRICT as $$ SELECT pg_catalog.uint4_and_int4($1::uint4, $2::int4) $$;
create operator pg_catalog.&(leftarg = uint4, rightarg = year, procedure = pg_catalog.uint4_and_year);


--dolphin_binary_functions.sql

--DROP FUNCTION IF EXISTS pg_catalog.varbinary_in (cstring);
CREATE OR REPLACE FUNCTION pg_catalog.varbinary_in (
cstring
) RETURNS varbinary LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'dolphin_binaryin';

--DROP FUNCTION IF EXISTS pg_catalog.bit2float4(bit);
CREATE OR REPLACE FUNCTION pg_catalog.bit2float4 (bit) RETURNS float4 AS
$$
BEGIN
    RETURN (SELECT bittouint8($1));
END;
$$
LANGUAGE plpgsql;

--DROP FUNCTION IF EXISTS pg_catalog.bit2float8(bit);
CREATE OR REPLACE FUNCTION pg_catalog.bit2float8 (bit) RETURNS float8 AS
$$
BEGIN
    RETURN (SELECT bittouint8($1));
END;
$$
LANGUAGE plpgsql;

--DROP FUNCTION IF EXISTS pg_catalog.bit_longblob(uint8,longblob);
CREATE OR REPLACE FUNCTION pg_catalog.bit_longblob (t1 uint8, t2 longblob) RETURNS uint8  AS
$$
DECLARE num NUMBER := to_number(cast(t2 as text));
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

DROP CAST IF EXISTS (json AS float8) CASCADE;
CREATE CAST (json AS float8) WITH FUNCTION pg_catalog.varlena2float8(anyelement) AS ASSIGNMENT;


DROP FUNCTION IF EXISTS pg_catalog.varlena2int1(anyelement) cascade;
DROP FUNCTION IF EXISTS pg_catalog.varlena2int2(anyelement) cascade;
DROP FUNCTION IF EXISTS pg_catalog.varlena2int4(anyelement) cascade;
DROP FUNCTION IF EXISTS pg_catalog.varlena2int8(anyelement) cascade;
DROP FUNCTION IF EXISTS pg_catalog.varlena2float4(anyelement) cascade;
DROP FUNCTION IF EXISTS pg_catalog.varlena2numeric(anyelement) cascade;
DROP FUNCTION IF EXISTS pg_catalog.varlena2uint1(anyelement) cascade;
DROP FUNCTION IF EXISTS pg_catalog.varlena2uint2(anyelement) cascade;
DROP FUNCTION IF EXISTS pg_catalog.varlena2uint4(anyelement) cascade;
DROP FUNCTION IF EXISTS pg_catalog.varlena2uint8(anyelement) cascade;
DROP FUNCTION IF EXISTS pg_catalog.Varlena2Bpchar(anyelement) cascade;
DROP FUNCTION IF EXISTS pg_catalog.Varlena2Varchar(anyelement) cascade;
DROP FUNCTION IF EXISTS pg_catalog.Varlena2Text(anyelement) cascade;
DROP FUNCTION IF EXISTS pg_catalog.Varlena2Date(anyelement) cascade;
DROP FUNCTION IF EXISTS pg_catalog.Varlena2Timestamptz(anyelement) cascade;
DROP FUNCTION IF EXISTS pg_catalog.Varlena2Datetime(anyelement) cascade;
DROP FUNCTION IF EXISTS pg_catalog.Varlena2Time(anyelement) cascade;
DROP FUNCTION IF EXISTS pg_catalog.Varlena2Year(anyelement) cascade;
DROP FUNCTION IF EXISTS pg_catalog.Varlena2Bit(anyelement, int4) cascade;
DROP FUNCTION IF EXISTS pg_catalog.varlenatoset(anyelement, int4) cascade;

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

CREATE OR REPLACE FUNCTION pg_catalog.varlena2int1(anyelement) RETURNS int1 LANGUAGE SQL IMMUTABLE STRICT as 'select cast(cast($1 as float8) as int1)';

CREATE CAST ("binary" AS int1) WITH FUNCTION pg_catalog.varlena2int1(anyelement) AS ASSIGNMENT;
CREATE CAST ("varbinary" AS int1) WITH FUNCTION pg_catalog.varlena2int1(anyelement) AS ASSIGNMENT;
CREATE CAST (blob AS int1) WITH FUNCTION pg_catalog.varlena2int1(anyelement) AS ASSIGNMENT;
CREATE CAST (tinyblob AS int1) WITH FUNCTION pg_catalog.varlena2int1(anyelement) AS ASSIGNMENT;
CREATE CAST (mediumblob AS int1) WITH FUNCTION pg_catalog.varlena2int1(anyelement) AS ASSIGNMENT;
CREATE CAST (longblob AS int1) WITH FUNCTION pg_catalog.varlena2int1(anyelement) AS ASSIGNMENT;
CREATE CAST (json AS int1) WITH FUNCTION pg_catalog.varlena2int1(anyelement) AS ASSIGNMENT;


CREATE OR REPLACE FUNCTION pg_catalog.varlena2int2(anyelement) RETURNS int2 LANGUAGE SQL IMMUTABLE STRICT as 'select cast(cast($1 as float8) as int2)';

CREATE CAST ("binary" AS int2) WITH FUNCTION pg_catalog.varlena2int2(anyelement) AS ASSIGNMENT;
CREATE CAST ("varbinary" AS int2) WITH FUNCTION pg_catalog.varlena2int2(anyelement) AS ASSIGNMENT;
CREATE CAST (blob AS int2) WITH FUNCTION pg_catalog.varlena2int2(anyelement) AS ASSIGNMENT;
CREATE CAST (tinyblob AS int2) WITH FUNCTION pg_catalog.varlena2int2(anyelement) AS ASSIGNMENT;
CREATE CAST (mediumblob AS int2) WITH FUNCTION pg_catalog.varlena2int2(anyelement) AS ASSIGNMENT;
CREATE CAST (longblob AS int2) WITH FUNCTION pg_catalog.varlena2int2(anyelement) AS ASSIGNMENT;
CREATE CAST (json AS int2) WITH FUNCTION pg_catalog.varlena2int2(anyelement) AS ASSIGNMENT;


CREATE OR REPLACE FUNCTION pg_catalog.varlena2int4(anyelement) RETURNS int4 LANGUAGE SQL IMMUTABLE STRICT as 'select cast(cast($1 as float8) as int4)';

CREATE CAST ("binary" AS int4) WITH FUNCTION pg_catalog.varlena2int4(anyelement) AS ASSIGNMENT;
CREATE CAST ("varbinary" AS int4) WITH FUNCTION pg_catalog.varlena2int4(anyelement) AS ASSIGNMENT;
CREATE CAST (blob AS int4) WITH FUNCTION pg_catalog.varlena2int4(anyelement) AS ASSIGNMENT;
CREATE CAST (tinyblob AS int4) WITH FUNCTION pg_catalog.varlena2int4(anyelement) AS ASSIGNMENT;
CREATE CAST (mediumblob AS int4) WITH FUNCTION pg_catalog.varlena2int4(anyelement) AS ASSIGNMENT;
CREATE CAST (longblob AS int4) WITH FUNCTION pg_catalog.varlena2int4(anyelement) AS ASSIGNMENT;
CREATE CAST (json AS int4) WITH FUNCTION pg_catalog.varlena2int4(anyelement) AS ASSIGNMENT;


CREATE OR REPLACE FUNCTION pg_catalog.varlena2int8(anyelement) RETURNS int8 LANGUAGE SQL IMMUTABLE STRICT as 'select cast(cast($1 as float8) as int8)';

CREATE CAST ("binary" AS int8) WITH FUNCTION pg_catalog.varlena2int8(anyelement) AS ASSIGNMENT;
CREATE CAST ("varbinary" AS int8) WITH FUNCTION pg_catalog.varlena2int8(anyelement) AS ASSIGNMENT;
CREATE CAST (blob AS int8) WITH FUNCTION pg_catalog.varlena2int8(anyelement) AS ASSIGNMENT;
CREATE CAST (tinyblob AS int8) WITH FUNCTION pg_catalog.varlena2int8(anyelement) AS ASSIGNMENT;
CREATE CAST (mediumblob AS int8) WITH FUNCTION pg_catalog.varlena2int8(anyelement) AS ASSIGNMENT;
CREATE CAST (longblob AS int8) WITH FUNCTION pg_catalog.varlena2int8(anyelement) AS ASSIGNMENT;
CREATE CAST (json AS int8) WITH FUNCTION pg_catalog.varlena2int8(anyelement) AS ASSIGNMENT;


CREATE OR REPLACE FUNCTION pg_catalog.varlena2float4(anyelement) RETURNS float4 LANGUAGE SQL IMMUTABLE STRICT as 'select cast(cast($1 as float8) as float4)';

CREATE CAST ("binary" AS float4) WITH FUNCTION pg_catalog.varlena2float4(anyelement) AS ASSIGNMENT;
CREATE CAST ("varbinary" AS float4) WITH FUNCTION pg_catalog.varlena2float4(anyelement) AS ASSIGNMENT;
CREATE CAST (blob AS float4) WITH FUNCTION pg_catalog.varlena2float4(anyelement) AS ASSIGNMENT;
CREATE CAST (tinyblob AS float4) WITH FUNCTION pg_catalog.varlena2float4(anyelement) AS ASSIGNMENT;
CREATE CAST (mediumblob AS float4) WITH FUNCTION pg_catalog.varlena2float4(anyelement) AS ASSIGNMENT;
CREATE CAST (longblob AS float4) WITH FUNCTION pg_catalog.varlena2float4(anyelement) AS ASSIGNMENT;
CREATE CAST (json AS float4) WITH FUNCTION pg_catalog.varlena2float4(anyelement) AS ASSIGNMENT;


CREATE OR REPLACE FUNCTION pg_catalog.varlena2numeric(anyelement) RETURNS numeric LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'Varlena2Numeric';

CREATE CAST ("binary" AS numeric) WITH FUNCTION pg_catalog.varlena2numeric(anyelement) AS ASSIGNMENT;
CREATE CAST ("varbinary" AS numeric) WITH FUNCTION pg_catalog.varlena2numeric(anyelement) AS ASSIGNMENT;
CREATE CAST (blob AS numeric) WITH FUNCTION pg_catalog.varlena2numeric(anyelement) AS ASSIGNMENT;
CREATE CAST (tinyblob AS numeric) WITH FUNCTION pg_catalog.varlena2numeric(anyelement) AS ASSIGNMENT;
CREATE CAST (mediumblob AS numeric) WITH FUNCTION pg_catalog.varlena2numeric(anyelement) AS ASSIGNMENT;
CREATE CAST (longblob AS numeric) WITH FUNCTION pg_catalog.varlena2numeric(anyelement) AS ASSIGNMENT;
CREATE CAST (json AS numeric) WITH FUNCTION pg_catalog.varlena2numeric(anyelement) AS ASSIGNMENT;


CREATE OR REPLACE FUNCTION pg_catalog.varlena2uint1(anyelement) RETURNS uint1 LANGUAGE SQL IMMUTABLE STRICT as 'select cast(cast($1 as float8) as uint1)';

CREATE CAST ("binary" AS uint1) WITH FUNCTION pg_catalog.varlena2uint1(anyelement) AS ASSIGNMENT;
CREATE CAST ("varbinary" AS uint1) WITH FUNCTION pg_catalog.varlena2uint1(anyelement) AS ASSIGNMENT;
CREATE CAST (blob AS uint1) WITH FUNCTION pg_catalog.varlena2uint1(anyelement) AS ASSIGNMENT;
CREATE CAST (tinyblob AS uint1) WITH FUNCTION pg_catalog.varlena2uint1(anyelement) AS ASSIGNMENT;
CREATE CAST (mediumblob AS uint1) WITH FUNCTION pg_catalog.varlena2uint1(anyelement) AS ASSIGNMENT;
CREATE CAST (longblob AS uint1) WITH FUNCTION pg_catalog.varlena2uint1(anyelement) AS ASSIGNMENT;
CREATE CAST (json AS uint1) WITH FUNCTION pg_catalog.varlena2uint1(anyelement) AS ASSIGNMENT;


CREATE OR REPLACE FUNCTION pg_catalog.varlena2uint2(anyelement) RETURNS uint2 LANGUAGE SQL IMMUTABLE STRICT as 'select cast(cast($1 as float8) as uint2)';

CREATE CAST ("binary" AS uint2) WITH FUNCTION pg_catalog.varlena2uint2(anyelement) AS ASSIGNMENT;
CREATE CAST ("varbinary" AS uint2) WITH FUNCTION pg_catalog.varlena2uint2(anyelement) AS ASSIGNMENT;
CREATE CAST (blob AS uint2) WITH FUNCTION pg_catalog.varlena2uint2(anyelement) AS ASSIGNMENT;
CREATE CAST (tinyblob AS uint2) WITH FUNCTION pg_catalog.varlena2uint2(anyelement) AS ASSIGNMENT;
CREATE CAST (mediumblob AS uint2) WITH FUNCTION pg_catalog.varlena2uint2(anyelement) AS ASSIGNMENT;
CREATE CAST (longblob AS uint2) WITH FUNCTION pg_catalog.varlena2uint2(anyelement) AS ASSIGNMENT;
CREATE CAST (json AS uint2) WITH FUNCTION pg_catalog.varlena2uint2(anyelement) AS ASSIGNMENT;


CREATE OR REPLACE FUNCTION pg_catalog.varlena2uint4(anyelement) RETURNS uint4 LANGUAGE SQL IMMUTABLE STRICT as 'select cast(cast($1 as float8) as uint4)';

CREATE CAST ("binary" AS uint4) WITH FUNCTION pg_catalog.varlena2uint4(anyelement) AS ASSIGNMENT;
CREATE CAST ("varbinary" AS uint4) WITH FUNCTION pg_catalog.varlena2uint4(anyelement) AS ASSIGNMENT;
CREATE CAST (blob AS uint4) WITH FUNCTION pg_catalog.varlena2uint4(anyelement) AS ASSIGNMENT;
CREATE CAST (tinyblob AS uint4) WITH FUNCTION pg_catalog.varlena2uint4(anyelement) AS ASSIGNMENT;
CREATE CAST (mediumblob AS uint4) WITH FUNCTION pg_catalog.varlena2uint4(anyelement) AS ASSIGNMENT;
CREATE CAST (longblob AS uint4) WITH FUNCTION pg_catalog.varlena2uint4(anyelement) AS ASSIGNMENT;
CREATE CAST (json AS uint4) WITH FUNCTION pg_catalog.varlena2uint4(anyelement) AS ASSIGNMENT;


CREATE OR REPLACE FUNCTION pg_catalog.varlena2uint8(anyelement) RETURNS uint8 LANGUAGE SQL IMMUTABLE STRICT as 'select cast(cast($1 as float8) as uint8)';

CREATE CAST ("binary" AS uint8) WITH FUNCTION pg_catalog.varlena2uint8(anyelement) AS ASSIGNMENT;
CREATE CAST ("varbinary" AS uint8) WITH FUNCTION pg_catalog.varlena2uint8(anyelement) AS ASSIGNMENT;
CREATE CAST (blob AS uint8) WITH FUNCTION pg_catalog.varlena2uint8(anyelement) AS ASSIGNMENT;
CREATE CAST (tinyblob AS uint8) WITH FUNCTION pg_catalog.varlena2uint8(anyelement) AS ASSIGNMENT;
CREATE CAST (mediumblob AS uint8) WITH FUNCTION pg_catalog.varlena2uint8(anyelement) AS ASSIGNMENT;
CREATE CAST (longblob AS uint8) WITH FUNCTION pg_catalog.varlena2uint8(anyelement) AS ASSIGNMENT;
CREATE CAST (json AS uint8) WITH FUNCTION pg_catalog.varlena2uint8(anyelement) AS ASSIGNMENT;


CREATE OR REPLACE FUNCTION pg_catalog.Varlena2Bpchar(anyelement) RETURNS char LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'Varlena2Bpchar';

CREATE CAST ("binary" AS char) WITH FUNCTION pg_catalog.Varlena2Bpchar(anyelement) AS ASSIGNMENT;
CREATE CAST ("varbinary" AS char) WITH FUNCTION pg_catalog.Varlena2Bpchar(anyelement) AS ASSIGNMENT;
CREATE CAST (blob AS char) WITH FUNCTION pg_catalog.Varlena2Bpchar(anyelement) AS ASSIGNMENT;
CREATE CAST (tinyblob AS char) WITH FUNCTION pg_catalog.Varlena2Bpchar(anyelement) AS ASSIGNMENT;
CREATE CAST (mediumblob AS char) WITH FUNCTION pg_catalog.Varlena2Bpchar(anyelement) AS ASSIGNMENT;
CREATE CAST (longblob AS char) WITH FUNCTION pg_catalog.Varlena2Bpchar(anyelement) AS ASSIGNMENT;
CREATE CAST (json AS char) WITH FUNCTION pg_catalog.Varlena2Bpchar(anyelement) AS ASSIGNMENT;


CREATE OR REPLACE FUNCTION pg_catalog.Varlena2Varchar(anyelement) RETURNS varchar LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'Varlena2Varchar';

CREATE CAST ("binary" AS varchar) WITH FUNCTION pg_catalog.Varlena2Varchar(anyelement) AS ASSIGNMENT;
CREATE CAST ("varbinary" AS varchar) WITH FUNCTION pg_catalog.Varlena2Varchar(anyelement) AS ASSIGNMENT;
CREATE CAST (blob AS varchar) WITH FUNCTION pg_catalog.Varlena2Varchar(anyelement) AS ASSIGNMENT;
CREATE CAST (tinyblob AS varchar) WITH FUNCTION pg_catalog.Varlena2Varchar(anyelement) AS ASSIGNMENT;
CREATE CAST (mediumblob AS varchar) WITH FUNCTION pg_catalog.Varlena2Varchar(anyelement) AS ASSIGNMENT;
CREATE CAST (longblob AS varchar) WITH FUNCTION pg_catalog.Varlena2Varchar(anyelement) AS ASSIGNMENT;
CREATE CAST (json AS varchar) WITH FUNCTION pg_catalog.Varlena2Varchar(anyelement) AS ASSIGNMENT;


CREATE OR REPLACE FUNCTION pg_catalog.Varlena2Text(anyelement) RETURNS text LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'Varlena2Text';

DROP CAST IF EXISTS (raw AS text);
CREATE CAST ("binary" AS text) WITH FUNCTION pg_catalog.Varlena2Text(anyelement) AS ASSIGNMENT;
CREATE CAST ("varbinary" AS text) WITH FUNCTION pg_catalog.Varlena2Text(anyelement) AS ASSIGNMENT;
CREATE CAST (raw AS text) WITH FUNCTION pg_catalog.Varlena2Text(anyelement) AS IMPLICIT;
CREATE CAST (blob AS text) WITH FUNCTION pg_catalog.Varlena2Text(anyelement) AS ASSIGNMENT;
CREATE CAST (tinyblob AS text) WITH FUNCTION pg_catalog.Varlena2Text(anyelement) AS ASSIGNMENT;
CREATE CAST (mediumblob AS text) WITH FUNCTION pg_catalog.Varlena2Text(anyelement) AS ASSIGNMENT;
CREATE CAST (longblob AS text) WITH FUNCTION pg_catalog.Varlena2Text(anyelement) AS ASSIGNMENT;
CREATE CAST (json AS text) WITH FUNCTION pg_catalog.Varlena2Text(anyelement) AS ASSIGNMENT;


CREATE OR REPLACE FUNCTION pg_catalog.Varlena2Date(anyelement) RETURNS date LANGUAGE SQL IMMUTABLE STRICT as 'select cast(cast($1 as text) as date)';

CREATE CAST ("binary" AS date) WITH FUNCTION pg_catalog.Varlena2Date(anyelement) AS ASSIGNMENT;
CREATE CAST ("varbinary" AS date) WITH FUNCTION pg_catalog.Varlena2Date(anyelement) AS ASSIGNMENT;
CREATE CAST (blob AS date) WITH FUNCTION pg_catalog.Varlena2Date(anyelement) AS ASSIGNMENT;
CREATE CAST (tinyblob AS date) WITH FUNCTION pg_catalog.Varlena2Date(anyelement) AS ASSIGNMENT;
CREATE CAST (mediumblob AS date) WITH FUNCTION pg_catalog.Varlena2Date(anyelement) AS ASSIGNMENT;
CREATE CAST (longblob AS date) WITH FUNCTION pg_catalog.Varlena2Date(anyelement) AS ASSIGNMENT;
CREATE CAST (json AS date) WITH FUNCTION pg_catalog.Varlena2Date(anyelement) AS ASSIGNMENT;


CREATE OR REPLACE FUNCTION pg_catalog.Varlena2Timestamptz(anyelement) RETURNS timestamptz LANGUAGE SQL IMMUTABLE STRICT as 
'select cast(cast($1 as text) as timestamptz)';

CREATE CAST ("binary" AS timestamptz) WITH FUNCTION pg_catalog.Varlena2Timestamptz(anyelement) AS ASSIGNMENT;
CREATE CAST ("varbinary" AS timestamptz) WITH FUNCTION pg_catalog.Varlena2Timestamptz(anyelement) AS ASSIGNMENT;
CREATE CAST (blob AS timestamptz) WITH FUNCTION pg_catalog.Varlena2Timestamptz(anyelement) AS ASSIGNMENT;
CREATE CAST (tinyblob AS timestamptz) WITH FUNCTION pg_catalog.Varlena2Timestamptz(anyelement) AS ASSIGNMENT;
CREATE CAST (mediumblob AS timestamptz) WITH FUNCTION pg_catalog.Varlena2Timestamptz(anyelement) AS ASSIGNMENT;
CREATE CAST (longblob AS timestamptz) WITH FUNCTION pg_catalog.Varlena2Timestamptz(anyelement) AS ASSIGNMENT;
CREATE CAST (json AS timestamptz) WITH FUNCTION pg_catalog.Varlena2Timestamptz(anyelement) AS ASSIGNMENT;


CREATE OR REPLACE FUNCTION pg_catalog.Varlena2Datetime(anyelement) RETURNS timestamp without time zone LANGUAGE SQL IMMUTABLE STRICT as 
'select cast(cast($1 as text) as timestamp without time zone)';

CREATE CAST ("binary" AS timestamp without time zone) WITH FUNCTION pg_catalog.Varlena2Datetime(anyelement) AS ASSIGNMENT;
CREATE CAST ("varbinary" AS timestamp without time zone) WITH FUNCTION pg_catalog.Varlena2Datetime(anyelement) AS ASSIGNMENT;
CREATE CAST (blob AS timestamp without time zone) WITH FUNCTION pg_catalog.Varlena2Datetime(anyelement) AS ASSIGNMENT;
CREATE CAST (tinyblob AS timestamp without time zone) WITH FUNCTION pg_catalog.Varlena2Datetime(anyelement) AS ASSIGNMENT;
CREATE CAST (mediumblob AS timestamp without time zone) WITH FUNCTION pg_catalog.Varlena2Datetime(anyelement) AS ASSIGNMENT;
CREATE CAST (longblob AS timestamp without time zone) WITH FUNCTION pg_catalog.Varlena2Datetime(anyelement) AS ASSIGNMENT;
CREATE CAST (json AS timestamp without time zone) WITH FUNCTION pg_catalog.Varlena2Datetime(anyelement);


CREATE OR REPLACE FUNCTION pg_catalog.Varlena2Time(anyelement) RETURNS time LANGUAGE SQL IMMUTABLE STRICT as 
'select cast(cast($1 as text) as time)';

CREATE CAST ("binary" AS time) WITH FUNCTION pg_catalog.Varlena2Time(anyelement) AS ASSIGNMENT;
CREATE CAST ("varbinary" AS time) WITH FUNCTION pg_catalog.Varlena2Time(anyelement) AS ASSIGNMENT;
CREATE CAST (blob AS time) WITH FUNCTION pg_catalog.Varlena2Time(anyelement) AS ASSIGNMENT;
CREATE CAST (tinyblob AS time) WITH FUNCTION pg_catalog.Varlena2Time(anyelement) AS ASSIGNMENT;
CREATE CAST (mediumblob AS time) WITH FUNCTION pg_catalog.Varlena2Time(anyelement) AS ASSIGNMENT;
CREATE CAST (longblob AS time) WITH FUNCTION pg_catalog.Varlena2Time(anyelement) AS ASSIGNMENT;
CREATE CAST (json AS time) WITH FUNCTION pg_catalog.Varlena2Time(anyelement) AS ASSIGNMENT;


CREATE OR REPLACE FUNCTION pg_catalog.Varlena2Year(anyelement) RETURNS year LANGUAGE SQL IMMUTABLE STRICT as 
'select cast(cast($1 as text) as year)';

CREATE CAST ("binary" AS year) WITH FUNCTION pg_catalog.Varlena2Year(anyelement) AS ASSIGNMENT;
CREATE CAST ("varbinary" AS year) WITH FUNCTION pg_catalog.Varlena2Year(anyelement) AS ASSIGNMENT;
CREATE CAST (blob AS year) WITH FUNCTION pg_catalog.Varlena2Year(anyelement) AS ASSIGNMENT;
CREATE CAST (tinyblob AS year) WITH FUNCTION pg_catalog.Varlena2Year(anyelement) AS ASSIGNMENT;
CREATE CAST (mediumblob AS year) WITH FUNCTION pg_catalog.Varlena2Year(anyelement) AS ASSIGNMENT;
CREATE CAST (longblob AS year) WITH FUNCTION pg_catalog.Varlena2Year(anyelement) AS ASSIGNMENT;
CREATE CAST (json AS year) WITH FUNCTION pg_catalog.Varlena2Year(anyelement) AS ASSIGNMENT;


CREATE OR REPLACE FUNCTION pg_catalog.Varlena2Bit(anyelement, int4) RETURNS bit LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'Varlena2Bit';

CREATE CAST ("binary" AS bit) WITH FUNCTION pg_catalog.Varlena2Bit(anyelement, int4) AS ASSIGNMENT;
CREATE CAST ("varbinary" AS bit) WITH FUNCTION pg_catalog.Varlena2Bit(anyelement, int4) AS ASSIGNMENT;
CREATE CAST (blob AS bit) WITH FUNCTION pg_catalog.Varlena2Bit(anyelement, int4) AS ASSIGNMENT;
CREATE CAST (tinyblob AS bit) WITH FUNCTION pg_catalog.Varlena2Bit(anyelement, int4) AS ASSIGNMENT;
CREATE CAST (mediumblob AS bit) WITH FUNCTION pg_catalog.Varlena2Bit(anyelement, int4) AS ASSIGNMENT;
CREATE CAST (longblob AS bit) WITH FUNCTION pg_catalog.Varlena2Bit(anyelement, int4) AS ASSIGNMENT;
CREATE CAST (json AS bit) WITH FUNCTION pg_catalog.Varlena2Bit(anyelement, int4) AS ASSIGNMENT;


CREATE OR REPLACE FUNCTION pg_catalog.varlenatoset(anyelement, int4) RETURNS anyset LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'varlenatoset';

CREATE CAST ("binary" AS anyset) WITH FUNCTION pg_catalog.varlenatoset(anyelement, int4) AS ASSIGNMENT;
CREATE CAST ("varbinary" AS anyset) WITH FUNCTION pg_catalog.varlenatoset(anyelement, int4) AS ASSIGNMENT;
CREATE CAST (blob AS anyset) WITH FUNCTION pg_catalog.varlenatoset(anyelement, int4) AS ASSIGNMENT;
CREATE CAST (tinyblob AS anyset) WITH FUNCTION pg_catalog.varlenatoset(anyelement, int4) AS ASSIGNMENT;
CREATE CAST (mediumblob AS anyset) WITH FUNCTION pg_catalog.varlenatoset(anyelement, int4) AS ASSIGNMENT;
CREATE CAST (longblob AS anyset) WITH FUNCTION pg_catalog.varlenatoset(anyelement, int4) AS ASSIGNMENT;
CREATE CAST (json AS anyset) WITH FUNCTION pg_catalog.varlenatoset(anyelement, int4) AS ASSIGNMENT;


DROP FUNCTION IF EXISTS pg_catalog.varlena_json(anyelement) cascade;
CREATE OR REPLACE FUNCTION pg_catalog.varlena_json(anyelement) RETURNS json LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'cast_json';

DROP FUNCTION IF EXISTS pg_catalog.to_base64 (tinyblob) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.to_base64 (blob) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.to_base64 (mediumblob) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.to_base64 (longblob) CASCADE;

CREATE OR REPLACE FUNCTION pg_catalog.to_base64 (tinyblob)  RETURNS text LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'base64_encode';
CREATE OR REPLACE FUNCTION pg_catalog.to_base64 (blob)  RETURNS text LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'base64_encode';
CREATE OR REPLACE FUNCTION pg_catalog.to_base64 (mediumblob)  RETURNS text LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'base64_encode';
CREATE OR REPLACE FUNCTION pg_catalog.to_base64 (longblob)  RETURNS text LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'base64_encode';


DROP FUNCTION IF EXISTS pg_catalog.binary_json(binary) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.varbinary_json(binary) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.tinyblob_json(binary) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.blob_json(binary) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.mediumblob_json(binary) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.longblob_json(binary) CASCADE;

DROP CAST IF EXISTS (binary as json) CASCADE;
DROP CAST IF EXISTS (varbinary as json) CASCADE;
DROP CAST IF EXISTS (tinyblob as json) CASCADE;
DROP CAST IF EXISTS (blob as json) CASCADE;
DROP CAST IF EXISTS (mediumblob as json) CASCADE;
DROP CAST IF EXISTS (longblob as json) CASCADE;

CREATE OR REPLACE FUNCTION pg_catalog.binary_json(binary)
RETURNS json 
AS
$$
BEGIN
    RETURN (SELECT to_json(concat('base64:type254:', to_base64($1))));
END;
$$
LANGUAGE plpgsql;
CREATE CAST (binary as json) WITH FUNCTION pg_catalog.binary_json(binary);

CREATE OR REPLACE FUNCTION pg_catalog.varbinary_json(varbinary)
RETURNS json 
AS
$$
BEGIN
    RETURN (SELECT to_json(concat('base64:type15:', to_base64($1))));
END;
$$
LANGUAGE plpgsql;
CREATE CAST (varbinary as json) WITH FUNCTION pg_catalog.varbinary_json(varbinary);

CREATE OR REPLACE FUNCTION pg_catalog.tinyblob_json(tinyblob)
RETURNS json 
AS
$$
BEGIN
    RETURN (SELECT to_json(concat('base64:type249:', to_base64($1))));
END;
$$
LANGUAGE plpgsql;
CREATE CAST (tinyblob as json) WITH FUNCTION pg_catalog.tinyblob_json(tinyblob);

CREATE OR REPLACE FUNCTION pg_catalog.blob_json(blob)
RETURNS json 
AS
$$
BEGIN
    RETURN (SELECT to_json(concat('base64:type252:', to_base64($1))));
END;
$$
LANGUAGE plpgsql;
CREATE CAST (blob as json) WITH FUNCTION pg_catalog.blob_json(blob);

CREATE OR REPLACE FUNCTION pg_catalog.mediumblob_json(mediumblob)
RETURNS json 
AS
$$
BEGIN
    RETURN (SELECT to_json(concat('base64:type250:', to_base64($1))));
END;
$$
LANGUAGE plpgsql;
CREATE CAST (mediumblob as json) WITH FUNCTION pg_catalog.mediumblob_json(mediumblob);

CREATE OR REPLACE FUNCTION pg_catalog.longblob_json(longblob)
RETURNS json 
AS
$$
BEGIN
    RETURN (SELECT to_json(concat('base64:type251:', to_base64($1))));
END;
$$
LANGUAGE plpgsql;
CREATE CAST (longblob as json) WITH FUNCTION pg_catalog.longblob_json(longblob);


DROP FUNCTION IF EXISTS pg_catalog.bittobinary(bit) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.bittovarbinary(bit) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.bittotinyblob(bit) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.bittomediumblob(bit) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.bittoblob(bit) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.bittolongblob(bit) CASCADE;

DROP CAST IF EXISTS (bit AS binary) CASCADE;
DROP CAST IF EXISTS (bit AS varbinary) CASCADE;
DROP CAST IF EXISTS (bit AS tinyblob) CASCADE;
DROP CAST IF EXISTS (bit AS mediumblob) CASCADE;
DROP CAST IF EXISTS (bit AS blob) CASCADE;
DROP CAST IF EXISTS (bit AS longblob) CASCADE;

CREATE OR REPLACE FUNCTION pg_catalog.bittobinary(bit) 
RETURNS binary LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'bittobinary';
CREATE CAST (bit AS binary) WITH FUNCTION bittobinary(bit) AS ASSIGNMENT;

CREATE OR REPLACE FUNCTION pg_catalog.bittovarbinary(bit) 
RETURNS varbinary LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'bittovarbinary';
CREATE CAST (bit AS varbinary) WITH FUNCTION bittovarbinary(bit) AS ASSIGNMENT;

CREATE OR REPLACE FUNCTION pg_catalog.bittotinyblob(bit) 
RETURNS tinyblob LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'bittotinyblob';
CREATE CAST (bit AS tinyblob) WITH FUNCTION bittotinyblob(bit) AS ASSIGNMENT;

CREATE OR REPLACE FUNCTION pg_catalog.bittomediumblob(bit) 
RETURNS mediumblob LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'bittomediumblob';
CREATE CAST (bit AS mediumblob) WITH FUNCTION bittomediumblob(bit) AS ASSIGNMENT;

CREATE OR REPLACE FUNCTION pg_catalog.bittoblob(bit) 
RETURNS blob LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'bittoblob';
CREATE CAST (bit AS blob) WITH FUNCTION bittoblob(bit) AS ASSIGNMENT;

CREATE OR REPLACE FUNCTION pg_catalog.bittolongblob(bit) 
RETURNS longblob LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'bittolongblob';
CREATE CAST (bit AS longblob) WITH FUNCTION bittolongblob(bit) AS ASSIGNMENT;


DROP FUNCTION IF EXISTS pg_catalog.blob_date_xor(
    blob,
    date
) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.blob_date_xor(
    date,
    blob
) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.blob_date_xor(
    blob,
    timestamptz
) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.blob_date_xor(
    timestamptz,
    blob
) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.blob_date_xor(
    time,
    bit
) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.blob_date_xor(
    bit,
    time
) CASCADE;

create function pg_catalog.blob_date_xor(
    blob,
    date
) RETURNS int16 LANGUAGE SQL IMMUTABLE STRICT as $$ SELECT pg_catalog.float8_date_xor($1::float8, $2) $$;
create operator pg_catalog.^(leftarg = blob, rightarg = date, procedure = pg_catalog.blob_date_xor);

create function pg_catalog.date_blob_xor(
    date,
    blob
) RETURNS int16 LANGUAGE SQL IMMUTABLE STRICT as $$ SELECT pg_catalog.date_float8_xor($1, $2::float8) $$;
create operator pg_catalog.^(leftarg = date, rightarg = blob, procedure = pg_catalog.date_blob_xor);

create function pg_catalog.blob_timestamptz_xor(
    blob,
    timestamptz
) RETURNS int16 LANGUAGE SQL IMMUTABLE STRICT as $$ SELECT pg_catalog.float8_timestamptz_xor($1::float8, $2) $$;
create operator pg_catalog.^(leftarg = blob, rightarg = timestamptz, procedure = pg_catalog.blob_timestamptz_xor);

create function pg_catalog.timestamptz_blob_xor(
    timestamptz,
    blob
) RETURNS int16 LANGUAGE SQL IMMUTABLE STRICT as $$ SELECT pg_catalog.timestamptz_float8_xor($1, $2::float8) $$;
create operator pg_catalog.^(leftarg = timestamptz, rightarg = blob, procedure = pg_catalog.timestamptz_blob_xor);

create function pg_catalog.time_xor_bit(
    time,
    bit
) RETURNS float8 LANGUAGE SQL IMMUTABLE STRICT as $$ SELECT pg_catalog.dpow($1::float8, $2::float8) $$;
create operator pg_catalog.^(leftarg = time, rightarg = bit, procedure = pg_catalog.time_xor_bit);

create function pg_catalog.bit_xor_time(
    bit,
    time
) RETURNS float8 LANGUAGE SQL IMMUTABLE STRICT as $$ SELECT pg_catalog.dpow($1::float8, $2::float8) $$;
create operator pg_catalog.^(leftarg = bit, rightarg = time, procedure = pg_catalog.bit_xor_time);

--update_castcontext
DROP FUNCTION IF EXISTS pg_catalog.update_castcontext(varchar, varchar) CASCADE;
CREATE FUNCTION pg_catalog.update_castcontext(varchar, varchar) RETURNS varchar AS
$$
DECLARE
    s_type ALIAS FOR $1;
    t_type ALIAS FOR $2;
BEGIN
    update pg_cast set castcontext='i', castowner=10 where castsource=(select oid from pg_type where typname=s_type) and casttarget=(select oid from pg_type where typname=t_type);
    RETURN 'SUCCESS';
END;
$$
LANGUAGE plpgsql;

DO
$$
BEGIN
    PERFORM pg_catalog.update_castcontext('int8', 'int1');
    PERFORM pg_catalog.update_castcontext('int8', 'int2');
    PERFORM pg_catalog.update_castcontext('int8', 'int4');

    PERFORM pg_catalog.update_castcontext('date', 'int4');
    PERFORM pg_catalog.update_castcontext('date', 'int8');
    PERFORM pg_catalog.update_castcontext('date', 'uint4');
    PERFORM pg_catalog.update_castcontext('date', 'uint8');
    PERFORM pg_catalog.update_castcontext('date', 'float4');
    PERFORM pg_catalog.update_castcontext('date', 'float8');
    PERFORM pg_catalog.update_castcontext('date', 'numeric');
    PERFORM pg_catalog.update_castcontext('timestamp', 'int8');
    PERFORM pg_catalog.update_castcontext('timestamp', 'uint8');
    PERFORM pg_catalog.update_castcontext('timestamp', 'float4');
    PERFORM pg_catalog.update_castcontext('timestamp', 'numeric');
    PERFORM pg_catalog.update_castcontext('timestamptz', 'int8');
    PERFORM pg_catalog.update_castcontext('timestamptz', 'uint8');
    PERFORM pg_catalog.update_castcontext('timestamptz', 'float4');
    PERFORM pg_catalog.update_castcontext('timestamptz', 'float8');
    PERFORM pg_catalog.update_castcontext('timestamptz', 'numeric');
    PERFORM pg_catalog.update_castcontext('time', 'int4');
    PERFORM pg_catalog.update_castcontext('time', 'int8');
    PERFORM pg_catalog.update_castcontext('time', 'uint4');
    PERFORM pg_catalog.update_castcontext('time', 'uint8');
    PERFORM pg_catalog.update_castcontext('time', 'float4');
    PERFORM pg_catalog.update_castcontext('time', 'numeric');
END
$$
LANGUAGE plpgsql;
DROP FUNCTION IF EXISTS pg_catalog.oct(bit);
CREATE OR REPLACE FUNCTION pg_catalog.oct(bit) RETURNS text AS
$$
BEGIN
    RETURN 0;
END;
$$
LANGUAGE plpgsql;

DROP FUNCTION IF EXISTS pg_catalog.update_castcontext(varchar, varchar) CASCADE;

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
DROP FUNCTION IF EXISTS pg_catalog.export_set (numeric, text, text, text, numeric) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.export_set (numeric, text, text, text) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.export_set (numeric, text, text) CASCADE;

DROP FUNCTION IF EXISTS pg_catalog.export_set (int8, "any", "any", "any", int8) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.export_set (int8, "any", "any", "any", int8) RETURNS text LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'export_set_5args_any';

DROP FUNCTION IF EXISTS pg_catalog.export_set (int8, "any", "any", "any") CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.export_set (int8, "any", "any", "any") RETURNS text LANGUAGE C STABLE STRICT  as '$libdir/dolphin', 'export_set_4args_any';

DROP FUNCTION IF EXISTS pg_catalog.export_set (int8, "any", "any") CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.export_set (int8, "any", "any")  RETURNS text LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'export_set_3args_any';
DROP FUNCTION IF EXISTS pg_catalog.sleep(d date) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.sleep(d date)
RETURNS int AS
$$
BEGIN
    RETURN (select sleep(date_int(d)));
END;
$$
LANGUAGE plpgsql;

DROP FUNCTION IF EXISTS pg_catalog.pg_open_tables() CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.pg_open_tables(TEXT) CASCADE;
CREATE FUNCTION pg_catalog.pg_open_tables()
    RETURNS TABLE ("oid" Oid, "relname" TEXT, "relnamespace" TEXT, "refcnt" int4, "lockcnt" int4, "accessexclusive_lockcnt" int4)
    LANGUAGE C VOLATILE STRICT as '$libdir/dolphin', 'pg_open_tables';
CREATE FUNCTION pg_catalog.pg_open_tables(TEXT)
    RETURNS TABLE ("oid" Oid, "relname" TEXT, "relnamespace" TEXT, "refcnt" int4, "lockcnt" int4, "accessexclusive_lockcnt" int4)
    LANGUAGE C VOLATILE STRICT as '$libdir/dolphin', 'pg_open_tables';

drop aggregate if exists pg_catalog.bit_xor(text);
DROP FUNCTION IF EXISTS pg_catalog.text_xor(uint8,text) CASCADE;

CREATE FUNCTION pg_catalog.text_xor (t1 uint8,t2 text) RETURNS uint8 LANGUAGE C IMMUTABLE as '$libdir/dolphin', 'uint8_xor_text';
create aggregate pg_catalog.bit_xor(text) (SFUNC=text_xor, STYPE= uint8);

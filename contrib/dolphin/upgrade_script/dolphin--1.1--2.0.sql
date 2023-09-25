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

DROP FUNCTION IF EXISTS pg_catalog.oct(text) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.oct(boolean) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.oct(bit) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.oct(numeric) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.oct(date) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.oct(timestamp without time zone) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.oct(timestamptz) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.oct(time) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.oct(text) RETURNS text LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'oct_str';
CREATE OR REPLACE FUNCTION pg_catalog.oct(numeric) RETURNS text LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'oct_num';

CREATE OR REPLACE FUNCTION pg_catalog.oct(date) RETURNS text AS $$ SELECT pg_catalog.oct($1::text) $$ LANGUAGE SQL;
CREATE OR REPLACE FUNCTION pg_catalog.oct(timestamp without time zone) RETURNS text AS $$ SELECT pg_catalog.oct($1::text) $$ LANGUAGE SQL;
CREATE OR REPLACE FUNCTION pg_catalog.oct(timestamptz) RETURNS text AS $$ SELECT pg_catalog.oct($1::text) $$ LANGUAGE SQL;
CREATE OR REPLACE FUNCTION pg_catalog.oct(time) RETURNS text AS $$ SELECT pg_catalog.oct($1::text) $$ LANGUAGE SQL;

DROP FUNCTION IF EXISTS pg_catalog.conv(date, int4, int4) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.conv(timestamp without time zone, int4, int4) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.conv(timestamptz, int4, int4) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.conv(time, int4, int4) CASCADE;

CREATE OR REPLACE FUNCTION pg_catalog.conv (
date, int4, int4
) RETURNS text AS $$ SELECT pg_catalog.conv($1::text, $2, $3) $$ LANGUAGE SQL;

CREATE OR REPLACE FUNCTION pg_catalog.conv (
timestamp without time zone, int4, int4
) RETURNS text AS $$ SELECT pg_catalog.conv($1::text, $2, $3) $$ LANGUAGE SQL;

CREATE OR REPLACE FUNCTION pg_catalog.conv (
timestamptz, int4, int4
) RETURNS text AS $$ SELECT pg_catalog.conv($1::text, $2, $3) $$ LANGUAGE SQL;

CREATE OR REPLACE FUNCTION pg_catalog.conv (
time, int4, int4
) RETURNS text AS $$ SELECT pg_catalog.conv($1::text, $2, $3) $$ LANGUAGE SQL;

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

DROP FUNCTION IF EXISTS pg_catalog.bittodate(bit) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.bittodate(bit) 
RETURNS date LANGUAGE SQL IMMUTABLE STRICT as 
'select cast(cast($1 as int8) as date)';
DROP CAST IF EXISTS (bit AS date) CASCADE;
CREATE CAST (bit AS date) WITH FUNCTION bittodate(bit) AS ASSIGNMENT;

DROP FUNCTION IF EXISTS pg_catalog.bittodatetime(bit) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.bittodatetime(bit) 
RETURNS timestamp without time zone LANGUAGE SQL IMMUTABLE STRICT as 
'select cast(cast($1 as int8) as timestamp without time zone)';
DROP CAST IF EXISTS (bit AS timestamp without time zone) CASCADE;
CREATE CAST (bit AS timestamp without time zone) WITH FUNCTION bittodatetime(bit) AS ASSIGNMENT;

DROP FUNCTION IF EXISTS pg_catalog.bittotimestamp(bit) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.bittotimestamp(bit) 
RETURNS timestamptz LANGUAGE SQL IMMUTABLE STRICT as 
'select cast(cast($1 as int8) as timestamptz)';
DROP CAST IF EXISTS (bit AS timestamptz) CASCADE;
CREATE CAST (bit AS timestamptz) WITH FUNCTION bittotimestamp(bit) AS ASSIGNMENT;

DROP FUNCTION IF EXISTS pg_catalog.bittotime(bit) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.bittotime(bit) 
RETURNS time LANGUAGE SQL IMMUTABLE STRICT as 
'select cast(cast($1 as int8) as time)';
DROP CAST IF EXISTS (bit AS time) CASCADE;
CREATE CAST (bit AS time) WITH FUNCTION bittotime(bit) AS ASSIGNMENT;

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

CREATE OR REPLACE FUNCTION pg_catalog.bpchar_time (char) RETURNS time LANGUAGE SQL IMMUTABLE STRICT as 'select cast(cast($1 as text) as time)';
CREATE CAST(char as time) WITH FUNCTION bpchar_time(char) AS IMPLICIT;

CREATE OR REPLACE FUNCTION pg_catalog.varchar_time (varchar) RETURNS time LANGUAGE SQL IMMUTABLE STRICT as 'select cast(cast($1 as text) as time)';
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

CREATE OR REPLACE FUNCTION pg_catalog.set_year (anyset) RETURNS year LANGUAGE SQL IMMUTABLE STRICT as 'select cast(cast($1 as int8) as year)';
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

CREATE OR REPLACE FUNCTION pg_catalog.enum_year(anyenum) RETURNS year LANGUAGE SQL IMMUTABLE STRICT as 'select cast(cast($1 as int8) as year)';

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
) RETURNS time LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint8_b_format_time';
CREATE CAST (uint1 AS time) WITH FUNCTION int8_b_format_time(uint1) AS ASSIGNMENT;

CREATE OR REPLACE FUNCTION pg_catalog.int16_b_format_time (
uint2
) RETURNS time LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint16_b_format_time';
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
) RETURNS time LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint64_b_format_time';
CREATE CAST (uint8 AS time) WITH FUNCTION int64_b_format_time(uint8) AS ASSIGNMENT;

CREATE OR REPLACE FUNCTION pg_catalog.int32_b_format_time (
uint4
) RETURNS time LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint32_b_format_time';

CREATE OR REPLACE FUNCTION pg_catalog.time_uint1 (time) RETURNS uint1 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'time2uint1';
CREATE CAST(time AS uint1) WITH FUNCTION time_uint1(time) AS ASSIGNMENT;

CREATE OR REPLACE FUNCTION pg_catalog.time_uint2 (time) RETURNS uint2 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'time2uint2';
CREATE CAST(time AS uint2) WITH FUNCTION time_uint2(time) AS ASSIGNMENT;

CREATE OR REPLACE FUNCTION pg_catalog.time_uint4 (time) RETURNS uint4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'time2uint4';
CREATE CAST(time AS uint4) WITH FUNCTION time_uint4(time) AS ASSIGNMENT;

CREATE OR REPLACE FUNCTION pg_catalog.time_uint8 (time) RETURNS uint8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'time2uint8';
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

create function pg_catalog.datetime_int8_xor(
    timestamp without time zone,
    uint8
) RETURNS int16 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'timestamp_int8_xor';
create operator pg_catalog.^(leftarg = timestamp without time zone, rightarg = uint8, procedure = pg_catalog.datetime_int8_xor);

create function pg_catalog.int8_datetime_xor(
    uint8,
    timestamp without time zone
) RETURNS int16 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'int8_timestamp_xor';
create operator pg_catalog.^(leftarg = uint8, rightarg = timestamp without time zone, procedure = pg_catalog.int8_datetime_xor);

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
CREATE CAST ("binary" AS text) WITH FUNCTION pg_catalog.Varlena2Text(anyelement) AS IMPLICIT;
CREATE CAST ("varbinary" AS text) WITH FUNCTION pg_catalog.Varlena2Text(anyelement) AS IMPLICIT;
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

DROP FUNCTION IF EXISTS pg_catalog.export_set (int16, "any", "any", "any", int8) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.export_set (int16, "any", "any", "any", int8) RETURNS text LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'export_set_5args_any';

DROP FUNCTION IF EXISTS pg_catalog.export_set (int16, "any", "any", "any") CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.export_set (int16, "any", "any", "any") RETURNS text LANGUAGE C STABLE STRICT  as '$libdir/dolphin', 'export_set_4args_any';

DROP FUNCTION IF EXISTS pg_catalog.export_set (int16, "any", "any") CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.export_set (int16, "any", "any")  RETURNS text LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'export_set_3args_any';

DROP FUNCTION IF EXISTS pg_catalog.export_set (bit, "any", "any", "any", bit) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.export_set (bit, "any", "any", "any", bit) RETURNS text LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'export_set_5args_bits';

DROP FUNCTION IF EXISTS pg_catalog.export_set (bit, "any", "any", "any", int8) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.export_set (bit, "any", "any", "any", int8) RETURNS text LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'export_set_5args_bit_1';

DROP FUNCTION IF EXISTS pg_catalog.export_set (int16, "any", "any", "any", bit) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.export_set (int16, "any", "any", "any", bit) RETURNS text LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'export_set_5args_bit_5';

DROP FUNCTION IF EXISTS pg_catalog.export_set (bit, "any", "any", "any") CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.export_set (bit, "any", "any", "any") RETURNS text LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'export_set_4args_bit';

DROP FUNCTION IF EXISTS pg_catalog.export_set (bit, "any", "any") CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.export_set (bit, "any", "any") RETURNS text LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'export_set_3args_bit';

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

DROP FUNCTION IF EXISTS pg_catalog.compress (text) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.compress (text)  RETURNS bytea LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'compress_text';

DROP FUNCTION IF EXISTS pg_catalog.compress (bytea) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.compress (bytea)  RETURNS bytea LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'compress_bytea';

DROP FUNCTION IF EXISTS pg_catalog.compress (boolean) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.compress (boolean) RETURNS bytea LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'compress_boolean';

DROP FUNCTION IF EXISTS pg_catalog.compress (bit) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.compress (bit) RETURNS bytea LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'compress_bit';

DROP FUNCTION IF EXISTS pg_catalog.uncompress (text) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.uncompress (text) RETURNS text LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'uncompress_text';

DROP FUNCTION IF EXISTS pg_catalog.uncompress (bytea) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.uncompress (bytea) RETURNS text LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'uncompress_bytea';

DROP FUNCTION IF EXISTS pg_catalog.uncompress (boolean) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.uncompress (boolean) RETURNS text LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'uncompress_boolean';

DROP FUNCTION IF EXISTS pg_catalog.uncompress (bit) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.uncompress (bit) RETURNS text LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'uncompress_bit';

DROP FUNCTION IF EXISTS pg_catalog.uncompressed_length (text) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.uncompressed_length (text) RETURNS uint4 LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'uncompressed_length_text';

DROP FUNCTION IF EXISTS pg_catalog.uncompressed_length (bytea) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.uncompressed_length (bytea) RETURNS uint4 LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'uncompressed_length_bytea';

DROP FUNCTION IF EXISTS pg_catalog.uncompressed_length (boolean) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.uncompressed_length (boolean) RETURNS uint4 LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'uncompressed_length_boolean';

DROP FUNCTION IF EXISTS pg_catalog.uncompressed_length (bit) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.uncompressed_length (bit) RETURNS uint4 LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'uncompressed_length_bit';

DROP FUNCTION IF EXISTS  pg_catalog.weight_string(TEXT, uint4) cascade;
CREATE OR REPLACE FUNCTION pg_catalog.weight_string(TEXT, uint4) RETURNS TEXT LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'weight_string_single';
DROP FUNCTION IF EXISTS  pg_catalog.weight_string(bytea, uint4) cascade;
CREATE OR REPLACE FUNCTION pg_catalog.weight_string(bytea, uint4) RETURNS TEXT LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'weight_string_bytea';
DROP FUNCTION IF EXISTS  pg_catalog.weight_string(boolean, uint4) cascade;
CREATE OR REPLACE FUNCTION pg_catalog.weight_string(boolean, uint4)  RETURNS TEXT LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'weight_string_boolean';
DROP FUNCTION IF EXISTS  pg_catalog.weight_string(numeric, uint4) cascade;
CREATE OR REPLACE FUNCTION pg_catalog.weight_string(numeric, uint4) RETURNS text AS
$$
BEGIN
    RETURN NULL;
END;
$$
LANGUAGE plpgsql;
DROP FUNCTION IF EXISTS  pg_catalog.weight_string(date, uint4) cascade;
CREATE OR REPLACE FUNCTION pg_catalog.weight_string(date, uint4)  RETURNS TEXT LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'weight_string_date';
DROP FUNCTION IF EXISTS  pg_catalog.weight_string(time, uint4) cascade;
CREATE OR REPLACE FUNCTION pg_catalog.weight_string(time, uint4)  RETURNS TEXT LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'weight_string_time';
DROP FUNCTION IF EXISTS  pg_catalog.weight_string(timestamp without time zone, uint4) cascade;
CREATE OR REPLACE FUNCTION pg_catalog.weight_string(timestamp without time zone, uint4)  RETURNS TEXT LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'weight_string_timestamp';
DROP FUNCTION IF EXISTS  pg_catalog.weight_string(timestamptz, uint4) cascade;
CREATE OR REPLACE FUNCTION pg_catalog.weight_string(timestamptz, uint4)  RETURNS TEXT LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'weight_string_timestamptz';
DROP FUNCTION IF EXISTS  pg_catalog.weight_string(interval, uint4) cascade;
CREATE OR REPLACE FUNCTION pg_catalog.weight_string(interval, uint4)  RETURNS TEXT LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'weight_string_interval';
DROP FUNCTION IF EXISTS  pg_catalog.weight_string(bit, uint4) cascade;
CREATE OR REPLACE FUNCTION pg_catalog.weight_string(bit, uint4) RETURNS TEXT LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'weight_string_bit';

DROP FUNCTION IF EXISTS  pg_catalog.weight_string(TEXT, TEXT, uint4) cascade;
CREATE OR REPLACE FUNCTION pg_catalog.weight_string(TEXT, TEXT, uint4)  RETURNS TEXT LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'weight_string';
DROP FUNCTION IF EXISTS pg_catalog.weight_string (bytea, text, uint4) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.weight_string(bytea, TEXT, uint4)  RETURNS TEXT LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'weight_string_bytea';
DROP FUNCTION IF EXISTS  pg_catalog.weight_string(boolean, TEXT, uint4) cascade;
CREATE OR REPLACE FUNCTION pg_catalog.weight_string(boolean, TEXT, uint4)  RETURNS TEXT LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'weight_string_boolean';
DROP FUNCTION IF EXISTS  pg_catalog.weight_string(numeric, TEXT, uint4) cascade;
CREATE OR REPLACE FUNCTION pg_catalog.weight_string(numeric, TEXT, uint4) RETURNS text AS
$$
BEGIN
    RETURN NULL;
END;
$$
LANGUAGE plpgsql;
DROP FUNCTION IF EXISTS  pg_catalog.weight_string(date, TEXT, uint4) cascade;
CREATE OR REPLACE FUNCTION pg_catalog.weight_string(date, TEXT, uint4)  RETURNS TEXT LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'weight_string_date';
DROP FUNCTION IF EXISTS  pg_catalog.weight_string(time, TEXT, uint4) cascade;
CREATE OR REPLACE FUNCTION pg_catalog.weight_string(time, TEXT, uint4)  RETURNS TEXT LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'weight_string_time';
DROP FUNCTION IF EXISTS  pg_catalog.weight_string(timestamp without time zone, TEXT, uint4) cascade;
CREATE OR REPLACE FUNCTION pg_catalog.weight_string(timestamp without time zone, TEXT, uint4)  RETURNS TEXT LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'weight_string_timestamp';
DROP FUNCTION IF EXISTS  pg_catalog.weight_string(timestamptz, TEXT, uint4) cascade;
CREATE OR REPLACE FUNCTION pg_catalog.weight_string(timestamptz, TEXT, uint4)  RETURNS TEXT LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'weight_string_timestamptz';
DROP FUNCTION IF EXISTS  pg_catalog.weight_string(interval, TEXT, uint4) cascade;
CREATE OR REPLACE FUNCTION pg_catalog.weight_string(interval, TEXT, uint4)  RETURNS TEXT LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'weight_string_interval';
DROP FUNCTION IF EXISTS  pg_catalog.weight_string(bit, TEXT, uint4) cascade;
CREATE OR REPLACE FUNCTION pg_catalog.weight_string(bit, TEXT, uint4)  RETURNS TEXT LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'weight_string_bit';

DROP FUNCTION IF EXISTS  pg_catalog.weight_string(TEXT, TEXT, uint4, uint4) cascade;
CREATE OR REPLACE FUNCTION pg_catalog.weight_string(TEXT, TEXT, uint4, uint4)  RETURNS TEXT LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'weight_string';
DROP FUNCTION IF EXISTS pg_catalog.weight_string (bytea, text, uint4, uint4) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.weight_string(bytea, TEXT, uint4, uint4)  RETURNS TEXT LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'weight_string_bytea';
DROP FUNCTION IF EXISTS  pg_catalog.weight_string(boolean, TEXT, uint4, uint4) cascade;
CREATE OR REPLACE FUNCTION pg_catalog.weight_string(boolean, TEXT, uint4, uint4)  RETURNS TEXT LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'weight_string_boolean';
DROP FUNCTION IF EXISTS  pg_catalog.weight_string(numeric, TEXT, uint4, uint4) cascade;
CREATE OR REPLACE FUNCTION pg_catalog.weight_string(numeric, TEXT, TEXT, uint4) RETURNS text AS
$$
BEGIN
    RETURN NULL;
END;
$$
LANGUAGE plpgsql;
DROP FUNCTION IF EXISTS  pg_catalog.weight_string(date, TEXT, uint4, uint4) cascade;
CREATE OR REPLACE FUNCTION pg_catalog.weight_string(date, TEXT, uint4, uint4)  RETURNS TEXT LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'weight_string_date';
DROP FUNCTION IF EXISTS  pg_catalog.weight_string(time, TEXT, uint4, uint4) cascade;
CREATE OR REPLACE FUNCTION pg_catalog.weight_string(time, TEXT, uint4, uint4)  RETURNS TEXT LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'weight_string_time';
DROP FUNCTION IF EXISTS  pg_catalog.weight_string(timestamp without time zone, TEXT, uint4, uint4) cascade;
CREATE OR REPLACE FUNCTION pg_catalog.weight_string(timestamp without time zone, TEXT, uint4, uint4)  RETURNS TEXT LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'weight_string_timestamp';
DROP FUNCTION IF EXISTS  pg_catalog.weight_string(timestamptz, TEXT, uint4, uint4) cascade;
CREATE OR REPLACE FUNCTION pg_catalog.weight_string(timestamptz, TEXT, uint4, uint4)  RETURNS TEXT LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'weight_string_timestamptz';
DROP FUNCTION IF EXISTS  pg_catalog.weight_string(interval, TEXT, uint4, uint4) cascade;
CREATE OR REPLACE FUNCTION pg_catalog.weight_string(interval, TEXT, uint4, uint4)  RETURNS TEXT LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'weight_string_interval';
DROP FUNCTION IF EXISTS pg_catalog.weight_string (bit, text, uint4, uint4) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.weight_string(bit, TEXT, uint4, uint4)  RETURNS TEXT LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'weight_string_bit';

drop aggregate if exists pg_catalog.bit_xor(text);
DROP FUNCTION IF EXISTS pg_catalog.text_xor(uint8,text) CASCADE;

CREATE FUNCTION pg_catalog.text_xor (t1 uint8,t2 text) RETURNS uint8 LANGUAGE C IMMUTABLE as '$libdir/dolphin', 'uint8_xor_text';
create aggregate pg_catalog.bit_xor(text) (SFUNC=text_xor, STYPE= uint8);

drop schema if exists performance_schema CASCADE;
create schema performance_schema;

CREATE OR REPLACE FUNCTION pg_catalog.get_statement_history(
OUT schema_name name,OUT unique_query_id bigint,OUT debug_query_id bigint,OUT query text,
OUT start_time timestamp with time zone, OUT finish_time timestamp with time zone,OUT thread_id bigint,
OUT session_id bigint, OUT n_returned_rows bigint, OUT n_tuples_inserted bigint,OUT n_tuples_updated bigint,
OUT n_tuples_deleted bigint,OUT cpu_time bigint,OUT lock_time bigint)
RETURNS setof record
AS $$
DECLARE
  row_data record;
  port text;
  query_str text;
  query_str_nodes text;
  user_name text;
  dblink_ext int;
  BEGIN
    --check privileges only sysadm or omsysadm can access the dbe_perf.statement table;
    SELECT SESSION_USER INTO user_name;
    IF pg_catalog.has_schema_privilege(user_name, 'dbe_perf', 'USAGE') = false 
    THEN 
       RAISE EXCEPTION 'user % has no privilege to use schema dbe_perf',user_name;
    END IF;

    IF pg_catalog.has_table_privilege(user_name, 'dbe_perf.statement_history', 'SELECT') = false 
    THEN 
       RAISE EXCEPTION 'user % has no privilege to access dbe_perf.statement_history',user_name;
    END IF;

    --check if dblink exists, lite mode does not have dblink
    EXECUTE('SELECT count(*) from pg_extension where extname=''dblink''') into dblink_ext;
    IF dblink_ext = 0
    THEN
        RAISE EXCEPTION 'dblink is needed by this function, please load dblink extension first';
    END IF;

    --get the port number 
    EXECUTE('SELECT setting FROM pg_settings WHERE name = ''port''') into port;
    
    query_str := concat(concat('select * from public.dblink(''dbname=postgres port=',port),''',''select schema_name,unique_query_id,debug_query_id,query ,start_time,finish_time, thread_id ,session_id ,n_returned_rows ,n_tuples_inserted ,n_tuples_updated ,n_tuples_deleted ,cpu_time , lock_time from dbe_perf.statement_history'') as t(schema_name name,unique_query_id bigint,debug_query_id bigint,query text,start_time timestamp with time zone, finish_time timestamp with time zone,thread_id bigint,session_id bigint, n_returned_rows bigint, n_tuples_inserted bigint, n_tuples_updated bigint,n_tuples_deleted bigint,cpu_time bigint,lock_time bigint)');
    FOR row_data IN EXECUTE(query_str) LOOP
        schema_name :=row_data.schema_name;
    unique_query_id := row_data.unique_query_id;
    debug_query_id := row_data.debug_query_id;
    query := row_data.query;
    start_time := row_data.start_time;
    finish_time := row_data.finish_time;
    thread_id := row_data.thread_id;
    session_id := row_data.session_id;
    n_returned_rows := row_data.n_returned_rows;
    n_tuples_inserted := row_data.n_tuples_inserted;
    n_tuples_updated := row_data.n_tuples_updated;
    n_tuples_deleted := row_data.n_tuples_deleted;
    cpu_time := row_data.cpu_time;
    lock_time:= row_data.lock_time;
    return next;
    END LOOP;
    return;
  END; $$
LANGUAGE 'plpgsql'
SECURITY DEFINER;

CREATE VIEW performance_schema.statement_history AS 
select * from pg_catalog.get_statement_history();

create view performance_schema.events_statements_current as 
select
  s.pid as THREAD_ID,
  NULL as EVENT_ID,  
  NULL as END_EVENT_ID,  
  NULL AS EVENT_NAME,   
  s.application_name AS SOURCE,  
  s.query_start AS TIMER_START,
  CASE
    WHEN s.state = 'active' THEN CURRENT_TIMESTAMP
    ELSE s.state_change
  END AS TIMER_END,
  CASE
    WHEN s.state = 'active' THEN CURRENT_TIMESTAMP - s.query_start
    ELSE s.state_change - s.query_start
  END AS TIMER_WAIT,
  NULL AS LOCK_TIME,   
  s.QUERY AS SQL_TEXT,
  SHA2(t.QUERY,256) AS DIGEST, 
  t.QUERY AS DIGEST_TEXT,
  h.schema_name AS SCHEMA_NAME, 
  NULL AS OBJECT_TYPE, 
  NULL AS OBJECT_SCHEMA, 
  NULL AS OBJECT_NAME, 
  NULL AS OBJECT_INSTANCE_BEGIN, 
  NULL AS MYSQL_ERRNO, 
  NULL AS RETURNED_SQLSTATE, 
  NULL AS MESSAGE_TEXT, 
  NULL AS ERRORS,      
  NULL AS WARNING_COUNT,
  (h.n_tuples_deleted+h.n_tuples_inserted+h.n_tuples_inserted)  AS ROW_AFFECTED,
  h.n_returned_rows AS ROW_SENT,
  NULL AS ROW_EXAMINED, 
  NULL AS CREATED_TMP_DISK_TABLES, 
  NULL AS CREATED_TMP_TABLES, 
  NULL AS SELECT_FULL_JOIN, 
  NULL AS SELECT_FULL_RANGE_JOIN, 
  NULL AS SELECT_RANGE, 
  NULL AS SELECT_RANGE_CHECK, 
  NULL AS SELECT_SCAN, 
  NULL AS SORT_MERGE_PASSES, 
  NULL AS SORT_RANGE, 
  NULL AS SORT_ROWS, 
  NULL AS SORT_SCAN, 
  NULL AS NO_INDEX_USED, 
  NULL AS NO_GOOD_INDEX_USED, 
  NULL AS NESTING_EVENT_ID, 
  NULL AS NESTING_EVENT_TYPE, 
  NULL AS NESTING_EVENT_LEVEL, 
  h.debug_query_id AS STATEMENT_ID,
  h.CPU_TIME AS CPU_TIME,    
  NULL AS MAX_CONTROLLED_MEMORY, 
  NULL AS MAX_TOTAL_MEMORY, 
  NULL AS EXECUTION_ENGINE 
  FROM pg_catalog.pg_stat_activity s
  left outer join performance_schema.statement_history h
  on s.query_id = h.debug_query_id
  and h.session_id = s.sessionid
  left outer join dbe_perf.statement t 
  on s.unique_sql_id = t.unique_sql_id
  where s.query_start IS NOT NULL
;
/* ----------------------
 *    Create performance_schema.events_statements_history
 * ----------------------
 */
create view performance_schema.events_statements_history as 
select
  h.thread_id as THREAD_ID,
  NULL as EVENT_ID,  
  NULL as END_EVENT_ID,  
  NULL AS EVENT_NAME,   
  s.application_name AS SOURCE,  
  h.start_time AS TIMER_START,
  h.finish_time AS TIMER_END,
  h.finish_time - h.start_time AS TIMER_WAIT,
  h.lock_time AS LOCK_TIME,   
  h.QUERY AS SQL_TEXT,
  SHA2(t.QUERY,256) AS DIGEST,
  t.QUERY AS DIGEST_TEXT,
  h.schema_name AS SCHEMA_NAME, 
  NULL AS OBJECT_TYPE, 
  NULL AS OBJECT_SCHEMA, 
  NULL AS OBJECT_NAME, 
  NULL AS OBJECT_INSTANCE_BEGIN, 
  NULL AS MYSQL_ERRNO, 
  NULL AS RETURNED_SQLSTATE, 
  NULL AS MESSAGE_TEXT, 
  0    AS ERRORS, 
  NULL AS WARNING_COUNT,
  (h.n_tuples_deleted+h.n_tuples_inserted+h.n_tuples_inserted)  AS ROW_AFFECTED,
  h.n_returned_rows AS ROW_SENT,
  NULL AS ROW_EXAMINED, 
  NULL AS CREATED_TMP_DISK_TABLES, 
  NULL AS CREATED_TMP_TABLES, 
  NULL AS SELECT_FULL_JOIN, 
  NULL AS SELECT_FULL_RANGE_JOIN, 
  NULL AS SELECT_RANGE, 
  NULL AS SELECT_RANGE_CHECK, 
  NULL AS SELECT_SCAN, 
  NULL AS SORT_MERGE_PASSES, 
  NULL AS SORT_RANGE, 
  NULL AS SORT_ROWS, 
  NULL AS SORT_SCAN, 
  NULL AS NO_INDEX_USED, 
  NULL AS NO_GOOD_INDEX_USED, 
  NULL AS NESTING_EVENT_ID, 
  NULL AS NESTING_EVENT_TYPE, 
  NULL AS NESTING_EVENT_LEVEL, 
  h.debug_query_id AS STATEMENT_ID, 
  h.CPU_TIME AS CPU_TIME,    
  NULL AS MAX_CONTROLLED_MEMORY, 
  NULL AS MAX_TOTAL_MEMORY, 
  NULL AS EXECUTION_ENGINE 
  FROM performance_schema.statement_history h 
  left outer join dbe_perf.statement t
  on h.unique_query_id = t.unique_sql_id
  inner join pg_catalog.pg_stat_activity s
  on h.session_id = s.sessionid
;
/* ----------------------
 *    Create performance_schema.events_statements_summary_by_digest
 * ----------------------
 */
create view performance_schema.events_statements_summary_by_digest as 
SELECT 
  h.schema_name AS SCHEMA_NAME, 
  SHA2(t.QUERY,256) AS DIGEST,
  t.QUERY AS DIGEST_TEXT,
  t.N_CALLS AS COUNT_STAR,
  t.TOTAL_ELAPSE_TIME AS SUM_TIMER_WAIT,
  t.MIN_ELAPSE_TIME AS MIN_TIMER_WAIT,
  CASE
    WHEN t.N_CALLS THEN round(t.TOTAL_ELAPSE_TIME/t.N_CALLS)
    ELSE 0
  END AS AVG_TIMER_WAIT,
  t.MAX_ELAPSE_TIME AS MAX_TIMER_WAIT,
  NULL AS SUM_LOCK_TIME,  
  NULL AS SUM_ERRORS,  
  NULL AS SUM_WARNINGS, 
  (t.n_tuples_deleted+t.n_tuples_inserted+t.n_tuples_inserted) AS SUM_ROWS_AFFECTED,
  t.N_RETURNED_ROWS AS SUM_ROWS_SENT,
  NULL AS SUM_ROWS_EXAMINED,
  NULL AS SUM_CREATED_TMP_DISK_TABLES,
  NULL AS SUM_CREATED_TMP_TABLES,
  NULL AS SUM_SELECT_FULL_JOIN,
  NULL AS SUM_SELECT_FULL_RANGE_JOIN,
  NULL AS SUM_SELECT_RANGE,
  NULL AS SUM_SELECT_RANGE_CHECK,
  NULL AS SUM_SELECT_SCAN,
  NULL AS SUM_SORT_MERGE_PASSES,
  NULL AS SUM_SORT_RANGE,
  NULL AS SUM_SORT_ROWS,
  t.sort_count AS SUM_SORT_SCAN,
  NULL AS SUM_NO_INDEX_USED,
  NULL AS SUM_NO_GOOD_INDEX_USED,
  t.CPU_TIME AS SUM_CPU_TIME,
  NULL AS MAX_CONTROLLED_MEMORY,
  NULL AS MAX_TOTAL_MEMORY,
  NULL AS COUNT_SECONDARY,
  NULL AS FIRST_SEEN,
  t.last_updated AS LAST_SEEN,
  NULL AS QUANTILE_95,
  NULL AS QUANTILE_99,
  NULL AS QUANTILE_999,
  NULL AS QUERY_SAMPLE_TEXT,
  NULL AS QUERY_SAMPLE_SEEN,
  NULL AS QUERY_SAMPLE_TIMER_WAIT
FROM dbe_perf.statement t
left outer join performance_schema.statement_history h
on h.unique_query_id = t.unique_sql_id
;
  
  
create view performance_schema.events_waits_current as 
SELECT 
  t.tid AS THREAD_ID,
  NULL AS EVENT_ID,
  NULL AS END_EVENT_ID,
  concat(concat(concat(concat(t.thread_name,'/'),t.WAIT_STATUS),'/'),t.WAIT_EVENT) AS EVENT_NAME,
  s.application_name AS SOURCE,
  s.query_start AS TIMER_START,
  CASE
    WHEN s.state = 'active' THEN CURRENT_TIMESTAMP
    ELSE s.state_change
  END AS TIMER_END,
  CASE
    WHEN s.state = 'active' THEN CURRENT_TIMESTAMP - s.query_start
    ELSE s.state_change - s.query_start
  END AS TIMER_WAIT,
  NULL AS SPINS,
  n.nspname AS OBJECT_SCHEMA,
  CASE
    WHEN c.reltype !=0 THEN c.relname
    ELSE NULL 
  END AS OBJECT_NAME,
  CASE
    WHEN c.reltype = 0 THEN c.relname
    ELSE NULL
  END AS INDEX_NAME,
  l.locktype AS OBJECT_TYPE,
  NULL AS OBJECT_INSTANCE_BEGIN,
  NULL AS NESTING_EVENT_ID,
  NULL AS NESTING_EVENT_TYPE,
  NULL AS OPERATION,
  NULL AS NUMBER_OF_BYTES,
  NULL AS FLAGS
FROM pg_catalog.pg_thread_wait_status t
inner join pg_catalog.pg_stat_activity s
  on t.sessionid = s.sessionid
  LEFT OUTER join pg_catalog.pg_locks l
  on t.locktag = l.locktag and l.pid = s.pid
  LEFT OUTER join pg_catalog.pg_class c
  on l.relation = c.oid 
  LEFT OUTER join pg_catalog.pg_namespace n
  on c.relnamespace = n.oid;
      

  
CREATE VIEW performance_schema.events_waits_summary_global_by_event_name AS
SELECT 
  concat(concat(w.type,''),w.event) AS EVENT_NAME,
  w.wait AS COUNT_STAR,
  w.total_wait_time as SUM_TIMER_WAIT,
  w.min_wait_time as MIN_TIMER_WAIT,
  w.avg_wait_time as AVG_TIMER_WAIT,
  w.max_wait_time as MAX_TIMER_WAIT
FROM dbe_perf.wait_events as w;


CREATE VIEW performance_schema.file_summary_by_instance AS
SELECT 
f.filenum AS FILE_NAME,
NULL AS EVENT_NAME,
NULL AS OBJECT_INSTANCE_BEGIN,
NULL AS COUNT_STAR,
NULL AS SUM_TIMER_WAIT,
NULL AS MIN_TIMER_WAIT,
NULL AS AVG_TIMER_WAIT,
NULL AS MAX_TIMER_WAIT,
f.phyrds AS COUNT_READ,
f.readtim AS SUM_TIMER_READ,
f.miniotim AS MIN_TIMER_READ, 
f.avgiotim AS AVG_TIMER_READ,
f.maxiowtm AS MAX_TIMER_READ,
f.phyblkrd*8192 AS SUM_NUMBER_OF_BYTES_READ, 
f.phywrts AS COUNT_WRITE,
f.writetim AS SUM_TIMER_WRITE,
f.miniotim AS MIN_TIMER_WRITE, 
f.avgiotim AVG_TIMER_WRITE,
f.maxiowtm AS MAX_TIMER_WRITE,
f.phyblkwrt*8192 AS SUM_NUMBER_OF_BYTES_WRITE,
NULL AS COUNT_MISC,
NULL AS SUM_TIMER_MISC,
NULL AS MIN_TIMER_MISC,
NULL AS AVG_TIMER_MISC,
NULL AS MAX_TIMER_MISC
FROM dbe_perf.file_iostat f;

CREATE VIEW performance_schema.table_io_waits_summary_by_table AS
SELECT
'TABLE' AS OBJECT_TYPE,  
n.nspname AS OBJECT_SCHEMA,
c.relname AS OBJECT_NAME,
NULL AS       COUNT_STAR,
NULL AS   SUM_TIMER_WAIT,
NULL AS   MIN_TIMER_WAIT,
NULL AS   AVG_TIMER_WAIT,
NULL AS   MAX_TIMER_WAIT,
SUM(f.phyrds) AS       COUNT_READ,
SUM(f.readtim) AS   SUM_TIMER_READ,
MIN(f.miniotim) AS   MIN_TIMER_READ,
AVG(f.avgiotim) AS   AVG_TIMER_READ,
MAX(f.maxiowtm) AS   MAX_TIMER_READ,
SUM(f.phywrts) AS      COUNT_WRITE,
SUM(f.writetim) AS  SUM_TIMER_WRITE,
MIN(f.miniotim) AS  MIN_TIMER_WRITE,
AVG(f.avgiotim) AS  AVG_TIMER_WRITE,
MAX(f.maxiowtm) AS  MAX_TIMER_WRITE,
NULL AS      COUNT_FETCH,
NULL AS  SUM_TIMER_FETCH,
NULL AS  MIN_TIMER_FETCH,
NULL AS  AVG_TIMER_FETCH,
NULL AS  MAX_TIMER_FETCH,
NULL AS     COUNT_INSERT,
NULL AS SUM_TIMER_INSERT,
NULL AS MIN_TIMER_INSERT,
NULL AS AVG_TIMER_INSERT,
NULL AS MAX_TIMER_INSERT,
NULL AS     COUNT_UPDATE,
NULL AS SUM_TIMER_UPDATE,
NULL AS MIN_TIMER_UPDATE,
NULL AS AVG_TIMER_UPDATE,
NULL AS MAX_TIMER_UPDATE,
NULL AS     COUNT_DELETE,
NULL AS SUM_TIMER_DELETE,
NULL AS MIN_TIMER_DELETE,
NULL AS AVG_TIMER_DELETE,
NULL AS MAX_TIMER_DELETE
FROM dbe_perf.file_iostat f, pg_class c, pg_namespace n
WHERE f.filenum = c.relfilenode
and c.relnamespace = n.oid
and c.relkind = 'r'
GROUP BY OBJECT_SCHEMA,OBJECT_NAME;

CREATE VIEW performance_schema.table_io_waits_summary_by_index_usage AS
SELECT
'TABLE' AS OBJECT_TYPE,  
n.nspname AS OBJECT_SCHEMA,
t.relname AS OBJECT_NAME,
c.relname AS INDEX_NAME,
NULL AS       COUNT_STAR,
NULL AS   SUM_TIMER_WAIT,
NULL AS   MIN_TIMER_WAIT,
NULL AS   AVG_TIMER_WAIT,
NULL AS   MAX_TIMER_WAIT,
SUM(f.phyrds) AS       COUNT_READ,
SUM(f.readtim) AS   SUM_TIMER_READ,
MIN(f.miniotim) AS   MIN_TIMER_READ,
AVG(f.avgiotim) AS   AVG_TIMER_READ,
MAX(f.maxiowtm) AS   MAX_TIMER_READ,
SUM(f.phywrts) AS      COUNT_WRITE,
SUM(f.writetim) AS  SUM_TIMER_WRITE,
MIN(f.miniotim) AS  MIN_TIMER_WRITE,
AVG(f.avgiotim) AS  AVG_TIMER_WRITE,
MAX(f.maxiowtm) AS  MAX_TIMER_WRITE,
NULL AS      COUNT_FETCH,
NULL AS  SUM_TIMER_FETCH,
NULL AS  MIN_TIMER_FETCH,
NULL AS  AVG_TIMER_FETCH,
NULL AS  MAX_TIMER_FETCH,
NULL AS     COUNT_INSERT,
NULL AS SUM_TIMER_INSERT,
NULL AS MIN_TIMER_INSERT,
NULL AS AVG_TIMER_INSERT,
NULL AS MAX_TIMER_INSERT,
NULL AS     COUNT_UPDATE,
NULL AS SUM_TIMER_UPDATE,
NULL AS MIN_TIMER_UPDATE,
NULL AS AVG_TIMER_UPDATE,
NULL AS MAX_TIMER_UPDATE,
NULL AS     COUNT_DELETE,
NULL AS SUM_TIMER_DELETE,
NULL AS MIN_TIMER_DELETE,
NULL AS AVG_TIMER_DELETE,
NULL AS MAX_TIMER_DELETE
FROM dbe_perf.file_iostat f, pg_class c, pg_namespace n,pg_index i, pg_class t
WHERE f.filenum = c.relfilenode
and c.relnamespace = n.oid
and c.relkind in ( 'i','I') 
and c.oid = i.indexrelid
and t.oid = i.indrelid
GROUP BY OBJECT_SCHEMA,OBJECT_NAME,INDEX_NAME;

GRANT USAGE ON schema performance_schema TO PUBLIC; 

GRANT SELECT,REFERENCES on all tables in schema performance_schema to public;

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

-- binary/varbinary -> text implicit
DROP FUNCTION IF EXISTS pg_catalog.hex(binary) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.hex(varbinary) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.uncompress(binary) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.uncompress(varbinary) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.uncompressed_length(binary) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.uncompressed_length(varbinary) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.weight_string(binary, uint4) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.weight_string(varbinary, uint4) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.weight_string(binary, text, uint4) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.weight_string(varbinary, text, uint4) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.weight_string(binary, text, uint4, uint4) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.weight_string(varbinary, text, uint4, uint4) CASCADE;

CREATE OR REPLACE FUNCTION pg_catalog.hex(binary) RETURNS text LANGUAGE SQL IMMUTABLE STRICT as $$ SELECT pg_catalog.hex($1::bytea) $$;
CREATE OR REPLACE FUNCTION pg_catalog.hex(varbinary) RETURNS text LANGUAGE SQL IMMUTABLE STRICT as $$ SELECT pg_catalog.hex($1::bytea) $$;

CREATE OR REPLACE FUNCTION pg_catalog.uncompress(binary) RETURNS text LANGUAGE SQL IMMUTABLE STRICT as $$ SELECT pg_catalog.uncompress($1::bytea) $$;
CREATE OR REPLACE FUNCTION pg_catalog.uncompress(varbinary) RETURNS text LANGUAGE SQL IMMUTABLE STRICT as $$ SELECT pg_catalog.uncompress($1::bytea) $$;

CREATE OR REPLACE FUNCTION pg_catalog.uncompressed_length(binary) RETURNS uint4 LANGUAGE SQL IMMUTABLE STRICT as $$ SELECT pg_catalog.uncompressed_length($1::bytea) $$;
CREATE OR REPLACE FUNCTION pg_catalog.uncompressed_length(varbinary) RETURNS uint4 LANGUAGE SQL IMMUTABLE STRICT as $$ SELECT pg_catalog.uncompressed_length($1::bytea) $$;

CREATE OR REPLACE FUNCTION pg_catalog.weight_string(binary, uint4) RETURNS TEXT LANGUAGE SQL IMMUTABLE STRICT as $$ SELECT pg_catalog.weight_string($1::bytea, $2) $$;
CREATE OR REPLACE FUNCTION pg_catalog.weight_string(varbinary, uint4) RETURNS TEXT LANGUAGE SQL IMMUTABLE STRICT as $$ SELECT pg_catalog.weight_string($1::bytea, $2) $$;

CREATE OR REPLACE FUNCTION pg_catalog.weight_string(binary, text, uint4) RETURNS TEXT LANGUAGE SQL IMMUTABLE STRICT as $$ SELECT pg_catalog.weight_string($1::bytea, $2, $3) $$;
CREATE OR REPLACE FUNCTION pg_catalog.weight_string(varbinary, text, uint4) RETURNS TEXT LANGUAGE SQL IMMUTABLE STRICT as $$ SELECT pg_catalog.weight_string($1::bytea, $2, $3) $$;

CREATE OR REPLACE FUNCTION pg_catalog.weight_string(binary, text, uint4, uint4) RETURNS TEXT LANGUAGE SQL IMMUTABLE STRICT as $$ SELECT pg_catalog.weight_string($1::bytea, $2, $3, $4) $$;
CREATE OR REPLACE FUNCTION pg_catalog.weight_string(varbinary, text, uint4, uint4) RETURNS TEXT LANGUAGE SQL IMMUTABLE STRICT as $$ SELECT pg_catalog.weight_string($1::bytea, $2, $3, $4) $$;

DROP FUNCTION IF EXISTS pg_catalog.bpcharbinarylike(char, binary) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.bpcharbinarynlike(char, binary) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.namebinarylike(name, binary) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.namebinarynlike(name, binary) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.binarylike(binary, binary) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.binarynlike(binary, binary) CASCADE;

CREATE OR REPLACE FUNCTION pg_catalog.bpcharbinarylike(
char,
binary
) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT as $$ SELECT pg_catalog.textbinarylike($1::text, $2) $$;

CREATE OPERATOR pg_catalog.~~(leftarg = char, rightarg = binary, procedure = pg_catalog.bpcharbinarylike);
CREATE OPERATOR pg_catalog.~~*(leftarg = char, rightarg = binary, procedure = pg_catalog.bpcharbinarylike);

CREATE OR REPLACE FUNCTION pg_catalog.bpcharbinarynlike(
char,
binary
) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT as $$ SELECT pg_catalog.textbinarynlike($1::text, $2) $$;

CREATE OPERATOR pg_catalog.!~~(leftarg = char, rightarg = binary, procedure = pg_catalog.bpcharbinarynlike);
CREATE OPERATOR pg_catalog.!~~*(leftarg = char, rightarg = binary, procedure = pg_catalog.bpcharbinarynlike);

CREATE OR REPLACE FUNCTION pg_catalog.namebinarylike(
name,
binary
) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT as $$ SELECT pg_catalog.textbinarylike($1::text, $2) $$;

CREATE OPERATOR pg_catalog.~~(leftarg = name, rightarg = binary, procedure = pg_catalog.namebinarylike);
CREATE OPERATOR pg_catalog.~~*(leftarg = name, rightarg = binary, procedure = pg_catalog.namebinarylike);

CREATE OR REPLACE FUNCTION pg_catalog.namebinarynlike(
name,
binary
) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT as $$ SELECT pg_catalog.textbinarynlike($1::text, $2) $$;

CREATE OPERATOR pg_catalog.!~~(leftarg = name, rightarg = binary, procedure = pg_catalog.namebinarynlike);
CREATE OPERATOR pg_catalog.!~~*(leftarg = name, rightarg = binary, procedure = pg_catalog.namebinarynlike);

CREATE OR REPLACE FUNCTION pg_catalog.binarylike(
binary,
binary
) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT as $$ SELECT pg_catalog.bytealike($1::bytea, $2::bytea) $$;

CREATE OPERATOR pg_catalog.~~(leftarg = binary, rightarg = binary, procedure = pg_catalog.binarylike);
CREATE OPERATOR pg_catalog.~~*(leftarg = binary, rightarg = binary, procedure = pg_catalog.binarylike);

CREATE OR REPLACE FUNCTION pg_catalog.binarynlike(
binary,
binary
) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT as $$ SELECT pg_catalog.byteanlike($1::bytea, $2::bytea) $$;

CREATE OPERATOR pg_catalog.!~~(leftarg = binary, rightarg = binary, procedure = pg_catalog.binarynlike);
CREATE OPERATOR pg_catalog.!~~*(leftarg = binary, rightarg = binary, procedure = pg_catalog.binarynlike);

-- lt
DROP FUNCTION IF EXISTS pg_catalog.time_binary_lt(time, binary) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.numeric_binary_lt(numeric, binary) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.uint1_binary_lt(uint1, binary) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.uint2_binary_lt(uint2, binary) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.uint4_binary_lt(uint4, binary) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.uint8_binary_lt(uint8, binary) CASCADE;

CREATE OR REPLACE FUNCTION pg_catalog.time_binary_lt(
time,
binary
) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT as $$ SELECT pg_catalog.float8lt($1::float8, $2::float8) $$;
CREATE OPERATOR pg_catalog.<(leftarg = time, rightarg = binary, procedure = pg_catalog.time_binary_lt);

CREATE OR REPLACE FUNCTION pg_catalog.numeric_binary_lt(
numeric,
binary
) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT as $$ SELECT pg_catalog.float8lt($1::float8, $2::float8) $$;
CREATE OPERATOR pg_catalog.<(leftarg = numeric, rightarg = binary, procedure = pg_catalog.numeric_binary_lt);

CREATE OR REPLACE FUNCTION pg_catalog.uint1_binary_lt(
uint1,
binary
) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT as $$ SELECT pg_catalog.float8lt($1::float8, $2::float8) $$;
CREATE OPERATOR pg_catalog.<(leftarg = uint1, rightarg = binary, procedure = pg_catalog.uint1_binary_lt);

CREATE OR REPLACE FUNCTION pg_catalog.uint2_binary_lt(
uint2,
binary
) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT as $$ SELECT pg_catalog.float8lt($1::float8, $2::float8) $$;
CREATE OPERATOR pg_catalog.<(leftarg = uint2, rightarg = binary, procedure = pg_catalog.uint2_binary_lt);

CREATE OR REPLACE FUNCTION pg_catalog.uint4_binary_lt(
uint4,
binary
) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT as $$ SELECT pg_catalog.float8lt($1::float8, $2::float8) $$;
CREATE OPERATOR pg_catalog.<(leftarg = uint4, rightarg = binary, procedure = pg_catalog.uint4_binary_lt);

CREATE OR REPLACE FUNCTION pg_catalog.uint8_binary_lt(
uint8,
binary
) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT as $$ SELECT pg_catalog.float8lt($1::float8, $2::float8) $$;
CREATE OPERATOR pg_catalog.<(leftarg = uint8, rightarg = binary, procedure = pg_catalog.uint8_binary_lt);

DROP FUNCTION IF EXISTS pg_catalog.binary_time_lt(binary, time) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.binary_numeric_lt(binary, numeric) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.binary_uint1_lt(binary, uint1) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.binary_uint2_lt(binary, uint2) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.binary_uint4_lt(binary, uint4) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.binary_uint8_lt(binary, uint8) CASCADE;

CREATE OR REPLACE FUNCTION pg_catalog.binary_time_lt(
binary,
time
) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT as $$ SELECT pg_catalog.float8lt($1::float8, $2::float8) $$;
CREATE OPERATOR pg_catalog.<(leftarg = binary, rightarg = time, procedure = pg_catalog.binary_time_lt);

CREATE OR REPLACE FUNCTION pg_catalog.binary_numeric_lt(
binary,
numeric
) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT as $$ SELECT pg_catalog.float8lt($1::float8, $2::float8) $$;
CREATE OPERATOR pg_catalog.<(leftarg = binary, rightarg = numeric, procedure = pg_catalog.binary_numeric_lt);

CREATE OR REPLACE FUNCTION pg_catalog.binary_uint1_lt(
binary,
uint1
) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT as $$ SELECT pg_catalog.float8lt($1::float8, $2::float8) $$;
CREATE OPERATOR pg_catalog.<(leftarg = binary, rightarg = uint1, procedure = pg_catalog.binary_uint1_lt);

CREATE OR REPLACE FUNCTION pg_catalog.binary_uint2_lt(
binary,
uint2
) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT as $$ SELECT pg_catalog.float8lt($1::float8, $2::float8) $$;
CREATE OPERATOR pg_catalog.<(leftarg = binary, rightarg = uint2, procedure = pg_catalog.binary_uint2_lt);

CREATE OR REPLACE FUNCTION pg_catalog.binary_uint4_lt(
binary,
uint4
) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT as $$ SELECT pg_catalog.float8lt($1::float8, $2::float8) $$;
CREATE OPERATOR pg_catalog.<(leftarg = binary, rightarg = uint4, procedure = pg_catalog.binary_uint4_lt);

CREATE OR REPLACE FUNCTION pg_catalog.binary_uint8_lt(
binary,
uint8
) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT as $$ SELECT pg_catalog.float8lt($1::float8, $2::float8) $$;
CREATE OPERATOR pg_catalog.<(leftarg = binary, rightarg = uint8, procedure = pg_catalog.binary_uint8_lt);

-- gt
DROP FUNCTION IF EXISTS pg_catalog.time_binary_gt(time, binary) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.numeric_binary_gt(numeric, binary) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.uint1_binary_gt(uint1, binary) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.uint2_binary_gt(uint2, binary) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.uint4_binary_gt(uint4, binary) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.uint8_binary_gt(uint8, binary) CASCADE;

CREATE OR REPLACE FUNCTION pg_catalog.time_binary_gt(
time,
binary
) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT as $$ SELECT pg_catalog.float8gt($1::float8, $2::float8) $$;
CREATE OPERATOR pg_catalog.>(leftarg = time, rightarg = binary, procedure = pg_catalog.time_binary_gt);

CREATE OR REPLACE FUNCTION pg_catalog.numeric_binary_gt(
numeric,
binary
) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT as $$ SELECT pg_catalog.float8gt($1::float8, $2::float8) $$;
CREATE OPERATOR pg_catalog.>(leftarg = numeric, rightarg = binary, procedure = pg_catalog.numeric_binary_gt);

CREATE OR REPLACE FUNCTION pg_catalog.uint1_binary_gt(
uint1,
binary
) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT as $$ SELECT pg_catalog.float8gt($1::float8, $2::float8) $$;
CREATE OPERATOR pg_catalog.>(leftarg = uint1, rightarg = binary, procedure = pg_catalog.uint1_binary_gt);

CREATE OR REPLACE FUNCTION pg_catalog.uint2_binary_gt(
uint2,
binary
) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT as $$ SELECT pg_catalog.float8gt($1::float8, $2::float8) $$;
CREATE OPERATOR pg_catalog.>(leftarg = uint2, rightarg = binary, procedure = pg_catalog.uint2_binary_gt);

CREATE OR REPLACE FUNCTION pg_catalog.uint4_binary_gt(
uint4,
binary
) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT as $$ SELECT pg_catalog.float8gt($1::float8, $2::float8) $$;
CREATE OPERATOR pg_catalog.>(leftarg = uint4, rightarg = binary, procedure = pg_catalog.uint4_binary_gt);

CREATE OR REPLACE FUNCTION pg_catalog.uint8_binary_gt(
uint8,
binary
) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT as $$ SELECT pg_catalog.float8gt($1::float8, $2::float8) $$;
CREATE OPERATOR pg_catalog.>(leftarg = uint8, rightarg = binary, procedure = pg_catalog.uint8_binary_gt);

DROP FUNCTION IF EXISTS pg_catalog.binary_time_gt(binary, time) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.binary_numeric_gt(binary, numeric) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.binary_uint1_gt(binary, uint1) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.binary_uint2_gt(binary, uint2) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.binary_uint4_gt(binary, uint4) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.binary_uint8_gt(binary, uint8) CASCADE;

CREATE OR REPLACE FUNCTION pg_catalog.binary_time_gt(
binary,
time
) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT as $$ SELECT pg_catalog.float8gt($1::float8, $2::float8) $$;
CREATE OPERATOR pg_catalog.>(leftarg = binary, rightarg = time, procedure = pg_catalog.binary_time_gt);

CREATE OR REPLACE FUNCTION pg_catalog.binary_numeric_gt(
binary,
numeric
) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT as $$ SELECT pg_catalog.float8gt($1::float8, $2::float8) $$;
CREATE OPERATOR pg_catalog.>(leftarg = binary, rightarg = numeric, procedure = pg_catalog.binary_numeric_gt);

CREATE OR REPLACE FUNCTION pg_catalog.binary_uint1_gt(
binary,
uint1
) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT as $$ SELECT pg_catalog.float8gt($1::float8, $2::float8) $$;
CREATE OPERATOR pg_catalog.>(leftarg = binary, rightarg = uint1, procedure = pg_catalog.binary_uint1_gt);

CREATE OR REPLACE FUNCTION pg_catalog.binary_uint2_gt(
binary,
uint2
) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT as $$ SELECT pg_catalog.float8gt($1::float8, $2::float8) $$;
CREATE OPERATOR pg_catalog.>(leftarg = binary, rightarg = uint2, procedure = pg_catalog.binary_uint2_gt);

CREATE OR REPLACE FUNCTION pg_catalog.binary_uint4_gt(
binary,
uint4
) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT as $$ SELECT pg_catalog.float8gt($1::float8, $2::float8) $$;
CREATE OPERATOR pg_catalog.>(leftarg = binary, rightarg = uint4, procedure = pg_catalog.binary_uint4_gt);

CREATE OR REPLACE FUNCTION pg_catalog.binary_uint8_gt(
binary,
uint8
) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT as $$ SELECT pg_catalog.float8gt($1::float8, $2::float8) $$;
CREATE OPERATOR pg_catalog.>(leftarg = binary, rightarg = uint8, procedure = pg_catalog.binary_uint8_gt);

-- le
DROP FUNCTION IF EXISTS pg_catalog.time_binary_le(time, binary) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.numeric_binary_le(numeric, binary) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.uint1_binary_le(uint1, binary) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.uint2_binary_le(uint2, binary) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.uint4_binary_le(uint4, binary) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.uint8_binary_le(uint8, binary) CASCADE;

CREATE OR REPLACE FUNCTION pg_catalog.time_binary_le(
time,
binary
) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT as $$ SELECT pg_catalog.float8le($1::float8, $2::float8) $$;
CREATE OPERATOR pg_catalog.<=(leftarg = time, rightarg = binary, procedure = pg_catalog.time_binary_le);

CREATE OR REPLACE FUNCTION pg_catalog.numeric_binary_le(
numeric,
binary
) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT as $$ SELECT pg_catalog.float8le($1::float8, $2::float8) $$;
CREATE OPERATOR pg_catalog.<=(leftarg = numeric, rightarg = binary, procedure = pg_catalog.numeric_binary_le);

CREATE OR REPLACE FUNCTION pg_catalog.uint1_binary_le(
uint1,
binary
) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT as $$ SELECT pg_catalog.float8le($1::float8, $2::float8) $$;
CREATE OPERATOR pg_catalog.<=(leftarg = uint1, rightarg = binary, procedure = pg_catalog.uint1_binary_le);

CREATE OR REPLACE FUNCTION pg_catalog.uint2_binary_le(
uint2,
binary
) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT as $$ SELECT pg_catalog.float8le($1::float8, $2::float8) $$;
CREATE OPERATOR pg_catalog.<=(leftarg = uint2, rightarg = binary, procedure = pg_catalog.uint2_binary_le);

CREATE OR REPLACE FUNCTION pg_catalog.uint4_binary_le(
uint4,
binary
) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT as $$ SELECT pg_catalog.float8le($1::float8, $2::float8) $$;
CREATE OPERATOR pg_catalog.<=(leftarg = uint4, rightarg = binary, procedure = pg_catalog.uint4_binary_le);

CREATE OR REPLACE FUNCTION pg_catalog.uint8_binary_le(
uint8,
binary
) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT as $$ SELECT pg_catalog.float8le($1::float8, $2::float8) $$;
CREATE OPERATOR pg_catalog.<=(leftarg = uint8, rightarg = binary, procedure = pg_catalog.uint8_binary_le);

DROP FUNCTION IF EXISTS pg_catalog.binary_time_le(binary, time) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.binary_numeric_le(binary, numeric) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.binary_uint1_le(binary, uint1) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.binary_uint2_le(binary, uint2) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.binary_uint4_le(binary, uint4) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.binary_uint8_le(binary, uint8) CASCADE;

CREATE OR REPLACE FUNCTION pg_catalog.binary_time_le(
binary,
time
) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT as $$ SELECT pg_catalog.float8le($1::float8, $2::float8) $$;
CREATE OPERATOR pg_catalog.<=(leftarg = binary, rightarg = time, procedure = pg_catalog.binary_time_le);

CREATE OR REPLACE FUNCTION pg_catalog.binary_numeric_le(
binary,
numeric
) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT as $$ SELECT pg_catalog.float8le($1::float8, $2::float8) $$;
CREATE OPERATOR pg_catalog.<=(leftarg = binary, rightarg = numeric, procedure = pg_catalog.binary_numeric_le);

CREATE OR REPLACE FUNCTION pg_catalog.binary_uint1_le(
binary,
uint1
) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT as $$ SELECT pg_catalog.float8le($1::float8, $2::float8) $$;
CREATE OPERATOR pg_catalog.<=(leftarg = binary, rightarg = uint1, procedure = pg_catalog.binary_uint1_le);

CREATE OR REPLACE FUNCTION pg_catalog.binary_uint2_le(
binary,
uint2
) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT as $$ SELECT pg_catalog.float8le($1::float8, $2::float8) $$;
CREATE OPERATOR pg_catalog.<=(leftarg = binary, rightarg = uint2, procedure = pg_catalog.binary_uint2_le);

CREATE OR REPLACE FUNCTION pg_catalog.binary_uint4_le(
binary,
uint4
) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT as $$ SELECT pg_catalog.float8le($1::float8, $2::float8) $$;
CREATE OPERATOR pg_catalog.<=(leftarg = binary, rightarg = uint4, procedure = pg_catalog.binary_uint4_le);

CREATE OR REPLACE FUNCTION pg_catalog.binary_uint8_le(
binary,
uint8
) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT as $$ SELECT pg_catalog.float8le($1::float8, $2::float8) $$;
CREATE OPERATOR pg_catalog.<=(leftarg = binary, rightarg = uint8, procedure = pg_catalog.binary_uint8_le);

-- ge
DROP FUNCTION IF EXISTS pg_catalog.time_binary_ge(time, binary) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.numeric_binary_ge(numeric, binary) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.uint1_binary_ge(uint1, binary) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.uint2_binary_ge(uint2, binary) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.uint4_binary_ge(uint4, binary) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.uint8_binary_ge(uint8, binary) CASCADE;

CREATE OR REPLACE FUNCTION pg_catalog.time_binary_ge(
time,
binary
) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT as $$ SELECT pg_catalog.float8ge($1::float8, $2::float8) $$;
CREATE OPERATOR pg_catalog.>=(leftarg = time, rightarg = binary, procedure = pg_catalog.time_binary_ge);

CREATE OR REPLACE FUNCTION pg_catalog.numeric_binary_ge(
numeric,
binary
) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT as $$ SELECT pg_catalog.float8ge($1::float8, $2::float8) $$;
CREATE OPERATOR pg_catalog.>=(leftarg = numeric, rightarg = binary, procedure = pg_catalog.numeric_binary_ge);

CREATE OR REPLACE FUNCTION pg_catalog.uint1_binary_ge(
uint1,
binary
) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT as $$ SELECT pg_catalog.float8ge($1::float8, $2::float8) $$;
CREATE OPERATOR pg_catalog.>=(leftarg = uint1, rightarg = binary, procedure = pg_catalog.uint1_binary_ge);

CREATE OR REPLACE FUNCTION pg_catalog.uint2_binary_ge(
uint2,
binary
) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT as $$ SELECT pg_catalog.float8ge($1::float8, $2::float8) $$;
CREATE OPERATOR pg_catalog.>=(leftarg = uint2, rightarg = binary, procedure = pg_catalog.uint2_binary_ge);

CREATE OR REPLACE FUNCTION pg_catalog.uint4_binary_ge(
uint4,
binary
) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT as $$ SELECT pg_catalog.float8ge($1::float8, $2::float8) $$;
CREATE OPERATOR pg_catalog.>=(leftarg = uint4, rightarg = binary, procedure = pg_catalog.uint4_binary_ge);

CREATE OR REPLACE FUNCTION pg_catalog.uint8_binary_ge(
uint8,
binary
) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT as $$ SELECT pg_catalog.float8ge($1::float8, $2::float8) $$;
CREATE OPERATOR pg_catalog.>=(leftarg = uint8, rightarg = binary, procedure = pg_catalog.uint8_binary_ge);

DROP FUNCTION IF EXISTS pg_catalog.binary_time_ge(binary, time) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.binary_numeric_ge(binary, numeric) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.binary_uint1_ge(binary, uint1) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.binary_uint2_ge(binary, uint2) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.binary_uint4_ge(binary, uint4) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.binary_uint8_ge(binary, uint8) CASCADE;

CREATE OR REPLACE FUNCTION pg_catalog.binary_time_ge(
binary,
time
) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT as $$ SELECT pg_catalog.float8ge($1::float8, $2::float8) $$;
CREATE OPERATOR pg_catalog.>=(leftarg = binary, rightarg = time, procedure = pg_catalog.binary_time_ge);

CREATE OR REPLACE FUNCTION pg_catalog.binary_numeric_ge(
binary,
numeric
) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT as $$ SELECT pg_catalog.float8ge($1::float8, $2::float8) $$;
CREATE OPERATOR pg_catalog.>=(leftarg = binary, rightarg = numeric, procedure = pg_catalog.binary_numeric_ge);

CREATE OR REPLACE FUNCTION pg_catalog.binary_uint1_ge(
binary,
uint1
) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT as $$ SELECT pg_catalog.float8ge($1::float8, $2::float8) $$;
CREATE OPERATOR pg_catalog.>=(leftarg = binary, rightarg = uint1, procedure = pg_catalog.binary_uint1_ge);

CREATE OR REPLACE FUNCTION pg_catalog.binary_uint2_ge(
binary,
uint2
) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT as $$ SELECT pg_catalog.float8ge($1::float8, $2::float8) $$;
CREATE OPERATOR pg_catalog.>=(leftarg = binary, rightarg = uint2, procedure = pg_catalog.binary_uint2_ge);

CREATE OR REPLACE FUNCTION pg_catalog.binary_uint4_ge(
binary,
uint4
) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT as $$ SELECT pg_catalog.float8ge($1::float8, $2::float8) $$;
CREATE OPERATOR pg_catalog.>=(leftarg = binary, rightarg = uint4, procedure = pg_catalog.binary_uint4_ge);

CREATE OR REPLACE FUNCTION pg_catalog.binary_uint8_ge(
binary,
uint8
) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT as $$ SELECT pg_catalog.float8ge($1::float8, $2::float8) $$;
CREATE OPERATOR pg_catalog.>=(leftarg = binary, rightarg = uint8, procedure = pg_catalog.binary_uint8_ge);
-- uint explicit cast
-- time
DROP FUNCTION IF EXISTS pg_catalog.time_cast_ui1(time) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.time_cast_ui1 (
time
) RETURNS uint1 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'time_cast_ui1';

DROP FUNCTION IF EXISTS pg_catalog.time_cast_ui2(time) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.time_cast_ui2 (
time
) RETURNS uint2 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'time_cast_ui2';

DROP FUNCTION IF EXISTS pg_catalog.time_cast_ui4(time) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.time_cast_ui4 (
time
) RETURNS uint4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'time_cast_ui4';

DROP FUNCTION IF EXISTS pg_catalog.time_cast_ui8(time) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.time_cast_ui8 (
time
) RETURNS uint8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'time_cast_ui8';
-- char
DROP FUNCTION IF EXISTS pg_catalog.char_cast_ui1(char) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.char_cast_ui1 (
char
) RETURNS uint1 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'char_cast_ui1';

DROP FUNCTION IF EXISTS pg_catalog.char_cast_ui2(char) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.char_cast_ui2 (
char
) RETURNS uint2 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'char_cast_ui2';

DROP FUNCTION IF EXISTS pg_catalog.char_cast_ui4(char) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.char_cast_ui4 (
char
) RETURNS uint4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'char_cast_ui4';

DROP FUNCTION IF EXISTS pg_catalog.char_cast_ui8(char) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.char_cast_ui8 (
char
) RETURNS uint8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'char_cast_ui8';
-- varchar
DROP FUNCTION IF EXISTS pg_catalog.varchar_cast_ui1(char) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.varchar_cast_ui1 (
char
) RETURNS uint1 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'varchar_cast_ui1';

DROP FUNCTION IF EXISTS pg_catalog.varchar_cast_ui2(char) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.varchar_cast_ui2 (
char
) RETURNS uint2 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'varchar_cast_ui2';

DROP FUNCTION IF EXISTS pg_catalog.varchar_cast_ui4(char) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.varchar_cast_ui4 (
char
) RETURNS uint4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'varchar_cast_ui4';

DROP FUNCTION IF EXISTS pg_catalog.varchar_cast_ui8(char) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.varchar_cast_ui8 (
char
) RETURNS uint8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'varchar_cast_ui8';
--varlena
DROP FUNCTION IF EXISTS pg_catalog.varlena2ui1(anyelement) cascade;
DROP FUNCTION IF EXISTS pg_catalog.varlena2ui2(anyelement) cascade;
DROP FUNCTION IF EXISTS pg_catalog.varlena2ui4(anyelement) cascade;
DROP FUNCTION IF EXISTS pg_catalog.varlena2ui8(anyelement) cascade;

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

CREATE OR REPLACE FUNCTION pg_catalog.varlena2ui1 (
anyelement
) RETURNS uint1 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'varlena2ui1';

CREATE OR REPLACE FUNCTION pg_catalog.varlena2ui2 (
anyelement
) RETURNS uint2 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'varlena2ui2';

CREATE OR REPLACE FUNCTION pg_catalog.varlena2ui4 (
anyelement
) RETURNS uint4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'varlena2ui4';

CREATE OR REPLACE FUNCTION pg_catalog.varlena2ui8 (
anyelement
) RETURNS uint8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'varlena2ui8';

CREATE CAST ("binary" AS uint1) WITH FUNCTION pg_catalog.varlena2ui1(anyelement) AS ASSIGNMENT;
CREATE CAST ("varbinary" AS uint1) WITH FUNCTION pg_catalog.varlena2ui1(anyelement) AS ASSIGNMENT;
CREATE CAST (blob AS uint1) WITH FUNCTION pg_catalog.varlena2ui1(anyelement) AS ASSIGNMENT;
CREATE CAST (tinyblob AS uint1) WITH FUNCTION pg_catalog.varlena2ui1(anyelement) AS ASSIGNMENT;
CREATE CAST (mediumblob AS uint1) WITH FUNCTION pg_catalog.varlena2ui1(anyelement) AS ASSIGNMENT;
CREATE CAST (longblob AS uint1) WITH FUNCTION pg_catalog.varlena2ui1(anyelement) AS ASSIGNMENT;
CREATE CAST (json AS uint1) WITH FUNCTION pg_catalog.varlena2ui1(anyelement) AS ASSIGNMENT;

CREATE CAST ("binary" AS uint2) WITH FUNCTION pg_catalog.varlena2ui2(anyelement) AS ASSIGNMENT;
CREATE CAST ("varbinary" AS uint2) WITH FUNCTION pg_catalog.varlena2ui2(anyelement) AS ASSIGNMENT;
CREATE CAST (blob AS uint2) WITH FUNCTION pg_catalog.varlena2ui2(anyelement) AS ASSIGNMENT;
CREATE CAST (tinyblob AS uint2) WITH FUNCTION pg_catalog.varlena2ui2(anyelement) AS ASSIGNMENT;
CREATE CAST (mediumblob AS uint2) WITH FUNCTION pg_catalog.varlena2ui2(anyelement) AS ASSIGNMENT;
CREATE CAST (longblob AS uint2) WITH FUNCTION pg_catalog.varlena2ui2(anyelement) AS ASSIGNMENT;
CREATE CAST (json AS uint2) WITH FUNCTION pg_catalog.varlena2ui2(anyelement) AS ASSIGNMENT;

CREATE CAST ("binary" AS uint4) WITH FUNCTION pg_catalog.varlena2ui4(anyelement) AS ASSIGNMENT;
CREATE CAST ("varbinary" AS uint4) WITH FUNCTION pg_catalog.varlena2ui4(anyelement) AS ASSIGNMENT;
CREATE CAST (blob AS uint4) WITH FUNCTION pg_catalog.varlena2ui4(anyelement) AS ASSIGNMENT;
CREATE CAST (tinyblob AS uint4) WITH FUNCTION pg_catalog.varlena2ui4(anyelement) AS ASSIGNMENT;
CREATE CAST (mediumblob AS uint4) WITH FUNCTION pg_catalog.varlena2ui4(anyelement) AS ASSIGNMENT;
CREATE CAST (longblob AS uint4) WITH FUNCTION pg_catalog.varlena2ui4(anyelement) AS ASSIGNMENT;
CREATE CAST (json AS uint4) WITH FUNCTION pg_catalog.varlena2ui4(anyelement) AS ASSIGNMENT;

CREATE CAST ("binary" AS uint8) WITH FUNCTION pg_catalog.varlena2ui8(anyelement) AS ASSIGNMENT;
CREATE CAST ("varbinary" AS uint8) WITH FUNCTION pg_catalog.varlena2ui8(anyelement) AS ASSIGNMENT;
CREATE CAST (blob AS uint8) WITH FUNCTION pg_catalog.varlena2ui8(anyelement) AS ASSIGNMENT;
CREATE CAST (tinyblob AS uint8) WITH FUNCTION pg_catalog.varlena2ui8(anyelement) AS ASSIGNMENT;
CREATE CAST (mediumblob AS uint8) WITH FUNCTION pg_catalog.varlena2ui8(anyelement) AS ASSIGNMENT;
CREATE CAST (longblob AS uint8) WITH FUNCTION pg_catalog.varlena2ui8(anyelement) AS ASSIGNMENT;
CREATE CAST (json AS uint8) WITH FUNCTION pg_catalog.varlena2ui8(anyelement) AS ASSIGNMENT;

DROP FUNCTION IF EXISTS pg_catalog.varlena_cast_ui1(anyelement) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.varlena_cast_ui1 (
anyelement
) RETURNS uint1 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'varlena_cast_ui1';

DROP FUNCTION IF EXISTS pg_catalog.varlena_cast_ui2(anyelement) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.varlena_cast_ui2 (
anyelement
) RETURNS uint2 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'varlena_cast_ui2';

DROP FUNCTION IF EXISTS pg_catalog.varlena_cast_ui4(anyelement) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.varlena_cast_ui4 (
anyelement
) RETURNS uint4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'varlena_cast_ui4';

DROP FUNCTION IF EXISTS pg_catalog.varlena_cast_ui8(anyelement) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.varlena_cast_ui8 (
anyelement
) RETURNS uint8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'varlena_cast_ui8';


DROP FUNCTION IF EXISTS pg_catalog.time_format(timestamp without time zone, text) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.time_format (timestamp without time zone, text) RETURNS text LANGUAGE SQL IMMUTABLE STRICT as $$ SELECT pg_catalog.time_format($1::text, $2) $$;

DROP FUNCTION IF EXISTS pg_catalog.timestamp_add (text, timestamptz, "any") CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.timestamp_add (text, timestamptz, "any") RETURNS text LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'timestamp_add_timestamptz';

create function pg_catalog.timestamp_pl_int4(
    timestamptz,
    int4
) RETURNS timestamptz LANGUAGE SQL IMMUTABLE STRICT as $$ SELECT pg_catalog.timestamptz_pl_interval($1, $2::interval) $$;
create operator pg_catalog.+(leftarg = timestamptz, rightarg = int4, procedure = pg_catalog.timestamp_pl_int4);

create function pg_catalog.timestamp_mi_int4(
    timestamptz,
    int4
) RETURNS timestamptz LANGUAGE SQL IMMUTABLE STRICT as $$ SELECT pg_catalog.timestamptz_mi_interval($1, $2::interval) $$;
create operator pg_catalog.-(leftarg = timestamptz, rightarg = int4, procedure = pg_catalog.timestamp_mi_int4);

create function pg_catalog.datetime_pl_int4(
    timestamp without time zone,
    int4
) RETURNS float8 LANGUAGE SQL IMMUTABLE STRICT as $$ SELECT pg_catalog.datetime_pl_float($1, $2::float8) $$;
create operator pg_catalog.+(leftarg = timestamp without time zone, rightarg = int4, procedure = pg_catalog.datetime_pl_int4);

create function pg_catalog.datetime_mi_int4(
    timestamp without time zone,
    int4
) RETURNS float8 LANGUAGE SQL IMMUTABLE STRICT as $$ SELECT pg_catalog.datetime_mi_float($1, $2::float8) $$;
create operator pg_catalog.-(leftarg = timestamp without time zone, rightarg = int4, procedure = pg_catalog.datetime_mi_int4);

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

-- left operator
DROP FUNCTION IF EXISTS pg_catalog.json_uplus(json);
CREATE OR REPLACE FUNCTION pg_catalog.json_uplus(json) RETURNS json LANGUAGE C IMMUTABLE as '$libdir/dolphin','json_uplus';
CREATE OPERATOR pg_catalog.+(rightarg = json, procedure = pg_catalog.json_uplus);

-- binary cmp operator
DROP FUNCTION IF EXISTS pg_catalog.json_eq(json, "any");
CREATE OR REPLACE FUNCTION pg_catalog.json_eq(json, "any") RETURNS bool LANGUAGE C IMMUTABLE as '$libdir/dolphin','json_eq';
CREATE OPERATOR pg_catalog.=(leftarg = json, rightarg = "any", procedure = pg_catalog.json_eq);
DROP FUNCTION IF EXISTS pg_catalog.json_eq(text, json);
CREATE OR REPLACE FUNCTION pg_catalog.json_eq(text, json) RETURNS bool LANGUAGE C IMMUTABLE as '$libdir/dolphin','json_eq';
CREATE OPERATOR pg_catalog.=(leftarg = text, rightarg = json, procedure = pg_catalog.json_eq);
DROP FUNCTION IF EXISTS pg_catalog.json_eq(bit, json);
CREATE OR REPLACE FUNCTION pg_catalog.json_eq(bit, json) RETURNS bool LANGUAGE C IMMUTABLE as '$libdir/dolphin','json_eq';
CREATE OPERATOR pg_catalog.=(leftarg = bit, rightarg = json, procedure = pg_catalog.json_eq);
DROP FUNCTION IF EXISTS pg_catalog.json_eq(boolean, json);
CREATE OR REPLACE FUNCTION pg_catalog.json_eq(boolean, json) RETURNS bool LANGUAGE C IMMUTABLE as '$libdir/dolphin','json_eq';
CREATE OPERATOR pg_catalog.=(leftarg = boolean, rightarg = json, procedure = pg_catalog.json_eq);
DROP FUNCTION IF EXISTS pg_catalog.json_eq(year, json);
CREATE OR REPLACE FUNCTION pg_catalog.json_eq(year, json) RETURNS bool LANGUAGE C IMMUTABLE as '$libdir/dolphin','json_eq';
CREATE OPERATOR pg_catalog.=(leftarg = year, rightarg = json, procedure = pg_catalog.json_eq);

DROP FUNCTION IF EXISTS pg_catalog.json_ne(json, "any");
CREATE OR REPLACE FUNCTION pg_catalog.json_ne(json, "any") RETURNS bool LANGUAGE C IMMUTABLE as '$libdir/dolphin','json_ne';
CREATE OPERATOR pg_catalog.!=(leftarg = json, rightarg = "any", procedure = pg_catalog.json_ne);
DROP FUNCTION IF EXISTS pg_catalog.json_ne(text, json);
CREATE OR REPLACE FUNCTION pg_catalog.json_ne(text, json) RETURNS bool LANGUAGE C IMMUTABLE as '$libdir/dolphin','json_ne';
CREATE OPERATOR pg_catalog.!=(leftarg = text, rightarg = json, procedure = pg_catalog.json_ne);
DROP FUNCTION IF EXISTS pg_catalog.json_ne(bit, json);
CREATE OR REPLACE FUNCTION pg_catalog.json_ne(bit, json) RETURNS bool LANGUAGE C IMMUTABLE as '$libdir/dolphin','json_ne';
CREATE OPERATOR pg_catalog.!=(leftarg = bit, rightarg = json, procedure = pg_catalog.json_ne);
DROP FUNCTION IF EXISTS pg_catalog.json_ne(boolean, json);
CREATE OR REPLACE FUNCTION pg_catalog.json_ne(boolean, json) RETURNS bool LANGUAGE C IMMUTABLE as '$libdir/dolphin','json_ne';
CREATE OPERATOR pg_catalog.!=(leftarg = boolean, rightarg = json, procedure = pg_catalog.json_ne);
DROP FUNCTION IF EXISTS pg_catalog.json_ne(year, json);
CREATE OR REPLACE FUNCTION pg_catalog.json_ne(year, json) RETURNS bool LANGUAGE C IMMUTABLE as '$libdir/dolphin','json_ne';
CREATE OPERATOR pg_catalog.!=(leftarg = year, rightarg = json, procedure = pg_catalog.json_ne);

DROP FUNCTION IF EXISTS pg_catalog.json_gt(json, "any");
CREATE OR REPLACE FUNCTION pg_catalog.json_gt(json, "any") RETURNS bool LANGUAGE C IMMUTABLE as '$libdir/dolphin','json_gt';
CREATE OPERATOR pg_catalog.>(leftarg = json, rightarg = "any", procedure = pg_catalog.json_gt);
DROP FUNCTION IF EXISTS pg_catalog.json_gt(text, json);
CREATE OR REPLACE FUNCTION pg_catalog.json_gt(text, json) RETURNS bool LANGUAGE C IMMUTABLE as '$libdir/dolphin','json_gt';
CREATE OPERATOR pg_catalog.>(leftarg = text, rightarg = json, procedure = pg_catalog.json_gt);
DROP FUNCTION IF EXISTS pg_catalog.json_gt(bit, json);
CREATE OR REPLACE FUNCTION pg_catalog.json_gt(bit, json) RETURNS bool LANGUAGE C IMMUTABLE as '$libdir/dolphin','json_gt';
CREATE OPERATOR pg_catalog.>(leftarg = bit, rightarg = json, procedure = pg_catalog.json_gt);
DROP FUNCTION IF EXISTS pg_catalog.json_gt(boolean, json);
CREATE OR REPLACE FUNCTION pg_catalog.json_gt(boolean, json) RETURNS bool LANGUAGE C IMMUTABLE as '$libdir/dolphin','json_gt';
CREATE OPERATOR pg_catalog.>(leftarg = boolean, rightarg = json, procedure = pg_catalog.json_gt);
DROP FUNCTION IF EXISTS pg_catalog.json_gt(year, json);
CREATE OR REPLACE FUNCTION pg_catalog.json_gt(year, json) RETURNS bool LANGUAGE C IMMUTABLE as '$libdir/dolphin','json_gt';
CREATE OPERATOR pg_catalog.>(leftarg = year, rightarg = json, procedure = pg_catalog.json_gt);

DROP FUNCTION IF EXISTS pg_catalog.json_ge(json, "any");
CREATE OR REPLACE FUNCTION pg_catalog.json_ge(json, "any") RETURNS bool LANGUAGE C IMMUTABLE as '$libdir/dolphin','json_ge';
CREATE OPERATOR pg_catalog.>=(leftarg = json, rightarg = "any", procedure = pg_catalog.json_ge);
DROP FUNCTION IF EXISTS pg_catalog.json_ge(text, json);
CREATE OR REPLACE FUNCTION pg_catalog.json_ge(text, json) RETURNS bool LANGUAGE C IMMUTABLE as '$libdir/dolphin','json_ge';
CREATE OPERATOR pg_catalog.>=(leftarg = text, rightarg = json, procedure = pg_catalog.json_ge);
DROP FUNCTION IF EXISTS pg_catalog.json_ge(bit, json);
CREATE OR REPLACE FUNCTION pg_catalog.json_ge(bit, json) RETURNS bool LANGUAGE C IMMUTABLE as '$libdir/dolphin','json_ge';
CREATE OPERATOR pg_catalog.>=(leftarg = bit, rightarg = json, procedure = pg_catalog.json_ge);
DROP FUNCTION IF EXISTS pg_catalog.json_ge(boolean, json);
CREATE OR REPLACE FUNCTION pg_catalog.json_ge(boolean, json) RETURNS bool LANGUAGE C IMMUTABLE as '$libdir/dolphin','json_ge';
CREATE OPERATOR pg_catalog.>=(leftarg = boolean, rightarg = json, procedure = pg_catalog.json_ge);
DROP FUNCTION IF EXISTS pg_catalog.json_ge(year, json);
CREATE OR REPLACE FUNCTION pg_catalog.json_ge(year, json) RETURNS bool LANGUAGE C IMMUTABLE as '$libdir/dolphin','json_ge';
CREATE OPERATOR pg_catalog.>=(leftarg = year, rightarg = json, procedure = pg_catalog.json_ge);

DROP FUNCTION IF EXISTS pg_catalog.json_lt(json, "any");
CREATE OR REPLACE FUNCTION pg_catalog.json_lt(json, "any") RETURNS bool LANGUAGE C IMMUTABLE as '$libdir/dolphin','json_lt';
CREATE OPERATOR pg_catalog.<(leftarg = json, rightarg = "any", procedure = pg_catalog.json_lt);
DROP FUNCTION IF EXISTS pg_catalog.json_lt(text, json);
CREATE OR REPLACE FUNCTION pg_catalog.json_lt(text, json) RETURNS bool LANGUAGE C IMMUTABLE as '$libdir/dolphin','json_lt';
CREATE OPERATOR pg_catalog.<(leftarg = text, rightarg = json, procedure = pg_catalog.json_lt);
DROP FUNCTION IF EXISTS pg_catalog.json_lt(bit, json);
CREATE OR REPLACE FUNCTION pg_catalog.json_lt(bit, json) RETURNS bool LANGUAGE C IMMUTABLE as '$libdir/dolphin','json_lt';
CREATE OPERATOR pg_catalog.<(leftarg = bit, rightarg = json, procedure = pg_catalog.json_lt);
DROP FUNCTION IF EXISTS pg_catalog.json_lt(boolean, json);
CREATE OR REPLACE FUNCTION pg_catalog.json_lt(boolean, json) RETURNS bool LANGUAGE C IMMUTABLE as '$libdir/dolphin','json_lt';
CREATE OPERATOR pg_catalog.<(leftarg = boolean, rightarg = json, procedure = pg_catalog.json_lt);
DROP FUNCTION IF EXISTS pg_catalog.json_lt(year, json);
CREATE OR REPLACE FUNCTION pg_catalog.json_lt(year, json) RETURNS bool LANGUAGE C IMMUTABLE as '$libdir/dolphin','json_lt';
CREATE OPERATOR pg_catalog.<(leftarg = year, rightarg = json, procedure = pg_catalog.json_lt);

DROP FUNCTION IF EXISTS pg_catalog.json_le(json, "any");
CREATE OR REPLACE FUNCTION pg_catalog.json_le(json, "any") RETURNS bool LANGUAGE C IMMUTABLE as '$libdir/dolphin','json_le';
CREATE OPERATOR pg_catalog.<=(leftarg = json, rightarg = "any", procedure = pg_catalog.json_le);
DROP FUNCTION IF EXISTS pg_catalog.json_le(text, json);
CREATE OR REPLACE FUNCTION pg_catalog.json_le(text, json) RETURNS bool LANGUAGE C IMMUTABLE as '$libdir/dolphin','json_le';
CREATE OPERATOR pg_catalog.<=(leftarg = text, rightarg = json, procedure = pg_catalog.json_le);
DROP FUNCTION IF EXISTS pg_catalog.json_le(bit, json);
CREATE OR REPLACE FUNCTION pg_catalog.json_le(bit, json) RETURNS bool LANGUAGE C IMMUTABLE as '$libdir/dolphin','json_le';
CREATE OPERATOR pg_catalog.<=(leftarg = bit, rightarg = json, procedure = pg_catalog.json_le);
DROP FUNCTION IF EXISTS pg_catalog.json_le(boolean, json);
CREATE OR REPLACE FUNCTION pg_catalog.json_le(boolean, json) RETURNS bool LANGUAGE C IMMUTABLE as '$libdir/dolphin','json_le';
CREATE OPERATOR pg_catalog.<=(leftarg = boolean, rightarg = json, procedure = pg_catalog.json_le);
DROP FUNCTION IF EXISTS pg_catalog.json_le(year, json);
CREATE OR REPLACE FUNCTION pg_catalog.json_le(year, json) RETURNS bool LANGUAGE C IMMUTABLE as '$libdir/dolphin','json_le';
CREATE OPERATOR pg_catalog.<=(leftarg = year, rightarg = json, procedure = pg_catalog.json_le);

DROP FUNCTION IF EXISTS pg_catalog.b_mod(json, anyelement);
CREATE OR REPLACE FUNCTION pg_catalog.b_mod(json, anyelement)
returns numeric
as
$$
begin
    return 0;
end;
$$
language plpgsql;

DROP FUNCTION IF EXISTS pg_catalog.b_mod(anyelement, json);
CREATE OR REPLACE FUNCTION pg_catalog.b_mod(anyelement, json)
returns numeric
as
$$
begin
    return null;
end;
$$
language plpgsql;

DROP FUNCTION IF EXISTS pg_catalog.b_mod(json, json);
CREATE OR REPLACE FUNCTION pg_catalog.b_mod(json, json)
returns numeric
as
$$
begin
    return null;
end;
$$
language plpgsql;

DROP FUNCTION IF EXISTS pg_catalog.div(json, anyelement);
CREATE OR REPLACE FUNCTION pg_catalog.div(json, anyelement)
returns numeric
as
$$
begin
    return 0;
end;
$$
language plpgsql;

DROP FUNCTION IF EXISTS pg_catalog.div(anyelement, json);
CREATE OR REPLACE FUNCTION pg_catalog.div(anyelement, json)
returns numeric
as
$$
begin
    return null;
end;
$$
language plpgsql;

DROP FUNCTION IF EXISTS pg_catalog.div(json, json);
CREATE OR REPLACE FUNCTION pg_catalog.div(json, json)
returns numeric
as
$$
begin
    return null;
end;
$$
language plpgsql;

DROP FUNCTION IF EXISTS pg_catalog.xor(a json, b anyelement);
CREATE OR REPLACE FUNCTION pg_catalog.xor(a json, b anyelement)
returns integer
as
$$
begin
    return (select a ^ b);
end;
$$
language plpgsql;

DROP FUNCTION IF EXISTS pg_catalog.xor(a anyelement, b json);
CREATE OR REPLACE FUNCTION pg_catalog.xor(a anyelement, b json)
returns integer
as
$$
begin
    return (select a ^ b);
end;
$$
language plpgsql;

DROP FUNCTION IF EXISTS pg_catalog.xor(a json, b json);
CREATE OR REPLACE FUNCTION pg_catalog.xor(a json, b json)
returns integer
as
$$
begin
    return (select a ^ b);
end;
$$
language plpgsql;

DROP FUNCTION IF EXISTS pg_catalog.a_sysdate();
CREATE OR REPLACE FUNCTION pg_catalog.a_sysdate()
returns timestamp without time zone LANGUAGE SQL
VOLATILE as $$ SELECT CURRENT_TIMESTAMP::timestamp(0) without time zone$$;

DROP CAST IF EXISTS (json AS boolean);
DROP FUNCTION IF EXISTS pg_catalog.json_to_bool(json);
CREATE OR REPLACE FUNCTION pg_catalog.json_to_bool(json) 
RETURNS boolean LANGUAGE SQL IMMUTABLE STRICT as 
'select cast(cast($1 as text) as boolean)';
CREATE CAST (json AS boolean) WITH FUNCTION json_to_bool(json) AS ASSIGNMENT;

DROP FUNCTION IF EXISTS pg_catalog.hex(int8);
DROP FUNCTION IF EXISTS pg_catalog.hex(int16);
CREATE OR REPLACE FUNCTION pg_catalog.hex(int16) RETURNS text LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'int_to_hex';

DROP FUNCTION IF EXISTS pg_catalog.bit_bin_in(cstring, oid, integer);
CREATE OR REPLACE FUNCTION pg_catalog.bit_bin_in(cstring, oid, integer) RETURNS bit LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin','bit_bin_in';
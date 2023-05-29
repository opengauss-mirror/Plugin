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
CREATE OR REPLACE FUNCTION pg_catalog.bit_count_bit (text) RETURNS int LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'bit_count_bit';
DROP FUNCTION IF EXISTS pg_catalog.bit_count(bit) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.bit_count (bit) RETURNS int AS $$ SELECT pg_catalog.bit_count_bit(cast($1 as text)) $$ LANGUAGE SQL;

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

DROP FUNCTION IF EXISTS pg_catalog.atan (float8, float8);
CREATE OR REPLACE FUNCTION pg_catalog.atan (float8, float8)  RETURNS float8 AS $$ SELECT pg_catalog.atan2($1, $2) $$ LANGUAGE SQL;

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

DROP FUNCTION IF EXISTS pg_catalog.unhex (text) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.unhex (boolean) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.unhex (bytea) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.unhex (bit) CASCADE;

CREATE OR REPLACE FUNCTION pg_catalog.unhex (text)  RETURNS text LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'hex_decode_text';
CREATE OR REPLACE FUNCTION pg_catalog.unhex (boolean)  RETURNS text LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'hex_decode_bool';
CREATE OR REPLACE FUNCTION pg_catalog.unhex (bytea)  RETURNS text LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'hex_decode_bytea';
CREATE OR REPLACE FUNCTION pg_catalog.unhex (bit)  RETURNS text LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'hex_decode_bit';


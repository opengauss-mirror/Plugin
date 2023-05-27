DROP FUNCTION IF EXISTS pg_catalog.dolphin_invoke();
CREATE FUNCTION pg_catalog.dolphin_invoke()
    RETURNS VOID AS '$libdir/dolphin','dolphin_invoke' LANGUAGE C STRICT;
DROP FUNCTION IF EXISTS pg_catalog.ord(varbit);
DROP FUNCTION IF EXISTS pg_catalog.oct(bit);

DROP FUNCTION IF EXISTS pg_catalog.substring_index ("any", "any", text) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.substring_index ("any", "any", numeric) CASCADE;

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
CREATE OR REPLACE FUNCTION pg_catalog.bit_count (numeric)  RETURNS int LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'bit_count_numeric';

DROP FUNCTION IF EXISTS pg_catalog.bit_count(text) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.bit_count (text)  RETURNS int LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'bit_count_text';

DROP FUNCTION IF EXISTS pg_catalog.bit_count_bit(text) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.bit_count_bit (text) RETURNS int LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'bit_count_bit';

DROP FUNCTION IF EXISTS pg_catalog.bit_count(bit) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.bit_count (bit) RETURNS int AS $$ SELECT pg_catalog.bit_count_bit(cast($1 as text)) $$ LANGUAGE SQL;

DROP FUNCTION IF EXISTS pg_catalog.bit_count(date) CASCADE;

DROP FUNCTION IF EXISTS pg_catalog.connection_id() CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.connection_id()
RETURNS int8 AS
$$
BEGIN
    RETURN (select pg_backend_pid());
END;
$$
LANGUAGE plpgsql;


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
DROP FUNCTION IF EXISTS pg_catalog.oct(numeric) CASCADE;

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
DROP FUNCTION IF EXISTS pg_catalog.to_base64 (boolean) CASCADE;
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
DROP FUNCTION IF EXISTS pg_catalog.unhex (bit) CASCADE;

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

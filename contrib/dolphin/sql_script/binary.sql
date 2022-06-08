/* binary */
DROP TYPE IF EXISTS pg_catalog.binary CASCADE;
DROP TYPE IF EXISTS pg_catalog._binary CASCADE;

CREATE TYPE pg_catalog.binary;

DROP FUNCTION IF EXISTS pg_catalog.binary_in(cstring) CASCADE;

CREATE FUNCTION pg_catalog.binary_in (
cstring
) RETURNS binary LANGUAGE INTERNAL IMMUTABLE STRICT as 'byteain';

DROP FUNCTION IF EXISTS pg_catalog.binary_out(binary) CASCADE;

CREATE FUNCTION pg_catalog.binary_out (
binary
) RETURNS cstring LANGUAGE INTERNAL IMMUTABLE STRICT as 'byteaout';

DROP FUNCTION IF EXISTS pg_catalog.binary_recv(internal) CASCADE;

CREATE FUNCTION pg_catalog.binary_recv (
internal
) RETURNS binary LANGUAGE INTERNAL IMMUTABLE STRICT as 'bytearecv';

DROP FUNCTION IF EXISTS pg_catalog.binary_send(binary) CASCADE;

CREATE FUNCTION pg_catalog.binary_send (
binary
) RETURNS bytea LANGUAGE INTERNAL IMMUTABLE STRICT as 'byteasend';

DROP FUNCTION IF EXISTS pg_catalog.binary_typmodin(_cstring) CASCADE;

CREATE FUNCTION pg_catalog.binary_typmodin (
_cstring
) RETURNS int LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'binary_typmodin';

DROP FUNCTION IF EXISTS pg_catalog.binary_typmodout(int) CASCADE;

CREATE FUNCTION pg_catalog.binary_typmodout (
int
) RETURNS cstring LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'binary_typmodout';

CREATE TYPE pg_catalog.binary (input=binary_in, output=binary_out,
                                typmod_in = binary_typmodin, typmod_out = binary_typmodout,
                                receive = binary_recv, send = binary_send,
                                STORAGE=EXTENDED, category='S');

/* varbinary */
DROP TYPE IF EXISTS pg_catalog.varbinary CASCADE;
DROP TYPE IF EXISTS pg_catalog._varbinary CASCADE;

CREATE TYPE pg_catalog.varbinary;

DROP FUNCTION IF EXISTS pg_catalog.varbinary_in(cstring) CASCADE;

CREATE FUNCTION pg_catalog.varbinary_in (
cstring
) RETURNS varbinary LANGUAGE INTERNAL IMMUTABLE STRICT as 'byteain';

DROP FUNCTION IF EXISTS pg_catalog.varbinary_out(varbinary) CASCADE;

CREATE FUNCTION pg_catalog.varbinary_out (
varbinary
) RETURNS cstring LANGUAGE INTERNAL IMMUTABLE STRICT as 'byteaout';

DROP FUNCTION IF EXISTS pg_catalog.varbinary_recv(internal, oid, int) CASCADE;

CREATE FUNCTION pg_catalog.varbinary_recv (
internal
) RETURNS varbinary LANGUAGE INTERNAL IMMUTABLE STRICT as 'bytearecv';

DROP FUNCTION IF EXISTS pg_catalog.varbinary_send(varbinary) CASCADE;

CREATE FUNCTION pg_catalog.varbinary_send (
varbinary
) RETURNS bytea LANGUAGE INTERNAL IMMUTABLE STRICT as 'byteasend';

DROP FUNCTION IF EXISTS pg_catalog.varbinary_typmodin(_cstring) CASCADE;

CREATE FUNCTION pg_catalog.varbinary_typmodin (
_cstring
) RETURNS int LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'varbinary_typmodin';

CREATE TYPE pg_catalog.varbinary (input=varbinary_in, output=varbinary_out,
                                typmod_in = varbinary_typmodin, typmod_out = binary_typmodout,
                                receive = varbinary_recv, send = varbinary_send,
                                STORAGE=EXTENDED, category='S');

-- to_binary
DROP FUNCTION IF EXISTS pg_catalog.to_binary(bytea, int) CASCADE;

CREATE FUNCTION pg_catalog.to_binary (
bytea,
int
) RETURNS binary LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'bytea2binary';
-- to_varbinary
DROP FUNCTION IF EXISTS pg_catalog.to_varbinary(bytea, int) CASCADE;

CREATE FUNCTION pg_catalog.to_varbinary (
bytea,
int
) RETURNS varbinary LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'bytea2var';


CREATE CAST (binary AS bytea) WITHOUT FUNCTION AS IMPLICIT;
CREATE CAST (varbinary AS bytea) WITHOUT FUNCTION AS IMPLICIT;
CREATE CAST (bytea AS binary) WITH FUNCTION to_binary(bytea, int) AS IMPLICIT;
CREATE CAST (bytea AS varbinary) WITH FUNCTION to_varbinary(bytea, int) AS IMPLICIT;

CREATE CAST (varbinary AS binary) WITH FUNCTION to_binary(bytea, int) AS IMPLICIT;
CREATE CAST (binary AS varbinary) WITH FUNCTION to_varbinary(bytea, int) AS IMPLICIT;
CREATE CAST (binary AS binary) WITH FUNCTION to_binary(bytea, int) AS IMPLICIT;
CREATE CAST (varbinary AS varbinary) WITH FUNCTION to_varbinary(bytea, int) AS IMPLICIT;

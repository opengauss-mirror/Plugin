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

DROP FUNCTION IF EXISTS pg_catalog.bit2float4(bit) CASCADE;

CREATE FUNCTION pg_catalog.bit2float4 (bit) RETURNS float4 AS
$$
BEGIN
    RETURN (SELECT int4($1));
END;
$$
LANGUAGE plpgsql;

DROP FUNCTION IF EXISTS pg_catalog.bit2float8(bit) CASCADE;

CREATE FUNCTION pg_catalog.bit2float8 (bit) RETURNS float8 AS
$$
BEGIN
    RETURN (SELECT int8($1));
END;
$$
LANGUAGE plpgsql;

DROP FUNCTION IF EXISTS pg_catalog.bit2numeric(bit) CASCADE;

CREATE FUNCTION pg_catalog.bit2numeric (bit) RETURNS numeric AS
$$
BEGIN
    RETURN (SELECT int8($1));
END;
$$
LANGUAGE plpgsql;

CREATE CAST (bit AS float4) WITH FUNCTION bit2float4(bit) AS IMPLICIT;
CREATE CAST (bit AS float8) WITH FUNCTION bit2float8(bit) AS IMPLICIT;
CREATE CAST (bit AS numeric) WITH FUNCTION bit2numeric(bit) AS IMPLICIT;

-- tinyblob
DROP TYPE IF EXISTS pg_catalog.tinyblob CASCADE;
DROP TYPE IF EXISTS pg_catalog._tinyblob CASCADE;

CREATE TYPE pg_catalog.tinyblob;

DROP FUNCTION IF EXISTS pg_catalog.tinyblob_rawin(cstring) CASCADE;

CREATE FUNCTION pg_catalog.tinyblob_rawin (
cstring
) RETURNS tinyblob LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'tinyblob_rawin';

DROP FUNCTION IF EXISTS pg_catalog.tinyblob_rawout(tinyblob) CASCADE;

CREATE FUNCTION pg_catalog.tinyblob_rawout (
tinyblob
) RETURNS cstring LANGUAGE INTERNAL IMMUTABLE STRICT as 'byteaout';

DROP FUNCTION IF EXISTS pg_catalog.tinyblob_recv(internal) CASCADE;

CREATE FUNCTION pg_catalog.tinyblob_recv (
internal
) RETURNS tinyblob LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'tinyblob_recv';

DROP FUNCTION IF EXISTS pg_catalog.tinyblob_send(tinyblob) CASCADE;

CREATE FUNCTION pg_catalog.tinyblob_send (
tinyblob
) RETURNS bytea LANGUAGE INTERNAL IMMUTABLE STRICT as 'byteasend';

CREATE TYPE pg_catalog.tinyblob (input=tinyblob_rawin, output=tinyblob_rawout, RECEIVE = tinyblob_recv, SEND = tinyblob_send, STORAGE=EXTENDED, category='S');


--mediumblob
DROP TYPE IF EXISTS pg_catalog.mediumblob CASCADE;
DROP TYPE IF EXISTS pg_catalog._mediumblob CASCADE;

CREATE TYPE pg_catalog.mediumblob;

DROP FUNCTION IF EXISTS pg_catalog.mediumblob_rawin(cstring) CASCADE;

CREATE FUNCTION pg_catalog.mediumblob_rawin (
cstring
) RETURNS mediumblob LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'mediumblob_rawin';

DROP FUNCTION IF EXISTS pg_catalog.mediumblob_rawout(mediumblob) CASCADE;

CREATE FUNCTION pg_catalog.mediumblob_rawout (
mediumblob
) RETURNS cstring LANGUAGE INTERNAL IMMUTABLE STRICT as 'byteaout';

DROP FUNCTION IF EXISTS pg_catalog.mediumblob_recv(internal) CASCADE;

CREATE FUNCTION pg_catalog.mediumblob_recv (
internal
) RETURNS mediumblob LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'mediumblob_recv';

DROP FUNCTION IF EXISTS pg_catalog.mediumblob_send(mediumblob) CASCADE;

CREATE FUNCTION pg_catalog.mediumblob_send (
mediumblob
) RETURNS bytea LANGUAGE INTERNAL IMMUTABLE STRICT as 'byteasend';

CREATE TYPE pg_catalog.mediumblob (input=mediumblob_rawin, output=mediumblob_rawout, RECEIVE = mediumblob_recv, SEND = mediumblob_send, STORAGE=EXTENDED, category='S');

-- longblob
DROP TYPE IF EXISTS pg_catalog.longblob CASCADE;
DROP TYPE IF EXISTS pg_catalog._longblob CASCADE;

CREATE TYPE pg_catalog.longblob;

DROP FUNCTION IF EXISTS pg_catalog.longblob_rawin(cstring) CASCADE;

CREATE FUNCTION pg_catalog.longblob_rawin (
cstring
) RETURNS longblob LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'longblob_rawin';

DROP FUNCTION IF EXISTS pg_catalog.longblob_rawout(longblob) CASCADE;

CREATE FUNCTION pg_catalog.longblob_rawout (
longblob
) RETURNS cstring LANGUAGE INTERNAL IMMUTABLE STRICT as 'byteaout';

DROP FUNCTION IF EXISTS pg_catalog.longblob_recv(internal) CASCADE;

CREATE FUNCTION pg_catalog.longblob_recv (
internal
) RETURNS longblob LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'longblob_recv';

DROP FUNCTION IF EXISTS pg_catalog.longblob_send(longblob) CASCADE;

CREATE FUNCTION pg_catalog.longblob_send (
longblob
) RETURNS bytea LANGUAGE INTERNAL IMMUTABLE STRICT as 'byteasend';

CREATE TYPE pg_catalog.longblob (input=longblob_rawin, output=longblob_rawout, RECEIVE = longblob_recv, SEND = longblob_send, STORAGE=EXTENDED, category='S');

-- to_tinyblob
DROP FUNCTION IF EXISTS pg_catalog.to_tinyblob(longblob) CASCADE;

CREATE FUNCTION pg_catalog.to_tinyblob (
longblob
) RETURNS tinyblob LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'longblob2tinyblob';

-- to_mediumblob
DROP FUNCTION IF EXISTS pg_catalog.to_mediumblob(longblob) CASCADE;

CREATE FUNCTION pg_catalog.to_mediumblob (
longblob
) RETURNS mediumblob LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'longblob2mediumblob';

CREATE CAST (tinyblob AS longblob) WITHOUT FUNCTION AS IMPLICIT;
CREATE CAST (mediumblob AS longblob) WITHOUT FUNCTION AS IMPLICIT;
CREATE CAST (blob AS longblob) WITHOUT FUNCTION AS IMPLICIT;

CREATE CAST (blob AS tinyblob) WITH FUNCTION to_tinyblob(longblob) AS IMPLICIT;
CREATE CAST (mediumblob AS tinyblob) WITH FUNCTION to_tinyblob(longblob) AS IMPLICIT;
CREATE CAST (longblob AS tinyblob) WITH FUNCTION to_tinyblob(longblob) AS IMPLICIT;

CREATE CAST (tinyblob AS blob) WITHOUT FUNCTION AS IMPLICIT;
CREATE CAST (mediumblob AS blob) WITHOUT FUNCTION AS IMPLICIT;
CREATE CAST (longblob AS blob) WITHOUT FUNCTION AS IMPLICIT;

CREATE CAST (tinyblob AS mediumblob) WITHOUT FUNCTION AS IMPLICIT;
CREATE CAST (blob AS mediumblob) WITH FUNCTION to_mediumblob(longblob) AS IMPLICIT;
CREATE CAST (longblob AS mediumblob) WITH FUNCTION to_mediumblob(longblob) AS IMPLICIT;

CREATE CAST (tinyblob AS raw) WITHOUT FUNCTION AS IMPLICIT;
CREATE CAST (mediumblob AS raw) WITHOUT FUNCTION AS IMPLICIT;
CREATE CAST (longblob AS raw) WITHOUT FUNCTION AS IMPLICIT;

CREATE CAST (raw AS longblob) WITHOUT FUNCTION AS IMPLICIT;
CREATE CAST (raw AS tinyblob) WITH FUNCTION to_tinyblob(longblob) AS IMPLICIT;
CREATE CAST (raw AS mediumblob) WITH FUNCTION to_mediumblob(longblob) AS IMPLICIT;

create operator pg_catalog.^(leftarg = int1, rightarg = int1, procedure = pg_catalog.int1xor);
create operator pg_catalog.^(leftarg = int2, rightarg = int2, procedure = pg_catalog.int2xor);
create operator pg_catalog.^(leftarg = int4, rightarg = int4, procedure = pg_catalog.int4xor);
create operator pg_catalog.^(leftarg = int8, rightarg = int8, procedure = pg_catalog.int8xor);
create operator pg_catalog.^(leftarg = bit, rightarg = bit, procedure = pg_catalog.bitxor);


create function pg_catalog.blobxor(
blob,
blob
)RETURNS int LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'blobxor';
create function pg_catalog.blobxor(
blob,
int
)RETURNS int LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'blobxor';
create function pg_catalog.blobxor(
int,
blob
)RETURNS int LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'blobxor';
create function pg_catalog.blobxor(
int8,
blob
)RETURNS int LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'blobxor';
create function pg_catalog.blobxor(
blob,
int8
)RETURNS int LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'blobxor';
create operator pg_catalog.^(leftarg = blob, rightarg = blob, procedure = pg_catalog.blobxor);
create operator pg_catalog.^(leftarg = blob, rightarg = integer, procedure = pg_catalog.blobxor);
create operator pg_catalog.^(leftarg = integer, rightarg = blob, procedure = pg_catalog.blobxor);
create operator pg_catalog.^(leftarg = int8, rightarg = blob, procedure = pg_catalog.blobxor);
create operator pg_catalog.^(leftarg = blob, rightarg = int8, procedure = pg_catalog.blobxor);

DROP FUNCTION IF EXISTS pg_catalog.bit_longblob(uint8,longblob) CASCADE;
CREATE FUNCTION pg_catalog.bit_longblob (t1 uint8, t2 longblob) RETURNS uint8  AS
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

DROP FUNCTION IF EXISTS pg_catalog.bit_blob(uint8,blob) CASCADE;
CREATE FUNCTION pg_catalog.bit_blob (t1 uint8, t2 blob) RETURNS uint8  AS
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

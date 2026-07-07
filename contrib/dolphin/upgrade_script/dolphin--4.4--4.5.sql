CREATE OR REPLACE FUNCTION pg_catalog.db_b_format(number, name) RETURNS text LANGUAGE SQL IMMUTABLE STRICT as 'select db_b_format($1::anyelement , $2::bigint)';

CREATE OR REPLACE FUNCTION pg_catalog.text_binary_eq(text, binary) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS
'SELECT pg_catalog.byteaeq($1::bytea, $2::bytea)';

CREATE OR REPLACE FUNCTION pg_catalog.binary_text_eq(binary, text) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS
'SELECT pg_catalog.byteaeq($1::bytea, $2::bytea)';

CREATE OR REPLACE FUNCTION pg_catalog.text_binary_ne(text, binary) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS
'SELECT pg_catalog.byteane($1::bytea, $2::bytea)';

CREATE OPERATOR pg_catalog.<>(leftarg = text, rightarg = binary, COMMUTATOR = operator(pg_catalog.<>),
NEGATOR = operator(pg_catalog.=), procedure = pg_catalog.text_binary_ne, restrict = neqsel, join = neqjoinsel);

CREATE OR REPLACE FUNCTION pg_catalog.binary_text_ne(binary, text) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS
'SELECT pg_catalog.byteane($1::bytea, $2::bytea)';

CREATE OPERATOR pg_catalog.<>(leftarg = binary, rightarg = text, COMMUTATOR = operator(pg_catalog.<>),
NEGATOR = operator(pg_catalog.=), procedure = pg_catalog.binary_text_ne, restrict = neqsel, join = neqjoinsel);

CREATE OR REPLACE FUNCTION pg_catalog.text_binary_gt(text, binary) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS
'SELECT pg_catalog.byteagt($1::bytea, $2::bytea)';

CREATE OPERATOR pg_catalog.>(leftarg = text, rightarg = binary, COMMUTATOR = operator(pg_catalog.<),
NEGATOR = operator(pg_catalog.<=), procedure = pg_catalog.text_binary_gt, restrict = scalargtsel,
join = scalargtjoinsel);

CREATE OR REPLACE FUNCTION pg_catalog.binary_text_gt(binary, text) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS
'SELECT pg_catalog.byteagt($1::bytea, $2::bytea)';

CREATE OPERATOR pg_catalog.>(leftarg = binary, rightarg = text, COMMUTATOR = operator(pg_catalog.<),
NEGATOR = operator(pg_catalog.<=), procedure = pg_catalog.binary_text_gt, restrict = scalargtsel,
join = scalargtjoinsel);

CREATE OR REPLACE FUNCTION pg_catalog.text_binary_lt(text, binary) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS
'SELECT pg_catalog.bytealt($1::bytea, $2::bytea)';

CREATE OPERATOR pg_catalog.<(leftarg = text, rightarg = binary, COMMUTATOR = operator(pg_catalog.>),
NEGATOR = operator(pg_catalog.>=), procedure = pg_catalog.text_binary_lt, restrict = scalarltsel,
join = scalarltjoinsel);

CREATE OR REPLACE FUNCTION pg_catalog.binary_text_lt(binary, text) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS
'SELECT pg_catalog.bytealt($1::bytea, $2::bytea)';

CREATE OPERATOR pg_catalog.<(leftarg = binary, rightarg = text, COMMUTATOR = operator(pg_catalog.>),
NEGATOR = operator(pg_catalog.>=), procedure = pg_catalog.binary_text_lt, restrict = scalarltsel,
join = scalarltjoinsel);

CREATE OR REPLACE FUNCTION pg_catalog.text_binary_ge(text, binary) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS
'SELECT pg_catalog.byteage($1::bytea, $2::bytea)';

CREATE OPERATOR pg_catalog.>=(leftarg = text, rightarg = binary, COMMUTATOR = operator(pg_catalog.<=),
NEGATOR = operator(pg_catalog.<), procedure = pg_catalog.text_binary_ge, restrict = scalargtsel,
join = scalargtjoinsel);

CREATE OR REPLACE FUNCTION pg_catalog.binary_text_ge(binary, text) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS
'SELECT pg_catalog.byteage($1::bytea, $2::bytea)';

CREATE OPERATOR pg_catalog.>=(leftarg = binary, rightarg = text, COMMUTATOR = operator(pg_catalog.<=),
NEGATOR = operator(pg_catalog.<), procedure = pg_catalog.binary_text_ge, restrict = scalargtsel,
join = scalargtjoinsel);

CREATE OR REPLACE FUNCTION pg_catalog.text_binary_le(text, binary) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS
'SELECT pg_catalog.byteale($1::bytea, $2::bytea)';

CREATE OPERATOR pg_catalog.<=(leftarg = text, rightarg = binary, COMMUTATOR = operator(pg_catalog.>=),
NEGATOR = operator(pg_catalog.>), procedure = pg_catalog.text_binary_le, restrict = scalarltsel,
join = scalarltjoinsel);

CREATE OR REPLACE FUNCTION pg_catalog.binary_text_le(binary, text) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS
'SELECT pg_catalog.byteale($1::bytea, $2::bytea)';

CREATE OPERATOR pg_catalog.<=(leftarg = binary, rightarg = text, COMMUTATOR = operator(pg_catalog.>=),
NEGATOR = operator(pg_catalog.>), procedure = pg_catalog.binary_text_le, restrict = scalarltsel,
join = scalarltjoinsel);

DROP FUNCTION IF EXISTS pg_catalog.date_format (timestamp without time zone, text);
CREATE OR REPLACE FUNCTION pg_catalog.date_format (timestamp without time zone, text) RETURNS TEXT LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'date_format_timestamp';
DROP FUNCTION IF EXISTS pg_catalog.date_format (timestamp with time zone, text);
CREATE OR REPLACE FUNCTION pg_catalog.date_format (timestamp with time zone, text) RETURNS TEXT LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'date_format_timestamptz';

CREATE OR REPLACE FUNCTION pg_catalog.boolean_tinyblob_eq(boolean, tinyblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::float8 = $2::float8)';
CREATE OR REPLACE FUNCTION pg_catalog.tinyblob_boolean_eq(tinyblob, boolean) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::float8 = $2::float8)';
CREATE OR REPLACE FUNCTION pg_catalog.boolean_tinyblob_ne(boolean, tinyblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::float8 != $2::float8)';
CREATE OR REPLACE FUNCTION pg_catalog.tinyblob_boolean_ne(tinyblob, boolean) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::float8 != $2::float8)';
CREATE OR REPLACE FUNCTION pg_catalog.boolean_tinyblob_gt(boolean, tinyblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::float8 > $2::float8)';
CREATE OR REPLACE FUNCTION pg_catalog.tinyblob_boolean_gt(tinyblob, boolean) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::float8 > $2::float8)';
CREATE OR REPLACE FUNCTION pg_catalog.boolean_tinyblob_lt(boolean, tinyblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::float8 < $2::float8)';
CREATE OR REPLACE FUNCTION pg_catalog.tinyblob_boolean_lt(tinyblob, boolean) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::float8 < $2::float8)';
CREATE OR REPLACE FUNCTION pg_catalog.boolean_tinyblob_ge(boolean, tinyblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::float8 >= $2::float8)';
CREATE OR REPLACE FUNCTION pg_catalog.tinyblob_boolean_ge(tinyblob, boolean) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::float8 >= $2::float8)';
CREATE OR REPLACE FUNCTION pg_catalog.boolean_tinyblob_le(boolean, tinyblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::float8 <= $2::float8)';
CREATE OR REPLACE FUNCTION pg_catalog.tinyblob_boolean_le(tinyblob, boolean) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::float8 <= $2::float8)';

CREATE OR REPLACE FUNCTION pg_catalog.boolean_mediumblob_eq(boolean, mediumblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::float8 = $2::float8)';
CREATE OR REPLACE FUNCTION pg_catalog.mediumblob_boolean_eq(mediumblob, boolean) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::float8 = $2::float8)';
CREATE OR REPLACE FUNCTION pg_catalog.boolean_mediumblob_ne(boolean, mediumblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::float8 != $2::float8)';
CREATE OR REPLACE FUNCTION pg_catalog.mediumblob_boolean_ne(mediumblob, boolean) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::float8 != $2::float8)';
CREATE OR REPLACE FUNCTION pg_catalog.boolean_mediumblob_gt(boolean, mediumblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::float8 > $2::float8)';
CREATE OR REPLACE FUNCTION pg_catalog.mediumblob_boolean_gt(mediumblob, boolean) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::float8 > $2::float8)';
CREATE OR REPLACE FUNCTION pg_catalog.boolean_mediumblob_lt(boolean, mediumblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::float8 < $2::float8)';
CREATE OR REPLACE FUNCTION pg_catalog.mediumblob_boolean_lt(mediumblob, boolean) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::float8 < $2::float8)';
CREATE OR REPLACE FUNCTION pg_catalog.boolean_mediumblob_ge(boolean, mediumblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::float8 >= $2::float8)';
CREATE OR REPLACE FUNCTION pg_catalog.mediumblob_boolean_ge(mediumblob, boolean) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::float8 >= $2::float8)';
CREATE OR REPLACE FUNCTION pg_catalog.boolean_mediumblob_le(boolean, mediumblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::float8 <= $2::float8)';
CREATE OR REPLACE FUNCTION pg_catalog.mediumblob_boolean_le(mediumblob, boolean) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::float8 <= $2::float8)';

CREATE OR REPLACE FUNCTION pg_catalog.boolean_blob_eq(boolean, blob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::float8 = $2::float8)';
CREATE OR REPLACE FUNCTION pg_catalog.blob_boolean_eq(blob, boolean) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::float8 = $2::float8)';
CREATE OR REPLACE FUNCTION pg_catalog.boolean_blob_ne(boolean, blob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::float8 != $2::float8)';
CREATE OR REPLACE FUNCTION pg_catalog.blob_boolean_ne(blob, boolean) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::float8 != $2::float8)';
CREATE OR REPLACE FUNCTION pg_catalog.boolean_blob_gt(boolean, blob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::float8 > $2::float8)';
CREATE OR REPLACE FUNCTION pg_catalog.blob_boolean_gt(blob, boolean) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::float8 > $2::float8)';
CREATE OR REPLACE FUNCTION pg_catalog.boolean_blob_lt(boolean, blob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::float8 < $2::float8)';
CREATE OR REPLACE FUNCTION pg_catalog.blob_boolean_lt(blob, boolean) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::float8 < $2::float8)';
CREATE OR REPLACE FUNCTION pg_catalog.boolean_blob_ge(boolean, blob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::float8 >= $2::float8)';
CREATE OR REPLACE FUNCTION pg_catalog.blob_boolean_ge(blob, boolean) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::float8 >= $2::float8)';
CREATE OR REPLACE FUNCTION pg_catalog.boolean_blob_le(boolean, blob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::float8 <= $2::float8)';
CREATE OR REPLACE FUNCTION pg_catalog.blob_boolean_le(blob, boolean) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::float8 <= $2::float8)';

CREATE OR REPLACE FUNCTION pg_catalog.boolean_longblob_eq(boolean, longblob) returns bool LANGUAGE SQL IMMUTABLE STRICT as 'select ($1::float8 = $2::float8)';
CREATE OR REPLACE FUNCTION pg_catalog.longblob_boolean_eq(longblob, boolean) returns bool LANGUAGE SQL IMMUTABLE STRICT as 'select ($1::float8 = $2::float8)';
CREATE OR REPLACE FUNCTION pg_catalog.boolean_longblob_ne(boolean, longblob) returns bool LANGUAGE SQL IMMUTABLE STRICT as 'select ($1::float8 != $2::float8)';
CREATE OR REPLACE FUNCTION pg_catalog.longblob_boolean_ne(longblob, boolean) returns bool LANGUAGE SQL IMMUTABLE STRICT as 'select ($1::float8 != $2::float8)';
CREATE OR REPLACE FUNCTION pg_catalog.boolean_longblob_gt(boolean, longblob) returns bool LANGUAGE SQL IMMUTABLE STRICT as 'select ($1::float8 > $2::float8)';
CREATE OR REPLACE FUNCTION pg_catalog.longblob_boolean_gt(longblob, boolean) returns bool LANGUAGE SQL IMMUTABLE STRICT as 'select ($1::float8 > $2::float8)';
CREATE OR REPLACE FUNCTION pg_catalog.boolean_longblob_lt(boolean, longblob) returns bool LANGUAGE SQL IMMUTABLE STRICT as 'select ($1::float8 < $2::float8)';
CREATE OR REPLACE FUNCTION pg_catalog.longblob_boolean_lt(longblob, boolean) returns bool LANGUAGE SQL IMMUTABLE STRICT as 'select ($1::float8 < $2::float8)';
CREATE OR REPLACE FUNCTION pg_catalog.boolean_longblob_ge(boolean, longblob) returns bool LANGUAGE SQL IMMUTABLE STRICT as 'select ($1::float8 >= $2::float8)';
CREATE OR REPLACE FUNCTION pg_catalog.longblob_boolean_ge(longblob, boolean) returns bool LANGUAGE SQL IMMUTABLE STRICT as 'select ($1::float8 >= $2::float8)';
CREATE OR REPLACE FUNCTION pg_catalog.boolean_longblob_le(boolean, longblob) returns bool LANGUAGE SQL IMMUTABLE STRICT as 'select ($1::float8 <= $2::float8)';
CREATE OR REPLACE FUNCTION pg_catalog.longblob_boolean_le(longblob, boolean) returns bool LANGUAGE SQL IMMUTABLE STRICT as 'select ($1::float8 <= $2::float8)';

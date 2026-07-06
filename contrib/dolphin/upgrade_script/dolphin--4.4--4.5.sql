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
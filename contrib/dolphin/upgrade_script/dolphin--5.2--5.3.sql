DROP FUNCTION IF EXISTS pg_catalog.date_format (timestamp without time zone, text);
CREATE OR REPLACE FUNCTION pg_catalog.date_format (timestamp without time zone, text) RETURNS TEXT LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'date_format_timestamp';
DROP FUNCTION IF EXISTS pg_catalog.date_format (timestamp with time zone, text);
CREATE OR REPLACE FUNCTION pg_catalog.date_format (timestamp with time zone, text) RETURNS TEXT LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'date_format_timestamptz';

CREATE OR REPLACE FUNCTION pg_catalog.db_b_format(number, name) RETURNS text LANGUAGE SQL IMMUTABLE STRICT as 'select db_b_format($1::anyelement , $2::bigint)';
CREATE OR REPLACE FUNCTION pg_catalog.db_b_format(number, number) RETURNS text LANGUAGE C IMMUTABLE as '$libdir/dolphin', 'db_b_format_numeric_precision';
CREATE OR REPLACE FUNCTION pg_catalog.db_b_format(unknown, int8) RETURNS text LANGUAGE C IMMUTABLE as '$libdir/dolphin', 'db_b_format';
CREATE OR REPLACE FUNCTION pg_catalog.db_b_format(unknown, int4) RETURNS text LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.db_b_format($1, $2::bigint)';
CREATE OR REPLACE FUNCTION pg_catalog.db_b_format(unknown, number) RETURNS text LANGUAGE C IMMUTABLE as '$libdir/dolphin', 'db_b_format_numeric_precision';
CREATE OR REPLACE FUNCTION pg_catalog.db_b_format(varchar, varchar) RETURNS text LANGUAGE C IMMUTABLE as '$libdir/dolphin', 'db_b_format_varchar';

CREATE OR REPLACE FUNCTION pg_catalog.text_binary_eq(text, binary) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS
'SELECT pg_catalog.byteaeq($1::bytea, $2::bytea)';

CREATE OR REPLACE FUNCTION pg_catalog.binary_text_eq(binary, text) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS
'SELECT pg_catalog.byteaeq($1::bytea, $2::bytea)';

CREATE OR REPLACE FUNCTION pg_catalog.text_binary_ne(text, binary) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS
'SELECT pg_catalog.byteane($1::bytea, $2::bytea)';

CREATE OR REPLACE FUNCTION pg_catalog.binary_text_ne(binary, text) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS
'SELECT pg_catalog.byteane($1::bytea, $2::bytea)';

CREATE OR REPLACE FUNCTION pg_catalog.text_binary_gt(text, binary) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS
'SELECT pg_catalog.byteagt($1::bytea, $2::bytea)';

CREATE OR REPLACE FUNCTION pg_catalog.binary_text_gt(binary, text) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS
'SELECT pg_catalog.byteagt($1::bytea, $2::bytea)';

CREATE OR REPLACE FUNCTION pg_catalog.text_binary_lt(text, binary) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS
'SELECT pg_catalog.bytealt($1::bytea, $2::bytea)';

CREATE OR REPLACE FUNCTION pg_catalog.binary_text_lt(binary, text) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS
'SELECT pg_catalog.bytealt($1::bytea, $2::bytea)';

CREATE OR REPLACE FUNCTION pg_catalog.text_binary_ge(text, binary) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS
'SELECT pg_catalog.byteage($1::bytea, $2::bytea)';

CREATE OR REPLACE FUNCTION pg_catalog.binary_text_ge(binary, text) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS
'SELECT pg_catalog.byteage($1::bytea, $2::bytea)';

CREATE OR REPLACE FUNCTION pg_catalog.text_binary_le(text, binary) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS
'SELECT pg_catalog.byteale($1::bytea, $2::bytea)';

CREATE OR REPLACE FUNCTION pg_catalog.binary_text_le(binary, text) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS
'SELECT pg_catalog.byteale($1::bytea, $2::bytea)';

DO $do$
DECLARE
    defined_count bigint;
    dolphin_owned_count bigint;
BEGIN
    SELECT count(*)
      INTO defined_count
      FROM pg_catalog.pg_operator opr
      JOIN pg_catalog.pg_namespace nsp
        ON nsp.oid = opr.oprnamespace
      JOIN pg_catalog.pg_type left_type
        ON left_type.oid = opr.oprleft
      JOIN pg_catalog.pg_namespace left_nsp
        ON left_nsp.oid = left_type.typnamespace
      JOIN pg_catalog.pg_type right_type
        ON right_type.oid = opr.oprright
      JOIN pg_catalog.pg_namespace right_nsp
        ON right_nsp.oid = right_type.typnamespace
     WHERE nsp.nspname = 'pg_catalog'
       AND opr.oprname IN ('<>', '>', '<', '>=', '<=')
       AND opr.oprcode <> 0
       AND ((left_nsp.nspname = 'pg_catalog'
             AND left_type.typname = 'text'
             AND right_nsp.nspname = 'pg_catalog'
             AND right_type.typname = 'binary')
         OR (left_nsp.nspname = 'pg_catalog'
             AND left_type.typname = 'binary'
             AND right_nsp.nspname = 'pg_catalog'
             AND right_type.typname = 'text'));

    SELECT count(*)
      INTO dolphin_owned_count
      FROM pg_catalog.pg_operator opr
      JOIN pg_catalog.pg_namespace nsp
        ON nsp.oid = opr.oprnamespace
      JOIN pg_catalog.pg_type left_type
        ON left_type.oid = opr.oprleft
      JOIN pg_catalog.pg_namespace left_nsp
        ON left_nsp.oid = left_type.typnamespace
      JOIN pg_catalog.pg_type right_type
        ON right_type.oid = opr.oprright
      JOIN pg_catalog.pg_namespace right_nsp
        ON right_nsp.oid = right_type.typnamespace
      JOIN pg_catalog.pg_depend dep
        ON dep.classid = 'pg_catalog.pg_operator'::regclass
       AND dep.objid = opr.oid
       AND dep.objsubid = 0
       AND dep.refclassid = 'pg_catalog.pg_extension'::regclass
       AND dep.deptype = 'e'
      JOIN pg_catalog.pg_extension ext
        ON ext.oid = dep.refobjid
       AND ext.extname = 'dolphin'
     WHERE nsp.nspname = 'pg_catalog'
       AND opr.oprname IN ('<>', '>', '<', '>=', '<=')
       AND opr.oprcode <> 0
       AND ((left_nsp.nspname = 'pg_catalog'
             AND left_type.typname = 'text'
             AND right_nsp.nspname = 'pg_catalog'
             AND right_type.typname = 'binary')
         OR (left_nsp.nspname = 'pg_catalog'
             AND left_type.typname = 'binary'
             AND right_nsp.nspname = 'pg_catalog'
             AND right_type.typname = 'text'));

    IF defined_count = 0 THEN
        EXECUTE $operator$
CREATE OPERATOR pg_catalog.<>(leftarg = text, rightarg = binary, COMMUTATOR = operator(pg_catalog.<>),
NEGATOR = operator(pg_catalog.=), procedure = pg_catalog.text_binary_ne, restrict = neqsel, join = neqjoinsel);
$operator$;

        EXECUTE $operator$
CREATE OPERATOR pg_catalog.<>(leftarg = binary, rightarg = text, COMMUTATOR = operator(pg_catalog.<>),
NEGATOR = operator(pg_catalog.=), procedure = pg_catalog.binary_text_ne, restrict = neqsel, join = neqjoinsel);
$operator$;

        EXECUTE $operator$
CREATE OPERATOR pg_catalog.>(leftarg = text, rightarg = binary, COMMUTATOR = operator(pg_catalog.<),
NEGATOR = operator(pg_catalog.<=), procedure = pg_catalog.text_binary_gt, restrict = scalargtsel,
join = scalargtjoinsel);
$operator$;

        EXECUTE $operator$
CREATE OPERATOR pg_catalog.>(leftarg = binary, rightarg = text, COMMUTATOR = operator(pg_catalog.<),
NEGATOR = operator(pg_catalog.<=), procedure = pg_catalog.binary_text_gt, restrict = scalargtsel,
join = scalargtjoinsel);
$operator$;

        EXECUTE $operator$
CREATE OPERATOR pg_catalog.<(leftarg = text, rightarg = binary, COMMUTATOR = operator(pg_catalog.>),
NEGATOR = operator(pg_catalog.>=), procedure = pg_catalog.text_binary_lt, restrict = scalarltsel,
join = scalarltjoinsel);
$operator$;

        EXECUTE $operator$
CREATE OPERATOR pg_catalog.<(leftarg = binary, rightarg = text, COMMUTATOR = operator(pg_catalog.>),
NEGATOR = operator(pg_catalog.>=), procedure = pg_catalog.binary_text_lt, restrict = scalarltsel,
join = scalarltjoinsel);
$operator$;

        EXECUTE $operator$
CREATE OPERATOR pg_catalog.>=(leftarg = text, rightarg = binary, COMMUTATOR = operator(pg_catalog.<=),
NEGATOR = operator(pg_catalog.<), procedure = pg_catalog.text_binary_ge, restrict = scalargtsel,
join = scalargtjoinsel);
$operator$;

        EXECUTE $operator$
CREATE OPERATOR pg_catalog.>=(leftarg = binary, rightarg = text, COMMUTATOR = operator(pg_catalog.<=),
NEGATOR = operator(pg_catalog.<), procedure = pg_catalog.binary_text_ge, restrict = scalargtsel,
join = scalargtjoinsel);
$operator$;

        EXECUTE $operator$
CREATE OPERATOR pg_catalog.<=(leftarg = text, rightarg = binary, COMMUTATOR = operator(pg_catalog.>=),
NEGATOR = operator(pg_catalog.>), procedure = pg_catalog.text_binary_le, restrict = scalarltsel,
join = scalarltjoinsel);
$operator$;

        EXECUTE $operator$
CREATE OPERATOR pg_catalog.<=(leftarg = binary, rightarg = text, COMMUTATOR = operator(pg_catalog.>=),
NEGATOR = operator(pg_catalog.>), procedure = pg_catalog.binary_text_le, restrict = scalarltsel,
join = scalarltjoinsel);
$operator$;
    ELSIF defined_count <> 10 OR dolphin_owned_count <> 10 THEN
        RAISE EXCEPTION 'cannot create Dolphin text/binary comparison operators: found % of 10 operators, % owned by Dolphin',
            defined_count, dolphin_owned_count;
    END IF;
END
$do$;

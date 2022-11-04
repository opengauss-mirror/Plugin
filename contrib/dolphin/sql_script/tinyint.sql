CREATE OPERATOR pg_catalog.+(
leftarg = int1, rightarg = int1, procedure = int1pl,
commutator=operator(pg_catalog.+)
);
COMMENT ON OPERATOR pg_catalog.+(int1, int1) IS 'int1pl';

CREATE OPERATOR pg_catalog.-(
leftarg = int1, rightarg = int1, procedure = int1mi,
commutator=operator(pg_catalog.-)
);
COMMENT ON OPERATOR pg_catalog.-(int1, int1) IS 'int1mi';

CREATE OPERATOR pg_catalog.*(
leftarg = int1, rightarg = int1, procedure = int1mul,
commutator=operator(pg_catalog.*)
);
COMMENT ON OPERATOR pg_catalog.*(int1, int1) IS 'int1mul';

CREATE OPERATOR pg_catalog./(
leftarg = int1, rightarg = int1, procedure = int1div,
commutator=operator(pg_catalog./)
);
COMMENT ON OPERATOR pg_catalog./(int1, int1) IS 'int1div';

CREATE OPERATOR pg_catalog.%(
leftarg = int1, rightarg = int1, procedure = int1mod,
commutator=operator(pg_catalog.%)
);
COMMENT ON OPERATOR pg_catalog.%(int1, int1) IS 'int1mod';

CREATE OPERATOR pg_catalog.&(
leftarg = int1, rightarg = int1, procedure = int1and,
commutator=operator(pg_catalog.&)
);
COMMENT ON OPERATOR pg_catalog.&(int1, int1) IS 'int1and';

CREATE OPERATOR pg_catalog.|(
leftarg = int1, rightarg = int1, procedure = int1or,
commutator=operator(pg_catalog.|)
);
COMMENT ON OPERATOR pg_catalog.|(int1, int1) IS 'int1or';

CREATE OPERATOR pg_catalog.#(
leftarg = int1, rightarg = int1, procedure = int1xor,
commutator=operator(pg_catalog.#)
);
COMMENT ON OPERATOR pg_catalog.#(int1, int1) IS 'int1xor';

CREATE OPERATOR pg_catalog.>>(
leftarg = int1, rightarg = int4, procedure = int1shr
);
COMMENT ON OPERATOR pg_catalog.>>(int1, int4) IS 'int1shr';

CREATE OPERATOR pg_catalog.<<(
leftarg = int1, rightarg = int4, procedure = int1shl
);
COMMENT ON OPERATOR pg_catalog.<<(int1, int4) IS 'int1shl';

CREATE OPERATOR pg_catalog.~(
rightarg = int1, procedure = int1not
);

CREATE OPERATOR pg_catalog.+(
rightarg = int1, procedure = int1up
);

CREATE OPERATOR pg_catalog.-(
rightarg = int1, procedure = int1um
);

CREATE OPERATOR pg_catalog.@(
rightarg = int1, procedure = int1abs
);

DROP FUNCTION IF EXISTS pg_catalog.int1_accum(numeric[], int1) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.int1_accum(numeric[], int1)
 RETURNS numeric[] LANGUAGE C STRICT AS  '$libdir/dolphin',  'int1_accum';

drop aggregate if exists pg_catalog.stddev_pop(int1);
create aggregate pg_catalog.stddev_pop(int1) (SFUNC=int1_accum, cFUNC=numeric_collect, STYPE= numeric[], finalfunc = numeric_stddev_pop, initcond = '{0,0,0}', initcollect = '{0,0,0}');

DROP FUNCTION IF EXISTS pg_catalog.int1_list_agg_transfn(internal, int1, text) CASCADE;

CREATE OR REPLACE FUNCTION pg_catalog.int1_list_agg_transfn(internal, int1, text)
 RETURNS internal LANGUAGE C AS	'$libdir/dolphin',	'int1_list_agg_transfn';

DROP FUNCTION IF EXISTS pg_catalog.int1_list_agg_noarg2_transfn(internal, int1) CASCADE;

CREATE OR REPLACE FUNCTION pg_catalog.int1_list_agg_noarg2_transfn(internal, int1)
 RETURNS internal LANGUAGE C  AS '$libdir/dolphin',	'int1_list_agg_noarg2_transfn';

drop aggregate if exists pg_catalog.listagg(int1,text);
create aggregate pg_catalog.listagg(int1,text) (SFUNC=int1_list_agg_transfn, finalfunc = list_agg_finalfn,  STYPE= internal);

drop aggregate if exists pg_catalog.listagg(int1);
create aggregate pg_catalog.listagg(int1) (SFUNC=int1_list_agg_noarg2_transfn, finalfunc = list_agg_finalfn,  STYPE= internal);

drop aggregate if exists pg_catalog.var_pop(int1);
create aggregate pg_catalog.var_pop(int1) (SFUNC=int1_accum, cFUNC=numeric_collect, STYPE= numeric[], finalfunc = numeric_var_pop, initcond = '{0,0,0}', initcollect = '{0,0,0}');

drop aggregate if exists pg_catalog.var_samp(int1);
create aggregate pg_catalog.var_samp(int1) (SFUNC=int1_accum, cFUNC=numeric_collect, STYPE= numeric[], finalfunc = numeric_var_samp, initcond = '{0,0,0}', initcollect = '{0,0,0}');

drop aggregate if exists pg_catalog.variance(int1);
create aggregate pg_catalog.variance(int1) (SFUNC=int1_accum, cFUNC=numeric_collect, STYPE= numeric[], finalfunc = numeric_var_samp, initcond = '{0,0,0}', initcollect = '{0,0,0}');

CREATE OR REPLACE FUNCTION pg_catalog.int12eq (
int1,int2
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int12eq';

CREATE OR REPLACE FUNCTION pg_catalog.int12lt (
int1,int2
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int12lt';

CREATE OR REPLACE FUNCTION pg_catalog.int12le (
int1,int2
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int12le';

CREATE OR REPLACE FUNCTION pg_catalog.int12gt (
int1,int2
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int12gt';

CREATE OR REPLACE FUNCTION pg_catalog.int12ge (
int1,int2
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int12ge';

CREATE OR REPLACE FUNCTION pg_catalog.int14eq (
int1,int4
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int14eq';

CREATE OR REPLACE FUNCTION pg_catalog.int14lt (
int1,int4
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int14lt';

CREATE OR REPLACE FUNCTION pg_catalog.int14le (
int1,int4
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int14le';

CREATE OR REPLACE FUNCTION pg_catalog.int14gt (
int1,int4
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int14gt';

CREATE OR REPLACE FUNCTION pg_catalog.int14ge (
int1,int4
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int14ge';

CREATE OR REPLACE FUNCTION pg_catalog.int18eq (
int1,int8
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int18eq';

CREATE OR REPLACE FUNCTION pg_catalog.int18lt (
int1,int8
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int18lt';

CREATE OR REPLACE FUNCTION pg_catalog.int18le (
int1,int8
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int18le';

CREATE OR REPLACE FUNCTION pg_catalog.int18gt (
int1,int8
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int18gt';

CREATE OR REPLACE FUNCTION pg_catalog.int18ge (
int1,int8
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int18ge';

CREATE OPERATOR pg_catalog.=(
leftarg = int1, rightarg = int2, procedure = int12eq,
restrict = eqsel, join = eqjoinsel,
HASHES, MERGES
);
COMMENT ON OPERATOR pg_catalog.=(int1, int2) IS 'int12eq';

CREATE OPERATOR pg_catalog.<(
leftarg = int1, rightarg = int2, procedure = int12lt,
restrict = scalarltsel, join = scalarltjoinsel
);
COMMENT ON OPERATOR pg_catalog.<(int1, int2) IS 'int12lt';

CREATE OPERATOR pg_catalog.<=(
leftarg = int1, rightarg = int2, procedure = int12le,
restrict = scalarltsel, join = scalarltjoinsel
);
COMMENT ON OPERATOR pg_catalog.<=(int1, int2) IS 'int12le';

CREATE OPERATOR pg_catalog.>(
leftarg = int1, rightarg = int2, procedure = int12gt,
restrict = scalargtsel, join = scalargtjoinsel
);
COMMENT ON OPERATOR pg_catalog.>(int1, int2) IS 'int12gt';

CREATE OPERATOR pg_catalog.>=(
leftarg = int1, rightarg = int2, procedure = int12ge,
restrict = scalargtsel, join = scalargtjoinsel
);
COMMENT ON OPERATOR pg_catalog.>=(int1, int2) IS 'int12ge';

CREATE OPERATOR pg_catalog.=(
leftarg = int1, rightarg = int4, procedure = int14eq,
restrict = eqsel, join = eqjoinsel,
HASHES, MERGES
);
COMMENT ON OPERATOR pg_catalog.=(int1, int4) IS 'int14eq';

CREATE OPERATOR pg_catalog.<(
leftarg = int1, rightarg = int4, procedure = int14lt,
restrict = scalarltsel, join = scalarltjoinsel
);
COMMENT ON OPERATOR pg_catalog.<(int1, int4) IS 'int14lt';

CREATE OPERATOR pg_catalog.<=(
leftarg = int1, rightarg = int4, procedure = int14le,
restrict = scalarltsel, join = scalarltjoinsel
);
COMMENT ON OPERATOR pg_catalog.<=(int1, int4) IS 'int14le';

CREATE OPERATOR pg_catalog.>(
leftarg = int1, rightarg = int4, procedure = int14gt,
restrict = scalargtsel, join = scalargtjoinsel
);
COMMENT ON OPERATOR pg_catalog.>(int1, int4) IS 'int14gt';

CREATE OPERATOR pg_catalog.>=(
leftarg = int1, rightarg = int4, procedure = int14ge,
restrict = scalargtsel, join = scalargtjoinsel
);
COMMENT ON OPERATOR pg_catalog.>=(int1, int4) IS 'int14ge';

CREATE OPERATOR pg_catalog.=(
leftarg = int1, rightarg = int8, procedure = int18eq,
restrict = eqsel, join = eqjoinsel,
HASHES, MERGES
);
COMMENT ON OPERATOR pg_catalog.=(int1, int8) IS 'int18eq';

CREATE OPERATOR pg_catalog.<(
leftarg = int1, rightarg = int8, procedure = int18lt,
restrict = scalarltsel, join = scalarltjoinsel
);
COMMENT ON OPERATOR pg_catalog.<(int1, int8) IS 'int18lt';

CREATE OPERATOR pg_catalog.<=(
leftarg = int1, rightarg = int8, procedure = int18le,
restrict = scalarltsel, join = scalarltjoinsel
);
COMMENT ON OPERATOR pg_catalog.<=(int1, int8) IS 'int18le';

CREATE OPERATOR pg_catalog.>(
leftarg = int1, rightarg = int8, procedure = int18gt,
restrict = scalargtsel, join = scalargtjoinsel
);
COMMENT ON OPERATOR pg_catalog.>(int1, int8) IS 'int18gt';

CREATE OPERATOR pg_catalog.>=(
leftarg = int1, rightarg = int8, procedure = int18ge,
restrict = scalargtsel, join = scalargtjoinsel
);
COMMENT ON OPERATOR pg_catalog.>=(int1, int8) IS 'int18ge';

CREATE OR REPLACE FUNCTION pg_catalog.int12cmp (
int1,int2
) RETURNS int4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int12cmp';

CREATE OR REPLACE FUNCTION pg_catalog.int14cmp (
int1,int4
) RETURNS int4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int14cmp';

CREATE OR REPLACE FUNCTION pg_catalog.int18cmp (
int1,int8
) RETURNS int4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int18cmp';

CREATE OPERATOR CLASS int1_ops
    FOR TYPE int1 USING btree family int1_ops AS
        OPERATOR        1       <(int1, int2),
        OPERATOR        1       <(int1, int4),
        OPERATOR        1       <(int1, int8),
        OPERATOR        2       <=(int1, int2),
        OPERATOR        2       <=(int1, int4),
        OPERATOR        2       <=(int1, int8),
        OPERATOR        3       =(int1, int2),
        OPERATOR        3       =(int1, int4),
        OPERATOR        3       =(int1, int8),
        OPERATOR        4       >=(int1, int2),
        OPERATOR        4       >=(int1, int4),
        OPERATOR        4       >=(int1, int8),
        OPERATOR        5       >(int1, int2),
        OPERATOR        5       >(int1, int4),
        OPERATOR        5       >(int1, int8),
        FUNCTION        1       int12cmp(int1, int2),
        FUNCTION        1       int14cmp(int1, int4),
        FUNCTION        1       int18cmp(int1, int8);

CREATE OPERATOR CLASS int1_ops
    FOR TYPE int1 USING cbtree family int1_ops AS
        OPERATOR        1       <(int1, int2),
        OPERATOR        1       <(int1, int4),
        OPERATOR        1       <(int1, int8),
        OPERATOR        2       <=(int1, int2),
        OPERATOR        2       <=(int1, int4),
        OPERATOR        2       <=(int1, int8),
        OPERATOR        3       =(int1, int2),
        OPERATOR        3       =(int1, int4),
        OPERATOR        3       =(int1, int8),
        OPERATOR        4       >=(int1, int2),
        OPERATOR        4       >=(int1, int4),
        OPERATOR        4       >=(int1, int8),
        OPERATOR        5       >(int1, int2),
        OPERATOR        5       >(int1, int4),
        OPERATOR        5       >(int1, int8),
        FUNCTION        1       int12cmp(int1, int2),
        FUNCTION        1       int14cmp(int1, int4),
        FUNCTION        1       int18cmp(int1, int8);

CREATE OPERATOR CLASS int1_ops
    FOR TYPE int1 USING ubtree family int1_ops AS
        OPERATOR        1       <(int1, int2),
        OPERATOR        1       <(int1, int4),
        OPERATOR        1       <(int1, int8),
        OPERATOR        2       <=(int1, int2),
        OPERATOR        2       <=(int1, int4),
        OPERATOR        2       <=(int1, int8),
        OPERATOR        3       =(int1, int2),
        OPERATOR        3       =(int1, int4),
        OPERATOR        3       =(int1, int8),
        OPERATOR        4       >=(int1, int2),
        OPERATOR        4       >=(int1, int4),
        OPERATOR        4       >=(int1, int8),
        OPERATOR        5       >(int1, int2),
        OPERATOR        5       >(int1, int4),
        OPERATOR        5       >(int1, int8),
        FUNCTION        1       int12cmp(int1, int2),
        FUNCTION        1       int14cmp(int1, int4),
        FUNCTION        1       int18cmp(int1, int8);

CREATE OPERATOR CLASS int1_ops
    FOR TYPE int1 USING hash family int1_ops AS
        OPERATOR        1       =(int1, int2),
        OPERATOR        1       =(int1, int4),
        OPERATOR        1       =(int1, int8),
        FUNCTION        1       hashint2(int2),
        FUNCTION        1       hashint4(int4),
        FUNCTION        1       hashint8(int8);

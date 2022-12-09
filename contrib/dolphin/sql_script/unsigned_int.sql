--------------------------------------------------------------
-- add new type  uint1
--------------------------------------------------------------
DROP TYPE IF EXISTS pg_catalog.uint1 CASCADE;
DROP TYPE IF EXISTS pg_catalog._uint1 CASCADE;

CREATE TYPE pg_catalog.uint1;
CREATE TYPE pg_catalog._uint1;

DROP FUNCTION IF EXISTS pg_catalog.uint1in(cstring) CASCADE;
CREATE FUNCTION pg_catalog.uint1in (
cstring
) RETURNS uint1 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint1in';

DROP FUNCTION IF EXISTS pg_catalog.uint1out(uint1) CASCADE;
CREATE FUNCTION pg_catalog.uint1out (
uint1
) RETURNS cstring LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'uint1out';

DROP FUNCTION IF EXISTS pg_catalog.uint1send(uint1) CASCADE;
CREATE FUNCTION pg_catalog.uint1send (
uint1
) RETURNS bytea LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'uint1send';

DROP FUNCTION IF EXISTS pg_catalog.uint1recv(internal) CASCADE;
CREATE FUNCTION pg_catalog.uint1recv (
internal
) RETURNS uint1 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'uint1recv';

CREATE TYPE pg_catalog.uint1 (input=uint1in, output=uint1out, RECEIVE = uint1recv, SEND = uint1send,  category='C' ,INTERNALLENGTH  = 1,PASSEDBYVALUE,alignment = char);
COMMENT ON TYPE pg_catalog.uint1 IS 'uint1';
COMMENT ON TYPE pg_catalog._uint1 IS '_uint1';

-- add new type  uint2
--------------------------------------------------------------
DROP TYPE IF EXISTS pg_catalog.uint2 CASCADE;
DROP TYPE IF EXISTS pg_catalog._uint2 CASCADE;

CREATE TYPE pg_catalog.uint2;
CREATE TYPE pg_catalog._uint2;

DROP FUNCTION IF EXISTS pg_catalog.uint2in(cstring) CASCADE;
CREATE FUNCTION pg_catalog.uint2in (
cstring
) RETURNS uint2 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint2in';

DROP FUNCTION IF EXISTS pg_catalog.uint2out(uint2) CASCADE;
CREATE FUNCTION pg_catalog.uint2out (
uint2
) RETURNS cstring LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'uint2out';

DROP FUNCTION IF EXISTS pg_catalog.uint2send(uint2) CASCADE;
CREATE FUNCTION pg_catalog.uint2send (
uint2

) RETURNS bytea LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'uint2send';

DROP FUNCTION IF EXISTS pg_catalog.uint2recv(internal) CASCADE;
CREATE FUNCTION pg_catalog.uint2recv (
internal
) RETURNS uint2 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'uint2recv';

CREATE TYPE pg_catalog.uint2 (input=uint2in, output=uint2out, RECEIVE = uint2recv, SEND = uint2send,  category='C' ,INTERNALLENGTH  = 2,PASSEDBYVALUE,alignment = int2);
COMMENT ON TYPE pg_catalog.uint2 IS 'uint2';
COMMENT ON TYPE pg_catalog._uint2 IS '_uint2';

-- add new type  uint4
--------------------------------------------------------------
DROP TYPE IF EXISTS pg_catalog.uint4 CASCADE;
DROP TYPE IF EXISTS pg_catalog._uint4 CASCADE;

CREATE TYPE pg_catalog.uint4;
CREATE TYPE pg_catalog._uint4;

DROP FUNCTION IF EXISTS pg_catalog.uint4in(cstring) CASCADE;
CREATE FUNCTION pg_catalog.uint4in (
cstring
) RETURNS uint4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint4in';

DROP FUNCTION IF EXISTS pg_catalog.uint4out(uint4) CASCADE;
CREATE FUNCTION pg_catalog.uint4out (
uint4
) RETURNS cstring LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'uint4out';

DROP FUNCTION IF EXISTS pg_catalog.uint4send(uint4) CASCADE;
CREATE FUNCTION pg_catalog.uint4send (
uint4

) RETURNS bytea LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'uint4send';

DROP FUNCTION IF EXISTS pg_catalog.uint4recv(internal) CASCADE;
CREATE FUNCTION pg_catalog.uint4recv (
internal
) RETURNS uint4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'uint4recv';

CREATE TYPE pg_catalog.uint4 (input=uint4in, output=uint4out, RECEIVE = uint4recv, SEND = uint4send,  category='C' ,INTERNALLENGTH  = 4,PASSEDBYVALUE,alignment = int4);
COMMENT ON TYPE pg_catalog.uint4 IS 'uint4';
COMMENT ON TYPE pg_catalog._uint4 IS '_uint4';

-- add new type  uint8
--------------------------------------------------------------
DROP TYPE IF EXISTS pg_catalog.uint8 CASCADE;
DROP TYPE IF EXISTS pg_catalog._uint8 CASCADE;

CREATE TYPE pg_catalog.uint8;
CREATE TYPE pg_catalog._uint8;

DROP FUNCTION IF EXISTS pg_catalog.uint8in(cstring) CASCADE;
CREATE FUNCTION pg_catalog.uint8in (
cstring
) RETURNS uint8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint8in';

DROP FUNCTION IF EXISTS pg_catalog.uint8out(uint8) CASCADE;
CREATE FUNCTION pg_catalog.uint8out (
uint8
) RETURNS cstring LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'uint8out';

DROP FUNCTION IF EXISTS pg_catalog.uint8send(uint1) CASCADE;
CREATE FUNCTION pg_catalog.uint8send (
uint8
) RETURNS bytea LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'uint8send';

DROP FUNCTION IF EXISTS pg_catalog.uint8recv(internal) CASCADE;
CREATE FUNCTION pg_catalog.uint8recv (
internal
) RETURNS uint8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'uint8recv';

CREATE TYPE pg_catalog.uint8 (input=uint8in, output=uint8out, RECEIVE = uint8recv, SEND = uint8send,  category='C' ,INTERNALLENGTH  = 8,PASSEDBYVALUE ,alignment = double);
COMMENT ON TYPE pg_catalog.uint8 IS 'uint8';
COMMENT ON TYPE pg_catalog._uint8 IS '_uint8';

----------------------------------------------------------------
-- add function
----------------------------------------------------------------
DROP FUNCTION IF EXISTS pg_catalog.i1toui1(int1) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.i2toui1(int2) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.i4toui1(int4) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.i8toui1(int8) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.f4toui1(float4) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.f8toui1(float8) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.ui2toui1(uint2) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.ui4toui1(uint4) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.ui8toui1(uint8) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.numeric_uint1(numeric) CASCADE;

DROP FUNCTION IF EXISTS pg_catalog.ui1toui2(uint1) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.ui1toui4(uint1) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.ui1toui8(uint1) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.ui1toi1(uint1) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.ui1toi2(uint1) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.ui1toi4(uint1) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.ui1toi8(uint1) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.ui1tof4(uint1) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.ui1tof8(uint1) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.uint1_numeric(uint1) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.uint1smaller(uint1,uint1) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.uint1larger(uint1,uint1) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.uint1eq(uint1,uint1) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.uint1ne(uint1,uint1) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.uint1lt(uint1,uint1) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.uint1le(uint1,uint1) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.uint1gt(uint1,uint1) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.uint1ge(uint1,uint1) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.uint1cmp(uint1,uint1) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.uint12cmp(uint1,uint2) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.uint14cmp(uint1,uint4) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.uint18cmp(uint1,uint8) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.uint1_int1cmp(uint1,int1) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.uint1_int2cmp(uint1,int2) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.uint1_int4cmp(uint1,int4) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.uint1_int8cmp(uint1,int8) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.hashuint1(uint1) CASCADE;

CREATE OR REPLACE FUNCTION pg_catalog.i1toui1 (
int1
) RETURNS uint1 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'i1toui1';

CREATE OR REPLACE FUNCTION pg_catalog.i2toui1 (
int2
) RETURNS uint1 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'i2toui1';

CREATE OR REPLACE FUNCTION  pg_catalog.i4toui1 (
int4
) RETURNS uint1 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'i4toui1';

CREATE OR REPLACE FUNCTION pg_catalog.i8toui1 (
int8
) RETURNS uint1 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'i8toui1';

CREATE OR REPLACE FUNCTION pg_catalog.f4toui1 (
float4
) RETURNS  uint1 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'f4toui1';
CREATE OR REPLACE FUNCTION pg_catalog.f8toui1 (
float8
) RETURNS uint1 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'f8toui1';

CREATE OR REPLACE FUNCTION pg_catalog.ui2toui1 (
uint2
) RETURNS uint1 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'ui2toui1';

CREATE OR REPLACE FUNCTION pg_catalog.ui4toui1 (
uint4
) RETURNS uint1 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'ui4toui1';

CREATE OR REPLACE FUNCTION pg_catalog.ui8toui1 (
uint8
) RETURNS uint1 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'ui8toui1';

CREATE OR REPLACE FUNCTION  pg_catalog.numeric_uint1 (
Numeric
) RETURNS  uint1 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'numeric_uint1';

CREATE OR REPLACE FUNCTION  pg_catalog.ui1toi1 (
uint1
) RETURNS int1 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'ui1toi1';
CREATE OR REPLACE FUNCTION pg_catalog.ui1toi2 (
uint1
) RETURNS int2 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'ui1toi2';

CREATE OR REPLACE FUNCTION  pg_catalog.ui1toi4 (
uint1
) RETURNS  int4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'ui1toi4';
CREATE OR REPLACE FUNCTION pg_catalog.ui1toi8 (
uint1
) RETURNS int8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'ui1toi8';

CREATE OR REPLACE FUNCTION pg_catalog.ui1tof4 (
uint1
) RETURNS  float4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'ui1tof4';
CREATE OR REPLACE FUNCTION pg_catalog.ui1tof8 (
uint1
) RETURNS float8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'ui1tof8';

CREATE OR REPLACE FUNCTION pg_catalog.ui1toui2 (
uint1
) RETURNS uint2 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'ui1toui2';

CREATE OR REPLACE FUNCTION pg_catalog.ui1toui4 (
uint1
) RETURNS  uint4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'ui1toui4';
CREATE OR REPLACE FUNCTION pg_catalog.ui1toui8 (
uint1
) RETURNS uint8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'ui1toui8';

CREATE OR REPLACE FUNCTION pg_catalog.uint1_numeric (
uint1
) RETURNS Numeric LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint1_numeric';

CREATE OR REPLACE FUNCTION pg_catalog.uint1smaller (
uint1,uint1
) RETURNS uint1 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint1smaller';

CREATE OR REPLACE FUNCTION pg_catalog.uint1larger (
uint1,uint1
) RETURNS uint1 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint1larger';

CREATE OR REPLACE FUNCTION pg_catalog.uint1eq (
uint1,uint1
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint1eq';

CREATE OR REPLACE FUNCTION pg_catalog.uint1ne (
uint1,uint1
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint1ne';

CREATE OR REPLACE FUNCTION pg_catalog.uint1lt (
uint1,uint1
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint1lt';

CREATE OR REPLACE FUNCTION pg_catalog.uint1le (
uint1,uint1
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint1le';

CREATE OR REPLACE FUNCTION pg_catalog.uint1gt (
uint1,uint1
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint1gt';

CREATE OR REPLACE FUNCTION pg_catalog.uint1ge (
uint1,uint1
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint1ge';

CREATE OR REPLACE FUNCTION pg_catalog.uint1cmp (
uint1,uint1
) RETURNS int4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint1cmp';

CREATE OR REPLACE FUNCTION pg_catalog.uint12cmp (
uint1,uint2
) RETURNS int4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint12cmp';

CREATE OR REPLACE FUNCTION pg_catalog.uint14cmp (
uint1,uint4
) RETURNS int4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint14cmp';

CREATE OR REPLACE FUNCTION pg_catalog.uint18cmp (
uint1,uint8
) RETURNS int4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint18cmp';

CREATE OR REPLACE FUNCTION pg_catalog.uint1_int1cmp (
uint1,int1
) RETURNS int4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint1_int1cmp';

CREATE OR REPLACE FUNCTION pg_catalog.uint1_int2cmp (
uint1,int2
) RETURNS int4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint1_int2cmp';

CREATE OR REPLACE FUNCTION pg_catalog.uint1_int4cmp (
uint1,int4
) RETURNS int4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint1_int4cmp';

CREATE OR REPLACE FUNCTION pg_catalog.uint1_int8cmp (
uint1,int8
) RETURNS int4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint1_int8cmp';

CREATE OR REPLACE FUNCTION pg_catalog.hashuint1 (
uint1
) RETURNS int4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'hashuint1';

DROP FUNCTION IF EXISTS pg_catalog.NUMTODAY(uint1) CASCADE;
CREATE OR REPLACE FUNCTION NUMTODAY(uint1)
RETURNS INTERVAL
AS $$ SELECT NUMTODSINTERVAL(uint1_numeric($1),'DAY')$$
LANGUAGE SQL IMMUTABLE STRICT;

DROP FUNCTION IF EXISTS pg_catalog.TO_TEXT(uint1) CASCADE;
CREATE OR REPLACE FUNCTION TO_TEXT(UINT1)
RETURNS TEXT
AS $$ select CAST(uint1out($1) AS VARCHAR2)  $$
LANGUAGE SQL  STRICT IMMUTABLE;

DROP FUNCTION IF EXISTS pg_catalog.uint1pl(uint1,uint1) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.uint1pl (
uint1,uint1
) RETURNS uint1 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint1pl';

DROP FUNCTION IF EXISTS pg_catalog.uint1mi(uint1,uint1) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.uint1mi (
uint1,uint1
) RETURNS uint1 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint1mi';

DROP FUNCTION IF EXISTS pg_catalog.uint1mul(uint1,uint1) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.uint1mul (
uint1,uint1
) RETURNS uint1 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint1mul';

DROP FUNCTION IF EXISTS pg_catalog.uint1div(uint1,uint1) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.uint1div (
uint1,uint1
) RETURNS float8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint1div';

DROP FUNCTION IF EXISTS pg_catalog.uint1abs(uint1) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.uint1abs (
uint1
) RETURNS uint1 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint1abs';

DROP FUNCTION IF EXISTS pg_catalog.uint1mod(uint1,uint1) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.uint1mod (
uint1,uint1
) RETURNS uint1 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint1mod';

DROP FUNCTION IF EXISTS pg_catalog.uint1and(uint1,uint1) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.uint1and (
uint1,uint1
) RETURNS uint1 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint1and';

DROP FUNCTION IF EXISTS pg_catalog.uint1or(uint1,uint1) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.uint1or (
uint1,uint1
) RETURNS uint1 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint1or';

DROP FUNCTION IF EXISTS pg_catalog.uint1xor(uint1,uint1) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.uint1xor (
uint1,uint1
) RETURNS uint1 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint1xor';

DROP FUNCTION IF EXISTS pg_catalog.uint1um(uint1) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.uint1um (
uint1
) RETURNS int2 LANGUAGE C STRICT IMMUTABLE  as '$libdir/dolphin',  'uint1um';

DROP FUNCTION IF EXISTS pg_catalog.uint1not(uint1) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.uint1not (
uint1
) RETURNS uint1 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint1not';

DROP FUNCTION IF EXISTS pg_catalog.uint1up(uint1) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.uint1up (
uint1
) RETURNS uint1 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint1up';

DROP FUNCTION IF EXISTS pg_catalog.uint1_avg_accum(bigint[], uint1) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.uint1_avg_accum(bigint[], uint1)
 RETURNS bigint[] LANGUAGE C STRICT AS  '$libdir/dolphin',  'uint1_avg_accum';
 
DROP FUNCTION IF EXISTS pg_catalog.uint1_accum(numeric[], uint1) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.uint1_accum(numeric[], uint1)
 RETURNS numeric[] LANGUAGE C STRICT AS  '$libdir/dolphin',  'uint1_accum';
 
DROP FUNCTION IF EXISTS pg_catalog.uint1_sum(int8, uint1) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.uint1_sum(int8, uint1)
RETURNS int8 LANGUAGE C AS  '$libdir/dolphin',  'uint1_sum';

DROP FUNCTION IF EXISTS pg_catalog.uint1_bool(uint1) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.uint1_bool(uint1)
RETURNS bool LANGUAGE C STRICT AS  '$libdir/dolphin',  'uint1_bool';

DROP FUNCTION IF EXISTS pg_catalog.bool_uint1(bool) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.bool_uint1(bool)
RETURNS uint1 LANGUAGE C STRICT AS  '$libdir/dolphin',  'bool_uint1';

CREATE OR REPLACE FUNCTION pg_catalog.CAST_TO_UNSIGNED(NUM NUMBER)
RETURNS UINT8
AS $$
BEGIN
    IF NUM IS NULL THEN
        RETURN NULL;
    ELSIF REGEXP_LIKE(NUM, '-.*\.') THEN
        RETURN 0;
    ELSIF NUM < -9223372036854775808 THEN
        RETURN 0;
    ELSIF NUM >= -9223372036854775808 and NUM < 0 THEN
        RETURN NUM + 18446744073709551616;
    ELSEIF NUM >= 0 and NUM <= 18446744073709551615 THEN
        RETURN NUM;
    ELSIF NUM > 18446744073709551615 THEN
		RETURN 18446744073709551615;
    END IF;
END;
$$ LANGUAGE PLPGSQL;

DROP FUNCTION IF EXISTS pg_catalog.uint1_list_agg_transfn(internal, uint1, text) CASCADE;

CREATE OR REPLACE FUNCTION pg_catalog.uint1_list_agg_transfn(internal, uint1, text)
 RETURNS internal LANGUAGE C AS	'$libdir/dolphin',	'uint1_list_agg_transfn';

DROP FUNCTION IF EXISTS pg_catalog.uint1_list_agg_noarg2_transfn(internal, uint1) CASCADE;

CREATE OR REPLACE FUNCTION pg_catalog.uint1_list_agg_noarg2_transfn(internal, uint1)
 RETURNS internal LANGUAGE C  AS '$libdir/dolphin',	'uint1_list_agg_noarg2_transfn';

 DROP FUNCTION IF EXISTS pg_catalog.numeric_cast_uint1(numeric) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.numeric_cast_uint1 (
numeric
) RETURNS uint1 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'numeric_cast_uint1';

DROP FUNCTION IF EXISTS pg_catalog.numeric_cast_uint2(numeric) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.numeric_cast_uint2 (
numeric
) RETURNS uint2 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'numeric_cast_uint2';

DROP FUNCTION IF EXISTS pg_catalog.numeric_cast_uint4(numeric) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.numeric_cast_uint4 (
numeric
) RETURNS uint4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'numeric_cast_uint4';

DROP FUNCTION IF EXISTS pg_catalog.numeric_cast_uint8(numeric) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.numeric_cast_uint8 (
numeric
) RETURNS uint8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'numeric_cast_uint8';
----------------------------------------------------------------
-- add cast
----------------------------------------------------------------
drop  CAST  IF EXISTS (int4 AS  uint1) CASCADE;
drop  CAST  IF EXISTS (int8 AS  uint1) CASCADE;
drop  CAST  IF EXISTS (int2 AS  uint1) CASCADE;
drop  CAST  IF EXISTS (int1 AS  uint1) CASCADE;
            
drop  CAST  IF EXISTS (TEXT AS  uint1) CASCADE;
drop  CAST  IF EXISTS (numeric AS  uint1) CASCADE;
drop  CAST  IF EXISTS (uint4 AS  uint1) CASCADE;
drop  CAST  IF EXISTS (uint8 AS  uint1) CASCADE;
drop  CAST  IF EXISTS (uint2 AS  uint1) CASCADE;
            
drop  CAST  IF EXISTS (uint1 AS  int4) CASCADE;
drop  CAST  IF EXISTS (uint1 AS  int8) CASCADE;
drop  CAST  IF EXISTS (uint1 AS  int2) CASCADE;
drop  CAST  IF EXISTS (uint1 AS  int1) CASCADE;
drop  CAST  IF EXISTS (uint1 AS  TEXT) CASCADE;
drop  CAST  IF EXISTS (uint1 AS  numeric) CASCADE;
drop  CAST  IF EXISTS (uint1 AS  INTERVAL) CASCADE;
drop  CAST  IF EXISTS (uint1 AS  uint4) CASCADE;
drop  CAST  IF EXISTS (uint1 AS  uint8) CASCADE;
drop  CAST  IF EXISTS (uint1 AS  uint2) CASCADE;
drop  CAST  IF EXISTS (uint1 AS  bool) CASCADE;
drop  CAST  IF EXISTS (bool AS  uint1) CASCADE;

CREATE CAST (int4 AS uint1) WITH FUNCTION i4toui1(int4) AS IMPLICIT;
CREATE CAST (int8 AS uint1) WITH FUNCTION i8toui1(int8) AS IMPLICIT;
CREATE CAST (int2 AS uint1) WITH FUNCTION i2toui1(int2) AS IMPLICIT;
CREATE CAST (int1 AS uint1) WITH FUNCTION i1toui1(int1) AS IMPLICIT;
CREATE CAST (float4 AS uint1) WITH FUNCTION f4toui1(float4) AS IMPLICIT;
CREATE CAST (float8 AS uint1) WITH FUNCTION f8toui1(float8) AS IMPLICIT;
CREATE CAST (uint8 AS uint1) WITH FUNCTION ui8toui1(uint8) AS IMPLICIT;
CREATE CAST (uint4 AS uint1) WITH FUNCTION ui4toui1(uint4) AS IMPLICIT;
CREATE CAST (uint2 AS uint1) WITH FUNCTION ui2toui1(uint2) AS IMPLICIT;

CREATE CAST (uint1 AS int1) WITH FUNCTION ui1toi1(uint1) AS IMPLICIT;
CREATE CAST (uint1 AS int4) WITH FUNCTION ui1toi4(uint1) AS IMPLICIT;
CREATE CAST (uint1 AS int8) WITH FUNCTION ui1toi8(uint1) AS IMPLICIT;
CREATE CAST (uint1 AS int2) WITH FUNCTION ui1toi2(uint1) AS IMPLICIT;
CREATE CAST (uint1 AS float4) WITH FUNCTION ui1tof4(uint1) AS IMPLICIT;
CREATE CAST (uint1 AS float8) WITH FUNCTION ui1tof8(uint1) AS IMPLICIT;
CREATE CAST (uint1 AS uint2) WITH FUNCTION ui1toui2(uint1) AS IMPLICIT;
CREATE CAST (uint1 AS uint4) WITH FUNCTION ui1toui4(uint1) AS IMPLICIT;
CREATE CAST (uint1 AS uint8) WITH FUNCTION ui1toui8(uint1) AS IMPLICIT;
CREATE CAST (uint1 AS bool) WITH FUNCTION uint1_bool(uint1) AS IMPLICIT;
CREATE CAST (bool AS uint1) WITH FUNCTION bool_uint1(bool) AS IMPLICIT;

CREATE CAST (UINT1 AS TEXT) WITH FUNCTION TO_TEXT(UINT1) AS IMPLICIT;
CREATE CAST (UINT1 AS numeric) WITH FUNCTION uint1_numeric(UINT1) AS IMPLICIT;
CREATE CAST (numeric AS UINT1) WITH FUNCTION numeric_uint1(Numeric) AS IMPLICIT;

CREATE CAST (UINT1 AS INTERVAL) WITH FUNCTION NUMTODAY(UINT1) AS IMPLICIT;
----------------------------------------------------------------
-- add  OPERATOR 
----------------------------------------------------------------

CREATE OPERATOR pg_catalog.=(
leftarg = uint1, rightarg = uint1, procedure = uint1eq,
commutator=operator(pg_catalog.=),
restrict = eqsel, join = eqjoinsel,
HASHES, MERGES
);
COMMENT ON OPERATOR pg_catalog.=(uint1, uint1) IS 'uint1eq';

CREATE OPERATOR pg_catalog.<>(
leftarg = uint1, rightarg = uint1, procedure = uint1ne,
restrict = neqsel, join = neqjoinsel
);
COMMENT ON OPERATOR pg_catalog.<>(uint1, uint1) IS 'uint1ne';

CREATE OPERATOR pg_catalog.<(
leftarg = uint1, rightarg = uint1, procedure = uint1lt,
restrict = scalarltsel, join = scalarltjoinsel
);
COMMENT ON OPERATOR pg_catalog.<(uint1, uint1) IS 'uint1lt';

CREATE OPERATOR pg_catalog.<=(
leftarg = uint1, rightarg = uint1, procedure = uint1le,
restrict = scalarltsel, join = scalarltjoinsel
);
COMMENT ON OPERATOR pg_catalog.<=(uint1, uint1) IS 'uint1le';

CREATE OPERATOR pg_catalog.>(
leftarg = uint1, rightarg = uint1, procedure = uint1gt,
restrict = scalargtsel, join = scalargtjoinsel
);
COMMENT ON OPERATOR pg_catalog.>(uint1, uint1) IS 'uint1gt';

CREATE OPERATOR pg_catalog.>=(
leftarg = uint1, rightarg = uint1, procedure = uint1ge,
restrict = scalargtsel, join = scalargtjoinsel
);
COMMENT ON OPERATOR pg_catalog.>=(uint1, uint1) IS 'uint1ge';

CREATE OPERATOR pg_catalog.+(
leftarg = uint1, rightarg = uint1, procedure = uint1pl,
commutator=operator(pg_catalog.+)
);
COMMENT ON OPERATOR pg_catalog.+(uint1, uint1) IS 'uint1pl';

CREATE OPERATOR pg_catalog.-(
leftarg = uint1, rightarg = uint1, procedure = uint1mi,
commutator=operator(pg_catalog.-)
);
COMMENT ON OPERATOR pg_catalog.-(uint1, uint1) IS 'uint1mi';

CREATE OPERATOR pg_catalog.*(
leftarg = uint1, rightarg = uint1, procedure = uint1mul,
commutator=operator(pg_catalog.*)
);
COMMENT ON OPERATOR pg_catalog.*(uint1, uint1) IS 'uint1mul';

CREATE OPERATOR pg_catalog./(
leftarg = uint1, rightarg = uint1, procedure = uint1div,
commutator=operator(pg_catalog./)
);
COMMENT ON OPERATOR pg_catalog./(uint1, uint1) IS 'uint1div';

CREATE OPERATOR pg_catalog.@(
rightarg = uint1,  procedure = uint1abs

);

CREATE OPERATOR pg_catalog.%(
leftarg = uint1, rightarg = uint1, procedure = uint1mod,
commutator=operator(pg_catalog.%)
);
COMMENT ON OPERATOR pg_catalog.%(uint1, uint1) IS 'uint1mod';

CREATE OPERATOR pg_catalog.+(
rightarg = uint1, procedure = uint1up
);

CREATE OPERATOR pg_catalog.-(
rightarg = uint1, procedure = uint1um
);

CREATE OR REPLACE FUNCTION uint1(text)
RETURNS uint1
LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'text_uint1';
CREATE CAST (TEXT AS UINT1) WITH FUNCTION UINT1(TEXT) AS IMPLICIT;
DROP FUNCTION IF EXISTS pg_catalog.uint1_int1_mod(uint1,int1) CASCADE;
CREATE FUNCTION pg_catalog.uint1_int1_mod (
uint1,int1
) RETURNS uint1 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint1_int1_mod';
CREATE OPERATOR pg_catalog.%(
leftarg = uint1, rightarg = int1, procedure = uint1_int1_mod,
commutator=operator(pg_catalog.%)
);
COMMENT ON OPERATOR pg_catalog.%(uint1, int1) IS 'uint1_int1_mod';

DROP FUNCTION IF EXISTS pg_catalog.int1_uint1_mod(int1,uint1) CASCADE;
CREATE FUNCTION pg_catalog.int1_uint1_mod (
int1,uint1
) RETURNS uint1 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int1_uint1_mod';
CREATE OPERATOR pg_catalog.%(
leftarg = int1, rightarg = uint1, procedure = int1_uint1_mod,
commutator=operator(pg_catalog.%)
);
COMMENT ON OPERATOR pg_catalog.%(int1, uint1) IS 'int1_uint1_mod';

-------------------uint1 int 1---------------

DROP FUNCTION IF EXISTS pg_catalog.uint1_pl_int1(uint1,int1) CASCADE;
CREATE FUNCTION pg_catalog.uint1_pl_int1 (
uint1,int1
) RETURNS uint1 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint1_pl_int1';

DROP FUNCTION IF EXISTS pg_catalog.uint1_mi_int1(uint1,int1) CASCADE;
CREATE FUNCTION pg_catalog.uint1_mi_int1 (
uint1,int1
) RETURNS uint1 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint1_mi_int1';

DROP FUNCTION IF EXISTS pg_catalog.uint1_mul_int1(uint1,int1) CASCADE;
CREATE FUNCTION pg_catalog.uint1_mul_int1 (
uint1,int1
) RETURNS uint1 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint1_mul_int1';

DROP FUNCTION IF EXISTS pg_catalog.uint1_div_int1(uint1,int1) CASCADE;
CREATE FUNCTION pg_catalog.uint1_div_int1 (
uint1,int1
) RETURNS float8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint1_div_int1';

DROP FUNCTION IF EXISTS pg_catalog.uint1_int1_eq(uint1,int1) CASCADE;
CREATE FUNCTION pg_catalog.uint1_int1_eq (
uint1,int1
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint1_int1_eq';

DROP FUNCTION IF EXISTS pg_catalog.uint1_int1_ne(uint1,int1) CASCADE;
CREATE FUNCTION pg_catalog.uint1_int1_ne (
uint1,int1
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint1_int1_ne';

DROP FUNCTION IF EXISTS pg_catalog.uint1_int1_lt(uint1,int1) CASCADE;
CREATE FUNCTION pg_catalog.uint1_int1_lt (
uint1,int1
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint1_int1_lt';

DROP FUNCTION IF EXISTS pg_catalog.uint1_int1_le(uint1,int1) CASCADE;
CREATE FUNCTION pg_catalog.uint1_int1_le (
uint1,int1
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint1_int1_le';

DROP FUNCTION IF EXISTS pg_catalog.uint1_int1_gt(uint1,int1) CASCADE;
CREATE FUNCTION pg_catalog.uint1_int1_gt (
uint1,int1
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint1_int1_gt';

DROP FUNCTION IF EXISTS pg_catalog.uint1_int1_ge(uint1,int1) CASCADE;
CREATE FUNCTION pg_catalog.uint1_int1_ge (
uint1,int1
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint1_int1_ge';

CREATE OPERATOR pg_catalog.+(
leftarg = uint1, rightarg = int1, procedure = uint1_pl_int1,
commutator=operator(pg_catalog.+)
);
COMMENT ON OPERATOR pg_catalog.+(uint1, int1) IS 'uint1_pl_int1';

CREATE OPERATOR pg_catalog.-(
leftarg = uint1, rightarg = int1, procedure = uint1_mi_int1,
commutator=operator(pg_catalog.-)
);
COMMENT ON OPERATOR pg_catalog.-(uint1, int1) IS 'uint1_mi_int1';

CREATE OPERATOR pg_catalog.*(
leftarg = uint1, rightarg = int1, procedure = uint1_mul_int1,
commutator=operator(pg_catalog.*)
);
COMMENT ON OPERATOR pg_catalog.*(uint1, int1) IS 'uint1_mul_int1';

CREATE OPERATOR pg_catalog./(
leftarg = uint1, rightarg = int1, procedure = uint1_div_int1,
commutator=operator(pg_catalog./)
);
COMMENT ON OPERATOR pg_catalog./(uint1, int1) IS 'uint1_div_int1';

CREATE OPERATOR pg_catalog.=(
leftarg = uint1, rightarg = int1, procedure = uint1_int1_eq,
restrict = eqsel, join = eqjoinsel,
HASHES, MERGES
);
COMMENT ON OPERATOR pg_catalog.=(uint1, int1) IS 'uint1_int1_eq';

CREATE OPERATOR pg_catalog.<>(
leftarg = uint1, rightarg = int1, procedure = uint1_int1_ne
);
COMMENT ON OPERATOR pg_catalog.<>(uint1, int1) IS 'uint1_int1_ne';

CREATE OPERATOR pg_catalog.<(
leftarg = uint1, rightarg = int1, procedure = uint1_int1_lt
);
COMMENT ON OPERATOR pg_catalog.<(uint1, int1) IS 'uint1_int1_lt';

CREATE OPERATOR pg_catalog.<=(
leftarg = uint1, rightarg = int1, procedure = uint1_int1_le
);
COMMENT ON OPERATOR pg_catalog.<=(uint1, int1) IS 'uint1_int1_le';

CREATE OPERATOR pg_catalog.>(
leftarg = uint1, rightarg = int1, procedure = uint1_int1_gt
);
COMMENT ON OPERATOR pg_catalog.>(uint1, int1) IS 'uint1_int1_gt';

CREATE OPERATOR pg_catalog.>=(
leftarg = uint1, rightarg = int1, procedure = uint1_int1_ge
);
COMMENT ON OPERATOR pg_catalog.>=(uint1, int1) IS 'uint1_int1_ge';
-------------------uint1 int 1---------------

-------------------int1 uint1---------------

DROP FUNCTION IF EXISTS pg_catalog.int1_pl_uint1(int1,uint1) CASCADE;
CREATE FUNCTION pg_catalog.int1_pl_uint1 (
int1,uint1
) RETURNS uint1 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int1_pl_uint1';

DROP FUNCTION IF EXISTS pg_catalog.int1_mi_uint1(int1,uint1) CASCADE;
CREATE FUNCTION pg_catalog.int1_mi_uint1 (
int1,uint1
) RETURNS uint1 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int1_mi_uint1';
DROP FUNCTION IF EXISTS pg_catalog.int1_mul_uint1(int1,uint1) CASCADE;
CREATE FUNCTION pg_catalog.int1_mul_uint1 (
int1,uint1
) RETURNS uint1 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int1_mul_uint1';

DROP FUNCTION IF EXISTS pg_catalog.int1_div_uint1(int1,uint1) CASCADE;
CREATE FUNCTION pg_catalog.int1_div_uint1 (
int1,uint1
) RETURNS float8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int1_div_uint1';

DROP FUNCTION IF EXISTS pg_catalog.int1_uint1_eq(int1,uint1) CASCADE;
CREATE FUNCTION pg_catalog.int1_uint1_eq (
int1,uint1
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int1_uint1_eq';

DROP FUNCTION IF EXISTS pg_catalog.int1_uint1_ne(int1,uint1) CASCADE;
CREATE FUNCTION pg_catalog.int1_uint1_ne (
int1,uint1
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int1_uint1_ne';

DROP FUNCTION IF EXISTS pg_catalog.int1_uint1_lt(int1,uint1) CASCADE;
CREATE FUNCTION pg_catalog.int1_uint1_lt (
int1,uint1
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int1_uint1_lt';

DROP FUNCTION IF EXISTS pg_catalog.int1_uint1_le(int1,uint1) CASCADE;
CREATE FUNCTION pg_catalog.int1_uint1_le (
int1,uint1
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int1_uint1_le';

DROP FUNCTION IF EXISTS pg_catalog.int1_uint1_gt(int1,uint1) CASCADE;
CREATE FUNCTION pg_catalog.int1_uint1_gt (
int1,uint1
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int1_uint1_gt';

DROP FUNCTION IF EXISTS pg_catalog.int1_uint1_ge(int1,uint1) CASCADE;
CREATE FUNCTION pg_catalog.int1_uint1_ge (
int1,uint1
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int1_uint1_ge';

CREATE OPERATOR pg_catalog.+(
leftarg = int1, rightarg = uint1, procedure = int1_pl_uint1,
commutator=operator(pg_catalog.+)
);
COMMENT ON OPERATOR pg_catalog.+(int1, uint1) IS 'int1_pl_uint1';

CREATE OPERATOR pg_catalog.-(
leftarg = int1, rightarg = uint1, procedure = int1_mi_uint1,
commutator=operator(pg_catalog.-)
);
COMMENT ON OPERATOR pg_catalog.-(int1, uint1) IS 'int1_mi_uint1';

CREATE OPERATOR pg_catalog.*(
leftarg = int1, rightarg = uint1, procedure = int1_mul_uint1,
commutator=operator(pg_catalog.*)
);
COMMENT ON OPERATOR pg_catalog.*(int1, uint1) IS 'int1_mul_uint1';

CREATE OPERATOR pg_catalog./(
leftarg = int1, rightarg = uint1, procedure = int1_div_uint1,
commutator=operator(pg_catalog./)
);
COMMENT ON OPERATOR pg_catalog./(int1, uint1) IS 'int1_div_uint1';

CREATE OPERATOR pg_catalog.=(
leftarg = int1, rightarg = uint1, procedure = int1_uint1_eq,
commutator=operator(pg_catalog.=),
restrict = eqsel, join = eqjoinsel,
HASHES, MERGES
);
COMMENT ON OPERATOR pg_catalog.=(int1, uint1) IS 'int1_uint1_eq';

CREATE OPERATOR pg_catalog.<>(
leftarg = int1, rightarg = uint1, procedure = int1_uint1_ne
);
COMMENT ON OPERATOR pg_catalog.<>(int1, uint1) IS 'int1_uint1_ne';

CREATE OPERATOR pg_catalog.<(
leftarg = int1, rightarg = uint1, procedure = int1_uint1_lt
);
COMMENT ON OPERATOR pg_catalog.<(int1, uint1) IS 'int1_uint1_lt';

CREATE OPERATOR pg_catalog.<=(
leftarg = int1, rightarg = uint1, procedure = int1_uint1_le
);
COMMENT ON OPERATOR pg_catalog.<=(int1, uint1) IS 'int1_uint1_le';

CREATE OPERATOR pg_catalog.>(
leftarg = int1, rightarg = uint1, procedure = int1_uint1_gt
);
COMMENT ON OPERATOR pg_catalog.>(int1, uint1) IS 'int1_uint1_gt';

CREATE OPERATOR pg_catalog.>=(
leftarg = int1, rightarg = uint1, procedure = int1_uint1_ge
);
COMMENT ON OPERATOR pg_catalog.>=(int1, uint1) IS 'int1_uint1_ge';

-------------------int1 uint1---------------

----------------------------------------------------------------
-- add  agg 
----------------------------------------------------------------

drop aggregate if exists pg_catalog.avg(uint1);
create aggregate pg_catalog.avg(uint1) (SFUNC=uint1_avg_accum, cFUNC=int8_avg_collect, STYPE= bigint[], finalfunc = int8_avg, initcond = '{0,0}', initcollect = '{0,0}');

drop aggregate if exists pg_catalog.sum(uint1);
create aggregate pg_catalog.sum(uint1) (SFUNC=uint1_sum, cFUNC=int8_sum_to_int8, STYPE= int8 );

drop aggregate if exists pg_catalog.min(uint1);
create aggregate pg_catalog.min(uint1) (SFUNC=uint1smaller,cFUNC = uint1smaller,  STYPE= uint1,SORTOP = '<');

drop aggregate if exists pg_catalog.max(uint1);
create aggregate pg_catalog.max(uint1) (SFUNC=uint1larger, cFUNC=uint1larger, STYPE= uint1,SORTOP = '>');

drop aggregate if exists pg_catalog.var_samp(uint1);
create aggregate pg_catalog.var_samp(uint1) (SFUNC=uint1_accum, cFUNC=numeric_collect, STYPE= numeric[], finalfunc = numeric_var_samp, initcond = '{0,0,0}', initcollect = '{0,0,0}');

drop aggregate if exists pg_catalog.var_pop(uint1);
create aggregate pg_catalog.var_pop(uint1) (SFUNC=uint1_accum, cFUNC=numeric_collect, STYPE= numeric[], finalfunc = numeric_var_pop, initcond = '{0,0,0}', initcollect = '{0,0,0}');

drop aggregate if exists pg_catalog.variance(uint1);
create aggregate pg_catalog.variance(uint1) (SFUNC=uint1_accum, cFUNC=numeric_collect, STYPE= numeric[], finalfunc = numeric_var_samp, initcond = '{0,0,0}', initcollect = '{0,0,0}');

drop aggregate if exists pg_catalog.stddev_pop(uint1);
create aggregate pg_catalog.stddev_pop(uint1) (SFUNC=uint1_accum, cFUNC=numeric_collect, STYPE= numeric[], finalfunc = numeric_stddev_pop, initcond = '{0,0,0}', initcollect = '{0,0,0}');

drop aggregate if exists pg_catalog.stddev_samp(uint1);
create aggregate pg_catalog.stddev_samp(uint1) (SFUNC=uint1_accum, cFUNC=numeric_collect, STYPE= numeric[], finalfunc = numeric_stddev_samp, initcond = '{0,0,0}', initcollect = '{0,0,0}');

drop aggregate if exists pg_catalog.stddev(uint1);
create aggregate pg_catalog.stddev(uint1) (SFUNC=uint1_accum, cFUNC=numeric_collect, STYPE= numeric[], finalfunc = numeric_stddev_samp, initcond = '{0,0,0}', initcollect = '{0,0,0}');

drop aggregate if exists pg_catalog.std(uint1);
create aggregate pg_catalog.std(uint1) (SFUNC=uint1_accum, cFUNC=numeric_collect, STYPE= numeric[], finalfunc = numeric_stddev_pop, initcond = '{0,0,0}', initcollect = '{0,0,0}');

drop aggregate if exists pg_catalog.bit_and(uint1);
create aggregate pg_catalog.bit_and(uint1) (SFUNC=uint1and, cFUNC = uint1and, STYPE= uint1);

drop aggregate if exists pg_catalog.bit_or(uint1);
create aggregate pg_catalog.bit_or(uint1) (SFUNC=uint1or, cFUNC = uint1or,  STYPE= uint1);
drop aggregate if exists pg_catalog.bit_xor(uint1);
create aggregate pg_catalog.bit_xor(uint1) (SFUNC=uint1xor, cFUNC = uint1xor,  STYPE= uint1);

drop aggregate if exists pg_catalog.listagg(uint1,text);
create aggregate pg_catalog.listagg(uint1,text) (SFUNC=uint1_list_agg_transfn, finalfunc = list_agg_finalfn,  STYPE= internal);

drop aggregate if exists pg_catalog.listagg(uint1);
create aggregate pg_catalog.listagg(uint1) (SFUNC=uint1_list_agg_noarg2_transfn, finalfunc = list_agg_finalfn,  STYPE= internal);

----------------------------------------------------------------
-- add UNIT2
----------------------------------------------------------------

----------------------------------------------------------------
-- add function
----------------------------------------------------------------
DROP FUNCTION IF EXISTS pg_catalog.i1toui2(int1) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.i2toui2(int2) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.i4toui2(int4) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.i8toui2(int8) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.f4toui2(float4) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.f8toui2(float8) CASCADE;

DROP FUNCTION IF EXISTS pg_catalog.ui4toui2(uint4) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.ui8toui2(uint8) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.numeric_uint2(numeric) CASCADE;

DROP FUNCTION IF EXISTS pg_catalog.ui2toui4(uint2) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.ui2toui8(uint2) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.ui2toi1(uint2) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.ui2toi2(uint2) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.ui2toi4(uint2) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.ui2toi8(uint2) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.ui2tof4(uint2) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.ui2tof8(uint2) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.uint2_numeric(uint2) CASCADE;

DROP FUNCTION IF EXISTS pg_catalog.uint2smaller(uint2,uint2) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.uint2larger(uint2,uint2) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.uint2eq(uint2,uint2) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.uint2ne(uint2,uint2) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.uint2lt(uint2,uint2) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.uint2le(uint2,uint2) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.uint2gt(uint2,uint2) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.uint2ge(uint2,uint2) CASCADE;

DROP FUNCTION IF EXISTS pg_catalog.uint2cmp(uint2,uint2) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.uint24cmp(uint2,uint4) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.uint28cmp(uint2,uint8) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.uint2_int2cmp(uint2,int2) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.uint2_int4cmp(uint2,int4) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.uint2_int8cmp(uint2,int8) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.hashuint2(uint2) CASCADE;

CREATE FUNCTION pg_catalog.i1toui2 (
int1
) RETURNS uint2 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'i1toui2';

CREATE FUNCTION pg_catalog.i2toui2 (
int2
) RETURNS uint2 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'i2toui2';

CREATE FUNCTION pg_catalog.i4toui2 (
int4
) RETURNS uint2 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'i4toui2';

CREATE FUNCTION pg_catalog.i8toui2 (
int8
) RETURNS uint2 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'i8toui2';

CREATE FUNCTION pg_catalog.f4toui2 (
float4
) RETURNS  uint2 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'f4toui2';
CREATE FUNCTION pg_catalog.f8toui2 (
float8
) RETURNS uint2 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'f8toui2';

CREATE FUNCTION pg_catalog.ui4toui2 (
uint4
) RETURNS uint2 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'ui4toui2';

CREATE FUNCTION pg_catalog.ui8toui2 (
uint8
) RETURNS uint2 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'ui8toui2';

CREATE FUNCTION pg_catalog.numeric_uint2 (
Numeric
) RETURNS  uint2 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'numeric_uint2';

CREATE FUNCTION pg_catalog.ui2toi1 (
uint2
) RETURNS int1 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'ui2toi1';
CREATE FUNCTION pg_catalog.ui2toi2 (
uint2
) RETURNS int2 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'ui2toi2';

CREATE FUNCTION pg_catalog.ui2toi4 (
uint2
) RETURNS  int4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'ui2toi4';
CREATE FUNCTION pg_catalog.ui2toi8 (
uint2
) RETURNS int8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'ui2toi8';

CREATE FUNCTION pg_catalog.ui2tof4 (
uint2
) RETURNS  float4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'ui2tof4';
CREATE FUNCTION pg_catalog.ui2tof8 (
uint2
) RETURNS float8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'ui2tof8';

CREATE FUNCTION pg_catalog.ui2toui4 (
uint2
) RETURNS  uint4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'ui2toui4';
CREATE FUNCTION pg_catalog.ui2toui8 (
uint2
) RETURNS uint8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'ui2toui8';

CREATE FUNCTION pg_catalog.uint2_numeric (
uint2
) RETURNS Numeric LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint2_numeric';

CREATE FUNCTION pg_catalog.uint2smaller (
uint2,uint2
) RETURNS uint2 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint2smaller';

CREATE FUNCTION pg_catalog.uint2larger (
uint2,uint2
) RETURNS uint2 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint2larger';

CREATE FUNCTION pg_catalog.uint2eq (
uint2,uint2
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint2eq';

CREATE FUNCTION pg_catalog.uint2ne (
uint2,uint2
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint2ne';

CREATE FUNCTION pg_catalog.uint2lt (
uint2,uint2
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint2lt';

CREATE FUNCTION pg_catalog.uint2le (
uint2,uint2
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint2le';

CREATE FUNCTION pg_catalog.uint2gt (
uint2,uint2
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint2gt';

CREATE FUNCTION pg_catalog.uint2ge (
uint2,uint2
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint2ge';

CREATE FUNCTION pg_catalog.uint2cmp (
uint2,uint2
) RETURNS int4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint2cmp';

CREATE FUNCTION pg_catalog.uint24cmp (
uint2,uint4
) RETURNS int4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint24cmp';

CREATE FUNCTION pg_catalog.uint28cmp (
uint2,uint8
) RETURNS int4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint28cmp';

CREATE FUNCTION pg_catalog.uint2_int2cmp (
uint2,int2
) RETURNS int4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint2_int2cmp';

CREATE FUNCTION pg_catalog.uint2_int4cmp (
uint2,int4
) RETURNS int4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint2_int4cmp';

CREATE FUNCTION pg_catalog.uint2_int8cmp (
uint2,int8
) RETURNS int4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint2_int8cmp';

CREATE FUNCTION pg_catalog.hashuint2 (
uint2
) RETURNS int4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'hashuint2';

CREATE OR REPLACE FUNCTION NUMTODAY(uint2)
RETURNS INTERVAL
AS $$ SELECT NUMTODSINTERVAL(uint2_numeric($1),'DAY')$$
LANGUAGE SQL IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION TO_TEXT(uint2)
RETURNS TEXT
AS $$ select CAST(uint2out($1) AS VARCHAR2)  $$
LANGUAGE SQL  STRICT IMMUTABLE;

DROP FUNCTION IF EXISTS pg_catalog.uint2_bool(uint2) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.uint2_bool(uint2)
RETURNS bool LANGUAGE C STRICT AS  '$libdir/dolphin',  'uint2_bool';

DROP FUNCTION IF EXISTS pg_catalog.bool_uint2(bool) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.bool_uint2(bool)
RETURNS uint2 LANGUAGE C STRICT AS  '$libdir/dolphin',  'bool_uint2';

DROP FUNCTION IF EXISTS pg_catalog.uint2pl(uint2,uint2) CASCADE;
CREATE FUNCTION pg_catalog.uint2pl (
uint2,uint2
) RETURNS uint2 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint2pl';

DROP FUNCTION IF EXISTS pg_catalog.uint2mi(uint2,uint2) CASCADE;
CREATE FUNCTION pg_catalog.uint2mi (
uint2,uint2
) RETURNS uint2 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint2mi';

DROP FUNCTION IF EXISTS pg_catalog.uint2mul(uint2,uint2) CASCADE;
CREATE FUNCTION pg_catalog.uint2mul (
uint2,uint2
) RETURNS uint2 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint2mul';

DROP FUNCTION IF EXISTS pg_catalog.uint2div(uint2,uint2) CASCADE;
CREATE FUNCTION pg_catalog.uint2div (
uint2,uint2
) RETURNS float8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint2div';

DROP FUNCTION IF EXISTS pg_catalog.uint2abs(uint2) CASCADE;
CREATE FUNCTION pg_catalog.uint2abs (
uint2
) RETURNS uint2 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint2abs';

DROP FUNCTION IF EXISTS pg_catalog.uint2mod(uint2,uint2) CASCADE;
CREATE FUNCTION pg_catalog.uint2mod (
uint2,uint2
) RETURNS uint2 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint2mod';

DROP FUNCTION IF EXISTS pg_catalog.uint2and(uint2,uint2) CASCADE;
CREATE FUNCTION pg_catalog.uint2and (
uint2,uint2
) RETURNS uint2 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint2and';

DROP FUNCTION IF EXISTS pg_catalog.uint2or(uint2,uint2) CASCADE;
CREATE FUNCTION pg_catalog.uint2or (
uint2,uint2
) RETURNS uint2 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint2or';

DROP FUNCTION IF EXISTS pg_catalog.uint2xor(uint2,uint2) CASCADE;
CREATE FUNCTION pg_catalog.uint2xor (
uint2,uint2
) RETURNS uint2 LANGUAGE C IMMUTABLE STRICT  as '$libdir/dolphin',  'uint2xor';

DROP FUNCTION IF EXISTS pg_catalog.uint2um(uint2) CASCADE;
CREATE FUNCTION pg_catalog.uint2um (
uint2
) RETURNS int4 LANGUAGE C IMMUTABLE STRICT  as '$libdir/dolphin',  'uint2um';

DROP FUNCTION IF EXISTS pg_catalog.uint2not(uint2) CASCADE;
CREATE FUNCTION pg_catalog.uint2not (
uint2
) RETURNS uint2 LANGUAGE C IMMUTABLE STRICT  as '$libdir/dolphin',  'uint2not';
DROP FUNCTION IF EXISTS pg_catalog.uint2up(uint2) CASCADE;
CREATE FUNCTION pg_catalog.uint2up (
uint2
) RETURNS uint2 LANGUAGE C IMMUTABLE STRICT  as '$libdir/dolphin',  'uint2up';

DROP FUNCTION IF EXISTS pg_catalog.uint2_avg_accum(bigint[], uint2) CASCADE;

CREATE OR REPLACE FUNCTION pg_catalog.uint2_avg_accum(bigint[], uint2)
 RETURNS bigint[] LANGUAGE C STRICT AS  '$libdir/dolphin',  'uint2_avg_accum';

DROP FUNCTION IF EXISTS pg_catalog.uint2_accum(numeric[], uint2) CASCADE;

CREATE OR REPLACE FUNCTION pg_catalog.uint2_accum(numeric[], uint2)
 RETURNS numeric[] LANGUAGE C STRICT AS  '$libdir/dolphin',  'uint2_accum';

DROP FUNCTION IF EXISTS pg_catalog.uint2_sum(int8, uint2) CASCADE;

CREATE OR REPLACE FUNCTION pg_catalog.uint2_sum(int8, uint2)
RETURNS int8 LANGUAGE C AS  '$libdir/dolphin',  'uint2_sum';

DROP FUNCTION IF EXISTS pg_catalog.uint2_list_agg_transfn(internal, uint2, text) CASCADE;

CREATE OR REPLACE FUNCTION pg_catalog.uint2_list_agg_transfn(internal, uint2, text)
 RETURNS internal LANGUAGE C AS	'$libdir/dolphin',	'uint2_list_agg_transfn';

DROP FUNCTION IF EXISTS pg_catalog.uint2_list_agg_noarg2_transfn(internal, uint2) CASCADE;

CREATE OR REPLACE FUNCTION pg_catalog.uint2_list_agg_noarg2_transfn(internal, uint2)
 RETURNS internal LANGUAGE C  AS '$libdir/dolphin',	'uint2_list_agg_noarg2_transfn';

----------------------------------------------------------------
-- add cast
----------------------------------------------------------------

drop  CAST  IF EXISTS(int4 AS  uint2) CASCADE;
drop  CAST  IF EXISTS(int8 AS  uint2) CASCADE;
drop  CAST  IF EXISTS(int2 AS  uint2) CASCADE;
drop  CAST  IF EXISTS(int1 AS  uint2) CASCADE;
drop  CAST  IF EXISTS(TEXT AS  uint2) CASCADE;
drop  CAST  IF EXISTS(numeric AS  uint2) CASCADE;
drop  CAST  IF EXISTS(uint4 AS  uint2) CASCADE;
drop  CAST  IF EXISTS(uint8 AS  uint2) CASCADE;
    
drop  CAST  IF EXISTS(uint2 AS  int4) CASCADE;
drop  CAST  IF EXISTS(uint2 AS  int8) CASCADE;
drop  CAST  IF EXISTS(uint2 AS  int2) CASCADE;
drop  CAST  IF EXISTS(uint2 AS  int1) CASCADE;
drop  CAST  IF EXISTS(uint2 AS  TEXT) CASCADE;
drop  CAST  IF EXISTS(uint2 AS  numeric) CASCADE;
drop  CAST  IF EXISTS(uint2 AS  INTERVAL) CASCADE;
drop  CAST  IF EXISTS(uint2 AS  uint4) CASCADE;
drop  CAST  IF EXISTS(uint2 AS  uint8) CASCADE;
drop  CAST  IF EXISTS (uint2 AS  bool) CASCADE;
drop  CAST  IF EXISTS (bool AS  uint2) CASCADE;

CREATE CAST (int4 AS uint2) WITH FUNCTION i4toui2(int4) AS IMPLICIT;
CREATE CAST (int8 AS uint2) WITH FUNCTION i8toui2(int8) AS IMPLICIT;
CREATE CAST (int2 AS uint2) WITH FUNCTION i2toui2(int2) AS IMPLICIT;
CREATE CAST (int1 AS uint2) WITH FUNCTION i1toui2(int1) AS IMPLICIT;
CREATE CAST (float4 AS uint2) WITH FUNCTION f4toui2(float4) AS IMPLICIT;
CREATE CAST (float8 AS uint2) WITH FUNCTION f8toui2(float8) AS IMPLICIT;
CREATE CAST (uint8 AS uint2) WITH FUNCTION ui8toui2(uint8) AS IMPLICIT;
CREATE CAST (uint4 AS uint2) WITH FUNCTION ui4toui2(uint4) AS IMPLICIT;

CREATE CAST (uint2 AS int1) WITH FUNCTION ui2toi1(uint2) AS IMPLICIT;
CREATE CAST (uint2 AS int4) WITH FUNCTION ui2toi4(uint2) AS IMPLICIT;
CREATE CAST (uint2 AS int8) WITH FUNCTION ui2toi8(uint2) AS IMPLICIT;
CREATE CAST (uint2 AS int2) WITH FUNCTION ui2toi2(uint2) AS IMPLICIT;
CREATE CAST (uint2 AS float4) WITH FUNCTION ui2tof4(uint2) AS IMPLICIT;
CREATE CAST (uint2 AS float8) WITH FUNCTION ui2tof8(uint2) AS IMPLICIT;

CREATE CAST (uint2 AS uint4) WITH FUNCTION ui2toui4(uint2) AS IMPLICIT;
CREATE CAST (uint2 AS uint8) WITH FUNCTION ui2toui8(uint2) AS IMPLICIT;

CREATE CAST (uint2 AS bool) WITH FUNCTION uint2_bool(uint2) AS IMPLICIT;
CREATE CAST (bool AS uint2) WITH FUNCTION bool_uint2(bool) AS IMPLICIT;

CREATE CAST (uint2 AS TEXT) WITH FUNCTION TO_TEXT(uint2) AS IMPLICIT;
CREATE CAST (uint2 AS numeric) WITH FUNCTION uint2_numeric(uint2) AS IMPLICIT;
CREATE CAST (numeric AS uint2) WITH FUNCTION numeric_uint2(Numeric) AS IMPLICIT;

CREATE CAST (uint2 AS INTERVAL) WITH FUNCTION NUMTODAY(uint2) AS IMPLICIT;

----------------------------------------------------------------
-- add  OPERATOR 
----------------------------------------------------------------

CREATE OPERATOR pg_catalog.=(
leftarg = uint2, rightarg = uint2, procedure = uint2eq,
commutator=operator(pg_catalog.=),
restrict = eqsel, join = eqjoinsel,
HASHES, MERGES
);
COMMENT ON OPERATOR pg_catalog.=(uint2, uint2) IS 'uint2eq';

CREATE OPERATOR pg_catalog.<>(
leftarg = uint2, rightarg = uint2, procedure = uint2ne,
restrict = neqsel, join = neqjoinsel
);
COMMENT ON OPERATOR pg_catalog.<>(uint2, uint2) IS 'uint2ne';

CREATE OPERATOR pg_catalog.<(
leftarg = uint2, rightarg = uint2, procedure = uint2lt,
restrict = scalarltsel, join = scalarltjoinsel
);
COMMENT ON OPERATOR pg_catalog.<(uint2, uint2) IS 'uint2lt';

CREATE OPERATOR pg_catalog.<=(
leftarg = uint2, rightarg = uint2, procedure = uint2le,
restrict = scalarltsel, join = scalarltjoinsel
);
COMMENT ON OPERATOR pg_catalog.<=(uint2, uint2) IS 'uint2le';

CREATE OPERATOR pg_catalog.>(
leftarg = uint2, rightarg = uint2, procedure = uint2gt,
restrict = scalargtsel, join = scalargtjoinsel
);
COMMENT ON OPERATOR pg_catalog.>(uint2, uint2) IS 'uint2gt';

CREATE OPERATOR pg_catalog.>=(
leftarg = uint2, rightarg = uint2, procedure = uint2ge,
restrict = scalargtsel, join = scalargtjoinsel
);
COMMENT ON OPERATOR pg_catalog.>=(uint2, uint2) IS 'uint2ge';

CREATE OPERATOR pg_catalog.+(
leftarg = uint2, rightarg = uint2, procedure = uint2pl,
commutator=operator(pg_catalog.+)
);
COMMENT ON OPERATOR pg_catalog.+(uint2, uint2) IS 'uint2pl';

CREATE OPERATOR pg_catalog.-(
leftarg = uint2, rightarg = uint2, procedure = uint2mi,
commutator=operator(pg_catalog.-)
);
COMMENT ON OPERATOR pg_catalog.-(uint2, uint2) IS 'uint2mi';

CREATE OPERATOR pg_catalog.*(
leftarg = uint2, rightarg = uint2, procedure = uint2mul,
commutator=operator(pg_catalog.*)
);
COMMENT ON OPERATOR pg_catalog.*(uint2, uint2) IS 'uint2mul';

CREATE OPERATOR pg_catalog./(
leftarg = uint2, rightarg = uint2, procedure = uint2div,
commutator=operator(pg_catalog./)
);
COMMENT ON OPERATOR pg_catalog./(uint2, uint2) IS 'uint2div';

CREATE OPERATOR pg_catalog.@(
rightarg = uint2,  procedure = uint2abs

);

CREATE OPERATOR pg_catalog.%(
leftarg = uint2, rightarg = uint2, procedure = uint2mod,
commutator=operator(pg_catalog.%)
);
COMMENT ON OPERATOR pg_catalog.%(uint2, uint2) IS 'uint2mod';

CREATE OPERATOR pg_catalog.+(
rightarg = uint2, procedure = uint2up
);

CREATE OPERATOR pg_catalog.-(
rightarg = uint2, procedure = uint2um
);

CREATE OR REPLACE FUNCTION uint2(text)
RETURNS uint2
LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'text_uint2';
CREATE CAST (TEXT AS uint2) WITH FUNCTION uint2(TEXT) AS IMPLICIT;

DROP FUNCTION IF EXISTS pg_catalog.uint2_int2_mod(uint2,int2) CASCADE;
CREATE FUNCTION pg_catalog.uint2_int2_mod (
uint2,int2
) RETURNS uint2 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint2_int2_mod';
CREATE OPERATOR pg_catalog.%(
leftarg = uint2, rightarg = int2, procedure = uint2_int2_mod,
commutator=operator(pg_catalog.%)
);
COMMENT ON OPERATOR pg_catalog.%(uint2, int2) IS 'uint2_int2_mod';

DROP FUNCTION IF EXISTS pg_catalog.int2_uint2_mod(int2,uint2) CASCADE;
CREATE FUNCTION pg_catalog.int2_uint2_mod (
int2,uint2
) RETURNS int2 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int2_uint2_mod';
CREATE OPERATOR pg_catalog.%(
leftarg = int2, rightarg = uint2, procedure = int2_uint2_mod,
commutator=operator(pg_catalog.%)
);
COMMENT ON OPERATOR pg_catalog.%(int2, uint2) IS 'int2_uint2_mod';

-------------------uint2 int2---------------
DROP FUNCTION IF EXISTS pg_catalog.uint2_pl_int2(uint2,int2) CASCADE;
CREATE FUNCTION pg_catalog.uint2_pl_int2 (
uint2,int2
) RETURNS uint2 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint2_pl_int2';

DROP FUNCTION IF EXISTS pg_catalog.uint2_mi_int2(uint2,int2) CASCADE;
CREATE FUNCTION pg_catalog.uint2_mi_int2 (
uint2,int2
) RETURNS uint2 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint2_mi_int2';

DROP FUNCTION IF EXISTS pg_catalog.uint2_mul_int2(uint2,int2) CASCADE;
CREATE FUNCTION pg_catalog.uint2_mul_int2 (
uint2,int2
) RETURNS uint2 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint2_mul_int2';

DROP FUNCTION IF EXISTS pg_catalog.uint2_div_int2(uint2,int2) CASCADE;
CREATE FUNCTION pg_catalog.uint2_div_int2 (
uint2,int2
) RETURNS float8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint2_div_int2';

DROP FUNCTION IF EXISTS pg_catalog.uint2_int2_eq(uint2,int2) CASCADE;
CREATE FUNCTION pg_catalog.uint2_int2_eq (
uint2,int2
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint2_int2_eq';

DROP FUNCTION IF EXISTS pg_catalog.uint2_int2_ne(uint2,int2) CASCADE;
CREATE FUNCTION pg_catalog.uint2_int2_ne (
uint2,int2
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint2_int2_ne';

DROP FUNCTION IF EXISTS pg_catalog.uint2_int2_lt(uint2,int2) CASCADE;
CREATE FUNCTION pg_catalog.uint2_int2_lt (
uint2,int2
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint2_int2_lt';

DROP FUNCTION IF EXISTS pg_catalog.uint2_int2_le(uint2,int2) CASCADE;
CREATE FUNCTION pg_catalog.uint2_int2_le (
uint2,int2
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint2_int2_le';

DROP FUNCTION IF EXISTS pg_catalog.uint2_int2_gt(uint2,int2) CASCADE;
CREATE FUNCTION pg_catalog.uint2_int2_gt (
uint2,int2
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint2_int2_gt';

DROP FUNCTION IF EXISTS pg_catalog.uint2_int2_ge(uint2,int2) CASCADE;
CREATE FUNCTION pg_catalog.uint2_int2_ge (
uint2,int2
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint2_int2_ge';

CREATE OPERATOR pg_catalog.+(
leftarg = uint2, rightarg = int2, procedure = uint2_pl_int2,
commutator=operator(pg_catalog.+)
);
COMMENT ON OPERATOR pg_catalog.+(uint2, uint2) IS 'uint2_pl_int2';

CREATE OPERATOR pg_catalog.-(
leftarg = uint2, rightarg = int2, procedure = uint2_mi_int2,
commutator=operator(pg_catalog.-)
);
COMMENT ON OPERATOR pg_catalog.-(uint2, int2) IS 'uint2_mi_int2';

CREATE OPERATOR pg_catalog.*(
leftarg = uint2, rightarg = int2, procedure = uint2_mul_int2,
commutator=operator(pg_catalog.*)
);
COMMENT ON OPERATOR pg_catalog.*(uint2, uint2) IS 'uint2_mul_int2';

CREATE OPERATOR pg_catalog./(
leftarg = uint2, rightarg = int2, procedure = uint2_div_int2,
commutator=operator(pg_catalog./)
);
COMMENT ON OPERATOR pg_catalog./(uint2, int2) IS 'uint2_div_int2';

CREATE OPERATOR pg_catalog.=(
leftarg = uint2, rightarg = int2, procedure = uint2_int2_eq,
restrict = eqsel, join = eqjoinsel,
HASHES, MERGES
);
COMMENT ON OPERATOR pg_catalog.=(uint2, int2) IS 'uint2_int2_eq';

CREATE OPERATOR pg_catalog.<>(
leftarg = uint2, rightarg = int2, procedure = uint2_int2_ne
);
COMMENT ON OPERATOR pg_catalog.<>(uint2, int2) IS 'uint2_int2_ne';

CREATE OPERATOR pg_catalog.<(
leftarg = uint2, rightarg = int2, procedure = uint2_int2_lt
);
COMMENT ON OPERATOR pg_catalog.<(uint2, int2) IS 'uint2_int2_lt';

CREATE OPERATOR pg_catalog.<=(
leftarg = uint2, rightarg = int2, procedure = uint2_int2_le
);
COMMENT ON OPERATOR pg_catalog.<=(uint2, int2) IS 'uint2_int2_le';

CREATE OPERATOR pg_catalog.>(
leftarg = uint2, rightarg = int2, procedure = uint2_int2_gt
);
COMMENT ON OPERATOR pg_catalog.>(uint2, int2) IS 'uint2_int2_gt';

CREATE OPERATOR pg_catalog.>=(
leftarg = uint2, rightarg = int2, procedure = uint2_int2_ge
);
COMMENT ON OPERATOR pg_catalog.>=(uint2, int2) IS 'uint2_int2_ge';

-------------------uint2 int2---------------

-------------------int2 uint2---------------

DROP FUNCTION IF EXISTS pg_catalog.int2_pl_uint2(int2,uint2) CASCADE;
CREATE FUNCTION pg_catalog.int2_pl_uint2 (
int2,uint2
) RETURNS uint2 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int2_pl_uint2';

DROP FUNCTION IF EXISTS pg_catalog.int2_mi_uint2(int2,uint2) CASCADE;
CREATE FUNCTION pg_catalog.int2_mi_uint2 (
int2,uint2
) RETURNS uint2 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int2_mi_uint2';
DROP FUNCTION IF EXISTS pg_catalog.int2_mul_uint2(int2,uint2) CASCADE;
CREATE FUNCTION pg_catalog.int2_mul_uint2 (
int2,uint2
) RETURNS uint2 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int2_mul_uint2';

DROP FUNCTION IF EXISTS pg_catalog.int2_div_uint2(int2,uint2) CASCADE;
CREATE FUNCTION pg_catalog.int2_div_uint2 (
int2,uint2
) RETURNS float8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int2_div_uint2';

DROP FUNCTION IF EXISTS pg_catalog.int2_uint2_eq(int2,uint2) CASCADE;
CREATE FUNCTION pg_catalog.int2_uint2_eq (
int2,uint2
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int2_uint2_eq';

DROP FUNCTION IF EXISTS pg_catalog.int2_uint2_ne(int2,uint2) CASCADE;
CREATE FUNCTION pg_catalog.int2_uint2_ne (
int2,uint2
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int2_uint2_ne';

DROP FUNCTION IF EXISTS pg_catalog.int2_uint2_lt(int2,uint2) CASCADE;
CREATE FUNCTION pg_catalog.int2_uint2_lt (
int2,uint2
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int2_uint2_lt';

DROP FUNCTION IF EXISTS pg_catalog.int2_uint2_le(int2,uint2) CASCADE;
CREATE FUNCTION pg_catalog.int2_uint2_le (
int2,uint2
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int2_uint2_le';

DROP FUNCTION IF EXISTS pg_catalog.int2_uint2_gt(int2,uint2) CASCADE;
CREATE FUNCTION pg_catalog.int2_uint2_gt (
int2,uint2
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int2_uint2_gt';

DROP FUNCTION IF EXISTS pg_catalog.int2_uint2_ge(int2,uint2) CASCADE;
CREATE FUNCTION pg_catalog.int2_uint2_ge (
int2,uint2
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int2_uint2_ge';

CREATE OPERATOR pg_catalog.+(
leftarg = int2, rightarg = uint2, procedure = int2_pl_uint2,
commutator=operator(pg_catalog.+)
);
COMMENT ON OPERATOR pg_catalog.+(int2, uint2) IS 'int2_pl_uint2';

CREATE OPERATOR pg_catalog.-(
leftarg = int2, rightarg = uint2, procedure = int2_mi_uint2,
commutator=operator(pg_catalog.-)
);
COMMENT ON OPERATOR pg_catalog.-(int2, uint2) IS 'int2_mi_uint2';

CREATE OPERATOR pg_catalog.*(
leftarg = int2, rightarg = uint2, procedure = int2_mul_uint2,
commutator=operator(pg_catalog.*)
);
COMMENT ON OPERATOR pg_catalog.*(int2, uint2) IS 'int2_mul_uint2';

CREATE OPERATOR pg_catalog./(
leftarg = int2, rightarg = uint2, procedure = int2_div_uint2,
commutator=operator(pg_catalog./)
);
COMMENT ON OPERATOR pg_catalog./(int2, uint2) IS 'int2_div_uint2';

CREATE OPERATOR pg_catalog.=(
leftarg = int2, rightarg = uint2, procedure = int2_uint2_eq,
commutator=operator(pg_catalog.=),
restrict = eqsel, join = eqjoinsel,
HASHES, MERGES
);
COMMENT ON OPERATOR pg_catalog.=(int2, uint2) IS 'int2_uint2_eq';

CREATE OPERATOR pg_catalog.<>(
leftarg = int2, rightarg = uint2, procedure = int2_uint2_ne
);
COMMENT ON OPERATOR pg_catalog.<>(int2, uint2) IS 'int2_uint2_ne';

CREATE OPERATOR pg_catalog.<(
leftarg = int2, rightarg = uint2, procedure = int2_uint2_lt
);
COMMENT ON OPERATOR pg_catalog.<(int2, uint2) IS 'int2_uint2_lt';

CREATE OPERATOR pg_catalog.<=(
leftarg = int2, rightarg = uint2, procedure = int2_uint2_le
);
COMMENT ON OPERATOR pg_catalog.<=(int2, uint2) IS 'int2_uint2_le';

CREATE OPERATOR pg_catalog.>(
leftarg = int2, rightarg = uint2, procedure = int2_uint2_gt
);
COMMENT ON OPERATOR pg_catalog.>(int2, uint2) IS 'int2_uint2_gt';

CREATE OPERATOR pg_catalog.>=(
leftarg = int2, rightarg = uint2, procedure = int2_uint2_ge
);
COMMENT ON OPERATOR pg_catalog.>=(int2, uint2) IS 'int2_uint2_ge';

-------------------int2 uint2---------------
----------------------------------------------------------------
-- add  agg 
----------------------------------------------------------------

drop aggregate if exists pg_catalog.avg(uint2);
create aggregate pg_catalog.avg(uint2) (SFUNC=uint2_avg_accum, cFUNC=int8_avg_collect, STYPE= bigint[], finalfunc = int8_avg, initcond = '{0,0}', initcollect = '{0,0}');

drop aggregate if exists pg_catalog.sum(uint2);
create aggregate pg_catalog.sum(uint2) (SFUNC=uint2_sum, cFUNC=int8_sum_to_int8, STYPE= int8 );

drop aggregate if exists pg_catalog.min(uint2);
create aggregate pg_catalog.min(uint2) (SFUNC=uint2smaller,cFUNC = uint2smaller,  STYPE= uint2,SORTOP = '<');

drop aggregate if exists pg_catalog.max(uint2);
create aggregate pg_catalog.max(uint2) (SFUNC=uint2larger, cFUNC=uint2larger, STYPE= uint2,SORTOP = '>');

drop aggregate if exists pg_catalog.var_samp(uint2);
create aggregate pg_catalog.var_samp(uint2) (SFUNC=uint2_accum, cFUNC=numeric_collect, STYPE= numeric[], finalfunc = numeric_var_samp, initcond = '{0,0,0}', initcollect = '{0,0,0}');

drop aggregate if exists pg_catalog.var_pop(uint2);
create aggregate pg_catalog.var_pop(uint2) (SFUNC=uint2_accum, cFUNC=numeric_collect, STYPE= numeric[], finalfunc = numeric_var_pop, initcond = '{0,0,0}', initcollect = '{0,0,0}');

drop aggregate if exists pg_catalog.variance(uint2);
create aggregate pg_catalog.variance(uint2) (SFUNC=uint2_accum, cFUNC=numeric_collect, STYPE= numeric[], finalfunc = numeric_var_samp, initcond = '{0,0,0}', initcollect = '{0,0,0}');

drop aggregate if exists pg_catalog.stddev_pop(uint2);
create aggregate pg_catalog.stddev_pop(uint2) (SFUNC=uint2_accum, cFUNC=numeric_collect, STYPE= numeric[], finalfunc = numeric_stddev_pop, initcond = '{0,0,0}', initcollect = '{0,0,0}');

drop aggregate if exists pg_catalog.stddev_samp(uint2);
create aggregate pg_catalog.stddev_samp(uint2) (SFUNC=uint2_accum, cFUNC=numeric_collect, STYPE= numeric[], finalfunc = numeric_stddev_samp, initcond = '{0,0,0}', initcollect = '{0,0,0}');

drop aggregate if exists pg_catalog.stddev(uint2);
create aggregate pg_catalog.stddev(uint2) (SFUNC=uint2_accum, cFUNC=numeric_collect, STYPE= numeric[], finalfunc = numeric_stddev_samp, initcond = '{0,0,0}', initcollect = '{0,0,0}');

drop aggregate if exists pg_catalog.std(uint2);
create aggregate pg_catalog.std(uint2) (SFUNC=uint2_accum, cFUNC=numeric_collect, STYPE= numeric[], finalfunc = numeric_stddev_pop, initcond = '{0,0,0}', initcollect = '{0,0,0}');

drop aggregate if exists pg_catalog.bit_and(uint2);
create aggregate pg_catalog.bit_and(uint2) (SFUNC=uint2and, cFUNC = uint2and, STYPE= uint2);

drop aggregate if exists pg_catalog.bit_or(uint2);
create aggregate pg_catalog.bit_or(uint2) (SFUNC=uint2or, cFUNC = uint2or,  STYPE= uint2);
drop aggregate if exists pg_catalog.bit_xor(uint2);
create aggregate pg_catalog.bit_xor(uint2) (SFUNC=uint2xor, cFUNC = uint2xor,  STYPE= uint2);

drop aggregate if exists pg_catalog.listagg(uint2,text);
create aggregate pg_catalog.listagg(uint2,text) (SFUNC=uint2_list_agg_transfn, finalfunc = list_agg_finalfn,  STYPE= internal);

drop aggregate if exists pg_catalog.listagg(uint2);
create aggregate pg_catalog.listagg(uint2) (SFUNC=uint2_list_agg_noarg2_transfn, finalfunc = list_agg_finalfn,  STYPE= internal);

----------------------------------------------------------------
-------------------uint4 ---------------
----------------------------------------------------------------

DROP FUNCTION IF EXISTS pg_catalog.i1toui4(int1) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.i2toui4(int2) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.i4toui4(int4) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.i8toui4(int8) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.f4toui4(float4) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.f8toui4(float8) CASCADE;

DROP FUNCTION IF EXISTS pg_catalog.ui8toui4(uint8) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.numeric_uint4(numeric) CASCADE;

DROP FUNCTION IF EXISTS pg_catalog.ui4toui8(uint4) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.ui4toi1(uint4) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.ui4toi2(uint4) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.ui4toi4(uint4) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.ui4toi8(uint4) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.ui4tof4(uint4) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.ui4tof8(uint4) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.uint4_numeric(uint4) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.uint4smaller(uint4,uint4) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.uint4larger(uint4,uint4) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.uint4eq(uint4,uint4) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.uint4ne(uint4,uint4) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.uint4lt(uint4,uint4) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.uint4le(uint4,uint4) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.uint4gt(uint4,uint4) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.uint4ge(uint4,uint4) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.uint4cmp(uint4,uint4) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.uint48cmp(uint4,uint8) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.uint4_int4cmp(uint4,int4) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.uint4_int8cmp(uint4,int8) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.hashuint4(uint4) CASCADE;
CREATE FUNCTION pg_catalog.i1toui4 (
int1
) RETURNS uint4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'i1toui4';
CREATE FUNCTION pg_catalog.i2toui4 (
int2
) RETURNS uint4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'i2toui4';
CREATE FUNCTION pg_catalog.i4toui4 (
int4
) RETURNS uint4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'i4toui4';
CREATE FUNCTION pg_catalog.i8toui4 (
int8
) RETURNS uint4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'i8toui4';
CREATE FUNCTION pg_catalog.f4toui4 (
float4
) RETURNS  uint4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'f4toui4';
CREATE FUNCTION pg_catalog.f8toui4 (
float8
) RETURNS uint4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'f8toui4';

CREATE FUNCTION pg_catalog.ui8toui4 (
uint8
) RETURNS uint4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'ui8toui4';
CREATE FUNCTION pg_catalog.numeric_uint4 (
Numeric
) RETURNS  uint4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'numeric_uint4';
CREATE FUNCTION pg_catalog.ui4toi1 (
uint4
) RETURNS int1 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'ui4toi1';
CREATE FUNCTION pg_catalog.ui4toi2 (
uint4
) RETURNS int2 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'ui4toi2';
CREATE FUNCTION pg_catalog.ui4toi4 (
uint4
) RETURNS  int4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'ui4toi4';
CREATE FUNCTION pg_catalog.ui4toi8 (
uint4
) RETURNS int8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'ui4toi8';
CREATE FUNCTION pg_catalog.ui4tof4 (
uint4
) RETURNS  float4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'ui4tof4';
CREATE FUNCTION pg_catalog.ui4tof8 (
uint4
) RETURNS float8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'ui4tof8';

CREATE FUNCTION pg_catalog.ui4toui8 (
uint4
) RETURNS uint8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'ui4toui8';
CREATE FUNCTION pg_catalog.uint4_numeric (
uint4
) RETURNS Numeric LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint4_numeric';
CREATE FUNCTION pg_catalog.uint4smaller (
uint4,uint4
) RETURNS uint4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint4smaller';
CREATE FUNCTION pg_catalog.uint4larger (
uint4,uint4
) RETURNS uint4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint4larger';
CREATE FUNCTION pg_catalog.uint4eq (
uint4,uint4
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint4eq';
CREATE FUNCTION pg_catalog.uint4ne (
uint4,uint4
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint4ne';
CREATE FUNCTION pg_catalog.uint4lt (
uint4,uint4
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint4lt';
CREATE FUNCTION pg_catalog.uint4le (
uint4,uint4
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint4le';
CREATE FUNCTION pg_catalog.uint4gt (
uint4,uint4
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint4gt';
CREATE FUNCTION pg_catalog.uint4ge (
uint4,uint4
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint4ge';
CREATE FUNCTION pg_catalog.uint4cmp (
uint4,uint4
) RETURNS int4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint4cmp';
CREATE FUNCTION pg_catalog.uint48cmp (
uint4,uint8
) RETURNS int4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint48cmp';
CREATE FUNCTION pg_catalog.uint4_int4cmp (
uint4,int4
) RETURNS int4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint4_int4cmp';
CREATE FUNCTION pg_catalog.uint4_int8cmp (
uint4,int8
) RETURNS int4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint4_int8cmp';
CREATE FUNCTION pg_catalog.hashuint4 (
uint4
) RETURNS int4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'hashuint4';
CREATE OR REPLACE FUNCTION NUMTODAY(uint4)
RETURNS INTERVAL
AS $$ SELECT NUMTODSINTERVAL(uint4_numeric($1),'DAY')$$
LANGUAGE SQL IMMUTABLE STRICT;
CREATE OR REPLACE FUNCTION TO_TEXT(uint4)
RETURNS TEXT
AS $$ select CAST(uint4out($1) AS VARCHAR2)  $$
LANGUAGE SQL  STRICT IMMUTABLE;

DROP FUNCTION IF EXISTS pg_catalog.uint4pl(uint4,uint4) CASCADE;
CREATE FUNCTION pg_catalog.uint4pl (
uint4,uint4
) RETURNS uint4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint4pl';
DROP FUNCTION IF EXISTS pg_catalog.uint4mi(uint4,uint4) CASCADE;
CREATE FUNCTION pg_catalog.uint4mi (
uint4,uint4
) RETURNS uint4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint4mi';
DROP FUNCTION IF EXISTS pg_catalog.uint4mul(uint4,uint4) CASCADE;
CREATE FUNCTION pg_catalog.uint4mul (
uint4,uint4
) RETURNS uint4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint4mul';
DROP FUNCTION IF EXISTS pg_catalog.uint4div(uint4,uint4) CASCADE;
CREATE FUNCTION pg_catalog.uint4div (
uint4,uint4
) RETURNS float8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint4div';
DROP FUNCTION IF EXISTS pg_catalog.uint4abs(uint4) CASCADE;
CREATE FUNCTION pg_catalog.uint4abs (
uint4
) RETURNS uint4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint4abs';
DROP FUNCTION IF EXISTS pg_catalog.uint4mod(uint4,uint4) CASCADE;
CREATE FUNCTION pg_catalog.uint4mod (
uint4,uint4
) RETURNS uint4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint4mod';
DROP FUNCTION IF EXISTS pg_catalog.uint4and(uint4,uint4) CASCADE;
CREATE FUNCTION pg_catalog.uint4and (
uint4,uint4
) RETURNS uint4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint4and';
DROP FUNCTION IF EXISTS pg_catalog.uint4or(uint4,uint4) CASCADE;
CREATE FUNCTION pg_catalog.uint4or (
uint4,uint4
) RETURNS uint4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint4or';
DROP FUNCTION IF EXISTS pg_catalog.uint4xor(uint4,uint4) CASCADE;
CREATE FUNCTION pg_catalog.uint4xor (
uint4,uint4
) RETURNS uint4 LANGUAGE C STRICT IMMUTABLE  as '$libdir/dolphin',  'uint4xor';

DROP FUNCTION IF EXISTS pg_catalog.uint4inc(uint4) CASCADE;
CREATE FUNCTION pg_catalog.uint4up (
uint4
) RETURNS uint4 LANGUAGE C STRICT IMMUTABLE  as '$libdir/dolphin',  'uint4up';
DROP FUNCTION IF EXISTS pg_catalog.uint4um(uint4) CASCADE;
CREATE FUNCTION pg_catalog.uint4um (
uint4
) RETURNS int8 LANGUAGE C STRICT IMMUTABLE  as '$libdir/dolphin',  'uint4um';
DROP FUNCTION IF EXISTS pg_catalog.uint4not(uint4) CASCADE;
CREATE FUNCTION pg_catalog.uint4not (
uint4
) RETURNS uint4 LANGUAGE C IMMUTABLE STRICT  as '$libdir/dolphin',  'uint4not';

DROP FUNCTION IF EXISTS pg_catalog.uint4_avg_accum(bigint[], uint4) CASCADE;

CREATE OR REPLACE FUNCTION pg_catalog.uint4_avg_accum(bigint[], uint4)
 RETURNS bigint[] LANGUAGE C IMMUTABLE STRICT AS  '$libdir/dolphin',  'uint4_avg_accum';

DROP FUNCTION IF EXISTS pg_catalog.uint4_accum(numeric[], uint4) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.uint4_accum(numeric[], uint4)
 RETURNS numeric[] LANGUAGE C STRICT AS  '$libdir/dolphin',  'uint4_accum';

DROP FUNCTION IF EXISTS pg_catalog.uint4_sum(int8, uint4) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.uint4_sum(int8, uint4)
RETURNS int8 LANGUAGE C AS  '$libdir/dolphin',  'uint4_sum';

DROP FUNCTION IF EXISTS pg_catalog.uint4_list_agg_transfn(internal, uint4, text) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.uint4_list_agg_transfn(internal, uint4, text)
 RETURNS internal LANGUAGE C AS	'$libdir/dolphin',	'uint4_list_agg_transfn';

DROP FUNCTION IF EXISTS pg_catalog.uint4_list_agg_noarg2_transfn(internal, uint4) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.uint4_list_agg_noarg2_transfn(internal, uint4)
 RETURNS internal LANGUAGE C  AS '$libdir/dolphin',	'uint4_list_agg_noarg2_transfn';

 
 DROP FUNCTION IF EXISTS pg_catalog.uint4_bool(uint4) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.uint4_bool(uint4)
RETURNS bool LANGUAGE C STRICT AS  '$libdir/dolphin',  'uint4_bool';

DROP FUNCTION IF EXISTS pg_catalog.bool_uint4(bool) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.bool_uint4(bool)
RETURNS uint4 LANGUAGE C STRICT  AS  '$libdir/dolphin',  'bool_uint4';
----------------------------------------------------------------
-- add cast
----------------------------------------------------------------

drop  CAST IF EXISTS(int4 AS  uint4) CASCADE;
drop  CAST IF EXISTS(int8 AS  uint4) CASCADE;
drop  CAST IF EXISTS(int2 AS  uint4) CASCADE;
drop  CAST IF EXISTS(int1 AS  uint4) CASCADE;
drop  CAST IF EXISTS(TEXT AS  uint4) CASCADE;
drop  CAST IF EXISTS(numeric AS  uint4) CASCADE;

drop  CAST IF EXISTS(uint8 AS  uint4) CASCADE;

drop  CAST IF EXISTS(uint4 AS  int4) CASCADE;
drop  CAST IF EXISTS(uint4 AS  int8) CASCADE;
drop  CAST IF EXISTS(uint4 AS  int2) CASCADE;
drop  CAST IF EXISTS(uint4 AS  int1) CASCADE;
drop  CAST IF EXISTS(uint4 AS  TEXT) CASCADE;
drop  CAST IF EXISTS(uint4 AS  numeric) CASCADE;
drop  CAST IF EXISTS(uint4 AS  INTERVAL) CASCADE;

drop  CAST IF EXISTS(uint4 AS  uint8) CASCADE;
drop  CAST  IF EXISTS (uint4 AS  bool) CASCADE;
drop  CAST  IF EXISTS (bool AS  uint4) CASCADE;

CREATE CAST (int4 AS uint4) WITH FUNCTION i4toui4(int4) AS IMPLICIT;
CREATE CAST (int8 AS uint4) WITH FUNCTION i8toui4(int8) AS IMPLICIT;
CREATE CAST (int2 AS uint4) WITH FUNCTION i2toui4(int2) AS IMPLICIT;
CREATE CAST (int1 AS uint4) WITH FUNCTION i1toui4(int1) AS IMPLICIT;
CREATE CAST (float4 AS uint4) WITH FUNCTION f4toui4(float4) AS IMPLICIT;
CREATE CAST (float8 AS uint4) WITH FUNCTION f8toui4(float8) AS IMPLICIT;
CREATE CAST (uint8 AS uint4) WITH FUNCTION ui8toui4(uint8) AS IMPLICIT;

CREATE CAST (uint4 AS int1) WITH FUNCTION ui4toi1(uint4) AS IMPLICIT;
CREATE CAST (uint4 AS int4) WITH FUNCTION ui4toi4(uint4) AS IMPLICIT;
CREATE CAST (uint4 AS int8) WITH FUNCTION ui4toi8(uint4) AS IMPLICIT;
CREATE CAST (uint4 AS int2) WITH FUNCTION ui4toi2(uint4) AS IMPLICIT;
CREATE CAST (uint4 AS float4) WITH FUNCTION ui4tof4(uint4) AS IMPLICIT;
CREATE CAST (uint4 AS float8) WITH FUNCTION ui4tof8(uint4) AS IMPLICIT;

CREATE CAST (uint4 AS uint8) WITH FUNCTION ui4toui8(uint4) AS IMPLICIT;

CREATE CAST (uint4 AS TEXT) WITH FUNCTION TO_TEXT(uint4) AS IMPLICIT;
CREATE CAST (uint4 AS numeric) WITH FUNCTION uint4_numeric(uint4) AS IMPLICIT;
CREATE CAST (numeric AS uint4) WITH FUNCTION numeric_uint4(Numeric) AS IMPLICIT;

CREATE CAST (uint4 AS INTERVAL) WITH FUNCTION NUMTODAY(uint4) AS IMPLICIT;

CREATE CAST (uint4 AS bool) WITH FUNCTION uint4_bool(uint4) AS IMPLICIT;
CREATE CAST (bool AS uint4) WITH FUNCTION bool_uint4(bool) AS IMPLICIT;

----------------------------------------------------------------
-- add  OPERATOR 
----------------------------------------------------------------

CREATE OPERATOR pg_catalog.=(
leftarg = uint4, rightarg = uint4, procedure = uint4eq,
commutator=operator(pg_catalog.=),
restrict = eqsel, join = eqjoinsel,
HASHES, MERGES
);
COMMENT ON OPERATOR pg_catalog.=(uint4, uint4) IS 'uint4eq';

CREATE OPERATOR pg_catalog.<>(
leftarg = uint4, rightarg = uint4, procedure = uint4ne,
restrict = neqsel, join = neqjoinsel
);
COMMENT ON OPERATOR pg_catalog.<>(uint4, uint4) IS 'uint4ne';

CREATE OPERATOR pg_catalog.<(
leftarg = uint4, rightarg = uint4, procedure = uint4lt,
restrict = scalarltsel, join = scalarltjoinsel
);
COMMENT ON OPERATOR pg_catalog.<(uint4, uint4) IS 'uint4lt';

CREATE OPERATOR pg_catalog.<=(
leftarg = uint4, rightarg = uint4, procedure = uint4le,
restrict = scalarltsel, join = scalarltjoinsel
);
COMMENT ON OPERATOR pg_catalog.<=(uint4, uint4) IS 'uint4le';

CREATE OPERATOR pg_catalog.>(
leftarg = uint4, rightarg = uint4, procedure = uint4gt,
restrict = scalargtsel, join = scalargtjoinsel
);
COMMENT ON OPERATOR pg_catalog.>(uint4, uint4) IS 'uint4gt';

CREATE OPERATOR pg_catalog.>=(
leftarg = uint4, rightarg = uint4, procedure = uint4ge,
restrict = scalargtsel, join = scalargtjoinsel
);
COMMENT ON OPERATOR pg_catalog.>=(uint4, uint4) IS 'uint4ge';

CREATE OPERATOR pg_catalog.+(
leftarg = uint4, rightarg = uint4, procedure = uint4pl,
commutator=operator(pg_catalog.+)
);
COMMENT ON OPERATOR pg_catalog.+(uint4, uint4) IS 'uint4pl';

CREATE OPERATOR pg_catalog.-(
leftarg = uint4, rightarg = uint4, procedure = uint4mi,
commutator=operator(pg_catalog.-)
);
COMMENT ON OPERATOR pg_catalog.-(uint4, uint4) IS 'uint4mi';

CREATE OPERATOR pg_catalog.*(
leftarg = uint4, rightarg = uint4, procedure = uint4mul,
commutator=operator(pg_catalog.*)
);
COMMENT ON OPERATOR pg_catalog.*(uint4, uint4) IS 'uint4mul';

CREATE OPERATOR pg_catalog./(
leftarg = uint4, rightarg = uint4, procedure = uint4div,
commutator=operator(pg_catalog./)
);
COMMENT ON OPERATOR pg_catalog./(uint4, uint4) IS 'uint4div';

CREATE OPERATOR pg_catalog.@(
rightarg = uint4,  procedure = uint4abs

);

CREATE OPERATOR pg_catalog.%(
leftarg = uint4, rightarg = uint4, procedure = uint4mod,
commutator=operator(pg_catalog.%)
);
COMMENT ON OPERATOR pg_catalog.%(uint4, uint4) IS 'uint4mod';

CREATE OPERATOR pg_catalog.+(
rightarg = uint4, procedure = uint4up
);

CREATE OPERATOR pg_catalog.-(
rightarg = uint4, procedure = uint4um
);

CREATE OR REPLACE FUNCTION uint4(text)
RETURNS uint4
LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'text_uint4';

CREATE CAST (TEXT AS uint4) WITH FUNCTION uint4(TEXT) AS IMPLICIT;

-------------------uint4 int4---------------
DROP FUNCTION IF EXISTS pg_catalog.uint4_pl_int4(uint4,int4) CASCADE;
CREATE FUNCTION pg_catalog.uint4_pl_int4 (
uint4,int4
) RETURNS uint4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint4_pl_int4';

DROP FUNCTION IF EXISTS pg_catalog.uint4_mi_int4(uint4,int4) CASCADE;
CREATE FUNCTION pg_catalog.uint4_mi_int4 (
uint4,int4
) RETURNS uint4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint4_mi_int4';
DROP FUNCTION IF EXISTS pg_catalog.uint4_mul_int4(uint4,int4) CASCADE;
CREATE FUNCTION pg_catalog.uint4_mul_int4 (
uint4,int4
) RETURNS uint4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint4_mul_int4';

DROP FUNCTION IF EXISTS pg_catalog.uint4_div_int4(uint4,int4) CASCADE;
CREATE FUNCTION pg_catalog.uint4_div_int4 (
uint4,int4
) RETURNS float8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint4_div_int4';

DROP FUNCTION IF EXISTS pg_catalog.uint4_int4_mod(uint4,int4) CASCADE;
CREATE FUNCTION pg_catalog.uint4_int4_mod (
uint4,int4
) RETURNS uint4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint4_int4_mod';

DROP FUNCTION IF EXISTS pg_catalog.uint4_int4_eq(uint4,int4) CASCADE;
CREATE FUNCTION pg_catalog.uint4_int4_eq (
uint4,int4
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint4_int4_eq';

DROP FUNCTION IF EXISTS pg_catalog.uint4_int4_ne(uint4,int4) CASCADE;
CREATE FUNCTION pg_catalog.uint4_int4_ne (
uint4,int4
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint4_int4_ne';

DROP FUNCTION IF EXISTS pg_catalog.uint4_int4_lt(uint4,int4) CASCADE;
CREATE FUNCTION pg_catalog.uint4_int4_lt (
uint4,int4
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint4_int4_lt';

DROP FUNCTION IF EXISTS pg_catalog.uint4_int4_le(uint4,int4) CASCADE;
CREATE FUNCTION pg_catalog.uint4_int4_le (
uint4,int4
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint4_int4_le';

DROP FUNCTION IF EXISTS pg_catalog.uint4_int4_gt(uint4,int4) CASCADE;
CREATE FUNCTION pg_catalog.uint4_int4_gt (
uint4,int4
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint4_int4_gt';

DROP FUNCTION IF EXISTS pg_catalog.uint4_int4_ge(uint4,int4) CASCADE;
CREATE FUNCTION pg_catalog.uint4_int4_ge (
uint4,int4
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint4_int4_ge';

CREATE OPERATOR pg_catalog.+(
leftarg = uint4, rightarg = int4, procedure = uint4_pl_int4,
commutator=operator(pg_catalog.+)
);
COMMENT ON OPERATOR pg_catalog.+(uint4, int4) IS 'uint4_pl_int4';

CREATE OPERATOR pg_catalog.-(
leftarg = uint4, rightarg = int4, procedure = uint4_mi_int4,
commutator=operator(pg_catalog.-)
);
COMMENT ON OPERATOR pg_catalog.-(uint4, int4) IS 'uint4_mi_int4';

CREATE OPERATOR pg_catalog.*(
leftarg = uint4, rightarg = int4, procedure = uint4_mul_int4,
commutator=operator(pg_catalog.*)
);
COMMENT ON OPERATOR pg_catalog.*(uint4, int4) IS 'uint4_mul_int4';

CREATE OPERATOR pg_catalog./(
leftarg = uint4, rightarg = int4, procedure = uint4_div_int4,
commutator=operator(pg_catalog./)
);
COMMENT ON OPERATOR pg_catalog./(uint4, int4) IS 'uint4_div_int4';

CREATE OPERATOR pg_catalog.%(
leftarg = uint4, rightarg = int4, procedure = uint4_int4_mod,
commutator=operator(pg_catalog.%)
);
COMMENT ON OPERATOR pg_catalog.%(uint4, int4) IS 'uint4_int4_mod';

CREATE OPERATOR pg_catalog.=(
leftarg = uint4, rightarg = int4, procedure = uint4_int4_eq,
restrict = eqsel, join = eqjoinsel,
HASHES, MERGES
);
COMMENT ON OPERATOR pg_catalog.=(uint4, int4) IS 'uint4_int4_eq';

CREATE OPERATOR pg_catalog.<>(
leftarg = uint4, rightarg = int4, procedure = uint4_int4_ne
);
COMMENT ON OPERATOR pg_catalog.<>(uint4, int4) IS 'uint4_int4_ne';

CREATE OPERATOR pg_catalog.<(
leftarg = uint4, rightarg = int4, procedure = uint4_int4_lt
);
COMMENT ON OPERATOR pg_catalog.<(uint4, int4) IS 'uint4_int4_lt';

CREATE OPERATOR pg_catalog.<=(
leftarg = uint4, rightarg = int4, procedure = uint4_int4_le
);
COMMENT ON OPERATOR pg_catalog.<=(uint4, int4) IS 'uint4_int4_le';

CREATE OPERATOR pg_catalog.>(
leftarg = uint4, rightarg = int4, procedure = uint4_int4_gt
);
COMMENT ON OPERATOR pg_catalog.>(uint4, int4) IS 'uint4_int4_gt';

CREATE OPERATOR pg_catalog.>=(
leftarg = uint4, rightarg = int4, procedure = uint4_int4_ge
);
COMMENT ON OPERATOR pg_catalog.>=(uint4, int4) IS 'uint4_int4_ge';

-------------------uint4 int4---------------

-------------------int4 uint4---------------
DROP FUNCTION IF EXISTS pg_catalog.int4_pl_uint4(int4,uint4) CASCADE;
CREATE FUNCTION pg_catalog.int4_pl_uint4 (
int4,uint4
) RETURNS uint4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int4_pl_uint4';

DROP FUNCTION IF EXISTS pg_catalog.int4_mi_uint4(int4,uint4) CASCADE;
CREATE FUNCTION pg_catalog.int4_mi_uint4 (
int4,uint4
) RETURNS uint4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int4_mi_uint4';
DROP FUNCTION IF EXISTS pg_catalog.int4_mul_uint4(int4,uint4) CASCADE;
CREATE FUNCTION pg_catalog.int4_mul_uint4 (
int4,uint4
) RETURNS uint4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int4_mul_uint4';

DROP FUNCTION IF EXISTS pg_catalog.int4_div_uint4(int4,uint4) CASCADE;
CREATE FUNCTION pg_catalog.int4_div_uint4 (
int4,uint4
) RETURNS float8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int4_div_uint4';

DROP FUNCTION IF EXISTS pg_catalog.int4_uint4_mod(int4,uint4) CASCADE;
CREATE FUNCTION pg_catalog.int4_uint4_mod (
int4,uint4
) RETURNS int4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int4_uint4_mod';

DROP FUNCTION IF EXISTS pg_catalog.int4_uint4_eq(int4,uint4) CASCADE;
CREATE FUNCTION pg_catalog.int4_uint4_eq (
int4,uint4
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int4_uint4_eq';

DROP FUNCTION IF EXISTS pg_catalog.int4_uint4_ne(int4,uint4) CASCADE;
CREATE FUNCTION pg_catalog.int4_uint4_ne (
int4,uint4
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int4_uint4_ne';

DROP FUNCTION IF EXISTS pg_catalog.int4_uint4_lt(int4,uint4) CASCADE;
CREATE FUNCTION pg_catalog.int4_uint4_lt (
int4,uint4
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int4_uint4_lt';

DROP FUNCTION IF EXISTS pg_catalog.int4_uint4_le(int4,uint4) CASCADE;
CREATE FUNCTION pg_catalog.int4_uint4_le (
int4,uint4
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int4_uint4_le';

DROP FUNCTION IF EXISTS pg_catalog.int4_uint4_gt(int4,uint4) CASCADE;
CREATE FUNCTION pg_catalog.int4_uint4_gt (
int4,uint4
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int4_uint4_gt';

DROP FUNCTION IF EXISTS pg_catalog.int4_uint4_ge(int4,uint4) CASCADE;
CREATE FUNCTION pg_catalog.int4_uint4_ge (
int4,uint4
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int4_uint4_ge';

CREATE OPERATOR pg_catalog.+(
leftarg = int4, rightarg = uint4, procedure = int4_pl_uint4,
commutator=operator(pg_catalog.+)
);
COMMENT ON OPERATOR pg_catalog.+(int4, uint4) IS 'int4_pl_uint4';

CREATE OPERATOR pg_catalog.-(
leftarg = int4, rightarg = uint4, procedure = int4_mi_uint4,
commutator=operator(pg_catalog.-)
);
COMMENT ON OPERATOR pg_catalog.-(int4, uint4) IS 'int4_mi_uint4';

CREATE OPERATOR pg_catalog.*(
leftarg = int4, rightarg = uint4, procedure = int4_mul_uint4,
commutator=operator(pg_catalog.*)
);
COMMENT ON OPERATOR pg_catalog.*(int4, uint4) IS 'int4_mul_uint4';

CREATE OPERATOR pg_catalog./(
leftarg = int4, rightarg = uint4, procedure = int4_div_uint4,
commutator=operator(pg_catalog./)
);
COMMENT ON OPERATOR pg_catalog./(int4, uint4) IS 'int4_div_uint4';

CREATE OPERATOR pg_catalog.%(
leftarg = int4, rightarg = uint4, procedure = int4_uint4_mod,
commutator=operator(pg_catalog.%)
);
COMMENT ON OPERATOR pg_catalog.%(int4, uint4) IS 'int4_uint4_mod';

CREATE OPERATOR pg_catalog.=(
leftarg = int4, rightarg = uint4, procedure = int4_uint4_eq,
commutator=operator(pg_catalog.=),
restrict = eqsel, join = eqjoinsel,
HASHES, MERGES
);
COMMENT ON OPERATOR pg_catalog.=(int4, uint4) IS 'int4_uint4_eq';

CREATE OPERATOR pg_catalog.<>(
leftarg = int4, rightarg = uint4, procedure = int4_uint4_ne
);
COMMENT ON OPERATOR pg_catalog.<>(int4, uint4) IS 'int4_uint4_ne';

CREATE OPERATOR pg_catalog.<(
leftarg = int4, rightarg = uint4, procedure = int4_uint4_lt
);
COMMENT ON OPERATOR pg_catalog.<(int4, uint4) IS 'int4_uint4_lt';

CREATE OPERATOR pg_catalog.<=(
leftarg = int4, rightarg = uint4, procedure = int4_uint4_le
);
COMMENT ON OPERATOR pg_catalog.<=(int4, uint4) IS 'int4_uint4_le';

CREATE OPERATOR pg_catalog.>(
leftarg = int4, rightarg = uint4, procedure = int4_uint4_gt
);
COMMENT ON OPERATOR pg_catalog.>(int4, uint4) IS 'int4_uint4_gt';

CREATE OPERATOR pg_catalog.>=(
leftarg = int4, rightarg = uint4, procedure = int4_uint4_ge
);
COMMENT ON OPERATOR pg_catalog.>=(int4, uint4) IS 'int4_uint4_ge';

-------------------int4 uint4---------------
----------------------------------------------------------------
-- add  agg 
----------------------------------------------------------------
drop aggregate if exists pg_catalog.avg(uint4);
create aggregate pg_catalog.avg(uint4) (SFUNC=uint4_avg_accum, cFUNC=int8_avg_collect, STYPE= bigint[], finalfunc = int8_avg, initcond = '{0,0}', initcollect = '{0,0}');
drop aggregate if exists pg_catalog.sum(uint4);
create aggregate pg_catalog.sum(uint4) (SFUNC=uint4_sum, cFUNC=int8_sum_to_int8, STYPE= int8 );
drop aggregate if exists pg_catalog.min(uint4);
create aggregate pg_catalog.min(uint4) (SFUNC=uint4smaller,cFUNC = uint4smaller,  STYPE= uint4,SORTOP = '<');
drop aggregate if exists pg_catalog.max(uint4);
create aggregate pg_catalog.max(uint4) (SFUNC=uint4larger, cFUNC=uint4larger, STYPE= uint4,SORTOP = '>');
drop aggregate if exists pg_catalog.var_samp(uint4);
create aggregate pg_catalog.var_samp(uint4) (SFUNC=uint4_accum, cFUNC=numeric_collect, STYPE= numeric[], finalfunc = numeric_var_samp, initcond = '{0,0,0}', initcollect = '{0,0,0}');
drop aggregate if exists pg_catalog.var_pop(uint4);
create aggregate pg_catalog.var_pop(uint4) (SFUNC=uint4_accum, cFUNC=numeric_collect, STYPE= numeric[], finalfunc = numeric_var_pop, initcond = '{0,0,0}', initcollect = '{0,0,0}');
drop aggregate if exists pg_catalog.variance(uint4);
create aggregate pg_catalog.variance(uint4) (SFUNC=uint4_accum, cFUNC=numeric_collect, STYPE= numeric[], finalfunc = numeric_var_samp, initcond = '{0,0,0}', initcollect = '{0,0,0}');
drop aggregate if exists pg_catalog.stddev_pop(uint4);
create aggregate pg_catalog.stddev_pop(uint4) (SFUNC=uint4_accum, cFUNC=numeric_collect, STYPE= numeric[], finalfunc = numeric_stddev_pop, initcond = '{0,0,0}', initcollect = '{0,0,0}');
drop aggregate if exists pg_catalog.stddev_samp(uint4);
create aggregate pg_catalog.stddev_samp(uint4) (SFUNC=uint4_accum, cFUNC=numeric_collect, STYPE= numeric[], finalfunc = numeric_stddev_samp, initcond = '{0,0,0}', initcollect = '{0,0,0}');
drop aggregate if exists pg_catalog.stddev(uint4);
create aggregate pg_catalog.stddev(uint4) (SFUNC=uint4_accum, cFUNC=numeric_collect, STYPE= numeric[], finalfunc = numeric_stddev_samp, initcond = '{0,0,0}', initcollect = '{0,0,0}');
drop aggregate if exists pg_catalog.std(uint4);
create aggregate pg_catalog.std(uint4) (SFUNC=uint4_accum, cFUNC=numeric_collect, STYPE= numeric[], finalfunc = numeric_stddev_pop, initcond = '{0,0,0}', initcollect = '{0,0,0}');
drop aggregate if exists pg_catalog.bit_and(uint4);
create aggregate pg_catalog.bit_and(uint4) (SFUNC=uint4and, cFUNC = uint4and, STYPE= uint4);
drop aggregate if exists pg_catalog.bit_or(uint4);
create aggregate pg_catalog.bit_or(uint4) (SFUNC=uint4or, cFUNC = uint4or,  STYPE= uint4);

drop aggregate if exists pg_catalog.bit_xor(uint4);
create aggregate pg_catalog.bit_xor(uint4) (SFUNC=uint4xor, cFUNC = uint4xor,  STYPE= uint4);

drop aggregate if exists pg_catalog.listagg(uint4,text);
create aggregate pg_catalog.listagg(uint4,text) (SFUNC=uint4_list_agg_transfn, finalfunc = list_agg_finalfn,  STYPE= internal);
drop aggregate if exists pg_catalog.listagg(uint4);
create aggregate pg_catalog.listagg(uint4) (SFUNC=uint4_list_agg_noarg2_transfn, finalfunc = list_agg_finalfn,  STYPE= internal);

------------------- uint8---------------
------------------- uint8
------------------- uint8---------------

------------------- uint8---------------
------------------- uint8
------------------- uint8---------------

----------------------------------------------------------------
-- add function
----------------------------------------------------------------

DROP FUNCTION IF EXISTS pg_catalog.i1toui8(int1) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.i2toui8(int2) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.i4toui8(int4) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.i8toui8(int8) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.f4toui8(float4) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.f8toui8(float8) CASCADE;

DROP FUNCTION IF EXISTS pg_catalog.numeric_uint8(numeric) CASCADE;

DROP FUNCTION IF EXISTS pg_catalog.ui8toi1(uint8) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.ui8toi2(uint8) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.ui8toi4(uint8) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.ui8toi8(uint8) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.ui8tof4(uint8) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.ui8tof8(uint8) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.uint8_numeric(uint8) CASCADE;

DROP FUNCTION IF EXISTS pg_catalog.uint8smaller(uint8,uint8) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.uint8larger(uint8,uint8) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.uint8eq(uint8,uint8) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.uint8ne(uint8,uint8) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.uint8lt(uint8,uint8) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.uint8le(uint8,uint8) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.uint8gt(uint8,uint8) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.uint8ge(uint8,uint8) CASCADE;

DROP FUNCTION IF EXISTS pg_catalog.uint8cmp(uint8,uint8) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.uint8_int8cmp(uint8,int8) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.hashuint8(uint8) CASCADE;

CREATE FUNCTION pg_catalog.i1toui8 (
int1
) RETURNS uint8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'i1toui8';

CREATE FUNCTION pg_catalog.i2toui8 (
int2
) RETURNS uint8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'i2toui8';

CREATE FUNCTION pg_catalog.i4toui8 (
int4
) RETURNS uint8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'i4toui8';

CREATE FUNCTION pg_catalog.i8toui8 (
int8
) RETURNS uint8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'i8toui8';

CREATE FUNCTION pg_catalog.f4toui8 (
float4
) RETURNS  uint8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'f4toui8';
CREATE FUNCTION pg_catalog.f8toui8 (
float8
) RETURNS uint8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'f8toui8';

CREATE FUNCTION pg_catalog.numeric_uint8 (
Numeric
) RETURNS  uint8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'numeric_uint8';

CREATE FUNCTION pg_catalog.ui8toi1 (
uint8
) RETURNS int1 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'ui8toi1';
CREATE FUNCTION pg_catalog.ui8toi2 (
uint8
) RETURNS int2 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'ui8toi2';

CREATE FUNCTION pg_catalog.ui8toi4 (
uint8
) RETURNS  int4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'ui8toi4';
CREATE FUNCTION pg_catalog.ui8toi8 (
uint8
) RETURNS int8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'ui8toi8';

CREATE FUNCTION pg_catalog.ui8tof4 (
uint8
) RETURNS  float4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'ui8tof4';
CREATE FUNCTION pg_catalog.ui8tof8 (
uint8
) RETURNS float8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'ui8tof8';

CREATE FUNCTION pg_catalog.uint8_numeric (
uint8
) RETURNS Numeric LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint8_numeric';

CREATE FUNCTION pg_catalog.uint8smaller (
uint8,uint8
) RETURNS uint8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint8smaller';

CREATE FUNCTION pg_catalog.uint8larger (
uint8,uint8
) RETURNS uint8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint8larger';

CREATE FUNCTION pg_catalog.uint8eq (
uint8,uint8
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint8eq';

CREATE FUNCTION pg_catalog.uint8ne (
uint8,uint8
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint8ne';

CREATE FUNCTION pg_catalog.uint8lt (
uint8,uint8
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint8lt';

CREATE FUNCTION pg_catalog.uint8le (
uint8,uint8
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint8le';

CREATE FUNCTION pg_catalog.uint8gt (
uint8,uint8
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint8gt';

CREATE FUNCTION pg_catalog.uint8ge (
uint8,uint8
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint8ge';

CREATE FUNCTION pg_catalog.uint8cmp (
uint8,uint8
) RETURNS int4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint8cmp';

CREATE FUNCTION pg_catalog.uint8_int8cmp (
uint8,int8
) RETURNS int4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint8_int8cmp';

CREATE FUNCTION pg_catalog.hashuint8 (
uint8
) RETURNS int4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'hashuint8';

CREATE OR REPLACE FUNCTION TO_TEXT(uint8)
RETURNS TEXT
AS $$ select CAST(uint8out($1) AS VARCHAR2)  $$
LANGUAGE SQL  STRICT IMMUTABLE;

DROP FUNCTION IF EXISTS pg_catalog.uint8_bool(uint8) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.uint8_bool(uint8)
RETURNS bool LANGUAGE C STRICT AS  '$libdir/dolphin',  'uint8_bool';

DROP FUNCTION IF EXISTS pg_catalog.bool_uint8(bool) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.bool_uint8(bool)
RETURNS uint8 LANGUAGE C STRICT AS  '$libdir/dolphin',  'bool_uint8';

DROP FUNCTION IF EXISTS pg_catalog.uint8pl(uint8,uint8) CASCADE;
CREATE FUNCTION pg_catalog.uint8pl (
uint8,uint8
) RETURNS uint8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint8pl';

DROP FUNCTION IF EXISTS pg_catalog.uint8mi(uint8,uint8) CASCADE;
CREATE FUNCTION pg_catalog.uint8mi (
uint8,uint8
) RETURNS uint8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint8mi';

DROP FUNCTION IF EXISTS pg_catalog.uint8mul(uint8,uint8) CASCADE;
CREATE FUNCTION pg_catalog.uint8mul (
uint8,uint8
) RETURNS uint8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint8mul';

DROP FUNCTION IF EXISTS pg_catalog.uint8div(uint8,uint8) CASCADE;
CREATE FUNCTION pg_catalog.uint8div (
uint8,uint8
) RETURNS float8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint8div';

DROP FUNCTION IF EXISTS pg_catalog.uint8abs(uint8) CASCADE;
CREATE FUNCTION pg_catalog.uint8abs (
uint8
) RETURNS uint8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint8abs';

DROP FUNCTION IF EXISTS pg_catalog.uint8mod(uint8,uint8) CASCADE;
CREATE FUNCTION pg_catalog.uint8mod (
uint8,uint8
) RETURNS uint8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint8mod';

DROP FUNCTION IF EXISTS pg_catalog.uint8and(uint8,uint8) CASCADE;
CREATE FUNCTION pg_catalog.uint8and (
uint8,uint8
) RETURNS uint8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint8and';

DROP FUNCTION IF EXISTS pg_catalog.uint8or(uint8,uint8) CASCADE;
CREATE FUNCTION pg_catalog.uint8or (
uint8,uint8
) RETURNS uint8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint8or';

DROP FUNCTION IF EXISTS pg_catalog.uint8xor(uint8,uint8) CASCADE;
CREATE FUNCTION pg_catalog.uint8xor (
uint8,uint8
) RETURNS uint8 LANGUAGE C STRICT IMMUTABLE  as '$libdir/dolphin',  'uint8xor';

DROP FUNCTION IF EXISTS pg_catalog.uint8up(uint8) CASCADE;
CREATE FUNCTION pg_catalog.uint8up (
uint8
) RETURNS uint8 LANGUAGE C STRICT IMMUTABLE  as '$libdir/dolphin',  'uint8up';

DROP FUNCTION IF EXISTS pg_catalog.uint8um(uint8) CASCADE;
CREATE FUNCTION pg_catalog.uint8um (
uint8
) RETURNS int8 LANGUAGE C STRICT IMMUTABLE  as '$libdir/dolphin',  'uint8um';

DROP FUNCTION IF EXISTS pg_catalog.uint8not(uint8) CASCADE;
CREATE FUNCTION pg_catalog.uint8not (
uint8
) RETURNS uint8 LANGUAGE C STRICT IMMUTABLE  as '$libdir/dolphin',  'uint8not';

DROP FUNCTION IF EXISTS pg_catalog.uint8_avg_accum(numeric[], uint8) CASCADE;

CREATE OR REPLACE FUNCTION pg_catalog.uint8_avg_accum(numeric[], uint8)
 RETURNS numeric[] LANGUAGE C STRICT IMMUTABLE AS  '$libdir/dolphin',  'uint8_avg_accum';

DROP FUNCTION IF EXISTS pg_catalog.uint8_accum(numeric[], uint8) CASCADE;

CREATE OR REPLACE FUNCTION pg_catalog.uint8_accum(numeric[], uint8)
 RETURNS numeric[] LANGUAGE C STRICT AS  '$libdir/dolphin',  'uint8_accum';

DROP FUNCTION IF EXISTS pg_catalog.uint8_sum(numeric, uint8) CASCADE;

CREATE OR REPLACE FUNCTION pg_catalog.uint8_sum(numeric, uint8)
RETURNS numeric LANGUAGE C AS  '$libdir/dolphin',  'uint8_sum';

DROP FUNCTION IF EXISTS pg_catalog.uint8_list_agg_transfn(internal, uint8, text) CASCADE;

CREATE OR REPLACE FUNCTION pg_catalog.uint8_list_agg_transfn(internal, uint8, text)
 RETURNS internal LANGUAGE C AS	'$libdir/dolphin',	'uint8_list_agg_transfn';

DROP FUNCTION IF EXISTS pg_catalog.uint8_list_agg_noarg2_transfn(internal, uint8) CASCADE;

CREATE OR REPLACE FUNCTION pg_catalog.uint8_list_agg_noarg2_transfn(internal, uint8)
 RETURNS internal LANGUAGE C  AS '$libdir/dolphin',	'uint8_list_agg_noarg2_transfn';

----------------------------------------------------------------
-- add cast
----------------------------------------------------------------

drop  CAST IF EXISTS(int4 AS  uint8) CASCADE;
drop  CAST IF EXISTS(int8 AS  uint8) CASCADE;
drop  CAST IF EXISTS(int2 AS  uint8) CASCADE;
drop  CAST IF EXISTS(int1 AS  uint8) CASCADE;
drop  CAST IF EXISTS(TEXT AS  uint8) CASCADE;
drop  CAST IF EXISTS(numeric AS  uint8) CASCADE;

          
drop  CAST IF EXISTS(uint8 AS  int4) CASCADE;
drop  CAST IF EXISTS(uint8 AS  int8) CASCADE;
drop  CAST IF EXISTS(uint8 AS  int2) CASCADE;
drop  CAST IF EXISTS(uint8 AS  int1) CASCADE;
drop  CAST IF EXISTS(uint8 AS  TEXT) CASCADE;
drop  CAST IF EXISTS(uint8 AS  numeric) CASCADE;

drop  CAST  IF EXISTS (uint8 AS  bool) CASCADE;
drop  CAST  IF EXISTS (bool AS  uint8) CASCADE;

CREATE CAST (int4 AS uint8) WITH FUNCTION i4toui8(int4) AS IMPLICIT;
CREATE CAST (int8 AS uint8) WITH FUNCTION i8toui8(int8) AS IMPLICIT;
CREATE CAST (int2 AS uint8) WITH FUNCTION i2toui8(int2) AS IMPLICIT;
CREATE CAST (int1 AS uint8) WITH FUNCTION i1toui8(int1) AS IMPLICIT;
CREATE CAST (float4 AS uint8) WITH FUNCTION f4toui8(float4) AS IMPLICIT;
CREATE CAST (float8 AS uint8) WITH FUNCTION f8toui8(float8) AS IMPLICIT;

CREATE CAST (uint8 AS int1) WITH FUNCTION ui8toi1(uint8) AS IMPLICIT;
CREATE CAST (uint8 AS int2) WITH FUNCTION ui8toi2(uint8) AS IMPLICIT;
CREATE CAST (uint8 AS int4) WITH FUNCTION ui8toi4(uint8) AS IMPLICIT;
CREATE CAST (uint8 AS int8) WITH FUNCTION ui8toi8(uint8) AS IMPLICIT;

CREATE CAST (uint8 AS float4) WITH FUNCTION ui8tof4(uint8) AS IMPLICIT;
CREATE CAST (uint8 AS float8) WITH FUNCTION ui8tof8(uint8) AS IMPLICIT;

CREATE CAST (uint8 AS TEXT) WITH FUNCTION TO_TEXT(uint8) AS IMPLICIT;

CREATE CAST (uint8 AS numeric) WITH FUNCTION uint8_numeric(uint8) AS IMPLICIT;
CREATE CAST (numeric AS uint8) WITH FUNCTION numeric_uint8(Numeric) AS IMPLICIT;

CREATE CAST (uint8 AS bool) WITH FUNCTION uint8_bool(uint8) AS IMPLICIT;
CREATE CAST (bool AS uint8) WITH FUNCTION bool_uint8(bool) AS IMPLICIT;

----------------------------------------------------------------
-- add  OPERATOR 
----------------------------------------------------------------

CREATE OPERATOR pg_catalog.=(
leftarg = uint8, rightarg = uint8, procedure = uint8eq,
commutator=operator(pg_catalog.=),
restrict = eqsel, join = eqjoinsel,
HASHES, MERGES
);
COMMENT ON OPERATOR pg_catalog.=(uint8, uint8) IS 'uint8eq';

CREATE OPERATOR pg_catalog.<>(
leftarg = uint8, rightarg = uint8, procedure = uint8ne,
restrict = neqsel, join = neqjoinsel
);
COMMENT ON OPERATOR pg_catalog.<>(uint8, uint8) IS 'uint8ne';

CREATE OPERATOR pg_catalog.<(
leftarg = uint8, rightarg = uint8, procedure = uint8lt,
restrict = scalarltsel, join = scalarltjoinsel
);
COMMENT ON OPERATOR pg_catalog.<(uint8, uint8) IS 'uint8lt';

CREATE OPERATOR pg_catalog.<=(
leftarg = uint8, rightarg = uint8, procedure = uint8le,
restrict = scalarltsel, join = scalarltjoinsel
);
COMMENT ON OPERATOR pg_catalog.<=(uint8, uint8) IS 'uint8le';

CREATE OPERATOR pg_catalog.>(
leftarg = uint8, rightarg = uint8, procedure = uint8gt,
restrict = scalargtsel, join = scalargtjoinsel
);
COMMENT ON OPERATOR pg_catalog.>(uint8, uint8) IS 'uint8gt';

CREATE OPERATOR pg_catalog.>=(
leftarg = uint8, rightarg = uint8, procedure = uint8ge,
restrict = scalargtsel, join = scalargtjoinsel
);
COMMENT ON OPERATOR pg_catalog.>=(uint8, uint8) IS 'uint8ge';

CREATE OPERATOR pg_catalog.+(
leftarg = uint8, rightarg = uint8, procedure = uint8pl,
commutator=operator(pg_catalog.+)
);
COMMENT ON OPERATOR pg_catalog.+(uint8, uint8) IS 'uint8pl';

CREATE OPERATOR pg_catalog.-(
leftarg = uint8, rightarg = uint8, procedure = uint8mi,
commutator=operator(pg_catalog.-)
);
COMMENT ON OPERATOR pg_catalog.-(uint8, uint8) IS 'uint8mi';

CREATE OPERATOR pg_catalog.*(
leftarg = uint8, rightarg = uint8, procedure = uint8mul,
commutator=operator(pg_catalog.*)
);
COMMENT ON OPERATOR pg_catalog.*(uint8, uint8) IS 'uint8mul';

CREATE OPERATOR pg_catalog./(
leftarg = uint8, rightarg = uint8, procedure = uint8div,
commutator=operator(pg_catalog./)
);
COMMENT ON OPERATOR pg_catalog./(uint8, uint8) IS 'uint8div';

CREATE OPERATOR pg_catalog.@(
rightarg = uint8,  procedure = uint8abs

);

CREATE OPERATOR pg_catalog.%(
leftarg = uint8, rightarg = uint8, procedure = uint8mod,
commutator=operator(pg_catalog.%)
);
COMMENT ON OPERATOR pg_catalog.%(uint8, uint8) IS 'uint8mod';

CREATE OPERATOR pg_catalog.+(
rightarg = uint8, procedure = uint8up
);

CREATE OPERATOR pg_catalog.-(
rightarg = uint8, procedure = uint8um
);

CREATE OR REPLACE FUNCTION uint8(text)
RETURNS uint8
LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'text_uint8';
CREATE CAST (TEXT AS uint8) WITH FUNCTION uint8(TEXT) AS IMPLICIT;

-------------------uint8 int8---------------

DROP FUNCTION IF EXISTS pg_catalog.uint8_pl_int8(uint8,int8) CASCADE;
CREATE FUNCTION pg_catalog.uint8_pl_int8 (
uint8,int8
) RETURNS uint8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint8_pl_int8';

DROP FUNCTION IF EXISTS pg_catalog.uint8_mi_int8(uint8,int8) CASCADE;
CREATE FUNCTION pg_catalog.uint8_mi_int8 (
uint8,int8
) RETURNS uint8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint8_mi_int8';

DROP FUNCTION IF EXISTS pg_catalog.uint8_mul_int8(uint8,int8) CASCADE;
CREATE FUNCTION pg_catalog.uint8_mul_int8 (
uint8,int8
) RETURNS uint8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint8_mul_int8';

DROP FUNCTION IF EXISTS pg_catalog.uint8_div_int8(uint8,int8) CASCADE;
CREATE FUNCTION pg_catalog.uint8_div_int8 (
uint8,int8
) RETURNS float8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint8_div_int8';

DROP FUNCTION IF EXISTS pg_catalog.uint8_int8_mod(uint8,int8) CASCADE;
CREATE FUNCTION pg_catalog.uint8_int8_mod (
uint8,int8
) RETURNS uint8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint8_int8_mod';

DROP FUNCTION IF EXISTS pg_catalog.uint8_int8_eq(uint8,int8) CASCADE;
CREATE FUNCTION pg_catalog.uint8_int8_eq (
uint8,int8
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint8_int8_eq';

DROP FUNCTION IF EXISTS pg_catalog.uint8_int8_ne(uint8,int8) CASCADE;
CREATE FUNCTION pg_catalog.uint8_int8_ne (
uint8,int8
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint8_int8_ne';

DROP FUNCTION IF EXISTS pg_catalog.uint8_int8_lt(uint8,int8) CASCADE;
CREATE FUNCTION pg_catalog.uint8_int8_lt (
uint8,int8
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint8_int8_lt';

DROP FUNCTION IF EXISTS pg_catalog.uint8_int8_le(uint8,int8) CASCADE;
CREATE FUNCTION pg_catalog.uint8_int8_le (
uint8,int8
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint8_int8_le';

DROP FUNCTION IF EXISTS pg_catalog.uint8_int8_gt(uint8,int8) CASCADE;
CREATE FUNCTION pg_catalog.uint8_int8_gt (
uint8,int8
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint8_int8_gt';

DROP FUNCTION IF EXISTS pg_catalog.uint8_int8_ge(uint8,int8) CASCADE;
CREATE FUNCTION pg_catalog.uint8_int8_ge (
uint8,int8
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint8_int8_ge';

CREATE OPERATOR pg_catalog.+(
leftarg = uint8, rightarg = int8, procedure = uint8_pl_int8,
commutator=operator(pg_catalog.+)
);
COMMENT ON OPERATOR pg_catalog.+(uint8, int8) IS 'uint8_pl_int8';

CREATE OPERATOR pg_catalog.-(
leftarg = uint8, rightarg = int8, procedure = uint8_mi_int8,
commutator=operator(pg_catalog.-)
);
COMMENT ON OPERATOR pg_catalog.-(uint8, int8) IS 'uint8_mi_int8';

CREATE OPERATOR pg_catalog.*(
leftarg = uint8, rightarg = int8, procedure = uint8_mul_int8,
commutator=operator(pg_catalog.*)
);
COMMENT ON OPERATOR pg_catalog.*(uint8, int8) IS 'uint8_mul_int8';

CREATE OPERATOR pg_catalog./(
leftarg = uint8, rightarg = int8, procedure = uint8_div_int8,
commutator=operator(pg_catalog./)
);
COMMENT ON OPERATOR pg_catalog./(uint8, int8) IS 'uint8_div_int8';

CREATE OPERATOR pg_catalog.%(
leftarg = uint8, rightarg = int8, procedure = uint8_int8_mod,
commutator=operator(pg_catalog.%)
);
COMMENT ON OPERATOR pg_catalog.%(uint8, int8) IS 'uint8_int8_mod';

CREATE OPERATOR pg_catalog.=(
leftarg = uint8, rightarg = int8, procedure = uint8_int8_eq,
restrict = eqsel, join = eqjoinsel,
HASHES, MERGES
);
COMMENT ON OPERATOR pg_catalog.=(uint8, int8) IS 'uint8_int8_eq';

CREATE OPERATOR pg_catalog.<>(
leftarg = uint8, rightarg = int8, procedure = uint8_int8_ne
);
COMMENT ON OPERATOR pg_catalog.<>(uint8, int8) IS 'uint8_int8_ne';

CREATE OPERATOR pg_catalog.<(
leftarg = uint8, rightarg = int8, procedure = uint8_int8_lt
);
COMMENT ON OPERATOR pg_catalog.<(uint8, int8) IS 'uint8_int8_lt';

CREATE OPERATOR pg_catalog.<=(
leftarg = uint8, rightarg = int8, procedure = uint8_int8_le
);
COMMENT ON OPERATOR pg_catalog.<=(uint8, int8) IS 'uint8_int8_le';

CREATE OPERATOR pg_catalog.>(
leftarg = uint8, rightarg = int8, procedure = uint8_int8_gt
);
COMMENT ON OPERATOR pg_catalog.>(uint8, int8) IS 'uint8_int8_gt';

CREATE OPERATOR pg_catalog.>=(
leftarg = uint8, rightarg = int8, procedure = uint8_int8_ge
);
COMMENT ON OPERATOR pg_catalog.>=(uint8, int8) IS 'uint8_int8_ge';

-------------------uint8 int8---------------

-------------------int8 uint8---------------
DROP FUNCTION IF EXISTS pg_catalog.int8_pl_uint8(int8,uint8) CASCADE;
CREATE FUNCTION pg_catalog.int8_pl_uint8 (
int8,uint8
) RETURNS uint8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int8_pl_uint8';

DROP FUNCTION IF EXISTS pg_catalog.int8_mi_uint8(int8,uint8) CASCADE;
CREATE FUNCTION pg_catalog.int8_mi_uint8 (
int8,uint8
) RETURNS uint8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int8_mi_uint8';
DROP FUNCTION IF EXISTS pg_catalog.int8_mul_uint8(int8,uint8) CASCADE;
CREATE FUNCTION pg_catalog.int8_mul_uint8 (
int8,uint8
) RETURNS uint8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int8_mul_uint8';

DROP FUNCTION IF EXISTS pg_catalog.int8_div_uint8(int8,uint8) CASCADE;
CREATE FUNCTION pg_catalog.int8_div_uint8 (
int8,uint8
) RETURNS float8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int8_div_uint8';

DROP FUNCTION IF EXISTS pg_catalog.int8_uint8_mod(int8,uint8) CASCADE;
CREATE FUNCTION pg_catalog.int8_uint8_mod (
int8,uint8
) RETURNS int8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int8_uint8_mod';

DROP FUNCTION IF EXISTS pg_catalog.int8_uint8_eq(int8,uint8) CASCADE;
CREATE FUNCTION pg_catalog.int8_uint8_eq (
int8,uint8
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int8_uint8_eq';

DROP FUNCTION IF EXISTS pg_catalog.int8_uint8_ne(int8,uint8) CASCADE;
CREATE FUNCTION pg_catalog.int8_uint8_ne (
int8,uint8
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int8_uint8_ne';

DROP FUNCTION IF EXISTS pg_catalog.int8_uint8_lt(int8,uint8) CASCADE;
CREATE FUNCTION pg_catalog.int8_uint8_lt (
int8,uint8
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int8_uint8_lt';

DROP FUNCTION IF EXISTS pg_catalog.int8_uint8_le(int8,uint8) CASCADE;
CREATE FUNCTION pg_catalog.int8_uint8_le (
int8,uint8
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int8_uint8_le';

DROP FUNCTION IF EXISTS pg_catalog.int8_uint8_gt(int8,uint8) CASCADE;
CREATE FUNCTION pg_catalog.int8_uint8_gt (
int8,uint8
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int8_uint8_gt';

DROP FUNCTION IF EXISTS pg_catalog.int8_uint8_ge(int8,uint8) CASCADE;
CREATE FUNCTION pg_catalog.int8_uint8_ge (
int8,uint8
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int8_uint8_ge';

CREATE OPERATOR pg_catalog.+(
leftarg = int8, rightarg = uint8, procedure = int8_pl_uint8,
commutator=operator(pg_catalog.+)
);
COMMENT ON OPERATOR pg_catalog.+(int8, uint8) IS 'int8_pl_uint8';

CREATE OPERATOR pg_catalog.-(
leftarg = int8, rightarg = uint8, procedure = int8_mi_uint8,
commutator=operator(pg_catalog.-)
);
COMMENT ON OPERATOR pg_catalog.-(int8, uint8) IS 'int8_mi_uint8';

CREATE OPERATOR pg_catalog.*(
leftarg = int8, rightarg = uint8, procedure = int8_mul_uint8,
commutator=operator(pg_catalog.*)
);
COMMENT ON OPERATOR pg_catalog.*(int8, uint8) IS 'int8_mul_uint8';

CREATE OPERATOR pg_catalog./(
leftarg = int8, rightarg = uint8, procedure = int8_div_uint8,
commutator=operator(pg_catalog./)
);
COMMENT ON OPERATOR pg_catalog./(int8, uint8) IS 'int8_div_uint8';

CREATE OPERATOR pg_catalog.%(
leftarg = int8, rightarg = uint8, procedure = int8_uint8_mod,
commutator=operator(pg_catalog.%)
);
COMMENT ON OPERATOR pg_catalog.%(int8, uint8) IS 'int8_uint8_mod';

CREATE OPERATOR pg_catalog.=(
leftarg = int8, rightarg = uint8, procedure = int8_uint8_eq,
commutator=operator(pg_catalog.=),
restrict = eqsel, join = eqjoinsel,
HASHES, MERGES
);
COMMENT ON OPERATOR pg_catalog.=(int8, uint8) IS 'int8_uint8_eq';

CREATE OPERATOR pg_catalog.<>(
leftarg = int8, rightarg = uint8, procedure = int8_uint8_ne
);
COMMENT ON OPERATOR pg_catalog.<>(int8, uint8) IS 'int8_uint8_ne';

CREATE OPERATOR pg_catalog.<(
leftarg = int8, rightarg = uint8, procedure = int8_uint8_lt
);
COMMENT ON OPERATOR pg_catalog.<(int8, uint8) IS 'int8_uint8_lt';

CREATE OPERATOR pg_catalog.<=(
leftarg = int8, rightarg = uint8, procedure = int8_uint8_le
);
COMMENT ON OPERATOR pg_catalog.<=(int8, uint8) IS 'int8_uint8_le';

CREATE OPERATOR pg_catalog.>(
leftarg = int8, rightarg = uint8, procedure = int8_uint8_gt
);
COMMENT ON OPERATOR pg_catalog.>(int8, uint8) IS 'int8_uint8_gt';

CREATE OPERATOR pg_catalog.>=(
leftarg = int8, rightarg = uint8, procedure = int8_uint8_ge
);
COMMENT ON OPERATOR pg_catalog.>=(int8, uint8) IS 'int8_uint8_ge';

-------------------int8 uint8---------------
----------------------------------------------------------------
-- add  agg 
----------------------------------------------------------------

drop aggregate if exists pg_catalog.avg(uint8);
create aggregate pg_catalog.avg(uint8) (SFUNC=uint8_avg_accum, cFUNC=numeric_avg_collect, STYPE= numeric[], finalfunc = numeric_avg, initcond = '{0,0}', initcollect = '{0,0}');

drop aggregate if exists pg_catalog.sum(uint8);
create aggregate pg_catalog.sum(uint8) (SFUNC=uint8_sum, cFUNC=numeric_add, STYPE= numeric );

drop aggregate if exists pg_catalog.min(uint8);
create aggregate pg_catalog.min(uint8) (SFUNC=uint8smaller,cFUNC = uint8smaller,  STYPE= uint8,SORTOP = '<');

drop aggregate if exists pg_catalog.max(uint8);
create aggregate pg_catalog.max(uint8) (SFUNC=uint8larger, cFUNC=uint8larger, STYPE= uint8,SORTOP = '>');

drop aggregate if exists pg_catalog.var_samp(uint8);
create aggregate pg_catalog.var_samp(uint8) (SFUNC=uint8_accum, cFUNC=numeric_collect, STYPE= numeric[], finalfunc = numeric_var_samp, initcond = '{0,0,0}', initcollect = '{0,0,0}');

drop aggregate if exists pg_catalog.var_pop(uint8);
create aggregate pg_catalog.var_pop(uint8) (SFUNC=uint8_accum, cFUNC=numeric_collect, STYPE= numeric[], finalfunc = numeric_var_pop, initcond = '{0,0,0}', initcollect = '{0,0,0}');

drop aggregate if exists pg_catalog.variance(uint8);
create aggregate pg_catalog.variance(uint8) (SFUNC=uint8_accum, cFUNC=numeric_collect, STYPE= numeric[], finalfunc = numeric_var_samp, initcond = '{0,0,0}', initcollect = '{0,0,0}');

drop aggregate if exists pg_catalog.stddev_pop(uint8);
create aggregate pg_catalog.stddev_pop(uint8) (SFUNC=uint8_accum, cFUNC=numeric_collect, STYPE= numeric[], finalfunc = numeric_stddev_pop, initcond = '{0,0,0}', initcollect = '{0,0,0}');

drop aggregate if exists pg_catalog.stddev_samp(uint8);
create aggregate pg_catalog.stddev_samp(uint8) (SFUNC=uint8_accum, cFUNC=numeric_collect, STYPE= numeric[], finalfunc = numeric_stddev_samp, initcond = '{0,0,0}', initcollect = '{0,0,0}');

drop aggregate if exists pg_catalog.stddev(uint8);
create aggregate pg_catalog.stddev(uint8) (SFUNC=uint8_accum, cFUNC=numeric_collect, STYPE= numeric[], finalfunc = numeric_stddev_samp, initcond = '{0,0,0}', initcollect = '{0,0,0}');

drop aggregate if exists pg_catalog.std(uint8);
create aggregate pg_catalog.std(uint8) (SFUNC=uint8_accum, cFUNC=numeric_collect, STYPE= numeric[], finalfunc = numeric_stddev_pop, initcond = '{0,0,0}', initcollect = '{0,0,0}');

drop aggregate if exists pg_catalog.bit_and(uint8);
create aggregate pg_catalog.bit_and(uint8) (SFUNC=uint8and, cFUNC = uint8and, STYPE= uint8);

drop aggregate if exists pg_catalog.bit_or(uint8);
create aggregate pg_catalog.bit_or(uint8) (SFUNC=uint8or, cFUNC = uint8or,  STYPE= uint8);
drop aggregate if exists pg_catalog.bit_xor(uint8);
create aggregate pg_catalog.bit_xor(uint8) (SFUNC=uint8xor, cFUNC = uint8xor,  STYPE= uint8);

drop aggregate if exists pg_catalog.listagg(uint8,text);
create aggregate pg_catalog.listagg(uint8,text) (SFUNC=uint8_list_agg_transfn, finalfunc = list_agg_finalfn,  STYPE= internal);

drop aggregate if exists pg_catalog.listagg(uint8);
create aggregate pg_catalog.listagg(uint8) (SFUNC=uint8_list_agg_noarg2_transfn, finalfunc = list_agg_finalfn,  STYPE= internal);

----------------------------------------------------------------
-- add  opfamily_opclass 
----------------------------------------------------------------
DROP FUNCTION IF EXISTS pg_catalog.uint1_uint2_eq(uint1,uint2) CASCADE;
CREATE FUNCTION pg_catalog.uint1_uint2_eq (
uint1,uint2
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint1_uint2_eq';

DROP FUNCTION IF EXISTS pg_catalog.uint1_uint2_lt(uint1,uint2) CASCADE;
CREATE FUNCTION pg_catalog.uint1_uint2_lt (
uint1,uint2
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint1_uint2_lt';

DROP FUNCTION IF EXISTS pg_catalog.uint1_uint2_le(uint1,uint2) CASCADE;
CREATE FUNCTION pg_catalog.uint1_uint2_le (
uint1,uint2
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint1_uint2_le';

DROP FUNCTION IF EXISTS pg_catalog.uint1_uint2_gt(uint1,uint2) CASCADE;
CREATE FUNCTION pg_catalog.uint1_uint2_gt (
uint1,uint2
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint1_uint2_gt';

DROP FUNCTION IF EXISTS pg_catalog.uint1_uint2_ge(uint1,uint2) CASCADE;
CREATE FUNCTION pg_catalog.uint1_uint2_ge (
uint1,uint2
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint1_uint2_ge';

CREATE OPERATOR pg_catalog.=(
leftarg = uint1, rightarg = uint2, procedure = uint1_uint2_eq,
restrict = eqsel, join = eqjoinsel,
HASHES, MERGES
);
COMMENT ON OPERATOR pg_catalog.=(uint1, uint2) IS 'uint1_uint2_eq';

CREATE OPERATOR pg_catalog.<(
leftarg = uint1, rightarg = uint2, procedure = uint1_uint2_lt
);
COMMENT ON OPERATOR pg_catalog.<(uint1, uint2) IS 'uint1_uint2_lt';

CREATE OPERATOR pg_catalog.<=(
leftarg = uint1, rightarg = uint2, procedure = uint1_uint2_le
);
COMMENT ON OPERATOR pg_catalog.<=(uint1, uint2) IS 'uint1_uint2_le';

CREATE OPERATOR pg_catalog.>(
leftarg = uint1, rightarg = uint2, procedure = uint1_uint2_gt
);
COMMENT ON OPERATOR pg_catalog.>(uint1, uint2) IS 'uint1_uint2_gt';

CREATE OPERATOR pg_catalog.>=(
leftarg = uint1, rightarg = uint2, procedure = uint1_uint2_ge
);
COMMENT ON OPERATOR pg_catalog.>=(uint1, uint2) IS 'uint1_uint2_ge';

DROP FUNCTION IF EXISTS pg_catalog.uint1_uint4_eq(uint1,uint4) CASCADE;
CREATE FUNCTION pg_catalog.uint1_uint4_eq (
uint1,uint4
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint1_uint4_eq';

DROP FUNCTION IF EXISTS pg_catalog.uint1_uint4_lt(uint1,uint4) CASCADE;
CREATE FUNCTION pg_catalog.uint1_uint4_lt (
uint1,uint4
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint1_uint4_lt';

DROP FUNCTION IF EXISTS pg_catalog.uint1_uint4_le(uint1,uint4) CASCADE;
CREATE FUNCTION pg_catalog.uint1_uint4_le (
uint1,uint4
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint1_uint4_le';

DROP FUNCTION IF EXISTS pg_catalog.uint1_uint4_gt(uint1,uint4) CASCADE;
CREATE FUNCTION pg_catalog.uint1_uint4_gt (
uint1,uint4
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint1_uint4_gt';

DROP FUNCTION IF EXISTS pg_catalog.uint1_uint4_ge(uint1,uint4) CASCADE;
CREATE FUNCTION pg_catalog.uint1_uint4_ge (
uint1,uint4
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint1_uint4_ge';

CREATE OPERATOR pg_catalog.=(
leftarg = uint1, rightarg = uint4, procedure = uint1_uint4_eq,
restrict = eqsel, join = eqjoinsel,
HASHES, MERGES
);
COMMENT ON OPERATOR pg_catalog.=(uint1, uint4) IS 'uint1_uint4_eq';

CREATE OPERATOR pg_catalog.<(
leftarg = uint1, rightarg = uint4, procedure = uint1_uint4_lt
);
COMMENT ON OPERATOR pg_catalog.<(uint1, uint4) IS 'uint1_uint4_lt';

CREATE OPERATOR pg_catalog.<=(
leftarg = uint1, rightarg = uint4, procedure = uint1_uint4_le
);
COMMENT ON OPERATOR pg_catalog.<=(uint1, uint4) IS 'uint1_uint4_le';

CREATE OPERATOR pg_catalog.>(
leftarg = uint1, rightarg = uint4, procedure = uint1_uint4_gt
);
COMMENT ON OPERATOR pg_catalog.>(uint1, uint4) IS 'uint1_uint4_gt';

CREATE OPERATOR pg_catalog.>=(
leftarg = uint1, rightarg = uint4, procedure = uint1_uint4_ge
);
COMMENT ON OPERATOR pg_catalog.>=(uint1, uint4) IS 'uint1_uint4_ge';

DROP FUNCTION IF EXISTS pg_catalog.uint1_uint8_eq(uint1,uint8) CASCADE;
CREATE FUNCTION pg_catalog.uint1_uint8_eq (
uint1,uint8
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint1_uint8_eq';

DROP FUNCTION IF EXISTS pg_catalog.uint1_uint8_lt(uint1,uint8) CASCADE;
CREATE FUNCTION pg_catalog.uint1_uint8_lt (
uint1,uint8
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint1_uint8_lt';

DROP FUNCTION IF EXISTS pg_catalog.uint1_uint8_le(uint1,uint8) CASCADE;
CREATE FUNCTION pg_catalog.uint1_uint8_le (
uint1,uint8
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint1_uint8_le';

DROP FUNCTION IF EXISTS pg_catalog.uint1_uint8_gt(uint1,uint8) CASCADE;
CREATE FUNCTION pg_catalog.uint1_uint8_gt (
uint1,uint8
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint1_uint8_gt';

DROP FUNCTION IF EXISTS pg_catalog.uint1_uint8_ge(uint1,uint8) CASCADE;
CREATE FUNCTION pg_catalog.uint1_uint8_ge (
uint1,uint8
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint1_uint8_ge';

CREATE OPERATOR pg_catalog.=(
leftarg = uint1, rightarg = uint8, procedure = uint1_uint8_eq,
restrict = eqsel, join = eqjoinsel,
HASHES, MERGES
);
COMMENT ON OPERATOR pg_catalog.=(uint1, uint8) IS 'uint1_uint8_eq';

CREATE OPERATOR pg_catalog.<(
leftarg = uint1, rightarg = uint8, procedure = uint1_uint8_lt
);
COMMENT ON OPERATOR pg_catalog.<(uint1, uint8) IS 'uint1_uint8_lt';

CREATE OPERATOR pg_catalog.<=(
leftarg = uint1, rightarg = uint8, procedure = uint1_uint8_le
);
COMMENT ON OPERATOR pg_catalog.<=(uint1, uint8) IS 'uint1_uint8_le';

CREATE OPERATOR pg_catalog.>(
leftarg = uint1, rightarg = uint8, procedure = uint1_uint8_gt
);
COMMENT ON OPERATOR pg_catalog.>(uint1, uint8) IS 'uint1_uint8_gt';

CREATE OPERATOR pg_catalog.>=(
leftarg = uint1, rightarg = uint8, procedure = uint1_uint8_ge
);
COMMENT ON OPERATOR pg_catalog.>=(uint1, uint8) IS 'uint1_uint8_ge';

DROP FUNCTION IF EXISTS pg_catalog.uint2_uint4_eq(uint2,uint4) CASCADE;
CREATE FUNCTION pg_catalog.uint2_uint4_eq (
uint2,uint4
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint2_uint4_eq';

DROP FUNCTION IF EXISTS pg_catalog.uint2_uint4_lt(uint2,uint4) CASCADE;
CREATE FUNCTION pg_catalog.uint2_uint4_lt (
uint2,uint4
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint2_uint4_lt';

DROP FUNCTION IF EXISTS pg_catalog.uint2_uint4_le(uint2,uint4) CASCADE;
CREATE FUNCTION pg_catalog.uint2_uint4_le (
uint2,uint4
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint2_uint4_le';

DROP FUNCTION IF EXISTS pg_catalog.uint2_uint4_gt(uint2,uint4) CASCADE;
CREATE FUNCTION pg_catalog.uint2_uint4_gt (
uint2,uint4
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint2_uint4_gt';

DROP FUNCTION IF EXISTS pg_catalog.uint2_uint4_ge(uint2,uint4) CASCADE;
CREATE FUNCTION pg_catalog.uint2_uint4_ge (
uint2,uint4
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint2_uint4_ge';

CREATE OPERATOR pg_catalog.=(
leftarg = uint2, rightarg = uint4, procedure = uint2_uint4_eq,
restrict = eqsel, join = eqjoinsel,
HASHES, MERGES
);
COMMENT ON OPERATOR pg_catalog.=(uint2, uint4) IS 'uint2_uint4_eq';

CREATE OPERATOR pg_catalog.<(
leftarg = uint2, rightarg = uint4, procedure = uint2_uint4_lt
);
COMMENT ON OPERATOR pg_catalog.<(uint2, uint4) IS 'uint2_uint4_lt';

CREATE OPERATOR pg_catalog.<=(
leftarg = uint2, rightarg = uint4, procedure = uint2_uint4_le
);
COMMENT ON OPERATOR pg_catalog.<=(uint2, uint4) IS 'uint2_uint4_le';

CREATE OPERATOR pg_catalog.>(
leftarg = uint2, rightarg = uint4, procedure = uint2_uint4_gt
);
COMMENT ON OPERATOR pg_catalog.>(uint2, uint4) IS 'uint2_uint4_gt';

CREATE OPERATOR pg_catalog.>=(
leftarg = uint2, rightarg = uint4, procedure = uint2_uint4_ge
);
COMMENT ON OPERATOR pg_catalog.>=(uint2, uint4) IS 'uint2_uint4_ge';

DROP FUNCTION IF EXISTS pg_catalog.uint2_uint8_eq(uint2,uint8) CASCADE;
CREATE FUNCTION pg_catalog.uint2_uint8_eq (
uint2,uint8
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint2_uint8_eq';

DROP FUNCTION IF EXISTS pg_catalog.uint2_uint8_lt(uint2,uint8) CASCADE;
CREATE FUNCTION pg_catalog.uint2_uint8_lt (
uint2,uint8
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint2_uint8_lt';

DROP FUNCTION IF EXISTS pg_catalog.uint2_uint8_le(uint2,uint8) CASCADE;
CREATE FUNCTION pg_catalog.uint2_uint8_le (
uint2,uint8
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint2_uint8_le';

DROP FUNCTION IF EXISTS pg_catalog.uint2_uint8_gt(uint2,uint8) CASCADE;
CREATE FUNCTION pg_catalog.uint2_uint8_gt (
uint2,uint8
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint2_uint8_gt';

DROP FUNCTION IF EXISTS pg_catalog.uint2_uint8_ge(uint2,uint8) CASCADE;
CREATE FUNCTION pg_catalog.uint2_uint8_ge (
uint2,uint8
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint2_uint8_ge';

CREATE OPERATOR pg_catalog.=(
leftarg = uint2, rightarg = uint8, procedure = uint2_uint8_eq,
restrict = eqsel, join = eqjoinsel,
HASHES, MERGES
);
COMMENT ON OPERATOR pg_catalog.=(uint2, uint8) IS 'uint2_uint8_eq';

CREATE OPERATOR pg_catalog.<(
leftarg = uint2, rightarg = uint8, procedure = uint2_uint8_lt
);
COMMENT ON OPERATOR pg_catalog.<(uint2, uint8) IS 'uint2_uint8_lt';

CREATE OPERATOR pg_catalog.<=(
leftarg = uint2, rightarg = uint8, procedure = uint2_uint8_le
);
COMMENT ON OPERATOR pg_catalog.<=(uint2, uint8) IS 'uint2_uint8_le';

CREATE OPERATOR pg_catalog.>(
leftarg = uint2, rightarg = uint8, procedure = uint2_uint8_gt
);
COMMENT ON OPERATOR pg_catalog.>(uint2, uint8) IS 'uint2_uint8_gt';

CREATE OPERATOR pg_catalog.>=(
leftarg = uint2, rightarg = uint8, procedure = uint2_uint8_ge
);
COMMENT ON OPERATOR pg_catalog.>=(uint2, uint8) IS 'uint2_uint8_ge';

DROP FUNCTION IF EXISTS pg_catalog.uint4_uint8_eq(uint4,uint8) CASCADE;
CREATE FUNCTION pg_catalog.uint4_uint8_eq (
uint4,uint8
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint4_uint8_eq';

DROP FUNCTION IF EXISTS pg_catalog.uint4_uint8_lt(uint4,uint8) CASCADE;
CREATE FUNCTION pg_catalog.uint4_uint8_lt (
uint4,uint8
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint4_uint8_lt';

DROP FUNCTION IF EXISTS pg_catalog.uint4_uint8_le(uint4,uint8) CASCADE;
CREATE FUNCTION pg_catalog.uint4_uint8_le (
uint4,uint8
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint4_uint8_le';

DROP FUNCTION IF EXISTS pg_catalog.uint4_uint8_gt(uint4,uint8) CASCADE;
CREATE FUNCTION pg_catalog.uint4_uint8_gt (
uint4,uint8
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint4_uint8_gt';

DROP FUNCTION IF EXISTS pg_catalog.uint4_uint8_ge(uint4,uint8) CASCADE;
CREATE FUNCTION pg_catalog.uint4_uint8_ge (
uint4,uint8
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint4_uint8_ge';

CREATE OPERATOR pg_catalog.=(
leftarg = uint4, rightarg = uint8, procedure = uint4_uint8_eq,
restrict = eqsel, join = eqjoinsel,
HASHES, MERGES
);
COMMENT ON OPERATOR pg_catalog.=(uint4, uint8) IS 'uint4_uint8_eq';

CREATE OPERATOR pg_catalog.<(
leftarg = uint4, rightarg = uint8, procedure = uint4_uint8_lt
);
COMMENT ON OPERATOR pg_catalog.<(uint4, uint8) IS 'uint4_uint8_lt';

CREATE OPERATOR pg_catalog.<=(
leftarg = uint4, rightarg = uint8, procedure = uint4_uint8_le
);
COMMENT ON OPERATOR pg_catalog.<=(uint4, uint8) IS 'uint4_uint8_le';

CREATE OPERATOR pg_catalog.>(
leftarg = uint4, rightarg = uint8, procedure = uint4_uint8_gt
);
COMMENT ON OPERATOR pg_catalog.>(uint4, uint8) IS 'uint4_uint8_gt';

CREATE OPERATOR pg_catalog.>=(
leftarg = uint4, rightarg = uint8, procedure = uint4_uint8_ge
);
COMMENT ON OPERATOR pg_catalog.>=(uint4, uint8) IS 'uint4_uint8_ge';

DROP FUNCTION IF EXISTS pg_catalog.uint1_int2_eq(uint1,int2) CASCADE;
CREATE FUNCTION pg_catalog.uint1_int2_eq (
uint1,int2
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint1_int2_eq';

DROP FUNCTION IF EXISTS pg_catalog.uint1_int2_lt(uint1,int2) CASCADE;
CREATE FUNCTION pg_catalog.uint1_int2_lt (
uint1,int2
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint1_int2_lt';

DROP FUNCTION IF EXISTS pg_catalog.uint1_int2_le(uint1,int2) CASCADE;
CREATE FUNCTION pg_catalog.uint1_int2_le (
uint1,int2
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint1_int2_le';

DROP FUNCTION IF EXISTS pg_catalog.uint1_int2_gt(uint1,int2) CASCADE;
CREATE FUNCTION pg_catalog.uint1_int2_gt (
uint1,int2
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint1_int2_gt';

DROP FUNCTION IF EXISTS pg_catalog.uint1_int2_ge(uint1,int2) CASCADE;
CREATE FUNCTION pg_catalog.uint1_int2_ge (
uint1,int2
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint1_int2_ge';

CREATE OPERATOR pg_catalog.=(
leftarg = uint1, rightarg = int2, procedure = uint1_int2_eq,
restrict = eqsel, join = eqjoinsel,
HASHES, MERGES
);
COMMENT ON OPERATOR pg_catalog.=(uint1, uint2) IS 'uint1_int2_eq';

CREATE OPERATOR pg_catalog.<(
leftarg = uint1, rightarg = int2, procedure = uint1_int2_lt
);
COMMENT ON OPERATOR pg_catalog.<(uint1, uint2) IS 'uint1_int2_lt';

CREATE OPERATOR pg_catalog.<=(
leftarg = uint1, rightarg = int2, procedure = uint1_int2_le
);
COMMENT ON OPERATOR pg_catalog.<=(uint1, uint2) IS 'uint1_int2_le';

CREATE OPERATOR pg_catalog.>(
leftarg = uint1, rightarg = int2, procedure = uint1_int2_gt
);
COMMENT ON OPERATOR pg_catalog.>(uint1, uint2) IS 'uint1_int2_gt';

CREATE OPERATOR pg_catalog.>=(
leftarg = uint1, rightarg = int2, procedure = uint1_int2_ge
);
COMMENT ON OPERATOR pg_catalog.>=(uint1, uint2) IS 'uint1_int2_ge';

DROP FUNCTION IF EXISTS pg_catalog.uint1_int4_eq(uint1,int4) CASCADE;
CREATE FUNCTION pg_catalog.uint1_int4_eq (
uint1,int4
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint1_int4_eq';

DROP FUNCTION IF EXISTS pg_catalog.uint1_int4_lt(uint1,int4) CASCADE;
CREATE FUNCTION pg_catalog.uint1_int4_lt (
uint1,int4
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint1_int4_lt';

DROP FUNCTION IF EXISTS pg_catalog.uint1_int4_le(uint1,int4) CASCADE;
CREATE FUNCTION pg_catalog.uint1_int4_le (
uint1,int4
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint1_int4_le';

DROP FUNCTION IF EXISTS pg_catalog.uint1_int4_gt(uint1,int4) CASCADE;
CREATE FUNCTION pg_catalog.uint1_int4_gt (
uint1,int4
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint1_int4_gt';

DROP FUNCTION IF EXISTS pg_catalog.uint1_int4_ge(uint1,int4) CASCADE;
CREATE FUNCTION pg_catalog.uint1_int4_ge (
uint1,int4
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint1_int4_ge';

CREATE OPERATOR pg_catalog.=(
leftarg = uint1, rightarg = int4, procedure = uint1_int4_eq,
restrict = eqsel, join = eqjoinsel,
HASHES, MERGES
);
COMMENT ON OPERATOR pg_catalog.=(uint1, uint4) IS 'uint1_int4_eq';

CREATE OPERATOR pg_catalog.<(
leftarg = uint1, rightarg = int4, procedure = uint1_int4_lt
);
COMMENT ON OPERATOR pg_catalog.<(uint1, uint4) IS 'uint1_int4_lt';

CREATE OPERATOR pg_catalog.<=(
leftarg = uint1, rightarg = int4, procedure = uint1_int4_le
);
COMMENT ON OPERATOR pg_catalog.<=(uint1, uint4) IS 'uint1_int4_le';

CREATE OPERATOR pg_catalog.>(
leftarg = uint1, rightarg = int4, procedure = uint1_int4_gt
);
COMMENT ON OPERATOR pg_catalog.>(uint1, uint4) IS 'uint1_int4_gt';

CREATE OPERATOR pg_catalog.>=(
leftarg = uint1, rightarg = int4, procedure = uint1_int4_ge
);
COMMENT ON OPERATOR pg_catalog.>=(uint1, uint4) IS 'uint1_int4_ge';

DROP FUNCTION IF EXISTS pg_catalog.uint1_int8_eq(uint1,int8) CASCADE;
CREATE FUNCTION pg_catalog.uint1_int8_eq (
uint1,int8
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint1_int8_eq';

DROP FUNCTION IF EXISTS pg_catalog.uint1_int8_lt(uint1,int8) CASCADE;
CREATE FUNCTION pg_catalog.uint1_int8_lt (
uint1,int8
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint1_int8_lt';

DROP FUNCTION IF EXISTS pg_catalog.uint1_int8_le(uint1,int8) CASCADE;
CREATE FUNCTION pg_catalog.uint1_int8_le (
uint1,int8
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint1_int8_le';

DROP FUNCTION IF EXISTS pg_catalog.uint1_int8_gt(uint1,int8) CASCADE;
CREATE FUNCTION pg_catalog.uint1_int8_gt (
uint1,int8
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint1_int8_gt';

DROP FUNCTION IF EXISTS pg_catalog.uint1_int8_ge(uint1,int8) CASCADE;
CREATE FUNCTION pg_catalog.uint1_int8_ge (
uint1,int8
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint1_int8_ge';

CREATE OPERATOR pg_catalog.=(
leftarg = uint1, rightarg = int8, procedure = uint1_int8_eq,
restrict = eqsel, join = eqjoinsel,
HASHES, MERGES
);
COMMENT ON OPERATOR pg_catalog.=(uint1, uint8) IS 'uint1_int8_eq';

CREATE OPERATOR pg_catalog.<(
leftarg = uint1, rightarg = int8, procedure = uint1_int8_lt
);
COMMENT ON OPERATOR pg_catalog.<(uint1, uint8) IS 'uint1_int8_lt';

CREATE OPERATOR pg_catalog.<=(
leftarg = uint1, rightarg = int8, procedure = uint1_int8_le
);
COMMENT ON OPERATOR pg_catalog.<=(uint1, uint8) IS 'uint1_int8_le';

CREATE OPERATOR pg_catalog.>(
leftarg = uint1, rightarg = int8, procedure = uint1_int8_gt
);
COMMENT ON OPERATOR pg_catalog.>(uint1, uint8) IS 'uint1_int8_gt';

CREATE OPERATOR pg_catalog.>=(
leftarg = uint1, rightarg = int8, procedure = uint1_int8_ge
);
COMMENT ON OPERATOR pg_catalog.>=(uint1, uint8) IS 'uint1_int8_ge';

DROP FUNCTION IF EXISTS pg_catalog.uint2_int4_eq(uint2,int4) CASCADE;
CREATE FUNCTION pg_catalog.uint2_int4_eq (
uint2,int4
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint2_int4_eq';

DROP FUNCTION IF EXISTS pg_catalog.uint2_int4_lt(uint2,int4) CASCADE;
CREATE FUNCTION pg_catalog.uint2_int4_lt (
uint2,int4
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint2_int4_lt';

DROP FUNCTION IF EXISTS pg_catalog.uint2_int4_le(uint2,int4) CASCADE;
CREATE FUNCTION pg_catalog.uint2_int4_le (
uint2,int4
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint2_int4_le';

DROP FUNCTION IF EXISTS pg_catalog.uint2_int4_gt(uint2,int4) CASCADE;
CREATE FUNCTION pg_catalog.uint2_int4_gt (
uint2,int4
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint2_int4_gt';

DROP FUNCTION IF EXISTS pg_catalog.uint2_int4_ge(uint2,int4) CASCADE;
CREATE FUNCTION pg_catalog.uint2_int4_ge (
uint2,int4
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint2_int4_ge';

CREATE OPERATOR pg_catalog.=(
leftarg = uint2, rightarg = int4, procedure = uint2_int4_eq,
restrict = eqsel, join = eqjoinsel,
HASHES, MERGES
);
COMMENT ON OPERATOR pg_catalog.=(uint2, uint4) IS 'uint2_int4_eq';

CREATE OPERATOR pg_catalog.<(
leftarg = uint2, rightarg = int4, procedure = uint2_int4_lt
);
COMMENT ON OPERATOR pg_catalog.<(uint2, uint4) IS 'uint2_int4_lt';

CREATE OPERATOR pg_catalog.<=(
leftarg = uint2, rightarg = int4, procedure = uint2_int4_le
);
COMMENT ON OPERATOR pg_catalog.<=(uint2, uint4) IS 'uint2_int4_le';

CREATE OPERATOR pg_catalog.>(
leftarg = uint2, rightarg = int4, procedure = uint2_int4_gt
);
COMMENT ON OPERATOR pg_catalog.>(uint2, uint4) IS 'uint2_int4_gt';

CREATE OPERATOR pg_catalog.>=(
leftarg = uint2, rightarg = int4, procedure = uint2_int4_ge
);
COMMENT ON OPERATOR pg_catalog.>=(uint2, uint4) IS 'uint2_int4_ge';

DROP FUNCTION IF EXISTS pg_catalog.uint2_int8_eq(uint2,int8) CASCADE;
CREATE FUNCTION pg_catalog.uint2_int8_eq (
uint2,int8
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint2_int8_eq';

DROP FUNCTION IF EXISTS pg_catalog.uint2_int8_lt(uint2,int8) CASCADE;
CREATE FUNCTION pg_catalog.uint2_int8_lt (
uint2,int8
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint2_int8_lt';

DROP FUNCTION IF EXISTS pg_catalog.uint2_int8_le(uint2,int8) CASCADE;
CREATE FUNCTION pg_catalog.uint2_int8_le (
uint2,int8
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint2_int8_le';

DROP FUNCTION IF EXISTS pg_catalog.uint2_int8_gt(uint2,int8) CASCADE;
CREATE FUNCTION pg_catalog.uint2_int8_gt (
uint2,int8
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint2_int8_gt';

DROP FUNCTION IF EXISTS pg_catalog.uint2_int8_ge(uint2,int8) CASCADE;
CREATE FUNCTION pg_catalog.uint2_int8_ge (
uint2,int8
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint2_int8_ge';

CREATE OPERATOR pg_catalog.=(
leftarg = uint2, rightarg = int8, procedure = uint2_int8_eq,
restrict = eqsel, join = eqjoinsel,
HASHES, MERGES
);
COMMENT ON OPERATOR pg_catalog.=(uint2, uint8) IS 'uint2_int8_eq';

CREATE OPERATOR pg_catalog.<(
leftarg = uint2, rightarg = int8, procedure = uint2_int8_lt
);
COMMENT ON OPERATOR pg_catalog.<(uint2, uint8) IS 'uint2_int8_lt';

CREATE OPERATOR pg_catalog.<=(
leftarg = uint2, rightarg = int8, procedure = uint2_int8_le
);
COMMENT ON OPERATOR pg_catalog.<=(uint2, uint8) IS 'uint2_int8_le';

CREATE OPERATOR pg_catalog.>(
leftarg = uint2, rightarg = int8, procedure = uint2_int8_gt
);
COMMENT ON OPERATOR pg_catalog.>(uint2, uint8) IS 'uint2_int8_gt';

CREATE OPERATOR pg_catalog.>=(
leftarg = uint2, rightarg = int8, procedure = uint2_int8_ge
);
COMMENT ON OPERATOR pg_catalog.>=(uint2, uint8) IS 'uint2_int8_ge';

DROP FUNCTION IF EXISTS pg_catalog.uint4_int8_eq(uint4,int8) CASCADE;
CREATE FUNCTION pg_catalog.uint4_int8_eq (
uint4,int8
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint4_int8_eq';

DROP FUNCTION IF EXISTS pg_catalog.uint4_int8_lt(uint4,int8) CASCADE;
CREATE FUNCTION pg_catalog.uint4_int8_lt (
uint4,int8
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint4_int8_lt';

DROP FUNCTION IF EXISTS pg_catalog.uint4_int8_le(uint4,int8) CASCADE;
CREATE FUNCTION pg_catalog.uint4_int8_le (
uint4,int8
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint4_int8_le';

DROP FUNCTION IF EXISTS pg_catalog.uint4_int8_gt(uint4,int8) CASCADE;
CREATE FUNCTION pg_catalog.uint4_int8_gt (
uint4,int8
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint4_int8_gt';

DROP FUNCTION IF EXISTS pg_catalog.uint4_int8_ge(uint4,int8) CASCADE;
CREATE FUNCTION pg_catalog.uint4_int8_ge (
uint4,int8
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint4_int8_ge';

CREATE OPERATOR pg_catalog.=(
leftarg = uint4, rightarg = int8, procedure = uint4_int8_eq,
restrict = eqsel, join = eqjoinsel,
HASHES, MERGES
);
COMMENT ON OPERATOR pg_catalog.=(uint4, uint8) IS 'uint4_int8_eq';

CREATE OPERATOR pg_catalog.<(
leftarg = uint4, rightarg = int8, procedure = uint4_int8_lt
);
COMMENT ON OPERATOR pg_catalog.<(uint4, uint8) IS 'uint4_int8_lt';

CREATE OPERATOR pg_catalog.<=(
leftarg = uint4, rightarg = int8, procedure = uint4_int8_le
);
COMMENT ON OPERATOR pg_catalog.<=(uint4, uint8) IS 'uint4_int8_le';

CREATE OPERATOR pg_catalog.>(
leftarg = uint4, rightarg = int8, procedure = uint4_int8_gt
);
COMMENT ON OPERATOR pg_catalog.>(uint4, uint8) IS 'uint4_int8_gt';

CREATE OPERATOR pg_catalog.>=(
leftarg = uint4, rightarg = int8, procedure = uint4_int8_ge
);
COMMENT ON OPERATOR pg_catalog.>=(uint4, uint8) IS 'uint4_int8_ge';

CREATE OR REPLACE FUNCTION pg_catalog.uint1_sortsupport (internal) RETURNS void LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'uint1_sortsupport';

CREATE OR REPLACE FUNCTION pg_catalog.uint2_sortsupport (internal) RETURNS void LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'uint2_sortsupport';

CREATE OR REPLACE FUNCTION pg_catalog.uint4_sortsupport (internal) RETURNS void LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'uint4_sortsupport';

CREATE OR REPLACE FUNCTION pg_catalog.uint8_sortsupport (internal) RETURNS void LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'uint8_sortsupport';

CREATE OPERATOR CLASS uint_ops
    DEFAULT FOR TYPE uint1 USING btree family integer_ops AS
        OPERATOR        1       < ,
        OPERATOR        1       <(uint1, uint2),
        OPERATOR        1       <(uint1, uint4),
        OPERATOR        1       <(uint1, uint8),
        OPERATOR        1       <(uint1, int1),
        OPERATOR        1       <(uint1, int2),
        OPERATOR        1       <(uint1, int4),
        OPERATOR        1       <(uint1, int8),
        OPERATOR        2       <= ,
        OPERATOR        2       <=(uint1, uint2),
        OPERATOR        2       <=(uint1, uint4),
        OPERATOR        2       <=(uint1, uint8),
        OPERATOR        2       <=(uint1, int1),
        OPERATOR        2       <=(uint1, int2),
        OPERATOR        2       <=(uint1, int4),
        OPERATOR        2       <=(uint1, int8),
        OPERATOR        3       = ,
        OPERATOR        3       =(uint1, uint2),
        OPERATOR        3       =(uint1, uint4),
        OPERATOR        3       =(uint1, uint8),
        OPERATOR        3       =(uint1, int1),
        OPERATOR        3       =(uint1, int2),
        OPERATOR        3       =(uint1, int4),
        OPERATOR        3       =(uint1, int8),
        OPERATOR        4       >= ,
        OPERATOR        4       >=(uint1, uint2),
        OPERATOR        4       >=(uint1, uint4),
        OPERATOR        4       >=(uint1, uint8),
        OPERATOR        4       >=(uint1, int1),
        OPERATOR        4       >=(uint1, int2),
        OPERATOR        4       >=(uint1, int4),
        OPERATOR        4       >=(uint1, int8),
        OPERATOR        5       > ,
        OPERATOR        5       >(uint1, uint2),
        OPERATOR        5       >(uint1, uint4),
        OPERATOR        5       >(uint1, uint8),
        OPERATOR        5       >(uint1, int1),
        OPERATOR        5       >(uint1, int2),
        OPERATOR        5       >(uint1, int4),
        OPERATOR        5       >(uint1, int8),
        FUNCTION        1       uint1cmp(uint1, uint1),
        FUNCTION        1       uint12cmp(uint1, uint2),
        FUNCTION        1       uint14cmp(uint1, uint4),
        FUNCTION        1       uint18cmp(uint1, uint8),
        FUNCTION        1       uint1_int1cmp(uint1, int1),
        FUNCTION        1       uint1_int2cmp(uint1, int2),
        FUNCTION        1       uint1_int4cmp(uint1, int4),
        FUNCTION        1       uint1_int8cmp(uint1, int8),
        FUNCTION        2       uint1_sortsupport(internal);

CREATE OPERATOR CLASS uint2_ops
    DEFAULT FOR TYPE uint2 USING btree family integer_ops AS
        OPERATOR        1       < ,
        OPERATOR        1       <(uint2, uint4),
        OPERATOR        1       <(uint2, uint8),
        OPERATOR        1       <(uint2, int2),
        OPERATOR        1       <(uint2, int4),
        OPERATOR        1       <(uint2, int8),
        OPERATOR        2       <= ,
        OPERATOR        2       <=(uint2, uint4),
        OPERATOR        2       <=(uint2, uint8),
        OPERATOR        2       <=(uint2, int2),
        OPERATOR        2       <=(uint2, int4),
        OPERATOR        2       <=(uint2, int8),
        OPERATOR        3       = ,
        OPERATOR        3       =(uint2, uint4),
        OPERATOR        3       =(uint2, uint8),
        OPERATOR        3       =(uint2, int2),
        OPERATOR        3       =(uint2, int4),
        OPERATOR        3       =(uint2, int8),
        OPERATOR        4       >= ,
        OPERATOR        4       >=(uint2, uint4),
        OPERATOR        4       >=(uint2, uint8),
        OPERATOR        4       >=(uint2, int2),
        OPERATOR        4       >=(uint2, int4),
        OPERATOR        4       >=(uint2, int8),
        OPERATOR        5       > ,
        OPERATOR        5       >(uint2, uint4),
        OPERATOR        5       >(uint2, uint8),
        OPERATOR        5       >(uint2, int2),
        OPERATOR        5       >(uint2, int4),
        OPERATOR        5       >(uint2, int8),
        FUNCTION        1       uint2cmp(uint2, uint2),
        FUNCTION        1       uint24cmp(uint2, uint4),
        FUNCTION        1       uint28cmp(uint2, uint8),
        FUNCTION        1       uint2_int2cmp(uint2, int2),
        FUNCTION        1       uint2_int4cmp(uint2, int4),
        FUNCTION        1       uint2_int8cmp(uint2, int8),
        FUNCTION        2       uint2_sortsupport(internal);

CREATE OPERATOR CLASS uint4_ops
    DEFAULT FOR TYPE uint4 USING btree family integer_ops AS
        OPERATOR        1       < ,
        OPERATOR        1       <(uint4, uint8),
        OPERATOR        1       <(uint4, int4),
        OPERATOR        1       <(uint4, int8),
        OPERATOR        2       <= ,
        OPERATOR        2       <=(uint4, uint8),
        OPERATOR        2       <=(uint4, int4),
        OPERATOR        2       <=(uint4, int8),
        OPERATOR        3       = ,
        OPERATOR        3       =(uint4, uint8),
        OPERATOR        3       =(uint4, int4),
        OPERATOR        3       =(uint4, int8),
        OPERATOR        4       >= ,
        OPERATOR        4       >=(uint4, uint8),
        OPERATOR        4       >=(uint4, int4),
        OPERATOR        4       >=(uint4, int8),
        OPERATOR        5       > ,
        OPERATOR        5       >(uint4, uint8),
        OPERATOR        5       >(uint4, int4),
        OPERATOR        5       >(uint4, int8),
        FUNCTION        1       uint4cmp(uint4, uint4),
        FUNCTION        1       uint48cmp(uint4, uint8),
        FUNCTION        1       uint4_int4cmp(uint4, int4),
        FUNCTION        1       uint4_int8cmp(uint4, int8),
        FUNCTION        2       uint4_sortsupport(internal);

CREATE OPERATOR CLASS uint8_ops
    DEFAULT FOR TYPE uint8 USING btree family integer_ops AS
        OPERATOR        1       < ,
        OPERATOR        1       <(uint8, int8),
        OPERATOR        2       <= ,
        OPERATOR        2       <=(uint8, int8),
        OPERATOR        3       = ,
        OPERATOR        3       =(uint8, int8),
        OPERATOR        4       >= ,
        OPERATOR        4       >=(uint8, int8),
        OPERATOR        5       > ,
        OPERATOR        5       >(uint8, int8),
        FUNCTION        1       uint8cmp(uint8, uint8),
        FUNCTION        1       uint8_int8cmp(uint8, int8),
        FUNCTION        2       uint8_sortsupport(internal);

CREATE OPERATOR CLASS uint1_ops
    DEFAULT FOR TYPE uint1 USING hash family integer_ops AS
        OPERATOR        1       = ,
        OPERATOR        1       =(uint1, uint2),
        OPERATOR        1       =(uint1, uint4),
        OPERATOR        1       =(uint1, uint8),
        OPERATOR        1       =(uint1, int1),
        OPERATOR        1       =(uint1, int2),
        OPERATOR        1       =(uint1, int4),
        OPERATOR        1       =(uint1, int8),
        OPERATOR        1       =(int1, uint1),
        FUNCTION        1       hashint1(int1),
        FUNCTION        1       hashuint1(uint1);

CREATE OPERATOR CLASS uint2_ops
    DEFAULT FOR TYPE uint2 USING hash family integer_ops AS
        OPERATOR        1       = ,
        OPERATOR        1       =(uint2, uint4),
        OPERATOR        1       =(uint2, uint8),
        OPERATOR        1       =(uint2, int2),
        OPERATOR        1       =(uint2, int4),
        OPERATOR        1       =(uint2, int8),
        OPERATOR        1       =(int2, uint2),
        FUNCTION        1       hashuint2(uint2);

CREATE OPERATOR CLASS uint4_ops
    DEFAULT FOR TYPE uint4 USING hash family integer_ops AS
        OPERATOR        1       = ,
        OPERATOR        1       =(uint4, uint8),
        OPERATOR        1       =(uint4, int4),
        OPERATOR        1       =(uint4, int8),
        OPERATOR        1       =(int4, uint4),
        FUNCTION        1       hashuint4(uint4);

CREATE OPERATOR CLASS uint8_ops
    DEFAULT FOR TYPE uint8 USING hash family integer_ops AS
        OPERATOR        1       = ,
        OPERATOR        1       =(uint8, int8),
        OPERATOR        1       =(int8, uint8),
        FUNCTION        1       hashuint8(uint8);    

-- << and >> operator
DROP FUNCTION IF EXISTS pg_catalog.uint1shr(uint1,int4) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.uint1shr (
uint1,int4
) RETURNS uint1 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint1shr';

CREATE OPERATOR pg_catalog.>>(
leftarg = uint1, rightarg = int4, procedure = uint1shr
);
COMMENT ON OPERATOR pg_catalog.>>(uint1, int4) IS 'uint1shr';

DROP FUNCTION IF EXISTS pg_catalog.uint1shl(uint1,int4) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.uint1shl (
uint1,int4
) RETURNS uint8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint1shl';

CREATE OPERATOR pg_catalog.<<(
leftarg = uint1, rightarg = int4, procedure = uint1shl
);
COMMENT ON OPERATOR pg_catalog.<<(uint1, int4) IS 'uint1shl';

DROP FUNCTION IF EXISTS pg_catalog.uint2shr(uint2,int4) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.uint2shr (
uint2,int4
) RETURNS uint2 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint2shr';

CREATE OPERATOR pg_catalog.>>(
leftarg = uint2, rightarg = int4, procedure = uint2shr
);
COMMENT ON OPERATOR pg_catalog.>>(uint2, int4) IS 'uint2shr';

DROP FUNCTION IF EXISTS pg_catalog.uint2shl(uint2,int4) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.uint2shl (
uint2,int4
) RETURNS uint8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint2shl';

CREATE OPERATOR pg_catalog.<<(
leftarg = uint2, rightarg = int4, procedure = uint2shl
);
COMMENT ON OPERATOR pg_catalog.<<(uint2, int4) IS 'uint2shl';

--uint4
DROP FUNCTION IF EXISTS pg_catalog.uint4shr(uint4,int4) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.uint4shr (
uint4,int4
) RETURNS uint4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint4shr';

CREATE OPERATOR pg_catalog.>>(
leftarg = uint4, rightarg = int4, procedure = uint4shr
);
COMMENT ON OPERATOR pg_catalog.>>(uint4, int4) IS 'uint4shr';

DROP FUNCTION IF EXISTS pg_catalog.uint4shl(uint4,int4) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.uint4shl (
uint4,int4
) RETURNS uint8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint4shl';

CREATE OPERATOR pg_catalog.<<(
leftarg = uint4, rightarg = int4, procedure = uint4shl
);
COMMENT ON OPERATOR pg_catalog.<<(uint4, int4) IS 'uint4shl';

--uint8
DROP FUNCTION IF EXISTS pg_catalog.uint8shr(uint8,int4) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.uint8shr (
uint8,int4
) RETURNS uint8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint8shr';

CREATE OPERATOR pg_catalog.>>(
leftarg = uint8, rightarg = int4, procedure = uint8shr
);
COMMENT ON OPERATOR pg_catalog.>>(uint8, int4) IS 'uint8shr';

DROP FUNCTION IF EXISTS pg_catalog.uint8shl(uint8,int4) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.uint8shl (
uint8,int4
) RETURNS uint8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint8shl';

CREATE OPERATOR pg_catalog.<<(
leftarg = uint8, rightarg = int4, procedure = uint8shl
);
COMMENT ON OPERATOR pg_catalog.<<(uint8, int4) IS 'uint8shl';

-- ~operator

CREATE OPERATOR pg_catalog.~(
rightarg = uint1, procedure = uint1not
);

CREATE OPERATOR pg_catalog.~(
rightarg = uint2, procedure = uint2not
);

CREATE OPERATOR pg_catalog.~(
rightarg = uint4, procedure = uint4not
);

CREATE OPERATOR pg_catalog.~(
rightarg = uint8, procedure = uint8not
);

--|

CREATE OPERATOR pg_catalog.|(
leftarg = uint1, rightarg = uint1, procedure = uint1or,
commutator=operator(pg_catalog.|)
);
COMMENT ON OPERATOR pg_catalog.|(uint1, uint1) IS 'uint1or';

CREATE OPERATOR pg_catalog.|(
leftarg = uint2, rightarg = uint2, procedure = uint2or,
commutator=operator(pg_catalog.|)
);
COMMENT ON OPERATOR pg_catalog.|(uint2, uint2) IS 'uint2or';

CREATE OPERATOR pg_catalog.|(
leftarg = uint4, rightarg = uint4, procedure = uint4or,
commutator=operator(pg_catalog.|)
);
COMMENT ON OPERATOR pg_catalog.|(uint4, uint4) IS 'uint4or';

CREATE OPERATOR pg_catalog.|(
leftarg = uint8, rightarg = uint8, procedure = uint8or,
commutator=operator(pg_catalog.|)
);
COMMENT ON OPERATOR pg_catalog.|(uint8, uint8) IS 'uint8or';

DROP FUNCTION IF EXISTS pg_catalog.uint1_or_int1(uint1, int1) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.uint1_or_int1 (
uint1, int1
) RETURNS uint1 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint1_or_int1';

CREATE OPERATOR pg_catalog.|(
leftarg = uint1, rightarg = int1, procedure = uint1_or_int1,
commutator=operator(pg_catalog.|)
);
COMMENT ON OPERATOR pg_catalog.|(uint1, int1) IS 'uint1_or_int1';

DROP FUNCTION IF EXISTS pg_catalog.int1_or_uint1(int1, uint1) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.int1_or_uint1 (
int1, uint1
) RETURNS uint1 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int1_or_uint1';

CREATE OPERATOR pg_catalog.|(
leftarg = int1, rightarg = uint1, procedure = int1_or_uint1,
commutator=operator(pg_catalog.|)
);
COMMENT ON OPERATOR pg_catalog.|(int1, uint1) IS 'int1_or_uint1';

DROP FUNCTION IF EXISTS pg_catalog.uint2_or_int2(uint2, int2) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.uint2_or_int2 (
uint2, int2
) RETURNS uint2 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint2_or_int2';

CREATE OPERATOR pg_catalog.|(
leftarg = uint2, rightarg = int2, procedure = uint2_or_int2,
commutator=operator(pg_catalog.|)
);
COMMENT ON OPERATOR pg_catalog.|(uint2, int2) IS 'uint2_or_int2';

DROP FUNCTION IF EXISTS pg_catalog.int2_or_uint2(int2, uint2) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.int2_or_uint2 (
int2, uint2
) RETURNS uint2 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int2_or_uint2';

CREATE OPERATOR pg_catalog.|(
leftarg = int2, rightarg = uint2, procedure = int2_or_uint2,
commutator=operator(pg_catalog.|)
);
COMMENT ON OPERATOR pg_catalog.|(int2, uint2) IS 'int2_or_uint2';


DROP FUNCTION IF EXISTS pg_catalog.uint4_or_int4(uint4, int4) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.uint4_or_int4 (
uint4, int4
) RETURNS uint4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint4_or_int4';

CREATE OPERATOR pg_catalog.|(
leftarg = uint4, rightarg = int4, procedure = uint4_or_int4,
commutator=operator(pg_catalog.|)
);
COMMENT ON OPERATOR pg_catalog.|(uint4, int4) IS 'uint4_or_int4';

DROP FUNCTION IF EXISTS pg_catalog.int4_or_uint4(int4, uint4) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.int4_or_uint4 (
int4, uint4
) RETURNS uint4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int4_or_uint4';

CREATE OPERATOR pg_catalog.|(
leftarg = int4, rightarg = uint4, procedure = int4_or_uint4,
commutator=operator(pg_catalog.|)
);
COMMENT ON OPERATOR pg_catalog.|(int4, uint4) IS 'int4_or_uint4';

DROP FUNCTION IF EXISTS pg_catalog.uint8_or_int8(uint8, int8) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.uint8_or_int8 (
uint8, int8
) RETURNS uint8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint8_or_int8';

CREATE OPERATOR pg_catalog.|(
leftarg = uint8, rightarg = int8, procedure = uint8_or_int8,
commutator=operator(pg_catalog.|)
);
COMMENT ON OPERATOR pg_catalog.|(uint8, int8) IS 'uint8_or_int8';

DROP FUNCTION IF EXISTS pg_catalog.int8_or_uint8(int8, uint8) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.int8_or_uint8 (
int8, uint8
) RETURNS uint8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int8_or_uint8';

CREATE OPERATOR pg_catalog.|(
leftarg = int8, rightarg = uint8, procedure = int8_or_uint8,
commutator=operator(pg_catalog.|)
);
COMMENT ON OPERATOR pg_catalog.|(int8, uint8) IS 'int8_or_uint8';

--#

CREATE OPERATOR pg_catalog.#(
leftarg = uint1, rightarg = uint1, procedure = uint1xor,
commutator=operator(pg_catalog.#)
);
COMMENT ON OPERATOR pg_catalog.#(uint1, uint1) IS 'uint1xor';

CREATE OPERATOR pg_catalog.#(
leftarg = uint2, rightarg = uint2, procedure = uint2xor,
commutator=operator(pg_catalog.#)
);
COMMENT ON OPERATOR pg_catalog.#(uint2, uint2) IS 'uint2xor';

CREATE OPERATOR pg_catalog.#(
leftarg = uint4, rightarg = uint4, procedure = uint4xor,
commutator=operator(pg_catalog.#)
);
COMMENT ON OPERATOR pg_catalog.#(uint4, uint4) IS 'uint4xor';

CREATE OPERATOR pg_catalog.#(
leftarg = uint8, rightarg = uint8, procedure = uint8xor,
commutator=operator(pg_catalog.#)
);
COMMENT ON OPERATOR pg_catalog.#(uint8, uint8) IS 'uint8xor';

DROP FUNCTION IF EXISTS pg_catalog.uint1_xor_int1(uint1, int1) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.uint1_xor_int1 (
uint1, int1
) RETURNS uint1 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint1_xor_int1';

CREATE OPERATOR pg_catalog.#(
leftarg = uint1, rightarg = int1, procedure = uint1_xor_int1
);
COMMENT ON OPERATOR pg_catalog.#(uint1, int1) IS 'uint1_xor_int1';

DROP FUNCTION IF EXISTS pg_catalog.int1_xor_uint1(int1, uint1) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.int1_xor_uint1 (
int1, uint1
) RETURNS uint1 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int1_xor_uint1';

CREATE OPERATOR pg_catalog.#(
leftarg = int1, rightarg = uint1, procedure = int1_xor_uint1
);
COMMENT ON OPERATOR pg_catalog.#(int1, uint1) IS 'int1_xor_uint1';

DROP FUNCTION IF EXISTS pg_catalog.uint2_xor_int2(uint2, int2) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.uint2_xor_int2 (
uint2, int2
) RETURNS uint2 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint2_xor_int2';

CREATE OPERATOR pg_catalog.#(
leftarg = uint2, rightarg = int2, procedure = uint2_xor_int2,
commutator=operator(pg_catalog.#)
);
COMMENT ON OPERATOR pg_catalog.#(uint2, int2) IS 'uint2_xor_int2';

DROP FUNCTION IF EXISTS pg_catalog.int2_xor_uint2(int2, uint2) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.int2_xor_uint2 (
int2, uint2
) RETURNS uint2 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int2_xor_uint2';

CREATE OPERATOR pg_catalog.#(
leftarg = int2, rightarg = uint2, procedure = int2_xor_uint2,
commutator=operator(pg_catalog.#)
);
COMMENT ON OPERATOR pg_catalog.#(int2, uint2) IS 'int2_xor_uint2';

DROP FUNCTION IF EXISTS pg_catalog.uint4_xor_int4(uint4, int4) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.uint4_xor_int4 (
uint4, int4
) RETURNS uint4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint4_xor_int4';

CREATE OPERATOR pg_catalog.#(
leftarg = uint4, rightarg = int4, procedure = uint4_xor_int4,
commutator=operator(pg_catalog.#)
);
COMMENT ON OPERATOR pg_catalog.#(uint4, int4) IS 'uint4_xor_int4';

DROP FUNCTION IF EXISTS pg_catalog.int4_xor_uint4(int4, uint4) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.int4_xor_uint4 (
int4, uint4
) RETURNS uint4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int4_xor_uint4';

CREATE OPERATOR pg_catalog.#(
leftarg = int4, rightarg = uint4, procedure = int4_xor_uint4,
commutator=operator(pg_catalog.#)
);
COMMENT ON OPERATOR pg_catalog.#(int4, uint4) IS 'int4_xor_uint4';

DROP FUNCTION IF EXISTS pg_catalog.uint8_xor_int8(uint8, int8) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.uint8_xor_int8 (
uint8, int8
) RETURNS uint8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint8_xor_int8';

CREATE OPERATOR pg_catalog.#(
leftarg = uint8, rightarg = int8, procedure = uint8_xor_int8,
commutator=operator(pg_catalog.#)
);
COMMENT ON OPERATOR pg_catalog.#(uint8, int8) IS 'uint8_xor_int8';

DROP FUNCTION IF EXISTS pg_catalog.int8_xor_uint8(int8, uint8) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.int8_xor_uint8 (
int8, uint8
) RETURNS uint8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int8_xor_uint8';

CREATE OPERATOR pg_catalog.#(
leftarg = int8, rightarg = uint8, procedure = int8_xor_uint8,
commutator=operator(pg_catalog.#)
);
COMMENT ON OPERATOR pg_catalog.#(int8, uint8) IS 'int8_xor_uint8';

-- & operator

CREATE OPERATOR pg_catalog.&(
leftarg = uint1, rightarg = uint1, procedure = uint1and,
commutator=operator(pg_catalog.&)
);
COMMENT ON OPERATOR pg_catalog.&(uint1, uint1) IS 'uint1and';

CREATE OPERATOR pg_catalog.&(
leftarg = uint2, rightarg = uint2, procedure = uint2and,
commutator=operator(pg_catalog.&)
);
COMMENT ON OPERATOR pg_catalog.&(uint2, uint2) IS 'uint2and';

CREATE OPERATOR pg_catalog.&(
leftarg = uint4, rightarg = uint4, procedure = uint4and,
commutator=operator(pg_catalog.&)
);
COMMENT ON OPERATOR pg_catalog.&(uint4, uint4) IS 'uint4and';

CREATE OPERATOR pg_catalog.&(
leftarg = uint8, rightarg = uint8, procedure = uint8and,
commutator=operator(pg_catalog.&)
);
COMMENT ON OPERATOR pg_catalog.&(uint8, uint8) IS 'uint8and';

DROP FUNCTION IF EXISTS pg_catalog.uint1_and_int1(uint1, int1) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.uint1_and_int1 (
uint1, int1
) RETURNS uint1 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint1_and_int1';

CREATE OPERATOR pg_catalog.&(
leftarg = uint1, rightarg = int1, procedure = uint1_and_int1
);
COMMENT ON OPERATOR pg_catalog.&(uint1, int1) IS 'uint1_and_int1';

DROP FUNCTION IF EXISTS pg_catalog.int1_and_uint1(int1, uint1) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.int1_and_uint1 (
int1, uint1
) RETURNS uint1 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int1_and_uint1';

CREATE OPERATOR pg_catalog.&(
leftarg = int1, rightarg = uint1, procedure = int1_and_uint1
);
COMMENT ON OPERATOR pg_catalog.&(int1, uint1) IS 'int1_and_uint1';

DROP FUNCTION IF EXISTS pg_catalog.uint2_and_int2(uint2, int2) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.uint2_and_int2 (
uint2, int2
) RETURNS uint2 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint2_and_int2';

CREATE OPERATOR pg_catalog.&(
leftarg = uint2, rightarg = int2, procedure = uint2_and_int2,
commutator=operator(pg_catalog.&)
);
COMMENT ON OPERATOR pg_catalog.&(uint2, int2) IS 'uint2_and_int2';

DROP FUNCTION IF EXISTS pg_catalog.int2_and_uint2(int2, uint2) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.int2_and_uint2 (
int2, uint2
) RETURNS uint2 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int2_and_uint2';

CREATE OPERATOR pg_catalog.&(
leftarg = int2, rightarg = uint2, procedure = int2_and_uint2,
commutator=operator(pg_catalog.&)
);
COMMENT ON OPERATOR pg_catalog.&(int2, uint2) IS 'int2_and_uint2';

DROP FUNCTION IF EXISTS pg_catalog.uint4_and_int4(uint4, int4) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.uint4_and_int4 (
uint4, int4
) RETURNS uint4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint4_and_int4';

CREATE OPERATOR pg_catalog.&(
leftarg = uint4, rightarg = int4, procedure = uint4_and_int4,
commutator=operator(pg_catalog.&)
);
COMMENT ON OPERATOR pg_catalog.&(uint4, int4) IS 'uint4_and_int4';

DROP FUNCTION IF EXISTS pg_catalog.int4_and_uint4(int4, uint4) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.int4_and_uint4 (
int4, uint4
) RETURNS uint4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int4_and_uint4';

CREATE OPERATOR pg_catalog.&(
leftarg = int4, rightarg = uint4, procedure = int4_and_uint4,
commutator=operator(pg_catalog.&)
);
COMMENT ON OPERATOR pg_catalog.&(int4, uint4) IS 'int4_and_uint4';

DROP FUNCTION IF EXISTS pg_catalog.uint8_and_int8(uint8, int8) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.uint8_and_int8 (
uint8, int8
) RETURNS uint8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint8_and_int8';

CREATE OPERATOR pg_catalog.&(
leftarg = uint8, rightarg = int8, procedure = uint8_and_int8,
commutator=operator(pg_catalog.&)
);
COMMENT ON OPERATOR pg_catalog.&(uint8, int8) IS 'uint8_and_int8';

DROP FUNCTION IF EXISTS pg_catalog.int8_and_uint8(int8, uint8) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.int8_and_uint8 (
int8, uint8
) RETURNS uint8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int8_and_uint8';

CREATE OPERATOR pg_catalog.&(
leftarg = int8, rightarg = uint8, procedure = int8_and_uint8,
commutator=operator(pg_catalog.&)
);
COMMENT ON OPERATOR pg_catalog.&(int8, uint8) IS 'int8_and_uint8';

-- cast
DROP FUNCTION IF EXISTS pg_catalog.i1_cast_ui1(int1) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.i1_cast_ui1 (
int1
) RETURNS uint1 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'i1_cast_ui1';

DROP FUNCTION IF EXISTS pg_catalog.i2_cast_ui1(int2) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.i2_cast_ui1 (
int2
) RETURNS uint1 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'i2_cast_ui1';

DROP FUNCTION IF EXISTS pg_catalog.i4_cast_ui1(int4) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.i4_cast_ui1 (
int4
) RETURNS uint1 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'i4_cast_ui1';

DROP FUNCTION IF EXISTS pg_catalog.i8_cast_ui1(int8) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.i8_cast_ui1 (
int8
) RETURNS uint1 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'i8_cast_ui1';

DROP FUNCTION IF EXISTS pg_catalog.i1_cast_ui2(int1) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.i1_cast_ui2 (
int1
) RETURNS uint2 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'i1_cast_ui2';

DROP FUNCTION IF EXISTS pg_catalog.i2_cast_ui2(int2) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.i2_cast_ui2 (
int2
) RETURNS uint2 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'i2_cast_ui2';

DROP FUNCTION IF EXISTS pg_catalog.i4_cast_ui2(int4) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.i4_cast_ui2 (
int4
) RETURNS uint2 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'i4_cast_ui2';

DROP FUNCTION IF EXISTS pg_catalog.i8_cast_ui2(int8) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.i8_cast_ui2 (
int8
) RETURNS uint2 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'i8_cast_ui2';

DROP FUNCTION IF EXISTS pg_catalog.i1_cast_ui4(int1) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.i1_cast_ui4 (
int1
) RETURNS uint4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'i1_cast_ui4';

DROP FUNCTION IF EXISTS pg_catalog.i2_cast_ui4(int2) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.i2_cast_ui4 (
int2
) RETURNS uint4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'i2_cast_ui4';

DROP FUNCTION IF EXISTS pg_catalog.i4_cast_ui4(int4) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.i4_cast_ui4 (
int4
) RETURNS uint4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'i4_cast_ui4';

DROP FUNCTION IF EXISTS pg_catalog.i8_cast_ui4(int8) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.i8_cast_ui4 (
int8
) RETURNS uint4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'i8_cast_ui4';

DROP FUNCTION IF EXISTS pg_catalog.i1_cast_ui8(int1) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.i1_cast_ui8 (
int1
) RETURNS uint8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'i1_cast_ui8';

DROP FUNCTION IF EXISTS pg_catalog.i2_cast_ui8(int2) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.i2_cast_ui8 (
int2
) RETURNS uint8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'i2_cast_ui8';

DROP FUNCTION IF EXISTS pg_catalog.i4_cast_ui8(int4) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.i4_cast_ui8 (
int4
) RETURNS uint8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'i4_cast_ui8';

DROP FUNCTION IF EXISTS pg_catalog.i8_cast_ui8(int8) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.i8_cast_ui8 (
int8
) RETURNS uint8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'i8_cast_ui8';

DROP FUNCTION IF EXISTS pg_catalog.f4_cast_ui1(float4) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.f4_cast_ui1 (
float4
) RETURNS uint1 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'f4_cast_ui1';

DROP FUNCTION IF EXISTS pg_catalog.f8_cast_ui1(float8) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.f8_cast_ui1 (
float8
) RETURNS uint1 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'f8_cast_ui1';

DROP FUNCTION IF EXISTS pg_catalog.f4_cast_ui2(float4) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.f4_cast_ui2 (
float4
) RETURNS uint2 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'f4_cast_ui2';

DROP FUNCTION IF EXISTS pg_catalog.f8_cast_ui2(float8) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.f8_cast_ui2 (
float8
) RETURNS uint2 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'f8_cast_ui2';

DROP FUNCTION IF EXISTS pg_catalog.f4_cast_ui4(float4) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.f4_cast_ui4 (
float4
) RETURNS uint4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'f4_cast_ui4';

DROP FUNCTION IF EXISTS pg_catalog.f8_cast_ui4(float8) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.f8_cast_ui4 (
float8
) RETURNS uint4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'f8_cast_ui4';

DROP FUNCTION IF EXISTS pg_catalog.f4_cast_ui8(float4) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.f4_cast_ui8 (
float4
) RETURNS uint8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'f4_cast_ui8';

DROP FUNCTION IF EXISTS pg_catalog.f8_cast_ui8(float8) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.f8_cast_ui8 (
float8
) RETURNS uint8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'f8_cast_ui8';

-- uint -> cash
DROP FUNCTION IF EXISTS pg_catalog.uint_cash(uint1) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.uint_cash (
uint1
) RETURNS money LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint_cash';
DROP FUNCTION IF EXISTS pg_catalog.uint_cash(uint2) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.uint_cash (
uint2
) RETURNS money LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint_cash';
DROP FUNCTION IF EXISTS pg_catalog.uint_cash(uint4) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.uint_cash (
uint4
) RETURNS money LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint_cash';
DROP FUNCTION IF EXISTS pg_catalog.uint_cash(uint8) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.uint_cash (
uint8
) RETURNS money LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint_cash';
drop CAST IF EXISTS (uint1 AS money) CASCADE;
CREATE CAST (uint1 AS money) WITH FUNCTION uint_cash(uint1) AS ASSIGNMENT;
drop CAST IF EXISTS (uint2 AS money) CASCADE;
CREATE CAST (uint2 AS money) WITH FUNCTION uint_cash(uint2) AS ASSIGNMENT;
drop CAST IF EXISTS (uint4 AS money) CASCADE;
CREATE CAST (uint4 AS money) WITH FUNCTION uint_cash(uint4) AS ASSIGNMENT;
drop CAST IF EXISTS (uint8 AS money) CASCADE;
CREATE CAST (uint8 AS money) WITH FUNCTION uint_cash(uint8) AS ASSIGNMENT;

-- uint4 <-> time
drop CAST IF EXISTS (uint4 AS abstime) CASCADE;
CREATE CAST (uint4 AS abstime) WITHOUT FUNCTION;
drop CAST IF EXISTS (abstime AS uint4) CASCADE;
CREATE CAST (abstime as uint4) WITHOUT FUNCTION;
drop CAST IF EXISTS (uint4 AS reltime) CASCADE;
CREATE CAST (uint4 AS reltime) WITHOUT FUNCTION;
drop CAST IF EXISTS (reltime AS uint4) CASCADE;
CREATE CAST (reltime as uint4) WITHOUT FUNCTION;

DROP FUNCTION IF EXISTS pg_catalog.int32_b_format_date(uint4) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.int32_b_format_date (
uint4
) RETURNS date LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int32_b_format_date';
drop CAST IF EXISTS (uint4 AS date) CASCADE;
CREATE CAST (uint4 AS date) WITH FUNCTION int32_b_format_date(uint4);

DROP FUNCTION IF EXISTS pg_catalog.int32_b_format_time(uint4) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.int32_b_format_time (
uint4
) RETURNS time LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int32_b_format_time';
drop CAST IF EXISTS (uint4 AS time) CASCADE;
CREATE CAST (uint4 AS time) WITH FUNCTION int32_b_format_time(uint4);

DROP FUNCTION IF EXISTS pg_catalog.int32_b_format_datetime(uint4) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.int32_b_format_datetime (
uint4
) RETURNS timestamp(0) without time zone LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int32_b_format_datetime';
drop CAST IF EXISTS (uint4 AS timestamp(0) without time zone) CASCADE;
CREATE CAST (uint4 AS timestamp(0) without time zone) WITH FUNCTION int32_b_format_datetime(uint4);

DROP FUNCTION IF EXISTS pg_catalog.int64_b_format_datetime(uint8) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.int64_b_format_datetime (
uint8
) RETURNS timestamp(0) without time zone LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int64_b_format_datetime';
drop CAST IF EXISTS (uint8 AS timestamp(0) without time zone) CASCADE;
CREATE CAST (uint8 AS timestamp(0) without time zone) WITH FUNCTION int64_b_format_datetime(uint8);

DROP FUNCTION IF EXISTS pg_catalog.int32_b_format_timestamp(uint4) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.int32_b_format_timestamp (
uint4
) RETURNS timestamptz LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int32_b_format_timestamp';
drop CAST IF EXISTS (uint4 AS timestamptz) CASCADE;
CREATE CAST (uint4 AS timestamptz) WITH FUNCTION int32_b_format_timestamp(uint4);

DROP FUNCTION IF EXISTS pg_catalog.int64_b_format_timestamp(uint8) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.int64_b_format_timestamp (
uint8
) RETURNS timestamptz LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int64_b_format_timestamp';
drop CAST IF EXISTS (uint8 AS timestamptz) CASCADE;
CREATE CAST (uint8 AS timestamptz) WITH FUNCTION int64_b_format_timestamp(uint8);

DROP FUNCTION IF EXISTS pg_catalog.int32_year(uint4) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.int32_year (
uint4
) RETURNS year LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int32_year';
drop CAST IF EXISTS (uint4 AS year) CASCADE;
CREATE CAST (uint4 AS year) WITH FUNCTION int32_year(uint4);

DROP FUNCTION IF EXISTS pg_catalog.year_uint4(year) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.year_uint4 (
year
) RETURNS uint4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'year_integer';
drop CAST IF EXISTS (uint4 AS year) CASCADE;
CREATE CAST (uint4 AS year) WITH FUNCTION int32_year(uint4);

drop CAST IF EXISTS (year AS uint4) CASCADE;
CREATE CAST (year AS uint4) WITH FUNCTION year_uint4(year);

-- uint <-> bit
DROP FUNCTION IF EXISTS pg_catalog.bittouint4(bit) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.bittouint4 (
bit
) RETURNS uint4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'bittouint4';
DROP FUNCTION IF EXISTS pg_catalog.bittouint8(bit) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.bittouint8 (
bit
) RETURNS uint8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'bittouint8';
DROP FUNCTION IF EXISTS pg_catalog.bitfromuint4(uint4, int4) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.bitfromuint4 (
uint4, int4
) RETURNS bit LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'bitfromuint4';
DROP FUNCTION IF EXISTS pg_catalog.bitfromuint8(uint8, int4) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.bitfromuint8 (
uint8, int4
) RETURNS bit LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'bitfromuint8';

drop CAST IF EXISTS (bit AS uint4) CASCADE;
drop CAST IF EXISTS (bit AS uint8) CASCADE;
drop CAST IF EXISTS (uint4 AS bit) CASCADE;
drop CAST IF EXISTS (uint8 AS bit) CASCADE;

CREATE CAST (bit AS uint4) WITH FUNCTION bittouint4(bit);
CREATE CAST (bit AS uint8) WITH FUNCTION bittouint8(bit);
CREATE CAST (uint4 AS bit) WITH FUNCTION bitfromuint4(uint4, int4);
CREATE CAST (uint8 AS bit) WITH FUNCTION bitfromuint8(uint8, int4);

-- uint <-> int16
DROP FUNCTION IF EXISTS pg_catalog.uint_16(uint1) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.uint_16 (
uint1
) RETURNS int16 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint_16';
DROP FUNCTION IF EXISTS pg_catalog.uint_16(uint2) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.uint_16 (
uint2
) RETURNS int16 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint_16';
DROP FUNCTION IF EXISTS pg_catalog.uint_16(uint4) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.uint_16 (
uint4
) RETURNS int16 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint_16';
DROP FUNCTION IF EXISTS pg_catalog.uint_16(uint8) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.uint_16 (
uint8
) RETURNS int16 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint_16';

DROP FUNCTION IF EXISTS pg_catalog.int16_u1(int16) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.int16_u1 (
int16
) RETURNS uint1 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int16_u1';

DROP FUNCTION IF EXISTS pg_catalog.int16_u2(int16) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.int16_u2 (
int16
) RETURNS uint2 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int16_u2';

DROP FUNCTION IF EXISTS pg_catalog.int16_u4(int16) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.int16_u4 (
int16
) RETURNS uint4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int16_u4';

DROP FUNCTION IF EXISTS pg_catalog.int16_u8(int16) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.int16_u8 (
int16
) RETURNS uint8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int16_u8';

drop CAST IF EXISTS (uint1 AS int16) CASCADE;
CREATE CAST (uint1 AS int16) WITH FUNCTION uint_16(uint1) AS ASSIGNMENT;
drop CAST IF EXISTS (uint2 AS int16) CASCADE;
CREATE CAST (uint2 AS int16) WITH FUNCTION uint_16(uint2) AS ASSIGNMENT;
drop CAST IF EXISTS (uint4 AS int16) CASCADE;
CREATE CAST (uint4 AS int16) WITH FUNCTION uint_16(uint4) AS ASSIGNMENT;
drop CAST IF EXISTS (uint8 AS int16) CASCADE;
CREATE CAST (uint8 AS int16) WITH FUNCTION uint_16(uint8) AS ASSIGNMENT;

drop CAST IF EXISTS (int16 AS uint1) CASCADE;
CREATE CAST (int16 AS uint1) WITH FUNCTION int16_u1(int16) AS ASSIGNMENT;
drop CAST IF EXISTS (int16 AS uint2) CASCADE;
CREATE CAST (int16 AS uint2) WITH FUNCTION int16_u2(int16) AS ASSIGNMENT;
drop CAST IF EXISTS (int16 AS uint4) CASCADE;
CREATE CAST (int16 AS uint4) WITH FUNCTION int16_u4(int16) AS ASSIGNMENT;
drop CAST IF EXISTS (int16 AS uint8) CASCADE;
CREATE CAST (int16 AS uint8) WITH FUNCTION int16_u8(int16) AS ASSIGNMENT;

DROP FUNCTION IF EXISTS pg_catalog.text_cast_uint1(text) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.text_cast_uint1 (
text
) RETURNS uint1 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'text_cast_uint1';

DROP FUNCTION IF EXISTS pg_catalog.text_cast_uint2(text) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.text_cast_uint2 (
text
) RETURNS uint2 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'text_cast_uint2';

DROP FUNCTION IF EXISTS pg_catalog.text_cast_uint4(text) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.text_cast_uint4 (
text
) RETURNS uint4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'text_cast_uint4';

DROP FUNCTION IF EXISTS pg_catalog.text_cast_uint8(text) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.text_cast_uint8 (
text
) RETURNS uint8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'text_cast_uint8';

-- ^ operator

CREATE OPERATOR pg_catalog.^(
leftarg = uint1, rightarg = uint1, procedure = uint1xor,
commutator=operator(pg_catalog.^)
);
COMMENT ON OPERATOR pg_catalog.^(uint1, uint1) IS 'uint1xor';


CREATE OPERATOR pg_catalog.^(
leftarg = uint2, rightarg = uint2, procedure = uint2xor,
commutator=operator(pg_catalog.^)
);
COMMENT ON OPERATOR pg_catalog.^(uint2, uint2) IS 'uint2xor';


CREATE OPERATOR pg_catalog.^(
leftarg = uint4, rightarg = uint4, procedure = uint4xor,
commutator=operator(pg_catalog.^)
);
COMMENT ON OPERATOR pg_catalog.^(uint4, uint4) IS 'uint4xor';


CREATE OPERATOR pg_catalog.^(
leftarg = uint8, rightarg = uint8, procedure = uint8xor,
commutator=operator(pg_catalog.^)
);
COMMENT ON OPERATOR pg_catalog.^(uint8, uint8) IS 'uint8xor';


CREATE OPERATOR pg_catalog.^(
leftarg = uint1, rightarg = int1, procedure = uint1_xor_int1
);
COMMENT ON OPERATOR pg_catalog.^(uint1, int1) IS 'uint1_xor_int1';


CREATE OPERATOR pg_catalog.^(
leftarg = int1, rightarg = uint1, procedure = int1_xor_uint1
);
COMMENT ON OPERATOR pg_catalog.^(int1, uint1) IS 'int1_xor_uint1';


CREATE OPERATOR pg_catalog.^(
leftarg = uint2, rightarg = int2, procedure = uint2_xor_int2,
commutator=operator(pg_catalog.^)
);
COMMENT ON OPERATOR pg_catalog.^(uint2, int2) IS 'uint2_xor_int2';


CREATE OPERATOR pg_catalog.^(
leftarg = int2, rightarg = uint2, procedure = int2_xor_uint2,
commutator=operator(pg_catalog.^)
);
COMMENT ON OPERATOR pg_catalog.^(int2, uint2) IS 'int2_xor_uint2';


CREATE OPERATOR pg_catalog.^(
leftarg = uint4, rightarg = int4, procedure = uint4_xor_int4,
commutator=operator(pg_catalog.^)
);
COMMENT ON OPERATOR pg_catalog.^(uint4, int4) IS 'uint4_xor_int4';


CREATE OPERATOR pg_catalog.^(
leftarg = int4, rightarg = uint4, procedure = int4_xor_uint4,
commutator=operator(pg_catalog.^)
);
COMMENT ON OPERATOR pg_catalog.^(int4, uint4) IS 'int4_xor_uint4';


CREATE OPERATOR pg_catalog.^(
leftarg = uint8, rightarg = int8, procedure = uint8_xor_int8,
commutator=operator(pg_catalog.^)
);
COMMENT ON OPERATOR pg_catalog.^(uint8, int8) IS 'uint8_xor_int8';


CREATE OPERATOR pg_catalog.^(
leftarg = int8, rightarg = uint8, procedure = int8_xor_uint8,
commutator=operator(pg_catalog.^)
);
COMMENT ON OPERATOR pg_catalog.^(int8, uint8) IS 'int8_xor_uint8';

DROP FUNCTION IF EXISTS pg_catalog.uint2_xor_bool(uint2, boolean) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.uint2_xor_bool (
uint2, boolean
) RETURNS uint2 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint2_xor_bool';

DROP FUNCTION IF EXISTS pg_catalog.uint1_xor_bool(uint1, boolean) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.uint1_xor_bool (
uint1, boolean
) RETURNS uint1 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint1_xor_bool';

DROP FUNCTION IF EXISTS pg_catalog.uint4_xor_bool(uint4, boolean) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.uint4_xor_bool (
uint4, boolean
) RETURNS uint4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint4_xor_bool';

DROP FUNCTION IF EXISTS pg_catalog.uint8_xor_bool(uint8, boolean) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.uint8_xor_bool (
uint8, boolean
) RETURNS uint8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint8_xor_bool';

DROP FUNCTION IF EXISTS pg_catalog.bool_xor_uint1(boolean, uint1) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.bool_xor_uint1 (
boolean, uint1
) RETURNS uint1 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'bool_xor_uint1';

DROP FUNCTION IF EXISTS pg_catalog.bool_xor_uint2(boolean, uint2) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.bool_xor_uint2 (
boolean, uint2
) RETURNS uint2 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'bool_xor_uint2';

DROP FUNCTION IF EXISTS pg_catalog.bool_xor_uint4(boolean, uint4) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.bool_xor_uint4 (
boolean, uint4
) RETURNS uint4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'bool_xor_uint4';

DROP FUNCTION IF EXISTS pg_catalog.bool_xor_uint8(boolean, uint8) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.bool_xor_uint8 (
boolean, uint8
) RETURNS uint8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'bool_xor_uint8';

CREATE OPERATOR pg_catalog.^(
leftarg = uint8, rightarg = boolean, procedure = uint8_xor_bool,
commutator=operator(pg_catalog.^)
);
COMMENT ON OPERATOR pg_catalog.^(uint8, boolean) IS 'uint8_xor_bool';
CREATE OPERATOR pg_catalog.^(
leftarg = uint4, rightarg = boolean, procedure = uint4_xor_bool,
commutator=operator(pg_catalog.^)
);
COMMENT ON OPERATOR pg_catalog.^(uint4, boolean) IS 'uint4_xor_bool';
CREATE OPERATOR pg_catalog.^(
leftarg = uint2, rightarg = boolean, procedure = uint2_xor_bool,
commutator=operator(pg_catalog.^)
);
COMMENT ON OPERATOR pg_catalog.^(uint2, boolean) IS 'uint2_xor_bool';
CREATE OPERATOR pg_catalog.^(
leftarg = uint1, rightarg = boolean, procedure = uint1_xor_bool,
commutator=operator(pg_catalog.^)
);
COMMENT ON OPERATOR pg_catalog.^(uint1, boolean) IS 'uint1_xor_bool';
CREATE OPERATOR pg_catalog.^(
leftarg = boolean, rightarg = uint1, procedure = bool_xor_uint1,
commutator=operator(pg_catalog.^)
);
COMMENT ON OPERATOR pg_catalog.^(boolean, uint1) IS 'bool_xor_uint1';
CREATE OPERATOR pg_catalog.^(
leftarg = boolean, rightarg = uint2, procedure = bool_xor_uint2,
commutator=operator(pg_catalog.^)
);
COMMENT ON OPERATOR pg_catalog.^(boolean, uint2) IS 'bool_xor_uint2';
CREATE OPERATOR pg_catalog.^(
leftarg = boolean, rightarg = uint4, procedure = bool_xor_uint4,
commutator=operator(pg_catalog.^)
);
COMMENT ON OPERATOR pg_catalog.^(boolean, uint4) IS 'bool_xor_uint4';
CREATE OPERATOR pg_catalog.^(
leftarg = boolean, rightarg = uint8, procedure = bool_xor_uint8,
commutator=operator(pg_catalog.^)
);
COMMENT ON OPERATOR pg_catalog.^(boolean, uint8) IS 'bool_xor_uint8';

DROP FUNCTION IF EXISTS timestamp_uint8(timestamp(0) with time zone) CASCADE;
CREATE OR REPLACE FUNCTION timestamp_uint8(timestamp(0) with time zone)
RETURNS uint8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'timestamp_uint8';
drop CAST IF EXISTS (timestamp(0) with time zone AS uint8) CASCADE;
CREATE CAST (timestamp(0) with time zone AS uint8) WITH FUNCTION timestamp_uint8(timestamp(0) with time zone);

DROP FUNCTION IF EXISTS cash_uint(money) CASCADE;
CREATE OR REPLACE FUNCTION cash_uint(money)
RETURNS uint8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'cash_uint';
drop CAST IF EXISTS (money AS uint8) CASCADE;
CREATE CAST (money AS uint8) WITH FUNCTION cash_uint(money) AS ASSIGNMENT;

DROP FUNCTION IF EXISTS pg_catalog.int8_xor(uint8,int8) CASCADE;
CREATE FUNCTION pg_catalog.int8_xor (
uint8,int8
) RETURNS uint8 LANGUAGE C as '$libdir/dolphin',  'uint8_xor_int8';
drop aggregate if exists pg_catalog.bit_xor(int8);
create aggregate pg_catalog.bit_xor(int8) (SFUNC=int8_xor, STYPE= uint8);

DROP FUNCTION IF EXISTS pg_catalog.uint8_xor(uint8,int8) CASCADE;
CREATE FUNCTION pg_catalog.uint8_xor (
uint8,int8
) RETURNS uint8 LANGUAGE C as '$libdir/dolphin',  'uint8xor';

DROP FUNCTION IF EXISTS pg_catalog.text_xor(uint8,text) CASCADE;
CREATE FUNCTION pg_catalog.text_xor (t1 uint8,t2 text) RETURNS uint8 AS
$$
DECLARE num NUMBER := to_number(t2);
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
drop aggregate if exists pg_catalog.bit_xor(text);
create aggregate pg_catalog.bit_xor(text) (SFUNC=text_xor, STYPE= uint8);

DROP FUNCTION IF EXISTS pg_catalog.uint_any_value (uint8, uint8) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.uint_any_value (uint8, uint8) RETURNS uint8 LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'uint_any_value';
drop aggregate if exists pg_catalog.any_value(uint8);
CREATE AGGREGATE pg_catalog.any_value(uint8) (
        sfunc = uint_any_value,
        stype = uint8
);

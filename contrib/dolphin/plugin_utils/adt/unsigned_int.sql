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
DROP FUNCTION IF EXISTS pg_catalog.hashuint1(uint1,uint1) CASCADE;

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


CREATE OR REPLACE FUNCTION pg_catalog.hashuint1 (
uint1,uint1
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
) RETURNS uint1 LANGUAGE C  IMMUTABLE STRICT as '$libdir/dolphin',  'uint1xor';


DROP FUNCTION IF EXISTS pg_catalog.uint1um(uint1) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.uint1um (
uint1
) RETURNS int2 LANGUAGE C STRICT IMMUTABLE  as '$libdir/dolphin',  'uint1um';

DROP FUNCTION IF EXISTS pg_catalog.uint1not(uint1) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.uint1not (
uint1
) RETURNS uint1 LANGUAGE C  IMMUTABLE STRICT as '$libdir/dolphin',  'uint1not';

DROP FUNCTION IF EXISTS pg_catalog.uint1up(uint1) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.uint1up (
uint1
) RETURNS uint1 LANGUAGE C  IMMUTABLE STRICT as '$libdir/dolphin',  'uint1up';

DROP FUNCTION IF EXISTS pg_catalog.uint1_avg_accum(bigint[], uint1) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.uint1_avg_accum(bigint[], uint1)
 RETURNS bigint[] LANGUAGE C STRICT AS  '$libdir/dolphin',  'uint1_avg_accum';
 
DROP FUNCTION IF EXISTS pg_catalog.uint1_accum(numeric[], uint1) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.uint1_accum(numeric[], uint1)
 RETURNS numeric[] LANGUAGE C STRICT AS  '$libdir/dolphin',  'uint1_accum';
 
DROP FUNCTION IF EXISTS pg_catalog.uint1_sum(int8, uint1) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.uint1_sum(int8, uint1)
RETURNS int8 LANGUAGE C  AS  '$libdir/dolphin',  'uint1_sum';

DROP FUNCTION IF EXISTS pg_catalog.uint1_bool(uint1) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.uint1_bool(uint1)
RETURNS bool LANGUAGE C  AS  '$libdir/dolphin',  'uint1_bool';


DROP FUNCTION IF EXISTS pg_catalog.bool_uint1(bool) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.bool_uint1(bool)
RETURNS uint1 LANGUAGE C  AS  '$libdir/dolphin',  'bool_uint1';



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
 RETURNS internal LANGUAGE C  AS	'$libdir/dolphin',	'uint1_list_agg_transfn';

DROP FUNCTION IF EXISTS pg_catalog.uint1_list_agg_noarg2_transfn(internal, uint1) CASCADE;

CREATE OR REPLACE FUNCTION pg_catalog.uint1_list_agg_noarg2_transfn(internal, uint1)
 RETURNS internal LANGUAGE C  AS	'$libdir/dolphin',	'uint1_list_agg_noarg2_transfn';
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
commutator=operator(pg_catalog.<>),
restrict = neqsel, join = neqjoinsel
);
COMMENT ON OPERATOR pg_catalog.<>(uint1, uint1) IS 'uint1ne';

CREATE OPERATOR pg_catalog.<(
leftarg = uint1, rightarg = uint1, procedure = uint1lt,
commutator=operator(pg_catalog.<),
restrict = scalarltsel, join = scalarltjoinsel
);
COMMENT ON OPERATOR pg_catalog.<(uint1, uint1) IS 'uint1lt';


CREATE OPERATOR pg_catalog.<=(
leftarg = uint1, rightarg = uint1, procedure = uint1le,
commutator=operator(pg_catalog.<=),
restrict = scalarltsel, join = scalarltjoinsel
);
COMMENT ON OPERATOR pg_catalog.<=(uint1, uint1) IS 'uint1le';

CREATE OPERATOR pg_catalog.>(
leftarg = uint1, rightarg = uint1, procedure = uint1gt,
commutator=operator(pg_catalog.>),
restrict = scalargtsel, join = scalargtjoinsel
);
COMMENT ON OPERATOR pg_catalog.>(uint1, uint1) IS 'uint1gt';

CREATE OPERATOR pg_catalog.>=(
leftarg = uint1, rightarg = uint1, procedure = uint1ge,
commutator=operator(pg_catalog.>=),
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



create or replace function uint1(text)
returns uint1
as $$ select cast(to_number($1) as uint1)$$
language sql IMMUTABLE strict;
CREATE CAST (TEXT AS UINT1) WITH FUNCTION UINT1(TEXT) AS IMPLICIT;
DROP FUNCTION IF EXISTS pg_catalog.uint1_int1_mod(uint1,int1) CASCADE;
CREATE FUNCTION pg_catalog.uint1_int1_mod (
uint1,int1
) RETURNS float8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint1_int1_mod';
CREATE OPERATOR pg_catalog.%(
leftarg = uint1, rightarg = int1, procedure = uint1_int1_mod,
commutator=operator(pg_catalog.%)
);
COMMENT ON OPERATOR pg_catalog.%(uint1, int1) IS 'uint1_int1_mod';
DROP FUNCTION IF EXISTS pg_catalog.uint1_int2_mod(uint1,int2) CASCADE;
CREATE FUNCTION pg_catalog.uint1_int2_mod (
uint1,int2
) RETURNS uint1 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint1_int2_mod';
CREATE OPERATOR pg_catalog.%(
leftarg = uint1, rightarg = int2, procedure = uint1_int2_mod,
commutator=operator(pg_catalog.%)
);
COMMENT ON OPERATOR pg_catalog.%(uint1, int2) IS 'uint1_int2_mod';
DROP FUNCTION IF EXISTS pg_catalog.uint1_int4_mod(uint1,int4) CASCADE;
CREATE FUNCTION pg_catalog.uint1_int4_mod (
uint1,int4
) RETURNS float8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint1_int4_mod';
CREATE OPERATOR pg_catalog.%(
leftarg = uint1, rightarg = int4, procedure = uint1_int4_mod,
commutator=operator(pg_catalog.%)
);
COMMENT ON OPERATOR pg_catalog.%(uint1, int4) IS 'uint1_int4_mod';
DROP FUNCTION IF EXISTS pg_catalog.uint1_int8_mod(uint1,int8) CASCADE;
CREATE FUNCTION pg_catalog.uint1_int8_mod (
uint1,int8
) RETURNS float8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint1_int8_mod';
CREATE OPERATOR pg_catalog.%(
leftarg = uint1, rightarg = int8, procedure = uint1_int8_mod,
commutator=operator(pg_catalog.%)
);
COMMENT ON OPERATOR pg_catalog.%(uint1, int8) IS 'uint1_int8_mod';


DROP FUNCTION IF EXISTS pg_catalog.uint1_uint2_mod(uint1,uint1) CASCADE;
CREATE FUNCTION pg_catalog.uint1_uint2_mod (
uint1,uint2
) RETURNS float8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint1_uint2_mod';
CREATE OPERATOR pg_catalog.%(
leftarg = uint1, rightarg = uint2, procedure = uint1_uint2_mod,
commutator=operator(pg_catalog.%)
);
COMMENT ON OPERATOR pg_catalog.%(uint1, uint2) IS 'uint1_uint2_mod';

DROP FUNCTION IF EXISTS pg_catalog.uint1_uint4_mod(uint1,uint4) CASCADE;
CREATE FUNCTION pg_catalog.uint1_uint4_mod (
uint1,uint4
) RETURNS float8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint1_uint4_mod';
CREATE OPERATOR pg_catalog.%(
leftarg = uint1, rightarg = uint4, procedure = uint1_uint4_mod,
commutator=operator(pg_catalog.%)
);
COMMENT ON OPERATOR pg_catalog.%(uint1, uint4) IS 'uint1_uint4_mod';
DROP FUNCTION IF EXISTS pg_catalog.uint1_uint8_mod(uint1,uint1) CASCADE;
CREATE FUNCTION pg_catalog.uint1_uint8_mod (
uint1,uint8
) RETURNS float8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint1_uint8_mod';
CREATE OPERATOR pg_catalog.%(
leftarg = uint1, rightarg = uint8, procedure = uint1_uint8_mod,
commutator=operator(pg_catalog.%)
);
COMMENT ON OPERATOR pg_catalog.%(uint1, uint8) IS 'uint1_uint8_mod';
DROP FUNCTION IF EXISTS pg_catalog.int1_uint1_mod(int1,uint1) CASCADE;
CREATE FUNCTION pg_catalog.int1_uint1_mod (
int1,uint1
) RETURNS float8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int1_uint1_mod';
CREATE OPERATOR pg_catalog.%(
leftarg = int1, rightarg = uint1, procedure = int1_uint1_mod,
commutator=operator(pg_catalog.%)
);
COMMENT ON OPERATOR pg_catalog.%(int1, uint1) IS 'int1_uint1_mod';
DROP FUNCTION IF EXISTS pg_catalog.int2_uint1_mod(int2,uint1) CASCADE;
CREATE FUNCTION pg_catalog.int2_uint1_mod (
int2,uint1
) RETURNS float8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int2_uint1_mod';
CREATE OPERATOR pg_catalog.%(
leftarg = int2, rightarg = uint1, procedure = int2_uint1_mod,
commutator=operator(pg_catalog.%)
);
COMMENT ON OPERATOR pg_catalog.%(int2, uint1) IS 'int2_uint1_mod';
DROP FUNCTION IF EXISTS pg_catalog.int4_uint1_mod(int4,uint1) CASCADE;
CREATE FUNCTION pg_catalog.int4_uint1_mod (
int4,uint1
) RETURNS float8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int4_uint1_mod';
CREATE OPERATOR pg_catalog.%(
leftarg = int4, rightarg = uint1, procedure = int4_uint1_mod,
commutator=operator(pg_catalog.%)
);
COMMENT ON OPERATOR pg_catalog.%(int4, uint1) IS 'int4_uint1_mod';
DROP FUNCTION IF EXISTS pg_catalog.int8_uint1_mod(int8,uint1) CASCADE;
CREATE FUNCTION pg_catalog.int8_uint1_mod (
int8,uint1
) RETURNS float8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int8_uint1_mod';
CREATE OPERATOR pg_catalog.%(
leftarg = int8, rightarg = uint1, procedure = int8_uint1_mod,
commutator=operator(pg_catalog.%)
);
COMMENT ON OPERATOR pg_catalog.%(int8, uint1) IS 'int8_uint1_mod';
-------------------uint1 int 1---------------



DROP FUNCTION IF EXISTS pg_catalog.uint1_pl_int1(uint1,int1) CASCADE;
CREATE FUNCTION pg_catalog.uint1_pl_int1 (
uint1,int1
) RETURNS int2 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint1_pl_int1';

DROP FUNCTION IF EXISTS pg_catalog.uint1_mi_int1(uint1,int1) CASCADE;
CREATE FUNCTION pg_catalog.uint1_mi_int1 (
uint1,int1
) RETURNS int2 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint1_mi_int1';

DROP FUNCTION IF EXISTS pg_catalog.uint1_mul_int1(uint1,int1) CASCADE;
CREATE FUNCTION pg_catalog.uint1_mul_int1 (
uint1,int1
) RETURNS uint8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint1_mul_int1';


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
commutator=operator(pg_catalog.=)
);
COMMENT ON OPERATOR pg_catalog.=(uint1, int1) IS 'uint1_int1_eq';



CREATE OPERATOR pg_catalog.<>(
leftarg = uint1, rightarg = int1, procedure = uint1_int1_ne,
commutator=operator(pg_catalog.<>)
);
COMMENT ON OPERATOR pg_catalog.<>(uint1, int1) IS 'uint1_int1_ne';


CREATE OPERATOR pg_catalog.<(
leftarg = uint1, rightarg = int1, procedure = uint1_int1_lt,
commutator=operator(pg_catalog.<)
);
COMMENT ON OPERATOR pg_catalog.<(uint1, int1) IS 'uint1_int1_lt';


CREATE OPERATOR pg_catalog.<=(
leftarg = uint1, rightarg = int1, procedure = uint1_int1_le,
commutator=operator(pg_catalog.<=)
);
COMMENT ON OPERATOR pg_catalog.<=(uint1, int1) IS 'uint1_int1_le';

CREATE OPERATOR pg_catalog.>(
leftarg = uint1, rightarg = int1, procedure = uint1_int1_gt,
commutator=operator(pg_catalog.>)
);
COMMENT ON OPERATOR pg_catalog.>(uint1, int1) IS 'uint1_int1_gt';


CREATE OPERATOR pg_catalog.>=(
leftarg = uint1, rightarg = int1, procedure = uint1_int1_ge,
commutator=operator(pg_catalog.>=)
);
COMMENT ON OPERATOR pg_catalog.>=(uint1, int1) IS 'uint1_int1_ge';
-------------------uint1 int 1---------------



-------------------uint1 int2---------------
DROP FUNCTION IF EXISTS pg_catalog.uint1_pl_int2(uint1,int2) CASCADE;
CREATE FUNCTION pg_catalog.uint1_pl_int2 (
uint1,int2
) RETURNS int4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint1_pl_int2';

DROP FUNCTION IF EXISTS pg_catalog.uint1_mi_int2(uint1,int2) CASCADE;
CREATE FUNCTION pg_catalog.uint1_mi_int2 (
uint1,int2
) RETURNS int4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint1_mi_int2';

DROP FUNCTION IF EXISTS pg_catalog.uint1_mul_int2(uint1,int2) CASCADE;
CREATE FUNCTION pg_catalog.uint1_mul_int2 (
uint1,int2
) RETURNS int4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint1_mul_int2';


DROP FUNCTION IF EXISTS pg_catalog.uint1_div_int2(uint1,int2) CASCADE;
CREATE FUNCTION pg_catalog.uint1_div_int2 (
uint1,int2
) RETURNS uint8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint1_div_int2';


DROP FUNCTION IF EXISTS pg_catalog.uint1_int2_eq(uint1,int2) CASCADE;
CREATE FUNCTION pg_catalog.uint1_int2_eq (
uint1,int2
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint1_int2_eq';


DROP FUNCTION IF EXISTS pg_catalog.uint1_int2_ne(uint1,int2) CASCADE;
CREATE FUNCTION pg_catalog.uint1_int2_ne (
uint1,int2
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint1_int2_ne';


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


CREATE OPERATOR pg_catalog.+(
leftarg = uint1, rightarg = int2, procedure = uint1_pl_int2,
commutator=operator(pg_catalog.+)
);
COMMENT ON OPERATOR pg_catalog.+(uint1, int2) IS 'uint1_pl_int2';


CREATE OPERATOR pg_catalog.-(
leftarg = uint1, rightarg = int2, procedure = uint1_mi_int2,
commutator=operator(pg_catalog.-)
);
COMMENT ON OPERATOR pg_catalog.-(uint1, int2) IS 'uint1_mi_int2';


CREATE OPERATOR pg_catalog.*(
leftarg = uint1, rightarg = int2, procedure = uint1_mul_int2,
commutator=operator(pg_catalog.*)
);
COMMENT ON OPERATOR pg_catalog.*(uint1, int2) IS 'uint1_mul_int2';


CREATE OPERATOR pg_catalog./(
leftarg = uint1, rightarg = int2, procedure = uint1_div_int2,
commutator=operator(pg_catalog./)
);
COMMENT ON OPERATOR pg_catalog./(uint1, int2) IS 'uint1_div_int2';



CREATE OPERATOR pg_catalog.=(
leftarg = uint1, rightarg = int2, procedure = uint1_int2_eq,
commutator=operator(pg_catalog.=)
);
COMMENT ON OPERATOR pg_catalog.=(uint1, int2) IS 'uint1_int2_eq';



CREATE OPERATOR pg_catalog.<>(
leftarg = uint1, rightarg = int2, procedure = uint1_int2_ne,
commutator=operator(pg_catalog.<>)
);
COMMENT ON OPERATOR pg_catalog.<>(uint1, int2) IS 'uint1_int2_ne';

CREATE OPERATOR pg_catalog.<(
leftarg = uint1, rightarg = int2, procedure = uint1_int2_lt,
commutator=operator(pg_catalog.<)
);
COMMENT ON OPERATOR pg_catalog.<(uint1, int2) IS 'uint1_int2_lt';



CREATE OPERATOR pg_catalog.<=(
leftarg = uint1, rightarg = int2, procedure = uint1_int2_le,
commutator=operator(pg_catalog.<=)
);
COMMENT ON OPERATOR pg_catalog.<=(uint1, int2) IS 'uint1_int2_le';


CREATE OPERATOR pg_catalog.>(
leftarg = uint1, rightarg = int2, procedure = uint1_int2_gt,
commutator=operator(pg_catalog.>)
);
COMMENT ON OPERATOR pg_catalog.>(uint1, int2) IS 'uint1_int2_gt';

CREATE OPERATOR pg_catalog.>=(
leftarg = uint1, rightarg = int2, procedure = uint1_int2_ge,
commutator=operator(pg_catalog.>=)
);
COMMENT ON OPERATOR pg_catalog.>=(uint1, int2) IS 'uint1_int2_ge';

-------------------uint1 int2---------------



-------------------uint1 int4---------------
DROP FUNCTION IF EXISTS pg_catalog.uint1_pl_int4(uint1,int4) CASCADE;
CREATE FUNCTION pg_catalog.uint1_pl_int4 (
uint1,int4
) RETURNS int8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint1_pl_int4';

DROP FUNCTION IF EXISTS pg_catalog.uint1_mi_int4(uint1,int4) CASCADE;
CREATE FUNCTION pg_catalog.uint1_mi_int4 (
uint1,int4
) RETURNS int8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint1_mi_int4';
DROP FUNCTION IF EXISTS pg_catalog.uint1_mul_int4(uint1,int4) CASCADE;
CREATE FUNCTION pg_catalog.uint1_mul_int4 (
uint1,int4
) RETURNS uint8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint1_mul_int4';


DROP FUNCTION IF EXISTS pg_catalog.uint1_div_int4(uint1,int4) CASCADE;
CREATE FUNCTION pg_catalog.uint1_div_int4 (
uint1,int4
) RETURNS float8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint1_div_int4';


DROP FUNCTION IF EXISTS pg_catalog.uint1_int4_eq(uint1,int4) CASCADE;
CREATE FUNCTION pg_catalog.uint1_int4_eq (
uint1,int4
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint1_int4_eq';


DROP FUNCTION IF EXISTS pg_catalog.uint1_int4_ne(uint1,int4) CASCADE;
CREATE FUNCTION pg_catalog.uint1_int4_ne (
uint1,int4
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint1_int4_ne';


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


CREATE OPERATOR pg_catalog.+(
leftarg = uint1, rightarg = int4, procedure = uint1_pl_int4,
commutator=operator(pg_catalog.+)
);
COMMENT ON OPERATOR pg_catalog.+(uint1, int4) IS 'uint1_pl_int4';

CREATE OPERATOR pg_catalog.-(
leftarg = uint1, rightarg = int4, procedure = uint1_mi_int4,
commutator=operator(pg_catalog.-)
);
COMMENT ON OPERATOR pg_catalog.-(uint1, int4) IS 'uint1_mi_int4';

CREATE OPERATOR pg_catalog.*(
leftarg = uint1, rightarg = int4, procedure = uint1_mul_int4,
commutator=operator(pg_catalog.*)
);
COMMENT ON OPERATOR pg_catalog.*(uint1, int4) IS 'uint1_mul_int4';


CREATE OPERATOR pg_catalog./(
leftarg = uint1, rightarg = int4, procedure = uint1_div_int4,
commutator=operator(pg_catalog./)
);
COMMENT ON OPERATOR pg_catalog./(uint1, int4) IS 'uint1_div_int4';



CREATE OPERATOR pg_catalog.=(
leftarg = uint1, rightarg = int4, procedure = uint1_int4_eq,
commutator=operator(pg_catalog.=)
);
COMMENT ON OPERATOR pg_catalog.=(uint1, int4) IS 'uint1_int4_eq';


CREATE OPERATOR pg_catalog.<>(
leftarg = uint1, rightarg = int4, procedure = uint1_int4_ne,
commutator=operator(pg_catalog.<>)
);
COMMENT ON OPERATOR pg_catalog.<>(uint1, int4) IS 'uint1_int4_ne';



CREATE OPERATOR pg_catalog.<(
leftarg = uint1, rightarg = int4, procedure = uint1_int4_lt,
commutator=operator(pg_catalog.<)
);
COMMENT ON OPERATOR pg_catalog.<(uint1, int4) IS 'uint1_int4_lt';


CREATE OPERATOR pg_catalog.<=(
leftarg = uint1, rightarg = int4, procedure = uint1_int4_le,
commutator=operator(pg_catalog.<=)
);
COMMENT ON OPERATOR pg_catalog.<=(uint1, int4) IS 'uint1_int4_le';

CREATE OPERATOR pg_catalog.>(
leftarg = uint1, rightarg = int4, procedure = uint1_int4_gt,
commutator=operator(pg_catalog.>)
);
COMMENT ON OPERATOR pg_catalog.>(uint1, int4) IS 'uint1_int4_gt';


CREATE OPERATOR pg_catalog.>=(
leftarg = uint1, rightarg = int4, procedure = uint1_int4_ge,
commutator=operator(pg_catalog.>=)
);
COMMENT ON OPERATOR pg_catalog.>=(uint1, int4) IS 'uint1_int4_ge';

-------------------uint1 int4---------------


-------------------uint1 int8---------------

DROP FUNCTION IF EXISTS pg_catalog.uint1_pl_int8(uint1,int8) CASCADE;
CREATE FUNCTION pg_catalog.uint1_pl_int8 (
uint1,int8
) RETURNS int16 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint1_pl_int8';

DROP FUNCTION IF EXISTS pg_catalog.uint1_mi_int8(uint1,int8) CASCADE;
CREATE FUNCTION pg_catalog.uint1_mi_int8 (
uint1,int8
) RETURNS int16 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint1_mi_int8';

DROP FUNCTION IF EXISTS pg_catalog.uint1_mul_int8(uint1,int8) CASCADE;
CREATE FUNCTION pg_catalog.uint1_mul_int8 (
uint1,int8
) RETURNS int16 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint1_mul_int8';


DROP FUNCTION IF EXISTS pg_catalog.uint1_div_int8(uint1,int8) CASCADE;
CREATE FUNCTION pg_catalog.uint1_div_int8 (
uint1,int8
) RETURNS float8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint1_div_int8';


DROP FUNCTION IF EXISTS pg_catalog.uint1_int8_eq(uint1,int8) CASCADE;
CREATE FUNCTION pg_catalog.uint1_int8_eq (
uint1,int8
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint1_int8_eq';


DROP FUNCTION IF EXISTS pg_catalog.uint1_int8_ne(uint1,int8) CASCADE;
CREATE FUNCTION pg_catalog.uint1_int8_ne (
uint1,int8
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint1_int8_ne';


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


CREATE OPERATOR pg_catalog.+(
leftarg = uint1, rightarg = int8, procedure = uint1_pl_int8,
commutator=operator(pg_catalog.+)
);
COMMENT ON OPERATOR pg_catalog.+(uint1, int8) IS 'uint1_pl_int8';

CREATE OPERATOR pg_catalog.-(
leftarg = uint1, rightarg = int8, procedure = uint1_mi_int8,
commutator=operator(pg_catalog.-)
);
COMMENT ON OPERATOR pg_catalog.-(uint1, int8) IS 'uint1_mi_int8';

CREATE OPERATOR pg_catalog.*(
leftarg = uint1, rightarg = int8, procedure = uint1_mul_int8,
commutator=operator(pg_catalog.*)
);
COMMENT ON OPERATOR pg_catalog.*(uint1, int8) IS 'uint1_mul_int8';

CREATE OPERATOR pg_catalog./(
leftarg = uint1, rightarg = int8, procedure = uint1_div_int8,
commutator=operator(pg_catalog./)
);
COMMENT ON OPERATOR pg_catalog./(uint1, int8) IS 'uint1_div_int8';


CREATE OPERATOR pg_catalog.=(
leftarg = uint1, rightarg = int8, procedure = uint1_int8_eq,
commutator=operator(pg_catalog.=)
);
COMMENT ON OPERATOR pg_catalog.=(uint1, int8) IS 'uint1_int8_eq';


CREATE OPERATOR pg_catalog.<>(
leftarg = uint1, rightarg = int8, procedure = uint1_int8_ne,
commutator=operator(pg_catalog.<>)
);
COMMENT ON OPERATOR pg_catalog.<>(uint1, int8) IS 'uint1_int8_ne';


CREATE OPERATOR pg_catalog.<(
leftarg = uint1, rightarg = int8, procedure = uint1_int8_lt,
commutator=operator(pg_catalog.<)
);
COMMENT ON OPERATOR pg_catalog.<(uint1, int8) IS 'uint1_int8_lt';

CREATE OPERATOR pg_catalog.<=(
leftarg = uint1, rightarg = int8, procedure = uint1_int8_le,
commutator=operator(pg_catalog.<=)
);
COMMENT ON OPERATOR pg_catalog.<=(uint1, int8) IS 'uint1_int8_le';

CREATE OPERATOR pg_catalog.>(
leftarg = uint1, rightarg = int8, procedure = uint1_int8_gt,
commutator=operator(pg_catalog.>)
);
COMMENT ON OPERATOR pg_catalog.>(uint1, int8) IS 'uint1_int8_gt';


CREATE OPERATOR pg_catalog.>=(
leftarg = uint1, rightarg = int8, procedure = uint1_int8_ge,
commutator=operator(pg_catalog.>=)
);
COMMENT ON OPERATOR pg_catalog.>=(uint1, int8) IS 'uint1_int8_ge';


-------------------uint1 int8---------------
	
-------------------uint1 uint2---------------

DROP FUNCTION IF EXISTS pg_catalog.uint1_pl_uint2(uint1,uint2) CASCADE;
CREATE FUNCTION pg_catalog.uint1_pl_uint2 (
uint1,uint2
) RETURNS uint4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint1_pl_uint2';

DROP FUNCTION IF EXISTS pg_catalog.uint1_mi_uint2(uint1,uint2) CASCADE;
CREATE FUNCTION pg_catalog.uint1_mi_uint2 (
uint1,uint2
) RETURNS int4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint1_mi_uint2';
DROP FUNCTION IF EXISTS pg_catalog.uint1_mul_uint2(uint1,uint2) CASCADE;
CREATE FUNCTION pg_catalog.uint1_mul_uint2 (
uint1,uint2
) RETURNS uint4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint1_mul_uint2';


DROP FUNCTION IF EXISTS pg_catalog.uint1_div_uint2(uint1,uint2) CASCADE;
CREATE FUNCTION pg_catalog.uint1_div_uint2 (
uint1,uint2
) RETURNS float8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint1_div_uint2';


DROP FUNCTION IF EXISTS pg_catalog.uint1_uint2_eq(uint1,uint2) CASCADE;
CREATE FUNCTION pg_catalog.uint1_uint2_eq (
uint1,uint2
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint1_uint2_eq';


DROP FUNCTION IF EXISTS pg_catalog.uint1_uint2_ne(uint1,uint2) CASCADE;
CREATE FUNCTION pg_catalog.uint1_uint2_ne (
uint1,uint2
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint1_uint2_ne';


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


CREATE OPERATOR pg_catalog.+(
leftarg = uint1, rightarg = uint2, procedure = uint1_pl_uint2,
commutator=operator(pg_catalog.+)
);
COMMENT ON OPERATOR pg_catalog.+(uint1, uint2) IS 'uint1_pl_uint2';

CREATE OPERATOR pg_catalog.-(
leftarg = uint1, rightarg = uint2, procedure = uint1_mi_uint2,
commutator=operator(pg_catalog.-)
);
COMMENT ON OPERATOR pg_catalog.-(uint1, uint2) IS 'uint1_mi_uint2';

CREATE OPERATOR pg_catalog.*(
leftarg = uint1, rightarg = uint2, procedure = uint1_mul_uint2,
commutator=operator(pg_catalog.*)
);
COMMENT ON OPERATOR pg_catalog.*(uint1, uint2) IS 'uint1_mul_uint2';


CREATE OPERATOR pg_catalog./(
leftarg = uint1, rightarg = uint2, procedure = uint1_div_uint2,
commutator=operator(pg_catalog./)
);
COMMENT ON OPERATOR pg_catalog./(uint1, uint2) IS 'uint1_div_uint2';


CREATE OPERATOR pg_catalog.=(
leftarg = uint1, rightarg = uint2, procedure = uint1_uint2_eq,
commutator=operator(pg_catalog.=)
);
COMMENT ON OPERATOR pg_catalog.=(uint1, uint2) IS 'uint1_uint2_eq';


CREATE OPERATOR pg_catalog.<>(
leftarg = uint1, rightarg = uint2, procedure = uint1_uint2_ne,
commutator=operator(pg_catalog.<>)
);
COMMENT ON OPERATOR pg_catalog.<>(uint1, uint2) IS 'uint1_uint2_ne';


CREATE OPERATOR pg_catalog.<(
leftarg = uint1, rightarg = uint2, procedure = uint1_uint2_lt,
commutator=operator(pg_catalog.<)
);
COMMENT ON OPERATOR pg_catalog.<(uint1, uint2) IS 'uint1_uint2_lt';


CREATE OPERATOR pg_catalog.<=(
leftarg = uint1, rightarg = uint2, procedure = uint1_uint2_le,
commutator=operator(pg_catalog.<=)
);
COMMENT ON OPERATOR pg_catalog.<=(uint1, uint2) IS 'uint1_uint2_le';


CREATE OPERATOR pg_catalog.>(
leftarg = uint1, rightarg = uint2, procedure = uint1_uint2_gt,
commutator=operator(pg_catalog.>)
);
COMMENT ON OPERATOR pg_catalog.>(uint1, uint2) IS 'uint1_uint2_gt';


CREATE OPERATOR pg_catalog.>=(
leftarg = uint1, rightarg = uint2, procedure = uint1_uint2_ge,
commutator=operator(pg_catalog.>=)
);
COMMENT ON OPERATOR pg_catalog.>=(uint1, uint2) IS 'uint1_uint2_ge';


-------------------uint1 uint2---------------



-------------------uint1 uint4---------------
DROP FUNCTION IF EXISTS pg_catalog.uint1_pl_uint4(uint1,uint4) CASCADE;
CREATE FUNCTION pg_catalog.uint1_pl_uint4 (
uint1,uint4
) RETURNS uint8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint1_pl_uint4';

DROP FUNCTION IF EXISTS pg_catalog.uint1_mi_uint4(uint1,uint4) CASCADE;
CREATE FUNCTION pg_catalog.uint1_mi_uint4 (
uint1,uint4
) RETURNS int8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint1_mi_uint4';
DROP FUNCTION IF EXISTS pg_catalog.uint1_mul_uint4(uint1,uint4) CASCADE;
CREATE FUNCTION pg_catalog.uint1_mul_uint4 (
uint1,uint4
) RETURNS uint8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint1_mul_uint4';


DROP FUNCTION IF EXISTS pg_catalog.uint1_div_uint4(uint1,uint4) CASCADE;
CREATE FUNCTION pg_catalog.uint1_div_uint4 (
uint1,uint4
) RETURNS float8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint1_div_uint4';


DROP FUNCTION IF EXISTS pg_catalog.uint1_uint4_eq(uint1,uint4) CASCADE;
CREATE FUNCTION pg_catalog.uint1_uint4_eq (
uint1,uint4
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint1_uint4_eq';


DROP FUNCTION IF EXISTS pg_catalog.uint1_uint4_ne(uint1,uint4) CASCADE;
CREATE FUNCTION pg_catalog.uint1_uint4_ne (
uint1,uint4
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint1_uint4_ne';


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

CREATE OPERATOR pg_catalog.+(
leftarg = uint1, rightarg = uint4, procedure = uint1_pl_uint4,
commutator=operator(pg_catalog.+)
);
COMMENT ON OPERATOR pg_catalog.+(uint1, uint4) IS 'uint1_pl_uint4';


CREATE OPERATOR pg_catalog.-(
leftarg = uint1, rightarg = uint4, procedure = uint1_mi_uint4,
commutator=operator(pg_catalog.-)
);
COMMENT ON OPERATOR pg_catalog.-(uint1, uint4) IS 'uint1_mi_uint4';


CREATE OPERATOR pg_catalog.*(
leftarg = uint1, rightarg = uint4, procedure = uint1_mul_uint4,
commutator=operator(pg_catalog.*)
);
COMMENT ON OPERATOR pg_catalog.*(uint1, uint4) IS 'uint1_mul_uint4';


CREATE OPERATOR pg_catalog./(
leftarg = uint1, rightarg = uint4, procedure = uint1_div_uint4,
commutator=operator(pg_catalog./)
);
COMMENT ON OPERATOR pg_catalog./(uint1, uint4) IS 'uint1_div_uint4';


CREATE OPERATOR pg_catalog.=(
leftarg = uint1, rightarg = uint4, procedure = uint1_uint4_eq,
commutator=operator(pg_catalog.=)
);
COMMENT ON OPERATOR pg_catalog.=(uint1, uint4) IS 'uint1_uint4_eq';


CREATE OPERATOR pg_catalog.<>(
leftarg = uint1, rightarg = uint4, procedure = uint1_uint4_ne,
commutator=operator(pg_catalog.<>)
);
COMMENT ON OPERATOR pg_catalog.<>(uint1, uint4) IS 'uint1_uint4_ne';


CREATE OPERATOR pg_catalog.<(
leftarg = uint1, rightarg = uint4, procedure = uint1_uint4_lt,
commutator=operator(pg_catalog.<)
);
COMMENT ON OPERATOR pg_catalog.<(uint1, uint4) IS 'uint1_uint4_lt';


CREATE OPERATOR pg_catalog.<=(
leftarg = uint1, rightarg = uint4, procedure = uint1_uint4_le,
commutator=operator(pg_catalog.<=)
);
COMMENT ON OPERATOR pg_catalog.<=(uint1, uint4) IS 'uint1_uint4_le';


CREATE OPERATOR pg_catalog.>(
leftarg = uint1, rightarg = uint4, procedure = uint1_uint4_gt,
commutator=operator(pg_catalog.>)
);
COMMENT ON OPERATOR pg_catalog.>(uint1, uint4) IS 'uint1_uint4_gt';


CREATE OPERATOR pg_catalog.>=(
leftarg = uint1, rightarg = uint4, procedure = uint1_uint4_ge,
commutator=operator(pg_catalog.>=)
);
COMMENT ON OPERATOR pg_catalog.>=(uint1, uint4) IS 'uint1_uint4_ge';




-------------------uint1 uint4---------------


-------------------uint1 uint8---------------

DROP FUNCTION IF EXISTS pg_catalog.uint1_pl_uint8(uint1,uint8) CASCADE;
CREATE FUNCTION pg_catalog.uint1_pl_uint8 (
uint1,uint8
) RETURNS int16 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint1_pl_uint8';

DROP FUNCTION IF EXISTS pg_catalog.uint1_mi_uint8(uint1,uint8) CASCADE;
CREATE FUNCTION pg_catalog.uint1_mi_uint8 (
uint1,uint8
) RETURNS int16 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint1_mi_uint8';
DROP FUNCTION IF EXISTS pg_catalog.uint1_mul_uint8(uint1,uint8) CASCADE;
CREATE FUNCTION pg_catalog.uint1_mul_uint8 (
uint1,uint8
) RETURNS int16 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint1_mul_uint8';


DROP FUNCTION IF EXISTS pg_catalog.uint1_div_uint8(uint1,uint8) CASCADE;
CREATE FUNCTION pg_catalog.uint1_div_uint8 (
uint1,uint8
) RETURNS float8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint1_div_uint8';


DROP FUNCTION IF EXISTS pg_catalog.uint1_uint8_eq(uint1,uint8) CASCADE;
CREATE FUNCTION pg_catalog.uint1_uint8_eq (
uint1,uint8
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint1_uint8_eq';


DROP FUNCTION IF EXISTS pg_catalog.uint1_uint8_ne(uint1,uint8) CASCADE;
CREATE FUNCTION pg_catalog.uint1_uint8_ne (
uint1,uint8
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint1_uint8_ne';


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

CREATE OPERATOR pg_catalog.+(
leftarg = uint1, rightarg = uint8, procedure = uint1_pl_uint8,
commutator=operator(pg_catalog.+)
);
COMMENT ON OPERATOR pg_catalog.+(uint1, uint8) IS 'uint1_pl_uint8';


CREATE OPERATOR pg_catalog.-(
leftarg = uint1, rightarg = uint8, procedure = uint1_mi_uint8,
commutator=operator(pg_catalog.-)
);
COMMENT ON OPERATOR pg_catalog.-(uint1, uint8) IS 'uint1_mi_uint8';


CREATE OPERATOR pg_catalog.*(
leftarg = uint1, rightarg = uint8, procedure = uint1_mul_uint8,
commutator=operator(pg_catalog.*)
);
COMMENT ON OPERATOR pg_catalog.*(uint1, uint8) IS 'uint1_mul_uint8';


CREATE OPERATOR pg_catalog./(
leftarg = uint1, rightarg = uint8, procedure = uint1_div_uint8,
commutator=operator(pg_catalog./)
);
COMMENT ON OPERATOR pg_catalog./(uint1, uint8) IS 'uint1_div_uint8';



CREATE OPERATOR pg_catalog.=(
leftarg = uint1, rightarg = uint8, procedure = uint1_uint8_eq,
commutator=operator(pg_catalog.=)
);
COMMENT ON OPERATOR pg_catalog.=(uint1, uint8) IS 'uint1_uint8_eq';



CREATE OPERATOR pg_catalog.<>(
leftarg = uint1, rightarg = uint8, procedure = uint1_uint8_ne,
commutator=operator(pg_catalog.<>)
);
COMMENT ON OPERATOR pg_catalog.<>(uint1, uint8) IS 'uint1_uint8_ne';




CREATE OPERATOR pg_catalog.<(
leftarg = uint1, rightarg = uint8, procedure = uint1_uint8_lt,
commutator=operator(pg_catalog.<)
);
COMMENT ON OPERATOR pg_catalog.<(uint1, uint8) IS 'uint1_uint8_lt';



CREATE OPERATOR pg_catalog.<=(
leftarg = uint1, rightarg = uint8, procedure = uint1_uint8_le,
commutator=operator(pg_catalog.<=)
);
COMMENT ON OPERATOR pg_catalog.<=(uint1, uint8) IS 'uint1_uint8_le';


CREATE OPERATOR pg_catalog.>(
leftarg = uint1, rightarg = uint8, procedure = uint1_uint8_gt,
commutator=operator(pg_catalog.>)
);
COMMENT ON OPERATOR pg_catalog.>(uint1, uint8) IS 'uint1_uint8_gt';



CREATE OPERATOR pg_catalog.>=(
leftarg = uint1, rightarg = uint8, procedure = uint1_uint8_ge,
commutator=operator(pg_catalog.>=)
);
COMMENT ON OPERATOR pg_catalog.>=(uint1, uint8) IS 'uint1_uint8_ge';


-------------------uint1 uint8---------------





-------------------int1 uint1---------------

DROP FUNCTION IF EXISTS pg_catalog.int1_pl_uint1(int1,uint1) CASCADE;
CREATE FUNCTION pg_catalog.int1_pl_uint1 (
int1,uint1
) RETURNS int2 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int1_pl_uint1';

DROP FUNCTION IF EXISTS pg_catalog.int1_mi_uint1(int1,uint1) CASCADE;
CREATE FUNCTION pg_catalog.int1_mi_uint1 (
int1,uint1
) RETURNS int2 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int1_mi_uint1';
DROP FUNCTION IF EXISTS pg_catalog.int1_mul_uint1(int1,uint1) CASCADE;
CREATE FUNCTION pg_catalog.int1_mul_uint1 (
int1,uint1
) RETURNS int2 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int1_mul_uint1';


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
commutator=operator(pg_catalog.=)
);
COMMENT ON OPERATOR pg_catalog.=(int1, uint1) IS 'int1_uint1_eq';



CREATE OPERATOR pg_catalog.<>(
leftarg = int1, rightarg = uint1, procedure = int1_uint1_ne,
commutator=operator(pg_catalog.<>)
);
COMMENT ON OPERATOR pg_catalog.<>(int1, uint1) IS 'int1_uint1_ne';




CREATE OPERATOR pg_catalog.<(
leftarg = int1, rightarg = uint1, procedure = int1_uint1_lt,
commutator=operator(pg_catalog.<)
);
COMMENT ON OPERATOR pg_catalog.<(int1, uint1) IS 'int1_uint1_lt';



CREATE OPERATOR pg_catalog.<=(
leftarg = int1, rightarg = uint1, procedure = int1_uint1_le,
commutator=operator(pg_catalog.<=)
);
COMMENT ON OPERATOR pg_catalog.<=(int1, uint1) IS 'int1_uint1_le';


CREATE OPERATOR pg_catalog.>(
leftarg = int1, rightarg = uint1, procedure = int1_uint1_gt,
commutator=operator(pg_catalog.>)
);
COMMENT ON OPERATOR pg_catalog.>=(int1, uint1) IS 'int1_uint1_gt';



CREATE OPERATOR pg_catalog.>=(
leftarg = int1, rightarg = uint1, procedure = int1_uint1_ge,
commutator=operator(pg_catalog.>=)
);
COMMENT ON OPERATOR pg_catalog.>=(int1, uint1) IS 'int1_uint1_ge';

-------------------int1 uint1---------------


-------------------int2 uint1---------------



DROP FUNCTION IF EXISTS pg_catalog.int2_pl_uint1(int2,uint1) CASCADE;
CREATE FUNCTION pg_catalog.int2_pl_uint1 (
int2,uint1
) RETURNS int4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int2_pl_uint1';

DROP FUNCTION IF EXISTS pg_catalog.int2_mi_uint1(int2,uint1) CASCADE;
CREATE FUNCTION pg_catalog.int2_mi_uint1 (
int2,uint1
) RETURNS int4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int2_mi_uint1';
DROP FUNCTION IF EXISTS pg_catalog.int2_mul_uint1(int2,uint1) CASCADE;
CREATE FUNCTION pg_catalog.int2_mul_uint1 (
int2,uint1
) RETURNS int4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int2_mul_uint1';


DROP FUNCTION IF EXISTS pg_catalog.int2_div_uint1(int2,uint1) CASCADE;
CREATE FUNCTION pg_catalog.int2_div_uint1 (
int2,uint1
) RETURNS float8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int2_div_uint1';


DROP FUNCTION IF EXISTS pg_catalog.int2_uint1_eq(int2,uint1) CASCADE;
CREATE FUNCTION pg_catalog.int2_uint1_eq (
int2,uint1
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int2_uint1_eq';


DROP FUNCTION IF EXISTS pg_catalog.int2_uint1_ne(int2,uint1) CASCADE;
CREATE FUNCTION pg_catalog.int2_uint1_ne (
int2,uint1
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int2_uint1_ne';


DROP FUNCTION IF EXISTS pg_catalog.int2_uint1_lt(int2,uint1) CASCADE;
CREATE FUNCTION pg_catalog.int2_uint1_lt (
int2,uint1
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int2_uint1_lt';


DROP FUNCTION IF EXISTS pg_catalog.int2_uint1_le(int2,uint1) CASCADE;
CREATE FUNCTION pg_catalog.int2_uint1_le (
int2,uint1
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int2_uint1_le';


DROP FUNCTION IF EXISTS pg_catalog.int2_uint1_gt(int2,uint1) CASCADE;
CREATE FUNCTION pg_catalog.int2_uint1_gt (
int2,uint1
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int2_uint1_gt';


DROP FUNCTION IF EXISTS pg_catalog.int2_uint1_ge(int2,uint1) CASCADE;
CREATE FUNCTION pg_catalog.int2_uint1_ge (
int2,uint1
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int2_uint1_ge';


CREATE OPERATOR pg_catalog.+(
leftarg = int2, rightarg = uint1, procedure = int2_pl_uint1,
commutator=operator(pg_catalog.+)
);
COMMENT ON OPERATOR pg_catalog.+(int2, uint1) IS 'int2_pl_uint1';


CREATE OPERATOR pg_catalog.-(
leftarg = int2, rightarg = uint1, procedure = int2_mi_uint1,
commutator=operator(pg_catalog.-)
);
COMMENT ON OPERATOR pg_catalog.-(int2, uint1) IS 'int2_mi_uint1';


CREATE OPERATOR pg_catalog.*(
leftarg = int2, rightarg = uint1, procedure = int2_mul_uint1,
commutator=operator(pg_catalog.*)
);
COMMENT ON OPERATOR pg_catalog.*(int2, uint1) IS 'int2_mul_uint1';


CREATE OPERATOR pg_catalog./(
leftarg = int2, rightarg = uint1, procedure = int2_div_uint1,
commutator=operator(pg_catalog./)
);
COMMENT ON OPERATOR pg_catalog./(int2, uint1) IS 'int2_div_uint1';



CREATE OPERATOR pg_catalog.=(
leftarg = int2, rightarg = uint1, procedure = int2_uint1_eq,
commutator=operator(pg_catalog.=)
);
COMMENT ON OPERATOR pg_catalog.=(int2, uint1) IS 'int2_uint1_eq';



CREATE OPERATOR pg_catalog.<>(
leftarg = int2, rightarg = uint1, procedure = int2_uint1_ne,
commutator=operator(pg_catalog.<>)
);
COMMENT ON OPERATOR pg_catalog.<>(int2, uint1) IS 'int2_uint1_ne';




CREATE OPERATOR pg_catalog.<(
leftarg = int2, rightarg = uint1, procedure = int2_uint1_lt,
commutator=operator(pg_catalog.<)
);
COMMENT ON OPERATOR pg_catalog.<(int2, uint1) IS 'int2_uint1_lt';



CREATE OPERATOR pg_catalog.<=(
leftarg = int2, rightarg = uint1, procedure = int2_uint1_le,
commutator=operator(pg_catalog.<=)
);
COMMENT ON OPERATOR pg_catalog.<=(int2, uint1) IS 'int2_uint1_le';


CREATE OPERATOR pg_catalog.>(
leftarg = int2, rightarg = uint1, procedure = int2_uint1_gt,
commutator=operator(pg_catalog.>)
);
COMMENT ON OPERATOR pg_catalog.>=(int2, uint1) IS 'int2_uint1_gt';



CREATE OPERATOR pg_catalog.>=(
leftarg = int2, rightarg = uint1, procedure = int2_uint1_ge,
commutator=operator(pg_catalog.>=)
);
COMMENT ON OPERATOR pg_catalog.>=(int2, uint1) IS 'int2_uint1_ge';


-------------------int2 uint1---------------

-------------------int4 uint1---------------
DROP FUNCTION IF EXISTS pg_catalog.int4_pl_uint1(int4,uint1) CASCADE;
CREATE FUNCTION pg_catalog.int4_pl_uint1 (
int4,uint1
) RETURNS int8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int4_pl_uint1';

DROP FUNCTION IF EXISTS pg_catalog.int4_mi_uint1(int4,uint1) CASCADE;
CREATE FUNCTION pg_catalog.int4_mi_uint1 (
int4,uint1
) RETURNS int8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int4_mi_uint1';
DROP FUNCTION IF EXISTS pg_catalog.int4_mul_uint1(int4,uint1) CASCADE;
CREATE FUNCTION pg_catalog.int4_mul_uint1 (
int4,uint1
) RETURNS int8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int4_mul_uint1';


DROP FUNCTION IF EXISTS pg_catalog.int4_div_uint1(int4,uint1) CASCADE;
CREATE FUNCTION pg_catalog.int4_div_uint1 (
int4,uint1
) RETURNS float8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int4_div_uint1';


DROP FUNCTION IF EXISTS pg_catalog.int4_uint1_eq(int4,uint1) CASCADE;
CREATE FUNCTION pg_catalog.int4_uint1_eq (
int4,uint1
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int4_uint1_eq';


DROP FUNCTION IF EXISTS pg_catalog.int4_uint1_ne(int4,uint1) CASCADE;
CREATE FUNCTION pg_catalog.int4_uint1_ne (
int4,uint1
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int4_uint1_ne';


DROP FUNCTION IF EXISTS pg_catalog.int4_uint1_lt(int4,uint1) CASCADE;
CREATE FUNCTION pg_catalog.int4_uint1_lt (
int4,uint1
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int4_uint1_lt';


DROP FUNCTION IF EXISTS pg_catalog.int4_uint1_le(int4,uint1) CASCADE;
CREATE FUNCTION pg_catalog.int4_uint1_le (
int4,uint1
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int4_uint1_le';


DROP FUNCTION IF EXISTS pg_catalog.int4_uint1_gt(int4,uint1) CASCADE;
CREATE FUNCTION pg_catalog.int4_uint1_gt (
int4,uint1
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int4_uint1_gt';


DROP FUNCTION IF EXISTS pg_catalog.int4_uint1_ge(int4,uint1) CASCADE;
CREATE FUNCTION pg_catalog.int4_uint1_ge (
int4,uint1
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int4_uint1_ge';

CREATE OPERATOR pg_catalog.+(
leftarg = int4, rightarg = uint1, procedure = int4_pl_uint1,
commutator=operator(pg_catalog.+)
);
COMMENT ON OPERATOR pg_catalog.+(int4, uint1) IS 'int4_pl_uint1';


CREATE OPERATOR pg_catalog.-(
leftarg = int4, rightarg = uint1, procedure = int4_mi_uint1,
commutator=operator(pg_catalog.-)
);
COMMENT ON OPERATOR pg_catalog.-(int4, uint1) IS 'int4_mi_uint1';


CREATE OPERATOR pg_catalog.*(
leftarg = int4, rightarg = uint1, procedure = int4_mul_uint1,
commutator=operator(pg_catalog.*)
);
COMMENT ON OPERATOR pg_catalog.*(int4, uint1) IS 'int4_mul_uint1';


CREATE OPERATOR pg_catalog./(
leftarg = int4, rightarg = uint1, procedure = int4_div_uint1,
commutator=operator(pg_catalog./)
);
COMMENT ON OPERATOR pg_catalog./(int4, uint1) IS 'int4_div_uint1';



CREATE OPERATOR pg_catalog.=(
leftarg = int4, rightarg = uint1, procedure = int4_uint1_eq,
commutator=operator(pg_catalog.=)
);
COMMENT ON OPERATOR pg_catalog.=(int4, uint1) IS 'int4_uint1_eq';



CREATE OPERATOR pg_catalog.<>(
leftarg = int4, rightarg = uint1, procedure = int4_uint1_ne,
commutator=operator(pg_catalog.<>)
);
COMMENT ON OPERATOR pg_catalog.<>(int4, uint1) IS 'int4_uint1_ne';




CREATE OPERATOR pg_catalog.<(
leftarg = int4, rightarg = uint1, procedure = int4_uint1_lt,
commutator=operator(pg_catalog.<)
);
COMMENT ON OPERATOR pg_catalog.<(int4, uint1) IS 'int4_uint1_lt';



CREATE OPERATOR pg_catalog.<=(
leftarg = int4, rightarg = uint1, procedure = int4_uint1_le,
commutator=operator(pg_catalog.<=)
);
COMMENT ON OPERATOR pg_catalog.<=(int4, uint1) IS 'int4_uint1_le';


CREATE OPERATOR pg_catalog.>(
leftarg = int4, rightarg = uint1, procedure = int4_uint1_gt,
commutator=operator(pg_catalog.>)
);
COMMENT ON OPERATOR pg_catalog.>=(int4, uint1) IS 'int4_uint1_gt';



CREATE OPERATOR pg_catalog.>=(
leftarg = int4, rightarg = uint1, procedure = int4_uint1_ge,
commutator=operator(pg_catalog.>=)
);
COMMENT ON OPERATOR pg_catalog.>=(int4, uint1) IS 'int4_uint1_ge';

-------------------int4 uint1---------------

-------------------int8 uint1---------------
DROP FUNCTION IF EXISTS pg_catalog.int8_pl_uint1(int8,uint1) CASCADE;
CREATE FUNCTION pg_catalog.int8_pl_uint1 (
int8,uint1
) RETURNS int16 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int8_pl_uint1';

DROP FUNCTION IF EXISTS pg_catalog.int8_mi_uint1(int8,uint1) CASCADE;
CREATE FUNCTION pg_catalog.int8_mi_uint1 (
int8,uint1
) RETURNS int16 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int8_mi_uint1';
DROP FUNCTION IF EXISTS pg_catalog.int8_mul_uint1(int8,uint1) CASCADE;
CREATE FUNCTION pg_catalog.int8_mul_uint1 (
int8,uint1
) RETURNS int16 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int8_mul_uint1';


DROP FUNCTION IF EXISTS pg_catalog.int8_div_uint1(int8,uint1) CASCADE;
CREATE FUNCTION pg_catalog.int8_div_uint1 (
int8,uint1
) RETURNS float8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int8_div_uint1';


DROP FUNCTION IF EXISTS pg_catalog.int8_uint1_eq(int8,uint1) CASCADE;
CREATE FUNCTION pg_catalog.int8_uint1_eq (
int8,uint1
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int8_uint1_eq';


DROP FUNCTION IF EXISTS pg_catalog.int8_uint1_ne(int8,uint1) CASCADE;
CREATE FUNCTION pg_catalog.int8_uint1_ne (
int8,uint1
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int8_uint1_ne';


DROP FUNCTION IF EXISTS pg_catalog.int8_uint1_lt(int8,uint1) CASCADE;
CREATE FUNCTION pg_catalog.int8_uint1_lt (
int8,uint1
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int8_uint1_lt';


DROP FUNCTION IF EXISTS pg_catalog.int8_uint1_le(int8,uint1) CASCADE;
CREATE FUNCTION pg_catalog.int8_uint1_le (
int8,uint1
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int8_uint1_le';


DROP FUNCTION IF EXISTS pg_catalog.int8_uint1_gt(int8,uint1) CASCADE;
CREATE FUNCTION pg_catalog.int8_uint1_gt (
int8,uint1
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int8_uint1_gt';


DROP FUNCTION IF EXISTS pg_catalog.int8_uint1_ge(int8,uint1) CASCADE;
CREATE FUNCTION pg_catalog.int8_uint1_ge (
int8,uint1
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int8_uint1_ge';

CREATE OPERATOR pg_catalog.+(
leftarg = int8, rightarg = uint1, procedure = int8_pl_uint1,
commutator=operator(pg_catalog.+)
);
COMMENT ON OPERATOR pg_catalog.+(int8, uint1) IS 'int8_pl_uint1';


CREATE OPERATOR pg_catalog.-(
leftarg = int8, rightarg = uint1, procedure = int8_mi_uint1,
commutator=operator(pg_catalog.-)
);
COMMENT ON OPERATOR pg_catalog.-(int8, uint1) IS 'int8_mi_uint1';


CREATE OPERATOR pg_catalog.*(
leftarg = int8, rightarg = uint1, procedure = int8_mul_uint1,
commutator=operator(pg_catalog.*)
);
COMMENT ON OPERATOR pg_catalog.*(int8, uint1) IS 'int8_mul_uint1';


CREATE OPERATOR pg_catalog./(
leftarg = int8, rightarg = uint1, procedure = int8_div_uint1,
commutator=operator(pg_catalog./)
);
COMMENT ON OPERATOR pg_catalog./(int8, uint1) IS 'int8_div_uint1';



CREATE OPERATOR pg_catalog.=(
leftarg = int8, rightarg = uint1, procedure = int8_uint1_eq,
commutator=operator(pg_catalog.=)
);
COMMENT ON OPERATOR pg_catalog.=(int8, uint1) IS 'int8_uint1_eq';



CREATE OPERATOR pg_catalog.<>(
leftarg = int8, rightarg = uint1, procedure = int8_uint1_ne,
commutator=operator(pg_catalog.<>)
);
COMMENT ON OPERATOR pg_catalog.<>(int8, uint1) IS 'int8_uint1_ne';




CREATE OPERATOR pg_catalog.<(
leftarg = int8, rightarg = uint1, procedure = int8_uint1_lt,
commutator=operator(pg_catalog.<)
);
COMMENT ON OPERATOR pg_catalog.<(int8, uint1) IS 'int8_uint1_lt';



CREATE OPERATOR pg_catalog.<=(
leftarg = int8, rightarg = uint1, procedure = int8_uint1_le,
commutator=operator(pg_catalog.<=)
);
COMMENT ON OPERATOR pg_catalog.<=(int8, uint1) IS 'int8_uint1_le';


CREATE OPERATOR pg_catalog.>(
leftarg = int8, rightarg = uint1, procedure = int8_uint1_gt,
commutator=operator(pg_catalog.>)
);
COMMENT ON OPERATOR pg_catalog.>=(int8, uint1) IS 'int8_uint1_gt';



CREATE OPERATOR pg_catalog.>=(
leftarg = int8, rightarg = uint1, procedure = int8_uint1_ge,
commutator=operator(pg_catalog.>=)
);
COMMENT ON OPERATOR pg_catalog.>=(int8, uint1) IS 'int8_uint1_ge';

-------------------int8 uint1---------------

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


drop aggregate if exists pg_catalog.bit_and(uint1);
create aggregate pg_catalog.bit_and(uint1) (SFUNC=uint1and, cFUNC = uint1and, STYPE= uint1);

drop aggregate if exists pg_catalog.bit_or(uint1);
create aggregate pg_catalog.bit_or(uint1) (SFUNC=uint1or, cFUNC = uint1or,  STYPE= uint1);


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
DROP FUNCTION IF EXISTS pg_catalog.hashuint2(uint2,uint2) CASCADE;

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


CREATE FUNCTION pg_catalog.hashuint2 (
uint2,uint2
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
RETURNS bool LANGUAGE C  AS  '$libdir/dolphin',  'uint2_bool';


DROP FUNCTION IF EXISTS pg_catalog.bool_uint2(bool) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.bool_uint2(bool)
RETURNS uint2 LANGUAGE C  AS  '$libdir/dolphin',  'bool_uint2';


DROP FUNCTION IF EXISTS pg_catalog.uint2pl(uint2,uint2) CASCADE;
CREATE FUNCTION pg_catalog.uint2pl (
uint2,uint2
) RETURNS uint2 LANGUAGE C IMMUTABLE  as '$libdir/dolphin',  'uint2pl';

DROP FUNCTION IF EXISTS pg_catalog.uint2mi(uint2,uint2) CASCADE;
CREATE FUNCTION pg_catalog.uint2mi (
uint2,uint2
) RETURNS uint2 LANGUAGE C IMMUTABLE  as '$libdir/dolphin',  'uint2mi';

DROP FUNCTION IF EXISTS pg_catalog.uint2mul(uint2,uint2) CASCADE;
CREATE FUNCTION pg_catalog.uint2mul (
uint2,uint2
) RETURNS uint2 LANGUAGE C IMMUTABLE  as '$libdir/dolphin',  'uint2mul';


DROP FUNCTION IF EXISTS pg_catalog.uint2div(uint2,uint2) CASCADE;
CREATE FUNCTION pg_catalog.uint2div (
uint2,uint2
) RETURNS float8 LANGUAGE C IMMUTABLE  as '$libdir/dolphin',  'uint2div';


DROP FUNCTION IF EXISTS pg_catalog.uint2abs(uint2) CASCADE;
CREATE FUNCTION pg_catalog.uint2abs (
uint2
) RETURNS uint2 LANGUAGE C IMMUTABLE  as '$libdir/dolphin',  'uint2abs';


DROP FUNCTION IF EXISTS pg_catalog.uint2mod(uint2,uint2) CASCADE;
CREATE FUNCTION pg_catalog.uint2mod (
uint2,uint2
) RETURNS uint2 LANGUAGE C IMMUTABLE  as '$libdir/dolphin',  'uint2mod';

DROP FUNCTION IF EXISTS pg_catalog.uint2and(uint2,uint2) CASCADE;
CREATE FUNCTION pg_catalog.uint2and (
uint2,uint2
) RETURNS uint2 LANGUAGE C IMMUTABLE  as '$libdir/dolphin',  'uint2and';

DROP FUNCTION IF EXISTS pg_catalog.uint2or(uint2,uint2) CASCADE;
CREATE FUNCTION pg_catalog.uint2or (
uint2,uint2
) RETURNS uint2 LANGUAGE C IMMUTABLE  as '$libdir/dolphin',  'uint2or';

DROP FUNCTION IF EXISTS pg_catalog.uint2xor(uint2,uint2) CASCADE;
CREATE FUNCTION pg_catalog.uint2xor (
uint2,uint2
) RETURNS uint2 LANGUAGE C  IMMUTABLE  as '$libdir/dolphin',  'uint2xor';



DROP FUNCTION IF EXISTS pg_catalog.uint2um(uint2) CASCADE;
CREATE FUNCTION pg_catalog.uint2um (
uint2
) RETURNS int2 LANGUAGE C  IMMUTABLE  as '$libdir/dolphin',  'uint2um';

DROP FUNCTION IF EXISTS pg_catalog.uint2not(uint2) CASCADE;
CREATE FUNCTION pg_catalog.uint2not (
uint2
) RETURNS uint2 LANGUAGE C  IMMUTABLE  as '$libdir/dolphin',  'uint2not';
DROP FUNCTION IF EXISTS pg_catalog.uint2up(uint2) CASCADE;
CREATE FUNCTION pg_catalog.uint2up (
uint2
) RETURNS uint2 LANGUAGE C  IMMUTABLE  as '$libdir/dolphin',  'uint2up';


DROP FUNCTION IF EXISTS pg_catalog.uint2_avg_accum(bigint[], uint2) CASCADE;

CREATE OR REPLACE FUNCTION pg_catalog.uint2_avg_accum(bigint[], uint2)
 RETURNS bigint[] LANGUAGE C  AS  '$libdir/dolphin',  'uint2_avg_accum';

DROP FUNCTION IF EXISTS pg_catalog.uint2_accum(numeric[], uint2) CASCADE;

CREATE OR REPLACE FUNCTION pg_catalog.uint2_accum(numeric[], uint2)
 RETURNS numeric[] LANGUAGE C  AS  '$libdir/dolphin',  'uint2_accum';

DROP FUNCTION IF EXISTS pg_catalog.uint2_sum(int8, uint2) CASCADE;

CREATE OR REPLACE FUNCTION pg_catalog.uint2_sum(int8, uint2)
RETURNS int8 LANGUAGE C  AS  '$libdir/dolphin',  'uint2_sum';




DROP FUNCTION IF EXISTS pg_catalog.uint2_list_agg_transfn(internal, uint2, text) CASCADE;

CREATE OR REPLACE FUNCTION pg_catalog.uint2_list_agg_transfn(internal, uint2, text)
 RETURNS internal LANGUAGE C  AS	'$libdir/dolphin',	'uint2_list_agg_transfn';



DROP FUNCTION IF EXISTS pg_catalog.uint2_list_agg_noarg2_transfn(internal, uint2) CASCADE;

CREATE OR REPLACE FUNCTION pg_catalog.uint2_list_agg_noarg2_transfn(internal, uint2)
 RETURNS internal LANGUAGE C  AS	'$libdir/dolphin',	'uint2_list_agg_noarg2_transfn';

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
commutator=operator(pg_catalog.<>),
restrict = neqsel, join = neqjoinsel
);
COMMENT ON OPERATOR pg_catalog.<>(uint2, uint2) IS 'uint2ne';





CREATE OPERATOR pg_catalog.<(
leftarg = uint2, rightarg = uint2, procedure = uint2lt,
commutator=operator(pg_catalog.<),
restrict = scalarltsel, join = scalarltjoinsel
);
COMMENT ON OPERATOR pg_catalog.<(uint2, uint2) IS 'uint2lt';


CREATE OPERATOR pg_catalog.<=(
leftarg = uint2, rightarg = uint2, procedure = uint2le,
commutator=operator(pg_catalog.<=),
restrict = scalarltsel, join = scalarltjoinsel
);
COMMENT ON OPERATOR pg_catalog.<=(uint2, uint2) IS 'uint2le';




CREATE OPERATOR pg_catalog.>(
leftarg = uint2, rightarg = uint2, procedure = uint2gt,
commutator=operator(pg_catalog.>),
restrict = scalargtsel, join = scalargtjoinsel
);
COMMENT ON OPERATOR pg_catalog.>(uint2, uint2) IS 'uint2gt';




CREATE OPERATOR pg_catalog.>=(
leftarg = uint2, rightarg = uint2, procedure = uint2ge,
commutator=operator(pg_catalog.>=),
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


create or replace function uint2(text)
returns uint2
as $$ select cast(to_number($1) as uint2)$$
language sql IMMUTABLE strict;
CREATE CAST (TEXT AS uint2) WITH FUNCTION uint2(TEXT) AS IMPLICIT;
DROP FUNCTION IF EXISTS pg_catalog.uint2_int1_mod(uint2,int1) CASCADE;
CREATE FUNCTION pg_catalog.uint2_int1_mod (
uint2,int1
) RETURNS float8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint2_int1_mod';
CREATE OPERATOR pg_catalog.%(
leftarg = uint2, rightarg = int1, procedure = uint2_int1_mod,
commutator=operator(pg_catalog.%)
);
COMMENT ON OPERATOR pg_catalog.%(uint2, int1) IS 'uint2_int1_mod';
DROP FUNCTION IF EXISTS pg_catalog.uint2_int2_mod(uint2,int2) CASCADE;
CREATE FUNCTION pg_catalog.uint2_int2_mod (
uint2,int2
) RETURNS uint2 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint2_int2_mod';
CREATE OPERATOR pg_catalog.%(
leftarg = uint2, rightarg = int2, procedure = uint2_int2_mod,
commutator=operator(pg_catalog.%)
);
COMMENT ON OPERATOR pg_catalog.%(uint2, int2) IS 'uint2_int2_mod';
DROP FUNCTION IF EXISTS pg_catalog.uint2_int4_mod(uint2,int4) CASCADE;
CREATE FUNCTION pg_catalog.uint2_int4_mod (
uint2,int4
) RETURNS float8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint2_int4_mod';
CREATE OPERATOR pg_catalog.%(
leftarg = uint2, rightarg = int4, procedure = uint2_int4_mod,
commutator=operator(pg_catalog.%)
);
COMMENT ON OPERATOR pg_catalog.%(uint2, int4) IS 'uint2_int4_mod';
DROP FUNCTION IF EXISTS pg_catalog.uint2_int8_mod(uint2,int8) CASCADE;
CREATE FUNCTION pg_catalog.uint2_int8_mod (
uint2,int8
) RETURNS float8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint2_int8_mod';
CREATE OPERATOR pg_catalog.%(
leftarg = uint2, rightarg = int8, procedure = uint2_int8_mod,
commutator=operator(pg_catalog.%)
);
COMMENT ON OPERATOR pg_catalog.%(uint2, int8) IS 'uint2_int8_mod';
DROP FUNCTION IF EXISTS pg_catalog.uint2_uint1_mod(uint2,uint1) CASCADE;
CREATE FUNCTION pg_catalog.uint2_uint1_mod (
uint2,uint1
) RETURNS float8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint2_uint1_mod';
CREATE OPERATOR pg_catalog.%(
leftarg = uint2, rightarg = uint1, procedure = uint2_uint1_mod,
commutator=operator(pg_catalog.%)
);
COMMENT ON OPERATOR pg_catalog.%(uint2, uint1) IS 'uint2_uint1_mod';
DROP FUNCTION IF EXISTS pg_catalog.uint2_uint4_mod(uint2,uint4) CASCADE;
CREATE FUNCTION pg_catalog.uint2_uint4_mod (
uint2,uint4
) RETURNS float8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint2_uint4_mod';
CREATE OPERATOR pg_catalog.%(
leftarg = uint2, rightarg = uint4, procedure = uint2_uint4_mod,
commutator=operator(pg_catalog.%)
);
COMMENT ON OPERATOR pg_catalog.%(uint2, uint4) IS 'uint2_uint4_mod';
DROP FUNCTION IF EXISTS pg_catalog.uint2_uint8_mod(uint2,uint2) CASCADE;
CREATE FUNCTION pg_catalog.uint2_uint8_mod (
uint2,uint8
) RETURNS float8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint2_uint8_mod';
CREATE OPERATOR pg_catalog.%(
leftarg = uint2, rightarg = uint8, procedure = uint2_uint8_mod,
commutator=operator(pg_catalog.%)
);
COMMENT ON OPERATOR pg_catalog.%(uint2, uint8) IS 'uint2_uint8_mod';
DROP FUNCTION IF EXISTS pg_catalog.int1_uint2_mod(int1,uint2) CASCADE;
CREATE FUNCTION pg_catalog.int1_uint2_mod (
int1,uint2
) RETURNS float8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int1_uint2_mod';
CREATE OPERATOR pg_catalog.%(
leftarg = int1, rightarg = uint2, procedure = int1_uint2_mod,
commutator=operator(pg_catalog.%)
);
COMMENT ON OPERATOR pg_catalog.%(int1, uint2) IS 'int1_uint2_mod';
DROP FUNCTION IF EXISTS pg_catalog.int2_uint2_mod(int2,uint2) CASCADE;
CREATE FUNCTION pg_catalog.int2_uint2_mod (
int2,uint2
) RETURNS float8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int2_uint2_mod';
CREATE OPERATOR pg_catalog.%(
leftarg = int2, rightarg = uint2, procedure = int2_uint2_mod,
commutator=operator(pg_catalog.%)
);
COMMENT ON OPERATOR pg_catalog.%(int2, uint2) IS 'int2_uint2_mod';
DROP FUNCTION IF EXISTS pg_catalog.int4_uint2_mod(int4,uint2) CASCADE;
CREATE FUNCTION pg_catalog.int4_uint2_mod (
int4,uint2
) RETURNS float8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int4_uint2_mod';
CREATE OPERATOR pg_catalog.%(
leftarg = int4, rightarg = uint2, procedure = int4_uint2_mod,
commutator=operator(pg_catalog.%)
);
COMMENT ON OPERATOR pg_catalog.%(int4, uint2) IS 'int4_uint2_mod';
DROP FUNCTION IF EXISTS pg_catalog.int8_uint2_mod(int8,uint2) CASCADE;
CREATE FUNCTION pg_catalog.int8_uint2_mod (
int8,uint2
) RETURNS float8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int8_uint2_mod';
CREATE OPERATOR pg_catalog.%(
leftarg = int8, rightarg = uint2, procedure = int8_uint2_mod,
commutator=operator(pg_catalog.%)
);
COMMENT ON OPERATOR pg_catalog.%(int8, uint2) IS 'int8_uint2_mod';
-------------------uint2 int 1---------------

DROP FUNCTION IF EXISTS pg_catalog.uint2_pl_int1(uint2,int1) CASCADE;
CREATE FUNCTION pg_catalog.uint2_pl_int1 (
uint2,int1
) RETURNS int2 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint2_pl_int1';

DROP FUNCTION IF EXISTS pg_catalog.uint2_mi_int1(uint2,int1) CASCADE;
CREATE FUNCTION pg_catalog.uint2_mi_int1 (
uint2,int1
) RETURNS int2 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint2_mi_int1';

DROP FUNCTION IF EXISTS pg_catalog.uint2_mul_int1(uint2,int1) CASCADE;
CREATE FUNCTION pg_catalog.uint2_mul_int1 (
uint2,int1
) RETURNS uint8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint2_mul_int1';


DROP FUNCTION IF EXISTS pg_catalog.uint2_div_int1(uint2,int1) CASCADE;
CREATE FUNCTION pg_catalog.uint2_div_int1 (
uint2,int1
) RETURNS float8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint2_div_int1';


DROP FUNCTION IF EXISTS pg_catalog.uint2_int1_eq(uint2,int1) CASCADE;
CREATE FUNCTION pg_catalog.uint2_int1_eq (
uint2,int1
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint2_int1_eq';


DROP FUNCTION IF EXISTS pg_catalog.uint2_int1_ne(uint2,int1) CASCADE;
CREATE FUNCTION pg_catalog.uint2_int1_ne (
uint2,int1
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint2_int1_ne';


DROP FUNCTION IF EXISTS pg_catalog.uint2_int1_lt(uint2,int1) CASCADE;
CREATE FUNCTION pg_catalog.uint2_int1_lt (
uint2,int1
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint2_int1_lt';


DROP FUNCTION IF EXISTS pg_catalog.uint2_int1_le(uint2,int1) CASCADE;
CREATE FUNCTION pg_catalog.uint2_int1_le (
uint2,int1
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint2_int1_le';


DROP FUNCTION IF EXISTS pg_catalog.uint2_int1_gt(uint2,int1) CASCADE;
CREATE FUNCTION pg_catalog.uint2_int1_gt (
uint2,int1
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint2_int1_gt';


DROP FUNCTION IF EXISTS pg_catalog.uint2_int1_ge(uint2,int1) CASCADE;
CREATE FUNCTION pg_catalog.uint2_int1_ge (
uint2,int1
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint2_int1_ge';
CREATE OPERATOR pg_catalog.+(
leftarg = uint2, rightarg = int1, procedure = uint2_pl_int1,
commutator=operator(pg_catalog.+)
);
COMMENT ON OPERATOR pg_catalog.+(uint2, uint2) IS 'uint2_pl_int1';


CREATE OPERATOR pg_catalog.-(
leftarg = uint2, rightarg = int1, procedure = uint2_mi_int1,
commutator=operator(pg_catalog.-)
);
COMMENT ON OPERATOR pg_catalog.-(uint2, int1) IS 'uint2_mi_int1';


CREATE OPERATOR pg_catalog.*(
leftarg = uint2, rightarg = int1, procedure = uint2_mul_int1,
commutator=operator(pg_catalog.*)
);
COMMENT ON OPERATOR pg_catalog.*(uint2, uint2) IS 'uint2_mul_int1';


CREATE OPERATOR pg_catalog./(
leftarg = uint2, rightarg = int1, procedure = uint2_div_int1,
commutator=operator(pg_catalog./)
);
COMMENT ON OPERATOR pg_catalog./(uint2, int1) IS 'uint2_div_int1';



CREATE OPERATOR pg_catalog.=(
leftarg = uint2, rightarg = int1, procedure = uint2_int1_eq,
commutator=operator(pg_catalog.=)
);
COMMENT ON OPERATOR pg_catalog.=(uint2, int1) IS 'uint2_int1_eq';



CREATE OPERATOR pg_catalog.<>(
leftarg = uint2, rightarg = int1, procedure = uint2_int1_ne,
commutator=operator(pg_catalog.<>)
);
COMMENT ON OPERATOR pg_catalog.<>(uint2, int1) IS 'uint2_int1_ne';


CREATE OPERATOR pg_catalog.<(
leftarg = uint2, rightarg = int1, procedure = uint2_int1_lt,
commutator=operator(pg_catalog.<)
);
COMMENT ON OPERATOR pg_catalog.<(uint2, int1) IS 'uint2_int1_lt';




CREATE OPERATOR pg_catalog.<=(
leftarg = uint2, rightarg = int1, procedure = uint2_int1_le,
commutator=operator(pg_catalog.<=)
);
COMMENT ON OPERATOR pg_catalog.<=(uint2, int1) IS 'uint2_int1_le';

CREATE OPERATOR pg_catalog.>(
leftarg = uint2, rightarg = int1, procedure = uint2_int1_gt,
commutator=operator(pg_catalog.>)
);
COMMENT ON OPERATOR pg_catalog.>(uint2, int1) IS 'uint2_int1_gt';


CREATE OPERATOR pg_catalog.>=(
leftarg = uint2, rightarg = int1, procedure = uint2_int1_ge,
commutator=operator(pg_catalog.>=)
);
COMMENT ON OPERATOR pg_catalog.>=(uint2, int1) IS 'uint2_int1_ge';
-------------------uint2 int 1---------------



-------------------uint2 int2---------------
DROP FUNCTION IF EXISTS pg_catalog.uint2_pl_int2(uint2,int2) CASCADE;
CREATE FUNCTION pg_catalog.uint2_pl_int2 (
uint2,int2
) RETURNS int4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint2_pl_int2';

DROP FUNCTION IF EXISTS pg_catalog.uint2_mi_int2(uint2,int2) CASCADE;
CREATE FUNCTION pg_catalog.uint2_mi_int2 (
uint2,int2
) RETURNS int4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint2_mi_int2';

DROP FUNCTION IF EXISTS pg_catalog.uint2_mul_int2(uint2,int2) CASCADE;
CREATE FUNCTION pg_catalog.uint2_mul_int2 (
uint2,int2
) RETURNS int4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint2_mul_int2';


DROP FUNCTION IF EXISTS pg_catalog.uint2_div_int2(uint2,int2) CASCADE;
CREATE FUNCTION pg_catalog.uint2_div_int2 (
uint2,int2
) RETURNS uint8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint2_div_int2';


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
commutator=operator(pg_catalog.=)
);
COMMENT ON OPERATOR pg_catalog.=(uint2, int2) IS 'uint2_int2_eq';



CREATE OPERATOR pg_catalog.<>(
leftarg = uint2, rightarg = int2, procedure = uint2_int2_ne,
commutator=operator(pg_catalog.<>)
);
COMMENT ON OPERATOR pg_catalog.<>(uint2, int2) IS 'uint2_int2_ne';




CREATE OPERATOR pg_catalog.<(
leftarg = uint2, rightarg = int2, procedure = uint2_int2_lt,
commutator=operator(pg_catalog.<)
);
COMMENT ON OPERATOR pg_catalog.<(uint2, int2) IS 'uint2_int2_lt';



CREATE OPERATOR pg_catalog.<=(
leftarg = uint2, rightarg = int2, procedure = uint2_int2_le,
commutator=operator(pg_catalog.<=)
);
COMMENT ON OPERATOR pg_catalog.<=(uint2, int2) IS 'uint2_int2_le';


CREATE OPERATOR pg_catalog.>(
leftarg = uint2, rightarg = int2, procedure = uint2_int2_gt,
commutator=operator(pg_catalog.>)
);
COMMENT ON OPERATOR pg_catalog.>(uint2, int2) IS 'uint2_int2_gt';

CREATE OPERATOR pg_catalog.>=(
leftarg = uint2, rightarg = int2, procedure = uint2_int2_ge,
commutator=operator(pg_catalog.>=)
);
COMMENT ON OPERATOR pg_catalog.>=(uint2, int2) IS 'uint2_int2_ge';

-------------------uint2 int2---------------



-------------------uint2 int4---------------
DROP FUNCTION IF EXISTS pg_catalog.uint2_pl_int4(uint2,int4) CASCADE;
CREATE FUNCTION pg_catalog.uint2_pl_int4 (
uint2,int4
) RETURNS int8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint2_pl_int4';

DROP FUNCTION IF EXISTS pg_catalog.uint2_mi_int4(uint2,int4) CASCADE;
CREATE FUNCTION pg_catalog.uint2_mi_int4 (
uint2,int4
) RETURNS int8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint2_mi_int4';
DROP FUNCTION IF EXISTS pg_catalog.uint2_mul_int4(uint2,int4) CASCADE;
CREATE FUNCTION pg_catalog.uint2_mul_int4 (
uint2,int4
) RETURNS uint8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint2_mul_int4';


DROP FUNCTION IF EXISTS pg_catalog.uint2_div_int4(uint2,int4) CASCADE;
CREATE FUNCTION pg_catalog.uint2_div_int4 (
uint2,int4
) RETURNS float8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint2_div_int4';


DROP FUNCTION IF EXISTS pg_catalog.uint2_int4_eq(uint2,int4) CASCADE;
CREATE FUNCTION pg_catalog.uint2_int4_eq (
uint2,int4
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint2_int4_eq';


DROP FUNCTION IF EXISTS pg_catalog.uint2_int4_ne(uint2,int4) CASCADE;
CREATE FUNCTION pg_catalog.uint2_int4_ne (
uint2,int4
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint2_int4_ne';


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


CREATE OPERATOR pg_catalog.+(
leftarg = uint2, rightarg = int4, procedure = uint2_pl_int4,
commutator=operator(pg_catalog.+)
);
COMMENT ON OPERATOR pg_catalog.+(uint2, uint2) IS 'uint2_pl_int4';

CREATE OPERATOR pg_catalog.-(
leftarg = uint2, rightarg = int4, procedure = uint2_mi_int4,
commutator=operator(pg_catalog.-)
);
COMMENT ON OPERATOR pg_catalog.-(uint2, int4) IS 'uint2_mi_int4';

CREATE OPERATOR pg_catalog.*(
leftarg = uint2, rightarg = int4, procedure = uint2_mul_int4,
commutator=operator(pg_catalog.*)
);
COMMENT ON OPERATOR pg_catalog.*(uint2, uint2) IS 'uint2_mul_int4';


CREATE OPERATOR pg_catalog./(
leftarg = uint2, rightarg = int4, procedure = uint2_div_int4,
commutator=operator(pg_catalog./)
);
COMMENT ON OPERATOR pg_catalog./(uint2, int4) IS 'uint2_div_int4';



CREATE OPERATOR pg_catalog.=(
leftarg = uint2, rightarg = int4, procedure = uint2_int4_eq,
commutator=operator(pg_catalog.=)
);
COMMENT ON OPERATOR pg_catalog.=(uint2, int4) IS 'uint2_int4_eq';


CREATE OPERATOR pg_catalog.<>(
leftarg = uint2, rightarg = int4, procedure = uint2_int4_ne,
commutator=operator(pg_catalog.<>)
);
COMMENT ON OPERATOR pg_catalog.<>(uint2, int4) IS 'uint2_int4_ne';



CREATE OPERATOR pg_catalog.<(
leftarg = uint2, rightarg = int4, procedure = uint2_int4_lt,
commutator=operator(pg_catalog.<)
);
COMMENT ON OPERATOR pg_catalog.<(uint2, int4) IS 'uint2_int4_lt';


CREATE OPERATOR pg_catalog.<=(
leftarg = uint2, rightarg = int4, procedure = uint2_int4_le,
commutator=operator(pg_catalog.<=)
);
COMMENT ON OPERATOR pg_catalog.<=(uint2, int4) IS 'uint2_int4_le';

CREATE OPERATOR pg_catalog.>(
leftarg = uint2, rightarg = int4, procedure = uint2_int4_gt,
commutator=operator(pg_catalog.>)
);
COMMENT ON OPERATOR pg_catalog.>(uint2, int4) IS 'uint2_int4_gt';


CREATE OPERATOR pg_catalog.>=(
leftarg = uint2, rightarg = int4, procedure = uint2_int4_ge,
commutator=operator(pg_catalog.>=)
);
COMMENT ON OPERATOR pg_catalog.>=(uint2, int4) IS 'uint2_int4_ge';

-------------------uint2 int4---------------


-------------------uint2 int8---------------

DROP FUNCTION IF EXISTS pg_catalog.uint2_pl_int8(uint2,int8) CASCADE;
CREATE FUNCTION pg_catalog.uint2_pl_int8 (
uint2,int8
) RETURNS int16 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint2_pl_int8';

DROP FUNCTION IF EXISTS pg_catalog.uint2_mi_int8(uint2,int8) CASCADE;
CREATE FUNCTION pg_catalog.uint2_mi_int8 (
uint2,int8
) RETURNS int16 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint2_mi_int8';

DROP FUNCTION IF EXISTS pg_catalog.uint2_mul_int8(uint2,int8) CASCADE;
CREATE FUNCTION pg_catalog.uint2_mul_int8 (
uint2,int8
) RETURNS int16 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint2_mul_int8';


DROP FUNCTION IF EXISTS pg_catalog.uint2_div_int8(uint2,int8) CASCADE;
CREATE FUNCTION pg_catalog.uint2_div_int8 (
uint2,int8
) RETURNS float8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint2_div_int8';


DROP FUNCTION IF EXISTS pg_catalog.uint2_int8_eq(uint2,int8) CASCADE;
CREATE FUNCTION pg_catalog.uint2_int8_eq (
uint2,int8
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint2_int8_eq';


DROP FUNCTION IF EXISTS pg_catalog.uint2_int8_ne(uint2,int8) CASCADE;
CREATE FUNCTION pg_catalog.uint2_int8_ne (
uint2,int8
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint2_int8_ne';


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

CREATE OPERATOR pg_catalog.+(
leftarg = uint2, rightarg = int8, procedure = uint2_pl_int8,
commutator=operator(pg_catalog.+)
);
COMMENT ON OPERATOR pg_catalog.+(uint2, int8) IS 'uint2_pl_int8';

CREATE OPERATOR pg_catalog.-(
leftarg = uint2, rightarg = int8, procedure = uint2_mi_int8,
commutator=operator(pg_catalog.-)
);
COMMENT ON OPERATOR pg_catalog.-(uint2, int8) IS 'uint2_mi_int8';

CREATE OPERATOR pg_catalog.*(
leftarg = uint2, rightarg = int8, procedure = uint2_mul_int8,
commutator=operator(pg_catalog.*)
);
COMMENT ON OPERATOR pg_catalog.*(uint2, int8) IS 'uint2_mul_int8';

CREATE OPERATOR pg_catalog./(
leftarg = uint2, rightarg = int8, procedure = uint2_div_int8,
commutator=operator(pg_catalog./)
);
COMMENT ON OPERATOR pg_catalog./(uint2, int8) IS 'uint2_div_int8';


CREATE OPERATOR pg_catalog.=(
leftarg = uint2, rightarg = int8, procedure = uint2_int8_eq,
commutator=operator(pg_catalog.=)
);
COMMENT ON OPERATOR pg_catalog.=(uint2, int8) IS 'uint2_int8_eq';


CREATE OPERATOR pg_catalog.<>(
leftarg = uint2, rightarg = int8, procedure = uint2_int8_ne,
commutator=operator(pg_catalog.<>)
);
COMMENT ON OPERATOR pg_catalog.<>(uint2, int8) IS 'uint2_int8_ne';


CREATE OPERATOR pg_catalog.<(
leftarg = uint2, rightarg = int8, procedure = uint2_int8_lt,
commutator=operator(pg_catalog.<)
);
COMMENT ON OPERATOR pg_catalog.<(uint2, int8) IS 'uint2_int8_lt';

CREATE OPERATOR pg_catalog.<=(
leftarg = uint2, rightarg = int8, procedure = uint2_int8_le,
commutator=operator(pg_catalog.<=)
);
COMMENT ON OPERATOR pg_catalog.<=(uint2, int8) IS 'uint2_int8_le';


CREATE OPERATOR pg_catalog.>(
leftarg = uint2, rightarg = int8, procedure = uint2_int8_gt,
commutator=operator(pg_catalog.>)
);
COMMENT ON OPERATOR pg_catalog.>(uint2, int8) IS 'uint2_int8_gt';


CREATE OPERATOR pg_catalog.>=(
leftarg = uint2, rightarg = int8, procedure = uint2_int8_ge,
commutator=operator(pg_catalog.>=)
);
COMMENT ON OPERATOR pg_catalog.>=(uint2, int8) IS 'uint2_int8_ge';


-------------------uint2 int8---------------
	
-------------------uint2 uint1---------------

DROP FUNCTION IF EXISTS pg_catalog.uint2_pl_uint1(uint2,uint1) CASCADE;
CREATE FUNCTION pg_catalog.uint2_pl_uint1 (
uint2,uint1
) RETURNS uint4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint2_pl_uint1';

DROP FUNCTION IF EXISTS pg_catalog.uint2_mi_uint1(uint2,uint1) CASCADE;
CREATE FUNCTION pg_catalog.uint2_mi_uint1 (
uint2,uint1
) RETURNS uint1 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint2_mi_uint1';
DROP FUNCTION IF EXISTS pg_catalog.uint2_mul_uint1(uint2,uint1) CASCADE;
CREATE FUNCTION pg_catalog.uint2_mul_uint1 (
uint2,uint1
) RETURNS uint4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint2_mul_uint1';


DROP FUNCTION IF EXISTS pg_catalog.uint2_div_uint1(uint2,uint1) CASCADE;
CREATE FUNCTION pg_catalog.uint2_div_uint1 (
uint2,uint1
) RETURNS float8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint2_div_uint1';


DROP FUNCTION IF EXISTS pg_catalog.uint2_uint1_eq(uint2,uint1) CASCADE;
CREATE FUNCTION pg_catalog.uint2_uint1_eq (
uint2,uint1
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint2_uint1_eq';


DROP FUNCTION IF EXISTS pg_catalog.uint2_uint1_ne(uint2,uint1) CASCADE;
CREATE FUNCTION pg_catalog.uint2_uint1_ne (
uint2,uint1
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint2_uint1_ne';


DROP FUNCTION IF EXISTS pg_catalog.uint2_uint1_lt(uint2,uint1) CASCADE;
CREATE FUNCTION pg_catalog.uint2_uint1_lt (
uint2,uint1
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint2_uint1_lt';


DROP FUNCTION IF EXISTS pg_catalog.uint2_uint1_le(uint2,uint1) CASCADE;
CREATE FUNCTION pg_catalog.uint2_uint1_le (
uint2,uint1
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint2_uint1_le';


DROP FUNCTION IF EXISTS pg_catalog.uint2_uint1_gt(uint2,uint1) CASCADE;
CREATE FUNCTION pg_catalog.uint2_uint1_gt (
uint2,uint1
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint2_uint1_gt';


DROP FUNCTION IF EXISTS pg_catalog.uint2_uint1_ge(uint2,uint1) CASCADE;
CREATE FUNCTION pg_catalog.uint2_uint1_ge (
uint2,uint1
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint2_uint1_ge';

CREATE OPERATOR pg_catalog.+(
leftarg = uint2, rightarg = uint1, procedure = uint2_pl_uint1,
commutator=operator(pg_catalog.+)
);
COMMENT ON OPERATOR pg_catalog.+(uint2, uint1) IS 'uint2_pl_uint1';

CREATE OPERATOR pg_catalog.-(
leftarg = uint2, rightarg = uint1, procedure = uint2_mi_uint1,
commutator=operator(pg_catalog.-)
);
COMMENT ON OPERATOR pg_catalog.-(uint2, uint1) IS 'uint2_mi_uint1';

CREATE OPERATOR pg_catalog.*(
leftarg = uint2, rightarg = uint1, procedure = uint2_mul_uint1,
commutator=operator(pg_catalog.*)
);
COMMENT ON OPERATOR pg_catalog.*(uint2, uint1) IS 'uint2_mul_uint1';


CREATE OPERATOR pg_catalog./(
leftarg = uint2, rightarg = uint1, procedure = uint2_div_uint1,
commutator=operator(pg_catalog./)
);
COMMENT ON OPERATOR pg_catalog./(uint2, uint1) IS 'uint2_div_uint1';


CREATE OPERATOR pg_catalog.=(
leftarg = uint2, rightarg = uint1, procedure = uint2_uint1_eq,
commutator=operator(pg_catalog.=)
);
COMMENT ON OPERATOR pg_catalog.=(uint2, uint1) IS 'uint2_uint1_eq';


CREATE OPERATOR pg_catalog.<>(
leftarg = uint2, rightarg = uint1, procedure = uint2_uint1_ne,
commutator=operator(pg_catalog.<>)
);
COMMENT ON OPERATOR pg_catalog.<>(uint2, uint1) IS 'uint2_uint1_ne';


CREATE OPERATOR pg_catalog.<(
leftarg = uint2, rightarg = uint1, procedure = uint2_uint1_lt,
commutator=operator(pg_catalog.<)
);
COMMENT ON OPERATOR pg_catalog.<(uint2, uint1) IS 'uint2_uint1_lt';


CREATE OPERATOR pg_catalog.<=(
leftarg = uint2, rightarg = uint1, procedure = uint2_uint1_le,
commutator=operator(pg_catalog.<=)
);
COMMENT ON OPERATOR pg_catalog.<=(uint2, uint1) IS 'uint2_uint1_le';


CREATE OPERATOR pg_catalog.>(
leftarg = uint2, rightarg = uint1, procedure = uint2_uint1_gt,
commutator=operator(pg_catalog.>)
);
COMMENT ON OPERATOR pg_catalog.>=(uint2, uint1) IS 'uint2_uint1_gt';


CREATE OPERATOR pg_catalog.>=(
leftarg = uint2, rightarg = uint1, procedure = uint2_uint1_ge,
commutator=operator(pg_catalog.>=)
);
COMMENT ON OPERATOR pg_catalog.>=(uint2, uint1) IS 'uint2_uint1_ge';


-------------------uint2 uint1---------------



-------------------uint2 uint4---------------
DROP FUNCTION IF EXISTS pg_catalog.uint2_pl_uint4(uint2,uint4) CASCADE;
CREATE FUNCTION pg_catalog.uint2_pl_uint4 (
uint2,uint4
) RETURNS uint8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint2_pl_uint4';

DROP FUNCTION IF EXISTS pg_catalog.uint2_mi_uint4(uint2,uint4) CASCADE;
CREATE FUNCTION pg_catalog.uint2_mi_uint4 (
uint2,uint4
) RETURNS int8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint2_mi_uint4';
DROP FUNCTION IF EXISTS pg_catalog.uint2_mul_uint4(uint2,uint4) CASCADE;
CREATE FUNCTION pg_catalog.uint2_mul_uint4 (
uint2,uint4
) RETURNS uint8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint2_mul_uint4';


DROP FUNCTION IF EXISTS pg_catalog.uint2_div_uint4(uint2,uint4) CASCADE;
CREATE FUNCTION pg_catalog.uint2_div_uint4 (
uint2,uint4
) RETURNS float8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint2_div_uint4';


DROP FUNCTION IF EXISTS pg_catalog.uint2_uint4_eq(uint2,uint4) CASCADE;
CREATE FUNCTION pg_catalog.uint2_uint4_eq (
uint2,uint4
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint2_uint4_eq';


DROP FUNCTION IF EXISTS pg_catalog.uint2_uint4_ne(uint2,uint4) CASCADE;
CREATE FUNCTION pg_catalog.uint2_uint4_ne (
uint2,uint4
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint2_uint4_ne';


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


CREATE OPERATOR pg_catalog.+(
leftarg = uint2, rightarg = uint4, procedure = uint2_pl_uint4,
commutator=operator(pg_catalog.+)
);
COMMENT ON OPERATOR pg_catalog.+(uint2, uint2) IS 'uint2_pl_uint4';


CREATE OPERATOR pg_catalog.-(
leftarg = uint2, rightarg = uint4, procedure = uint2_mi_uint4,
commutator=operator(pg_catalog.-)
);
COMMENT ON OPERATOR pg_catalog.-(uint2, uint4) IS 'uint2_mi_uint4';


CREATE OPERATOR pg_catalog.*(
leftarg = uint2, rightarg = uint4, procedure = uint2_mul_uint4,
commutator=operator(pg_catalog.*)
);
COMMENT ON OPERATOR pg_catalog.*(uint2, uint2) IS 'uint2_mul_uint4';


CREATE OPERATOR pg_catalog./(
leftarg = uint2, rightarg = uint4, procedure = uint2_div_uint4,
commutator=operator(pg_catalog./%)
);
COMMENT ON OPERATOR pg_catalog./(uint2, uint4) IS 'uint2_div_uint4';


CREATE OPERATOR pg_catalog.=(
leftarg = uint2, rightarg = uint4, procedure = uint2_uint4_eq,
commutator=operator(pg_catalog.=)
);
COMMENT ON OPERATOR pg_catalog.=(uint2, uint4) IS 'uint2_uint4_eq';


CREATE OPERATOR pg_catalog.<>(
leftarg = uint2, rightarg = uint4, procedure = uint2_uint4_ne,
commutator=operator(pg_catalog.<>)
);
COMMENT ON OPERATOR pg_catalog.<>(uint2, uint4) IS 'uint2_uint4_ne';


CREATE OPERATOR pg_catalog.<(
leftarg = uint2, rightarg = uint4, procedure = uint2_uint4_lt,
commutator=operator(pg_catalog.<)
);
COMMENT ON OPERATOR pg_catalog.<(uint2, uint4) IS 'uint2_uint4_lt';


CREATE OPERATOR pg_catalog.<=(
leftarg = uint2, rightarg = uint4, procedure = uint2_uint4_le,
commutator=operator(pg_catalog.<=)
);
COMMENT ON OPERATOR pg_catalog.<=(uint2, uint4) IS 'uint2_uint4_le';


CREATE OPERATOR pg_catalog.>(
leftarg = uint2, rightarg = uint4, procedure = uint2_uint4_gt,
commutator=operator(pg_catalog.>)
);
COMMENT ON OPERATOR pg_catalog.>(uint2, uint4) IS 'uint2_uint4_gt';


CREATE OPERATOR pg_catalog.>=(
leftarg = uint2, rightarg = uint4, procedure = uint2_uint4_ge,
commutator=operator(pg_catalog.>=)
);
COMMENT ON OPERATOR pg_catalog.>=(uint2, uint4) IS 'uint2_uint4_ge';




-------------------uint2 uint4---------------


-------------------uint2 uint8---------------

DROP FUNCTION IF EXISTS pg_catalog.uint2_pl_uint8(uint2,uint8) CASCADE;
CREATE FUNCTION pg_catalog.uint2_pl_uint8 (
uint2,uint8
) RETURNS int16 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint2_pl_uint8';

DROP FUNCTION IF EXISTS pg_catalog.uint2_mi_uint8(uint2,uint8) CASCADE;
CREATE FUNCTION pg_catalog.uint2_mi_uint8 (
uint2,uint8
) RETURNS int16 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint2_mi_uint8';
DROP FUNCTION IF EXISTS pg_catalog.uint2_mul_uint8(uint2,uint8) CASCADE;
CREATE FUNCTION pg_catalog.uint2_mul_uint8 (
uint2,uint8
) RETURNS int16 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint2_mul_uint8';


DROP FUNCTION IF EXISTS pg_catalog.uint2_div_uint8(uint2,uint8) CASCADE;
CREATE FUNCTION pg_catalog.uint2_div_uint8 (
uint2,uint8
) RETURNS float8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint2_div_uint8';


DROP FUNCTION IF EXISTS pg_catalog.uint2_uint8_eq(uint2,uint8) CASCADE;
CREATE FUNCTION pg_catalog.uint2_uint8_eq (
uint2,uint8
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint2_uint8_eq';


DROP FUNCTION IF EXISTS pg_catalog.uint2_uint8_ne(uint2,uint8) CASCADE;
CREATE FUNCTION pg_catalog.uint2_uint8_ne (
uint2,uint8
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint2_uint8_ne';


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

CREATE OPERATOR pg_catalog.+(
leftarg = uint2, rightarg = uint8, procedure = uint2_pl_uint8,
commutator=operator(pg_catalog.+)
);
COMMENT ON OPERATOR pg_catalog.+(uint2, uint2) IS 'uint2_pl_uint8';


CREATE OPERATOR pg_catalog.-(
leftarg = uint2, rightarg = uint8, procedure = uint2_mi_uint8,
commutator=operator(pg_catalog.-)
);
COMMENT ON OPERATOR pg_catalog.-(uint2, uint8) IS 'uint2_mi_uint8';


CREATE OPERATOR pg_catalog.*(
leftarg = uint2, rightarg = uint8, procedure = uint2_mul_uint8,
commutator=operator(pg_catalog.*)
);
COMMENT ON OPERATOR pg_catalog.*(uint2, uint2) IS 'uint2_mul_uint8';


CREATE OPERATOR pg_catalog./(
leftarg = uint2, rightarg = uint8, procedure = uint2_div_uint8,
commutator=operator(pg_catalog./)
);
COMMENT ON OPERATOR pg_catalog./(uint2, uint8) IS 'uint2_div_uint8';



CREATE OPERATOR pg_catalog.=(
leftarg = uint2, rightarg = uint8, procedure = uint2_uint8_eq,
commutator=operator(pg_catalog.=)
);
COMMENT ON OPERATOR pg_catalog.=(uint2, uint8) IS 'uint2_uint8_eq';



CREATE OPERATOR pg_catalog.<>(
leftarg = uint2, rightarg = uint8, procedure = uint2_uint8_ne,
commutator=operator(pg_catalog.<>)
);
COMMENT ON OPERATOR pg_catalog.<>(uint2, uint8) IS 'uint2_uint8_ne';




CREATE OPERATOR pg_catalog.<(
leftarg = uint2, rightarg = uint8, procedure = uint2_uint8_lt,
commutator=operator(pg_catalog.<)
);
COMMENT ON OPERATOR pg_catalog.<(uint2, uint8) IS 'uint2_uint8_lt';



CREATE OPERATOR pg_catalog.<=(
leftarg = uint2, rightarg = uint8, procedure = uint2_uint8_le,
commutator=operator(pg_catalog.<=)
);
COMMENT ON OPERATOR pg_catalog.<=(uint2, uint8) IS 'uint2_uint8_le';


CREATE OPERATOR pg_catalog.>(
leftarg = uint2, rightarg = uint8, procedure = uint2_uint8_gt,
commutator=operator(pg_catalog.>)
);
COMMENT ON OPERATOR pg_catalog.>(uint2, uint8) IS 'uint2_uint8_gt';



CREATE OPERATOR pg_catalog.>=(
leftarg = uint2, rightarg = uint8, procedure = uint2_uint8_ge,
commutator=operator(pg_catalog.>=)
);
COMMENT ON OPERATOR pg_catalog.>=(uint2, uint8) IS 'uint2_uint8_ge';

-------------------uint2 uint8---------------
-------------------int1 uint2---------------

DROP FUNCTION IF EXISTS pg_catalog.int1_pl_uint2(int1,uint2) CASCADE;
CREATE FUNCTION pg_catalog.int1_pl_uint2 (
int1,uint2
) RETURNS int2 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int1_pl_uint2';

DROP FUNCTION IF EXISTS pg_catalog.int1_mi_uint2(int1,uint2) CASCADE;
CREATE FUNCTION pg_catalog.int1_mi_uint2 (
int1,uint2
) RETURNS int2 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int1_mi_uint2';
DROP FUNCTION IF EXISTS pg_catalog.int1_mul_uint2(int1,uint2) CASCADE;
CREATE FUNCTION pg_catalog.int1_mul_uint2 (
int1,uint2
) RETURNS int2 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int1_mul_uint2';


DROP FUNCTION IF EXISTS pg_catalog.int1_div_uint2(int1,uint2) CASCADE;
CREATE FUNCTION pg_catalog.int1_div_uint2 (
int1,uint2
) RETURNS float8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int1_div_uint2';


DROP FUNCTION IF EXISTS pg_catalog.int1_uint2_eq(int1,uint2) CASCADE;
CREATE FUNCTION pg_catalog.int1_uint2_eq (
int1,uint2
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int1_uint2_eq';


DROP FUNCTION IF EXISTS pg_catalog.int1_uint2_ne(int1,uint2) CASCADE;
CREATE FUNCTION pg_catalog.int1_uint2_ne (
int1,uint2
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int1_uint2_ne';


DROP FUNCTION IF EXISTS pg_catalog.int1_uint2_lt(int1,uint2) CASCADE;
CREATE FUNCTION pg_catalog.int1_uint2_lt (
int1,uint2
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int1_uint2_lt';


DROP FUNCTION IF EXISTS pg_catalog.int1_uint2_le(int1,uint2) CASCADE;
CREATE FUNCTION pg_catalog.int1_uint2_le (
int1,uint2
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int1_uint2_le';


DROP FUNCTION IF EXISTS pg_catalog.int1_uint2_gt(int1,uint2) CASCADE;
CREATE FUNCTION pg_catalog.int1_uint2_gt (
int1,uint2
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int1_uint2_gt';


DROP FUNCTION IF EXISTS pg_catalog.int1_uint2_ge(int1,uint2) CASCADE;
CREATE FUNCTION pg_catalog.int1_uint2_ge (
int1,uint2
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int1_uint2_ge';

CREATE OPERATOR pg_catalog.+(
leftarg = int1, rightarg = uint2, procedure = int1_pl_uint2,
commutator=operator(pg_catalog.+)
);
COMMENT ON OPERATOR pg_catalog.+(int1, uint2) IS 'int1_pl_uint2';


CREATE OPERATOR pg_catalog.-(
leftarg = int1, rightarg = uint2, procedure = int1_mi_uint2,
commutator=operator(pg_catalog.-)
);
COMMENT ON OPERATOR pg_catalog.-(int1, uint2) IS 'int1_mi_uint2';


CREATE OPERATOR pg_catalog.*(
leftarg = int1, rightarg = uint2, procedure = int1_mul_uint2,
commutator=operator(pg_catalog.*)
);
COMMENT ON OPERATOR pg_catalog.*(int1, uint2) IS 'int1_mul_uint2';


CREATE OPERATOR pg_catalog./(
leftarg = int1, rightarg = uint2, procedure = int1_div_uint2,
commutator=operator(pg_catalog./)
);
COMMENT ON OPERATOR pg_catalog./(int1, uint2) IS 'int1_div_uint2';



CREATE OPERATOR pg_catalog.=(
leftarg = int1, rightarg = uint2, procedure = int1_uint2_eq,
commutator=operator(pg_catalog.=)
);
COMMENT ON OPERATOR pg_catalog.=(int1, uint2) IS 'int1_uint2_eq';



CREATE OPERATOR pg_catalog.<>(
leftarg = int1, rightarg = uint2, procedure = int1_uint2_ne,
commutator=operator(pg_catalog.<>)
);
COMMENT ON OPERATOR pg_catalog.<>(int1, uint2) IS 'int1_uint2_ne';




CREATE OPERATOR pg_catalog.<(
leftarg = int1, rightarg = uint2, procedure = int1_uint2_lt,
commutator=operator(pg_catalog.<)
);
COMMENT ON OPERATOR pg_catalog.<(int1, uint2) IS 'int1_uint2_lt';



CREATE OPERATOR pg_catalog.<=(
leftarg = int1, rightarg = uint2, procedure = int1_uint2_le,
commutator=operator(pg_catalog.<=)
);
COMMENT ON OPERATOR pg_catalog.<=(int1, uint2) IS 'int1_uint2_le';


CREATE OPERATOR pg_catalog.>(
leftarg = int1, rightarg = uint2, procedure = int1_uint2_gt,
commutator=operator(pg_catalog.>)
);
COMMENT ON OPERATOR pg_catalog.>=(int1, uint2) IS 'int1_uint2_gt';



CREATE OPERATOR pg_catalog.>=(
leftarg = int1, rightarg = uint2, procedure = int1_uint2_ge,
commutator=operator(pg_catalog.>=)
);
COMMENT ON OPERATOR pg_catalog.>=(int1, uint2) IS 'int1_uint2_ge';

-------------------int1 uint2---------------


-------------------int2 uint2---------------



DROP FUNCTION IF EXISTS pg_catalog.int2_pl_uint2(int2,uint2) CASCADE;
CREATE FUNCTION pg_catalog.int2_pl_uint2 (
int2,uint2
) RETURNS int4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int2_pl_uint2';

DROP FUNCTION IF EXISTS pg_catalog.int2_mi_uint2(int2,uint2) CASCADE;
CREATE FUNCTION pg_catalog.int2_mi_uint2 (
int2,uint2
) RETURNS int4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int2_mi_uint2';
DROP FUNCTION IF EXISTS pg_catalog.int2_mul_uint2(int2,uint2) CASCADE;
CREATE FUNCTION pg_catalog.int2_mul_uint2 (
int2,uint2
) RETURNS int4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int2_mul_uint2';


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
commutator=operator(pg_catalog.=)
);
COMMENT ON OPERATOR pg_catalog.=(int2, uint2) IS 'int2_uint2_eq';



CREATE OPERATOR pg_catalog.<>(
leftarg = int2, rightarg = uint2, procedure = int2_uint2_ne,
commutator=operator(pg_catalog.<>)
);
COMMENT ON OPERATOR pg_catalog.<>(int2, uint2) IS 'int2_uint2_ne';




CREATE OPERATOR pg_catalog.<(
leftarg = int2, rightarg = uint2, procedure = int2_uint2_lt,
commutator=operator(pg_catalog.<)
);
COMMENT ON OPERATOR pg_catalog.<(int2, uint2) IS 'int2_uint2_lt';



CREATE OPERATOR pg_catalog.<=(
leftarg = int2, rightarg = uint2, procedure = int2_uint2_le,
commutator=operator(pg_catalog.<=)
);
COMMENT ON OPERATOR pg_catalog.<=(int2, uint2) IS 'int2_uint2_le';


CREATE OPERATOR pg_catalog.>(
leftarg = int2, rightarg = uint2, procedure = int2_uint2_gt,
commutator=operator(pg_catalog.>)
);
COMMENT ON OPERATOR pg_catalog.>=(int2, uint2) IS 'int2_uint2_gt';



CREATE OPERATOR pg_catalog.>=(
leftarg = int2, rightarg = uint2, procedure = int2_uint2_ge,
commutator=operator(pg_catalog.>=)
);
COMMENT ON OPERATOR pg_catalog.>=(int2, uint2) IS 'int2_uint2_ge';

-------------------int2 uint2---------------

-------------------int4 uint2---------------
DROP FUNCTION IF EXISTS pg_catalog.int4_pl_uint2(int4,uint2) CASCADE;
CREATE FUNCTION pg_catalog.int4_pl_uint2 (
int4,uint2
) RETURNS int8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int4_pl_uint2';

DROP FUNCTION IF EXISTS pg_catalog.int4_mi_uint2(int4,uint2) CASCADE;
CREATE FUNCTION pg_catalog.int4_mi_uint2 (
int4,uint2
) RETURNS int8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int4_mi_uint2';
DROP FUNCTION IF EXISTS pg_catalog.int4_mul_uint2(int4,uint2) CASCADE;
CREATE FUNCTION pg_catalog.int4_mul_uint2 (
int4,uint2
) RETURNS int8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int4_mul_uint2';


DROP FUNCTION IF EXISTS pg_catalog.int4_div_uint2(int4,uint2) CASCADE;
CREATE FUNCTION pg_catalog.int4_div_uint2 (
int4,uint2
) RETURNS float8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int4_div_uint2';


DROP FUNCTION IF EXISTS pg_catalog.int4_uint2_eq(int4,uint2) CASCADE;
CREATE FUNCTION pg_catalog.int4_uint2_eq (
int4,uint2
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int4_uint2_eq';


DROP FUNCTION IF EXISTS pg_catalog.int4_uint2_ne(int4,uint2) CASCADE;
CREATE FUNCTION pg_catalog.int4_uint2_ne (
int4,uint2
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int4_uint2_ne';


DROP FUNCTION IF EXISTS pg_catalog.int4_uint2_lt(int4,uint2) CASCADE;
CREATE FUNCTION pg_catalog.int4_uint2_lt (
int4,uint2
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int4_uint2_lt';


DROP FUNCTION IF EXISTS pg_catalog.int4_uint2_le(int4,uint2) CASCADE;
CREATE FUNCTION pg_catalog.int4_uint2_le (
int4,uint2
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int4_uint2_le';


DROP FUNCTION IF EXISTS pg_catalog.int4_uint2_gt(int4,uint2) CASCADE;
CREATE FUNCTION pg_catalog.int4_uint2_gt (
int4,uint2
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int4_uint2_gt';


DROP FUNCTION IF EXISTS pg_catalog.int4_uint2_ge(int4,uint2) CASCADE;
CREATE FUNCTION pg_catalog.int4_uint2_ge (
int4,uint2
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int4_uint2_ge';

CREATE OPERATOR pg_catalog.+(
leftarg = int4, rightarg = uint2, procedure = int4_pl_uint2,
commutator=operator(pg_catalog.+)
);
COMMENT ON OPERATOR pg_catalog.+(int4, uint2) IS 'int4_pl_uint2';


CREATE OPERATOR pg_catalog.-(
leftarg = int4, rightarg = uint2, procedure = int4_mi_uint2,
commutator=operator(pg_catalog.-)
);
COMMENT ON OPERATOR pg_catalog.-(int4, uint2) IS 'int4_mi_uint2';


CREATE OPERATOR pg_catalog.*(
leftarg = int4, rightarg = uint2, procedure = int4_mul_uint2,
commutator=operator(pg_catalog.*)
);
COMMENT ON OPERATOR pg_catalog.*(int4, uint2) IS 'int4_mul_uint2';


CREATE OPERATOR pg_catalog./(
leftarg = int4, rightarg = uint2, procedure = int4_div_uint2,
commutator=operator(pg_catalog./)
);
COMMENT ON OPERATOR pg_catalog./(int4, uint2) IS 'int4_div_uint2';



CREATE OPERATOR pg_catalog.=(
leftarg = int4, rightarg = uint2, procedure = int4_uint2_eq,
commutator=operator(pg_catalog.=)
);
COMMENT ON OPERATOR pg_catalog.=(int4, uint2) IS 'int4_uint2_eq';



CREATE OPERATOR pg_catalog.<>(
leftarg = int4, rightarg = uint2, procedure = int4_uint2_ne,
commutator=operator(pg_catalog.<>)
);
COMMENT ON OPERATOR pg_catalog.<>(int4, uint2) IS 'int4_uint2_ne';




CREATE OPERATOR pg_catalog.<(
leftarg = int4, rightarg = uint2, procedure = int4_uint2_lt,
commutator=operator(pg_catalog.<)
);
COMMENT ON OPERATOR pg_catalog.<(int4, uint2) IS 'int4_uint2_lt';



CREATE OPERATOR pg_catalog.<=(
leftarg = int4, rightarg = uint2, procedure = int4_uint2_le,
commutator=operator(pg_catalog.<=)
);
COMMENT ON OPERATOR pg_catalog.<=(int4, uint2) IS 'int4_uint2_le';


CREATE OPERATOR pg_catalog.>(
leftarg = int4, rightarg = uint2, procedure = int4_uint2_gt,
commutator=operator(pg_catalog.>)
);
COMMENT ON OPERATOR pg_catalog.>=(int4, uint2) IS 'int4_uint2_gt';



CREATE OPERATOR pg_catalog.>=(
leftarg = int4, rightarg = uint2, procedure = int4_uint2_ge,
commutator=operator(pg_catalog.>=)
);
COMMENT ON OPERATOR pg_catalog.>=(int4, uint2) IS 'int4_uint2_ge';

-------------------int4 uint2---------------

-------------------int8 uint2---------------
DROP FUNCTION IF EXISTS pg_catalog.int8_pl_uint2(int8,uint2) CASCADE;
CREATE FUNCTION pg_catalog.int8_pl_uint2 (
int8,uint2
) RETURNS int16 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int8_pl_uint2';

DROP FUNCTION IF EXISTS pg_catalog.int8_mi_uint2(int8,uint2) CASCADE;
CREATE FUNCTION pg_catalog.int8_mi_uint2 (
int8,uint2
) RETURNS int16 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int8_mi_uint2';
DROP FUNCTION IF EXISTS pg_catalog.int8_mul_uint2(int8,uint2) CASCADE;
CREATE FUNCTION pg_catalog.int8_mul_uint2 (
int8,uint2
) RETURNS int16 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int8_mul_uint2';


DROP FUNCTION IF EXISTS pg_catalog.int8_div_uint2(int8,uint2) CASCADE;
CREATE FUNCTION pg_catalog.int8_div_uint2 (
int8,uint2
) RETURNS float8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int8_div_uint2';


DROP FUNCTION IF EXISTS pg_catalog.int8_uint2_eq(int8,uint2) CASCADE;
CREATE FUNCTION pg_catalog.int8_uint2_eq (
int8,uint2
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int8_uint2_eq';


DROP FUNCTION IF EXISTS pg_catalog.int8_uint2_ne(int8,uint2) CASCADE;
CREATE FUNCTION pg_catalog.int8_uint2_ne (
int8,uint2
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int8_uint2_ne';


DROP FUNCTION IF EXISTS pg_catalog.int8_uint2_lt(int8,uint2) CASCADE;
CREATE FUNCTION pg_catalog.int8_uint2_lt (
int8,uint2
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int8_uint2_lt';


DROP FUNCTION IF EXISTS pg_catalog.int8_uint2_le(int8,uint2) CASCADE;
CREATE FUNCTION pg_catalog.int8_uint2_le (
int8,uint2
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int8_uint2_le';


DROP FUNCTION IF EXISTS pg_catalog.int8_uint2_gt(int8,uint2) CASCADE;
CREATE FUNCTION pg_catalog.int8_uint2_gt (
int8,uint2
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int8_uint2_gt';


DROP FUNCTION IF EXISTS pg_catalog.int8_uint2_ge(int8,uint2) CASCADE;
CREATE FUNCTION pg_catalog.int8_uint2_ge (
int8,uint2
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int8_uint2_ge';

CREATE OPERATOR pg_catalog.+(
leftarg = int8, rightarg = uint2, procedure = int8_pl_uint2,
commutator=operator(pg_catalog.+)
);
COMMENT ON OPERATOR pg_catalog.+(int8, uint2) IS 'int8_pl_uint2';


CREATE OPERATOR pg_catalog.-(
leftarg = int8, rightarg = uint2, procedure = int8_mi_uint2,
commutator=operator(pg_catalog.-)
);
COMMENT ON OPERATOR pg_catalog.-(int8, uint2) IS 'int8_mi_uint2';


CREATE OPERATOR pg_catalog.*(
leftarg = int8, rightarg = uint2, procedure = int8_mul_uint2,
commutator=operator(pg_catalog.*)
);
COMMENT ON OPERATOR pg_catalog.*(int8, uint2) IS 'int8_mul_uint2';


CREATE OPERATOR pg_catalog./(
leftarg = int8, rightarg = uint2, procedure = int8_div_uint2,
commutator=operator(pg_catalog./)
);
COMMENT ON OPERATOR pg_catalog./(int8, uint2) IS 'int8_div_uint2';



CREATE OPERATOR pg_catalog.=(
leftarg = int8, rightarg = uint2, procedure = int8_uint2_eq,
commutator=operator(pg_catalog.=)
);
COMMENT ON OPERATOR pg_catalog.=(int8, uint2) IS 'int8_uint2_eq';



CREATE OPERATOR pg_catalog.<>(
leftarg = int8, rightarg = uint2, procedure = int8_uint2_ne,
commutator=operator(pg_catalog.<>)
);
COMMENT ON OPERATOR pg_catalog.<>(int8, uint2) IS 'int8_uint2_ne';




CREATE OPERATOR pg_catalog.<(
leftarg = int8, rightarg = uint2, procedure = int8_uint2_lt,
commutator=operator(pg_catalog.<)
);
COMMENT ON OPERATOR pg_catalog.<(int8, uint2) IS 'int8_uint2_lt';



CREATE OPERATOR pg_catalog.<=(
leftarg = int8, rightarg = uint2, procedure = int8_uint2_le,
commutator=operator(pg_catalog.<=)
);
COMMENT ON OPERATOR pg_catalog.<=(int8, uint2) IS 'int8_uint2_le';


CREATE OPERATOR pg_catalog.>(
leftarg = int8, rightarg = uint2, procedure = int8_uint2_gt,
commutator=operator(pg_catalog.>)
);
COMMENT ON OPERATOR pg_catalog.>=(int8, uint2) IS 'int8_uint2_gt';



CREATE OPERATOR pg_catalog.>=(
leftarg = int8, rightarg = uint2, procedure = int8_uint2_ge,
commutator=operator(pg_catalog.>=)
);
COMMENT ON OPERATOR pg_catalog.>=(int8, uint2) IS 'int8_uint2_ge';

-------------------int8 uint2---------------
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


drop aggregate if exists pg_catalog.bit_and(uint2);
create aggregate pg_catalog.bit_and(uint2) (SFUNC=uint2and, cFUNC = uint2and, STYPE= uint2);

drop aggregate if exists pg_catalog.bit_or(uint2);
create aggregate pg_catalog.bit_or(uint2) (SFUNC=uint2or, cFUNC = uint2or,  STYPE= uint2);


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
DROP FUNCTION IF EXISTS pg_catalog.hashuint4(uint4,uint4) CASCADE;
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
CREATE FUNCTION pg_catalog.hashuint4 (
uint4,uint4
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
) RETURNS int2 LANGUAGE C STRICT IMMUTABLE  as '$libdir/dolphin',  'uint4um';
DROP FUNCTION IF EXISTS pg_catalog.uint4not(uint4) CASCADE;
CREATE FUNCTION pg_catalog.uint4not (
uint4
) RETURNS uint4 LANGUAGE C  IMMUTABLE  as '$libdir/dolphin',  'uint4not';

DROP FUNCTION IF EXISTS pg_catalog.uint4_avg_accum(bigint[], uint4) CASCADE;

CREATE OR REPLACE FUNCTION pg_catalog.uint4_avg_accum(bigint[], uint4)
 RETURNS bigint[] LANGUAGE C STRICT AS  '$libdir/dolphin',  'uint4_avg_accum';

DROP FUNCTION IF EXISTS pg_catalog.uint4_accum(numeric[], uint4) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.uint4_accum(numeric[], uint4)
 RETURNS numeric[] LANGUAGE C STRICT AS  '$libdir/dolphin',  'uint4_accum';

DROP FUNCTION IF EXISTS pg_catalog.uint4_sum(int8, uint4) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.uint4_sum(int8, uint4)
RETURNS int8 LANGUAGE C  AS  '$libdir/dolphin',  'uint4_sum';

DROP FUNCTION IF EXISTS pg_catalog.uint4_list_agg_transfn(internal, uint4, text) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.uint4_list_agg_transfn(internal, uint4, text)
 RETURNS internal LANGUAGE C  AS	'$libdir/dolphin',	'uint4_list_agg_transfn';

DROP FUNCTION IF EXISTS pg_catalog.uint4_list_agg_noarg2_transfn(internal, uint4) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.uint4_list_agg_noarg2_transfn(internal, uint4)
 RETURNS internal LANGUAGE C  AS	'$libdir/dolphin',	'uint4_list_agg_noarg2_transfn';

 
 DROP FUNCTION IF EXISTS pg_catalog.uint4_bool(uint4) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.uint4_bool(uint4)
RETURNS bool LANGUAGE C  AS  '$libdir/dolphin',  'uint4_bool';


DROP FUNCTION IF EXISTS pg_catalog.bool_uint4(bool) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.bool_uint4(bool)
RETURNS uint4 LANGUAGE C  AS  '$libdir/dolphin',  'bool_uint4';
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
commutator=operator(pg_catalog.<>),
restrict = neqsel, join = neqjoinsel
);
COMMENT ON OPERATOR pg_catalog.<>(uint4, uint4) IS 'uint4ne';


CREATE OPERATOR pg_catalog.<(
leftarg = uint4, rightarg = uint4, procedure = uint4lt,
commutator=operator(pg_catalog.<),
restrict = scalarltsel, join = scalarltjoinsel
);
COMMENT ON OPERATOR pg_catalog.<(uint4, uint4) IS 'uint4lt';



CREATE OPERATOR pg_catalog.<=(
leftarg = uint4, rightarg = uint4, procedure = uint4le,
commutator=operator(pg_catalog.<=),
restrict = scalarltsel, join = scalarltjoinsel
);
COMMENT ON OPERATOR pg_catalog.<=(uint4, uint4) IS 'uint4le';




CREATE OPERATOR pg_catalog.>(
leftarg = uint4, rightarg = uint4, procedure = uint4gt,
commutator=operator(pg_catalog.>),
restrict = scalargtsel, join = scalargtjoinsel
);
COMMENT ON OPERATOR pg_catalog.>(uint4, uint4) IS 'uint4gt';




CREATE OPERATOR pg_catalog.>=(
leftarg = uint4, rightarg = uint4, procedure = uint4ge,
commutator=operator(pg_catalog.>=),
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


create or replace function uint4(text)
returns uint4
as $$ select cast(to_number($1) as uint4)$$
language sql IMMUTABLE strict;


CREATE CAST (TEXT AS uint4) WITH FUNCTION uint4(TEXT) AS IMPLICIT;

-------------------uint4 int 1---------------
DROP FUNCTION IF EXISTS pg_catalog.uint4_pl_int1(uint4,int1) CASCADE;
CREATE FUNCTION pg_catalog.uint4_pl_int1 (
uint4,int1
) RETURNS int2 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint4_pl_int1';

DROP FUNCTION IF EXISTS pg_catalog.uint4_mi_int1(uint4,int1) CASCADE;
CREATE FUNCTION pg_catalog.uint4_mi_int1 (
uint4,int1
) RETURNS int2 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint4_mi_int1';

DROP FUNCTION IF EXISTS pg_catalog.uint4_mul_int1(uint4,int1) CASCADE;
CREATE FUNCTION pg_catalog.uint4_mul_int1 (
uint4,int1
) RETURNS uint8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint4_mul_int1';


DROP FUNCTION IF EXISTS pg_catalog.uint4_div_int1(uint4,int1) CASCADE;
CREATE FUNCTION pg_catalog.uint4_div_int1 (
uint4,int1
) RETURNS float8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint4_div_int1';

DROP FUNCTION IF EXISTS pg_catalog.uint4_int1_mod(uint4,int1) CASCADE;
CREATE FUNCTION pg_catalog.uint4_int1_mod (
uint4,int1
) RETURNS float8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint4_int1_mod';

DROP FUNCTION IF EXISTS pg_catalog.uint4_int1_eq(uint4,int1) CASCADE;
CREATE FUNCTION pg_catalog.uint4_int1_eq (
uint4,int1
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint4_int1_eq';


DROP FUNCTION IF EXISTS pg_catalog.uint4_int1_ne(uint4,int1) CASCADE;
CREATE FUNCTION pg_catalog.uint4_int1_ne (
uint4,int1
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint4_int1_ne';


DROP FUNCTION IF EXISTS pg_catalog.uint4_int1_lt(uint4,int1) CASCADE;
CREATE FUNCTION pg_catalog.uint4_int1_lt (
uint4,int1
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint4_int1_lt';


DROP FUNCTION IF EXISTS pg_catalog.uint4_int1_le(uint4,int1) CASCADE;
CREATE FUNCTION pg_catalog.uint4_int1_le (
uint4,int1
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint4_int1_le';


DROP FUNCTION IF EXISTS pg_catalog.uint4_int1_gt(uint4,int1) CASCADE;
CREATE FUNCTION pg_catalog.uint4_int1_gt (
uint4,int1
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint4_int1_gt';


DROP FUNCTION IF EXISTS pg_catalog.uint4_int1_ge(uint4,int1) CASCADE;
CREATE FUNCTION pg_catalog.uint4_int1_ge (
uint4,int1
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint4_int1_ge';

CREATE OPERATOR pg_catalog.+(
leftarg = uint4, rightarg = int1, procedure = uint4_pl_int1,
commutator=operator(pg_catalog.+)
);
COMMENT ON OPERATOR pg_catalog.+(uint4, int1) IS 'uint4_pl_int1';


CREATE OPERATOR pg_catalog.-(
leftarg = uint4, rightarg = int1, procedure = uint4_mi_int1,
commutator=operator(pg_catalog.-)
);
COMMENT ON OPERATOR pg_catalog.-(uint4, int1) IS 'uint4_mi_int1';


CREATE OPERATOR pg_catalog.*(
leftarg = uint4, rightarg = int1, procedure = uint4_mul_int1,
commutator=operator(pg_catalog.*)
);
COMMENT ON OPERATOR pg_catalog.*(uint4, int1) IS 'uint4_mul_int1';


CREATE OPERATOR pg_catalog./(
leftarg = uint4, rightarg = int1, procedure = uint4_div_int1,
commutator=operator(pg_catalog./)
);
COMMENT ON OPERATOR pg_catalog./(uint4, int1) IS 'uint4_div_int1';

CREATE OPERATOR pg_catalog.%(
leftarg = uint4, rightarg = int1, procedure = uint4_int1_mod,
commutator=operator(pg_catalog.%)
);
COMMENT ON OPERATOR pg_catalog.%(uint4, int1) IS 'uint4_int1_mod';

CREATE OPERATOR pg_catalog.=(
leftarg = uint4, rightarg = int1, procedure = uint4_int1_eq,
commutator=operator(pg_catalog.=)
);
COMMENT ON OPERATOR pg_catalog.=(uint4, int1) IS 'uint4_int1_eq';



CREATE OPERATOR pg_catalog.<>(
leftarg = uint4, rightarg = int1, procedure = uint4_int1_ne,
commutator=operator(pg_catalog.<>)
);
COMMENT ON OPERATOR pg_catalog.<>(uint4, int1) IS 'uint4_int1_ne';


CREATE OPERATOR pg_catalog.<(
leftarg = uint4, rightarg = int1, procedure = uint4_int1_lt,
commutator=operator(pg_catalog.<)
);
COMMENT ON OPERATOR pg_catalog.<(uint4, int1) IS 'uint4_int1_lt';


CREATE OPERATOR pg_catalog.<=(
leftarg = uint4, rightarg = int1, procedure = uint4_int1_le,
commutator=operator(pg_catalog.<=)
);
COMMENT ON OPERATOR pg_catalog.<=(uint4, int1) IS 'uint4_int1_le';

CREATE OPERATOR pg_catalog.>(
leftarg = uint4, rightarg = int1, procedure = uint4_int1_gt,
commutator=operator(pg_catalog.>)
);
COMMENT ON OPERATOR pg_catalog.>(uint4, int1) IS 'uint4_int1_gt';


CREATE OPERATOR pg_catalog.>=(
leftarg = uint4, rightarg = int1, procedure = uint4_int1_ge,
commutator=operator(pg_catalog.>=)
);
COMMENT ON OPERATOR pg_catalog.>=(uint4, int1) IS 'uint4_int1_ge';
-------------------uint4 int 1---------------


-------------------uint4 int2---------------
DROP FUNCTION IF EXISTS pg_catalog.uint4_pl_int2(uint4,int2) CASCADE;
CREATE FUNCTION pg_catalog.uint4_pl_int2 (
uint4,int2
) RETURNS int4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint4_pl_int2';

DROP FUNCTION IF EXISTS pg_catalog.uint4_mi_int2(uint4,int2) CASCADE;
CREATE FUNCTION pg_catalog.uint4_mi_int2 (
uint4,int2
) RETURNS int4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint4_mi_int2';

DROP FUNCTION IF EXISTS pg_catalog.uint4_mul_int2(uint4,int2) CASCADE;
CREATE FUNCTION pg_catalog.uint4_mul_int2 (
uint4,int2
) RETURNS int4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint4_mul_int2';


DROP FUNCTION IF EXISTS pg_catalog.uint4_div_int2(uint4,int2) CASCADE;
CREATE FUNCTION pg_catalog.uint4_div_int2 (
uint4,int2
) RETURNS uint8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint4_div_int2';

DROP FUNCTION IF EXISTS pg_catalog.uint4_int2_mod(uint4,int2) CASCADE;
CREATE FUNCTION pg_catalog.uint4_int2_mod (
uint4,int2
) RETURNS uint8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint4_int2_mod';

DROP FUNCTION IF EXISTS pg_catalog.uint4_int2_eq(uint4,int2) CASCADE;
CREATE FUNCTION pg_catalog.uint4_int2_eq (
uint4,int2
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint4_int2_eq';


DROP FUNCTION IF EXISTS pg_catalog.uint4_int2_ne(uint4,int2) CASCADE;
CREATE FUNCTION pg_catalog.uint4_int2_ne (
uint4,int2
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint4_int2_ne';


DROP FUNCTION IF EXISTS pg_catalog.uint4_int2_lt(uint4,int2) CASCADE;
CREATE FUNCTION pg_catalog.uint4_int2_lt (
uint4,int2
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint4_int2_lt';


DROP FUNCTION IF EXISTS pg_catalog.uint4_int2_le(uint4,int2) CASCADE;
CREATE FUNCTION pg_catalog.uint4_int2_le (
uint4,int2
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint4_int2_le';


DROP FUNCTION IF EXISTS pg_catalog.uint4_int2_gt(uint4,int2) CASCADE;
CREATE FUNCTION pg_catalog.uint4_int2_gt (
uint4,int2
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint4_int2_gt';


DROP FUNCTION IF EXISTS pg_catalog.uint4_int2_ge(uint4,int2) CASCADE;
CREATE FUNCTION pg_catalog.uint4_int2_ge (
uint4,int2
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint4_int2_ge';


CREATE OPERATOR pg_catalog.+(
leftarg = uint4, rightarg = int2, procedure = uint4_pl_int2,
commutator=operator(pg_catalog.+)
);
COMMENT ON OPERATOR pg_catalog.+(uint4, int2) IS 'uint4_pl_int2';


CREATE OPERATOR pg_catalog.-(
leftarg = uint4, rightarg = int2, procedure = uint4_mi_int2,
commutator=operator(pg_catalog.-)
);
COMMENT ON OPERATOR pg_catalog.-(uint4, int2) IS 'uint4_mi_int2';


CREATE OPERATOR pg_catalog.*(
leftarg = uint4, rightarg = int2, procedure = uint4_mul_int2,
commutator=operator(pg_catalog.*)
);
COMMENT ON OPERATOR pg_catalog.*(uint4, int2) IS 'uint4_mul_int2';


CREATE OPERATOR pg_catalog./(
leftarg = uint4, rightarg = int2, procedure = uint4_div_int2,
commutator=operator(pg_catalog./)
);
COMMENT ON OPERATOR pg_catalog./(uint4, int2) IS 'uint4_div_int2';

CREATE OPERATOR pg_catalog.%(
leftarg = uint4, rightarg = int2, procedure = uint4_int2_mod,
commutator=operator(pg_catalog.%)
);
COMMENT ON OPERATOR pg_catalog.%(uint4, int2) IS 'uint4_int2_mod';

CREATE OPERATOR pg_catalog.=(
leftarg = uint4, rightarg = int2, procedure = uint4_int2_eq,
commutator=operator(pg_catalog.=)
);
COMMENT ON OPERATOR pg_catalog.=(uint4, int2) IS 'uint4_int2_eq';



CREATE OPERATOR pg_catalog.<>(
leftarg = uint4, rightarg = int2, procedure = uint4_int2_ne,
commutator=operator(pg_catalog.<>)
);
COMMENT ON OPERATOR pg_catalog.<>(uint4, int2) IS 'uint4_int2_ne';




CREATE OPERATOR pg_catalog.<(
leftarg = uint4, rightarg = int2, procedure = uint4_int2_lt,
commutator=operator(pg_catalog.<)
);
COMMENT ON OPERATOR pg_catalog.<(uint4, int2) IS 'uint4_int2_lt';



CREATE OPERATOR pg_catalog.<=(
leftarg = uint4, rightarg = int2, procedure = uint4_int2_le,
commutator=operator(pg_catalog.<=)
);
COMMENT ON OPERATOR pg_catalog.<=(uint4, int2) IS 'uint4_int2_le';


CREATE OPERATOR pg_catalog.>(
leftarg = uint4, rightarg = int2, procedure = uint4_int2_gt,
commutator=operator(pg_catalog.>)
);
COMMENT ON OPERATOR pg_catalog.>(uint4, int2) IS 'uint4_int2_gt';

CREATE OPERATOR pg_catalog.>=(
leftarg = uint4, rightarg = int2, procedure = uint4_int2_ge,
commutator=operator(pg_catalog.>=)
);
COMMENT ON OPERATOR pg_catalog.>=(uint4, int2) IS 'uint4_int2_ge';

-------------------uint4 int2---------------
-------------------uint4 int4---------------
DROP FUNCTION IF EXISTS pg_catalog.uint4_pl_int4(uint4,int4) CASCADE;
CREATE FUNCTION pg_catalog.uint4_pl_int4 (
uint4,int4
) RETURNS int8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint4_pl_int4';

DROP FUNCTION IF EXISTS pg_catalog.uint4_mi_int4(uint4,int4) CASCADE;
CREATE FUNCTION pg_catalog.uint4_mi_int4 (
uint4,int4
) RETURNS int8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint4_mi_int4';
DROP FUNCTION IF EXISTS pg_catalog.uint4_mul_int4(uint4,int4) CASCADE;
CREATE FUNCTION pg_catalog.uint4_mul_int4 (
uint4,int4
) RETURNS uint8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint4_mul_int4';


DROP FUNCTION IF EXISTS pg_catalog.uint4_div_int4(uint4,int4) CASCADE;
CREATE FUNCTION pg_catalog.uint4_div_int4 (
uint4,int4
) RETURNS float8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint4_div_int4';

DROP FUNCTION IF EXISTS pg_catalog.uint4_int4_mod(uint4,int4) CASCADE;
CREATE FUNCTION pg_catalog.uint4_int4_mod (
uint4,int4
) RETURNS float8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint4_int4_mod';

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
commutator=operator(pg_catalog.=)
);
COMMENT ON OPERATOR pg_catalog.=(uint4, int4) IS 'uint4_int4_eq';


CREATE OPERATOR pg_catalog.<>(
leftarg = uint4, rightarg = int4, procedure = uint4_int4_ne,
commutator=operator(pg_catalog.<>)
);
COMMENT ON OPERATOR pg_catalog.<>(uint4, int4) IS 'uint4_int4_ne';



CREATE OPERATOR pg_catalog.<(
leftarg = uint4, rightarg = int4, procedure = uint4_int4_lt,
commutator=operator(pg_catalog.<)
);
COMMENT ON OPERATOR pg_catalog.<(uint4, int4) IS 'uint4_int4_lt';


CREATE OPERATOR pg_catalog.<=(
leftarg = uint4, rightarg = int4, procedure = uint4_int4_le,
commutator=operator(pg_catalog.<=)
);
COMMENT ON OPERATOR pg_catalog.<=(uint4, int4) IS 'uint4_int4_le';

CREATE OPERATOR pg_catalog.>(
leftarg = uint4, rightarg = int4, procedure = uint4_int4_gt,
commutator=operator(pg_catalog.>)
);
COMMENT ON OPERATOR pg_catalog.>(uint4, int4) IS 'uint4_int4_gt';


CREATE OPERATOR pg_catalog.>=(
leftarg = uint4, rightarg = int4, procedure = uint4_int4_ge,
commutator=operator(pg_catalog.>=)
);
COMMENT ON OPERATOR pg_catalog.>=(uint4, int4) IS 'uint4_int4_ge';

-------------------uint4 int4---------------


-------------------uint4 int8---------------

DROP FUNCTION IF EXISTS pg_catalog.uint4_pl_int8(uint4,int8) CASCADE;
CREATE FUNCTION pg_catalog.uint4_pl_int8 (
uint4,int8
) RETURNS int16 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint4_pl_int8';

DROP FUNCTION IF EXISTS pg_catalog.uint4_mi_int8(uint4,int8) CASCADE;
CREATE FUNCTION pg_catalog.uint4_mi_int8 (
uint4,int8
) RETURNS int16 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint4_mi_int8';

DROP FUNCTION IF EXISTS pg_catalog.uint4_mul_int8(uint4,int8) CASCADE;
CREATE FUNCTION pg_catalog.uint4_mul_int8 (
uint4,int8
) RETURNS int16 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint4_mul_int8';


DROP FUNCTION IF EXISTS pg_catalog.uint4_div_int8(uint4,int8) CASCADE;
CREATE FUNCTION pg_catalog.uint4_div_int8 (
uint4,int8
) RETURNS float8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint4_div_int8';

DROP FUNCTION IF EXISTS pg_catalog.uint4_int8_mod(uint4,int8) CASCADE;
CREATE FUNCTION pg_catalog.uint4_int8_mod (
uint4,int8
) RETURNS float8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint4_int8_mod';

DROP FUNCTION IF EXISTS pg_catalog.uint4_int8_eq(uint4,int8) CASCADE;
CREATE FUNCTION pg_catalog.uint4_int8_eq (
uint4,int8
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint4_int8_eq';


DROP FUNCTION IF EXISTS pg_catalog.uint4_int8_ne(uint4,int8) CASCADE;
CREATE FUNCTION pg_catalog.uint4_int8_ne (
uint4,int8
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint4_int8_ne';


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


CREATE OPERATOR pg_catalog.+(
leftarg = uint4, rightarg = int8, procedure = uint4_pl_int8,
commutator=operator(pg_catalog.+)
);
COMMENT ON OPERATOR pg_catalog.+(uint4, int8) IS 'uint4_pl_int8';

CREATE OPERATOR pg_catalog.-(
leftarg = uint4, rightarg = int8, procedure = uint4_mi_int8,
commutator=operator(pg_catalog.-)
);
COMMENT ON OPERATOR pg_catalog.-(uint4, int8) IS 'uint4_mi_int8';

CREATE OPERATOR pg_catalog.*(
leftarg = uint4, rightarg = int8, procedure = uint4_mul_int8,
commutator=operator(pg_catalog.*)
);
COMMENT ON OPERATOR pg_catalog.*(uint4, int8) IS 'uint4_mul_int8';

CREATE OPERATOR pg_catalog./(
leftarg = uint4, rightarg = int8, procedure = uint4_div_int8,
commutator=operator(pg_catalog./)
);
COMMENT ON OPERATOR pg_catalog./(uint4, int8) IS 'uint4_div_int8';

CREATE OPERATOR pg_catalog.%(
leftarg = uint4, rightarg = int8, procedure = uint4_int8_mod,
commutator=operator(pg_catalog.%)
);
COMMENT ON OPERATOR pg_catalog.%(uint4, int8) IS 'uint4_int8_mod';

CREATE OPERATOR pg_catalog.=(
leftarg = uint4, rightarg = int8, procedure = uint4_int8_eq,
commutator=operator(pg_catalog.=)
);
COMMENT ON OPERATOR pg_catalog.=(uint4, int8) IS 'uint4_int8_eq';


CREATE OPERATOR pg_catalog.<>(
leftarg = uint4, rightarg = int8, procedure = uint4_int8_ne,
commutator=operator(pg_catalog.<>)
);
COMMENT ON OPERATOR pg_catalog.<>(uint4, int8) IS 'uint4_int8_ne';


CREATE OPERATOR pg_catalog.<(
leftarg = uint4, rightarg = int8, procedure = uint4_int8_lt,
commutator=operator(pg_catalog.<)
);
COMMENT ON OPERATOR pg_catalog.<(uint4, int8) IS 'uint4_int8_lt';

CREATE OPERATOR pg_catalog.<=(
leftarg = uint4, rightarg = int8, procedure = uint4_int8_le,
commutator=operator(pg_catalog.<=)
);
COMMENT ON OPERATOR pg_catalog.<=(uint4, int8) IS 'uint4_int8_le';

CREATE OPERATOR pg_catalog.>(
leftarg = uint4, rightarg = int8, procedure = uint4_int8_gt,
commutator=operator(pg_catalog.>)
);
COMMENT ON OPERATOR pg_catalog.>(uint4, int8) IS 'uint4_int8_gt';


CREATE OPERATOR pg_catalog.>=(
leftarg = uint4, rightarg = int8, procedure = uint4_int8_ge,
commutator=operator(pg_catalog.>=)
);
COMMENT ON OPERATOR pg_catalog.>=(uint4, int8) IS 'uint4_int8_ge';


-------------------uint4 int8---------------
	
-------------------uint4 uint2---------------

DROP FUNCTION IF EXISTS pg_catalog.uint4_pl_uint2(uint4,uint2) CASCADE;
CREATE FUNCTION pg_catalog.uint4_pl_uint2 (
uint4,uint2
) RETURNS uint4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint4_pl_uint2';

DROP FUNCTION IF EXISTS pg_catalog.uint4_mi_uint2(uint4,uint2) CASCADE;
CREATE FUNCTION pg_catalog.uint4_mi_uint2 (
uint4,uint2
) RETURNS int4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint4_mi_uint2';
DROP FUNCTION IF EXISTS pg_catalog.uint4_mul_uint2(uint4,uint2) CASCADE;
CREATE FUNCTION pg_catalog.uint4_mul_uint2 (
uint4,uint2
) RETURNS uint4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint4_mul_uint2';


DROP FUNCTION IF EXISTS pg_catalog.uint4_div_uint2(uint4,uint2) CASCADE;
CREATE FUNCTION pg_catalog.uint4_div_uint2 (
uint4,uint2
) RETURNS float8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint4_div_uint2';

DROP FUNCTION IF EXISTS pg_catalog.uint4_uint2_mod(uint4,uint2) CASCADE;
CREATE FUNCTION pg_catalog.uint4_uint2_mod (
uint4,uint2
) RETURNS float8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint4_uint2_mod';


DROP FUNCTION IF EXISTS pg_catalog.uint4_uint2_eq(uint4,uint2) CASCADE;
CREATE FUNCTION pg_catalog.uint4_uint2_eq (
uint4,uint2
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint4_uint2_eq';


DROP FUNCTION IF EXISTS pg_catalog.uint4_uint2_ne(uint4,uint2) CASCADE;
CREATE FUNCTION pg_catalog.uint4_uint2_ne (
uint4,uint2
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint4_uint2_ne';


DROP FUNCTION IF EXISTS pg_catalog.uint4_uint2_lt(uint4,uint2) CASCADE;
CREATE FUNCTION pg_catalog.uint4_uint2_lt (
uint4,uint2
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint4_uint2_lt';


DROP FUNCTION IF EXISTS pg_catalog.uint4_uint2_le(uint4,uint2) CASCADE;
CREATE FUNCTION pg_catalog.uint4_uint2_le (
uint4,uint2
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint4_uint2_le';


DROP FUNCTION IF EXISTS pg_catalog.uint4_uint2_gt(uint4,uint2) CASCADE;
CREATE FUNCTION pg_catalog.uint4_uint2_gt (
uint4,uint2
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint4_uint2_gt';


DROP FUNCTION IF EXISTS pg_catalog.uint4_uint2_ge(uint4,uint2) CASCADE;
CREATE FUNCTION pg_catalog.uint4_uint2_ge (
uint4,uint2
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint4_uint2_ge';


CREATE OPERATOR pg_catalog.+(
leftarg = uint4, rightarg = uint2, procedure = uint4_pl_uint2,
commutator=operator(pg_catalog.+)
);
COMMENT ON OPERATOR pg_catalog.+(uint4, uint2) IS 'uint4_pl_uint2';

CREATE OPERATOR pg_catalog.-(
leftarg = uint4, rightarg = uint2, procedure = uint4_mi_uint2,
commutator=operator(pg_catalog.-)
);
COMMENT ON OPERATOR pg_catalog.-(uint4, uint2) IS 'uint4_mi_uint2';

CREATE OPERATOR pg_catalog.*(
leftarg = uint4, rightarg = uint2, procedure = uint4_mul_uint2,
commutator=operator(pg_catalog.*)
);
COMMENT ON OPERATOR pg_catalog.*(uint4, uint2) IS 'uint4_mul_uint2';


CREATE OPERATOR pg_catalog./(
leftarg = uint4, rightarg = uint2, procedure = uint4_div_uint2,
commutator=operator(pg_catalog./)
);
COMMENT ON OPERATOR pg_catalog./(uint4, uint2) IS 'uint4_div_uint2';

CREATE OPERATOR pg_catalog.%(
leftarg = uint4, rightarg = uint2, procedure = uint4_uint2_mod,
commutator=operator(pg_catalog.%)
);
COMMENT ON OPERATOR pg_catalog.%(uint4, uint2) IS 'uint4_uint2_mod';

CREATE OPERATOR pg_catalog.=(
leftarg = uint4, rightarg = uint2, procedure = uint4_uint2_eq,
commutator=operator(pg_catalog.=)
);
COMMENT ON OPERATOR pg_catalog.=(uint4, uint2) IS 'uint4_uint2_eq';


CREATE OPERATOR pg_catalog.<>(
leftarg = uint4, rightarg = uint2, procedure = uint4_uint2_ne,
commutator=operator(pg_catalog.<>)
);
COMMENT ON OPERATOR pg_catalog.<>(uint4, uint2) IS 'uint4_uint2_ne';


CREATE OPERATOR pg_catalog.<(
leftarg = uint4, rightarg = uint2, procedure = uint4_uint2_lt,
commutator=operator(pg_catalog.<)
);
COMMENT ON OPERATOR pg_catalog.<(uint4, uint2) IS 'uint4_uint2_lt';


CREATE OPERATOR pg_catalog.<=(
leftarg = uint4, rightarg = uint2, procedure = uint4_uint2_le,
commutator=operator(pg_catalog.<=)
);
COMMENT ON OPERATOR pg_catalog.<=(uint4, uint2) IS 'uint4_uint2_le';


CREATE OPERATOR pg_catalog.>(
leftarg = uint4, rightarg = uint2, procedure = uint4_uint2_gt,
commutator=operator(pg_catalog.>)
);
COMMENT ON OPERATOR pg_catalog.>=(uint4, uint2) IS 'uint4_uint2_gt';


CREATE OPERATOR pg_catalog.>=(
leftarg = uint4, rightarg = uint2, procedure = uint4_uint2_ge,
commutator=operator(pg_catalog.>=)
);
COMMENT ON OPERATOR pg_catalog.>=(uint4, uint2) IS 'uint4_uint2_ge';


-------------------uint4 uint2---------------
-------------------uint4 uint1---------------
DROP FUNCTION IF EXISTS pg_catalog.uint4_pl_uint1(uint4,uint1) CASCADE;
CREATE FUNCTION pg_catalog.uint4_pl_uint1 (
uint4,uint1
) RETURNS uint8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint4_pl_uint1';

DROP FUNCTION IF EXISTS pg_catalog.uint4_mi_uint1(uint4,uint1) CASCADE;
CREATE FUNCTION pg_catalog.uint4_mi_uint1 (
uint4,uint1
) RETURNS int8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint4_mi_uint1';
DROP FUNCTION IF EXISTS pg_catalog.uint4_mul_uint1(uint4,uint1) CASCADE;
CREATE FUNCTION pg_catalog.uint4_mul_uint1 (
uint4,uint1
) RETURNS uint8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint4_mul_uint1';


DROP FUNCTION IF EXISTS pg_catalog.uint4_div_uint1(uint4,uint1) CASCADE;
CREATE FUNCTION pg_catalog.uint4_div_uint1 (
uint4,uint1
) RETURNS float8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint4_div_uint1';

DROP FUNCTION IF EXISTS pg_catalog.uint4_uint1_mod(uint4,uint1) CASCADE;
CREATE FUNCTION pg_catalog.uint4_uint1_mod (
uint4,uint1
) RETURNS uint8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint4_uint1_mod';

DROP FUNCTION IF EXISTS pg_catalog.uint4_uint1_eq(uint4,uint1) CASCADE;
CREATE FUNCTION pg_catalog.uint4_uint1_eq (
uint4,uint1
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint4_uint1_eq';


DROP FUNCTION IF EXISTS pg_catalog.uint4_uint1_ne(uint4,uint1) CASCADE;
CREATE FUNCTION pg_catalog.uint4_uint1_ne (
uint4,uint1
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint4_uint1_ne';


DROP FUNCTION IF EXISTS pg_catalog.uint4_uint1_lt(uint4,uint1) CASCADE;
CREATE FUNCTION pg_catalog.uint4_uint1_lt (
uint4,uint1
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint4_uint1_lt';


DROP FUNCTION IF EXISTS pg_catalog.uint4_uint1_le(uint4,uint1) CASCADE;
CREATE FUNCTION pg_catalog.uint4_uint1_le (
uint4,uint1
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint4_uint1_le';


DROP FUNCTION IF EXISTS pg_catalog.uint4_uint1_gt(uint4,uint1) CASCADE;
CREATE FUNCTION pg_catalog.uint4_uint1_gt (
uint4,uint1
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint4_uint1_gt';


DROP FUNCTION IF EXISTS pg_catalog.uint4_uint1_ge(uint4,uint1) CASCADE;
CREATE FUNCTION pg_catalog.uint4_uint1_ge (
uint4,uint1
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint4_uint1_ge';

CREATE OPERATOR pg_catalog.+(
leftarg = uint4, rightarg = uint1, procedure = uint4_pl_uint1,
commutator=operator(pg_catalog.+)
);
COMMENT ON OPERATOR pg_catalog.+(uint4, uint1) IS 'uint4_pl_uint1';


CREATE OPERATOR pg_catalog.-(
leftarg = uint4, rightarg = uint1, procedure = uint4_mi_uint1,
commutator=operator(pg_catalog.-)
);
COMMENT ON OPERATOR pg_catalog.-(uint4, uint1) IS 'uint4_mi_uint1';


CREATE OPERATOR pg_catalog.*(
leftarg = uint4, rightarg = uint1, procedure = uint4_mul_uint1,
commutator=operator(pg_catalog.*)
);
COMMENT ON OPERATOR pg_catalog.*(uint4, uint1) IS 'uint4_mul_uint1';


CREATE OPERATOR pg_catalog./(
leftarg = uint4, rightarg = uint1, procedure = uint4_div_uint1,
commutator=operator(pg_catalog./)
);
COMMENT ON OPERATOR pg_catalog./(uint4, uint1) IS 'uint4_div_uint1';

CREATE OPERATOR pg_catalog.%(
leftarg = uint4, rightarg = uint1, procedure = uint4_uint1_mod,
commutator=operator(pg_catalog.%)
);
COMMENT ON OPERATOR pg_catalog.%(uint4, uint1) IS 'uint4_uint1_mod';

CREATE OPERATOR pg_catalog.=(
leftarg = uint4, rightarg = uint1, procedure = uint4_uint1_eq,
commutator=operator(pg_catalog.=)
);
COMMENT ON OPERATOR pg_catalog.=(uint4, uint1) IS 'uint4_uint1_eq';


CREATE OPERATOR pg_catalog.<>(
leftarg = uint4, rightarg = uint1, procedure = uint4_uint1_ne,
commutator=operator(pg_catalog.<>)
);
COMMENT ON OPERATOR pg_catalog.<>(uint4, uint1) IS 'uint4_uint1_ne';


CREATE OPERATOR pg_catalog.<(
leftarg = uint4, rightarg = uint1, procedure = uint4_uint1_lt,
commutator=operator(pg_catalog.<)
);
COMMENT ON OPERATOR pg_catalog.<(uint4, uint1) IS 'uint4_uint1_lt';


CREATE OPERATOR pg_catalog.<=(
leftarg = uint4, rightarg = uint1, procedure = uint4_uint1_le,
commutator=operator(pg_catalog.<=)
);
COMMENT ON OPERATOR pg_catalog.<=(uint4, uint1) IS 'uint4_uint1_le';


CREATE OPERATOR pg_catalog.>(
leftarg = uint4, rightarg = uint1, procedure = uint4_uint1_gt,
commutator=operator(pg_catalog.>)
);
COMMENT ON OPERATOR pg_catalog.>=(uint4, uint1) IS 'uint4_uint1_gt';


CREATE OPERATOR pg_catalog.>=(
leftarg = uint4, rightarg = uint1, procedure = uint4_uint1_ge,
commutator=operator(pg_catalog.>=)
);
COMMENT ON OPERATOR pg_catalog.>=(uint4, uint1) IS 'uint4_uint1_ge';




-------------------uint4 uint1---------------
-------------------uint4 uint8---------------

DROP FUNCTION IF EXISTS pg_catalog.uint4_pl_uint8(uint4,uint8) CASCADE;
CREATE FUNCTION pg_catalog.uint4_pl_uint8 (
uint4,uint8
) RETURNS int16 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint4_pl_uint8';

DROP FUNCTION IF EXISTS pg_catalog.uint4_mi_uint8(uint4,uint8) CASCADE;
CREATE FUNCTION pg_catalog.uint4_mi_uint8 (
uint4,uint8
) RETURNS int16 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint4_mi_uint8';
DROP FUNCTION IF EXISTS pg_catalog.uint4_mul_uint8(uint4,uint8) CASCADE;
CREATE FUNCTION pg_catalog.uint4_mul_uint8 (
uint4,uint8
) RETURNS int16 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint4_mul_uint8';


DROP FUNCTION IF EXISTS pg_catalog.uint4_div_uint8(uint4,uint8) CASCADE;
CREATE FUNCTION pg_catalog.uint4_div_uint8 (
uint4,uint8
) RETURNS float8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint4_div_uint8';

DROP FUNCTION IF EXISTS pg_catalog.uint4_uint8_mod(uint4,uint8) CASCADE;
CREATE FUNCTION pg_catalog.uint4_uint8_mod (
uint4,uint8
) RETURNS float8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint4_uint8_mod';


DROP FUNCTION IF EXISTS pg_catalog.uint4_uint8_eq(uint4,uint8) CASCADE;
CREATE FUNCTION pg_catalog.uint4_uint8_eq (
uint4,uint8
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint4_uint8_eq';


DROP FUNCTION IF EXISTS pg_catalog.uint4_uint8_ne(uint4,uint8) CASCADE;
CREATE FUNCTION pg_catalog.uint4_uint8_ne (
uint4,uint8
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint4_uint8_ne';


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

CREATE OPERATOR pg_catalog.+(
leftarg = uint4, rightarg = uint8, procedure = uint4_pl_uint8,
commutator=operator(pg_catalog.+)
);
COMMENT ON OPERATOR pg_catalog.+(uint4, uint8) IS 'uint4_pl_uint8';


CREATE OPERATOR pg_catalog.-(
leftarg = uint4, rightarg = uint8, procedure = uint4_mi_uint8,
commutator=operator(pg_catalog.-)
);
COMMENT ON OPERATOR pg_catalog.-(uint4, uint8) IS 'uint4_mi_uint8';


CREATE OPERATOR pg_catalog.*(
leftarg = uint4, rightarg = uint8, procedure = uint4_mul_uint8,
commutator=operator(pg_catalog.*)
);
COMMENT ON OPERATOR pg_catalog.*(uint4, uint8) IS 'uint4_mul_uint8';


CREATE OPERATOR pg_catalog./(
leftarg = uint4, rightarg = uint8, procedure = uint4_div_uint8,
commutator=operator(pg_catalog./)
);
COMMENT ON OPERATOR pg_catalog./(uint4, uint8) IS 'uint4_div_uint8';

CREATE OPERATOR pg_catalog.%(
leftarg = uint4, rightarg = uint8, procedure = uint4_uint8_mod,
commutator=operator(pg_catalog.%)
);
COMMENT ON OPERATOR pg_catalog.%(uint4, uint8) IS 'uint4_uint8_mod';


CREATE OPERATOR pg_catalog.=(
leftarg = uint4, rightarg = uint8, procedure = uint4_uint8_eq,
commutator=operator(pg_catalog.=)
);
COMMENT ON OPERATOR pg_catalog.=(uint4, uint8) IS 'uint4_uint8_eq';



CREATE OPERATOR pg_catalog.<>(
leftarg = uint4, rightarg = uint8, procedure = uint4_uint8_ne,
commutator=operator(pg_catalog.<>)
);
COMMENT ON OPERATOR pg_catalog.<>(uint4, uint8) IS 'uint4_uint8_ne';




CREATE OPERATOR pg_catalog.<(
leftarg = uint4, rightarg = uint8, procedure = uint4_uint8_lt,
commutator=operator(pg_catalog.<)
);
COMMENT ON OPERATOR pg_catalog.<(uint4, uint8) IS 'uint4_uint8_lt';



CREATE OPERATOR pg_catalog.<=(
leftarg = uint4, rightarg = uint8, procedure = uint4_uint8_le,
commutator=operator(pg_catalog.<=)
);
COMMENT ON OPERATOR pg_catalog.<=(uint4, uint8) IS 'uint4_uint8_le';


CREATE OPERATOR pg_catalog.>(
leftarg = uint4, rightarg = uint8, procedure = uint4_uint8_gt,
commutator=operator(pg_catalog.>)
);
COMMENT ON OPERATOR pg_catalog.>(uint4, uint8) IS 'uint4_uint8_gt';



CREATE OPERATOR pg_catalog.>=(
leftarg = uint4, rightarg = uint8, procedure = uint4_uint8_ge,
commutator=operator(pg_catalog.>=)
);
COMMENT ON OPERATOR pg_catalog.>=(uint4, uint8) IS 'uint4_uint8_ge';


-------------------uint4 uint8---------------





-------------------int1 uint4---------------

DROP FUNCTION IF EXISTS pg_catalog.int1_pl_uint4(int1,uint4) CASCADE;
CREATE FUNCTION pg_catalog.int1_pl_uint4 (
int1,uint4
) RETURNS int2 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int1_pl_uint4';

DROP FUNCTION IF EXISTS pg_catalog.int1_mi_uint4(int1,uint4) CASCADE;
CREATE FUNCTION pg_catalog.int1_mi_uint4 (
int1,uint4
) RETURNS int2 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int1_mi_uint4';
DROP FUNCTION IF EXISTS pg_catalog.int1_mul_uint4(int1,uint4) CASCADE;
CREATE FUNCTION pg_catalog.int1_mul_uint4 (
int1,uint4
) RETURNS int2 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int1_mul_uint4';


DROP FUNCTION IF EXISTS pg_catalog.int1_div_uint4(int1,uint4) CASCADE;
CREATE FUNCTION pg_catalog.int1_div_uint4 (
int1,uint4
) RETURNS float8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int1_div_uint4';

DROP FUNCTION IF EXISTS pg_catalog.int1_uint4_mod(int1,uint4) CASCADE;
CREATE FUNCTION pg_catalog.int1_uint4_mod (
int1,uint4
) RETURNS float8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int1_uint4_mod';

DROP FUNCTION IF EXISTS pg_catalog.int1_uint4_eq(int1,uint4) CASCADE;
CREATE FUNCTION pg_catalog.int1_uint4_eq (
int1,uint4
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int1_uint4_eq';


DROP FUNCTION IF EXISTS pg_catalog.int1_uint4_ne(int1,uint4) CASCADE;
CREATE FUNCTION pg_catalog.int1_uint4_ne (
int1,uint4
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int1_uint4_ne';


DROP FUNCTION IF EXISTS pg_catalog.int1_uint4_lt(int1,uint4) CASCADE;
CREATE FUNCTION pg_catalog.int1_uint4_lt (
int1,uint4
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int1_uint4_lt';


DROP FUNCTION IF EXISTS pg_catalog.int1_uint4_le(int1,uint4) CASCADE;
CREATE FUNCTION pg_catalog.int1_uint4_le (
int1,uint4
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int1_uint4_le';


DROP FUNCTION IF EXISTS pg_catalog.int1_uint4_gt(int1,uint4) CASCADE;
CREATE FUNCTION pg_catalog.int1_uint4_gt (
int1,uint4
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int1_uint4_gt';


DROP FUNCTION IF EXISTS pg_catalog.int1_uint4_ge(int1,uint4) CASCADE;
CREATE FUNCTION pg_catalog.int1_uint4_ge (
int1,uint4
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int1_uint4_ge';

CREATE OPERATOR pg_catalog.+(
leftarg = int1, rightarg = uint4, procedure = int1_pl_uint4,
commutator=operator(pg_catalog.+)
);
COMMENT ON OPERATOR pg_catalog.+(int1, uint4) IS 'int1_pl_uint4';


CREATE OPERATOR pg_catalog.-(
leftarg = int1, rightarg = uint4, procedure = int1_mi_uint4,
commutator=operator(pg_catalog.-)
);
COMMENT ON OPERATOR pg_catalog.-(int1, uint4) IS 'int1_mi_uint4';


CREATE OPERATOR pg_catalog.*(
leftarg = int1, rightarg = uint4, procedure = int1_mul_uint4,
commutator=operator(pg_catalog.*)
);
COMMENT ON OPERATOR pg_catalog.*(int1, uint4) IS 'int1_mul_uint4';


CREATE OPERATOR pg_catalog./(
leftarg = int1, rightarg = uint4, procedure = int1_div_uint4,
commutator=operator(pg_catalog./)
);
COMMENT ON OPERATOR pg_catalog./(int1, uint4) IS 'int1_div_uint4';


CREATE OPERATOR pg_catalog.%(
leftarg = int1, rightarg = uint4, procedure = int1_uint4_mod,
commutator=operator(pg_catalog.%)
);
COMMENT ON OPERATOR pg_catalog.%(int1, uint4) IS 'int1_uint4_mod';


CREATE OPERATOR pg_catalog.=(
leftarg = int1, rightarg = uint4, procedure = int1_uint4_eq,
commutator=operator(pg_catalog.=)
);
COMMENT ON OPERATOR pg_catalog.=(int1, uint4) IS 'int1_uint4_eq';



CREATE OPERATOR pg_catalog.<>(
leftarg = int1, rightarg = uint4, procedure = int1_uint4_ne,
commutator=operator(pg_catalog.<>)
);
COMMENT ON OPERATOR pg_catalog.<>(int1, uint4) IS 'int1_uint4_ne';




CREATE OPERATOR pg_catalog.<(
leftarg = int1, rightarg = uint4, procedure = int1_uint4_lt,
commutator=operator(pg_catalog.<)
);
COMMENT ON OPERATOR pg_catalog.<(int1, uint4) IS 'int1_uint4_lt';



CREATE OPERATOR pg_catalog.<=(
leftarg = int1, rightarg = uint4, procedure = int1_uint4_le,
commutator=operator(pg_catalog.<=)
);
COMMENT ON OPERATOR pg_catalog.<=(int1, uint4) IS 'int1_uint4_le';


CREATE OPERATOR pg_catalog.>(
leftarg = int1, rightarg = uint4, procedure = int1_uint4_gt,
commutator=operator(pg_catalog.>)
);
COMMENT ON OPERATOR pg_catalog.>=(int1, uint4) IS 'int1_uint4_gt';



CREATE OPERATOR pg_catalog.>=(
leftarg = int1, rightarg = uint4, procedure = int1_uint4_ge,
commutator=operator(pg_catalog.>=)
);
COMMENT ON OPERATOR pg_catalog.>=(int1, uint4) IS 'int1_uint4_ge';

-------------------int1 uint4---------------


-------------------int2 uint4---------------



DROP FUNCTION IF EXISTS pg_catalog.int2_pl_uint4(int2,uint4) CASCADE;
CREATE FUNCTION pg_catalog.int2_pl_uint4 (
int2,uint4
) RETURNS int4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int2_pl_uint4';

DROP FUNCTION IF EXISTS pg_catalog.int2_mi_uint4(int2,uint4) CASCADE;
CREATE FUNCTION pg_catalog.int2_mi_uint4 (
int2,uint4
) RETURNS int4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int2_mi_uint4';
DROP FUNCTION IF EXISTS pg_catalog.int2_mul_uint4(int2,uint4) CASCADE;
CREATE FUNCTION pg_catalog.int2_mul_uint4 (
int2,uint4
) RETURNS int4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int2_mul_uint4';


DROP FUNCTION IF EXISTS pg_catalog.int2_div_uint4(int2,uint4) CASCADE;
CREATE FUNCTION pg_catalog.int2_div_uint4 (
int2,uint4
) RETURNS float8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int2_div_uint4';

DROP FUNCTION IF EXISTS pg_catalog.int2_uint4_mod(int2,uint4) CASCADE;
CREATE FUNCTION pg_catalog.int2_uint4_mod (
int2,uint4
) RETURNS float8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int2_uint4_mod';

DROP FUNCTION IF EXISTS pg_catalog.int2_uint4_eq(int2,uint4) CASCADE;
CREATE FUNCTION pg_catalog.int2_uint4_eq (
int2,uint4
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int2_uint4_eq';


DROP FUNCTION IF EXISTS pg_catalog.int2_uint4_ne(int2,uint4) CASCADE;
CREATE FUNCTION pg_catalog.int2_uint4_ne (
int2,uint4
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int2_uint4_ne';


DROP FUNCTION IF EXISTS pg_catalog.int2_uint4_lt(int2,uint4) CASCADE;
CREATE FUNCTION pg_catalog.int2_uint4_lt (
int2,uint4
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int2_uint4_lt';


DROP FUNCTION IF EXISTS pg_catalog.int2_uint4_le(int2,uint4) CASCADE;
CREATE FUNCTION pg_catalog.int2_uint4_le (
int2,uint4
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int2_uint4_le';


DROP FUNCTION IF EXISTS pg_catalog.int2_uint4_gt(int2,uint4) CASCADE;
CREATE FUNCTION pg_catalog.int2_uint4_gt (
int2,uint4
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int2_uint4_gt';


DROP FUNCTION IF EXISTS pg_catalog.int2_uint4_ge(int2,uint4) CASCADE;
CREATE FUNCTION pg_catalog.int2_uint4_ge (
int2,uint4
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int2_uint4_ge';


CREATE OPERATOR pg_catalog.+(
leftarg = int2, rightarg = uint4, procedure = int2_pl_uint4,
commutator=operator(pg_catalog.+)
);
COMMENT ON OPERATOR pg_catalog.+(int2, uint4) IS 'int2_pl_uint4';


CREATE OPERATOR pg_catalog.-(
leftarg = int2, rightarg = uint4, procedure = int2_mi_uint4,
commutator=operator(pg_catalog.-)
);
COMMENT ON OPERATOR pg_catalog.-(int2, uint4) IS 'int2_mi_uint4';


CREATE OPERATOR pg_catalog.*(
leftarg = int2, rightarg = uint4, procedure = int2_mul_uint4,
commutator=operator(pg_catalog.*)
);
COMMENT ON OPERATOR pg_catalog.*(int2, uint4) IS 'int2_mul_uint4';


CREATE OPERATOR pg_catalog./(
leftarg = int2, rightarg = uint4, procedure = int2_div_uint4,
commutator=operator(pg_catalog./)
);
COMMENT ON OPERATOR pg_catalog./(int2, uint4) IS 'int2_div_uint4';

CREATE OPERATOR pg_catalog.%(
leftarg = int2, rightarg = uint4, procedure = int2_uint4_mod,
commutator=operator(pg_catalog.%)
);
COMMENT ON OPERATOR pg_catalog.%(int2, uint4) IS 'int2_uint4_mod';

CREATE OPERATOR pg_catalog.=(
leftarg = int2, rightarg = uint4, procedure = int2_uint4_eq,
commutator=operator(pg_catalog.=)
);
COMMENT ON OPERATOR pg_catalog.=(int2, uint4) IS 'int2_uint4_eq';



CREATE OPERATOR pg_catalog.<>(
leftarg = int2, rightarg = uint4, procedure = int2_uint4_ne,
commutator=operator(pg_catalog.<>)
);
COMMENT ON OPERATOR pg_catalog.<>(int2, uint4) IS 'int2_uint4_ne';




CREATE OPERATOR pg_catalog.<(
leftarg = int2, rightarg = uint4, procedure = int2_uint4_lt,
commutator=operator(pg_catalog.<)
);
COMMENT ON OPERATOR pg_catalog.<(int2, uint4) IS 'int2_uint4_lt';



CREATE OPERATOR pg_catalog.<=(
leftarg = int2, rightarg = uint4, procedure = int2_uint4_le,
commutator=operator(pg_catalog.<=)
);
COMMENT ON OPERATOR pg_catalog.<=(int2, uint4) IS 'int2_uint4_le';


CREATE OPERATOR pg_catalog.>(
leftarg = int2, rightarg = uint4, procedure = int2_uint4_gt,
commutator=operator(pg_catalog.>)
);
COMMENT ON OPERATOR pg_catalog.>=(int2, uint4) IS 'int2_uint4_gt';



CREATE OPERATOR pg_catalog.>=(
leftarg = int2, rightarg = uint4, procedure = int2_uint4_ge,
commutator=operator(pg_catalog.>=)
);
COMMENT ON OPERATOR pg_catalog.>=(int2, uint4) IS 'int2_uint4_ge';

-------------------int2 uint4---------------

-------------------int4 uint4---------------
DROP FUNCTION IF EXISTS pg_catalog.int4_pl_uint4(int4,uint4) CASCADE;
CREATE FUNCTION pg_catalog.int4_pl_uint4 (
int4,uint4
) RETURNS int8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int4_pl_uint4';

DROP FUNCTION IF EXISTS pg_catalog.int4_mi_uint4(int4,uint4) CASCADE;
CREATE FUNCTION pg_catalog.int4_mi_uint4 (
int4,uint4
) RETURNS int8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int4_mi_uint4';
DROP FUNCTION IF EXISTS pg_catalog.int4_mul_uint4(int4,uint4) CASCADE;
CREATE FUNCTION pg_catalog.int4_mul_uint4 (
int4,uint4
) RETURNS int8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int4_mul_uint4';


DROP FUNCTION IF EXISTS pg_catalog.int4_div_uint4(int4,uint4) CASCADE;
CREATE FUNCTION pg_catalog.int4_div_uint4 (
int4,uint4
) RETURNS float8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int4_div_uint4';

DROP FUNCTION IF EXISTS pg_catalog.int4_uint4_mod(int4,uint4) CASCADE;
CREATE FUNCTION pg_catalog.int4_uint4_mod (
int4,uint4
) RETURNS float8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int4_uint4_mod';

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
commutator=operator(pg_catalog.=)
);
COMMENT ON OPERATOR pg_catalog.=(int4, uint4) IS 'int4_uint4_eq';



CREATE OPERATOR pg_catalog.<>(
leftarg = int4, rightarg = uint4, procedure = int4_uint4_ne,
commutator=operator(pg_catalog.<>)
);
COMMENT ON OPERATOR pg_catalog.<>(int4, uint4) IS 'int4_uint4_ne';




CREATE OPERATOR pg_catalog.<(
leftarg = int4, rightarg = uint4, procedure = int4_uint4_lt,
commutator=operator(pg_catalog.<)
);
COMMENT ON OPERATOR pg_catalog.<(int4, uint4) IS 'int4_uint4_lt';



CREATE OPERATOR pg_catalog.<=(
leftarg = int4, rightarg = uint4, procedure = int4_uint4_le,
commutator=operator(pg_catalog.<=)
);
COMMENT ON OPERATOR pg_catalog.<=(int4, uint4) IS 'int4_uint4_le';


CREATE OPERATOR pg_catalog.>(
leftarg = int4, rightarg = uint4, procedure = int4_uint4_gt,
commutator=operator(pg_catalog.>)
);
COMMENT ON OPERATOR pg_catalog.>=(int4, uint4) IS 'int4_uint4_gt';



CREATE OPERATOR pg_catalog.>=(
leftarg = int4, rightarg = uint4, procedure = int4_uint4_ge,
commutator=operator(pg_catalog.>=)
);
COMMENT ON OPERATOR pg_catalog.>=(int4, uint4) IS 'int4_uint4_ge';

-------------------int4 uint4---------------

-------------------int8 uint4---------------
DROP FUNCTION IF EXISTS pg_catalog.int8_pl_uint4(int8,uint4) CASCADE;
CREATE FUNCTION pg_catalog.int8_pl_uint4 (
int8,uint4
) RETURNS int16 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int8_pl_uint4';

DROP FUNCTION IF EXISTS pg_catalog.int8_mi_uint4(int8,uint4) CASCADE;
CREATE FUNCTION pg_catalog.int8_mi_uint4 (
int8,uint4
) RETURNS int16 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int8_mi_uint4';
DROP FUNCTION IF EXISTS pg_catalog.int8_mul_uint4(int8,uint4) CASCADE;
CREATE FUNCTION pg_catalog.int8_mul_uint4 (
int8,uint4
) RETURNS int16 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int8_mul_uint4';


DROP FUNCTION IF EXISTS pg_catalog.int8_div_uint4(int8,uint4) CASCADE;
CREATE FUNCTION pg_catalog.int8_div_uint4 (
int8,uint4
) RETURNS float8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int8_div_uint4';

DROP FUNCTION IF EXISTS pg_catalog.int8_uint4_mod(int8,uint4) CASCADE;
CREATE FUNCTION pg_catalog.int8_uint4_mod (
int8,uint4
) RETURNS float8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int8_uint4_mod';

DROP FUNCTION IF EXISTS pg_catalog.int8_uint4_eq(int8,uint4) CASCADE;
CREATE FUNCTION pg_catalog.int8_uint4_eq (
int8,uint4
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int8_uint4_eq';


DROP FUNCTION IF EXISTS pg_catalog.int8_uint4_ne(int8,uint4) CASCADE;
CREATE FUNCTION pg_catalog.int8_uint4_ne (
int8,uint4
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int8_uint4_ne';


DROP FUNCTION IF EXISTS pg_catalog.int8_uint4_lt(int8,uint4) CASCADE;
CREATE FUNCTION pg_catalog.int8_uint4_lt (
int8,uint4
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int8_uint4_lt';


DROP FUNCTION IF EXISTS pg_catalog.int8_uint4_le(int8,uint4) CASCADE;
CREATE FUNCTION pg_catalog.int8_uint4_le (
int8,uint4
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int8_uint4_le';


DROP FUNCTION IF EXISTS pg_catalog.int8_uint4_gt(int8,uint4) CASCADE;
CREATE FUNCTION pg_catalog.int8_uint4_gt (
int8,uint4
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int8_uint4_gt';


DROP FUNCTION IF EXISTS pg_catalog.int8_uint4_ge(int8,uint4) CASCADE;
CREATE FUNCTION pg_catalog.int8_uint4_ge (
int8,uint4
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int8_uint4_ge';

CREATE OPERATOR pg_catalog.+(
leftarg = int8, rightarg = uint4, procedure = int8_pl_uint4,
commutator=operator(pg_catalog.+)
);
COMMENT ON OPERATOR pg_catalog.+(int8, uint4) IS 'int8_pl_uint4';


CREATE OPERATOR pg_catalog.-(
leftarg = int8, rightarg = uint4, procedure = int8_mi_uint4,
commutator=operator(pg_catalog.-)
);
COMMENT ON OPERATOR pg_catalog.-(int8, uint4) IS 'int8_mi_uint4';


CREATE OPERATOR pg_catalog.*(
leftarg = int8, rightarg = uint4, procedure = int8_mul_uint4,
commutator=operator(pg_catalog.*)
);
COMMENT ON OPERATOR pg_catalog.*(int8, uint4) IS 'int8_mul_uint4';


CREATE OPERATOR pg_catalog./(
leftarg = int8, rightarg = uint4, procedure = int8_div_uint4,
commutator=operator(pg_catalog./)
);
COMMENT ON OPERATOR pg_catalog./(int8, uint4) IS 'int8_div_uint4';

CREATE OPERATOR pg_catalog.%(
leftarg = int8, rightarg = uint4, procedure = int8_uint4_mod,
commutator=operator(pg_catalog.%)
);
COMMENT ON OPERATOR pg_catalog.%(int8, uint4) IS 'int8_uint4_mod';

CREATE OPERATOR pg_catalog.=(
leftarg = int8, rightarg = uint4, procedure = int8_uint4_eq,
commutator=operator(pg_catalog.=)
);
COMMENT ON OPERATOR pg_catalog.=(int8, uint4) IS 'int8_uint4_eq';



CREATE OPERATOR pg_catalog.<>(
leftarg = int8, rightarg = uint4, procedure = int8_uint4_ne,
commutator=operator(pg_catalog.<>)
);
COMMENT ON OPERATOR pg_catalog.<>(int8, uint4) IS 'int8_uint4_ne';




CREATE OPERATOR pg_catalog.<(
leftarg = int8, rightarg = uint4, procedure = int8_uint4_lt,
commutator=operator(pg_catalog.<)
);
COMMENT ON OPERATOR pg_catalog.<(int8, uint4) IS 'int8_uint4_lt';



CREATE OPERATOR pg_catalog.<=(
leftarg = int8, rightarg = uint4, procedure = int8_uint4_le,
commutator=operator(pg_catalog.<=)
);
COMMENT ON OPERATOR pg_catalog.<=(int8, uint4) IS 'int8_uint4_le';


CREATE OPERATOR pg_catalog.>(
leftarg = int8, rightarg = uint4, procedure = int8_uint4_gt,
commutator=operator(pg_catalog.>)
);
COMMENT ON OPERATOR pg_catalog.>=(int8, uint4) IS 'int8_uint4_gt';



CREATE OPERATOR pg_catalog.>=(
leftarg = int8, rightarg = uint4, procedure = int8_uint4_ge,
commutator=operator(pg_catalog.>=)
);
COMMENT ON OPERATOR pg_catalog.>=(int8, uint4) IS 'int8_uint4_ge';

-------------------int8 uint4---------------
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
drop aggregate if exists pg_catalog.bit_and(uint4);
create aggregate pg_catalog.bit_and(uint4) (SFUNC=uint4and, cFUNC = uint4and, STYPE= uint4);
drop aggregate if exists pg_catalog.bit_or(uint4);
create aggregate pg_catalog.bit_or(uint4) (SFUNC=uint4or, cFUNC = uint4or,  STYPE= uint4);
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
DROP FUNCTION IF EXISTS pg_catalog.hashuint8(uint8,uint8) CASCADE;

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


CREATE FUNCTION pg_catalog.hashuint8 (
uint8,uint8
) RETURNS int4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'hashuint8';



CREATE OR REPLACE FUNCTION NUMTODAY(uint8)
RETURNS INTERVAL
AS $$ SELECT NUMTODSINTERVAL(uint8_numeric($1),'DAY')$$
LANGUAGE SQL IMMUTABLE STRICT;
CREATE OR REPLACE FUNCTION TO_TEXT(uint8)
RETURNS TEXT
AS $$ select CAST(uint8out($1) AS VARCHAR2)  $$
LANGUAGE SQL  STRICT IMMUTABLE;


DROP FUNCTION IF EXISTS pg_catalog.uint8_bool(uint8) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.uint8_bool(uint8)
RETURNS bool LANGUAGE C  AS  '$libdir/dolphin',  'uint8_bool';

DROP FUNCTION IF EXISTS pg_catalog.bool_uint8(bool) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.bool_uint8(bool)
RETURNS uint8 LANGUAGE C  AS  '$libdir/dolphin',  'bool_uint8';

DROP FUNCTION IF EXISTS pg_catalog.uint8pl(uint8,uint8) CASCADE;
CREATE FUNCTION pg_catalog.uint8pl (
uint8,uint8
) RETURNS uint8 LANGUAGE C IMMUTABLE  as '$libdir/dolphin',  'uint8pl';

DROP FUNCTION IF EXISTS pg_catalog.uint8mi(uint8,uint8) CASCADE;
CREATE FUNCTION pg_catalog.uint8mi (
uint8,uint8
) RETURNS uint8 LANGUAGE C IMMUTABLE  as '$libdir/dolphin',  'uint8mi';

DROP FUNCTION IF EXISTS pg_catalog.uint8mul(uint8,uint8) CASCADE;
CREATE FUNCTION pg_catalog.uint8mul (
uint8,uint8
) RETURNS uint8 LANGUAGE C IMMUTABLE  as '$libdir/dolphin',  'uint8mul';


DROP FUNCTION IF EXISTS pg_catalog.uint8div(uint8,uint8) CASCADE;
CREATE FUNCTION pg_catalog.uint8div (
uint8,uint8
) RETURNS float8 LANGUAGE C IMMUTABLE  as '$libdir/dolphin',  'uint8div';


DROP FUNCTION IF EXISTS pg_catalog.uint8abs(uint8) CASCADE;
CREATE FUNCTION pg_catalog.uint8abs (
uint8
) RETURNS uint8 LANGUAGE C IMMUTABLE  as '$libdir/dolphin',  'uint8abs';


DROP FUNCTION IF EXISTS pg_catalog.uint8mod(uint8,uint8) CASCADE;
CREATE FUNCTION pg_catalog.uint8mod (
uint8,uint8
) RETURNS uint8 LANGUAGE C IMMUTABLE  as '$libdir/dolphin',  'uint8mod';

DROP FUNCTION IF EXISTS pg_catalog.uint8and(uint8,uint8) CASCADE;
CREATE FUNCTION pg_catalog.uint8and (
uint8,uint8
) RETURNS uint8 LANGUAGE C IMMUTABLE  as '$libdir/dolphin',  'uint8and';

DROP FUNCTION IF EXISTS pg_catalog.uint8or(uint8,uint8) CASCADE;
CREATE FUNCTION pg_catalog.uint8or (
uint8,uint8
) RETURNS uint8 LANGUAGE C IMMUTABLE  as '$libdir/dolphin',  'uint8or';

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
) RETURNS int2 LANGUAGE C STRICT IMMUTABLE  as '$libdir/dolphin',  'uint8um';

DROP FUNCTION IF EXISTS pg_catalog.uint8not(uint8) CASCADE;
CREATE FUNCTION pg_catalog.uint8not (
uint8
) RETURNS uint8 LANGUAGE C STRICT IMMUTABLE  as '$libdir/dolphin',  'uint8not';



DROP FUNCTION IF EXISTS pg_catalog.uint8_avg_accum(bigint[], uint8) CASCADE;

CREATE OR REPLACE FUNCTION pg_catalog.uint8_avg_accum(bigint[], uint8)
 RETURNS bigint[] LANGUAGE C STRICT AS  '$libdir/dolphin',  'uint8_avg_accum';

DROP FUNCTION IF EXISTS pg_catalog.uint8_accum(numeric[], uint8) CASCADE;

CREATE OR REPLACE FUNCTION pg_catalog.uint8_accum(numeric[], uint8)
 RETURNS numeric[] LANGUAGE C STRICT AS  '$libdir/dolphin',  'uint8_accum';

DROP FUNCTION IF EXISTS pg_catalog.uint8_sum(int8, uint8) CASCADE;

CREATE OR REPLACE FUNCTION pg_catalog.uint8_sum(int8, uint8)
RETURNS int8 LANGUAGE C  AS  '$libdir/dolphin',  'uint8_sum';




DROP FUNCTION IF EXISTS pg_catalog.uint8_list_agg_transfn(internal, uint8, text) CASCADE;

CREATE OR REPLACE FUNCTION pg_catalog.uint8_list_agg_transfn(internal, uint8, text)
 RETURNS internal LANGUAGE C  AS	'$libdir/dolphin',	'uint8_list_agg_transfn';



DROP FUNCTION IF EXISTS pg_catalog.uint8_list_agg_noarg2_transfn(internal, uint8) CASCADE;

CREATE OR REPLACE FUNCTION pg_catalog.uint8_list_agg_noarg2_transfn(internal, uint8)
 RETURNS internal LANGUAGE C  AS	'$libdir/dolphin',	'uint8_list_agg_noarg2_transfn';

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
drop  CAST IF EXISTS(uint8 AS  INTERVAL) CASCADE;


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

CREATE CAST (uint8 AS INTERVAL) WITH FUNCTION NUMTODAY(uint8) AS IMPLICIT;


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
commutator=operator(pg_catalog.<>),
restrict = neqsel, join = neqjoinsel
);
COMMENT ON OPERATOR pg_catalog.<>(uint8, uint8) IS 'uint8ne';




CREATE OPERATOR pg_catalog.<(
leftarg = uint8, rightarg = uint8, procedure = uint8lt,
commutator=operator(pg_catalog.<),
restrict = scalarltsel, join = scalarltjoinsel
);
COMMENT ON OPERATOR pg_catalog.<(uint8, uint8) IS 'uint8lt';



CREATE OPERATOR pg_catalog.<=(
leftarg = uint8, rightarg = uint8, procedure = uint8le,
commutator=operator(pg_catalog.<=),
restrict = scalarltsel, join = scalarltjoinsel
);
COMMENT ON OPERATOR pg_catalog.<=(uint8, uint8) IS 'uint8le';




CREATE OPERATOR pg_catalog.>(
leftarg = uint8, rightarg = uint8, procedure = uint8gt,
commutator=operator(pg_catalog.>),
restrict = scalargtsel, join = scalargtjoinsel
);
COMMENT ON OPERATOR pg_catalog.>(uint8, uint8) IS 'uint8gt';




CREATE OPERATOR pg_catalog.>=(
leftarg = uint8, rightarg = uint8, procedure = uint8ge,
commutator=operator(pg_catalog.>=),
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

create or replace function uint8(text)
returns uint8
as $$ select cast(to_number($1) as uint8)$$
language sql IMMUTABLE strict;
CREATE CAST (TEXT AS uint8) WITH FUNCTION uint8(TEXT) AS IMPLICIT;

-------------------uint8 int 1---------------

DROP FUNCTION IF EXISTS pg_catalog.uint8_pl_int1(uint8,int1) CASCADE;
CREATE FUNCTION pg_catalog.uint8_pl_int1 (
uint8,int1
) RETURNS int2 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint8_pl_int1';

DROP FUNCTION IF EXISTS pg_catalog.uint8_mi_int1(uint8,int1) CASCADE;
CREATE FUNCTION pg_catalog.uint8_mi_int1 (
uint8,int1
) RETURNS int2 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint8_mi_int1';

DROP FUNCTION IF EXISTS pg_catalog.uint8_mul_int1(uint8,int1) CASCADE;
CREATE FUNCTION pg_catalog.uint8_mul_int1 (
uint8,int1
) RETURNS uint8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint8_mul_int1';


DROP FUNCTION IF EXISTS pg_catalog.uint8_div_int1(uint8,int1) CASCADE;
CREATE FUNCTION pg_catalog.uint8_div_int1 (
uint8,int1
) RETURNS float8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint8_div_int1';

DROP FUNCTION IF EXISTS pg_catalog.uint8_int1_mod(uint8,int1) CASCADE;
CREATE FUNCTION pg_catalog.uint8_int1_mod (
uint8,int1
) RETURNS float8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint8_int1_mod';

DROP FUNCTION IF EXISTS pg_catalog.uint8_int1_eq(uint8,int1) CASCADE;
CREATE FUNCTION pg_catalog.uint8_int1_eq (
uint8,int1
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint8_int1_eq';


DROP FUNCTION IF EXISTS pg_catalog.uint8_int1_ne(uint8,int1) CASCADE;
CREATE FUNCTION pg_catalog.uint8_int1_ne (
uint8,int1
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint8_int1_ne';


DROP FUNCTION IF EXISTS pg_catalog.uint8_int1_lt(uint8,int1) CASCADE;
CREATE FUNCTION pg_catalog.uint8_int1_lt (
uint8,int1
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint8_int1_lt';


DROP FUNCTION IF EXISTS pg_catalog.uint8_int1_le(uint8,int1) CASCADE;
CREATE FUNCTION pg_catalog.uint8_int1_le (
uint8,int1
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint8_int1_le';


DROP FUNCTION IF EXISTS pg_catalog.uint8_int1_gt(uint8,int1) CASCADE;
CREATE FUNCTION pg_catalog.uint8_int1_gt (
uint8,int1
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint8_int1_gt';


DROP FUNCTION IF EXISTS pg_catalog.uint8_int1_ge(uint8,int1) CASCADE;
CREATE FUNCTION pg_catalog.uint8_int1_ge (
uint8,int1
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint8_int1_ge';
CREATE OPERATOR pg_catalog.+(
leftarg = uint8, rightarg = int1, procedure = uint8_pl_int1,
commutator=operator(pg_catalog.+)
);
COMMENT ON OPERATOR pg_catalog.+(uint8, int1) IS 'uint8_pl_int1';


CREATE OPERATOR pg_catalog.-(
leftarg = uint8, rightarg = int1, procedure = uint8_mi_int1,
commutator=operator(pg_catalog.-)
);
COMMENT ON OPERATOR pg_catalog.-(uint8, int1) IS 'uint8_mi_int1';


CREATE OPERATOR pg_catalog.*(
leftarg = uint8, rightarg = int1, procedure = uint8_mul_int1,
commutator=operator(pg_catalog.*)
);
COMMENT ON OPERATOR pg_catalog.*(uint8, int1) IS 'uint8_mul_int1';


CREATE OPERATOR pg_catalog./(
leftarg = uint8, rightarg = int1, procedure = uint8_div_int1,
commutator=operator(pg_catalog./)
);
COMMENT ON OPERATOR pg_catalog./(uint8, int1) IS 'uint8_div_int1';

CREATE OPERATOR pg_catalog.%(
leftarg = uint8, rightarg = int1, procedure = uint8_int1_mod,
commutator=operator(pg_catalog.%)
);
COMMENT ON OPERATOR pg_catalog.%(uint8, int1) IS 'uint8_int1_mod';

CREATE OPERATOR pg_catalog.=(
leftarg = uint8, rightarg = int1, procedure = uint8_int1_eq,
commutator=operator(pg_catalog.=)
);
COMMENT ON OPERATOR pg_catalog.=(uint8, int1) IS 'uint8_int1_eq';



CREATE OPERATOR pg_catalog.<>(
leftarg = uint8, rightarg = int1, procedure = uint8_int1_ne,
commutator=operator(pg_catalog.<>)
);
COMMENT ON OPERATOR pg_catalog.<>(uint8, int1) IS 'uint8_int1_ne';


CREATE OPERATOR pg_catalog.<(
leftarg = uint8, rightarg = int1, procedure = uint8_int1_lt,
commutator=operator(pg_catalog.<)
);
COMMENT ON OPERATOR pg_catalog.<(uint8, int1) IS 'uint8_int1_lt';


CREATE OPERATOR pg_catalog.<=(
leftarg = uint8, rightarg = int1, procedure = uint8_int1_le,
commutator=operator(pg_catalog.<=)
);
COMMENT ON OPERATOR pg_catalog.<=(uint8, int1) IS 'uint8_int1_le';

CREATE OPERATOR pg_catalog.>(
leftarg = uint8, rightarg = int1, procedure = uint8_int1_gt,
commutator=operator(pg_catalog.>)
);
COMMENT ON OPERATOR pg_catalog.>(uint8, int1) IS 'uint8_int1_gt';


CREATE OPERATOR pg_catalog.>=(
leftarg = uint8, rightarg = int1, procedure = uint8_int1_ge,
commutator=operator(pg_catalog.>=)
);
COMMENT ON OPERATOR pg_catalog.>=(uint8, int1) IS 'uint8_int1_ge';
-------------------uint8 int 1---------------



-------------------uint8 int2---------------
DROP FUNCTION IF EXISTS pg_catalog.uint8_pl_int2(uint8,int2) CASCADE;
CREATE FUNCTION pg_catalog.uint8_pl_int2 (
uint8,int2
) RETURNS int4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint8_pl_int2';

DROP FUNCTION IF EXISTS pg_catalog.uint8_mi_int2(uint8,int2) CASCADE;
CREATE FUNCTION pg_catalog.uint8_mi_int2 (
uint8,int2
) RETURNS int4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint8_mi_int2';

DROP FUNCTION IF EXISTS pg_catalog.uint8_mul_int2(uint8,int2) CASCADE;
CREATE FUNCTION pg_catalog.uint8_mul_int2 (
uint8,int2
) RETURNS int4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint8_mul_int2';


DROP FUNCTION IF EXISTS pg_catalog.uint8_div_int2(uint8,int2) CASCADE;
CREATE FUNCTION pg_catalog.uint8_div_int2 (
uint8,int2
) RETURNS uint8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint8_div_int2';

DROP FUNCTION IF EXISTS pg_catalog.uint8_int2_mod(uint8,int2) CASCADE;
CREATE FUNCTION pg_catalog.uint8_int2_mod (
uint8,int2
) RETURNS uint8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint8_int2_mod';

DROP FUNCTION IF EXISTS pg_catalog.uint8_int2_eq(uint8,int2) CASCADE;
CREATE FUNCTION pg_catalog.uint8_int2_eq (
uint8,int2
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint8_int2_eq';


DROP FUNCTION IF EXISTS pg_catalog.uint8_int2_ne(uint8,int2) CASCADE;
CREATE FUNCTION pg_catalog.uint8_int2_ne (
uint8,int2
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint8_int2_ne';


DROP FUNCTION IF EXISTS pg_catalog.uint8_int2_lt(uint8,int2) CASCADE;
CREATE FUNCTION pg_catalog.uint8_int2_lt (
uint8,int2
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint8_int2_lt';


DROP FUNCTION IF EXISTS pg_catalog.uint8_int2_le(uint8,int2) CASCADE;
CREATE FUNCTION pg_catalog.uint8_int2_le (
uint8,int2
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint8_int2_le';


DROP FUNCTION IF EXISTS pg_catalog.uint8_int2_gt(uint8,int2) CASCADE;
CREATE FUNCTION pg_catalog.uint8_int2_gt (
uint8,int2
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint8_int2_gt';


DROP FUNCTION IF EXISTS pg_catalog.uint8_int2_ge(uint8,int2) CASCADE;
CREATE FUNCTION pg_catalog.uint8_int2_ge (
uint8,int2
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint8_int2_ge';

CREATE OPERATOR pg_catalog.+(
leftarg = uint8, rightarg = int2, procedure = uint8_pl_int2,
commutator=operator(pg_catalog.+)
);
COMMENT ON OPERATOR pg_catalog.+(uint8, int2) IS 'uint8_pl_int2';


CREATE OPERATOR pg_catalog.-(
leftarg = uint8, rightarg = int2, procedure = uint8_mi_int2,
commutator=operator(pg_catalog.-)
);
COMMENT ON OPERATOR pg_catalog.-(uint8, int2) IS 'uint8_mi_int2';


CREATE OPERATOR pg_catalog.*(
leftarg = uint8, rightarg = int2, procedure = uint8_mul_int2,
commutator=operator(pg_catalog.*)
);
COMMENT ON OPERATOR pg_catalog.*(uint8, int2) IS 'uint8_mul_int2';


CREATE OPERATOR pg_catalog./(
leftarg = uint8, rightarg = int2, procedure = uint8_div_int2,
commutator=operator(pg_catalog./)
);
COMMENT ON OPERATOR pg_catalog./(uint8, int2) IS 'uint8_div_int2';

CREATE OPERATOR pg_catalog.%(
leftarg = uint8, rightarg = int2, procedure = uint8_int2_mod,
commutator=operator(pg_catalog.%)
);
COMMENT ON OPERATOR pg_catalog.%(uint8, int2) IS 'uint8_int2_mod';

CREATE OPERATOR pg_catalog.=(
leftarg = uint8, rightarg = int2, procedure = uint8_int2_eq,
commutator=operator(pg_catalog.=)
);
COMMENT ON OPERATOR pg_catalog.=(uint8, int2) IS 'uint8_int2_eq';



CREATE OPERATOR pg_catalog.<>(
leftarg = uint8, rightarg = int2, procedure = uint8_int2_ne,
commutator=operator(pg_catalog.<>)
);
COMMENT ON OPERATOR pg_catalog.<>(uint8, int2) IS 'uint8_int2_ne';




CREATE OPERATOR pg_catalog.<(
leftarg = uint8, rightarg = int2, procedure = uint8_int2_lt,
commutator=operator(pg_catalog.<)
);
COMMENT ON OPERATOR pg_catalog.<(uint8, int2) IS 'uint8_int2_lt';



CREATE OPERATOR pg_catalog.<=(
leftarg = uint8, rightarg = int2, procedure = uint8_int2_le,
commutator=operator(pg_catalog.<=)
);
COMMENT ON OPERATOR pg_catalog.<=(uint8, int2) IS 'uint8_int2_le';


CREATE OPERATOR pg_catalog.>(
leftarg = uint8, rightarg = int2, procedure = uint8_int2_gt,
commutator=operator(pg_catalog.>)
);
COMMENT ON OPERATOR pg_catalog.>(uint8, int2) IS 'uint8_int2_gt';

CREATE OPERATOR pg_catalog.>=(
leftarg = uint8, rightarg = int2, procedure = uint8_int2_ge,
commutator=operator(pg_catalog.>=)
);
COMMENT ON OPERATOR pg_catalog.>=(uint8, int2) IS 'uint8_int2_ge';

-------------------uint8 int2---------------



-------------------uint8 int4---------------
DROP FUNCTION IF EXISTS pg_catalog.uint8_pl_int4(uint8,int4) CASCADE;
CREATE FUNCTION pg_catalog.uint8_pl_int4 (
uint8,int4
) RETURNS int8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint8_pl_int4';

DROP FUNCTION IF EXISTS pg_catalog.uint8_mi_int4(uint8,int4) CASCADE;
CREATE FUNCTION pg_catalog.uint8_mi_int4 (
uint8,int4
) RETURNS int8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint8_mi_int4';
DROP FUNCTION IF EXISTS pg_catalog.uint8_mul_int4(uint8,int4) CASCADE;
CREATE FUNCTION pg_catalog.uint8_mul_int4 (
uint8,int4
) RETURNS uint8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint8_mul_int4';


DROP FUNCTION IF EXISTS pg_catalog.uint8_div_int4(uint8,int4) CASCADE;
CREATE FUNCTION pg_catalog.uint8_div_int4 (
uint8,int4
) RETURNS float8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint8_div_int4';

DROP FUNCTION IF EXISTS pg_catalog.uint8_int4_mod(uint8,int4) CASCADE;
CREATE FUNCTION pg_catalog.uint8_int4_mod (
uint8,int4
) RETURNS float8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint8_int4_mod';

DROP FUNCTION IF EXISTS pg_catalog.uint8_int4_eq(uint8,int4) CASCADE;
CREATE FUNCTION pg_catalog.uint8_int4_eq (
uint8,int4
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint8_int4_eq';


DROP FUNCTION IF EXISTS pg_catalog.uint8_int4_ne(uint8,int4) CASCADE;
CREATE FUNCTION pg_catalog.uint8_int4_ne (
uint8,int4
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint8_int4_ne';


DROP FUNCTION IF EXISTS pg_catalog.uint8_int4_lt(uint8,int4) CASCADE;
CREATE FUNCTION pg_catalog.uint8_int4_lt (
uint8,int4
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint8_int4_lt';


DROP FUNCTION IF EXISTS pg_catalog.uint8_int4_le(uint8,int4) CASCADE;
CREATE FUNCTION pg_catalog.uint8_int4_le (
uint8,int4
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint8_int4_le';


DROP FUNCTION IF EXISTS pg_catalog.uint8_int4_gt(uint8,int4) CASCADE;
CREATE FUNCTION pg_catalog.uint8_int4_gt (
uint8,int4
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint8_int4_gt';


DROP FUNCTION IF EXISTS pg_catalog.uint8_int4_ge(uint8,int4) CASCADE;
CREATE FUNCTION pg_catalog.uint8_int4_ge (
uint8,int4
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint8_int4_ge';


CREATE OPERATOR pg_catalog.+(
leftarg = uint8, rightarg = int4, procedure = uint8_pl_int4,
commutator=operator(pg_catalog.+)
);
COMMENT ON OPERATOR pg_catalog.+(uint8, int4) IS 'uint8_pl_int4';

CREATE OPERATOR pg_catalog.-(
leftarg = uint8, rightarg = int4, procedure = uint8_mi_int4,
commutator=operator(pg_catalog.-)
);
COMMENT ON OPERATOR pg_catalog.-(uint8, int4) IS 'uint8_mi_int4';

CREATE OPERATOR pg_catalog.*(
leftarg = uint8, rightarg = int4, procedure = uint8_mul_int4,
commutator=operator(pg_catalog.*)
);
COMMENT ON OPERATOR pg_catalog.*(uint8, int4) IS 'uint8_mul_int4';


CREATE OPERATOR pg_catalog./(
leftarg = uint8, rightarg = int4, procedure = uint8_div_int4,
commutator=operator(pg_catalog./)
);
COMMENT ON OPERATOR pg_catalog./(uint8, int4) IS 'uint8_div_int4';

CREATE OPERATOR pg_catalog.%(
leftarg = uint8, rightarg = int4, procedure = uint8_int4_mod,
commutator=operator(pg_catalog.%)
);
COMMENT ON OPERATOR pg_catalog.%(uint8, int4) IS 'uint8_int4_mod';

CREATE OPERATOR pg_catalog.=(
leftarg = uint8, rightarg = int4, procedure = uint8_int4_eq,
commutator=operator(pg_catalog.=)
);
COMMENT ON OPERATOR pg_catalog.=(uint8, int4) IS 'uint8_int4_eq';


CREATE OPERATOR pg_catalog.<>(
leftarg = uint8, rightarg = int4, procedure = uint8_int4_ne,
commutator=operator(pg_catalog.<>)
);
COMMENT ON OPERATOR pg_catalog.<>(uint8, int4) IS 'uint8_int4_ne';



CREATE OPERATOR pg_catalog.<(
leftarg = uint8, rightarg = int4, procedure = uint8_int4_lt,
commutator=operator(pg_catalog.<)
);
COMMENT ON OPERATOR pg_catalog.<(uint8, int4) IS 'uint8_int4_lt';


CREATE OPERATOR pg_catalog.<=(
leftarg = uint8, rightarg = int4, procedure = uint8_int4_le,
commutator=operator(pg_catalog.<=)
);
COMMENT ON OPERATOR pg_catalog.<=(uint8, int4) IS 'uint8_int4_le';

CREATE OPERATOR pg_catalog.>(
leftarg = uint8, rightarg = int4, procedure = uint8_int4_gt,
commutator=operator(pg_catalog.>)
);
COMMENT ON OPERATOR pg_catalog.>(uint8, int4) IS 'uint8_int4_gt';


CREATE OPERATOR pg_catalog.>=(
leftarg = uint8, rightarg = int4, procedure = uint8_int4_ge,
commutator=operator(pg_catalog.>=)
);
COMMENT ON OPERATOR pg_catalog.>=(uint8, int4) IS 'uint8_int4_ge';

-------------------uint8 int4---------------


-------------------uint8 int8---------------

DROP FUNCTION IF EXISTS pg_catalog.uint8_pl_int8(uint8,int8) CASCADE;
CREATE FUNCTION pg_catalog.uint8_pl_int8 (
uint8,int8
) RETURNS int16 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint8_pl_int8';

DROP FUNCTION IF EXISTS pg_catalog.uint8_mi_int8(uint8,int8) CASCADE;
CREATE FUNCTION pg_catalog.uint8_mi_int8 (
uint8,int8
) RETURNS int16 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint8_mi_int8';

DROP FUNCTION IF EXISTS pg_catalog.uint8_mul_int8(uint8,int8) CASCADE;
CREATE FUNCTION pg_catalog.uint8_mul_int8 (
uint8,int8
) RETURNS int16 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint8_mul_int8';


DROP FUNCTION IF EXISTS pg_catalog.uint8_div_int8(uint8,int8) CASCADE;
CREATE FUNCTION pg_catalog.uint8_div_int8 (
uint8,int8
) RETURNS float8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint8_div_int8';

DROP FUNCTION IF EXISTS pg_catalog.uint8_int8_mod(uint8,int8) CASCADE;
CREATE FUNCTION pg_catalog.uint8_int8_mod (
uint8,int8
) RETURNS float8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint8_int8_mod';

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
commutator=operator(pg_catalog.=)
);
COMMENT ON OPERATOR pg_catalog.=(uint8, int8) IS 'uint8_int8_eq';


CREATE OPERATOR pg_catalog.<>(
leftarg = uint8, rightarg = int8, procedure = uint8_int8_ne,
commutator=operator(pg_catalog.<>)
);
COMMENT ON OPERATOR pg_catalog.<>(uint8, int8) IS 'uint8_int8_ne';


CREATE OPERATOR pg_catalog.<(
leftarg = uint8, rightarg = int8, procedure = uint8_int8_lt,
commutator=operator(pg_catalog.<)
);
COMMENT ON OPERATOR pg_catalog.<(uint8, int8) IS 'uint8_int8_lt';

CREATE OPERATOR pg_catalog.<=(
leftarg = uint8, rightarg = int8, procedure = uint8_int8_le,
commutator=operator(pg_catalog.<=)
);
COMMENT ON OPERATOR pg_catalog.<=(uint8, int8) IS 'uint8_int8_le';

CREATE OPERATOR pg_catalog.>(
leftarg = uint8, rightarg = int8, procedure = uint8_int8_gt,
commutator=operator(pg_catalog.>)
);
COMMENT ON OPERATOR pg_catalog.>(uint8, int8) IS 'uint8_int8_gt';


CREATE OPERATOR pg_catalog.>=(
leftarg = uint8, rightarg = int8, procedure = uint8_int8_ge,
commutator=operator(pg_catalog.>=)
);
COMMENT ON OPERATOR pg_catalog.>=(uint8, int8) IS 'uint8_int8_ge';


-------------------uint8 int8---------------
	
-------------------uint8 uint1---------------

DROP FUNCTION IF EXISTS pg_catalog.uint8_pl_uint1(uint8,uint1) CASCADE;
CREATE FUNCTION pg_catalog.uint8_pl_uint1 (
uint8,uint1
) RETURNS uint4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint8_pl_uint1';

DROP FUNCTION IF EXISTS pg_catalog.uint8_mi_uint1(uint8,uint1) CASCADE;
CREATE FUNCTION pg_catalog.uint8_mi_uint1 (
uint8,uint1
) RETURNS int4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint8_mi_uint1';
DROP FUNCTION IF EXISTS pg_catalog.uint8_mul_uint1(uint8,uint1) CASCADE;
CREATE FUNCTION pg_catalog.uint8_mul_uint1 (
uint8,uint1
) RETURNS uint4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint8_mul_uint1';


DROP FUNCTION IF EXISTS pg_catalog.uint8_div_uint1(uint8,uint1) CASCADE;
CREATE FUNCTION pg_catalog.uint8_div_uint1 (
uint8,uint1
) RETURNS float8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint8_div_uint1';

DROP FUNCTION IF EXISTS pg_catalog.uint8_uint1_mod(uint8,uint1) CASCADE;
CREATE FUNCTION pg_catalog.uint8_uint1_mod (
uint8,uint1
) RETURNS float8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint8_uint1_mod';


DROP FUNCTION IF EXISTS pg_catalog.uint8_uint1_eq(uint8,uint1) CASCADE;
CREATE FUNCTION pg_catalog.uint8_uint1_eq (
uint8,uint1
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint8_uint1_eq';


DROP FUNCTION IF EXISTS pg_catalog.uint8_uint1_ne(uint8,uint1) CASCADE;
CREATE FUNCTION pg_catalog.uint8_uint1_ne (
uint8,uint1
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint8_uint1_ne';


DROP FUNCTION IF EXISTS pg_catalog.uint8_uint1_lt(uint8,uint1) CASCADE;
CREATE FUNCTION pg_catalog.uint8_uint1_lt (
uint8,uint1
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint8_uint1_lt';


DROP FUNCTION IF EXISTS pg_catalog.uint8_uint1_le(uint8,uint1) CASCADE;
CREATE FUNCTION pg_catalog.uint8_uint1_le (
uint8,uint1
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint8_uint1_le';


DROP FUNCTION IF EXISTS pg_catalog.uint8_uint1_gt(uint8,uint1) CASCADE;
CREATE FUNCTION pg_catalog.uint8_uint1_gt (
uint8,uint1
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint8_uint1_gt';


DROP FUNCTION IF EXISTS pg_catalog.uint8_uint1_ge(uint8,uint1) CASCADE;
CREATE FUNCTION pg_catalog.uint8_uint1_ge (
uint8,uint1
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint8_uint1_ge';

CREATE OPERATOR pg_catalog.+(
leftarg = uint8, rightarg = uint1, procedure = uint8_pl_uint1,
commutator=operator(pg_catalog.+)
);
COMMENT ON OPERATOR pg_catalog.+(uint8, uint1) IS 'uint8_pl_uint1';

CREATE OPERATOR pg_catalog.-(
leftarg = uint8, rightarg = uint1, procedure = uint8_mi_uint1,
commutator=operator(pg_catalog.-)
);
COMMENT ON OPERATOR pg_catalog.-(uint8, uint1) IS 'uint8_mi_uint1';

CREATE OPERATOR pg_catalog.*(
leftarg = uint8, rightarg = uint1, procedure = uint8_mul_uint1,
commutator=operator(pg_catalog.*)
);
COMMENT ON OPERATOR pg_catalog.*(uint8, uint1) IS 'uint8_mul_uint1';


CREATE OPERATOR pg_catalog./(
leftarg = uint8, rightarg = uint1, procedure = uint8_div_uint1,
commutator=operator(pg_catalog./)
);
COMMENT ON OPERATOR pg_catalog./(uint8, uint1) IS 'uint8_div_uint1';

CREATE OPERATOR pg_catalog.%(
leftarg = uint8, rightarg = uint1, procedure = uint8_uint1_mod,
commutator=operator(pg_catalog.%)
);
COMMENT ON OPERATOR pg_catalog.%(uint8, uint1) IS 'uint8_uint1_mod';

CREATE OPERATOR pg_catalog.=(
leftarg = uint8, rightarg = uint1, procedure = uint8_uint1_eq,
commutator=operator(pg_catalog.=)
);
COMMENT ON OPERATOR pg_catalog.=(uint8, uint1) IS 'uint8_uint1_eq';


CREATE OPERATOR pg_catalog.<>(
leftarg = uint8, rightarg = uint1, procedure = uint8_uint1_ne,
commutator=operator(pg_catalog.<>)
);
COMMENT ON OPERATOR pg_catalog.<>(uint8, uint1) IS 'uint8_uint1_ne';


CREATE OPERATOR pg_catalog.<(
leftarg = uint8, rightarg = uint1, procedure = uint8_uint1_lt,
commutator=operator(pg_catalog.<)
);
COMMENT ON OPERATOR pg_catalog.<(uint8, uint1) IS 'uint8_uint1_lt';


CREATE OPERATOR pg_catalog.<=(
leftarg = uint8, rightarg = uint1, procedure = uint8_uint1_le,
commutator=operator(pg_catalog.<=)
);
COMMENT ON OPERATOR pg_catalog.<=(uint8, uint1) IS 'uint8_uint1_le';


CREATE OPERATOR pg_catalog.>(
leftarg = uint8, rightarg = uint1, procedure = uint8_uint1_gt,
commutator=operator(pg_catalog.>)
);
COMMENT ON OPERATOR pg_catalog.>=(uint8, uint1) IS 'uint8_uint1_gt';


CREATE OPERATOR pg_catalog.>=(
leftarg = uint8, rightarg = uint1, procedure = uint8_uint1_ge,
commutator=operator(pg_catalog.>=)
);
COMMENT ON OPERATOR pg_catalog.>=(uint8, uint1) IS 'uint8_uint1_ge';


-------------------uint8 uint1---------------



-------------------uint8 uint4---------------
DROP FUNCTION IF EXISTS pg_catalog.uint8_pl_uint4(uint8,uint4) CASCADE;
CREATE FUNCTION pg_catalog.uint8_pl_uint4 (
uint8,uint4
) RETURNS uint8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint8_pl_uint4';

DROP FUNCTION IF EXISTS pg_catalog.uint8_mi_uint4(uint8,uint4) CASCADE;
CREATE FUNCTION pg_catalog.uint8_mi_uint4 (
uint8,uint4
) RETURNS int8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint8_mi_uint4';
DROP FUNCTION IF EXISTS pg_catalog.uint8_mul_uint4(uint8,uint4) CASCADE;
CREATE FUNCTION pg_catalog.uint8_mul_uint4 (
uint8,uint4
) RETURNS uint8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint8_mul_uint4';


DROP FUNCTION IF EXISTS pg_catalog.uint8_div_uint4(uint8,uint4) CASCADE;
CREATE FUNCTION pg_catalog.uint8_div_uint4 (
uint8,uint4
) RETURNS float8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint8_div_uint4';

DROP FUNCTION IF EXISTS pg_catalog.uint8_uint4_mod(uint8,uint4) CASCADE;
CREATE FUNCTION pg_catalog.uint8_uint4_mod (
uint8,uint4
) RETURNS float8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint8_uint4_mod';

DROP FUNCTION IF EXISTS pg_catalog.uint8_uint4_eq(uint8,uint4) CASCADE;
CREATE FUNCTION pg_catalog.uint8_uint4_eq (
uint8,uint4
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint8_uint4_eq';


DROP FUNCTION IF EXISTS pg_catalog.uint8_uint4_ne(uint8,uint4) CASCADE;
CREATE FUNCTION pg_catalog.uint8_uint4_ne (
uint8,uint4
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint8_uint4_ne';


DROP FUNCTION IF EXISTS pg_catalog.uint8_uint4_lt(uint8,uint4) CASCADE;
CREATE FUNCTION pg_catalog.uint8_uint4_lt (
uint8,uint4
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint8_uint4_lt';


DROP FUNCTION IF EXISTS pg_catalog.uint8_uint4_le(uint8,uint4) CASCADE;
CREATE FUNCTION pg_catalog.uint8_uint4_le (
uint8,uint4
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint8_uint4_le';


DROP FUNCTION IF EXISTS pg_catalog.uint8_uint4_gt(uint8,uint4) CASCADE;
CREATE FUNCTION pg_catalog.uint8_uint4_gt (
uint8,uint4
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint8_uint4_gt';


DROP FUNCTION IF EXISTS pg_catalog.uint8_uint4_ge(uint8,uint4) CASCADE;
CREATE FUNCTION pg_catalog.uint8_uint4_ge (
uint8,uint4
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint8_uint4_ge';


CREATE OPERATOR pg_catalog.+(
leftarg = uint8, rightarg = uint4, procedure = uint8_pl_uint4,
commutator=operator(pg_catalog.+)
);
COMMENT ON OPERATOR pg_catalog.+(uint8, uint4) IS 'uint8_pl_uint4';


CREATE OPERATOR pg_catalog.-(
leftarg = uint8, rightarg = uint4, procedure = uint8_mi_uint4,
commutator=operator(pg_catalog.-)
);
COMMENT ON OPERATOR pg_catalog.-(uint8, uint4) IS 'uint8_mi_uint4';


CREATE OPERATOR pg_catalog.*(
leftarg = uint8, rightarg = uint4, procedure = uint8_mul_uint4,
commutator=operator(pg_catalog.*)
);
COMMENT ON OPERATOR pg_catalog.*(uint8, uint4) IS 'uint8_mul_uint4';


CREATE OPERATOR pg_catalog./(
leftarg = uint8, rightarg = uint4, procedure = uint8_div_uint4,
commutator=operator(pg_catalog./)
);
COMMENT ON OPERATOR pg_catalog./(uint8, uint4) IS 'uint8_div_uint4';

CREATE OPERATOR pg_catalog.%(
leftarg = uint8, rightarg = uint4, procedure = uint8_uint4_mod,
commutator=operator(pg_catalog.%)
);
COMMENT ON OPERATOR pg_catalog.%(uint8, uint4) IS 'uint8_uint4_mod';



CREATE OPERATOR pg_catalog.=(
leftarg = uint8, rightarg = uint4, procedure = uint8_uint4_eq,
commutator=operator(pg_catalog.=)
);
COMMENT ON OPERATOR pg_catalog.=(uint8, uint4) IS 'uint8_uint4_eq';


CREATE OPERATOR pg_catalog.<>(
leftarg = uint8, rightarg = uint4, procedure = uint8_uint4_ne,
commutator=operator(pg_catalog.<>)
);
COMMENT ON OPERATOR pg_catalog.<>(uint8, uint4) IS 'uint8_uint4_ne';


CREATE OPERATOR pg_catalog.<(
leftarg = uint8, rightarg = uint4, procedure = uint8_uint4_lt,
commutator=operator(pg_catalog.<)
);
COMMENT ON OPERATOR pg_catalog.<(uint8, uint4) IS 'uint8_uint4_lt';


CREATE OPERATOR pg_catalog.<=(
leftarg = uint8, rightarg = uint4, procedure = uint8_uint4_le,
commutator=operator(pg_catalog.<=)
);
COMMENT ON OPERATOR pg_catalog.<=(uint8, uint4) IS 'uint8_uint4_le';


CREATE OPERATOR pg_catalog.>(
leftarg = uint8, rightarg = uint4, procedure = uint8_uint4_gt,
commutator=operator(pg_catalog.>)
);
COMMENT ON OPERATOR pg_catalog.>=(uint8, uint4) IS 'uint8_uint4_gt';


CREATE OPERATOR pg_catalog.>=(
leftarg = uint8, rightarg = uint4, procedure = uint8_uint4_ge,
commutator=operator(pg_catalog.>=)
);
COMMENT ON OPERATOR pg_catalog.>=(uint8, uint4) IS 'uint8_uint4_ge';




-------------------uint8 uint4---------------


-------------------uint8 uint2---------------

DROP FUNCTION IF EXISTS pg_catalog.uint8_pl_uint2(uint8,uint2) CASCADE;
CREATE FUNCTION pg_catalog.uint8_pl_uint2 (
uint8,uint2
) RETURNS int16 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint8_pl_uint2';

DROP FUNCTION IF EXISTS pg_catalog.uint8_mi_uint2(uint8,uint2) CASCADE;
CREATE FUNCTION pg_catalog.uint8_mi_uint2 (
uint8,uint2
) RETURNS int16 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint8_mi_uint2';
DROP FUNCTION IF EXISTS pg_catalog.uint8_mul_uint2(uint8,uint2) CASCADE;
CREATE FUNCTION pg_catalog.uint8_mul_uint2 (
uint8,uint2
) RETURNS int16 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint8_mul_uint2';


DROP FUNCTION IF EXISTS pg_catalog.uint8_div_uint2(uint8,uint2) CASCADE;
CREATE FUNCTION pg_catalog.uint8_div_uint2 (
uint8,uint2
) RETURNS float8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint8_div_uint2';

DROP FUNCTION IF EXISTS pg_catalog.uint8_uint2_mod(uint8,uint2) CASCADE;
CREATE FUNCTION pg_catalog.uint8_uint2_mod (
uint8,uint2
) RETURNS float8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint8_uint2_mod';


DROP FUNCTION IF EXISTS pg_catalog.uint8_uint2_eq(uint8,uint2) CASCADE;
CREATE FUNCTION pg_catalog.uint8_uint2_eq (
uint8,uint2
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint8_uint2_eq';


DROP FUNCTION IF EXISTS pg_catalog.uint8_uint2_ne(uint8,uint2) CASCADE;
CREATE FUNCTION pg_catalog.uint8_uint2_ne (
uint8,uint2
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint8_uint2_ne';


DROP FUNCTION IF EXISTS pg_catalog.uint8_uint2_lt(uint8,uint2) CASCADE;
CREATE FUNCTION pg_catalog.uint8_uint2_lt (
uint8,uint2
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint8_uint2_lt';


DROP FUNCTION IF EXISTS pg_catalog.uint8_uint2_le(uint8,uint2) CASCADE;
CREATE FUNCTION pg_catalog.uint8_uint2_le (
uint8,uint2
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint8_uint2_le';


DROP FUNCTION IF EXISTS pg_catalog.uint8_uint2_gt(uint8,uint2) CASCADE;
CREATE FUNCTION pg_catalog.uint8_uint2_gt (
uint8,uint2
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint8_uint2_gt';


DROP FUNCTION IF EXISTS pg_catalog.uint8_uint2_ge(uint8,uint2) CASCADE;
CREATE FUNCTION pg_catalog.uint8_uint2_ge (
uint8,uint2
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint8_uint2_ge';

CREATE OPERATOR pg_catalog.+(
leftarg = uint8, rightarg = uint2, procedure = uint8_pl_uint2,
commutator=operator(pg_catalog.+)
);
COMMENT ON OPERATOR pg_catalog.+(uint8, uint2) IS 'uint8_pl_uint2';


CREATE OPERATOR pg_catalog.-(
leftarg = uint8, rightarg = uint2, procedure = uint8_mi_uint2,
commutator=operator(pg_catalog.-)
);
COMMENT ON OPERATOR pg_catalog.-(uint8, uint2) IS 'uint8_mi_uint2';


CREATE OPERATOR pg_catalog.*(
leftarg = uint8, rightarg = uint2, procedure = uint8_mul_uint2,
commutator=operator(pg_catalog.*)
);
COMMENT ON OPERATOR pg_catalog.*(uint8, uint2) IS 'uint8_mul_uint2';


CREATE OPERATOR pg_catalog./(
leftarg = uint8, rightarg = uint2, procedure = uint8_div_uint2,
commutator=operator(pg_catalog./)
);
COMMENT ON OPERATOR pg_catalog./(uint8, uint2) IS 'uint8_div_uint2';

CREATE OPERATOR pg_catalog.%(
leftarg = uint8, rightarg = uint2, procedure = uint8_uint2_mod,
commutator=operator(pg_catalog.%)
);
COMMENT ON OPERATOR pg_catalog.%(uint8, uint2) IS 'uint8_uint2_mod';


CREATE OPERATOR pg_catalog.=(
leftarg = uint8, rightarg = uint2, procedure = uint8_uint2_eq,
commutator=operator(pg_catalog.=)
);
COMMENT ON OPERATOR pg_catalog.=(uint8, uint2) IS 'uint8_uint2_eq';



CREATE OPERATOR pg_catalog.<>(
leftarg = uint8, rightarg = uint2, procedure = uint8_uint2_ne,
commutator=operator(pg_catalog.<>)
);
COMMENT ON OPERATOR pg_catalog.<>(uint8, uint2) IS 'uint8_uint2_ne';




CREATE OPERATOR pg_catalog.<(
leftarg = uint8, rightarg = uint2, procedure = uint8_uint2_lt,
commutator=operator(pg_catalog.<)
);
COMMENT ON OPERATOR pg_catalog.<(uint8, uint2) IS 'uint8_uint2_lt';



CREATE OPERATOR pg_catalog.<=(
leftarg = uint8, rightarg = uint2, procedure = uint8_uint2_le,
commutator=operator(pg_catalog.<=)
);
COMMENT ON OPERATOR pg_catalog.<=(uint8, uint2) IS 'uint8_uint2_le';


CREATE OPERATOR pg_catalog.>(
leftarg = uint8, rightarg = uint2, procedure = uint8_uint2_gt,
commutator=operator(pg_catalog.>)
);
COMMENT ON OPERATOR pg_catalog.>=(uint8, uint2) IS 'uint8_uint2_gt';



CREATE OPERATOR pg_catalog.>=(
leftarg = uint8, rightarg = uint2, procedure = uint8_uint2_ge,
commutator=operator(pg_catalog.>=)
);
COMMENT ON OPERATOR pg_catalog.>=(uint8, uint2) IS 'uint8_uint2_ge';

-------------------uint8 uint2---------------
-------------------int1 uint8---------------

DROP FUNCTION IF EXISTS pg_catalog.int1_pl_uint8(int1,uint8) CASCADE;
CREATE FUNCTION pg_catalog.int1_pl_uint8 (
int1,uint8
) RETURNS int2 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int1_pl_uint8';

DROP FUNCTION IF EXISTS pg_catalog.int1_mi_uint8(int1,uint8) CASCADE;
CREATE FUNCTION pg_catalog.int1_mi_uint8 (
int1,uint8
) RETURNS int2 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int1_mi_uint8';
DROP FUNCTION IF EXISTS pg_catalog.int1_mul_uint8(int1,uint8) CASCADE;
CREATE FUNCTION pg_catalog.int1_mul_uint8 (
int1,uint8
) RETURNS int2 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int1_mul_uint8';


DROP FUNCTION IF EXISTS pg_catalog.int1_div_uint8(int1,uint8) CASCADE;
CREATE FUNCTION pg_catalog.int1_div_uint8 (
int1,uint8
) RETURNS float8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int1_div_uint8';

DROP FUNCTION IF EXISTS pg_catalog.int1_uint8_mod(int1,uint8) CASCADE;
CREATE FUNCTION pg_catalog.int1_uint8_mod (
int1,uint8
) RETURNS float8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int1_uint8_mod';


DROP FUNCTION IF EXISTS pg_catalog.int1_uint8_eq(int1,uint8) CASCADE;
CREATE FUNCTION pg_catalog.int1_uint8_eq (
int1,uint8
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int1_uint8_eq';


DROP FUNCTION IF EXISTS pg_catalog.int1_uint8_ne(int1,uint8) CASCADE;
CREATE FUNCTION pg_catalog.int1_uint8_ne (
int1,uint8
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int1_uint8_ne';


DROP FUNCTION IF EXISTS pg_catalog.int1_uint8_lt(int1,uint8) CASCADE;
CREATE FUNCTION pg_catalog.int1_uint8_lt (
int1,uint8
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int1_uint8_lt';


DROP FUNCTION IF EXISTS pg_catalog.int1_uint8_le(int1,uint8) CASCADE;
CREATE FUNCTION pg_catalog.int1_uint8_le (
int1,uint8
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int1_uint8_le';


DROP FUNCTION IF EXISTS pg_catalog.int1_uint8_gt(int1,uint8) CASCADE;
CREATE FUNCTION pg_catalog.int1_uint8_gt (
int1,uint8
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int1_uint8_gt';


DROP FUNCTION IF EXISTS pg_catalog.int1_uint8_ge(int1,uint8) CASCADE;
CREATE FUNCTION pg_catalog.int1_uint8_ge (
int1,uint8
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int1_uint8_ge';

CREATE OPERATOR pg_catalog.+(
leftarg = int1, rightarg = uint8, procedure = int1_pl_uint8,
commutator=operator(pg_catalog.+)
);
COMMENT ON OPERATOR pg_catalog.+(int1, uint8) IS 'int1_pl_uint8';


CREATE OPERATOR pg_catalog.-(
leftarg = int1, rightarg = uint8, procedure = int1_mi_uint8,
commutator=operator(pg_catalog.-)
);
COMMENT ON OPERATOR pg_catalog.-(int1, uint8) IS 'int1_mi_uint8';


CREATE OPERATOR pg_catalog.*(
leftarg = int1, rightarg = uint8, procedure = int1_mul_uint8,
commutator=operator(pg_catalog.*)
);
COMMENT ON OPERATOR pg_catalog.*(int1, uint8) IS 'int1_mul_uint8';


CREATE OPERATOR pg_catalog./(
leftarg = int1, rightarg = uint8, procedure = int1_div_uint8,
commutator=operator(pg_catalog./)
);
COMMENT ON OPERATOR pg_catalog./(int1, uint8) IS 'int1_div_uint8';

CREATE OPERATOR pg_catalog.%(
leftarg = int1, rightarg = uint8, procedure = int1_uint8_mod,
commutator=operator(pg_catalog.%)
);
COMMENT ON OPERATOR pg_catalog.%(int1, uint8) IS 'int1_uint8_mod';


CREATE OPERATOR pg_catalog.=(
leftarg = int1, rightarg = uint8, procedure = int1_uint8_eq,
commutator=operator(pg_catalog.=)
);
COMMENT ON OPERATOR pg_catalog.=(int1, uint8) IS 'int1_uint8_eq';



CREATE OPERATOR pg_catalog.<>(
leftarg = int1, rightarg = uint8, procedure = int1_uint8_ne,
commutator=operator(pg_catalog.<>)
);
COMMENT ON OPERATOR pg_catalog.<>(int1, uint8) IS 'int1_uint8_ne';




CREATE OPERATOR pg_catalog.<(
leftarg = int1, rightarg = uint8, procedure = int1_uint8_lt,
commutator=operator(pg_catalog.<)
);
COMMENT ON OPERATOR pg_catalog.<(int1, uint8) IS 'int1_uint8_lt';



CREATE OPERATOR pg_catalog.<=(
leftarg = int1, rightarg = uint8, procedure = int1_uint8_le,
commutator=operator(pg_catalog.<=)
);
COMMENT ON OPERATOR pg_catalog.<=(int1, uint8) IS 'int1_uint8_le';


CREATE OPERATOR pg_catalog.>(
leftarg = int1, rightarg = uint8, procedure = int1_uint8_gt,
commutator=operator(pg_catalog.>)
);
COMMENT ON OPERATOR pg_catalog.>=(int1, uint8) IS 'int1_uint8_gt';



CREATE OPERATOR pg_catalog.>=(
leftarg = int1, rightarg = uint8, procedure = int1_uint8_ge,
commutator=operator(pg_catalog.>=)
);
COMMENT ON OPERATOR pg_catalog.>=(int1, uint8) IS 'int1_uint8_ge';

-------------------int1 uint8---------------


-------------------int2 uint8---------------



DROP FUNCTION IF EXISTS pg_catalog.int2_pl_uint8(int2,uint8) CASCADE;
CREATE FUNCTION pg_catalog.int2_pl_uint8 (
int2,uint8
) RETURNS int4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int2_pl_uint8';

DROP FUNCTION IF EXISTS pg_catalog.int2_mi_uint8(int2,uint8) CASCADE;
CREATE FUNCTION pg_catalog.int2_mi_uint8 (
int2,uint8
) RETURNS int4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int2_mi_uint8';
DROP FUNCTION IF EXISTS pg_catalog.int2_mul_uint8(int2,uint8) CASCADE;
CREATE FUNCTION pg_catalog.int2_mul_uint8 (
int2,uint8
) RETURNS int4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int2_mul_uint8';


DROP FUNCTION IF EXISTS pg_catalog.int2_div_uint8(int2,uint8) CASCADE;
CREATE FUNCTION pg_catalog.int2_div_uint8 (
int2,uint8
) RETURNS float8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int2_div_uint8';

DROP FUNCTION IF EXISTS pg_catalog.int2_uint8_mod(int2,uint8) CASCADE;
CREATE FUNCTION pg_catalog.int2_uint8_mod (
int2,uint8
) RETURNS float8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int2_uint8_mod';


DROP FUNCTION IF EXISTS pg_catalog.int2_uint8_eq(int2,uint8) CASCADE;
CREATE FUNCTION pg_catalog.int2_uint8_eq (
int2,uint8
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int2_uint8_eq';


DROP FUNCTION IF EXISTS pg_catalog.int2_uint8_ne(int2,uint8) CASCADE;
CREATE FUNCTION pg_catalog.int2_uint8_ne (
int2,uint8
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int2_uint8_ne';


DROP FUNCTION IF EXISTS pg_catalog.int2_uint8_lt(int2,uint8) CASCADE;
CREATE FUNCTION pg_catalog.int2_uint8_lt (
int2,uint8
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int2_uint8_lt';


DROP FUNCTION IF EXISTS pg_catalog.int2_uint8_le(int2,uint8) CASCADE;
CREATE FUNCTION pg_catalog.int2_uint8_le (
int2,uint8
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int2_uint8_le';


DROP FUNCTION IF EXISTS pg_catalog.int2_uint8_gt(int2,uint8) CASCADE;
CREATE FUNCTION pg_catalog.int2_uint8_gt (
int2,uint8
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int2_uint8_gt';


DROP FUNCTION IF EXISTS pg_catalog.int2_uint8_ge(int2,uint8) CASCADE;
CREATE FUNCTION pg_catalog.int2_uint8_ge (
int2,uint8
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int2_uint8_ge';


CREATE OPERATOR pg_catalog.+(
leftarg = int2, rightarg = uint8, procedure = int2_pl_uint8,
commutator=operator(pg_catalog.+)
);
COMMENT ON OPERATOR pg_catalog.+(int2, uint8) IS 'int2_pl_uint8';


CREATE OPERATOR pg_catalog.-(
leftarg = int2, rightarg = uint8, procedure = int2_mi_uint8,
commutator=operator(pg_catalog.-)
);
COMMENT ON OPERATOR pg_catalog.-(int2, uint8) IS 'int2_mi_uint8';


CREATE OPERATOR pg_catalog.*(
leftarg = int2, rightarg = uint8, procedure = int2_mul_uint8,
commutator=operator(pg_catalog.*)
);
COMMENT ON OPERATOR pg_catalog.*(int2, uint8) IS 'int2_mul_uint8';


CREATE OPERATOR pg_catalog./(
leftarg = int2, rightarg = uint8, procedure = int2_div_uint8,
commutator=operator(pg_catalog./)
);
COMMENT ON OPERATOR pg_catalog./(int2, uint8) IS 'int2_div_uint8';

CREATE OPERATOR pg_catalog.%(
leftarg = int2, rightarg = uint8, procedure = int2_uint8_mod,
commutator=operator(pg_catalog.%)
);
COMMENT ON OPERATOR pg_catalog.%(int2, uint8) IS 'int2_uint8_mod';

CREATE OPERATOR pg_catalog.=(
leftarg = int2, rightarg = uint8, procedure = int2_uint8_eq,
commutator=operator(pg_catalog.=)
);
COMMENT ON OPERATOR pg_catalog.=(int2, uint8) IS 'int2_uint8_eq';



CREATE OPERATOR pg_catalog.<>(
leftarg = int2, rightarg = uint8, procedure = int2_uint8_ne,
commutator=operator(pg_catalog.<>)
);
COMMENT ON OPERATOR pg_catalog.<>(int2, uint8) IS 'int2_uint8_ne';




CREATE OPERATOR pg_catalog.<(
leftarg = int2, rightarg = uint8, procedure = int2_uint8_lt,
commutator=operator(pg_catalog.<)
);
COMMENT ON OPERATOR pg_catalog.<(int2, uint8) IS 'int2_uint8_lt';



CREATE OPERATOR pg_catalog.<=(
leftarg = int2, rightarg = uint8, procedure = int2_uint8_le,
commutator=operator(pg_catalog.<=)
);
COMMENT ON OPERATOR pg_catalog.<=(int2, uint8) IS 'int2_uint8_le';


CREATE OPERATOR pg_catalog.>(
leftarg = int2, rightarg = uint8, procedure = int2_uint8_gt,
commutator=operator(pg_catalog.>)
);
COMMENT ON OPERATOR pg_catalog.>=(int2, uint8) IS 'int2_uint8_gt';



CREATE OPERATOR pg_catalog.>=(
leftarg = int2, rightarg = uint8, procedure = int2_uint8_ge,
commutator=operator(pg_catalog.>=)
);
COMMENT ON OPERATOR pg_catalog.>=(int2, uint8) IS 'int2_uint8_ge';


-------------------int2 uint8---------------

-------------------int4 uint8---------------
DROP FUNCTION IF EXISTS pg_catalog.int4_pl_uint8(int4,uint8) CASCADE;
CREATE FUNCTION pg_catalog.int4_pl_uint8 (
int4,uint8
) RETURNS int8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int4_pl_uint8';

DROP FUNCTION IF EXISTS pg_catalog.int4_mi_uint8(int4,uint8) CASCADE;
CREATE FUNCTION pg_catalog.int4_mi_uint8 (
int4,uint8
) RETURNS int8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int4_mi_uint8';
DROP FUNCTION IF EXISTS pg_catalog.int4_mul_uint8(int4,uint8) CASCADE;
CREATE FUNCTION pg_catalog.int4_mul_uint8 (
int4,uint8
) RETURNS int8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int4_mul_uint8';


DROP FUNCTION IF EXISTS pg_catalog.int4_div_uint8(int4,uint8) CASCADE;
CREATE FUNCTION pg_catalog.int4_div_uint8 (
int4,uint8
) RETURNS float8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int4_div_uint8';


DROP FUNCTION IF EXISTS pg_catalog.int4_uint8_mod(int4,uint8) CASCADE;
CREATE FUNCTION pg_catalog.int4_uint8_mod (
int4,uint8
) RETURNS float8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int4_uint8_mod';

DROP FUNCTION IF EXISTS pg_catalog.int4_uint8_eq(int4,uint8) CASCADE;
CREATE FUNCTION pg_catalog.int4_uint8_eq (
int4,uint8
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int4_uint8_eq';


DROP FUNCTION IF EXISTS pg_catalog.int4_uint8_ne(int4,uint8) CASCADE;
CREATE FUNCTION pg_catalog.int4_uint8_ne (
int4,uint8
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int4_uint8_ne';


DROP FUNCTION IF EXISTS pg_catalog.int4_uint8_lt(int4,uint8) CASCADE;
CREATE FUNCTION pg_catalog.int4_uint8_lt (
int4,uint8
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int4_uint8_lt';


DROP FUNCTION IF EXISTS pg_catalog.int4_uint8_le(int4,uint8) CASCADE;
CREATE FUNCTION pg_catalog.int4_uint8_le (
int4,uint8
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int4_uint8_le';


DROP FUNCTION IF EXISTS pg_catalog.int4_uint8_gt(int4,uint8) CASCADE;
CREATE FUNCTION pg_catalog.int4_uint8_gt (
int4,uint8
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int4_uint8_gt';


DROP FUNCTION IF EXISTS pg_catalog.int4_uint8_ge(int4,uint8) CASCADE;
CREATE FUNCTION pg_catalog.int4_uint8_ge (
int4,uint8
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int4_uint8_ge';

CREATE OPERATOR pg_catalog.+(
leftarg = int4, rightarg = uint8, procedure = int4_pl_uint8,
commutator=operator(pg_catalog.+)
);
COMMENT ON OPERATOR pg_catalog.+(int4, uint8) IS 'int4_pl_uint8';


CREATE OPERATOR pg_catalog.-(
leftarg = int4, rightarg = uint8, procedure = int4_mi_uint8,
commutator=operator(pg_catalog.-)
);
COMMENT ON OPERATOR pg_catalog.-(int4, uint8) IS 'int4_mi_uint8';


CREATE OPERATOR pg_catalog.*(
leftarg = int4, rightarg = uint8, procedure = int4_mul_uint8,
commutator=operator(pg_catalog.*)
);
COMMENT ON OPERATOR pg_catalog.*(int4, uint8) IS 'int4_mul_uint8';


CREATE OPERATOR pg_catalog./(
leftarg = int4, rightarg = uint8, procedure = int4_div_uint8,
commutator=operator(pg_catalog./)
);

COMMENT ON OPERATOR pg_catalog./(int4, uint8) IS 'int4_div_uint8';

CREATE OPERATOR pg_catalog.%(
leftarg = int4, rightarg = uint8, procedure = int4_uint8_mod,
commutator=operator(pg_catalog.%)
);

COMMENT ON OPERATOR pg_catalog.%(int4, uint8) IS 'int4_uint8_mod';

CREATE OPERATOR pg_catalog.=(
leftarg = int4, rightarg = uint8, procedure = int4_uint8_eq,
commutator=operator(pg_catalog.=)
);
COMMENT ON OPERATOR pg_catalog.=(int4, uint8) IS 'int4_uint8_eq';



CREATE OPERATOR pg_catalog.<>(
leftarg = int4, rightarg = uint8, procedure = int4_uint8_ne,
commutator=operator(pg_catalog.<>)
);
COMMENT ON OPERATOR pg_catalog.<>(int4, uint8) IS 'int4_uint8_ne';




CREATE OPERATOR pg_catalog.<(
leftarg = int4, rightarg = uint8, procedure = int4_uint8_lt,
commutator=operator(pg_catalog.<)
);
COMMENT ON OPERATOR pg_catalog.<(int4, uint8) IS 'int4_uint8_lt';



CREATE OPERATOR pg_catalog.<=(
leftarg = int4, rightarg = uint8, procedure = int4_uint8_le,
commutator=operator(pg_catalog.<=)
);
COMMENT ON OPERATOR pg_catalog.<=(int4, uint8) IS 'int4_uint8_le';


CREATE OPERATOR pg_catalog.>(
leftarg = int4, rightarg = uint8, procedure = int4_uint8_gt,
commutator=operator(pg_catalog.>)
);
COMMENT ON OPERATOR pg_catalog.>=(int4, uint8) IS 'int4_uint8_gt';



CREATE OPERATOR pg_catalog.>=(
leftarg = int4, rightarg = uint8, procedure = int4_uint8_ge,
commutator=operator(pg_catalog.>=)
);
COMMENT ON OPERATOR pg_catalog.>=(int4, uint8) IS 'int4_uint8_ge';

-------------------int4 uint8---------------

-------------------int8 uint8---------------
DROP FUNCTION IF EXISTS pg_catalog.int8_pl_uint8(int8,uint8) CASCADE;
CREATE FUNCTION pg_catalog.int8_pl_uint8 (
int8,uint8
) RETURNS int16 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int8_pl_uint8';

DROP FUNCTION IF EXISTS pg_catalog.int8_mi_uint8(int8,uint8) CASCADE;
CREATE FUNCTION pg_catalog.int8_mi_uint8 (
int8,uint8
) RETURNS int16 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int8_mi_uint8';
DROP FUNCTION IF EXISTS pg_catalog.int8_mul_uint8(int8,uint8) CASCADE;
CREATE FUNCTION pg_catalog.int8_mul_uint8 (
int8,uint8
) RETURNS int16 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int8_mul_uint8';


DROP FUNCTION IF EXISTS pg_catalog.int8_div_uint8(int8,uint8) CASCADE;
CREATE FUNCTION pg_catalog.int8_div_uint8 (
int8,uint8
) RETURNS float8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int8_div_uint8';

DROP FUNCTION IF EXISTS pg_catalog.int8_uint8_mod(int8,uint8) CASCADE;
CREATE FUNCTION pg_catalog.int8_uint8_mod (
int8,uint8
) RETURNS float8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int8_uint8_mod';


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
commutator=operator(pg_catalog.=)
);
COMMENT ON OPERATOR pg_catalog.=(int8, uint8) IS 'int8_uint8_eq';



CREATE OPERATOR pg_catalog.<>(
leftarg = int8, rightarg = uint8, procedure = int8_uint8_ne,
commutator=operator(pg_catalog.<>)
);
COMMENT ON OPERATOR pg_catalog.<>(int8, uint8) IS 'int8_uint8_ne';




CREATE OPERATOR pg_catalog.<(
leftarg = int8, rightarg = uint8, procedure = int8_uint8_lt,
commutator=operator(pg_catalog.<)
);
COMMENT ON OPERATOR pg_catalog.<(int8, uint8) IS 'int8_uint8_lt';



CREATE OPERATOR pg_catalog.<=(
leftarg = int8, rightarg = uint8, procedure = int8_uint8_le,
commutator=operator(pg_catalog.<=)
);
COMMENT ON OPERATOR pg_catalog.<=(int8, uint8) IS 'int8_uint8_le';


CREATE OPERATOR pg_catalog.>(
leftarg = int8, rightarg = uint8, procedure = int8_uint8_gt,
commutator=operator(pg_catalog.>)
);
COMMENT ON OPERATOR pg_catalog.>=(int8, uint8) IS 'int8_uint8_gt';



CREATE OPERATOR pg_catalog.>=(
leftarg = int8, rightarg = uint8, procedure = int8_uint8_ge,
commutator=operator(pg_catalog.>=)
);
COMMENT ON OPERATOR pg_catalog.>=(int8, uint8) IS 'int8_uint8_ge';

-------------------int8 uint8---------------
----------------------------------------------------------------
-- add  agg 
----------------------------------------------------------------



drop aggregate if exists pg_catalog.avg(uint8);
create aggregate pg_catalog.avg(uint8) (SFUNC=uint8_avg_accum, cFUNC=int8_avg_collect, STYPE= bigint[], finalfunc = int8_avg, initcond = '{0,0}', initcollect = '{0,0}');


drop aggregate if exists pg_catalog.sum(uint8);
create aggregate pg_catalog.sum(uint8) (SFUNC=uint8_sum, cFUNC=int8_sum_to_int8, STYPE= int8 );




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


drop aggregate if exists pg_catalog.bit_and(uint8);
create aggregate pg_catalog.bit_and(uint8) (SFUNC=uint8and, cFUNC = uint8and, STYPE= uint8);

drop aggregate if exists pg_catalog.bit_or(uint8);
create aggregate pg_catalog.bit_or(uint8) (SFUNC=uint8or, cFUNC = uint8or,  STYPE= uint8);


drop aggregate if exists pg_catalog.listagg(uint8,text);
create aggregate pg_catalog.listagg(uint8,text) (SFUNC=uint8_list_agg_transfn, finalfunc = list_agg_finalfn,  STYPE= internal);


drop aggregate if exists pg_catalog.listagg(uint8);
create aggregate pg_catalog.listagg(uint8) (SFUNC=uint8_list_agg_noarg2_transfn, finalfunc = list_agg_finalfn,  STYPE= internal);
----------------------------------------------------------------
-- add  opfamily_opclass 
----------------------------------------------------------------


DO $$
DECLARE
ans boolean;
oidopfamily    oid;
oidoptype     oid;
oidopampro     oid;
BEGIN

    
    insert into pg_catalog.pg_opfamily values (403, 'uint1_ops', 11, 10);    
    select oid into oidopfamily from pg_catalog.pg_opfamily where opfmethod = 403 and opfname = 'uint1_ops';
    select oid into oidoptype from pg_catalog.pg_type where typname = 'uint1';
    insert into pg_catalog.pg_opclass values (403, 'uint1_ops', 11, 10, oidopfamily, oidoptype, true, 0);
	select oid into oidopampro from pg_catalog.pg_proc where proname = 'uint1cmp';
	insert into pg_catalog.pg_amproc values (oidopfamily, oidoptype, oidoptype, 1, oidopampro);
    
    insert into pg_catalog.pg_opfamily values (405, 'uint1_ops', 11, 10);    
    select oid into oidopfamily from pg_catalog.pg_opfamily where opfmethod = 405 and opfname = 'uint1_ops';
    select oid into oidoptype from pg_catalog.pg_type where typname = 'uint1';
    insert into pg_catalog.pg_opclass values (405, 'uint1_ops', 11, 10, oidopfamily, oidoptype, true, 0);
	select oid into oidopampro from pg_catalog.pg_proc where proname = 'hashuint1';
	insert into pg_catalog.pg_amproc values (oidopfamily, oidoptype, oidoptype, 1, oidopampro);
    
    
    insert into pg_catalog.pg_opfamily values (403, 'uint2_ops', 11, 10);    
    select oid into oidopfamily from pg_catalog.pg_opfamily where opfmethod = 403 and opfname = 'uint2_ops';
    select oid into oidoptype from pg_catalog.pg_type where typname = 'uint2';
    insert into pg_catalog.pg_opclass values (403, 'uint2_ops', 11, 10, oidopfamily, oidoptype, true, 0);
	select oid into oidopampro from pg_catalog.pg_proc where proname = 'uint2cmp';
	insert into pg_catalog.pg_amproc values (oidopfamily, oidoptype, oidoptype, 1, oidopampro);
    
    
    insert into pg_catalog.pg_opfamily values (405, 'uint2_ops', 11, 10);    
    select oid into oidopfamily from pg_catalog.pg_opfamily where opfmethod = 405 and opfname = 'uint2_ops';
    select oid into oidoptype from pg_catalog.pg_type where typname = 'uint2';
    insert into pg_catalog.pg_opclass values (405, 'uint2_ops', 11, 10, oidopfamily, oidoptype, true, 0);
	select oid into oidopampro from pg_catalog.pg_proc where proname = 'hashuint2';
	insert into pg_catalog.pg_amproc values (oidopfamily, oidoptype, oidoptype, 1, oidopampro);
    
    
    insert into pg_catalog.pg_opfamily values (403, 'uint4_ops', 11, 10);    
    select oid into oidopfamily from pg_catalog.pg_opfamily where opfmethod = 403 and opfname = 'uint4_ops';
    select oid into oidoptype from pg_catalog.pg_type where typname = 'uint4';
    insert into pg_catalog.pg_opclass values (403, 'uint4_ops', 11, 10, oidopfamily, oidoptype, true, 0);
	select oid into oidopampro from pg_catalog.pg_proc where proname = 'uint4cmp';
	insert into pg_catalog.pg_amproc values (oidopfamily, oidoptype, oidoptype, 1, oidopampro);
    
    
    insert into pg_catalog.pg_opfamily values (405, 'uint4_ops', 11, 10);    
    select oid into oidopfamily from pg_catalog.pg_opfamily where opfmethod = 405 and opfname = 'uint4_ops';
    select oid into oidoptype from pg_catalog.pg_type where typname = 'uint4';
    insert into pg_catalog.pg_opclass values (405, 'uint4_ops', 11, 10, oidopfamily, oidoptype, true, 0);
	select oid into oidopampro from pg_catalog.pg_proc where proname = 'hashuint4';
	insert into pg_catalog.pg_amproc values (oidopfamily, oidoptype, oidoptype, 1, oidopampro);
    
     insert into pg_catalog.pg_opfamily values (403, 'uint8_ops', 11, 10);    
    select oid into oidopfamily from pg_catalog.pg_opfamily where opfmethod = 403 and opfname = 'uint8_ops';
    select oid into oidoptype from pg_catalog.pg_type where typname = 'uint8';
    insert into pg_catalog.pg_opclass values (403, 'uint8_ops', 11, 10, oidopfamily, oidoptype, true, 0);
	select oid into oidopampro from pg_catalog.pg_proc where proname = 'uint8cmp';
	insert into pg_catalog.pg_amproc values (oidopfamily, oidoptype, oidoptype, 1, oidopampro);
    
    
    insert into pg_catalog.pg_opfamily values (405, 'uint8_ops', 11, 10);    
    select oid into oidopfamily from pg_catalog.pg_opfamily where opfmethod = 405 and opfname = 'uint8_ops';
    select oid into oidoptype from pg_catalog.pg_type where typname = 'uint8';
    insert into pg_catalog.pg_opclass values (405, 'uint8_ops', 11, 10, oidopfamily, oidoptype, true, 0);
	select oid into oidopampro from pg_catalog.pg_proc where proname = 'hashuint8';
	insert into pg_catalog.pg_amproc values (oidopfamily, oidoptype, oidoptype, 1, oidopampro);
END$$;




DO $$
DECLARE
oidopfamily oid;
oidoptype oid;
oidopproc oid;
oidoperator oid;
BEGIN

   select oid into oidopfamily from pg_catalog.pg_opfamily where opfmethod = 403 and opfname = 'uint1_ops';
   select oid into oidoptype from pg_catalog.pg_type where typname = 'uint1';
   select oid  into oidopproc from pg_catalog.pg_proc where proname = 'uint1lt';
   select oid  into oidoperator from pg_catalog.pg_operator where oprcode = oidopproc;
   insert into pg_catalog.pg_amop values (oidopfamily, oidoptype, oidoptype,  1,'s',oidoperator, 403, 0);
   
   
   select oid into oidopfamily from pg_catalog.pg_opfamily where opfmethod = 403 and opfname = 'uint1_ops';
   select oid into oidoptype from pg_catalog.pg_type where typname = 'uint1';
   select oid  into oidopproc from pg_catalog.pg_proc where proname = 'uint1le';
   select oid  into oidoperator from pg_catalog.pg_operator where oprcode = oidopproc;
   insert into pg_catalog.pg_amop values (oidopfamily, oidoptype, oidoptype,  2,'s',oidoperator, 403, 0);
   
    select oid into oidopfamily from pg_catalog.pg_opfamily where opfmethod = 403 and opfname = 'uint1_ops';
   select oid into oidoptype from pg_catalog.pg_type where typname = 'uint1';
   select oid  into oidopproc from pg_catalog.pg_proc where proname = 'uint1eq';
   select oid  into oidoperator from pg_catalog.pg_operator where oprcode = oidopproc;
   insert into pg_catalog.pg_amop values (oidopfamily, oidoptype, oidoptype,  3,'s',oidoperator, 403, 0);
   
   select oid into oidopfamily from pg_catalog.pg_opfamily where opfmethod = 403 and opfname = 'uint1_ops';
   select oid into oidoptype from pg_catalog.pg_type where typname = 'uint1';
   select oid  into oidopproc from pg_catalog.pg_proc where proname = 'uint1gt';
   select oid  into oidoperator from pg_catalog.pg_operator where oprcode = oidopproc;
   insert into pg_catalog.pg_amop values (oidopfamily, oidoptype, oidoptype,  5,'s',oidoperator, 403, 0);
   
   
    select oid into oidopfamily from pg_catalog.pg_opfamily where opfmethod = 403 and opfname = 'uint1_ops';
   select oid into oidoptype from pg_catalog.pg_type where typname = 'uint1';
   select oid  into oidopproc from pg_catalog.pg_proc where proname = 'uint1ge';
   select oid  into oidoperator from pg_catalog.pg_operator where oprcode = oidopproc;
   insert into pg_catalog.pg_amop values (oidopfamily, oidoptype, oidoptype,  4,'s',oidoperator, 403, 0);

   
   select oid into oidopfamily from pg_catalog.pg_opfamily where opfmethod = 405 and opfname = 'uint1_ops';
   select oid into oidoptype from pg_catalog.pg_type where typname = 'uint1';
   select oid  into oidopproc from pg_catalog.pg_proc where proname = 'uint1eq';
   select oid  into oidoperator from pg_catalog.pg_operator where oprcode = oidopproc;
   insert into pg_catalog.pg_amop values (oidopfamily, oidoptype, oidoptype,  1,'s',oidoperator, 405, 0);
END$$;



DO $$
DECLARE
oidopfamily oid;
oidoptype oid;
oidopproc oid;
oidoperator oid;
BEGIN

   select oid into oidopfamily from pg_catalog.pg_opfamily where opfmethod = 403 and opfname = 'uint2_ops';
   select oid into oidoptype from pg_catalog.pg_type where typname = 'uint2';
   select oid  into oidopproc from pg_catalog.pg_proc where proname = 'uint2lt';
   select oid  into oidoperator from pg_catalog.pg_operator where oprcode = oidopproc;
   insert into pg_catalog.pg_amop values (oidopfamily, oidoptype, oidoptype,  1,'s',oidoperator, 403, 0);
   
   
    select oid into oidopfamily from pg_catalog.pg_opfamily where opfmethod = 403 and opfname = 'uint2_ops';
   select oid into oidoptype from pg_catalog.pg_type where typname = 'uint2';
   select oid  into oidopproc from pg_catalog.pg_proc where proname = 'uint2le';
   select oid  into oidoperator from pg_catalog.pg_operator where oprcode = oidopproc;
   insert into pg_catalog.pg_amop values (oidopfamily, oidoptype, oidoptype,  2,'s',oidoperator, 403, 0);
   
     select oid into oidopfamily from pg_catalog.pg_opfamily where opfmethod = 403 and opfname = 'uint2_ops';
   select oid into oidoptype from pg_catalog.pg_type where typname = 'uint2';
   select oid  into oidopproc from pg_catalog.pg_proc where proname = 'uint2eq';
   select oid  into oidoperator from pg_catalog.pg_operator where oprcode = oidopproc;
   insert into pg_catalog.pg_amop values (oidopfamily, oidoptype, oidoptype,  3,'s',oidoperator, 403, 0);
   
    select oid into oidopfamily from pg_catalog.pg_opfamily where opfmethod = 403 and opfname = 'uint2_ops';
   select oid into oidoptype from pg_catalog.pg_type where typname = 'uint2';
   select oid  into oidopproc from pg_catalog.pg_proc where proname = 'uint2gt';
   select oid  into oidoperator from pg_catalog.pg_operator where oprcode = oidopproc;
   insert into pg_catalog.pg_amop values (oidopfamily, oidoptype, oidoptype,  4,'s',oidoperator, 403, 0);
   
   
   select oid into oidopfamily from pg_catalog.pg_opfamily where opfmethod = 403 and opfname = 'uint2_ops';
   select oid into oidoptype from pg_catalog.pg_type where typname = 'uint2';
   select oid  into oidopproc from pg_catalog.pg_proc where proname = 'uint2ge';
   select oid  into oidoperator from pg_catalog.pg_operator where oprcode = oidopproc;
   insert into pg_catalog.pg_amop values (oidopfamily, oidoptype, oidoptype,  5,'s',oidoperator, 403, 0);

   
   select oid into oidopfamily from pg_catalog.pg_opfamily where opfmethod = 405 and opfname = 'uint2_ops';
   select oid into oidoptype from pg_catalog.pg_type where typname = 'uint2';
   select oid  into oidopproc from pg_catalog.pg_proc where proname = 'uint2eq';
   select oid  into oidoperator from pg_catalog.pg_operator where oprcode = oidopproc;
   insert into pg_catalog.pg_amop values (oidopfamily, oidoptype, oidoptype,  1,'s',oidoperator, 405, 0);
END$$;


DO $$
DECLARE
oidopfamily oid;
oidoptype oid;
oidopproc oid;
oidoperator oid;
BEGIN

   select oid into oidopfamily from pg_catalog.pg_opfamily where opfmethod = 403 and opfname = 'uint4_ops';
   select oid into oidoptype from pg_catalog.pg_type where typname = 'uint4';
   select oid  into oidopproc from pg_catalog.pg_proc where proname = 'uint4lt';
   select oid  into oidoperator from pg_catalog.pg_operator where oprcode = oidopproc;
   insert into pg_catalog.pg_amop values (oidopfamily, oidoptype, oidoptype,  2,'s',oidoperator, 403, 0);
   
   
    select oid into oidopfamily from pg_catalog.pg_opfamily where opfmethod = 403 and opfname = 'uint4_ops';
   select oid into oidoptype from pg_catalog.pg_type where typname = 'uint4';
   select oid  into oidopproc from pg_catalog.pg_proc where proname = 'uint4le';
   select oid  into oidoperator from pg_catalog.pg_operator where oprcode = oidopproc;
   insert into pg_catalog.pg_amop values (oidopfamily, oidoptype, oidoptype, 3,'s',oidoperator, 403, 0);
   
     select oid into oidopfamily from pg_catalog.pg_opfamily where opfmethod = 403 and opfname = 'uint4_ops';
   select oid into oidoptype from pg_catalog.pg_type where typname = 'uint4';
   select oid  into oidopproc from pg_catalog.pg_proc where proname = 'uint4eq';
   select oid  into oidoperator from pg_catalog.pg_operator where oprcode = oidopproc;
   insert into pg_catalog.pg_amop values (oidopfamily, oidoptype, oidoptype,  0,'s',oidoperator, 403, 0);
   
     select oid into oidopfamily from pg_catalog.pg_opfamily where opfmethod = 403 and opfname = 'uint4_ops';
   select oid into oidoptype from pg_catalog.pg_type where typname = 'uint4';
   select oid  into oidopproc from pg_catalog.pg_proc where proname = 'uint4gt';
   select oid  into oidoperator from pg_catalog.pg_operator where oprcode = oidopproc;
   insert into pg_catalog.pg_amop values (oidopfamily, oidoptype, oidoptype,  5,'s',oidoperator, 403, 0);
   
   
    select oid into oidopfamily from pg_catalog.pg_opfamily where opfmethod = 403 and opfname = 'uint4_ops';
   select oid into oidoptype from pg_catalog.pg_type where typname = 'uint4';
   select oid  into oidopproc from pg_catalog.pg_proc where proname = 'uint4ge';
   select oid  into oidoperator from pg_catalog.pg_operator where oprcode = oidopproc;
   insert into pg_catalog.pg_amop values (oidopfamily, oidoptype, oidoptype,  4,'s',oidoperator, 403, 0);

   
   select oid into oidopfamily from pg_catalog.pg_opfamily where opfmethod = 405 and opfname = 'uint4_ops';
   select oid into oidoptype from pg_catalog.pg_type where typname = 'uint4';
   select oid  into oidopproc from pg_catalog.pg_proc where proname = 'uint4eq';
   select oid  into oidoperator from pg_catalog.pg_operator where oprcode = oidopproc;
   insert into pg_catalog.pg_amop values (oidopfamily, oidoptype, oidoptype,  1,'s',oidoperator, 405, 0);
END$$;



DO $$
DECLARE
oidopfamily oid;
oidoptype oid;
oidopproc oid;
oidoperator oid;
BEGIN

   select oid into oidopfamily from pg_catalog.pg_opfamily where opfmethod = 403 and opfname = 'uint8_ops';
   select oid into oidoptype from pg_catalog.pg_type where typname = 'uint8';
   select oid  into oidopproc from pg_catalog.pg_proc where proname = 'uint8lt';
   select oid  into oidoperator from pg_catalog.pg_operator where oprcode = oidopproc;
   insert into pg_catalog.pg_amop values (oidopfamily, oidoptype, oidoptype,  1,'s',oidoperator, 403, 0);
   
   
   select oid into oidopfamily from pg_catalog.pg_opfamily where opfmethod = 403 and opfname = 'uint8_ops';
   select oid into oidoptype from pg_catalog.pg_type where typname = 'uint8';
   select oid  into oidopproc from pg_catalog.pg_proc where proname = 'uint8le';
   select oid  into oidoperator from pg_catalog.pg_operator where oprcode = oidopproc;
   insert into pg_catalog.pg_amop values (oidopfamily, oidoptype, oidoptype,  2,'s',oidoperator, 403, 0);
   
    select oid into oidopfamily from pg_catalog.pg_opfamily where opfmethod = 403 and opfname = 'uint8_ops';
   select oid into oidoptype from pg_catalog.pg_type where typname = 'uint8';
   select oid  into oidopproc from pg_catalog.pg_proc where proname = 'uint8eq';
   select oid  into oidoperator from pg_catalog.pg_operator where oprcode = oidopproc;
   insert into pg_catalog.pg_amop values (oidopfamily, oidoptype, oidoptype,  3,'s',oidoperator, 403, 0);
   
    select oid into oidopfamily from pg_catalog.pg_opfamily where opfmethod = 403 and opfname = 'uint8_ops';
   select oid into oidoptype from pg_catalog.pg_type where typname = 'uint8';
   select oid  into oidopproc from pg_catalog.pg_proc where proname = 'uint8gt';
   select oid  into oidoperator from pg_catalog.pg_operator where oprcode = oidopproc;
   insert into pg_catalog.pg_amop values (oidopfamily, oidoptype, oidoptype,  5,'s',oidoperator, 403, 0);
   
   
   select oid into oidopfamily from pg_catalog.pg_opfamily where opfmethod = 403 and opfname = 'uint8_ops';
   select oid into oidoptype from pg_catalog.pg_type where typname = 'uint8';
   select oid  into oidopproc from pg_catalog.pg_proc where proname = 'uint8ge';
   select oid  into oidoperator from pg_catalog.pg_operator where oprcode = oidopproc;
   insert into pg_catalog.pg_amop values (oidopfamily, oidoptype, oidoptype,  4,'s',oidoperator, 403, 0);

   
   select oid into oidopfamily from pg_catalog.pg_opfamily where opfmethod = 405 and opfname = 'uint8_ops';
   select oid into oidoptype from pg_catalog.pg_type where typname = 'uint8';
   select oid  into oidopproc from pg_catalog.pg_proc where proname = 'uint8eq';
   select oid  into oidoperator from pg_catalog.pg_operator where oprcode = oidopproc;
   insert into pg_catalog.pg_amop values (oidopfamily, oidoptype, oidoptype,  1,'s',oidoperator, 405, 0);
END$$;




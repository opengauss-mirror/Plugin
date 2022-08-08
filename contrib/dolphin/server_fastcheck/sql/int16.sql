--
-- INT16
-- Test int16 128-bit integers.
--
CREATE SCHEMA schema_int16;
SET search_path = schema_int16;
SET ENABLE_BETA_FEATURES = OFF;

-- should fail
CREATE TABLE INT16_TBL(q1 int16, q2 int16, q3 serial);

SET ENABLE_BETA_FEATURES = ON;
CREATE TABLE INT16_TBL(q1 int16, q2 int16, q3 serial);


-- do not support create btree index on int16 for now
CREATE INDEX int16idx on INT16_TBL(q1);

INSERT INTO INT16_TBL VALUES('  123   ','  456');
INSERT INTO INT16_TBL VALUES('456   ','12345678901234567890123456789');
INSERT INTO INT16_TBL VALUES('123456789012345678901234567890','123');
INSERT INTO INT16_TBL VALUES(+1234567890123456789012345678901,'12345678901234567890123456789012');
INSERT INTO INT16_TBL VALUES('+123456789012345678901234567890123','-1234567890123456789012345678901234');

-- test boundary
INSERT INTO INT16_TBL VALUES(170141183460469231731687303715884105727, -170141183460469231731687303715884105728);
INSERT INTO INT16_TBL VALUES(170141183460469231731687303715884105728, 0);
INSERT INTO INT16_TBL VALUES(0, -170141183460469231731687303715884105729);

-- bad inputs
INSERT INTO INT16_TBL(q1) VALUES ('      ');
INSERT INTO INT16_TBL(q1) VALUES ('xxx');
INSERT INTO INT16_TBL(q1) VALUES ('4321170141183460469231731687303715884105727');
INSERT INTO INT16_TBL(q1) VALUES ('-143170141183460469231731687303715884105727');
INSERT INTO INT16_TBL(q1) VALUES ('- 123');
INSERT INTO INT16_TBL(q1) VALUES ('  345     5');
INSERT INTO INT16_TBL(q1) VALUES ('');

-- do not support order for now
SELECT * FROM INT16_TBL ORDER BY q1,q2;

-- support compare
SELECT count(*) FROM INT16_TBL WHERE q2 = q2 + 1;
SELECT count(*) FROM INT16_TBL WHERE q2 <> q2 - 1;
SELECT count(*) FROM INT16_TBL WHERE q2 < q2 * 1;
SELECT count(*) FROM INT16_TBL WHERE q2 <= q2 / 1;
SELECT count(*) FROM INT16_TBL WHERE q2 <= q2;
SELECT count(*) FROM INT16_TBL WHERE q2 > q2 + 1;
SELECT count(*) FROM INT16_TBL WHERE q2 >= q2 + 1;

-- support type casts
\dC int16
SELECT CAST(q1 AS int2) FROM INT16_TBL WHERE q3 < 3 ORDER BY q3;
SELECT CAST(q1 AS int4) FROM INT16_TBL WHERE q3 < 3 ORDER BY q3;
SELECT CAST(q1 AS int8) FROM INT16_TBL WHERE q3 < 3 ORDER BY q3;
SELECT CAST(q1 AS numeric) FROM INT16_TBL ORDER BY q3;
SELECT CAST(q1 AS float4) FROM INT16_TBL ORDER BY q3;
SELECT CAST(q1 AS float8) FROM INT16_TBL ORDER BY q3;
SELECT CAST(q1 AS boolean) FROM INT16_TBL ORDER BY q3;


-- some may overflow
SELECT CAST(q1 AS int2) FROM INT16_TBL ORDER BY q3;
SELECT CAST(q1 AS int4) FROM INT16_TBL ORDER BY q3;
SELECT CAST(q1 AS int8) FROM INT16_TBL ORDER BY q3;
SELECT CAST(q1 AS oid) FROM INT16_TBL ORDER BY q3;

-- cast to int16
CREATE TABLE TEST_TBL(
    v1 bigint,
    v2 boolean,
    v3 double precision,
    v4 integer,
    v5 numeric,
    v6 oid,
    v7 real,
    v8 smallint,
    v9 tinyint);

INSERT INTO TEST_TBL VALUES(1, 2, 3, 4, 5, 6, 7, 8, 9);

CREATE TABLE CAST_TBL(
    v1 int16,
    v2 int16,
    v3 int16,
    v4 int16,
    v5 int16,
    v6 int16,
    v7 int16,
    v8 int16,
    v9 int16);

INSERT INTO CAST_TBL SELECT * FROM TEST_TBL;

SELECT * FROM CAST_TBL;

DROP SCHEMA schema_int16 CASCADE;
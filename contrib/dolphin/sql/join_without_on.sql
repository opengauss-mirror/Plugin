drop database if exists join_without_on;
create database join_without_on dbcompatibility 'b';
\c join_without_on

CREATE TABLE J1_TBL (
  i integer,
  j integer,
  t text
);

CREATE TABLE J2_TBL (
  i integer,
  k integer
);

CREATE TABLE J3_TBL (
  i integer,
  k integer
);

CREATE TABLE J4_TBL (
  i integer,
  k integer
);

INSERT INTO J1_TBL VALUES (1, 4, 'one');
INSERT INTO J1_TBL VALUES (2, 3, 'two');;
INSERT INTO J1_TBL VALUES (0, NULL, 'zero');

INSERT INTO J2_TBL VALUES (1, -1);
INSERT INTO J2_TBL VALUES (NULL, 0);

INSERT INTO J3_TBL VALUES (1, -1);
INSERT INTO J3_TBL VALUES (2, 2);

INSERT INTO J4_TBL VALUES (1, -1);
INSERT INTO J4_TBL VALUES (NULL, 0);

--JOIN
SELECT '' AS "xxx", *
  FROM J1_TBL JOIN J2_TBL;

--INNER JOIN
SELECT '' AS "xxx", *
  FROM J1_TBL INNER JOIN J2_TBL;

-- THREE JOIN
SELECT '' AS "xxx", *
  FROM J1_TBL  JOIN J2_TBL JOIN J3_TBL;

--THREE INNER JOIN
SELECT '' AS "xxx", *
  FROM J1_TBL INNER JOIN J2_TBL INNER JOIN J3_TBL;

--THREE JOIN WITH ONE ON
SELECT  * FROM J1_TBL  JOIN (J2_TBL JOIN J3_TBL) ON J1_TBL.i = J2_TBL.i;
SELECT  * FROM J1_TBL  INNER JOIN J2_TBL JOIN J3_TBL ON J2_TBL.i = J3_TBL.i;
SELECT  * FROM J1_TBL  JOIN J2_TBL JOIN J3_TBL ON J1_TBL.i = J3_TBL.i;

--FOUR JOIN WITH ON
SELECT  * FROM J1_TBL  JOIN J2_TBL JOIN J3_TBL JOIN J4_TBL ON J1_TBL.i = J4_TBL.i;
SELECT  * FROM J1_TBL  JOIN J2_TBL INNER JOIN J3_TBL INNER JOIN J4_TBL ON J1_TBL.i = J4_TBL.i;

\c postgres
drop database if exists join_without_on;
drop database if exists mysql;
create database mysql dbcompatibility 'b';
\c mysql
create extension b_sql_plugin;
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
 xxx | i | j |  t   | i | k  
-----+---+---+------+---+----
     | 1 | 4 | one  | 1 | -1
     | 2 | 3 | two  | 1 | -1
     | 0 |   | zero | 1 | -1
     | 1 | 4 | one  |   |  0
     | 2 | 3 | two  |   |  0
     | 0 |   | zero |   |  0
(6 rows)

--INNER JOIN
SELECT '' AS "xxx", *
  FROM J1_TBL INNER JOIN J2_TBL;
 xxx | i | j |  t   | i | k  
-----+---+---+------+---+----
     | 1 | 4 | one  | 1 | -1
     | 2 | 3 | two  | 1 | -1
     | 0 |   | zero | 1 | -1
     | 1 | 4 | one  |   |  0
     | 2 | 3 | two  |   |  0
     | 0 |   | zero |   |  0
(6 rows)

-- THREE JOIN
SELECT '' AS "xxx", *
  FROM J1_TBL  JOIN J2_TBL JOIN J3_TBL;
 xxx | i | j |  t   | i | k  | i | k  
-----+---+---+------+---+----+---+----
     | 1 | 4 | one  | 1 | -1 | 1 | -1
     | 1 | 4 | one  | 1 | -1 | 2 |  2
     | 2 | 3 | two  | 1 | -1 | 1 | -1
     | 2 | 3 | two  | 1 | -1 | 2 |  2
     | 0 |   | zero | 1 | -1 | 1 | -1
     | 0 |   | zero | 1 | -1 | 2 |  2
     | 1 | 4 | one  |   |  0 | 1 | -1
     | 1 | 4 | one  |   |  0 | 2 |  2
     | 2 | 3 | two  |   |  0 | 1 | -1
     | 2 | 3 | two  |   |  0 | 2 |  2
     | 0 |   | zero |   |  0 | 1 | -1
     | 0 |   | zero |   |  0 | 2 |  2
(12 rows)

--THREE INNER JOIN
SELECT '' AS "xxx", *
  FROM J1_TBL INNER JOIN J2_TBL INNER JOIN J3_TBL;
 xxx | i | j |  t   | i | k  | i | k  
-----+---+---+------+---+----+---+----
     | 1 | 4 | one  | 1 | -1 | 1 | -1
     | 1 | 4 | one  | 1 | -1 | 2 |  2
     | 2 | 3 | two  | 1 | -1 | 1 | -1
     | 2 | 3 | two  | 1 | -1 | 2 |  2
     | 0 |   | zero | 1 | -1 | 1 | -1
     | 0 |   | zero | 1 | -1 | 2 |  2
     | 1 | 4 | one  |   |  0 | 1 | -1
     | 1 | 4 | one  |   |  0 | 2 |  2
     | 2 | 3 | two  |   |  0 | 1 | -1
     | 2 | 3 | two  |   |  0 | 2 |  2
     | 0 |   | zero |   |  0 | 1 | -1
     | 0 |   | zero |   |  0 | 2 |  2
(12 rows)

--THREE JOIN WITH ONE ON
SELECT  * FROM J1_TBL  JOIN (J2_TBL JOIN J3_TBL) ON J1_TBL.i = J2_TBL.i;
 i | j |  t  | i | k  | i | k  
---+---+-----+---+----+---+----
 1 | 4 | one | 1 | -1 | 1 | -1
 1 | 4 | one | 1 | -1 | 2 |  2
(2 rows)

SELECT  * FROM J1_TBL  INNER JOIN J2_TBL JOIN J3_TBL ON J2_TBL.i = J3_TBL.i;
 i | j |  t   | i | k  | i | k  
---+---+------+---+----+---+----
 1 | 4 | one  | 1 | -1 | 1 | -1
 2 | 3 | two  | 1 | -1 | 1 | -1
 0 |   | zero | 1 | -1 | 1 | -1
(3 rows)

SELECT  * FROM J1_TBL  JOIN J2_TBL JOIN J3_TBL ON J1_TBL.i = J3_TBL.i;
 i | j |  t  | i | k  | i | k  
---+---+-----+---+----+---+----
 1 | 4 | one | 1 | -1 | 1 | -1
 1 | 4 | one |   |  0 | 1 | -1
 2 | 3 | two | 1 | -1 | 2 |  2
 2 | 3 | two |   |  0 | 2 |  2
(4 rows)

--FOUR JOIN WITH ON
SELECT  * FROM J1_TBL  JOIN J2_TBL JOIN J3_TBL JOIN J4_TBL ON J1_TBL.i = J4_TBL.i;
 i | j |  t  | i | k  | i | k  | i | k  
---+---+-----+---+----+---+----+---+----
 1 | 4 | one |   |  0 | 1 | -1 | 1 | -1
 1 | 4 | one | 1 | -1 | 1 | -1 | 1 | -1
 1 | 4 | one |   |  0 | 2 |  2 | 1 | -1
 1 | 4 | one | 1 | -1 | 2 |  2 | 1 | -1
(4 rows)

SELECT  * FROM J1_TBL  JOIN J2_TBL INNER JOIN J3_TBL INNER JOIN J4_TBL ON J1_TBL.i = J4_TBL.i;
 i | j |  t  | i | k  | i | k  | i | k  
---+---+-----+---+----+---+----+---+----
 1 | 4 | one |   |  0 | 1 | -1 | 1 | -1
 1 | 4 | one | 1 | -1 | 1 | -1 | 1 | -1
 1 | 4 | one |   |  0 | 2 |  2 | 1 | -1
 1 | 4 | one | 1 | -1 | 2 |  2 | 1 | -1
(4 rows)
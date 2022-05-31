DROP TABLE IF EXISTS template_string;
CREATE TABLE template_string(a TEXT, b BYTEA);
INSERT INTO template_string VALUES('abcdefghijklmnopqrstuvwxyz', 'abcdefghijklmnopqrstuvwxyz');

SELECT
SUBSTR(a, 1, 6) AS "normal",
SUBSTR(a, 0, 6) AS "from = 0",
SUBSTR(a, -1, 6) AS "from < 0",
SUBSTR(a, 1, 0) AS "for = 0",
SUBSTR(a, 1, -6) AS "for < 0"
FROM template_string;

SELECT SUBSTRING(a, 1, -6) AS "for < 0" FROM template_string;

SELECT
SUBSTRING(a, 1, 6) AS "normal",
SUBSTRING(a, 0, 6) AS "from = 0",
SUBSTRING(a, -1, 6) AS "from < 0",
SUBSTRING(a, 1, 0) AS "for = 0"
FROM template_string;

SELECT
SUBSTR(b, 1, 6) AS "normal",
SUBSTR(b, 0, 6) AS "from = 0",
SUBSTR(b, -1, 6) AS "from < 0",
SUBSTR(b, 1, 0) AS "for = 0",
SUBSTR(b, 1, -6) AS "for < 0"
FROM template_string;


SELECT SUBSTRING(b, 1, -6) AS "for < 0" FROM template_string;

SELECT
SUBSTRING(b, 1, 6) AS "normal",
SUBSTRING(b, 0, 6) AS "from = 0",
SUBSTRING(b, -1, 6) AS "from < 0",
SUBSTRING(b, 1, 0) AS "for = 0"
FROM template_string;

DROP TABLE IF EXISTS template_string;

drop database if exists test;
create database test dbcompatibility 'B';
\c test

DROP TABLE IF EXISTS template_string;
CREATE TABLE template_string(a TEXT, b BYTEA);
INSERT INTO template_string VALUES('abcdefghijklmnopqrstuvwxyz', 'abcdefghijklmnopqrstuvwxyz');

SELECT
SUBSTR(a, 1, 6) AS "normal",
SUBSTR('你是我的好兄弟', 1, 6) AS "Chinese",
SUBSTR(a, 0, 6) AS "from = 0",
SUBSTR(a, -1, 6) AS "from < 0",
SUBSTR(a, 1, 0) AS "for = 0",
SUBSTR(a, 1, -6) AS "for < 0"
FROM template_string;

SELECT SUBSTRING(a, 1, -6) AS "for < 0" FROM template_string;

SELECT
SUBSTRING(a, 1, 6) AS "normal",
SUBSTRING('你是我的好兄弟', 1, 6) AS "Chinese",
SUBSTRING(a, 0, 6) AS "from = 0",
SUBSTRING(a, -1, 6) AS "from < 0",
SUBSTRING(a, 1, 0) AS "for = 0"
FROM template_string;

SELECT
MID(a, 1, 6) AS "normal",
MID('你是我的好兄弟', 1, 6) AS "Chinese",
MID(a, 0, 6) AS "from = 0",
MID(a, -1, 6) AS "from < 0",
MID(a, 1, 0) AS "for = 0",
MID(a, 1, -6) AS "for < 0"
FROM template_string;

SELECT
SUBSTR(b, 1, 6) AS "normal",
SUBSTR(b, 0, 6) AS "from = 0",
SUBSTR(b, -1, 6) AS "from < 0",
SUBSTR(b, 1, 0) AS "for = 0",
SUBSTR(b, 1, -6) AS "for < 0"
FROM template_string;


SELECT SUBSTRING(b, 1, -6) AS "for < 0" FROM template_string;

SELECT
SUBSTRING(b, 1, 6) AS "normal",
SUBSTRING(b, 0, 6) AS "from = 0",
SUBSTRING(b, -1, 6) AS "from < 0",
SUBSTRING(b, 1, 0) AS "for = 0"
FROM template_string;

SELECT
MID(b, 1, 6) AS "normal",
MID(b, 0, 6) AS "from = 0",
MID(b, -1, 6) AS "from < 0",
MID(b, 1, 0) AS "for = 0",
MID(b, 1, -6) AS "for < 0"
FROM template_string;

SELECT SUBSTR(a FROM 1 FOR 6) FROM template_string;
SELECT SUBSTR(a FOR 6 FROM 1) FROM template_string;

SELECT SUBSTR(a FROM 1) FROM template_string;
SELECT SUBSTR(a FROM 0) FROM template_string;
SELECT SUBSTR(a FROM -1) FROM template_string;
SELECT SUBSTR(a FOR 1) FROM template_string;
SELECT SUBSTR(a FOR 0) FROM template_string;
SELECT SUBSTR(a FOR -1) FROM template_string;

SELECT MID(a FROM 1 FOR 6) FROM template_string;
SELECT MID(a FOR 6 FROM 1) FROM template_string;

SELECT MID(a FROM 1) FROM template_string;
SELECT MID(a FROM 0) FROM template_string;
SELECT MID(a FROM -1) FROM template_string;
SELECT MID(a FOR 1) FROM template_string;
SELECT MID(a FOR 0) FROM template_string;
SELECT MID(a FOR -1) FROM template_string;

\c postgres
drop database test;

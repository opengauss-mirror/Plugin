drop database if exists db_b_substr_test;
create database db_b_substr_test dbcompatibility 'A';
\c db_b_substr_test

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

\c contrib_regression
drop database db_b_substr_test;

create schema db_b_substr_test;
set current_schema to 'db_b_substr_test';

set bytea_output to escape;

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

SELECT SUBSTR(b FROM 1) FROM template_string;
SELECT SUBSTR(b FROM 0) FROM template_string;
SELECT SUBSTR(b FROM -1) FROM template_string;
SELECT SUBSTR(b FOR 1) FROM template_string;
SELECT SUBSTR(b FOR 0) FROM template_string;
SELECT SUBSTR(b FOR -1) FROM template_string;

SELECT SUBSTRING(a FROM 1) FROM template_string;
SELECT SUBSTRING(a FROM 0) FROM template_string;
SELECT SUBSTRING(a FROM -1) FROM template_string;
SELECT SUBSTRING(a FOR 1) FROM template_string;
SELECT SUBSTRING(a FOR 0) FROM template_string;
SELECT SUBSTRING(a FOR -1) FROM template_string;

SELECT SUBSTRING(b FROM 1) FROM template_string;
SELECT SUBSTRING(b FROM 0) FROM template_string;
SELECT SUBSTRING(b FROM -1) FROM template_string;
SELECT SUBSTRING(b FOR 1) FROM template_string;
SELECT SUBSTRING(b FOR 0) FROM template_string;
SELECT SUBSTRING(b FOR -1) FROM template_string;

SELECT MID(a FROM 1 FOR 6) FROM template_string;
SELECT MID(a FOR 6 FROM 1) FROM template_string;

SELECT MID(a FROM 1) FROM template_string;
SELECT MID(a FROM 0) FROM template_string;
SELECT MID(a FROM -1) FROM template_string;
SELECT MID(a FOR 1) FROM template_string;
SELECT MID(a FOR 0) FROM template_string;
SELECT MID(a FOR -1) FROM template_string;

SELECT MID(b FROM 1) FROM template_string;
SELECT MID(b FROM 0) FROM template_string;
SELECT MID(b FROM -1) FROM template_string;
SELECT MID(b FOR 1) FROM template_string;
SELECT MID(b FOR 0) FROM template_string;
SELECT MID(b FOR -1) FROM template_string;

-- The input is in hexadecimal format. Two hexadecimal digits are one byte. The substring is obtained by byte.
select substring(E'\\xDEADBEEF'::bytea,2);
select substring(E'\\xDEADBEEF'::bytea,2,2);
select substring(E'\\xDEADBEEF'::bytea,2,8);
select substring(E'\\xDEADBEEF'::bytea,2,10);
select substring(E'\\xDEADBEEF'::bytea,2,0);
select substring(E'\\xDEADBEEF'::bytea,2,-1);

select substr(E'\\xDEADBEEF'::bytea,2);
select substr(E'\\xDEADBEEF'::bytea,2,2);
select substr(E'\\xDEADBEEF'::bytea,2,8);
select substr(E'\\xDEADBEEF'::bytea,2,10);
select substr(E'\\xDEADBEEF'::bytea,2,0);
select substr(E'\\xDEADBEEF'::bytea,2,-1);

select mid(E'\\xDEADBEEF'::bytea,2);
select mid(E'\\xDEADBEEF'::bytea,2,2);
select mid(E'\\xDEADBEEF'::bytea,2,8);
select mid(E'\\xDEADBEEF'::bytea,2,10);
select mid(E'\\xDEADBEEF'::bytea,2,0);
select mid(E'\\xDEADBEEF'::bytea,2,-1);

-- test column table
create table test_column (c1 text, c2 int) with (orientation = column);
create table test_row (c1 text, c2 int);

insert into test_column values('abcdefg', 2), ('abcdefg', 0), ('abcdefg', -2);
insert into test_row(c1, c2) select c1, c2 from test_column;

select c1, c2, substring(c1 from c2) from test_column order by c1;
select c1, c2, substring(c1 from c2) from test_row order by c1;

select c1, c2, substring(c1 for c2) from test_column order by c1;
select c1, c2, substring(c1 for c2) from test_row order by c1;


select c1, c2, substr(c1 from c2) from test_column order by c1;
select c1, c2, substr(c1 from c2) from test_row order by c1;

select c1, c2, substr(c1 for c2) from test_column order by c1;
select c1, c2, substr(c1 for c2) from test_row order by c1;

drop schema db_b_substr_test cascade;
reset current_schema;

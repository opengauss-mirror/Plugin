SELECT TRIM('  bar   ');
SELECT TRIM(LEADING 'x' FROM 'xxxbarxxx');
SELECT TRIM(BOTH 'x' FROM 'xxxbarxxx');
SELECT TRIM(TRAILING 'x' FROM 'xxxbarxxx');
SELECT TRIM(LEADING 'xyz' FROM 'xyzxbarxxyz');
SELECT TRIM(BOTH 'xyz' FROM 'xyzxbarxxyz');
SELECT TRIM(TRAILING 'xyz' FROM 'xyzxbarxxyz');

drop database if exists test;
create database test dbcompatibility 'B';
\c test
create extension dolphin;

SELECT TRIM('  bar   ');
SELECT TRIM(LEADING 'x' FROM 'xxxbarxxx');
SELECT TRIM(BOTH 'x' FROM 'xxxbarxxx');
SELECT TRIM(TRAILING 'x' FROM 'xxxbarxxx');
SELECT TRIM(LEADING 'xyz' FROM 'xyzxbarxxyz');
SELECT TRIM(BOTH 'xyz' FROM 'xyzxbarxxyz');
SELECT TRIM(TRAILING 'xyz' FROM 'xyzxbarxxyz');

\c postgres
drop database test;
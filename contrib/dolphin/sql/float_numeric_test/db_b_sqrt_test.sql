SELECT SQRT(64);
SELECT SQRT(-64);

drop database if exists test;
create database test dbcompatibility 'B';
\c test

SELECT SQRT(64);
SELECT SQRT(-64);

SELECT SQRT(64::numeric);
SELECT SQRT(-64::numeric);

\c postgres
drop database test;

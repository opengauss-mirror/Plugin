SELECT LOG(10);
drop database if exists test;
create database test dbcompatibility 'B';
\c test
create extension dolphin;

SELECT LOG(10);
SELECT LOG10(100);
SELECT LOG2(64);

SELECT LOG(10::numeric);
SELECT LOG10(100::numeric);
SELECT LOG2(64::numeric);

SELECT LOG(0) IS NULL;
SELECT LOG10(0) IS NULL;
SELECT LOG2(0) IS NULL;

SELECT LOG(-1) IS NULL;
SELECT LOG10(-1) IS NULL;
SELECT LOG2(-1) IS NULL;

SELECT LOG(0::numeric) IS NULL;
SELECT LOG10(0::numeric) IS NULL;
SELECT LOG2(0::numeric) IS NULL;

SELECT LOG(-1::numeric) IS NULL;
SELECT LOG10(-1::numeric) IS NULL;
SELECT LOG2(-1::numeric) IS NULL;

SELECT LOG(2, 64) IS NULL;
SELECT LOG(-2, 64) IS NULL;
SELECT LOG(2, -64) IS NULL;
SELECT LOG(0, 64) IS NULL;
SELECT LOG(2, 0) IS NULL;

\c postgres
drop database test;

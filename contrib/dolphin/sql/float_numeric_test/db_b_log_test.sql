drop database if exists db_b_log_test;
create database db_b_log_test dbcompatibility 'A';
\c db_b_log_test

SELECT LOG(10);

\c contrib_regression
drop database db_b_log_test;

create schema db_b_log_test;
set current_schema to 'db_b_log_test';

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

select log(111);
select log(b'111');
select log(7);
select log2(111);
select log2(b'111');
select log2(7);
select log10(111);
select log10(b'111');
select log10(7);

select log(b'111'::int);
select log2(b'111'::int);
select log10(b'111'::int);

drop schema db_b_log_test cascade;
reset current_schema;

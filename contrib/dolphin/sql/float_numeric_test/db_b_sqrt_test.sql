set dolphin.b_compatibility_mode to off;
drop database if exists db_b_sqrt_test;
create database db_b_sqrt_test dbcompatibility 'A';
\c db_b_sqrt_test

SELECT SQRT(64);
SELECT SQRT(-64);

\c contrib_regression
set dolphin.b_compatibility_mode to off;
drop database db_b_sqrt_test;
set dolphin.b_compatibility_mode to on;

create schema db_b_sqrt_test;
set current_schema to 'db_b_sqrt_test';

SELECT SQRT(64);
SELECT SQRT(-64);

SELECT SQRT(64::numeric);
SELECT SQRT(-64::numeric);

select sqrt(111);
select sqrt(b'111');
select sqrt(7);
select sqrt(b'111'::int);

drop schema db_b_sqrt_test cascade;
reset current_schema;

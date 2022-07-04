SELECT SQRT(64);
SELECT SQRT(-64);

drop database if exists db_b_sqrt_test;
create database db_b_sqrt_test dbcompatibility 'B';
\c db_b_sqrt_test

SELECT SQRT(64);
SELECT SQRT(-64);

SELECT SQRT(64::numeric);
SELECT SQRT(-64::numeric);

select sqrt(111);
select sqrt(b'111');
select sqrt(7);
select sqrt(b'111'::int);

\c postgres
drop database db_b_sqrt_test;

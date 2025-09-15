create database test_b dbcompatibility 'B';
\c test_b
SELECT datcompatibility FROM pg_database WHERE datname = current_database();
-- expect error
create extension spq;
\c regression
drop database test_b;

create database test_c dbcompatibility 'C';
\c test_c
SELECT datcompatibility FROM pg_database WHERE datname = current_database();
-- expect error
create extension spq;
\c regression
drop database test_c;

create database test_d dbcompatibility 'D';
\c test_d
SELECT datcompatibility FROM pg_database WHERE datname = current_database();
-- expect error
create extension spq;
\c regression
drop database test_d;

create database test_pg dbcompatibility 'PG';
\c test_pg
SELECT datcompatibility FROM pg_database WHERE datname = current_database();
-- expect error
create extension spq;
\c regression
drop database test_pg;
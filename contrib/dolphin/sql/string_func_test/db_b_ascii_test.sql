set dolphin.b_compatibility_mode to off;
drop database if exists db_b_ascii_test;
create database db_b_ascii_test dbcompatibility 'A';
\c db_b_ascii_test

SELECT ASCII('a');
SELECT ASCII('你');

\c contrib_regression
set dolphin.b_compatibility_mode to off;
drop database db_b_ascii_test;
set dolphin.b_compatibility_mode to on;

create schema db_b_ascii_test;
set current_schema to 'db_b_ascii_test';

SELECT ASCII('a');
SELECT ASCII('你');

drop schema db_b_ascii_test cascade;
reset current_schema;

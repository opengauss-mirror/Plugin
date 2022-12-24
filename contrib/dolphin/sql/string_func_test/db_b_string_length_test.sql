drop database if exists db_b_string_length_test;
create database db_b_string_length_test dbcompatibility 'A';
\c db_b_string_length_test

SELECT length('jose');
SELECT length('你好呀');
SELECT LENGTH(B'101');
SELECT length('你好呀jose');

\c contrib_regression
drop database db_b_string_length_test;

create schema db_b_string_length_test;
set current_schema to 'db_b_string_length_test';

SELECT length('jose');
SELECT length('你好呀');
SELECT LENGTH(B'101');
SELECT length('你好呀jose');

drop schema db_b_string_length_test cascade;
reset current_schema;
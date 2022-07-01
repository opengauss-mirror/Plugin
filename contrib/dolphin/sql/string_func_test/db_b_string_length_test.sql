SELECT length('jose');
SELECT length('你好呀');
SELECT LENGTH(B'101');
SELECT length('你好呀jose');

drop database if exists db_b_string_length_test;
create database db_b_string_length_test dbcompatibility 'B';
\c db_b_string_length_test

SELECT length('jose');
SELECT length('你好呀');
SELECT LENGTH(B'101');
SELECT length('你好呀jose');

\c postgres
drop database db_b_string_length_test;
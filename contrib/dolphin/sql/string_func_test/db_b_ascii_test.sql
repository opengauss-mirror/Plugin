SELECT ASCII('a');
SELECT ASCII('你');
drop database if exists db_b_ascii_test;
create database db_b_ascii_test dbcompatibility 'B';
\c db_b_ascii_test

SELECT ASCII('a');
SELECT ASCII('你');

\c postgres
drop database db_b_ascii_test;

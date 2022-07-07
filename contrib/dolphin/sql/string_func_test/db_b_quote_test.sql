drop database if exists db_b_quote_test;
create database db_b_quote_test dbcompatibility 'B';
\c db_b_quote_test

SELECT QUOTE(E'Don\'t!');
SELECT QUOTE(E'O\'hello');
SELECT QUOTE('O\hello');

\c postgres
drop database db_b_quote_test;
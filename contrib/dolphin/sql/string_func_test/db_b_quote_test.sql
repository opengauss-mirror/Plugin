drop database if exists test;
create database test dbcompatibility 'B';
\c test
create extension dolphin;

SELECT QUOTE(E'Don\'t!');
SELECT QUOTE(E'O\'hello');
SELECT QUOTE('O\hello');

\c postgres
drop database test;
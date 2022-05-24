SELECT ASCII('a');
SELECT ASCII('你');
drop database if exists test;
create database test dbcompatibility 'B';
\c test
create extension dolphin;

SELECT ASCII('a');
SELECT ASCII('你');

\c postgres
drop database test;

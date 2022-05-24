SELECT length('jose');
SELECT length('你好呀');
SELECT LENGTH(B'101');
SELECT length('你好呀jose');

drop database if exists test;
create database test dbcompatibility 'B';
\c test
create extension dolphin;

SELECT length('jose');
SELECT length('你好呀');
SELECT LENGTH(B'101');
SELECT length('你好呀jose');

\c postgres
drop database test;
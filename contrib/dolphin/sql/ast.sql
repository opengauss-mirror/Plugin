drop database if exists ast_test;
create database ast_test dbcompatibility 'b';
\c ast_test
create extension dolphin;
ast select * from test;
ast create table test(id int);
ast create table test(id int(5));
ast USE "custcomcenter";
ast select 1;select 1;
ast select 1;ast select 1;
\c postgres
drop database ast_test;
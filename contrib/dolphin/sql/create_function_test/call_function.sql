drop database if exists db_func_call1;
create database db_func_call1 dbcompatibility 'B';
\c db_func_call1

CREATE FUNCTION f1 (s CHAR(20)) RETURNS int  CONTAINS SQL AS $$ select 1 $$ ;

call f1('a');
drop function f1; 

CREATE FUNCTION f1 () RETURNS int  no SQL AS $$ select 1 $$ ;

call f1;

call f1; 

drop function f1;

\c postgres
drop database if exists db_func_call1;


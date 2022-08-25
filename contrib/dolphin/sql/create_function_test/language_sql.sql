drop database if exists db_func_2;
create database db_func_2 dbcompatibility 'B';
\c db_func_2

CREATE FUNCTION f1 (s CHAR(20)) RETURNS int  AS $$ select 1 $$ ;


CREATE FUNCTION f2 (s int) RETURNS int AS $$ select s; $$ ;


CREATE FUNCTION f3 (s int) RETURNS int  LANGUAGE SQL AS $$ select s; $$ ;


CREATE FUNCTION f4 (s int) RETURNS int  LANGUAGE SQL AS $$ select s; $$ ;

call f1(1);

call f2(2);

call f3(3);

call f4(4);


\c postgres
drop database if exists db_func_2;

create schema db_func_4;
set current_schema to 'db_func_4';

CREATE FUNCTION f1 (s CHAR(20)) RETURNS int SQL SECURITY DEFINER AS $$ select 1 $$ ;


CREATE FUNCTION f2 (s int) RETURNS int NO SQL SQL SECURITY DEFINER AS $$ select s; $$ ;


CREATE FUNCTION f3 (s int) RETURNS int  SQL SECURITY INVOKER  READS SQL DATA LANGUAGE SQL AS $$ select s; $$ ;


CREATE FUNCTION f4 (s int) RETURNS int SQL SECURITY INVOKER CONTAINS SQL  LANGUAGE SQL AS $$ select s; $$ ;

call f1(1);

call f2(2);

call f3(3);

call f4(4);


drop schema db_func_4 cascade;
reset current_schema;

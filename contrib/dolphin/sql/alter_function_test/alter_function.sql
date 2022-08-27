drop database if exists db_alter_func_1;
create database db_alter_func_1 dbcompatibility 'B';
\c db_alter_func_1

CREATE FUNCTION f1 (s CHAR(20)) RETURNS int  AS $$ select 1 $$ ;

ALTER FUNCTION f1 (s char(20)) NO SQL;

ALTER FUNCTION f1 (s char(20)) CONTAINS SQL;

ALTER FUNCTION f1 (s char(20)) NO SQL CONTAINS SQL ;

ALTER FUNCTION f1 (s char(20)) CONTAINS SQL LANGUAGE SQL ;

ALTER FUNCTION f1 (s char(20)) CONTAINS SQL MODIFIES SQL DATA;

ALTER FUNCTION f1 (s char(20)) READS SQL DATA;

ALTER FUNCTION f1 (s char(20)) SQL SECURITY INVOKER;

ALTER FUNCTION f1 (s char(20)) SQL SECURITY DEFINER;

-- error 
ALTER FUNCTION f1 (s char(20)) DETERMINISTIC;

ALTER FUNCTION f1 (s char(20)) NOT DETERMINISTIC;

ALTER FUNCTION f1 READS SQL DATA;

\c postgres
drop database db_alter_func_1;
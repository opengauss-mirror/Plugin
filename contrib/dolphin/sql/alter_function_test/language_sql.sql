drop database if exists db_alter_func_sql;
create database db_alter_func_sql dbcompatibility 'B';
\c db_alter_func_sql


-- test func
CREATE FUNCTION f1 (s CHAR(20)) RETURNS int  AS $$ 
begin
return 100;
end;
 $$ language plpgsql;

ALTER FUNCTION f1 (s char(20)) language sql;

ALTER FUNCTION f1 (s char(20)) language plpgsql;

ALTER FUNCTION f1 (s char(20)) language gis ;


CREATE OR REPLACE FUNCTION func(s int)
RETURNS int
LANGUAGE SQL
NO SQL
SQL SECURITY DEFINER
AS $$ select s; $$ ;

ALTER FUNCTION func(s int) LANGUAGE PLPGSQL ;
ALTER FUNCTION func(s int) LANGUAGE PYTHON;
ALTER FUNCTION func(s int) LANGUAGE JAVA;
ALTER FUNCTION func(s int) LANGUAGE test;
ALTER FUNCTION func(s int) LANGUAGE sql;

select func(2);

select f1('a');

-- test proc
CREATE  PROCEDURE proc1(a int )
AS
BEGIN
RAISE NOTICE '%','aaa';
END;
/

alter procedure proc1(a int) language sql;

alter procedure proc1(a int) language sqls;

alter procedure proc1(a int) language java;

call proc1(1);

\c postgres
drop database db_alter_func_sql;

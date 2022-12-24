create schema db_alter_func_sql;
set current_schema to 'db_alter_func_sql';


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

--retest for language sql problem 
create table language_1093039(id int,age int,name varchar2(16));
insert into language_1093039 values(1,22,'小王');
insert into language_1093039 values(2,23,'小张');
insert into language_1093039 values(3,21,'小弓');

--创建函数
create function fun_1(id1 int,age1 int,name1 varchar2)return int
LANGUAGE SQL
as
a1 int;
begin
insert into language_1093039 values(id1,age1,name1);
update language_1093039 set age=23 where id=3;
delete from language_1093039 where id=2;
select count(*) into a1 from language_1093039;
return a1;
end;
/

create function fun_2(id1 int,age1 int,name1 varchar2)return int
as
a1 int;
begin
insert into language_1093039 values(id1,age1,name1);
update language_1093039 set age=23 where id=3;
delete from language_1093039 where id=2;
select count(*) into a1 from language_1093039;
return a1;
end;
/


create function fun_3(id1 int,age1 int,name1 varchar2)return int
LANGUAGE PLPGSQL
as
a1 int;
begin
insert into language_1093039 values(id1,age1,name1);
update language_1093039 set age=23 where id=3;
delete from language_1093039 where id=2;
select count(*) into a1 from language_1093039;
return a1;
end;
/

create function fun_err(id1 int,age1 int,name1 varchar2)return int
LANGUAGE JAVA
as
a1 int;
begin
insert into language_1093039 values(id1,age1,name1);
update language_1093039 set age=23 where id=3;
delete from language_1093039 where id=2;
select count(*) into a1 from language_1093039;
return a1;
end;
/

--创建存储过程
create procedure pro_1(id1 int,age1 int,name1 varchar2)
as
a1 int;
begin
insert into language_1093039 values(id1,age1,name1);
update language_1093039 set name='小张' where id=3;
delete from language_1093039 where id=1;
select count(*) into a1 from language_1093039;
end;
/


create procedure pro_2(id1 int,age1 int,name1 varchar2)
LANGUAGE SQL
as 
a1 int;
begin
insert into language_1093039 values(id1,age1,name1);
update language_1093039 set name='小张' where id=3;
delete from language_1093039 where id=1;
select count(*) into a1 from language_1093039;
end;
/

create procedure pro_3(id1 int,age1 int,name1 varchar2)
LANGUAGE PLPGSQL
as
a1 int;
begin
insert into language_1093039 values(id1,age1,name1);
update language_1093039 set name='小张' where id=3;
delete from language_1093039 where id=1;
select count(*) into a1 from language_1093039;
end;
/


create procedure pro_err(id1 int,age1 int,name1 varchar2)
LANGUAGE JAVA
as
a1 int;
begin
insert into language_1093039 values(id1,age1,name1);
update language_1093039 set name='小张' where id=3;
delete from language_1093039 where id=1;
select count(*) into a1 from language_1093039;
end;
/


call fun_1(1,2,'a');

call fun_2(1,2,'a');

call fun_3(1,2,'a');

call pro_1(1,2,'a');

call pro_2(1,2,'a');

call pro_3(1,2,'a');


drop schema db_alter_func_sql cascade;
reset current_schema;

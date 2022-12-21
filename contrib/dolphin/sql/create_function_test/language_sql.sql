create schema db_func_2;
set current_schema to 'db_func_2';

CREATE FUNCTION f1 (s CHAR(20)) RETURNS int  AS $$ select 1 $$ ;


CREATE FUNCTION f2 (s int) RETURNS int AS $$ select s; $$ ;


CREATE FUNCTION f3 (s int) RETURNS int  LANGUAGE SQL AS $$ select s; $$ ;


CREATE FUNCTION f4 (s int) RETURNS int  LANGUAGE SQL AS $$ select s; $$ ;

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

call f1(1);

call f2(2);

call f3(3);

call f4(4);



drop schema db_func_2 cascade;
reset current_schema;

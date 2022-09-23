create database test_quoted dbcompatibility 'B';
\c test_quoted;
create function func_show0001(integer, integer)  returns integer comment 'co自定义函数0001'
    as 'select $1 + $2;'
    language sql
    immutable
    returns null on null input;
show function status where "Name" = 'func_show0001';
show function status where "Name" like 'func_show0001';

create table test("ID" int);
insert into test values(10);
\d+ test
select * from test where "ID"=10;
select * from test where "ID" like 10;
select * from test where id=10;
drop table test;

create table test(ID "int4");
insert into test values(10);
\d+ test
select * from test where "ID"=10;
select * from test where "ID" like 10;
select * from test where id=10;
drop table test;

set lower_case_table_names to 0;

create table Test(ID "int4");
insert into Test values(10);
select * from Test where "ID"=10;
select * from Test where "ID" like 10;
select * from Test where id=10;
drop table Test;
\c postgres
drop database test_quoted;
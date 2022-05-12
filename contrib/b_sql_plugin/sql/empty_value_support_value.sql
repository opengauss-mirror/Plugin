drop database if exists b;
create database b dbcompatibility 'b';

\c b
create extension dolphin;
create table test1(num int not null);
insert into test1 value();
insert into test1 value(),();
set sql_mode = '';
insert into test1 value();
select * from test1;
insert into test1 value(),();
select * from test1;

\c postgres
drop database if exists b;
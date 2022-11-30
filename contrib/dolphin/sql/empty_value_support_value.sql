drop database if exists empty_value_support_value;
create database empty_value_support_value dbcompatibility 'b';

\c empty_value_support_value
create table test1(num int not null);
insert into test1 value();
insert into test1 value(),();
set dolphin.sql_mode = '';
insert into test1 value();
select * from test1;
insert into test1 value(),();
select * from test1;

\c postgres
drop database if exists empty_value_support_value;
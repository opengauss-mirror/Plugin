create schema empty_value_support_value;
set current_schema to 'empty_value_support_value';
create table test1(num int not null);
insert into test1 value();
insert into test1 value(),();
set dolphin.sql_mode = '';
insert into test1 value();
select * from test1;
insert into test1 value(),();
select * from test1;

drop schema empty_value_support_value cascade;
reset current_schema;
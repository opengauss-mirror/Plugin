create schema ustore_test;
set current_schema to 'ustore_test';

set enable_default_ustore_table = true;


create table t_year_0005(ts1 year primary key, ts2 year(2)check(ts2 > 1998), ts3 year(4) default 2012);
create table t_year_0005_01(ts1 year primary key, ts2 year(2)check(ts2 > 1998), ts3 year(4) default 2012);

-- clear
drop table if exists t_year_0005 cascade;
drop table if exists t_year_0005_01 cascade;
reset enable_default_ustore_table;
drop schema ustore_test cascade;
reset current_schema;
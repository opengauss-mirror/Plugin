create schema ustore_test;
set current_schema to 'ustore_test';

set enable_default_ustore_table = true;


create table t_year_0005(ts1 year primary key, ts2 year(2)check(ts2 > 1998), ts3 year(4) default 2012);
create table t_year_0005_01(ts1 year primary key, ts2 year(2)check(ts2 > 1998), ts3 year(4) default 2012);

-- clear
drop table if exists t_year_0005 cascade;
drop table if exists t_year_0005_01 cascade;
reset enable_default_ustore_table;

-- test for excluding storage
drop table if exists t_table_0018;
create table t_table_0018(
col01 int comment '首列',
col02 int not null,
col03 date default '2000-01-01 00:00:01',
col04 varchar(255) unique,
col05 varchar(10) check(length(col05)>=5),
col06 int
) with (storage_type = ustore);
create index i_table_0018_01 on t_table_0018(col05);
-- expect error
create table t_table_0018_18 like t_table_0018 excluding storage;
drop table if exists t_table_0018;

drop schema ustore_test cascade;
reset current_schema;

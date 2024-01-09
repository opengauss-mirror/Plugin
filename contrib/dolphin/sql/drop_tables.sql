create schema test_drop_tables;
set current_schema='test_drop_tables';

create table t1(a int);
create table t2(a int);
create table t3(a int);
drop tables t1,t2;
drop tables t3;

create table test_if_exists(a int);
drop tables if exists test_if_exists,not_exist_table1,not_exist_table2;
drop table if exists test_if_exists;

create table test_direct_drop(a int);
drop tables test_direct_drop,not_exist_table1,not_exist_table2;
drop table if exists test_direct_drop;

create table test_restrict(a int);
drop table test_restrict restrict;

create table test_cascade(a int);
drop table test_cascade cascade;

DROP SCHEMA test_drop_tables CASCADE;
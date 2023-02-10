create schema uncommon_table_option;
set current_schema = uncommon_table_option;

drop table if exists test;
create table test(
col1 int,
col2 varchar(255))
autoextend_size = 4M;
alter table test autoextend_size = 0.5M;
alter table test autoextend_size = abc;
alter table test autoextend_size = 1;
alter table test autoextend_size = 0.5;
drop table test;

create table test(
col1 int,
col2 varchar(255))
avg_row_length = 32;
alter table test avg_row_length = 3.2;
drop table test;

create table test(
col1 int,
col2 varchar(255))
checksum = 0;
alter table test checksum = 0.5;
alter table test checksum = x'12';
drop table test;

create table test(
col1 int,
col2 varchar(255))
connection = 'connection';
alter table test connection = 'connection';
drop table test;

create table test(
col1 int,
col2 varchar(255))
data directory = 'test';
alter table test index directory = 'test';
drop table test;

create table test(
col1 int,
col2 varchar(255))
delay_key_write = 0;
alter table test delay_key_write = 1;
alter table test delay_key_write = x'aa';
drop table test;

create table test(
col1 int,
col2 varchar(255))
encryption = 'N';
alter table test encryption = 'Y';
drop table test;

create table test(
col1 int,
col2 varchar(255))
engine_attribute = 'engine_attribute';
alter table test engine_attribute = 'engine_attribute';
drop table test;

create table test(
col1 int,
col2 varchar(255))
insert_method = no;
alter table test insert_method = first;
alter table test insert_method = last; 
drop table test;

create table test(
col1 int,
col2 varchar(255))
key_block_size = 1;
alter table test key_block_size = 1.52;
drop table test;

create table test(
col1 int,
col2 varchar(255))
max_rows = 1;
alter table test max_rows = 1.342;
drop table test;

create table test(
col1 int,
col2 varchar(255))
min_rows = 2;
alter table test min_rows = 1.531;
drop table test;

create table test(
col1 int,
col2 varchar(255))
pack_keys = default;
alter table test pack_keys = 1;
alter table test pack_keys = 0;
alter table test pack_keys = x'1';
drop table test;

create table test(
col1 int,
col2 varchar(255))
password = 'password';
alter table test password = 'password';
drop table test;

create table test(
col1 int,
col2 varchar(255))
start transaction;
alter table test start transaction;
drop table test;

create table test(
col1 int,
col2 varchar(255))
secondary_engine_attribute = 'secondary_engine_attribute';
alter table test secondary_engine_attribute = 'secondary_engine_attribute';
drop table test;

create table test(
col1 int,
col2 varchar(255))
stats_auto_recalc = default;
alter table test stats_auto_recalc = 1;
alter table test stats_auto_recalc = 0;
alter table test stats_auto_recalc = x'1';
drop table test;

create table test(
col1 int,
col2 varchar(255))
stats_persistent = default;
alter table test stats_persistent = 1;
alter table test stats_persistent = 0;
alter table test stats_persistent = x'1';
drop table test;

create table test(
col1 int,
col2 varchar(255))
stats_sample_pages = 1;
alter table test stats_sample_pages = 1.23;
alter table test stats_sample_pages = x'1';
drop table test;

create table test(
col1 int,
col2 varchar(255))
union(tb1, tb2);
alter table test union(tb1, tb2, tb3);
drop table test;

create tablespace test add datafile 'data.ibd';
create table test(
col1 int,
col2 varchar(255))
tablespace test storage disk; 
alter table test tablespace test storage memory;
drop table test;
drop tablespace test;

create temporary table if not exists test(
col1 int,
col2 varchar(64),
col3 int)
autoextend_size = 8M
autoextend_size = 4M;
alter table test autoextend_size = 8M, autoextend_size = 4M;
drop table test;

create temporary table if not exists test(
col1 int,
col2 varchar(64),
col3 int)
avg_row_length = 11
avg_row_length = 10;
alter table test avg_row_length = 11, avg_row_length = 10;
drop table test;

create temporary table if not exists test(
col1 int,
col2 varchar(64),
col3 int)
checksum = 1
checksum = 2;
alter table test checksum = 1, checksum = 2; 
drop table test;

create temporary table if not exists test(
col1 int,
col2 varchar(64),
col3 int)
connection = 'test'
connection = 'connection';
alter table test connection = 'test', connection = 'connection';
drop table test;

create temporary table if not exists test(
col1 int,
col2 varchar(64),
col3 int)
delay_key_write = 0
delay_key_write = 1;
alter table test delay_key_write = 0, delay_key_write = 1;
drop table test;

create temporary table if not exists test(
col1 int,
col2 varchar(64),
col3 int)
encryption = 'N'
encryption = 'Y';
alter table test encryption = 'N', encryption = 'Y'; 
drop table test;

create temporary table if not exists test(
col1 int,
col2 varchar(64),
col3 int)
insert_method = no
insert_method = last;
alter table test insert_method = first, insert_method = last; 
drop table test;

create temporary table if not exists test(
col1 int,
col2 varchar(64),
col3 int)
engine = myisam
key_block_size = 10
key_block_size = 20;
alter table test key_block_size = 10, key_block_size = 20; 
drop table test;

create temporary table if not exists test(
col1 int,
col2 varchar(64),
col3 int)
max_rows = 10
max_rows = 20;
alter table test max_rows = 10, max_rows = 20;
drop table test;

create temporary table if not exists test(
col1 int,
col2 varchar(64),
col3 int)
min_rows = 10
min_rows = 5;
alter table test min_rows = 1, min_rows = 0;
drop table test;

create temporary table if not exists test(
col1 int,
col2 varchar(64),
col3 int)
pack_keys = default
pack_keys = 0;
alter table test pack_keys = 1, pack_keys = 0; 
drop table test;

create temporary table if not exists test(
col1 int,
col2 varchar(64),
col3 int)
password = 'password'
password = 'test'; 
alter table test password = 'password', password = 'test';
drop table test;

create temporary table if not exists test(
col1 int,
col2 varchar(64),
col3 int)
stats_auto_recalc = default
stats_auto_recalc = 0;
alter table test stats_auto_recalc = 1, stats_auto_recalc = 0; 
drop table test;

create temporary table if not exists test(
col1 int,
col2 varchar(64),
col3 int)
stats_persistent = default
stats_persistent = 0;
alter table test stats_persistent = 1, stats_persistent = 0; 
drop table test;

create temporary table if not exists test(
col1 int,
col2 varchar(64),
col3 int)
stats_sample_pages = 2
stats_sample_pages = 1;
alter table test stats_sample_pages = 2, stats_sample_pages = 1;  
drop table test;

create temporary table if not exists test(
col1 int,
col2 varchar(64),
col3 int)
union(tb1, tb2, tb3)
union(tb1, tb3);
alter table test union(tb1, tb2, tb3), union(tb1, tb3);
drop table test;

create tablespace test add datafile 'data.ibd';
create temporary table if not exists test(
col1 int,
col2 varchar(64),
col3 int)
tablespace test storage disk
tablespace test storage memory;
alter table test tablespace test storage disk, tablespace test storage memory;
drop table test;
drop tablespace test;

create table test(
col1 int)
autoextend_size = 4M
avg_row_length = 10
checksum = 1;
alter table test autoextend_size = 8M, avg_row_length = 20, checksum = 0;
drop table test;

create table test(
col1 int)
connection = 'connection'
data directory = 'data_directory'
delay_key_write = 1;
alter table test connection = 'connection_test', data directory = 'data_directory_test', delay_key_write = 0;
drop table test;

drop schema uncommon_table_option;

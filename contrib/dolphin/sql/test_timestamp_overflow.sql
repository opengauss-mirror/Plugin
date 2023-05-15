drop schema if exists test_timestamp_overflow;
create schema test_timestamp_overflow;
set current_schema = test_timestamp_overflow;

set time zone 'PRC';
create table test(c timestamp);
create table test1(c datetime);
--test overflow
show dolphin.sql_mode;
insert into test values('294277-1-9 4:00:54.775807');
insert into test values('294277-1-9 4:00:54.5');
insert into test values('294277-1-9 4:00:54.4');
insert into test1 values('294277-1-9 4:00:54.775807');
insert into test1 values('294277-1-9 4:00:54.5');
insert into test1 values('294277-1-9 4:00:54.3');
insert into test1 values('294277-1-9 4:00:54.2');
insert into test1 values('294277-1-9 4:00:00');
set dolphin.sql_mode = 'sql_mode_full_group,pipes_as_concat,ansi_quotes,no_zero_date,pad_char_to_full_length';
show dolphin.sql_mode;
insert into test values('294277-1-9 4:00:54.775807');
insert into test values('294277-1-9 4:00:54.5');
insert into test values('294277-1-9 4:00:54.4');
insert into test1 values('294277-1-9 4:00:54.775807');
insert into test1 values('294277-1-9 4:00:54.5');
insert into test1 values('294277-1-9 4:00:54.3');
insert into test1 values('294277-1-9 4:00:54.2');
--test zero date
set dolphin.sql_mode = 'sql_mode_full_group,pipes_as_concat,ansi_quotes,pad_char_to_full_length';
show dolphin.sql_mode;
insert into test values('0000-00-00 01:00:00');
insert into test values('0000-00-00 11:00:00');
insert into test values('0000-00-00 00:00:00');
insert into test1 values('0000-00-00 01:00:00');
insert into test1 values('0000-00-00 11:00:00');
insert into test1 values('0000-00-00 00:00:00');
select c from test order by c;
select c from test1 order by c;
drop table test;
drop table test1;

reset current_schema;
drop schema test_timestamp_overflow cascade;

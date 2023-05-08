drop schema if exists test_timestamp_overflow;
create schema test_timestamp_overflow;
set current_schema = test_timestamp_overflow;

set time zone 'PRC';
create table test(c timestamp);
--test overflow
show dolphin.sql_mode;
insert into test values('294277-1-9 4:00:54.775807');
insert into test values('294277-1-9 4:00:54.5');
insert into test values('294277-1-9 4:00:54.4');
set dolphin.sql_mode = 'sql_mode_full_group,pipes_as_concat,ansi_quotes,no_zero_date,pad_char_to_full_length';
show dolphin.sql_mode;
insert into test values('294277-1-9 4:00:54.775807');
insert into test values('294277-1-9 4:00:54.5');
insert into test values('294277-1-9 4:00:54.4');
--test zero date
set dolphin.sql_mode = 'sql_mode_full_group,pipes_as_concat,ansi_quotes,pad_char_to_full_length';
show dolphin.sql_mode;
insert into test values('0000-00-00 01:00:00');
insert into test values('0000-00-00 11:00:00');
insert into test values('0000-00-00 00:00:00');
select c from test order by c;
drop table test;

reset current_schema;
drop schema test_timestamp_overflow cascade;

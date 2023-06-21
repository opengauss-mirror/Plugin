drop schema if exists test_dayofweek;
create schema test_dayofweek;
set current_schema = test_dayofweek;

set time zone 'PRC';
show dolphin.sql_mode;
select current_timestamp;
select dayofweek('14:14:14');
select dayofweek(time'10:10:10');
select dayofweek('2022-01-01 00:00:00'::time);
select dayofweek('2022-06-28 00:00:00'::time) from dual;
select dayofweek(time '2022-06-28 00:00:00');
select dayofweek(time'24:00:00');
select dayofweek(time'48:00:00');
select dayofweek(time'838:59:59');
create table t_timestamp(c timestamp);
create table t_datetime(c datetime);
insert into t_timestamp values('14:14:14');
insert ignore into t_timestamp values('14:14:14');
insert into t_datetime values('14:14:14');
insert ignore into t_datetime values('14:14:14');
select * from t_timestamp;
select * from t_datetime;

set dolphin.sql_mode = 'sql_mode_full_group,pipes_as_concat,ansi_quotes,no_zero_date,pad_char_to_full_length';
show dolphin.sql_mode;
select current_timestamp;
select dayofweek('14:14:14');
select dayofweek(time'10:10:10');
select dayofweek('2022-01-01 00:00:00'::time);
select dayofweek('2022-06-28 00:00:00'::time) from dual;
select dayofweek(time '2022-06-28 00:00:00');
select dayofweek(time'24:00:00');
select dayofweek(time'48:00:00');
select dayofweek(time'838:59:59');
insert into t_timestamp values('14:14:14');
insert ignore into t_timestamp values('14:14:14');
insert into t_datetime values('14:14:14');
insert ignore into t_datetime values('14:14:14');
select * from t_timestamp;
select * from t_datetime;

reset current_schema;
drop schema if exists test_dayofweek cascade;
drop schema if exists test_timestampn;
create schema test_timestampn;
set current_schema = test_timestampn;
set time zone 'PRC';
create table t_timestamp(c timestamp);
create table t_timestamp1(c timestamp(1));
create table t_timestamp2(c timestamp(2));
create table t_timestamp3(c timestamp(3));
create table t_timestamp4(c timestamp(4));
create table t_timestamp5(c timestamp(5));
create table t_timestamp6(c timestamp(6));

insert into t_timestamp values('294277-1-9 4:00:54');
insert into t_timestamp values('294277-1-9 4:00:54+08');
insert into t_timestamp1 values('294277-1-9 4:00:54');
insert into t_timestamp1 values('294277-1-9 4:00:54+08');
insert into t_timestamp1 values('294277-1-9 4:00:54.4+08');
insert into t_timestamp2 values('294277-1-9 4:00:54');
insert into t_timestamp2 values('294277-1-9 4:00:54+08');
insert into t_timestamp2 values('294277-1-9 4:00:54.08+08');
insert into t_timestamp3 values('294277-1-9 4:00:54');
insert into t_timestamp3 values('294277-1-9 4:00:54+08');
insert into t_timestamp3 values('294277-1-9 4:00:54.08+08');
insert into t_timestamp3 values('294277-1-9 4:00:54.008+08');
insert into t_timestamp4 values('294277-1-9 4:00:54');
insert into t_timestamp4 values('294277-1-9 4:00:54+08');
insert into t_timestamp4 values('294277-1-9 4:00:54.08+08');
insert into t_timestamp4 values('294277-1-9 4:00:54.0008+08');
insert into t_timestamp5 values('294277-1-9 4:00:54');
insert into t_timestamp5 values('294277-1-9 4:00:54+08');
insert into t_timestamp5 values('294277-1-9 4:00:54.08+08');
insert into t_timestamp5 values('294277-1-9 4:00:54.00008+08');
insert into t_timestamp6 values('294277-1-9 4:00:54');
insert into t_timestamp6 values('294277-1-9 4:00:54+08');
insert into t_timestamp6 values('294277-1-9 4:00:54.08+08');
insert into t_timestamp6 values('294277-1-9 4:00:54.000008+08');

select * from t_timestamp;
select * from t_timestamp1;
select * from t_timestamp2;
select * from t_timestamp3;
select * from t_timestamp4;
select * from t_timestamp5;
select * from t_timestamp6;

drop table t_timestamp;
drop table t_timestamp1;
drop table t_timestamp2;
drop table t_timestamp3;
drop table t_timestamp4;
drop table t_timestamp5;
drop table t_timestamp6;
reset current_schema;
drop schema test_timestampn cascade;

-- test yearweek with timestamps
drop table if exists person_track_info;
CREATE TABLE person_track_info (
    "ID" "uint8"(20) NOT NULL,
    "SOURCE_ID" "uint8"(20),
    "OCCURRENCE_TIME" timestamp(6) without time zone NOT NULL,
    "OCCURRENCE_TIMEtz" timestamp(6) with time zone NOT NULL
)
PARTITION BY RANGE ("yearweek"(("OCCURRENCE_TIME")))
(
    PARTITION "part202549" VALUES LESS THAN ('202550'),
    PARTITION "part202550" VALUES LESS THAN ('202551'),
    PARTITION "part202551" VALUES LESS THAN ('202552'),
    PARTITION "part202552" VALUES LESS THAN ('202601'),
    PARTITION "part202601" VALUES LESS THAN ('202602'),
    PARTITION "part202602" VALUES LESS THAN ('202603'),
    PARTITION "part202650" VALUES LESS THAN ('202650')
)
ENABLE ROW MOVEMENT;

select pg_get_tabledef('person_track_info');

insert into person_track_info values (1, 1, '2026-06-01', '2026-06-01');
select yearweek(OCCURRENCE_TIME), yearweek(OCCURRENCE_TIMEtz) from person_track_info;

drop table if exists person_track_info;

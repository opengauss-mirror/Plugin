drop schema if exists compare_operator_plan_schema cascade;
create schema compare_operator_plan_schema;
set current_schema to 'compare_operator_plan_schema';
set dolphin.b_compatibility_mode to on;

drop table if exists t13,t14,t15,t16,t17,t18,t19,t20,t21,t22;

create table t12(a bit(1) primary key);
create table t13(a bit(64) primary key);
create table t14(a boolean primary key);
create table t15(a date primary key);
create table t16(a time primary key);
create table t17(a time(4) primary key);
create table t18(a datetime primary key);
create table t19(a datetime(4) default '2022-11-11 11:11:11' primary key);
create table t20(a timestamp primary key);
create table t21(a timestamp(4) default '2022-11-11 11:11:11' primary key);
create table t22(a year primary key);

insert into t12 values(b'1'), (b'0');
insert into t13 values(b'111'), (b'10101100'), (b'1111001100');
insert into t14 values(true), (false);
insert into t15 values('2023-02-05'), ('1999-09-09'), ('2008-06-24');
insert into t16 values('19:10:50'), ('12:12:12'), ('08:59:59');
insert into t17 values('19:10:50.3456'), ('12:12:12.189'), ('08:59:59.666');
insert into t18 values('2023-02-05 19:10:50'), ('1999-09-09 12:12:12'), ('2008-06-24 08:59:59');
insert into t19 values('2023-02-05 19:10:50.456'), ('1999-09-09 12:12:12.189'), ('2008-06-24 08:59:59.666');
insert into t20 values('2023-02-05 19:10:50'), ('1999-09-09 12:12:12'), ('2008-06-24 08:59:59');
insert into t21 values('2023-02-05 19:10:50.456'), ('1999-09-09 12:12:12.189'), ('2008-06-24 08:59:59.666');
insert into t22 values('2023'), ('1999'), ('2008'), ('2024'), ('2077'), ('1984');

set enable_seqscan to off;

-- a = b
explain (costs off) select * from t12, t13 where t12.a = t13.a;
explain (costs off) select * from t12, t14 where t12.a = t14.a;
explain (costs off) select * from t12, t15 where t12.a = t15.a;
explain (costs off) select * from t12, t16 where t12.a = t16.a;
explain (costs off) select * from t12, t17 where t12.a = t17.a;
explain (costs off) select * from t12, t18 where t12.a = t18.a;
explain (costs off) select * from t12, t19 where t12.a = t19.a;
explain (costs off) select * from t12, t20 where t12.a = t20.a;
explain (costs off) select * from t12, t21 where t12.a = t21.a;

explain (costs off) select * from t13, t12 where t13.a = t12.a;
explain (costs off) select * from t13, t14 where t13.a = t14.a;
explain (costs off) select * from t13, t15 where t13.a = t15.a;
explain (costs off) select * from t13, t16 where t13.a = t16.a;
explain (costs off) select * from t13, t17 where t13.a = t17.a;
explain (costs off) select * from t13, t18 where t13.a = t18.a;
explain (costs off) select * from t13, t19 where t13.a = t19.a;
explain (costs off) select * from t13, t20 where t13.a = t20.a;
explain (costs off) select * from t13, t21 where t13.a = t21.a;

explain (costs off) select * from t14, t12 where t14.a = t12.a;
explain (costs off) select * from t14, t13 where t14.a = t13.a;
explain (costs off) select * from t14, t15 where t14.a = t15.a;
explain (costs off) select * from t14, t16 where t14.a = t16.a;
explain (costs off) select * from t14, t17 where t14.a = t17.a;
explain (costs off) select * from t14, t18 where t14.a = t18.a;
explain (costs off) select * from t14, t19 where t14.a = t19.a;
explain (costs off) select * from t14, t20 where t14.a = t20.a;
explain (costs off) select * from t14, t21 where t14.a = t21.a;

explain (costs off) select * from t15, t12 where t15.a = t12.a;
explain (costs off) select * from t15, t13 where t15.a = t13.a;
explain (costs off) select * from t15, t14 where t15.a = t14.a;
explain (costs off) select * from t15, t16 where t15.a = t16.a;
explain (costs off) select * from t15, t17 where t15.a = t17.a;
explain (costs off) select * from t15, t18 where t15.a = t18.a;
explain (costs off) select * from t15, t19 where t15.a = t19.a;
explain (costs off) select * from t15, t20 where t15.a = t20.a;
explain (costs off) select * from t15, t21 where t15.a = t21.a;

explain (costs off) select * from t16, t12 where t16.a = t12.a;
explain (costs off) select * from t16, t13 where t16.a = t13.a;
explain (costs off) select * from t16, t14 where t16.a = t14.a;
explain (costs off) select * from t16, t15 where t16.a = t15.a;
explain (costs off) select * from t16, t17 where t16.a = t17.a;
explain (costs off) select * from t16, t18 where t16.a = t18.a;
explain (costs off) select * from t16, t19 where t16.a = t19.a;
explain (costs off) select * from t16, t20 where t16.a = t20.a;
explain (costs off) select * from t16, t21 where t16.a = t21.a;

explain (costs off) select * from t17, t12 where t17.a = t12.a;
explain (costs off) select * from t17, t13 where t17.a = t13.a;
explain (costs off) select * from t17, t14 where t17.a = t14.a;
explain (costs off) select * from t17, t15 where t17.a = t15.a;
explain (costs off) select * from t17, t16 where t17.a = t16.a;
explain (costs off) select * from t17, t18 where t17.a = t18.a;
explain (costs off) select * from t17, t19 where t17.a = t19.a;
explain (costs off) select * from t17, t20 where t17.a = t20.a;
explain (costs off) select * from t17, t21 where t17.a = t21.a;

explain (costs off) select * from t18, t12 where t18.a = t12.a;
explain (costs off) select * from t18, t13 where t18.a = t13.a;
explain (costs off) select * from t18, t14 where t18.a = t14.a;
explain (costs off) select * from t18, t15 where t18.a = t15.a;
explain (costs off) select * from t18, t16 where t18.a = t16.a;
explain (costs off) select * from t18, t17 where t18.a = t17.a;
explain (costs off) select * from t18, t19 where t18.a = t19.a;
explain (costs off) select * from t18, t20 where t18.a = t20.a;
explain (costs off) select * from t18, t21 where t18.a = t21.a;

explain (costs off) select * from t19, t12 where t19.a = t12.a;
explain (costs off) select * from t19, t13 where t19.a = t13.a;
explain (costs off) select * from t19, t14 where t19.a = t14.a;
explain (costs off) select * from t19, t15 where t19.a = t15.a;
explain (costs off) select * from t19, t16 where t19.a = t16.a;
explain (costs off) select * from t19, t17 where t19.a = t17.a;
explain (costs off) select * from t19, t18 where t19.a = t18.a;
explain (costs off) select * from t19, t20 where t19.a = t20.a;
explain (costs off) select * from t19, t21 where t19.a = t21.a;

explain (costs off) select * from t20, t12 where t20.a = t12.a;
explain (costs off) select * from t20, t13 where t20.a = t13.a;
explain (costs off) select * from t20, t14 where t20.a = t14.a;
explain (costs off) select * from t20, t15 where t20.a = t15.a;
explain (costs off) select * from t20, t16 where t20.a = t16.a;
explain (costs off) select * from t20, t17 where t20.a = t17.a;
explain (costs off) select * from t20, t18 where t20.a = t18.a;
explain (costs off) select * from t20, t19 where t20.a = t19.a;
explain (costs off) select * from t20, t21 where t20.a = t21.a;

explain (costs off) select * from t21, t12 where t21.a = t12.a;
explain (costs off) select * from t21, t13 where t21.a = t13.a;
explain (costs off) select * from t21, t14 where t21.a = t14.a;
explain (costs off) select * from t21, t15 where t21.a = t15.a;
explain (costs off) select * from t21, t16 where t21.a = t16.a;
explain (costs off) select * from t21, t17 where t21.a = t17.a;
explain (costs off) select * from t21, t18 where t21.a = t18.a;
explain (costs off) select * from t21, t19 where t21.a = t19.a;
explain (costs off) select * from t21, t20 where t21.a = t20.a;

-- a > b
explain (costs off) select * from t12, t13 where t12.a > t13.a;
explain (costs off) select * from t12, t14 where t12.a > t14.a;
explain (costs off) select * from t12, t15 where t12.a > t15.a;
explain (costs off) select * from t12, t16 where t12.a > t16.a;
explain (costs off) select * from t12, t17 where t12.a > t17.a;
explain (costs off) select * from t12, t18 where t12.a > t18.a;
explain (costs off) select * from t12, t19 where t12.a > t19.a;
explain (costs off) select * from t12, t20 where t12.a > t20.a;
explain (costs off) select * from t12, t21 where t12.a > t21.a;

explain (costs off) select * from t13, t12 where t13.a > t12.a;
explain (costs off) select * from t13, t14 where t13.a > t14.a;
explain (costs off) select * from t13, t15 where t13.a > t15.a;
explain (costs off) select * from t13, t16 where t13.a > t16.a;
explain (costs off) select * from t13, t17 where t13.a > t17.a;
explain (costs off) select * from t13, t18 where t13.a > t18.a;
explain (costs off) select * from t13, t19 where t13.a > t19.a;
explain (costs off) select * from t13, t20 where t13.a > t20.a;
explain (costs off) select * from t13, t21 where t13.a > t21.a;

explain (costs off) select * from t14, t12 where t14.a > t12.a;
explain (costs off) select * from t14, t13 where t14.a > t13.a;
explain (costs off) select * from t14, t15 where t14.a > t15.a;
explain (costs off) select * from t14, t16 where t14.a > t16.a;
explain (costs off) select * from t14, t17 where t14.a > t17.a;
explain (costs off) select * from t14, t18 where t14.a > t18.a;
explain (costs off) select * from t14, t19 where t14.a > t19.a;
explain (costs off) select * from t14, t20 where t14.a > t20.a;
explain (costs off) select * from t14, t21 where t14.a > t21.a;

explain (costs off) select * from t15, t12 where t15.a > t12.a;
explain (costs off) select * from t15, t13 where t15.a > t13.a;
explain (costs off) select * from t15, t14 where t15.a > t14.a;
explain (costs off) select * from t15, t16 where t15.a > t16.a;
explain (costs off) select * from t15, t17 where t15.a > t17.a;
explain (costs off) select * from t15, t18 where t15.a > t18.a;
explain (costs off) select * from t15, t19 where t15.a > t19.a;
explain (costs off) select * from t15, t20 where t15.a > t20.a;
explain (costs off) select * from t15, t21 where t15.a > t21.a;

explain (costs off) select * from t16, t12 where t16.a > t12.a;
explain (costs off) select * from t16, t13 where t16.a > t13.a;
explain (costs off) select * from t16, t14 where t16.a > t14.a;
explain (costs off) select * from t16, t15 where t16.a > t15.a;
explain (costs off) select * from t16, t17 where t16.a > t17.a;
explain (costs off) select * from t16, t18 where t16.a > t18.a;
explain (costs off) select * from t16, t19 where t16.a > t19.a;
explain (costs off) select * from t16, t20 where t16.a > t20.a;
explain (costs off) select * from t16, t21 where t16.a > t21.a;

explain (costs off) select * from t17, t12 where t17.a > t12.a;
explain (costs off) select * from t17, t13 where t17.a > t13.a;
explain (costs off) select * from t17, t14 where t17.a > t14.a;
explain (costs off) select * from t17, t15 where t17.a > t15.a;
explain (costs off) select * from t17, t16 where t17.a > t16.a;
explain (costs off) select * from t17, t18 where t17.a > t18.a;
explain (costs off) select * from t17, t19 where t17.a > t19.a;
explain (costs off) select * from t17, t20 where t17.a > t20.a;
explain (costs off) select * from t17, t21 where t17.a > t21.a;

explain (costs off) select * from t18, t12 where t18.a > t12.a;
explain (costs off) select * from t18, t13 where t18.a > t13.a;
explain (costs off) select * from t18, t14 where t18.a > t14.a;
explain (costs off) select * from t18, t15 where t18.a > t15.a;
explain (costs off) select * from t18, t16 where t18.a > t16.a;
explain (costs off) select * from t18, t17 where t18.a > t17.a;
explain (costs off) select * from t18, t19 where t18.a > t19.a;
explain (costs off) select * from t18, t20 where t18.a > t20.a;
explain (costs off) select * from t18, t21 where t18.a > t21.a;

explain (costs off) select * from t19, t12 where t19.a > t12.a;
explain (costs off) select * from t19, t13 where t19.a > t13.a;
explain (costs off) select * from t19, t14 where t19.a > t14.a;
explain (costs off) select * from t19, t15 where t19.a > t15.a;
explain (costs off) select * from t19, t16 where t19.a > t16.a;
explain (costs off) select * from t19, t17 where t19.a > t17.a;
explain (costs off) select * from t19, t18 where t19.a > t18.a;
explain (costs off) select * from t19, t20 where t19.a > t20.a;
explain (costs off) select * from t19, t21 where t19.a > t21.a;

explain (costs off) select * from t20, t12 where t20.a > t12.a;
explain (costs off) select * from t20, t13 where t20.a > t13.a;
explain (costs off) select * from t20, t14 where t20.a > t14.a;
explain (costs off) select * from t20, t15 where t20.a > t15.a;
explain (costs off) select * from t20, t16 where t20.a > t16.a;
explain (costs off) select * from t20, t17 where t20.a > t17.a;
explain (costs off) select * from t20, t18 where t20.a > t18.a;
explain (costs off) select * from t20, t19 where t20.a > t19.a;
explain (costs off) select * from t20, t21 where t20.a > t21.a;

explain (costs off) select * from t21, t12 where t21.a > t12.a;
explain (costs off) select * from t21, t13 where t21.a > t13.a;
explain (costs off) select * from t21, t14 where t21.a > t14.a;
explain (costs off) select * from t21, t15 where t21.a > t15.a;
explain (costs off) select * from t21, t16 where t21.a > t16.a;
explain (costs off) select * from t21, t17 where t21.a > t17.a;
explain (costs off) select * from t21, t18 where t21.a > t18.a;
explain (costs off) select * from t21, t19 where t21.a > t19.a;
explain (costs off) select * from t21, t20 where t21.a > t20.a;

-- a >= b
explain (costs off) select * from t12, t13 where t12.a >= t13.a;
explain (costs off) select * from t12, t14 where t12.a >= t14.a;
explain (costs off) select * from t12, t15 where t12.a >= t15.a;
explain (costs off) select * from t12, t16 where t12.a >= t16.a;
explain (costs off) select * from t12, t17 where t12.a >= t17.a;
explain (costs off) select * from t12, t18 where t12.a >= t18.a;
explain (costs off) select * from t12, t19 where t12.a >= t19.a;
explain (costs off) select * from t12, t20 where t12.a >= t20.a;
explain (costs off) select * from t12, t21 where t12.a >= t21.a;

explain (costs off) select * from t13, t12 where t13.a >= t12.a;
explain (costs off) select * from t13, t14 where t13.a >= t14.a;
explain (costs off) select * from t13, t15 where t13.a >= t15.a;
explain (costs off) select * from t13, t16 where t13.a >= t16.a;
explain (costs off) select * from t13, t17 where t13.a >= t17.a;
explain (costs off) select * from t13, t18 where t13.a >= t18.a;
explain (costs off) select * from t13, t19 where t13.a >= t19.a;
explain (costs off) select * from t13, t20 where t13.a >= t20.a;
explain (costs off) select * from t13, t21 where t13.a >= t21.a;

explain (costs off) select * from t14, t12 where t14.a >= t12.a;
explain (costs off) select * from t14, t13 where t14.a >= t13.a;
explain (costs off) select * from t14, t15 where t14.a >= t15.a;
explain (costs off) select * from t14, t16 where t14.a >= t16.a;
explain (costs off) select * from t14, t17 where t14.a >= t17.a;
explain (costs off) select * from t14, t18 where t14.a >= t18.a;
explain (costs off) select * from t14, t19 where t14.a >= t19.a;
explain (costs off) select * from t14, t20 where t14.a >= t20.a;
explain (costs off) select * from t14, t21 where t14.a >= t21.a;

explain (costs off) select * from t15, t12 where t15.a >= t12.a;
explain (costs off) select * from t15, t13 where t15.a >= t13.a;
explain (costs off) select * from t15, t14 where t15.a >= t14.a;
explain (costs off) select * from t15, t16 where t15.a >= t16.a;
explain (costs off) select * from t15, t17 where t15.a >= t17.a;
explain (costs off) select * from t15, t18 where t15.a >= t18.a;
explain (costs off) select * from t15, t19 where t15.a >= t19.a;
explain (costs off) select * from t15, t20 where t15.a >= t20.a;
explain (costs off) select * from t15, t21 where t15.a >= t21.a;

explain (costs off) select * from t16, t12 where t16.a >= t12.a;
explain (costs off) select * from t16, t13 where t16.a >= t13.a;
explain (costs off) select * from t16, t14 where t16.a >= t14.a;
explain (costs off) select * from t16, t15 where t16.a >= t15.a;
explain (costs off) select * from t16, t17 where t16.a >= t17.a;
explain (costs off) select * from t16, t18 where t16.a >= t18.a;
explain (costs off) select * from t16, t19 where t16.a >= t19.a;
explain (costs off) select * from t16, t20 where t16.a >= t20.a;
explain (costs off) select * from t16, t21 where t16.a >= t21.a;

explain (costs off) select * from t17, t12 where t17.a >= t12.a;
explain (costs off) select * from t17, t13 where t17.a >= t13.a;
explain (costs off) select * from t17, t14 where t17.a >= t14.a;
explain (costs off) select * from t17, t15 where t17.a >= t15.a;
explain (costs off) select * from t17, t16 where t17.a >= t16.a;
explain (costs off) select * from t17, t18 where t17.a >= t18.a;
explain (costs off) select * from t17, t19 where t17.a >= t19.a;
explain (costs off) select * from t17, t20 where t17.a >= t20.a;
explain (costs off) select * from t17, t21 where t17.a >= t21.a;

explain (costs off) select * from t18, t12 where t18.a >= t12.a;
explain (costs off) select * from t18, t13 where t18.a >= t13.a;
explain (costs off) select * from t18, t14 where t18.a >= t14.a;
explain (costs off) select * from t18, t15 where t18.a >= t15.a;
explain (costs off) select * from t18, t16 where t18.a >= t16.a;
explain (costs off) select * from t18, t17 where t18.a >= t17.a;
explain (costs off) select * from t18, t19 where t18.a >= t19.a;
explain (costs off) select * from t18, t20 where t18.a >= t20.a;
explain (costs off) select * from t18, t21 where t18.a >= t21.a;

explain (costs off) select * from t19, t12 where t19.a >= t12.a;
explain (costs off) select * from t19, t13 where t19.a >= t13.a;
explain (costs off) select * from t19, t14 where t19.a >= t14.a;
explain (costs off) select * from t19, t15 where t19.a >= t15.a;
explain (costs off) select * from t19, t16 where t19.a >= t16.a;
explain (costs off) select * from t19, t17 where t19.a >= t17.a;
explain (costs off) select * from t19, t18 where t19.a >= t18.a;
explain (costs off) select * from t19, t20 where t19.a >= t20.a;
explain (costs off) select * from t19, t21 where t19.a >= t21.a;

explain (costs off) select * from t20, t12 where t20.a >= t12.a;
explain (costs off) select * from t20, t13 where t20.a >= t13.a;
explain (costs off) select * from t20, t14 where t20.a >= t14.a;
explain (costs off) select * from t20, t15 where t20.a >= t15.a;
explain (costs off) select * from t20, t16 where t20.a >= t16.a;
explain (costs off) select * from t20, t17 where t20.a >= t17.a;
explain (costs off) select * from t20, t18 where t20.a >= t18.a;
explain (costs off) select * from t20, t19 where t20.a >= t19.a;
explain (costs off) select * from t20, t21 where t20.a >= t21.a;

explain (costs off) select * from t21, t12 where t21.a >= t12.a;
explain (costs off) select * from t21, t13 where t21.a >= t13.a;
explain (costs off) select * from t21, t14 where t21.a >= t14.a;
explain (costs off) select * from t21, t15 where t21.a >= t15.a;
explain (costs off) select * from t21, t16 where t21.a >= t16.a;
explain (costs off) select * from t21, t17 where t21.a >= t17.a;
explain (costs off) select * from t21, t18 where t21.a >= t18.a;
explain (costs off) select * from t21, t19 where t21.a >= t19.a;
explain (costs off) select * from t21, t20 where t21.a >= t20.a;

-- a < b
explain (costs off) select * from t12, t13 where t12.a < t13.a;
explain (costs off) select * from t12, t14 where t12.a < t14.a;
explain (costs off) select * from t12, t15 where t12.a < t15.a;
explain (costs off) select * from t12, t16 where t12.a < t16.a;
explain (costs off) select * from t12, t17 where t12.a < t17.a;
explain (costs off) select * from t12, t18 where t12.a < t18.a;
explain (costs off) select * from t12, t19 where t12.a < t19.a;
explain (costs off) select * from t12, t20 where t12.a < t20.a;
explain (costs off) select * from t12, t21 where t12.a < t21.a;

explain (costs off) select * from t13, t12 where t13.a < t12.a;
explain (costs off) select * from t13, t14 where t13.a < t14.a;
explain (costs off) select * from t13, t15 where t13.a < t15.a;
explain (costs off) select * from t13, t16 where t13.a < t16.a;
explain (costs off) select * from t13, t17 where t13.a < t17.a;
explain (costs off) select * from t13, t18 where t13.a < t18.a;
explain (costs off) select * from t13, t19 where t13.a < t19.a;
explain (costs off) select * from t13, t20 where t13.a < t20.a;
explain (costs off) select * from t13, t21 where t13.a < t21.a;

explain (costs off) select * from t14, t12 where t14.a < t12.a;
explain (costs off) select * from t14, t13 where t14.a < t13.a;
explain (costs off) select * from t14, t15 where t14.a < t15.a;
explain (costs off) select * from t14, t16 where t14.a < t16.a;
explain (costs off) select * from t14, t17 where t14.a < t17.a;
explain (costs off) select * from t14, t18 where t14.a < t18.a;
explain (costs off) select * from t14, t19 where t14.a < t19.a;
explain (costs off) select * from t14, t20 where t14.a < t20.a;
explain (costs off) select * from t14, t21 where t14.a < t21.a;

explain (costs off) select * from t15, t12 where t15.a < t12.a;
explain (costs off) select * from t15, t13 where t15.a < t13.a;
explain (costs off) select * from t15, t14 where t15.a < t14.a;
explain (costs off) select * from t15, t16 where t15.a < t16.a;
explain (costs off) select * from t15, t17 where t15.a < t17.a;
explain (costs off) select * from t15, t18 where t15.a < t18.a;
explain (costs off) select * from t15, t19 where t15.a < t19.a;
explain (costs off) select * from t15, t20 where t15.a < t20.a;
explain (costs off) select * from t15, t21 where t15.a < t21.a;

explain (costs off) select * from t16, t12 where t16.a < t12.a;
explain (costs off) select * from t16, t13 where t16.a < t13.a;
explain (costs off) select * from t16, t14 where t16.a < t14.a;
explain (costs off) select * from t16, t15 where t16.a < t15.a;
explain (costs off) select * from t16, t17 where t16.a < t17.a;
explain (costs off) select * from t16, t18 where t16.a < t18.a;
explain (costs off) select * from t16, t19 where t16.a < t19.a;
explain (costs off) select * from t16, t20 where t16.a < t20.a;
explain (costs off) select * from t16, t21 where t16.a < t21.a;

explain (costs off) select * from t17, t12 where t17.a < t12.a;
explain (costs off) select * from t17, t13 where t17.a < t13.a;
explain (costs off) select * from t17, t14 where t17.a < t14.a;
explain (costs off) select * from t17, t15 where t17.a < t15.a;
explain (costs off) select * from t17, t16 where t17.a < t16.a;
explain (costs off) select * from t17, t18 where t17.a < t18.a;
explain (costs off) select * from t17, t19 where t17.a < t19.a;
explain (costs off) select * from t17, t20 where t17.a < t20.a;
explain (costs off) select * from t17, t21 where t17.a < t21.a;

explain (costs off) select * from t18, t12 where t18.a < t12.a;
explain (costs off) select * from t18, t13 where t18.a < t13.a;
explain (costs off) select * from t18, t14 where t18.a < t14.a;
explain (costs off) select * from t18, t15 where t18.a < t15.a;
explain (costs off) select * from t18, t16 where t18.a < t16.a;
explain (costs off) select * from t18, t17 where t18.a < t17.a;
explain (costs off) select * from t18, t19 where t18.a < t19.a;
explain (costs off) select * from t18, t20 where t18.a < t20.a;
explain (costs off) select * from t18, t21 where t18.a < t21.a;

explain (costs off) select * from t19, t12 where t19.a < t12.a;
explain (costs off) select * from t19, t13 where t19.a < t13.a;
explain (costs off) select * from t19, t14 where t19.a < t14.a;
explain (costs off) select * from t19, t15 where t19.a < t15.a;
explain (costs off) select * from t19, t16 where t19.a < t16.a;
explain (costs off) select * from t19, t17 where t19.a < t17.a;
explain (costs off) select * from t19, t18 where t19.a < t18.a;
explain (costs off) select * from t19, t20 where t19.a < t20.a;
explain (costs off) select * from t19, t21 where t19.a < t21.a;

explain (costs off) select * from t20, t12 where t20.a < t12.a;
explain (costs off) select * from t20, t13 where t20.a < t13.a;
explain (costs off) select * from t20, t14 where t20.a < t14.a;
explain (costs off) select * from t20, t15 where t20.a < t15.a;
explain (costs off) select * from t20, t16 where t20.a < t16.a;
explain (costs off) select * from t20, t17 where t20.a < t17.a;
explain (costs off) select * from t20, t18 where t20.a < t18.a;
explain (costs off) select * from t20, t19 where t20.a < t19.a;
explain (costs off) select * from t20, t21 where t20.a < t21.a;

explain (costs off) select * from t21, t12 where t21.a < t12.a;
explain (costs off) select * from t21, t13 where t21.a < t13.a;
explain (costs off) select * from t21, t14 where t21.a < t14.a;
explain (costs off) select * from t21, t15 where t21.a < t15.a;
explain (costs off) select * from t21, t16 where t21.a < t16.a;
explain (costs off) select * from t21, t17 where t21.a < t17.a;
explain (costs off) select * from t21, t18 where t21.a < t18.a;
explain (costs off) select * from t21, t19 where t21.a < t19.a;
explain (costs off) select * from t21, t20 where t21.a < t20.a;

-- a <= b
explain (costs off) select * from t12, t13 where t12.a <= t13.a;
explain (costs off) select * from t12, t14 where t12.a <= t14.a;
explain (costs off) select * from t12, t15 where t12.a <= t15.a;
explain (costs off) select * from t12, t16 where t12.a <= t16.a;
explain (costs off) select * from t12, t17 where t12.a <= t17.a;
explain (costs off) select * from t12, t18 where t12.a <= t18.a;
explain (costs off) select * from t12, t19 where t12.a <= t19.a;
explain (costs off) select * from t12, t20 where t12.a <= t20.a;
explain (costs off) select * from t12, t21 where t12.a <= t21.a;

explain (costs off) select * from t13, t12 where t13.a <= t12.a;
explain (costs off) select * from t13, t14 where t13.a <= t14.a;
explain (costs off) select * from t13, t15 where t13.a <= t15.a;
explain (costs off) select * from t13, t16 where t13.a <= t16.a;
explain (costs off) select * from t13, t17 where t13.a <= t17.a;
explain (costs off) select * from t13, t18 where t13.a <= t18.a;
explain (costs off) select * from t13, t19 where t13.a <= t19.a;
explain (costs off) select * from t13, t20 where t13.a <= t20.a;
explain (costs off) select * from t13, t21 where t13.a <= t21.a;

explain (costs off) select * from t14, t12 where t14.a <= t12.a;
explain (costs off) select * from t14, t13 where t14.a <= t13.a;
explain (costs off) select * from t14, t15 where t14.a <= t15.a;
explain (costs off) select * from t14, t16 where t14.a <= t16.a;
explain (costs off) select * from t14, t17 where t14.a <= t17.a;
explain (costs off) select * from t14, t18 where t14.a <= t18.a;
explain (costs off) select * from t14, t19 where t14.a <= t19.a;
explain (costs off) select * from t14, t20 where t14.a <= t20.a;
explain (costs off) select * from t14, t21 where t14.a <= t21.a;

explain (costs off) select * from t15, t12 where t15.a <= t12.a;
explain (costs off) select * from t15, t13 where t15.a <= t13.a;
explain (costs off) select * from t15, t14 where t15.a <= t14.a;
explain (costs off) select * from t15, t16 where t15.a <= t16.a;
explain (costs off) select * from t15, t17 where t15.a <= t17.a;
explain (costs off) select * from t15, t18 where t15.a <= t18.a;
explain (costs off) select * from t15, t19 where t15.a <= t19.a;
explain (costs off) select * from t15, t20 where t15.a <= t20.a;
explain (costs off) select * from t15, t21 where t15.a <= t21.a;

explain (costs off) select * from t16, t12 where t16.a <= t12.a;
explain (costs off) select * from t16, t13 where t16.a <= t13.a;
explain (costs off) select * from t16, t14 where t16.a <= t14.a;
explain (costs off) select * from t16, t15 where t16.a <= t15.a;
explain (costs off) select * from t16, t17 where t16.a <= t17.a;
explain (costs off) select * from t16, t18 where t16.a <= t18.a;
explain (costs off) select * from t16, t19 where t16.a <= t19.a;
explain (costs off) select * from t16, t20 where t16.a <= t20.a;
explain (costs off) select * from t16, t21 where t16.a <= t21.a;

explain (costs off) select * from t17, t12 where t17.a <= t12.a;
explain (costs off) select * from t17, t13 where t17.a <= t13.a;
explain (costs off) select * from t17, t14 where t17.a <= t14.a;
explain (costs off) select * from t17, t15 where t17.a <= t15.a;
explain (costs off) select * from t17, t16 where t17.a <= t16.a;
explain (costs off) select * from t17, t18 where t17.a <= t18.a;
explain (costs off) select * from t17, t19 where t17.a <= t19.a;
explain (costs off) select * from t17, t20 where t17.a <= t20.a;
explain (costs off) select * from t17, t21 where t17.a <= t21.a;

explain (costs off) select * from t18, t12 where t18.a <= t12.a;
explain (costs off) select * from t18, t13 where t18.a <= t13.a;
explain (costs off) select * from t18, t14 where t18.a <= t14.a;
explain (costs off) select * from t18, t15 where t18.a <= t15.a;
explain (costs off) select * from t18, t16 where t18.a <= t16.a;
explain (costs off) select * from t18, t17 where t18.a <= t17.a;
explain (costs off) select * from t18, t19 where t18.a <= t19.a;
explain (costs off) select * from t18, t20 where t18.a <= t20.a;
explain (costs off) select * from t18, t21 where t18.a <= t21.a;

explain (costs off) select * from t19, t12 where t19.a <= t12.a;
explain (costs off) select * from t19, t13 where t19.a <= t13.a;
explain (costs off) select * from t19, t14 where t19.a <= t14.a;
explain (costs off) select * from t19, t15 where t19.a <= t15.a;
explain (costs off) select * from t19, t16 where t19.a <= t16.a;
explain (costs off) select * from t19, t17 where t19.a <= t17.a;
explain (costs off) select * from t19, t18 where t19.a <= t18.a;
explain (costs off) select * from t19, t20 where t19.a <= t20.a;
explain (costs off) select * from t19, t21 where t19.a <= t21.a;

explain (costs off) select * from t20, t12 where t20.a <= t12.a;
explain (costs off) select * from t20, t13 where t20.a <= t13.a;
explain (costs off) select * from t20, t14 where t20.a <= t14.a;
explain (costs off) select * from t20, t15 where t20.a <= t15.a;
explain (costs off) select * from t20, t16 where t20.a <= t16.a;
explain (costs off) select * from t20, t17 where t20.a <= t17.a;
explain (costs off) select * from t20, t18 where t20.a <= t18.a;
explain (costs off) select * from t20, t19 where t20.a <= t19.a;
explain (costs off) select * from t20, t21 where t20.a <= t21.a;

explain (costs off) select * from t21, t12 where t21.a <= t12.a;
explain (costs off) select * from t21, t13 where t21.a <= t13.a;
explain (costs off) select * from t21, t14 where t21.a <= t14.a;
explain (costs off) select * from t21, t15 where t21.a <= t15.a;
explain (costs off) select * from t21, t16 where t21.a <= t16.a;
explain (costs off) select * from t21, t17 where t21.a <= t17.a;
explain (costs off) select * from t21, t18 where t21.a <= t18.a;
explain (costs off) select * from t21, t19 where t21.a <= t19.a;
explain (costs off) select * from t21, t20 where t21.a <= t20.a;


---------- tail ----------
reset enable_seqscan;
drop table t12,t13,t14,t15,t16,t17,t18,t19,t20,t21,t22;
drop schema compare_operator_plan_schema cascade;
reset current_schema;

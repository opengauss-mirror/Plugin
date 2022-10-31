---- b compatibility case
drop database if exists b_datetime_func_test;
create database b_datetime_func_test dbcompatibility 'b';
\c b_datetime_func_test
set datestyle = 'ISO,ymd';
set time zone "Asia/Shanghai";
-- test part-two function
-- test dayname
show lc_time_names;
-- 边界值,坏值
select dayname('0000-1-1');
select dayname('0000-0-1');
select dayname('0000-1-0');
select dayname('0000-2-28');
select dayname('0000-2-29');
select dayname('9999-12-31');
select dayname('10000-1-1');
select dayname('0000-1-1 23:59:59.9999995');
select dayname('0000-12-31 22:59:59.9999995');
select dayname('0000-12-31 23:59:59.9999995');
-- 进位
select dayname('0000-2-28 23:59:59.999999');
select dayname('0000-2-28 23:59:59.9999999');
select dayname('9999-12-31 23:59:59.999999');
select dayname('9999-12-31 23:59:59.9999999');
-- 不同类型输入
select dayname(date'0000-1-1');
select dayname(datetime'9999-12-31 12:12:12');
select dayname(0);
select dayname(101);
select dayname(99991231);
select dayname(20211112235959.999999);
select dayname(20211112235959.9999999);
select dayname(20211112.1);
select dayname(20211112.999999);
select dayname(-1);
select dayname(9999999999999999999999999);

-- test monthname
-- 边界值，坏值
select monthname('0000-1-1');
select monthname('0000-0-1');
select monthname('0000-1-0');
select monthname('0000-2-28');
select monthname('0000-2-29');
select monthname('9999-12-31');
select monthname('10000-1-1');
select monthname('0000-1-1 23:59:59.9999995');
select monthname('0000-12-31 22:59:59.9999995');
select monthname('0000-12-31 23:59:59.9999995');
-- 进位
select monthname('0001-2-28 23:59:59.999999');
select monthname('0001-2-28 23:59:59.9999999');
select monthname('9999-12-31 23:59:59.999999');
select monthname('9999-12-31 23:59:59.9999999');
select monthname('2021-11-30 23:59:59.999999');
select monthname('2021-11-30 23:59:59.9999999');
-- 不同类型输入
select monthname(date'0000-1-1');
select monthname(datetime'9999-12-31 12:12:12');
select monthname(-1);
select monthname(0);
select monthname(101);
select monthname(101.1);
select monthname(99991231);
select monthname(99991231.123);
select monthname(20211130235959.999999);
select monthname(20211130235959.999999);
select monthname(20211130235959.9999999);
select monthname(99999999999999999999999);

-- GUC参数lc_time_names
show lc_time_names;
select dayname('2021-11-12');
select monthname('2021-11-12');
alter system set lc_time_names = 'zh_CN';
select pg_sleep(1);
select dayname('2021-11-12');
select monthname('2021-11-12');
alter system set lc_time_names = 'aaa';
alter system set lc_time_names = 'en_US';
select pg_sleep(1);

-- test time_to_sec
-- 边界值，坏值
select time_to_sec('-838:59:59.1');
select time_to_sec('-838:59:59');
select time_to_sec('838:59:59.1');
select time_to_sec('838:59:59');
select time_to_sec('-3 838:59:59');
select time_to_sec('3 838:59:59');
select time_to_sec('839:00:00');
select time_to_sec('-839:00:00');
select time_to_sec('0000-2-28 1:00:00');
select time_to_sec('0000-2-29 1:00:00');
select time_to_sec('0000-0-28 1:00:00');
select time_to_sec('0000-2-0 1:00:00');
select time_to_sec('9999-12-31 23:59:59');
select time_to_sec('10000-1-1 23:59:59');
-- 进位
select time_to_sec('1:0:0.999999');
select time_to_sec('1:0:0.9999999');
select time_to_sec('0000-2-28 1:0:0.999999');
select time_to_sec('0000-2-29 1:0:0.9999999');
select time_to_sec('2021-1-31 23:59:59.999999');
select time_to_sec('2021-1-31 23:59:59.9999999');
select time_to_sec('9999-12-31 23:59:59.999999');
select time_to_sec('9999-12-31 23:59:59.9999999');
-- 不同类型输入
select time_to_sec(date'2021-11-12');
select time_to_sec(datetime'2021-11-12 23:59:59.9999994');
select time_to_sec(datetime'2021-11-12 23:59:59.9999995');
select time_to_sec(time'838:59:59');
select time_to_sec(0);
select time_to_sec(30);
select time_to_sec(59);
select time_to_sec(60);
select time_to_sec(235959.999999);
select time_to_sec(235959.9999999);

-- test month
-- 边界值,坏值
select month('0000-1-1');
select month('0000-0-1');
select month('0000-1-0');
select month('0000-2-28');
select month('0000-2-29');
select month('9999-12-31');
select month('10000-1-1');
-- 进位
select month('0000-2-28 23:59:59.999999');
select month('0000-2-28 23:59:59.9999999');
select month('9999-12-31 23:59:59.999999');
select month('9999-12-31 23:59:59.9999999');
select month('2021-11-30 23:59:59.999999');
select month('2021-11-30 23:59:59.9999999');
-- 不同类型输入
select month(date'0000-1-1');
select month(datetime'9999-12-31 12:12:12');
select month(-1);
select month(0);
select month(101);
select month(101.1);
select month(99991231);
select month(99991231.1);
select month(20211130235959.999999);
select month(20211130235959.9999999);

-- test last_day
set b_compatibility_mode = true;
-- 边界值,坏值
select last_day('0000-1-1');
select last_day('0000-0-1');
select last_day('0000-1-0');
select last_day('0000-2-28');
select last_day('0000-2-29');
select last_day('9999-12-31');
select last_day('10000-1-1');
select last_day('0000-1-1 23:59:59.999999');
select last_day('0000-1-1 23:59:59.9999995');
-- 进位
select last_day('0000-2-28 23:59:59.999999');
select last_day('0000-2-28 23:59:59.9999999');
select last_day('9999-12-31 23:59:59.999999');
select last_day('9999-12-31 23:59:59.9999999');
select last_day('2021-11-30 23:59:59.999999');
select last_day('2021-11-30 23:59:59.9999999');
-- 不同类型输入
select last_day(date'0000-1-1');
select last_day(datetime'9999-12-31 12:12:12');
select last_day(-1);
select last_day(0);
select last_day(101);
select last_day(101.1);
select last_day(99991231);
select last_day(20211130235959.999999);
select last_day(20211130235959.9999999);
select last_day(20211112235959.999999);
select last_day(20211112235959.9999999);
set b_compatibility_mode = false;

-- test date
-- 边界值,坏值
select date('0000-1-1');
select date('0000-0-1');
select date('0000-1-0');
select date('0000-2-28');
select date('0000-2-29');
select date('9999-12-31');
select date('10000-1-1');
select date('0000-1-1 12:12:12.9999995');
select date('0000-12-31 22:59:59.999999');
-- 进位
select date('0000-2-28 23:59:59.999999');
select date('0000-2-28 23:59:59.9999999');
select date('9999-12-31 23:59:59.999999');
select date('9999-12-31 23:59:59.9999999');
select date('2021-11-30 23:59:59.999999');
select date('2021-11-30 23:59:59.9999999');
-- 不同类型输入
select date(date'0000-1-1');
select date(datetime'9999-12-31 12:12:12');
select date(0);
select date(-1);
select date(101);
select date(99991231);
select date(20211130235959.999999);
select date(20211130235959.9999999);
select date(20211112235959.999999);
select date(20211112235959.9999999);

-- test day
-- 边界值,坏值
select day('0000-1-1');
select day('0000-0-1');
select day('0000-1-0');
select day('0000-2-28');
select day('0000-2-29');
select day('9999-12-31');
select day('10000-1-1');
select day('0000-1-1 12:12:12.9999995');
select day('0000-12-31 23:59:59.9999995');
-- 进位
select day('0000-2-28 23:59:59.999999');
select day('0000-2-28 23:59:59.9999999');
select day('9999-12-31 23:59:59.999999');
select day('9999-12-31 23:59:59.9999999');
select day('2021-11-30 23:59:59.999999');
select day('2021-11-30 23:59:59.9999999');
-- 不同类型输入
select day(date'0000-1-1');
select day(datetime'9999-12-31 12:12:12');
select day(-1);
select day(0);
select day(101.1);
select day(99991231);
select day(20211130235959.999999);
select day(20211130235959.9999999);

--test week
-- 单参数
show default_week_format;
-- 边界值,坏值
select week('0000-1-1');
select week('0000-0-1');
select week('0000-1-0');
select week('0000-2-28');
select week('0000-2-29');
select week('9999-12-31');
select week('10000-1-1');
select week('0000-1-1 12:00:00.9999995');
select week('0000-12-31 23:59:59.9999995');
-- 进位
select week('0000-2-28 23:59:59.999999');
select week('0000-2-28 23:59:59.9999999');
select week('9999-12-31 23:59:59.999999');
select week('9999-12-31 23:59:59.9999999');
select week('2021-11-30 23:59:59.999999');
select week('2021-11-30 23:59:59.9999999');
-- 不同类型输入
select week(date'0000-1-1');
select week(datetime'9999-12-31 12:12:12');
select week(-1);
select week(0);
select week(101);
select week(101.1);
select week(99991231);
select week(99991231.123);
select week(20211130235959.999999);
select week(20211130235959.9999999);
-- 双参数 
select week('2000-1-1', 0);
select week('2000-1-1', 1);
select week('2000-1-1', 2);
select week('2000-1-1', 3);
select week('2000-1-1', 4);
select week('2000-1-1', 5);
select week('2000-1-1', 6);
select week('2000-1-1', 7);
select week(20000101, 0);
select week(date'0000-1-1', 1);
select week(datetime'9999-12-31 12:12:12', 2);
select week(datetime'9999-12-31 23:59:59.999999', 2);
select week(date'2000-1-1', 2);
-- GUC
show default_week_format;
alter system set default_week_format = 1;
select pg_sleep(1);
select week('2000-1-1');
alter system set default_week_format = 2;
select pg_sleep(1);
select week('2000-1-1');
alter system set default_week_format = 0;
select pg_sleep(1);

--test yearweek
-- 单参数
-- 边界值,坏值
select yearweek('0000-1-1');
select yearweek('0000-0-1');
select yearweek('0000-1-0');
select yearweek('0000-2-28');
select yearweek('0000-2-29');
select yearweek('9999-12-31');
select yearweek('10000-1-1');
select yearweek('0000-1-1 12:12:12.9999995');
select yearweek('0000-12-31 23:59:59.9999999');
-- 进位
select yearweek('0000-2-28 23:59:59.999999');
select yearweek('0000-2-28 23:59:59.9999999');
select yearweek('9999-12-31 23:59:59.999999');
select yearweek('9999-12-31 23:59:59.9999999');
select yearweek('2021-11-30 23:59:59.999999');
select yearweek('2021-11-30 23:59:59.9999999');
-- 不同类型输入
select yearweek(date'0000-1-1');
select yearweek(datetime'9999-12-31 12:12:12');
select yearweek(-1);
select yearweek(0);
select yearweek(101);
select yearweek(101.1);
select yearweek(99991231);
select yearweek(99991231.123);
select yearweek(20211130235959.999999);
select yearweek(20211130235959.9999999);

-- 双参数 
select yearweek('2000-1-1', 0);
select yearweek('2000-1-1', 1);
select yearweek('2000-1-1', 2);
select yearweek('2000-1-1', 3);
select yearweek('2000-1-1', 4);
select yearweek('2000-1-1', 5);
select yearweek('2000-1-1', 6);
select yearweek('2000-1-1', 7);
select yearweek(20000101, 0);
select yearweek(date'0000-1-1', 1);
select yearweek(datetime'9999-12-31 12:12:12', 2);
select yearweek(datetime'9999-12-31 23:59:59.999999', 2);
select yearweek(date'2000-1-1', 2);
\c contrib_regression
DROP DATABASE b_datetime_func_test;
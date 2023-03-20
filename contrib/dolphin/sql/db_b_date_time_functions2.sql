---- b compatibility case
drop database if exists b_datetime_func_test2;
create database b_datetime_func_test2 encoding 'UTF-8' lc_collate 'C' lc_ctype 'C' dbcompatibility 'B';
\c b_datetime_func_test2
set datestyle = 'ISO,ymd';
set time zone "Asia/Shanghai";
create table test(funcname text, result text);
-- test part-two function
-- test dayname
alter system set dolphin.lc_time_names = 'en_US';
show dolphin.lc_time_names;
-- 严格模式或者非严格模式都有值
-- 功能
insert into test values('dayname(''2021-11-12'')', dayname('2021-11-12'));
insert into test values('dayname(''2021-11-13'')', dayname('2021-11-13'));
insert into test values('dayname(''2021-11-14'')', dayname('2021-11-14'));
insert into test values('dayname(''2021-11-15'')', dayname('2021-11-15'));
insert into test values('dayname(''2021-11-16'')', dayname('2021-11-16'));
insert into test values('dayname(''2021-11-17'')', dayname('2021-11-17'));
insert into test values('dayname(''2021-11-18'')', dayname('2021-11-18'));
insert into test values('dayname(''0000-1-1 23:59:59.9999995'')', dayname('0000-1-1 23:59:59.9999995'));
insert into test values('dayname(''0000-12-31 22:59:59.9999995'')', dayname('0000-12-31 22:59:59.9999995'));
insert into test values('dayname(''0000-12-31 23:59:59.9999995'')', dayname('0000-12-31 23:59:59.9999995'));
-- 边界
insert into test values('dayname(''0000-1-1'')', dayname('0000-1-1'));
insert into test values('dayname(''9999-12-31'')', dayname('9999-12-31'));
insert into test values('dayname(''0000-1-1 00:00:00'')', dayname('0000-1-1 00:00:00'));
insert into test values('dayname(''9999-12-31 23:59:59.999999'')', dayname('9999-12-31 23:59:59.999999'));
-- 类型
insert into test values('dayname(date''0000-1-1'')', dayname(date'0000-1-1'));
insert into test values('dayname(cast(''9999-12-31 12:12:12'' as datetime))', dayname(cast('9999-12-31 12:12:12' as datetime)));
insert into test values('dayname(101)', dayname(101));
insert into test values('dayname(99991231)', dayname(99991231));
insert into test values('dayname(99991231235959.999999)', dayname(99991231235959.999999));
insert into test values('dayname(20211112235959.999999)', dayname(20211112235959.999999));
insert into test values('dayname(20211112235959.9999999)', dayname(20211112235959.9999999));
-- 特异
insert into test values('dayname(null)', dayname(null));

-- 非严格模式，参数不合法，报warning，返回NULL或者对应值
set dolphin.sql_mode = 'sql_mode_full_group,pipes_as_concat,ansi_quotes,no_zero_date';
insert into test values('dayname(''10000-1-1'')', dayname('10000-1-1'));
insert into test values('dayname(''9999-12-31 23:59:59.9999995'')', dayname('9999-12-31 23:59:59.9999995'));
insert into test values('dayname(-1)', dayname(-1));
insert into test values('dayname(0)', dayname(0));
insert into test values('dayname(100000101)', dayname(100000101));
insert into test values('dayname(99991231235959.9999995)', dayname(99991231235959.9999995));
insert into test values('dayname(9999999999999999999999)', dayname(9999999999999999999999));

-- 严格模式，参数不合法，抛出错误
set dolphin.sql_mode = 'sql_mode_strict,sql_mode_full_group,pipes_as_concat,ansi_quotes,no_zero_date';
insert into test values('dayname(''10000-1-1'')', dayname('10000-1-1'));
insert into test values('dayname(''9999-12-31 23:59:59.9999995'')', dayname('9999-12-31 23:59:59.9999995'));
insert into test values('dayname(-1)', dayname(-1));
insert into test values('dayname(0)', dayname(0));
insert into test values('dayname(100000101)', dayname(100000101));
insert into test values('dayname(99991231235959.9999995)', dayname(99991231235959.9999995));
insert into test values('dayname(9999999999999999999999)', dayname(9999999999999999999999));

-- test monthname
alter system set dolphin.lc_time_names = 'en_US';
show dolphin.lc_time_names;
-- 严格模式或者非严格模式都有值
-- 功能
insert into test values('monthname(''2021-1-1'')', monthname('2021-1-1'));
insert into test values('monthname(''2021-2-1'')', monthname('2021-2-1'));
insert into test values('monthname(''2021-3-1'')', monthname('2021-3-1'));
insert into test values('monthname(''2021-4-1'')', monthname('2021-4-1'));
insert into test values('monthname(''2021-5-1'')', monthname('2021-5-1'));
insert into test values('monthname(''2021-6-1'')', monthname('2021-6-1'));
insert into test values('monthname(''2021-7-1'')', monthname('2021-7-1'));
insert into test values('monthname(''2021-8-1'')', monthname('2021-8-1'));
insert into test values('monthname(''2021-9-1'')', monthname('2021-9-1'));
insert into test values('monthname(''2021-10-1'')', monthname('2021-10-1'));
insert into test values('monthname(''2021-11-1'')', monthname('2021-11-1'));
insert into test values('monthname(''2021-12-1'')', monthname('2021-12-1'));
insert into test values('monthname(''0000-1-1 23:59:59.9999995'')', monthname('0000-1-1 23:59:59.9999995'));
insert into test values('monthname(''0000-12-31 22:59:59.9999995'')', monthname('0000-12-31 22:59:59.9999995'));
insert into test values('monthname(''0000-12-31 23:59:59.9999995'')', monthname('0000-12-31 23:59:59.9999995'));
-- 边界
insert into test values('monthname(''0000-1-1'')', monthname('0000-1-1'));
insert into test values('monthname(''9999-12-31'')', monthname('9999-12-31'));
insert into test values('monthname(''0000-1-1 00:00:00'')', monthname('0000-1-1 00:00:00'));
insert into test values('monthname(''9999-12-31 23:59:59.999999'')', monthname('9999-12-31 23:59:59.999999'));
-- 类型
insert into test values('monthname(date''0000-1-1'')', monthname(date'0000-1-1'));
insert into test values('monthname(cast(''9999-12-31 12:12:12'' as datetime))', monthname(cast('9999-12-31 12:12:12' as datetime)));
insert into test values('monthname(101)', monthname(101));
insert into test values('monthname(99991231)', monthname(99991231));
insert into test values('monthname(99991231235959.999999)', monthname(99991231235959.999999));
insert into test values('monthname(20211112235959.999999)', monthname(20211112235959.999999));
insert into test values('monthname(20211112235959.9999999)', monthname(20211112235959.9999999));
-- 特异
insert into test values('monthname(null)', monthname(null));
insert into test values('monthname(''2021-00-01'')', monthname('2021-00-01'));
insert into test values('monthname(20210001)', monthname(20210001));
insert into test values('monthname(0)', monthname(0));
set dolphin.sql_mode = 'sql_mode_strict,sql_mode_full_group,pipes_as_concat,ansi_quotes';
insert into test values('monthname(''0000-00-00'')', monthname('0000-00-00'));
set dolphin.sql_mode = 'sql_mode_strict,sql_mode_full_group,pipes_as_concat,ansi_quotes,no_zero_date';
insert into test values('monthname(''0000-00-00'')', monthname('0000-00-00'));

-- 非严格模式，参数不合法，报warning，返回NULL或者对应值
set dolphin.sql_mode = 'sql_mode_full_group,pipes_as_concat,ansi_quotes,no_zero_date';
insert into test values('monthname(''10000-1-1'')', monthname('10000-1-1'));
insert into test values('monthname(''9999-12-31 23:59:59.9999995'')', monthname('9999-12-31 23:59:59.9999995'));
insert into test values('monthname(-1)', monthname(-1));
insert into test values('monthname(100000101)', monthname(100000101));
insert into test values('monthname(99991231235959.9999995)', monthname(99991231235959.9999995));
insert into test values('monthname(9999999999999999999999)', monthname(9999999999999999999999));

-- 严格模式，参数不合法，抛出错误
set dolphin.sql_mode = 'sql_mode_strict,sql_mode_full_group,pipes_as_concat,ansi_quotes,no_zero_date';
insert into test values('monthname(''10000-1-1'')', monthname('10000-1-1'));
insert into test values('monthname(''9999-12-31 23:59:59.9999995'')', monthname('9999-12-31 23:59:59.9999995'));
insert into test values('monthname(-1)', monthname(-1));
insert into test values('monthname(100000101)', monthname(100000101));
insert into test values('monthname(99991231235959.9999995)', monthname(99991231235959.9999995));
insert into test values('monthname(9999999999999999999999)', monthname(9999999999999999999999));

-- test lc_time_names
alter system set dolphin.lc_time_names = 'en_US';
select pg_sleep(1);
show dolphin.lc_time_names;
select dayname('2021-11-12');
select monthname('2021-11-12');
alter system set dolphin.lc_time_names = 'zh_CN';
select pg_sleep(1);
select dayname('2021-11-12');
select monthname('2021-11-12');
alter system set dolphin.lc_time_names = 'aaa';
alter system set dolphin.lc_time_names = 'en_US';
select pg_sleep(1);

-- test time_to_sec
-- 严格模式或者非严格模式都有值
-- 功能
insert into test values('time_to_sec(''00:00:00'')', time_to_sec('00:00:00'));
insert into test values('time_to_sec(''00:00:01'')', time_to_sec('00:00:01'));
insert into test values('time_to_sec(''-00:00:01'')', time_to_sec('-00:00:01'));
insert into test values('time_to_sec(''12:12:12.1'')', time_to_sec('12:12:12.1'));
insert into test values('time_to_sec(''-12:12:12.1'')', time_to_sec('-12:12:12.1'));
insert into test values('time_to_sec(''111:12:12.1'')', time_to_sec('111:12:12.1'));
insert into test values('time_to_sec(''-111:12:12.1'')', time_to_sec('-111:12:12.1'));
insert into test values('time_to_sec(''2021-11-12 00:00:00'')', time_to_sec('2021-11-12 00:00:00'));
insert into test values('time_to_sec(''2021-11-12 12:12:12'')', time_to_sec('2021-11-12 12:12:12'));
insert into test values('time_to_sec(''2021-11-12 23:59:59'')', time_to_sec('2021-11-12 23:59:59'));
insert into test values('time_to_sec(''2021-11-12 23:59:59.9999995'')', time_to_sec('2021-11-12 23:59:59.9999995'));
-- 边界
insert into test values('time_to_sec(''838:59:59'')', time_to_sec('838:59:59'));
insert into test values('time_to_sec(''-838:59:59'')', time_to_sec('-838:59:59'));
insert into test values('time_to_sec(''34 22:59:59'')', time_to_sec('34 22:59:59'));
insert into test values('time_to_sec(''-34 22:59:59'')', time_to_sec('-34 22:59:59'));
-- 类型
insert into test values('time_to_sec(date''2021-11-12'')', time_to_sec(date'2021-11-12'));
insert into test values('time_to_sec(cast(''9999-12-31 12:12:12'' as datetime))', time_to_sec(cast('9999-12-31 12:12:12' as datetime)));
insert into test values('time_to_sec(-1)', time_to_sec(-1));
insert into test values('time_to_sec(0)', time_to_sec(0));
insert into test values('time_to_sec(1)', time_to_sec(1));
insert into test values('time_to_sec(-8385959)', time_to_sec(-8385959));
insert into test values('time_to_sec(8385959)', time_to_sec(8385959));
-- 特异
insert into test values('time_to_sec(null)', time_to_sec(null));

-- 非严格模式，参数不合法，报warning，返回NULL或者对应值边界值
set dolphin.sql_mode = 'sql_mode_full_group,pipes_as_concat,ansi_quotes,no_zero_date';
insert into test values('time_to_sec(''00:00:60'')', time_to_sec('00:00:60'));
insert into test values('time_to_sec(''00:60:00'')', time_to_sec('00:60:00'));
insert into test values('time_to_sec(''838:59:59.1'')', time_to_sec('838:59:59.1'));
insert into test values('time_to_sec(''-838:59:59.1'')', time_to_sec('-838:59:59.1'));
insert into test values('time_to_sec(''839:00:00'')', time_to_sec('839:00:00'));
insert into test values('time_to_sec(''-839:00:00'')', time_to_sec('-839:00:00'));
insert into test values('time_to_sec(''34 22:59:59.1'')', time_to_sec('34 22:59:59.1'));
insert into test values('time_to_sec(''-34 22:59:59.1'')', time_to_sec('-34 22:59:59.1'));
insert into test values('time_to_sec(''34 23:00:00'')', time_to_sec('34 23:00:00'));
insert into test values('time_to_sec(''-34 23:00:00'')', time_to_sec('-34 23:00:00'));

-- 严格模式，参数不合法，抛出错误
set dolphin.sql_mode = 'sql_mode_strict,sql_mode_full_group,pipes_as_concat,ansi_quotes,no_zero_date';
insert into test values('time_to_sec(''00:00:60'')', time_to_sec('00:00:60'));
insert into test values('time_to_sec(''00:60:00'')', time_to_sec('00:60:00'));
insert into test values('time_to_sec(''838:59:59.1'')', time_to_sec('838:59:59.1'));
insert into test values('time_to_sec(''-838:59:59.1'')', time_to_sec('-838:59:59.1'));
insert into test values('time_to_sec(''839:00:00'')', time_to_sec('839:00:00'));
insert into test values('time_to_sec(''-839:00:00'')', time_to_sec('-839:00:00'));
insert into test values('time_to_sec(''34 22:59:59.1'')', time_to_sec('34 22:59:59.1'));
insert into test values('time_to_sec(''-34 22:59:59.1'')', time_to_sec('-34 22:59:59.1'));
insert into test values('time_to_sec(''34 23:00:00'')', time_to_sec('34 23:00:00'));
insert into test values('time_to_sec(''-34 23:00:00'')', time_to_sec('-34 23:00:00'));

-- test month
-- 严格模式或者非严格模式都有值
-- 功能
insert into test values('month(''2021-1-1'')', month('2021-1-1'));
insert into test values('month(''2021-2-1'')', month('2021-2-1'));
insert into test values('month(''2021-3-1'')', month('2021-3-1'));
insert into test values('month(''2021-4-1'')', month('2021-4-1'));
insert into test values('month(''2021-5-1'')', month('2021-5-1'));
insert into test values('month(''2021-6-1'')', month('2021-6-1'));
insert into test values('month(''2021-7-1'')', month('2021-7-1'));
insert into test values('month(''2021-8-1'')', month('2021-8-1'));
insert into test values('month(''2021-9-1'')', month('2021-9-1'));
insert into test values('month(''2021-10-1'')', month('2021-10-1'));
insert into test values('month(''2021-11-1'')', month('2021-11-1'));
insert into test values('month(''2021-12-1'')', month('2021-12-1'));
insert into test values('month(''0000-1-1 23:59:59.9999995'')', month('0000-1-1 23:59:59.9999995'));
insert into test values('month(''0000-12-31 22:59:59.9999995'')', month('0000-12-31 22:59:59.9999995'));
insert into test values('month(''0000-12-31 23:59:59.9999995'')', month('0000-12-31 23:59:59.9999995'));
insert into test values('month(''0000-1-1'')', month('0000-1-1'));
insert into test values('month(''9999-12-31'')', month('9999-12-31'));
insert into test values('month(''0000-1-1 00:00:00'')', month('0000-1-1 00:00:00'));
insert into test values('month(''9999-12-31 23:59:59.999999'')', month('9999-12-31 23:59:59.999999'));
-- 类型
insert into test values('month(date''0000-1-1'')', month(date'0000-1-1'));
insert into test values('month(cast(''9999-12-31 12:12:12'' as datetime))', month(cast('9999-12-31 12:12:12' as datetime)));
insert into test values('month(101)', month(101));
insert into test values('month(99991231)', month(99991231));
insert into test values('month(99991231235959.999999)', month(99991231235959.999999));
insert into test values('month(20211112235959.999999)', month(20211112235959.999999));
insert into test values('month(20211112235959.9999999)', month(20211112235959.9999999));
-- 特异
insert into test values('month(null)', month(null));
insert into test values('month(''2021-00-01'')', month('2021-00-01'));
insert into test values('month(''2021-01-00'')', month('2021-01-00'));
insert into test values('month(''2021-00-00'')', month('2021-00-00'));
insert into test values('month(20210001)', month(20210001));
insert into test values('month(20210100)', month(20210100));
insert into test values('month(20210000)', month(20210000));
insert into test values('month(0)', month(0));
set dolphin.sql_mode = 'sql_mode_strict,sql_mode_full_group,pipes_as_concat,ansi_quotes';
insert into test values('month(''0000-00-00'')', month('0000-00-00'));
set dolphin.sql_mode = 'sql_mode_strict,sql_mode_full_group,pipes_as_concat,ansi_quotes,no_zero_date';
insert into test values('month(''0000-00-00'')', month('0000-00-00'));

-- 非严格模式，参数不合法，报warning，返回NULL或者对应值
set dolphin.sql_mode = 'sql_mode_full_group,pipes_as_concat,ansi_quotes,no_zero_date';
insert into test values('month(''10000-1-1'')', month('10000-1-1'));
insert into test values('month(''9999-12-31 23:59:59.9999995'')', month('9999-12-31 23:59:59.9999995'));
insert into test values('month(-1)', month(-1));
insert into test values('month(100000101)', month(100000101));
insert into test values('month(99991231235959.9999995)', month(99991231235959.9999995));
insert into test values('month(9999999999999999999999)', month(9999999999999999999999));

-- 严格模式，参数不合法，抛出错误
set dolphin.sql_mode = 'sql_mode_strict,sql_mode_full_group,pipes_as_concat,ansi_quotes,no_zero_date';
insert into test values('month(''10000-1-1'')', month('10000-1-1'));
insert into test values('month(''9999-12-31 23:59:59.9999995'')', month('9999-12-31 23:59:59.9999995'));
insert into test values('month(-1)', month(-1));
insert into test values('month(100000101)', month(100000101));
insert into test values('month(99991231235959.9999995)', month(99991231235959.9999995));
insert into test values('month(9999999999999999999999)', month(9999999999999999999999));

-- test last_day
set dolphin.b_compatibility_mode = true;
-- 严格模式或者非严格模式都有值
-- 功能
insert into test values('last_day(''0000-2-1'')', last_day('0000-2-1'));
insert into test values('last_day(''2021-1-1'')', last_day('2021-1-1'));
insert into test values('last_day(''2021-2-1'')', last_day('2021-2-1'));
insert into test values('last_day(''2021-3-1'')', last_day('2021-3-1'));
insert into test values('last_day(''2021-4-1'')', last_day('2021-4-1'));
insert into test values('last_day(''2021-5-1'')', last_day('2021-5-1'));
insert into test values('last_day(''2021-6-1'')', last_day('2021-6-1'));
insert into test values('last_day(''2021-7-1'')', last_day('2021-7-1'));
insert into test values('last_day(''2021-8-1'')', last_day('2021-8-1'));
insert into test values('last_day(''2021-9-1'')', last_day('2021-9-1'));
insert into test values('last_day(''2021-10-1'')', last_day('2021-10-1'));
insert into test values('last_day(''2021-11-1'')', last_day('2021-11-1'));
insert into test values('last_day(''2021-12-1'')', last_day('2021-12-1'));
insert into test values('last_day(''0000-12-31 23:59:59.9999995'')', last_day('0000-12-31 23:59:59.9999995'));
-- 边界
insert into test values('last_day(''0000-1-1'')', last_day('0000-1-1'));
insert into test values('last_day(''9999-12-31'')', last_day('9999-12-31'));
insert into test values('last_day(''0000-1-1 00:00:00'')', last_day('0000-1-1 00:00:00'));
insert into test values('last_day(''9999-12-31 23:59:59.999999'')', last_day('9999-12-31 23:59:59.999999'));
-- 类型
insert into test values('last_day(date''0000-1-1'')', last_day(date'0000-1-1'));
insert into test values('last_day(cast(''9999-12-31 12:12:12'' as datetime))', last_day(cast('9999-12-31 12:12:12' as datetime)));
insert into test values('last_day(101)', last_day(101));
insert into test values('last_day(99991231)', last_day(99991231));
insert into test values('last_day(99991231235959.999999)', last_day(99991231235959.999999));
insert into test values('last_day(20211112235959.999999)', last_day(20211112235959.999999));
insert into test values('last_day(20211112235959.9999999)', last_day(20211112235959.9999999));
-- 特异
insert into test values('last_day(null)', last_day(null));
insert into test values('last_day(''2021-01-00'')', last_day('2021-01-00'));
insert into test values('last_day(20210100)', last_day(20210100));

-- 非严格模式，参数不合法，报warning，返回NULL或者对应值
set dolphin.sql_mode = 'sql_mode_full_group,pipes_as_concat,ansi_quotes,no_zero_date';
insert into test values('last_day(''2021-00-01'')', last_day('2021-00-01'));
insert into test values('last_day(20210001)', last_day(20210001));
insert into test values('last_day(''0000-1-1 23:59:59.9999995'')', last_day('0000-1-1 23:59:59.9999995'));
insert into test values('last_day(''0000-12-31 22:59:59.9999995'')', last_day('0000-12-31 22:59:59.9999995'));
insert into test values('last_day(''10000-1-1'')', last_day('10000-1-1'));
insert into test values('last_day(''9999-12-31 23:59:59.9999995'')', last_day('9999-12-31 23:59:59.9999995'));
insert into test values('last_day(-1)', last_day(-1));
insert into test values('last_day(0)', last_day(0));
insert into test values('last_day(100000101)', last_day(100000101));
insert into test values('last_day(99991231235959.9999995)', last_day(99991231235959.9999995));
insert into test values('last_day(9999999999999999999999)', last_day(9999999999999999999999));

-- 严格模式，参数不合法，抛出错误
set dolphin.sql_mode = 'sql_mode_strict,sql_mode_full_group,pipes_as_concat,ansi_quotes,no_zero_date';
insert into test values('last_day(''2021-00-01'')', last_day('2021-00-01'));
insert into test values('last_day(20210001)', last_day(20210001));
insert into test values('last_day(''0000-1-1 23:59:59.9999995'')', last_day('0000-1-1 23:59:59.9999995'));
insert into test values('last_day(''0000-12-31 22:59:59.9999995'')', last_day('0000-12-31 22:59:59.9999995'));
insert into test values('last_day(''10000-1-1'')', last_day('10000-1-1'));
insert into test values('last_day(''9999-12-31 23:59:59.9999995'')', last_day('9999-12-31 23:59:59.9999995'));
insert into test values('last_day(-1)', last_day(-1));
insert into test values('last_day(0)', last_day(0));
insert into test values('last_day(100000101)', last_day(100000101));
insert into test values('last_day(99991231235959.9999995)', last_day(99991231235959.9999995));
insert into test values('last_day(9999999999999999999999)', last_day(9999999999999999999999));
set dolphin.b_compatibility_mode = false;

-- test date
-- 严格模式或者非严格模式都有值
-- 功能
insert into test values('date(''2021-1-1'')', date('2021-1-1'));
insert into test values('date(''2021-1-1 23:59:59'')', date('2021-1-1 23:59:59'));
insert into test values('date(''2021-1-1 23:59:59.9999995'')', date('2021-1-1 23:59:59.9999995'));
-- 边界
insert into test values('date(''0000-1-1'')', date('0000-1-1'));
insert into test values('date(''9999-12-31'')', date('9999-12-31'));
insert into test values('date(''0000-1-1 00:00:00'')', date('0000-1-1 00:00:00'));
insert into test values('date(''9999-12-31 23:59:59.999999'')', date('9999-12-31 23:59:59.999999'));
insert into test values('date(''0000-1-1 23:59:59.9999995'')', date('0000-1-1 23:59:59.9999995'));
insert into test values('date(''0000-12-31 22:59:59.9999995'')', date('0000-12-31 22:59:59.9999995'));
insert into test values('date(''0000-12-31 23:59:59.9999995'')', date('0000-12-31 23:59:59.9999995'));
-- 类型
insert into test values('date(date''0000-1-1'')', date(date'0000-1-1'));
insert into test values('date(cast(''9999-12-31 12:12:12'' as datetime))', date(cast('9999-12-31 12:12:12' as datetime)));
insert into test values('date(101)', date(101));
insert into test values('date(99991231)', date(99991231));
insert into test values('date(99991231235959.999999)', date(99991231235959.999999));
insert into test values('date(20211112235959.999999)', date(20211112235959.999999));
insert into test values('date(20211112235959.9999999)', date(20211112235959.9999999));
-- 特异
insert into test values('date(null)', date(null));
insert into test values('date(''2021-1-0'')', date('2021-1-0'));
insert into test values('date(''2021-0-1'')', date('2021-0-1'));
insert into test values('date(''2021-0-0'')', date('2021-0-0'));
insert into test values('date(0)', date(0));
insert into test values('date(20210031)', date(20210031));
insert into test values('date(20210100)', date(20210100));
insert into test values('date(20210100)', date(20210100));
set dolphin.sql_mode = 'sql_mode_strict,sql_mode_full_group,pipes_as_concat,ansi_quotes,no_zero_date';
insert into test values('date(''0000-00-00'')', date('0000-00-00'));
set dolphin.sql_mode = 'sql_mode_strict,sql_mode_full_group,pipes_as_concat,ansi_quotes';
insert into test values('date(''0000-00-00'')', date('0000-00-00'));

-- 非严格模式，参数不合法，报warning，返回NULL或者对应值
set dolphin.sql_mode = 'sql_mode_full_group,pipes_as_concat,ansi_quotes,no_zero_date';
insert into test values('date(''10000-1-1'')', date('10000-1-1'));
insert into test values('date(''9999-12-31 23:59:59.9999995'')', date('9999-12-31 23:59:59.9999995'));
insert into test values('date(-1)', date(-1));
insert into test values('date(100000101)', date(100000101));
insert into test values('date(99991231235959.9999995)', date(99991231235959.9999995));
insert into test values('date(9999999999999999999999)', date(9999999999999999999999));

-- 严格模式，参数不合法，抛出错误
set dolphin.sql_mode = 'sql_mode_strict,sql_mode_full_group,pipes_as_concat,ansi_quotes,no_zero_date';
insert into test values('date(''10000-1-1'')', date('10000-1-1'));
insert into test values('date(''9999-12-31 23:59:59.9999995'')', date('9999-12-31 23:59:59.9999995'));
insert into test values('date(-1)', date(-1));
insert into test values('date(100000101)', date(100000101));
insert into test values('date(99991231235959.9999995)', date(99991231235959.9999995));
insert into test values('date(9999999999999999999999)', date(9999999999999999999999));

-- test day
-- 严格模式或者非严格模式都有值
-- 功能
insert into test values('day(''2021-1-1'')', day('2021-1-1'));
insert into test values('day(''2021-1-1 00:00:00'')', day('2021-1-1 00:00:00'));
insert into test values('day(''2021-1-1 23:59:59.9999995'')', day('2021-1-1 23:59:59.9999995'));
insert into test values('day(''0000-1-1 23:59:59.9999995'')', day('0000-1-1 23:59:59.9999995'));
insert into test values('day(''0000-12-31 22:59:59.9999995'')', day('0000-12-31 22:59:59.9999995'));
insert into test values('day(''0000-12-31 23:59:59.9999995'')', day('0000-12-31 23:59:59.9999995'));
-- 边界
insert into test values('day(''0000-1-1'')', day('0000-1-1'));
insert into test values('day(''9999-12-31'')', day('9999-12-31'));
insert into test values('day(''0000-1-1 00:00:00'')', day('0000-1-1 00:00:00'));
insert into test values('day(''9999-12-31 23:59:59.999999'')', day('9999-12-31 23:59:59.999999'));
-- 类型
insert into test values('day(date''0000-1-1'')', day(date'0000-1-1'));
insert into test values('day(cast(''9999-12-31 12:12:12'' as datetime))', day(cast('9999-12-31 12:12:12' as datetime)));
insert into test values('day(101)', day(101));
insert into test values('day(99991231)', day(99991231));
insert into test values('day(99991231235959.999999)', day(99991231235959.999999));
insert into test values('day(20211112235959.999999)', day(20211112235959.999999));
insert into test values('day(20211112235959.9999999)', day(20211112235959.9999999));
-- 特异
insert into test values('day(null)', day(null));
insert into test values('day(''2021-00-01'')', day('2021-00-01'));
insert into test values('day(''2021-01-00'')', day('2021-01-00'));
insert into test values('day(''2021-00-00'')', day('2021-00-00'));
insert into test values('day(20210001)', day(20210001));
insert into test values('day(20210100)', day(20210100));
insert into test values('day(20210000)', day(20210000));
insert into test values('day(0)', day(0));
set dolphin.sql_mode = 'sql_mode_strict,sql_mode_full_group,pipes_as_concat,ansi_quotes,no_zero_date';
insert into test values('day(''0000-00-00'')', day('0000-00-00'));
set dolphin.sql_mode = 'sql_mode_strict,sql_mode_full_group,pipes_as_concat,ansi_quotes';
insert into test values('day(''0000-00-00'')', day('0000-00-00'));

-- 非严格模式，参数不合法，报warning，返回NULL或者对应值
set dolphin.sql_mode = 'sql_mode_full_group,pipes_as_concat,ansi_quotes,no_zero_date';
insert into test values('day(''10000-1-1'')', day('10000-1-1'));
insert into test values('day(''9999-12-31 23:59:59.9999995'')', day('9999-12-31 23:59:59.9999995'));
insert into test values('day(-1)', day(-1));
insert into test values('day(100000101)', day(100000101));
insert into test values('day(99991231235959.9999995)', day(99991231235959.9999995));
insert into test values('day(9999999999999999999999)', day(9999999999999999999999));

-- 严格模式，参数不合法，抛出错误
set dolphin.sql_mode = 'sql_mode_strict,sql_mode_full_group,pipes_as_concat,ansi_quotes,no_zero_date';
insert into test values('day(''10000-1-1'')', day('10000-1-1'));
insert into test values('day(''9999-12-31 23:59:59.9999995'')', day('9999-12-31 23:59:59.9999995'));
insert into test values('day(-1)', day(-1));
insert into test values('day(100000101)', day(100000101));
insert into test values('day(99991231235959.9999995)', day(99991231235959.9999995));
insert into test values('day(9999999999999999999999)', day(9999999999999999999999));

--test week
-- 严格模式或者非严格模式都有值
-- 单参数-功能
alter system set dolphin.default_week_format = 0;
select pg_sleep(1);
show dolphin.default_week_format;
insert into test values('week(''2000-1-1'')', week('2000-1-1'));
alter system set dolphin.default_week_format = 1;
select pg_sleep(1);
show dolphin.default_week_format;
insert into test values('week(''2000-1-1'')', week('2000-1-1'));
alter system set dolphin.default_week_format = 2;
select pg_sleep(1);
show dolphin.default_week_format;
insert into test values('week(''2000-1-1'')', week('2000-1-1'));
alter system set dolphin.default_week_format = 3;
select pg_sleep(1);
show dolphin.default_week_format;
insert into test values('week(''2000-1-1'')', week('2000-1-1'));
alter system set dolphin.default_week_format = 4;
select pg_sleep(1);
show dolphin.default_week_format;
insert into test values('week(''2000-1-1'')', week('2000-1-1'));
alter system set dolphin.default_week_format = 5;
select pg_sleep(1);
show dolphin.default_week_format;
insert into test values('week(''2000-1-1'')', week('2000-1-1'));
alter system set dolphin.default_week_format = 6;
select pg_sleep(1);
show dolphin.default_week_format;
insert into test values('week(''2000-1-1'')', week('2000-1-1'));
alter system set dolphin.default_week_format = 7;
select pg_sleep(1);
show dolphin.default_week_format;
insert into test values('week(''2000-1-1'')', week('2000-1-1'));
-- 单参数-边界
alter system set dolphin.default_week_format = 0;
select pg_sleep(1);
show dolphin.default_week_format;
insert into test values('week(''0000-1-1'')', week('0000-1-1'));
insert into test values('week(''9999-12-31'')', week('9999-12-31'));
insert into test values('week(''0000-1-1 00:00:00'')', week('0000-1-1 00:00:00'));
insert into test values('week(''9999-12-31 23:59:59.999999'')', week('9999-12-31 23:59:59.999999'));
insert into test values('week(''0000-12-31 23:59:59.9999995'')', week('0000-12-31 23:59:59.9999995'));
-- 单参数-类型
insert into test values('week(date''0000-1-1'')', week(date'0000-1-1'));
insert into test values('week(cast(''9999-12-31 12:12:12'' as datetime))', week(cast('9999-12-31 12:12:12' as datetime)));
insert into test values('week(101)', week(101));
insert into test values('week(99991231)', week(99991231));
insert into test values('week(99991231235959.999999)', week(99991231235959.999999));
insert into test values('week(20211112235959.999999)', week(20211112235959.999999));
insert into test values('week(20211112235959.9999999)', week(20211112235959.9999999));
-- 单参数-特异
insert into test values('week(null)', week(null));

-- 单参数-非严格模式，参数不合法，报warning，返回NULL或者对应值
set dolphin.sql_mode = 'sql_mode_full_group,pipes_as_concat,ansi_quotes,no_zero_date';
insert into test values('week(''10000-1-1'')', week('10000-1-1'));
insert into test values('week(''9999-12-31 23:59:59.9999995'')', week('9999-12-31 23:59:59.9999995'));
insert into test values('week(0)', week(0));
insert into test values('week(-1)', week(-1));
insert into test values('week(100000101)', week(100000101));
insert into test values('week(99991231235959.9999995)', week(99991231235959.9999995));
insert into test values('week(9999999999999999999999)', week(9999999999999999999999));
insert into test values('week(''0000-1-1 23:59:59.9999995'')', week('0000-1-1 23:59:59.9999995'));
insert into test values('week(''0000-12-31 22:59:59.9999995'')', week('0000-12-31 22:59:59.9999995'));

-- 单参数-严格模式，参数不合法，抛出错误
set dolphin.sql_mode = 'sql_mode_strict,sql_mode_full_group,pipes_as_concat,ansi_quotes,no_zero_date';
insert into test values('week(''10000-1-1'')', week('10000-1-1'));
insert into test values('week(''9999-12-31 23:59:59.9999995'')', week('9999-12-31 23:59:59.9999995'));
insert into test values('week(0)', week(0));
insert into test values('week(-1)', week(-1));
insert into test values('week(100000101)', week(100000101));
insert into test values('week(99991231235959.9999995)', week(99991231235959.9999995));
insert into test values('week(9999999999999999999999)', week(9999999999999999999999));
insert into test values('week(''0000-1-1 23:59:59.9999995'')', week('0000-1-1 23:59:59.9999995'));
insert into test values('week(''0000-12-31 22:59:59.9999995'')', week('0000-12-31 22:59:59.9999995'));

-- 双参数-功能
insert into test values('week(''2000-1-1'', 0)', week('2000-1-1', 0));
insert into test values('week(''2000-1-1'', 1)', week('2000-1-1', 1));
insert into test values('week(''2000-1-1'', 2)', week('2000-1-1', 2));
insert into test values('week(''2000-1-1'', 3)', week('2000-1-1', 3));
insert into test values('week(''2000-1-1'', 4)', week('2000-1-1', 4));
insert into test values('week(''2000-1-1'', 5)', week('2000-1-1', 5));
insert into test values('week(''2000-1-1'', 6)', week('2000-1-1', 6));
insert into test values('week(''2000-1-1'', 7)', week('2000-1-1', 7));
insert into test values('week(''0000-12-31 23:59:59.9999995'', 0)', week('0000-12-31 23:59:59.9999995', 0));
-- 双参数-特异
insert into test values('week(null, 0)', week(null, 0));
insert into test values('week(''2000-1-1'', null)', week('2000-1-1', null));
insert into test values('week(null, null)', week(null, null));

-- 双参数-非严格模式，参数不合法，报warning，返回NULL或者对应值
set dolphin.sql_mode = 'sql_mode_full_group,pipes_as_concat,ansi_quotes,no_zero_date';
insert into test values('week(''10000-1-1'', 0)', week('10000-1-1', 0));
insert into test values('week(''9999-12-31 23:59:59.9999995'', 0)', week('9999-12-31 23:59:59.9999995', 0));
insert into test values('week(0, 0)', week(0, 0));
insert into test values('week(-1, 0)', week(-1, 0));
insert into test values('week(100000101, 0)', week(100000101, 0));
insert into test values('week(99991231235959.9999995, 0)', week(99991231235959.9999995, 0));
insert into test values('week(9999999999999999999999, 0)', week(9999999999999999999999, 0));
insert into test values('week(''0000-1-1 23:59:59.9999995'', 0)', week('0000-1-1 23:59:59.9999995', 0));
insert into test values('week(''0000-12-31 22:59:59.9999995'', 0)', week('0000-12-31 22:59:59.9999995', 0));

-- 双参数-严格模式，参数不合法，抛出错误
set dolphin.sql_mode = 'sql_mode_strict,sql_mode_full_group,pipes_as_concat,ansi_quotes,no_zero_date';
insert into test values('week(''10000-1-1'', 0)', week('10000-1-1', 0));
insert into test values('week(''9999-12-31 23:59:59.9999995'', 0)', week('9999-12-31 23:59:59.9999995', 0));
insert into test values('week(0, 0)', week(0, 0));
insert into test values('week(-1, 0)', week(-1, 0));
insert into test values('week(100000101, 0)', week(100000101, 0));
insert into test values('week(99991231235959.9999995, 0)', week(99991231235959.9999995, 0));
insert into test values('week(9999999999999999999999, 0)', week(9999999999999999999999, 0));
insert into test values('week(''0000-1-1 23:59:59.9999995'', 0)', week('0000-1-1 23:59:59.9999995', 0));
insert into test values('week(''0000-12-31 22:59:59.9999995'', 0)', week('0000-12-31 22:59:59.9999995', 0));

--test yearweek
-- 严格模式或者非严格模式都有值
-- 单参数-功能
insert into test values('yearweek(''2000-1-1'')', yearweek('2000-1-1'));
insert into test values('yearweek(''0000-12-31 23:59:59.9999995'')', yearweek('0000-12-31 23:59:59.9999995'));
-- 单参数-边界
insert into test values('yearweek(''0000-1-1'')', yearweek('0000-1-1'));
insert into test values('yearweek(''9999-12-31'')', yearweek('9999-12-31'));
insert into test values('yearweek(''0000-1-1 00:00:00'')', yearweek('0000-1-1 00:00:00'));
insert into test values('yearweek(''9999-12-31 23:59:59.999999'')', yearweek('9999-12-31 23:59:59.999999'));
-- 单参数-类型
insert into test values('yearweek(date''0000-1-1'')', yearweek(date'0000-1-1'));
insert into test values('yearweek(cast(''9999-12-31 12:12:12'' as datetime))', yearweek(cast('9999-12-31 12:12:12' as datetime)));
insert into test values('yearweek(101)', yearweek(101));
insert into test values('yearweek(99991231)', yearweek(99991231));
insert into test values('yearweek(99991231235959.999999)', yearweek(99991231235959.999999));
insert into test values('yearweek(20211112235959.999999)', yearweek(20211112235959.999999));
insert into test values('yearweek(20211112235959.9999999)', yearweek(20211112235959.9999999));
-- 单参数-特异
insert into test values('yearweek(null)', yearweek(null));

-- 单参数-非严格模式，参数不合法，报warning，返回NULL或者对应值
set dolphin.sql_mode = 'sql_mode_full_group,pipes_as_concat,ansi_quotes,no_zero_date';
insert into test values('yearweek(''10000-1-1'')', yearweek('10000-1-1'));
insert into test values('yearweek(''9999-12-31 23:59:59.9999995'')', yearweek('9999-12-31 23:59:59.9999995'));
insert into test values('yearweek(0)', yearweek(0));
insert into test values('yearweek(-1)', yearweek(-1));
insert into test values('yearweek(100000101)', yearweek(100000101));
insert into test values('yearweek(99991231235959.9999995)', yearweek(99991231235959.9999995));
insert into test values('yearweek(9999999999999999999999)', yearweek(9999999999999999999999));
insert into test values('yearweek(''0000-1-1 23:59:59.9999995'')', yearweek('0000-1-1 23:59:59.9999995'));
insert into test values('yearweek(''0000-12-31 22:59:59.9999995'')', yearweek('0000-12-31 22:59:59.9999995'));

-- 单参数-严格模式，参数不合法，抛出错误
set dolphin.sql_mode = 'sql_mode_strict,sql_mode_full_group,pipes_as_concat,ansi_quotes,no_zero_date';
insert into test values('yearweek(''10000-1-1'')', yearweek('10000-1-1'));
insert into test values('yearweek(''9999-12-31 23:59:59.9999995'')', yearweek('9999-12-31 23:59:59.9999995'));
insert into test values('yearweek(0)', yearweek(0));
insert into test values('yearweek(-1)', yearweek(-1));
insert into test values('yearweek(100000101)', yearweek(100000101));
insert into test values('yearweek(99991231235959.9999995)', yearweek(99991231235959.9999995));
insert into test values('yearweek(9999999999999999999999)', yearweek(9999999999999999999999));
insert into test values('yearweek(''0000-1-1 23:59:59.9999995'')', yearweek('0000-1-1 23:59:59.9999995'));
insert into test values('yearweek(''0000-12-31 22:59:59.9999995'')', yearweek('0000-12-31 22:59:59.9999995'));

-- 双参数-功能
insert into test values('yearweek(''2000-1-1'', 0)', yearweek('2000-1-1', 0));
insert into test values('yearweek(''2000-1-1'', 1)', yearweek('2000-1-1', 1));
insert into test values('yearweek(''2000-1-1'', 2)', yearweek('2000-1-1', 2));
insert into test values('yearweek(''2000-1-1'', 3)', yearweek('2000-1-1', 3));
insert into test values('yearweek(''2000-1-1'', 4)', yearweek('2000-1-1', 4));
insert into test values('yearweek(''2000-1-1'', 5)', yearweek('2000-1-1', 5));
insert into test values('yearweek(''2000-1-1'', 6)', yearweek('2000-1-1', 6));
insert into test values('yearweek(''2000-1-1'', 7)', yearweek('2000-1-1', 7));
insert into test values('yearweek(''0000-12-31 23:59:59.9999995'', 0)', yearweek('0000-12-31 23:59:59.9999995', 0));
-- 双参数-特异
insert into test values('yearweek(null, 0)', yearweek(null, 0));
insert into test values('yearweek(''2000-1-1'', null)', yearweek('2000-1-1', null));
insert into test values('yearweek(null, null)', yearweek(null, null));

-- 双参数-非严格模式，参数不合法，报warning，返回NULL或者对应值
set dolphin.sql_mode = 'sql_mode_full_group,pipes_as_concat,ansi_quotes,no_zero_date';
insert into test values('yearweek(''10000-1-1'', 0)', yearweek('10000-1-1', 0));
insert into test values('yearweek(''9999-12-31 23:59:59.9999995'', 0)', yearweek('9999-12-31 23:59:59.9999995', 0));
insert into test values('yearweek(0, 0)', yearweek(0, 0));
insert into test values('yearweek(-1, 0)', yearweek(-1, 0));
insert into test values('yearweek(100000101, 0)', yearweek(100000101, 0));
insert into test values('yearweek(99991231235959.9999995, 0)', yearweek(99991231235959.9999995, 0));
insert into test values('yearweek(9999999999999999999999, 0)', yearweek(9999999999999999999999, 0));
insert into test values('yearweek(''0000-1-1 23:59:59.9999995'', 0)', yearweek('0000-1-1 23:59:59.9999995', 0));
insert into test values('yearweek(''0000-12-31 22:59:59.9999995'', 0)', yearweek('0000-12-31 22:59:59.9999995', 0));

-- 双参数-严格模式，参数不合法，抛出错误
set dolphin.sql_mode = 'sql_mode_strict,sql_mode_full_group,pipes_as_concat,ansi_quotes,no_zero_date';
insert into test values('yearweek(''10000-1-1'', 0)', yearweek('10000-1-1', 0));
insert into test values('yearweek(''9999-12-31 23:59:59.9999995'', 0)', yearweek('9999-12-31 23:59:59.9999995', 0));
insert into test values('yearweek(0, 0)', yearweek(0, 0));
insert into test values('yearweek(-1, 0)', yearweek(-1, 0));
insert into test values('yearweek(100000101, 0)', yearweek(100000101, 0));
insert into test values('yearweek(99991231235959.9999995, 0)', yearweek(99991231235959.9999995, 0));
insert into test values('yearweek(9999999999999999999999, 0)', yearweek(9999999999999999999999, 0));
insert into test values('yearweek(''0000-1-1 23:59:59.9999995'', 0)', yearweek('0000-1-1 23:59:59.9999995', 0));
insert into test values('yearweek(''0000-12-31 22:59:59.9999995'', 0)', yearweek('0000-12-31 22:59:59.9999995', 0));

-- 结果
select * from test order by funcname;
drop table test;
\c contrib_regression
DROP DATABASE b_datetime_func_test2;
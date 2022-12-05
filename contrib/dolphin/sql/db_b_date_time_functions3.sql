---- b compatibility case
drop database if exists b_datetime_func_test;
create database b_datetime_func_test dbcompatibility 'b';
\c b_datetime_func_test
set datestyle = 'ISO,ymd';
set time zone "Asia/Shanghai";
create table test(funcname text, result text);
-- test datediff
-- 严格模式或者非严格模式都有值
-- 功能
insert into test values('datediff(''2021-11-12'', ''2021-11-13'')', datediff('2021-11-12','2021-11-13'));
insert into test values('datediff(''2021-01-01'', ''2021-12-31'')', datediff('2021-01-01','2021-12-31'));
insert into test values('datediff(''2021-12-31'', ''2022-01-01'')', datediff('2021-12-31','2022-01-01'));
insert into test values('datediff(''2021-12-31 12:12:12'', ''2022-01-01 10:10:10'')', datediff('2021-12-31 12:12:12','2022-01-01 10:10:10'));
-- 边界
insert into test values('datediff(''0000-01-01'', ''9999-12-31'')', datediff('0000-01-01', '9999-12-31'));
insert into test values('datediff(''9999-12-31'', ''0000-01-01'')', datediff('9999-12-31', '0000-01-01'));
-- 类型
insert into test values('datediff(date''2021-11-12'', date''2021-11-13'')', datediff(date'2021-11-13', date'2021-11-13'));
insert into test values('datediff(cast(''2021-12-31 12:12:12'' as datetime), cast(''2022-01-01 10:10:10'' as datetime))', datediff(cast('2021-12-31 12:12:12' as datetime), cast('2022-01-01 10:10:10' as datetime)));
insert into test values('datediff(20211112, 20211113)', datediff(20211112, 20211113));
insert into test values('datediff(time''10:10:10'', ''2022-11-30'')', datediff(time'10:10:10', '2022-11-30'));
insert into test values('datediff(''2022-11-30'', time''10:10:10'')', datediff('2022-11-30', time'10:10:10'));
insert into test values('datediff(time''10:10:10'', time''10:10:10'')', datediff(time'10:10:10', time'10:10:10'));
insert into test values('datediff(101, ''2021-11-12'')', datediff(101, '2021-11-12'));
insert into test values('datediff(''2021-11-12'', 101)', datediff('2021-11-12', 101));
insert into test values('datediff(''2021-11-12'', 101)', datediff('2021-11-12', 101));
-- 特异
insert into test values('datediff(null, ''2021-11-12'')', datediff(null, '2021-11-12'));
insert into test values('datediff(''2021-11-12'', null)', datediff('2021-11-12', null));
insert into test values('datediff(null, null)', datediff(null, null));

-- 非严格模式，参数不合法，报warning，返回NULL或者对应值
set dolphin.sql_mode = 'sql_mode_full_group,pipes_as_concat,ansi_quotes';
insert into test values('datediff(''10000-01-01'', ''2021-11-12'')', datediff('10000-01-01', '2021-11-12'));
insert into test values('datediff(100000101, 20211112)', datediff(100000101, 20211112));
insert into test values('datediff(99991231235959.9999995, 20211112)', datediff(99991231235959.9999995, 20211112));
insert into test values('datediff(9999999999999999999999, 20211112)', datediff(9999999999999999999999, 20211112));

-- 严格模式，参数不合法，抛出错误
set dolphin.sql_mode = 'sql_mode_strict,sql_mode_full_group,pipes_as_concat,ansi_quotes';
insert into test values('datediff(''10000-01-01'', ''2021-11-12'')', datediff('10000-01-01', '2021-11-12'));
insert into test values('datediff(100000101, 20211112)', datediff(100000101, 20211112));
insert into test values('datediff(99991231235959.9999995, 20211112)', datediff(99991231235959.9999995, 20211112));
insert into test values('datediff(9999999999999999999999, 20211112)', datediff(9999999999999999999999, 20211112));

-- test from_days
-- 严格模式或者非严格模式都有值
-- 功能
insert into test values('from_days(2000000)', from_days(2000000));
insert into test values('from_days(''2000000'')', from_days('2000000'));
-- 边界
insert into test values('from_days(366)', from_days(366));
insert into test values('from_days(3652499)', from_days(3652499));
insert into test values('from_days(''366'')', from_days('366'));
insert into test values('from_days(''3652499'')', from_days('3652499'));
-- 特异
insert into test values('from_days(null)', from_days(null));
insert into test values('from_days(-1)', from_days(-1));
insert into test values('from_days(0)', from_days(0));
insert into test values('from_days(365)', from_days(365));
insert into test values('from_days(1111111111111111111)', from_days(1111111111111111111));
insert into test values('from_days(''1111111111111111111'')', from_days('1111111111111111111'));

-- test timestampdiff
set dolphin.b_compatibility_mode = true;
-- 严格模式或者非严格模式都有值
-- 功能
insert into test values('timestampdiff(microsecond, ''0000-1-1 00:00:00'', ''0000-1-2'')', timestampdiff(microsecond, '0000-1-1 00:00:00', '0000-1-2'));
insert into test values('timestampdiff(second, ''0000-1-1 00:00:00'', ''0000-1-2'')', timestampdiff(second, '0000-1-1 00:00:00', '0000-1-2'));
insert into test values('timestampdiff(minute, ''0000-1-1 00:00:00'', ''0000-1-2'')', timestampdiff(minute, '0000-1-1 00:00:00', '0000-1-2'));
insert into test values('timestampdiff(hour, ''0000-1-1'', ''0000-1-2'')', timestampdiff(hour, '0000-1-1', '0000-1-2'));
insert into test values('timestampdiff(year, ''9999-12-31'', ''0000-12-31'')', timestampdiff(year, '9999-12-31', '0000-12-31'));
insert into test values('timestampdiff(quarter, ''9999-12-31'', ''0000-12-31'')', timestampdiff(quarter, '9999-12-31', '0000-12-31'));
insert into test values('timestampdiff(month, ''9999-12-31'', ''0000-12-31'')', timestampdiff(month, '9999-12-31', '0000-12-31'));
insert into test values('timestampdiff(week, ''9999-12-31'', ''0000-12-31'')', timestampdiff(week, '9999-12-31', '0000-12-31'));
insert into test values('timestampdiff(day, ''9999-12-31'', ''0000-12-31'')', timestampdiff(day, '9999-12-31', '0000-12-31'));
insert into test values('timestampdiff(microsecond, time''-838:59:59'', time''838:59:59'')', timestampdiff(microsecond, time'-838:59:59', time'838:59:59'));
insert into test values('timestampdiff(second, time''-838:59:59'', time''838:59:59'')', timestampdiff(second, time'-838:59:59', time'838:59:59'));
insert into test values('timestampdiff(minute, time''-838:59:59'', time''838:59:59'')', timestampdiff(minute, time'-838:59:59', time'838:59:59'));
insert into test values('timestampdiff(hour, time''-838:59:59'', time''838:59:59'')', timestampdiff(hour, time'-838:59:59', time'838:59:59'));
insert into test values('timestampdiff(day, time''-838:59:59'', time''838:59:59'')', timestampdiff(day, time'-838:59:59', time'838:59:59'));
insert into test values('timestampdiff(month, time''-838:59:59'', time''838:59:59'')', timestampdiff(month, time'-838:59:59', time'838:59:59'));
insert into test values('timestampdiff(quarter, time''-838:59:59'', time''838:59:59'')', timestampdiff(quarter, time'-838:59:59', time'838:59:59'));
insert into test values('timestampdiff(year, time''-838:59:59'', time''838:59:59'')', timestampdiff(year, time'-838:59:59', time'838:59:59'));
insert into test values('timestampdiff(week, time''-838:59:59'', time''838:59:59'')', timestampdiff(week, time'-838:59:59', time'838:59:59'));
insert into test values('timestampdiff(microsecond, ''2022-9-12'', time''12:12:12'')', timestampdiff(microsecond, '2022-9-12', time'12:12:12'));
insert into test values('timestampdiff(microsecond, time''12:12:12'', ''2022-9-12'')', timestampdiff(microsecond, time'12:12:12', '2022-9-12'));
insert into test values('timestampdiff(microsecond, ''2022-9-12 12:12:12'', time''838:59:59'')', timestampdiff(microsecond, '2022-9-12 12:12:12', time'838:59:59'));
-- 边界
insert into test values('timestampdiff(microsecond, ''0000-1-1'', ''9999-12-31'')', timestampdiff(microsecond, '0000-1-1', '9999-12-31'));
insert into test values('timestampdiff(second, ''0000-1-1'', ''9999-12-31'')', timestampdiff(second, '0000-1-1', '9999-12-31'));
insert into test values('timestampdiff(minute, ''0000-1-1'', ''9999-12-31'')', timestampdiff(minute, '0000-1-1', '9999-12-31'));
insert into test values('timestampdiff(hour, ''0000-1-1'', ''9999-12-31'')', timestampdiff(hour, '9999-12-31', '0000-12-31'));
insert into test values('timestampdiff(day, ''0000-1-1'', ''9999-12-31'')', timestampdiff(day, '0000-1-1', '9999-12-31'));
insert into test values('timestampdiff(year, ''0000-1-1'', ''9999-12-31'')', timestampdiff(year, '0000-1-1', '9999-12-31'));
insert into test values('timestampdiff(quarter, ''0000-1-1'', ''9999-12-31'')', timestampdiff(quarter, '0000-1-1', '9999-12-31'));
insert into test values('timestampdiff(month, ''0000-1-1'', ''9999-12-31'')', timestampdiff(month, '0000-1-1', '9999-12-31'));
insert into test values('timestampdiff(week, ''0000-1-1'', ''9999-12-31'')', timestampdiff(week, '0000-1-1', '9999-12-31'));
-- 类型
insert into test values('timestampdiff(microsecond, cast(''2022-9-12 12:12:12.999999'' as datetime), time''12:12:12.999999'')', timestampdiff(microsecond, cast('2022-9-12 12:12:12.999999' as datetime), time'12:12:12.999999'));
insert into test values('timestampdiff(microsecond, time''12:12:12.999999'', cast(''2022-9-12 12:12:12.999999'' as datetime))', timestampdiff(microsecond, time'12:12:12.999999', cast('2022-9-12 12:12:12.999999' as datetime)));
insert into test values('timestampdiff(microsecond, date''2022-9-12'', time''12:12:12.999999'')',timestampdiff(microsecond, date'2022-9-12', time'12:12:12.999999'));
insert into test values('timestampdiff(microsecond, time''12:12:12.999999'', date''2022-9-12'')', timestampdiff(microsecond, time'12:12:12.999999', date'2022-9-12'));
insert into test values('timestampdiff(microsecond, ''20211112121212.999999'', ''20220912111111.999999'')', timestampdiff(microsecond, '20211112121212.999999', '20220912111111.999999'));
insert into test values('timestampdiff(microsecond, 101, ''20211112121212.999999'')', timestampdiff(microsecond, 101, '20211112121212.999999'));
insert into test values('timestampdiff(microsecond, ''20211112121212.999999'', 101)', timestampdiff(microsecond, '20211112121212.999999', 101));
insert into test values('timestampdiff(microsecond, 99991231235959.999999, 101)', timestampdiff(microsecond, 99991231235959.999999, 101));
insert into test values('timestampdiff(microsecond, 101, 99991231235959.999999)', timestampdiff(microsecond, 101, 99991231235959.999999));
-- 特异
insert into test values('timestampdiff(week, null, ''9999-12-31'')', timestampdiff(week, null, '9999-12-31'));
insert into test values('timestampdiff(week, ''9999-12-31'', null)', timestampdiff(week, '9999-12-31', null));
insert into test values('timestampdiff(week, null, null)', timestampdiff(week, null, null));

-- 非严格模式，参数不合法，报warning，返回NULL或者对应值
set dolphin.sql_mode = 'sql_mode_full_group,pipes_as_concat,ansi_quotes';
insert into test values('timestampdiff(microsecond, ''abc'', ''2021-11-12'')', timestampdiff(microsecond, 'abc', '2021-11-12'));
insert into test values('timestampdiff(microsecond, ''2021-11-12'', ''10000-1-1'')', timestampdiff(microsecond, '2021-11-12', '10000-1-1'));
insert into test values('timestampdiff(microsecond, ''10000-1-1'', ''2021-11-12'')', timestampdiff(microsecond, '10000-1-1', '2021-11-12'));

-- 严格模式，参数不合法，抛出错误
set dolphin.sql_mode = 'sql_mode_strict,sql_mode_full_group,pipes_as_concat,ansi_quotes';
insert into test values('timestampdiff(microsecond, ''abc'', ''2021-11-12'')', timestampdiff(microsecond, 'abc', '2021-11-12'));
insert into test values('timestampdiff(microsecond, ''2021-11-12'', ''10000-1-1'')', timestampdiff(microsecond, '2021-11-12', '10000-1-1'));
insert into test values('timestampdiff(microsecond, ''10000-1-1'', ''2021-11-12'')', timestampdiff(microsecond, '10000-1-1', '2021-11-12'));
set dolphin.b_compatibility_mode = false;

-- test convert_tz
-- 严格模式或者非严格模式都有值
-- 双字母,第一次转换超[1970-1-1 00:00:01.000000, 2038-01-19 03:14:07.999999]，结果不转换
insert into test values('convert_tz(''1970-1-1 00:00:01.000000'', ''MET'', ''GMT'')', convert_tz('1970-1-1 00:00:01.000000', 'MET', 'GMT'));
insert into test values('convert_tz(''1970-1-1 00:00:01.000000'', ''CCT'', ''GMT'')', convert_tz('1970-1-1 00:00:01.000000', 'CCT', 'GMT'));
insert into test values('convert_tz(''2038-01-19 03:14:07.999999'', ''EDT'', ''GMT'')', convert_tz('2038-01-19 03:14:07.999999', 'EDT', 'GMT'));
-- 双字母正常转换
insert into test values('convert_tz(''1234-1-1 00:00:00'', ''MET'', ''GMT'')', convert_tz('1234-1-1 00:00:00', 'MET', 'GMT'));
insert into test values('convert_tz(''1970-1-1 00:00:01.000000'', ''GMT'', ''GMT'')', convert_tz('1970-1-1 00:00:01.000000', 'GMT', 'GMT'));
insert into test values('convert_tz(''1970-1-1 00:00:01.000000'', ''EDT'', ''GMT'')', convert_tz('1970-1-1 00:00:01.000000', 'EDT', 'GMT'));
insert into test values('convert_tz(''1970-1-1 00:00:01.000000'', ''CDT'', ''GMT'')', convert_tz('1970-1-1 00:00:01.000000', 'CDT', 'GMT'));
insert into test values('convert_tz(''1970-1-1 00:00:01.000000'', ''EST'', ''GMT'')', convert_tz('1970-1-1 00:00:01.000000', 'EST', 'GMT'));
insert into test values('convert_tz(''1970-1-1 00:00:01.000000'', ''MDT'', ''GMT'')', convert_tz('1970-1-1 00:00:01.000000', 'MDT', 'GMT'));
insert into test values('convert_tz(''1970-1-1 00:00:01.000000'', ''MST'', ''GMT'')', convert_tz('1970-1-1 00:00:01.000000', 'MST', 'GMT'));
insert into test values('convert_tz(''2038-01-19 03:14:07.999999'', ''JST'', ''GMT'')',convert_tz('2038-01-19 03:14:07.999999', 'JST', 'GMT'));
insert into test values('convert_tz(''2038-01-19 03:14:07.999999'', ''KST'', ''GMT'')',convert_tz('2038-01-19 03:14:07.999999', 'KST', 'GMT'));
insert into test values('convert_tz(''2038-01-19 03:14:07.999999'', ''CCT'', ''GMT'')',convert_tz('2038-01-19 03:14:07.999999', 'CCT', 'GMT'));
-- 双数字,第一次转换超[1970-1-1 00:00:01.000000, 2038-01-19 03:14:07.999999]，结果不转换
insert into test values('convert_tz(''1970-1-1 00:00:01.000000'', ''+1:00'', ''+0:00'')', convert_tz('1970-1-1 00:00:01.000000', '+1:00', '+0:00'));
insert into test values('convert_tz(''2038-01-19 03:14:07.999999'', ''-1:00'', ''+0:00'')', convert_tz('1970-1-1 00:00:01.000000', '+1:00', '+0:00'));
-- 双数字正常转换
insert into test values('convert_tz(''1970-1-1 00:00:01.000000'', ''-1:00'', ''+0:00'')', convert_tz('1970-1-1 00:00:01.000000', '-1:00', '+0:00'));
insert into test values('convert_tz(''1970-1-1 00:00:01.000000'', ''-2:00'', ''+0:00'')', convert_tz('1970-1-1 00:00:01.000000', '-2:00', '+0:00'));
insert into test values('convert_tz(''1970-1-1 00:00:01.000000'', ''-10:00'', ''+0:00'')', convert_tz('1970-1-1 00:00:01.000000', '-10:00', '+0:00'));
insert into test values('convert_tz(''2038-01-19 03:14:07.999999'', ''+1:00'', ''+0:00'')', convert_tz('2038-01-19 03:14:07.999999', '+1:00', '+0:00'));
insert into test values('convert_tz(''2038-01-19 03:14:07.999999'', ''+2:00'', ''+0:00'')', convert_tz('2038-01-19 03:14:07.999999', '+2:00', '+0:00'));
insert into test values('convert_tz(''2038-01-19 03:14:07.999999'', ''+10:00'', ''+0:00'')', convert_tz('2038-01-19 03:14:07.999999', '+10:00', '+0:00'));
-- 字母数字混合
insert into test values('convert_tz(''1970-1-1 00:00:01.000000'', ''-1:00'', ''GMT'')', convert_tz('1970-1-1 00:00:01.000000', '-1:00', 'GMT'));
insert into test values('convert_tz(''1970-1-1 00:00:01.000000'', ''GMT'', ''+3:00'')', convert_tz('1970-1-1 00:00:01.000000', 'GMT', '+3:00'));
insert into test values('convert_tz(''2038-01-19 03:14:07.999999'', ''+2:00'', ''GMT'')', convert_tz('2038-01-19 03:14:07.999999', '+2:00', 'GMT'));
insert into test values('convert_tz(''2038-01-19 03:14:07.999999'', ''GMT'', ''+7:00'')', convert_tz('2038-01-19 03:14:07.999999', 'GMT', '+7:00'));
insert into test values('convert_tz(time''10:10:10'', ''GMT'', ''+7:00'')', convert_tz(time'10:10:10', 'GMT', '+7:00'));
insert into test values('convert_tz(101, ''GMT'', ''+7:00'')', convert_tz(101, 'GMT', '+7:00'));
-- 特异
insert into test values('convert_tz(null, ''+1:00'', ''+0:00'')', convert_tz(null, '+1:00', '+0:00'));
insert into test values('convert_tz(''2021-11-12 12:12:12'', null, ''+0:00'')', convert_tz('2021-11-12 12:12:12', null, '+0:00'));
insert into test values('convert_tz(''2021-11-12 12:12:12'', ''+1:00'', null)', convert_tz('2021-11-12 12:12:12', '+1:00', null));
insert into test values('convert_tz(''2021-11-12 12:12:12'', ''abc'', ''+0:00'')', convert_tz('2021-11-12 12:12:12', 'abc', '+0:00'));
insert into test values('convert_tz(''2021-11-12 12:12:12'', ''+1:00'', ''abc'')', convert_tz('2021-11-12 12:12:12', '+1:00', 'abc'));
insert into test values('convert_tz(''2021-11-12 12:12:12'', ''+13:01'', ''+0:00'')', convert_tz('2021-11-12 12:12:12', '+13:01', '+0:00'));
insert into test values('convert_tz(''2021-11-12 12:12:12'', ''+0:00'', ''-13:00'')', convert_tz('2021-11-12 12:12:12', '+0:00', '-13:00'));
-- 非严格模式，参数不合法，报warning，返回NULL或者对应值
set dolphin.sql_mode = 'sql_mode_full_group,pipes_as_concat,ansi_quotes';
insert into test values('convert_tz(''10000-1-1 10:10:10'', ''+1:00'', ''+0:00'')', convert_tz('10000-1-1 10:10:10', '+1:00', '+0:00'));

-- 严格模式，参数不合法，抛出错误
set dolphin.sql_mode = 'sql_mode_strict,sql_mode_full_group,pipes_as_concat,ansi_quotes';
insert into test values('convert_tz(''10000-1-1 10:10:10'', ''+1:00'', ''+0:00'')', convert_tz('10000-1-1 10:10:10', '+1:00', '+0:00'));


-- test adddate(date/datetime/time, INTERVAL/days)
-- 严格模式或者非严格模式都有值
-- 功能-unit
insert into test values('adddate(''0001-01-01'',INTERVAL 1 year)', adddate('0001-01-01',INTERVAL 1 year));
insert into test values('adddate(''0001-01-01'',INTERVAL 1 day)', adddate('0001-01-01',INTERVAL 1 day));
insert into test values('adddate(''0001-01-01'',INTERVAL 1 month)', adddate('0001-01-01',INTERVAL 1 month));
insert into test values('adddate(''0001-01-01'',INTERVAL 1 hour)', adddate('0001-01-01',INTERVAL 1 hour));
insert into test values('adddate(''0001-01-01'',INTERVAL 1 minute)', adddate('0001-01-01',INTERVAL 1 minute));
insert into test values('adddate(''0001-01-01'',INTERVAL 1 second)', adddate('0001-01-01',INTERVAL 1 second));
insert into test values('adddate(''0001-01-01 12:12:12'',INTERVAL 1 hour)', adddate('0001-01-01 12:12:12',INTERVAL 1 hour));
insert into test values('adddate(''0001-01-01 12:12:12'',INTERVAL 1 minute)', adddate('0001-01-01 12:12:12',INTERVAL 1 minute));
insert into test values('adddate(''0001-01-01 12:12:12'',INTERVAL 1 second)', adddate('0001-01-01 12:12:12',INTERVAL 1 second));
insert into test values('adddate(''9999-12-31'',INTERVAL -1 year)', adddate('9999-12-31',INTERVAL -1 year));
insert into test values('adddate(''9999-12-31'',INTERVAL -1 day)', adddate('9999-12-31',INTERVAL -1 day));
insert into test values('adddate(''9999-12-31'',INTERVAL -1 month)', adddate('9999-12-31',INTERVAL -1 month));
insert into test values('adddate(''9999-12-31'',INTERVAL -1 hour)', adddate('9999-12-31',INTERVAL -1 hour));
insert into test values('adddate(''9999-12-31'',INTERVAL -1 minute)', adddate('9999-12-31',INTERVAL -1 minute));
insert into test values('adddate(''9999-12-31'',INTERVAL -1 second)', adddate('9999-12-31',INTERVAL -1 second));
insert into test values('adddate(''9999-12-31 12:12:12'',INTERVAL -1 hour)', adddate('9999-12-31 12:12:12',INTERVAL -1 hour));
insert into test values('adddate(''9999-12-31 12:12:12'',INTERVAL -1 minute)', adddate('9999-12-31 12:12:12',INTERVAL -1 minute));
insert into test values('adddate(''9999-12-31 12:12:12'',INTERVAL -1 second)', adddate('9999-12-31 12:12:12',INTERVAL -1 second));
insert into test values('adddate(''2021-11-12'', -1)', adddate('2021-11-12', -1));
insert into test values('adddate(''2021-11-12'', 1)', adddate('2021-11-12', 1));
-- 功能-一些进位情况
insert into test values('adddate(''0001-12-31'',INTERVAL 1 day)',adddate('0001-12-31',INTERVAL 1 day));
insert into test values('adddate(''2021-12-31'',INTERVAL 1 day)',adddate('2021-12-31',INTERVAL 1 day));
insert into test values('adddate(''9998-12-31'',INTERVAL 1 day)',adddate('9998-12-31',INTERVAL 1 day));
insert into test values('adddate(''0001-12-31'',INTERVAL 1 month)',adddate('0001-12-31',INTERVAL 1 month));
insert into test values('adddate(''1999-12-31'',INTERVAL 1 day)',adddate('1999-12-31',INTERVAL 1 day));
insert into test values('adddate(''1999-12-31'',INTERVAL 1 year)',adddate('1999-12-31',INTERVAL 1 year));
insert into test values('adddate(''1999-12-31 23:59:59'',INTERVAL 1 second)',adddate('1999-12-31 23:59:59',INTERVAL 1 second));
insert into test values('adddate(''1999-12-31 23:59:00'',INTERVAL 1 minute)',adddate('1999-12-31 23:59:00',INTERVAL 1 minute));
insert into test values('adddate(''1999-12-31 23:00:00'',INTERVAL 1 hour)',adddate('1999-12-31 23:00:00',INTERVAL 1 hour));
-- 功能-第一参数为显示time类型
insert into test values('adddate(time''12:12:12'', INTERVAL 1 day)', adddate(time'12:12:12', INTERVAL 1 day));
insert into test values('adddate(time''12:12:12'', INTERVAL 1 hour)', adddate(time'12:12:12', INTERVAL 1 hour));
insert into test values('adddate(time''12:12:12'', INTERVAL 1 minute)', adddate(time'12:12:12', INTERVAL 1 minute));
insert into test values('adddate(time''12:12:12'', INTERVAL 1 second)', adddate(time'12:12:12', INTERVAL 1 second));
insert into test values('adddate(time''12:12:12'', INTERVAL -1 day)', adddate(time'12:12:12', INTERVAL -1 day));
insert into test values('adddate(time''12:12:12'', INTERVAL -1 hour)', adddate(time'12:12:12', INTERVAL -1 hour));
insert into test values('adddate(time''12:12:12'', INTERVAL -1 minute)', adddate(time'12:12:12', INTERVAL -1 minute));
insert into test values('adddate(time''12:12:12'', INTERVAL -1 second)', adddate(time'12:12:12', INTERVAL -1 second));
insert into test values('adddate(time''12:12:12'', 1)', adddate(time'12:12:12', 1));
insert into test values('adddate(time''12:12:12'', -1)', adddate(time'12:12:12', -1));
-- 类型
insert into test values('adddate(date''0001-01-01'',INTERVAL 1 year)', adddate(date'0001-01-01',INTERVAL 1 year));
insert into test values('adddate(cast(''9999-12-31 12:12:12'' as datetime),INTERVAL -1 second)', adddate(cast('9999-12-31 12:12:12' as datetime), INTERVAL -1 second));
insert into test values('adddate(time''838:59:59'', INTERVAL -1 second)', adddate(time'838:59:59',INTERVAL -1 second));
insert into test values('adddate(101, INTERVAL 1 year)', adddate(101, INTERVAL 1 year));
insert into test values('adddate(101, INTERVAL 100 second)', adddate(101, INTERVAL 100 second));
insert into test values('adddate(101, 1)', adddate(101, 1));
-- 特异
insert into test values('adddate(null, INTERVAL 1 day)', adddate(null, INTERVAL 1 day));
insert into test values('adddate(''2021-11-12'', null)', adddate('2021-11-12', null));
insert into test values('adddate(null, null)', adddate(null, null));

-- 非严格模式，参数或者结果不合法，报warning，返回NULL或者对应值
set dolphin.sql_mode = 'sql_mode_full_group,pipes_as_concat,ansi_quotes';
insert into test values('adddate(0, INTERVAL 100 second)', adddate(0, INTERVAL 100 second));
insert into test values('adddate(''10000-1-1'', INTERVAL -1 day)', adddate('10000-1-1', INTERVAL -1 day));
insert into test values('adddate(''9999-12-31'', INTERVAL 1 day)', adddate('9999-12-31', INTERVAL 1 day));
insert into test values('adddate(''9999-12-31'', INTERVAL 1 month)', adddate('9999-12-31', INTERVAL 1 month));
insert into test values('adddate(''9999-12-31'', INTERVAL 1 year)', adddate('9999-12-31', INTERVAL 1 year));
insert into test values('adddate(''9999-12-31 23:59:59'', INTERVAL 1 second)', adddate('9999-12-31 23:59:59', INTERVAL 1 second));
insert into test values('adddate(''9999-12-31 23:59:00'', INTERVAL 1 minute)', adddate('9999-12-31 23:59:00', INTERVAL 1 minute));
insert into test values('adddate(''9999-12-31 23:00:00'', INTERVAL 1 hour)', adddate('9999-12-31 23:00:00', INTERVAL 1 hour));
insert into test values('adddate(time''838:59:59'', INTERVAL 1 second)', adddate(time'838:59:59', INTERVAL 1 second));
insert into test values('adddate(time''-838:59:59'', INTERVAL -1 second)', adddate(time'-838:59:59', INTERVAL -1 second));
-- 结果小于'0001-1-1 00:00:00'时，MSQ日期部分总为'0000-00-00'，openGauss中非严格模式下返回NULL
insert into test values('adddate(''0001-1-1'', INTERVAL -1 day)', adddate('0001-1-1', INTERVAL -1 second));
insert into test values('adddate(''0001-1-1 00:00:00'', INTERVAL -1 second)', adddate('0001-1-1', INTERVAL -1 second));

-- 严格模式，参数不合法，抛出错误
set dolphin.sql_mode = 'sql_mode_strict,sql_mode_full_group,pipes_as_concat,ansi_quotes';
insert into test values('adddate(0, INTERVAL 100 second)', adddate(0, INTERVAL 100 second));
insert into test values('adddate(''10000-1-1'', INTERVAL -1 day)', adddate('10000-1-1', INTERVAL -1 day));
insert into test values('adddate(''9999-12-31'', INTERVAL 1 day)', adddate('9999-12-31', INTERVAL 1 day));
insert into test values('adddate(''9999-12-31'', INTERVAL 1 month)', adddate('9999-12-31', INTERVAL 1 month));
insert into test values('adddate(''9999-12-31'', INTERVAL 1 year)', adddate('9999-12-31', INTERVAL 1 year));
insert into test values('adddate(''9999-12-31 23:59:59'', INTERVAL 1 second)', adddate('9999-12-31 23:59:59', INTERVAL 1 second));
insert into test values('adddate(''9999-12-31 23:59:00'', INTERVAL 1 minute)', adddate('9999-12-31 23:59:00', INTERVAL 1 minute));
insert into test values('adddate(''9999-12-31 23:00:00'', INTERVAL 1 hour)', adddate('9999-12-31 23:00:00', INTERVAL 1 hour));
insert into test values('adddate(time''838:59:59'', INTERVAL 1 second)', adddate(time'838:59:59', INTERVAL 1 second));
insert into test values('adddate(time''-838:59:59'', INTERVAL -1 second)', adddate(time'-838:59:59', INTERVAL -1 second));
-- 结果小于'0001-1-1 00:00:00'时，MSQ日期部分总为'0000-00-00'，openGauss中严格模式下抛出错误。
insert into test values('adddate(''0001-1-1'', INTERVAL -1 day)', adddate('0001-1-1', INTERVAL -1 day));
insert into test values('adddate(''0001-1-1 00:00:00'', INTERVAL -1 second)', adddate('0001-1-1 00:00:00', INTERVAL -1 second));

-- test date_add(date/datetime/time, INTERVAL)
-- 严格模式或者非严格模式都有值
-- 功能-unit
insert into test values('date_add(''0001-01-01'',INTERVAL 1 year)', date_add('0001-01-01',INTERVAL 1 year));
insert into test values('date_add(''0001-01-01'',INTERVAL 1 day)', date_add('0001-01-01',INTERVAL 1 day));
insert into test values('date_add(''0001-01-01'',INTERVAL 1 month)', date_add('0001-01-01',INTERVAL 1 month));
insert into test values('date_add(''0001-01-01'',INTERVAL 1 hour)', date_add('0001-01-01',INTERVAL 1 hour));
insert into test values('date_add(''0001-01-01'',INTERVAL 1 minute)', date_add('0001-01-01',INTERVAL 1 minute));
insert into test values('date_add(''0001-01-01'',INTERVAL 1 second)', date_add('0001-01-01',INTERVAL 1 second));
insert into test values('date_add(''0001-01-01 12:12:12'',INTERVAL 1 hour)', date_add('0001-01-01 12:12:12',INTERVAL 1 hour));
insert into test values('date_add(''0001-01-01 12:12:12'',INTERVAL 1 minute)', date_add('0001-01-01 12:12:12',INTERVAL 1 minute));
insert into test values('date_add(''0001-01-01 12:12:12'',INTERVAL 1 second)', date_add('0001-01-01 12:12:12',INTERVAL 1 second));
insert into test values('date_add(''9999-12-31'',INTERVAL -1 year)', date_add('9999-12-31',INTERVAL -1 year));
insert into test values('date_add(''9999-12-31'',INTERVAL -1 day)', date_add('9999-12-31',INTERVAL -1 day));
insert into test values('date_add(''9999-12-31'',INTERVAL -1 month)', date_add('9999-12-31',INTERVAL -1 month));
insert into test values('date_add(''9999-12-31'',INTERVAL -1 hour)', date_add('9999-12-31',INTERVAL -1 hour));
insert into test values('date_add(''9999-12-31'',INTERVAL -1 minute)', date_add('9999-12-31',INTERVAL -1 minute));
insert into test values('date_add(''9999-12-31'',INTERVAL -1 second)', date_add('9999-12-31',INTERVAL -1 second));
insert into test values('date_add(''9999-12-31 12:12:12'',INTERVAL -1 hour)', date_add('9999-12-31 12:12:12',INTERVAL -1 hour));
insert into test values('date_add(''9999-12-31 12:12:12'',INTERVAL -1 minute)', date_add('9999-12-31 12:12:12',INTERVAL -1 minute));
insert into test values('date_add(''9999-12-31 12:12:12'',INTERVAL -1 second)', date_add('9999-12-31 12:12:12',INTERVAL -1 second));
-- 功能-一些进位情况
insert into test values('date_add(''0001-12-31'',INTERVAL 1 day)',date_add('0001-12-31',INTERVAL 1 day));
insert into test values('date_add(''2021-12-31'',INTERVAL 1 day)',date_add('2021-12-31',INTERVAL 1 day));
insert into test values('date_add(''9998-12-31'',INTERVAL 1 day)',date_add('9998-12-31',INTERVAL 1 day));
insert into test values('date_add(''0001-12-31'',INTERVAL 1 month)',date_add('0001-12-31',INTERVAL 1 month));
insert into test values('date_add(''1999-12-31'',INTERVAL 1 day)',date_add('1999-12-31',INTERVAL 1 day));
insert into test values('date_add(''1999-12-31'',INTERVAL 1 year)',date_add('1999-12-31',INTERVAL 1 year));
insert into test values('date_add(''1999-12-31 23:59:59'',INTERVAL 1 second)',date_add('1999-12-31 23:59:59',INTERVAL 1 second));
insert into test values('date_add(''1999-12-31 23:59:00'',INTERVAL 1 minute)',date_add('1999-12-31 23:59:00',INTERVAL 1 minute));
insert into test values('date_add(''1999-12-31 23:00:00'',INTERVAL 1 hour)',date_add('1999-12-31 23:00:00',INTERVAL 1 hour));
-- 功能-第一参数为显示time类型
insert into test values('date_add(time''12:12:12'', INTERVAL 1 day)', date_add(time'12:12:12', INTERVAL 1 day));
insert into test values('date_add(time''12:12:12'', INTERVAL 1 hour)', date_add(time'12:12:12', INTERVAL 1 hour));
insert into test values('date_add(time''12:12:12'', INTERVAL 1 minute)', date_add(time'12:12:12', INTERVAL 1 minute));
insert into test values('date_add(time''12:12:12'', INTERVAL 1 second)', date_add(time'12:12:12', INTERVAL 1 second));
insert into test values('date_add(time''12:12:12'', INTERVAL -1 day)', date_add(time'12:12:12', INTERVAL -1 day));
insert into test values('date_add(time''12:12:12'', INTERVAL -1 hour)', date_add(time'12:12:12', INTERVAL -1 hour));
insert into test values('date_add(time''12:12:12'', INTERVAL -1 minute)', date_add(time'12:12:12', INTERVAL -1 minute));
insert into test values('date_add(time''12:12:12'', INTERVAL -1 second)', date_add(time'12:12:12', INTERVAL -1 second));
-- 类型
insert into test values('date_add(date''0001-01-01'',INTERVAL 1 year)', date_add(date'0001-01-01',INTERVAL 1 year));
insert into test values('date_add(cast(''9999-12-31 12:12:12'' as datetime),INTERVAL -1 second)', date_add(cast('9999-12-31 12:12:12' as datetime), INTERVAL -1 second));
insert into test values('date_add(time''838:59:59'', INTERVAL -1 second)', date_add(time'838:59:59',INTERVAL -1 second));
insert into test values('date_add(101, INTERVAL 1 year)', date_add(101, INTERVAL 1 year));
insert into test values('date_add(101, INTERVAL 100 second)', date_add(101, INTERVAL 100 second));
-- 特异
insert into test values('date_add(null, INTERVAL 1 day)', date_add(null, INTERVAL 1 day));

-- 非严格模式，参数或者结果不合法，报warning，返回NULL或者对应值
set dolphin.sql_mode = 'sql_mode_full_group,pipes_as_concat,ansi_quotes';
insert into test values('date_add(0, INTERVAL 100 second)', date_add(0, INTERVAL 100 second));
insert into test values('date_add(''10000-1-1'', INTERVAL -1 day)', date_add('10000-1-1', INTERVAL -1 day));
insert into test values('date_add(''9999-12-31'', INTERVAL 1 day)', date_add('9999-12-31', INTERVAL 1 day));
insert into test values('date_add(''9999-12-31'', INTERVAL 1 month)', date_add('9999-12-31', INTERVAL 1 month));
insert into test values('date_add(''9999-12-31'', INTERVAL 1 year)', date_add('9999-12-31', INTERVAL 1 year));
insert into test values('date_add(''9999-12-31 23:59:59'', INTERVAL 1 second)', date_add('9999-12-31 23:59:59', INTERVAL 1 second));
insert into test values('date_add(''9999-12-31 23:59:00'', INTERVAL 1 minute)', date_add('9999-12-31 23:59:00', INTERVAL 1 minute));
insert into test values('date_add(''9999-12-31 23:00:00'', INTERVAL 1 hour)', date_add('9999-12-31 23:00:00', INTERVAL 1 hour));
insert into test values('date_add(time''838:59:59'', INTERVAL 1 second)', date_add(time'838:59:59', INTERVAL 1 second));
insert into test values('date_add(time''-838:59:59'', INTERVAL -1 second)', date_add(time'-838:59:59', INTERVAL -1 second));
-- 结果小于'0001-1-1 00:00:00'时，MSQ日期部分总为'0000-00-00'，openGauss中非严格模式下返回NULL
insert into test values('date_add(''0001-1-1'', INTERVAL -1 day)', date_add('0001-1-1', INTERVAL -1 second));
insert into test values('date_add(''0001-1-1 00:00:00'', INTERVAL -1 second)', date_add('0001-1-1', INTERVAL -1 second));

-- 严格模式，参数不合法，抛出错误
set dolphin.sql_mode = 'sql_mode_strict,sql_mode_full_group,pipes_as_concat,ansi_quotes';
insert into test values('date_add(0, INTERVAL 100 second)', date_add(0, INTERVAL 100 second));
insert into test values('date_add(''10000-1-1'', INTERVAL -1 day)', date_add('10000-1-1', INTERVAL -1 day));
insert into test values('date_add(''9999-12-31'', INTERVAL 1 day)', date_add('9999-12-31', INTERVAL 1 day));
insert into test values('date_add(''9999-12-31'', INTERVAL 1 month)', date_add('9999-12-31', INTERVAL 1 month));
insert into test values('date_add(''9999-12-31'', INTERVAL 1 year)', date_add('9999-12-31', INTERVAL 1 year));
insert into test values('date_add(''9999-12-31 23:59:59'', INTERVAL 1 second)', date_add('9999-12-31 23:59:59', INTERVAL 1 second));
insert into test values('date_add(''9999-12-31 23:59:00'', INTERVAL 1 minute)', date_add('9999-12-31 23:59:00', INTERVAL 1 minute));
insert into test values('date_add(''9999-12-31 23:00:00'', INTERVAL 1 hour)', date_add('9999-12-31 23:00:00', INTERVAL 1 hour));
insert into test values('date_add(time''838:59:59'', INTERVAL 1 second)', date_add(time'838:59:59', INTERVAL 1 second));
insert into test values('date_add(time''-838:59:59'', INTERVAL -1 second)', date_add(time'-838:59:59', INTERVAL -1 second));
-- 结果小于'0001-1-1 00:00:00'时，MSQ日期部分总为'0000-00-00'，openGauss中严格模式下抛出错误。
insert into test values('date_add(''0001-1-1'', INTERVAL -1 day)', date_add('0001-1-1', INTERVAL -1 day));
insert into test values('date_add(''0001-1-1 00:00:00'', INTERVAL -1 second)', date_add('0001-1-1 00:00:00', INTERVAL -1 second));


-- test date_sub(date/datetime/time, INTERVAL)
-- 严格模式或者非严格模式都有值
-- 功能-unit
insert into test values('date_sub(''0001-01-01'',INTERVAL -1 year)', date_sub('0001-01-01',INTERVAL -1 year));
insert into test values('date_sub(''0001-01-01'',INTERVAL -1 day)', date_sub('0001-01-01',INTERVAL -1 day));
insert into test values('date_sub(''0001-01-01'',INTERVAL -1 month)', date_sub('0001-01-01',INTERVAL -1 month));
insert into test values('date_sub(''0001-01-01'',INTERVAL -1 hour)', date_sub('0001-01-01',INTERVAL -1 hour));
insert into test values('date_sub(''0001-01-01'',INTERVAL -1 minute)', date_sub('0001-01-01',INTERVAL -1 minute));
insert into test values('date_sub(''0001-01-01'',INTERVAL -1 second)', date_sub('0001-01-01',INTERVAL -1 second));
insert into test values('date_sub(''0001-01-01 12:12:12'',INTERVAL -1 hour)', date_sub('0001-01-01 12:12:12',INTERVAL -1 hour));
insert into test values('date_sub(''0001-01-01 12:12:12'',INTERVAL -1 minute)', date_sub('0001-01-01 12:12:12',INTERVAL -1 minute));
insert into test values('date_sub(''0001-01-01 12:12:12'',INTERVAL -1 second)', date_sub('0001-01-01 12:12:12',INTERVAL -1 second));
insert into test values('date_sub(''9999-12-31'',INTERVAL 1 year)', date_sub('9999-12-31',INTERVAL 1 year));
insert into test values('date_sub(''9999-12-31'',INTERVAL 1 day)', date_sub('9999-12-31',INTERVAL 1 day));
insert into test values('date_sub(''9999-12-31'',INTERVAL 1 month)', date_sub('9999-12-31',INTERVAL 1 month));
insert into test values('date_sub(''9999-12-31'',INTERVAL 1 hour)', date_sub('9999-12-31',INTERVAL 1 hour));
insert into test values('date_sub(''9999-12-31'',INTERVAL 1 minute)', date_sub('9999-12-31',INTERVAL 1 minute));
insert into test values('date_sub(''9999-12-31'',INTERVAL 1 second)', date_sub('9999-12-31',INTERVAL 1 second));
insert into test values('date_sub(''9999-12-31 12:12:12'',INTERVAL 1 hour)', date_sub('9999-12-31 12:12:12',INTERVAL 1 hour));
insert into test values('date_sub(''9999-12-31 12:12:12'',INTERVAL 1 minute)', date_sub('9999-12-31 12:12:12',INTERVAL 1 minute));
insert into test values('date_sub(''9999-12-31 12:12:12'',INTERVAL 1 second)', date_sub('9999-12-31 12:12:12',INTERVAL 1 second));
-- 功能-一些进位情况
insert into test values('date_sub(''0001-12-31'',INTERVAL -1 day)',date_sub('0001-12-31',INTERVAL -1 day));
insert into test values('date_sub(''2021-12-31'',INTERVAL -1 day)',date_sub('2021-12-31',INTERVAL -1 day));
insert into test values('date_sub(''9998-12-31'',INTERVAL -1 day)',date_sub('9998-12-31',INTERVAL -1 day));
insert into test values('date_sub(''0001-12-31'',INTERVAL -1 month)',date_sub('0001-12-31',INTERVAL -1 month));
insert into test values('date_sub(''1999-12-31'',INTERVAL -1 day)',date_sub('1999-12-31',INTERVAL -1 day));
insert into test values('date_sub(''1999-12-31'',INTERVAL -1 year)',date_sub('1999-12-31',INTERVAL -1 year));
insert into test values('date_sub(''1999-12-31 23:59:59'',INTERVAL -1 second)',date_sub('1999-12-31 23:59:59',INTERVAL -1 second));
insert into test values('date_sub(''1999-12-31 23:59:00'',INTERVAL -1 minute)',date_sub('1999-12-31 23:59:00',INTERVAL -1 minute));
insert into test values('date_sub(''1999-12-31 23:00:00'',INTERVAL -1 hour)',date_sub('1999-12-31 23:00:00',INTERVAL -1 hour));
-- 功能-第一参数为显示time类型
insert into test values('date_sub(time''12:12:12'', INTERVAL 1 day)', date_sub(time'12:12:12', INTERVAL 1 day));
insert into test values('date_sub(time''12:12:12'', INTERVAL 1 hour)', date_sub(time'12:12:12', INTERVAL 1 hour));
insert into test values('date_sub(time''12:12:12'', INTERVAL 1 minute)', date_sub(time'12:12:12', INTERVAL 1 minute));
insert into test values('date_sub(time''12:12:12'', INTERVAL 1 second)', date_sub(time'12:12:12', INTERVAL 1 second));
insert into test values('date_sub(time''12:12:12'', INTERVAL -1 day)', date_sub(time'12:12:12', INTERVAL -1 day));
insert into test values('date_sub(time''12:12:12'', INTERVAL -1 hour)', date_sub(time'12:12:12', INTERVAL -1 hour));
insert into test values('date_sub(time''12:12:12'', INTERVAL -1 minute)', date_sub(time'12:12:12', INTERVAL -1 minute));
insert into test values('date_sub(time''12:12:12'', INTERVAL -1 second)', date_sub(time'12:12:12', INTERVAL -1 second));
-- 类型
insert into test values('date_sub(date''0001-01-01'',INTERVAL -1 year)', date_sub(date'0001-01-01',INTERVAL -1 year));
insert into test values('date_sub(cast(''9999-12-31 12:12:12'' as datetime),INTERVAL -1 second)', date_sub(cast('9999-12-31 12:12:12' as datetime) ,INTERVAL -1 second));
insert into test values('date_sub(time''838:59:59'', INTERVAL 1 second)', date_sub(time'838:59:59',INTERVAL 1 second));
insert into test values('date_sub(101, INTERVAL 1 year)', date_sub(101, INTERVAL 1 year));
insert into test values('date_sub(101, INTERVAL 100 second)', date_sub(101, INTERVAL 100 second));
-- 特异
insert into test values('date_sub(null, INTERVAL 1 day)', date_sub(null, INTERVAL 1 day));

-- 非严格模式，参数或者结果不合法，报warning，返回NULL或者对应值
set dolphin.sql_mode = 'sql_mode_full_group,pipes_as_concat,ansi_quotes';
insert into test values('date_sub(0, INTERVAL 100 second)', date_sub(0, INTERVAL 100 second));
insert into test values('date_sub(''10000-1-1'', INTERVAL 1 day)', date_sub('10000-1-1', INTERVAL 1 day));
insert into test values('date_sub(''9999-12-31'', INTERVAL -1 day)', date_sub('9999-12-31', INTERVAL -1 day));
insert into test values('date_sub(''9999-12-31'', INTERVAL -1 month)', date_sub('9999-12-31', INTERVAL -1 month));
insert into test values('date_sub(''9999-12-31'', INTERVAL -1 year)', date_sub('9999-12-31', INTERVAL -1 year));
insert into test values('date_sub(''9999-12-31 23:59:59'', INTERVAL -1 second)', date_sub('9999-12-31 23:59:59', INTERVAL -1 second));
insert into test values('date_sub(''9999-12-31 23:59:00'', INTERVAL -1 minute)', date_sub('9999-12-31 23:59:00', INTERVAL -1 minute));
insert into test values('date_sub(''9999-12-31 23:00:00'', INTERVAL -1 hour)', date_sub('9999-12-31 23:00:00', INTERVAL -1 hour));
insert into test values('date_sub(time''838:59:59'', INTERVAL -1 second)', date_sub(time'838:59:59', INTERVAL -1 second));
insert into test values('date_sub(time''-838:59:59'', INTERVAL 1 second)', date_sub(time'-838:59:59', INTERVAL 1 second));
-- 结果小于'0001-1-1 00:00:00'时，MSQ日期部分总为'0000-00-00'，openGauss中非严格模式下返回NULL
insert into test values('date_sub(''0001-1-1'', INTERVAL 1 day)', date_sub('0001-1-1', INTERVAL 1 day));
insert into test values('date_sub(''0001-1-1 00:00:00'', INTERVAL 1 second)', date_sub('0001-1-1 00:00:00', INTERVAL 1 second));

-- 严格模式，参数不合法，抛出错误
set dolphin.sql_mode = 'sql_mode_strict,sql_mode_full_group,pipes_as_concat,ansi_quotes';
insert into test values('date_sub(0, INTERVAL 100 second)', date_sub(0, INTERVAL 100 second));
insert into test values('date_sub(''10000-1-1'', INTERVAL 1 day)', date_sub('10000-1-1', INTERVAL 1 day));
insert into test values('date_sub(''9999-12-31'', INTERVAL -1 day)', date_sub('9999-12-31', INTERVAL -1 day));
insert into test values('date_sub(''9999-12-31'', INTERVAL -1 month)', date_sub('9999-12-31', INTERVAL -1 month));
insert into test values('date_sub(''9999-12-31'', INTERVAL -1 year)', date_sub('9999-12-31', INTERVAL -1 year));
insert into test values('date_sub(''9999-12-31 23:59:59'', INTERVAL -1 second)', date_sub('9999-12-31 23:59:59', INTERVAL -1 second));
insert into test values('date_sub(''9999-12-31 23:59:00'', INTERVAL -1 minute)', date_sub('9999-12-31 23:59:00', INTERVAL -1 minute));
insert into test values('date_sub(''9999-12-31 23:00:00'', INTERVAL -1 hour)', date_sub('9999-12-31 23:00:00', INTERVAL -1 hour));
insert into test values('date_sub(time''838:59:59'', INTERVAL -1 second)', date_sub(time'838:59:59', INTERVAL -1 second));
insert into test values('date_sub(time''-838:59:59'', INTERVAL 1 second)', date_sub(time'-838:59:59', INTERVAL 1 second));
-- 结果小于'0001-1-1 00:00:00'时，MSQ日期部分总为'0000-00-00'，openGauss中非严格模式下返回NULL
insert into test values('date_sub(''0001-1-1'', INTERVAL 1 day)', date_sub('0001-1-1', INTERVAL 1 day));
insert into test values('date_sub(''0001-1-1 00:00:00'', INTERVAL 1 second)', date_sub('0001-1-1 00:00:00', INTERVAL 1 second));

-- test addtime(datetime/time, time)
-- 严格模式或者非严格模式都有值
-- 功能
insert into test values('addtime(''00:00:00'', ''00:00:00'')', addtime('00:00:00', '00:00:00'));
insert into test values('addtime(''11:22:33'', ''20:20:20'')', addtime('11:22:33', '20:20:20'));
insert into test values('addtime(''11:22:33.999999'', ''00:00:00.000001'')', addtime('11:22:33.999999', '00:00:00.000001'));
insert into test values('addtime(''838:59:58'', ''00:00:01'')', addtime('838:59:58', '00:00:01'));
insert into test values('addtime(''838:59:59'', ''-00:00:01'')', addtime('838:59:59', '-00:00:01'));
insert into test values('addtime(''-838:59:58'', ''-00:00:01'')', addtime('-838:59:58', '-00:00:01'));
insert into test values('addtime(''-838:59:59'', ''00:00:01'')', addtime('-838:59:59', '00:00:01'));
insert into test values('addtime(''9999-12-31 23:59:59.999999'', ''00:00:00'')', addtime('9999-12-31 23:59:59.999999','00:00:00'));
insert into test values('addtime(''9999-12-31 23:59:59.999999'', ''-00:00:01'')', addtime('9999-12-31 23:59:59.999999','00:00:00'));
insert into test values('addtime(''9999-12-31 23:59:59.999999'', ''-838:59:59'')', addtime('9999-12-31 23:59:59.999999','-838:59:59'));
insert into test values('addtime(''2021-11-12 23:59:59'', ''23:59:10'')', addtime('2021-11-12 23:59:59', '23:59:10'));
-- 类型
insert into test values('addtime(cast(''9999-12-31 23:59:59.999999'' as datetime(6)), ''00:00:00'')', addtime(cast('9999-12-31 23:59:59.999999' as datetime(6)),'00:00:00'));
insert into test values('addtime(time''12:12:12.999999'', ''00:00:00.000001'')', addtime(time'12:12:12.999999', '00:00:00.000001'));
insert into test values('addtime(''-8385959'', ''00:00:01'')', addtime('-8385959', '00:00:01'));
insert into test values('addtime(-8385959, ''00:00:01'')', addtime(-8385959, '00:00:01'));
insert into test values('addtime(0, ''00:00:01'')', addtime(0, '00:00:01'));
-- 特异
insert into test values('addtime(null, ''00:00:01'')', addtime(null, '00:00:01'));
insert into test values('addtime(''00:00:01'', null)', addtime('00:00:01', null));
insert into test values('addtime(null, null)', addtime(null, null));
insert into test values('addtime(''12:12:12'', ''2021-11-12 12:12:12'')', addtime('12:12:12', '2021-11-12 12:12:12'));
insert into test values('addtime(''0001-1-1 00:00:00'', ''-00:00:01'')', addtime('0001-1-1 00:00:00', '-00:00:01'));

-- 非严格模式，参数或者结果不合法，报warning，返回NULL或者对应值
set dolphin.sql_mode = 'sql_mode_full_group,pipes_as_concat,ansi_quotes';
insert into test values('addtime(''839:00:00'', ''00:00:00'')', addtime('839:00:00', '00:00:00'));
insert into test values('addtime(''838:59:59'', ''00:00:01'')', addtime('838:59:59', '00:00:01'));
insert into test values('addtime(''838:59:59.1'', ''00:00:00'')', addtime('838:59:59.1', '00:00:00'));
insert into test values('addtime(''-838:59:59.1'', ''00:00:00'')', addtime('-838:59:59.1', '00:00:00'));
insert into test values('addtime(''-839:00:00'', ''00:00:00'')', addtime('-839:00:00', '00:00:00'));
insert into test values('addtime(''-838:59:59'', ''-00:00:01'')', addtime('-838:59:59', '-00:00:01'));
insert into test values('addtime(''9999-12-31 23:59:59'', ''00:00:01'')', addtime('9999-12-31 23:59:59', '00:00:01'));
insert into test values('addtime(''10000-1-1 00:00:00'', ''00:00:00'')', addtime('10000-1-1 00:00:00', '00:00:00'));

-- 严格模式，参数或结果不合法，抛出错误
set dolphin.sql_mode = 'sql_mode_strict,sql_mode_full_group,pipes_as_concat,ansi_quotes';
insert into test values('addtime(''839:00:00'', ''00:00:00'')', addtime('839:00:00', '00:00:00'));
insert into test values('addtime(''838:59:59'', ''00:00:01'')', addtime('838:59:59', '00:00:01'));
insert into test values('addtime(''838:59:59.1'', ''00:00:00'')', addtime('838:59:59.1', '00:00:00'));
insert into test values('addtime(''-838:59:59.1'', ''00:00:00'')', addtime('-838:59:59.1', '00:00:00'));
insert into test values('addtime(''-839:00:00'', ''00:00:00'')', addtime('-839:00:00', '00:00:00'));
insert into test values('addtime(''-838:59:59'', ''-00:00:01'')', addtime('-838:59:59', '-00:00:01'));
insert into test values('addtime(''9999-12-31 23:59:59'', ''00:00:01'')', addtime('9999-12-31 23:59:59', '00:00:01'));
insert into test values('addtime(''10000-1-1 00:00:00'', ''00:00:00'')', addtime('10000-1-1 00:00:00', '00:00:00'));

select * from test order by funcname;
drop table test;
\c contrib_regression
DROP DATABASE b_datetime_func_test;
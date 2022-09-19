---- b compatibility case
drop database if exists b_datetime_func_test;
create database b_datetime_func_test dbcompatibility 'b';
\c b_datetime_func_test
set datestyle = 'ISO,ymd';
set time zone "Asia/Shanghai";
-- test datediff
-- 边界值，坏值
select datediff('0000-1-2', '0000-1-1');
select datediff('9999-12-31', '0000-1-1');
select datediff('2021-12-31', '2020-1-1');
select datediff('2021-0-1', '2021-1-1');
-- 进位
select datediff('0000-1-2 23:59:59.999999', '0000-1-1 00:00:00');
select datediff('0000-1-2 23:59:59.9999999', '0000-1-1 00:00:00');
select datediff('0000-1-2 00:00:00', '0000-1-1 23:59:59.999999');
select datediff('0000-1-2 00:00:00', '0000-1-1 23:59:59.9999999');
select datediff('2021-12-31 23:59:59.999999', '2022-1-1 00:00:00');
select datediff('2021-12-31 23:59:59.9999999', '2022-1-1 00:00:00');
select datediff('9999-12-31 23:59:59.999999', '0000-1-1 00:00:00');
select datediff('9999-12-31 23:59:59.9999999', '0000-1-1 00:00:00');
-- 不同类型输入
select datediff(20211112, 20211113);
select datediff(20211112, '20211113');
select datediff(date'2021-11-12', date'2021-11-12');
select datediff(datetime'2021-11-12 12:12:12', datetime'2021-11-12 23:59:59.9999999');
select datediff('20211112235959.9999999', 20211112235959.9999999);

-- test from_days
select from_days(0);
select from_days(355);
select from_days(366);
select from_days(355.5);
select from_days(-1);
select from_days(3652500);
select from_days(3652499);
select from_days(3652499.5);
select from_days('3652499abc');
select from_days('111111111111111111111111111111111111');

-- test timestampdiff
set b_compatibility_mode = true;
-- 边界值，坏值
select timestampdiff(microsecond, '0000-1-1 00:00:00', '0000-1-2');
select timestampdiff(second, '0000-1-1 00:00:00', '0000-1-2');
select timestampdiff(minute, '0000-1-1 00:00:00', '0000-1-2');
select timestampdiff(hour, '0000-1-1 00:00:00', '0000-1-2');
select timestampdiff(day, '9999-11-12 00:00:00', '9999-11-11');
select timestampdiff(day, '10000-1-1 00:00:00', '9999-12-31');
select timestampdiff(year, '9999-12-31', '0000-12-31');
select timestampdiff(quarter, '9999-12-31', '0000-12-31');
select timestampdiff(month, '9999-12-31', '0000-12-31');
select timestampdiff(week, '9999-12-31', '0000-12-31');
select timestampdiff(microsecond, time'-838:59:59', time'838:59:59');
select timestampdiff(second, time'-838:59:59', time'838:59:59');
select timestampdiff(minute, time'-838:59:59', time'838:59:59');
select timestampdiff(hour, time'-838:59:59', time'838:59:59');
select timestampdiff(day, time'-838:59:59', time'838:59:59');
select timestampdiff(month, time'-838:59:59', time'838:59:59');
select timestampdiff(quarter, time'-838:59:59', time'838:59:59');
select timestampdiff(year, time'-838:59:59', time'838:59:59');
select timestampdiff(microsecond, '0000-0-1', '0000-1-2');
select timestampdiff(microsecond, '9999-0-1', '0000-1-2');
-- 混合类型
select timestampdiff(microsecond, '2022-9-12', time'12:12:12');
select timestampdiff(microsecond, time'12:12:12', '2022-9-12');
select timestampdiff(microsecond, datetime'2022-9-12 12:12:12.999999', time'12:12:12.999999');
select timestampdiff(microsecond, time'12:12:12.999999', datetime'2022-9-12 12:12:12.999999');
select timestampdiff(microsecond, date'2022-9-12', time'12:12:12.999999');
select timestampdiff(microsecond, time'12:12:12.999999', date'2022-9-12');
set b_compatibility_mode = false;

-- test convert_tz
-- 双字母,第一次转换超[1970-1-1 00:00:01.000000, 2038-01-19 03:14:07.999999]，结果不转换
select convert_tz('1970-1-1 00:00:01.000000', 'MET', 'GMT');
select convert_tz('1970-1-1 00:00:01.000000', 'CCT', 'GMT');
select convert_tz('2038-01-19 03:14:07.999999', 'EDT', 'GMT');
-- 双字母正常转换
select convert_tz('1234-1-1 00:00:00', 'MET', 'GMT');
select convert_tz('1970-1-1 00:00:01.000000', 'GMT', 'GMT');
select convert_tz('1970-1-1 00:00:01.000000', 'EDT', 'GMT');
select convert_tz('1970-1-1 00:00:01.000000', 'CDT', 'GMT');
select convert_tz('1970-1-1 00:00:01.000000', 'EST', 'GMT');
select convert_tz('1970-1-1 00:00:01.000000', 'MDT', 'GMT');
select convert_tz('1970-1-1 00:00:01.000000', 'MST', 'GMT');
select convert_tz('2038-01-19 03:14:07.999999', 'JST', 'GMT');
select convert_tz('2038-01-19 03:14:07.999999', 'KST', 'GMT');
select convert_tz('2038-01-19 03:14:07.999999', 'CCT', 'GMT');
-- 双数字,第一次转换超[1970-1-1 00:00:01.000000, 2038-01-19 03:14:07.999999]，结果不转换
select convert_tz('1970-1-1 00:00:01.000000', '+1:00', '+0:00');
select convert_tz('2038-01-19 03:14:07.999999', '-1:00', '+0:00');
-- 双数字正常转换
select convert_tz('1970-1-1 00:00:01.000000', '-1:00', '+0:00');
select convert_tz('1970-1-1 00:00:01.000000', '-2:00', '+0:00');
select convert_tz('1970-1-1 00:00:01.000000', '-10:00', '+0:00');
select convert_tz('2038-01-19 03:14:07.999999', '+1:00', '+0:00');
select convert_tz('2038-01-19 03:14:07.999999', '+2:00', '+0:00');
select convert_tz('2038-01-19 03:14:07.999999', '+10:00', '+0:00');
-- 字母数字混合
select convert_tz('1970-1-1 00:00:01.000000', '-1:00', 'GMT');
select convert_tz('1970-1-1 00:00:01.000000', 'GMT', '+3:00');
select convert_tz('2038-01-19 03:14:07.999999', '+2:00', 'GMT');
select convert_tz('2038-01-19 03:14:07.999999', 'GMT', '+7:00');

-- test adddate(date,INTERVAL expr unit)
-- 边界值，坏值
select adddate('0001-01-01',INTERVAL 1 day);
select adddate('0001-02-28',INTERVAL 1 day);
select adddate('0004-02-29',INTERVAL 1 day);
select adddate('0001-12-31',INTERVAL 1 month);
select adddate('9999-12-30',INTERVAL 1 day);
select adddate('0000-01-01',INTERVAL 1 day);
select adddate(2001-01-01,INTERVAL 1 day);
select adddate('9999-12-12',INTERVAL 1 year);
-- 进位
select adddate('0001-01-31',INTERVAL 1 day);
select adddate('0001-2-28',INTERVAL 1 day);
select adddate('0001-12-31',INTERVAL 1 day);
select adddate('2021-12-31',INTERVAL 1 day);
select adddate('9998-12-31',INTERVAL 1 day);
select adddate('0001-12-31',INTERVAL 1 month);
select adddate('1999-12-31',INTERVAL 1 day);
-- 不同类型输入
select adddate(date'001-01-01',INTERVAL 1 day);
select adddate(datetime'001-01-01 12:21:21',INTERVAL 1 day);
select adddate(time'1:1:1',INTERVAL 1 day);
select adddate('2001-12-12',INTERVAL 1 day);
select adddate('2001-12-12 12:12:12',INTERVAL 1 day);

-- test date_add(date,INTERVAL expr unit)
-- 边界值，坏值
select date_add('0001-01-01',INTERVAL 1 day);
select date_add('0001-02-28',INTERVAL 1 day);
select date_add('0004-02-29',INTERVAL 1 day);
select date_add('0001-12-31',INTERVAL 1 month);
select date_add('9999-12-30',INTERVAL 1 day);
select date_add('0000-01-01',INTERVAL 1 day);
select date_add(2001-01-01,INTERVAL 1 day);
select date_add('9999-12-12',INTERVAL 1 year);
-- 进位
select date_add('0001-01-31',INTERVAL 1 day);
select date_add('0001-2-28',INTERVAL 1 day);
select date_add('0001-12-31',INTERVAL 1 day);
select date_add('2021-12-31',INTERVAL 1 day);
select date_add('9998-12-31',INTERVAL 1 day);
select date_add('0001-12-31',INTERVAL 1 month);
select date_add('1999-12-31',INTERVAL 1 day);
-- 不同类型输入
select date_add(date'001-01-01',INTERVAL 1 day);
select date_add(datetime'001-01-01 12:21:21',INTERVAL 1 day);
select date_add(time'1:1:1',INTERVAL 1 day);
select date_add('2001-12-12',INTERVAL 1 day);
select date_add('2001-12-12 12:12:12',INTERVAL 1 day);

-- test date_sub(date,INTERVAL expr unit)
-- 边界值，坏值
select date_sub('0001-01-01',INTERVAL '-1' day);
select date_sub('0001-02-28',INTERVAL '-1' day);
select date_sub('0004-02-29',INTERVAL '-1' day);
select date_sub('0001-12-31',INTERVAL '-1' month);
select date_sub('9999-12-30',INTERVAL '-1' day);
select date_sub('0000-01-01',INTERVAL '-1' day);
select date_sub(2001-01-01,INTERVAL '-1' day);
select date_sub('9999-12-12',INTERVAL '-1' year);
-- 进位
select date_sub('0001-01-31',INTERVAL '-1' day);
select date_sub('0001-2-28',INTERVAL '-1' day);
select date_sub('0001-12-31',INTERVAL '-1' day);
select date_sub('2021-12-31',INTERVAL '-1' day);
select date_sub('9998-12-31',INTERVAL '-1' day);
select date_sub('0001-12-31',INTERVAL '-1' month);
select date_sub('1999-12-31',INTERVAL '-1' day);
-- 不同类型输入
select date_sub(date'001-01-01',INTERVAL '-1' day);
select date_sub(datetime'001-01-01 12:21:21',INTERVAL '-1' day);
select date_sub(time'1:1:1',INTERVAL '-1' day);
select date_sub('2001-12-12',INTERVAL '-1' day);
select date_sub('2001-12-12 12:12:12',INTERVAL '-1' day);

-- test addtime(expr1,expr2)
-- 边界值，坏值
select addtime('0000-01-01 0:0:0','20:20:30');
select addtime('0000-01-01 00:00:00.000000','00:00:00.000000');
select addtime('9999-12-31 23:59:59.999999','0:0:0');
select addtime(0000-01-01 00:00:00.000000,'00:00:00.000000');
select addtime('0000-01-01 00:00:00.000000',00:00:00.000000);
select addtime('0000-01-01 00:00:00.000000','2001-01-01');
-- 进位
select addtime('2000-12-31 23:59:59','0:0:1');
select addtime('2000-12-31 23:59:59.9','0:0:0.1');
select addtime('2000-1-31 23:59:59.9','0:0:0.1');
select addtime('2000-1-31 23:59:59','0:0:1');
-- 不同类型输入
select addtime('11:22:33','10:20:30');
select addtime('2020-03-04 11:22:33', '10:20:30');
select addtime(datetime'2020-03-04 11:22:33','10:20:30');
select addtime(time'11:22:33','10:20:30');
select addtime(datetime'2020-03-04 11:22:33',time'10:20:30');
select addtime('11:22:33',time'10:20:30');

-- test adddate(date/datetime/time,days)
-- 边界值，坏值
select adddate('0001-01-01',999);
select adddate('0001-02-28',1000000);
select adddate('0004-02-29',1);
select adddate('9999-12-30',1);
select adddate('9999-12-31',0);
select adddate('9999-12-31',1);
select adddate('10000-12-31',1)
select adddate('0000-01-01',-1);
select adddate(2001-01-01,1);
select adddate('12:12:12',0.5);
select adddate('12:12:12',45);
select adddate(time'132:122:12',-55);
select adddate('132:122:12',5);
select adddate(time'sssssss',5);
select adddate('sssssss',5);
-- 进位
select adddate('0001-01-31',1);
select adddate('0001-2-28',1);
select adddate('0001-12-31',1);
select adddate('2021-12-31',1);
select adddate('9998-12-31',1);
-- 不同类型输入
select adddate(date'0001-01-01',1);
select adddate('0001-01-01',1);
select adddate(datetime'0001-01-01 12:21:21',1);
select adddate('0001-01-01 12:21:21',1);
select adddate(time'1:1:1',1);
select adddate(time'1:1:1',30);
select adddate(time'1:1:1',-30);
select adddate(time'1:1:1',-100);
select adddate('1:1:1',1);
select adddate('2001-12-12',1);
select adddate('2001-12-12 12:12:12',1);

-- test adddate(date/datetime/time,INTERVAL expr unit)
-- 边界值，坏值
select adddate('0001-01-01',INTERVAL 1 day);
select adddate('0001-02-28',INTERVAL 1 day);
select adddate('0004-02-29',INTERVAL 1 day);
select adddate('0001-12-31',INTERVAL 1 month);
select adddate('9999-12-30',INTERVAL 1 day);
select adddate('0000-01-01',INTERVAL 1 day);
select adddate(2001-01-01,INTERVAL 1 day);
select adddate('9999-12-12',INTERVAL 1 year);
select adddate('0000-00-00',INTERVAL 1 day);
select adddate('1000000-01-00',INTERVAL 1 day);
select adddate('0002-03-01',INTERVAL '-3' year);
select adddate(time'12:12:12',INTERVAL 1 year);
select adddate('12:12:12',INTERVAL 1 year);
-- 进位
select adddate('0001-01-31',INTERVAL 1 day);
select adddate('0001-2-28',INTERVAL 1 day);
select adddate('0001-12-31',INTERVAL 1 day);
select adddate('2021-12-31',INTERVAL 1 day);
select adddate('9998-12-31',INTERVAL 1 day);
select adddate('0001-12-31',INTERVAL 1 month);
select adddate('1999-12-31',INTERVAL 1 day);
-- 不同类型输入
select adddate(date'001-01-01',INTERVAL 1 day);
select adddate(datetime'001-01-01 12:21:21',INTERVAL 1 day);
select adddate(time'1:1:1',INTERVAL 1 day);
select adddate('2001-12-12',INTERVAL 1 day);
select adddate('2001-12-12 12:12:12',INTERVAL 1 day);

-- test date_add(date,INTERVAL expr unit)
-- 边界值，坏值
select date_add('0001-01-01',INTERVAL 1 day);
select date_add('0001-02-28',INTERVAL 1 day);
select date_add('0004-02-29',INTERVAL 1 day);
select date_add('0001-01-01',1);
select date_add('0001-02-28',1);
select date_add('0004-02-29',1);
select date_add('0001-12-31',INTERVAL 1 month);
select date_add('9999-12-30',1);
select date_add('0000-01-01',1);
select date_add(2001-01-01,1);
select date_add('9999-12-12',INTERVAL 1 year);
select date_add('9999-12-12 12:12:12',INTERVAL 1 day);
select date_add(time'12:12:12',1);
select date_add('12:12:12',1);
select date_add('133:131:45',5);
-- 进位
select date_add('0001-01-31',INTERVAL 1 day);
select date_add('0001-2-28',INTERVAL 1 day);
select date_add('0001-12-31',INTERVAL 1 day);
select date_add('2021-12-31',INTERVAL 1 day);
select date_add('9998-12-31',INTERVAL 1 day);
select date_add('0001-01-31',INTERVAL 1 day);
select date_add('0001-2-28',1);
select date_add('0001-12-31',1);
select date_add('2021-12-31',1);
select date_add('9998-12-31',1);
select date_add('0001-12-31',INTERVAL 1 month);
select date_add('1999-12-31',INTERVAL 1 day);
-- 不同类型输入
select date_add(date'001-01-01',INTERVAL 1 day);
select date_add(datetime'001-01-01 12:21:21',INTERVAL 1 day);
select date_add(time'1:1:1',INTERVAL 1 day);
select date_add('2001-12-12',INTERVAL 1 day);
select date_add('2001-12-12 12:12:12',INTERVAL 1 day);

-- test date_sub(date,INTERVAL expr unit)
-- 边界值，坏值
select date_sub('0001-01-01',INTERVAL '-1' day);
select date_sub('0001-02-28',INTERVAL '-1' day);
select date_sub('0004-02-29',INTERVAL '-1' day);
select date_sub('0001-12-31',INTERVAL '-1' month);
select date_sub('9999-12-30',INTERVAL '-1' day);
select date_sub('0000-01-01',INTERVAL '-1' day);
select date_sub(2001-01-01,INTERVAL '-1' day);
select date_sub('9999-12-12',INTERVAL '-1' year);
select date_sub('2001-01-01',1);
select date_sub('0001-02-28',1);
select date_sub('9999-12-30',-1);
select date_sub('0000-01-01',-1);
select date_sub(2001-01-01,-1);
-- 进位
select date_sub('0001-01-31',INTERVAL '-1' day);
select date_sub('0001-2-28',INTERVAL '-1' day);
select date_sub('0001-12-31',INTERVAL '-1' day);
select date_sub('2021-12-31',INTERVAL '-1' day);
select date_sub('9998-12-31',INTERVAL '-1' day);
select date_sub('0001-01-31',-1);
select date_sub('0001-2-28',-1);
select date_sub('0001-12-31',-1);
select date_sub('2021-12-31',-1);
select date_sub('9998-12-31',-1);
select date_sub('1999-12-31',-1);
-- 不同类型输入
select date_sub(date'001-01-01',INTERVAL '-1' day);
select date_sub(date'001-01-01',-1);
select date_sub(datetime'001-01-01 12:21:21',INTERVAL '-1' day);
select date_sub(time'1:1:1',INTERVAL '-1' day);
select date_sub('2001-12-12',INTERVAL '-1' day);
select date_sub('2001-12-12 12:12:12',INTERVAL '-1' day);

-- test addtime(expr1,expr2)
-- 边界值，坏值
select addtime('0000-01-01 0:0:0','20:20:30');
select addtime('0000-01-01 0:0:0',1);
select addtime('0000-01-01 00:00:00.000000','00:00:00.000000');
select addtime('0000-01-01 00:00:00.000000',1);
select addtime('9999-12-31 23:59:59.999999','0:0:0');
select addtime('9999-12-31 23:59:59.999999',1);
select addtime(0000-01-01 00:00:00.000000,1);
select addtime('0000-01-01 00:00:00.000000',00:00:00.000000);
select addtime('0000-01-01 00:00:00.000000','2001-01-01');
-- 进位
select addtime('2000-12-31 23:59:59','0:0:1');
select addtime('2000-12-31 23:59:59.9','0:0:0.1');
select addtime('2000-1-31 23:59:59.9','0:0:0.1');
select addtime('2000-1-31 23:59:59','0:0:1');
-- 不同类型输入
select addtime('11:22:33','10:20:30');
select addtime('11:22:33',999);
select addtime('2020-03-04 11:22:33', '10:20:30');
select addtime('2020-03-04 11:22:33', 1);
select addtime(datetime'2020-03-04 11:22:33','10:20:30');
select addtime(datetime'2020-03-04 11:22:33', 0);
select addtime(time'11:22:33','10:20:30');
select addtime(datetime'2020-03-04 11:22:33',time'10:20:30');
select addtime('11:22:33',time'10:20:30');

\c contrib_regression
DROP DATABASE b_datetime_func_test;
--
-- Test Time functions(Stage 3) under 'b' compatibility
-- Contains to_days(), to_seconds(), unix_timestamp(), utc_date(), utc_time()、timestampadd()
--
create schema b_time_funcs3;
set current_schema to 'b_time_funcs3';

create table func_test3(functionName varchar(256),result varchar(256));
truncate table func_test3;

-- TO_DAYS()
-- 正常测试测试
insert into func_test3(functionName,result) values('TO_DAYS(''2022-1-1'')',TO_DAYS('2022-1-1'));
insert into func_test3(functionName,result) values('TO_DAYS(''44440101'')',TO_DAYS('44440101'));
insert into func_test3(functionName,result) values('TO_DAYS(20000229)',TO_DAYS(20000229));
insert into func_test3(functionName,result) values('TO_DAYS(''2022-1-1 1:1:1'')',TO_DAYS('2022-1-1 1:1:1'));
insert into func_test3(functionName,result) values('TO_DAYS(''2022-2-2 2:2:2.0000015'')',TO_DAYS('2022-2-2 2:2:2.0000015'));
insert into func_test3(functionName,result) values('TO_DAYS(''20220101010101'')',TO_DAYS('20220101010101'));
insert into func_test3(functionName,result) values('TO_DAYS(20220101010101)',TO_DAYS(20220101010101));
insert into func_test3(functionName,result) values('TO_DAYS(''20220101010101.000001'')',TO_DAYS('20220101010101.000001'));
insert into func_test3(functionName,result) values('TO_DAYS(20220101010101.000002)',TO_DAYS(20220101010101.000002));
-- 非法的date/datetime格式
insert into func_test3(functionName,result) values('TO_DAYS(''2022-1-32'')',TO_DAYS('2022-1-32'));
insert into func_test3(functionName,result) values('TO_DAYS(''2022-13-1'')',TO_DAYS('2022-13-1'));
insert into func_test3(functionName,result) values('TO_DAYS(''2022-2-2 2:2:60'')',TO_DAYS('2022-2-2 2:2:60'));
insert into func_test3(functionName,result) values('TO_DAYS(''2022-2-2 2:60:2'')',TO_DAYS('2022-2-2 2:60:2'));
insert into func_test3(functionName,result) values('TO_DAYS(''2022-2-2 24:2:2'')',TO_DAYS('2022-2-2 24:2:2'));
-- 超长参数
insert into func_test3(functionName,result) values('TO_DAYS(''99999999999-1-1'')',TO_DAYS('99999999999-1-1'));
insert into func_test3(functionName,result) values('TO_DAYS(''00000000000-1-1'')',TO_DAYS('00000000000-1-1'));
insert into func_test3(functionName,result) values('TO_DAYS(''00000000000-00000000001-1'')',TO_DAYS('00000000000-00000000001-1'));
insert into func_test3(functionName,result) values('TO_DAYS(''00000000000-00000000001-0000000001'')',TO_DAYS('00000000000-00000000001-0000000001'));
-- 特殊类型参数
insert into func_test3(functionName,result) values('TO_DAYS(true)',TO_DAYS(true));
insert into func_test3(functionName,result) values('TO_DAYS(false)',TO_DAYS(false));
insert into func_test3(functionName,result) values('TO_DAYS(null)',TO_DAYS(null));
insert into func_test3(functionName,result) values('TO_DAYS(date''2000-1-1'')',TO_DAYS(date'2000-1-1'));
insert into func_test3(functionName,result) values('TO_DAYS(cast(''2022-2-2 2:2:2'' as datetime))',TO_DAYS(cast('2022-2-2 2:2:2' as datetime)));
insert into func_test3(functionName,result) values('TO_DAYS(time''1:1:1'')',TO_DAYS(time'1:1:1'));
insert into func_test3(functionName,result) values('TO_DAYS(time''25:0:0'')',TO_DAYS(time'25:0:0'));
-- 数值
insert into func_test3(functionName,result) values('TO_DAYS(1)',TO_DAYS(1));
insert into func_test3(functionName,result) values('TO_DAYS(1)',TO_DAYS(1));
insert into func_test3(functionName,result) values('TO_DAYS(001)',TO_DAYS(001));                           -- 单位数年月日
insert into func_test3(functionName,result) values('TO_DAYS(101)',TO_DAYS(101));                           -- 双位日+单位月
insert into func_test3(functionName,result) values('TO_DAYS(0101)',TO_DAYS(0101));                         -- 双位日月
insert into func_test3(functionName,result) values('TO_DAYS(00101)',TO_DAYS(00101));                       -- 双位日月+单位年
insert into func_test3(functionName,result) values('TO_DAYS(000101)',TO_DAYS(000101));                     -- 双位年月日
insert into func_test3(functionName,result) values('TO_DAYS(00000101)',TO_DAYS(00000101));                 -- -四位年+双位月日
insert into func_test3(functionName,result) values('TO_DAYS(00000101001)',TO_DAYS(00000101001));           -- 四位年+双位月日+单位时分秒
insert into func_test3(functionName,result) values('TO_DAYS(00000101000001)',TO_DAYS(00000101000001));     -- 四位年+双位月日时分秒
insert into func_test3(functionName,result) values('TO_DAYS(01000001)',TO_DAYS(01000001));                 -- 双位日+双位时分秒
insert into func_test3(functionName,result) values('TO_DAYS(0101000001)',TO_DAYS(0101000001));             -- -双位日月+双位时分秒
insert into func_test3(functionName,result) values('TO_DAYS(00101000001)',TO_DAYS(00101000001));           -- -单位年+双位日月+双位时分秒
insert into func_test3(functionName,result) values('TO_DAYS(0000101000001)',TO_DAYS(0000101000001));      -- -三位年+双位日月+双位时分秒
-- 边界测试
-- 最小值
insert into func_test3(functionName,result) values('TO_DAYS(''0000-1-1'')',TO_DAYS('0000-1-1'));
insert into func_test3(functionName,result) values('TO_DAYS(''0000-1-1 00:00:00'')',TO_DAYS('0000-1-1 00:00:00'));
insert into func_test3(functionName,result) values('TO_DAYS(''0000-0-0'')',TO_DAYS('0000-0-0'));
-- 最大值
insert into func_test3(functionName,result) values('TO_DAYS(''9999-12-31'')',TO_DAYS('9999-12-31'));
insert into func_test3(functionName,result) values('TO_DAYS(''9999-12-31 23:59:59.999999'')',TO_DAYS('9999-12-31 23:59:59.999999'));
insert into func_test3(functionName,result) values('TO_DAYS(''10000-1-1'')',TO_DAYS('10000-1-1'));
insert into func_test3(functionName,result) values('TO_DAYS(''10000-1-1 00:00:00'')',TO_DAYS('10000-1-1 00:00:00'));

-- TO_SECONDS()
-- 正常测试
-- date格式
insert into func_test3(functionName, result) values('TO_SECONDS(''2022-07-27'')', TO_SECONDS('2022-07-27'));
insert into func_test3(functionName, result) values('TO_SECONDS(''20220727'')', TO_SECONDS('20220727'));
-- datetime格式
insert into func_test3(functionName, result) values('TO_SECONDS(''2022-07-27 15:25:30'')', TO_SECONDS('2022-07-27 15:25:30'));
insert into func_test3(functionName, result) values('TO_SECONDS(''2022-07-27 15:25:30.8888855'')', TO_SECONDS('2022-07-27 15:25:30.8888855'));
insert into func_test3(functionName, result) values('TO_SECONDS(''20220727152530'')', TO_SECONDS('20220727152530'));
insert into func_test3(functionName, result) values('TO_SECONDS(''2022-07-27 15:25:30.8888854'')', TO_SECONDS('2022-07-27 15:25:30.8888854'));
-- 特异参数测试
-- 不存在的日期或时间
insert into func_test3(functionName, result) values('TO_SECONDS(''2022-07-32'')', TO_SECONDS('2022-07-32'));
insert into func_test3(functionName, result) values('TO_SECONDS(''2022-13-27'')', TO_SECONDS('2022-13-27'));
insert into func_test3(functionName, result) values('TO_SECONDS(''2022-07-27 12:00:61'')', TO_SECONDS('2022-07-27 12:00:61'));
insert into func_test3(functionName, result) values('TO_SECONDS(''2022-07-27 12:61:00'')', TO_SECONDS('2022-07-27 12:61:00'));
insert into func_test3(functionName, result) values('TO_SECONDS(''2022-07-27 25:00:00'')', TO_SECONDS('2022-07-27 25:00:00'));
-- 超大参数
insert into func_test3(functionName, result) values('TO_SECONDS(''99999999999999999999-07-27'')', TO_SECONDS('99999999999999999999-07-27'));
-- 特殊参数类型
insert into func_test3(functionName, result) values('TO_SECONDS(null)', TO_SECONDS(null));
insert into func_test3(functionName, result) values('TO_SECONDS(true)', TO_SECONDS(true));
insert into func_test3(functionName, result) values('TO_SECONDS(date''2022-04-05'')', TO_SECONDS(date'2022-04-05'));
insert into func_test3(functionName, result) values('TO_SECONDS(cast(''2022-04-05 14:35:00'' as datetime))', TO_SECONDS(cast('2022-04-05 14:35:00' as datetime)));
insert into func_test3(functionName, result) values('TO_SECONDS(cast(''2022-04-05 14:35:00.888'' as datetime))', TO_SECONDS(cast('2022-04-05 14:35:00.888' as datetime)));
insert into func_test3(functionName, result) values('TO_SECONDS(time''1:1:1'')', TO_SECONDS(time'1:1:1'));
insert into func_test3(functionName, result) values('TO_SECONDS(time''25:00:00'')', TO_SECONDS(time'25:00:00'));
-- 数值类型参数
insert into func_test3(functionName, result) values('TO_SECONDS(050505)', TO_SECONDS(050505));
insert into func_test3(functionName, result) values('TO_SECONDS(20220801)', TO_SECONDS(20220801));
insert into func_test3(functionName, result) values('TO_SECONDS(20220801182030)', TO_SECONDS(20220801182030));
insert into func_test3(functionName, result) values('TO_SECONDS(20220801182030.8888855)', TO_SECONDS(20220801182030.8888855));
-- 任意分隔符参数
insert into func_test3(functionName, result) values('TO_SECONDS(''0,1,1,0,0,0'')', TO_SECONDS('0,1,1,0,0,0'));
-- 边界测试
-- 最大值
insert into func_test3(functionName, result) values('TO_SECONDS(''9999-12-31'')', TO_SECONDS('9999-12-31'));
insert into func_test3(functionName, result) values('TO_SECONDS(''9999-12-31 23:59:59'')', TO_SECONDS('9999-12-31 23:59:59'));
insert into func_test3(functionName, result) values('TO_SECONDS(''9999-12-31 23:59:59.999999'')', TO_SECONDS('9999-12-31 23:59:59.999999'));
insert into func_test3(functionName, result) values('TO_SECONDS(''10000-01-01'')', TO_SECONDS('10000-01-01'));
-- 最小值
insert into func_test3(functionName, result) values('TO_SECONDS(''0000-01-01'')', TO_SECONDS('0000-01-01'));
insert into func_test3(functionName, result) values('TO_SECONDS(''0000-01-01 00:00:00'')', TO_SECONDS('0000-01-01 00:00:00'));
insert into func_test3(functionName, result) values('TO_SECONDS(''0000-00-00'')', TO_SECONDS('0000-00-00'));
insert into func_test3(functionName, result) values('TO_SECONDS(''0000-00-00 00:00:00'')', TO_SECONDS('0000-00-00 00:00:00'));

set timezone to 'PRC';
-- UNIX_TIMESTAMP()
-- 正常测试
-- date格式
insert into func_test3(functionName, result) values('UNIX_TIMESTAMP(''2022-07-27'')', UNIX_TIMESTAMP('2022-07-27'));
insert into func_test3(functionName, result) values('UNIX_TIMESTAMP(''20220727'')', UNIX_TIMESTAMP('20220727'));
-- datetime格式
insert into func_test3(functionName, result) values('UNIX_TIMESTAMP(''2022-07-27 15:25:30'')', UNIX_TIMESTAMP('2022-07-27 15:25:30'));
insert into func_test3(functionName, result) values('UNIX_TIMESTAMP(''20220727152530'')', UNIX_TIMESTAMP('20220727152530'));
insert into func_test3(functionName, result) values('UNIX_TIMESTAMP(''2022-07-27 15:25:30.8888855'')', UNIX_TIMESTAMP('2022-07-27 15:25:30.8888855'));
insert into func_test3(functionName, result) values('UNIX_TIMESTAMP(''20220727152530.8888854'')', UNIX_TIMESTAMP('20220727152530.8888854'));
-- 特异参数测试
-- 不存在的日期或时间
insert into func_test3(functionName, result) values('UNIX_TIMESTAMP(''2022-07-32'')', UNIX_TIMESTAMP('2022-07-32'));
insert into func_test3(functionName, result) values('UNIX_TIMESTAMP(''2022-13-27'')', UNIX_TIMESTAMP('2022-13-27'));
insert into func_test3(functionName, result) values('UNIX_TIMESTAMP(''2022-07-27 12:00:61'')', UNIX_TIMESTAMP('2022-07-27 12:00:61'));
insert into func_test3(functionName, result) values('UNIX_TIMESTAMP(''2022-07-27 12:61:00'')', UNIX_TIMESTAMP('2022-07-27 12:61:00'));
insert into func_test3(functionName, result) values('UNIX_TIMESTAMP(''2022-07-27 25:00:00'')', UNIX_TIMESTAMP('2022-07-27 25:00:00'));
-- 超大参数
insert into func_test3(functionName, result) values('UNIX_TIMESTAMP(''99999999999999999999-07-27'')', UNIX_TIMESTAMP('99999999999999999999-07-27'));
-- 特殊类型参数
insert into func_test3(functionName, result) values('UNIX_TIMESTAMP(null)', UNIX_TIMESTAMP(null));
insert into func_test3(functionName, result) values('UNIX_TIMESTAMP(true)', UNIX_TIMESTAMP(true));
insert into func_test3(functionName, result) values('UNIX_TIMESTAMP(date''2022-04-05'')', UNIX_TIMESTAMP(date'2022-04-05'));
insert into func_test3(functionName, result) values('UNIX_TIMESTAMP(cast(''2022-04-05 14:35:00'' as datetime))', UNIX_TIMESTAMP(cast('2022-04-05 14:35:00' as datetime)));
insert into func_test3(functionName, result) values('UNIX_TIMESTAMP(cast(''2022-04-05 14:35:00.888'' as datetime))', UNIX_TIMESTAMP(cast('2022-04-05 14:35:00.888' as datetime)));
insert into func_test3(functionName, result) values('UNIX_TIMESTAMP(time''1:1:1'')', UNIX_TIMESTAMP(time'1:1:1'));
insert into func_test3(functionName, result) values('UNIX_TIMESTAMP(time''25:00:00'')', UNIX_TIMESTAMP(time'25:00:00'));
-- 数值类型参数
insert into func_test3(functionName, result) values('UNIX_TIMESTAMP(050505)', UNIX_TIMESTAMP(050505));
insert into func_test3(functionName, result) values('UNIX_TIMESTAMP(20220801)', UNIX_TIMESTAMP(20220801));
insert into func_test3(functionName, result) values('UNIX_TIMESTAMP(20220801182030)', UNIX_TIMESTAMP(20220801182030));
insert into func_test3(functionName, result) values('UNIX_TIMESTAMP(20220801182030.8888855)', UNIX_TIMESTAMP(20220801182030.8888855));
-- 任意分隔符参数
insert into func_test3(functionName, result) values('UNIX_TIMESTAMP(''0,1,1,0,0,0'')', UNIX_TIMESTAMP('0,1,1,0,0,0'));
-- 边界测试
-- 最大值
insert into func_test3(functionName, result) values('UNIX_TIMESTAMP(''2038-01-19'')', UNIX_TIMESTAMP('2038-01-19'));
insert into func_test3(functionName, result) values('UNIX_TIMESTAMP(''2038-01-19 11:14:07'')', UNIX_TIMESTAMP('2038-01-19 11:14:07'));
insert into func_test3(functionName, result) values('UNIX_TIMESTAMP(''2038-01-19 11:14:07.9999'')', UNIX_TIMESTAMP('2038-01-19 11:14:07.9999'));
insert into func_test3(functionName, result) values('UNIX_TIMESTAMP(''2038-01-19 11:14:07.999999999'')', UNIX_TIMESTAMP('2038-01-19 11:14:07.999999999'));
insert into func_test3(functionName, result) values('UNIX_TIMESTAMP(''2038-01-19 11:14:08'')', UNIX_TIMESTAMP('2038-01-19 11:14:08'));
-- 最小值
insert into func_test3(functionName, result) values('UNIX_TIMESTAMP(''1970-01-01'')', UNIX_TIMESTAMP('1970-01-01'));
insert into func_test3(functionName, result) values('UNIX_TIMESTAMP(''1970-01-01 08:00:00'')', UNIX_TIMESTAMP('1970-01-01 08:00:00'));
insert into func_test3(functionName, result) values('UNIX_TIMESTAMP(''1970-01-01 08:00:01'')', UNIX_TIMESTAMP('1970-01-01 08:00:01'));
insert into func_test3(functionName, result) values('UNIX_TIMESTAMP(''1970-01-01 08:00:00.999999'')', UNIX_TIMESTAMP('1970-01-01 08:00:00.999999'));
insert into func_test3(functionName, result) values('UNIX_TIMESTAMP(''1970-01-01 07:59:59'')', UNIX_TIMESTAMP('1970-01-01 07:59:59'));
insert into func_test3(functionName, result) values('UNIX_TIMESTAMP(''1969-12-31'')', UNIX_TIMESTAMP('1969-12-31'));
reset timezone;
show timezone;
-- UTC_DATE
insert into func_test3(functionName, result) values('UTC_DATE', UTC_DATE);
insert into func_test3(functionName, result) values('UTC_DATE()', UTC_DATE());
-- UTC_TIME
insert into func_test3(functionName, result) values('UTC_TIME', UTC_TIME);
insert into func_test3(functionName, result) values('UTC_TIME()', UTC_TIME());
insert into func_test3(functionName, result) values('UTC_TIME(0)', UTC_TIME(0));
insert into func_test3(functionName, result) values('UTC_TIME(1)', UTC_TIME(1));
insert into func_test3(functionName, result) values('UTC_TIME(2)', UTC_TIME(2));
insert into func_test3(functionName, result) values('UTC_TIME(3)', UTC_TIME(3));
insert into func_test3(functionName, result) values('UTC_TIME(4)', UTC_TIME(4));
insert into func_test3(functionName, result) values('UTC_TIME(5)', UTC_TIME(5));
insert into func_test3(functionName, result) values('UTC_TIME(6)', UTC_TIME(6));
insert into func_test3(functionName, result) values('UTC_TIME(-1)', UTC_TIME(-1));
-- UTC_TIMESTAMP
insert into func_test3(functionName, result) values('UTC_TIMESTAMP', UTC_TIMESTAMP);
insert into func_test3(functionName, result) values('UTC_TIMESTAMP()', UTC_TIMESTAMP());
insert into func_test3(functionName, result) values('UTC_TIMESTAMP(0)', UTC_TIMESTAMP(0));
insert into func_test3(functionName, result) values('UTC_TIMESTAMP(1)', UTC_TIMESTAMP(1));
insert into func_test3(functionName, result) values('UTC_TIMESTAMP(2)', UTC_TIMESTAMP(2));
insert into func_test3(functionName, result) values('UTC_TIMESTAMP(3)', UTC_TIMESTAMP(3));
insert into func_test3(functionName, result) values('UTC_TIMESTAMP(4)', UTC_TIMESTAMP(4));
insert into func_test3(functionName, result) values('UTC_TIMESTAMP(5)', UTC_TIMESTAMP(5));
insert into func_test3(functionName, result) values('UTC_TIMESTAMP(6)', UTC_TIMESTAMP(6));
insert into func_test3(functionName, result) values('UTC_TIMESTAMP(-1)', UTC_TIMESTAMP(-1));

select * from func_test3;

select to_seconds('-838:59:59');
select to_seconds(-8385959);

-- 进位和非法值
set dolphin.b_compatibility_mode = true;
set dolphin.sql_mode = 'sql_mode_full_group,pipes_as_concat,ansi_quotes';
create table test1(a time);
insert into test1 values('23:65:66');
select * from test1;
truncate test1;
insert into test1 values('23:65');
select * from test1;
truncate test1;
insert into test1 values('23-65');
select * from test1;
truncate test1;
insert into test1 values('23:59:59');
select * from test1;
truncate test1;
insert into test1 values('23:59:59.8888');
select * from test1;
truncate test1;
insert into test1 values('23:65:66.8888');
select * from test1;
truncate test1;
insert into test1 values('0:0:0');
select * from test1;
truncate test1;
insert into test1 values('-1:-:-1:1');
select * from test1;
truncate test1;
insert into test1 values('23:55:56.1234');
select * from test1;
truncate test1;

select time'23:65:66';
select time'23:65';
select time'23-65';
select time'23:59:59.8888';
select time'23:65:66.8888';
select time'0:0:0';
select time'-1:-1:-1';
select time'23:55:56.1234';
select cast('23:65:66' as time);
select cast('23:65'as time);
select cast('23-65' as time);
select cast('23:59:59.8888' as time);
select cast('23:65:66.8888' as time);
select cast('0:0:0' as time);
select cast('-1:-1:-1' as time);
select cast('23:55:56.1234' as time);
select addtime('12aaa43', '12aa43');
select time('23:65:66');
select time('23:65');
select time('23-65');
select time('23:59:59.8888');
select time('23:65:66.8888');
select time('0:0:0');
select time('-1:-1:-1');
select time('23:55:56.1234');

select cast('5555555' as time);
select cast('55555555' as time);
select cast('555555555' as time);
select cast('5555555555' as time);

select cast('6666666' as time);
select cast('66666666' as time);
select cast('6666666666' as time);

set dolphin.sql_mode = 'sql_mode_strict,sql_mode_full_group,pipes_as_concat,ansi_quotes,no_zero_date';
insert into test1 values('23:65:66');
select * from test1;
truncate test1;
insert into test1 values('23:65');
select * from test1;
truncate test1;
insert into test1 values('23-65');
select * from test1;
truncate test1;
insert into test1 values('23:59:59');
select * from test1;
truncate test1;
insert into test1 values('23:59:59.8888');
select * from test1;
truncate test1;
insert into test1 values('23:65:66.8888');
select * from test1;
truncate test1;
insert into test1 values('0:0:0');
select * from test1;
truncate test1;
insert into test1 values('-1:-:-1:1');
select * from test1;
truncate test1;
insert into test1 values('23:55:56.1234');
select * from test1;
truncate test1;

select time'23:65:66';
select time'23:65';
select time'23-65';
select time'23:59:59.8888';
select time'23:65:66.8888';
select time'0:0:0';
select time'-1:-1:-1';
select time'23:55:56.1234';
select cast('23:65:66' as time);
select cast('23:65'as time);
select cast('23-65' as time);
select cast('23:59:59.8888' as time);
select cast('23:65:66.8888' as time);
select cast('0:0:0' as time);
select cast('-1:-1:-1' as time);
select cast('23:55:56.1234' as time);
select addtime('12aaa43', '12aa43');

-- str_to_date
set dolphin.sql_mode = 'sql_mode_strict,sql_mode_full_group,pipes_as_concat,ansi_quotes,no_zero_date';
select str_to_date('20231103','%Y%m%d');
select str_to_date('2023113','%Y%m%d');
select str_to_date('2023-11-03','%Y-%m-%d');
select str_to_date('2023,11,03','%Y,%m,%d');
select str_to_date('03,11,2023','%d,%m,%Y');
select str_to_date('2023-11-03 17','%Y-%m-%d');
select str_to_date('2023-11-03 17:30:00','%Y-%m-%d');
select str_to_date('2023-13-03','%Y-%m-%d');
select str_to_date('2023-November-3', '%Y-%M-%e');
select str_to_date('November,3,2023', '%M,%e,%Y');
select str_to_date('November 3 2023', '%M %d %Y');
select str_to_date('Friday, November 3, 2023', '%W,%M %e, %Y');
select str_to_date('20231103 173050','%Y%m%d %h%i%s');
select str_to_date('20231103 1730','%Y%m%d %h%i');
select str_to_date('20231103 17','%Y%m%d %h');
select str_to_date('2023-11-03 17:30:00','%Y-%m-%d %H:%i:%s');
select str_to_date('2023,11,03 17:30:10', '%Y,%m,%d %h:%i:%s');
select str_to_date('03,11,2023 dummy','%d,%m,%Y'); 
select str_to_date('20231103 1730 dummy','%Y%m%d %h%i');
select str_to_date('20231103', '%Y%m%d');
select str_to_date('0000-1-1 00:00:00', '%Y-%m-%d');
select str_to_date('2023','%Y');
select str_to_date('0000','%Y');
select str_to_date('9999','%Y');
select str_to_date('11','%h');
select str_to_date('13','%h');
select str_to_date('1730','%h%i');
select str_to_date('173005','%h%i%s');
select str_to_date('November,11,2023', '%M %e %Y');
select str_to_date('Friday, November 3, 2023', '%W %M %e %Y');
select str_to_date('10000-11-03', '%Y-%m-%d');
select str_to_date('202311', '%Y%m%d');
select str_to_date('0000-00-00', '%Y-%m-%d');
select str_to_date('0000-01-00', '%Y-%m-%d');
select str_to_date('0000-00-01', '%Y-%m-%d');
select str_to_date('2023-02-31', '%Y-%m-%d');
SELECT STR_TO_DATE('31/11/22', '%d/%m/%Y');
SELECT STR_TO_DATE('2023-02-31','%Y-%m-%d');
SELECT STR_TO_DATE('2004.09.12 10:61:59','%Y.%m.%d %T');
SELECT STR_TO_DATE('0000-00-00','%Y-%m-%d');


set dolphin.sql_mode = 'sql_mode_strict,sql_mode_full_group,pipes_as_concat,ansi_quotes';
select str_to_date('20231103','%Y%m%d');
select str_to_date('2023113','%Y%m%d');
select str_to_date('2023-11-03','%Y-%m-%d');
select str_to_date('2023,11,03','%Y,%m,%d');
select str_to_date('03,11,2023','%d,%m,%Y');
select str_to_date('2023-11-03 17','%Y-%m-%d');
select str_to_date('2023-11-03 17:30:00','%Y-%m-%d');
select str_to_date('2023-13-03','%Y-%m-%d');
select str_to_date('2023-November-3', '%Y-%M-%e');
select str_to_date('November,3,2023', '%M,%e,%Y');
select str_to_date('November 3 2023', '%M %d %Y');
select str_to_date('Friday, November 3, 2023', '%W,%M %e, %Y');
select str_to_date('20231103 173050','%Y%m%d %h%i%s');
select str_to_date('20231103 1730','%Y%m%d %h%i');
select str_to_date('20231103 17','%Y%m%d %h');
select str_to_date('2023-11-03 17:30:00','%Y-%m-%d %H:%i:%s');
select str_to_date('2023,11,03 17:30:10', '%Y,%m,%d %h:%i:%s');
select str_to_date('03,11,2023 dummy','%d,%m,%Y'); 
select str_to_date('20231103 1730 dummy','%Y%m%d %h%i');
select str_to_date('20231103', '%Y%m%d');
select str_to_date('0000-1-1 00:00:00', '%Y-%m-%d');
select str_to_date('2023','%Y');
select str_to_date('0000','%Y');
select str_to_date('9999','%Y');
select str_to_date('11','%h');
select str_to_date('13','%h');
select str_to_date('1730','%h%i');
select str_to_date('173005','%h%i%s');
select str_to_date('November,11,2023', '%M %e %Y');
select str_to_date('Friday, November 3, 2023', '%W %M %e %Y');
select str_to_date('10000-11-03', '%Y-%m-%d');
select str_to_date('202311', '%Y%m%d');
select str_to_date('0000-00-00', '%Y-%m-%d');
select str_to_date('0000-01-00', '%Y-%m-%d');
select str_to_date('0000-00-01', '%Y-%m-%d');
select str_to_date('2023-02-31', '%Y-%m-%d');
SELECT STR_TO_DATE('31/11/22', '%d/%m/%Y');
SELECT STR_TO_DATE('2023-02-31','%Y-%m-%d');
SELECT STR_TO_DATE('2004.09.12 10:61:59','%Y.%m.%d %T');
SELECT STR_TO_DATE('0000-00-00','%Y-%m-%d');


set dolphin.b_compatibility_mode = false;
select time('23:65:66');
select time('23:65');
select time('23-65');
select time('23:59:59.8888');
select time('23:65:66.8888');
select time('0:0:0');
select time('-1:-1:-1');
select time('23:55:56.1234');

set dolphin.b_compatibility_mode = true;
create table test_time(t time);
insert into test_time values ('11:12:23.123456');
insert into test_time values ('22:11:33');
insert into test_time values ('-22:11:33');

select date_format(t, '%H%i%s.%f') from test_time; 
select to_char(t, 'hh24miss') from test_time;

drop table test_time;

set dolphin.b_compatibility_mode = true;
set dolphin.sql_mode='';
create table t1(c1 int1, c2 int2, c3 int4, c4 int8, c5 uint1, c6 uint2, c7 uint8);
create table t2(c1 time, c2 time, c3 time, c4 time, c5 time, c6 time, c7 time);
insert into t1 values('-69', '-32769', '-32769', '-32769', '69', '32769', '32769');
insert into t2  select c1, c2, c3, c4, c5, c6, c7 from t1;
reset dolphin.sql_mode;
insert ignore into t2  select c1, c2, c3, c4, c5, c6, c7 from t1;
select * from t2;
drop table if exists t1;
drop table if exists t2;

-- 0 day or 0 month
select cast(10000 as datetime);
select cast(10100 as datetime);
select cast(10001 as datetime);
select cast(10000 as timestamp with time zone);
select cast(10100 as timestamp with time zone);
select cast(10001 as timestamp with time zone);

select cast('10000' as datetime);
select cast('10100' as datetime);
select cast('10001' as datetime);
select cast('10000' as timestamp with time zone);
select cast('10100' as timestamp with time zone);
select cast('10001' as timestamp with time zone);

set dolphin.b_compatibility_mode = true;
select cast(3.1415926 as datetime);
select cast(3.1415926::float8 as datetime);
select cast('3.1415926' as datetime);
select cast(3.1415926 as timestamp with time zone);
select cast(3.1415926::float8 as timestamp with time zone);
select cast('3.1415926' as timestamp with time zone);

create table t1(a datetime, b timestamp with time zone);
set dolphin.sql_mode = 'sql_mode_full_group,pipes_as_concat,ansi_quotes';
insert into t1 values ('3.1415926', '3.1415926');
select * from t1;
drop table if exists t1;


drop schema b_time_funcs3 cascade;
reset current_schema;

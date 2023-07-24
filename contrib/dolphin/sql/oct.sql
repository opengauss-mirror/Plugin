create schema db_oct;
set current_schema to 'db_oct';

-- 测试正常数字十进制转八进制
SELECT OCT(10);

-- 测试边界
SELECT OCT(18446744073709551614);
SELECT OCT(18446744073709551615);
SELECT OCT(18446744073709551616);
SELECT OCT(-18446744073709551615);
SELECT OCT(-18446744073709551616);
SELECT OCT(-18446744073709551617);

-- 测试小数、分数
SELECT OCT('9.2');
SELECT OCT('9.6');
SELECT OCT('-9.2');
SELECT OCT('-9.6');

SELECT OCT(9.2);
SELECT OCT(9.6);
SELECT OCT(-9.2);
SELECT OCT(-9.6);
select oct(-13/4);

-- 测试特殊符号
SELECT OCT('+19.6');
SELECT OCT('a9');
SELECT OCT('!9');
SELECT OCT('@#9');

-- 测试正常数字十进制转八进制
SELECT OCT('10');

-- 测试边界
SELECT OCT('18446744073709551614');
SELECT OCT('18446744073709551615');
SELECT OCT('18446744073709551616');
SELECT OCT('-18446744073709551615');
SELECT OCT('-18446744073709551616');
SELECT OCT('-18446744073709551617');

-- 测试输入bool值
SELECT OCT(True);
SELECT OCT(False);

-- 测试输入日期类型
set datestyle = 'ISO, MDY';
SELECT OCT('2023-12-31'::date);
SELECT OCT('2023-12-31'::datetime);
SELECT OCT('2023-12-31'::timestamp);
SELECT OCT('12:34:56'::time);

SELECT CONV('2023-12-31'::date, 10, 8);
SELECT CONV('2023-12-31'::datetime, 10, 8);
SELECT CONV('2023-12-31'::timestamp, 10, 8);
SELECT CONV('12:34:56'::time, 10, 8);
reset datestyle;

-- 测试处理表格
drop table if exists test_oct;
create table test_oct (name text);
insert into test_oct values('10'),('11');
select oct(name) from test_oct;
drop table if exists test_oct;

drop schema db_oct cascade;
reset current_schema;
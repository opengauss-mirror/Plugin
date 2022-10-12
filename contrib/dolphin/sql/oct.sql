drop database if exists db_oct;
create database db_oct dbcompatibility 'b';
\c db_oct

-- 测试正常数字十进制转八进制
SELECT OCT(10);

-- 测试边界
SELECT OCT(18446744073709551614);
SELECT OCT(18446744073709551615);
SELECT OCT(18446744073709551616);
SELECT OCT(-18446744073709551615);
SELECT OCT(-18446744073709551616);
SELECT OCT(-18446744073709551617);

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

-- 测试处理表格
drop table if exists test_oct;
create table test_oct (name text);
insert into test_oct values('10'),('11');
select oct(name) from test_oct;
drop table if exists test_oct;

\c postgres
drop database if exists db_oct;
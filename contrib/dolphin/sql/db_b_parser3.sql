drop database if exists mysql_test;
create database mysql_test dbcompatibility 'b';
\c mysql_test
create extension dolphin;

--测试点一：验证lcase函数
select lcase('ABc'), lcase('哈哈'), lcase('123456'),lcase('哈市&%%￥#'),lcase(null);

--测试点二：验证lower函数
select lower('ABc'), lower('哈哈'), lower('123456'),lower('哈市&%%￥#'),lower(null);

--测试点三：验证ucase函数
select ucase('ABc'), ucase('哈哈'), ucase('123456'),ucase('哈市&%%￥#'),ucase(null);
select upper('ABc'), upper('哈哈'), upper('123456'),upper('哈市&%%￥#'),upper(null);
--测试点四：验证rand和random函数
-- select rand(), random();--不显示小数点前的0
-- set behavior_compat_options := 'display_leading_zero';
-- select rand(), random();--显示小数点前的0
--测试点五：验证truncate函数
select truncate(111.28);--返回111
select truncate(111.28,0);--返回111
select truncate(111.28,1);--返回111.2
select truncate(111.28,5);
select truncate(111.28,500);
select truncate(111.28,-1);--返回110
select truncate(111.28,-4);--返回0
select  trunc(111.28),  trunc(111.28,0),  trunc(111.28,1),   trunc(111.28,5),   trunc(111.28,500),   trunc(111.28,-1),    trunc(111.28,-4);
--测试点六：验证current_date

--测试点一：验证:=操作符
--step4:建表;expect:创建成功
drop table if exists tb_db_b_parser0003;
create table tb_db_b_parser0003(a1 int, a2 int);
insert into tb_db_b_parser0003 select generate_series(1,5);
--step5:使用:=(赋值)操作符修改a1列值;
update tb_db_b_parser0003 set a1 := 11;
--step6:使用:=(赋值)操作符同时修改a1,a2列值;
update tb_db_b_parser0003 set (a1,a2) := (12,13);
--step7:使用:=(赋值)操作符,修改参数值;
show io_limits; --默认是0
set io_limits := 100;
--alter system set设置参数值
alter system set pagewriter_thread_num := 5;

--测试点二：验证div操作符
select 8div3;--8
select 8 div3;--结果为8
select 8 div 3;
select 8 div 0;

select 8div 3, 8div 3div2, 8div 3div 2;

select 8 div;

select 8 as div;

select div 1;

select '-12.3abc' div null;
select '-12.3abc' div -100.1;
select '-12.3abc' div 0;
select '-12.3abc' div 5;
select '-12.3abc' div 158.3;
select '-12.3abc' div -8.222e4;
select '-12.3abc' div true;
select '-12.3abc' div false;
select '-12.3abc' div 'null';
select 123456 div 5 div 4;
select 8 div 1 where 100 div 3 div 4 = 0;
select 8 div 3 where 100 div 3 div 4 > 0;

--测试点三：验证mod操作符
select 8mod3;--返回mod3 8
select 8 mod3;--返回mod3 8
select 8 mod 3; 
select 8 mod 0;
select 8mod 3, 8mod 3mod2, 8mod 3mod 2;
select 8 mod;--返回8
select 8 as mod;--返回mod 8
select mod 1;

select '-12.3abc' mod null;
select '-12.3abc' mod -100.1;
select '-12.3abc' mod 0;
select '-12.3abc' mod 5;
select '-12.3abc' mod 158.3;
select '-12.3abc' mod -8.222e4;
select '-12.3abc' mod true;
select '-12.3abc' mod false;
select '-12.3abc' mod 'null';
select 123456 mod 5 mod 4;
select 8 mod 1 where 100 mod 3 mod 4 = 0;
select 8 mod 3 where 100 mod 3 mod 4 > 0;

--测试点四：验证xor操作符

select xor1;

select 1 xor 1;
select 1 xor null;
select null xor 1;
select 1 xor 0;
select '-12.3abc' xor null;
select '-12.3abc' xor -100.1;
select '-12.3abc' xor 0;
select '-12.3abc' xor 5;
select '-12.3abc' xor 158.3;
select '-12.3abc' xor -8.222e4;
select '-12.3abc' xor true;
select '-12.3abc' xor false;
select '-12.3abc' xor 'null';


\c postgres
drop database if exists mysql_test;
create schema db_b_parser3;
set current_schema to 'db_b_parser3';

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

-- without binary as numeric
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

set dolphin.b_compatibility_mode to on;
create table tmp_res(a int);
create table div_test(
c1 int1,
c2 int2,
c3 int4,
c4 int8,
c5 uint1,
c6 uint2,
c7 uint4,
c8 uint8,
c9 float,
c10 double,
c11 numeric,
c12 bit,
c13 binary,
c14 interval,
c15 json);
insert into div_test values(0,0,0,0,0,0,0,0,0,0,0,b'0',0,0,'{"0":"0"}');
-- strict mode only, no warning and error
set dolphin.sql_mode = 'sql_mode_strict';
select c1/c1 from div_test;
select c1/c2 from div_test;
select c1/c3 from div_test;
select c1/c4 from div_test;
select c1/c5 from div_test;
select c1/c6 from div_test;
select c1/c7 from div_test;
select c1/c8 from div_test;
select c1/c9 from div_test;
select c1/c10 from div_test;
select c1/c11 from div_test;
select c1/c12 from div_test;
select c1/c13 from div_test;
select c1/c14 from div_test;
select c1/c15 from div_test;
select c1 mod c1 from div_test;
select c1 mod c2 from div_test;
select c1 mod c3 from div_test;
select c1 mod c4 from div_test;
select c1 mod c5 from div_test;
select c1 mod c6 from div_test;
select c1 mod c7 from div_test;
select c1 mod c8 from div_test;
select c1 mod c9 from div_test;
select c1 mod c10 from div_test;
select c1 mod c11 from div_test;
select c1 mod c12 from div_test;
select c1 mod c13 from div_test;
select c1 mod c14 from div_test;
select c1 mod c15 from div_test;
select c1 div c1 from div_test;
select c1 div c2 from div_test;
select c1 div c3 from div_test;
select c1 div c4 from div_test;
select c1 div c5 from div_test;
select c1 div c6 from div_test;
select c1 div c7 from div_test;
select c1 div c8 from div_test;
select c1 div c9 from div_test;
select c1 div c10 from div_test;
select c1 div c11 from div_test;
select c1 div c12 from div_test;
select c1 div c13 from div_test;
select c1 div c14 from div_test;
select c1 div c15 from div_test;

insert into tmp_res select c1/c1 from div_test;
insert into tmp_res select c1/c2 from div_test;
insert into tmp_res select c1/c3 from div_test;
insert into tmp_res select c1/c4 from div_test;
insert into tmp_res select c1/c5 from div_test;
insert into tmp_res select c1/c6 from div_test;
insert into tmp_res select c1/c7 from div_test;
insert into tmp_res select c1/c8 from div_test;
insert into tmp_res select c1/c9 from div_test;
insert into tmp_res select c1/c10 from div_test;
insert into tmp_res select c1/c11 from div_test;
insert into tmp_res select c1/c12 from div_test;
insert into tmp_res select c1/c13 from div_test;
insert into tmp_res select c1/c14 from div_test;
insert into tmp_res select c1/c15 from div_test;
insert into tmp_res select c1 mod c1 from div_test;
insert into tmp_res select c1 mod c2 from div_test;
insert into tmp_res select c1 mod c3 from div_test;
insert into tmp_res select c1 mod c4 from div_test;
insert into tmp_res select c1 mod c5 from div_test;
insert into tmp_res select c1 mod c6 from div_test;
insert into tmp_res select c1 mod c7 from div_test;
insert into tmp_res select c1 mod c8 from div_test;
insert into tmp_res select c1 mod c9 from div_test;
insert into tmp_res select c1 mod c10 from div_test;
insert into tmp_res select c1 mod c11 from div_test;
insert into tmp_res select c1 mod c12 from div_test;
insert into tmp_res select c1 mod c13 from div_test;
insert into tmp_res select c1 mod c14 from div_test;
insert into tmp_res select c1 mod c15 from div_test;
insert into tmp_res select c1 div c1 from div_test;
insert into tmp_res select c1 div c2 from div_test;
insert into tmp_res select c1 div c3 from div_test;
insert into tmp_res select c1 div c4 from div_test;
insert into tmp_res select c1 div c5 from div_test;
insert into tmp_res select c1 div c6 from div_test;
insert into tmp_res select c1 div c7 from div_test;
insert into tmp_res select c1 div c8 from div_test;
insert into tmp_res select c1 div c9 from div_test;
insert into tmp_res select c1 div c10 from div_test;
insert into tmp_res select c1 div c11 from div_test;
insert into tmp_res select c1 div c12 from div_test;
insert into tmp_res select c1 div c13 from div_test;
insert into tmp_res select c1 div c14 from div_test;
insert into tmp_res select c1 div c15 from div_test;

-- non-strict mode and non-err_division_by_zero, no warning and error
set dolphin.sql_mode = '';
select c1/c1 from div_test;
select c1/c2 from div_test;
select c1/c3 from div_test;
select c1/c4 from div_test;
select c1/c5 from div_test;
select c1/c6 from div_test;
select c1/c7 from div_test;
select c1/c8 from div_test;
select c1/c9 from div_test;
select c1/c10 from div_test;
select c1/c11 from div_test;
select c1/c12 from div_test;
select c1/c13 from div_test;
select c1/c14 from div_test;
select c1/c15 from div_test;
select c1 mod c1 from div_test;
select c1 mod c2 from div_test;
select c1 mod c3 from div_test;
select c1 mod c4 from div_test;
select c1 mod c5 from div_test;
select c1 mod c6 from div_test;
select c1 mod c7 from div_test;
select c1 mod c8 from div_test;
select c1 mod c9 from div_test;
select c1 mod c10 from div_test;
select c1 mod c11 from div_test;
select c1 mod c12 from div_test;
select c1 mod c13 from div_test;
select c1 mod c14 from div_test;
select c1 mod c15 from div_test;
select c1 div c1 from div_test;
select c1 div c2 from div_test;
select c1 div c3 from div_test;
select c1 div c4 from div_test;
select c1 div c5 from div_test;
select c1 div c6 from div_test;
select c1 div c7 from div_test;
select c1 div c8 from div_test;
select c1 div c9 from div_test;
select c1 div c10 from div_test;
select c1 div c11 from div_test;
select c1 div c12 from div_test;
select c1 div c13 from div_test;
select c1 div c14 from div_test;
select c1 div c15 from div_test;

insert into tmp_res select c1/c1 from div_test;
insert into tmp_res select c1/c2 from div_test;
insert into tmp_res select c1/c3 from div_test;
insert into tmp_res select c1/c4 from div_test;
insert into tmp_res select c1/c5 from div_test;
insert into tmp_res select c1/c6 from div_test;
insert into tmp_res select c1/c7 from div_test;
insert into tmp_res select c1/c8 from div_test;
insert into tmp_res select c1/c9 from div_test;
insert into tmp_res select c1/c10 from div_test;
insert into tmp_res select c1/c11 from div_test;
insert into tmp_res select c1/c12 from div_test;
insert into tmp_res select c1/c13 from div_test;
insert into tmp_res select c1/c14 from div_test;
insert into tmp_res select c1/c15 from div_test;
insert into tmp_res select c1 mod c1 from div_test;
insert into tmp_res select c1 mod c2 from div_test;
insert into tmp_res select c1 mod c3 from div_test;
insert into tmp_res select c1 mod c4 from div_test;
insert into tmp_res select c1 mod c5 from div_test;
insert into tmp_res select c1 mod c6 from div_test;
insert into tmp_res select c1 mod c7 from div_test;
insert into tmp_res select c1 mod c8 from div_test;
insert into tmp_res select c1 mod c9 from div_test;
insert into tmp_res select c1 mod c10 from div_test;
insert into tmp_res select c1 mod c11 from div_test;
insert into tmp_res select c1 mod c12 from div_test;
insert into tmp_res select c1 mod c13 from div_test;
insert into tmp_res select c1 mod c14 from div_test;
insert into tmp_res select c1 mod c15 from div_test;
insert into tmp_res select c1 div c1 from div_test;
insert into tmp_res select c1 div c2 from div_test;
insert into tmp_res select c1 div c3 from div_test;
insert into tmp_res select c1 div c4 from div_test;
insert into tmp_res select c1 div c5 from div_test;
insert into tmp_res select c1 div c6 from div_test;
insert into tmp_res select c1 div c7 from div_test;
insert into tmp_res select c1 div c8 from div_test;
insert into tmp_res select c1 div c9 from div_test;
insert into tmp_res select c1 div c10 from div_test;
insert into tmp_res select c1 div c11 from div_test;
insert into tmp_res select c1 div c12 from div_test;
insert into tmp_res select c1 div c13 from div_test;
insert into tmp_res select c1 div c14 from div_test;
insert into tmp_res select c1 div c15 from div_test;

-- strict mode and err_division_by_zero, select warning, insert error
set dolphin.sql_mode = 'sql_mode_strict,ERROR_FOR_DIVISION_BY_ZERO';
select c1/c1 from div_test;
select c1/c2 from div_test;
select c1/c3 from div_test;
select c1/c4 from div_test;
select c1/c5 from div_test;
select c1/c6 from div_test;
select c1/c7 from div_test;
select c1/c8 from div_test;
select c1/c9 from div_test;
select c1/c10 from div_test;
select c1/c11 from div_test;
select c1/c12 from div_test;
select c1/c13 from div_test;
select c1/c14 from div_test;
select c1/c15 from div_test;
select c1 mod c1 from div_test;
select c1 mod c2 from div_test;
select c1 mod c3 from div_test;
select c1 mod c4 from div_test;
select c1 mod c5 from div_test;
select c1 mod c6 from div_test;
select c1 mod c7 from div_test;
select c1 mod c8 from div_test;
select c1 mod c9 from div_test;
select c1 mod c10 from div_test;
select c1 mod c11 from div_test;
select c1 mod c12 from div_test;
select c1 mod c13 from div_test;
select c1 mod c14 from div_test;
select c1 mod c15 from div_test;
select c1 div c1 from div_test;
select c1 div c2 from div_test;
select c1 div c3 from div_test;
select c1 div c4 from div_test;
select c1 div c5 from div_test;
select c1 div c6 from div_test;
select c1 div c7 from div_test;
select c1 div c8 from div_test;
select c1 div c9 from div_test;
select c1 div c10 from div_test;
select c1 div c11 from div_test;
select c1 div c12 from div_test;
select c1 div c13 from div_test;
select c1 div c14 from div_test;
select c1 div c15 from div_test;

insert into tmp_res select c1/c1 from div_test;
insert into tmp_res select c1/c2 from div_test;
insert into tmp_res select c1/c3 from div_test;
insert into tmp_res select c1/c4 from div_test;
insert into tmp_res select c1/c5 from div_test;
insert into tmp_res select c1/c6 from div_test;
insert into tmp_res select c1/c7 from div_test;
insert into tmp_res select c1/c8 from div_test;
insert into tmp_res select c1/c9 from div_test;
insert into tmp_res select c1/c10 from div_test;
insert into tmp_res select c1/c11 from div_test;
insert into tmp_res select c1/c12 from div_test;
insert into tmp_res select c1/c13 from div_test;
insert into tmp_res select c1/c14 from div_test;
insert into tmp_res select c1/c15 from div_test;
insert into tmp_res select c1 mod c1 from div_test;
insert into tmp_res select c1 mod c2 from div_test;
insert into tmp_res select c1 mod c3 from div_test;
insert into tmp_res select c1 mod c4 from div_test;
insert into tmp_res select c1 mod c5 from div_test;
insert into tmp_res select c1 mod c6 from div_test;
insert into tmp_res select c1 mod c7 from div_test;
insert into tmp_res select c1 mod c8 from div_test;
insert into tmp_res select c1 mod c9 from div_test;
insert into tmp_res select c1 mod c10 from div_test;
insert into tmp_res select c1 mod c11 from div_test;
insert into tmp_res select c1 mod c12 from div_test;
insert into tmp_res select c1 mod c13 from div_test;
insert into tmp_res select c1 mod c14 from div_test;
insert into tmp_res select c1 mod c15 from div_test;
insert into tmp_res select c1 div c1 from div_test;
insert into tmp_res select c1 div c2 from div_test;
insert into tmp_res select c1 div c3 from div_test;
insert into tmp_res select c1 div c4 from div_test;
insert into tmp_res select c1 div c5 from div_test;
insert into tmp_res select c1 div c6 from div_test;
insert into tmp_res select c1 div c7 from div_test;
insert into tmp_res select c1 div c8 from div_test;
insert into tmp_res select c1 div c9 from div_test;
insert into tmp_res select c1 div c10 from div_test;
insert into tmp_res select c1 div c11 from div_test;
insert into tmp_res select c1 div c12 from div_test;
insert into tmp_res select c1 div c13 from div_test;
insert into tmp_res select c1 div c14 from div_test;
insert into tmp_res select c1 div c15 from div_test;

-- non-strict mode and err_division_by_zero, select warning, insert warning
set dolphin.sql_mode = 'ERROR_FOR_DIVISION_BY_ZERO';
select c1/c1 from div_test;
select c1/c2 from div_test;
select c1/c3 from div_test;
select c1/c4 from div_test;
select c1/c5 from div_test;
select c1/c6 from div_test;
select c1/c7 from div_test;
select c1/c8 from div_test;
select c1/c9 from div_test;
select c1/c10 from div_test;
select c1/c11 from div_test;
select c1/c12 from div_test;
select c1/c13 from div_test;
select c1/c14 from div_test;
select c1/c15 from div_test;
select c1 mod c1 from div_test;
select c1 mod c2 from div_test;
select c1 mod c3 from div_test;
select c1 mod c4 from div_test;
select c1 mod c5 from div_test;
select c1 mod c6 from div_test;
select c1 mod c7 from div_test;
select c1 mod c8 from div_test;
select c1 mod c9 from div_test;
select c1 mod c10 from div_test;
select c1 mod c11 from div_test;
select c1 mod c12 from div_test;
select c1 mod c13 from div_test;
select c1 mod c14 from div_test;
select c1 mod c15 from div_test;
select c1 div c1 from div_test;
select c1 div c2 from div_test;
select c1 div c3 from div_test;
select c1 div c4 from div_test;
select c1 div c5 from div_test;
select c1 div c6 from div_test;
select c1 div c7 from div_test;
select c1 div c8 from div_test;
select c1 div c9 from div_test;
select c1 div c10 from div_test;
select c1 div c11 from div_test;
select c1 div c12 from div_test;
select c1 div c13 from div_test;
select c1 div c14 from div_test;
select c1 div c15 from div_test;

insert into tmp_res select c1/c1 from div_test;
insert into tmp_res select c1/c2 from div_test;
insert into tmp_res select c1/c3 from div_test;
insert into tmp_res select c1/c4 from div_test;
insert into tmp_res select c1/c5 from div_test;
insert into tmp_res select c1/c6 from div_test;
insert into tmp_res select c1/c7 from div_test;
insert into tmp_res select c1/c8 from div_test;
insert into tmp_res select c1/c9 from div_test;
insert into tmp_res select c1/c10 from div_test;
insert into tmp_res select c1/c11 from div_test;
insert into tmp_res select c1/c12 from div_test;
insert into tmp_res select c1/c13 from div_test;
insert into tmp_res select c1/c14 from div_test;
insert into tmp_res select c1/c15 from div_test;
insert into tmp_res select c1 mod c1 from div_test;
insert into tmp_res select c1 mod c2 from div_test;
insert into tmp_res select c1 mod c3 from div_test;
insert into tmp_res select c1 mod c4 from div_test;
insert into tmp_res select c1 mod c5 from div_test;
insert into tmp_res select c1 mod c6 from div_test;
insert into tmp_res select c1 mod c7 from div_test;
insert into tmp_res select c1 mod c8 from div_test;
insert into tmp_res select c1 mod c9 from div_test;
insert into tmp_res select c1 mod c10 from div_test;
insert into tmp_res select c1 mod c11 from div_test;
insert into tmp_res select c1 mod c12 from div_test;
insert into tmp_res select c1 mod c13 from div_test;
insert into tmp_res select c1 mod c14 from div_test;
insert into tmp_res select c1 mod c15 from div_test;
insert into tmp_res select c1 div c1 from div_test;
insert into tmp_res select c1 div c2 from div_test;
insert into tmp_res select c1 div c3 from div_test;
insert into tmp_res select c1 div c4 from div_test;
insert into tmp_res select c1 div c5 from div_test;
insert into tmp_res select c1 div c6 from div_test;
insert into tmp_res select c1 div c7 from div_test;
insert into tmp_res select c1 div c8 from div_test;
insert into tmp_res select c1 div c9 from div_test;
insert into tmp_res select c1 div c10 from div_test;
insert into tmp_res select c1 div c11 from div_test;
insert into tmp_res select c1 div c12 from div_test;
insert into tmp_res select c1 div c13 from div_test;
insert into tmp_res select c1 div c14 from div_test;
insert into tmp_res select c1 div c15 from div_test;
select count(*) from tmp_res;
select count(*) from tmp_res where a is null;
drop table tmp_res;
drop table div_test;

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

create table t1 (a int, b varchar(20));
insert into t1 values(1, 'true');

select a = 1 xor b = 'true' from t1;

select acos(11);
select acos(1.000001);
select acos(-1.000001);

drop schema db_b_parser3 cascade;
reset current_schema;
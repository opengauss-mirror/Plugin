create schema db_b_parser2;
set current_schema to 'db_b_parser2';
--验证DAYOFMONTH() DAYOFWEEK() DAYOFYEAR() HOUR() MICROSECOND() MINUTE() QUARTER() SECOND() WEEKDAY() WEEKOFYEAR() YEAR()
select DAYOFMONTH(datetime '2021-11-4 16:30:44.341191');

select DAYOFWEEK(datetime '2021-11-4 16:30:44.341191');

select DAYOFYEAR(datetime '2021-12-31 16:30:44.341191');

select HOUR(datetime '2021-11-4 16:30:44.341191');

select MICROSECOND(datetime '2021-11-4 16:30:44.341191');

select MINUTE(datetime '2021-11-4 16:30:44.341191');

select QUARTER(datetime '2021-11-4 16:30:44.341191');

select SECOND(datetime '2021-11-4 16:30:44.341191');

select WEEKDAY(datetime '2021-11-4 16:30:44.341191');

select WEEKOFYEAR(datetime '2021-11-4 16:30:44.341191');

select YEAR(datetime '2021-11-4 16:30:44.341191');

SELECT dayofmonth('2022-10-09 09:55:28.12345');
SELECT dayofweek('2022-10-09 09:55:28.12345');
SELECT dayofyear('2022-10-09 09:55:28.12345');
SELECT hour('2022-10-09 09:55:28.12345');
SELECT microsecond('2022-10-09 09:55:28.12345');
SELECT minute('2022-10-09 09:55:28.12345');
SELECT quarter('2022-10-09 09:55:28.12345');
SELECT second('2022-10-09 09:55:28.12345');
SELECT weekday('2022-10-09 09:55:28.12345');
SELECT weekofyear('2022-10-09 09:55:28.12345');
select now(),dayofmonth(now()),dayofmonth('2022-06-13');
select now(),dayofyear(now()),dayofyear('2022-06-13');

select second('10:20:30');
select second('30:20:10.123456');
select second('2022-10-09 11:22:33');
select second('2022-10-09 11:22:33.456789');
select second('20221009112233');
select second('373839');
select second('2022-10-09');

select minute('10:20:30');
select minute('30:20:10.123456');
select minute('2022-10-09 11:22:33');
select minute('2022-10-09 11:22:33.456789');
select minute('20221009112233');
select minute('373839');
select minute('2022-10-09');

select microsecond('10:20:30');
select microsecond('30:20:10.123456');
select microsecond('2022-10-09 11:22:33');
select microsecond('2022-10-09 11:22:33.456789');
select microsecond('20221009112233');
select microsecond('373839');
select microsecond('2022-10-09');

select hour('10:20:30');
select hour('30:20:10.123456');
select hour('2022-10-09 11:22:33');
select hour('2022-10-09 11:22:33.456789');
select hour('20221009112233');
select hour('373839');
select hour('2022-10-09');

SELECT dayofmonth(20221009);
SELECT dayofweek(20221009);
SELECT dayofyear(20221009);
SELECT quarter(20221009);
SELECT weekday(20221009);
SELECT weekofyear(20221009);

SELECT dayofmonth('');
SELECT day('');
SELECT dayofmonth(true);
SELECT day(true);
SELECT dayofmonth(false);
SELECT day(false);

select year('');
select year('2022');
select year('2022-06');
select year('2022-06-13');
select year('9999-06-13');
select year('10000-06-13');
select year(20221009);

--测试点：LOCATE(substr,str)
select locate('test','testgood');
select locate('','testgood');
select locate(NULL,'testgood');
select locate(true,'testgood');
select locate(false,'testgood');

--测试点：LOCATE(substr,str,pos)
select locate('test','11testgood',5);
select locate('test','11testgood',3);

--测试点：IFNULL
SELECT IFNULL(NULL, 'test');
SELECT IFNULL('test', NULL);
SELECT IFNULL(NULL, NULL);
SELECT IFNULL('test', '');
SELECT IFNULL('', 'test');

--测试点：IF(expr1,expr2,expr3)

SELECT IF(TRUE,'A','B');
SELECT IF(1,'A','B');
SELECT IF(12.35,'A','B');
SELECT IF(-12,'A','B');
SELECT IF('12.35abc','A','B');
SELECT IF('打上卡','A','B');


SELECT IF(FALSE,'A','B');
SELECT IF(NULL,'A','B');
SELECT IF(0,'A','B');
SELECT IF(NULL,'A','B');
SELECT IF('','A','B');
SELECT IF('12.25abc','A','B');

--测试点：验证bit_length（返回位数）
select * from pg_proc where proname in ('db_b_parser_bit_length');
--返回8、8、16
select bit_length(b'1'), bit_length(b'101'), bit_length(b'1011111111');
--返回8、24、80
select bit_length('1'), bit_length('101'), bit_length('1011111111');
--返回72、16、24
select bit_length('哈1哈\n'),bit_length('\n'),bit_length('\\n');
--返回0、8、空值
select bit_length(''),bit_length(' '),bit_length(NULL);

--测试点：验证octet_length（返回字节数）
select * from pg_proc where proname in ('db_b_parser_octet_length');
--返回1、1、2
select octet_length(b'1'), octet_length(b'101'), octet_length(b'1011111111');
--返回1、3、10
select octet_length('1'), octet_length('101'), octet_length('1011111111');
--返回9、2、3
select octet_length('哈1哈\n'),octet_length('\n'),octet_length('\\n');--一个汉字3个字节；数字、字母、特殊字符一个字节
--返回0、8、空值
select octet_length(''),bit_length(' '),bit_length(NULL);

--测试点：验证length（返回字符数）
select * from pg_proc where proname in ('db_b_parser_length');
--返回1、1、2
select length(b'1'), length(b'101'), length(b'1011111111');
--返回1、3、10
select length('1'), length('101'), length('1011111111');
--返回8、1、2
select length(E'哈1哈\n'),length(E'\n'),length(E'\\n');
--返回0、1、空
select length(''),length(' '),length(NULL);

set dolphin.sql_mode='treat_bxconst_as_binary';
create table t_binary(a binary, b varbinary(10), c binary(10));
insert into t_binary values(null,null,null);
insert into t_binary values(b'1',b'111',b'111');
insert into t_binary values(b'1',b'11111',b'111');
insert into t_binary values(b'1',b'111111111',b'111');
select *,length(a),length(b),length(c) from t_binary order by 1,2,3;
drop table t_binary;
reset dolphin.sql_mode;

create table fchar_test(fchar char(10));
insert into fchar_test values('零一二三四五六七八九');
insert into fchar_test values('零一二三四五六七八九0');
insert into fchar_test values('零一二三四五六七八九十');
select fchar,length(fchar) from fchar_test order by 1,2;
drop table fchar_test;

drop schema db_b_parser2 cascade;
reset current_schema;

drop database if exists db_char_latin1;
create database db_char_latin1 dbcompatibility 'B' encoding 'latin1' LC_CTYPE 'en_US' lc_collate 'en_US';
\c db_char_latin1
\encoding latin1;
create table t1(a character(4));
insert into t1 values('四个字符');
insert into t1 values('四个字0');
insert into t1 values('一二三四五');
insert into t1 values('四个字01');

\c postgres
drop database if exists db_char_latin1;
drop database if exists db_char_utf8;
create database db_char_utf8 dbcompatibility 'b' encoding 'utf8';
\c db_char_utf8
\encoding utf8;
create table t1(a character(4));
insert into t1 values('四个字符');
insert into t1 values('四个字0');
insert into t1 values('一二三四五');
insert into t1 values('四个字01');


\c postgres
drop database if exists db_char_utf8;
drop database if exists db_char_ascii;
create database db_char_ascii dbcompatibility 'b' encoding 'sql_ascii';
\c db_char_ascii
\encoding sql_ascii;
create table t1(a character(4));
insert into t1 values('四个字符');
insert into t1 values('四个字0');
insert into t1 values('一二三四五');
insert into t1 values('四个字01');

\c postgres
drop database if exists db_char_ascii;
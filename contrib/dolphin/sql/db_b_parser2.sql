drop database if exists mysql_test;
create database mysql_test dbcompatibility 'b';
\c mysql_test
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

\c postgres
drop database if exists mysql_test;
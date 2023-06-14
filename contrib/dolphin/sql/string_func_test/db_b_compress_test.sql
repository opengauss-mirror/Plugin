create schema db_b_compress_test;
set current_schema to 'db_b_compress_test';
set dolphin.sql_mode = '';
-- compress测试用例
-- 基准测试
select length(compress('string for test compress function aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa '))<length('string for test compress function aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa ');

create table t1 (a binary(255), b char(4));
insert into t1 (a,b) values (compress('string for test compress function aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa '),'b ');

SELECT HEX(COMPRESS('2022-05-12 10:30:00'));
SELECT HEX(COMPRESS(E'\\x01020304'));
SELECT HEX(COMPRESS(true));
SELECT HEX(COMPRESS(123.45));
SELECT HEX(COMPRESS(-123.45));

-- literal写法测试
SELECT HEX(COMPRESS('123456789'::integer));
SELECT HEX(COMPRESS('123456789.0'::real));
SELECT HEX(COMPRESS('123456789.0'::double precision));
SELECT HEX(COMPRESS('1'::boolean));
SELECT HEX(COMPRESS('123456789'::text));
SELECT HEX(COMPRESS('123456789'::bytea));
SELECT HEX(COMPRESS(123456789::bit(32)));
SELECT HEX(COMPRESS('2023-06-01 00:23:59'::clob));
SELECT HEX(COMPRESS('string for test compress function '::name));
SELECT HEX(COMPRESS('string for test compress function '::character(15)));
SELECT HEX(COMPRESS('string for test compress function '::char(10)));
SELECT HEX(COMPRESS('string for test compress function '::varchar(10)));
SELECT HEX(COMPRESS('2023-06-01 00:23:59'::date));
SELECT HEX(COMPRESS('2023-06-01 00:23:59'::time));
SELECT HEX(COMPRESS('2023-06-01 00:23:59'::timestamp));
SELECT HEX(COMPRESS('23:25:38.691729-07'::timetz));
SELECT HEX(COMPRESS('59 second'::interval));
SELECT HEX(COMPRESS('59 second'::reltime));
SELECT HEX(COMPRESS(date '2021-12-31'));
SELECT COMPRESS(E'\x74\x65\x73\x74') = COMPRESS(U&'\0074\0065\0073\0074');
SELECT COMPRESS(E'\\x74657374') = COMPRESS('\x74657374');

-- 变量测试
SET enable_set_variable_b_format = 1;
SET @var1 = 100;
SET @var2 = 'test';
SET @var3 = date '2021-12-31';
SELECT HEX(COMPRESS(@var1));
SELECT HEX(COMPRESS(@var2));
SELECT HEX(COMPRESS(@var3));

-- 空值测试
select HEX(compress(''));
select HEX(compress(' '));
SELECT COMPRESS(NULL);

-- 超长字符串测试
SELECT HEX(COMPRESS(1E+1000));
SELECT HEX(COMPRESS(1E+1001)); --ERROR
SELECT HEX(COMPRESS(RPAD('a', 1024, 'a')));
SELECT HEX(COMPRESS(RPAD('a', 1025, 'a')));

-- 表达式测试
SELECT HEX(COMPRESS(UPPER('test')));
SELECT HEX(COMPRESS(CONCAT('my', 'name')));

-- uncompress测试用例
-- 基准测试
select uncompress(compress('string for test compress function aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa '));

select uncompress(a) from t1;
drop table t1;

SELECT UNCOMPRESS(COMPRESS('2022-05-12 10:30:00'));
SELECT UNCOMPRESS(COMPRESS(E'\\x01020304'));
SELECT UNCOMPRESS(COMPRESS(true));
SELECT UNCOMPRESS(COMPRESS(123.45));
SELECT UNCOMPRESS(COMPRESS(-123.45));

-- literal写法测试
SELECT UNCOMPRESS(COMPRESS(E'\x74\x65\x73\x74'));
SELECT UNCOMPRESS(COMPRESS(E'\\x74657374'));
SELECT UNCOMPRESS(COMPRESS(U&'\0074\0065\0073\0074'));
SELECT UNCOMPRESS(COMPRESS('123456789'::integer));
SELECT UNCOMPRESS(COMPRESS('123456789.0'::real));
SELECT UNCOMPRESS(COMPRESS('123456789.0'::double precision));
SELECT UNCOMPRESS(COMPRESS('1'::boolean));
SELECT UNCOMPRESS(COMPRESS('123456789'::text));
SELECT UNCOMPRESS(COMPRESS('123456789'::bytea));
SELECT HEX(UNCOMPRESS(COMPRESS(123456789::bit(32))));
SELECT UNCOMPRESS(COMPRESS('2023-06-01 00:23:59'::clob));
SELECT UNCOMPRESS(COMPRESS('string for test compress function '::name));
SELECT UNCOMPRESS(COMPRESS('string for test compress function '::character(15)));
SELECT UNCOMPRESS(COMPRESS('string for test compress function '::char(10)));
SELECT UNCOMPRESS(COMPRESS('string for test compress function '::varchar(10)));
SELECT UNCOMPRESS(COMPRESS('2023-06-01 00:23:59'::date));
SELECT UNCOMPRESS(COMPRESS('2023-06-01 00:23:59'::time));
SELECT UNCOMPRESS(COMPRESS('2023-06-01 00:23:59'::timestamp));
SELECT UNCOMPRESS(COMPRESS('59 minute'::interval));
SELECT UNCOMPRESS(COMPRESS(date '2021-12-31'));
SELECT UNCOMPRESS('123456789'::integer); --ERROR
SELECT UNCOMPRESS('123456789.0'::double precision); --ERROR
SELECT UNCOMPRESS('1'::boolean); --ERROR
SELECT UNCOMPRESS(123456789::bit(32)); --ERROR
SELECT UNCOMPRESS('123456789'::text); --ERROR
SELECT UNCOMPRESS('123456789'::bytea); --ERROR
SELECT UNCOMPRESS('2023-06-01 00:23:59'::date); --ERROR

-- 变量测试
SET @var1 = COMPRESS(100);
SET @var2 = COMPRESS('test');
SET @var3 = COMPRESS(date '2021-12-31');

-- 空值测试
SELECT UNCOMPRESS('');
SELECT UNCOMPRESS(' '); --ERROR
SELECT UNCOMPRESS(NULL);
SELECT UNCOMPRESS(COMPRESS(''));
SELECT UNCOMPRESS(COMPRESS(' '));
SELECT UNCOMPRESS(COMPRESS(NULL));

-- 范围超限测试
SELECT UNCOMPRESS(COMPRESS(RPAD('a', 1000000, 'a')));
SELECT UNCOMPRESS(1E+1000); --ERROR
SELECT UNCOMPRESS(1E+1001); --ERROR
SELECT UNCOMPRESS(COMPRESS(RPAD('a', 1024, 'a')));
SELECT UNCOMPRESS(COMPRESS(RPAD('a', 1025, 'a')));

-- 表达式测试
SELECT UNCOMPRESS(UPPER('test')); --ERROR
SELECT UNCOMPRESS(CONCAT('my', 'name')); --ERROR

-- uncompressed_length测试用例
-- 基准测试
select UNCOMPRESSED_LENGTH(COMPRESS('string for test compress function aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa '))=length('string for test compress function aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa ');
SELECT UNCOMPRESSED_LENGTH(COMPRESS('2022-05-12 10:30:00'));
SELECT UNCOMPRESSED_LENGTH(COMPRESS(E'\\x01020304'));
SELECT UNCOMPRESSED_LENGTH(COMPRESS(true));
SELECT UNCOMPRESSED_LENGTH(COMPRESS(123.45));
SELECT UNCOMPRESSED_LENGTH(COMPRESS(-123.45));

-- literal写法测试
SELECT UNCOMPRESSED_LENGTH(COMPRESS(E'\x74\x65\x73\x74'));
SELECT UNCOMPRESSED_LENGTH(COMPRESS(E'\\x74657374'));
SELECT UNCOMPRESSED_LENGTH(COMPRESS(U&'\0074\0065\0073\0074'));
SELECT UNCOMPRESSED_LENGTH(COMPRESS('123456789'::integer));
SELECT UNCOMPRESSED_LENGTH(COMPRESS('123456789.0'::real));
SELECT UNCOMPRESSED_LENGTH(COMPRESS('123456789.0'::double precision));
SELECT UNCOMPRESSED_LENGTH(COMPRESS('1'::boolean));
SELECT UNCOMPRESSED_LENGTH(COMPRESS('123456789'::text));
SELECT UNCOMPRESSED_LENGTH(COMPRESS('123456789'::bytea));
SELECT UNCOMPRESSED_LENGTH(COMPRESS(123456789::bit(32)));
SELECT UNCOMPRESSED_LENGTH(COMPRESS('2023-06-01 00:23:59'::clob));
SELECT UNCOMPRESSED_LENGTH(COMPRESS('string for test compress function '::name));
SELECT UNCOMPRESSED_LENGTH(COMPRESS('string for test compress function '::character(15)));
SELECT UNCOMPRESSED_LENGTH(COMPRESS('string for test compress function '::char(10)));
SELECT UNCOMPRESSED_LENGTH(COMPRESS('string for test compress function '::varchar(10)));
SELECT UNCOMPRESSED_LENGTH(COMPRESS('2023-06-01 00:23:59'::date));
SELECT UNCOMPRESSED_LENGTH(COMPRESS('2023-06-01 00:23:59'::time));
SELECT UNCOMPRESSED_LENGTH(COMPRESS('2023-06-01 00:23:59'::timestamp));
SELECT UNCOMPRESSED_LENGTH(COMPRESS('59 minute'::interval));
SELECT UNCOMPRESSED_LENGTH(COMPRESS(date '2021-12-31'));
SELECT UNCOMPRESSED_LENGTH(COMPRESS(DATETIME '2022-05-12 10:30:00'));
SELECT UNCOMPRESSED_LENGTH('123456789'::integer); --ERROR
SELECT UNCOMPRESSED_LENGTH('123456789.0'::double precision); --ERROR
SELECT UNCOMPRESSED_LENGTH('1'::boolean); --ERROR
SELECT UNCOMPRESSED_LENGTH(123456789::bit(32)); --ERROR
SELECT UNCOMPRESSED_LENGTH('123456789'::text); --ERROR
SELECT UNCOMPRESSED_LENGTH('123456789'::bytea); --ERROR
SELECT UNCOMPRESSED_LENGTH('2023-06-01 00:23:59'::date); --ERROR

-- 空值测试
select UNCOMPRESSED_LENGTH('');
select UNCOMPRESSED_LENGTH(' '); --ERROR
SELECT UNCOMPRESSED_LENGTH(NULL);
SELECT UNCOMPRESSED_LENGTH(COMPRESS(''));
SELECT UNCOMPRESSED_LENGTH(COMPRESS(' '));
SELECT UNCOMPRESSED_LENGTH(COMPRESS(NULL));

-- 超限测试
SELECT UNCOMPRESSED_LENGTH(COMPRESS(RPAD('a', 1000000, 'a')));
SELECT UNCOMPRESSED_LENGTH(1E+1001); --ERROR
SELECT UNCOMPRESSED_LENGTH(COMPRESS(RPAD('a', 1024, 'a')));
SELECT UNCOMPRESSED_LENGTH(COMPRESS(RPAD('a', 1025, 'a')));

-- 表达式
SELECT UNCOMPRESSED_LENGTH(UPPER('test')); --ERROR
SELECT UNCOMPRESSED_LENGTH(CONCAT('my', 'name'));

SET enable_set_variable_b_format = default;
set dolphin.sql_mode = default;
drop schema db_b_compress_test cascade;
reset current_schema;

create schema db_b_weightstring_test;
set current_schema to 'db_b_weightstring_test';

-- 基准测试
select hex(weight_string('aa' as char(3)));
select hex(weight_string('a' as char(1)));
select hex(weight_string('ab' as char(1)));
select hex(weight_string('ab'));
select hex(weight_string('aa' as binary(3)));
select hex(weight_string('abc' as binary(2)));
select hex(weight_string('abc' as char(2) LEVEL 1 ));
select hex(weight_string('abc' as char(2) LEVEL 1 ASC));
select hex(weight_string('abc' as char(2) LEVEL 1 DESC));
select hex(weight_string('abc' as char(2) LEVEL 1 REVERSE));
select hex(weight_string('abc' as char(2) LEVEL 1 DESC REVERSE));
select hex(weight_string('abc' as char(2) LEVEL 1 DESC, 1 REVERSE));
select hex(weight_string('abc' as char(2) LEVEL 1 DESC, 2 REVERSE));
select hex(weight_string('abc' as char(2) LEVEL 2 DESC, 1 REVERSE));
select hex(weight_string('abc' as char(2) LEVEL 2 DESC  REVERSE));
select hex(weight_string('abc' as char(2) LEVEL 2,3,5 REVERSE));
select hex(weight_string(cast('aa' as binary(3))));
-- 其他literal
SELECT HEX(WEIGHT_STRING(E'\x74\x65\x73\x74' as char(10)));
SELECT HEX(WEIGHT_STRING(E'\\x74657374' as char(10)));
SELECT HEX(WEIGHT_STRING(U&'\0074\0065\0073\0074' as char(10)));
SELECT HEX(WEIGHT_STRING('123456789'::integer as char(10)));
SELECT HEX(WEIGHT_STRING('123456789.0'::real as char(20)));
SELECT HEX(WEIGHT_STRING('123456789.0'::double precision as char(20)));
SELECT HEX(WEIGHT_STRING('1'::boolean as char(10)));
SELECT HEX(WEIGHT_STRING('123456789'::character(9) as char(20)));
SELECT HEX(WEIGHT_STRING('123456789'::text as char(20)));
SELECT HEX(WEIGHT_STRING('123456789'::bytea as char(20)));
SELECT HEX(WEIGHT_STRING('2023-06-01 00:23:59'::date as char(20)));
SELECT HEX(WEIGHT_STRING('2023-06-01 00:23:59'::time as char(20)));
SELECT HEX(WEIGHT_STRING('2023-06-01 00:23:59'::timestamp as char(20)));
SELECT HEX(WEIGHT_STRING('59 minute'::interval as char(20)));
SELECT HEX(WEIGHT_STRING(date '2021-12-31' as char(10)));
SELECT HEX(WEIGHT_STRING(E'\x74\x65\x73\x74' as char(10)));
SELECT HEX(WEIGHT_STRING(E'\\x74657374' as char(10)));
SELECT HEX(WEIGHT_STRING(U&'\0074\0065\0073\0074' as char(10)));
SELECT HEX(WEIGHT_STRING('123456789'::integer as binary(10)));
SELECT HEX(WEIGHT_STRING('123456789.0'::real as binary (20)));
SELECT HEX(WEIGHT_STRING('123456789.0'::double precision as binary (20)));
SELECT HEX(WEIGHT_STRING('1'::boolean as binary(10)));
SELECT HEX(WEIGHT_STRING('123456789'::character(9) as binary (20)));
SELECT HEX(WEIGHT_STRING('123456789'::text as binary (20)));
SELECT HEX(WEIGHT_STRING('123456789'::bytea as binary (20)));
SELECT HEX(WEIGHT_STRING('2023-06-01 00:23:59'::date as binary (20)));
SELECT HEX(WEIGHT_STRING('2023-06-01 00:23:59'::time as binary (20)));
SELECT HEX(WEIGHT_STRING('2023-06-01 00:23:59'::timestamp as binary (20)));
SELECT HEX(WEIGHT_STRING('59 minute'::interval as binary (20)));
SELECT HEX(WEIGHT_STRING(date '2021-12-31' as binary (10)));
-- 变量测试
SET enable_set_variable_b_format = 1;
SET @var1 = 100;
SET @var2 = 'test';
SET @var3 = date '2021-12-31';
SELECT HEX(WEIGHT_STRING(@var1 as char(10))); 
SELECT HEX(WEIGHT_STRING(@var2 as char(10)));
SELECT HEX(WEIGHT_STRING(@var3 as char(10)));
SELECT HEX(WEIGHT_STRING(@var1 as BINARY (10))); 
SELECT HEX(WEIGHT_STRING(@var2 as BINARY (10)));
SELECT HEX(WEIGHT_STRING(@var3 as BINARY (10)));
SET @var4 = WEIGHT_STRING('test' AS BINARY(10));
SET @var5 = WEIGHT_STRING('test' AS char(10));
-- NULL值处理
select hex(weight_string(NULL));
select hex(weight_string(NULL as binary(10)));
select hex(weight_string(NULL as char(10)));
select hex(weight_string('test' as char(NULL)));
select hex(weight_string(''));
select hex(weight_string(' '));
-- 超长字符串测试
select hex(weight_string(RPAD('a', 1000000, 'a') as char(10)));
select hex(weight_string(RPAD('a', 1000000000, 'a') as char(1000000000))); 
select hex(weight_string(RPAD('a', 1000000000, 'a') as binary(1000000000))); 
-- 异常入参测试
select hex(weight_string('a' as char(-1)));  
select hex(weight_string('abc' as char(2) LEVEL 2-5 DESC));
SELECT HEX(WEIGHT_STRING('abc' as char(cast ( '10' as integer )) LEVEL '1'::numeric REVERSE));
SELECT HEX(WEIGHT_STRING('ab' AS CHAR(1E+308)));
SELECT HEX(WEIGHT_STRING('ab' AS BINARY(1E+308)));
SELECT HEX(WEIGHT_STRING('ab' AS CHAR(1E+309)));
SELECT HEX(WEIGHT_STRING('ab' AS BINARY(1E+309)));
select hex(weight_string('a' as char(0)));
select hex(weight_string('a' as binary(0)));
select hex(weight_string('a' as char(-1)));
select hex(weight_string('a' as binary(-1))); 
SELECT HEX(WEIGHT_STRING('Hello' LEVEL -1)); 
SELECT HEX(WEIGHT_STRING('Hello' LEVEL 7));
SELECT HEX(WEIGHT_STRING('Hello' LEVEL 3,1));
SELECT HEX(WEIGHT_STRING('Hello' LEVEL 3-1));
-- 表达式
SELECT HEX(WEIGHT_STRING (UPPER('test') as char(10))); 
SELECT HEX(WEIGHT_STRING (CONCAT('my', 'name') as char(10))); 
-- 兼容字符集测试（SQL_ASCII GBK LATIN1 UTF8）
SET client_encoding TO SQL_ASCII;
SELECT HEX(WEIGHT_STRING(CONVERT(E'\x57\x65\x69\x67\x68\x74\x20\x53\x74\x72\x69\x6e\x67\xe6\xb5\x8b\xe8\xaf\x95\x31' USING SQL_ASCII) AS CHAR(100))); 
SET client_encoding TO GBK;
SELECT HEX(WEIGHT_STRING(CONVERT(E'\x57\x65\x69\x67\x68\x74\x20\x53\x74\x72\x57\x65\x69\x67\x68\x74\x20\x53\x74\x72' USING GBK) AS CHAR(100))); 
SET client_encoding TO LATIN1;
SELECT HEX(WEIGHT_STRING(CONVERT(E'\x57\x65\x69\x67\x68\x74\x20\x53\x74\x72\x57\x65\x69\x67\x68\x74\x20\x53\x74\x72' USING LATIN1) AS CHAR(100))); 
SET client_encoding TO UTF8;
SELECT HEX(WEIGHT_STRING(CONVERT(E'\x57\x65\x69\x67\x68\x74\x20\x53\x74\x72\x69\x6e\x67\xe6\xb5\x8b\xe8\xaf\x95\x31' USING UTF8) AS CHAR(100))); 
-- 不兼容字符集测试（WIN874 LATIN 6 LATIN8 EUC_KR）
SET client_encoding TO WIN874;
SELECT WEIGHT_STRING(CONVERT('foo' USING WIN874) AS CHAR(100)) = WEIGHT_STRING(CONVERT('foo' USING LATIN6) AS CHAR(100));
SELECT WEIGHT_STRING(CONVERT('foo' USING LATIN8) AS CHAR(100)) = WEIGHT_STRING(CONVERT('foo' USING WIN874) AS CHAR(100));
SELECT WEIGHT_STRING(CONVERT('foo' USING EUC_KR) AS CHAR(100)) = WEIGHT_STRING(CONVERT('foo' USING LATIN8) AS CHAR(100));
set enable_set_variable_b_format = default;
drop schema db_b_weightstring_test cascade;
reset current_schema;

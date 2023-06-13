create schema db_b_nameconst_test;
set current_schema to 'db_b_nameconst_test';

-- 基准测试
SELECT NAME_CONST('test', 123);
SELECT NAME_CONST('test', -1);
SELECT NAME_CONST('test', 1.0);
SELECT NAME_CONST('test', -1.0);
SELECT NAME_CONST('test', 'test');
SELECT NAME_CONST(1, 'test');
SELECT NAME_CONST(0, 'test');
SELECT NAME_CONST('mybool', TRUE);
SELECT NAME_CONST('mybig', 9223372036854775808);
SELECT NAME_CONST('my$name', 'Hello');
-- 其他literal
SELECT NAME_CONST(E'\x74\x65\x73\x74', U&'\0074\0065\0073\0074');
SELECT NAME_CONST('test', '42'::integer);
SELECT NAME_CONST('42'::int1, 'test');
SELECT NAME_CONST('42'::int2, 'test');
SELECT NAME_CONST('42'::int4, 'test');
SELECT NAME_CONST('42'::int8, 'test');
SELECT NAME_CONST('42'::int16, 'test');
SELECT NAME_CONST('3.14'::double precision, 'test');
SELECT NAME_CONST('42.1'::float4, 'test');
SELECT NAME_CONST('42.1'::float8, 'test');
SELECT NAME_CONST('42'::NUMERIC, 'test');
SELECT NAME_CONST('test', '1'::boolean);
SELECT NAME_CONST('0'::boolean, 'test');
SELECT NAME_CONST('2023-06-01 00:23:59'::date, 'test');
SELECT NAME_CONST('test', '2023-06-01 00:23:59'::time);
SELECT NAME_CONST('2023-06-01 00:23:59'::time, 'test');
SELECT NAME_CONST('test', '2023-06-01 00:23:59'::timestamp);
SELECT NAME_CONST('2023-06-01 00:23:59'::timestamp, 'test');
SELECT NAME_CONST('2020-01-01 00:00:01'::abstime, 123);
SELECT NAME_CONST('2020-01-01 00:00:01'::smalldatetime, 123);
SELECT NAME_CONST('23:25:38.691729-07'::timetz, 123);
SELECT NAME_CONST('test', '59 minute'::interval);
SELECT NAME_CONST('59 minute'::interval, 'test');
SELECT NAME_CONST('59 minute'::reltime, 'test');
SELECT NAME_CONST('2023-06-01 00:23:59'::bytea, 'test');
SELECT NAME_CONST('test'::name, 123);
SELECT NAME_CONST('test'::text, 123);
SELECT NAME_CONST('test'::cstring, 123);
SELECT NAME_CONST('test'::clob, 123);
SELECT NAME_CONST('test', '2023-06-01 00:23:59'::character(15));
SELECT NAME_CONST('2023-06-01 00:23:59'::character(15), 'test');
SELECT NAME_CONST('2023-06-01 00:23:59'::char(10), 'test');
SELECT NAME_CONST('2023-06-01 00:23:59'::varchar(10), 'test');
SELECT NAME_CONST('acde070d-8c4c-4f0d-9d8a-162843c10333'::uuid, 123);
SELECT NAME_CONST('1.23'::money, 123);
SELECT NAME_CONST(date '2021-12-31', E'\\u4F60\\u597D');
-- 变量测试
SET enable_set_variable_b_format = 1;
SET @var1 = 100;
SET @var2 = 'test';
SET @var3 = date '2021-12-31';
SELECT NAME_CONST('test', @var1); --ERROR
SELECT NAME_CONST(@var2, 100); --ERROR
SELECT NAME_CONST(@var1, @var3); --ERROR
SELECT NAME_CONST(@var3, @var2); --ERROR
-- NULL值和空值
SELECT NAME_CONST('test', NULL); 
SELECT NAME_CONST(NULL, NULL); --ERROR
SELECT NAME_CONST(NULL, 'test'); --ERROR
SELECT NAME_CONST('myempty', '');
SELECT NAME_CONST('myspace', ' ');
SELECT NAME_CONST('', 'test');
SELECT NAME_CONST(' ', 'test');
-- 超出取值范围的入参测试
SELECT NAME_CONST(-1, 'test'); --ERROR
SELECT NAME_CONST(-2.0, 'test'); --ERROR
SELECT NAME_CONST(1E+1000, 'test');
SELECT NAME_CONST(1E+1001, 'test'); --ERROR
SELECT NAME_CONST('test', 1E+1000);
SELECT NAME_CONST('test', 1E+1001); --ERROR
SELECT NAME_CONST('test', '9999-99-99 99:99:99'::time); --ERROR
SELECT NAME_CONST('9999-99-99 99:99:99'::time, 'test'); --ERROR
SELECT NAME_CONST('test', 'test'::integer); --ERROR
SELECT NAME_CONST('test'::integer, 'test'); --ERROR
SELECT NAME_CONST('test', '2'::boolean);  --ERROR
-- 函数及表达式
SELECT NAME_CONST('mylong', REPEAT('a', 1000)); --ERROR
SELECT NAME_CONST(REPEAT('a', 1000), 'Hello'); --ERROR
SELECT NAME_CONST('test', UPPER('test')); --ERROR
SELECT NAME_CONST(UPPER('test'), 'test');--ERROR
SELECT NAME_CONST(CONCAT('my', 'name'), 'Hello'); --ERROR
SELECT NAME_CONST('a', -(1 OR 2)) OR 1;
SELECT NAME_CONST('test', (1 + 2)); --ERROR
SELECT NAME_CONST((1 * 2), 'test'); --ERROR
-- MySQL的其他测试用例
CREATE TABLE t1 (a INT);
INSERT INTO t1 VALUES (1),(2),(3);
SELECT NAME_CONST('flag',1)    * MAX(a) FROM t1;
SELECT NAME_CONST('flag',1.5)  * MAX(a) FROM t1;
SELECT NAME_CONST('flag',-1)   * MAX(a) FROM t1;
SELECT NAME_CONST('flag',-1.5) * MAX(a) FROM t1;
SELECT NAME_CONST('flag', SQRT(4)) * MAX(a) FROM t1; --error
SELECT NAME_CONST('flag',-SQRT(4)) * MAX(a) FROM t1; --error
DROP TABLE t1;

CREATE TABLE t1(a INT);
INSERT INTO t1 VALUES (), (), (); 
SELECT NAME_CONST(a, '1') FROM t1; --error
SET INSERT_ID= NAME_CONST(a, a); --error
DROP TABLE t1;

select NAME_CONST('_id',1234) as id;

SELECT NAME_CONST('a', -(1 OR 2)) OR 1; --ERROR
SELECT NAME_CONST('a', -(1 AND 2)) OR 1; --ERROR
SELECT NAME_CONST('a', -(1)) OR 1;

CREATE TABLE t1 (a INT);
SELECT 1 from t1 HAVING NAME_CONST('', a); --error
DROP TABLE t1;

set enable_set_variable_b_format = default;
drop schema db_b_nameconst_test cascade;
reset current_schema;

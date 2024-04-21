drop database if exists db_b_atan_test;
create database db_b_atan_test dbcompatibility 'A';


\c db_b_atan_test

select atan(-2, 2);
-- error
select atan2(-2, 2);
-- -0.7853981633974483
select atan2(pi(), 0);
-- 1.5707963267948966
select pi(),atan(1);
-- 3.141593 | 0.7853981633974483
SELECT ATAN2(1, 1); 
-- 0.7853981633974483
SELECT ATAN2(0.5, 0.5); 
-- 0.7853981633974483
SELECT ATAN2(-1, 1); 
-- -0.7853981633974483
SELECT ATAN2(-0.5, 0.5);
-- -0.7853981633974483

\c contrib_regression
drop database db_b_atan_test;
create schema db_b_atan_test;
set current_schema to 'db_b_atan_test';

-- 基本功能

select atan(-2, 2);
-- -0.7853981633974483
select atan(pi(), 0);
-- 1.5707963267948966
select pi(),atan(1);
-- 3.141593 | 0.7853981633974483
SELECT ATAN(1, 1); 
-- 0.7853981633974483
SELECT ATAN(0.5, 0.5); 
-- 0.7853981633974483
SELECT ATAN(-1, 1); 
-- -0.7853981633974483
SELECT ATAN(-0.5, 0.5);
-- -0.7853981633974483
SELECT ATAN('42'::integer, '21.0'::double precision);
-- 1.10714871779409
SELECT ATAN('42'::bigint, '21'::smallint);
-- 1.10714871779409
SELECT ATAN('42':: real, '21.0':: numeric(10,2));
-- 1.10714871779409

-- boolean类型
SELECT ATAN(true, false);
-- 1.5707963267948966
SELECT ATAN(true, 0.5);
-- 1.1071487177940904
SELECT ATAN(0.5, true);
-- 0.4636476090008061
SELECT ATAN(2, true);
-- 1.1071487177940904
SELECT ATAN(true);
-- 0.7853981633974483

-- 空值测试

SELECT ATAN(NULL, 1); 
-- NULL
SELECT ATAN(1, NULL); 
-- NULL
SELECT ATAN(NULL, NULL); 
-- NULL

-- 超出取值范围的入参测试	

SELECT ATAN(1E+308, 1E+308); 
-- 0.7853981633974483
SELECT ATAN(-1E+308, 1E+308); 
-- -0.7853981633974483
SELECT ATAN(1E+309, 1E+308); 
-- Error
SELECT ATAN(-1E+309, 1E+308); 
-- Error

-- 非数值型入参测试

SELECT ATAN('1', 1); 
-- 0.7853981633974483
SELECT ATAN(1, '1'); 
-- 0.7853981633974483
SELECT ATAN(2, '1'::character(1));
-- 1.10714871779409
SELECT ATAN('2'::text, 1);
-- 1.10714871779409
SELECT ATAN('a', 1); 
-- ERROR
SELECT ATAN(2, '1'::boolean);
-- 1.1071487177940904
SELECT ATAN('2'::interval, 1);
-- ERROR
SELECT ATAN('2022-05-12', 1);
-- ERROR
-- without date->float8 implicit
SELECT ATAN('2022-05-12'::date, 1);
-- 1.57079627734016
-- https://gitee.com/opengauss/Plugin/pulls/815 fix it
SELECT ATAN('2023-06-01 00:23:59'::timestamp, 1);
-- 1.57079632679485
SELECT ATAN('2'::bytea, 1);
-- 1.10714871779409
SELECT ATAN(ARRAY[1,2,3], 1);
-- ERROR

drop schema db_b_atan_test cascade;
reset current_schema;
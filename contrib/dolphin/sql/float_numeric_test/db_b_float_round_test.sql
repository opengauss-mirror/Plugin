-- the test case of A format
drop database if exists test;
create database test dbcompatibility 'A';
\c test
SELECT 2.5::float4::int1;
SELECT 2.5::float8::int1;
SELECT 2.5::int1;
SELECT 2.5::float4::int2;
SELECT 2.5::float8::int2;
SELECT 2.5::int2;
SELECT 2.5::float4::int4;
SELECT 2.5::float8::int4;
SELECT 2.5::int4;
SELECT 2.5::float4::int8;
SELECT 2.5::float8::int8;
SELECT 2.5::int8;

SELECT 3.5::float4::int1;
SELECT 3.5::float8::int1;
SELECT 3.5::int1;
SELECT 3.5::float4::int2;
SELECT 3.5::float8::int2;
SELECT 3.5::int2;
SELECT 3.5::float4::int4;
SELECT 3.5::float8::int4;
SELECT 3.5::int4;
SELECT 3.5::float4::int8;
SELECT 3.5::float8::int8;
SELECT 3.5::int8;

SELECT dround(2.5);
SELECT dround(3.5);

\c contrib_regression
drop database test;

-- the test case of dolphin plugin
create schema test;
set current_schema to 'test';

SELECT 2.5::float4::int1;
SELECT 2.5::float8::int1;
SELECT 2.5::int1;
SELECT 2.5::float4::int2;
SELECT 2.5::float8::int2;
SELECT 2.5::int2;
SELECT 2.5::float4::int4;
SELECT 2.5::float8::int4;
SELECT 2.5::int4;
SELECT 2.5::float4::int8;
SELECT 2.5::float8::int8;
SELECT 2.5::int8;

SELECT 3.5::float4::int1;
SELECT 3.5::float8::int1;
SELECT 3.5::int1;
SELECT 3.5::float4::int2;
SELECT 3.5::float8::int2;
SELECT 3.5::int2;
SELECT 3.5::float4::int4;
SELECT 3.5::float8::int4;
SELECT 3.5::int4;
SELECT 3.5::float4::int8;
SELECT 3.5::float8::int8;
SELECT 3.5::int8;

SELECT dround(2.5);
SELECT dround(3.5);

drop schema test cascade;
reset current_schema;

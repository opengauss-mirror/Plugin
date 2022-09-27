drop database if exists implicit_cast;
create database implicit_cast dbcompatibility 'b';
\c implicit_cast

select 1::int1 % 1::float4;
select 1::int2 % 1::float4;
select 1::int4 % 1::float4;
select 1::int8 % 1::float4;

select 1::int1 % 1::float8;
select 1::int2 % 1::float8;
select 1::int4 % 1::float8;
select 1::int8 % 1::float8;

select 1::int1 % 1::text;
select 1::int2 % 1::text;
select 1::int4 % 1::text;
select 1::int8 % 1::text;

select 1::int1 # 1::float4;
select 1::int2 # 1::float4;
select 1::int4 # 1::float4;
select 1::int8 # 1::float4;

select 1::int1 # 1::float8;
select 1::int2 # 1::float8;
select 1::int4 # 1::float8;
select 1::int8 # 1::float8;

select 1::int1 # 1::text;
select 1::int2 # 1::text;
select 1::int4 # 1::text;
select 1::int8 # 1::text;

select 1::int1 & 1::float4;
select 1::int2 & 1::float4;
select 1::int4 & 1::float4;
select 1::int8 & 1::float4;

select 1::int1 & 1::float8;
select 1::int2 & 1::float8;
select 1::int4 & 1::float8;
select 1::int8 & 1::float8;

select 1::int1 & 1::text;
select 1::int2 & 1::text;
select 1::int4 & 1::text;
select 1::int8 & 1::text;

select 1::int1 | 1::float4;
select 1::int2 | 1::float4;
select 1::int4 | 1::float4;
select 1::int8 | 1::float4;

select 1::int1 | 1::float8;
select 1::int2 | 1::float8;
select 1::int4 | 1::float8;
select 1::int8 | 1::float8;

select 1::int1 | 1::text;
select 1::int2 | 1::text;
select 1::int4 | 1::text;
select 1::int8 | 1::text;

\c postgres
drop database if exists implicit_cast;
create schema implicit_cast;
set current_schema to 'implicit_cast';

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

drop schema implicit_cast cascade;
reset current_schema;
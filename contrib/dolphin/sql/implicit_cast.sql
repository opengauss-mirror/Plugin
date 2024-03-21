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
set dolphin.b_compatibility_mode to off;
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
set dolphin.b_compatibility_mode to on;
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

create table t1(a bit default 0,b int default b'0', c bit default 0::int unsigned, d int unsigned default b'0');
desc t1;
insert into t1 values (default,default,default,default);
select * from t1;
drop table t1;

drop schema implicit_cast cascade;
reset current_schema;
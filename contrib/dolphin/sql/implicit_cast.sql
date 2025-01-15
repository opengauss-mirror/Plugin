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


create table test1(id int, ptype varchar(30));
insert into test1 values (1, 'type1');
insert into test1 values (2, 'type2');
insert into test1 values (3, 'type3');
select * from test1 t1 where t1.id = 1 and binary(t1.ptype) in ('type1'::varchar, 'type2'::varchar);
select * from test1 t1 where t1.id = 1 and binary(t1.ptype) in ('type1'::char(30), 'type2'::char(30));
select * from test1 t1 where t1.id = 1 and binary(t1.ptype)::varbinary(30) in ('type1'::varchar, 'type2'::varchar);
select * from test1 t1 where t1.id = 1 and binary(t1.ptype)::varbinary(30) in ('type1'::char(30), 'type2'::char(30));
drop table test1;

set b_format_behavior_compat_options = 'fetch,enable_set_variables,enable_multi_charset';
create table t1(a int not null auto_increment primary key);
SET @tzid= LAST_INSERT_ID();
select @tzid;
insert into t1 values(NULL);
SET @tzid= LAST_INSERT_ID();
select @tzid;
reset b_format_behavior_compat_options;
drop table t1;

drop schema implicit_cast cascade;
reset current_schema;
create schema uint_cast3;
set current_schema to 'uint_cast3';
select 1::uint1::int16;
select 1::int16::uint1;
select 1::uint2::int16;
select 1::int16::uint2;
select 1::uint4::int16;
select 1::int16::uint4;
select 1::uint8::int16;
select 1::int16::uint8;

select 1::uint1::clob;
select 1::clob::uint1;
select 1::uint2::clob;
select 1::clob::uint2;
select 1::uint4::clob;
select 1::clob::uint4;
select 1::uint8::clob;
select 1::clob::uint8;

select 1::uint4::abstime;
select 1::abstime::uint4;

select 1::uint4::reltime;
select 1::reltime::uint4;

select 1::uint1::bpchar;
select 1::bpchar::uint1;
select 1::uint2::bpchar;
select 1::bpchar::uint2;
select 1::uint4::bpchar;
select 1::bpchar::uint4;
select 1::uint8::bpchar;
select 1::bpchar::uint8;

select 1::uint1::varchar;
select 1::varchar::uint1;
select 1::uint2::varchar;
select 1::varchar::uint2;
select 1::uint4::varchar;
select 1::varchar::uint4;
select 1::uint8::varchar;
select 1::varchar::uint8;

select 1::uint4::bit;
select 1::bit::uint4;
select 1::uint8::bit;
select 1::bit::uint8;

select 1::uint1::nvarchar2;
select 1::nvarchar2::uint1;
select 1::uint2::nvarchar2;
select 1::nvarchar2::uint2;
select 1::uint4::nvarchar2;
select 1::nvarchar2::uint4;
select 1::uint8::nvarchar2;
select 1::nvarchar2::uint8;

select 1::uint1::money;
select 1::uint2::money;;
select 1::uint4::money;
select 1::uint8::money;

select 1::uint1::interval;
select 1::uint2::interval;;
select 1::uint4::interval;

select 151503::uint4::time;
select 19990101::uint4::date;

select 19990101::uint4::timestamp;
select 19990101::uint4::timestamptz;
select 19990101::uint4::datetime;

select 19990101222222::uint8::timestamp;
select 19990101222222::uint8::timestamptz;
select 19990101222222::uint8::datetime;

select '-1'::uint1::date;
select '256'::uint1::date;
select 2004::uint2::date;
select '65536'::uint2::date;
select '65535'::uint2::date;
select 2067::uint8::date;
select '-1'::uint8::date;
select '18446744073709551615'::uint8::date;

select 1999::uint4::year;
select 1999::year::uint4;

select (-1)::text::uint1;
select (-1)::text::uint2;
select (-1)::text::uint4;
select (-1)::text::uint8;

select '255'::text::uint1;
select '65535'::text::uint2;
select '4294967295'::text::uint4;
select '18446744073709551615'::text::uint8;

select '256'::text::uint1;
select '65536'::text::uint2;
select '4294967296'::text::uint4;
select '18446744073709551616'::text::uint8;

-- test: binary to unsigned
--- 配置参数
set dolphin.b_compatibility_mode=on;
set b_format_behavior_compat_options=enable_set_variables;
set bytea_output=escape;
--- 建表
drop table if exists t_binary0001 cascade;
create table t_binary0001(
c1 int not null,
c2 binary,
c3 binary(10),
c4 binary(255),
c5 varbinary(1),
c6 varbinary(10),
c7 varbinary(255)) charset utf8mb3;
--- 插入数据
set @v1='abcdefghijklmnopqrstuvwxyz';
set @v2='a熊猫竹竹爱吃竹子';
set @v3=hex(@v2);
set @v4=unhex(@v3);
set @v5=bin(121314);
set @v6=oct(999999);

insert into t_binary0001 values (
1, substr(@v1,1,1), substr(@v1,1,10), repeat(@v1, 9),
substr(@v1,1,1), substr(@v1,1,10), repeat(@v1, 9));
insert into t_binary0001 values (
2, substr(@v2,1,1), substr(@v2,1,4), repeat(@v2, 9),
substr(@v2,1,1), substr(@v2,1,4), repeat(@v2, 9));
insert into t_binary0001 values (
3, substr(@v3,1,1), substr(@v3,1,4), substr(repeat(@v3, 10),1,255),
substr(@v3,1,1), substr(@v3,1,4), substr(repeat(@v3, 10),1,255));
insert into t_binary0001 values (
4, substr(@v4,1,1), substr(@v4,1,4), substr(repeat(@v4, 10),1,255),
substr(@v4,1,1), substr(@v4,1,4), substr(repeat(@v4, 10),1,255));
insert into t_binary0001 values (
5, substr(@v5,1,1), substr(@v5,1,4), substr(repeat(@v5, 10),1,255),
substr(@v5,1,1), substr(@v5,1,4), substr(repeat(@v5, 10),1,255));
insert into t_binary0001 values (
6, substr(@v6,1,1), substr(@v6,1,4), substr(repeat(@v6, 10),1,255),
substr(@v6,1,1), substr(@v6,1,4), substr(repeat(@v6, 10),1,255));

--- 使用cast/convert函数转换
select c1, cast(c2 as unsigned), cast(c3 as unsigned), cast(c4 as unsigned),
cast(c5 as unsigned) from t_binary0001 order by 1,2,3,4,5;
select c1, convert(c2, unsigned), convert(c3, unsigned), convert(c4, unsigned),
convert(c5, unsigned), convert(c6, unsigned), convert(c7, unsigned) from
t_binary0001 order by 1,2,3,4,5,6,7;

--- 建表并插入数据
create table t_binary(a binary(255));
create table t_integer(a1 int8 unsigned, a2 int4 unsigned, a3 int2 unsigned, a4 int1 unsigned,
	a5 int8, a6 int4, a7 int2, a8 int1);
insert into t_binary values('2e1');
insert into t_binary values('1ab');
insert into t_binary values('0x1ab');
insert into t_binary values('123');
--- 测试转换结果
select cast(a as int8 unsigned), cast(a as int4 unsigned), cast(a as int2 unsigned), cast(a as int1 unsigned),
cast(a as int8), cast(a as int4), cast(a as int2), cast(a as int1) from t_binary;
insert ignore into t_integer select a, a, a, a, a, a, a, a from t_binary;
select * from t_integer;

--- 清理
drop table t_integer cascade;
drop table t_binary cascade;
drop table t_binary0001 cascade;

--- 建表并插入数据
drop table if exists t_bigint0007;
create table t_bigint0007(
c1 int,
c2 bigint unsigned,
c3 bigint(1) unsigned,
c4 bigint(255) unsigned,
c5 bigint unsigned default '10',
c6 bigint unsigned auto_increment primary key,
c7 bigint(1) unsigned default 0,
c8 bigint(10) unsigned not null default 99,
c9 bigint unsigned unique);
set @val = 18446744073709551615;
insert t_bigint0007 values(1, @val, @val, @val, @val, @val, @val, @val, @val);

--- 测试转换结果
select * from t_bigint0007;

--- 清理
drop table t_bigint0007 cascade;

SET dolphin.sql_mode = '';
select cast('-0' as unsigned);
create table t_uint(a uint1, b uint2, c uint4, d uint8);
insert into t_uint values('-0', '-0', '-0', '-0');
select * from t_uint;
drop table t_uint;

select cast(1 as signed int);
select cast(1 as unsigned int);
select convert(1 , signed int);
select convert(1 , unsigned int);

select cast(1 as signed integer);
select cast(1 as unsigned integer);
select convert(1 , signed integer);
select convert(1 , unsigned integer);

select cast(1 as integer signed);
select cast(1 as integer unsigned);
select convert(1 , integer signed);
select convert(1 , integer unsigned);

select cast(1 as  signed);
select cast(1 as unsigned);
select convert(1 , signed);
select convert(1 , unsigned);

select (-1)::nvarchar2::uint1;
select (-1)::nvarchar2::uint2;
select (-1)::nvarchar2::uint4;
select (-1)::nvarchar2::uint8;

select '255'::nvarchar2::uint1;
select '65535'::nvarchar2::uint2;
select '4294967295'::nvarchar2::uint4;
select '18446744073709551615'::nvarchar2::uint8;

select '256'::nvarchar2::uint1;
select '65536'::nvarchar2::uint2;
select '4294967296'::nvarchar2::uint4;
select '18446744073709551616'::nvarchar2::uint8;

drop schema uint_cast3 cascade;
reset current_schema;

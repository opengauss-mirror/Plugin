drop database if exists uint_cast2;
create database uint_cast2 dbcompatibility 'b';
\c uint_cast2

drop table if exists t1 ;
create table t1(a uint8);

insert into t1 values(1::bool);
insert into t1 values(0::bool);
insert into t1 values((-1)::bool);

insert into t1 values(1.0::float4);
insert into t1 values(0.0::float4);
insert into t1 values((-1.0)::float4);
insert into t1 values(9223372036854775807.0::float4);
insert into t1 values(9223372036854775808.0::float4);

insert into t1 values(1.0::float8);
insert into t1 values(0.0::float8);
insert into t1 values((-1.0)::float8);
insert into t1 values(18446744073709550515.0::float8);
insert into t1 values(18446744073709550515.0::float8);
insert into t1 values(1::numeric);
insert into t1 values(0::numeric);
insert into t1 values((-1)::numeric);
insert into t1 values(18446744073709550515::numeric);
insert into t1 values(18446744073709550516::numeric);

insert into t1 values(1::int8);
insert into t1 values(0::int8);
insert into t1 values((-1)::int8);
insert into t1 values(9223372036854775807::int8);
insert into t1 values(9223372036854775808::int8);

insert into t1 values(1::int4);
insert into t1 values(0::int4);
insert into t1 values((-1)::int4);
insert into t1 values(2147483647::int4);
insert into t1 values(2147483648::int4);

insert into t1 values(1::int2);
insert into t1 values(0::int2);
insert into t1 values((-1)::int2);
insert into t1 values(32767::int2);
insert into t1 values(32768::int2);


insert into t1 values(1::int1);
insert into t1 values(0::int1);
insert into t1 values((-1)::int1);
insert into t1 values(255::int1);
insert into t1 values(256::int1);

insert into t1 values(1::uint4);
insert into t1 values(0::uint4);
insert into t1 values((-1)::uint4);
insert into t1 values(4294967295::uint4);
insert into t1 values(4294967296::uint4);

insert into t1 values(1::uint2);
insert into t1 values(0::uint2);
insert into t1 values((-1)::uint2);
insert into t1 values(65535::uint2);
insert into t1 values(65536::uint2);

insert into t1 values(1::uint1);
insert into t1 values(0::uint1);
insert into t1 values((-1)::uint1);
insert into t1 values(255::uint1);
insert into t1 values(256::uint1);

drop table if exists t1 ;

create table t1(a uint4);

insert into t1 values(1::bool);
insert into t1 values(0::bool);
insert into t1 values((-1)::bool);

insert into t1 values(1.0::float4);
insert into t1 values(0.0::float4);
insert into t1 values((-1.0)::float4);
insert into t1 values(4294967295::float4);
insert into t1 values(4294967296::float4);

insert into t1 values(1.0::float8);
insert into t1 values(0.0::float8);
insert into t1 values((-1.0)::float8);
insert into t1 values(4294967295::float8);
insert into t1 values(4294967296::float8);
insert into t1 values(1::numeric);
insert into t1 values(0::numeric);
insert into t1 values((-1)::numeric);
insert into t1 values(4294967295::numeric);
insert into t1 values(4294967296::numeric);

insert into t1 values(1::int8);
insert into t1 values(0::int8);
insert into t1 values((-1)::int8);
insert into t1 values(9223372036854775807::int8);
insert into t1 values(9223372036854775808::int8);
insert into t1 values(4294967295::int8);
insert into t1 values(4294967296::int8);

insert into t1 values(1::int4);
insert into t1 values(0::int4);
insert into t1 values((-1)::int4);
insert into t1 values(2147483647::int4);
insert into t1 values(2147483648::int4);

insert into t1 values(1::int2);
insert into t1 values(0::int2);
insert into t1 values((-1)::int2);
insert into t1 values(32767::int2);
insert into t1 values(32768::int2);


insert into t1 values(1::int1);
insert into t1 values(0::int1);
insert into t1 values((-1)::int1);
insert into t1 values(255::int1);
insert into t1 values(256::int1);


insert into t1 values(1::uint8);
insert into t1 values(0::uint8);
insert into t1 values((-1)::uint8);
insert into t1 values(4294967295::uint8);
insert into t1 values(4294967296::uint8);

insert into t1 values(1::uint2);
insert into t1 values(0::uint2);
insert into t1 values((-1)::uint2);
insert into t1 values(65535::uint2);
insert into t1 values(65536::uint2);

insert into t1 values(1::uint1);
insert into t1 values(0::uint1);
insert into t1 values((-1)::uint1);
insert into t1 values(255::uint1);
insert into t1 values(256::uint1);

drop table if exists t1 ;


create table t1(a uint2);

insert into t1 values(1::bool);
insert into t1 values(0::bool);
insert into t1 values((-1)::bool);

insert into t1 values(1.0::float4);
insert into t1 values(0.0::float4);
insert into t1 values((-1.0)::float4);
insert into t1 values(65535::float4);
insert into t1 values(65536::float4);

insert into t1 values(1.0::float8);
insert into t1 values(0.0::float8);
insert into t1 values((-1.0)::float8);
insert into t1 values(65535::float8);
insert into t1 values(65536::float8);

insert into t1 values(1::numeric);
insert into t1 values(0::numeric);
insert into t1 values((-1)::numeric);
insert into t1 values(65535::numeric);
insert into t1 values(65536::numeric);


insert into t1 values(1::int8);
insert into t1 values(0::int8);
insert into t1 values((-1)::int8);
insert into t1 values(9223372036854775807::int8);
insert into t1 values(9223372036854775808::int8);
insert into t1 values(65535::int8);
insert into t1 values(65536::int8);

insert into t1 values(1::int4);
insert into t1 values(0::int4);
insert into t1 values((-1)::int4);
insert into t1 values(2147483647::int4);
insert into t1 values(2147483648::int4);
insert into t1 values(65535::int4);
insert into t1 values(65536::int4);

insert into t1 values(1::int2);
insert into t1 values(0::int2);
insert into t1 values((-1)::int2);
insert into t1 values(32767::int2);
insert into t1 values(32768::int2);

insert into t1 values(1::int1);
insert into t1 values(0::int1);
insert into t1 values((-1)::int1);
insert into t1 values(255::int1);
insert into t1 values(256::int1);


insert into t1 values(1::uint8);
insert into t1 values(0::uint8);
insert into t1 values((-1)::uint8);
insert into t1 values(4294967295::uint8);
insert into t1 values(4294967296::uint8);
insert into t1 values(65535::uint8);
insert into t1 values(65536::uint8);

insert into t1 values(1::uint4);
insert into t1 values(0::uint4);
insert into t1 values((-1)::uint4);
insert into t1 values(4294967295::uint4);
insert into t1 values(4294967296::uint4);
insert into t1 values(65535::uint4);
insert into t1 values(65536::uint4);

insert into t1 values(1::uint1);
insert into t1 values(0::uint1);
insert into t1 values((-1)::uint1);
insert into t1 values(255::uint1);
insert into t1 values(256::uint1);

drop table if exists t1 ;

create table t1(a uint1);
insert into t1 values(1::int8);
insert into t1 values(0::int8);
insert into t1 values((-1)::int8);
insert into t1 values(9223372036854775807::int8);
insert into t1 values(9223372036854775808::int8);
insert into t1 values(255::int8);
insert into t1 values(256::int8);

insert into t1 values(1::bool);
insert into t1 values(0::bool);
insert into t1 values((-1)::bool);

insert into t1 values(1.0::float4);
insert into t1 values(0.0::float4);
insert into t1 values((-1.0)::float4);
insert into t1 values(255::float4);
insert into t1 values(256::float4);

insert into t1 values(1.0::float8);
insert into t1 values(0.0::float8);
insert into t1 values((-1.0)::float8);
insert into t1 values(255::float8);
insert into t1 values(256::float8);

insert into t1 values(1::numeric);
insert into t1 values(0::numeric);
insert into t1 values((-1)::numeric);
insert into t1 values(255::numeric);
insert into t1 values(256::numeric);

insert into t1 values(1::int4);
insert into t1 values(0::int4);
insert into t1 values((-1)::int4);
insert into t1 values(2147483647::int4);
insert into t1 values(2147483648::int4);
insert into t1 values(255::int4);
insert into t1 values(256::int4);

insert into t1 values(1::int2);
insert into t1 values(0::int2);
insert into t1 values((-1)::int2);
insert into t1 values(32767::int2);
insert into t1 values(32768::int2);
insert into t1 values(255::int4);
insert into t1 values(256::int4);

insert into t1 values(1::int1);
insert into t1 values(0::int1);
insert into t1 values((-1)::int1);
insert into t1 values(255::int1);
insert into t1 values(256::int1);

insert into t1 values(1::uint8);
insert into t1 values(0::uint8);
insert into t1 values((-1)::uint8);
insert into t1 values(4294967295::uint8);
insert into t1 values(4294967296::uint8);
insert into t1 values(255::uint8);
insert into t1 values(256::uint8);

insert into t1 values(1::uint4);
insert into t1 values(0::uint4);
insert into t1 values((-1)::uint4);
insert into t1 values(4294967295::uint4);
insert into t1 values(4294967296::uint4);
insert into t1 values(255::uint4);
insert into t1 values(256::uint4);

insert into t1 values(1::uint1);
insert into t1 values(0::uint1);
insert into t1 values((-1)::uint1);
insert into t1 values(255::uint1);
insert into t1 values(256::uint1);

\c postgres
drop database uint_cast2;
drop database if exists tinyint_cast;
create database tinyint_cast dbcompatibility 'b';
\c tinyint_cast

create table t1(a int1);

insert into t1 values('');
insert into t1 values('1.4');
insert into t1 values('-1.5');

select (0)::int1::bool;
select (1)::int1::bool;

select (-1)::bool::int1;
select (0)::bool::int1;
select (1)::bool::int1;
select (-127.5)::float4::int1;
select (-128.4)::float4::int1;
select (127.9)::float4::int1;
select (127.3)::float4::int1;
select (-127.5)::float8::int1;
select (-128.4)::float8::int1;
select (127.9)::float8::int1;
select (127.3)::float8::int1;

select (-127.5)::numeric::int1;
select (-128.4)::numeric::int1;
select (127.9)::numeric::int1;
select (127.3)::numeric::int1;

select (-128)::int2::int1;
select (127)::int2::int1;
select (32767)::int2::int1;
select (-32768)::int2::int1;

select (127)::int4::int1;
select (-128)::int4::int1;
select (2147483647)::int4::int1;
select (-2147483648)::int4::int1;

select (127)::int8::int1;
select (-128)::int8::int1;
select (9223372036854775807)::int8::int1;
select (-9223372036854775808)::int8::int1;

select (-1)::int1::uint1;
select (0)::int1::uint1;
select (1)::int1::uint1;
select (127)::int1::uint1;
select (-128)::int1::uint1;

select (-1)::int1::uint2;
select (0)::int1::uint2;
select (1)::int1::uint2;
select (127)::int1::uint2;
select (-128)::int1::uint2;

select (-1)::int1::uint4;
select (0)::int1::uint4;
select (1)::int1::uint4;
select (127)::int1::uint4;
select (-128)::int1::uint4;

select (-1)::int1::uint8;
select (0)::int1::uint8;
select (1)::int1::uint8;
select (127)::int1::uint8;
select (-128)::int1::uint8;

select 255::uint1::int1;

select 255::uint2::int1;
select 65535::uint2::int1;

select 255::uint4::int1;
select 4294967295::uint4::int1;

select 255::uint8::int1;
select 18446744073709551615::uint8::int1;

select 1::int1::int16;
select 1::int16::int1;

select 1::int1::clob;
select 1::clob::int1;

select 1::int1::bpchar;
select 1::bpchar::int1;

select 1::int1::varchar;
select 1::varchar::int1;

select 1::int1::nvarchar2;
select 1::nvarchar2::int1;

select 1::int1::interval;

select (-1)::text::int1;
select '127'::text::int1;
select '-128'::text::int1;

\c postgres
drop database tinyint_cast;
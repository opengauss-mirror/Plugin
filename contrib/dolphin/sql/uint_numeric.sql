create schema uint_numeric;
set current_schema to 'uint_numeric';

select (-1)::numeric::uint1;
select (-1)::numeric::uint2;
select (-1)::numeric::uint4;
select (-1)::numeric::uint8;

select null::numeric::uint1;
select null::numeric::uint2;
select null::numeric::uint4;
select null::numeric::uint8;

select 255::numeric::uint1;
select 65535::numeric::uint2;
select 4294967295::numeric::uint4;
select 18446744073709551615::numeric(30)::uint8;

select 256::numeric::uint1;
select 65536::numeric::uint2;
select 4294967296::numeric::uint4;
select 18446744073709551616::numeric(30)::uint8;

select (-1)::float4::uint1;
select (-1)::float4::uint2;
select (-1)::float4::uint4;
select (-1)::float4::uint8;

select null::float4::uint1;
select null::float4::uint2;
select null::float4::uint4;
select null::float4::uint8;

select 255::float4::uint1;
select 65535::float4::uint2;
select 4294967295::float4::uint4;
select 18446744073709551615::float4::uint8;

select 256::float4::uint1;
select 65536::float4::uint2;
select 4294967296::float4::uint4;
select 18446744073709551616::float4::uint8;

select (-1)::float8::uint1;
select (-1)::float8::uint2;
select (-1)::float8::uint4;
select (-1)::float8::uint8;

select null::float8::uint1;
select null::float8::uint2;
select null::float8::uint4;
select null::float8::uint8;

select 255::float8::uint1;
select 65535::float8::uint2;
select 4294967295::float8::uint4;
select 18446744073709551615::float8::uint8;

select 256::float8::uint1;
select 65536::float8::uint2;
select 4294967296::float8::uint4;
select 18446744073709551616::float8::uint8;

create table t1(a uint1);
create table t2(a uint2);
create table t3(a uint4);
create table t4(a uint8);

create table num(a float4, b float8, c numeric);
insert into num values(-1, -1, -1);

insert into t1 values((-1)::float4);
insert into t1 values((-1)::float8);
insert into t1 values((-1)::numeric);

insert into t2 values((-1)::float4);
insert into t2 values((-1)::float8);
insert into t2 values((-1)::numeric);

insert into t3 values((-1)::float4);
insert into t3 values((-1)::float8);
insert into t3 values((-1)::numeric);

insert into t4 values((-1)::float4);
insert into t4 values((-1)::float8);
insert into t4 values((-1)::numeric);

insert into t1 select a from num;
insert into t1 select b from num;
insert into t1 select c from num;

insert into t2 select a from num;
insert into t2 select b from num;
insert into t2 select c from num;

insert into t3 select a from num;
insert into t3 select b from num;
insert into t3 select c from num;

insert into t4 select a from num;
insert into t4 select b from num;
insert into t4 select c from num;


drop schema uint_numeric cascade;
reset current_schema;
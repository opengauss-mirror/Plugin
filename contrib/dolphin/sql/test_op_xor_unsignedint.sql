drop database if exists test_op_xor_unsignedint;
create database test_op_xor_unsignedint with dbcompatibility='B';
\c test_op_xor_unsignedint

select (-1)::uint1 ^ 2::int1;
select (-1)::uint1 ^ 2::int2;
select (-1)::uint1 ^ 2::int4;
select (-1)::uint1 ^ 2::int8;
select (-1)::uint1 ^ 2::uint1;
select (-1)::uint1 ^ 2::uint2;
select (-1)::uint1 ^ 2::uint4;
select (-1)::uint1 ^ 2::uint8;
select (-1)::uint1 ^ 2::bool;
select (-1)::uint1 ^ 2::char;
select (-1)::uint1 ^ 2::varchar;


select (-10)::uint1 ^ (-2)::int2;
select (-10)::uint1 ^ (-2)::int4;
select (-10)::uint1 ^ (-2)::int8;
select (-10)::uint1 ^ (-2)::uint1;
select (-10)::uint1 ^ (-2)::uint2;
select (-10)::uint1 ^ (-2)::uint4;
select (-10)::uint1 ^ (-2)::uint8;
select (-10)::uint1 ^ (-2)::bool;
select (-10)::uint1 ^ (-2)::varchar;


select 2 ::uint1 ^ 2::int1;
select 2 ::uint1 ^ 2::int2;
select 2 ::uint1 ^ 2::int4;
select 2 ::uint1 ^ 2::int8;
select 2 ::uint1 ^ 2::uint1;
select 2 ::uint1 ^ 2::uint2;
select 2 ::uint1 ^ 2::uint4;
select 2 ::uint1 ^ 2::uint8;
select 2 ::uint1 ^ 2::bool;
select 2 ::uint1 ^ 2::varchar;



select (-1)::uint2 ^ 2::int1;
select (-1)::uint2 ^ 2::int2;
select (-1)::uint2 ^ 2::int4;
select (-1)::uint2 ^ 2::int8;
select (-1)::uint2 ^ 2::uint1;
select (-1)::uint2 ^ 2::uint2;
select (-1)::uint2 ^ 2::uint4;
select (-1)::uint2 ^ 2::uint8;
select (-1)::uint2 ^ 2::bool;
select (-1)::uint2 ^ 2::char;
select (-1)::uint2 ^ 2::varchar;


select (-10)::uint2 ^ (-2)::int2;
select (-10)::uint2 ^ (-2)::int4;
select (-10)::uint2 ^ (-2)::int8;
select (-10)::uint2 ^ (-2)::uint1;
select (-10)::uint2 ^ (-2)::uint2;
select (-10)::uint2 ^ (-2)::uint4;
select (-10)::uint2 ^ (-2)::uint8;
select (-10)::uint2 ^ (-2)::bool;
select (-10)::uint2 ^ (-2)::varchar;


select 2 ::uint2 ^ 2::int1;
select 2 ::uint2 ^ 2::int2;
select 2 ::uint2 ^ 2::int4;
select 2 ::uint2 ^ 2::int8;
select 2 ::uint2 ^ 2::uint1;
select 2 ::uint2 ^ 2::uint2;
select 2 ::uint2 ^ 2::uint4;
select 2 ::uint2 ^ 2::uint8;
select 2 ::uint2 ^ 2::bool;
select 2 ::uint2 ^ 2::char;
select 2 ::uint2 ^ 2::varchar;



select (-1)::uint4 ^ 2::int1;
select (-1)::uint4 ^ 2::int2;
select (-1)::uint4 ^ 2::int4;
select (-1)::uint4 ^ 2::int8;
select (-1)::uint4 ^ 2::uint1;
select (-1)::uint4 ^ 2::uint2;
select (-1)::uint4 ^ 2::uint4;
select (-1)::uint4 ^ 2::uint8;
select (-1)::uint4 ^ 2::bool;
select (-1)::uint4 ^ 2::char;
select (-1)::uint4 ^ 2::varchar;


select (-10)::uint4 ^ (-2)::int2;
select (-10)::uint4 ^ (-2)::int4;
select (-10)::uint4 ^ (-2)::int8;
select (-10)::uint4 ^ (-2)::uint1;
select (-10)::uint4 ^ (-2)::uint2;
select (-10)::uint4 ^ (-2)::uint4;
select (-10)::uint4 ^ (-2)::uint8;
select (-10)::uint4 ^ (-2)::bool;
select (-10)::uint4 ^ (-2)::varchar;


select 2 ::uint4 ^ 2::int1;
select 2 ::uint4 ^ 2::int2;
select 2 ::uint4 ^ 2::int4;
select 2 ::uint4 ^ 2::int8;
select 2 ::uint4 ^ 2::uint1;
select 2 ::uint4 ^ 2::uint2;
select 2 ::uint4 ^ 2::uint4;
select 2 ::uint4 ^ 2::uint8;
select 2 ::uint4 ^ 2::bool;
select 2 ::uint4 ^ 2::char;
select 2 ::uint4 ^ 2::varchar;


select (-1)::uint8 ^ 2::int1;
select (-1)::uint8 ^ 2::int2;
select (-1)::uint8 ^ 2::int4;
select (-1)::uint8 ^ 2::int8;
select (-1)::uint8 ^ 2::uint1;
select (-1)::uint8 ^ 2::uint2;
select (-1)::uint8 ^ 2::uint4;
select (-1)::uint8 ^ 2::uint8;
select (-1)::uint8 ^ 2::bool;
select (-1)::uint8 ^ 2::char;
select (-1)::uint8 ^ 2::varchar;


select (-10)::uint8 ^ (-2)::int2;
select (-10)::uint8 ^ (-2)::int4;
select (-10)::uint8 ^ (-2)::int8;
select (-10)::uint8 ^ (-2)::uint1;
select (-10)::uint8 ^ (-2)::uint2;
select (-10)::uint8 ^ (-2)::uint4;
select (-10)::uint8 ^ (-2)::uint8;
select (-10)::uint8 ^ (-2)::bool;
select (-10)::uint8 ^ (-2)::varchar;


select 2 ::uint8 ^ 2::int1;
select 2 ::uint8 ^ 2::int2;
select 2 ::uint8 ^ 2::int4;
select 2 ::uint8 ^ 2::int8;
select 2 ::uint8 ^ 2::uint1;
select 2 ::uint8 ^ 2::uint2;
select 2 ::uint8 ^ 2::uint4;
select 2 ::uint8 ^ 2::uint8;
select 2 ::uint8 ^ 2::bool;
select 2 ::uint8 ^ 2::char;
select 2 ::uint8 ^ 2::varchar;

\c postgres
drop database test_op_xor_unsignedint;

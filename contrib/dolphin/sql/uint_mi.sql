drop database if exists uint_mi;
create database uint_mi dbcompatibility 'b';
\c uint_mi

--uint8
select 18446744073709551615::uint8 - 0::int1;
select 18446744073709551615::uint8 - 0::int2;
select 18446744073709551615::uint8 - 0::int4;
select 18446744073709551615::uint8 - 0::int8;
select 18446744073709551615::uint8 - 0::uint1;
select 18446744073709551615::uint8 - 0::uint2;
select 18446744073709551615::uint8 - 0::uint4;
select 18446744073709551615::uint8 - 0::uint8;

select 18446744073709551615::uint8 - null::int1;
select 18446744073709551615::uint8 - null::int2;
select 18446744073709551615::uint8 - null::int4;
select 18446744073709551615::uint8 - null::int8;
select 18446744073709551615::uint8 - null::uint1;
select 18446744073709551615::uint8 - null::uint2;
select 18446744073709551615::uint8 - null::uint4;
select 18446744073709551615::uint8 - null::uint8;

select 18446744073709551615::uint8 - (-1)::int2;
select 18446744073709551615::uint8 - (-1)::int4;
select 18446744073709551615::uint8 - (-1)::int8;

select 0::uint8 - 1::int1;
select 0::uint8 - 1::int2;
select 0::uint8 - 1::int4;
select 0::uint8 - 1::int8;
select 0::uint8 - 1::uint1;
select 0::uint8 - 1::uint2;
select 0::uint8 - 1::uint4;
select 0::uint8 - 1::uint8;

--uint4
select 4294967295::uint4 - 0::int1;
select 4294967295::uint4 - 0::int2;
select 4294967295::uint4 - 0::int4;
select 4294967295::uint4 - 0::int8;
select 4294967295::uint4 - 0::uint1;
select 4294967295::uint4 - 0::uint2;
select 4294967295::uint4 - 0::uint4;
select 4294967295::uint4 - 0::uint8;

select 4294967295::uint4 - null::int1;
select 4294967295::uint4 - null::int2;
select 4294967295::uint4 - null::int4;
select 4294967295::uint4 - null::int8;
select 4294967295::uint4 - null::uint1;
select 4294967295::uint4 - null::uint2;
select 4294967295::uint4 - null::uint4;
select 4294967295::uint4 - null::uint8;

select 4294967295::uint4 - (-1)::int2;
select 4294967295::uint4 - (-1)::int4;
select 4294967295::uint4 - (-1)::int8;

select 4294967295::uint4 - 9223372036854775807::int8;
select 4294967295::uint4 - 18446744073709551615::uint8;

select 0::uint4 - 1::int1;
select 0::uint4 - 1::int2;
select 0::uint4 - 1::int4;
select 0::uint4 - 1::int8;
select 0::uint4 - 1::uint1;
select 0::uint4 - 1::uint2;
select 0::uint4 - 1::uint4;
select 0::uint4 - 1::uint8;

--uint2
select 65535::uint2 - 0::int1;
select 65535::uint2 - 0::int2;
select 65535::uint2 - 0::int4;
select 65535::uint2 - 0::int8;
select 65535::uint2 - 0::uint1;
select 65535::uint2 - 0::uint2;
select 65535::uint2 - 0::uint4;
select 65535::uint2 - 0::uint8;

select 65535::uint2 - null::int1;
select 65535::uint2 - null::int2;
select 65535::uint2 - null::int4;
select 65535::uint2 - null::int8;
select 65535::uint2 - null::uint1;
select 65535::uint2 - null::uint2;
select 65535::uint2 - null::uint4;
select 65535::uint2 - null::uint8;

select 65535::uint2 - (-1)::int2;
select 65535::uint2 - (-1)::int4;
select 65535::uint2 - (-1)::int8;

select 65535::uint2 - 2147483647::int4;
select 65535::uint2 - 9223372036854775807::int8;
select 65535::uint2 - 4294967295::uint4;
select 65535::uint2 - 18446744073709551615::uint8;

select 0::uint2 - 1::int1;
select 0::uint2 - 1::int2;
select 0::uint2 - 1::int4;
select 0::uint2 - 1::int8;
select 0::uint2 - 1::uint1;
select 0::uint2 - 1::uint2;
select 0::uint2 - 1::uint4;
select 0::uint2 - 1::uint8;

--uint1
select 255::uint1 - 0::int1;
select 255::uint1 - 0::int2;
select 255::uint1 - 0::int4;
select 255::uint1 - 0::int8;
select 255::uint1 - 0::uint1;
select 255::uint1 - 0::uint2;
select 255::uint1 - 0::uint4;
select 255::uint1 - 0::uint8;

select 255::uint1 - null::int1;
select 255::uint1 - null::int2;
select 255::uint1 - null::int4;
select 255::uint1 - null::int8;
select 255::uint1 - null::uint1;
select 255::uint1 - null::uint2;
select 255::uint1 - null::uint4;
select 255::uint1 - null::uint8;

select 255::uint1 - (-1)::int2;
select 255::uint1 - (-1)::int4;
select 255::uint1 - (-1)::int8;

select 255::uint1 - 32767::int2;
select 255::uint1 - 2147483647::int4;
select 255::uint1 - 9223372036854775807::int8;
select 255::uint1 - 65535::uint2;
select 255::uint1 - 4294967295::uint4;
select 255::uint1 - 18446744073709551615::uint8;

select 0::uint1 - 1::int1;
select 0::uint1 - 1::int2;
select 0::uint1 - 1::int4;
select 0::uint1 - 1::int8;
select 0::uint1 - 1::uint1;
select 0::uint1 - 1::uint2;
select 0::uint1 - 1::uint4;
select 0::uint1 - 1::uint8;

--int8
select 9223372036854775807::int8 - 0::uint1;
select 9223372036854775807::int8 - 0::uint2;
select 9223372036854775807::int8 - 0::uint4;
select 9223372036854775807::int8 - 0::uint8;

select 9223372036854775807::int8 - null::uint1;
select 9223372036854775807::int8 - null::uint2;
select 9223372036854775807::int8 - null::uint4;
select 9223372036854775807::int8 - null::uint8;

select 9223372036854775807::int8 - 18446744073709551615::uint8;

select 0::int8 - 1::uint1;
select 0::int8 - 1::uint2;
select 0::int8 - 1::uint4;
select 0::int8 - 1::uint8;

--int4
select 2147483647::int4 - 0::uint1;
select 2147483647::int4 - 0::uint2;
select 2147483647::int4 - 0::uint4;
select 2147483647::int4 - 0::uint8;

select 2147483647::int4 - null::uint1;
select 2147483647::int4 - null::uint2;
select 2147483647::int4 - null::uint4;
select 2147483647::int4 - null::uint8;

select 2147483647::int4 - 4294967295::uint4;
select 2147483647::int4 - 18446744073709551615::uint8;

select 0::int4 - 1::uint1;
select 0::int4 - 1::uint2;
select 0::int4 - 1::uint4;
select 0::int4 - 1::uint8;

--int2
select 32767::int2 - 0::uint1;
select 32767::int2 - 0::uint2;
select 32767::int2 - 0::uint4;
select 32767::int2 - 0::uint8;

select 32767::int2 - null::uint1;
select 32767::int2 - null::uint2;
select 32767::int2 - null::uint4;
select 32767::int2 - null::uint8;

select 32767::int2 - 65535::uint2;
select 32767::int2 - 4294967295::uint4;
select 32767::int2 - 18446744073709551615::uint8;

select 0::int2 - 1::uint1;
select 0::int2 - 1::uint2;
select 0::int2 - 1::uint4;
select 0::int2 - 1::uint8;
--int1
select 255::int1 - 0::uint1;
select 255::int1 - 0::uint2;
select 255::int1 - 0::uint4;
select 255::int1 - 0::uint8;

select 255::int1 - null::uint1;
select 255::int1 - null::uint2;
select 255::int1 - null::uint4;
select 255::int1 - null::uint8;

select 255::int1 - 65535::uint2;
select 255::int1 - 4294967295::uint4;
select 255::int1 - 18446744073709551615::uint8;

select 0::int1 - 1::uint1;
select 0::int1 - 1::uint2;
select 0::int1 - 1::uint4;
select 0::int1 - 1::uint8;

\c postgres
drop database uint_mi
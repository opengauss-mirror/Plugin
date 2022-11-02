drop database if exists uint_xor;
create database uint_xor dbcompatibility 'b';
\c uint_xor

--uint8
select 18446744073709551615::uint8 # 0::int1;
select 18446744073709551615::uint8 # 0::int2;
select 18446744073709551615::uint8 # 0::int4;
select 18446744073709551615::uint8 # 0::int8;
select 18446744073709551615::uint8 # 0::uint1;
select 18446744073709551615::uint8 # 0::uint2;
select 18446744073709551615::uint8 # 0::uint4;
select 18446744073709551615::uint8 # 0::uint8;

select 18446744073709551615::uint8 # 127::int1;
select 18446744073709551615::uint8 # 32767::int2;
select 18446744073709551615::uint8 # 2147483647::int4;
select 18446744073709551615::uint8 # 9223372036854775807::int8;
select 18446744073709551615::uint8 # 255::uint1;
select 18446744073709551615::uint8 # 65535::uint2;
select 18446744073709551615::uint8 # 4294967295::uint4;
select 18446744073709551615::uint8 # 18446744073709551615::uint8;

select 18446744073709551615::uint8 # (-32768)::int2;
select 18446744073709551615::uint8 # (-2147483648)::int4;
select 18446744073709551615::uint8 # (-9223372036854775808)::int8;

--uint4
select 4294967295::uint4 # 0::int1;
select 4294967295::uint4 # 0::int2;
select 4294967295::uint4 # 0::int4;
select 4294967295::uint4 # 0::int8;
select 4294967295::uint4 # 0::uint1;
select 4294967295::uint4 # 0::uint2;
select 4294967295::uint4 # 0::uint4;
select 4294967295::uint4 # 0::uint8;

select 4294967295::uint4 # 127::int1;
select 4294967295::uint4 # 32767::int2;
select 4294967295::uint4 # 2147483647::int4;
select 4294967295::uint4 # 9223372036854775807::int8;
select 4294967295::uint4 # 255::uint1;
select 4294967295::uint4 # 65535::uint2;
select 4294967295::uint4 # 4294967295::uint4;
select 4294967295::uint4 # 18446744073709551615::uint8;

select 4294967295::uint4 # (-32768)::int2;
select 4294967295::uint4 # (-2147483648)::int4;
select 4294967295::uint4 # (-9223372036854775808)::int8;


--uint2
select 65535::uint2 # 0::int1;
select 65535::uint2 # 0::int2;
select 65535::uint2 # 0::int4;
select 65535::uint2 # 0::int8;
select 65535::uint2 # 0::uint1;
select 65535::uint2 # 0::uint2;
select 65535::uint2 # 0::uint4;
select 65535::uint2 # 0::uint8;

select 65535::uint2 # 127::int1;
select 65535::uint2 # 32767::int2;
select 65535::uint2 # 2147483647::int4;
select 65535::uint2 # 9223372036854775807::int8;
select 65535::uint2 # 255::uint1;
select 65535::uint2 # 65535::uint2;
select 65535::uint2 # 4294967295::uint4;
select 65535::uint2 # 18446744073709551615::uint8;

select 65535::uint2 # (-32768)::int2;
select 65535::uint2 # (-2147483648)::int4;
select 65535::uint2 # (-9223372036854775808)::int8;

--uint1
select 255::uint1 # 0::int1;
select 255::uint1 # 0::int2;
select 255::uint1 # 0::int4;
select 255::uint1 # 0::int8;
select 255::uint1 # 0::uint1;
select 255::uint1 # 0::uint2;
select 255::uint1 # 0::uint4;
select 255::uint1 # 0::uint8;

select 255::uint1 # 127::int1;
select 255::uint1 # 32767::int2;
select 255::uint1 # 2147483647::int4;
select 255::uint1 # 9223372036854775807::int8;
select 255::uint1 # 255::uint1;
select 255::uint1 # 65535::uint2;
select 255::uint1 # 4294967295::uint4;
select 255::uint1 # 18446744073709551615::uint8;

select 255::uint1 # (-32768)::int2;
select 255::uint1 # (-2147483648)::int4;
select 255::uint1 # (-9223372036854775808)::int8;

--int8
select 9223372036854775807::int8 # 0::uint1;
select 9223372036854775807::int8 # 0::uint2;
select 9223372036854775807::int8 # 0::uint4;
select 9223372036854775807::int8 # 0::uint8;

select 9223372036854775807::int8 # 255::uint1;
select 9223372036854775807::int8 # 65535::uint2;
select 9223372036854775807::int8 # 4294967295::uint4;
select 9223372036854775807::int8 # 18446744073709551615::uint8;

select 9223372036854775807::int8 # 1::uint1;
select 9223372036854775807::int8 # 1::uint2;
select 9223372036854775807::int8 # 1::uint4;
select 9223372036854775807::int8 # 1::uint8;

select (-9223372036854775808)::int8 # 255::uint1;
select (-9223372036854775808)::int8 # 65535::uint2;
select (-9223372036854775808)::int8 # 4294967295::uint4;
select (-9223372036854775808)::int8 # 18446744073709551615::uint8;

--int4
select 2147483647::int4 # 0::uint1;
select 2147483647::int4 # 0::uint2;
select 2147483647::int4 # 0::uint4;
select 2147483647::int4 # 0::uint8;

select 2147483647::int4 # 255::uint1;
select 2147483647::int4 # 65535::uint2;
select 2147483647::int4 # 4294967295::uint4;
select 2147483647::int4 # 18446744073709551615::uint8;

select 2147483647::int4 # 1::uint1;
select 2147483647::int4 # 1::uint2;
select 2147483647::int4 # 1::uint4;
select 2147483647::int4 # 1::uint8;

select (-2147483648)::int4 # 255::uint1;
select (-2147483648)::int4 # 65535::uint2;
select (-2147483648)::int4 # 4294967295::uint4;
select (-2147483648)::int4 # 18446744073709551615::uint8;

--int2
select 32767::int2 # 0::uint1;
select 32767::int2 # 0::uint2;
select 32767::int2 # 0::uint4;
select 32767::int2 # 0::uint8;

select 32767::int2 # 255::uint1;
select 32767::int2 # 65535::uint2;
select 32767::int2 # 4294967295::uint4;
select 32767::int2 # 18446744073709551615::uint8;

select 32767::int2 # 1::uint1;
select 32767::int2 # 1::uint2;
select 32767::int2 # 1::uint4;
select 32767::int2 # 1::uint8;

select (-32768)::int2 # 255::uint1;
select (-32768)::int2 # 65535::uint2;
select (-32768)::int2 # 4294967295::uint4;
select (-32768)::int2 # 18446744073709551615::uint8;

--int1
select 127::int1 # 0::uint1;
select 127::int1 # 0::uint2;
select 127::int1 # 0::uint4;
select 127::int1 # 0::uint8;

select 127::int1 # 255::uint1;
select 127::int1 # 65535::uint2;
select 127::int1 # 4294967295::uint4;
select 127::int1 # 18446744073709551615::uint8;

select 127::int1 # 1::uint1;
select 127::int1 # 1::uint2;
select 127::int1 # 1::uint4;
select 127::int1 # 1::uint8;

\c postgres
drop database uint_xor
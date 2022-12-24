create schema uint_div;
set current_schema to 'uint_div';

--uint8
select 18446744073709551615::uint8 / 0::int1;
select 18446744073709551615::uint8 / 0::int2;
select 18446744073709551615::uint8 / 0::int4;
select 18446744073709551615::uint8 / 0::int8;
select 18446744073709551615::uint8 / 0::uint1;
select 18446744073709551615::uint8 / 0::uint2;
select 18446744073709551615::uint8 / 0::uint4;
select 18446744073709551615::uint8 / 0::uint8;

select 18446744073709551615::uint8 / 2::int1;
select 18446744073709551615::uint8 / 2::int2;
select 18446744073709551615::uint8 / 2::int4;
select 18446744073709551615::uint8 / 2::int8;
select 18446744073709551615::uint8 / 2::uint1;
select 18446744073709551615::uint8 / 2::uint2;
select 18446744073709551615::uint8 / 2::uint4;
select 18446744073709551615::uint8 / 2::uint8;

select 18446744073709551615::uint8 / (-1)::int2;
select 18446744073709551615::uint8 / (-1)::int4;
select 18446744073709551615::uint8 / (-1)::int8;

--uint4
select 4294967295::uint4 / 0::int1;
select 4294967295::uint4 / 0::int2;
select 4294967295::uint4 / 0::int4;
select 4294967295::uint4 / 0::int8;
select 4294967295::uint4 / 0::uint1;
select 4294967295::uint4 / 0::uint2;
select 4294967295::uint4 / 0::uint4;
select 4294967295::uint4 / 0::uint8;

select 4294967295::uint4 / (-1)::int2;
select 4294967295::uint4 / (-1)::int4;
select 4294967295::uint4 / (-1)::int8;

select 4294967295::uint4 / 9223372036854775807::int8;
select 4294967295::uint4 / 18446744073709551615::uint8;

--uint2
select 65535::uint2 / 0::int1;
select 65535::uint2 / 0::int2;
select 65535::uint2 / 0::int4;
select 65535::uint2 / 0::int8;
select 65535::uint2 / 0::uint1;
select 65535::uint2 / 0::uint2;
select 65535::uint2 / 0::uint4;
select 65535::uint2 / 0::uint8;

select 65535::uint2 / (-1)::int2;
select 65535::uint2 / (-1)::int4;
select 65535::uint2 / (-1)::int8;

select 65535::uint2 / 2147483647::int4;
select 65535::uint2 / 9223372036854775807::int8;
select 65535::uint2 / 4294967295::uint4;
select 65535::uint2 / 18446744073709551615::uint8;

--uint1
select 255::uint1 / 0::int1;
select 255::uint1 / 0::int2;
select 255::uint1 / 0::int4;
select 255::uint1 / 0::int8;
select 255::uint1 / 0::uint1;
select 255::uint1 / 0::uint2;
select 255::uint1 / 0::uint4;
select 255::uint1 / 0::uint8;

select 255::uint1 / (-1)::int2;
select 255::uint1 / (-1)::int4;
select 255::uint1 / (-1)::int8;

select 255::uint1 / 32767::int2;
select 255::uint1 / 2147483647::int4;
select 255::uint1 / 9223372036854775807::int8;
select 255::uint1 / 65535::uint2;
select 255::uint1 / 4294967295::uint4;
select 255::uint1 / 18446744073709551615::uint8;

--int8
select 9223372036854775807::int8 / 0::uint1;
select 9223372036854775807::int8 / 0::uint2;
select 9223372036854775807::int8 / 0::uint4;
select 9223372036854775807::int8 / 0::uint8;

select 9223372036854775807::int8 / 18446744073709551615::uint8;

select 9223372036854775807::int8 / 1::uint1;
select 9223372036854775807::int8 / 1::uint2;
select 9223372036854775807::int8 / 1::uint4;
select 9223372036854775807::int8 / 1::uint8;

--int4
select 2147483647::int4 / 0::uint1;
select 2147483647::int4 / 0::uint2;
select 2147483647::int4 / 0::uint4;
select 2147483647::int4 / 0::uint8;

select 2147483647::int4 / 4294967295::uint4;
select 2147483647::int4 / 18446744073709551615::uint8;

select 2147483647::int4 / 1::uint1;
select 2147483647::int4 / 1::uint2;
select 2147483647::int4 / 1::uint4;
select 2147483647::int4 / 1::uint8;

--int2
select 32767::int2 / 0::uint1;
select 32767::int2 / 0::uint2;
select 32767::int2 / 0::uint4;
select 32767::int2 / 0::uint8;

select 32767::int2 / 65535::uint2;
select 32767::int2 / 4294967295::uint4;
select 32767::int2 / 18446744073709551615::uint8;

select 32767::int2 / 1::uint1;
select 32767::int2 / 1::uint2;
select 32767::int2 / 1::uint4;
select 32767::int2 / 1::uint8;
--int1
select 127::int1 / 0::uint1;
select 127::int1 / 0::uint2;
select 127::int1 / 0::uint4;
select 127::int1 / 0::uint8;

select 127::int1 / 65535::uint2;
select 127::int1 / 4294967295::uint4;
select 127::int1 / 18446744073709551615::uint8;

select 127::int1 / 1::uint1;
select 127::int1 / 1::uint2;
select 127::int1 / 1::uint4;
select 127::int1 / 1::uint8;

drop schema uint_div cascade;
reset current_schema;
drop database if exists uint_mod2;
create database uint_mod2 dbcompatibility 'b';
\c uint_mod2

--uint8
select 18446744073709551615::uint8 mod 0::int1;
select 18446744073709551615::uint8 mod 0::int2;
select 18446744073709551615::uint8 mod 0::int4;
select 18446744073709551615::uint8 mod 0::int8;
select 18446744073709551615::uint8 mod 0::uint1;
select 18446744073709551615::uint8 mod 0::uint2;
select 18446744073709551615::uint8 mod 0::uint4;
select 18446744073709551615::uint8 mod 0::uint8;

select 18446744073709551615::uint8 mod null::int1;
select 18446744073709551615::uint8 mod null::int2;
select 18446744073709551615::uint8 mod null::int4;
select 18446744073709551615::uint8 mod null::int8;
select 18446744073709551615::uint8 mod null::uint1;
select 18446744073709551615::uint8 mod null::uint2;
select 18446744073709551615::uint8 mod null::uint4;
select 18446744073709551615::uint8 mod null::uint8;

select 18446744073709551615::uint8 mod 2::int1;
select 18446744073709551615::uint8 mod 2::int2;
select 18446744073709551615::uint8 mod 2::int4;
select 18446744073709551615::uint8 mod 2::int8;
select 18446744073709551615::uint8 mod 2::uint1;
select 18446744073709551615::uint8 mod 2::uint2;
select 18446744073709551615::uint8 mod 2::uint4;
select 18446744073709551615::uint8 mod 2::uint8;

select 18446744073709551615::uint8 mod (-1)::int2;
select 18446744073709551615::uint8 mod (-1)::int4;
select 18446744073709551615::uint8 mod (-1)::int8;

--uint4
select 4294967295::uint4 mod 0::int1;
select 4294967295::uint4 mod 0::int2;
select 4294967295::uint4 mod 0::int4;
select 4294967295::uint4 mod 0::int8;
select 4294967295::uint4 mod 0::uint1;
select 4294967295::uint4 mod 0::uint2;
select 4294967295::uint4 mod 0::uint4;
select 4294967295::uint4 mod 0::uint8;

select 4294967295::uint4 mod null::int1;
select 4294967295::uint4 mod null::int2;
select 4294967295::uint4 mod null::int4;
select 4294967295::uint4 mod null::int8;
select 4294967295::uint4 mod null::uint1;
select 4294967295::uint4 mod null::uint2;
select 4294967295::uint4 mod null::uint4;
select 4294967295::uint4 mod null::uint8;

select 4294967295::uint4 mod (-1)::int2;
select 4294967295::uint4 mod (-1)::int4;
select 4294967295::uint4 mod (-1)::int8;

select 4294967295::uint4 mod 9223372036854775807::int8;
select 4294967295::uint4 mod 18446744073709551615::uint8;

--uint2
select 65535::uint2 mod 0::int1;
select 65535::uint2 mod 0::int2;
select 65535::uint2 mod 0::int4;
select 65535::uint2 mod 0::int8;
select 65535::uint2 mod 0::uint1;
select 65535::uint2 mod 0::uint2;
select 65535::uint2 mod 0::uint4;
select 65535::uint2 mod 0::uint8;

select 65535::uint2 mod null::int1;
select 65535::uint2 mod null::int2;
select 65535::uint2 mod null::int4;
select 65535::uint2 mod null::int8;
select 65535::uint2 mod null::uint1;
select 65535::uint2 mod null::uint2;
select 65535::uint2 mod null::uint4;
select 65535::uint2 mod null::uint8;

select 65535::uint2 mod (-1)::int2;
select 65535::uint2 mod (-1)::int4;
select 65535::uint2 mod (-1)::int8;

select 65535::uint2 mod 2147483647::int4;
select 65535::uint2 mod 9223372036854775807::int8;
select 65535::uint2 mod 4294967295::uint4;
select 65535::uint2 mod 18446744073709551615::uint8;

--uint1
select 255::uint1 mod 0::int1;
select 255::uint1 mod 0::int2;
select 255::uint1 mod 0::int4;
select 255::uint1 mod 0::int8;
select 255::uint1 mod 0::uint1;
select 255::uint1 mod 0::uint2;
select 255::uint1 mod 0::uint4;
select 255::uint1 mod 0::uint8;

select 255::uint1 mod null::int1;
select 255::uint1 mod null::int2;
select 255::uint1 mod null::int4;
select 255::uint1 mod null::int8;
select 255::uint1 mod null::uint1;
select 255::uint1 mod null::uint2;
select 255::uint1 mod null::uint4;
select 255::uint1 mod null::uint8;

select 255::uint1 mod (-1)::int2;
select 255::uint1 mod (-1)::int4;
select 255::uint1 mod (-1)::int8;

select 255::uint1 mod 32767::int2;
select 255::uint1 mod 2147483647::int4;
select 255::uint1 mod 9223372036854775807::int8;
select 255::uint1 mod 65535::uint2;
select 255::uint1 mod 4294967295::uint4;
select 255::uint1 mod 18446744073709551615::uint8;

--int8
select 9223372036854775807::int8 mod 0::uint1;
select 9223372036854775807::int8 mod 0::uint2;
select 9223372036854775807::int8 mod 0::uint4;
select 9223372036854775807::int8 mod 0::uint8;

select 9223372036854775807::int8 mod null::uint1;
select 9223372036854775807::int8 mod null::uint2;
select 9223372036854775807::int8 mod null::uint4;
select 9223372036854775807::int8 mod null::uint8;

select 9223372036854775807::int8 mod 18446744073709551615::uint8;

select 9223372036854775807::int8 mod 1::uint1;
select 9223372036854775807::int8 mod 1::uint2;
select 9223372036854775807::int8 mod 1::uint4;
select 9223372036854775807::int8 mod 1::uint8;

select (-9223372036854775808)::int8 mod 255::uint1;
select (-9223372036854775808)::int8 mod 65535::uint2;
select (-9223372036854775808)::int8 mod 4294967295::uint4;
select (-9223372036854775808)::int8 mod 18446744073709551615::uint8;

--int4
select 2147483647::int4 mod 0::uint1;
select 2147483647::int4 mod 0::uint2;
select 2147483647::int4 mod 0::uint4;
select 2147483647::int4 mod 0::uint8;

select 2147483647::int4 mod null::uint1;
select 2147483647::int4 mod null::uint2;
select 2147483647::int4 mod null::uint4;
select 2147483647::int4 mod null::uint8;

select 2147483647::int4 mod 4294967295::uint4;
select 2147483647::int4 mod 18446744073709551615::uint8;

select 2147483647::int4 mod 1::uint1;
select 2147483647::int4 mod 1::uint2;
select 2147483647::int4 mod 1::uint4;
select 2147483647::int4 mod 1::uint8;

select (-2147483648)::int4 mod 255::uint1;
select (-2147483648)::int4 mod 65535::uint2;
select (-2147483648)::int4 mod 4294967295::uint4;
select (-2147483648)::int4 mod 18446744073709551615::uint8;

--int2
select 32767::int2 mod 0::uint1;
select 32767::int2 mod 0::uint2;
select 32767::int2 mod 0::uint4;
select 32767::int2 mod 0::uint8;

select 32767::int2 mod null::uint1;
select 32767::int2 mod null::uint2;
select 32767::int2 mod null::uint4;
select 32767::int2 mod null::uint8;

select 32767::int2 mod 65535::uint2;
select 32767::int2 mod 4294967295::uint4;
select 32767::int2 mod 18446744073709551615::uint8;

select 32767::int2 mod 1::uint1;
select 32767::int2 mod 1::uint2;
select 32767::int2 mod 1::uint4;
select 32767::int2 mod 1::uint8;

select (-32768)::int2 mod 255::uint1;
select (-32768)::int2 mod 65535::uint2;
select (-32768)::int2 mod 4294967295::uint4;
select (-32768)::int2 mod 18446744073709551615::uint8;

--int1
select 127::int1 mod 0::uint1;
select 127::int1 mod 0::uint2;
select 127::int1 mod 0::uint4;
select 127::int1 mod 0::uint8;

select 127::int1 mod null::uint1;
select 127::int1 mod null::uint2;
select 127::int1 mod null::uint4;
select 127::int1 mod null::uint8;

select 127::int1 mod 65535::uint2;
select 127::int1 mod 4294967295::uint4;
select 127::int1 mod 18446744073709551615::uint8;

select 127::int1 mod 1::uint1;
select 127::int1 mod 1::uint2;
select 127::int1 mod 1::uint4;
select 127::int1 mod 1::uint8;

\c postgres
drop database uint_mod2
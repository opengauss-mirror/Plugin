create schema uint_operator;
set current_schema to 'uint_operator';

-- >
select 1::uint1 > 1::uint1;
select 1::uint1 > 2::uint1;
select 2::uint1 > 1::uint1;

select 1::uint2 > 1::uint2;
select 1::uint2 > 2::uint2;
select 2::uint2 > 1::uint2;

select 1::uint4 > 1::uint4;
select 1::uint4 > 2::uint4;
select 2::uint4 > 1::uint4;

select 1::uint8 > 1::uint8;
select 1::uint8 > 2::uint8;
select 2::uint8 > 1::uint8;

select 1::int1 > 1::uint1;
select 1::int1 > 2::uint1;
select 2::int1 > 1::uint1;

select 1::int2 > 1::uint2;
select 1::int2 > 2::uint2;
select 2::int2 > 1::uint2;

select 1::int4 > 1::uint4;
select 1::int4 > 2::uint4;
select 2::int4 > 1::uint4;

select 1::int8 > 1::uint8;
select 1::int8 > 2::uint8;
select 2::int8 > 1::uint8;

select 1::uint1 > 1::int1;
select 1::uint1 > 2::int1;
select 2::uint1 > 1::int1;

select 1::uint2 > 1::int2;
select 1::uint2 > 2::int2;
select 2::uint2 > 1::int2;

select 1::uint4 > 1::int4;
select 1::uint4 > 2::int4;
select 2::uint4 > 1::int4;

select 1::uint8 > 1::int8;
select 1::uint8 > 2::int8;
select 2::uint8 > 1::int8;

-- <
select 1::uint1 < 1::uint1;
select 1::uint1 < 2::uint1;
select 2::uint1 < 1::uint1;

select 1::uint2 < 1::uint2;
select 1::uint2 < 2::uint2;
select 2::uint2 < 1::uint2;

select 1::uint4 < 1::uint4;
select 1::uint4 < 2::uint4;
select 2::uint4 < 1::uint4;

select 1::uint8 < 1::uint8;
select 1::uint8 < 2::uint8;
select 2::uint8 < 1::uint8;

select 1::int1 < 1::uint1;
select 1::int1 < 2::uint1;
select 2::int1 < 1::uint1;

select 1::int2 < 1::uint2;
select 1::int2 < 2::uint2;
select 2::int2 < 1::uint2;

select 1::int4 < 1::uint4;
select 1::int4 < 2::uint4;
select 2::int4 < 1::uint4;

select 1::int8 < 1::uint8;
select 1::int8 < 2::uint8;
select 2::int8 < 1::uint8;

select 1::uint1 < 1::int1;
select 1::uint1 < 2::int1;
select 2::uint1 < 1::int1;

select 1::uint2 < 1::int2;
select 1::uint2 < 2::int2;
select 2::uint2 < 1::int2;

select 1::uint4 < 1::int4;
select 1::uint4 < 2::int4;
select 2::uint4 < 1::int4;

select 1::uint8 < 1::int8;
select 1::uint8 < 2::int8;
select 2::uint8 < 1::int8;

-- >=
select 1::uint1 >= 1::uint1;
select 1::uint1 >= 2::uint1;
select 2::uint1 >= 1::uint1;

select 1::uint2 >= 1::uint2;
select 1::uint2 >= 2::uint2;
select 2::uint2 >= 1::uint2;

select 1::uint4 >= 1::uint4;
select 1::uint4 >= 2::uint4;
select 2::uint4 >= 1::uint4;

select 1::uint8 >= 1::uint8;
select 1::uint8 >= 2::uint8;
select 2::uint8 >= 1::uint8;

select 1::int1 >= 1::uint1;
select 1::int1 >= 2::uint1;
select 2::int1 >= 1::uint1;

select 1::int2 >= 1::uint2;
select 1::int2 >= 2::uint2;
select 2::int2 >= 1::uint2;

select 1::int4 >= 1::uint4;
select 1::int4 >= 2::uint4;
select 2::int4 >= 1::uint4;

select 1::int8 >= 1::uint8;
select 1::int8 >= 2::uint8;
select 2::int8 >= 1::uint8;

select 1::uint1 >= 1::int1;
select 1::uint1 >= 2::int1;
select 2::uint1 >= 1::int1;

select 1::uint2 >= 1::int2;
select 1::uint2 >= 2::int2;
select 2::uint2 >= 1::int2;

select 1::uint4 >= 1::int4;
select 1::uint4 >= 2::int4;
select 2::uint4 >= 1::int4;

select 1::uint8 >= 1::int8;
select 1::uint8 >= 2::int8;
select 2::uint8 >= 1::int8;

-- <=
select 1::uint1 <= 1::uint1;
select 1::uint1 <= 2::uint1;
select 2::uint1 <= 1::uint1;

select 1::uint2 <= 1::uint2;
select 1::uint2 <= 2::uint2;
select 2::uint2 <= 1::uint2;

select 1::uint4 <= 1::uint4;
select 1::uint4 <= 2::uint4;
select 2::uint4 <= 1::uint4;

select 1::uint8 <= 1::uint8;
select 1::uint8 <= 2::uint8;
select 2::uint8 <= 1::uint8;

select 1::int1 <= 1::uint1;
select 1::int1 <= 2::uint1;
select 2::int1 <= 1::uint1;

select 1::int2 <= 1::uint2;
select 1::int2 <= 2::uint2;
select 2::int2 <= 1::uint2;

select 1::int4 <= 1::uint4;
select 1::int4 <= 2::uint4;
select 2::int4 <= 1::uint4;

select 1::int8 <= 1::uint8;
select 1::int8 <= 2::uint8;
select 2::int8 <= 1::uint8;

select 1::uint1 <= 1::int1;
select 1::uint1 <= 2::int1;
select 2::uint1 <= 1::int1;

select 1::uint2 <= 1::int2;
select 1::uint2 <= 2::int2;
select 2::uint2 <= 1::int2;

select 1::uint4 <= 1::int4;
select 1::uint4 <= 2::int4;
select 2::uint4 <= 1::int4;

select 1::uint8 <= 1::int8;
select 1::uint8 <= 2::int8;
select 2::uint8 <= 1::int8;

-- =
select 1::uint1 = 1::uint1;
select 1::uint1 = 2::uint1;
select 2::uint1 = 1::uint1;

select 1::uint2 = 1::uint2;
select 1::uint2 = 2::uint2;
select 2::uint2 = 1::uint2;

select 1::uint4 = 1::uint4;
select 1::uint4 = 2::uint4;
select 2::uint4 = 1::uint4;

select 1::uint8 = 1::uint8;
select 1::uint8 = 2::uint8;
select 2::uint8 = 1::uint8;

select 1::int1 = 1::uint1;
select 1::int1 = 2::uint1;
select 2::int1 = 1::uint1;

select 1::int2 = 1::uint2;
select 1::int2 = 2::uint2;
select 2::int2 = 1::uint2;

select 1::int4 = 1::uint4;
select 1::int4 = 2::uint4;
select 2::int4 = 1::uint4;

select 1::int8 = 1::uint8;
select 1::int8 = 2::uint8;
select 2::int8 = 1::uint8;

select 1::uint1 = 1::int1;
select 1::uint1 = 2::int1;
select 2::uint1 = 1::int1;

select 1::uint2 = 1::int2;
select 1::uint2 = 2::int2;
select 2::uint2 = 1::int2;

select 1::uint4 = 1::int4;
select 1::uint4 = 2::int4;
select 2::uint4 = 1::int4;

select 1::uint8 = 1::int8;
select 1::uint8 = 2::int8;
select 2::uint8 = 1::int8;

-- <>
select 1::uint1 <> 1::uint1;
select 1::uint1 <> 2::uint1;
select 2::uint1 <> 1::uint1;

select 1::uint2 <> 1::uint2;
select 1::uint2 <> 2::uint2;
select 2::uint2 <> 1::uint2;

select 1::uint4 <> 1::uint4;
select 1::uint4 <> 2::uint4;
select 2::uint4 <> 1::uint4;

select 1::uint8 <> 1::uint8;
select 1::uint8 <> 2::uint8;
select 2::uint8 <> 1::uint8;

select 1::int1 <> 1::uint1;
select 1::int1 <> 2::uint1;
select 2::int1 <> 1::uint1;

select 1::int2 <> 1::uint2;
select 1::int2 <> 2::uint2;
select 2::int2 <> 1::uint2;

select 1::int4 <> 1::uint4;
select 1::int4 <> 2::uint4;
select 2::int4 <> 1::uint4;

select 1::int8 <> 1::uint8;
select 1::int8 <> 2::uint8;
select 2::int8 <> 1::uint8;

select 1::uint1 <> 1::int1;
select 1::uint1 <> 2::int1;
select 2::uint1 <> 1::int1;

select 1::uint2 <> 1::int2;
select 1::uint2 <> 2::int2;
select 2::uint2 <> 1::int2;

select 1::uint4 <> 1::int4;
select 1::uint4 <> 2::int4;
select 2::uint4 <> 1::int4;

select 1::uint8 <> 1::int8;
select 1::uint8 <> 2::int8;
select 2::uint8 <> 1::int8;

-- >>
select 255::uint1 >> 1;
select 255::uint1 >> 5;
select 255::uint1 >> 63;
select 255::uint1 >> 64;

select 65535::uint2 >> 1;
select 65535::uint2 >> 5;
select 65535::uint2 >> 63;
select 65535::uint2 >> 64;

select 4294967295::uint4 >> 1;
select 4294967295::uint4 >> 5;
select 4294967295::uint4 >> 63;
select 4294967295::uint4 >> 64;

select 18446744073709551615::uint8 >> 1;
select 18446744073709551615::uint8 >> 5;
select 18446744073709551615::uint8 >> 63;
select 18446744073709551615::uint8 >> 64;

--<<
select 1::uint1 << 1;
select 1::uint1 << 5;
select 1::uint1 << 63;
select 1::uint1 << 64;

select 1::uint2 << 1;
select 1::uint2 << 5;
select 1::uint2 << 63;
select 1::uint2 << 64;

select 1::uint4 << 1;
select 1::uint4 << 5;
select 1::uint4 << 63;
select 1::uint4 << 64;

select 1::uint8 << 1;
select 1::uint8 << 5;
select 1::uint8 << 63;
select 1::uint8 << 64;

-- @
select @1::uint1;
select @1::uint2;
select @1::uint4;
select @1::uint8;

-- +
select +1::uint1;
select +1::uint2;
select +1::uint4;
select +1::uint8;

-- -
select -1::uint1;
select -1::uint2;
select -1::uint4;
select -1::uint8;

-- ~
select ~1::uint1;
select ~1::uint2;
select ~1::uint4;
select ~1::uint8;

select ~0::uint1;
select ~0::uint2;
select ~0::uint4;
select ~0::uint8;

drop schema uint_operator cascade;
reset current_schema;


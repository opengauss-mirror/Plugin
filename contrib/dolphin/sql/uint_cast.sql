create schema uint_cast;
set current_schema to 'uint_cast';

select (-1)::bool::uint8;
select (0)::bool::uint8;
select (1)::bool::uint8;
select (-1.0)::float4::uint8;
select (0.0)::float4::uint8;
select (1.0)::float4::uint8;
select (-1.0)::float8::uint8;
select (0.0)::float8::uint8;
select (1.0)::float8::uint8;
select (92233720368547758.0)::float8::uint8;
select (9223372036854775808.0)::float8::uint8;
select (-9223372036854775808.0)::float8::uint8;
select (-9223372036854775809.0)::float8::uint8;
select (18446744073709550515)::float8::uint8;
select (-18446744073709550515)::float8::uint8;
select (92233720368547758.0)::float4::uint8;
select (9223372036854775808.0)::float4::uint8;
select (-9223372036854775808.0)::float4::uint8;
select (-9223372036854775809.0)::float4::uint8;
select (18446744073709550515)::float4::uint8;
select (-18446744073709550515)::float4::uint8;

select (-922337203685477580999.0)::float4::uint1;
select (-922337203685477580999.0)::float8::uint1;
select (-922337203685477580999.0)::float4::uint2;
select (-922337203685477580999.0)::float8::uint2;
select (-922337203685477580999.0)::float4::uint4;
select (-922337203685477580999.0)::float8::uint4;
select (-922337203685477580999.0)::float4::uint8;
select (-922337203685477580999.0)::float8::uint8;

select (0)::numeric::uint8;
select (-1)::numeric::uint8;
select (18446744073709551615)::numeric(20)::uint8;
select (18446744073709551616)::numeric(20)::uint8;

select (-1)::int2::uint8;
select (0)::int2::uint8;
select (1)::int2::uint8;
select (32767)::int2::uint8;
select (32768)::int2::uint8;
select (-32768)::int2::uint8;
select (-32769)::int2::uint8;

select (-1)::int4::uint8;
select (0)::int4::uint8;
select (1)::int4::uint8;
select (2147483647)::int4::uint8;
select (2147483648)::int4::uint8;
select (-2147483648)::int4::uint8;
select (-2147483649)::int4::uint8;

select (-1)::int8::uint8;
select (0)::int8::uint8;
select (1)::int8::uint8;
select (9223372036854775807)::int8::uint8;
select (9223372036854775808)::int8::uint8;
select (-9223372036854775808)::int8::uint8;
select (-9223372036854775809)::int8::uint8;

select (-1)::bool::uint4;
select (0)::bool::uint4;
select (1)::bool::uint4;
select (-1.0)::float4::uint4;
select (0.0)::float4::uint4;
select (1.0)::float4::uint4;
select (-1.0)::float8::uint4;
select (0.0)::float8::uint4;
select (1.0)::float8::uint4;
select (0)::numeric::uint4;
select (-1)::numeric::uint4;
select (4294967295)::numeric::uint4;
select (4294967296)::numeric::uint4;

select (-1)::int2::uint4;
select (0)::int2::uint4;
select (1)::int2::uint4;
select (32767)::int2::uint4;
select (32768)::int2::uint4;
select (-32768)::int2::uint4;
select (-32769)::int2::uint4;

select (-1)::int4::uint4;
select (0)::int4::uint4;
select (1)::int4::uint4;
select (2147483647)::int4::uint4;
select (2147483648)::int4::uint4;
select (-2147483648)::int4::uint4;
select (-2147483649)::int4::uint4;

select (-1)::int8::uint4;
select (0)::int8::uint4;
select (1)::int8::uint4;
select (9223372036854775807)::int8::uint4;
select (9223372036854775808)::int8::uint4;
select (-9223372036854775808)::int8::uint4;
select (-9223372036854775809)::int8::uint4;

select (4294967295)::int8::uint4;
select (4294967296)::int8::uint4;
select (-2147483648)::int8::uint4;
select (-2147483649)::int8::uint4;

select (-1)::bool::uint2;
select (0)::bool::uint2;
select (1)::bool::uint2;
select (-1.0)::float4::uint2;
select (0.0)::float4::uint2;
select (1.0)::float4::uint2;
select (-1.0)::float8::uint2;
select (0.0)::float8::uint2;
select (1.0)::float8::uint2;
select (0)::numeric::uint2;
select (-1)::numeric::uint2;
select (32767)::numeric::uint2;
select (32768)::numeric::uint2;

select (-1)::int2::uint2;
select (0)::int2::uint2;
select (1)::int2::uint2;
select (32767)::int2::uint2;
select (32768)::int2::uint2;
select (-32768)::int2::uint2;
select (-32769)::int2::uint2;

select (-1)::int4::uint2;
select (0)::int4::uint2;
select (1)::int4::uint2;
select (2147483647)::int4::uint2;
select (2147483648)::int4::uint2;
select (-2147483648)::int4::uint2;
select (-2147483649)::int4::uint2;
select (65535)::int4::uint2;
select (65536)::int4::uint2;
select (-32768)::int4::uint2;
select (-32769)::int4::uint2;

select (-1)::int8::uint2;
select (0)::int8::uint2;
select (1)::int8::uint2;
select (9223372036854775807)::int8::uint2;
select (9223372036854775808)::int8::uint2;
select (-9223372036854775808)::int8::uint2;
select (-9223372036854775809)::int8::uint2;
select (65535)::int8::uint2;
select (65536)::int8::uint2;
select (-32768)::int8::uint2;
select (-32769)::int8::uint2;

select (-1)::bool::uint1;
select (0)::bool::uint1;
select (1)::bool::uint1;
select (-1.0)::float4::uint1;
select (0.0)::float4::uint1;
select (1.0)::float4::uint1;
select (-1.0)::float8::uint1;
select (0.0)::float8::uint1;
select (1.0)::float8::uint1;

select (0)::numeric::uint1;
select (-1)::numeric::uint1;
select (255)::numeric::uint1;
select (256)::numeric::uint1;

select (-1)::int2::uint1;
select (0)::int2::uint1;
select (1)::int2::uint1;
select (32767)::int2::uint1;
select (32768)::int2::uint1;
select (-32768)::int2::uint1;
select (-32769)::int2::uint1;

select (255)::int2::uint1;
select (256)::int2::uint1;
select (-128)::int2::uint1;
select (-129)::int2::uint1;


select (-1)::int4::uint1;
select (0)::int4::uint1;
select (1)::int4::uint1;
select (2147483647)::int4::uint1;
select (2147483648)::int4::uint1;
select (-2147483648)::int4::uint1;
select (-2147483649)::int4::uint1;

select (255)::int4::uint1;
select (256)::int4::uint1;
select (-128)::int4::uint1;
select (-129)::int4::uint1;

select (-1)::int8::uint1;
select (0)::int8::uint1;
select (1)::int8::uint1;
select (9223372036854775807)::int8::uint1;
select (9223372036854775808)::int8::uint1;
select (-9223372036854775808)::int8::uint1;
select (-9223372036854775809)::int8::uint1;

select (255)::int8::uint1;
select (256)::int8::uint1;
select (-128)::int8::uint1;
select (-129)::int8::uint1;


select (-1)::int1::uint1;
select (0)::int1::uint1;
select (1)::int1::uint1;
select (255)::int1::uint1;
select (256)::int1::uint1;

select (-1)::int1::uint2;
select (0)::int1::uint2;
select (1)::int1::uint2;
select (255)::int1::uint2;
select (256)::int1::uint2;

select (-1)::int1::uint4;
select (0)::int1::uint4;
select (1)::int1::uint4;
select (255)::int1::uint4;
select (256)::int1::uint4;

select (-1)::int1::uint8;
select (0)::int1::uint8;
select (1)::int1::uint8;
select (255)::int1::uint8;
select (256)::int1::uint8;

select 255::uint1::int1;
select 255::uint1::int2;
select 255::uint1::int4;
select 255::uint1::int8;
select 255::uint1::float4;
select 255::uint1::float8;
select 255::uint1::numeric;

select 255::uint2::int1;
select 65535::uint2::int1;
select 32767::uint2::int2;
select 65535::uint2::int2;
select 65535::uint2::int4;
select 65535::uint2::int8;
select 65535::uint2::float4;
select 65535::uint2::float8;
select 65535::uint2::numeric;

select 255::uint4::int1;
select 4294967295::uint4::int1;
select 32767::uint4::int2;
select 4294967295::uint4::int2;
select 2147483647::uint4::int4;
select 4294967295::uint4::int4;
select 4294967295::uint4::int8;
select 4294967295::uint4::float4;
select 4294967295::uint4::float8;
select 4294967295::uint4::numeric;

select 255::uint8::int1;
select 18446744073709551615::uint8::int1;
select 32767::uint8::int2;
select 18446744073709551615::uint8::int2;
select 2147483647::uint8::int4;
select 18446744073709551615::uint8::int4;
select 9223372036854775807::uint8::int8;
select 18446744073709551615::uint8::int8;
select 18446744073709551615::uint8::float4;
select 18446744073709551615::uint8::float8;
select 18446744073709551615::uint8::numeric(30);

select (0)::uint1::bool;
select (1)::uint1::bool;

select (0)::uint2::bool;
select (1)::uint2::bool;

select (0)::uint4::bool;
select (1)::uint4::bool;

select (0)::uint8::bool;
select (1)::uint8::bool;

select cast('-1' as uint1);
select cast('-1' as uint2);
select cast('-1' as uint4);
select cast('-1' as uint8);
select cast('-1' as unsigned);
select '-1'::uint8;
select cast('-128' as uint1);
select cast('-129' as uint1);
select cast('255' as uint1);
select cast('256' as uint1);

drop schema uint_cast cascade;
reset current_schema;
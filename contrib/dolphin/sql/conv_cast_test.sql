create schema conv_cast_test;
set current_schema to 'conv_cast_test';

select conv(-211111111111111111111111111111111111111111111111111111111177777,10,8);
select conv(-366666666666666666666666666666666666666, 10, 8);
select conv(-266666666666666666666666666666666666666, 10, 8);
select conv(-170141183460469231731687303715884105729,10,8);
select conv(-170141183460469231731687303715884105728,10,8);
select conv(-170141183460469231731687303715884105727,10,8);
select conv(-18446744073709551617,10,8);
select conv(-18446744073709551616,10,8);
select conv(-18446744073709551615,10,8);
select conv(-9223372036854775809,10,8);
select conv(-9223372036854775808,10,8);
select conv(-9223372036854775807,10,8);
select conv(-123456,10,8);
select conv(-1,10,8);
select conv(0,10,8);
select conv(211111111111111111111111111111111111111111111111111111111177777,10,8);
select conv(366666666666666666666666666666666666666, 10, 8);
select conv(266666666666666666666666666666666666666, 10, 8);
select conv(170141183460469231731687303715884105729,10,8);
select conv(170141183460469231731687303715884105728,10,8);
select conv(170141183460469231731687303715884105727,10,8);
select conv(18446744073709551617,10,8);
select conv(18446744073709551616,10,8);
select conv(18446744073709551615,10,8);
select conv(9223372036854775809,10,8);
select conv(9223372036854775808,10,8);
select conv(9223372036854775807,10,8);
select conv(123456,10,8);
select conv(1,10,8);

select 127::tinyint::bit(64)::tinyint;
select 32767::smallint::bit(64)::smallint;
select 2147483647::int::bit(64)::int;
select 9223372036854775807::bigint::bit(64)::bigint;

select conv('-211111111111111111111111111111111111111111111111111111111177777',10,8);
select conv('-366666666666666666666666666666666666666', 10, 8);
select conv('-266666666666666666666666666666666666666', 10, 8);
select conv('-170141183460469231731687303715884105729',10,8);
select conv('-170141183460469231731687303715884105728',10,8);
select conv('-170141183460469231731687303715884105727',10,8);
select conv('-18446744073709551617',10,8);
select conv('-18446744073709551616',10,8);
select conv('-18446744073709551615',10,8);
select conv('-9223372036854775809',10,8);
select conv('-9223372036854775808',10,8);
select conv('-9223372036854775807',10,8);
select conv('-123456',10,8);
select conv('-1',10,8);
select conv('0',10,8);
select conv('211111111111111111111111111111111111111111111111111111111177777',10,8);
select conv('366666666666666666666666666666666666666', 10, 8);
select conv('266666666666666666666666666666666666666', 10, 8);
select conv('170141183460469231731687303715884105729',10,8);
select conv('170141183460469231731687303715884105728',10,8);
select conv('170141183460469231731687303715884105727',10,8);
select conv('18446744073709551617',10,8);
select conv('18446744073709551616',10,8);
select conv('18446744073709551615',10,8);
select conv('9223372036854775809',10,8);
select conv('9223372036854775808',10,8);
select conv('9223372036854775807',10,8);
select conv('123456',10,8);
select conv('1',10,8);

select ''::bit;
select ''::bit(10);
select ''::bit(64);

set dolphin.b_compatibility_mode to on;

select ''::bit;
select ''::bit(10);
select ''::bit(64);

select 8385959::char(30)::time;
select -8385958.999999::char(30)::time(6);
select -8385959::varchar(30)::time;
select 8385958.999999::varchar(30)::time(6);

select '838:59:59'::char(30)::time;
select '-838:59:58.999999'::char(30)::time(6);
select '-838:59:59'::varchar(30)::time;
select '838:59:58.999999'::varchar(30)::time(6);

select cast(b'1111111111111111111111111111111111111111111111111111111111111111111111111111111111111111' as unsigned);
select cast(b'1111111111111111111111111111111111111111111111111111111111111111111111111111111111111111' as signed);
select cast(b'1111111111111111111111111111111111111111111111111111111111111111' as unsigned);
select cast(b'1111111111111111111111111111111111111111111111111111111111111111' as signed);
select cast(b'111111111111111111111111111111111111111111111111111111111111111' as unsigned);
select cast(b'111111111111111111111111111111111111111111111111111111111111111' as signed);
select cast(b'11111111111111111111111111111111111111111111111111111111111111111' as unsigned);
select cast(b'11111111111111111111111111111111111111111111111111111111111111111' as signed);

select 'true'::bool::bit;
select 'true'::bool::bit(10);
select 'true'::bool::bit(64);
select 'true'::bool::float4;
select 'true'::bool::float8;

select 'false'::bool::bit;
select 'false'::bool::bit(10);
select 'false'::bool::bit(64);
select 'false'::bool::float4;
select 'false'::bool::float8;

drop schema conv_cast_test cascade;
reset current_schema;

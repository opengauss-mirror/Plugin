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

select cast(b'1111111111111111111111111111111111111111111111111111111111111111111111111111111111111111' as unsigned);
select cast(b'1111111111111111111111111111111111111111111111111111111111111111111111111111111111111111' as signed);
select cast(b'1111111111111111111111111111111111111111111111111111111111111111' as unsigned);
select cast(b'1111111111111111111111111111111111111111111111111111111111111111' as signed);
select cast(b'111111111111111111111111111111111111111111111111111111111111111' as unsigned);
select cast(b'111111111111111111111111111111111111111111111111111111111111111' as signed);
select cast(b'11111111111111111111111111111111111111111111111111111111111111111' as unsigned);
select cast(b'11111111111111111111111111111111111111111111111111111111111111111' as signed);

drop schema conv_cast_test cascade;
reset current_schema;

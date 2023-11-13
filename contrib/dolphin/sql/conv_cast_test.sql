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

select time'-2 34:25:59'::float4;
select time'-838:59:59'::float4;
select time'0'::float4;
select time'-0'::float4;
select time'-2 34:25:59'::float8;
select time'-838:59:59'::float8;
select time'0'::float8;
select time'-0'::float8;

select 127::tinyint::bit(64)::tinyint;
select 32767::smallint::bit(64)::smallint;
select 2147483647::int::bit(64)::int;
select 9223372036854775807::bigint::bit(64)::bigint;

select '255'::uint1::time;
select '65535'::uint2::time;
select '4294967295'::uint4::time;
select '18446744073709551615'::uint8::time;

select '4294967295'::uint1::time;
select '4294967295'::uint2::time;
select '4294967295'::uint4::time;
select '4294967295'::uint8::time;

select '4294967295'::int1::time;
select '4294967295'::int2::time;
select '4294967295'::int4::time;
select '4294967295'::int8::time;

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

select conv('10', 8, 10);
select conv('180', 8, 10);
select conv('910', 8, 10);
select conv('B1', 8, 10);
select conv('B1', 16, 10);

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

SELECT '20220101121212'::date;
SELECT '20220101121212.5'::date;

select cast(b'1111111111111111111111111111111111111111111111111111111111111111111111111111111111111111' as unsigned);
select cast(b'1111111111111111111111111111111111111111111111111111111111111111111111111111111111111111' as signed);
select cast(b'1111111111111111111111111111111111111111111111111111111111111111' as unsigned);
select cast(b'1111111111111111111111111111111111111111111111111111111111111111' as signed);
select cast(b'111111111111111111111111111111111111111111111111111111111111111' as unsigned);
select cast(b'111111111111111111111111111111111111111111111111111111111111111' as signed);
select cast(b'11111111111111111111111111111111111111111111111111111111111111111' as unsigned);
select cast(b'11111111111111111111111111111111111111111111111111111111111111111' as signed);

select hex('9999-12-31 23:59:59'::datetime::bit(64));
select hex('99991231235959'::datetime::bit(64));

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

create table test_date(a date);
set dolphin.sql_mode = sql_mode_strict,sql_mode_full_group,pipes_as_concat,ansi_quotes,no_zero_date,pad_char_to_full_length,auto_recompile_function;
select 0::date;
insert into test_date values(0);
select 1::date;
insert into test_date values(1);
set dolphin.sql_mode = sql_mode_strict,sql_mode_full_group,pipes_as_concat,ansi_quotes,pad_char_to_full_length,auto_recompile_function;
select 0::date;
insert into test_date values(0);
select 1::date;
insert into test_date values(1);
set dolphin.sql_mode = sql_mode_full_group,pipes_as_concat,ansi_quotes,no_zero_date,pad_char_to_full_length,auto_recompile_function;
select 0::date;
insert into test_date values(0);
select 1::date;
insert into test_date values(1);
set dolphin.sql_mode = sql_mode_full_group,pipes_as_concat,ansi_quotes,pad_char_to_full_length,auto_recompile_function;
select 0::date;
insert into test_date values(0);
select 1::date;
insert into test_date values(1);
reset dolphin.sql_mode;
select * from test_date;

select b'11100000111000';
select conv(b'11100000111000', 10, 8);
select conv(b'11100000111000', 20, 8);
select conv(b'11100000111000'::int8, 20, 8);
select x'4c';
select conv(x'4c', 10, 8);
select conv(x'4c', 30, 8);
select conv(x'4c'::int8, 30, 8);

set dolphin.sql_mode = treat_bxconst_as_binary;

select b'11100000111000';
select conv(b'11100000111000', 10, 8);
select conv(b'11100000111000', 20, 8);
select conv(b'11100000111000'::int8, 20, 8);
select x'4c';
select conv(x'4c', 10, 8);
select conv(x'4c', 30, 8);
select conv(x'4c'::int8, 30, 8);

reset dolphin.sql_mode;

drop schema conv_cast_test cascade;
reset current_schema;

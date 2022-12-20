---- b compatibility case
drop database if exists none_strict_warning_test;
create database none_strict_warning_test dbcompatibility 'b';
\c none_strict_warning_test

reset dolphin.sql_mode;
create table test_int1(c1 int1);
create table test_int8(c1 int8);
---- numeric__int1
---- over float8_max 严格模式error，非严格模式warning, 结果127
select cast(2e308 as int1);
---- int8_max~float8_max 严格模式error，非严格模式warning，结果127
select cast(1e308 as int1);

---- float4in
---- over float8_max 严格模式error，非严格模式warning，结果float4_max
select cast(3.5e308 as float4);
---- float4_max~float8_max 严格模式error，非严格模式warning，结果float4_max
select cast(3.5e38 as float4);

---- float8in
---- over float8_max 严格模式error，非严格模式warning，结果float8_max
select cast(3.5e308 as float8);

---- dtoi4
create table test_int4(c1 int4);
---- over int4_max 严格模式error，非严格模式warning，结果int4_max
insert into test_int4 values(3.5e38::float8);
---- over float8_max 严格模式error，非严格模式双warning，结果int4_max
insert into test_int4 values(3.5e308::float8);

---- dtoi2
create table test_int2(c1 int2);
---- over int2_max 严格模式error，非严格模式warning，结果int2_max
insert into test_int2 values(3.5e38::float8);
---- over float8_max 严格模式error，非严格模式双warning，结果int2_max
insert into test_int2 values(3.5e308::float8);

---- ftoi4
---- over int2_max 严格模式error，非严格模式warning，结果int4_max
insert into test_int4 values(3.4e38::float4);
---- over float4_max 严格模式error，非严格模式双warning，结果int4_max
insert into test_int4 values(3.5e38::float4);
---- over float8_max 严格模式error，非严格模式双warning，结果int4_max
insert into test_int4 values(3.5e308::float4);

---- ftoi2
---- over int2_max 严格模式error，非严格模式warning，结果int2_max
insert into test_int2 values(3.4e38::float4);
---- over float4_max 严格模式error，非严格模式双warning，结果int2_max
insert into test_int2 values(3.5e38::float4);
---- over float8_max 严格模式error，非严格模式双warning，结果int2_max
insert into test_int2 values(3.5e308::float4);

---- PgStrtoint16Internal
---- over int2_max 严格模式error，非严格模式warning，结果int2_max
insert into test_int2 values('32768');

---- PgStrtoint32Internal
---- over int4_max 严格模式error，非严格模式warning，结果int4_max
insert into test_int4 values('2147483648');

---- PgAtoiInternal 暂未找到如何调用

---- Scanint8Internal
---- over int8_max 严格模式error，非严格模式warning，结果int8_max
insert into test_int8 values('9223372036854775808');

---- i4toi2
---- over int2_max 严格模式error，非严格模式warning，结果int2_max
insert into test_int2 values(2147483647::int4);
insert ignore into test_int2 values(2147483647::int4);

---- i2toi1
---- over int1_max 严格模式error，非严格模式warning，结果int1_max
insert into test_int1 values(32767::int2);
insert ignore into test_int1 values(32767::int2);

---- i4toi1
---- over int1_max 严格模式error，非严格模式warning，结果int1_max
insert into test_int1 values(2147483647::int4);
insert ignore into test_int1 values(2147483647::int4);

---- i8toi1
---- over int1_max 严格模式error，非严格模式warning，结果int1_max
insert into test_int1 values(9223372036854775807::int8);
insert ignore into test_int1 values(9223372036854775807::int8);

---- f4toi1
---- over int1_max 严格模式error，非严格模式warning，结果int1_max
insert into test_int1 values(3e38::float4);
insert ignore into test_int1 values(3e38::float4);

---- f8toi1
---- over int1_max 严格模式error，非严格模式warning，结果int1_max
insert into test_int1 values(1e308::float8);
insert ignore into test_int1 values(1e308::float8);

---- int84
---- over int4_max 严格模式error，非严格模式warning，结果int4_max
insert into test_int4 values(2147483648::int8);
insert ignore into test_int4 values(9223372036854775808::int8);
---- over int8_max 严格模式error，非严格模式warning，结果int4_max
insert into test_int8 values(2147483648::int4);
insert ignore into test_int8 values(2147483648::int4);

---- int82
---- over int2_max 严格模式error，非严格模式warning，结果int2_max
insert into test_int2 values(32768::int8);
insert ignore into test_int2 values(9223372036854775808::int8);
---- over int8_max 严格模式error，非严格模式warning，结果int2_max
insert into test_int8 values(2147483648::int2);
insert ignore into test_int8 values(32768::int2);

---- dtoi8
---- over int8_max 严格模式error，非严格模式warning，结果int8_max
insert into test_int8 values(9223372036854775807.5::double);
insert ignore into test_int8 values(9223372036854775807.5::double);

---- ftoi8
---- over int8_max 严格模式error，非严格模式warning，结果int8_max
insert into test_int8 values(9223372036854775807.5::float);
insert ignore into test_int8 values(9223372036854775807.5::float);

---- numeric_int1
---- over int1_max 严格模式error，非严格模式warning，结果int1_max
insert into test_int1 values(1e30);
insert ignore into test_int1 values(1e30);

---- numeric_int2
---- over int2_max 严格模式error，非严格模式warning，结果int2_max
insert into test_int2 values(1e30);
insert ignore into test_int2 values(1e30);

---- numericvar_to_int32
---- over int4_max 严格模式error，非严格模式warning，结果int4_max
insert into test_int4 values(1e30);
insert ignore into test_int4 values(1e30);

---- numeric_int8
---- over int8_max 严格模式error，非严格模式warning，结果int8_max
insert into test_int8 values(1e30);
insert ignore into test_int8 values(1e30);

---- numeric_uint1
---- over uint1_max 严格模式error，非严格模式warning，结果uint1_max
create table test_uint1(c1 uint1);
insert into test_uint1 values(1e30);
insert ignore into test_uint1 values(1e30);

---- numeric_uint2
---- over uint2_max 严格模式error，非严格模式warning，结果uint2_max
create table test_uint2(c1 uint2);
insert into test_uint2 values(1e30);
insert ignore into test_uint2 values(1e30);

---- numeric_uint4
---- over uint4_max 严格模式error，非严格模式warning，结果uint4_max
create table test_uint4(c1 uint4);
insert into test_uint4 values(1e30);
insert ignore into test_uint4 values(1e30);

---- numeric_uint8
---- over uint8_max 严格模式error，非严格模式warning，结果uint8_max
create table test_uint8(c1 uint8);
insert into test_uint8 values(1e30);
insert ignore into test_uint8 values(1e30);

---- numeric_cast_uint1
select cast(1e30 as uint1);

---- numeric_cast_uint2
select cast(1e30 as uint2);

---- numeric_cast_uint4
select cast(1e30 as uint4);

---- numeric_cast_uint8
select cast(1e30 as uint8);

---- i2toui1
---- over uint1_max 严格模式error，非严格模式warning，结果uint1_max
insert into test_uint1 values(256::int2);
insert into test_uint1 values(-1::int2);
insert ignore into test_uint1 values(256::int2);
insert ignore into test_uint1 values(-1::int2);

---- i4toui1
---- over uint1_max 严格模式error，非严格模式warning，结果uint1_max
insert into test_uint1 values(256::int4);
insert into test_uint1 values(-1::int4);
insert ignore into test_uint1 values(256::int4);
insert ignore into test_uint1 values(-1::int4);

---- i8toui1
---- over uint1_max 严格模式error，非严格模式warning，结果uint1_max
insert into test_uint1 values(256::int8);
insert into test_uint1 values(-1::int8);
insert ignore into test_uint1 values(256::int8);
insert ignore into test_uint1 values(-1::int8);

---- f4toui1
---- over uint1_max 严格模式error，非严格模式warning，结果uint1_max
insert into test_uint1 values(256.5::float4);
insert into test_uint1 values(-1.5::float4);
insert ignore into test_uint1 values(256.5::float4);
insert ignore into test_uint1 values(-1.5::float4);

---- f8toui1
---- over uint1_max 严格模式error，非严格模式warning，结果uint1_max
insert into test_uint1 values(256.5::float8);
insert into test_uint1 values(-1.5::float8);
insert ignore into test_uint1 values(256.5::float8);
insert ignore into test_uint1 values(-1.5::float8);

---- ui2toui1
---- over uint1_max 严格模式error，非严格模式warning，结果uint1_max
insert into test_uint1 values(256::uint2);
insert into test_uint1 values(-1::uint2);
insert ignore into test_uint1 values(256::uint2);
insert ignore into test_uint1 values(-1::uint2);

---- ui2toi2
---- over int2_max 严格模式error，非严格模式warning，结果int2_max
insert into test_int2 values(32768::uint2);
insert into test_int2 values(-32769::uint2);
insert ignore into test_int2 values(32768::uint2);
insert ignore into test_int2 values(-32769::uint2);

---- i2toui2
---- over uint2_max 严格模式error，非严格模式warning，结果int2_max
insert into test_uint2 values(65536::int2);
insert into test_uint2 values(-1::int2);
insert ignore into test_uint2 values(65536::int2);
insert ignore into test_uint2 values(-1::int2);

---- i4toui2
---- over uint2_max 严格模式error，非严格模式warning，结果uint2_max
insert into test_uint2 values(65536::int4);
insert into test_uint2 values(-1::int4);
insert ignore into test_uint2 values(65536::int4);
insert ignore into test_uint2 values(-1::int4);

---- i8toui2
---- over uint2_max 严格模式error，非严格模式warning，结果uint2_max
insert into test_uint2 values(65536::int8);
insert into test_uint2 values(-1::int8);
insert ignore into test_uint2 values(65536::int8);
insert ignore into test_uint2 values(-1::int8);

---- f4toui2
---- over uint2_max 严格模式error，非严格模式warning，结果uint2_max
insert into test_uint2 values(65536.5::float4);
insert into test_uint2 values(-1.5::float4);
insert ignore into test_uint2 values(65536.5::float4);
insert ignore into test_uint2 values(-1.5::float4);

---- f8toui2
---- over uint2_max 严格模式error，非严格模式warning，结果uint2_max
insert into test_uint2 values(65536.5::float8);
insert into test_uint2 values(-1.5::float8);
insert ignore into test_uint2 values(65536.5::float8);
insert ignore into test_uint2 values(-1.5::float8);

---- ui4toui1
---- over uint1_max 严格模式error，非严格模式warning，结果uint1_max
insert into test_uint1 values(256::uint4);
insert into test_uint1 values(-1::uint4);
insert ignore into test_uint1 values(256::uint4);
insert ignore into test_uint1 values(-1::uint4);

---- ui4toi1
---- over int1_max 严格模式error，非严格模式warning，结果int1_max
insert into test_int1 values(128::uint4);
insert into test_int1 values(-129::uint4);
insert ignore into test_int1 values(128::uint4);
insert ignore into test_int1 values(-129::uint4);

---- i1toui4
---- over uint4_max 严格模式error，非严格模式warning，结果int1_max
insert into test_uint4 values(2147483648::int1);
insert into test_uint4 values(-129::int1);
insert ignore into test_uint4 values(2147483648::int1);
insert ignore into test_uint4 values(-129::int1);

---- ui2toi1
---- over int1_max 严格模式error，非严格模式warning，结果int1_max
insert into test_int1 values(128::uint2);
insert into test_int1 values(-129::uint2);
insert ignore into test_int1 values(128::uint2);
insert ignore into test_int1 values(-129::uint2);

---- i1toui2
---- over uint2_max 严格模式error，非严格模式warning，结果int1_max
insert into test_uint2 values(65536::int1);
insert into test_uint2 values(-1::int1);
insert ignore into test_uint2 values(65536::int1);
insert ignore into test_uint2 values(-1::int1);

---- ui1toi1
---- over int1_max 严格模式error，非严格模式warning，结果int1_max
insert into test_int1 values(128::uint1);
insert into test_int1 values(-129::uint1);
insert ignore into test_int1 values(128::uint1);
insert ignore into test_int1 values(-129::uint1);

---- i1toui1
---- over uint1_max 严格模式error，非严格模式warning，结果int1_max
insert into test_uint1 values(256::int1);
insert into test_uint1 values(-1::int1);
insert ignore into test_uint1 values(256::int1);
insert ignore into test_uint1 values(-1::int1);

---- ui4toui2
---- over uint2_max 严格模式error，非严格模式warning，结果uint2_max
insert into test_uint2 values(65536::uint4);
insert into test_uint2 values(-1::uint4);
insert ignore into test_uint2 values(65536::uint4);
insert ignore into test_uint2 values(-1::uint4);

---- ui4toi2
---- over int2_max 严格模式error，非严格模式warning，结果int2_max
insert into test_int2 values(32768::uint4);
insert into test_int2 values(-32769::uint4);
insert ignore into test_int2 values(32768::uint4);
insert ignore into test_int2 values(-32769::uint4);

---- i2toui4
---- over uint4_max 严格模式error，非严格模式warning，结果int2_max
insert into test_uint4 values(4294967296::int2);
insert into test_uint4 values(-129::int2);
insert ignore into test_uint4 values(4294967296::int2);
insert ignore into test_uint4 values(-129::int2);

---- ui4toi4
---- over int4_max 严格模式error，非严格模式warning，结果int4_max
insert into test_int4 values(2147483648::uint4);
insert into test_int4 values(-2147483649::uint4);
insert ignore into test_int4 values(2147483648::uint4);
insert ignore into test_int4 values(-2147483649::uint4);

---- i4toui4
---- over uint4_max 严格模式error，非严格模式warning，结果int4_max
insert into test_uint4 values(4294967296::int4);
insert into test_uint4 values(-129::int4);
insert ignore into test_uint4 values(4294967296::int4);
insert ignore into test_uint4 values(-129::int4);

---- ui4toi4
---- over int4_max 严格模式error，非严格模式warning，结果int4_max
insert into test_int4 values(4294967296::uint4);
insert into test_int4 values(-2147483649::uint4);
insert ignore into test_int4 values(4294967296::uint4);
insert ignore into test_int4 values(-2147483649::uint4);

---- f4toui4
---- over uint4_max 严格模式error，非严格模式warning，结果uint4_max
insert into test_uint4 values(4294967296.5::float4);
insert into test_uint4 values(-129.5::float4);
insert ignore into test_uint4 values(4294967296.5::float4);
insert ignore into test_uint4 values(-129.5::float4);

---- f4toui4
---- over uint4_max 严格模式error，非严格模式warning，结果uint4_max
insert into test_uint4 values(4294967296.5::float8);
insert into test_uint4 values(-129.5::float8);
insert ignore into test_uint4 values(4294967296.5::float8);
insert ignore into test_uint4 values(-129.5::float8);

---- ui8toui1
---- over uint1_max 严格模式error，非严格模式warning，结果uint1_max
insert into test_uint1 values(256::uint8);
insert into test_uint1 values(-1::uint8);
insert ignore into test_uint1 values(256::uint8);
insert ignore into test_uint1 values(-1::uint8);

---- ui8toi1
---- over int1_max 严格模式error，非严格模式warning，结果int1_max
insert into test_int1 values(128::uint8);
insert into test_int1 values(-129::uint8);
insert ignore into test_int1 values(128::uint8);
insert ignore into test_int1 values(-129::uint8);

---- i1toui8
---- over int1_max 严格模式error，非严格模式warning，结果int1_max
insert into test_uint8 values(256::int1);
insert into test_uint8 values(-129::int1);
insert ignore into test_uint8 values(256::int1);
insert ignore into test_uint8 values(-129::int1);

---- ui8toui2
---- over uint2_max 严格模式error，非严格模式warning，结果uint2_max
insert into test_uint2 values(65536::uint8);
insert into test_uint2 values(-1::uint8);
insert ignore into test_uint2 values(65536::uint8);
insert ignore into test_uint2 values(-1::uint8);

---- ui8toi2
---- over int2_max 严格模式error，非严格模式warning，结果int2_max
insert into test_int2 values(32768::uint8);
insert into test_int2 values(-32769::uint8);
insert ignore into test_int2 values(32768::uint8);
insert ignore into test_int2 values(-32769::uint8);

---- i2toui8
---- over int2_max 严格模式error，非严格模式warning，结果int2_max
insert into test_uint8 values(32768::int2);
insert into test_uint8 values(-32769::int2);
insert ignore into test_uint8 values(32768::int2);
insert ignore into test_uint8 values(-32769::int2);

---- ui8toi4
---- over int4_max 严格模式error，非严格模式warning，结果int4_max
insert into test_int4 values(4294967296::uint8);
insert into test_int4 values(-2147483649::uint8);
insert ignore into test_int4 values(4294967296::uint8);
insert ignore into test_int4 values(-2147483649::uint8);

---- i4toui8
---- over int4_max 严格模式error，非严格模式warning，结果int4_max
insert into test_uint8 values(4294967296::int4);
insert into test_uint8 values(-2147483649::int4);
insert ignore into test_uint8 values(4294967296::int4);
insert ignore into test_uint8 values(-2147483649::int4);

---- ui8toui4
---- over uint4_max 严格模式error，非严格模式warning，结果uint4_max
insert into test_uint4 values(4294967296::uint8);
insert into test_uint4 values(-129::uint8);
insert ignore into test_uint4 values(4294967296::uint8);
insert ignore into test_uint4 values(-129::uint8);

---- ui8toi8
---- over int8_max 严格模式error，非严格模式warning，结果int8_max
insert into test_int8 values(9223372036854775808::uint8);
insert into test_int8 values(-9223372036854775809::uint8);
insert ignore into test_int8 values(9223372036854775808::uint8);
insert ignore into test_int8 values(-9223372036854775808::uint8);

---- i8toui8
---- over int8_max 严格模式error，非严格模式warning，结果int8_max
insert into test_uint8 values(9223372036854775808::int8);
insert into test_uint8 values(-2147483649::int8);
insert ignore into test_uint8 values(9223372036854775808::int8);
insert ignore into test_uint8 values(-2147483649::int8);

---- f4toui8
---- over uint8_max 严格模式error，非严格模式warning，结果uint8_max
insert into test_uint8 values(18446744073709551616::float4);
insert into test_uint8 values(-2147483649::float4);
insert ignore into test_uint8 values(18446744073709551616::float4);
insert ignore into test_uint8 values(-2147483649::float4);

---- f8toui8
---- over uint8_max 严格模式error，非严格模式warning，结果uint8_max
insert into test_uint8 values(18446744073709551616::float8);
insert into test_uint8 values(-2147483649::float8);
insert ignore into test_uint8 values(18446744073709551616::float8);
insert ignore into test_uint8 values(-2147483649::float8);

---- i1_cast_ui1
---- 127
select cast(128::int1 as uint1);
---- 246
select cast(-10::int1 as uint1);

---- i4_cast_ui1
---- 255
select cast(2147483648::int4 as uint1);
---- 246
select cast(-10::int4 as uint1);

---- i8_cast_ui1
---- 255
select cast(9223372036854775808::int8 as uint1);
---- 246
select cast(-10::int8 as uint1);

---- i4_cast_ui2
---- 65535
select cast(2147483648::int4 as uint2);
---- 65526
select cast(-10::int4 as uint2);

---- i8_cast_ui2
---- 65535
select cast(9223372036854775808::int8 as uint2);
---- 65526
select cast(-10::int8 as uint2);

---- i8_cast_ui4
---- 4294967295
select cast(9223372036854775808::int8 as uint4);
---- 4294967286
select cast(-10::int8 as uint4);

---- f4_cast_ui1
---- 255
select cast(9223372036854775808.5::float4 as uint1);
---- 245
select cast(-10.5::float4 as uint1);

---- f8_cast_ui1
---- 255
select cast(9223372036854775808.5::float8 as uint1);
---- 245
select cast(-10.5::float8 as uint1);

---- f4_cast_ui2
---- 65535
select cast(65535.5::float4 as uint2);
---- 65525
select cast(-10.5::float4 as uint2);

---- f8_cast_ui2
---- 65535
select cast(65535.5::float8 as uint2);
---- 65525
select cast(-10.5::float8 as uint2);

---- f4_cast_ui4
---- 65535
select cast(4294967295.5::float4 as uint4);
---- 65525
select cast(-10.5::float4 as uint4);

---- f8_cast_ui4
---- 65535
select cast(4294967295.5::float8 as uint4);
---- 65525
select cast(-10.5::float8 as uint4);

---- unexpected
---- f4_cast_ui8
---- 65535
select cast(18446744073709551615.5::float4 as uint8);
---- 65525
select cast(-10.5::float4 as uint8);

---- f8_cast_ui8
---- 65535
select cast(18446744073709551615.5::float8 as uint8);
---- 65525
select cast(-10.5::float8 as uint8);

select * from test_int1;
select * from test_int2;
select * from test_int4;
select * from test_int8;
select * from test_uint1;
select * from test_uint2;
select * from test_uint4;
select * from test_uint8;
drop table if exists test_int1,test_int2,test_int4,test_int8,test_uint1,test_uint2,test_uint4,test_uint8;
set dolphin.sql_mode = sql_mode_full_group,pipes_as_concat,ansi_quotes;

create table test_int1(c1 int1);
create table test_int8(c1 int8);
---- numeric__int1
---- over float8_max 严格模式error，非严格模式warning, 结果127
select cast(2e308 as int1);
---- int8_max~float8_max 严格模式error，非严格模式warning，结果127
select cast(1e308 as int1);

---- float4in
---- over float8_max 严格模式error，非严格模式warning，结果float4_max
select cast(3.5e308 as float4);
---- float4_max~float8_max 严格模式error，非严格模式warning，结果float4_max
select cast(3.5e38 as float4);

---- float8in
---- over float8_max 严格模式error，非严格模式warning，结果float8_max
select cast(3.5e308 as float8);

---- dtoi4
create table test_int4(c1 int4);
---- over int4_max 严格模式error，非严格模式warning，结果int4_max
insert into test_int4 values(3.5e38::float8);
---- over float8_max 严格模式error，非严格模式双warning，结果int4_max
insert into test_int4 values(3.5e308::float8);

---- dtoi2
create table test_int2(c1 int2);
---- over int2_max 严格模式error，非严格模式warning，结果int2_max
insert into test_int2 values(3.5e38::float8);
---- over float8_max 严格模式error，非严格模式双warning，结果int2_max
insert into test_int2 values(3.5e308::float8);

---- ftoi4
---- over int2_max 严格模式error，非严格模式warning，结果int4_max
insert into test_int4 values(3.4e38::float4);
---- over float4_max 严格模式error，非严格模式双warning，结果int4_max
insert into test_int4 values(3.5e38::float4);
---- over float8_max 严格模式error，非严格模式双warning，结果int4_max
insert into test_int4 values(3.5e308::float4);

---- ftoi2
---- over int2_max 严格模式error，非严格模式warning，结果int2_max
insert into test_int2 values(3.4e38::float4);
---- over float4_max 严格模式error，非严格模式双warning，结果int2_max
insert into test_int2 values(3.5e38::float4);
---- over float8_max 严格模式error，非严格模式双warning，结果int2_max
insert into test_int2 values(3.5e308::float4);

---- PgStrtoint16Internal
---- over int2_max 严格模式error，非严格模式warning，结果int2_max
insert into test_int2 values('32768');

---- PgStrtoint32Internal
---- over int4_max 严格模式error，非严格模式warning，结果int4_max
insert into test_int4 values('2147483648');

---- PgAtoiInternal 暂未找到如何调用

---- Scanint8Internal
---- over int8_max 严格模式error，非严格模式warning，结果int8_max
insert into test_int8 values('9223372036854775808');

---- i4toi2
---- over int2_max 严格模式error，非严格模式warning，结果int2_max
insert into test_int2 values(2147483647::int4);
insert ignore into test_int2 values(2147483647::int4);

---- i2toi1
---- over int1_max 严格模式error，非严格模式warning，结果int1_max
insert into test_int1 values(32767::int2);
insert ignore into test_int1 values(32767::int2);

---- i4toi1
---- over int1_max 严格模式error，非严格模式warning，结果int1_max
insert into test_int1 values(2147483647::int4);
insert ignore into test_int1 values(2147483647::int4);

---- i8toi1
---- over int1_max 严格模式error，非严格模式warning，结果int1_max
insert into test_int1 values(9223372036854775807::int8);
insert ignore into test_int1 values(9223372036854775807::int8);

---- f4toi1
---- over int1_max 严格模式error，非严格模式warning，结果int1_max
insert into test_int1 values(3e38::float4);
insert ignore into test_int1 values(3e38::float4);

---- f8toi1
---- over int1_max 严格模式error，非严格模式warning，结果int1_max
insert into test_int1 values(1e308::float8);
insert ignore into test_int1 values(1e308::float8);

---- int84
---- over int4_max 严格模式error，非严格模式warning，结果int4_max
insert into test_int4 values(2147483648::int8);
insert ignore into test_int4 values(9223372036854775808::int8);
---- over int8_max 严格模式error，非严格模式warning，结果int4_max
insert into test_int8 values(2147483648::int4);
insert ignore into test_int8 values(2147483648::int4);

---- int82
---- over int2_max 严格模式error，非严格模式warning，结果int2_max
insert into test_int2 values(32768::int8);
insert ignore into test_int2 values(9223372036854775808::int8);
---- over int8_max 严格模式error，非严格模式warning，结果int2_max
insert into test_int8 values(2147483648::int2);
insert ignore into test_int8 values(32768::int2);

---- dtoi8
---- over int8_max 严格模式error，非严格模式warning，结果int8_max
insert into test_int8 values(9223372036854775807.5::double);
insert ignore into test_int8 values(9223372036854775807.5::double);

---- ftoi8
---- over int8_max 严格模式error，非严格模式warning，结果int8_max
insert into test_int8 values(9223372036854775807.5::float);
insert ignore into test_int8 values(9223372036854775807.5::float);

---- numeric_int1
---- over int1_max 严格模式error，非严格模式warning，结果int1_max
insert into test_int1 values(1e30);
insert ignore into test_int1 values(1e30);

---- numeric_int2
---- over int2_max 严格模式error，非严格模式warning，结果int2_max
insert into test_int2 values(1e30);
insert ignore into test_int2 values(1e30);

---- numericvar_to_int32
---- over int4_max 严格模式error，非严格模式warning，结果int4_max
insert into test_int4 values(1e30);
insert ignore into test_int4 values(1e30);

---- numeric_int8
---- over int8_max 严格模式error，非严格模式warning，结果int8_max
insert into test_int8 values(1e30);
insert ignore into test_int8 values(1e30);

---- numeric_uint1
---- over uint1_max 严格模式error，非严格模式warning，结果uint1_max
create table test_uint1(c1 uint1);
insert into test_uint1 values(1e30);
insert ignore into test_uint1 values(1e30);

---- numeric_uint2
---- over uint2_max 严格模式error，非严格模式warning，结果uint2_max
create table test_uint2(c1 uint2);
insert into test_uint2 values(1e30);
insert ignore into test_uint2 values(1e30);

---- numeric_uint4
---- over uint4_max 严格模式error，非严格模式warning，结果uint4_max
create table test_uint4(c1 uint4);
insert into test_uint4 values(1e30);
insert ignore into test_uint4 values(1e30);

---- numeric_uint8
---- over uint8_max 严格模式error，非严格模式warning，结果uint8_max
create table test_uint8(c1 uint8);
insert into test_uint8 values(1e30);
insert ignore into test_uint8 values(1e30);

---- numeric_cast_uint1
select cast(1e30 as uint1);

---- numeric_cast_uint2
select cast(1e30 as uint2);

---- numeric_cast_uint4
select cast(1e30 as uint4);

---- numeric_cast_uint8
select cast(1e30 as uint8);

---- i2toui1
---- over uint1_max 严格模式error，非严格模式warning，结果uint1_max
insert into test_uint1 values(256::int2);
insert into test_uint1 values(-1::int2);
insert ignore into test_uint1 values(256::int2);
insert ignore into test_uint1 values(-1::int2);

---- i4toui1
---- over uint1_max 严格模式error，非严格模式warning，结果uint1_max
insert into test_uint1 values(256::int4);
insert into test_uint1 values(-1::int4);
insert ignore into test_uint1 values(256::int4);
insert ignore into test_uint1 values(-1::int4);

---- i8toui1
---- over uint1_max 严格模式error，非严格模式warning，结果uint1_max
insert into test_uint1 values(256::int8);
insert into test_uint1 values(-1::int8);
insert ignore into test_uint1 values(256::int8);
insert ignore into test_uint1 values(-1::int8);

---- f4toui1
---- over uint1_max 严格模式error，非严格模式warning，结果uint1_max
insert into test_uint1 values(256.5::float4);
insert into test_uint1 values(-1.5::float4);
insert ignore into test_uint1 values(256.5::float4);
insert ignore into test_uint1 values(-1.5::float4);

---- f8toui1
---- over uint1_max 严格模式error，非严格模式warning，结果uint1_max
insert into test_uint1 values(256.5::float8);
insert into test_uint1 values(-1.5::float8);
insert ignore into test_uint1 values(256.5::float8);
insert ignore into test_uint1 values(-1.5::float8);

---- ui2toui1
---- over uint1_max 严格模式error，非严格模式warning，结果uint1_max
insert into test_uint1 values(256::uint2);
insert into test_uint1 values(-1::uint2);
insert ignore into test_uint1 values(256::uint2);
insert ignore into test_uint1 values(-1::uint2);

---- ui2toi2
---- over int2_max 严格模式error，非严格模式warning，结果int2_max
insert into test_int2 values(32768::uint2);
insert into test_int2 values(-32769::uint2);
insert ignore into test_int2 values(32768::uint2);
insert ignore into test_int2 values(-32769::uint2);

---- i2toui2
---- over uint2_max 严格模式error，非严格模式warning，结果int2_max
insert into test_uint2 values(65536::int2);
insert into test_uint2 values(-1::int2);
insert ignore into test_uint2 values(65536::int2);
insert ignore into test_uint2 values(-1::int2);

---- i4toui2
---- over uint2_max 严格模式error，非严格模式warning，结果uint2_max
insert into test_uint2 values(65536::int4);
insert into test_uint2 values(-1::int4);
insert ignore into test_uint2 values(65536::int4);
insert ignore into test_uint2 values(-1::int4);

---- i8toui2
---- over uint2_max 严格模式error，非严格模式warning，结果uint2_max
insert into test_uint2 values(65536::int8);
insert into test_uint2 values(-1::int8);
insert ignore into test_uint2 values(65536::int8);
insert ignore into test_uint2 values(-1::int8);

---- f4toui2
---- over uint2_max 严格模式error，非严格模式warning，结果uint2_max
insert into test_uint2 values(65536.5::float4);
insert into test_uint2 values(-1.5::float4);
insert ignore into test_uint2 values(65536.5::float4);
insert ignore into test_uint2 values(-1.5::float4);

---- f8toui2
---- over uint2_max 严格模式error，非严格模式warning，结果uint2_max
insert into test_uint2 values(65536.5::float8);
insert into test_uint2 values(-1.5::float8);
insert ignore into test_uint2 values(65536.5::float8);
insert ignore into test_uint2 values(-1.5::float8);

---- ui4toui1
---- over uint1_max 严格模式error，非严格模式warning，结果uint1_max
insert into test_uint1 values(256::uint4);
insert into test_uint1 values(-1::uint4);
insert ignore into test_uint1 values(256::uint4);
insert ignore into test_uint1 values(-1::uint4);

---- ui4toi1
---- over int1_max 严格模式error，非严格模式warning，结果int1_max
insert into test_int1 values(128::uint4);
insert into test_int1 values(-129::uint4);
insert ignore into test_int1 values(128::uint4);
insert ignore into test_int1 values(-129::uint4);

---- i1toui4
---- over uint4_max 严格模式error，非严格模式warning，结果int1_max
insert into test_uint4 values(2147483648::int1);
insert into test_uint4 values(-129::int1);
insert ignore into test_uint4 values(2147483648::int1);
insert ignore into test_uint4 values(-129::int1);

---- ui2toi1
---- over int1_max 严格模式error，非严格模式warning，结果int1_max
insert into test_int1 values(128::uint2);
insert into test_int1 values(-129::uint2);
insert ignore into test_int1 values(128::uint2);
insert ignore into test_int1 values(-129::uint2);

---- i1toui2
---- over uint2_max 严格模式error，非严格模式warning，结果int1_max
insert into test_uint2 values(65536::int1);
insert into test_uint2 values(-1::int1);
insert ignore into test_uint2 values(65536::int1);
insert ignore into test_uint2 values(-1::int1);

---- ui1toi1
---- over int1_max 严格模式error，非严格模式warning，结果int1_max
insert into test_int1 values(128::uint1);
insert into test_int1 values(-129::uint1);
insert ignore into test_int1 values(128::uint1);
insert ignore into test_int1 values(-129::uint1);

---- i1toui1
---- over uint1_max 严格模式error，非严格模式warning，结果int1_max
insert into test_uint1 values(256::int1);
insert into test_uint1 values(-1::int1);
insert ignore into test_uint1 values(256::int1);
insert ignore into test_uint1 values(-1::int1);

---- ui4toui2
---- over uint2_max 严格模式error，非严格模式warning，结果uint2_max
insert into test_uint2 values(65536::uint4);
insert into test_uint2 values(-1::uint4);
insert ignore into test_uint2 values(65536::uint4);
insert ignore into test_uint2 values(-1::uint4);

---- ui4toi2
---- over int2_max 严格模式error，非严格模式warning，结果int2_max
insert into test_int2 values(32768::uint4);
insert into test_int2 values(-32769::uint4);
insert ignore into test_int2 values(32768::uint4);
insert ignore into test_int2 values(-32769::uint4);

---- i2toui4
---- over uint4_max 严格模式error，非严格模式warning，结果int2_max
insert into test_uint4 values(4294967296::int2);
insert into test_uint4 values(-129::int2);
insert ignore into test_uint4 values(4294967296::int2);
insert ignore into test_uint4 values(-129::int2);

---- ui4toi4
---- over int4_max 严格模式error，非严格模式warning，结果int4_max
insert into test_int4 values(2147483648::uint4);
insert into test_int4 values(-2147483649::uint4);
insert ignore into test_int4 values(2147483648::uint4);
insert ignore into test_int4 values(-2147483649::uint4);

---- i4toui4
---- over uint4_max 严格模式error，非严格模式warning，结果int4_max
insert into test_uint4 values(4294967296::int4);
insert into test_uint4 values(-129::int4);
insert ignore into test_uint4 values(4294967296::int4);
insert ignore into test_uint4 values(-129::int4);

---- ui4toi4
---- over int4_max 严格模式error，非严格模式warning，结果int4_max
insert into test_int4 values(4294967296::uint4);
insert into test_int4 values(-2147483649::uint4);
insert ignore into test_int4 values(4294967296::uint4);
insert ignore into test_int4 values(-2147483649::uint4);

---- f4toui4
---- over uint4_max 严格模式error，非严格模式warning，结果uint4_max
insert into test_uint4 values(4294967296.5::float4);
insert into test_uint4 values(-129.5::float4);
insert ignore into test_uint4 values(4294967296.5::float4);
insert ignore into test_uint4 values(-129.5::float4);

---- f4toui4
---- over uint4_max 严格模式error，非严格模式warning，结果uint4_max
insert into test_uint4 values(4294967296.5::float8);
insert into test_uint4 values(-129.5::float8);
insert ignore into test_uint4 values(4294967296.5::float8);
insert ignore into test_uint4 values(-129.5::float8);

---- ui8toui1
---- over uint1_max 严格模式error，非严格模式warning，结果uint1_max
insert into test_uint1 values(256::uint8);
insert into test_uint1 values(-1::uint8);
insert ignore into test_uint1 values(256::uint8);
insert ignore into test_uint1 values(-1::uint8);

---- ui8toi1
---- over int1_max 严格模式error，非严格模式warning，结果int1_max
insert into test_int1 values(128::uint8);
insert into test_int1 values(-129::uint8);
insert ignore into test_int1 values(128::uint8);
insert ignore into test_int1 values(-129::uint8);

---- i1toui8
---- over int1_max 严格模式error，非严格模式warning，结果int1_max
insert into test_uint8 values(256::int1);
insert into test_uint8 values(-129::int1);
insert ignore into test_uint8 values(256::int1);
insert ignore into test_uint8 values(-129::int1);

---- ui8toui2
---- over uint2_max 严格模式error，非严格模式warning，结果uint2_max
insert into test_uint2 values(65536::uint8);
insert into test_uint2 values(-1::uint8);
insert ignore into test_uint2 values(65536::uint8);
insert ignore into test_uint2 values(-1::uint8);

---- ui8toi2
---- over int2_max 严格模式error，非严格模式warning，结果int2_max
insert into test_int2 values(32768::uint8);
insert into test_int2 values(-32769::uint8);
insert ignore into test_int2 values(32768::uint8);
insert ignore into test_int2 values(-32769::uint8);

---- i2toui8
---- over int2_max 严格模式error，非严格模式warning，结果int2_max
insert into test_uint8 values(32768::int2);
insert into test_uint8 values(-32769::int2);
insert ignore into test_uint8 values(32768::int2);
insert ignore into test_uint8 values(-32769::int2);

---- ui8toi4
---- over int4_max 严格模式error，非严格模式warning，结果int4_max
insert into test_int4 values(4294967296::uint8);
insert into test_int4 values(-2147483649::uint8);
insert ignore into test_int4 values(4294967296::uint8);
insert ignore into test_int4 values(-2147483649::uint8);

---- i4toui8
---- over int4_max 严格模式error，非严格模式warning，结果int4_max
insert into test_uint8 values(4294967296::int4);
insert into test_uint8 values(-2147483649::int4);
insert ignore into test_uint8 values(4294967296::int4);
insert ignore into test_uint8 values(-2147483649::int4);

---- ui8toui4
---- over uint4_max 严格模式error，非严格模式warning，结果uint4_max
insert into test_uint4 values(4294967296::uint8);
insert into test_uint4 values(-129::uint8);
insert ignore into test_uint4 values(4294967296::uint8);
insert ignore into test_uint4 values(-129::uint8);

---- ui8toi8
---- over int8_max 严格模式error，非严格模式warning，结果int8_max
insert into test_int8 values(9223372036854775808::uint8);
insert into test_int8 values(-9223372036854775809::uint8);
insert ignore into test_int8 values(9223372036854775808::uint8);
insert ignore into test_int8 values(-9223372036854775808::uint8);

---- i8toui8
---- over int8_max 严格模式error，非严格模式warning，结果int8_max
insert into test_uint8 values(9223372036854775808::int8);
insert into test_uint8 values(-2147483649::int8);
insert ignore into test_uint8 values(9223372036854775808::int8);
insert ignore into test_uint8 values(-2147483649::int8);

---- f4toui8
---- over uint8_max 严格模式error，非严格模式warning，结果uint8_max
insert into test_uint8 values(18446744073709551616::float4);
insert into test_uint8 values(-2147483649::float4);
insert ignore into test_uint8 values(18446744073709551616::float4);
insert ignore into test_uint8 values(-2147483649::float4);

---- f8toui8
---- over uint8_max 严格模式error，非严格模式warning，结果uint8_max
insert into test_uint8 values(18446744073709551616::float8);
insert into test_uint8 values(-2147483649::float8);
insert ignore into test_uint8 values(18446744073709551616::float8);
insert ignore into test_uint8 values(-2147483649::float8);

---- i1_cast_ui1
---- 127
select cast(128::int1 as uint1);
---- 246
select cast(-10::int1 as uint1);

---- i4_cast_ui1
---- 255
select cast(2147483648::int4 as uint1);
---- 246
select cast(-10::int4 as uint1);

---- i8_cast_ui1
---- 255
select cast(9223372036854775808::int8 as uint1);
---- 246
select cast(-10::int8 as uint1);

---- i4_cast_ui2
---- 65535
select cast(2147483648::int4 as uint2);
---- 65526
select cast(-10::int4 as uint2);

---- i8_cast_ui2
---- 65535
select cast(9223372036854775808::int8 as uint2);
---- 65526
select cast(-10::int8 as uint2);

---- i8_cast_ui4
---- 4294967295
select cast(9223372036854775808::int8 as uint4);
---- 4294967286
select cast(-10::int8 as uint4);

---- f4_cast_ui1
---- 255
select cast(9223372036854775808.5::float4 as uint1);
---- 245
select cast(-10.5::float4 as uint1);

---- f8_cast_ui1
---- 255
select cast(9223372036854775808.5::float8 as uint1);
---- 245
select cast(-10.5::float8 as uint1);

---- f4_cast_ui2
---- 65535
select cast(65535.5::float4 as uint2);
---- 65525
select cast(-10.5::float4 as uint2);

---- f8_cast_ui2
---- 65535
select cast(65535.5::float8 as uint2);
---- 65525
select cast(-10.5::float8 as uint2);

---- f4_cast_ui4
---- 65535
select cast(4294967295.5::float4 as uint4);
---- 65525
select cast(-10.5::float4 as uint4);

---- f8_cast_ui4
---- 65535
select cast(4294967295.5::float8 as uint4);
---- 65525
select cast(-10.5::float8 as uint4);

---- unexpected
---- f4_cast_ui8
---- 65535
select cast(18446744073709551615.5::float4 as uint8);
---- 65525
select cast(-10.5::float4 as uint8);

---- f8_cast_ui8
---- 65535
select cast(18446744073709551615.5::float8 as uint8);
---- 65525
select cast(-10.5::float8 as uint8);

select * from test_int1;
select * from test_int2;
select * from test_int4;
select * from test_int8;
select * from test_uint1;
select * from test_uint2;
select * from test_uint4;
select * from test_uint8;
---- drop database
\c contrib_regression
DROP DATABASE none_strict_warning_test;
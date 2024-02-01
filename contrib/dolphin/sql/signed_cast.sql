create schema signed_cast;
set current_schema to 'signed_cast';
-- when b_compatibility_mode = false;
set dolphin.b_compatibility_mode = false;
--- strict mode
SET dolphin.sql_mode = 'sql_mode_strict';
---- numeric_cast_int8
select cast(123::numeric as signed);
select cast(1e30 as int8);
select cast(1e30 as signed);
select cast(18446744073709551615 as signed);
select cast(18446744073709551616 as signed);
select cast(9223372036854775807 as signed);
select cast(9223372036854775808 as signed);
select cast(-9223372036854775808 as signed);
select cast(-9223372036854775809 as signed);
---- uint8_cast_int8
select cast(123::uint8 as signed);
select cast(1e30::uint8 as int8);
select cast(1e30::uint8 as signed);
select cast(18446744073709551615::uint8 as signed);
select cast(18446744073709551616::uint8 as signed);
select cast(9223372036854775807::uint8 as signed);
select cast(9223372036854775808::uint8 as signed);
select cast(-9223372036854775808::uint8 as signed);
select cast(-9223372036854775809::uint8 as signed);
---- text_cast_int8
select cast('123'::text as signed);
select cast('18446744073709551615'::text as signed);
select cast('18446744073709551616'::text as signed);
select cast('9223372036854775807'::text as signed);
select cast('9223372036854775808'::text as signed);
select cast('-9223372036854775808'::text as signed);
select cast('-9223372036854775809'::text as signed);
---- nvarchar2_cast_int8
select cast('123'::nvarchar2 as signed);
select cast('18446744073709551615'::nvarchar2 as signed);
select cast('18446744073709551616'::nvarchar2 as signed);
select cast('9223372036854775807'::nvarchar2 as signed);
select cast('9223372036854775808'::nvarchar2 as signed);
select cast('-9223372036854775808'::nvarchar2 as signed);
select cast('-9223372036854775809'::nvarchar2 as signed);
---- timestamp_cast_int8
select cast('2023-01-01 00:00:00'::timestamp as signed);
---- time_cast_int8
select cast('23:01:01'::time as signed);
---- bit_cast_int8
select cast(b'1111' as signed);
---- unknown_cast_int8
select cast('123' as signed);
select cast('18446744073709551615' as signed);
select cast('18446744073709551616' as signed);
select cast('9223372036854775807'as signed);
select cast('9223372036854775808'as signed);
select cast('-9223372036854775808' as signed);
select cast('-9223372036854775809' as signed);
---- float4_cast_int8
select cast('123.4'::float4 as signed);
select cast('123.6'::float4 as signed);
select cast('123'::float4 as signed);
select cast('1e30'::float4 as int8);
select cast('1e30'::float4 as signed);
select cast('18446744073709551615'::float4 as signed);
select cast('18446744073709551616'::float4 as signed);
select cast('9223372036854775807'::float4 as signed);
select cast('9223372036854775808'::float4 as signed);
select cast('-9223372036854775808'::float4 as signed);
select cast('-9223372036854775809'::float4 as signed);
---- float8_cast_int8
select cast('123.4'::float8 as signed);
select cast('123.6'::float8 as signed);
select cast('123'::float8 as signed);
select cast('1e30'::float8 as int8);
select cast('1e30'::float8 as signed);
select cast('18446744073709551615'::float8 as signed);
select cast('18446744073709551616'::float8 as signed);
select cast('9223372036854775807'::float8 as signed);
select cast('9223372036854775808'::float8 as signed);
select cast('-9223372036854775808'::float8 as signed);
select cast('-9223372036854775809'::float8 as signed);
---- table test
----- numeric
drop table if exists test_numeric;
create table test_numeric(a numeric(20,0));
insert into test_numeric values(9223372036854775807),
                                (9223372036854775808),
                                (-9223372036854775808),
                                (-9223372036854775809),
                                (18446744073709551615),
                                (18446744073709551616);
select * from test_numeric;
select cast(a as signed) from test_numeric;
----- float4
drop table if exists test_float;
create table test_float(a FLOAT4);
insert into test_float values(9223372036854775807),
                               (9223372036854775808),
                               (-9223372036854775808),
                               (-9223372036854775809),
                               (18446744073709551615),
                               (18446744073709551616);
select * from test_float;
select cast(a as signed) from test_float;
----- float8
drop table if exists test_double;
create table test_double(a FLOAT8);
insert into test_double values(9223372036854775807),
                               (9223372036854775808),
                               (-9223372036854775808),
                               (-9223372036854775809),
                               (18446744073709551615),
                               (18446744073709551616);
select * from test_double;
select cast(a as signed) from test_double;
----- VARCHAR
drop table if exists test_varchar;
create table test_varchar(a VARCHAR(20));
insert into test_varchar values('9223372036854775807'),
                               ('9223372036854775808'),
                               ('-9223372036854775808'),
                               ('-9223372036854775809'),
                               ('18446744073709551615'),
                               ('18446744073709551616');
select * from test_varchar;
select cast(a as signed) from test_varchar;
----- NVARCHAR2
drop table if exists test_nvarchar2;
create table test_nvarchar2(a NVARCHAR2(20));
insert into test_nvarchar2 values('9223372036854775807'),
                               ('9223372036854775808'),
                               ('-9223372036854775808'),
                               ('-9223372036854775809'),
                               ('18446744073709551615'),
                               ('18446744073709551616');
select * from test_nvarchar2;
select cast(a as signed) from test_nvarchar2;
----- TEXT
drop table if exists test_text;
create table test_text(a VARCHAR(20));
insert into test_text values('9223372036854775807'),
                               ('9223372036854775808'),
                               ('-9223372036854775808'),
                               ('-9223372036854775809'),
                               ('18446744073709551615'),
                               ('18446744073709551616');
select * from test_text;
select cast(a as signed) from test_text;
----- bpchar
drop table if exists test_bpchar;
create table test_bpchar(a CHAR(20));
insert into test_bpchar values('9223372036854775807'),
                               ('9223372036854775808'),
                               ('-9223372036854775808'),
                               ('-9223372036854775809'),
                               ('18446744073709551615'),
                               ('18446744073709551616');
select * from test_bpchar;
select cast(a as signed) from test_bpchar;
----- uint8
drop table if exists test_uint8;
create table test_uint8(a bigint unsigned);
insert into test_uint8 values(9223372036854775807),
                               (9223372036854775808),
                               (18446744073709551615);														 
insert into test_uint8 values(18446744073709551616);
insert into test_uint8 values(-9223372036854775809);
insert into test_uint8 values(-9223372036854775808);
select * from test_uint8;
select cast(a as signed) from test_uint8;
----- bit
drop table if exists test_bit;
create table test_bit(a bit(64));
insert into test_bit values(b'111111111111111111111111111111111111111111111111111111111111111'),
                           (b'1000000000000000000000000000000000000000000000000000000000000000'),
						   (b'1111111111111111111111111111111111111111111111111111111111111110'),
                           (b'1111111111111111111111111111111111111111111111111111111111111111');
select * from test_bit;
select cast(a as signed) from test_bit;
select cast(b'111111111111111111111111111111111111111111111111111111111111111' as signed);--9223372036854775807
select cast(b'1000000000000000000000000000000000000000000000000000000000000000' as signed);--9223372036854775808
select cast(b'1111111111111111111111111111111111111111111111111111111111111110' as signed);--18446744073709551614
select cast(b'1111111111111111111111111111111111111111111111111111111111111111' as signed);--18446744073709551615
select cast(b'10000000000000000000000000000000000000000000000000000000000000000' as signed);--18446744073709551616
--- no strict mode
set dolphin.sql_mode = '';
---- numeric_cast_int8
select cast(123::numeric as signed);
select cast(1e30 as int8);
select cast(1e30 as signed);
select cast(18446744073709551615 as signed);
select cast(18446744073709551616 as signed);
select cast(9223372036854775807 as signed);
select cast(9223372036854775808 as signed);
select cast(-9223372036854775808 as signed);
select cast(-9223372036854775809 as signed);
---- uint8_cast_int8
select cast(123::uint8 as signed);
select cast(1e30::uint8 as int8);
select cast(1e30::uint8 as signed);
select cast(18446744073709551615::uint8 as signed);
select cast(18446744073709551616::uint8 as signed);
select cast(9223372036854775807::uint8 as signed);
select cast(9223372036854775808::uint8 as signed);
select cast(-9223372036854775808::uint8 as signed);
select cast(-9223372036854775809::uint8 as signed);
---- text_cast_int8
select cast('123'::text as signed);
select cast('18446744073709551615'::text as signed);
select cast('18446744073709551616'::text as signed);
select cast('9223372036854775807'::text as signed);
select cast('9223372036854775808'::text as signed);
select cast('-9223372036854775808'::text as signed);
select cast('-9223372036854775809'::text as signed);
---- timestamp_cast_int8
select cast('2023-01-01 00:00:00'::timestamp as signed);
---- time_cast_int8
select cast('23:01:01'::time as signed);
---- bit_cast_int8
select cast(b'1111' as signed);
---- unknown_cast_int8
select cast('123' as signed);
select cast('18446744073709551615' as signed);
select cast('18446744073709551616' as signed);
select cast('9223372036854775807'as signed);
select cast('9223372036854775808'as signed);
select cast('-9223372036854775808' as signed);
select cast('-9223372036854775809' as signed);
---- float4_cast_int8
select cast('123.4'::float4 as signed);
select cast('123.6'::float4 as signed);
select cast('123'::float4 as signed);
select cast('1e30'::float4 as int8);
select cast('1e30'::float4 as signed);
select cast('18446744073709551615'::float4 as signed);
select cast('18446744073709551616'::float4 as signed);
select cast('9223372036854775807'::float4 as signed);
select cast('9223372036854775808'::float4 as signed);
select cast('-9223372036854775808'::float4 as signed);
select cast('-9223372036854775809'::float4 as signed);
---- float8_cast_int8
select cast('123.4'::float8 as signed);
select cast('123.6'::float8 as signed);
select cast('123'::float8 as signed);
select cast('1e30'::float8 as int8);
select cast('1e30'::float8 as signed);
select cast('18446744073709551615'::float8 as signed);
select cast('18446744073709551616'::float8 as signed);
select cast('9223372036854775807'::float8 as signed);
select cast('9223372036854775808'::float8 as signed);
select cast('-9223372036854775808'::float8 as signed);
select cast('-9223372036854775809'::float8 as signed);
---- table test
----- numeric
drop table if exists test_numeric;
create table test_numeric(a numeric(20,0));
insert into test_numeric values(9223372036854775807),
                                (9223372036854775808),
                                (-9223372036854775808),
                                (-9223372036854775809),
                                (18446744073709551615),
                                (18446744073709551616);
select * from test_numeric;
select cast(a as signed) from test_numeric;
----- float4
drop table if exists test_float;
create table test_float(a FLOAT4);
insert into test_float values(9223372036854775807),
                               (9223372036854775808),
                               (-9223372036854775808),
                               (-9223372036854775809),
                               (18446744073709551615),
                               (18446744073709551616);
select * from test_float;
select cast(a as signed) from test_float;
----- float8
drop table if exists test_double;
create table test_double(a FLOAT8);
insert into test_double values(9223372036854775807),
                               (9223372036854775808),
                               (-9223372036854775808),
                               (-9223372036854775809),
                               (18446744073709551615),
                               (18446744073709551616);
select * from test_double;
select cast(a as signed) from test_double;
----- VARCHAR
drop table if exists test_varchar;
create table test_varchar(a VARCHAR(20));
insert into test_varchar values('9223372036854775807'),
                               ('9223372036854775808'),
                               ('-9223372036854775808'),
                               ('-9223372036854775809'),
                               ('18446744073709551615'),
                               ('18446744073709551616');
select * from test_varchar;
select cast(a as signed) from test_varchar;
----- TEXT
drop table if exists test_text;
create table test_text(a VARCHAR(20));
insert into test_text values('9223372036854775807'),
                               ('9223372036854775808'),
                               ('-9223372036854775808'),
                               ('-9223372036854775809'),
                               ('18446744073709551615'),
                               ('18446744073709551616');
select * from test_text;
select cast(a as signed) from test_text;
----- bpchar
drop table if exists test_bpchar;
create table test_bpchar(a CHAR(20));
insert into test_bpchar values('9223372036854775807'),
                               ('9223372036854775808'),
                               ('-9223372036854775808'),
                               ('-9223372036854775809'),
                               ('18446744073709551615'),
                               ('18446744073709551616');
select * from test_bpchar;
select cast(a as signed) from test_bpchar;
----- uint8
drop table if exists test_uint8;
create table test_uint8(a bigint unsigned);
insert into test_uint8 values(9223372036854775807),
                               (9223372036854775808),
                               (18446744073709551615);														 
insert into test_uint8 values(18446744073709551616);
insert into test_uint8 values(-9223372036854775809);
insert into test_uint8 values(-9223372036854775808);
select * from test_uint8;
select cast(a as signed) from test_uint8;
----- bit
drop table if exists test_bit;
create table test_bit(a bit(64));
insert into test_bit values(b'111111111111111111111111111111111111111111111111111111111111111'),
                           (b'1000000000000000000000000000000000000000000000000000000000000000'),
						   (b'1111111111111111111111111111111111111111111111111111111111111110'),
                           (b'1111111111111111111111111111111111111111111111111111111111111111');
select * from test_bit;
select cast(a as signed) from test_bit;
select cast(b'111111111111111111111111111111111111111111111111111111111111111' as signed);--9223372036854775807
select cast(b'1000000000000000000000000000000000000000000000000000000000000000' as signed);--9223372036854775808
select cast(b'1111111111111111111111111111111111111111111111111111111111111110' as signed);--18446744073709551614
select cast(b'1111111111111111111111111111111111111111111111111111111111111111' as signed);--18446744073709551615
select cast(b'10000000000000000000000000000000000000000000000000000000000000000' as signed);--18446744073709551616
-- when b_compatibility_mode = true
set dolphin.b_compatibility_mode = true;
--- strict mode
SET dolphin.sql_mode = 'sql_mode_strict';
---- numeric_cast_int8
select cast(123::numeric as signed);
select cast(1e30 as int8);
select cast(1e30 as signed);
select cast(18446744073709551615 as signed);
select cast(18446744073709551616 as signed);
select cast(9223372036854775807 as signed);
select cast(9223372036854775808 as signed);
select cast(-9223372036854775808 as signed);
select cast(-9223372036854775809 as signed);
---- uint8_cast_int8
select cast(123::uint8 as signed);
select cast(1e30::uint8 as int8);
select cast(1e30::uint8 as signed);
select cast(18446744073709551615::uint8 as signed);
select cast(18446744073709551616::uint8 as signed);
select cast(9223372036854775807::uint8 as signed);
select cast(9223372036854775808::uint8 as signed);
select cast(-9223372036854775808::uint8 as signed);
select cast(-9223372036854775809::uint8 as signed);
---- text_cast_int8
select cast('123'::text as signed);
select cast('18446744073709551615'::text as signed);
select cast('18446744073709551616'::text as signed);
select cast('9223372036854775807'::text as signed);
select cast('9223372036854775808'::text as signed);
select cast('-9223372036854775808'::text as signed);
select cast('-9223372036854775809'::text as signed);
---- timestamp_cast_int8
select cast('2023-01-01 00:00:00'::timestamp as signed);
---- time_cast_int8
select cast('23:01:01'::time as signed);
---- bit_cast_int8
select cast(b'1111' as signed);
---- unknown_cast_int8
select cast('123' as signed);
select cast('18446744073709551615' as signed);
select cast('18446744073709551616' as signed);
select cast('9223372036854775807'as signed);
select cast('9223372036854775808'as signed);
select cast('-9223372036854775808' as signed);
select cast('-9223372036854775809' as signed);
---- float4_cast_int8
select cast('123.4'::float4 as signed);
select cast('123.6'::float4 as signed);
select cast('123'::float4 as signed);
select cast('1e30'::float4 as int8);
select cast('1e30'::float4 as signed);
select cast('18446744073709551615'::float4 as signed);
select cast('18446744073709551616'::float4 as signed);
select cast('9223372036854775807'::float4 as signed);
select cast('9223372036854775808'::float4 as signed);
select cast('-9223372036854775808'::float4 as signed);
select cast('-9223372036854775809'::float4 as signed);
---- float8_cast_int8
select cast('123.4'::float8 as signed);
select cast('123.6'::float8 as signed);
select cast('123'::float8 as signed);
select cast('1e30'::float8 as int8);
select cast('1e30'::float8 as signed);
select cast('18446744073709551615'::float8 as signed);
select cast('18446744073709551616'::float8 as signed);
select cast('9223372036854775807'::float8 as signed);
select cast('9223372036854775808'::float8 as signed);
select cast('-9223372036854775808'::float8 as signed);
select cast('-9223372036854775809'::float8 as signed);
---- table test
----- numeric
drop table if exists test_numeric;
create table test_numeric(a numeric(20,0));
insert into test_numeric values(9223372036854775807),
                                (9223372036854775808),
                                (-9223372036854775808),
                                (-9223372036854775809),
                                (18446744073709551615),
                                (18446744073709551616);
select * from test_numeric;
select cast(a as signed) from test_numeric;
----- float4
drop table if exists test_float;
create table test_float(a FLOAT4);
insert into test_float values(9223372036854775807),
                               (9223372036854775808),
                               (-9223372036854775808),
                               (-9223372036854775809),
                               (18446744073709551615),
                               (18446744073709551616);
select * from test_float;
select cast(a as signed) from test_float;
----- float8
drop table if exists test_double;
create table test_double(a FLOAT8);
insert into test_double values(9223372036854775807),
                               (9223372036854775808),
                               (-9223372036854775808),
                               (-9223372036854775809),
                               (18446744073709551615),
                               (18446744073709551616);
select * from test_double;
select cast(a as signed) from test_double;
----- VARCHAR
drop table if exists test_varchar;
create table test_varchar(a VARCHAR(20));
insert into test_varchar values('9223372036854775807'),
                               ('9223372036854775808'),
                               ('-9223372036854775808'),
                               ('-9223372036854775809'),
                               ('18446744073709551615'),
                               ('18446744073709551616');
select * from test_varchar;
select cast(a as signed) from test_varchar;
----- TEXT
drop table if exists test_text;
create table test_text(a VARCHAR(20));
insert into test_text values('9223372036854775807'),
                               ('9223372036854775808'),
                               ('-9223372036854775808'),
                               ('-9223372036854775809'),
                               ('18446744073709551615'),
                               ('18446744073709551616');
select * from test_text;
select cast(a as signed) from test_text;
----- bpchar
drop table if exists test_bpchar;
create table test_bpchar(a CHAR(20));
insert into test_bpchar values('9223372036854775807'),
                               ('9223372036854775808'),
                               ('-9223372036854775808'),
                               ('-9223372036854775809'),
                               ('18446744073709551615'),
                               ('18446744073709551616');
select * from test_bpchar;
select cast(a as signed) from test_bpchar;
----- uint8
drop table if exists test_uint8;
create table test_uint8(a bigint unsigned);
insert into test_uint8 values(9223372036854775807),
                               (9223372036854775808),
                               (18446744073709551615);														 
insert into test_uint8 values(18446744073709551616);
insert into test_uint8 values(-9223372036854775809);
insert into test_uint8 values(-9223372036854775808);
select * from test_uint8;
select cast(a as signed) from test_uint8;
----- bit
drop table if exists test_bit;
create table test_bit(a bit(64));
insert into test_bit values(b'111111111111111111111111111111111111111111111111111111111111111'),
                           (b'1000000000000000000000000000000000000000000000000000000000000000'),
						   (b'1111111111111111111111111111111111111111111111111111111111111110'),
                           (b'1111111111111111111111111111111111111111111111111111111111111111');
select * from test_bit;
select cast(a as signed) from test_bit;
select cast(b'111111111111111111111111111111111111111111111111111111111111111' as signed);--9223372036854775807
select cast(b'1000000000000000000000000000000000000000000000000000000000000000' as signed);--9223372036854775808
select cast(b'1111111111111111111111111111111111111111111111111111111111111110' as signed);--18446744073709551614
select cast(b'1111111111111111111111111111111111111111111111111111111111111111' as signed);--18446744073709551615
select cast(b'10000000000000000000000000000000000000000000000000000000000000000' as signed);--18446744073709551616
--- no strict mode
set dolphin.sql_mode = '';
---- numeric_cast_int8
select cast(123::numeric as signed);
select cast(1e30 as int8);
select cast(1e30 as signed);
select cast(18446744073709551615 as signed);
select cast(18446744073709551616 as signed);
select cast(9223372036854775807 as signed);
select cast(9223372036854775808 as signed);
select cast(-9223372036854775808 as signed);
select cast(-9223372036854775809 as signed);
---- uint8_cast_int8
select cast(123::uint8 as signed);
select cast(1e30::uint8 as int8);
select cast(1e30::uint8 as signed);
select cast(18446744073709551615::uint8 as signed);
select cast(18446744073709551616::uint8 as signed);
select cast(9223372036854775807::uint8 as signed);
select cast(9223372036854775808::uint8 as signed);
select cast(-9223372036854775808::uint8 as signed);
select cast(-9223372036854775809::uint8 as signed);
---- text_cast_int8
select cast('123'::text as signed);
select cast('18446744073709551615'::text as signed);
select cast('18446744073709551616'::text as signed);
select cast('9223372036854775807'::text as signed);
select cast('9223372036854775808'::text as signed);
select cast('-9223372036854775808'::text as signed);
select cast('-9223372036854775809'::text as signed);
---- timestamp_cast_int8
select cast('2023-01-01 00:00:00'::timestamp as signed);
---- time_cast_int8
select cast('23:01:01'::time as signed);
---- bit_cast_int8
select cast(b'1111' as signed);
---- unknown_cast_int8
select cast('123' as signed);
select cast('18446744073709551615' as signed);
select cast('18446744073709551616' as signed);
select cast('9223372036854775807'as signed);
select cast('9223372036854775808'as signed);
select cast('-9223372036854775808' as signed);
select cast('-9223372036854775809' as signed);
---- float4_cast_int8
select cast('123.4'::float4 as signed);
select cast('123.6'::float4 as signed);
select cast('123'::float4 as signed);
select cast('1e30'::float4 as int8);
select cast('1e30'::float4 as signed);
select cast('18446744073709551615'::float4 as signed);
select cast('18446744073709551616'::float4 as signed);
select cast('9223372036854775807'::float4 as signed);
select cast('9223372036854775808'::float4 as signed);
select cast('-9223372036854775808'::float4 as signed);
select cast('-9223372036854775809'::float4 as signed);
---- float8_cast_int8
select cast('123.4'::float8 as signed);
select cast('123.6'::float8 as signed);
select cast('123'::float8 as signed);
select cast('1e30'::float8 as int8);
select cast('1e30'::float8 as signed);
select cast('18446744073709551615'::float8 as signed);
select cast('18446744073709551616'::float8 as signed);
select cast('9223372036854775807'::float8 as signed);
select cast('9223372036854775808'::float8 as signed);
select cast('-9223372036854775808'::float8 as signed);
select cast('-9223372036854775809'::float8 as signed);
---- table test
----- numeric
drop table if exists test_numeric;
create table test_numeric(a numeric(20,0));
insert into test_numeric values(9223372036854775807),
                                (9223372036854775808),
                                (-9223372036854775808),
                                (-9223372036854775809),
                                (18446744073709551615),
                                (18446744073709551616);
select * from test_numeric;
select cast(a as signed) from test_numeric;
----- float4
drop table if exists test_float;
create table test_float(a FLOAT4);
insert into test_float values(9223372036854775807),
                               (9223372036854775808),
                               (-9223372036854775808),
                               (-9223372036854775809),
                               (18446744073709551615),
                               (18446744073709551616);
select * from test_float;
select cast(a as signed) from test_float;
----- float8
drop table if exists test_double;
create table test_double(a FLOAT8);
insert into test_double values(9223372036854775807),
                               (9223372036854775808),
                               (-9223372036854775808),
                               (-9223372036854775809),
                               (18446744073709551615),
                               (18446744073709551616);
select * from test_double;
select cast(a as signed) from test_double;
----- VARCHAR
drop table if exists test_varchar;
create table test_varchar(a VARCHAR(20));
insert into test_varchar values('9223372036854775807'),
                               ('9223372036854775808'),
                               ('-9223372036854775808'),
                               ('-9223372036854775809'),
                               ('18446744073709551615'),
                               ('18446744073709551616');
select * from test_varchar;
select cast(a as signed) from test_varchar;
----- TEXT
drop table if exists test_text;
create table test_text(a VARCHAR(20));
insert into test_text values('9223372036854775807'),
                               ('9223372036854775808'),
                               ('-9223372036854775808'),
                               ('-9223372036854775809'),
                               ('18446744073709551615'),
                               ('18446744073709551616');
select * from test_text;
select cast(a as signed) from test_text;
----- bpchar
drop table if exists test_bpchar;
create table test_bpchar(a CHAR(20));
insert into test_bpchar values('9223372036854775807'),
                               ('9223372036854775808'),
                               ('-9223372036854775808'),
                               ('-9223372036854775809'),
                               ('18446744073709551615'),
                               ('18446744073709551616');
select * from test_bpchar;
select cast(a as signed) from test_bpchar;
----- uint8
drop table if exists test_uint8;
create table test_uint8(a bigint unsigned);
insert into test_uint8 values(9223372036854775807),
                               (9223372036854775808),
                               (18446744073709551615);														 
insert into test_uint8 values(18446744073709551616);
insert into test_uint8 values(-9223372036854775809);
insert into test_uint8 values(-9223372036854775808);
select * from test_uint8;
select cast(a as signed) from test_uint8;
----- bit
drop table if exists test_bit;
create table test_bit(a bit(64));
insert into test_bit values(b'111111111111111111111111111111111111111111111111111111111111111'),
                           (b'1000000000000000000000000000000000000000000000000000000000000000'),
						   (b'1111111111111111111111111111111111111111111111111111111111111110'),
                           (b'1111111111111111111111111111111111111111111111111111111111111111');
select * from test_bit;
select cast(a as signed) from test_bit;
select cast(b'111111111111111111111111111111111111111111111111111111111111111' as signed);--9223372036854775807
select cast(b'1000000000000000000000000000000000000000000000000000000000000000' as signed);--9223372036854775808
select cast(b'1111111111111111111111111111111111111111111111111111111111111110' as signed);--18446744073709551614
select cast(b'1111111111111111111111111111111111111111111111111111111111111111' as signed);--18446744073709551615
select cast(b'10000000000000000000000000000000000000000000000000000000000000000' as signed);--18446744073709551616

--- non-strict mode
SET dolphin.sql_mode = '';
select cast('abc' as signed); --warning
select cast('1.9' as signed); --warning

drop table if exists test_numeric;
drop table if exists test_float;
drop table if exists test_double;
drop table if exists test_varchar;
drop table if exists test_text;
drop table if exists test_bpchar;
drop table if exists test_bit;
drop schema signed_cast cascade;
reset current_schema;
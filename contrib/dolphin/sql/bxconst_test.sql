create schema bxconst_test;
set current_schema to 'bxconst_test';

set dolphin.b_compatibility_mode to on;

-- 将bxconst当作bit处理
set dolphin.sql_mode = sql_mode_strict,sql_mode_full_group,pipes_as_concat,ansi_quotes,no_zero_date,pad_char_to_full_length,auto_recompile_function,error_for_division_by_zero;

create table t_bit(a bit(16));
create table t_bin(a binary(6));

select b'11100000111000';
select pg_typeof(b'11100000111000');
select x'4c';
select pg_typeof(x'4c');

insert into t_bit values(b'11100000111000'), (x'4c');
insert into t_bin values(b'11100000111000'), (x'4c');

select * from t_bit;
select * from t_bin;

drop table t_bit;
drop table t_bin;

select 0x01 | 2;
select 0x01 | (2::int1);
select 0x01 | (2::int2);
select 0x01 | (2::int4);
select 0x01 | (2::int8);
select 0x01 | (2::uint1);
select 0x01 | (2::uint2);
select 0x01 | (2::uint4);
select 0x01 | (2::uint8);
select 0x01 | (2::float4);
select 0x01 | (2::float8);
select 0x01 | (2::numeric);
select 2 | 0x01;
select (2::int1) | 0x01;
select (2::int2) | 0x01;
select (2::int4) | 0x01;
select (2::int8) | 0x01;
select (2::uint1) | 0x01;
select (2::uint2) | 0x01;
select (2::uint4) | 0x01;
select (2::uint8) | 0x01;
select (2::float4) | 0x01;
select (2::float8) | 0x01;
select (2::numeric) | 0x01;
select 0x01 | 0x02;

-- 将bxconst当作binary处理
set dolphin.sql_mode = sql_mode_strict,sql_mode_full_group,pipes_as_concat,ansi_quotes,no_zero_date,pad_char_to_full_length,auto_recompile_function,error_for_division_by_zero,treat_bxconst_as_binary;

create table t_bit(a bit(16));
create table t_bin(a binary(6));

select b'11100000111000';
select pg_typeof(b'11100000111000');
select x'4c';
select pg_typeof(x'4c');

insert into t_bit values(b'11100000111000'), (x'4c');
insert into t_bin values(b'11100000111000'), (x'4c');

select * from t_bit;
select * from t_bin;

drop table t_bit;
drop table t_bin;

select 0x01 | 2;
select 0x01 | (2::int1);
select 0x01 | (2::int2);
select 0x01 | (2::int4);
select 0x01 | (2::int8);
select 0x01 | (2::uint1);
select 0x01 | (2::uint2);
select 0x01 | (2::uint4);
select 0x01 | (2::uint8);
select 0x01 | (2::float4);
select 0x01 | (2::float8);
select 0x01 | (2::numeric);
select 2 | 0x01;
select (2::int1) | 0x01;
select (2::int2) | 0x01;
select (2::int4) | 0x01;
select (2::int8) | 0x01;
select (2::uint1) | 0x01;
select (2::uint2) | 0x01;
select (2::uint4) | 0x01;
select (2::uint8) | 0x01;
select (2::float4) | 0x01;
select (2::float8) | 0x01;
select (2::numeric) | 0x01;
select 0x01 | 0x02;

reset dolphin.sql_mode;

drop schema bxconst_test cascade;
reset current_schema;
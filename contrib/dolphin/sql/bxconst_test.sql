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

select x'01' | 2;
select x'01' | (2::int1);
select x'01' | (2::int2);
select x'01' | (2::int4);
select x'01' | (2::int8);
select x'01' | (2::uint1);
select x'01' | (2::uint2);
select x'01' | (2::uint4);
select x'01' | (2::uint8);
select x'01' | (2::float4);
select x'01' | (2::float8);
select x'01' | (2::numeric);
select 2 | x'01';
select (2::int1) | x'01';
select (2::int2) | x'01';
select (2::int4) | x'01';
select (2::int8) | x'01';
select (2::uint1) | x'01';
select (2::uint2) | x'01';
select (2::uint4) | x'01';
select (2::uint8) | x'01';
select (2::float4) | x'01';
select (2::float8) | x'01';
select (2::numeric) | x'01';
select x'01' | x'02';

select 5::binary div 2;
select 5::binary div (2::int1);
select 5::binary div (2::int2);
select 5::binary div (2::int4);
select 5::binary div (2::int8);
select 5::binary div (2::uint1);
select 5::binary div (2::uint2);
select 5::binary div (2::uint4);
select 5::binary div (2::uint8);
select 5::binary div (2::float4);
select 5::binary div (2::float8);
select 5::binary div (2::numeric);
select 5 div 2::binary;
select (5::int1) div 2::binary;
select (5::int2) div 2::binary;
select (5::int4) div 2::binary;
select (5::int8) div 2::binary;
select (5::uint1) div 2::binary;
select (5::uint2) div 2::binary;
select (5::uint4) div 2::binary;
select (5::uint8) div 2::binary;
select (5::float4) div 2::binary;
select (5::float8) div 2::binary;
select (5::numeric) div 2::binary;
select 5::binary div 2::binary;

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

select x'01' | 2;
select x'01' | (2::int1);
select x'01' | (2::int2);
select x'01' | (2::int4);
select x'01' | (2::int8);
select x'01' | (2::uint1);
select x'01' | (2::uint2);
select x'01' | (2::uint4);
select x'01' | (2::uint8);
select x'01' | (2::float4);
select x'01' | (2::float8);
select x'01' | (2::numeric);
select 2 | x'01';
select (2::int1) | x'01';
select (2::int2) | x'01';
select (2::int4) | x'01';
select (2::int8) | x'01';
select (2::uint1) | x'01';
select (2::uint2) | x'01';
select (2::uint4) | x'01';
select (2::uint8) | x'01';
select (2::float4) | x'01';
select (2::float8) | x'01';
select (2::numeric) | x'01';
select x'01' | x'02';

select x'01' div 2;
select x'01' div (2::int1);
select x'01' div (2::int2);
select x'01' div (2::int4);
select x'01' div (2::int8);
select x'01' div (2::uint1);
select x'01' div (2::uint2);
select x'01' div (2::uint4);
select x'01' div (2::uint8);
select x'01' div (2::float4);
select x'01' div (2::float8);
select x'01' div (2::numeric);
select 2 div x'01';
select (2::int1) div x'01';
select (2::int2) div x'01';
select (2::int4) div x'01';
select (2::int8) div x'01';
select (2::uint1) div x'01';
select (2::uint2) div x'01';
select (2::uint4) div x'01';
select (2::uint8) div x'01';
select (2::float4) div x'01';
select (2::float8) div x'01';
select (2::numeric) div x'01';
select x'01' div x'02';

reset dolphin.sql_mode;

drop schema bxconst_test cascade;
reset current_schema;
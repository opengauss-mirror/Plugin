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

reset dolphin.sql_mode;

drop schema bxconst_test cascade;
reset current_schema;

create table t_null(c1 enum('a','','b'));
create table t_notnull(c1 enum('a','b'));

--strict sql_mode
insert into t_null values(0);
insert into t_notnull values(0);
insert into t_null values('');
insert into t_notnull values('');

insert into t_null values(0::int1);
insert into t_null values(0::int2);
insert into t_null values(0::int4);
insert into t_null values(0::int8);
insert into t_null values(0::uint1);
insert into t_null values(0::uint2);
insert into t_null values(0::uint4);
insert into t_null values(0::uint8);
insert into t_null values(0::float4);
insert into t_null values(0::float8);
insert into t_null values(0::numeric);

insert ignore into t_null values(0);
insert ignore into t_notnull values(0);
insert ignore into t_null values('');
insert ignore into t_notnull values('');

insert ignore into t_null values(0::int1);
insert ignore into t_null values(0::int2);
insert ignore into t_null values(0::int4);
insert ignore into t_null values(0::int8);
insert ignore into t_null values(0::uint1);
insert ignore into t_null values(0::uint2);
insert ignore into t_null values(0::uint4);
insert ignore into t_null values(0::uint8);
insert ignore into t_null values(0::float4);
insert ignore into t_null values(0::float8);
insert ignore into t_null values(0::numeric);

insert into t_null values('test');
insert into t_null values('test'::text);
insert into t_null values('test'::bpchar);
insert into t_null values('test'::bpchar(10));
insert into t_null values('test'::varchar);
insert into t_null values('test'::varchar(10));
insert into t_null values('test'::nvarchar2);
insert into t_null values('test'::nvarchar2(10));

insert ignore into t_null values('test');
insert ignore into t_null values('test'::text);
insert ignore into t_null values('test'::bpchar);
insert ignore into t_null values('test'::bpchar(10));
insert ignore into t_null values('test'::varchar);
insert ignore into t_null values('test'::varchar(10));
insert ignore into t_null values('test'::nvarchar2);
insert ignore into t_null values('test'::nvarchar2(10));

--non_strict sql_mode
set dolphin.sql_mode = '';
insert into t_null values(0);
insert into t_notnull values(0);
insert into t_null values('');
insert into t_notnull values('');
insert into t_null values('test');

select c1, c1+0 from t_null;
select c1, c1+0 from t_notnull;
set dolphin.sql_mode = sql_mode_strict,sql_mode_full_group,pipes_as_concat,ansi_quotes,no_zero_date,pad_char_to_full_length,auto_recompile_function,error_for_division_by_zero;
drop table t_null;
drop table t_notnull;


create table t_null(c1 enum('a','','b'));
create table t_notnull(c1 enum('a','b'));

--strict sql_mode
insert into t_null values(0);
insert into t_notnull values(0);
insert into t_null values('');
insert into t_notnull values('');

insert ignore into t_null values(0);
insert ignore into t_notnull values(0);
insert ignore into t_null values('');
insert ignore into t_notnull values('');

insert into t_null values('test');
insert ignore into t_null values('test');

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

create schema default_guc;
set current_schema to 'default_guc';
set dolphin.use_const_value_as_colname = false;

show behavior_compat_options;
select 0.123;
select md5(0.123);
select md5('0.123');

set behavior_compat_options = '';
show behavior_compat_options;
select 0.123;
select md5(0.123);
select md5('0.123');

drop schema default_guc cascade;
reset current_schema;
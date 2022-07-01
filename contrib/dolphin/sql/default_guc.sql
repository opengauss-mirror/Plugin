drop database if exists default_guc;
create database default_guc dbcompatibility 'b';
\c default_guc

show behavior_compat_options;
select 0.123;
select md5(0.123);
select md5('0.123');

set behavior_compat_options = '';
show behavior_compat_options;
select 0.123;
select md5(0.123);
select md5('0.123');

\c postgres
drop database if exists default_guc;
drop database if exists test;
create database test dbcompatibility 'b';
\c test

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
drop database if exists test;
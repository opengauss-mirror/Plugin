drop database if exists format_test;
create database format_test dbcompatibility 'B';
\c format_test
create extension b_sql_plugin;

select if(TRUE, 1, 2);
select if(FALSE, 1, 2);
-- wrong type for first param
select if('abc', 1, 2);

\c postgres
drop database if exists format_test;
drop database if exists mysql;
create database mysql dbcompatibility 'b';
\c mysql
create extension b_sql_plugin;
select regexp('a', true);

\c postgres
drop database if exists mysql;
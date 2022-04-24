drop database if exists format_test;
create database format_test dbcompatibility 'B';
\c format_test
create extension b_sql_plugin;

select crc32('abc');
select crc32('');
select crc32(1);

\c postgres
drop database if exists format_test;

drop database if exists format_test;
create database format_test dbcompatibility 'B';
\c format_test

select convert(1 using 'utf8');
select convert('1' using 'utf8');
select convert('a' using 'utf8');
select convert(1.1 using 'utf8');
select convert(null using 'utf8');
select convert(1);
select convert(1 using 'utf8');
select convert(1 using 'gbk');
select convert(1 using 'utf8');

\c postgres
drop database if exists format_test;

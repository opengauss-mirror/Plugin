drop database if exists db_convert;
create database db_convert dbcompatibility 'B';
\c db_convert
select convert(1 using 'utf8');
select convert('1' using 'utf8');
select convert('a' using 'utf8');
select convert(1.1 using 'utf8');
select convert(null using 'utf8');
select convert(1);
select convert(1 using 'utf8');
select convert(1 using 'gbk');
select convert(1 using 'utf8');
select convert('测试' using 'utf8');
select convert('测试' using utf8);
select convert(11.1, decimal(10,3));
select convert(1 using decimal(10,3));
\c postgres
drop database if exists db_convert;

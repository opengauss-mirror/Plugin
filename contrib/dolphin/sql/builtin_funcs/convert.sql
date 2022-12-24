create schema db_convert;
set current_schema to 'db_convert';
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
drop schema db_convert cascade;
reset current_schema;

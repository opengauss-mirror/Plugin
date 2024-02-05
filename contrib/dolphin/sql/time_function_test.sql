create schema time_function_test;
set search_path to time_function_test;
    
create table test_year(year year);
insert into test_year values  (1901), (2000), (2023);
select SECOND(year) from test_year;
select MINUTE(year) from test_year;
select HOUR(year) from test_year;
select YEAR(year) from test_year;
    
reset search_path;

set dolphin.sql_mode = '';
select cast(2147483647 as time);
create table test_cast_time(time time);
insert into test_cast_time select cast(2147483647 as time);
insert ignore into test_cast_time select cast(2147483647 as time);
select * from test_cast_time;
drop table if exists test_cast_time;

drop if exists table test_cast_time;
reset dolphin.sql_mode;
select cast(2147483647 as time);
create table test_cast_time(time time);
insert into test_cast_time select cast(2147483647 as time);
insert ignore into test_cast_time select cast(2147483647 as time);
select * from test_cast_time;
drop table if exists test_cast_time;

drop schema time_function_test cascade ;
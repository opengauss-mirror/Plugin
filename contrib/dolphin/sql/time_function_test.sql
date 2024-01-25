create schema time_function_test;
set search_path to time_function_test;
    
create table test_year(year year);
insert into test_year values  (1901), (2000), (2023);
select SECOND(year) from test_year;
select MINUTE(year) from test_year;
select HOUR(year) from test_year;
select YEAR(year) from test_year;
    
reset search_path;
drop schema time_function_test cascade ;
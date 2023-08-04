create schema db_not_between;
set current_schema to 'db_not_between';
select 2 not between 2 and 23;
select 2.1 not between 2.1 and 12.3;
select true not between false and true;
select 'abc' not between 'abc' and 'deg';
select 11 not between 1.1 and 12.5;
select true not between 0 and 12;
select true not between 0.9 and 12.6;
select true between '0.5' and '12.3';
select true not between 'a0.5' and '12.3aa';
select 1 not between 'a0.5' and '12.3aa';
select 1 not between '0.5' and '12.3';
select 1 not between 0 and '12.3';
select 1.1 not between 0 and '12.3';
select 1.1 not between 0.3 and '12.3a';
select '1a' not between 'B' and '12.3b';
select 'a2.2a' not between 2.1 and '4';
select 12.7 not between '11@#&%*' and '19.9hd';
select 'a2.2a' not between 0 and B'101';
select 'a2.2a' not between 1 and B'101';
select '2.2' not between 1 and B'101';
select 2.2 not between 1 and B'101';
select B'001' not between 1 and B'101';
select '©12gdja12' not between '©11.1a' and 15.4;
select B'001' not between 0.1 and true;
select 2022-06-07 not between 2022-06-06 and 2022-06-10;
select b'1111111111111111111111111' not between 0 and 999999999;
select 0 not between '测' and '15.2';
select 1 not between '测1' and '1';
select 1 not between '1测' and '1';
select '2023-01-01 12:30:00'::datetime not between '2023-06-28 11:30:30'::datetime and '2023-07-28 17:30:30'::datetime;
select '2023-01-01 00:00:00' not between date_add(now(),interval-30 minute) and now();
select '2023-01-01 12:30:00'::time not between '2023-06-28 11:30:30'::time and '2023-07-28 17:30:30'::time;
select '12:30:00'::time not between '11:30:30'::time and '2023-07-28 17:30:30'::time;
select '12:30:00' not between '12:30:30' and '17:30:30';
select 1 not between 0 and cast(18446744073709551615 as unsigned);
select 1 not between 0 and cast(18446744073709551615 as signed);
select cast(0 as unsigned) not between -1 and 1;
select cast(0 as unsigned) not between 0 and -1;
select 1.5::numeric not between 0::numeric and 2::numeric;
select 0.5::float not between 0::float and 2::float;
select b'11' not between 1::float and 2.5::decimal;
select 100 not between '-Inf'::float and '+Inf'::float;
select 2 not between symmetric 23 and 2;
select 2.1 not between symmetric 12.3 and 2.0;
select true not between symmetric true and false;
select 'abc' not between symmetric 'deg' and 'abc';
drop table if exists test_collate;
create table test_collate(c1 text, c2 text, c3 text)charset 'utf8' collate 'utf8_unicode_ci';
insert into test_collate values
('t','T','T'),
('T','t','t'),
('TT','tT','Tt');
select c1 not between c2 and c3 from test_collate ;
drop table test_collate;
drop table if exists test_nocollate;
create table test_nocollate(c1 text, c2 text, c3 text);
insert into test_nocollate values
('t','T','T'),
('T','t','t'),
('TT','tT','Tt');
select c1 not between c2 and c3 from test_nocollate ;
drop table test_nocollate;
drop schema db_not_between cascade;
reset current_schema;
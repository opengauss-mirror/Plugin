drop database if exists db_not_between;
create database db_not_between dbcompatibility 'B';
\c db_not_between
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
\c postgres
drop database if exists db_not_between;
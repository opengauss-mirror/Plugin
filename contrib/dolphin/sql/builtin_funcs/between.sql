drop database if exists db_between;
create database db_between dbcompatibility 'B';
\c db_between
select 2 between 2.4 and 23.3;
select 2 between 2 and 23.3;
select 2 between 1.2 and 23;
select 2 between '1.2' and 23.3;
select 2 between 1.2 and '23.3';
select 2 between '1' and 3;
select 2 between '1' and '23.3';
select '2' between '1a' and 3;
select 2.2 between 1 and 3;
select 2.2 between '1' and '3';
select 2.2 between '2.1' and '2.4';
select 2.2 between '2.1a' and '2.4';
select '2.2' between 2.1 and 4;
select '2.2' between '2.1' and 4;
select '2.2' between 2.1 and '4';
select '2.2' between 2.1 and '4.4';
select '2.2' between 2 and '4.4';
select '2.2' between 2.1 and '4';
select '2.5' between '2.4' and '23.3'; 
select '2.2a' between 2.1 and '4';
select 'a2.2a' between 2.1 and '4';
select '2.2a' between '2' and '4';
select '2.2a' between '2.1' and 4.4;
select '2.2a' between '2.1' and '4.4';
select 12.7 between '11.1a' and 13;
select 12.7 between '11.1a' and 13.3;
select 12.7 between '11@#&%*' and '19.9hd';
select 11.2 between '11.2a' and '11.2';
select 11.22 between '11.22' and '11.2a';
select 'a2.2a' between 2.1 and '4';
select 'a2.2a' between 0 and B'101';
select 'a2.2a' between 1 and B'101';
select '2.2' between 1 and B'101';
select 2.2 between 1 and B'101';
select B'001' between 1 and B'101';
select B'001' between 1 and 7.7;
select '©12gdja12' between '©11.1a' and '15.4';
select B'001' between 0.1 and true;
select B'001' between 0.1 and FALSE;
select 2022-06-07 11:30:12 between 2022-06-07 11:30:12 and 2022-06-07 11:40:12;
select 2022-06-07between 2022-06-06 and 2022-06-10;
select '@12gdja12' between 12 and 15;
select '%&*(@12gdja12' between 12.1 and 15;
select '%&*(@12gdja12' between 12.1 and 15.4;
select '%&*(@12gdja12' between '12.1' and 15.4;
select '%&*(@12gdja12' between '12.1' and '15.4';
select '%&*(@12gdja12' between '12.1a' and '15.4';
select '%&*(@12gdja12' between '12.1a' and 'a15.4';
select '%&*(@12gdja12' between '%&*11.1a' and '15.4';
select '12%&*(@12gdja12' between '11.1a' and '15.4';
select '12%&*(@12gdja12' between 'a11.1a' and '15.4';
select '^%^&12%&*(@12gdja12' between 'a11.1a' and '^%^&15.4a';
select '11.2' between '11.1' and '15.4';
select '11.2' between 'ggg11.1' and '15.4';
select 11.2 between 'ggg11.1' and 'ggg15.4';
select 11.2 between '11.2a' and '11.2';
select '11.2' between E'11.2\'a' and '11.2';
select b'1111111111111111111111111' between 0 and 999999999;
\c postgres
drop database if exists db_between;
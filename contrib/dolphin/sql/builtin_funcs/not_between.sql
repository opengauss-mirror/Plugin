drop database if exists db_not_between;
create database db_not_between dbcompatibility 'B';
\c db_not_between

select 1 not between 2.4 and 23.3;
select 1.2 not between 2 and 23.3;
select 0.2 not between 1.2 and 23;
select 0.2 not between '1.2' and 23.3;
select 0.2 not between 1.2 and '23.3';
select 0.2 not between '1' and 3;
select 0.2 not between '1' and '23.3';

select '0.2' not between '1a' and 3;

select 0.2 not between 1 and 3;
select 0.2 not between '1' and '3';
select 0.2 not between '2.1' and '2.4';
select 0.2 not between '2.1a' and '2.4';

select '0.2' not between 2.1 and 4;
select '0.2' not between '2.1' and 4;
select '0.2' not between 2.1 and '4';
select '0.2' not between 2.1 and '4.4';
select '0.2' not between 2 and '4.4';
select '0.2' not between 2.1 and '4';
select '0.5' not between '2.4' and '23.3'; 

select '0.2a' not between 2.1 and '4';
select 'a0.2a' not between 2.1 and '4';
select '0.2a' not between '2' and '4';
select '0.2a' not between '2.1' and 4.4;
select '0.2a' not between '2.1' and '4.4';

select 12.7 not between '11.1a' and 13;
select 12.7 not between '11.1a' and 13.3;
select 12.7 not between '11@#&%*' and '19.9hd';
select 11.2 not between '11.2a' and '11.2';
select 11.22 not between '11.22' and '11.2a';
select 'a2.2a' not between 2.1 and '4';
select 'a2.2a' not between 0 and B'101';
select 'a2.2a' not between 1 and B'101';
select '2.2' not between 1 and B'101';
select 2.2 not between 1 and B'101';
select B'001' not between 1 and B'101';
select B'001' not between 1 and 7.7;
select '©12gdja12' not between '©11.1a' and '15.4';
select B'001' not between 0.1 and true;
select B'001' not between 0.1 and FALSE;
select 2022-06-07 11:30:12 not between 2022-06-07 11:30:12 and 2022-06-07 11:40:12;
select 2022-06-07not between 2022-06-06 and 2022-06-10;
select '@12gdja12' not between 12 and 15;
select '%&*(@12gdja12' not between 12.1 and 15;
select '%&*(@12gdja12' not between 12.1 and 15.4;
select '%&*(@12gdja12' not between '12.1' and 15.4;
select '%&*(@12gdja12' not between '12.1' and '15.4';
select '%&*(@12gdja12' not between '12.1a' and '15.4';
select '%&*(@12gdja12' not between '12.1a' and 'a15.4';
select '%&*(@12gdja12' not between '%&*11.1a' and '15.4';
select '12%&*(@12gdja12' not between '11.1a' and '15.4';
select '12%&*(@12gdja12' not between 'a11.1a' and '15.4';
select '^%^&12%&*(@12gdja12' not between 'a11.1a' and '^%^&15.4a';
select '11.2' not between '11.1' and '15.4';
select '11.2' not between 'ggg11.1' and '15.4';
select 11.2 not between 'ggg11.1' and 'ggg15.4';
select 11.2 not between '11.2a' and '11.2';
select '11.2' not between E'11.2\'a' and '11.2';
\c postgres
drop database if exists db_not_between;
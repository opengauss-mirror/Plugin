drop database if exists format_test;
create database format_test dbcompatibility 'B';
\c format_test

select field(4,1,2,3,4);
select field(2,1.1,2.1,3.1);
select field(2,'1','2','3','4');
select field(3.1,1,2,3,4);
select field('4','1','2','3','4');
select field('2',1,2,3,4);
select field('2',1.1,1.2,1.3);
select field('a','b','c','a');
select field(4.4,1,3,5,4);
select field(4,1,2,3,4);
select field(1,'a','b','1');
select field('a','b',1,'a');
select field(1,2,1.2);
\c postgres
drop database if exists format_test;

drop database if exists db_field;
create database db_field dbcompatibility 'B';
\c db_field

select field(4,1,2,3,4);
select field(2,1.1,2.1,3.1);
select field(2,'1','2','3','4');
select field(3.1,1,2,3,4);
select field('2a','1','2','3','4');
select field('2a',1,'2','3','4');
select field(1,'1a', 2);
select field('ceshi.000','wo','ceshi','disange');
select field(true,'1a', 2);
select field(true,1, 2);
select field(true);
select field('sdhfgs','dhgf', 2);
select field('sdhfgs','dhgf', '2');
select field('sdhfgs',null,1,'dhgf', '2');
select field('测试',null,1,'dhgf', '2');
select field(' ','@',null,' ','',' ');
\c postgres
drop database if exists db_field;

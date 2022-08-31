drop database if exists db_elt;
create database db_elt dbcompatibility 'B';
\c db_elt
select elt(1,1);
select elt(-1,1);
select elt(1.2,'a');
select elt(1,1.2);
select elt(1,'1.2');
select elt('1a','1.2a');
select elt('1.6a','1.2a',23);
select elt('1.6测','1.2a',23);
select elt('1.6测','1.2a测试',23);
select elt(9,'1.2a测试',23);
select elt(-1,'1.2a测试',23);
select elt(2,null,'1.2a测试',23);
select elt(1,null,'1.2a测试',23);
select elt(true,12,null,'1.2a测试',23);
select elt('1',1);
select elt('1',1.2);
select elt(1,'a','b');
select elt(1,'a',2);
\c postgres
drop database if exists db_elt;

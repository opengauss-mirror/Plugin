drop database if exists db_elt;
create database db_elt dbcompatibility 'B';
\c db_elt
select elt(1,1);
select elt(-1,1);
select elt(1.2,'a');
select elt(1,1.2);
select elt(1,'1.2');
select elt('1',1);
select elt('1',1.2);
select elt('1','a');
select elt('1','a');
select elt(1,'a');
select elt(1,'a','b');
select elt(1,'a',2);
select elt(2,'a',2);

\c postgres
drop database if exists db_elt;

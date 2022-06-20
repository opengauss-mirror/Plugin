drop database if exists format_test;
create database format_test dbcompatibility 'B';
\c format_test
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
drop database if exists format_test;

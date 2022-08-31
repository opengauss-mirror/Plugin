drop database if exists db_space;
create database db_space dbcompatibility 'B';
\c db_space
select space('a');
select space(10);
select space(-1);
select space('0');
select space('11a');
select space('11.6a');
select space(11.6);
select space(true);
select space(1,2,3);
select space(2147483647111);
\c postgres
drop database if exists db_space;

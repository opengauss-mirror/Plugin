drop database if exists db_space;
create database db_space dbcompatibility 'B';
\c db_space

select space('a');
select space(10);
select space(-1);
select space('0');
select space('-1');
\c postgres
drop database if exists db_space;

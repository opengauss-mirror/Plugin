drop database if exists format_test;
create database format_test dbcompatibility 'B';
\c format_test

select space('a');
select space(10);
select space(-1);
select space('0');
select space('-1');
\c postgres
drop database if exists format_test;

drop database if exists format_test;
create database format_test dbcompatibility 'B';
\c format_test

select bin(1);
select bin(0);
select bin('0');
select bin('a');
select bin(1.1);
select bin(null);
select bin(true);
select bin(false);

\c postgres
drop database if exists format_test;

drop database if exists db_bin;
create database db_bin dbcompatibility 'B';
\c db_bin

select bin(1);
select bin(0);
select bin('0');
select bin('a');
select bin(1.1);
select bin(null);
select bin(true);
select bin(false);

\c postgres
drop database if exists db_bin;

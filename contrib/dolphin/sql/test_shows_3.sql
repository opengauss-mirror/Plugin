drop database if exists db_show_3;
create database db_show_3 dbcompatibility 'b';
\c db_show_3
show databases;
create schema aa1;
create schema aa2;
create schema aa3;
show databases like 'aa%';
create schema abb1;
create schema abb2;
create schema abb3;
show databases like '%ab%';
show databases where database = 'abb2';
show schemas where Database = 'abb2';
\c postgres
drop database if exists db_show_3;

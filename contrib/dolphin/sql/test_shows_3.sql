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
create user u1 with password 'abc@1234';
create user u2 with password 'abc@1234';
set role u1 password 'abc@1234';
show databases;
set role u2 password 'abc@1234';
show databases;
\c postgres
drop database if exists db_show_3;
drop user u1;
drop user u2;

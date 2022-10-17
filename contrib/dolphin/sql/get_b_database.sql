drop database if exists get_db;
create database get_db dbcompatibility 'b';
\c get_db
select database();
create schema testdb;
use testdb;
select database();
create schema testdb1;
select database();
use testdb1;
select database();
\c postgres
drop database if exists get_db;

drop database if exists mysql;
create database mysql dbcompatibility 'b';
\c mysql
create extension dolphin;
select regexp('a', true);

\c postgres
drop database if exists mysql;
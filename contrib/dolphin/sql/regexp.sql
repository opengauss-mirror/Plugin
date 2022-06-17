drop database if exists mysql;
create database mysql dbcompatibility 'b';
\c mysql
select regexp('a', true);

\c postgres
drop database if exists mysql;
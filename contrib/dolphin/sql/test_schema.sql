drop database if exists schema_test;
create database schema_test dbcompatibility 'b';
\c schema_test

SELECT SCHEMA();

\c postgres
drop database if exists schema_test;

drop database if exists test;
create database test dbcompatibility 'B';
\c test
DROP TABLE IF EXISTS fixed_test;
CREATE TABLE fixed_test (a fixed(10, 5));
\d fixed_test
DROP TABLE fixed_test;
\c postgres
drop database test;
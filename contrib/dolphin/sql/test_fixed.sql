drop database if exists test_fixed;
create database test_fixed dbcompatibility 'B';
\c test_fixed
DROP TABLE IF EXISTS fixed_test;
CREATE TABLE fixed_test (a fixed(10, 5));
\d fixed_test
DROP TABLE fixed_test;
\c postgres
drop database test_fixed;
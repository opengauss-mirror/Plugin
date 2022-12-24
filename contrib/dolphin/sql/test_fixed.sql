create schema test_fixed;
set current_schema to 'test_fixed';
DROP TABLE IF EXISTS fixed_test;
CREATE TABLE fixed_test (a fixed(10, 5));
\d fixed_test
DROP TABLE fixed_test;
drop schema test_fixed cascade;
reset current_schema;
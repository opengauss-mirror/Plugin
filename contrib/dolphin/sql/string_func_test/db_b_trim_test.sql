set dolphin.b_compatibility_mode to off;
drop database if exists db_b_trim_test;
create database db_b_trim_test dbcompatibility 'A';
\c db_b_trim_test

SELECT TRIM('  bar   ');
SELECT TRIM(LEADING 'x' FROM 'xxxbarxxx');
SELECT TRIM(BOTH 'x' FROM 'xxxbarxxx');
SELECT TRIM(TRAILING 'x' FROM 'xxxbarxxx');
SELECT TRIM(LEADING 'xyz' FROM 'xyzxbarxxyz');
SELECT TRIM(BOTH 'xyz' FROM 'xyzxbarxxyz');
SELECT TRIM(TRAILING 'xyz' FROM 'xyzxbarxxyz');

\c contrib_regression
set dolphin.b_compatibility_mode to off;
drop database db_b_trim_test;
set dolphin.b_compatibility_mode to on;

create schema db_b_trim_test;
set current_schema to 'db_b_trim_test';

SELECT TRIM('  bar   ');
SELECT TRIM(LEADING 'x' FROM 'xxxbarxxx');
SELECT TRIM(BOTH 'x' FROM 'xxxbarxxx');
SELECT TRIM(TRAILING 'x' FROM 'xxxbarxxx');
SELECT TRIM(LEADING 'xyz' FROM 'xyzxbarxxyz');
SELECT TRIM(BOTH 'xyz' FROM 'xyzxbarxxyz');
SELECT TRIM(TRAILING 'xyz' FROM 'xyzxbarxxyz');

set bytea_output to escape;

SELECT TRIM(LEADING 'x'::bytea FROM 'xxxbarxxx'::bytea);
SELECT TRIM(BOTH 'x'::bytea FROM 'xxxbarxxx'::bytea);
SELECT TRIM(TRAILING 'x'::bytea FROM 'xxxbarxxx'::bytea);
SELECT TRIM(LEADING 'xyz'::bytea FROM 'xyzxbarxxyz'::bytea);
SELECT TRIM(BOTH 'xyz'::bytea FROM 'xyzxbarxxyz'::bytea);
SELECT TRIM(TRAILING 'xyz'::bytea FROM 'xyzxbarxxyz'::bytea);

SELECT TRIM('   X  '::bytea);
SELECT TRIM(LEADING '   X  '::bytea);
SELECT TRIM(TRAILING '   X  '::bytea);

drop schema db_b_trim_test cascade;
reset current_schema;
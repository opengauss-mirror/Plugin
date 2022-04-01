drop database if exists format_test;
create database format_test dbcompatibility 'B';
\c format_test
create extension b_sql_plugin;
-- test for b_format_mode = false
select format(1234.456, 2);
select format(1234.456, 2, 'en_US');
select format('%1s!', 'Hello');
select format('%1s %1s!', 'Hello', 'world');

-- test for b_format_mode = true
set b_format_mode = 1;

-- test for basic functionality of FORMAT
select format(234.567, 3);
select format(234.567, 2);
select format(234.567, 1);
select format(234.567, 8);
select format(234.567, 0);

-- test for carry
select format(999.999, 3);
select format(999.999, 2);
select format(999.999, 0);

-- test for thousand separator
select format(1234.45, 0);
select format(1234567.89, 1);

-- test for different locale
select format(1234.45, 2, 'en_US');
select format(1234.45, 2, 'de_DE');
select format(1234.45, 2, 'fr_FR');

-- test for type check
select format(1234.456, 2.49);
select format(1234.456, 2.99);

-- wrong type of first param
select format('abcde', 2);
-- non-existent locale input
select format(1234.456, 2, 'zz_AA');
-- wrong type of third param
select format(1234.456, 2, 123123);

-- test for setting b_format_mode back to false
set b_format_mode = 0;
select format(1234.456, 2);
select format(1234.456, 2, 'en_US');
select format('%1s!', 'Hello');
select format('%1s %1s!', 'Hello', 'world');

\c postgres
drop database format_test;

-- test for A compatibility to ensure the original functionality is good.
create database db_a_format_test dbcompatibility 'A';
\c db_a_format_test
select format(1234.456, 2);
select format(1234.456, 2, 'en_US');
\c postgres
drop database db_a_format_test;
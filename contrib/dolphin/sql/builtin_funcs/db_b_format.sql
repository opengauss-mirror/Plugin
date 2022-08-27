drop database if exists db_db_b_format;
create database db_db_b_format dbcompatibility 'B';
\c db_db_b_format
-- test for b_compatibility_mode = false
select format(1234.456, 2);
select format(1234.456, 2, 'en_US');
select format('%1s!', 'Hello');
select format('%1s %1s!', 'Hello', 'world');

-- test for b_compatibility_mode = true
set b_compatibility_mode = 1;

-- test for basic functionality of FORMAT
select format(234.567, 3);
select format(234.567, 2);
select format(234.567, 1);
select format(234.567, 8);
select format(234.567, 0);

-- test for float number started with 0, like 0.123456
select format(0.123,2);
select format(5/7,2);

-- test for enormous number
select format(132454676878465464652222238888856744654563565446554798798451344787945.3153,2);

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

-- test for issue I50Z8L
select format(1123.456,2.5,2.52);
select format(1213.456,1,0);
select format(1213.456,1,2);
select format(1123.456,1,-2);
select format(1123.456,4,2/3);
select format(1213.456,'2','3');
select format(1123.456,'2','5.5');
select format(1123.456,'2','3/4');
select format(1123.456,'2','zxc');
select format(1123.456,'2','ASD');
select format(1123.456,'2','ADxc');
select format(12345.6789,3,'de_DE');
select format(1123.456,'2','2022-12-12');
select format(1123.456,'2',2022-12-12);
select format(1123.456,'2','2013-10-10 00:00:00');
select format(12345.6789,3,54563463374648346543156435468225862565364364356435436546345635436587463);

-- test for cstring input
select format('1.23456',2);

-- test for negative input
select format('-123',2);
select format('--123',2);
select format('-123.456',2);
select format('-1234.456',2);
select format('-1234564564868789790867567.456',2);

-- test for cstring input starting with '+'
select format(+1.456,2);
select format('+1.456',2);
select format(+123.456,2);
select format('+123.456',2);
select format('++123.456',2);

-- test for invalid first param input
select format('abc.12345',2);
select format('abc1234',2);
select format('-abc1234',2);
select format('.ab',2);
select format('...aaab',2);
select format('.12345',2);

-- test for truncated input
select format('-123abc456.4556',2);
select format('-123abc456.4556',2);
select format('-123abc456.4556',6);
select format('123465#$%^.123^&2',6);
select format('123.4 56',2);

-- test for trim
select format('  1234.456',2);
select format('  1234.456    ',2);
select format('  1234.456    ',8);
select format('  1234.4  56    ',8);
select format('    abcc',2);
select format('    abcc   ',2);
select format('    abcc   ',2);
select format('       ',2);

-- test for null params
select format(null, 1);
select format(1, null);
select format(1, 1, null);
select format(null, null, 1);
select format(null, null, null);

-- test for setting b_compatibility_mode back to false
set b_compatibility_mode = 0;
select format(1234.456, 2);
select format(1234.456, 2, 'en_US');
select format('%1s!', 'Hello');
select format('%1s %1s!', 'Hello', 'world');

-- test for function calls that parameters are more than 3.
select format('%s, %s, %s', 1, 1, 1);
select format('%s, %s, %s, %s',1, 1, 1, 1);
select format('%s, %s, %s, %s, %s',1 ,1 ,1 ,1, 1);
select format('%s, %s, %s, %s, %s, %s',1 ,1 ,1 ,1, 1, 1);

-- test for original function format with variadic array grammar.
select format('%s, %s', variadic array['Hello','World']);
select format('%s, %s', variadic array[1, 2]);
select format('%s, %s', variadic array[true, false]);
select format('%s, %s', variadic array[true, false]::text[]);
select format('%2$s, %1$s', variadic array['first', 'second']);
select format('%2$s, %1$s', variadic array[1, 2]);

\c postgres
drop database db_db_b_format;

-- test for A compatibility to ensure the original functionality is good.
create database db_db_b_format dbcompatibility 'A';
\c db_db_b_format
select format(1234.456, 2);
select format(1234.456, 2, 'en_US');
\c postgres
drop database db_db_b_format;
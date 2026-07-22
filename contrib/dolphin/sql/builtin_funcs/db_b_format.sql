create schema db_db_b_format;
set current_schema to 'db_db_b_format';
-- test for b_compatibility_mode = false
set dolphin.b_compatibility_mode to off;
select format(1234.456, 2);
select format(1234.456, 2, 'en_US');
select format('%1s!', 'Hello');
select format('%1s %1s!', 'Hello', 'world');

-- test for b_compatibility_mode = true
set dolphin.b_compatibility_mode = 1;

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

-- test overflow
select format(123123.456,100000000000);
select format(123123.456,1000000000000);
select format(123123.456,10000000000000);
select format(123123.456,545634633746483465436546345635436587463);
select format(123123.456,'513513413546587468514351345435413');

select format(123123.456,100000000000,'de_DE');
select format(123123.456,1000000000000,'de_DE');
select format(123123.456,10000000000000,'de_DE');
select format(123123.456,545634633746483465436546345635436587463,'de_DE');
select format(123123.456,'513513413546587468514351345435413','de_DE');

-- test uuid-like varchar inputs follow MySQL-style string numeric conversion
select pg_typeof(uuid());
set client_min_messages to error;
with cases(id, case_group, result, expected) as (
    values
    (1, 'same_uuid_digit_540',
        format('540c08fe-d862-11cf-f278-f82e3f2348c4'::varchar, '540c08fe-d862-11cf-f278-f82e3f2348c4'::varchar),
        '540.00000000000000000000000000000000'),
    (2, 'digit_540_precision_32',
        format('540c08fe-d862-11cf-f278-f82e3f2348c4'::varchar, '32'::varchar),
        '540.00000000000000000000000000000000'),
    (3, 'digit_540_precision_0b',
        format('540c08fe-d862-11cf-f278-f82e3f2348c4'::varchar, '0b8870ca-dc88-11cf-681d-78b46a3ffcce'::varchar),
        '540'),
    (4, 'numeric_uuid_0b_first',
        format('0b8870ca-dc88-11cf-681d-78b46a3ffcce'::varchar, '2'::varchar),
        '0.00'),
    (5, 'numeric_uuid_123_first',
        format('12345678-dc88-11cf-681d-78b46a3ffcce'::varchar, '2'::varchar),
        '12,345,678.00'),
    (6, 'lower_a_uuid_first',
        format('a3c80160-dc89-11cf-658e-78b46a3ffcce'::varchar, '2'::varchar),
        '0.00'),
    (7, 'lower_e_uuid_first',
        format('e3c80160-dc89-11cf-658e-78b46a3ffcce'::varchar, '2'::varchar),
        '0.00'),
    (8, 'upper_E_uuid_first',
        format('E3c80160-dc89-11cf-658e-78b46a3ffcce'::varchar, '2'::varchar),
        '0.00'),
    (9, 'digit_e_digit_first',
        format('1e2-dc88-11cf-681d-78b46a3ffcce'::varchar, '2'::varchar),
        '100.00'),
    (10, 'digit_E_digit_first',
        format('1E2-dc88-11cf-681d-78b46a3ffcce'::varchar, '2'::varchar),
        '100.00'),
    (11, 'digit_e_letter_first',
        format('1ea3-dc88-11cf-681d-78b46a3ffcce'::varchar, '2'::varchar),
        '1.00'),
    (12, 'digit_E_letter_first',
        format('1Ea3-dc88-11cf-681d-78b46a3ffcce'::varchar, '2'::varchar),
        '1.00'),
    (13, 'digit_e_digit_second',
        format('123.456'::varchar, '1e2-dc88-11cf-681d-78b46a3ffcce'::varchar),
        '123.5'),
    (14, 'digit_e_letter_second',
        format('123.456'::varchar, '1ea3-dc88-11cf-681d-78b46a3ffcce'::varchar),
        '123.5'),
    (15, 'numeric_uuid_2b_second',
        format('123.456'::varchar, '2b8870ca-dc88-11cf-681d-78b46a3ffcce'::varchar),
        '123.46'),
    (16, 'alpha_uuid_second',
        format('123.456'::varchar, 'a3c80160-dc89-11cf-658e-78b46a3ffcce'::varchar),
        '123'),
    (17, 'digit_540_precision_2e1',
        format('540c08fe-d862-11cf-f278-f82e3f2348c4'::varchar, '2e1b70ca-dc88-11cf-681d-78b46a3ffcce'::varchar),
        '540.00'),
    (18, 'digit_540_precision_2E1',
        format('540c08fe-d862-11cf-f278-f82e3f2348c4'::varchar, '2E1b70ca-dc88-11cf-681d-78b46a3ffcce'::varchar),
        '540.00'),
    (19, 'leading_space_digit_first',
        format('  540c08fe-d862-11cf-f278-f82e3f2348c4'::varchar, '2'::varchar),
        '540.00'),
    (20, 'plus_digit_first',
        format('+540c08fe-d862-11cf-f278-f82e3f2348c4'::varchar, '2'::varchar),
        '540.00'),
    (21, 'minus_digit_first',
        format('-540c08fe-d862-11cf-f278-f82e3f2348c4'::varchar, '2'::varchar),
        '-540.00'),
    (22, 'decimal_digit_first',
        format('.540c08fe-d862-11cf-f278-f82e3f2348c4'::varchar, '2'::varchar),
        '0.54'),
    (23, 'space_plus_precision',
        format('540c08fe-d862-11cf-f278-f82e3f2348c4'::varchar, '  +2b8870ca-dc88-11cf-681d-78b46a3ffcce'::varchar),
        '540.00'),
    (24, 'leading_zero_digits_first',
        format('054b8870ca-dc88-11cf-681d-78b46a3ffcce'::varchar, '2'::varchar),
        '54.00'),
    (25, 'leading_multi_zero_digits_first',
        format('000123b70ca-dc88-11cf-681d-78b46a3ffcce'::varchar, '2'::varchar),
        '123.00'),
    (26, 'negative_leading_zero_digits_first',
        format('-00054b70ca-dc88-11cf-681d-78b46a3ffcce'::varchar, '2'::varchar),
        '-54.00'),
    (27, 'leading_zero_decimal_first',
        format('000.540b70ca-dc88-11cf-681d-78b46a3ffcce'::varchar, '2'::varchar),
        '0.54'),
    (28, 'negative_leading_zero_decimal_first',
        format('-000.540b70ca-dc88-11cf-681d-78b46a3ffcce'::varchar, '2'::varchar),
        '-0.54'),
    (29, 'all_zero_digits_first',
        format('0000b70ca-dc88-11cf-681d-78b46a3ffcce'::varchar, '2'::varchar),
        '0.00')
)
select id, case_group, result, expected,
       case when result = expected then 'OK' else 'MISMATCH' end as status
from cases
order by id;
select pg_typeof(format(uuid(), uuid()));

-- test scientific notation prefix in string numeric conversion
with cases(id, case_group, result, expected) as (
    values
    (1, 'scientific_e_digit_first',
        format('540e02fe-d862-11cf-f278-f82e3f2348c4', '3e2'),
        '54,000.000'),
    (2, 'scientific_e_zero_letter_first',
        format('540e0afe-d862-11cf-f278-f82e3f2348c4', '3e2'),
        '540.000'),
    (3, 'scientific_e_letter_first',
        format('540eaafe-d862-11cf-f278-f82e3f2348c4', '3e2'),
        '540.000'),
    (4, 'scientific_e_empty_first',
        format('540e', '3e2'),
        '540.000'),
    (5, 'scientific_e_plus_digit_first',
        format('540e+2afe-d862-11cf-f278-f82e3f2348c4', '3e2'),
        '54,000.000'),
    (6, 'scientific_e_minus_digit_first',
        format('540e-2afe-d862-11cf-f278-f82e3f2348c4', '3e2'),
        '5.400'),
    (7, 'scientific_upper_e_digit_first',
        format('540E02fe-d862-11cf-f278-f82e3f2348c4', '3e2'),
        '54,000.000'),
    (8, 'scientific_e_single_digit_exp',
        format('145e1aafe-d862-11cf-f278-f82e3f2348c4', '3e2'),
        '1,450.000'),
    (9, 'scientific_e_zero_padded_exp',
        format('145e001aafe-d862-11cf-f278-f82e3f2348c4', '3e2'),
        '1,450.000'),
    (10, 'scientific_e_letter_exp',
        format('145eabb1aafe-d862-11cf-f278-f82e3f2348c4', '3e2'),
        '145.000'),
    (11, 'scientific_precision_exp',
        format('145eabb1aafe-d862-11cf-f278-f82e3f2348c4', '10e2'),
        '145.0000000000'),
    (12, 'scientific_precision_dash',
        format('145eabb1aafe-d862-11cf-f278-f82e3f2348c4', '1-12'),
        '145.0'),
    (13, 'scientific_upper_e_zero_padded_exp',
        format('145E001aafe-d862-11cf-f278-f82e3f2348c4', '3e2'),
        '1,450.000'),
    (14, 'scientific_upper_e_letter_exp',
        format('145Eabb1aafe-d862-11cf-f278-f82e3f2348c4', '3e2'),
        '145.000')
)
select id, case_group, result, expected,
       case when result = expected then 'OK' else 'MISMATCH' end as status
from cases
order by id;
select 'overflow_scientific_value' as case_group,
       left(format('1e100000-dc88-11cf-681d-78b46a3ffcce'::varchar, '2'::varchar), 7) as result_prefix,
       right(format('1e100000-dc88-11cf-681d-78b46a3ffcce'::varchar, '2'::varchar), 3) as result_suffix,
       length(format('1e100000-dc88-11cf-681d-78b46a3ffcce'::varchar, '2'::varchar)) < 500 as bounded_result;
select 'underflow_scientific_value' as case_group,
       format('1e-100000-dc88-11cf-681d-78b46a3ffcce'::varchar, '2'::varchar) as result;
select 'overflow_scientific_precision' as case_group,
       format('123.456'::varchar, '1e100000-dc88-11cf-681d-78b46a3ffcce'::varchar) as result;
select 'negative_overflow_scientific_value' as case_group,
       left(format('-1e100000-dc88-11cf-681d-78b46a3ffcce'::varchar, '2'::varchar), 8) as result_prefix,
       right(format('-1e100000-dc88-11cf-681d-78b46a3ffcce'::varchar, '2'::varchar), 3) as result_suffix,
       length(format('-1e100000-dc88-11cf-681d-78b46a3ffcce'::varchar, '2'::varchar)) < 500 as bounded_result;
select 'invalid_scientific_value' as case_group,
       format('1e+-100000-dc88-11cf-681d-78b46a3ffcce'::varchar, '2'::varchar) as result;
select 'negative_underflow_scientific_value' as case_group,
       format('-1e-100000-dc88-11cf-681d-78b46a3ffcce'::varchar, '2'::varchar) as result;
reset client_min_messages;
-- test for setting b_compatibility_mode back to false
set dolphin.b_compatibility_mode = 0;
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

drop schema db_db_b_format cascade;
reset current_schema;

-- test for A compatibility to ensure the original functionality is good.
create database db_db_b_format dbcompatibility 'A';
\c db_db_b_format
select format(1234.456, 2);
select format(1234.456, 2, 'en_US');
\c postgres
drop database db_db_b_format;
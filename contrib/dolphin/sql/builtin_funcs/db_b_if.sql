drop database if exists format_test;
create database format_test dbcompatibility 'B';
\c format_test

select if(TRUE, 1, 2);
select if(FALSE, 1, 2);
-- wrong type for first param
select if('abc', 1, 2);

-- '2022-01-30' is text, date '2022-01-30' is date
CREATE VIEW test_view as select '2022-01-30' as text_type, date '2022-01-30' as date_type;
\d+ test_view

-- string to date
select if (true, '2022-01-30', date '2022-01-30') as a, if (false, '2022-01-30', date '2022-01-30') as b;

-- string to numeric
select if (true, '2022-01-30', 1.1::numeric(10, 2)) as a, if (false, '2022-01-30', 1.1::numeric(10, 2)) as b;

-- string to float
select if (true, '2022-01-30', 1.1::float8) as a, if (false, '2022-01-30', 1.1::float8) as b;

-- string to integer
select if (true, '2022-01-30', 1::int4) as a, if (false, '2022-01-30', 1::int4) as b;

-- date to numeric
select if (true, date '2022-01-30', 1.1::numeric(10, 2)) as a, if (false, date '2022-01-30', 1.1::numeric(10, 2)) as b;

-- date to float
select if (true, date '2022-01-30', 1.1::float8) as a, if (false, date '2022-01-30', 1.1::float8) as b;

-- date to integer
select if (true, date '2022-01-30', 1::int4) as a, if (false, date '2022-01-30', 1::int4) as b;

-- numeric to float
select if (true, 2.2::numeric(10, 2), 1.1::float8) as a, if (false, 2.2::numeric(10, 2), 1.1::float8) as b;

-- numeric to integer
select if (true, 2.2::numeric(10, 2), 1::int4) as a, if (false, 2.2::numeric(10, 2), 1::int4) as b;

-- float to integer
select if (true, 2.2::float8, 1::int4) as a, if (false, 2.2::float8, 1::int4) as b;

-- TODO
-- binary to date
select if (true, 'aaa'::binary(5), date '2022-01-30') as a, if (false, 'aaa'::binary(5), date '2022-01-30') as b;
-- binary to numeric
select if (true, 'aaa'::binary(5), 1.1::numeric(10, 2)) as a, if (false, 'aaa'::binary(5), 1.1::numeric(10, 2)) as b;
-- binary to float
select if (true, 'aaa'::binary(5), 1.1::float8) as a, if (false, 'aaa'::binary(5), 1.1::float8) as b;
-- binary to integer
select if (true, 'aaa'::binary(5), 1::int4) as a, if (false, 'aaa'::binary(5), 1::int4) as b;
-- varbinary to date
select if (true, 'aaa'::varbinary(5), date '2022-01-30') as a, if (false, 'aaa'::varbinary(5), date '2022-01-30') as b;
-- varbinary to numeric
select if (true, 'aaa'::varbinary(5), 1.1::numeric(10, 2)) as a, if (false, 'aaa'::varbinary(5), 1.1::numeric(10, 2)) as b;
-- varbinary to float
select if (true, 'aaa'::varbinary(5), 1.1::float8) as a, if (false, 'aaa'::varbinary(5), 1.1::float8) as b;
-- varbinary to integer
select if (true, 'aaa'::varbinary(5), 1::int4) as a, if (false, 'aaa'::varbinary(5), 1::int4) as b;
-- blob to date
select if (true, 'aaa'::blob, date '2022-01-30') as a, if (false, 'aaa'::blob, date '2022-01-30') as b;
-- blob to numeric
select if (true, 'aaa'::blob, 1.1::numeric(10, 2)) as a, if (false, 'aaa'::blob, 1.1::numeric(10, 2)) as b;
-- blob to float
select if (true, 'aaa'::blob, 1.1::float8) as a, if (false, 'aaa'::blob, 1.1::float8) as b;
-- blob to integer
select if (true, 'aaa'::blob, 1::int4) as a, if (false, 'aaa'::blob, 1::int4) as b;

-- ERROR
-- string to boolean
select if (true, '2022-01-30', true) as a, if (false, '2022-01-30', true) as b;

-- float to boolean
select if (true, 1.1::float8, true) as a, if (false, 1.1::float8, true) as b;

-- numeric to boolean
select if (true, 2.2::numeric(10, 2), true) as a, if (false, 2.2::numeric(10, 2), true) as b;

\c postgres
drop database if exists format_test;
-- the test case of A format
drop database if exists test;
create database test dbcompatibility 'A';
\c test
SELECT 2.5::float4::int1;
SELECT 2.5::float8::int1;
SELECT 2.5::int1;
SELECT 2.5::float4::int2;
SELECT 2.5::float8::int2;
SELECT 2.5::int2;
SELECT 2.5::float4::int4;
SELECT 2.5::float8::int4;
SELECT 2.5::int4;
SELECT 2.5::float4::int8;
SELECT 2.5::float8::int8;
SELECT 2.5::int8;

SELECT 3.5::float4::int1;
SELECT 3.5::float8::int1;
SELECT 3.5::int1;
SELECT 3.5::float4::int2;
SELECT 3.5::float8::int2;
SELECT 3.5::int2;
SELECT 3.5::float4::int4;
SELECT 3.5::float8::int4;
SELECT 3.5::int4;
SELECT 3.5::float4::int8;
SELECT 3.5::float8::int8;
SELECT 3.5::int8;

SELECT dround(2.5);
SELECT dround(3.5);

\c contrib_regression
drop database test;

-- the test case of dolphin plugin
create schema test;
set current_schema to 'test';

SELECT 2.5::float4::int1;
SELECT 2.5::float8::int1;
SELECT 2.5::int1;
SELECT 2.5::float4::int2;
SELECT 2.5::float8::int2;
SELECT 2.5::int2;
SELECT 2.5::float4::int4;
SELECT 2.5::float8::int4;
SELECT 2.5::int4;
SELECT 2.5::float4::int8;
SELECT 2.5::float8::int8;
SELECT 2.5::int8;

SELECT 3.5::float4::int1;
SELECT 3.5::float8::int1;
SELECT 3.5::int1;
SELECT 3.5::float4::int2;
SELECT 3.5::float8::int2;
SELECT 3.5::int2;
SELECT 3.5::float4::int4;
SELECT 3.5::float8::int4;
SELECT 3.5::int4;
SELECT 3.5::float4::int8;
SELECT 3.5::float8::int8;
SELECT 3.5::int8;

SELECT dround(2.5);
SELECT dround(3.5);

set dolphin.sql_mode = '';

create table test_type_table_round
(
   `float4` float4,
   `float8` float8,
   `numeric` decimal(20, 6),
   `time(4)` time(4),
   `datetime(4)` datetime(4) default '2022-11-11 11:11:11',
   `timestamp(4)` timestamp(4) default '2022-11-11 11:11:11',
   `char` char(100),
   `varchar` varchar(100), 
   `binary` binary(100),
   `varbinary` varbinary(100),
   `tinyblob` tinyblob,
   `blob` blob,
   `mediumblob` mediumblob,
   `longblob` longblob,
   `text` text,
   `enum_t` enum('2.5', '3.5', '-2.5','-3.5'),
   `set_t` set('2.5', '3.5', '-2.5', '-3.5')
);

insert into test_type_table_round values (2.5, 2.5, 2.5, '19:10:52.5', '2023-02-05 19:10:52.5', '2023-02-05 19:10:52.5', '2.5', '2.5', '2.5', '2.5', '2.5', '2.5', '2.5', '2.5', '2.5', '2.5', '2.5');

insert into test_type_table_round values (-2.5, -2.5, -2.5, '19:10:52.5', '2023-02-05 19:10:52.5', '2023-02-05 19:10:52.5', '-2.5', '-2.5', '-2.5', '-2.5', '-2.5', '-2.5', '-2.5', '-2.5', '-2.5', '-2.5', '-2.5');

insert into test_type_table_round values (3.5, 3.5, 3.5, '19:10:53.5', '2023-02-05 19:10:53.5', '2023-02-05 19:10:53.5', '3.5', '3.5', '3.5', '3.5', '3.5', '3.5', '3.5', '3.5', '3.5', '3.5', '3.5');

insert into test_type_table_round values (-3.5, -3.5, -3.5, '19:10:53.5', '2023-02-05 19:10:53.5', '2023-02-05 19:10:53.5', '-3.5', '-3.5', '-3.5', '-3.5', '-3.5', '-3.5', '-3.5', '-3.5', '-3.5', '-3.5', '-3.5');

create table test_round1 as select round(`float4`) as round1, round(`float8`) as round2, round(`numeric`) as round3, round(`time(4)`) as round4, round(`datetime(4)`) as round5, round(`timestamp(4)`) as round6, round(`char`) as round7, round(`varchar`) as round8, round(`binary`) as round9,round(`varbinary`) as round10, round(`tinyblob`) as round11, round(`blob`) as round12, round(`mediumblob`) as round13, round(`longblob`) as round14, round(`text`) as round15, round(`enum_t`) as round16, round(`set_t`) as round17 from test_type_table_round;

select * from test_round1;

show create table test_round1;

drop table test_round1;
drop table test_type_table_round;

drop schema test cascade;
reset current_schema;

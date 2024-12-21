set dolphin.b_compatibility_mode to off;
drop database if exists db_b_log_test;
create database db_b_log_test dbcompatibility 'A';
\c db_b_log_test

SELECT LOG(10);

\c contrib_regression
set dolphin.b_compatibility_mode to off;
drop database db_b_log_test;
set dolphin.b_compatibility_mode to on;

create schema db_b_log_test;
set current_schema to 'db_b_log_test';

SELECT LOG(10);
SELECT LOG10(100);
SELECT LOG2(64);
SELECT log2(0.1);
SELECT log2(.1);

SELECT LOG(10::numeric);
SELECT LOG10(100::numeric);
SELECT LOG2(64::numeric);

SELECT LOG(0) IS NULL;
SELECT LOG10(0) IS NULL;
SELECT LOG2(0) IS NULL;

SELECT LOG(-1) IS NULL;
SELECT LOG10(-1) IS NULL;
SELECT LOG2(-1) IS NULL;

SELECT LOG(0::numeric) IS NULL;
SELECT LOG10(0::numeric) IS NULL;
SELECT LOG2(0::numeric) IS NULL;

SELECT LOG(-1::numeric) IS NULL;
SELECT LOG10(-1::numeric) IS NULL;
SELECT LOG2(-1::numeric) IS NULL;

SELECT LOG(2, 64) IS NULL;
SELECT LOG(-2, 64) IS NULL;
SELECT LOG(2, -64) IS NULL;
SELECT LOG(0, 64) IS NULL;
SELECT LOG(2, 0) IS NULL;

select log(111);
select log(b'111');
select log(7);
select log2(111);
select log2(b'111');
select log2(7);
select log10(111);
select log10(b'111');
select log10(7);

select log(b'111'::int);
select log2(b'111'::int);
select log10(b'111'::int);

set dolphin.sql_mode = '';

create table test_type_table
(
`int1` tinyint,
`uint1` tinyint unsigned,
`int2` smallint,
`uint2` smallint unsigned,
`int4` integer,
`uint4` integer unsigned,
`int8` bigint,
`uint8` bigint unsigned,
`float4` float4,
`float8` float8,
`numeric` decimal(20, 6),
`bit1` bit(1),
`bit64` bit(64),
`boolean` boolean,
`date` date,
`time` time,
`time(4)` time(4),
`datetime` datetime,
`datetime(4)` datetime(4) default '2022-11-11 11:11:11',
`timestamp` timestamp,
`timestamp(4)` timestamp(4) default '2022-11-11 11:11:11',
`year` year,
`char` char(100),
`varchar` varchar(100),
`binary` binary(100),
`varbinary` varbinary(100),
`tinyblob` tinyblob,
`blob` blob,
`mediumblob` mediumblob,
`longblob` longblob,
`text` text,
`enum_t` enum('a', 'b', 'c'),
`set_t` set('a', 'b', 'c'),
`json` json
);

insert into test_type_table values (1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, b'1', b'111', true,
'2023-02-05', '19:10:50', '19:10:50.3456', '2023-02-05 19:10:50',
'2023-02-05 19:10:50.456', '2023-02-05 19:10:50', '2023-02-05 19:10:50.456',
'2023', '1.23a', '1.23a', '1.23a', '1.23a', '1.23a', '1.23a', '1.23a', '1.23a', '1.23a',
'a', 'a,c', json_object('a', 1, 'b', 2));

create table test_log as select  log10(`int1`) as log1, log10(`uint1`) as log2, log10(`int2`) as log3,  log10(`uint2`) as log4,  log10(`int4`) as log5, log10(`uint4`) as log6,  
log10(`int8`) as log7,  log10(`uint8`) as log8, log10(`float4`) as log9, log10(`float8`) as log10, log10(`numeric`) as log11, log10(`bit1`) as log12,  log10(`bit64`) as log13,  
log10(`boolean`) as log14, log10(`char`) as log15,  log10(`varchar`) as log16,  log10(`binary`) as log17, log10(`varbinary`) as log18, log10(`text`) as log19, log10(`numeric`) 
as log20 from test_type_table;
\d test_log
drop table if exists test_log;

create table test_log as select  log2(`int1`) as log1, log2(`uint1`) as log2, log2(`int2`) as log3,  log2(`uint2`) as log4,  log2(`int4`) as log5, log2(`uint4`) as log6,  
log2(`int8`) as log7,  log2(`uint8`) as log8, log2(`float4`) as log9, log2(`float8`) as log10, log2(`numeric`) as log11, log2(`bit1`) as log12,  log2(`bit64`) as log13,  
log2(`boolean`) as log14, log2(`char`) as log15,  log2(`varchar`) as log16,  log2(`binary`) as log17, log2(`varbinary`) as log18, log2(`text`) as log19, log2(`numeric`) 
as log20 from test_type_table;
\d test_log
drop table if exists test_log;
drop table if exists test_type_table;
reset dolphin.sql_mode;

create table test_log(val double);
insert into test_log select log10(0);
insert into test_log select log10(-10);
insert into test_log select log2(0);
insert into test_log select log2(-10);
insert ignore into test_log select log10(0);
insert ignore into test_log select log10(-10);
insert ignore into test_log select log2(0);
insert ignore into test_log select log2(-10);
set dolphin.sql_mode = '';
insert into test_log select log10(0);
insert into test_log select log10(-10);
insert into test_log select log2(0);
insert into test_log select log2(-10);
select * from test_log;
reset dolphin.sql_mode;
drop table if exists test_log;
drop schema db_b_log_test cascade;
reset current_schema;

create schema test_math_func;
set current_schema to 'test_math_func';

set dolphin.b_compatibility_mode = on;

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

insert into test_type_table values (1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, b'1', b'111', true, '2023-02-05', '19:10:50', '19:10:50.3456', '2023-02-05 19:10:50', '2023-02-05 19:10:50.456', '2023-02-05 19:10:50', '2023-02-05 19:10:50.456',
'2023', '1.23a', '1.23a', '1.23a', '1.23a', '1.23a', '1.23a', '1.23a', '1.23a', '1.23a', 'a', 'a,c', json_object('a', 1, 'b', 2));


-- degrees math function
select degrees(`int1`), degrees(`uint1`), degrees(`int2`), degrees(`uint2`), degrees(`int4`), degrees(`uint4`), degrees(`int8`), degrees(`uint8`), degrees(`float4`), degrees(`float8`), degrees(`numeric`),degrees(`bit1`), degrees(`bit64`), degrees(`boolean`), degrees(`date`), degrees(`time`), degrees(`time(4)`), degrees(`datetime`),degrees(`datetime(4)`), degrees(`timestamp`), degrees(`timestamp(4)`), degrees(`year`), degrees(`char`), degrees(`varchar`), degrees(`binary`), degrees(`varbinary`), degrees(`tinyblob`), degrees(`blob`), degrees(`mediumblob`), degrees(`longblob`), degrees(`text`), degrees(`enum_t`), degrees(`set_t`), degrees(`json`) from test_type_table;   

create table test_double_degrees(d1 double,  d2 double,  d3 double,  d4 double,  d5 double,  d6 double,  d7 double,  d8 double,  d9 double,  d10 double,  d11 double,  d12 double,  d13 double,  d14 double,  d15 double,  d16 double,  d17 double,  d18 double,  d19 double,  d20 double,  d21 double,  d22 double,  d23 double,  d24 double,  d25 double,  d26 double,  d27 double,  d28 double,  d29 double,  d30 double,  d31 double,  d32 double,  d33 double,  d34 double);

insert ignore into test_double_degrees  select degrees(`int1`), degrees(`uint1`), degrees(`int2`), degrees(`uint2`), degrees(`int4`), degrees(`uint4`), degrees(`int8`), degrees(`uint8`), degrees(`float4`), degrees(`float8`), degrees(`numeric`),degrees(`bit1`), degrees(`bit64`),degrees(`boolean`), degrees(`date`), degrees(`time`), degrees(`time(4)`), degrees(`datetime`),degrees(`datetime(4)`), degrees(`timestamp`), degrees(`timestamp(4)`), degrees(`year`), degrees(`char`), degrees(`varchar`), degrees(`binary`),degrees(`varbinary`), degrees(`tinyblob`), degrees(`blob`), degrees(`mediumblob`), degrees(`longblob`), degrees(`text`), degrees(`enum_t`), degrees(`set_t`), degrees(`json`) from test_type_table;

set dolphin.sql_mode = 'sql_mode_full_group,pipes_as_concat,ansi_quotes';

insert into test_double_degrees  select degrees(`int1`), degrees(`uint1`), degrees(`int2`), degrees(`uint2`), degrees(`int4`), degrees(`uint4`), degrees(`int8`), degrees(`uint8`), degrees(`float4`), degrees(`float8`), degrees(`numeric`),degrees(`bit1`), degrees(`bit64`),degrees(`boolean`), degrees(`date`), degrees(`time`), degrees(`time(4)`), degrees(`datetime`),degrees(`datetime(4)`), degrees(`timestamp`), degrees(`timestamp(4)`), degrees(`year`), degrees(`char`), degrees(`varchar`), degrees(`binary`),degrees(`varbinary`), degrees(`tinyblob`), degrees(`blob`), degrees(`mediumblob`), degrees(`longblob`), degrees(`text`), degrees(`enum_t`), degrees(`set_t`), degrees(`json`) from test_type_table;

select * from test_double_degrees order by 1;

drop table if exists test_double_degrees;
drop table if exists test_type_table;

drop schema test_math_func cascade;
reset current_schema;
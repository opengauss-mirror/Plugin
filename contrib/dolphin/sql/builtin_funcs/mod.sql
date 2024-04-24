create schema db_test_mod;
set current_schema to 'db_test_mod';
set dolphin.sql_mode = '';
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

insert into test_type_table values (1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, b'1', b'111', true,
'2023-02-05', '19:10:50', '19:10:50.3456', '2023-02-05 19:10:50',
'2023-02-05 19:10:50.456', '2023-02-05 19:10:50', '2023-02-05 19:10:50.456',
'2023', '1.23a', '1.23a', '1.23a', '1.23a', '1.23a', '1.23a', '1.23a', '1.23a', '1.23a',
'a', 'a,c', json_object('a', 1, 'b', 2));

create table test_mod as select  mod(`int1`, `int1`) as mod1, mod(`uint1`, `uint1`) as mod2, mod(`int2`, `int2`) as mod3,  mod(`uint2`, `uint2`) as mod4,  mod(`int4`, `int4`) as mod5, 
mod(`uint4`, `uint4`) as mod6,  mod(`int8`, `int8`) as mod7,  mod(`uint8`, `uint8`) as mod8, mod(`float4`, `float4`) as mod9, mod(`float8`, `float8`) as mod10,  mod(`numeric`, `numeric`) as mod11, 
mod(`bit1`, `bit1`) as mod12,  mod(`bit64`, `bit64`) as mod13,  mod(`boolean`, `boolean`) as mod14, mod(`char`, `char`) as mod15,  mod(`varchar`, `varchar`) as mod16, mod(`text`, `text`) as mod17, 
mod(`binary`,`binary`) as mod18, mod(`varbinary`,`varbinary`) as mod19 from test_type_table;

select * from test_mod;

drop table test_mod;
drop table test_type_table;

drop schema db_test_mod cascade;
reset bytea_output;
reset dolphin.sql_mode;
reset dolphin.b_compatibility_mode;
reset current_schema;
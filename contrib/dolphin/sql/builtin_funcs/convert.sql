create schema db_convert;
set current_schema to 'db_convert';
set dolphin.b_compatibility_mode to on;

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
   `enum_t` enum('2023-01-01', '2024-01-01', '2025-01-01'),
   `set_t` set('2023-01-01', '2024-01-01', '2025-01-01'),
   `json` json   
);

insert into test_type_table values (1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, b'1', b'111', true, 
'2023-02-05', '19:10:50', '19:10:50.3456', '2023-02-05 19:10:50', 
'2023-02-05 19:10:50.456', '2023-02-05 19:10:50', '2023-02-05 19:10:50.456',
'2023', '1.23a', '1.23a', '1.23a', '1.23a', '1.23a', '1.23a', '1.23a', '1.23a', '1.23a',
'2023-01-01', '2023-01-01', json_object('a', 1, 'b', 2));

select convert(`int1` using utf8), convert(`uint1` using utf8), convert(`int2` using utf8), convert(`uint2` using utf8), convert(`int4` using utf8), 
convert(`uint4` using utf8), convert(`int8` using utf8), convert(`uint8` using utf8), convert(`float4` using utf8), convert(`float8` using utf8),
convert(`numeric` using utf8), convert(`bit1` using utf8), convert(`bit64` using utf8), convert(`boolean` using utf8), convert(`date` using utf8),
convert(`time` using utf8), convert(`time(4)` using utf8), convert(`datetime` using utf8), convert(`datetime(4)` using utf8), convert(`timestamp` using utf8), 
convert(`timestamp(4)` using utf8), convert(`year` using utf8), convert(`char` using utf8), convert(`varchar` using utf8), convert(`binary` using utf8), 
convert(`varbinary` using utf8), convert(`tinyblob` using utf8), convert(`blob` using utf8), convert(`mediumblob` using utf8), convert(`longblob` using utf8), 
convert(`text` using utf8), convert(`enum_t` using utf8), convert(`set_t` using utf8), convert(`json` using utf8) from test_type_table;

select convert(`int1` using 'utf8'), convert(`uint1` using 'utf8'), convert(`int2` using 'utf8'), convert(`uint2` using 'utf8'), convert(`int4` using 'utf8'), 
convert(`uint4` using 'utf8'), convert(`int8` using 'utf8'), convert(`uint8` using 'utf8'), convert(`float4` using 'utf8'), convert(`float8` using 'utf8'),
convert(`numeric` using 'utf8'), convert(`bit1` using 'utf8'), convert(`bit64` using 'utf8'), convert(`boolean` using 'utf8'), convert(`date` using 'utf8'),
convert(`time` using 'utf8'), convert(`time(4)` using 'utf8'), convert(`datetime` using 'utf8'), convert(`datetime(4)` using 'utf8'), convert(`timestamp` using 'utf8'), 
convert(`timestamp(4)` using 'utf8'), convert(`year` using 'utf8'), convert(`char` using 'utf8'), convert(`varchar` using 'utf8'), convert(`binary` using 'utf8'), 
convert(`varbinary` using 'utf8'), convert(`tinyblob` using 'utf8'), convert(`blob` using 'utf8'), convert(`mediumblob` using 'utf8'), convert(`longblob` using 'utf8'), 
convert(`text` using 'utf8'), convert(`enum_t` using 'utf8'), convert(`set_t` using 'utf8'), convert(`json` using 'utf8') from test_type_table;

drop table test_type_table;

select convert(1 using 'utf8');
select convert('1' using 'utf8');
select convert('a' using 'utf8');
select convert(1.1 using 'utf8');
select convert(null using 'utf8');
select convert(1);
select convert(1 using 'utf8');
select convert(1 using 'gbk');
select convert(1 using 'utf8');
select convert('测试' using 'utf8');
select convert('测试' using utf8);
select convert('测试' using gbk);
select convert('测试' using latin1);
select convert(11.1, decimal(10,3));
select convert(1 using decimal(10,3));
select pg_typeof(convert('1', char));
select pg_typeof(convert('1', char(10)));
set dolphin.b_compatibility_mode = on;
select pg_typeof(convert('1', char));
select pg_typeof(convert('1', char(10)));
drop schema db_convert cascade;
reset current_schema;

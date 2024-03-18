create schema test_std;
set current_schema to 'test_std';
set dolphin.b_compatibility_mode to on;

create table all_types_table (
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
    `time4` time(4),
    `datetime` datetime,
    `datetime4` datetime(4) default '2023-12-30 12:00:12',
    `timestamp` timestamp,
    `timestamp4` timestamp(4) default '2023-12-30 12:00:12',
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
    `json` json,
    `null` integer
);

INSERT INTO all_types_table
VALUES(
        1,
        1,
        1,
        1,
        1,
        1,
        1,
        1,
        1.0,
        1.0,
        3.14259,
        1,
        10101100,
        1,
        '2024-01-11',
        '11:47:58',
        '11:47:58.7896',
        '2024-01-11 11:49:25',
        '2024-01-11 11:49:25.1234',
        '2024-01-11 11:49:25',
        '2024-01-11 11:49:25.1234',
        '2024',
        '62.345*67-89',
        '62.345*67-89',
        '67890 - 12345 = 55545',
        '67890 - 12345 = 55545',
        '67890 - 12345 = 55545',
        '67890 - 12345 = 55545',
        '67890 - 12345 = 55545',
        '67890 - 12345 = 55545',
        '67890 - 12345 = 55545',
        'b',
        'a,b',
        json_object('a', 2, 'b', 3),
        NULL
    );

SELECT STD(`int1`) OVER () AS `int1` FROM all_types_table;
SELECT STD(`uint1`) OVER () AS `uint1` FROM all_types_table;
SELECT STD(`int2`) OVER () AS `int2` FROM all_types_table;
SELECT STD(`uint2`) OVER () AS `uint2` FROM all_types_table;
SELECT STD(`int4`) OVER () AS `int4` FROM all_types_table;
SELECT STD(`uint4`) OVER () AS `uint4` FROM all_types_table;
SELECT STD(`int8`) OVER () AS `int8` FROM all_types_table;
SELECT STD(`uint8`) OVER () AS `uint8` FROM all_types_table;
SELECT STD(`float4`) OVER () AS `float4` FROM all_types_table;
SELECT STD(`float8`) OVER () AS `float8` FROM all_types_table;
SELECT STD(`numeric`) OVER () AS `numeric` FROM all_types_table;
SELECT STD(`bit1`) OVER () AS `bit1` FROM all_types_table;
SELECT STD(`bit64`) OVER () AS `bit64` FROM all_types_table;
SELECT STD(`boolean`) OVER () AS `boolean` FROM all_types_table;
SELECT STD(`date`) OVER () AS `date` FROM all_types_table;
SELECT STD(`time`) OVER () AS `time` FROM all_types_table;
SELECT STD(`time4`) OVER () AS `time4` FROM all_types_table;
SELECT STD(`datetime`) OVER () AS `datetime` FROM all_types_table;
SELECT STD(`datetime4`) OVER () AS `datetime4` FROM all_types_table;
SELECT STD(`timestamp`) OVER () AS `timestamp` FROM all_types_table;
SELECT STD(`timestamp4`) OVER () AS `timestamp4` FROM all_types_table;
SELECT STD(`year`) OVER () AS `year` FROM all_types_table;
SELECT STD(`char`) OVER () AS `char` FROM all_types_table;
SELECT STD(`varchar`) OVER () AS `varchar` FROM all_types_table;
SELECT STD(`binary`) OVER () AS `binary` FROM all_types_table;
SELECT STD(`varbinary`) OVER () AS `varbinary` FROM all_types_table;
SELECT STD(`tinyblob`) OVER () AS `tinyblob` FROM all_types_table;
SELECT STD(`blob`) OVER () AS `blob` FROM all_types_table;
SELECT STD(`mediumblob`) OVER () AS `mediumblob` FROM all_types_table;
SELECT STD(`longblob`) OVER () AS `longblob` FROM all_types_table;
SELECT STD(`text`) OVER () AS `text` FROM all_types_table;
SELECT STD(`enum_t`) OVER () AS `enum_t` FROM all_types_table;
SELECT STD(`set_t`) OVER () AS `set_t` FROM all_types_table;
SELECT STD(`json`) OVER () AS `json` FROM all_types_table;
SELECT STD(`NULL`) OVER () AS `NULL` FROM all_types_table;

INSERT INTO all_types_table
VALUES(
        2,
        2,
        2,
        2,
        2,
        2,
        2,
        2,
        2.0,
        2.0,
        2.14259,
        0,
        10101101,
        0,
        '2024-02-11',
        '12:47:58',
        '12:47:58.7896',
        '2024-02-11 11:49:25',
        '2024-02-11 11:49:25.1234',
        '2024-02-11 11:49:25',
        '2024-02-11 11:49:25.1234',
        '2023',
        '61.345*67-89',
        '61.345*67-89',
        '61890 - 12345 = 55545',
        '61890 - 12345 = 55545',
        '61890 - 12345 = 55545',
        '61890 - 12345 = 55545',
        '61890 - 12345 = 55545',
        '61890 - 12345 = 55545',
        '61890 - 12345 = 55545',
        'a',
        'a',
        json_object('a', 2, 'b', 1),
        NULL
    );

SELECT STD(`int1`) OVER () AS `int1` FROM all_types_table;
SELECT STD(`uint1`) OVER () AS `uint1` FROM all_types_table;
SELECT STD(`int2`) OVER () AS `int2` FROM all_types_table;
SELECT STD(`uint2`) OVER () AS `uint2` FROM all_types_table;
SELECT STD(`int4`) OVER () AS `int4` FROM all_types_table;
SELECT STD(`uint4`) OVER () AS `uint4` FROM all_types_table;
SELECT STD(`int8`) OVER () AS `int8` FROM all_types_table;
SELECT STD(`uint8`) OVER () AS `uint8` FROM all_types_table;
SELECT STD(`float4`) OVER () AS `float4` FROM all_types_table;
SELECT STD(`float8`) OVER () AS `float8` FROM all_types_table;
SELECT STD(`numeric`) OVER () AS `numeric` FROM all_types_table;
SELECT STD(`bit1`) OVER () AS `bit1` FROM all_types_table;
SELECT STD(`bit64`) OVER () AS `bit64` FROM all_types_table;
SELECT STD(`boolean`) OVER () AS `boolean` FROM all_types_table;
SELECT STD(`date`) OVER () AS `date` FROM all_types_table;
SELECT STD(`time`) OVER () AS `time` FROM all_types_table;
SELECT STD(`time4`) OVER () AS `time4` FROM all_types_table;
SELECT STD(`datetime`) OVER () AS `datetime` FROM all_types_table;
SELECT STD(`datetime4`) OVER () AS `datetime4` FROM all_types_table;
SELECT STD(`timestamp`) OVER () AS `timestamp` FROM all_types_table;
SELECT STD(`timestamp4`) OVER () AS `timestamp4` FROM all_types_table;
SELECT STD(`year`) OVER () AS `year` FROM all_types_table;
SELECT STD(`char`) OVER () AS `char` FROM all_types_table;
SELECT STD(`varchar`) OVER () AS `varchar` FROM all_types_table;
SELECT STD(`binary`) OVER () AS `binary` FROM all_types_table;
SELECT STD(`varbinary`) OVER () AS `varbinary` FROM all_types_table;
SELECT STD(`tinyblob`) OVER () AS `tinyblob` FROM all_types_table;
SELECT STD(`blob`) OVER () AS `blob` FROM all_types_table;
SELECT STD(`mediumblob`) OVER () AS `mediumblob` FROM all_types_table;
SELECT STD(`longblob`) OVER () AS `longblob` FROM all_types_table;
SELECT STD(`text`) OVER () AS `text` FROM all_types_table;
SELECT STD(`enum_t`) OVER () AS `enum_t` FROM all_types_table;
SELECT STD(`set_t`) OVER () AS `set_t` FROM all_types_table;
SELECT STD(`json`) OVER () AS `json` FROM all_types_table;
SELECT STD(`NULL`) OVER () AS `NULL` FROM all_types_table;

create view test_std_view as select STD(`json`) OVER () AS `json` FROM all_types_table;
\d test_std_view

drop view test_std_view;
drop table all_types_table;
set dolphin.b_compatibility_mode to off;
drop schema test_std cascade;
reset current_schema;

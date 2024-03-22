create schema test_var_pop;
set current_schema to 'test_var_pop';
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

SELECT VARIANCE(`int1`) OVER () AS `int1` FROM all_types_table;
SELECT VARIANCE(`uint1`) OVER () AS `uint1` FROM all_types_table;
SELECT VARIANCE(`int2`) OVER () AS `int2` FROM all_types_table;
SELECT VARIANCE(`uint2`) OVER () AS `uint2` FROM all_types_table;
SELECT VARIANCE(`int4`) OVER () AS `int4` FROM all_types_table;
SELECT VARIANCE(`uint4`) OVER () AS `uint4` FROM all_types_table;
SELECT VARIANCE(`int8`) OVER () AS `int8` FROM all_types_table;
SELECT VARIANCE(`uint8`) OVER () AS `uint8` FROM all_types_table;
SELECT VARIANCE(`float4`) OVER () AS `float4` FROM all_types_table;
SELECT VARIANCE(`float8`) OVER () AS `float8` FROM all_types_table;
SELECT VARIANCE(`numeric`) OVER () AS `numeric` FROM all_types_table;
SELECT VARIANCE(`bit1`) OVER () AS `bit1` FROM all_types_table;
SELECT VARIANCE(`bit64`) OVER () AS `bit64` FROM all_types_table;
SELECT VARIANCE(`boolean`) OVER () AS `boolean` FROM all_types_table;
SELECT VARIANCE(`date`) OVER () AS `date` FROM all_types_table;
SELECT VARIANCE(`time`) OVER () AS `time` FROM all_types_table;
SELECT VARIANCE(`time4`) OVER () AS `time4` FROM all_types_table;
SELECT VARIANCE(`datetime`) OVER () AS `datetime` FROM all_types_table;
SELECT VARIANCE(`datetime4`) OVER () AS `datetime4` FROM all_types_table;
SELECT VARIANCE(`timestamp`) OVER () AS `timestamp` FROM all_types_table;
SELECT VARIANCE(`timestamp4`) OVER () AS `timestamp4` FROM all_types_table;
SELECT VARIANCE(`year`) OVER () AS `year` FROM all_types_table;
SELECT VARIANCE(`char`) OVER () AS `char` FROM all_types_table;
SELECT VARIANCE(`varchar`) OVER () AS `varchar` FROM all_types_table;
SELECT VARIANCE(`binary`) OVER () AS `binary` FROM all_types_table;
SELECT VARIANCE(`varbinary`) OVER () AS `varbinary` FROM all_types_table;
SELECT VARIANCE(`tinyblob`) OVER () AS `tinyblob` FROM all_types_table;
SELECT VARIANCE(`blob`) OVER () AS `blob` FROM all_types_table;
SELECT VARIANCE(`mediumblob`) OVER () AS `mediumblob` FROM all_types_table;
SELECT VARIANCE(`longblob`) OVER () AS `longblob` FROM all_types_table;
SELECT VARIANCE(`text`) OVER () AS `text` FROM all_types_table;
SELECT VARIANCE(`enum_t`) OVER () AS `enum_t` FROM all_types_table;
SELECT VARIANCE(`set_t`) OVER () AS `set_t` FROM all_types_table;
SELECT VARIANCE(`json`) OVER () AS `json` FROM all_types_table;
SELECT VARIANCE(`NULL`) OVER () AS `NULL` FROM all_types_table;

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

SELECT VARIANCE(`int1`) OVER () AS `int1` FROM all_types_table;
SELECT VARIANCE(`uint1`) OVER () AS `uint1` FROM all_types_table;
SELECT VARIANCE(`int2`) OVER () AS `int2` FROM all_types_table;
SELECT VARIANCE(`uint2`) OVER () AS `uint2` FROM all_types_table;
SELECT VARIANCE(`int4`) OVER () AS `int4` FROM all_types_table;
SELECT VARIANCE(`uint4`) OVER () AS `uint4` FROM all_types_table;
SELECT VARIANCE(`int8`) OVER () AS `int8` FROM all_types_table;
SELECT VARIANCE(`uint8`) OVER () AS `uint8` FROM all_types_table;
SELECT VARIANCE(`float4`) OVER () AS `float4` FROM all_types_table;
SELECT VARIANCE(`float8`) OVER () AS `float8` FROM all_types_table;
SELECT VARIANCE(`numeric`) OVER () AS `numeric` FROM all_types_table;
SELECT VARIANCE(`bit1`) OVER () AS `bit1` FROM all_types_table;
SELECT VARIANCE(`bit64`) OVER () AS `bit64` FROM all_types_table;
SELECT VARIANCE(`boolean`) OVER () AS `boolean` FROM all_types_table;
SELECT VARIANCE(`date`) OVER () AS `date` FROM all_types_table;
SELECT VARIANCE(`time`) OVER () AS `time` FROM all_types_table;
SELECT VARIANCE(`time4`) OVER () AS `time4` FROM all_types_table;
SELECT VARIANCE(`datetime`) OVER () AS `datetime` FROM all_types_table;
SELECT VARIANCE(`datetime4`) OVER () AS `datetime4` FROM all_types_table;
SELECT VARIANCE(`timestamp`) OVER () AS `timestamp` FROM all_types_table;
SELECT VARIANCE(`timestamp4`) OVER () AS `timestamp4` FROM all_types_table;
SELECT VARIANCE(`year`) OVER () AS `year` FROM all_types_table;
SELECT VARIANCE(`char`) OVER () AS `char` FROM all_types_table;
SELECT VARIANCE(`varchar`) OVER () AS `varchar` FROM all_types_table;
SELECT VARIANCE(`binary`) OVER () AS `binary` FROM all_types_table;
SELECT VARIANCE(`varbinary`) OVER () AS `varbinary` FROM all_types_table;
SELECT VARIANCE(`tinyblob`) OVER () AS `tinyblob` FROM all_types_table;
SELECT VARIANCE(`blob`) OVER () AS `blob` FROM all_types_table;
SELECT VARIANCE(`mediumblob`) OVER () AS `mediumblob` FROM all_types_table;
SELECT VARIANCE(`longblob`) OVER () AS `longblob` FROM all_types_table;
SELECT VARIANCE(`text`) OVER () AS `text` FROM all_types_table;
SELECT VARIANCE(`enum_t`) OVER () AS `enum_t` FROM all_types_table;
SELECT VARIANCE(`set_t`) OVER () AS `set_t` FROM all_types_table;
SELECT VARIANCE(`json`) OVER () AS `json` FROM all_types_table;
SELECT VARIANCE(`NULL`) OVER () AS `NULL` FROM all_types_table;

create view test_var_pop_view as select VARIANCE(`json`) OVER () AS `json` FROM all_types_table;
\d test_var_pop_view

drop view test_var_pop_view;
drop table all_types_table;
set dolphin.b_compatibility_mode to off;
drop schema test_var_pop cascade;
reset current_schema;

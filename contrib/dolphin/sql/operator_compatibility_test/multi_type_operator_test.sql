drop schema if exists multi_type_operator_test_schema cascade;
create schema multi_type_operator_test_schema;
set current_schema to 'multi_type_operator_test_schema';
---------- head ----------
set dolphin.b_compatibility_mode to on;
set dolphin.sql_mode to 'sql_mode_full_group,pipes_as_concat,ansi_quotes,no_zero_date';
drop table if exists test_multi_type_table;
CREATE TABLE test_multi_type_table
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
    `date` date,
    `time` time,
    `time(4)` time(4),
    `datetime` datetime,
    `datetime(4)` datetime(4),
    `timestamp` timestamp,
    `timestamp(4)` timestamp(4),
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
    `set_t` set('a', 'b', 'c')
);
INSERT INTO test_multi_type_table
VALUES (1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, b'1', b'111', '2023-02-05', '19:10:50', '19:10:50.3456', '2023-02-05 19:10:50', '2023-02-05 19:10:50.456', '2023-02-05 19:10:50', '2023-02-05 19:10:50.456', '2023', '1.23a', '1.23a', '1.23a', '1.23a', '1.23a', '1.23a', '1.23a', '1.23a', '1.23a', 'a', 'a,c');
-- int op string
SELECT pg_typeof(`int4` + `char`) FROM test_multi_type_table;
SELECT pg_typeof(`int4` - `char`) FROM test_multi_type_table;
SELECT pg_typeof(`int4` * `char`) FROM test_multi_type_table;
SELECT pg_typeof(`int4` / `char`) FROM test_multi_type_table;
SELECT pg_typeof(`int4` + `text`) FROM test_multi_type_table;
SELECT pg_typeof(`int4` - `text`) FROM test_multi_type_table;
SELECT pg_typeof(`int4` * `text`) FROM test_multi_type_table;
SELECT pg_typeof(`int4` / `text`) FROM test_multi_type_table;

-- float op string
SELECT pg_typeof(`float8` + `char`) FROM test_multi_type_table;
SELECT pg_typeof(`float8` - `char`) FROM test_multi_type_table;
SELECT pg_typeof(`float8` * `char`) FROM test_multi_type_table;
SELECT pg_typeof(`float8` / `char`) FROM test_multi_type_table;
SELECT pg_typeof(`float8` + `text`) FROM test_multi_type_table;
SELECT pg_typeof(`float8` - `text`) FROM test_multi_type_table;
SELECT pg_typeof(`float8` * `text`) FROM test_multi_type_table;
SELECT pg_typeof(`float8` / `text`) FROM test_multi_type_table;

-- numeric op string
SELECT pg_typeof(`numeric` + `char`) FROM test_multi_type_table;
SELECT pg_typeof(`numeric` - `char`) FROM test_multi_type_table;
SELECT pg_typeof(`numeric` * `char`) FROM test_multi_type_table;
SELECT pg_typeof(`numeric` / `char`) FROM test_multi_type_table;
SELECT pg_typeof(`numeric` + `text`) FROM test_multi_type_table;
SELECT pg_typeof(`numeric` - `text`) FROM test_multi_type_table;
SELECT pg_typeof(`numeric` * `text`) FROM test_multi_type_table;
SELECT pg_typeof(`numeric` / `text`) FROM test_multi_type_table;

-- int op time
SELECT pg_typeof(`int4` + `date`) FROM test_multi_type_table;
SELECT pg_typeof(`int4` - `date`) FROM test_multi_type_table;
SELECT pg_typeof(`int4` * `date`) FROM test_multi_type_table;
SELECT pg_typeof(`int4` / `date`) FROM test_multi_type_table;
SELECT pg_typeof(`int4` + `time`) FROM test_multi_type_table;
SELECT pg_typeof(`int4` - `time`) FROM test_multi_type_table;
SELECT pg_typeof(`int4` * `time`) FROM test_multi_type_table;
SELECT pg_typeof(`int4` / `time`) FROM test_multi_type_table;
SELECT pg_typeof(`int4` + `datetime`) FROM test_multi_type_table;
SELECT pg_typeof(`int4` - `datetime`) FROM test_multi_type_table;
SELECT pg_typeof(`int4` * `datetime`) FROM test_multi_type_table;
SELECT pg_typeof(`int4` / `datetime`) FROM test_multi_type_table;
SELECT pg_typeof(`int4` + `datetime(4)`) FROM test_multi_type_table;
SELECT pg_typeof(`int4` - `datetime(4)`) FROM test_multi_type_table;
SELECT pg_typeof(`int4` * `datetime(4)`) FROM test_multi_type_table;
SELECT pg_typeof(`int4` / `datetime(4)`) FROM test_multi_type_table;
SELECT pg_typeof(`int4` + `year`) FROM test_multi_type_table;
SELECT pg_typeof(`int4` - `year`) FROM test_multi_type_table;
SELECT pg_typeof(`int4` * `year`) FROM test_multi_type_table;
SELECT pg_typeof(`int4` / `year`) FROM test_multi_type_table;

-- float op time
SELECT pg_typeof(`float8` + `date`) FROM test_multi_type_table;
SELECT pg_typeof(`float8` - `date`) FROM test_multi_type_table;
SELECT pg_typeof(`float8` * `date`) FROM test_multi_type_table;
SELECT pg_typeof(`float8` / `date`) FROM test_multi_type_table;
SELECT pg_typeof(`float8` + `time`) FROM test_multi_type_table;
SELECT pg_typeof(`float8` - `time`) FROM test_multi_type_table;
SELECT pg_typeof(`float8` * `time`) FROM test_multi_type_table;
SELECT pg_typeof(`float8` / `time`) FROM test_multi_type_table;
SELECT pg_typeof(`float8` + `datetime`) FROM test_multi_type_table;
SELECT pg_typeof(`float8` - `datetime`) FROM test_multi_type_table;
SELECT pg_typeof(`float8` * `datetime`) FROM test_multi_type_table;
SELECT pg_typeof(`float8` / `datetime`) FROM test_multi_type_table;
SELECT pg_typeof(`float8` + `datetime(4)`) FROM test_multi_type_table;
SELECT pg_typeof(`float8` - `datetime(4)`) FROM test_multi_type_table;
SELECT pg_typeof(`float8` * `datetime(4)`) FROM test_multi_type_table;
SELECT pg_typeof(`float8` / `datetime(4)`) FROM test_multi_type_table;
SELECT pg_typeof(`float8` + `year`) FROM test_multi_type_table;
SELECT pg_typeof(`float8` - `year`) FROM test_multi_type_table;
SELECT pg_typeof(`float8` * `year`) FROM test_multi_type_table;
SELECT pg_typeof(`float8` / `year`) FROM test_multi_type_table;

-- numeric op time
SELECT pg_typeof(`numeric` + `date`) FROM test_multi_type_table;
SELECT pg_typeof(`numeric` - `date`) FROM test_multi_type_table;
SELECT pg_typeof(`numeric` * `date`) FROM test_multi_type_table;
SELECT pg_typeof(`numeric` / `date`) FROM test_multi_type_table;
SELECT pg_typeof(`numeric` + `time`) FROM test_multi_type_table;
SELECT pg_typeof(`numeric` - `time`) FROM test_multi_type_table;
SELECT pg_typeof(`numeric` * `time`) FROM test_multi_type_table;
SELECT pg_typeof(`numeric` / `time`) FROM test_multi_type_table;
SELECT pg_typeof(`numeric` + `datetime`) FROM test_multi_type_table;
SELECT pg_typeof(`numeric` - `datetime`) FROM test_multi_type_table;
SELECT pg_typeof(`numeric` * `datetime`) FROM test_multi_type_table;
SELECT pg_typeof(`numeric` / `datetime`) FROM test_multi_type_table;
SELECT pg_typeof(`numeric` + `datetime(4)`) FROM test_multi_type_table;
SELECT pg_typeof(`numeric` - `datetime(4)`) FROM test_multi_type_table;
SELECT pg_typeof(`numeric` * `datetime(4)`) FROM test_multi_type_table;
SELECT pg_typeof(`numeric` / `datetime(4)`) FROM test_multi_type_table;
SELECT pg_typeof(`numeric` + `year`) FROM test_multi_type_table;
SELECT pg_typeof(`numeric` - `year`) FROM test_multi_type_table;
SELECT pg_typeof(`numeric` * `year`) FROM test_multi_type_table;
SELECT pg_typeof(`numeric` / `year`) FROM test_multi_type_table;

-- string op time
SELECT pg_typeof(`char` + `date`) FROM test_multi_type_table;
SELECT pg_typeof(`char` - `date`) FROM test_multi_type_table;
SELECT pg_typeof(`char` * `date`) FROM test_multi_type_table;
SELECT pg_typeof(`char` / `date`) FROM test_multi_type_table;
SELECT pg_typeof(`char` + `time`) FROM test_multi_type_table;
SELECT pg_typeof(`char` - `time`) FROM test_multi_type_table;
SELECT pg_typeof(`char` * `time`) FROM test_multi_type_table;
SELECT pg_typeof(`char` / `time`) FROM test_multi_type_table;
SELECT pg_typeof(`char` + `datetime`) FROM test_multi_type_table;
SELECT pg_typeof(`char` - `datetime`) FROM test_multi_type_table;
SELECT pg_typeof(`char` * `datetime`) FROM test_multi_type_table;
SELECT pg_typeof(`char` / `datetime`) FROM test_multi_type_table;
SELECT pg_typeof(`char` + `datetime(4)`) FROM test_multi_type_table;
SELECT pg_typeof(`char` - `datetime(4)`) FROM test_multi_type_table;
SELECT pg_typeof(`char` * `datetime(4)`) FROM test_multi_type_table;
SELECT pg_typeof(`char` / `datetime(4)`) FROM test_multi_type_table;
SELECT pg_typeof(`text` + `date`) FROM test_multi_type_table;
SELECT pg_typeof(`text` - `date`) FROM test_multi_type_table;
SELECT pg_typeof(`text` * `date`) FROM test_multi_type_table;
SELECT pg_typeof(`text` / `date`) FROM test_multi_type_table;
SELECT pg_typeof(`text` + `time`) FROM test_multi_type_table;
SELECT pg_typeof(`text` - `time`) FROM test_multi_type_table;
SELECT pg_typeof(`text` * `time`) FROM test_multi_type_table;
SELECT pg_typeof(`text` / `time`) FROM test_multi_type_table;
SELECT pg_typeof(`text` + `datetime`) FROM test_multi_type_table;
SELECT pg_typeof(`text` - `datetime`) FROM test_multi_type_table;
SELECT pg_typeof(`text` * `datetime`) FROM test_multi_type_table;
SELECT pg_typeof(`text` / `datetime`) FROM test_multi_type_table;
SELECT pg_typeof(`text` + `datetime(4)`) FROM test_multi_type_table;
SELECT pg_typeof(`text` - `datetime(4)`) FROM test_multi_type_table;
SELECT pg_typeof(`text` * `datetime(4)`) FROM test_multi_type_table;
SELECT pg_typeof(`text` / `datetime(4)`) FROM test_multi_type_table;
SELECT pg_typeof(`text` + `year`) FROM test_multi_type_table;
SELECT pg_typeof(`text` - `year`) FROM test_multi_type_table;
SELECT pg_typeof(`text` * `year`) FROM test_multi_type_table;
SELECT pg_typeof(`text` / `year`) FROM test_multi_type_table;


---------- tail ----------
drop schema multi_type_operator_test_schema cascade;
reset current_schema;
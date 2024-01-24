drop schema if exists multi_type_or_test_schema cascade;
create schema multi_type_or_test_schema;
set current_schema to 'multi_type_or_test_schema';
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
    `boolean` boolean,
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
    `set_t` set('a', 'b', 'c'),
    `json` json
);
INSERT INTO test_multi_type_table
VALUES (1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, b'1', b'111', true, '2023-02-05', '19:10:50', '19:10:50.3456', '2023-02-05 19:10:50', '2023-02-05 19:10:50.456', '2023-02-05 19:10:50', '2023-02-05 19:10:50.456', '2023', '1.23a', '1.23a', '1.23a', '1.23a', '1.23a', '1.23a', '1.23a', '1.23a', '1.23a', 'a', 'a,c', json_object('a', 1, 'b', 2));

-- type test
DROP TABLE IF EXISTS test_multi_type;
CREATE TABLE test_multi_type AS SELECT
`int1` or `int1` AS `int1_or_int1`,
`int1` or `uint1` AS `int1_or_uint1`,
`int1` or `int2` AS `int1_or_int2`,
`int1` or `uint2` AS `int1_or_uint2`,
`int1` or `int4` AS `int1_or_int4`,
`int1` or `uint4` AS `int1_or_uint4`,
`int1` or `int8` AS `int1_or_int8`,
`int1` or `uint8` AS `int1_or_uint8`,
`int1` or `float4` AS `int1_or_float4`,
`int1` or `float8` AS `int1_or_float8`,
`int1` or `numeric` AS `int1_or_numeric`,
`int1` or `bit1` AS `int1_or_bit1`,
`int1` or `bit64` AS `int1_or_bit64`,
`int1` or `boolean` AS `int1_or_boolean`,
`int1` or `date` AS `int1_or_date`,
`int1` or `time` AS `int1_or_time`,
`int1` or `time(4)` AS `int1_or_time(4)`,
`int1` or `datetime` AS `int1_or_datetime`,
`int1` or `datetime(4)` AS `int1_or_datetime(4)`,
`int1` or `timestamp` AS `int1_or_timestamp`,
`int1` or `timestamp(4)` AS `int1_or_timestamp(4)`,
`int1` or `year` AS `int1_or_year`,
`int1` or `char` AS `int1_or_char`,
`int1` or `varchar` AS `int1_or_varchar`,
`int1` or `binary` AS `int1_or_binary`,
`int1` or `varbinary` AS `int1_or_varbinary`,
`int1` or `tinyblob` AS `int1_or_tinyblob`,
`int1` or `blob` AS `int1_or_blob`,
`int1` or `mediumblob` AS `int1_or_mediumblob`,
`int1` or `longblob` AS `int1_or_longblob`,
`int1` or `text` AS `int1_or_text`,
`int1` or `enum_t` AS `int1_or_enum_t`,
`int1` or `set_t` AS `int1_or_set_t`,
`int1` or `json` AS `int1_or_json`,
`uint1` or `int1` AS `uint1_or_int1`,
`uint1` or `uint1` AS `uint1_or_uint1`,
`uint1` or `int2` AS `uint1_or_int2`,
`uint1` or `uint2` AS `uint1_or_uint2`,
`uint1` or `int4` AS `uint1_or_int4`,
`uint1` or `uint4` AS `uint1_or_uint4`,
`uint1` or `int8` AS `uint1_or_int8`,
`uint1` or `uint8` AS `uint1_or_uint8`,
`uint1` or `float4` AS `uint1_or_float4`,
`uint1` or `float8` AS `uint1_or_float8`,
`uint1` or `numeric` AS `uint1_or_numeric`,
`uint1` or `bit1` AS `uint1_or_bit1`,
`uint1` or `bit64` AS `uint1_or_bit64`,
`uint1` or `boolean` AS `uint1_or_boolean`,
`uint1` or `date` AS `uint1_or_date`,
`uint1` or `time` AS `uint1_or_time`,
`uint1` or `time(4)` AS `uint1_or_time(4)`,
`uint1` or `datetime` AS `uint1_or_datetime`,
`uint1` or `datetime(4)` AS `uint1_or_datetime(4)`,
`uint1` or `timestamp` AS `uint1_or_timestamp`,
`uint1` or `timestamp(4)` AS `uint1_or_timestamp(4)`,
`uint1` or `year` AS `uint1_or_year`,
`uint1` or `char` AS `uint1_or_char`,
`uint1` or `varchar` AS `uint1_or_varchar`,
`uint1` or `binary` AS `uint1_or_binary`,
`uint1` or `varbinary` AS `uint1_or_varbinary`,
`uint1` or `tinyblob` AS `uint1_or_tinyblob`,
`uint1` or `blob` AS `uint1_or_blob`,
`uint1` or `mediumblob` AS `uint1_or_mediumblob`,
`uint1` or `longblob` AS `uint1_or_longblob`,
`uint1` or `text` AS `uint1_or_text`,
`uint1` or `enum_t` AS `uint1_or_enum_t`,
`uint1` or `set_t` AS `uint1_or_set_t`,
`uint1` or `json` AS `uint1_or_json`,
`int2` or `int1` AS `int2_or_int1`,
`int2` or `uint1` AS `int2_or_uint1`,
`int2` or `int2` AS `int2_or_int2`,
`int2` or `uint2` AS `int2_or_uint2`,
`int2` or `int4` AS `int2_or_int4`,
`int2` or `uint4` AS `int2_or_uint4`,
`int2` or `int8` AS `int2_or_int8`,
`int2` or `uint8` AS `int2_or_uint8`,
`int2` or `float4` AS `int2_or_float4`,
`int2` or `float8` AS `int2_or_float8`,
`int2` or `numeric` AS `int2_or_numeric`,
`int2` or `bit1` AS `int2_or_bit1`,
`int2` or `bit64` AS `int2_or_bit64`,
`int2` or `boolean` AS `int2_or_boolean`,
`int2` or `date` AS `int2_or_date`,
`int2` or `time` AS `int2_or_time`,
`int2` or `time(4)` AS `int2_or_time(4)`,
`int2` or `datetime` AS `int2_or_datetime`,
`int2` or `datetime(4)` AS `int2_or_datetime(4)`,
`int2` or `timestamp` AS `int2_or_timestamp`,
`int2` or `timestamp(4)` AS `int2_or_timestamp(4)`,
`int2` or `year` AS `int2_or_year`,
`int2` or `char` AS `int2_or_char`,
`int2` or `varchar` AS `int2_or_varchar`,
`int2` or `binary` AS `int2_or_binary`,
`int2` or `varbinary` AS `int2_or_varbinary`,
`int2` or `tinyblob` AS `int2_or_tinyblob`,
`int2` or `blob` AS `int2_or_blob`,
`int2` or `mediumblob` AS `int2_or_mediumblob`,
`int2` or `longblob` AS `int2_or_longblob`,
`int2` or `text` AS `int2_or_text`,
`int2` or `enum_t` AS `int2_or_enum_t`,
`int2` or `set_t` AS `int2_or_set_t`,
`int2` or `json` AS `int2_or_json`,
`uint2` or `int1` AS `uint2_or_int1`,
`uint2` or `uint1` AS `uint2_or_uint1`,
`uint2` or `int2` AS `uint2_or_int2`,
`uint2` or `uint2` AS `uint2_or_uint2`,
`uint2` or `int4` AS `uint2_or_int4`,
`uint2` or `uint4` AS `uint2_or_uint4`,
`uint2` or `int8` AS `uint2_or_int8`,
`uint2` or `uint8` AS `uint2_or_uint8`,
`uint2` or `float4` AS `uint2_or_float4`,
`uint2` or `float8` AS `uint2_or_float8`,
`uint2` or `numeric` AS `uint2_or_numeric`,
`uint2` or `bit1` AS `uint2_or_bit1`,
`uint2` or `bit64` AS `uint2_or_bit64`,
`uint2` or `boolean` AS `uint2_or_boolean`,
`uint2` or `date` AS `uint2_or_date`,
`uint2` or `time` AS `uint2_or_time`,
`uint2` or `time(4)` AS `uint2_or_time(4)`,
`uint2` or `datetime` AS `uint2_or_datetime`,
`uint2` or `datetime(4)` AS `uint2_or_datetime(4)`,
`uint2` or `timestamp` AS `uint2_or_timestamp`,
`uint2` or `timestamp(4)` AS `uint2_or_timestamp(4)`,
`uint2` or `year` AS `uint2_or_year`,
`uint2` or `char` AS `uint2_or_char`,
`uint2` or `varchar` AS `uint2_or_varchar`,
`uint2` or `binary` AS `uint2_or_binary`,
`uint2` or `varbinary` AS `uint2_or_varbinary`,
`uint2` or `tinyblob` AS `uint2_or_tinyblob`,
`uint2` or `blob` AS `uint2_or_blob`,
`uint2` or `mediumblob` AS `uint2_or_mediumblob`,
`uint2` or `longblob` AS `uint2_or_longblob`,
`uint2` or `text` AS `uint2_or_text`,
`uint2` or `enum_t` AS `uint2_or_enum_t`,
`uint2` or `set_t` AS `uint2_or_set_t`,
`uint2` or `json` AS `uint2_or_json`,
`int4` or `int1` AS `int4_or_int1`,
`int4` or `uint1` AS `int4_or_uint1`,
`int4` or `int2` AS `int4_or_int2`,
`int4` or `uint2` AS `int4_or_uint2`,
`int4` or `int4` AS `int4_or_int4`,
`int4` or `uint4` AS `int4_or_uint4`,
`int4` or `int8` AS `int4_or_int8`,
`int4` or `uint8` AS `int4_or_uint8`,
`int4` or `float4` AS `int4_or_float4`,
`int4` or `float8` AS `int4_or_float8`,
`int4` or `numeric` AS `int4_or_numeric`,
`int4` or `bit1` AS `int4_or_bit1`,
`int4` or `bit64` AS `int4_or_bit64`,
`int4` or `boolean` AS `int4_or_boolean`,
`int4` or `date` AS `int4_or_date`,
`int4` or `time` AS `int4_or_time`,
`int4` or `time(4)` AS `int4_or_time(4)`,
`int4` or `datetime` AS `int4_or_datetime`,
`int4` or `datetime(4)` AS `int4_or_datetime(4)`,
`int4` or `timestamp` AS `int4_or_timestamp`,
`int4` or `timestamp(4)` AS `int4_or_timestamp(4)`,
`int4` or `year` AS `int4_or_year`,
`int4` or `char` AS `int4_or_char`,
`int4` or `varchar` AS `int4_or_varchar`,
`int4` or `binary` AS `int4_or_binary`,
`int4` or `varbinary` AS `int4_or_varbinary`,
`int4` or `tinyblob` AS `int4_or_tinyblob`,
`int4` or `blob` AS `int4_or_blob`,
`int4` or `mediumblob` AS `int4_or_mediumblob`,
`int4` or `longblob` AS `int4_or_longblob`,
`int4` or `text` AS `int4_or_text`,
`int4` or `enum_t` AS `int4_or_enum_t`,
`int4` or `set_t` AS `int4_or_set_t`,
`int4` or `json` AS `int4_or_json`,
`uint4` or `int1` AS `uint4_or_int1`,
`uint4` or `uint1` AS `uint4_or_uint1`,
`uint4` or `int2` AS `uint4_or_int2`,
`uint4` or `uint2` AS `uint4_or_uint2`,
`uint4` or `int4` AS `uint4_or_int4`,
`uint4` or `uint4` AS `uint4_or_uint4`,
`uint4` or `int8` AS `uint4_or_int8`,
`uint4` or `uint8` AS `uint4_or_uint8`,
`uint4` or `float4` AS `uint4_or_float4`,
`uint4` or `float8` AS `uint4_or_float8`,
`uint4` or `numeric` AS `uint4_or_numeric`,
`uint4` or `bit1` AS `uint4_or_bit1`,
`uint4` or `bit64` AS `uint4_or_bit64`,
`uint4` or `boolean` AS `uint4_or_boolean`,
`uint4` or `date` AS `uint4_or_date`,
`uint4` or `time` AS `uint4_or_time`,
`uint4` or `time(4)` AS `uint4_or_time(4)`,
`uint4` or `datetime` AS `uint4_or_datetime`,
`uint4` or `datetime(4)` AS `uint4_or_datetime(4)`,
`uint4` or `timestamp` AS `uint4_or_timestamp`,
`uint4` or `timestamp(4)` AS `uint4_or_timestamp(4)`,
`uint4` or `year` AS `uint4_or_year`,
`uint4` or `char` AS `uint4_or_char`,
`uint4` or `varchar` AS `uint4_or_varchar`,
`uint4` or `binary` AS `uint4_or_binary`,
`uint4` or `varbinary` AS `uint4_or_varbinary`,
`uint4` or `tinyblob` AS `uint4_or_tinyblob`,
`uint4` or `blob` AS `uint4_or_blob`,
`uint4` or `mediumblob` AS `uint4_or_mediumblob`,
`uint4` or `longblob` AS `uint4_or_longblob`,
`uint4` or `text` AS `uint4_or_text`,
`uint4` or `enum_t` AS `uint4_or_enum_t`,
`uint4` or `set_t` AS `uint4_or_set_t`,
`uint4` or `json` AS `uint4_or_json`,
`int8` or `int1` AS `int8_or_int1`,
`int8` or `uint1` AS `int8_or_uint1`,
`int8` or `int2` AS `int8_or_int2`,
`int8` or `uint2` AS `int8_or_uint2`,
`int8` or `int4` AS `int8_or_int4`,
`int8` or `uint4` AS `int8_or_uint4`,
`int8` or `int8` AS `int8_or_int8`,
`int8` or `uint8` AS `int8_or_uint8`,
`int8` or `float4` AS `int8_or_float4`,
`int8` or `float8` AS `int8_or_float8`,
`int8` or `numeric` AS `int8_or_numeric`,
`int8` or `bit1` AS `int8_or_bit1`,
`int8` or `bit64` AS `int8_or_bit64`,
`int8` or `boolean` AS `int8_or_boolean`,
`int8` or `date` AS `int8_or_date`,
`int8` or `time` AS `int8_or_time`,
`int8` or `time(4)` AS `int8_or_time(4)`,
`int8` or `datetime` AS `int8_or_datetime`,
`int8` or `datetime(4)` AS `int8_or_datetime(4)`,
`int8` or `timestamp` AS `int8_or_timestamp`,
`int8` or `timestamp(4)` AS `int8_or_timestamp(4)`,
`int8` or `year` AS `int8_or_year`,
`int8` or `char` AS `int8_or_char`,
`int8` or `varchar` AS `int8_or_varchar`,
`int8` or `binary` AS `int8_or_binary`,
`int8` or `varbinary` AS `int8_or_varbinary`,
`int8` or `tinyblob` AS `int8_or_tinyblob`,
`int8` or `blob` AS `int8_or_blob`,
`int8` or `mediumblob` AS `int8_or_mediumblob`,
`int8` or `longblob` AS `int8_or_longblob`,
`int8` or `text` AS `int8_or_text`,
`int8` or `enum_t` AS `int8_or_enum_t`,
`int8` or `set_t` AS `int8_or_set_t`,
`int8` or `json` AS `int8_or_json`,
`uint8` or `int1` AS `uint8_or_int1`,
`uint8` or `uint1` AS `uint8_or_uint1`,
`uint8` or `int2` AS `uint8_or_int2`,
`uint8` or `uint2` AS `uint8_or_uint2`,
`uint8` or `int4` AS `uint8_or_int4`,
`uint8` or `uint4` AS `uint8_or_uint4`,
`uint8` or `int8` AS `uint8_or_int8`,
`uint8` or `uint8` AS `uint8_or_uint8`,
`uint8` or `float4` AS `uint8_or_float4`,
`uint8` or `float8` AS `uint8_or_float8`,
`uint8` or `numeric` AS `uint8_or_numeric`,
`uint8` or `bit1` AS `uint8_or_bit1`,
`uint8` or `bit64` AS `uint8_or_bit64`,
`uint8` or `boolean` AS `uint8_or_boolean`,
`uint8` or `date` AS `uint8_or_date`,
`uint8` or `time` AS `uint8_or_time`,
`uint8` or `time(4)` AS `uint8_or_time(4)`,
`uint8` or `datetime` AS `uint8_or_datetime`,
`uint8` or `datetime(4)` AS `uint8_or_datetime(4)`,
`uint8` or `timestamp` AS `uint8_or_timestamp`,
`uint8` or `timestamp(4)` AS `uint8_or_timestamp(4)`,
`uint8` or `year` AS `uint8_or_year`,
`uint8` or `char` AS `uint8_or_char`,
`uint8` or `varchar` AS `uint8_or_varchar`,
`uint8` or `binary` AS `uint8_or_binary`,
`uint8` or `varbinary` AS `uint8_or_varbinary`,
`uint8` or `tinyblob` AS `uint8_or_tinyblob`,
`uint8` or `blob` AS `uint8_or_blob`,
`uint8` or `mediumblob` AS `uint8_or_mediumblob`,
`uint8` or `longblob` AS `uint8_or_longblob`,
`uint8` or `text` AS `uint8_or_text`,
`uint8` or `enum_t` AS `uint8_or_enum_t`,
`uint8` or `set_t` AS `uint8_or_set_t`,
`uint8` or `json` AS `uint8_or_json`,
`float4` or `int1` AS `float4_or_int1`,
`float4` or `uint1` AS `float4_or_uint1`,
`float4` or `int2` AS `float4_or_int2`,
`float4` or `uint2` AS `float4_or_uint2`,
`float4` or `int4` AS `float4_or_int4`,
`float4` or `uint4` AS `float4_or_uint4`,
`float4` or `int8` AS `float4_or_int8`,
`float4` or `uint8` AS `float4_or_uint8`,
`float4` or `float4` AS `float4_or_float4`,
`float4` or `float8` AS `float4_or_float8`,
`float4` or `numeric` AS `float4_or_numeric`,
`float4` or `bit1` AS `float4_or_bit1`,
`float4` or `bit64` AS `float4_or_bit64`,
`float4` or `boolean` AS `float4_or_boolean`,
`float4` or `date` AS `float4_or_date`,
`float4` or `time` AS `float4_or_time`,
`float4` or `time(4)` AS `float4_or_time(4)`,
`float4` or `datetime` AS `float4_or_datetime`,
`float4` or `datetime(4)` AS `float4_or_datetime(4)`,
`float4` or `timestamp` AS `float4_or_timestamp`,
`float4` or `timestamp(4)` AS `float4_or_timestamp(4)`,
`float4` or `year` AS `float4_or_year`,
`float4` or `char` AS `float4_or_char`,
`float4` or `varchar` AS `float4_or_varchar`,
`float4` or `binary` AS `float4_or_binary`,
`float4` or `varbinary` AS `float4_or_varbinary`,
`float4` or `tinyblob` AS `float4_or_tinyblob`,
`float4` or `blob` AS `float4_or_blob`,
`float4` or `mediumblob` AS `float4_or_mediumblob`,
`float4` or `longblob` AS `float4_or_longblob`,
`float4` or `text` AS `float4_or_text`,
`float4` or `enum_t` AS `float4_or_enum_t`,
`float4` or `set_t` AS `float4_or_set_t`,
`float4` or `json` AS `float4_or_json`,
`float8` or `int1` AS `float8_or_int1`,
`float8` or `uint1` AS `float8_or_uint1`,
`float8` or `int2` AS `float8_or_int2`,
`float8` or `uint2` AS `float8_or_uint2`,
`float8` or `int4` AS `float8_or_int4`,
`float8` or `uint4` AS `float8_or_uint4`,
`float8` or `int8` AS `float8_or_int8`,
`float8` or `uint8` AS `float8_or_uint8`,
`float8` or `float4` AS `float8_or_float4`,
`float8` or `float8` AS `float8_or_float8`,
`float8` or `numeric` AS `float8_or_numeric`,
`float8` or `bit1` AS `float8_or_bit1`,
`float8` or `bit64` AS `float8_or_bit64`,
`float8` or `boolean` AS `float8_or_boolean`,
`float8` or `date` AS `float8_or_date`,
`float8` or `time` AS `float8_or_time`,
`float8` or `time(4)` AS `float8_or_time(4)`,
`float8` or `datetime` AS `float8_or_datetime`,
`float8` or `datetime(4)` AS `float8_or_datetime(4)`,
`float8` or `timestamp` AS `float8_or_timestamp`,
`float8` or `timestamp(4)` AS `float8_or_timestamp(4)`,
`float8` or `year` AS `float8_or_year`,
`float8` or `char` AS `float8_or_char`,
`float8` or `varchar` AS `float8_or_varchar`,
`float8` or `binary` AS `float8_or_binary`,
`float8` or `varbinary` AS `float8_or_varbinary`,
`float8` or `tinyblob` AS `float8_or_tinyblob`,
`float8` or `blob` AS `float8_or_blob`,
`float8` or `mediumblob` AS `float8_or_mediumblob`,
`float8` or `longblob` AS `float8_or_longblob`,
`float8` or `text` AS `float8_or_text`,
`float8` or `enum_t` AS `float8_or_enum_t`,
`float8` or `set_t` AS `float8_or_set_t`,
`float8` or `json` AS `float8_or_json`,
`numeric` or `int1` AS `numeric_or_int1`,
`numeric` or `uint1` AS `numeric_or_uint1`,
`numeric` or `int2` AS `numeric_or_int2`,
`numeric` or `uint2` AS `numeric_or_uint2`,
`numeric` or `int4` AS `numeric_or_int4`,
`numeric` or `uint4` AS `numeric_or_uint4`,
`numeric` or `int8` AS `numeric_or_int8`,
`numeric` or `uint8` AS `numeric_or_uint8`,
`numeric` or `float4` AS `numeric_or_float4`,
`numeric` or `float8` AS `numeric_or_float8`,
`numeric` or `numeric` AS `numeric_or_numeric`,
`numeric` or `bit1` AS `numeric_or_bit1`,
`numeric` or `bit64` AS `numeric_or_bit64`,
`numeric` or `boolean` AS `numeric_or_boolean`,
`numeric` or `date` AS `numeric_or_date`,
`numeric` or `time` AS `numeric_or_time`,
`numeric` or `time(4)` AS `numeric_or_time(4)`,
`numeric` or `datetime` AS `numeric_or_datetime`,
`numeric` or `datetime(4)` AS `numeric_or_datetime(4)`,
`numeric` or `timestamp` AS `numeric_or_timestamp`,
`numeric` or `timestamp(4)` AS `numeric_or_timestamp(4)`,
`numeric` or `year` AS `numeric_or_year`,
`numeric` or `char` AS `numeric_or_char`,
`numeric` or `varchar` AS `numeric_or_varchar`,
`numeric` or `binary` AS `numeric_or_binary`,
`numeric` or `varbinary` AS `numeric_or_varbinary`,
`numeric` or `tinyblob` AS `numeric_or_tinyblob`,
`numeric` or `blob` AS `numeric_or_blob`,
`numeric` or `mediumblob` AS `numeric_or_mediumblob`,
`numeric` or `longblob` AS `numeric_or_longblob`,
`numeric` or `text` AS `numeric_or_text`,
`numeric` or `enum_t` AS `numeric_or_enum_t`,
`numeric` or `set_t` AS `numeric_or_set_t`,
`numeric` or `json` AS `numeric_or_json`,
`bit1` or `int1` AS `bit1_or_int1`,
`bit1` or `uint1` AS `bit1_or_uint1`,
`bit1` or `int2` AS `bit1_or_int2`,
`bit1` or `uint2` AS `bit1_or_uint2`,
`bit1` or `int4` AS `bit1_or_int4`,
`bit1` or `uint4` AS `bit1_or_uint4`,
`bit1` or `int8` AS `bit1_or_int8`,
`bit1` or `uint8` AS `bit1_or_uint8`,
`bit1` or `float4` AS `bit1_or_float4`,
`bit1` or `float8` AS `bit1_or_float8`,
`bit1` or `numeric` AS `bit1_or_numeric`,
`bit1` or `bit1` AS `bit1_or_bit1`,
`bit1` or `bit64` AS `bit1_or_bit64`,
`bit1` or `boolean` AS `bit1_or_boolean`,
`bit1` or `date` AS `bit1_or_date`,
`bit1` or `time` AS `bit1_or_time`,
`bit1` or `time(4)` AS `bit1_or_time(4)`,
`bit1` or `datetime` AS `bit1_or_datetime`,
`bit1` or `datetime(4)` AS `bit1_or_datetime(4)`,
`bit1` or `timestamp` AS `bit1_or_timestamp`,
`bit1` or `timestamp(4)` AS `bit1_or_timestamp(4)`,
`bit1` or `year` AS `bit1_or_year`,
`bit1` or `char` AS `bit1_or_char`,
`bit1` or `varchar` AS `bit1_or_varchar`,
`bit1` or `binary` AS `bit1_or_binary`,
`bit1` or `varbinary` AS `bit1_or_varbinary`,
`bit1` or `tinyblob` AS `bit1_or_tinyblob`,
`bit1` or `blob` AS `bit1_or_blob`,
`bit1` or `mediumblob` AS `bit1_or_mediumblob`,
`bit1` or `longblob` AS `bit1_or_longblob`,
`bit1` or `text` AS `bit1_or_text`,
`bit1` or `enum_t` AS `bit1_or_enum_t`,
`bit1` or `set_t` AS `bit1_or_set_t`,
`bit1` or `json` AS `bit1_or_json`,
`bit64` or `int1` AS `bit64_or_int1`,
`bit64` or `uint1` AS `bit64_or_uint1`,
`bit64` or `int2` AS `bit64_or_int2`,
`bit64` or `uint2` AS `bit64_or_uint2`,
`bit64` or `int4` AS `bit64_or_int4`,
`bit64` or `uint4` AS `bit64_or_uint4`,
`bit64` or `int8` AS `bit64_or_int8`,
`bit64` or `uint8` AS `bit64_or_uint8`,
`bit64` or `float4` AS `bit64_or_float4`,
`bit64` or `float8` AS `bit64_or_float8`,
`bit64` or `numeric` AS `bit64_or_numeric`,
`bit64` or `bit1` AS `bit64_or_bit1`,
`bit64` or `bit64` AS `bit64_or_bit64`,
`bit64` or `boolean` AS `bit64_or_boolean`,
`bit64` or `date` AS `bit64_or_date`,
`bit64` or `time` AS `bit64_or_time`,
`bit64` or `time(4)` AS `bit64_or_time(4)`,
`bit64` or `datetime` AS `bit64_or_datetime`,
`bit64` or `datetime(4)` AS `bit64_or_datetime(4)`,
`bit64` or `timestamp` AS `bit64_or_timestamp`,
`bit64` or `timestamp(4)` AS `bit64_or_timestamp(4)`,
`bit64` or `year` AS `bit64_or_year`,
`bit64` or `char` AS `bit64_or_char`,
`bit64` or `varchar` AS `bit64_or_varchar`,
`bit64` or `binary` AS `bit64_or_binary`,
`bit64` or `varbinary` AS `bit64_or_varbinary`,
`bit64` or `tinyblob` AS `bit64_or_tinyblob`,
`bit64` or `blob` AS `bit64_or_blob`,
`bit64` or `mediumblob` AS `bit64_or_mediumblob`,
`bit64` or `longblob` AS `bit64_or_longblob`,
`bit64` or `text` AS `bit64_or_text`,
`bit64` or `enum_t` AS `bit64_or_enum_t`,
`bit64` or `set_t` AS `bit64_or_set_t`,
`bit64` or `json` AS `bit64_or_json`,
`boolean` or `int1` AS `boolean_or_int1`,
`boolean` or `uint1` AS `boolean_or_uint1`,
`boolean` or `int2` AS `boolean_or_int2`,
`boolean` or `uint2` AS `boolean_or_uint2`,
`boolean` or `int4` AS `boolean_or_int4`,
`boolean` or `uint4` AS `boolean_or_uint4`,
`boolean` or `int8` AS `boolean_or_int8`,
`boolean` or `uint8` AS `boolean_or_uint8`,
`boolean` or `float4` AS `boolean_or_float4`,
`boolean` or `float8` AS `boolean_or_float8`,
`boolean` or `numeric` AS `boolean_or_numeric`,
`boolean` or `bit1` AS `boolean_or_bit1`,
`boolean` or `bit64` AS `boolean_or_bit64`,
`boolean` or `boolean` AS `boolean_or_boolean`,
`boolean` or `date` AS `boolean_or_date`,
`boolean` or `time` AS `boolean_or_time`,
`boolean` or `time(4)` AS `boolean_or_time(4)`,
`boolean` or `datetime` AS `boolean_or_datetime`,
`boolean` or `datetime(4)` AS `boolean_or_datetime(4)`,
`boolean` or `timestamp` AS `boolean_or_timestamp`,
`boolean` or `timestamp(4)` AS `boolean_or_timestamp(4)`,
`boolean` or `year` AS `boolean_or_year`,
`boolean` or `char` AS `boolean_or_char`,
`boolean` or `varchar` AS `boolean_or_varchar`,
`boolean` or `binary` AS `boolean_or_binary`,
`boolean` or `varbinary` AS `boolean_or_varbinary`,
`boolean` or `tinyblob` AS `boolean_or_tinyblob`,
`boolean` or `blob` AS `boolean_or_blob`,
`boolean` or `mediumblob` AS `boolean_or_mediumblob`,
`boolean` or `longblob` AS `boolean_or_longblob`,
`boolean` or `text` AS `boolean_or_text`,
`boolean` or `enum_t` AS `boolean_or_enum_t`,
`boolean` or `set_t` AS `boolean_or_set_t`,
`boolean` or `json` AS `boolean_or_json`,
`date` or `int1` AS `date_or_int1`,
`date` or `uint1` AS `date_or_uint1`,
`date` or `int2` AS `date_or_int2`,
`date` or `uint2` AS `date_or_uint2`,
`date` or `int4` AS `date_or_int4`,
`date` or `uint4` AS `date_or_uint4`,
`date` or `int8` AS `date_or_int8`,
`date` or `uint8` AS `date_or_uint8`,
`date` or `float4` AS `date_or_float4`,
`date` or `float8` AS `date_or_float8`,
`date` or `numeric` AS `date_or_numeric`,
`date` or `bit1` AS `date_or_bit1`,
`date` or `bit64` AS `date_or_bit64`,
`date` or `boolean` AS `date_or_boolean`,
`date` or `date` AS `date_or_date`,
`date` or `time` AS `date_or_time`,
`date` or `time(4)` AS `date_or_time(4)`,
`date` or `datetime` AS `date_or_datetime`,
`date` or `datetime(4)` AS `date_or_datetime(4)`,
`date` or `timestamp` AS `date_or_timestamp`,
`date` or `timestamp(4)` AS `date_or_timestamp(4)`,
`date` or `year` AS `date_or_year`,
`date` or `char` AS `date_or_char`,
`date` or `varchar` AS `date_or_varchar`,
`date` or `binary` AS `date_or_binary`,
`date` or `varbinary` AS `date_or_varbinary`,
`date` or `tinyblob` AS `date_or_tinyblob`,
`date` or `blob` AS `date_or_blob`,
`date` or `mediumblob` AS `date_or_mediumblob`,
`date` or `longblob` AS `date_or_longblob`,
`date` or `text` AS `date_or_text`,
`date` or `enum_t` AS `date_or_enum_t`,
`date` or `set_t` AS `date_or_set_t`,
`date` or `json` AS `date_or_json`,
`time` or `int1` AS `time_or_int1`,
`time` or `uint1` AS `time_or_uint1`,
`time` or `int2` AS `time_or_int2`,
`time` or `uint2` AS `time_or_uint2`,
`time` or `int4` AS `time_or_int4`,
`time` or `uint4` AS `time_or_uint4`,
`time` or `int8` AS `time_or_int8`,
`time` or `uint8` AS `time_or_uint8`,
`time` or `float4` AS `time_or_float4`,
`time` or `float8` AS `time_or_float8`,
`time` or `numeric` AS `time_or_numeric`,
`time` or `bit1` AS `time_or_bit1`,
`time` or `bit64` AS `time_or_bit64`,
`time` or `boolean` AS `time_or_boolean`,
`time` or `date` AS `time_or_date`,
`time` or `time` AS `time_or_time`,
`time` or `time(4)` AS `time_or_time(4)`,
`time` or `datetime` AS `time_or_datetime`,
`time` or `datetime(4)` AS `time_or_datetime(4)`,
`time` or `timestamp` AS `time_or_timestamp`,
`time` or `timestamp(4)` AS `time_or_timestamp(4)`,
`time` or `year` AS `time_or_year`,
`time` or `char` AS `time_or_char`,
`time` or `varchar` AS `time_or_varchar`,
`time` or `binary` AS `time_or_binary`,
`time` or `varbinary` AS `time_or_varbinary`,
`time` or `tinyblob` AS `time_or_tinyblob`,
`time` or `blob` AS `time_or_blob`,
`time` or `mediumblob` AS `time_or_mediumblob`,
`time` or `longblob` AS `time_or_longblob`,
`time` or `text` AS `time_or_text`,
`time` or `enum_t` AS `time_or_enum_t`,
`time` or `set_t` AS `time_or_set_t`,
`time` or `json` AS `time_or_json`,
`time(4)` or `int1` AS `time(4)_or_int1`,
`time(4)` or `uint1` AS `time(4)_or_uint1`,
`time(4)` or `int2` AS `time(4)_or_int2`,
`time(4)` or `uint2` AS `time(4)_or_uint2`,
`time(4)` or `int4` AS `time(4)_or_int4`,
`time(4)` or `uint4` AS `time(4)_or_uint4`,
`time(4)` or `int8` AS `time(4)_or_int8`,
`time(4)` or `uint8` AS `time(4)_or_uint8`,
`time(4)` or `float4` AS `time(4)_or_float4`,
`time(4)` or `float8` AS `time(4)_or_float8`,
`time(4)` or `numeric` AS `time(4)_or_numeric`,
`time(4)` or `bit1` AS `time(4)_or_bit1`,
`time(4)` or `bit64` AS `time(4)_or_bit64`,
`time(4)` or `boolean` AS `time(4)_or_boolean`,
`time(4)` or `date` AS `time(4)_or_date`,
`time(4)` or `time` AS `time(4)_or_time`,
`time(4)` or `time(4)` AS `time(4)_or_time(4)`,
`time(4)` or `datetime` AS `time(4)_or_datetime`,
`time(4)` or `datetime(4)` AS `time(4)_or_datetime(4)`,
`time(4)` or `timestamp` AS `time(4)_or_timestamp`,
`time(4)` or `timestamp(4)` AS `time(4)_or_timestamp(4)`,
`time(4)` or `year` AS `time(4)_or_year`,
`time(4)` or `char` AS `time(4)_or_char`,
`time(4)` or `varchar` AS `time(4)_or_varchar`,
`time(4)` or `binary` AS `time(4)_or_binary`,
`time(4)` or `varbinary` AS `time(4)_or_varbinary`,
`time(4)` or `tinyblob` AS `time(4)_or_tinyblob`,
`time(4)` or `blob` AS `time(4)_or_blob`,
`time(4)` or `mediumblob` AS `time(4)_or_mediumblob`,
`time(4)` or `longblob` AS `time(4)_or_longblob`,
`time(4)` or `text` AS `time(4)_or_text`,
`time(4)` or `enum_t` AS `time(4)_or_enum_t`,
`time(4)` or `set_t` AS `time(4)_or_set_t`,
`time(4)` or `json` AS `time(4)_or_json`,
`datetime` or `int1` AS `datetime_or_int1`,
`datetime` or `uint1` AS `datetime_or_uint1`,
`datetime` or `int2` AS `datetime_or_int2`,
`datetime` or `uint2` AS `datetime_or_uint2`,
`datetime` or `int4` AS `datetime_or_int4`,
`datetime` or `uint4` AS `datetime_or_uint4`,
`datetime` or `int8` AS `datetime_or_int8`,
`datetime` or `uint8` AS `datetime_or_uint8`,
`datetime` or `float4` AS `datetime_or_float4`,
`datetime` or `float8` AS `datetime_or_float8`,
`datetime` or `numeric` AS `datetime_or_numeric`,
`datetime` or `bit1` AS `datetime_or_bit1`,
`datetime` or `bit64` AS `datetime_or_bit64`,
`datetime` or `boolean` AS `datetime_or_boolean`,
`datetime` or `date` AS `datetime_or_date`,
`datetime` or `time` AS `datetime_or_time`,
`datetime` or `time(4)` AS `datetime_or_time(4)`,
`datetime` or `datetime` AS `datetime_or_datetime`,
`datetime` or `datetime(4)` AS `datetime_or_datetime(4)`,
`datetime` or `timestamp` AS `datetime_or_timestamp`,
`datetime` or `timestamp(4)` AS `datetime_or_timestamp(4)`,
`datetime` or `year` AS `datetime_or_year`,
`datetime` or `char` AS `datetime_or_char`,
`datetime` or `varchar` AS `datetime_or_varchar`,
`datetime` or `binary` AS `datetime_or_binary`,
`datetime` or `varbinary` AS `datetime_or_varbinary`,
`datetime` or `tinyblob` AS `datetime_or_tinyblob`,
`datetime` or `blob` AS `datetime_or_blob`,
`datetime` or `mediumblob` AS `datetime_or_mediumblob`,
`datetime` or `longblob` AS `datetime_or_longblob`,
`datetime` or `text` AS `datetime_or_text`,
`datetime` or `enum_t` AS `datetime_or_enum_t`,
`datetime` or `set_t` AS `datetime_or_set_t`,
`datetime` or `json` AS `datetime_or_json`,
`datetime(4)` or `int1` AS `datetime(4)_or_int1`,
`datetime(4)` or `uint1` AS `datetime(4)_or_uint1`,
`datetime(4)` or `int2` AS `datetime(4)_or_int2`,
`datetime(4)` or `uint2` AS `datetime(4)_or_uint2`,
`datetime(4)` or `int4` AS `datetime(4)_or_int4`,
`datetime(4)` or `uint4` AS `datetime(4)_or_uint4`,
`datetime(4)` or `int8` AS `datetime(4)_or_int8`,
`datetime(4)` or `uint8` AS `datetime(4)_or_uint8`,
`datetime(4)` or `float4` AS `datetime(4)_or_float4`,
`datetime(4)` or `float8` AS `datetime(4)_or_float8`,
`datetime(4)` or `numeric` AS `datetime(4)_or_numeric`,
`datetime(4)` or `bit1` AS `datetime(4)_or_bit1`,
`datetime(4)` or `bit64` AS `datetime(4)_or_bit64`,
`datetime(4)` or `boolean` AS `datetime(4)_or_boolean`,
`datetime(4)` or `date` AS `datetime(4)_or_date`,
`datetime(4)` or `time` AS `datetime(4)_or_time`,
`datetime(4)` or `time(4)` AS `datetime(4)_or_time(4)`,
`datetime(4)` or `datetime` AS `datetime(4)_or_datetime`,
`datetime(4)` or `datetime(4)` AS `datetime(4)_or_datetime(4)`,
`datetime(4)` or `timestamp` AS `datetime(4)_or_timestamp`,
`datetime(4)` or `timestamp(4)` AS `datetime(4)_or_timestamp(4)`,
`datetime(4)` or `year` AS `datetime(4)_or_year`,
`datetime(4)` or `char` AS `datetime(4)_or_char`,
`datetime(4)` or `varchar` AS `datetime(4)_or_varchar`,
`datetime(4)` or `binary` AS `datetime(4)_or_binary`,
`datetime(4)` or `varbinary` AS `datetime(4)_or_varbinary`,
`datetime(4)` or `tinyblob` AS `datetime(4)_or_tinyblob`,
`datetime(4)` or `blob` AS `datetime(4)_or_blob`,
`datetime(4)` or `mediumblob` AS `datetime(4)_or_mediumblob`,
`datetime(4)` or `longblob` AS `datetime(4)_or_longblob`,
`datetime(4)` or `text` AS `datetime(4)_or_text`,
`datetime(4)` or `enum_t` AS `datetime(4)_or_enum_t`,
`datetime(4)` or `set_t` AS `datetime(4)_or_set_t`,
`datetime(4)` or `json` AS `datetime(4)_or_json`,
`timestamp` or `int1` AS `timestamp_or_int1`,
`timestamp` or `uint1` AS `timestamp_or_uint1`,
`timestamp` or `int2` AS `timestamp_or_int2`,
`timestamp` or `uint2` AS `timestamp_or_uint2`,
`timestamp` or `int4` AS `timestamp_or_int4`,
`timestamp` or `uint4` AS `timestamp_or_uint4`,
`timestamp` or `int8` AS `timestamp_or_int8`,
`timestamp` or `uint8` AS `timestamp_or_uint8`,
`timestamp` or `float4` AS `timestamp_or_float4`,
`timestamp` or `float8` AS `timestamp_or_float8`,
`timestamp` or `numeric` AS `timestamp_or_numeric`,
`timestamp` or `bit1` AS `timestamp_or_bit1`,
`timestamp` or `bit64` AS `timestamp_or_bit64`,
`timestamp` or `boolean` AS `timestamp_or_boolean`,
`timestamp` or `date` AS `timestamp_or_date`,
`timestamp` or `time` AS `timestamp_or_time`,
`timestamp` or `time(4)` AS `timestamp_or_time(4)`,
`timestamp` or `datetime` AS `timestamp_or_datetime`,
`timestamp` or `datetime(4)` AS `timestamp_or_datetime(4)`,
`timestamp` or `timestamp` AS `timestamp_or_timestamp`,
`timestamp` or `timestamp(4)` AS `timestamp_or_timestamp(4)`,
`timestamp` or `year` AS `timestamp_or_year`,
`timestamp` or `char` AS `timestamp_or_char`,
`timestamp` or `varchar` AS `timestamp_or_varchar`,
`timestamp` or `binary` AS `timestamp_or_binary`,
`timestamp` or `varbinary` AS `timestamp_or_varbinary`,
`timestamp` or `tinyblob` AS `timestamp_or_tinyblob`,
`timestamp` or `blob` AS `timestamp_or_blob`,
`timestamp` or `mediumblob` AS `timestamp_or_mediumblob`,
`timestamp` or `longblob` AS `timestamp_or_longblob`,
`timestamp` or `text` AS `timestamp_or_text`,
`timestamp` or `enum_t` AS `timestamp_or_enum_t`,
`timestamp` or `set_t` AS `timestamp_or_set_t`,
`timestamp` or `json` AS `timestamp_or_json`,
`timestamp(4)` or `int1` AS `timestamp(4)_or_int1`,
`timestamp(4)` or `uint1` AS `timestamp(4)_or_uint1`,
`timestamp(4)` or `int2` AS `timestamp(4)_or_int2`,
`timestamp(4)` or `uint2` AS `timestamp(4)_or_uint2`,
`timestamp(4)` or `int4` AS `timestamp(4)_or_int4`,
`timestamp(4)` or `uint4` AS `timestamp(4)_or_uint4`,
`timestamp(4)` or `int8` AS `timestamp(4)_or_int8`,
`timestamp(4)` or `uint8` AS `timestamp(4)_or_uint8`,
`timestamp(4)` or `float4` AS `timestamp(4)_or_float4`,
`timestamp(4)` or `float8` AS `timestamp(4)_or_float8`,
`timestamp(4)` or `numeric` AS `timestamp(4)_or_numeric`,
`timestamp(4)` or `bit1` AS `timestamp(4)_or_bit1`,
`timestamp(4)` or `bit64` AS `timestamp(4)_or_bit64`,
`timestamp(4)` or `boolean` AS `timestamp(4)_or_boolean`,
`timestamp(4)` or `date` AS `timestamp(4)_or_date`,
`timestamp(4)` or `time` AS `timestamp(4)_or_time`,
`timestamp(4)` or `time(4)` AS `timestamp(4)_or_time(4)`,
`timestamp(4)` or `datetime` AS `timestamp(4)_or_datetime`,
`timestamp(4)` or `datetime(4)` AS `timestamp(4)_or_datetime(4)`,
`timestamp(4)` or `timestamp` AS `timestamp(4)_or_timestamp`,
`timestamp(4)` or `timestamp(4)` AS `timestamp(4)_or_timestamp(4)`,
`timestamp(4)` or `year` AS `timestamp(4)_or_year`,
`timestamp(4)` or `char` AS `timestamp(4)_or_char`,
`timestamp(4)` or `varchar` AS `timestamp(4)_or_varchar`,
`timestamp(4)` or `binary` AS `timestamp(4)_or_binary`,
`timestamp(4)` or `varbinary` AS `timestamp(4)_or_varbinary`,
`timestamp(4)` or `tinyblob` AS `timestamp(4)_or_tinyblob`,
`timestamp(4)` or `blob` AS `timestamp(4)_or_blob`,
`timestamp(4)` or `mediumblob` AS `timestamp(4)_or_mediumblob`,
`timestamp(4)` or `longblob` AS `timestamp(4)_or_longblob`,
`timestamp(4)` or `text` AS `timestamp(4)_or_text`,
`timestamp(4)` or `enum_t` AS `timestamp(4)_or_enum_t`,
`timestamp(4)` or `set_t` AS `timestamp(4)_or_set_t`,
`timestamp(4)` or `json` AS `timestamp(4)_or_json`,
`year` or `int1` AS `year_or_int1`,
`year` or `uint1` AS `year_or_uint1`,
`year` or `int2` AS `year_or_int2`,
`year` or `uint2` AS `year_or_uint2`,
`year` or `int4` AS `year_or_int4`,
`year` or `uint4` AS `year_or_uint4`,
`year` or `int8` AS `year_or_int8`,
`year` or `uint8` AS `year_or_uint8`,
`year` or `float4` AS `year_or_float4`,
`year` or `float8` AS `year_or_float8`,
`year` or `numeric` AS `year_or_numeric`,
`year` or `bit1` AS `year_or_bit1`,
`year` or `bit64` AS `year_or_bit64`,
`year` or `boolean` AS `year_or_boolean`,
`year` or `date` AS `year_or_date`,
`year` or `time` AS `year_or_time`,
`year` or `time(4)` AS `year_or_time(4)`,
`year` or `datetime` AS `year_or_datetime`,
`year` or `datetime(4)` AS `year_or_datetime(4)`,
`year` or `timestamp` AS `year_or_timestamp`,
`year` or `timestamp(4)` AS `year_or_timestamp(4)`,
`year` or `year` AS `year_or_year`,
`year` or `char` AS `year_or_char`,
`year` or `varchar` AS `year_or_varchar`,
`year` or `binary` AS `year_or_binary`,
`year` or `varbinary` AS `year_or_varbinary`,
`year` or `tinyblob` AS `year_or_tinyblob`,
`year` or `blob` AS `year_or_blob`,
`year` or `mediumblob` AS `year_or_mediumblob`,
`year` or `longblob` AS `year_or_longblob`,
`year` or `text` AS `year_or_text`,
`year` or `enum_t` AS `year_or_enum_t`,
`year` or `set_t` AS `year_or_set_t`,
`year` or `json` AS `year_or_json`,
`char` or `int1` AS `char_or_int1`,
`char` or `uint1` AS `char_or_uint1`,
`char` or `int2` AS `char_or_int2`,
`char` or `uint2` AS `char_or_uint2`,
`char` or `int4` AS `char_or_int4`,
`char` or `uint4` AS `char_or_uint4`,
`char` or `int8` AS `char_or_int8`,
`char` or `uint8` AS `char_or_uint8`,
`char` or `float4` AS `char_or_float4`,
`char` or `float8` AS `char_or_float8`,
`char` or `numeric` AS `char_or_numeric`,
`char` or `bit1` AS `char_or_bit1`,
`char` or `bit64` AS `char_or_bit64`,
`char` or `boolean` AS `char_or_boolean`,
`char` or `date` AS `char_or_date`,
`char` or `time` AS `char_or_time`,
`char` or `time(4)` AS `char_or_time(4)`,
`char` or `datetime` AS `char_or_datetime`,
`char` or `datetime(4)` AS `char_or_datetime(4)`,
`char` or `timestamp` AS `char_or_timestamp`,
`char` or `timestamp(4)` AS `char_or_timestamp(4)`,
`char` or `year` AS `char_or_year`,
`char` or `char` AS `char_or_char`,
`char` or `varchar` AS `char_or_varchar`,
`char` or `binary` AS `char_or_binary`,
`char` or `varbinary` AS `char_or_varbinary`,
`char` or `tinyblob` AS `char_or_tinyblob`,
`char` or `blob` AS `char_or_blob`,
`char` or `mediumblob` AS `char_or_mediumblob`,
`char` or `longblob` AS `char_or_longblob`,
`char` or `text` AS `char_or_text`,
`char` or `enum_t` AS `char_or_enum_t`,
`char` or `set_t` AS `char_or_set_t`,
`char` or `json` AS `char_or_json`,
`varchar` or `int1` AS `varchar_or_int1`,
`varchar` or `uint1` AS `varchar_or_uint1`,
`varchar` or `int2` AS `varchar_or_int2`,
`varchar` or `uint2` AS `varchar_or_uint2`,
`varchar` or `int4` AS `varchar_or_int4`,
`varchar` or `uint4` AS `varchar_or_uint4`,
`varchar` or `int8` AS `varchar_or_int8`,
`varchar` or `uint8` AS `varchar_or_uint8`,
`varchar` or `float4` AS `varchar_or_float4`,
`varchar` or `float8` AS `varchar_or_float8`,
`varchar` or `numeric` AS `varchar_or_numeric`,
`varchar` or `bit1` AS `varchar_or_bit1`,
`varchar` or `bit64` AS `varchar_or_bit64`,
`varchar` or `boolean` AS `varchar_or_boolean`,
`varchar` or `date` AS `varchar_or_date`,
`varchar` or `time` AS `varchar_or_time`,
`varchar` or `time(4)` AS `varchar_or_time(4)`,
`varchar` or `datetime` AS `varchar_or_datetime`,
`varchar` or `datetime(4)` AS `varchar_or_datetime(4)`,
`varchar` or `timestamp` AS `varchar_or_timestamp`,
`varchar` or `timestamp(4)` AS `varchar_or_timestamp(4)`,
`varchar` or `year` AS `varchar_or_year`,
`varchar` or `char` AS `varchar_or_char`,
`varchar` or `varchar` AS `varchar_or_varchar`,
`varchar` or `binary` AS `varchar_or_binary`,
`varchar` or `varbinary` AS `varchar_or_varbinary`,
`varchar` or `tinyblob` AS `varchar_or_tinyblob`,
`varchar` or `blob` AS `varchar_or_blob`,
`varchar` or `mediumblob` AS `varchar_or_mediumblob`,
`varchar` or `longblob` AS `varchar_or_longblob`,
`varchar` or `text` AS `varchar_or_text`,
`varchar` or `enum_t` AS `varchar_or_enum_t`,
`varchar` or `set_t` AS `varchar_or_set_t`,
`varchar` or `json` AS `varchar_or_json`,
`binary` or `int1` AS `binary_or_int1`,
`binary` or `uint1` AS `binary_or_uint1`,
`binary` or `int2` AS `binary_or_int2`,
`binary` or `uint2` AS `binary_or_uint2`,
`binary` or `int4` AS `binary_or_int4`,
`binary` or `uint4` AS `binary_or_uint4`,
`binary` or `int8` AS `binary_or_int8`,
`binary` or `uint8` AS `binary_or_uint8`,
`binary` or `float4` AS `binary_or_float4`,
`binary` or `float8` AS `binary_or_float8`,
`binary` or `numeric` AS `binary_or_numeric`,
`binary` or `bit1` AS `binary_or_bit1`,
`binary` or `bit64` AS `binary_or_bit64`,
`binary` or `boolean` AS `binary_or_boolean`,
`binary` or `date` AS `binary_or_date`,
`binary` or `time` AS `binary_or_time`,
`binary` or `time(4)` AS `binary_or_time(4)`,
`binary` or `datetime` AS `binary_or_datetime`,
`binary` or `datetime(4)` AS `binary_or_datetime(4)`,
`binary` or `timestamp` AS `binary_or_timestamp`,
`binary` or `timestamp(4)` AS `binary_or_timestamp(4)`,
`binary` or `year` AS `binary_or_year`,
`binary` or `char` AS `binary_or_char`,
`binary` or `varchar` AS `binary_or_varchar`,
`binary` or `binary` AS `binary_or_binary`,
`binary` or `varbinary` AS `binary_or_varbinary`,
`binary` or `tinyblob` AS `binary_or_tinyblob`,
`binary` or `blob` AS `binary_or_blob`,
`binary` or `mediumblob` AS `binary_or_mediumblob`,
`binary` or `longblob` AS `binary_or_longblob`,
`binary` or `text` AS `binary_or_text`,
`binary` or `enum_t` AS `binary_or_enum_t`,
`binary` or `set_t` AS `binary_or_set_t`,
`binary` or `json` AS `binary_or_json`,
`varbinary` or `int1` AS `varbinary_or_int1`,
`varbinary` or `uint1` AS `varbinary_or_uint1`,
`varbinary` or `int2` AS `varbinary_or_int2`,
`varbinary` or `uint2` AS `varbinary_or_uint2`,
`varbinary` or `int4` AS `varbinary_or_int4`,
`varbinary` or `uint4` AS `varbinary_or_uint4`,
`varbinary` or `int8` AS `varbinary_or_int8`,
`varbinary` or `uint8` AS `varbinary_or_uint8`,
`varbinary` or `float4` AS `varbinary_or_float4`,
`varbinary` or `float8` AS `varbinary_or_float8`,
`varbinary` or `numeric` AS `varbinary_or_numeric`,
`varbinary` or `bit1` AS `varbinary_or_bit1`,
`varbinary` or `bit64` AS `varbinary_or_bit64`,
`varbinary` or `boolean` AS `varbinary_or_boolean`,
`varbinary` or `date` AS `varbinary_or_date`,
`varbinary` or `time` AS `varbinary_or_time`,
`varbinary` or `time(4)` AS `varbinary_or_time(4)`,
`varbinary` or `datetime` AS `varbinary_or_datetime`,
`varbinary` or `datetime(4)` AS `varbinary_or_datetime(4)`,
`varbinary` or `timestamp` AS `varbinary_or_timestamp`,
`varbinary` or `timestamp(4)` AS `varbinary_or_timestamp(4)`,
`varbinary` or `year` AS `varbinary_or_year`,
`varbinary` or `char` AS `varbinary_or_char`,
`varbinary` or `varchar` AS `varbinary_or_varchar`,
`varbinary` or `binary` AS `varbinary_or_binary`,
`varbinary` or `varbinary` AS `varbinary_or_varbinary`,
`varbinary` or `tinyblob` AS `varbinary_or_tinyblob`,
`varbinary` or `blob` AS `varbinary_or_blob`,
`varbinary` or `mediumblob` AS `varbinary_or_mediumblob`,
`varbinary` or `longblob` AS `varbinary_or_longblob`,
`varbinary` or `text` AS `varbinary_or_text`,
`varbinary` or `enum_t` AS `varbinary_or_enum_t`,
`varbinary` or `set_t` AS `varbinary_or_set_t`,
`varbinary` or `json` AS `varbinary_or_json`,
`tinyblob` or `int1` AS `tinyblob_or_int1`,
`tinyblob` or `uint1` AS `tinyblob_or_uint1`,
`tinyblob` or `int2` AS `tinyblob_or_int2`,
`tinyblob` or `uint2` AS `tinyblob_or_uint2`,
`tinyblob` or `int4` AS `tinyblob_or_int4`,
`tinyblob` or `uint4` AS `tinyblob_or_uint4`,
`tinyblob` or `int8` AS `tinyblob_or_int8`,
`tinyblob` or `uint8` AS `tinyblob_or_uint8`,
`tinyblob` or `float4` AS `tinyblob_or_float4`,
`tinyblob` or `float8` AS `tinyblob_or_float8`,
`tinyblob` or `numeric` AS `tinyblob_or_numeric`,
`tinyblob` or `bit1` AS `tinyblob_or_bit1`,
`tinyblob` or `bit64` AS `tinyblob_or_bit64`,
`tinyblob` or `boolean` AS `tinyblob_or_boolean`,
`tinyblob` or `date` AS `tinyblob_or_date`,
`tinyblob` or `time` AS `tinyblob_or_time`,
`tinyblob` or `time(4)` AS `tinyblob_or_time(4)`,
`tinyblob` or `datetime` AS `tinyblob_or_datetime`,
`tinyblob` or `datetime(4)` AS `tinyblob_or_datetime(4)`,
`tinyblob` or `timestamp` AS `tinyblob_or_timestamp`,
`tinyblob` or `timestamp(4)` AS `tinyblob_or_timestamp(4)`,
`tinyblob` or `year` AS `tinyblob_or_year`,
`tinyblob` or `char` AS `tinyblob_or_char`,
`tinyblob` or `varchar` AS `tinyblob_or_varchar`,
`tinyblob` or `binary` AS `tinyblob_or_binary`,
`tinyblob` or `varbinary` AS `tinyblob_or_varbinary`,
`tinyblob` or `tinyblob` AS `tinyblob_or_tinyblob`,
`tinyblob` or `blob` AS `tinyblob_or_blob`,
`tinyblob` or `mediumblob` AS `tinyblob_or_mediumblob`,
`tinyblob` or `longblob` AS `tinyblob_or_longblob`,
`tinyblob` or `text` AS `tinyblob_or_text`,
`tinyblob` or `enum_t` AS `tinyblob_or_enum_t`,
`tinyblob` or `set_t` AS `tinyblob_or_set_t`,
`tinyblob` or `json` AS `tinyblob_or_json`,
`blob` or `int1` AS `blob_or_int1`,
`blob` or `uint1` AS `blob_or_uint1`,
`blob` or `int2` AS `blob_or_int2`,
`blob` or `uint2` AS `blob_or_uint2`,
`blob` or `int4` AS `blob_or_int4`,
`blob` or `uint4` AS `blob_or_uint4`,
`blob` or `int8` AS `blob_or_int8`,
`blob` or `uint8` AS `blob_or_uint8`,
`blob` or `float4` AS `blob_or_float4`,
`blob` or `float8` AS `blob_or_float8`,
`blob` or `numeric` AS `blob_or_numeric`,
`blob` or `bit1` AS `blob_or_bit1`,
`blob` or `bit64` AS `blob_or_bit64`,
`blob` or `boolean` AS `blob_or_boolean`,
`blob` or `date` AS `blob_or_date`,
`blob` or `time` AS `blob_or_time`,
`blob` or `time(4)` AS `blob_or_time(4)`,
`blob` or `datetime` AS `blob_or_datetime`,
`blob` or `datetime(4)` AS `blob_or_datetime(4)`,
`blob` or `timestamp` AS `blob_or_timestamp`,
`blob` or `timestamp(4)` AS `blob_or_timestamp(4)`,
`blob` or `year` AS `blob_or_year`,
`blob` or `char` AS `blob_or_char`,
`blob` or `varchar` AS `blob_or_varchar`,
`blob` or `binary` AS `blob_or_binary`,
`blob` or `varbinary` AS `blob_or_varbinary`,
`blob` or `tinyblob` AS `blob_or_tinyblob`,
`blob` or `blob` AS `blob_or_blob`,
`blob` or `mediumblob` AS `blob_or_mediumblob`,
`blob` or `longblob` AS `blob_or_longblob`,
`blob` or `text` AS `blob_or_text`,
`blob` or `enum_t` AS `blob_or_enum_t`,
`blob` or `set_t` AS `blob_or_set_t`,
`blob` or `json` AS `blob_or_json`,
`mediumblob` or `int1` AS `mediumblob_or_int1`,
`mediumblob` or `uint1` AS `mediumblob_or_uint1`,
`mediumblob` or `int2` AS `mediumblob_or_int2`,
`mediumblob` or `uint2` AS `mediumblob_or_uint2`,
`mediumblob` or `int4` AS `mediumblob_or_int4`,
`mediumblob` or `uint4` AS `mediumblob_or_uint4`,
`mediumblob` or `int8` AS `mediumblob_or_int8`,
`mediumblob` or `uint8` AS `mediumblob_or_uint8`,
`mediumblob` or `float4` AS `mediumblob_or_float4`,
`mediumblob` or `float8` AS `mediumblob_or_float8`,
`mediumblob` or `numeric` AS `mediumblob_or_numeric`,
`mediumblob` or `bit1` AS `mediumblob_or_bit1`,
`mediumblob` or `bit64` AS `mediumblob_or_bit64`,
`mediumblob` or `boolean` AS `mediumblob_or_boolean`,
`mediumblob` or `date` AS `mediumblob_or_date`,
`mediumblob` or `time` AS `mediumblob_or_time`,
`mediumblob` or `time(4)` AS `mediumblob_or_time(4)`,
`mediumblob` or `datetime` AS `mediumblob_or_datetime`,
`mediumblob` or `datetime(4)` AS `mediumblob_or_datetime(4)`,
`mediumblob` or `timestamp` AS `mediumblob_or_timestamp`,
`mediumblob` or `timestamp(4)` AS `mediumblob_or_timestamp(4)`,
`mediumblob` or `year` AS `mediumblob_or_year`,
`mediumblob` or `char` AS `mediumblob_or_char`,
`mediumblob` or `varchar` AS `mediumblob_or_varchar`,
`mediumblob` or `binary` AS `mediumblob_or_binary`,
`mediumblob` or `varbinary` AS `mediumblob_or_varbinary`,
`mediumblob` or `tinyblob` AS `mediumblob_or_tinyblob`,
`mediumblob` or `blob` AS `mediumblob_or_blob`,
`mediumblob` or `mediumblob` AS `mediumblob_or_mediumblob`,
`mediumblob` or `longblob` AS `mediumblob_or_longblob`,
`mediumblob` or `text` AS `mediumblob_or_text`,
`mediumblob` or `enum_t` AS `mediumblob_or_enum_t`,
`mediumblob` or `set_t` AS `mediumblob_or_set_t`,
`mediumblob` or `json` AS `mediumblob_or_json`,
`longblob` or `int1` AS `longblob_or_int1`,
`longblob` or `uint1` AS `longblob_or_uint1`,
`longblob` or `int2` AS `longblob_or_int2`,
`longblob` or `uint2` AS `longblob_or_uint2`,
`longblob` or `int4` AS `longblob_or_int4`,
`longblob` or `uint4` AS `longblob_or_uint4`,
`longblob` or `int8` AS `longblob_or_int8`,
`longblob` or `uint8` AS `longblob_or_uint8`,
`longblob` or `float4` AS `longblob_or_float4`,
`longblob` or `float8` AS `longblob_or_float8`,
`longblob` or `numeric` AS `longblob_or_numeric`,
`longblob` or `bit1` AS `longblob_or_bit1`,
`longblob` or `bit64` AS `longblob_or_bit64`,
`longblob` or `boolean` AS `longblob_or_boolean`,
`longblob` or `date` AS `longblob_or_date`,
`longblob` or `time` AS `longblob_or_time`,
`longblob` or `time(4)` AS `longblob_or_time(4)`,
`longblob` or `datetime` AS `longblob_or_datetime`,
`longblob` or `datetime(4)` AS `longblob_or_datetime(4)`,
`longblob` or `timestamp` AS `longblob_or_timestamp`,
`longblob` or `timestamp(4)` AS `longblob_or_timestamp(4)`,
`longblob` or `year` AS `longblob_or_year`,
`longblob` or `char` AS `longblob_or_char`,
`longblob` or `varchar` AS `longblob_or_varchar`,
`longblob` or `binary` AS `longblob_or_binary`,
`longblob` or `varbinary` AS `longblob_or_varbinary`,
`longblob` or `tinyblob` AS `longblob_or_tinyblob`,
`longblob` or `blob` AS `longblob_or_blob`,
`longblob` or `mediumblob` AS `longblob_or_mediumblob`,
`longblob` or `longblob` AS `longblob_or_longblob`,
`longblob` or `text` AS `longblob_or_text`,
`longblob` or `enum_t` AS `longblob_or_enum_t`,
`longblob` or `set_t` AS `longblob_or_set_t`,
`longblob` or `json` AS `longblob_or_json`,
`text` or `int1` AS `text_or_int1`,
`text` or `uint1` AS `text_or_uint1`,
`text` or `int2` AS `text_or_int2`,
`text` or `uint2` AS `text_or_uint2`,
`text` or `int4` AS `text_or_int4`,
`text` or `uint4` AS `text_or_uint4`,
`text` or `int8` AS `text_or_int8`,
`text` or `uint8` AS `text_or_uint8`,
`text` or `float4` AS `text_or_float4`,
`text` or `float8` AS `text_or_float8`,
`text` or `numeric` AS `text_or_numeric`,
`text` or `bit1` AS `text_or_bit1`,
`text` or `bit64` AS `text_or_bit64`,
`text` or `boolean` AS `text_or_boolean`,
`text` or `date` AS `text_or_date`,
`text` or `time` AS `text_or_time`,
`text` or `time(4)` AS `text_or_time(4)`,
`text` or `datetime` AS `text_or_datetime`,
`text` or `datetime(4)` AS `text_or_datetime(4)`,
`text` or `timestamp` AS `text_or_timestamp`,
`text` or `timestamp(4)` AS `text_or_timestamp(4)`,
`text` or `year` AS `text_or_year`,
`text` or `char` AS `text_or_char`,
`text` or `varchar` AS `text_or_varchar`,
`text` or `binary` AS `text_or_binary`,
`text` or `varbinary` AS `text_or_varbinary`,
`text` or `tinyblob` AS `text_or_tinyblob`,
`text` or `blob` AS `text_or_blob`,
`text` or `mediumblob` AS `text_or_mediumblob`,
`text` or `longblob` AS `text_or_longblob`,
`text` or `text` AS `text_or_text`,
`text` or `enum_t` AS `text_or_enum_t`,
`text` or `set_t` AS `text_or_set_t`,
`text` or `json` AS `text_or_json`,
`enum_t` or `int1` AS `enum_t_or_int1`,
`enum_t` or `uint1` AS `enum_t_or_uint1`,
`enum_t` or `int2` AS `enum_t_or_int2`,
`enum_t` or `uint2` AS `enum_t_or_uint2`,
`enum_t` or `int4` AS `enum_t_or_int4`,
`enum_t` or `uint4` AS `enum_t_or_uint4`,
`enum_t` or `int8` AS `enum_t_or_int8`,
`enum_t` or `uint8` AS `enum_t_or_uint8`,
`enum_t` or `float4` AS `enum_t_or_float4`,
`enum_t` or `float8` AS `enum_t_or_float8`,
`enum_t` or `numeric` AS `enum_t_or_numeric`,
`enum_t` or `bit1` AS `enum_t_or_bit1`,
`enum_t` or `bit64` AS `enum_t_or_bit64`,
`enum_t` or `boolean` AS `enum_t_or_boolean`,
`enum_t` or `date` AS `enum_t_or_date`,
`enum_t` or `time` AS `enum_t_or_time`,
`enum_t` or `time(4)` AS `enum_t_or_time(4)`,
`enum_t` or `datetime` AS `enum_t_or_datetime`,
`enum_t` or `datetime(4)` AS `enum_t_or_datetime(4)`,
`enum_t` or `timestamp` AS `enum_t_or_timestamp`,
`enum_t` or `timestamp(4)` AS `enum_t_or_timestamp(4)`,
`enum_t` or `year` AS `enum_t_or_year`,
`enum_t` or `char` AS `enum_t_or_char`,
`enum_t` or `varchar` AS `enum_t_or_varchar`,
`enum_t` or `binary` AS `enum_t_or_binary`,
`enum_t` or `varbinary` AS `enum_t_or_varbinary`,
`enum_t` or `tinyblob` AS `enum_t_or_tinyblob`,
`enum_t` or `blob` AS `enum_t_or_blob`,
`enum_t` or `mediumblob` AS `enum_t_or_mediumblob`,
`enum_t` or `longblob` AS `enum_t_or_longblob`,
`enum_t` or `text` AS `enum_t_or_text`,
`enum_t` or `enum_t` AS `enum_t_or_enum_t`,
`enum_t` or `set_t` AS `enum_t_or_set_t`,
`enum_t` or `json` AS `enum_t_or_json`,
`set_t` or `int1` AS `set_t_or_int1`,
`set_t` or `uint1` AS `set_t_or_uint1`,
`set_t` or `int2` AS `set_t_or_int2`,
`set_t` or `uint2` AS `set_t_or_uint2`,
`set_t` or `int4` AS `set_t_or_int4`,
`set_t` or `uint4` AS `set_t_or_uint4`,
`set_t` or `int8` AS `set_t_or_int8`,
`set_t` or `uint8` AS `set_t_or_uint8`,
`set_t` or `float4` AS `set_t_or_float4`,
`set_t` or `float8` AS `set_t_or_float8`,
`set_t` or `numeric` AS `set_t_or_numeric`,
`set_t` or `bit1` AS `set_t_or_bit1`,
`set_t` or `bit64` AS `set_t_or_bit64`,
`set_t` or `boolean` AS `set_t_or_boolean`,
`set_t` or `date` AS `set_t_or_date`,
`set_t` or `time` AS `set_t_or_time`,
`set_t` or `time(4)` AS `set_t_or_time(4)`,
`set_t` or `datetime` AS `set_t_or_datetime`,
`set_t` or `datetime(4)` AS `set_t_or_datetime(4)`,
`set_t` or `timestamp` AS `set_t_or_timestamp`,
`set_t` or `timestamp(4)` AS `set_t_or_timestamp(4)`,
`set_t` or `year` AS `set_t_or_year`,
`set_t` or `char` AS `set_t_or_char`,
`set_t` or `varchar` AS `set_t_or_varchar`,
`set_t` or `binary` AS `set_t_or_binary`,
`set_t` or `varbinary` AS `set_t_or_varbinary`,
`set_t` or `tinyblob` AS `set_t_or_tinyblob`,
`set_t` or `blob` AS `set_t_or_blob`,
`set_t` or `mediumblob` AS `set_t_or_mediumblob`,
`set_t` or `longblob` AS `set_t_or_longblob`,
`set_t` or `text` AS `set_t_or_text`,
`set_t` or `enum_t` AS `set_t_or_enum_t`,
`set_t` or `set_t` AS `set_t_or_set_t`,
`set_t` or `json` AS `set_t_or_json`,
`json` or `int1` AS `json_or_int1`,
`json` or `uint1` AS `json_or_uint1`,
`json` or `int2` AS `json_or_int2`,
`json` or `uint2` AS `json_or_uint2`,
`json` or `int4` AS `json_or_int4`,
`json` or `uint4` AS `json_or_uint4`,
`json` or `int8` AS `json_or_int8`,
`json` or `uint8` AS `json_or_uint8`,
`json` or `float4` AS `json_or_float4`,
`json` or `float8` AS `json_or_float8`,
`json` or `numeric` AS `json_or_numeric`,
`json` or `bit1` AS `json_or_bit1`,
`json` or `bit64` AS `json_or_bit64`,
`json` or `boolean` AS `json_or_boolean`,
`json` or `date` AS `json_or_date`,
`json` or `time` AS `json_or_time`,
`json` or `time(4)` AS `json_or_time(4)`,
`json` or `datetime` AS `json_or_datetime`,
`json` or `datetime(4)` AS `json_or_datetime(4)`,
`json` or `timestamp` AS `json_or_timestamp`,
`json` or `timestamp(4)` AS `json_or_timestamp(4)`,
`json` or `year` AS `json_or_year`,
`json` or `char` AS `json_or_char`,
`json` or `varchar` AS `json_or_varchar`,
`json` or `binary` AS `json_or_binary`,
`json` or `varbinary` AS `json_or_varbinary`,
`json` or `tinyblob` AS `json_or_tinyblob`,
`json` or `blob` AS `json_or_blob`,
`json` or `mediumblob` AS `json_or_mediumblob`,
`json` or `longblob` AS `json_or_longblob`,
`json` or `text` AS `json_or_text`,
`json` or `enum_t` AS `json_or_enum_t`,
`json` or `set_t` AS `json_or_set_t`,
`json` or `json` AS `json_or_json`
FROM test_multi_type_table;
SHOW COLUMNS FROM test_multi_type;
-- test value
delete from test_multi_type;
delete from test_multi_type_table;
insert into test_multi_type values(null);
insert into test_multi_type_table values(1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                                   b'1', b'111', true,
                                   '2023-02-05', '19:10:50', '19:10:50.3456', '2023-02-05 19:10:50', '2023-02-05 19:10:50.456', '2023-02-05 19:10:50', '2023-02-05 19:10:50.456', '2023',
                                   '1.23a', '1.23a', '1.23a', '1.23a', '1.23a', '1.23a', '1.23a', '1.23a', '1.23a',
                                   'a', 'a,c',
                                   json_object('a', 1, 'b', 2));
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int1_or_int1` = test_multi_type_table.`int1` or test_multi_type_table.`int1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int1_or_uint1` = test_multi_type_table.`int1` or test_multi_type_table.`uint1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int1_or_int2` = test_multi_type_table.`int1` or test_multi_type_table.`int2`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int1_or_uint2` = test_multi_type_table.`int1` or test_multi_type_table.`uint2`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int1_or_int4` = test_multi_type_table.`int1` or test_multi_type_table.`int4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int1_or_uint4` = test_multi_type_table.`int1` or test_multi_type_table.`uint4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int1_or_int8` = test_multi_type_table.`int1` or test_multi_type_table.`int8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int1_or_uint8` = test_multi_type_table.`int1` or test_multi_type_table.`uint8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int1_or_float4` = test_multi_type_table.`int1` or test_multi_type_table.`float4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int1_or_float8` = test_multi_type_table.`int1` or test_multi_type_table.`float8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int1_or_numeric` = test_multi_type_table.`int1` or test_multi_type_table.`numeric`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int1_or_bit1` = test_multi_type_table.`int1` or test_multi_type_table.`bit1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int1_or_bit64` = test_multi_type_table.`int1` or test_multi_type_table.`bit64`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int1_or_boolean` = test_multi_type_table.`int1` or test_multi_type_table.`boolean`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int1_or_date` = test_multi_type_table.`int1` or test_multi_type_table.`date`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int1_or_time` = test_multi_type_table.`int1` or test_multi_type_table.`time`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int1_or_time(4)` = test_multi_type_table.`int1` or test_multi_type_table.`time(4)`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int1_or_datetime` = test_multi_type_table.`int1` or test_multi_type_table.`datetime`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int1_or_datetime(4)` = test_multi_type_table.`int1` or test_multi_type_table.`datetime(4)`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int1_or_timestamp` = test_multi_type_table.`int1` or test_multi_type_table.`timestamp`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int1_or_timestamp(4)` = test_multi_type_table.`int1` or test_multi_type_table.`timestamp(4)`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int1_or_year` = test_multi_type_table.`int1` or test_multi_type_table.`year`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int1_or_char` = test_multi_type_table.`int1` or test_multi_type_table.`char`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int1_or_varchar` = test_multi_type_table.`int1` or test_multi_type_table.`varchar`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int1_or_binary` = test_multi_type_table.`int1` or test_multi_type_table.`binary`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int1_or_varbinary` = test_multi_type_table.`int1` or test_multi_type_table.`varbinary`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int1_or_tinyblob` = test_multi_type_table.`int1` or test_multi_type_table.`tinyblob`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int1_or_blob` = test_multi_type_table.`int1` or test_multi_type_table.`blob`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int1_or_mediumblob` = test_multi_type_table.`int1` or test_multi_type_table.`mediumblob`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int1_or_longblob` = test_multi_type_table.`int1` or test_multi_type_table.`longblob`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int1_or_text` = test_multi_type_table.`int1` or test_multi_type_table.`text`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int1_or_enum_t` = test_multi_type_table.`int1` or test_multi_type_table.`enum_t`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int1_or_set_t` = test_multi_type_table.`int1` or test_multi_type_table.`set_t`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int1_or_json` = test_multi_type_table.`int1` or test_multi_type_table.`json`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint1_or_int1` = test_multi_type_table.`uint1` or test_multi_type_table.`int1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint1_or_uint1` = test_multi_type_table.`uint1` or test_multi_type_table.`uint1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint1_or_int2` = test_multi_type_table.`uint1` or test_multi_type_table.`int2`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint1_or_uint2` = test_multi_type_table.`uint1` or test_multi_type_table.`uint2`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint1_or_int4` = test_multi_type_table.`uint1` or test_multi_type_table.`int4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint1_or_uint4` = test_multi_type_table.`uint1` or test_multi_type_table.`uint4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint1_or_int8` = test_multi_type_table.`uint1` or test_multi_type_table.`int8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint1_or_uint8` = test_multi_type_table.`uint1` or test_multi_type_table.`uint8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint1_or_float4` = test_multi_type_table.`uint1` or test_multi_type_table.`float4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint1_or_float8` = test_multi_type_table.`uint1` or test_multi_type_table.`float8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint1_or_numeric` = test_multi_type_table.`uint1` or test_multi_type_table.`numeric`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint1_or_bit1` = test_multi_type_table.`uint1` or test_multi_type_table.`bit1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint1_or_bit64` = test_multi_type_table.`uint1` or test_multi_type_table.`bit64`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint1_or_boolean` = test_multi_type_table.`uint1` or test_multi_type_table.`boolean`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint1_or_date` = test_multi_type_table.`uint1` or test_multi_type_table.`date`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint1_or_time` = test_multi_type_table.`uint1` or test_multi_type_table.`time`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint1_or_time(4)` = test_multi_type_table.`uint1` or test_multi_type_table.`time(4)`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint1_or_datetime` = test_multi_type_table.`uint1` or test_multi_type_table.`datetime`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint1_or_datetime(4)` = test_multi_type_table.`uint1` or test_multi_type_table.`datetime(4)`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint1_or_timestamp` = test_multi_type_table.`uint1` or test_multi_type_table.`timestamp`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint1_or_timestamp(4)` = test_multi_type_table.`uint1` or test_multi_type_table.`timestamp(4)`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint1_or_year` = test_multi_type_table.`uint1` or test_multi_type_table.`year`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint1_or_char` = test_multi_type_table.`uint1` or test_multi_type_table.`char`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint1_or_varchar` = test_multi_type_table.`uint1` or test_multi_type_table.`varchar`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint1_or_binary` = test_multi_type_table.`uint1` or test_multi_type_table.`binary`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint1_or_varbinary` = test_multi_type_table.`uint1` or test_multi_type_table.`varbinary`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint1_or_tinyblob` = test_multi_type_table.`uint1` or test_multi_type_table.`tinyblob`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint1_or_blob` = test_multi_type_table.`uint1` or test_multi_type_table.`blob`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint1_or_mediumblob` = test_multi_type_table.`uint1` or test_multi_type_table.`mediumblob`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint1_or_longblob` = test_multi_type_table.`uint1` or test_multi_type_table.`longblob`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint1_or_text` = test_multi_type_table.`uint1` or test_multi_type_table.`text`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint1_or_enum_t` = test_multi_type_table.`uint1` or test_multi_type_table.`enum_t`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint1_or_set_t` = test_multi_type_table.`uint1` or test_multi_type_table.`set_t`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint1_or_json` = test_multi_type_table.`uint1` or test_multi_type_table.`json`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int2_or_int1` = test_multi_type_table.`int2` or test_multi_type_table.`int1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int2_or_uint1` = test_multi_type_table.`int2` or test_multi_type_table.`uint1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int2_or_int2` = test_multi_type_table.`int2` or test_multi_type_table.`int2`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int2_or_uint2` = test_multi_type_table.`int2` or test_multi_type_table.`uint2`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int2_or_int4` = test_multi_type_table.`int2` or test_multi_type_table.`int4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int2_or_uint4` = test_multi_type_table.`int2` or test_multi_type_table.`uint4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int2_or_int8` = test_multi_type_table.`int2` or test_multi_type_table.`int8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int2_or_uint8` = test_multi_type_table.`int2` or test_multi_type_table.`uint8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int2_or_float4` = test_multi_type_table.`int2` or test_multi_type_table.`float4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int2_or_float8` = test_multi_type_table.`int2` or test_multi_type_table.`float8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int2_or_numeric` = test_multi_type_table.`int2` or test_multi_type_table.`numeric`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int2_or_bit1` = test_multi_type_table.`int2` or test_multi_type_table.`bit1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int2_or_bit64` = test_multi_type_table.`int2` or test_multi_type_table.`bit64`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int2_or_boolean` = test_multi_type_table.`int2` or test_multi_type_table.`boolean`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int2_or_date` = test_multi_type_table.`int2` or test_multi_type_table.`date`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int2_or_time` = test_multi_type_table.`int2` or test_multi_type_table.`time`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int2_or_time(4)` = test_multi_type_table.`int2` or test_multi_type_table.`time(4)`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int2_or_datetime` = test_multi_type_table.`int2` or test_multi_type_table.`datetime`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int2_or_datetime(4)` = test_multi_type_table.`int2` or test_multi_type_table.`datetime(4)`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int2_or_timestamp` = test_multi_type_table.`int2` or test_multi_type_table.`timestamp`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int2_or_timestamp(4)` = test_multi_type_table.`int2` or test_multi_type_table.`timestamp(4)`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int2_or_year` = test_multi_type_table.`int2` or test_multi_type_table.`year`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int2_or_char` = test_multi_type_table.`int2` or test_multi_type_table.`char`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int2_or_varchar` = test_multi_type_table.`int2` or test_multi_type_table.`varchar`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int2_or_binary` = test_multi_type_table.`int2` or test_multi_type_table.`binary`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int2_or_varbinary` = test_multi_type_table.`int2` or test_multi_type_table.`varbinary`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int2_or_tinyblob` = test_multi_type_table.`int2` or test_multi_type_table.`tinyblob`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int2_or_blob` = test_multi_type_table.`int2` or test_multi_type_table.`blob`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int2_or_mediumblob` = test_multi_type_table.`int2` or test_multi_type_table.`mediumblob`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int2_or_longblob` = test_multi_type_table.`int2` or test_multi_type_table.`longblob`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int2_or_text` = test_multi_type_table.`int2` or test_multi_type_table.`text`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int2_or_enum_t` = test_multi_type_table.`int2` or test_multi_type_table.`enum_t`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int2_or_set_t` = test_multi_type_table.`int2` or test_multi_type_table.`set_t`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int2_or_json` = test_multi_type_table.`int2` or test_multi_type_table.`json`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint2_or_int1` = test_multi_type_table.`uint2` or test_multi_type_table.`int1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint2_or_uint1` = test_multi_type_table.`uint2` or test_multi_type_table.`uint1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint2_or_int2` = test_multi_type_table.`uint2` or test_multi_type_table.`int2`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint2_or_uint2` = test_multi_type_table.`uint2` or test_multi_type_table.`uint2`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint2_or_int4` = test_multi_type_table.`uint2` or test_multi_type_table.`int4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint2_or_uint4` = test_multi_type_table.`uint2` or test_multi_type_table.`uint4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint2_or_int8` = test_multi_type_table.`uint2` or test_multi_type_table.`int8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint2_or_uint8` = test_multi_type_table.`uint2` or test_multi_type_table.`uint8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint2_or_float4` = test_multi_type_table.`uint2` or test_multi_type_table.`float4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint2_or_float8` = test_multi_type_table.`uint2` or test_multi_type_table.`float8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint2_or_numeric` = test_multi_type_table.`uint2` or test_multi_type_table.`numeric`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint2_or_bit1` = test_multi_type_table.`uint2` or test_multi_type_table.`bit1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint2_or_bit64` = test_multi_type_table.`uint2` or test_multi_type_table.`bit64`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint2_or_boolean` = test_multi_type_table.`uint2` or test_multi_type_table.`boolean`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint2_or_date` = test_multi_type_table.`uint2` or test_multi_type_table.`date`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint2_or_time` = test_multi_type_table.`uint2` or test_multi_type_table.`time`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint2_or_time(4)` = test_multi_type_table.`uint2` or test_multi_type_table.`time(4)`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint2_or_datetime` = test_multi_type_table.`uint2` or test_multi_type_table.`datetime`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint2_or_datetime(4)` = test_multi_type_table.`uint2` or test_multi_type_table.`datetime(4)`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint2_or_timestamp` = test_multi_type_table.`uint2` or test_multi_type_table.`timestamp`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint2_or_timestamp(4)` = test_multi_type_table.`uint2` or test_multi_type_table.`timestamp(4)`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint2_or_year` = test_multi_type_table.`uint2` or test_multi_type_table.`year`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint2_or_char` = test_multi_type_table.`uint2` or test_multi_type_table.`char`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint2_or_varchar` = test_multi_type_table.`uint2` or test_multi_type_table.`varchar`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint2_or_binary` = test_multi_type_table.`uint2` or test_multi_type_table.`binary`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint2_or_varbinary` = test_multi_type_table.`uint2` or test_multi_type_table.`varbinary`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint2_or_tinyblob` = test_multi_type_table.`uint2` or test_multi_type_table.`tinyblob`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint2_or_blob` = test_multi_type_table.`uint2` or test_multi_type_table.`blob`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint2_or_mediumblob` = test_multi_type_table.`uint2` or test_multi_type_table.`mediumblob`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint2_or_longblob` = test_multi_type_table.`uint2` or test_multi_type_table.`longblob`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint2_or_text` = test_multi_type_table.`uint2` or test_multi_type_table.`text`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint2_or_enum_t` = test_multi_type_table.`uint2` or test_multi_type_table.`enum_t`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint2_or_set_t` = test_multi_type_table.`uint2` or test_multi_type_table.`set_t`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint2_or_json` = test_multi_type_table.`uint2` or test_multi_type_table.`json`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int4_or_int1` = test_multi_type_table.`int4` or test_multi_type_table.`int1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int4_or_uint1` = test_multi_type_table.`int4` or test_multi_type_table.`uint1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int4_or_int2` = test_multi_type_table.`int4` or test_multi_type_table.`int2`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int4_or_uint2` = test_multi_type_table.`int4` or test_multi_type_table.`uint2`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int4_or_int4` = test_multi_type_table.`int4` or test_multi_type_table.`int4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int4_or_uint4` = test_multi_type_table.`int4` or test_multi_type_table.`uint4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int4_or_int8` = test_multi_type_table.`int4` or test_multi_type_table.`int8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int4_or_uint8` = test_multi_type_table.`int4` or test_multi_type_table.`uint8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int4_or_float4` = test_multi_type_table.`int4` or test_multi_type_table.`float4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int4_or_float8` = test_multi_type_table.`int4` or test_multi_type_table.`float8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int4_or_numeric` = test_multi_type_table.`int4` or test_multi_type_table.`numeric`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int4_or_bit1` = test_multi_type_table.`int4` or test_multi_type_table.`bit1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int4_or_bit64` = test_multi_type_table.`int4` or test_multi_type_table.`bit64`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int4_or_boolean` = test_multi_type_table.`int4` or test_multi_type_table.`boolean`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int4_or_date` = test_multi_type_table.`int4` or test_multi_type_table.`date`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int4_or_time` = test_multi_type_table.`int4` or test_multi_type_table.`time`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int4_or_time(4)` = test_multi_type_table.`int4` or test_multi_type_table.`time(4)`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int4_or_datetime` = test_multi_type_table.`int4` or test_multi_type_table.`datetime`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int4_or_datetime(4)` = test_multi_type_table.`int4` or test_multi_type_table.`datetime(4)`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int4_or_timestamp` = test_multi_type_table.`int4` or test_multi_type_table.`timestamp`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int4_or_timestamp(4)` = test_multi_type_table.`int4` or test_multi_type_table.`timestamp(4)`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int4_or_year` = test_multi_type_table.`int4` or test_multi_type_table.`year`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int4_or_char` = test_multi_type_table.`int4` or test_multi_type_table.`char`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int4_or_varchar` = test_multi_type_table.`int4` or test_multi_type_table.`varchar`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int4_or_binary` = test_multi_type_table.`int4` or test_multi_type_table.`binary`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int4_or_varbinary` = test_multi_type_table.`int4` or test_multi_type_table.`varbinary`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int4_or_tinyblob` = test_multi_type_table.`int4` or test_multi_type_table.`tinyblob`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int4_or_blob` = test_multi_type_table.`int4` or test_multi_type_table.`blob`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int4_or_mediumblob` = test_multi_type_table.`int4` or test_multi_type_table.`mediumblob`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int4_or_longblob` = test_multi_type_table.`int4` or test_multi_type_table.`longblob`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int4_or_text` = test_multi_type_table.`int4` or test_multi_type_table.`text`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int4_or_enum_t` = test_multi_type_table.`int4` or test_multi_type_table.`enum_t`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int4_or_set_t` = test_multi_type_table.`int4` or test_multi_type_table.`set_t`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int4_or_json` = test_multi_type_table.`int4` or test_multi_type_table.`json`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint4_or_int1` = test_multi_type_table.`uint4` or test_multi_type_table.`int1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint4_or_uint1` = test_multi_type_table.`uint4` or test_multi_type_table.`uint1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint4_or_int2` = test_multi_type_table.`uint4` or test_multi_type_table.`int2`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint4_or_uint2` = test_multi_type_table.`uint4` or test_multi_type_table.`uint2`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint4_or_int4` = test_multi_type_table.`uint4` or test_multi_type_table.`int4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint4_or_uint4` = test_multi_type_table.`uint4` or test_multi_type_table.`uint4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint4_or_int8` = test_multi_type_table.`uint4` or test_multi_type_table.`int8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint4_or_uint8` = test_multi_type_table.`uint4` or test_multi_type_table.`uint8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint4_or_float4` = test_multi_type_table.`uint4` or test_multi_type_table.`float4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint4_or_float8` = test_multi_type_table.`uint4` or test_multi_type_table.`float8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint4_or_numeric` = test_multi_type_table.`uint4` or test_multi_type_table.`numeric`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint4_or_bit1` = test_multi_type_table.`uint4` or test_multi_type_table.`bit1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint4_or_bit64` = test_multi_type_table.`uint4` or test_multi_type_table.`bit64`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint4_or_boolean` = test_multi_type_table.`uint4` or test_multi_type_table.`boolean`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint4_or_date` = test_multi_type_table.`uint4` or test_multi_type_table.`date`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint4_or_time` = test_multi_type_table.`uint4` or test_multi_type_table.`time`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint4_or_time(4)` = test_multi_type_table.`uint4` or test_multi_type_table.`time(4)`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint4_or_datetime` = test_multi_type_table.`uint4` or test_multi_type_table.`datetime`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint4_or_datetime(4)` = test_multi_type_table.`uint4` or test_multi_type_table.`datetime(4)`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint4_or_timestamp` = test_multi_type_table.`uint4` or test_multi_type_table.`timestamp`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint4_or_timestamp(4)` = test_multi_type_table.`uint4` or test_multi_type_table.`timestamp(4)`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint4_or_year` = test_multi_type_table.`uint4` or test_multi_type_table.`year`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint4_or_char` = test_multi_type_table.`uint4` or test_multi_type_table.`char`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint4_or_varchar` = test_multi_type_table.`uint4` or test_multi_type_table.`varchar`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint4_or_binary` = test_multi_type_table.`uint4` or test_multi_type_table.`binary`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint4_or_varbinary` = test_multi_type_table.`uint4` or test_multi_type_table.`varbinary`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint4_or_tinyblob` = test_multi_type_table.`uint4` or test_multi_type_table.`tinyblob`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint4_or_blob` = test_multi_type_table.`uint4` or test_multi_type_table.`blob`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint4_or_mediumblob` = test_multi_type_table.`uint4` or test_multi_type_table.`mediumblob`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint4_or_longblob` = test_multi_type_table.`uint4` or test_multi_type_table.`longblob`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint4_or_text` = test_multi_type_table.`uint4` or test_multi_type_table.`text`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint4_or_enum_t` = test_multi_type_table.`uint4` or test_multi_type_table.`enum_t`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint4_or_set_t` = test_multi_type_table.`uint4` or test_multi_type_table.`set_t`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint4_or_json` = test_multi_type_table.`uint4` or test_multi_type_table.`json`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int8_or_int1` = test_multi_type_table.`int8` or test_multi_type_table.`int1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int8_or_uint1` = test_multi_type_table.`int8` or test_multi_type_table.`uint1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int8_or_int2` = test_multi_type_table.`int8` or test_multi_type_table.`int2`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int8_or_uint2` = test_multi_type_table.`int8` or test_multi_type_table.`uint2`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int8_or_int4` = test_multi_type_table.`int8` or test_multi_type_table.`int4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int8_or_uint4` = test_multi_type_table.`int8` or test_multi_type_table.`uint4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int8_or_int8` = test_multi_type_table.`int8` or test_multi_type_table.`int8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int8_or_uint8` = test_multi_type_table.`int8` or test_multi_type_table.`uint8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int8_or_float4` = test_multi_type_table.`int8` or test_multi_type_table.`float4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int8_or_float8` = test_multi_type_table.`int8` or test_multi_type_table.`float8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int8_or_numeric` = test_multi_type_table.`int8` or test_multi_type_table.`numeric`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int8_or_bit1` = test_multi_type_table.`int8` or test_multi_type_table.`bit1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int8_or_bit64` = test_multi_type_table.`int8` or test_multi_type_table.`bit64`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int8_or_boolean` = test_multi_type_table.`int8` or test_multi_type_table.`boolean`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int8_or_date` = test_multi_type_table.`int8` or test_multi_type_table.`date`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int8_or_time` = test_multi_type_table.`int8` or test_multi_type_table.`time`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int8_or_time(4)` = test_multi_type_table.`int8` or test_multi_type_table.`time(4)`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int8_or_datetime` = test_multi_type_table.`int8` or test_multi_type_table.`datetime`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int8_or_datetime(4)` = test_multi_type_table.`int8` or test_multi_type_table.`datetime(4)`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int8_or_timestamp` = test_multi_type_table.`int8` or test_multi_type_table.`timestamp`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int8_or_timestamp(4)` = test_multi_type_table.`int8` or test_multi_type_table.`timestamp(4)`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int8_or_year` = test_multi_type_table.`int8` or test_multi_type_table.`year`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int8_or_char` = test_multi_type_table.`int8` or test_multi_type_table.`char`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int8_or_varchar` = test_multi_type_table.`int8` or test_multi_type_table.`varchar`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int8_or_binary` = test_multi_type_table.`int8` or test_multi_type_table.`binary`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int8_or_varbinary` = test_multi_type_table.`int8` or test_multi_type_table.`varbinary`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int8_or_tinyblob` = test_multi_type_table.`int8` or test_multi_type_table.`tinyblob`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int8_or_blob` = test_multi_type_table.`int8` or test_multi_type_table.`blob`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int8_or_mediumblob` = test_multi_type_table.`int8` or test_multi_type_table.`mediumblob`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int8_or_longblob` = test_multi_type_table.`int8` or test_multi_type_table.`longblob`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int8_or_text` = test_multi_type_table.`int8` or test_multi_type_table.`text`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int8_or_enum_t` = test_multi_type_table.`int8` or test_multi_type_table.`enum_t`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int8_or_set_t` = test_multi_type_table.`int8` or test_multi_type_table.`set_t`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int8_or_json` = test_multi_type_table.`int8` or test_multi_type_table.`json`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint8_or_int1` = test_multi_type_table.`uint8` or test_multi_type_table.`int1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint8_or_uint1` = test_multi_type_table.`uint8` or test_multi_type_table.`uint1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint8_or_int2` = test_multi_type_table.`uint8` or test_multi_type_table.`int2`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint8_or_uint2` = test_multi_type_table.`uint8` or test_multi_type_table.`uint2`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint8_or_int4` = test_multi_type_table.`uint8` or test_multi_type_table.`int4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint8_or_uint4` = test_multi_type_table.`uint8` or test_multi_type_table.`uint4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint8_or_int8` = test_multi_type_table.`uint8` or test_multi_type_table.`int8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint8_or_uint8` = test_multi_type_table.`uint8` or test_multi_type_table.`uint8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint8_or_float4` = test_multi_type_table.`uint8` or test_multi_type_table.`float4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint8_or_float8` = test_multi_type_table.`uint8` or test_multi_type_table.`float8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint8_or_numeric` = test_multi_type_table.`uint8` or test_multi_type_table.`numeric`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint8_or_bit1` = test_multi_type_table.`uint8` or test_multi_type_table.`bit1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint8_or_bit64` = test_multi_type_table.`uint8` or test_multi_type_table.`bit64`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint8_or_boolean` = test_multi_type_table.`uint8` or test_multi_type_table.`boolean`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint8_or_date` = test_multi_type_table.`uint8` or test_multi_type_table.`date`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint8_or_time` = test_multi_type_table.`uint8` or test_multi_type_table.`time`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint8_or_time(4)` = test_multi_type_table.`uint8` or test_multi_type_table.`time(4)`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint8_or_datetime` = test_multi_type_table.`uint8` or test_multi_type_table.`datetime`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint8_or_datetime(4)` = test_multi_type_table.`uint8` or test_multi_type_table.`datetime(4)`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint8_or_timestamp` = test_multi_type_table.`uint8` or test_multi_type_table.`timestamp`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint8_or_timestamp(4)` = test_multi_type_table.`uint8` or test_multi_type_table.`timestamp(4)`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint8_or_year` = test_multi_type_table.`uint8` or test_multi_type_table.`year`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint8_or_char` = test_multi_type_table.`uint8` or test_multi_type_table.`char`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint8_or_varchar` = test_multi_type_table.`uint8` or test_multi_type_table.`varchar`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint8_or_binary` = test_multi_type_table.`uint8` or test_multi_type_table.`binary`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint8_or_varbinary` = test_multi_type_table.`uint8` or test_multi_type_table.`varbinary`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint8_or_tinyblob` = test_multi_type_table.`uint8` or test_multi_type_table.`tinyblob`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint8_or_blob` = test_multi_type_table.`uint8` or test_multi_type_table.`blob`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint8_or_mediumblob` = test_multi_type_table.`uint8` or test_multi_type_table.`mediumblob`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint8_or_longblob` = test_multi_type_table.`uint8` or test_multi_type_table.`longblob`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint8_or_text` = test_multi_type_table.`uint8` or test_multi_type_table.`text`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint8_or_enum_t` = test_multi_type_table.`uint8` or test_multi_type_table.`enum_t`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint8_or_set_t` = test_multi_type_table.`uint8` or test_multi_type_table.`set_t`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint8_or_json` = test_multi_type_table.`uint8` or test_multi_type_table.`json`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`float4_or_int1` = test_multi_type_table.`float4` or test_multi_type_table.`int1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`float4_or_uint1` = test_multi_type_table.`float4` or test_multi_type_table.`uint1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`float4_or_int2` = test_multi_type_table.`float4` or test_multi_type_table.`int2`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`float4_or_uint2` = test_multi_type_table.`float4` or test_multi_type_table.`uint2`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`float4_or_int4` = test_multi_type_table.`float4` or test_multi_type_table.`int4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`float4_or_uint4` = test_multi_type_table.`float4` or test_multi_type_table.`uint4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`float4_or_int8` = test_multi_type_table.`float4` or test_multi_type_table.`int8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`float4_or_uint8` = test_multi_type_table.`float4` or test_multi_type_table.`uint8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`float4_or_float4` = test_multi_type_table.`float4` or test_multi_type_table.`float4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`float4_or_float8` = test_multi_type_table.`float4` or test_multi_type_table.`float8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`float4_or_numeric` = test_multi_type_table.`float4` or test_multi_type_table.`numeric`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`float4_or_bit1` = test_multi_type_table.`float4` or test_multi_type_table.`bit1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`float4_or_bit64` = test_multi_type_table.`float4` or test_multi_type_table.`bit64`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`float4_or_boolean` = test_multi_type_table.`float4` or test_multi_type_table.`boolean`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`float4_or_date` = test_multi_type_table.`float4` or test_multi_type_table.`date`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`float4_or_time` = test_multi_type_table.`float4` or test_multi_type_table.`time`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`float4_or_time(4)` = test_multi_type_table.`float4` or test_multi_type_table.`time(4)`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`float4_or_datetime` = test_multi_type_table.`float4` or test_multi_type_table.`datetime`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`float4_or_datetime(4)` = test_multi_type_table.`float4` or test_multi_type_table.`datetime(4)`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`float4_or_timestamp` = test_multi_type_table.`float4` or test_multi_type_table.`timestamp`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`float4_or_timestamp(4)` = test_multi_type_table.`float4` or test_multi_type_table.`timestamp(4)`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`float4_or_year` = test_multi_type_table.`float4` or test_multi_type_table.`year`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`float4_or_char` = test_multi_type_table.`float4` or test_multi_type_table.`char`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`float4_or_varchar` = test_multi_type_table.`float4` or test_multi_type_table.`varchar`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`float4_or_binary` = test_multi_type_table.`float4` or test_multi_type_table.`binary`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`float4_or_varbinary` = test_multi_type_table.`float4` or test_multi_type_table.`varbinary`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`float4_or_tinyblob` = test_multi_type_table.`float4` or test_multi_type_table.`tinyblob`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`float4_or_blob` = test_multi_type_table.`float4` or test_multi_type_table.`blob`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`float4_or_mediumblob` = test_multi_type_table.`float4` or test_multi_type_table.`mediumblob`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`float4_or_longblob` = test_multi_type_table.`float4` or test_multi_type_table.`longblob`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`float4_or_text` = test_multi_type_table.`float4` or test_multi_type_table.`text`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`float4_or_enum_t` = test_multi_type_table.`float4` or test_multi_type_table.`enum_t`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`float4_or_set_t` = test_multi_type_table.`float4` or test_multi_type_table.`set_t`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`float4_or_json` = test_multi_type_table.`float4` or test_multi_type_table.`json`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`float8_or_int1` = test_multi_type_table.`float8` or test_multi_type_table.`int1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`float8_or_uint1` = test_multi_type_table.`float8` or test_multi_type_table.`uint1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`float8_or_int2` = test_multi_type_table.`float8` or test_multi_type_table.`int2`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`float8_or_uint2` = test_multi_type_table.`float8` or test_multi_type_table.`uint2`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`float8_or_int4` = test_multi_type_table.`float8` or test_multi_type_table.`int4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`float8_or_uint4` = test_multi_type_table.`float8` or test_multi_type_table.`uint4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`float8_or_int8` = test_multi_type_table.`float8` or test_multi_type_table.`int8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`float8_or_uint8` = test_multi_type_table.`float8` or test_multi_type_table.`uint8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`float8_or_float4` = test_multi_type_table.`float8` or test_multi_type_table.`float4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`float8_or_float8` = test_multi_type_table.`float8` or test_multi_type_table.`float8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`float8_or_numeric` = test_multi_type_table.`float8` or test_multi_type_table.`numeric`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`float8_or_bit1` = test_multi_type_table.`float8` or test_multi_type_table.`bit1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`float8_or_bit64` = test_multi_type_table.`float8` or test_multi_type_table.`bit64`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`float8_or_boolean` = test_multi_type_table.`float8` or test_multi_type_table.`boolean`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`float8_or_date` = test_multi_type_table.`float8` or test_multi_type_table.`date`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`float8_or_time` = test_multi_type_table.`float8` or test_multi_type_table.`time`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`float8_or_time(4)` = test_multi_type_table.`float8` or test_multi_type_table.`time(4)`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`float8_or_datetime` = test_multi_type_table.`float8` or test_multi_type_table.`datetime`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`float8_or_datetime(4)` = test_multi_type_table.`float8` or test_multi_type_table.`datetime(4)`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`float8_or_timestamp` = test_multi_type_table.`float8` or test_multi_type_table.`timestamp`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`float8_or_timestamp(4)` = test_multi_type_table.`float8` or test_multi_type_table.`timestamp(4)`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`float8_or_year` = test_multi_type_table.`float8` or test_multi_type_table.`year`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`float8_or_char` = test_multi_type_table.`float8` or test_multi_type_table.`char`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`float8_or_varchar` = test_multi_type_table.`float8` or test_multi_type_table.`varchar`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`float8_or_binary` = test_multi_type_table.`float8` or test_multi_type_table.`binary`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`float8_or_varbinary` = test_multi_type_table.`float8` or test_multi_type_table.`varbinary`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`float8_or_tinyblob` = test_multi_type_table.`float8` or test_multi_type_table.`tinyblob`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`float8_or_blob` = test_multi_type_table.`float8` or test_multi_type_table.`blob`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`float8_or_mediumblob` = test_multi_type_table.`float8` or test_multi_type_table.`mediumblob`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`float8_or_longblob` = test_multi_type_table.`float8` or test_multi_type_table.`longblob`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`float8_or_text` = test_multi_type_table.`float8` or test_multi_type_table.`text`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`float8_or_enum_t` = test_multi_type_table.`float8` or test_multi_type_table.`enum_t`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`float8_or_set_t` = test_multi_type_table.`float8` or test_multi_type_table.`set_t`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`float8_or_json` = test_multi_type_table.`float8` or test_multi_type_table.`json`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`numeric_or_int1` = test_multi_type_table.`numeric` or test_multi_type_table.`int1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`numeric_or_uint1` = test_multi_type_table.`numeric` or test_multi_type_table.`uint1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`numeric_or_int2` = test_multi_type_table.`numeric` or test_multi_type_table.`int2`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`numeric_or_uint2` = test_multi_type_table.`numeric` or test_multi_type_table.`uint2`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`numeric_or_int4` = test_multi_type_table.`numeric` or test_multi_type_table.`int4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`numeric_or_uint4` = test_multi_type_table.`numeric` or test_multi_type_table.`uint4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`numeric_or_int8` = test_multi_type_table.`numeric` or test_multi_type_table.`int8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`numeric_or_uint8` = test_multi_type_table.`numeric` or test_multi_type_table.`uint8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`numeric_or_float4` = test_multi_type_table.`numeric` or test_multi_type_table.`float4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`numeric_or_float8` = test_multi_type_table.`numeric` or test_multi_type_table.`float8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`numeric_or_numeric` = test_multi_type_table.`numeric` or test_multi_type_table.`numeric`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`numeric_or_bit1` = test_multi_type_table.`numeric` or test_multi_type_table.`bit1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`numeric_or_bit64` = test_multi_type_table.`numeric` or test_multi_type_table.`bit64`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`numeric_or_boolean` = test_multi_type_table.`numeric` or test_multi_type_table.`boolean`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`numeric_or_date` = test_multi_type_table.`numeric` or test_multi_type_table.`date`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`numeric_or_time` = test_multi_type_table.`numeric` or test_multi_type_table.`time`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`numeric_or_time(4)` = test_multi_type_table.`numeric` or test_multi_type_table.`time(4)`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`numeric_or_datetime` = test_multi_type_table.`numeric` or test_multi_type_table.`datetime`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`numeric_or_datetime(4)` = test_multi_type_table.`numeric` or test_multi_type_table.`datetime(4)`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`numeric_or_timestamp` = test_multi_type_table.`numeric` or test_multi_type_table.`timestamp`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`numeric_or_timestamp(4)` = test_multi_type_table.`numeric` or test_multi_type_table.`timestamp(4)`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`numeric_or_year` = test_multi_type_table.`numeric` or test_multi_type_table.`year`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`numeric_or_char` = test_multi_type_table.`numeric` or test_multi_type_table.`char`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`numeric_or_varchar` = test_multi_type_table.`numeric` or test_multi_type_table.`varchar`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`numeric_or_binary` = test_multi_type_table.`numeric` or test_multi_type_table.`binary`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`numeric_or_varbinary` = test_multi_type_table.`numeric` or test_multi_type_table.`varbinary`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`numeric_or_tinyblob` = test_multi_type_table.`numeric` or test_multi_type_table.`tinyblob`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`numeric_or_blob` = test_multi_type_table.`numeric` or test_multi_type_table.`blob`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`numeric_or_mediumblob` = test_multi_type_table.`numeric` or test_multi_type_table.`mediumblob`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`numeric_or_longblob` = test_multi_type_table.`numeric` or test_multi_type_table.`longblob`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`numeric_or_text` = test_multi_type_table.`numeric` or test_multi_type_table.`text`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`numeric_or_enum_t` = test_multi_type_table.`numeric` or test_multi_type_table.`enum_t`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`numeric_or_set_t` = test_multi_type_table.`numeric` or test_multi_type_table.`set_t`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`numeric_or_json` = test_multi_type_table.`numeric` or test_multi_type_table.`json`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`bit1_or_int1` = test_multi_type_table.`bit1` or test_multi_type_table.`int1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`bit1_or_uint1` = test_multi_type_table.`bit1` or test_multi_type_table.`uint1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`bit1_or_int2` = test_multi_type_table.`bit1` or test_multi_type_table.`int2`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`bit1_or_uint2` = test_multi_type_table.`bit1` or test_multi_type_table.`uint2`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`bit1_or_int4` = test_multi_type_table.`bit1` or test_multi_type_table.`int4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`bit1_or_uint4` = test_multi_type_table.`bit1` or test_multi_type_table.`uint4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`bit1_or_int8` = test_multi_type_table.`bit1` or test_multi_type_table.`int8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`bit1_or_uint8` = test_multi_type_table.`bit1` or test_multi_type_table.`uint8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`bit1_or_float4` = test_multi_type_table.`bit1` or test_multi_type_table.`float4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`bit1_or_float8` = test_multi_type_table.`bit1` or test_multi_type_table.`float8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`bit1_or_numeric` = test_multi_type_table.`bit1` or test_multi_type_table.`numeric`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`bit1_or_bit1` = test_multi_type_table.`bit1` or test_multi_type_table.`bit1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`bit1_or_bit64` = test_multi_type_table.`bit1` or test_multi_type_table.`bit64`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`bit1_or_boolean` = test_multi_type_table.`bit1` or test_multi_type_table.`boolean`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`bit1_or_date` = test_multi_type_table.`bit1` or test_multi_type_table.`date`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`bit1_or_time` = test_multi_type_table.`bit1` or test_multi_type_table.`time`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`bit1_or_time(4)` = test_multi_type_table.`bit1` or test_multi_type_table.`time(4)`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`bit1_or_datetime` = test_multi_type_table.`bit1` or test_multi_type_table.`datetime`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`bit1_or_datetime(4)` = test_multi_type_table.`bit1` or test_multi_type_table.`datetime(4)`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`bit1_or_timestamp` = test_multi_type_table.`bit1` or test_multi_type_table.`timestamp`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`bit1_or_timestamp(4)` = test_multi_type_table.`bit1` or test_multi_type_table.`timestamp(4)`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`bit1_or_year` = test_multi_type_table.`bit1` or test_multi_type_table.`year`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`bit1_or_char` = test_multi_type_table.`bit1` or test_multi_type_table.`char`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`bit1_or_varchar` = test_multi_type_table.`bit1` or test_multi_type_table.`varchar`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`bit1_or_binary` = test_multi_type_table.`bit1` or test_multi_type_table.`binary`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`bit1_or_varbinary` = test_multi_type_table.`bit1` or test_multi_type_table.`varbinary`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`bit1_or_tinyblob` = test_multi_type_table.`bit1` or test_multi_type_table.`tinyblob`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`bit1_or_blob` = test_multi_type_table.`bit1` or test_multi_type_table.`blob`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`bit1_or_mediumblob` = test_multi_type_table.`bit1` or test_multi_type_table.`mediumblob`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`bit1_or_longblob` = test_multi_type_table.`bit1` or test_multi_type_table.`longblob`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`bit1_or_text` = test_multi_type_table.`bit1` or test_multi_type_table.`text`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`bit1_or_enum_t` = test_multi_type_table.`bit1` or test_multi_type_table.`enum_t`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`bit1_or_set_t` = test_multi_type_table.`bit1` or test_multi_type_table.`set_t`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`bit1_or_json` = test_multi_type_table.`bit1` or test_multi_type_table.`json`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`bit64_or_int1` = test_multi_type_table.`bit64` or test_multi_type_table.`int1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`bit64_or_uint1` = test_multi_type_table.`bit64` or test_multi_type_table.`uint1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`bit64_or_int2` = test_multi_type_table.`bit64` or test_multi_type_table.`int2`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`bit64_or_uint2` = test_multi_type_table.`bit64` or test_multi_type_table.`uint2`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`bit64_or_int4` = test_multi_type_table.`bit64` or test_multi_type_table.`int4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`bit64_or_uint4` = test_multi_type_table.`bit64` or test_multi_type_table.`uint4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`bit64_or_int8` = test_multi_type_table.`bit64` or test_multi_type_table.`int8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`bit64_or_uint8` = test_multi_type_table.`bit64` or test_multi_type_table.`uint8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`bit64_or_float4` = test_multi_type_table.`bit64` or test_multi_type_table.`float4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`bit64_or_float8` = test_multi_type_table.`bit64` or test_multi_type_table.`float8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`bit64_or_numeric` = test_multi_type_table.`bit64` or test_multi_type_table.`numeric`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`bit64_or_bit1` = test_multi_type_table.`bit64` or test_multi_type_table.`bit1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`bit64_or_bit64` = test_multi_type_table.`bit64` or test_multi_type_table.`bit64`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`bit64_or_boolean` = test_multi_type_table.`bit64` or test_multi_type_table.`boolean`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`bit64_or_date` = test_multi_type_table.`bit64` or test_multi_type_table.`date`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`bit64_or_time` = test_multi_type_table.`bit64` or test_multi_type_table.`time`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`bit64_or_time(4)` = test_multi_type_table.`bit64` or test_multi_type_table.`time(4)`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`bit64_or_datetime` = test_multi_type_table.`bit64` or test_multi_type_table.`datetime`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`bit64_or_datetime(4)` = test_multi_type_table.`bit64` or test_multi_type_table.`datetime(4)`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`bit64_or_timestamp` = test_multi_type_table.`bit64` or test_multi_type_table.`timestamp`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`bit64_or_timestamp(4)` = test_multi_type_table.`bit64` or test_multi_type_table.`timestamp(4)`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`bit64_or_year` = test_multi_type_table.`bit64` or test_multi_type_table.`year`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`bit64_or_char` = test_multi_type_table.`bit64` or test_multi_type_table.`char`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`bit64_or_varchar` = test_multi_type_table.`bit64` or test_multi_type_table.`varchar`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`bit64_or_binary` = test_multi_type_table.`bit64` or test_multi_type_table.`binary`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`bit64_or_varbinary` = test_multi_type_table.`bit64` or test_multi_type_table.`varbinary`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`bit64_or_tinyblob` = test_multi_type_table.`bit64` or test_multi_type_table.`tinyblob`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`bit64_or_blob` = test_multi_type_table.`bit64` or test_multi_type_table.`blob`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`bit64_or_mediumblob` = test_multi_type_table.`bit64` or test_multi_type_table.`mediumblob`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`bit64_or_longblob` = test_multi_type_table.`bit64` or test_multi_type_table.`longblob`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`bit64_or_text` = test_multi_type_table.`bit64` or test_multi_type_table.`text`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`bit64_or_enum_t` = test_multi_type_table.`bit64` or test_multi_type_table.`enum_t`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`bit64_or_set_t` = test_multi_type_table.`bit64` or test_multi_type_table.`set_t`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`bit64_or_json` = test_multi_type_table.`bit64` or test_multi_type_table.`json`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`boolean_or_int1` = test_multi_type_table.`boolean` or test_multi_type_table.`int1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`boolean_or_uint1` = test_multi_type_table.`boolean` or test_multi_type_table.`uint1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`boolean_or_int2` = test_multi_type_table.`boolean` or test_multi_type_table.`int2`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`boolean_or_uint2` = test_multi_type_table.`boolean` or test_multi_type_table.`uint2`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`boolean_or_int4` = test_multi_type_table.`boolean` or test_multi_type_table.`int4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`boolean_or_uint4` = test_multi_type_table.`boolean` or test_multi_type_table.`uint4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`boolean_or_int8` = test_multi_type_table.`boolean` or test_multi_type_table.`int8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`boolean_or_uint8` = test_multi_type_table.`boolean` or test_multi_type_table.`uint8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`boolean_or_float4` = test_multi_type_table.`boolean` or test_multi_type_table.`float4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`boolean_or_float8` = test_multi_type_table.`boolean` or test_multi_type_table.`float8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`boolean_or_numeric` = test_multi_type_table.`boolean` or test_multi_type_table.`numeric`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`boolean_or_bit1` = test_multi_type_table.`boolean` or test_multi_type_table.`bit1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`boolean_or_bit64` = test_multi_type_table.`boolean` or test_multi_type_table.`bit64`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`boolean_or_boolean` = test_multi_type_table.`boolean` or test_multi_type_table.`boolean`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`boolean_or_date` = test_multi_type_table.`boolean` or test_multi_type_table.`date`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`boolean_or_time` = test_multi_type_table.`boolean` or test_multi_type_table.`time`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`boolean_or_time(4)` = test_multi_type_table.`boolean` or test_multi_type_table.`time(4)`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`boolean_or_datetime` = test_multi_type_table.`boolean` or test_multi_type_table.`datetime`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`boolean_or_datetime(4)` = test_multi_type_table.`boolean` or test_multi_type_table.`datetime(4)`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`boolean_or_timestamp` = test_multi_type_table.`boolean` or test_multi_type_table.`timestamp`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`boolean_or_timestamp(4)` = test_multi_type_table.`boolean` or test_multi_type_table.`timestamp(4)`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`boolean_or_year` = test_multi_type_table.`boolean` or test_multi_type_table.`year`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`boolean_or_char` = test_multi_type_table.`boolean` or test_multi_type_table.`char`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`boolean_or_varchar` = test_multi_type_table.`boolean` or test_multi_type_table.`varchar`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`boolean_or_binary` = test_multi_type_table.`boolean` or test_multi_type_table.`binary`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`boolean_or_varbinary` = test_multi_type_table.`boolean` or test_multi_type_table.`varbinary`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`boolean_or_tinyblob` = test_multi_type_table.`boolean` or test_multi_type_table.`tinyblob`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`boolean_or_blob` = test_multi_type_table.`boolean` or test_multi_type_table.`blob`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`boolean_or_mediumblob` = test_multi_type_table.`boolean` or test_multi_type_table.`mediumblob`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`boolean_or_longblob` = test_multi_type_table.`boolean` or test_multi_type_table.`longblob`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`boolean_or_text` = test_multi_type_table.`boolean` or test_multi_type_table.`text`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`boolean_or_enum_t` = test_multi_type_table.`boolean` or test_multi_type_table.`enum_t`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`boolean_or_set_t` = test_multi_type_table.`boolean` or test_multi_type_table.`set_t`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`boolean_or_json` = test_multi_type_table.`boolean` or test_multi_type_table.`json`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`date_or_int1` = test_multi_type_table.`date` or test_multi_type_table.`int1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`date_or_uint1` = test_multi_type_table.`date` or test_multi_type_table.`uint1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`date_or_int2` = test_multi_type_table.`date` or test_multi_type_table.`int2`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`date_or_uint2` = test_multi_type_table.`date` or test_multi_type_table.`uint2`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`date_or_int4` = test_multi_type_table.`date` or test_multi_type_table.`int4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`date_or_uint4` = test_multi_type_table.`date` or test_multi_type_table.`uint4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`date_or_int8` = test_multi_type_table.`date` or test_multi_type_table.`int8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`date_or_uint8` = test_multi_type_table.`date` or test_multi_type_table.`uint8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`date_or_float4` = test_multi_type_table.`date` or test_multi_type_table.`float4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`date_or_float8` = test_multi_type_table.`date` or test_multi_type_table.`float8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`date_or_numeric` = test_multi_type_table.`date` or test_multi_type_table.`numeric`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`date_or_bit1` = test_multi_type_table.`date` or test_multi_type_table.`bit1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`date_or_bit64` = test_multi_type_table.`date` or test_multi_type_table.`bit64`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`date_or_boolean` = test_multi_type_table.`date` or test_multi_type_table.`boolean`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`date_or_date` = test_multi_type_table.`date` or test_multi_type_table.`date`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`date_or_time` = test_multi_type_table.`date` or test_multi_type_table.`time`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`date_or_time(4)` = test_multi_type_table.`date` or test_multi_type_table.`time(4)`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`date_or_datetime` = test_multi_type_table.`date` or test_multi_type_table.`datetime`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`date_or_datetime(4)` = test_multi_type_table.`date` or test_multi_type_table.`datetime(4)`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`date_or_timestamp` = test_multi_type_table.`date` or test_multi_type_table.`timestamp`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`date_or_timestamp(4)` = test_multi_type_table.`date` or test_multi_type_table.`timestamp(4)`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`date_or_year` = test_multi_type_table.`date` or test_multi_type_table.`year`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`date_or_char` = test_multi_type_table.`date` or test_multi_type_table.`char`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`date_or_varchar` = test_multi_type_table.`date` or test_multi_type_table.`varchar`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`date_or_binary` = test_multi_type_table.`date` or test_multi_type_table.`binary`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`date_or_varbinary` = test_multi_type_table.`date` or test_multi_type_table.`varbinary`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`date_or_tinyblob` = test_multi_type_table.`date` or test_multi_type_table.`tinyblob`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`date_or_blob` = test_multi_type_table.`date` or test_multi_type_table.`blob`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`date_or_mediumblob` = test_multi_type_table.`date` or test_multi_type_table.`mediumblob`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`date_or_longblob` = test_multi_type_table.`date` or test_multi_type_table.`longblob`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`date_or_text` = test_multi_type_table.`date` or test_multi_type_table.`text`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`date_or_enum_t` = test_multi_type_table.`date` or test_multi_type_table.`enum_t`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`date_or_set_t` = test_multi_type_table.`date` or test_multi_type_table.`set_t`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`date_or_json` = test_multi_type_table.`date` or test_multi_type_table.`json`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`time_or_int1` = test_multi_type_table.`time` or test_multi_type_table.`int1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`time_or_uint1` = test_multi_type_table.`time` or test_multi_type_table.`uint1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`time_or_int2` = test_multi_type_table.`time` or test_multi_type_table.`int2`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`time_or_uint2` = test_multi_type_table.`time` or test_multi_type_table.`uint2`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`time_or_int4` = test_multi_type_table.`time` or test_multi_type_table.`int4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`time_or_uint4` = test_multi_type_table.`time` or test_multi_type_table.`uint4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`time_or_int8` = test_multi_type_table.`time` or test_multi_type_table.`int8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`time_or_uint8` = test_multi_type_table.`time` or test_multi_type_table.`uint8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`time_or_float4` = test_multi_type_table.`time` or test_multi_type_table.`float4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`time_or_float8` = test_multi_type_table.`time` or test_multi_type_table.`float8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`time_or_numeric` = test_multi_type_table.`time` or test_multi_type_table.`numeric`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`time_or_bit1` = test_multi_type_table.`time` or test_multi_type_table.`bit1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`time_or_bit64` = test_multi_type_table.`time` or test_multi_type_table.`bit64`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`time_or_boolean` = test_multi_type_table.`time` or test_multi_type_table.`boolean`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`time_or_date` = test_multi_type_table.`time` or test_multi_type_table.`date`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`time_or_time` = test_multi_type_table.`time` or test_multi_type_table.`time`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`time_or_time(4)` = test_multi_type_table.`time` or test_multi_type_table.`time(4)`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`time_or_datetime` = test_multi_type_table.`time` or test_multi_type_table.`datetime`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`time_or_datetime(4)` = test_multi_type_table.`time` or test_multi_type_table.`datetime(4)`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`time_or_timestamp` = test_multi_type_table.`time` or test_multi_type_table.`timestamp`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`time_or_timestamp(4)` = test_multi_type_table.`time` or test_multi_type_table.`timestamp(4)`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`time_or_year` = test_multi_type_table.`time` or test_multi_type_table.`year`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`time_or_char` = test_multi_type_table.`time` or test_multi_type_table.`char`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`time_or_varchar` = test_multi_type_table.`time` or test_multi_type_table.`varchar`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`time_or_binary` = test_multi_type_table.`time` or test_multi_type_table.`binary`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`time_or_varbinary` = test_multi_type_table.`time` or test_multi_type_table.`varbinary`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`time_or_tinyblob` = test_multi_type_table.`time` or test_multi_type_table.`tinyblob`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`time_or_blob` = test_multi_type_table.`time` or test_multi_type_table.`blob`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`time_or_mediumblob` = test_multi_type_table.`time` or test_multi_type_table.`mediumblob`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`time_or_longblob` = test_multi_type_table.`time` or test_multi_type_table.`longblob`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`time_or_text` = test_multi_type_table.`time` or test_multi_type_table.`text`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`time_or_enum_t` = test_multi_type_table.`time` or test_multi_type_table.`enum_t`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`time_or_set_t` = test_multi_type_table.`time` or test_multi_type_table.`set_t`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`time_or_json` = test_multi_type_table.`time` or test_multi_type_table.`json`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`time(4)_or_int1` = test_multi_type_table.`time(4)` or test_multi_type_table.`int1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`time(4)_or_uint1` = test_multi_type_table.`time(4)` or test_multi_type_table.`uint1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`time(4)_or_int2` = test_multi_type_table.`time(4)` or test_multi_type_table.`int2`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`time(4)_or_uint2` = test_multi_type_table.`time(4)` or test_multi_type_table.`uint2`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`time(4)_or_int4` = test_multi_type_table.`time(4)` or test_multi_type_table.`int4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`time(4)_or_uint4` = test_multi_type_table.`time(4)` or test_multi_type_table.`uint4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`time(4)_or_int8` = test_multi_type_table.`time(4)` or test_multi_type_table.`int8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`time(4)_or_uint8` = test_multi_type_table.`time(4)` or test_multi_type_table.`uint8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`time(4)_or_float4` = test_multi_type_table.`time(4)` or test_multi_type_table.`float4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`time(4)_or_float8` = test_multi_type_table.`time(4)` or test_multi_type_table.`float8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`time(4)_or_numeric` = test_multi_type_table.`time(4)` or test_multi_type_table.`numeric`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`time(4)_or_bit1` = test_multi_type_table.`time(4)` or test_multi_type_table.`bit1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`time(4)_or_bit64` = test_multi_type_table.`time(4)` or test_multi_type_table.`bit64`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`time(4)_or_boolean` = test_multi_type_table.`time(4)` or test_multi_type_table.`boolean`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`time(4)_or_date` = test_multi_type_table.`time(4)` or test_multi_type_table.`date`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`time(4)_or_time` = test_multi_type_table.`time(4)` or test_multi_type_table.`time`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`time(4)_or_time(4)` = test_multi_type_table.`time(4)` or test_multi_type_table.`time(4)`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`time(4)_or_datetime` = test_multi_type_table.`time(4)` or test_multi_type_table.`datetime`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`time(4)_or_datetime(4)` = test_multi_type_table.`time(4)` or test_multi_type_table.`datetime(4)`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`time(4)_or_timestamp` = test_multi_type_table.`time(4)` or test_multi_type_table.`timestamp`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`time(4)_or_timestamp(4)` = test_multi_type_table.`time(4)` or test_multi_type_table.`timestamp(4)`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`time(4)_or_year` = test_multi_type_table.`time(4)` or test_multi_type_table.`year`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`time(4)_or_char` = test_multi_type_table.`time(4)` or test_multi_type_table.`char`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`time(4)_or_varchar` = test_multi_type_table.`time(4)` or test_multi_type_table.`varchar`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`time(4)_or_binary` = test_multi_type_table.`time(4)` or test_multi_type_table.`binary`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`time(4)_or_varbinary` = test_multi_type_table.`time(4)` or test_multi_type_table.`varbinary`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`time(4)_or_tinyblob` = test_multi_type_table.`time(4)` or test_multi_type_table.`tinyblob`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`time(4)_or_blob` = test_multi_type_table.`time(4)` or test_multi_type_table.`blob`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`time(4)_or_mediumblob` = test_multi_type_table.`time(4)` or test_multi_type_table.`mediumblob`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`time(4)_or_longblob` = test_multi_type_table.`time(4)` or test_multi_type_table.`longblob`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`time(4)_or_text` = test_multi_type_table.`time(4)` or test_multi_type_table.`text`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`time(4)_or_enum_t` = test_multi_type_table.`time(4)` or test_multi_type_table.`enum_t`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`time(4)_or_set_t` = test_multi_type_table.`time(4)` or test_multi_type_table.`set_t`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`time(4)_or_json` = test_multi_type_table.`time(4)` or test_multi_type_table.`json`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`datetime_or_int1` = test_multi_type_table.`datetime` or test_multi_type_table.`int1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`datetime_or_uint1` = test_multi_type_table.`datetime` or test_multi_type_table.`uint1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`datetime_or_int2` = test_multi_type_table.`datetime` or test_multi_type_table.`int2`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`datetime_or_uint2` = test_multi_type_table.`datetime` or test_multi_type_table.`uint2`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`datetime_or_int4` = test_multi_type_table.`datetime` or test_multi_type_table.`int4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`datetime_or_uint4` = test_multi_type_table.`datetime` or test_multi_type_table.`uint4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`datetime_or_int8` = test_multi_type_table.`datetime` or test_multi_type_table.`int8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`datetime_or_uint8` = test_multi_type_table.`datetime` or test_multi_type_table.`uint8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`datetime_or_float4` = test_multi_type_table.`datetime` or test_multi_type_table.`float4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`datetime_or_float8` = test_multi_type_table.`datetime` or test_multi_type_table.`float8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`datetime_or_numeric` = test_multi_type_table.`datetime` or test_multi_type_table.`numeric`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`datetime_or_bit1` = test_multi_type_table.`datetime` or test_multi_type_table.`bit1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`datetime_or_bit64` = test_multi_type_table.`datetime` or test_multi_type_table.`bit64`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`datetime_or_boolean` = test_multi_type_table.`datetime` or test_multi_type_table.`boolean`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`datetime_or_date` = test_multi_type_table.`datetime` or test_multi_type_table.`date`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`datetime_or_time` = test_multi_type_table.`datetime` or test_multi_type_table.`time`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`datetime_or_time(4)` = test_multi_type_table.`datetime` or test_multi_type_table.`time(4)`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`datetime_or_datetime` = test_multi_type_table.`datetime` or test_multi_type_table.`datetime`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`datetime_or_datetime(4)` = test_multi_type_table.`datetime` or test_multi_type_table.`datetime(4)`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`datetime_or_timestamp` = test_multi_type_table.`datetime` or test_multi_type_table.`timestamp`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`datetime_or_timestamp(4)` = test_multi_type_table.`datetime` or test_multi_type_table.`timestamp(4)`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`datetime_or_year` = test_multi_type_table.`datetime` or test_multi_type_table.`year`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`datetime_or_char` = test_multi_type_table.`datetime` or test_multi_type_table.`char`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`datetime_or_varchar` = test_multi_type_table.`datetime` or test_multi_type_table.`varchar`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`datetime_or_binary` = test_multi_type_table.`datetime` or test_multi_type_table.`binary`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`datetime_or_varbinary` = test_multi_type_table.`datetime` or test_multi_type_table.`varbinary`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`datetime_or_tinyblob` = test_multi_type_table.`datetime` or test_multi_type_table.`tinyblob`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`datetime_or_blob` = test_multi_type_table.`datetime` or test_multi_type_table.`blob`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`datetime_or_mediumblob` = test_multi_type_table.`datetime` or test_multi_type_table.`mediumblob`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`datetime_or_longblob` = test_multi_type_table.`datetime` or test_multi_type_table.`longblob`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`datetime_or_text` = test_multi_type_table.`datetime` or test_multi_type_table.`text`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`datetime_or_enum_t` = test_multi_type_table.`datetime` or test_multi_type_table.`enum_t`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`datetime_or_set_t` = test_multi_type_table.`datetime` or test_multi_type_table.`set_t`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`datetime_or_json` = test_multi_type_table.`datetime` or test_multi_type_table.`json`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`datetime(4)_or_int1` = test_multi_type_table.`datetime(4)` or test_multi_type_table.`int1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`datetime(4)_or_uint1` = test_multi_type_table.`datetime(4)` or test_multi_type_table.`uint1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`datetime(4)_or_int2` = test_multi_type_table.`datetime(4)` or test_multi_type_table.`int2`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`datetime(4)_or_uint2` = test_multi_type_table.`datetime(4)` or test_multi_type_table.`uint2`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`datetime(4)_or_int4` = test_multi_type_table.`datetime(4)` or test_multi_type_table.`int4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`datetime(4)_or_uint4` = test_multi_type_table.`datetime(4)` or test_multi_type_table.`uint4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`datetime(4)_or_int8` = test_multi_type_table.`datetime(4)` or test_multi_type_table.`int8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`datetime(4)_or_uint8` = test_multi_type_table.`datetime(4)` or test_multi_type_table.`uint8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`datetime(4)_or_float4` = test_multi_type_table.`datetime(4)` or test_multi_type_table.`float4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`datetime(4)_or_float8` = test_multi_type_table.`datetime(4)` or test_multi_type_table.`float8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`datetime(4)_or_numeric` = test_multi_type_table.`datetime(4)` or test_multi_type_table.`numeric`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`datetime(4)_or_bit1` = test_multi_type_table.`datetime(4)` or test_multi_type_table.`bit1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`datetime(4)_or_bit64` = test_multi_type_table.`datetime(4)` or test_multi_type_table.`bit64`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`datetime(4)_or_boolean` = test_multi_type_table.`datetime(4)` or test_multi_type_table.`boolean`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`datetime(4)_or_date` = test_multi_type_table.`datetime(4)` or test_multi_type_table.`date`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`datetime(4)_or_time` = test_multi_type_table.`datetime(4)` or test_multi_type_table.`time`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`datetime(4)_or_time(4)` = test_multi_type_table.`datetime(4)` or test_multi_type_table.`time(4)`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`datetime(4)_or_datetime` = test_multi_type_table.`datetime(4)` or test_multi_type_table.`datetime`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`datetime(4)_or_datetime(4)` = test_multi_type_table.`datetime(4)` or test_multi_type_table.`datetime(4)`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`datetime(4)_or_timestamp` = test_multi_type_table.`datetime(4)` or test_multi_type_table.`timestamp`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`datetime(4)_or_timestamp(4)` = test_multi_type_table.`datetime(4)` or test_multi_type_table.`timestamp(4)`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`datetime(4)_or_year` = test_multi_type_table.`datetime(4)` or test_multi_type_table.`year`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`datetime(4)_or_char` = test_multi_type_table.`datetime(4)` or test_multi_type_table.`char`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`datetime(4)_or_varchar` = test_multi_type_table.`datetime(4)` or test_multi_type_table.`varchar`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`datetime(4)_or_binary` = test_multi_type_table.`datetime(4)` or test_multi_type_table.`binary`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`datetime(4)_or_varbinary` = test_multi_type_table.`datetime(4)` or test_multi_type_table.`varbinary`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`datetime(4)_or_tinyblob` = test_multi_type_table.`datetime(4)` or test_multi_type_table.`tinyblob`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`datetime(4)_or_blob` = test_multi_type_table.`datetime(4)` or test_multi_type_table.`blob`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`datetime(4)_or_mediumblob` = test_multi_type_table.`datetime(4)` or test_multi_type_table.`mediumblob`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`datetime(4)_or_longblob` = test_multi_type_table.`datetime(4)` or test_multi_type_table.`longblob`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`datetime(4)_or_text` = test_multi_type_table.`datetime(4)` or test_multi_type_table.`text`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`datetime(4)_or_enum_t` = test_multi_type_table.`datetime(4)` or test_multi_type_table.`enum_t`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`datetime(4)_or_set_t` = test_multi_type_table.`datetime(4)` or test_multi_type_table.`set_t`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`datetime(4)_or_json` = test_multi_type_table.`datetime(4)` or test_multi_type_table.`json`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`timestamp_or_int1` = test_multi_type_table.`timestamp` or test_multi_type_table.`int1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`timestamp_or_uint1` = test_multi_type_table.`timestamp` or test_multi_type_table.`uint1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`timestamp_or_int2` = test_multi_type_table.`timestamp` or test_multi_type_table.`int2`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`timestamp_or_uint2` = test_multi_type_table.`timestamp` or test_multi_type_table.`uint2`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`timestamp_or_int4` = test_multi_type_table.`timestamp` or test_multi_type_table.`int4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`timestamp_or_uint4` = test_multi_type_table.`timestamp` or test_multi_type_table.`uint4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`timestamp_or_int8` = test_multi_type_table.`timestamp` or test_multi_type_table.`int8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`timestamp_or_uint8` = test_multi_type_table.`timestamp` or test_multi_type_table.`uint8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`timestamp_or_float4` = test_multi_type_table.`timestamp` or test_multi_type_table.`float4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`timestamp_or_float8` = test_multi_type_table.`timestamp` or test_multi_type_table.`float8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`timestamp_or_numeric` = test_multi_type_table.`timestamp` or test_multi_type_table.`numeric`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`timestamp_or_bit1` = test_multi_type_table.`timestamp` or test_multi_type_table.`bit1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`timestamp_or_bit64` = test_multi_type_table.`timestamp` or test_multi_type_table.`bit64`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`timestamp_or_boolean` = test_multi_type_table.`timestamp` or test_multi_type_table.`boolean`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`timestamp_or_date` = test_multi_type_table.`timestamp` or test_multi_type_table.`date`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`timestamp_or_time` = test_multi_type_table.`timestamp` or test_multi_type_table.`time`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`timestamp_or_time(4)` = test_multi_type_table.`timestamp` or test_multi_type_table.`time(4)`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`timestamp_or_datetime` = test_multi_type_table.`timestamp` or test_multi_type_table.`datetime`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`timestamp_or_datetime(4)` = test_multi_type_table.`timestamp` or test_multi_type_table.`datetime(4)`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`timestamp_or_timestamp` = test_multi_type_table.`timestamp` or test_multi_type_table.`timestamp`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`timestamp_or_timestamp(4)` = test_multi_type_table.`timestamp` or test_multi_type_table.`timestamp(4)`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`timestamp_or_year` = test_multi_type_table.`timestamp` or test_multi_type_table.`year`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`timestamp_or_char` = test_multi_type_table.`timestamp` or test_multi_type_table.`char`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`timestamp_or_varchar` = test_multi_type_table.`timestamp` or test_multi_type_table.`varchar`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`timestamp_or_binary` = test_multi_type_table.`timestamp` or test_multi_type_table.`binary`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`timestamp_or_varbinary` = test_multi_type_table.`timestamp` or test_multi_type_table.`varbinary`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`timestamp_or_tinyblob` = test_multi_type_table.`timestamp` or test_multi_type_table.`tinyblob`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`timestamp_or_blob` = test_multi_type_table.`timestamp` or test_multi_type_table.`blob`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`timestamp_or_mediumblob` = test_multi_type_table.`timestamp` or test_multi_type_table.`mediumblob`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`timestamp_or_longblob` = test_multi_type_table.`timestamp` or test_multi_type_table.`longblob`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`timestamp_or_text` = test_multi_type_table.`timestamp` or test_multi_type_table.`text`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`timestamp_or_enum_t` = test_multi_type_table.`timestamp` or test_multi_type_table.`enum_t`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`timestamp_or_set_t` = test_multi_type_table.`timestamp` or test_multi_type_table.`set_t`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`timestamp_or_json` = test_multi_type_table.`timestamp` or test_multi_type_table.`json`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`timestamp(4)_or_int1` = test_multi_type_table.`timestamp(4)` or test_multi_type_table.`int1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`timestamp(4)_or_uint1` = test_multi_type_table.`timestamp(4)` or test_multi_type_table.`uint1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`timestamp(4)_or_int2` = test_multi_type_table.`timestamp(4)` or test_multi_type_table.`int2`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`timestamp(4)_or_uint2` = test_multi_type_table.`timestamp(4)` or test_multi_type_table.`uint2`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`timestamp(4)_or_int4` = test_multi_type_table.`timestamp(4)` or test_multi_type_table.`int4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`timestamp(4)_or_uint4` = test_multi_type_table.`timestamp(4)` or test_multi_type_table.`uint4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`timestamp(4)_or_int8` = test_multi_type_table.`timestamp(4)` or test_multi_type_table.`int8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`timestamp(4)_or_uint8` = test_multi_type_table.`timestamp(4)` or test_multi_type_table.`uint8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`timestamp(4)_or_float4` = test_multi_type_table.`timestamp(4)` or test_multi_type_table.`float4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`timestamp(4)_or_float8` = test_multi_type_table.`timestamp(4)` or test_multi_type_table.`float8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`timestamp(4)_or_numeric` = test_multi_type_table.`timestamp(4)` or test_multi_type_table.`numeric`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`timestamp(4)_or_bit1` = test_multi_type_table.`timestamp(4)` or test_multi_type_table.`bit1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`timestamp(4)_or_bit64` = test_multi_type_table.`timestamp(4)` or test_multi_type_table.`bit64`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`timestamp(4)_or_boolean` = test_multi_type_table.`timestamp(4)` or test_multi_type_table.`boolean`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`timestamp(4)_or_date` = test_multi_type_table.`timestamp(4)` or test_multi_type_table.`date`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`timestamp(4)_or_time` = test_multi_type_table.`timestamp(4)` or test_multi_type_table.`time`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`timestamp(4)_or_time(4)` = test_multi_type_table.`timestamp(4)` or test_multi_type_table.`time(4)`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`timestamp(4)_or_datetime` = test_multi_type_table.`timestamp(4)` or test_multi_type_table.`datetime`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`timestamp(4)_or_datetime(4)` = test_multi_type_table.`timestamp(4)` or test_multi_type_table.`datetime(4)`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`timestamp(4)_or_timestamp` = test_multi_type_table.`timestamp(4)` or test_multi_type_table.`timestamp`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`timestamp(4)_or_timestamp(4)` = test_multi_type_table.`timestamp(4)` or test_multi_type_table.`timestamp(4)`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`timestamp(4)_or_year` = test_multi_type_table.`timestamp(4)` or test_multi_type_table.`year`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`timestamp(4)_or_char` = test_multi_type_table.`timestamp(4)` or test_multi_type_table.`char`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`timestamp(4)_or_varchar` = test_multi_type_table.`timestamp(4)` or test_multi_type_table.`varchar`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`timestamp(4)_or_binary` = test_multi_type_table.`timestamp(4)` or test_multi_type_table.`binary`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`timestamp(4)_or_varbinary` = test_multi_type_table.`timestamp(4)` or test_multi_type_table.`varbinary`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`timestamp(4)_or_tinyblob` = test_multi_type_table.`timestamp(4)` or test_multi_type_table.`tinyblob`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`timestamp(4)_or_blob` = test_multi_type_table.`timestamp(4)` or test_multi_type_table.`blob`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`timestamp(4)_or_mediumblob` = test_multi_type_table.`timestamp(4)` or test_multi_type_table.`mediumblob`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`timestamp(4)_or_longblob` = test_multi_type_table.`timestamp(4)` or test_multi_type_table.`longblob`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`timestamp(4)_or_text` = test_multi_type_table.`timestamp(4)` or test_multi_type_table.`text`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`timestamp(4)_or_enum_t` = test_multi_type_table.`timestamp(4)` or test_multi_type_table.`enum_t`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`timestamp(4)_or_set_t` = test_multi_type_table.`timestamp(4)` or test_multi_type_table.`set_t`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`timestamp(4)_or_json` = test_multi_type_table.`timestamp(4)` or test_multi_type_table.`json`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`year_or_int1` = test_multi_type_table.`year` or test_multi_type_table.`int1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`year_or_uint1` = test_multi_type_table.`year` or test_multi_type_table.`uint1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`year_or_int2` = test_multi_type_table.`year` or test_multi_type_table.`int2`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`year_or_uint2` = test_multi_type_table.`year` or test_multi_type_table.`uint2`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`year_or_int4` = test_multi_type_table.`year` or test_multi_type_table.`int4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`year_or_uint4` = test_multi_type_table.`year` or test_multi_type_table.`uint4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`year_or_int8` = test_multi_type_table.`year` or test_multi_type_table.`int8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`year_or_uint8` = test_multi_type_table.`year` or test_multi_type_table.`uint8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`year_or_float4` = test_multi_type_table.`year` or test_multi_type_table.`float4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`year_or_float8` = test_multi_type_table.`year` or test_multi_type_table.`float8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`year_or_numeric` = test_multi_type_table.`year` or test_multi_type_table.`numeric`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`year_or_bit1` = test_multi_type_table.`year` or test_multi_type_table.`bit1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`year_or_bit64` = test_multi_type_table.`year` or test_multi_type_table.`bit64`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`year_or_boolean` = test_multi_type_table.`year` or test_multi_type_table.`boolean`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`year_or_date` = test_multi_type_table.`year` or test_multi_type_table.`date`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`year_or_time` = test_multi_type_table.`year` or test_multi_type_table.`time`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`year_or_time(4)` = test_multi_type_table.`year` or test_multi_type_table.`time(4)`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`year_or_datetime` = test_multi_type_table.`year` or test_multi_type_table.`datetime`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`year_or_datetime(4)` = test_multi_type_table.`year` or test_multi_type_table.`datetime(4)`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`year_or_timestamp` = test_multi_type_table.`year` or test_multi_type_table.`timestamp`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`year_or_timestamp(4)` = test_multi_type_table.`year` or test_multi_type_table.`timestamp(4)`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`year_or_year` = test_multi_type_table.`year` or test_multi_type_table.`year`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`year_or_char` = test_multi_type_table.`year` or test_multi_type_table.`char`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`year_or_varchar` = test_multi_type_table.`year` or test_multi_type_table.`varchar`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`year_or_binary` = test_multi_type_table.`year` or test_multi_type_table.`binary`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`year_or_varbinary` = test_multi_type_table.`year` or test_multi_type_table.`varbinary`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`year_or_tinyblob` = test_multi_type_table.`year` or test_multi_type_table.`tinyblob`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`year_or_blob` = test_multi_type_table.`year` or test_multi_type_table.`blob`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`year_or_mediumblob` = test_multi_type_table.`year` or test_multi_type_table.`mediumblob`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`year_or_longblob` = test_multi_type_table.`year` or test_multi_type_table.`longblob`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`year_or_text` = test_multi_type_table.`year` or test_multi_type_table.`text`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`year_or_enum_t` = test_multi_type_table.`year` or test_multi_type_table.`enum_t`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`year_or_set_t` = test_multi_type_table.`year` or test_multi_type_table.`set_t`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`year_or_json` = test_multi_type_table.`year` or test_multi_type_table.`json`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`char_or_int1` = test_multi_type_table.`char` or test_multi_type_table.`int1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`char_or_uint1` = test_multi_type_table.`char` or test_multi_type_table.`uint1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`char_or_int2` = test_multi_type_table.`char` or test_multi_type_table.`int2`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`char_or_uint2` = test_multi_type_table.`char` or test_multi_type_table.`uint2`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`char_or_int4` = test_multi_type_table.`char` or test_multi_type_table.`int4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`char_or_uint4` = test_multi_type_table.`char` or test_multi_type_table.`uint4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`char_or_int8` = test_multi_type_table.`char` or test_multi_type_table.`int8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`char_or_uint8` = test_multi_type_table.`char` or test_multi_type_table.`uint8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`char_or_float4` = test_multi_type_table.`char` or test_multi_type_table.`float4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`char_or_float8` = test_multi_type_table.`char` or test_multi_type_table.`float8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`char_or_numeric` = test_multi_type_table.`char` or test_multi_type_table.`numeric`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`char_or_bit1` = test_multi_type_table.`char` or test_multi_type_table.`bit1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`char_or_bit64` = test_multi_type_table.`char` or test_multi_type_table.`bit64`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`char_or_boolean` = test_multi_type_table.`char` or test_multi_type_table.`boolean`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`char_or_date` = test_multi_type_table.`char` or test_multi_type_table.`date`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`char_or_time` = test_multi_type_table.`char` or test_multi_type_table.`time`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`char_or_time(4)` = test_multi_type_table.`char` or test_multi_type_table.`time(4)`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`char_or_datetime` = test_multi_type_table.`char` or test_multi_type_table.`datetime`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`char_or_datetime(4)` = test_multi_type_table.`char` or test_multi_type_table.`datetime(4)`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`char_or_timestamp` = test_multi_type_table.`char` or test_multi_type_table.`timestamp`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`char_or_timestamp(4)` = test_multi_type_table.`char` or test_multi_type_table.`timestamp(4)`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`char_or_year` = test_multi_type_table.`char` or test_multi_type_table.`year`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`char_or_char` = test_multi_type_table.`char` or test_multi_type_table.`char`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`char_or_varchar` = test_multi_type_table.`char` or test_multi_type_table.`varchar`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`char_or_binary` = test_multi_type_table.`char` or test_multi_type_table.`binary`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`char_or_varbinary` = test_multi_type_table.`char` or test_multi_type_table.`varbinary`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`char_or_tinyblob` = test_multi_type_table.`char` or test_multi_type_table.`tinyblob`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`char_or_blob` = test_multi_type_table.`char` or test_multi_type_table.`blob`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`char_or_mediumblob` = test_multi_type_table.`char` or test_multi_type_table.`mediumblob`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`char_or_longblob` = test_multi_type_table.`char` or test_multi_type_table.`longblob`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`char_or_text` = test_multi_type_table.`char` or test_multi_type_table.`text`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`char_or_enum_t` = test_multi_type_table.`char` or test_multi_type_table.`enum_t`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`char_or_set_t` = test_multi_type_table.`char` or test_multi_type_table.`set_t`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`char_or_json` = test_multi_type_table.`char` or test_multi_type_table.`json`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`varchar_or_int1` = test_multi_type_table.`varchar` or test_multi_type_table.`int1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`varchar_or_uint1` = test_multi_type_table.`varchar` or test_multi_type_table.`uint1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`varchar_or_int2` = test_multi_type_table.`varchar` or test_multi_type_table.`int2`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`varchar_or_uint2` = test_multi_type_table.`varchar` or test_multi_type_table.`uint2`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`varchar_or_int4` = test_multi_type_table.`varchar` or test_multi_type_table.`int4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`varchar_or_uint4` = test_multi_type_table.`varchar` or test_multi_type_table.`uint4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`varchar_or_int8` = test_multi_type_table.`varchar` or test_multi_type_table.`int8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`varchar_or_uint8` = test_multi_type_table.`varchar` or test_multi_type_table.`uint8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`varchar_or_float4` = test_multi_type_table.`varchar` or test_multi_type_table.`float4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`varchar_or_float8` = test_multi_type_table.`varchar` or test_multi_type_table.`float8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`varchar_or_numeric` = test_multi_type_table.`varchar` or test_multi_type_table.`numeric`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`varchar_or_bit1` = test_multi_type_table.`varchar` or test_multi_type_table.`bit1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`varchar_or_bit64` = test_multi_type_table.`varchar` or test_multi_type_table.`bit64`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`varchar_or_boolean` = test_multi_type_table.`varchar` or test_multi_type_table.`boolean`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`varchar_or_date` = test_multi_type_table.`varchar` or test_multi_type_table.`date`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`varchar_or_time` = test_multi_type_table.`varchar` or test_multi_type_table.`time`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`varchar_or_time(4)` = test_multi_type_table.`varchar` or test_multi_type_table.`time(4)`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`varchar_or_datetime` = test_multi_type_table.`varchar` or test_multi_type_table.`datetime`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`varchar_or_datetime(4)` = test_multi_type_table.`varchar` or test_multi_type_table.`datetime(4)`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`varchar_or_timestamp` = test_multi_type_table.`varchar` or test_multi_type_table.`timestamp`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`varchar_or_timestamp(4)` = test_multi_type_table.`varchar` or test_multi_type_table.`timestamp(4)`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`varchar_or_year` = test_multi_type_table.`varchar` or test_multi_type_table.`year`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`varchar_or_char` = test_multi_type_table.`varchar` or test_multi_type_table.`char`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`varchar_or_varchar` = test_multi_type_table.`varchar` or test_multi_type_table.`varchar`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`varchar_or_binary` = test_multi_type_table.`varchar` or test_multi_type_table.`binary`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`varchar_or_varbinary` = test_multi_type_table.`varchar` or test_multi_type_table.`varbinary`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`varchar_or_tinyblob` = test_multi_type_table.`varchar` or test_multi_type_table.`tinyblob`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`varchar_or_blob` = test_multi_type_table.`varchar` or test_multi_type_table.`blob`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`varchar_or_mediumblob` = test_multi_type_table.`varchar` or test_multi_type_table.`mediumblob`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`varchar_or_longblob` = test_multi_type_table.`varchar` or test_multi_type_table.`longblob`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`varchar_or_text` = test_multi_type_table.`varchar` or test_multi_type_table.`text`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`varchar_or_enum_t` = test_multi_type_table.`varchar` or test_multi_type_table.`enum_t`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`varchar_or_set_t` = test_multi_type_table.`varchar` or test_multi_type_table.`set_t`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`varchar_or_json` = test_multi_type_table.`varchar` or test_multi_type_table.`json`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`binary_or_int1` = test_multi_type_table.`binary` or test_multi_type_table.`int1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`binary_or_uint1` = test_multi_type_table.`binary` or test_multi_type_table.`uint1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`binary_or_int2` = test_multi_type_table.`binary` or test_multi_type_table.`int2`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`binary_or_uint2` = test_multi_type_table.`binary` or test_multi_type_table.`uint2`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`binary_or_int4` = test_multi_type_table.`binary` or test_multi_type_table.`int4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`binary_or_uint4` = test_multi_type_table.`binary` or test_multi_type_table.`uint4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`binary_or_int8` = test_multi_type_table.`binary` or test_multi_type_table.`int8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`binary_or_uint8` = test_multi_type_table.`binary` or test_multi_type_table.`uint8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`binary_or_float4` = test_multi_type_table.`binary` or test_multi_type_table.`float4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`binary_or_float8` = test_multi_type_table.`binary` or test_multi_type_table.`float8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`binary_or_numeric` = test_multi_type_table.`binary` or test_multi_type_table.`numeric`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`binary_or_bit1` = test_multi_type_table.`binary` or test_multi_type_table.`bit1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`binary_or_bit64` = test_multi_type_table.`binary` or test_multi_type_table.`bit64`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`binary_or_boolean` = test_multi_type_table.`binary` or test_multi_type_table.`boolean`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`binary_or_date` = test_multi_type_table.`binary` or test_multi_type_table.`date`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`binary_or_time` = test_multi_type_table.`binary` or test_multi_type_table.`time`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`binary_or_time(4)` = test_multi_type_table.`binary` or test_multi_type_table.`time(4)`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`binary_or_datetime` = test_multi_type_table.`binary` or test_multi_type_table.`datetime`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`binary_or_datetime(4)` = test_multi_type_table.`binary` or test_multi_type_table.`datetime(4)`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`binary_or_timestamp` = test_multi_type_table.`binary` or test_multi_type_table.`timestamp`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`binary_or_timestamp(4)` = test_multi_type_table.`binary` or test_multi_type_table.`timestamp(4)`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`binary_or_year` = test_multi_type_table.`binary` or test_multi_type_table.`year`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`binary_or_char` = test_multi_type_table.`binary` or test_multi_type_table.`char`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`binary_or_varchar` = test_multi_type_table.`binary` or test_multi_type_table.`varchar`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`binary_or_binary` = test_multi_type_table.`binary` or test_multi_type_table.`binary`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`binary_or_varbinary` = test_multi_type_table.`binary` or test_multi_type_table.`varbinary`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`binary_or_tinyblob` = test_multi_type_table.`binary` or test_multi_type_table.`tinyblob`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`binary_or_blob` = test_multi_type_table.`binary` or test_multi_type_table.`blob`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`binary_or_mediumblob` = test_multi_type_table.`binary` or test_multi_type_table.`mediumblob`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`binary_or_longblob` = test_multi_type_table.`binary` or test_multi_type_table.`longblob`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`binary_or_text` = test_multi_type_table.`binary` or test_multi_type_table.`text`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`binary_or_enum_t` = test_multi_type_table.`binary` or test_multi_type_table.`enum_t`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`binary_or_set_t` = test_multi_type_table.`binary` or test_multi_type_table.`set_t`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`binary_or_json` = test_multi_type_table.`binary` or test_multi_type_table.`json`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`varbinary_or_int1` = test_multi_type_table.`varbinary` or test_multi_type_table.`int1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`varbinary_or_uint1` = test_multi_type_table.`varbinary` or test_multi_type_table.`uint1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`varbinary_or_int2` = test_multi_type_table.`varbinary` or test_multi_type_table.`int2`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`varbinary_or_uint2` = test_multi_type_table.`varbinary` or test_multi_type_table.`uint2`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`varbinary_or_int4` = test_multi_type_table.`varbinary` or test_multi_type_table.`int4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`varbinary_or_uint4` = test_multi_type_table.`varbinary` or test_multi_type_table.`uint4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`varbinary_or_int8` = test_multi_type_table.`varbinary` or test_multi_type_table.`int8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`varbinary_or_uint8` = test_multi_type_table.`varbinary` or test_multi_type_table.`uint8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`varbinary_or_float4` = test_multi_type_table.`varbinary` or test_multi_type_table.`float4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`varbinary_or_float8` = test_multi_type_table.`varbinary` or test_multi_type_table.`float8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`varbinary_or_numeric` = test_multi_type_table.`varbinary` or test_multi_type_table.`numeric`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`varbinary_or_bit1` = test_multi_type_table.`varbinary` or test_multi_type_table.`bit1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`varbinary_or_bit64` = test_multi_type_table.`varbinary` or test_multi_type_table.`bit64`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`varbinary_or_boolean` = test_multi_type_table.`varbinary` or test_multi_type_table.`boolean`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`varbinary_or_date` = test_multi_type_table.`varbinary` or test_multi_type_table.`date`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`varbinary_or_time` = test_multi_type_table.`varbinary` or test_multi_type_table.`time`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`varbinary_or_time(4)` = test_multi_type_table.`varbinary` or test_multi_type_table.`time(4)`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`varbinary_or_datetime` = test_multi_type_table.`varbinary` or test_multi_type_table.`datetime`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`varbinary_or_datetime(4)` = test_multi_type_table.`varbinary` or test_multi_type_table.`datetime(4)`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`varbinary_or_timestamp` = test_multi_type_table.`varbinary` or test_multi_type_table.`timestamp`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`varbinary_or_timestamp(4)` = test_multi_type_table.`varbinary` or test_multi_type_table.`timestamp(4)`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`varbinary_or_year` = test_multi_type_table.`varbinary` or test_multi_type_table.`year`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`varbinary_or_char` = test_multi_type_table.`varbinary` or test_multi_type_table.`char`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`varbinary_or_varchar` = test_multi_type_table.`varbinary` or test_multi_type_table.`varchar`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`varbinary_or_binary` = test_multi_type_table.`varbinary` or test_multi_type_table.`binary`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`varbinary_or_varbinary` = test_multi_type_table.`varbinary` or test_multi_type_table.`varbinary`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`varbinary_or_tinyblob` = test_multi_type_table.`varbinary` or test_multi_type_table.`tinyblob`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`varbinary_or_blob` = test_multi_type_table.`varbinary` or test_multi_type_table.`blob`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`varbinary_or_mediumblob` = test_multi_type_table.`varbinary` or test_multi_type_table.`mediumblob`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`varbinary_or_longblob` = test_multi_type_table.`varbinary` or test_multi_type_table.`longblob`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`varbinary_or_text` = test_multi_type_table.`varbinary` or test_multi_type_table.`text`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`varbinary_or_enum_t` = test_multi_type_table.`varbinary` or test_multi_type_table.`enum_t`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`varbinary_or_set_t` = test_multi_type_table.`varbinary` or test_multi_type_table.`set_t`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`varbinary_or_json` = test_multi_type_table.`varbinary` or test_multi_type_table.`json`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`tinyblob_or_int1` = test_multi_type_table.`tinyblob` or test_multi_type_table.`int1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`tinyblob_or_uint1` = test_multi_type_table.`tinyblob` or test_multi_type_table.`uint1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`tinyblob_or_int2` = test_multi_type_table.`tinyblob` or test_multi_type_table.`int2`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`tinyblob_or_uint2` = test_multi_type_table.`tinyblob` or test_multi_type_table.`uint2`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`tinyblob_or_int4` = test_multi_type_table.`tinyblob` or test_multi_type_table.`int4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`tinyblob_or_uint4` = test_multi_type_table.`tinyblob` or test_multi_type_table.`uint4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`tinyblob_or_int8` = test_multi_type_table.`tinyblob` or test_multi_type_table.`int8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`tinyblob_or_uint8` = test_multi_type_table.`tinyblob` or test_multi_type_table.`uint8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`tinyblob_or_float4` = test_multi_type_table.`tinyblob` or test_multi_type_table.`float4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`tinyblob_or_float8` = test_multi_type_table.`tinyblob` or test_multi_type_table.`float8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`tinyblob_or_numeric` = test_multi_type_table.`tinyblob` or test_multi_type_table.`numeric`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`tinyblob_or_bit1` = test_multi_type_table.`tinyblob` or test_multi_type_table.`bit1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`tinyblob_or_bit64` = test_multi_type_table.`tinyblob` or test_multi_type_table.`bit64`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`tinyblob_or_boolean` = test_multi_type_table.`tinyblob` or test_multi_type_table.`boolean`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`tinyblob_or_date` = test_multi_type_table.`tinyblob` or test_multi_type_table.`date`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`tinyblob_or_time` = test_multi_type_table.`tinyblob` or test_multi_type_table.`time`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`tinyblob_or_time(4)` = test_multi_type_table.`tinyblob` or test_multi_type_table.`time(4)`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`tinyblob_or_datetime` = test_multi_type_table.`tinyblob` or test_multi_type_table.`datetime`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`tinyblob_or_datetime(4)` = test_multi_type_table.`tinyblob` or test_multi_type_table.`datetime(4)`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`tinyblob_or_timestamp` = test_multi_type_table.`tinyblob` or test_multi_type_table.`timestamp`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`tinyblob_or_timestamp(4)` = test_multi_type_table.`tinyblob` or test_multi_type_table.`timestamp(4)`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`tinyblob_or_year` = test_multi_type_table.`tinyblob` or test_multi_type_table.`year`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`tinyblob_or_char` = test_multi_type_table.`tinyblob` or test_multi_type_table.`char`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`tinyblob_or_varchar` = test_multi_type_table.`tinyblob` or test_multi_type_table.`varchar`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`tinyblob_or_binary` = test_multi_type_table.`tinyblob` or test_multi_type_table.`binary`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`tinyblob_or_varbinary` = test_multi_type_table.`tinyblob` or test_multi_type_table.`varbinary`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`tinyblob_or_tinyblob` = test_multi_type_table.`tinyblob` or test_multi_type_table.`tinyblob`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`tinyblob_or_blob` = test_multi_type_table.`tinyblob` or test_multi_type_table.`blob`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`tinyblob_or_mediumblob` = test_multi_type_table.`tinyblob` or test_multi_type_table.`mediumblob`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`tinyblob_or_longblob` = test_multi_type_table.`tinyblob` or test_multi_type_table.`longblob`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`tinyblob_or_text` = test_multi_type_table.`tinyblob` or test_multi_type_table.`text`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`tinyblob_or_enum_t` = test_multi_type_table.`tinyblob` or test_multi_type_table.`enum_t`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`tinyblob_or_set_t` = test_multi_type_table.`tinyblob` or test_multi_type_table.`set_t`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`tinyblob_or_json` = test_multi_type_table.`tinyblob` or test_multi_type_table.`json`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`blob_or_int1` = test_multi_type_table.`blob` or test_multi_type_table.`int1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`blob_or_uint1` = test_multi_type_table.`blob` or test_multi_type_table.`uint1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`blob_or_int2` = test_multi_type_table.`blob` or test_multi_type_table.`int2`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`blob_or_uint2` = test_multi_type_table.`blob` or test_multi_type_table.`uint2`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`blob_or_int4` = test_multi_type_table.`blob` or test_multi_type_table.`int4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`blob_or_uint4` = test_multi_type_table.`blob` or test_multi_type_table.`uint4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`blob_or_int8` = test_multi_type_table.`blob` or test_multi_type_table.`int8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`blob_or_uint8` = test_multi_type_table.`blob` or test_multi_type_table.`uint8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`blob_or_float4` = test_multi_type_table.`blob` or test_multi_type_table.`float4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`blob_or_float8` = test_multi_type_table.`blob` or test_multi_type_table.`float8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`blob_or_numeric` = test_multi_type_table.`blob` or test_multi_type_table.`numeric`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`blob_or_bit1` = test_multi_type_table.`blob` or test_multi_type_table.`bit1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`blob_or_bit64` = test_multi_type_table.`blob` or test_multi_type_table.`bit64`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`blob_or_boolean` = test_multi_type_table.`blob` or test_multi_type_table.`boolean`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`blob_or_date` = test_multi_type_table.`blob` or test_multi_type_table.`date`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`blob_or_time` = test_multi_type_table.`blob` or test_multi_type_table.`time`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`blob_or_time(4)` = test_multi_type_table.`blob` or test_multi_type_table.`time(4)`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`blob_or_datetime` = test_multi_type_table.`blob` or test_multi_type_table.`datetime`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`blob_or_datetime(4)` = test_multi_type_table.`blob` or test_multi_type_table.`datetime(4)`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`blob_or_timestamp` = test_multi_type_table.`blob` or test_multi_type_table.`timestamp`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`blob_or_timestamp(4)` = test_multi_type_table.`blob` or test_multi_type_table.`timestamp(4)`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`blob_or_year` = test_multi_type_table.`blob` or test_multi_type_table.`year`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`blob_or_char` = test_multi_type_table.`blob` or test_multi_type_table.`char`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`blob_or_varchar` = test_multi_type_table.`blob` or test_multi_type_table.`varchar`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`blob_or_binary` = test_multi_type_table.`blob` or test_multi_type_table.`binary`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`blob_or_varbinary` = test_multi_type_table.`blob` or test_multi_type_table.`varbinary`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`blob_or_tinyblob` = test_multi_type_table.`blob` or test_multi_type_table.`tinyblob`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`blob_or_blob` = test_multi_type_table.`blob` or test_multi_type_table.`blob`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`blob_or_mediumblob` = test_multi_type_table.`blob` or test_multi_type_table.`mediumblob`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`blob_or_longblob` = test_multi_type_table.`blob` or test_multi_type_table.`longblob`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`blob_or_text` = test_multi_type_table.`blob` or test_multi_type_table.`text`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`blob_or_enum_t` = test_multi_type_table.`blob` or test_multi_type_table.`enum_t`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`blob_or_set_t` = test_multi_type_table.`blob` or test_multi_type_table.`set_t`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`blob_or_json` = test_multi_type_table.`blob` or test_multi_type_table.`json`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`mediumblob_or_int1` = test_multi_type_table.`mediumblob` or test_multi_type_table.`int1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`mediumblob_or_uint1` = test_multi_type_table.`mediumblob` or test_multi_type_table.`uint1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`mediumblob_or_int2` = test_multi_type_table.`mediumblob` or test_multi_type_table.`int2`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`mediumblob_or_uint2` = test_multi_type_table.`mediumblob` or test_multi_type_table.`uint2`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`mediumblob_or_int4` = test_multi_type_table.`mediumblob` or test_multi_type_table.`int4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`mediumblob_or_uint4` = test_multi_type_table.`mediumblob` or test_multi_type_table.`uint4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`mediumblob_or_int8` = test_multi_type_table.`mediumblob` or test_multi_type_table.`int8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`mediumblob_or_uint8` = test_multi_type_table.`mediumblob` or test_multi_type_table.`uint8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`mediumblob_or_float4` = test_multi_type_table.`mediumblob` or test_multi_type_table.`float4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`mediumblob_or_float8` = test_multi_type_table.`mediumblob` or test_multi_type_table.`float8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`mediumblob_or_numeric` = test_multi_type_table.`mediumblob` or test_multi_type_table.`numeric`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`mediumblob_or_bit1` = test_multi_type_table.`mediumblob` or test_multi_type_table.`bit1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`mediumblob_or_bit64` = test_multi_type_table.`mediumblob` or test_multi_type_table.`bit64`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`mediumblob_or_boolean` = test_multi_type_table.`mediumblob` or test_multi_type_table.`boolean`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`mediumblob_or_date` = test_multi_type_table.`mediumblob` or test_multi_type_table.`date`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`mediumblob_or_time` = test_multi_type_table.`mediumblob` or test_multi_type_table.`time`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`mediumblob_or_time(4)` = test_multi_type_table.`mediumblob` or test_multi_type_table.`time(4)`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`mediumblob_or_datetime` = test_multi_type_table.`mediumblob` or test_multi_type_table.`datetime`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`mediumblob_or_datetime(4)` = test_multi_type_table.`mediumblob` or test_multi_type_table.`datetime(4)`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`mediumblob_or_timestamp` = test_multi_type_table.`mediumblob` or test_multi_type_table.`timestamp`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`mediumblob_or_timestamp(4)` = test_multi_type_table.`mediumblob` or test_multi_type_table.`timestamp(4)`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`mediumblob_or_year` = test_multi_type_table.`mediumblob` or test_multi_type_table.`year`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`mediumblob_or_char` = test_multi_type_table.`mediumblob` or test_multi_type_table.`char`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`mediumblob_or_varchar` = test_multi_type_table.`mediumblob` or test_multi_type_table.`varchar`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`mediumblob_or_binary` = test_multi_type_table.`mediumblob` or test_multi_type_table.`binary`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`mediumblob_or_varbinary` = test_multi_type_table.`mediumblob` or test_multi_type_table.`varbinary`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`mediumblob_or_tinyblob` = test_multi_type_table.`mediumblob` or test_multi_type_table.`tinyblob`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`mediumblob_or_blob` = test_multi_type_table.`mediumblob` or test_multi_type_table.`blob`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`mediumblob_or_mediumblob` = test_multi_type_table.`mediumblob` or test_multi_type_table.`mediumblob`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`mediumblob_or_longblob` = test_multi_type_table.`mediumblob` or test_multi_type_table.`longblob`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`mediumblob_or_text` = test_multi_type_table.`mediumblob` or test_multi_type_table.`text`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`mediumblob_or_enum_t` = test_multi_type_table.`mediumblob` or test_multi_type_table.`enum_t`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`mediumblob_or_set_t` = test_multi_type_table.`mediumblob` or test_multi_type_table.`set_t`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`mediumblob_or_json` = test_multi_type_table.`mediumblob` or test_multi_type_table.`json`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`longblob_or_int1` = test_multi_type_table.`longblob` or test_multi_type_table.`int1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`longblob_or_uint1` = test_multi_type_table.`longblob` or test_multi_type_table.`uint1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`longblob_or_int2` = test_multi_type_table.`longblob` or test_multi_type_table.`int2`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`longblob_or_uint2` = test_multi_type_table.`longblob` or test_multi_type_table.`uint2`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`longblob_or_int4` = test_multi_type_table.`longblob` or test_multi_type_table.`int4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`longblob_or_uint4` = test_multi_type_table.`longblob` or test_multi_type_table.`uint4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`longblob_or_int8` = test_multi_type_table.`longblob` or test_multi_type_table.`int8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`longblob_or_uint8` = test_multi_type_table.`longblob` or test_multi_type_table.`uint8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`longblob_or_float4` = test_multi_type_table.`longblob` or test_multi_type_table.`float4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`longblob_or_float8` = test_multi_type_table.`longblob` or test_multi_type_table.`float8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`longblob_or_numeric` = test_multi_type_table.`longblob` or test_multi_type_table.`numeric`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`longblob_or_bit1` = test_multi_type_table.`longblob` or test_multi_type_table.`bit1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`longblob_or_bit64` = test_multi_type_table.`longblob` or test_multi_type_table.`bit64`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`longblob_or_boolean` = test_multi_type_table.`longblob` or test_multi_type_table.`boolean`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`longblob_or_date` = test_multi_type_table.`longblob` or test_multi_type_table.`date`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`longblob_or_time` = test_multi_type_table.`longblob` or test_multi_type_table.`time`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`longblob_or_time(4)` = test_multi_type_table.`longblob` or test_multi_type_table.`time(4)`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`longblob_or_datetime` = test_multi_type_table.`longblob` or test_multi_type_table.`datetime`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`longblob_or_datetime(4)` = test_multi_type_table.`longblob` or test_multi_type_table.`datetime(4)`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`longblob_or_timestamp` = test_multi_type_table.`longblob` or test_multi_type_table.`timestamp`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`longblob_or_timestamp(4)` = test_multi_type_table.`longblob` or test_multi_type_table.`timestamp(4)`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`longblob_or_year` = test_multi_type_table.`longblob` or test_multi_type_table.`year`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`longblob_or_char` = test_multi_type_table.`longblob` or test_multi_type_table.`char`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`longblob_or_varchar` = test_multi_type_table.`longblob` or test_multi_type_table.`varchar`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`longblob_or_binary` = test_multi_type_table.`longblob` or test_multi_type_table.`binary`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`longblob_or_varbinary` = test_multi_type_table.`longblob` or test_multi_type_table.`varbinary`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`longblob_or_tinyblob` = test_multi_type_table.`longblob` or test_multi_type_table.`tinyblob`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`longblob_or_blob` = test_multi_type_table.`longblob` or test_multi_type_table.`blob`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`longblob_or_mediumblob` = test_multi_type_table.`longblob` or test_multi_type_table.`mediumblob`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`longblob_or_longblob` = test_multi_type_table.`longblob` or test_multi_type_table.`longblob`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`longblob_or_text` = test_multi_type_table.`longblob` or test_multi_type_table.`text`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`longblob_or_enum_t` = test_multi_type_table.`longblob` or test_multi_type_table.`enum_t`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`longblob_or_set_t` = test_multi_type_table.`longblob` or test_multi_type_table.`set_t`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`longblob_or_json` = test_multi_type_table.`longblob` or test_multi_type_table.`json`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`text_or_int1` = test_multi_type_table.`text` or test_multi_type_table.`int1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`text_or_uint1` = test_multi_type_table.`text` or test_multi_type_table.`uint1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`text_or_int2` = test_multi_type_table.`text` or test_multi_type_table.`int2`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`text_or_uint2` = test_multi_type_table.`text` or test_multi_type_table.`uint2`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`text_or_int4` = test_multi_type_table.`text` or test_multi_type_table.`int4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`text_or_uint4` = test_multi_type_table.`text` or test_multi_type_table.`uint4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`text_or_int8` = test_multi_type_table.`text` or test_multi_type_table.`int8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`text_or_uint8` = test_multi_type_table.`text` or test_multi_type_table.`uint8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`text_or_float4` = test_multi_type_table.`text` or test_multi_type_table.`float4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`text_or_float8` = test_multi_type_table.`text` or test_multi_type_table.`float8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`text_or_numeric` = test_multi_type_table.`text` or test_multi_type_table.`numeric`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`text_or_bit1` = test_multi_type_table.`text` or test_multi_type_table.`bit1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`text_or_bit64` = test_multi_type_table.`text` or test_multi_type_table.`bit64`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`text_or_boolean` = test_multi_type_table.`text` or test_multi_type_table.`boolean`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`text_or_date` = test_multi_type_table.`text` or test_multi_type_table.`date`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`text_or_time` = test_multi_type_table.`text` or test_multi_type_table.`time`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`text_or_time(4)` = test_multi_type_table.`text` or test_multi_type_table.`time(4)`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`text_or_datetime` = test_multi_type_table.`text` or test_multi_type_table.`datetime`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`text_or_datetime(4)` = test_multi_type_table.`text` or test_multi_type_table.`datetime(4)`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`text_or_timestamp` = test_multi_type_table.`text` or test_multi_type_table.`timestamp`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`text_or_timestamp(4)` = test_multi_type_table.`text` or test_multi_type_table.`timestamp(4)`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`text_or_year` = test_multi_type_table.`text` or test_multi_type_table.`year`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`text_or_char` = test_multi_type_table.`text` or test_multi_type_table.`char`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`text_or_varchar` = test_multi_type_table.`text` or test_multi_type_table.`varchar`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`text_or_binary` = test_multi_type_table.`text` or test_multi_type_table.`binary`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`text_or_varbinary` = test_multi_type_table.`text` or test_multi_type_table.`varbinary`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`text_or_tinyblob` = test_multi_type_table.`text` or test_multi_type_table.`tinyblob`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`text_or_blob` = test_multi_type_table.`text` or test_multi_type_table.`blob`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`text_or_mediumblob` = test_multi_type_table.`text` or test_multi_type_table.`mediumblob`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`text_or_longblob` = test_multi_type_table.`text` or test_multi_type_table.`longblob`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`text_or_text` = test_multi_type_table.`text` or test_multi_type_table.`text`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`text_or_enum_t` = test_multi_type_table.`text` or test_multi_type_table.`enum_t`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`text_or_set_t` = test_multi_type_table.`text` or test_multi_type_table.`set_t`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`text_or_json` = test_multi_type_table.`text` or test_multi_type_table.`json`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`enum_t_or_int1` = test_multi_type_table.`enum_t` or test_multi_type_table.`int1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`enum_t_or_uint1` = test_multi_type_table.`enum_t` or test_multi_type_table.`uint1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`enum_t_or_int2` = test_multi_type_table.`enum_t` or test_multi_type_table.`int2`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`enum_t_or_uint2` = test_multi_type_table.`enum_t` or test_multi_type_table.`uint2`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`enum_t_or_int4` = test_multi_type_table.`enum_t` or test_multi_type_table.`int4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`enum_t_or_uint4` = test_multi_type_table.`enum_t` or test_multi_type_table.`uint4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`enum_t_or_int8` = test_multi_type_table.`enum_t` or test_multi_type_table.`int8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`enum_t_or_uint8` = test_multi_type_table.`enum_t` or test_multi_type_table.`uint8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`enum_t_or_float4` = test_multi_type_table.`enum_t` or test_multi_type_table.`float4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`enum_t_or_float8` = test_multi_type_table.`enum_t` or test_multi_type_table.`float8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`enum_t_or_numeric` = test_multi_type_table.`enum_t` or test_multi_type_table.`numeric`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`enum_t_or_bit1` = test_multi_type_table.`enum_t` or test_multi_type_table.`bit1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`enum_t_or_bit64` = test_multi_type_table.`enum_t` or test_multi_type_table.`bit64`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`enum_t_or_boolean` = test_multi_type_table.`enum_t` or test_multi_type_table.`boolean`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`enum_t_or_date` = test_multi_type_table.`enum_t` or test_multi_type_table.`date`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`enum_t_or_time` = test_multi_type_table.`enum_t` or test_multi_type_table.`time`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`enum_t_or_time(4)` = test_multi_type_table.`enum_t` or test_multi_type_table.`time(4)`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`enum_t_or_datetime` = test_multi_type_table.`enum_t` or test_multi_type_table.`datetime`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`enum_t_or_datetime(4)` = test_multi_type_table.`enum_t` or test_multi_type_table.`datetime(4)`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`enum_t_or_timestamp` = test_multi_type_table.`enum_t` or test_multi_type_table.`timestamp`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`enum_t_or_timestamp(4)` = test_multi_type_table.`enum_t` or test_multi_type_table.`timestamp(4)`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`enum_t_or_year` = test_multi_type_table.`enum_t` or test_multi_type_table.`year`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`enum_t_or_char` = test_multi_type_table.`enum_t` or test_multi_type_table.`char`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`enum_t_or_varchar` = test_multi_type_table.`enum_t` or test_multi_type_table.`varchar`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`enum_t_or_binary` = test_multi_type_table.`enum_t` or test_multi_type_table.`binary`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`enum_t_or_varbinary` = test_multi_type_table.`enum_t` or test_multi_type_table.`varbinary`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`enum_t_or_tinyblob` = test_multi_type_table.`enum_t` or test_multi_type_table.`tinyblob`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`enum_t_or_blob` = test_multi_type_table.`enum_t` or test_multi_type_table.`blob`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`enum_t_or_mediumblob` = test_multi_type_table.`enum_t` or test_multi_type_table.`mediumblob`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`enum_t_or_longblob` = test_multi_type_table.`enum_t` or test_multi_type_table.`longblob`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`enum_t_or_text` = test_multi_type_table.`enum_t` or test_multi_type_table.`text`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`enum_t_or_enum_t` = test_multi_type_table.`enum_t` or test_multi_type_table.`enum_t`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`enum_t_or_set_t` = test_multi_type_table.`enum_t` or test_multi_type_table.`set_t`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`enum_t_or_json` = test_multi_type_table.`enum_t` or test_multi_type_table.`json`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`set_t_or_int1` = test_multi_type_table.`set_t` or test_multi_type_table.`int1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`set_t_or_uint1` = test_multi_type_table.`set_t` or test_multi_type_table.`uint1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`set_t_or_int2` = test_multi_type_table.`set_t` or test_multi_type_table.`int2`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`set_t_or_uint2` = test_multi_type_table.`set_t` or test_multi_type_table.`uint2`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`set_t_or_int4` = test_multi_type_table.`set_t` or test_multi_type_table.`int4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`set_t_or_uint4` = test_multi_type_table.`set_t` or test_multi_type_table.`uint4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`set_t_or_int8` = test_multi_type_table.`set_t` or test_multi_type_table.`int8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`set_t_or_uint8` = test_multi_type_table.`set_t` or test_multi_type_table.`uint8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`set_t_or_float4` = test_multi_type_table.`set_t` or test_multi_type_table.`float4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`set_t_or_float8` = test_multi_type_table.`set_t` or test_multi_type_table.`float8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`set_t_or_numeric` = test_multi_type_table.`set_t` or test_multi_type_table.`numeric`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`set_t_or_bit1` = test_multi_type_table.`set_t` or test_multi_type_table.`bit1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`set_t_or_bit64` = test_multi_type_table.`set_t` or test_multi_type_table.`bit64`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`set_t_or_boolean` = test_multi_type_table.`set_t` or test_multi_type_table.`boolean`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`set_t_or_date` = test_multi_type_table.`set_t` or test_multi_type_table.`date`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`set_t_or_time` = test_multi_type_table.`set_t` or test_multi_type_table.`time`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`set_t_or_time(4)` = test_multi_type_table.`set_t` or test_multi_type_table.`time(4)`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`set_t_or_datetime` = test_multi_type_table.`set_t` or test_multi_type_table.`datetime`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`set_t_or_datetime(4)` = test_multi_type_table.`set_t` or test_multi_type_table.`datetime(4)`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`set_t_or_timestamp` = test_multi_type_table.`set_t` or test_multi_type_table.`timestamp`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`set_t_or_timestamp(4)` = test_multi_type_table.`set_t` or test_multi_type_table.`timestamp(4)`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`set_t_or_year` = test_multi_type_table.`set_t` or test_multi_type_table.`year`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`set_t_or_char` = test_multi_type_table.`set_t` or test_multi_type_table.`char`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`set_t_or_varchar` = test_multi_type_table.`set_t` or test_multi_type_table.`varchar`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`set_t_or_binary` = test_multi_type_table.`set_t` or test_multi_type_table.`binary`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`set_t_or_varbinary` = test_multi_type_table.`set_t` or test_multi_type_table.`varbinary`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`set_t_or_tinyblob` = test_multi_type_table.`set_t` or test_multi_type_table.`tinyblob`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`set_t_or_blob` = test_multi_type_table.`set_t` or test_multi_type_table.`blob`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`set_t_or_mediumblob` = test_multi_type_table.`set_t` or test_multi_type_table.`mediumblob`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`set_t_or_longblob` = test_multi_type_table.`set_t` or test_multi_type_table.`longblob`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`set_t_or_text` = test_multi_type_table.`set_t` or test_multi_type_table.`text`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`set_t_or_enum_t` = test_multi_type_table.`set_t` or test_multi_type_table.`enum_t`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`set_t_or_set_t` = test_multi_type_table.`set_t` or test_multi_type_table.`set_t`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`set_t_or_json` = test_multi_type_table.`set_t` or test_multi_type_table.`json`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`json_or_int1` = test_multi_type_table.`json` or test_multi_type_table.`int1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`json_or_uint1` = test_multi_type_table.`json` or test_multi_type_table.`uint1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`json_or_int2` = test_multi_type_table.`json` or test_multi_type_table.`int2`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`json_or_uint2` = test_multi_type_table.`json` or test_multi_type_table.`uint2`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`json_or_int4` = test_multi_type_table.`json` or test_multi_type_table.`int4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`json_or_uint4` = test_multi_type_table.`json` or test_multi_type_table.`uint4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`json_or_int8` = test_multi_type_table.`json` or test_multi_type_table.`int8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`json_or_uint8` = test_multi_type_table.`json` or test_multi_type_table.`uint8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`json_or_float4` = test_multi_type_table.`json` or test_multi_type_table.`float4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`json_or_float8` = test_multi_type_table.`json` or test_multi_type_table.`float8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`json_or_numeric` = test_multi_type_table.`json` or test_multi_type_table.`numeric`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`json_or_bit1` = test_multi_type_table.`json` or test_multi_type_table.`bit1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`json_or_bit64` = test_multi_type_table.`json` or test_multi_type_table.`bit64`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`json_or_boolean` = test_multi_type_table.`json` or test_multi_type_table.`boolean`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`json_or_date` = test_multi_type_table.`json` or test_multi_type_table.`date`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`json_or_time` = test_multi_type_table.`json` or test_multi_type_table.`time`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`json_or_time(4)` = test_multi_type_table.`json` or test_multi_type_table.`time(4)`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`json_or_datetime` = test_multi_type_table.`json` or test_multi_type_table.`datetime`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`json_or_datetime(4)` = test_multi_type_table.`json` or test_multi_type_table.`datetime(4)`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`json_or_timestamp` = test_multi_type_table.`json` or test_multi_type_table.`timestamp`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`json_or_timestamp(4)` = test_multi_type_table.`json` or test_multi_type_table.`timestamp(4)`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`json_or_year` = test_multi_type_table.`json` or test_multi_type_table.`year`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`json_or_char` = test_multi_type_table.`json` or test_multi_type_table.`char`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`json_or_varchar` = test_multi_type_table.`json` or test_multi_type_table.`varchar`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`json_or_binary` = test_multi_type_table.`json` or test_multi_type_table.`binary`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`json_or_varbinary` = test_multi_type_table.`json` or test_multi_type_table.`varbinary`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`json_or_tinyblob` = test_multi_type_table.`json` or test_multi_type_table.`tinyblob`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`json_or_blob` = test_multi_type_table.`json` or test_multi_type_table.`blob`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`json_or_mediumblob` = test_multi_type_table.`json` or test_multi_type_table.`mediumblob`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`json_or_longblob` = test_multi_type_table.`json` or test_multi_type_table.`longblob`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`json_or_text` = test_multi_type_table.`json` or test_multi_type_table.`text`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`json_or_enum_t` = test_multi_type_table.`json` or test_multi_type_table.`enum_t`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`json_or_set_t` = test_multi_type_table.`json` or test_multi_type_table.`set_t`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`json_or_json` = test_multi_type_table.`json` or test_multi_type_table.`json`;
\x
SELECT * FROM test_multi_type;
---------- tail ----------
drop schema multi_type_or_test_schema cascade;
reset current_schema;

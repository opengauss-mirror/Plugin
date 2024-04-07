drop schema if exists nullsafe_eq_in_schema cascade;
create schema nullsafe_eq_in_schema;
set current_schema to 'nullsafe_eq_in_schema';
---------- head ----------
set dolphin.b_compatibility_mode to on;
set dolphin.sql_mode to 'sql_mode_full_group,pipes_as_concat,ansi_quotes,no_zero_date';
drop table if exists test_multi_type_table;
CREATE TABLE test_multi_type_table(`int1` tinyint, `uint1` tinyint unsigned, `int2` smallint, `uint2` smallint unsigned, `int4` integer, `uint4` integer unsigned, `int8` bigint, `uint8` bigint unsigned, `float4` float4, `float8` float8, `numeric` decimal(20, 6), `bit1` bit(1), `bit64` bit(64), `boolean` boolean, `date` date, `time` time, `time(4)` time(4), `datetime` datetime, `datetime(4)` datetime(4) default '2022-11-11 11:11:11', `timestamp` timestamp, `timestamp(4)` timestamp(4) default '2022-11-11 11:11:11', `year` year, `char` char(100), `varchar` varchar(100), `binary` binary(100), `varbinary` varbinary(100), `tinyblob` tinyblob, `blob` blob, `mediumblob` mediumblob, `longblob` longblob, `text` text, `enum_t` enum('a', 'b', 'c'), `set_t` set('a', 'b', 'c'), `json` json);

INSERT INTO test_multi_type_table VALUES(1,1,1,1,1,1,1,1,1,1,1,b'1',b'1',true,'2023-02-05', '19:10:50', '19:10:50.3456', '2023-02-05 19:10:50', '2023-02-05 19:10:50.456', '2023-02-05 19:10:50', '2023-02-05 19:10:50.456', '2023', '1.23a', '1.23a', '1.23a', '1.23a', '1.23a', '1.23a', '1.23a', '1.23a', '1.23a', 'a', 'b,c', json_object('a',1,'b',1));

CREATE TABLE test_multi_type SELECT
`int1` <=> `int1` in (select `int1` from test_multi_type_table) AS `int1_in_int1`,
`uint1` <=> `uint1` in (select `uint1` from test_multi_type_table) AS `uint1_in_uint1`,
`int2` <=> `int2` in (select `int2` from test_multi_type_table) AS `int2_in_int2`,
`uint2` <=> `uint2` in (select `uint2` from test_multi_type_table) AS `uint2_in_uint2`,
`int4` <=> `int4` in (select `int4` from test_multi_type_table) AS `int4_in_int4`,
`uint4` <=> `uint4` in (select `uint4` from test_multi_type_table) AS `uint4_in_uint4`,
`int8` <=> `int8` in (select `int8` from test_multi_type_table) AS `int8_in_int8`,
`uint8` <=> `uint8` in (select `uint8` from test_multi_type_table) AS `uint8_in_uint8`,
`float4` <=> `float4` in (select `float4` from test_multi_type_table) AS `float4_in_float4`,
`float8` <=> `float8` in (select `float8` from test_multi_type_table) AS `float8_in_float8`,
`numeric` <=> `numeric` in (select `numeric` from test_multi_type_table) AS `numeric_in_numeric`,
`bit1` <=> `bit1` in (select `bit1` from test_multi_type_table) AS `bit1_in_bit1`,
`bit64` <=> `bit64` in (select `bit64` from test_multi_type_table) AS `bit64_in_bit64`,
`boolean` <=> `boolean` in (select `boolean` from test_multi_type_table) AS `boolean_in_boolean`,
`date` <=> `date` in (select `date` from test_multi_type_table) AS `date_in_date`,
`time` <=> `time` in (select `time` from test_multi_type_table) AS `time_in_time`,
`time(4)` <=> `time(4)` in (select `time(4)` from test_multi_type_table) AS `time(4)_in_time(4)`,
`datetime` <=> `datetime` in (select `datetime` from test_multi_type_table) AS `datetime_in_datetime`,
`datetime(4)` <=> `datetime(4)` in (select `datetime(4)` from test_multi_type_table) AS `datetime(4)_in_datetime(4)`,
`timestamp` <=> `timestamp` in (select `timestamp` from test_multi_type_table) AS `timestamp_in_timestamp`,
`timestamp(4)` <=> `timestamp(4)` in (select `timestamp(4)` from test_multi_type_table) AS `timestamp(4)_in_timestamp(4)`,
`year` <=> `year` in (select `year` from test_multi_type_table) AS `year_in_year`,
`char` <=> `char` in (select `char` from test_multi_type_table) AS `char_in_char`,
`varchar` <=> `varchar` in (select `varchar` from test_multi_type_table) AS `varchar_in_varchar`,
`binary` <=> `binary` in (select `binary` from test_multi_type_table) AS `binary_in_binary`,
`varbinary` <=> `varbinary` in (select `varbinary` from test_multi_type_table) AS `varbinary_in_varbinary`,
`tinyblob` <=> `tinyblob` in (select `tinyblob` from test_multi_type_table) AS `tinyblob_in_tinyblob`,
`blob` <=> `blob` in (select `blob` from test_multi_type_table) AS `blob_in_blob`,
`mediumblob` <=> `mediumblob` in (select `mediumblob` from test_multi_type_table) AS `mediumblob_in_mediumblob`,
`longblob` <=> `longblob` in (select `longblob` from test_multi_type_table) AS `longblob_in_longblob`,
`text` <=> `text` in (select `text` from test_multi_type_table) AS `text_in_text`,
`enum_t` <=> `enum_t` in (select `enum_t` from test_multi_type_table) AS `enum_t_in_enum_t`,
`set_t` <=> `set_t` in (select `set_t` from test_multi_type_table) AS `set_t_in_set_t`,
`json` <=> `json` in (select `json` from test_multi_type_table) AS `json_in_json`,
`int1` <=> `int1` not in (select `int1` from test_multi_type_table) AS `int1_not_in_int1`,
`uint1` <=> `uint1` not in (select `uint1` from test_multi_type_table) AS `uint1_not_in_uint1`,
`int2` <=> `int2` not in (select `int2` from test_multi_type_table) AS `int2_not_in_int2`,
`uint2` <=> `uint2` not in (select `uint2` from test_multi_type_table) AS `uint2_not_in_uint2`,
`int4` <=> `int4` not in (select `int4` from test_multi_type_table) AS `int4_not_in_int4`,
`uint4` <=> `uint4` not in (select `uint4` from test_multi_type_table) AS `uint4_not_in_uint4`,
`int8` <=> `int8` not in (select `int8` from test_multi_type_table) AS `int8_not_in_int8`,
`uint8` <=> `uint8` not in (select `uint8` from test_multi_type_table) AS `uint8_not_in_uint8`,
`float4` <=> `float4` not in (select `float4` from test_multi_type_table) AS `float4_not_in_float4`,
`float8` <=> `float8` not in (select `float8` from test_multi_type_table) AS `float8_not_in_float8`,
`numeric` <=> `numeric` not in (select `numeric` from test_multi_type_table) AS `numeric_not_in_numeric`,
`bit1` <=> `bit1` not in (select `bit1` from test_multi_type_table) AS `bit1_not_in_bit1`,
`bit64` <=> `bit64` not in (select `bit64` from test_multi_type_table) AS `bit64_not_in_bit64`,
`boolean` <=> `boolean` not in (select `boolean` from test_multi_type_table) AS `boolean_not_in_boolean`,
`date` <=> `date` not in (select `date` from test_multi_type_table) AS `date_not_in_date`,
`time` <=> `time` not in (select `time` from test_multi_type_table) AS `time_not_in_time`,
`time(4)` <=> `time(4)` not in (select `time(4)` from test_multi_type_table) AS `time(4)_not_in_time(4)`,
`datetime` <=> `datetime` not in (select `datetime` from test_multi_type_table) AS `datetime_not_in_datetime`,
`datetime(4)` <=> `datetime(4)` not in (select `datetime(4)` from test_multi_type_table) AS `datetime(4)_not_in_datetime(4)`,
`timestamp` <=> `timestamp` not in (select `timestamp` from test_multi_type_table) AS `timestamp_not_in_timestamp`,
`timestamp(4)` <=> `timestamp(4)` not in (select `timestamp(4)` from test_multi_type_table) AS `timestamp(4)_not_in_timestamp(4)`,
`year` <=> `year` not in (select `year` from test_multi_type_table) AS `year_not_in_year`,
`char` <=> `char` not in (select `char` from test_multi_type_table) AS `char_not_in_char`,
`varchar` <=> `varchar` not in (select `varchar` from test_multi_type_table) AS `varchar_not_in_varchar`,
`binary` <=> `binary` not in (select `binary` from test_multi_type_table) AS `binary_not_in_binary`,
`varbinary` <=> `varbinary` not in (select `varbinary` from test_multi_type_table) AS `varbinary_not_in_varbinary`,
`tinyblob` <=> `tinyblob` not in (select `tinyblob` from test_multi_type_table) AS `tinyblob_not_in_tinyblob`,
`blob` <=> `blob` not in (select `blob` from test_multi_type_table) AS `blob_not_in_blob`,
`mediumblob` <=> `mediumblob` not in (select `mediumblob` from test_multi_type_table) AS `mediumblob_not_in_mediumblob`,
`longblob` <=> `longblob` not in (select `longblob` from test_multi_type_table) AS `longblob_not_in_longblob`,
`text` <=> `text` not in (select `text` from test_multi_type_table) AS `text_not_in_text`,
`enum_t` <=> `enum_t` not in (select `enum_t` from test_multi_type_table) AS `enum_t_not_in_enum_t`,
`set_t` <=> `set_t` not in (select `set_t` from test_multi_type_table) AS `set_t_not_in_set_t`,
`json` <=> `json` not in (select `json` from test_multi_type_table) AS `json_not_in_json`
from test_multi_type_table;
\x
SHOW COLUMNS from test_multi_type;
DELETE from test_multi_type;
INSERT INTO test_multi_type VALUES(null);

UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int1_in_int1` = test_multi_type_table.`int1` <=> `int1` in (select `int1` from test_multi_type_table);
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint1_in_uint1` = test_multi_type_table.`uint1` <=> `uint1` in (select `uint1` from test_multi_type_table);
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int2_in_int2` = test_multi_type_table.`int2` <=> `int2` in (select `int2` from test_multi_type_table);
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint2_in_uint2` = test_multi_type_table.`uint2` <=> `uint2` in (select `uint2` from test_multi_type_table);
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int4_in_int4` = test_multi_type_table.`int4` <=> `int4` in (select `int4` from test_multi_type_table);
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint4_in_uint4` = test_multi_type_table.`uint4` <=> `uint4` in (select `uint4` from test_multi_type_table);
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int8_in_int8` = test_multi_type_table.`int8` <=> `int8` in (select `int8` from test_multi_type_table);
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint8_in_uint8` = test_multi_type_table.`uint8` <=> `uint8` in (select `uint8` from test_multi_type_table);
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`float4_in_float4` = test_multi_type_table.`float4` <=> `float4` in (select `float4` from test_multi_type_table);
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`float8_in_float8` = test_multi_type_table.`float8` <=> `float8` in (select `float8` from test_multi_type_table);
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`numeric_in_numeric` = test_multi_type_table.`numeric` <=> `numeric` in (select `numeric` from test_multi_type_table);
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`bit1_in_bit1` = test_multi_type_table.`bit1` <=> `bit1` in (select `bit1` from test_multi_type_table);
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`bit64_in_bit64` = test_multi_type_table.`bit64` <=> `bit64` in (select `bit64` from test_multi_type_table);
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`boolean_in_boolean` = test_multi_type_table.`boolean` <=> `boolean` in (select `boolean` from test_multi_type_table);
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`date_in_date` = test_multi_type_table.`date` <=> `date` in (select `date` from test_multi_type_table);
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`time_in_time` = test_multi_type_table.`time` <=> `time` in (select `time` from test_multi_type_table);
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`time(4)_in_time(4)` = test_multi_type_table.`time(4)` <=> `time(4)` in (select `time(4)` from test_multi_type_table);
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`datetime_in_datetime` = test_multi_type_table.`datetime` <=> `datetime` in (select `datetime` from test_multi_type_table);
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`datetime(4)_in_datetime(4)` = test_multi_type_table.`datetime(4)` <=> `datetime(4)` in (select `datetime(4)` from test_multi_type_table);
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`timestamp_in_timestamp` = test_multi_type_table.`timestamp` <=> `timestamp` in (select `timestamp` from test_multi_type_table);
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`timestamp(4)_in_timestamp(4)` = test_multi_type_table.`timestamp(4)` <=> `timestamp(4)` in (select `timestamp(4)` from test_multi_type_table);
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`year_in_year` = test_multi_type_table.`year` <=> `year` in (select `year` from test_multi_type_table);
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`char_in_char` = test_multi_type_table.`char` <=> `char` in (select `char` from test_multi_type_table);
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`varchar_in_varchar` = test_multi_type_table.`varchar` <=> `varchar` in (select `varchar` from test_multi_type_table);
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`binary_in_binary` = test_multi_type_table.`binary` <=> `binary` in (select `binary` from test_multi_type_table);
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`varbinary_in_varbinary` = test_multi_type_table.`varbinary` <=> `varbinary` in (select `varbinary` from test_multi_type_table);
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`tinyblob_in_tinyblob` = test_multi_type_table.`tinyblob` <=> `tinyblob` in (select `tinyblob` from test_multi_type_table);
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`blob_in_blob` = test_multi_type_table.`blob` <=> `blob` in (select `blob` from test_multi_type_table);
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`mediumblob_in_mediumblob` = test_multi_type_table.`mediumblob` <=> `mediumblob` in (select `mediumblob` from test_multi_type_table);
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`longblob_in_longblob` = test_multi_type_table.`longblob` <=> `longblob` in (select `longblob` from test_multi_type_table);
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`text_in_text` = test_multi_type_table.`text` <=> `text` in (select `text` from test_multi_type_table);
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`enum_t_in_enum_t` = test_multi_type_table.`enum_t` <=> `enum_t` in (select `enum_t` from test_multi_type_table);
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`set_t_in_set_t` = test_multi_type_table.`set_t` <=> `set_t` in (select `set_t` from test_multi_type_table);
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`json_in_json` = test_multi_type_table.`json` <=> `json` in (select `json` from test_multi_type_table);
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int1_not_in_int1` = test_multi_type_table.`int1` <=> `int1` not in (select `int1` from test_multi_type_table);
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint1_not_in_uint1` = test_multi_type_table.`uint1` <=> `uint1` not in (select `uint1` from test_multi_type_table);
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int2_not_in_int2` = test_multi_type_table.`int2` <=> `int2` not in (select `int2` from test_multi_type_table);
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint2_not_in_uint2` = test_multi_type_table.`uint2` <=> `uint2` not in (select `uint2` from test_multi_type_table);
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int4_not_in_int4` = test_multi_type_table.`int4` <=> `int4` not in (select `int4` from test_multi_type_table);
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint4_not_in_uint4` = test_multi_type_table.`uint4` <=> `uint4` not in (select `uint4` from test_multi_type_table);
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int8_not_in_int8` = test_multi_type_table.`int8` <=> `int8` not in (select `int8` from test_multi_type_table);
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint8_not_in_uint8` = test_multi_type_table.`uint8` <=> `uint8` not in (select `uint8` from test_multi_type_table);
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`float4_not_in_float4` = test_multi_type_table.`float4` <=> `float4` not in (select `float4` from test_multi_type_table);
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`float8_not_in_float8` = test_multi_type_table.`float8` <=> `float8` not in (select `float8` from test_multi_type_table);
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`numeric_not_in_numeric` = test_multi_type_table.`numeric` <=> `numeric` not in (select `numeric` from test_multi_type_table);
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`bit1_not_in_bit1` = test_multi_type_table.`bit1` <=> `bit1` not in (select `bit1` from test_multi_type_table);
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`bit64_not_in_bit64` = test_multi_type_table.`bit64` <=> `bit64` not in (select `bit64` from test_multi_type_table);
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`boolean_not_in_boolean` = test_multi_type_table.`boolean` <=> `boolean` not in (select `boolean` from test_multi_type_table);
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`date_not_in_date` = test_multi_type_table.`date` <=> `date` not in (select `date` from test_multi_type_table);
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`time_not_in_time` = test_multi_type_table.`time` <=> `time` not in (select `time` from test_multi_type_table);
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`time(4)_not_in_time(4)` = test_multi_type_table.`time(4)` <=> `time(4)` not in (select `time(4)` from test_multi_type_table);
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`datetime_not_in_datetime` = test_multi_type_table.`datetime` <=> `datetime` not in (select `datetime` from test_multi_type_table);
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`datetime(4)_not_in_datetime(4)` = test_multi_type_table.`datetime(4)` <=> `datetime(4)` not in (select `datetime(4)` from test_multi_type_table);
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`timestamp_not_in_timestamp` = test_multi_type_table.`timestamp` <=> `timestamp` not in (select `timestamp` from test_multi_type_table);
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`timestamp(4)_not_in_timestamp(4)` = test_multi_type_table.`timestamp(4)` <=> `timestamp(4)` not in (select `timestamp(4)` from test_multi_type_table);
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`year_not_in_year` = test_multi_type_table.`year` <=> `year` not in (select `year` from test_multi_type_table);
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`char_not_in_char` = test_multi_type_table.`char` <=> `char` not in (select `char` from test_multi_type_table);
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`varchar_not_in_varchar` = test_multi_type_table.`varchar` <=> `varchar` not in (select `varchar` from test_multi_type_table);
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`binary_not_in_binary` = test_multi_type_table.`binary` <=> `binary` not in (select `binary` from test_multi_type_table);
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`varbinary_not_in_varbinary` = test_multi_type_table.`varbinary` <=> `varbinary` not in (select `varbinary` from test_multi_type_table);
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`tinyblob_not_in_tinyblob` = test_multi_type_table.`tinyblob` <=> `tinyblob` not in (select `tinyblob` from test_multi_type_table);
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`blob_not_in_blob` = test_multi_type_table.`blob` <=> `blob` not in (select `blob` from test_multi_type_table);
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`mediumblob_not_in_mediumblob` = test_multi_type_table.`mediumblob` <=> `mediumblob` not in (select `mediumblob` from test_multi_type_table);
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`longblob_not_in_longblob` = test_multi_type_table.`longblob` <=> `longblob` not in (select `longblob` from test_multi_type_table);
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`text_not_in_text` = test_multi_type_table.`text` <=> `text` not in (select `text` from test_multi_type_table);
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`enum_t_not_in_enum_t` = test_multi_type_table.`enum_t` <=> `enum_t` not in (select `enum_t` from test_multi_type_table);
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`set_t_not_in_set_t` = test_multi_type_table.`set_t` <=> `set_t` not in (select `set_t` from test_multi_type_table);
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`json_not_in_json` = test_multi_type_table.`json` <=> `json` not in (select `json` from test_multi_type_table);
SELECT * from test_multi_type order by 1;
DROP TABLE test_multi_type;

-- strict select
set dolphin.sql_mode to 'sql_mode_strict,sql_mode_full_group,pipes_as_concat,ansi_quotes,no_zero_date';
SELECT
    `char` <=> `char` in (select `char` from test_multi_type_table) AS `char_in_char`,
    `char` <=> `char` not in (select `char` from test_multi_type_table) AS `char_not_in_char`,
    `varchar` <=> `varchar` in (select `varchar` from test_multi_type_table) AS `varchar_in_varchar`,
    `varchar` <=> `varchar` not in (select `varchar` from test_multi_type_table) AS `varchar_not_in_varchar`,
    `binary` <=> `binary` in (select `binary` from test_multi_type_table) AS `binary_in_binary`,
    `binary` <=> `binary` not in (select `binary` from test_multi_type_table) AS `binary_not_in_binary`,
    `varbinary` <=> `varbinary` in (select `varbinary` from test_multi_type_table) AS `varbinary_in_varbinary`,
    `varbinary` <=> `varbinary` not in (select `varbinary` from test_multi_type_table) AS `varbinary_not_in_varbinary`,
    `text` <=> `text` in (select `text` from test_multi_type_table) AS `text_in_text`,
    `text` <=> `text` not in (select `text` from test_multi_type_table) AS `text_not_in_text`
from test_multi_type_table;
\x

-- strict insert
CREATE TABLE TEST(a boolean);
INSERT INTO TEST SELECT `datetime` <=> `datetime` in (select `datetime` from test_multi_type_table) from test_multi_type_table;
INSERT INTO TEST SELECT `datetime(4)` <=> `datetime(4)` in (select `datetime(4)` from test_multi_type_table) from test_multi_type_table;
INSERT INTO TEST SELECT `timestamp` <=> `timestamp` in (select `timestamp` from test_multi_type_table) from test_multi_type_table;
INSERT INTO TEST SELECT `timestamp(4)` <=> `timestamp(4)` in (select `timestamp(4)` from test_multi_type_table) from test_multi_type_table;
INSERT INTO TEST SELECT `char` <=> `char` in (select `char` from test_multi_type_table) from test_multi_type_table;
INSERT INTO TEST SELECT `char` <=> `char` not in (select `char` from test_multi_type_table) from test_multi_type_table;
INSERT INTO TEST SELECT `varchar` <=> `varchar` in (select `varchar` from test_multi_type_table) from test_multi_type_table;
INSERT INTO TEST SELECT `varchar` <=> `varchar` not in (select `varchar` from test_multi_type_table) from test_multi_type_table;
INSERT INTO TEST SELECT `binary` <=> `binary` in (select `binary` from test_multi_type_table) from test_multi_type_table;
INSERT INTO TEST SELECT `binary` <=> `binary` not in (select `binary` from test_multi_type_table) from test_multi_type_table;
INSERT INTO TEST SELECT `varbinary` <=> `varbinary` in (select `varbinary` from test_multi_type_table) from test_multi_type_table;
INSERT INTO TEST SELECT `varbinary` <=> `varbinary` not in (select `varbinary` from test_multi_type_table) from test_multi_type_table;
INSERT INTO TEST SELECT `text` <=> `text` in (select `text` from test_multi_type_table) from test_multi_type_table;
INSERT INTO TEST SELECT `text` <=> `text` not in (select `text` from test_multi_type_table) from test_multi_type_table;

-- strict insert ignore
INSERT IGNORE INTO TEST SELECT `datetime` <=> `datetime` in (select `datetime` from test_multi_type_table) from test_multi_type_table;
INSERT IGNORE INTO TEST SELECT `datetime(4)` <=> `datetime(4)` in (select `datetime(4)` from test_multi_type_table) from test_multi_type_table;
INSERT IGNORE INTO TEST SELECT `timestamp` <=> `timestamp` in (select `timestamp` from test_multi_type_table) from test_multi_type_table;
INSERT IGNORE INTO TEST SELECT `timestamp(4)` <=> `timestamp(4)` in (select `timestamp(4)` from test_multi_type_table) from test_multi_type_table;
INSERT IGNORE INTO TEST SELECT `char` <=> `char` in (select `char` from test_multi_type_table) from test_multi_type_table;
INSERT IGNORE INTO TEST SELECT `char` <=> `char` not in (select `char` from test_multi_type_table) from test_multi_type_table;
INSERT IGNORE INTO TEST SELECT `varchar` <=> `varchar` in (select `varchar` from test_multi_type_table) from test_multi_type_table;
INSERT IGNORE INTO TEST SELECT `varchar` <=> `varchar` not in (select `varchar` from test_multi_type_table) from test_multi_type_table;
INSERT IGNORE INTO TEST SELECT `binary` <=> `binary` in (select `binary` from test_multi_type_table) from test_multi_type_table;
INSERT IGNORE INTO TEST SELECT `binary` <=> `binary` not in (select `binary` from test_multi_type_table) from test_multi_type_table;
INSERT IGNORE INTO TEST SELECT `varbinary` <=> `varbinary` in (select `varbinary` from test_multi_type_table) from test_multi_type_table;
INSERT IGNORE INTO TEST SELECT `varbinary` <=> `varbinary` not in (select `varbinary` from test_multi_type_table) from test_multi_type_table;
INSERT IGNORE INTO TEST SELECT `text` <=> `text` in (select `text` from test_multi_type_table) from test_multi_type_table;
INSERT IGNORE INTO TEST SELECT `text` <=> `text` not in (select `text` from test_multi_type_table) from test_multi_type_table;

-- precedence test
select 127 <=> 0 in (select 0);
select 127 <=> (0 in (select 0));
select (127 <=> 0) in (select 0);
select 0 <=> 127 not in (select 12);
select 0 <=> (127 not in (select 12));
select (0 <=> 127) not in (select 12);

DROP TABLE test_multi_type_table;
DROP TABLE TEST;

---------- tail ----------
drop schema nullsafe_eq_in_schema cascade;
reset current_schema;
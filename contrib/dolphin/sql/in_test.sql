drop schema if exists in_test cascade;
create schema in_test;
set current_schema to 'in_test';
---------- head ----------
set dolphin.b_compatibility_mode to on;
set dolphin.sql_mode to 'sql_mode_full_group,pipes_as_concat,ansi_quotes,no_zero_date';
drop table if exists test_multi_type_table;
CREATE TABLE test_multi_type_table(`int1` tinyint, `uint1` tinyint unsigned, `int2` smallint, `uint2` smallint unsigned, `int4` integer, `uint4` integer unsigned, `int8` bigint, `uint8` bigint unsigned, `float4` float4, `float8` float8, `numeric` decimal(20, 6), `bit1` bit(1), `bit64` bit(64), `boolean` boolean, `date` date, `time` time, `time(4)` time(4), `datetime` datetime, `datetime(4)` datetime(4) default '2022-11-11 11:11:11', `timestamp` timestamp, `timestamp(4)` timestamp(4) default '2022-11-11 11:11:11', `year` year, `char` char(100), `varchar` varchar(100), `binary` binary(100), `varbinary` varbinary(100), `tinyblob` tinyblob, `blob` blob, `mediumblob` mediumblob, `longblob` longblob, `text` text, `enum_t` enum('a', 'b', 'c'), `set_t` set('a', 'b', 'c'), `json` json);

INSERT INTO test_multi_type_table VALUES(1,1,1,1,1,1,1,1,1,1,1,b'1',b'1',true,'2023-02-05', '19:10:50', '19:10:50.3456', '2023-02-05 19:10:50', '2023-02-05 19:10:50.456', '2023-02-05 19:10:50', '2023-02-05 19:10:50.456', '2023', '1.23a', '1.23a', '1.23a', '1.23a', '1.23a', '1.23a', '1.23a', '1.23a', '1.23a', 'a', 'b,c', json_object('a',1,'b',1));

CREATE TABLE test_multi_type SELECT
`int1` in (select `int1` from test_multi_type_table) AS `int1_in_subquery`,
`uint1` in (select `uint1` from test_multi_type_table) AS `uint1_in_subquery`,
`int2` in (select `int2` from test_multi_type_table) AS `int2_in_subquery`,
`uint2` in (select `uint2` from test_multi_type_table) AS `uint2_in_subquery`,
`int4` in (select `int4` from test_multi_type_table) AS `int4_in_subquery`,
`uint4` in (select `uint4` from test_multi_type_table) AS `uint4_in_subquery`,
`int8` in (select `int8` from test_multi_type_table) AS `int8_in_subquery`,
`uint8` in (select `uint8` from test_multi_type_table) AS `uint8_in_subquery`,
`float4` in (select `float4` from test_multi_type_table) AS `float4_in_subquery`,
`float8` in (select `float8` from test_multi_type_table) AS `float8_in_subquery`,
`numeric` in (select `numeric` from test_multi_type_table) AS `numeric_in_subquery`,
`bit1` in (select `bit1` from test_multi_type_table) AS `bit1_in_subquery`,
`bit64` in (select `bit64` from test_multi_type_table) AS `bit64_in_subquery`,
`boolean` in (select `boolean` from test_multi_type_table) AS `boolean_in_subquery`,
`date` in (select `date` from test_multi_type_table) AS `date_in_subquery`,
`time` in (select `time` from test_multi_type_table) AS `time_in_subquery`,
`time(4)` in (select `time(4)` from test_multi_type_table) AS `time(4)_in_subquery`,
`datetime` in (select `datetime` from test_multi_type_table) AS `datetime_in_subquery`,
`datetime(4)` in (select `datetime(4)` from test_multi_type_table) AS `datetime(4)_in_subquery`,
`timestamp` in (select `timestamp` from test_multi_type_table) AS `timestamp_in_subquery`,
`timestamp(4)` in (select `timestamp(4)` from test_multi_type_table) AS `timestamp(4)_in_subquery`,
`year` in (select `year` from test_multi_type_table) AS `year_in_subquery`,
`char` in (select `char` from test_multi_type_table) AS `char_in_subquery`,
`varchar` in (select `varchar` from test_multi_type_table) AS `varchar_in_subquery`,
`binary` in (select `binary` from test_multi_type_table) AS `binary_in_subquery`,
`varbinary` in (select `varbinary` from test_multi_type_table) AS `varbinary_in_subquery`,
`tinyblob` in (select `tinyblob` from test_multi_type_table) AS `tinyblob_in_subquery`,
`blob` in (select `blob` from test_multi_type_table) AS `blob_in_subquery`,
`mediumblob` in (select `mediumblob` from test_multi_type_table) AS `mediumblob_in_subquery`,
`longblob` in (select `longblob` from test_multi_type_table) AS `longblob_in_subquery`,
`text` in (select `text` from test_multi_type_table) AS `text_in_subquery`,
`enum_t` in (select `enum_t` from test_multi_type_table) AS `enum_t_in_subquery`,
`set_t` in (select `set_t` from test_multi_type_table) AS `set_t_in_subquery`,
`json` in (select `json` from test_multi_type_table) AS `json_in_subquery`,
`int1` in (`int1`) AS `int1_in_int1`,
`uint1` in (`uint1`) AS `uint1_in_uint1`,
`int2` in (`int2`) AS `int2_in_int2`,
`uint2` in (`uint2`) AS `uint2_in_uint2`,
`int4` in (`int4`) AS `int4_in_int4`,
`uint4` in (`uint4`) AS `uint4_in_uint4`,
`int8` in (`int8`) AS `int8_in_int8`,
`uint8` in (`uint8`) AS `uint8_in_uint8`,
`float4` in (`float4`) AS `float4_in_float4`,
`float8` in (`float8`) AS `float8_in_float8`,
`numeric` in (`numeric`) AS `numeric_in_numeric`,
`bit1` in (`bit1`) AS `bit1_in_bit1`,
`bit64` in (`bit64`) AS `bit64_in_bit64`,
`boolean` in (`boolean`) AS `boolean_in_boolean`,
`date` in (`date`) AS `date_in_date`,
`time` in (`time`) AS `time_in_time`,
`time(4)` in (`time(4)`) AS `time(4)_in_time(4)`,
`datetime` in (`datetime`) AS `datetime_in_datetime`,
`datetime(4)` in (`datetime(4)`) AS `datetime(4)_in_datetime(4)`,
`timestamp` in (`timestamp`) AS `timestamp_in_timestamp`,
`timestamp(4)` in (`timestamp(4)`) AS `timestamp(4)_in_timestamp(4)`,
`year` in (`year`) AS `year_in_year`,
`char` in (`char`) AS `char_in_char`,
`varchar` in (`varchar`) AS `varchar_in_varchar`,
`binary` in (`binary`) AS `binary_in_binary`,
`varbinary` in (`varbinary`) AS `varbinary_in_varbinary`,
`tinyblob` in (`tinyblob`) AS `tinyblob_in_tinyblob`,
`blob` in (`blob`) AS `blob_in_blob`,
`mediumblob` in (`mediumblob`) AS `mediumblob_in_mediumblob`,
`longblob` in (`longblob`) AS `longblob_in_longblob`,
`text` in (`text`) AS `text_in_text`,
`enum_t` in (`enum_t`) AS `enum_t_in_enum_t`,
`set_t` in (`set_t`) AS `set_t_in_set_t`,
`json` in (`json`) AS `json_in_json`,
`int1` not between `int1` and `int1` AS `int1_not_between_int1`,
`uint1` not between `uint1` and `uint1` AS `uint1_not_between_uint1`,
`int2` not between `int2` and `int2` AS `int2_not_between_int2`,
`uint2` not between `uint2` and `uint2` AS `uint2_not_between_uint2`,
`int4` not between `int4` and `int4` AS `int4_not_between_int4`,
`uint4` not between `uint4` and `uint4` AS `uint4_not_between_uint4`,
`int8` not between `int8` and `int8` AS `int8_not_between_int8`,
`uint8` not between `uint8` and `uint8` AS `uint8_not_between_uint8`,
`float4` not between `float4` and `float4` AS `float4_not_between_float4`,
`float8` not between `float8` and `float8` AS `float8_not_between_float8`,
`numeric` not between `numeric` and `numeric` AS `numeric_not_between_numeric`,
`bit1` not between `bit1` and `bit1` AS `bit1_not_between_bit1`,
`bit64` not between `bit64` and `bit64` AS `bit64_not_between_bit64`,
`boolean` not between `boolean` and `boolean` AS `boolean_not_between_boolean`,
`date` not between `date` and `date` AS `date_not_between_date`,
`time` not between `time` and `time` AS `time_not_between_time`,
`time(4)` not between `time(4)` and `time(4)` AS `time(4)_not_between_time(4)`,
`datetime` not between `datetime` and `datetime` AS `datetime_not_between_datetime`,
`datetime(4)` not between `datetime(4)` and `datetime(4)` AS `datetime(4)_not_between_datetime(4)`,
`timestamp` not between `timestamp` and `timestamp` AS `timestamp_not_between_timestamp`,
`timestamp(4)` not between `timestamp(4)` and `timestamp(4)` AS `timestamp(4)_not_between_timestamp(4)`,
`year` not between `year` and `year` AS `year_not_between_year`,
`char` not between `char` and `char` AS `char_not_between_char`,
`varchar` not between `varchar` and `varchar` AS `varchar_not_between_varchar`,
`binary` not between `binary` and `binary` AS `binary_not_between_binary`,
`varbinary` not between `varbinary` and `varbinary` AS `varbinary_not_between_varbinary`,
`tinyblob` not between `tinyblob` and `tinyblob` AS `tinyblob_not_between_tinyblob`,
`blob` not between `blob` and `blob` AS `blob_not_between_blob`,
`mediumblob` not between `mediumblob` and `mediumblob` AS `mediumblob_not_between_mediumblob`,
`longblob` not between `longblob` and `longblob` AS `longblob_not_between_longblob`,
`text` not between `text` and `text` AS `text_not_between_text`,
`enum_t` not between `enum_t` and `enum_t` AS `enum_t_not_between_enum_t`,
`set_t` not between `set_t` and `set_t` AS `set_t_not_between_set_t`,
`json` not between `json` and `json` AS `json_not_between_json`,
`int1` sounds like `int1` AS `int1_sounds_like_int1`,
`uint1` sounds like `uint1` AS `uint1_sounds_like_uint1`,
`int2` sounds like `int2` AS `int2_sounds_like_int2`,
`uint2` sounds like `uint2` AS `uint2_sounds_like_uint2`,
`int4` sounds like `int4` AS `int4_sounds_like_int4`,
`uint4` sounds like `uint4` AS `uint4_sounds_like_uint4`,
`int8` sounds like `int8` AS `int8_sounds_like_int8`,
`uint8` sounds like `uint8` AS `uint8_sounds_like_uint8`,
`float4` sounds like `float4` AS `float4_sounds_like_float4`,
`float8` sounds like `float8` AS `float8_sounds_like_float8`,
`numeric` sounds like `numeric` AS `numeric_sounds_like_numeric`,
-- `bit1` sounds like `bit1` AS `bit1_sounds_like_bit1`,
-- `bit64` sounds like `bit64` AS `bit64_sounds_like_bit64`,
-- `boolean` sounds like `boolean` AS `boolean_sounds_like_boolean`,
`date` sounds like `date` AS `date_sounds_like_date`,
`time` sounds like `time` AS `time_sounds_like_time`,
`time(4)` sounds like `time(4)` AS `time(4)_sounds_like_time(4)`,
`datetime` sounds like `datetime` AS `datetime_sounds_like_datetime`,
`datetime(4)` sounds like `datetime(4)` AS `datetime(4)_sounds_like_datetime(4)`,
`timestamp` sounds like `timestamp` AS `timestamp_sounds_like_timestamp`,
`timestamp(4)` sounds like `timestamp(4)` AS `timestamp(4)_sounds_like_timestamp(4)`,
-- `year` sounds like `year` AS `year_sounds_like_year`,
`char` sounds like `char` AS `char_sounds_like_char`,
`varchar` sounds like `varchar` AS `varchar_sounds_like_varchar`,
`binary` sounds like `binary` AS `binary_sounds_like_binary`,
`varbinary` sounds like `varbinary` AS `varbinary_sounds_like_varbinary`,
-- `tinyblob` sounds like `tinyblob` AS `tinyblob_sounds_like_tinyblob`,
-- `blob` sounds like `blob` AS `blob_sounds_like_blob`,
-- `mediumblob` sounds like `mediumblob` AS `mediumblob_sounds_like_mediumblob`,
-- `longblob` sounds like `longblob` AS `longblob_sounds_like_longblob`,
`text` sounds like `text` AS `text_sounds_like_text`,
-- `enum_t` sounds like `enum_t` AS `enum_t_sounds_like_enum_t`,
`set_t` sounds like `set_t` AS `set_t_sounds_like_set_t`
-- `json` sounds like `json` AS `json_sounds_like_json`
from test_multi_type_table;

select `bit1` sounds like `bit1` AS `bit1_sounds_like_bit1` from test_multi_type_table;
select `bit64` sounds like `bit64` AS `bit64_sounds_like_bit64` from test_multi_type_table;
select `boolean` sounds like `boolean` AS `boolean_sounds_like_boolean` from test_multi_type_table;
select `year` sounds like `year` AS `year_sounds_like_year` from test_multi_type_table;
select `tinyblob` sounds like `tinyblob` AS `tinyblob_sounds_like_tinyblob` from test_multi_type_table;
select `blob` sounds like `blob` AS `blob_sounds_like_blob` from test_multi_type_table;
select `mediumblob` sounds like `mediumblob` AS `mediumblob_sounds_like_mediumblob` from test_multi_type_table;
select `longblob` sounds like `longblob` AS `longblob_sounds_like_longblob` from test_multi_type_table;
select `enum_t` sounds like `enum_t` AS `enum_t_sounds_like_enum_t` from test_multi_type_table;
select `json` sounds like `json` AS `json_sounds_like_json` from test_multi_type_table;

\x
select * from test_multi_type;

create table test_string_type select
`char` like `char` AS `char_like_char`,
`varchar` like `varchar` AS `varchar_like_varchar`,
`binary` like `binary` AS `binary_like_binary`,
`varbinary` like `varbinary` AS `varbinary_like_varbinary`,
`text` like `text` AS `text_like_text`,
`char` like `char` escape '2' AS `char_like_escape_char`,
`varchar` like `varchar` escape '2' AS `varchar_like_escape_varchar`,
`binary` like `binary` escape '2' AS `binary_like_escape_binary`,
`varbinary` like `varbinary` escape '2' AS `varbinary_like_escape_varbinary`,
`text` like `text` escape '2' AS `text_like_escape_text`,
`char` not regexp `char` AS `char_not_regexp_char`,
`varchar` not regexp `varchar` AS `varchar_not_regexp_varchar`,
`binary` not regexp `binary` AS `binary_not_regexp_binary`,
`varbinary` not regexp `varbinary` AS `varbinary_not_regexp_varbinary`,
`text` not regexp `text` AS `text_not_regexp_text`
from test_multi_type_table;

select * from test_string_type;

---------- tail ----------
drop schema in_test cascade;
reset current_schema;
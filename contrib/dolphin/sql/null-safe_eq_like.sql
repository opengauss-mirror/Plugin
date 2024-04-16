drop schema if exists nullsafe_eq_like_schema cascade;
create schema nullsafe_eq_like_schema;
set current_schema to 'nullsafe_eq_like_schema';
---------- head ----------
set dolphin.b_compatibility_mode to on;
set dolphin.sql_mode to 'sql_mode_full_group,pipes_as_concat,ansi_quotes,no_zero_date';
drop table if exists test_multi_type_table;
CREATE TABLE test_multi_type_table(`int1` tinyint, `uint1` tinyint unsigned, `int2` smallint, `uint2` smallint unsigned, `int4` integer, `uint4` integer unsigned, `int8` bigint, `uint8` bigint unsigned, `float4` float4, `float8` float8, `numeric` decimal(20, 6), `bit1` bit(1), `bit64` bit(64), `boolean` boolean, `date` date, `time` time, `time(4)` time(4), `datetime` datetime, `datetime(4)` datetime(4) default '2022-11-11 11:11:11', `timestamp` timestamp, `timestamp(4)` timestamp(4) default '2022-11-11 11:11:11', `year` year, `char` char(100), `varchar` varchar(100), `binary` binary(100), `varbinary` varbinary(100), `tinyblob` tinyblob, `blob` blob, `mediumblob` mediumblob, `longblob` longblob, `text` text, `enum_t` enum('a', 'b', 'c'), `set_t` set('a', 'b', 'c'), `json` json);
INSERT INTO test_multi_type_table VALUES(0x7F, 0xFF, 0x7FFF, 0xFFFF, 0x7FFFFFFF, 0xFFFFFFFF, 0x7FFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 0.9884282, 5.288433915413254, 43391541, b'1', b'100110100110', true, '2023-02-05', '19:10:50', '19:10:50.3456', '2023-02-05 19:10:50', '2023-02-05 19:10:50.456', '2023-02-05 19:10:50', '2023-02-05 19:10:50.456', '2023', '1.23a', '1.23a', '1.23a', '1.23a', '1.23a', '1.23a', '1.23a', '1.23a', '1.23a', 'a', 'b,c', json_object('a',1,'b',1));
CREATE TABLE test_enum_table(day_of_week enum('0','1','2','3','4','5','6'));
INSERT INTO test_enum_table VALUES ('1');

CREATE TABLE test_multi_type SELECT
`int1` <=> day_of_week like 'foo' as `int1_like_enum`,
`uint1` <=> day_of_week like 'foo' as `uint1_like_enum`,
`int2` <=> day_of_week like 'foo' as `int2_like_enum`,
`uint2` <=> day_of_week like 'foo' as `uint2_like_enum`,
`int4` <=> day_of_week like 'foo' as `int4_like_enum`,
`uint4` <=> day_of_week like 'foo' as `uint4_like_enum`,
`int8` <=> day_of_week like 'foo' as `int8_like_enum`,
`uint8` <=> day_of_week like 'foo' as `uint8_like_enum`,
`float4` <=> day_of_week like 'foo' as `float4_like_enum`,
`float8` <=> day_of_week like 'foo' as `float8_like_enum`,
`numeric` <=> day_of_week like 'foo' as `numeric_like_enum`,
`bit1` <=> day_of_week like 'foo' as `bit1_like_enum`,
`bit64` <=> day_of_week like 'foo' as `bit64_like_enum`,
`boolean` <=> day_of_week like 'foo' as `boolean_like_enum`,
`date` <=> day_of_week like 'foo' as `date_like_enum`,
`time` <=> day_of_week like 'foo' as `time_like_enum`,
`time(4)` <=> day_of_week like 'foo' as `time(4)_like_enum`,
`datetime` <=> day_of_week like 'foo' as `datetime_like_enum`,
`datetime(4)` <=> day_of_week like 'foo' as `datetime(4)_like_enum`,
`timestamp` <=> day_of_week like 'foo' as `timestamp_like_enum`,
`timestamp(4)` <=> day_of_week like 'foo' as `timestamp(4)_like_enum`,
`year` <=> day_of_week like 'foo' as `year_like_enum`,
`char` <=> day_of_week like 'foo' as `char_like_enum`,
`varchar` <=> day_of_week like 'foo' as `varchar_like_enum`,
`binary` <=> day_of_week like 'foo' as `binary_like_enum`,
`varbinary` <=> day_of_week like 'foo' as `varbinary_like_enum`,
`tinyblob` <=> day_of_week like 'foo' as `tinyblob_like_enum`,
`blob` <=> day_of_week like 'foo' as `blob_like_enum`,
`mediumblob` <=> day_of_week like 'foo' as `mediumblob_like_enum`,
`longblob` <=> day_of_week like 'foo' as `longblob_like_enum`,
`text` <=> day_of_week like 'foo' as `text_like_enum`,
`enum_t` <=> day_of_week like 'foo' as `enum_t_like_enum`,
`set_t` <=> day_of_week like 'foo' as `set_t_like_enum`,
`json` <=> day_of_week like 'foo' as `json_like_enum`,
`int1` <=> day_of_week not like 'foo' as `int1_not_like_enum`,
`uint1` <=> day_of_week not like 'foo' as `uint1_not_like_enum`,
`int2` <=> day_of_week not like 'foo' as `int2_not_like_enum`,
`uint2` <=> day_of_week not like 'foo' as `uint2_not_like_enum`,
`int4` <=> day_of_week not like 'foo' as `int4_not_like_enum`,
`uint4` <=> day_of_week not like 'foo' as `uint4_not_like_enum`,
`int8` <=> day_of_week not like 'foo' as `int8_not_like_enum`,
`uint8` <=> day_of_week not like 'foo' as `uint8_not_like_enum`,
`float4` <=> day_of_week not like 'foo' as `float4_not_like_enum`,
`float8` <=> day_of_week not like 'foo' as `float8_not_like_enum`,
`numeric` <=> day_of_week not like 'foo' as `numeric_not_like_enum`,
`bit1` <=> day_of_week not like 'foo' as `bit1_not_like_enum`,
`bit64` <=> day_of_week not like 'foo' as `bit64_not_like_enum`,
`boolean` <=> day_of_week not like 'foo' as `boolean_not_like_enum`,
`date` <=> day_of_week not like 'foo' as `date_not_like_enum`,
`time` <=> day_of_week not like 'foo' as `time_not_like_enum`,
`time(4)` <=> day_of_week not like 'foo' as `time(4)_not_like_enum`,
`datetime` <=> day_of_week not like 'foo' as `datetime_not_like_enum`,
`datetime(4)` <=> day_of_week not like 'foo' as `datetime(4)_not_like_enum`,
`timestamp` <=> day_of_week not like 'foo' as `timestamp_not_like_enum`,
`timestamp(4)` <=> day_of_week not like 'foo' as `timestamp(4)_not_like_enum`,
`year` <=> day_of_week not like 'foo' as `year_not_like_enum`,
`char` <=> day_of_week not like 'foo' as `char_not_like_enum`,
`varchar` <=> day_of_week not like 'foo' as `varchar_not_like_enum`,
`binary` <=> day_of_week not like 'foo' as `binary_not_like_enum`,
`varbinary` <=> day_of_week not like 'foo' as `varbinary_not_like_enum`,
`tinyblob` <=> day_of_week not like 'foo' as `tinyblob_not_like_enum`,
`blob` <=> day_of_week not like 'foo' as `blob_not_like_enum`,
`mediumblob` <=> day_of_week not like 'foo' as `mediumblob_not_like_enum`,
`longblob` <=> day_of_week not like 'foo' as `longblob_not_like_enum`,
`text` <=> day_of_week not like 'foo' as `text_not_like_enum`,
`enum_t` <=> day_of_week not like 'foo' as `enum_t_not_like_enum`,
`set_t` <=> day_of_week not like 'foo' as `set_t_not_like_enum`,
`json` <=> day_of_week not like 'foo' as `json_not_like_enum`
FROM test_multi_type_table, test_enum_table;
\x
SHOW COLUMNS from test_multi_type;
DELETE from test_multi_type;
INSERT INTO test_multi_type VALUES(null);

UPDATE test_multi_type, test_multi_type_table, test_enum_table SET test_multi_type.`int1_like_enum` = test_multi_type_table.`int1` <=> test_enum_table.day_of_week like 'foo';
UPDATE test_multi_type, test_multi_type_table, test_enum_table SET test_multi_type.`uint1_like_enum` = test_multi_type_table.`uint1` <=> test_enum_table.day_of_week like 'foo';
UPDATE test_multi_type, test_multi_type_table, test_enum_table SET test_multi_type.`int2_like_enum` = test_multi_type_table.`int2` <=> test_enum_table.day_of_week like 'foo';
UPDATE test_multi_type, test_multi_type_table, test_enum_table SET test_multi_type.`uint2_like_enum` = test_multi_type_table.`uint2` <=> test_enum_table.day_of_week like 'foo';
UPDATE test_multi_type, test_multi_type_table, test_enum_table SET test_multi_type.`int4_like_enum` = test_multi_type_table.`int4` <=> test_enum_table.day_of_week like 'foo';
UPDATE test_multi_type, test_multi_type_table, test_enum_table SET test_multi_type.`uint4_like_enum` = test_multi_type_table.`uint4` <=> test_enum_table.day_of_week like 'foo';
UPDATE test_multi_type, test_multi_type_table, test_enum_table SET test_multi_type.`int8_like_enum` = test_multi_type_table.`int8` <=> test_enum_table.day_of_week like 'foo';
UPDATE test_multi_type, test_multi_type_table, test_enum_table SET test_multi_type.`uint8_like_enum` = test_multi_type_table.`uint8` <=> test_enum_table.day_of_week like 'foo';
UPDATE test_multi_type, test_multi_type_table, test_enum_table SET test_multi_type.`float4_like_enum` = test_multi_type_table.`float4` <=> test_enum_table.day_of_week like 'foo';
UPDATE test_multi_type, test_multi_type_table, test_enum_table SET test_multi_type.`float8_like_enum` = test_multi_type_table.`float8` <=> test_enum_table.day_of_week like 'foo';
UPDATE test_multi_type, test_multi_type_table, test_enum_table SET test_multi_type.`numeric_like_enum` = test_multi_type_table.`numeric` <=> test_enum_table.day_of_week like 'foo';
UPDATE test_multi_type, test_multi_type_table, test_enum_table SET test_multi_type.`bit1_like_enum` = test_multi_type_table.`bit1` <=> test_enum_table.day_of_week like 'foo';
UPDATE test_multi_type, test_multi_type_table, test_enum_table SET test_multi_type.`bit64_like_enum` = test_multi_type_table.`bit64` <=> test_enum_table.day_of_week like 'foo';
UPDATE test_multi_type, test_multi_type_table, test_enum_table SET test_multi_type.`boolean_like_enum` = test_multi_type_table.`boolean` <=> test_enum_table.day_of_week like 'foo';
UPDATE test_multi_type, test_multi_type_table, test_enum_table SET test_multi_type.`date_like_enum` = test_multi_type_table.`date` <=> test_enum_table.day_of_week like 'foo';
UPDATE test_multi_type, test_multi_type_table, test_enum_table SET test_multi_type.`time_like_enum` = test_multi_type_table.`time` <=> test_enum_table.day_of_week like 'foo';
UPDATE test_multi_type, test_multi_type_table, test_enum_table SET test_multi_type.`time(4)_like_enum` = test_multi_type_table.`time(4)` <=> test_enum_table.day_of_week like 'foo';
UPDATE test_multi_type, test_multi_type_table, test_enum_table SET test_multi_type.`datetime_like_enum` = test_multi_type_table.`datetime` <=> test_enum_table.day_of_week like 'foo';
UPDATE test_multi_type, test_multi_type_table, test_enum_table SET test_multi_type.`datetime(4)_like_enum` = test_multi_type_table.`datetime(4)` <=> test_enum_table.day_of_week like 'foo';
UPDATE test_multi_type, test_multi_type_table, test_enum_table SET test_multi_type.`timestamp_like_enum` = test_multi_type_table.`timestamp` <=> test_enum_table.day_of_week like 'foo';
UPDATE test_multi_type, test_multi_type_table, test_enum_table SET test_multi_type.`timestamp(4)_like_enum` = test_multi_type_table.`timestamp(4)` <=> test_enum_table.day_of_week like 'foo';
UPDATE test_multi_type, test_multi_type_table, test_enum_table SET test_multi_type.`year_like_enum` = test_multi_type_table.`year` <=> test_enum_table.day_of_week like 'foo';
UPDATE test_multi_type, test_multi_type_table, test_enum_table SET test_multi_type.`char_like_enum` = test_multi_type_table.`char` <=> test_enum_table.day_of_week like 'foo';
UPDATE test_multi_type, test_multi_type_table, test_enum_table SET test_multi_type.`varchar_like_enum` = test_multi_type_table.`varchar` <=> test_enum_table.day_of_week like 'foo';
UPDATE test_multi_type, test_multi_type_table, test_enum_table SET test_multi_type.`binary_like_enum` = test_multi_type_table.`binary` <=> test_enum_table.day_of_week like 'foo';
UPDATE test_multi_type, test_multi_type_table, test_enum_table SET test_multi_type.`varbinary_like_enum` = test_multi_type_table.`varbinary` <=> test_enum_table.day_of_week like 'foo';
UPDATE test_multi_type, test_multi_type_table, test_enum_table SET test_multi_type.`tinyblob_like_enum` = test_multi_type_table.`tinyblob` <=> test_enum_table.day_of_week like 'foo';
UPDATE test_multi_type, test_multi_type_table, test_enum_table SET test_multi_type.`blob_like_enum` = test_multi_type_table.`blob` <=> test_enum_table.day_of_week like 'foo';
UPDATE test_multi_type, test_multi_type_table, test_enum_table SET test_multi_type.`mediumblob_like_enum` = test_multi_type_table.`mediumblob` <=> test_enum_table.day_of_week like 'foo';
UPDATE test_multi_type, test_multi_type_table, test_enum_table SET test_multi_type.`longblob_like_enum` = test_multi_type_table.`longblob` <=> test_enum_table.day_of_week like 'foo';
UPDATE test_multi_type, test_multi_type_table, test_enum_table SET test_multi_type.`text_like_enum` = test_multi_type_table.`text` <=> test_enum_table.day_of_week like 'foo';
UPDATE test_multi_type, test_multi_type_table, test_enum_table SET test_multi_type.`enum_t_like_enum` = test_multi_type_table.`enum_t` <=> test_enum_table.day_of_week like 'foo';
UPDATE test_multi_type, test_multi_type_table, test_enum_table SET test_multi_type.`set_t_like_enum` = test_multi_type_table.`set_t` <=> test_enum_table.day_of_week like 'foo';
UPDATE test_multi_type, test_multi_type_table, test_enum_table SET test_multi_type.`json_like_enum` = test_multi_type_table.`json` <=> test_enum_table.day_of_week like 'foo';
UPDATE test_multi_type, test_multi_type_table, test_enum_table SET test_multi_type.`int1_not_like_enum` = test_multi_type_table.`int1` <=> test_enum_table.day_of_week not like 'foo';
UPDATE test_multi_type, test_multi_type_table, test_enum_table SET test_multi_type.`uint1_not_like_enum` = test_multi_type_table.`uint1` <=> test_enum_table.day_of_week not like 'foo';
UPDATE test_multi_type, test_multi_type_table, test_enum_table SET test_multi_type.`int2_not_like_enum` = test_multi_type_table.`int2` <=> test_enum_table.day_of_week not like 'foo';
UPDATE test_multi_type, test_multi_type_table, test_enum_table SET test_multi_type.`uint2_not_like_enum` = test_multi_type_table.`uint2` <=> test_enum_table.day_of_week not like 'foo';
UPDATE test_multi_type, test_multi_type_table, test_enum_table SET test_multi_type.`int4_not_like_enum` = test_multi_type_table.`int4` <=> test_enum_table.day_of_week not like 'foo';
UPDATE test_multi_type, test_multi_type_table, test_enum_table SET test_multi_type.`uint4_not_like_enum` = test_multi_type_table.`uint4` <=> test_enum_table.day_of_week not like 'foo';
UPDATE test_multi_type, test_multi_type_table, test_enum_table SET test_multi_type.`int8_not_like_enum` = test_multi_type_table.`int8` <=> test_enum_table.day_of_week not like 'foo';
UPDATE test_multi_type, test_multi_type_table, test_enum_table SET test_multi_type.`uint8_not_like_enum` = test_multi_type_table.`uint8` <=> test_enum_table.day_of_week not like 'foo';
UPDATE test_multi_type, test_multi_type_table, test_enum_table SET test_multi_type.`float4_not_like_enum` = test_multi_type_table.`float4` <=> test_enum_table.day_of_week not like 'foo';
UPDATE test_multi_type, test_multi_type_table, test_enum_table SET test_multi_type.`float8_not_like_enum` = test_multi_type_table.`float8` <=> test_enum_table.day_of_week not like 'foo';
UPDATE test_multi_type, test_multi_type_table, test_enum_table SET test_multi_type.`numeric_not_like_enum` = test_multi_type_table.`numeric` <=> test_enum_table.day_of_week not like 'foo';
UPDATE test_multi_type, test_multi_type_table, test_enum_table SET test_multi_type.`bit1_not_like_enum` = test_multi_type_table.`bit1` <=> test_enum_table.day_of_week not like 'foo';
UPDATE test_multi_type, test_multi_type_table, test_enum_table SET test_multi_type.`bit64_not_like_enum` = test_multi_type_table.`bit64` <=> test_enum_table.day_of_week not like 'foo';
UPDATE test_multi_type, test_multi_type_table, test_enum_table SET test_multi_type.`boolean_not_like_enum` = test_multi_type_table.`boolean` <=> test_enum_table.day_of_week not like 'foo';
UPDATE test_multi_type, test_multi_type_table, test_enum_table SET test_multi_type.`date_not_like_enum` = test_multi_type_table.`date` <=> test_enum_table.day_of_week not like 'foo';
UPDATE test_multi_type, test_multi_type_table, test_enum_table SET test_multi_type.`time_not_like_enum` = test_multi_type_table.`time` <=> test_enum_table.day_of_week not like 'foo';
UPDATE test_multi_type, test_multi_type_table, test_enum_table SET test_multi_type.`time(4)_not_like_enum` = test_multi_type_table.`time(4)` <=> test_enum_table.day_of_week not like 'foo';
UPDATE test_multi_type, test_multi_type_table, test_enum_table SET test_multi_type.`datetime_not_like_enum` = test_multi_type_table.`datetime` <=> test_enum_table.day_of_week not like 'foo';
UPDATE test_multi_type, test_multi_type_table, test_enum_table SET test_multi_type.`datetime(4)_not_like_enum` = test_multi_type_table.`datetime(4)` <=> test_enum_table.day_of_week not like 'foo';
UPDATE test_multi_type, test_multi_type_table, test_enum_table SET test_multi_type.`timestamp_not_like_enum` = test_multi_type_table.`timestamp` <=> test_enum_table.day_of_week not like 'foo';
UPDATE test_multi_type, test_multi_type_table, test_enum_table SET test_multi_type.`timestamp(4)_not_like_enum` = test_multi_type_table.`timestamp(4)` <=> test_enum_table.day_of_week not like 'foo';
UPDATE test_multi_type, test_multi_type_table, test_enum_table SET test_multi_type.`year_not_like_enum` = test_multi_type_table.`year` <=> test_enum_table.day_of_week not like 'foo';
UPDATE test_multi_type, test_multi_type_table, test_enum_table SET test_multi_type.`char_not_like_enum` = test_multi_type_table.`char` <=> test_enum_table.day_of_week not like 'foo';
UPDATE test_multi_type, test_multi_type_table, test_enum_table SET test_multi_type.`varchar_not_like_enum` = test_multi_type_table.`varchar` <=> test_enum_table.day_of_week not like 'foo';
UPDATE test_multi_type, test_multi_type_table, test_enum_table SET test_multi_type.`binary_not_like_enum` = test_multi_type_table.`binary` <=> test_enum_table.day_of_week not like 'foo';
UPDATE test_multi_type, test_multi_type_table, test_enum_table SET test_multi_type.`varbinary_not_like_enum` = test_multi_type_table.`varbinary` <=> test_enum_table.day_of_week not like 'foo';
UPDATE test_multi_type, test_multi_type_table, test_enum_table SET test_multi_type.`tinyblob_not_like_enum` = test_multi_type_table.`tinyblob` <=> test_enum_table.day_of_week not like 'foo';
UPDATE test_multi_type, test_multi_type_table, test_enum_table SET test_multi_type.`blob_not_like_enum` = test_multi_type_table.`blob` <=> test_enum_table.day_of_week not like 'foo';
UPDATE test_multi_type, test_multi_type_table, test_enum_table SET test_multi_type.`mediumblob_not_like_enum` = test_multi_type_table.`mediumblob` <=> test_enum_table.day_of_week not like 'foo';
UPDATE test_multi_type, test_multi_type_table, test_enum_table SET test_multi_type.`longblob_not_like_enum` = test_multi_type_table.`longblob` <=> test_enum_table.day_of_week not like 'foo';
UPDATE test_multi_type, test_multi_type_table, test_enum_table SET test_multi_type.`text_not_like_enum` = test_multi_type_table.`text` <=> test_enum_table.day_of_week not like 'foo';
UPDATE test_multi_type, test_multi_type_table, test_enum_table SET test_multi_type.`enum_t_not_like_enum` = test_multi_type_table.`enum_t` <=> test_enum_table.day_of_week not like 'foo';
UPDATE test_multi_type, test_multi_type_table, test_enum_table SET test_multi_type.`set_t_not_like_enum` = test_multi_type_table.`set_t` <=> test_enum_table.day_of_week not like 'foo';
UPDATE test_multi_type, test_multi_type_table, test_enum_table SET test_multi_type.`json_not_like_enum` = test_multi_type_table.`json` <=> test_enum_table.day_of_week not like 'foo';
SELECT * from test_multi_type order by 1;
DROP TABLE test_multi_type;

-- strict select
set dolphin.sql_mode to 'sql_mode_strict,sql_mode_full_group,pipes_as_concat,ansi_quotes,no_zero_date';
SELECT
    `char` <=> day_of_week like 'foo' as `char_like_enum`,
    `varchar` <=> day_of_week like 'foo' as `varchar_like_enum`,
    `binary` <=> day_of_week like 'foo' as `binary_like_enum`,
    `varbinary` <=> day_of_week like 'foo' as `varbinary_like_enum`,
    `datetime` <=> day_of_week not like 'foo' as `datetime_not_like_enum`,
    `datetime(4)` <=> day_of_week not like 'foo' as `datetime(4)_not_like_enum`,
    `timestamp` <=> day_of_week not like 'foo' as `timestamp_not_like_enum`,
    `timestamp(4)` <=> day_of_week not like 'foo' as `timestamp(4)_not_like_enum`,
    `char` <=> day_of_week not like 'foo' as `char_not_like_enum`,
    `varchar` <=> day_of_week not like 'foo' as `varchar_not_like_enum`,
    `binary` <=> day_of_week not like 'foo' as `binary_not_like_enum`,
    `varbinary` <=> day_of_week not like 'foo' as `varbinary_not_like_enum`
from test_multi_type_table, test_enum_table;
\x

-- strict insert
CREATE TABLE TEST(a boolean);
INSERT INTO TEST SELECT `char` <=> day_of_week like 'foo' as `char_like_enum` FROM test_multi_type_table, test_enum_table;
INSERT INTO TEST SELECT `varchar` <=> day_of_week like 'foo' as `varchar_like_enum` FROM test_multi_type_table, test_enum_table;
INSERT INTO TEST SELECT `binary` <=> day_of_week like 'foo' as `binary_like_enum` FROM test_multi_type_table, test_enum_table;
INSERT INTO TEST SELECT `varbinary` <=> day_of_week like 'foo' as `varbinary_like_enum` FROM test_multi_type_table, test_enum_table;
INSERT INTO TEST SELECT `datetime` <=> day_of_week not like 'foo' as `datetime_not_like_enum` FROM test_multi_type_table, test_enum_table;
INSERT INTO TEST SELECT `datetime(4)` <=> day_of_week not like 'foo' as `datetime(4)_not_like_enum` FROM test_multi_type_table, test_enum_table;
INSERT INTO TEST SELECT `timestamp` <=> day_of_week not like 'foo' as `timestamp_not_like_enum` FROM test_multi_type_table, test_enum_table;
INSERT INTO TEST SELECT `timestamp(4)` <=> day_of_week not like 'foo' as `timestamp(4)_not_like_enum` FROM test_multi_type_table, test_enum_table;
INSERT INTO TEST SELECT `char` <=> day_of_week not like 'foo' as `char_not_like_enum` FROM test_multi_type_table, test_enum_table;
INSERT INTO TEST SELECT `varchar` <=> day_of_week not like 'foo' as `varchar_not_like_enum` FROM test_multi_type_table, test_enum_table;
INSERT INTO TEST SELECT `binary` <=> day_of_week not like 'foo' as `binary_not_like_enum` FROM test_multi_type_table, test_enum_table;
INSERT INTO TEST SELECT `varbinary` <=> day_of_week not like 'foo' as `varbinary_not_like_enum` FROM test_multi_type_table, test_enum_table;

-- strict insert ignore
INSERT IGNORE INTO TEST SELECT `char` <=> day_of_week like 'foo' as `char_like_enum` FROM test_multi_type_table, test_enum_table;
INSERT IGNORE INTO TEST SELECT `varchar` <=> day_of_week like 'foo' as `varchar_like_enum` FROM test_multi_type_table, test_enum_table;
INSERT IGNORE INTO TEST SELECT `binary` <=> day_of_week like 'foo' as `binary_like_enum` FROM test_multi_type_table, test_enum_table;
INSERT IGNORE INTO TEST SELECT `varbinary` <=> day_of_week like 'foo' as `varbinary_like_enum` FROM test_multi_type_table, test_enum_table;
INSERT IGNORE INTO TEST SELECT `datetime` <=> day_of_week not like 'foo' as `datetime_not_like_enum` FROM test_multi_type_table, test_enum_table;
INSERT IGNORE INTO TEST SELECT `datetime(4)` <=> day_of_week not like 'foo' as `datetime(4)_not_like_enum` FROM test_multi_type_table, test_enum_table;
INSERT IGNORE INTO TEST SELECT `timestamp` <=> day_of_week not like 'foo' as `timestamp_not_like_enum` FROM test_multi_type_table, test_enum_table;
INSERT IGNORE INTO TEST SELECT `timestamp(4)` <=> day_of_week not like 'foo' as `timestamp(4)_not_like_enum` FROM test_multi_type_table, test_enum_table;
INSERT IGNORE INTO TEST SELECT `char` <=> day_of_week not like 'foo' as `char_not_like_enum` FROM test_multi_type_table, test_enum_table;
INSERT IGNORE INTO TEST SELECT `varchar` <=> day_of_week not like 'foo' as `varchar_not_like_enum` FROM test_multi_type_table, test_enum_table;
INSERT IGNORE INTO TEST SELECT `binary` <=> day_of_week not like 'foo' as `binary_not_like_enum` FROM test_multi_type_table, test_enum_table;
INSERT IGNORE INTO TEST SELECT `varbinary` <=> day_of_week not like 'foo' as `varbinary_not_like_enum` FROM test_multi_type_table, test_enum_table;

DROP TABLE test_multi_type_table;
DROP TABLE test_enum_table;
DROP TABLE TEST;

---------- tail ----------
drop schema nullsafe_eq_like_schema cascade;
reset current_schema;
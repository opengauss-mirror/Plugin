drop schema if exists pipes_as_concat_test_schema cascade;
create schema pipes_as_concat_test_schema;
set current_schema to 'pipes_as_concat_test_schema';
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
    `char` char(100),
    `varchar` varchar(100),
    `binary` binary(100),
    `varbinary` varbinary(100),
    `text` text
);
INSERT INTO test_multi_type_table VALUES(0x7F, 0xFF, 0x7FFF, 0xFFFF, 0x7FFFFFFF, 0xFFFFFFFF, 0x7FFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 0.9884282, 5.288433915413254, 43391541, b'1', b'100110100110', true, '1.23a', '1.23a', '1.23a', '1.23a', '1.23a');

CREATE TABLE test_multi_type SELECT
`int1` || `int1` AS `int1_||_int1`,
`int1` || `uint1` AS `int1_||_uint1`,
`int1` || `int2` AS `int1_||_int2`,
`int1` || `uint2` AS `int1_||_uint2`,
`int1` || `int4` AS `int1_||_int4`,
`int1` || `uint4` AS `int1_||_uint4`,
`int1` || `int8` AS `int1_||_int8`,
`int1` || `uint8` AS `int1_||_uint8`,
`int1` || `float4` AS `int1_||_float4`,
`int1` || `float8` AS `int1_||_float8`,
`int1` || `numeric` AS `int1_||_numeric`,
`int1` || `bit1` AS `int1_||_bit1`,
`int1` || `bit64` AS `int1_||_bit64`,
`int1` || `boolean` AS `int1_||_boolean`,
`int1` || `char` AS `int1_||_char`,
`int1` || `varchar` AS `int1_||_varchar`,
`int1` || `binary` AS `int1_||_binary`,
`int1` || `varbinary` AS `int1_||_varbinary`,
`int1` || `text` AS `int1_||_text`,
`uint1` || `int1` AS `uint1_||_int1`,
`uint1` || `uint1` AS `uint1_||_uint1`,
`uint1` || `int2` AS `uint1_||_int2`,
`uint1` || `uint2` AS `uint1_||_uint2`,
`uint1` || `int4` AS `uint1_||_int4`,
`uint1` || `uint4` AS `uint1_||_uint4`,
`uint1` || `int8` AS `uint1_||_int8`,
`uint1` || `uint8` AS `uint1_||_uint8`,
`uint1` || `float4` AS `uint1_||_float4`,
`uint1` || `float8` AS `uint1_||_float8`,
`uint1` || `numeric` AS `uint1_||_numeric`,
`uint1` || `bit1` AS `uint1_||_bit1`,
`uint1` || `bit64` AS `uint1_||_bit64`,
`uint1` || `boolean` AS `uint1_||_boolean`,
`uint1` || `char` AS `uint1_||_char`,
`uint1` || `varchar` AS `uint1_||_varchar`,
`uint1` || `binary` AS `uint1_||_binary`,
`uint1` || `varbinary` AS `uint1_||_varbinary`,
`uint1` || `text` AS `uint1_||_text`,
`int2` || `int1` AS `int2_||_int1`,
`int2` || `uint1` AS `int2_||_uint1`,
`int2` || `int2` AS `int2_||_int2`,
`int2` || `uint2` AS `int2_||_uint2`,
`int2` || `int4` AS `int2_||_int4`,
`int2` || `uint4` AS `int2_||_uint4`,
`int2` || `int8` AS `int2_||_int8`,
`int2` || `uint8` AS `int2_||_uint8`,
`int2` || `float4` AS `int2_||_float4`,
`int2` || `float8` AS `int2_||_float8`,
`int2` || `numeric` AS `int2_||_numeric`,
`int2` || `bit1` AS `int2_||_bit1`,
`int2` || `bit64` AS `int2_||_bit64`,
`int2` || `boolean` AS `int2_||_boolean`,
`int2` || `char` AS `int2_||_char`,
`int2` || `varchar` AS `int2_||_varchar`,
`int2` || `binary` AS `int2_||_binary`,
`int2` || `varbinary` AS `int2_||_varbinary`,
`int2` || `text` AS `int2_||_text`,
`uint2` || `int1` AS `uint2_||_int1`,
`uint2` || `uint1` AS `uint2_||_uint1`,
`uint2` || `int2` AS `uint2_||_int2`,
`uint2` || `uint2` AS `uint2_||_uint2`,
`uint2` || `int4` AS `uint2_||_int4`,
`uint2` || `uint4` AS `uint2_||_uint4`,
`uint2` || `int8` AS `uint2_||_int8`,
`uint2` || `uint8` AS `uint2_||_uint8`,
`uint2` || `float4` AS `uint2_||_float4`,
`uint2` || `float8` AS `uint2_||_float8`,
`uint2` || `numeric` AS `uint2_||_numeric`,
`uint2` || `bit1` AS `uint2_||_bit1`,
`uint2` || `bit64` AS `uint2_||_bit64`,
`uint2` || `boolean` AS `uint2_||_boolean`,
`uint2` || `char` AS `uint2_||_char`,
`uint2` || `varchar` AS `uint2_||_varchar`,
`uint2` || `binary` AS `uint2_||_binary`,
`uint2` || `varbinary` AS `uint2_||_varbinary`,
`uint2` || `text` AS `uint2_||_text`,
`int4` || `int1` AS `int4_||_int1`,
`int4` || `uint1` AS `int4_||_uint1`,
`int4` || `int2` AS `int4_||_int2`,
`int4` || `uint2` AS `int4_||_uint2`,
`int4` || `int4` AS `int4_||_int4`,
`int4` || `uint4` AS `int4_||_uint4`,
`int4` || `int8` AS `int4_||_int8`,
`int4` || `uint8` AS `int4_||_uint8`,
`int4` || `float4` AS `int4_||_float4`,
`int4` || `float8` AS `int4_||_float8`,
`int4` || `numeric` AS `int4_||_numeric`,
`int4` || `bit1` AS `int4_||_bit1`,
`int4` || `bit64` AS `int4_||_bit64`,
`int4` || `boolean` AS `int4_||_boolean`,
`int4` || `char` AS `int4_||_char`,
`int4` || `varchar` AS `int4_||_varchar`,
`int4` || `binary` AS `int4_||_binary`,
`int4` || `varbinary` AS `int4_||_varbinary`,
`int4` || `text` AS `int4_||_text`,
`uint4` || `int1` AS `uint4_||_int1`,
`uint4` || `uint1` AS `uint4_||_uint1`,
`uint4` || `int2` AS `uint4_||_int2`,
`uint4` || `uint2` AS `uint4_||_uint2`,
`uint4` || `int4` AS `uint4_||_int4`,
`uint4` || `uint4` AS `uint4_||_uint4`,
`uint4` || `int8` AS `uint4_||_int8`,
`uint4` || `uint8` AS `uint4_||_uint8`,
`uint4` || `float4` AS `uint4_||_float4`,
`uint4` || `float8` AS `uint4_||_float8`,
`uint4` || `numeric` AS `uint4_||_numeric`,
`uint4` || `bit1` AS `uint4_||_bit1`,
`uint4` || `bit64` AS `uint4_||_bit64`,
`uint4` || `boolean` AS `uint4_||_boolean`,
`uint4` || `char` AS `uint4_||_char`,
`uint4` || `varchar` AS `uint4_||_varchar`,
`uint4` || `binary` AS `uint4_||_binary`,
`uint4` || `varbinary` AS `uint4_||_varbinary`,
`uint4` || `text` AS `uint4_||_text`,
`int8` || `int1` AS `int8_||_int1`,
`int8` || `uint1` AS `int8_||_uint1`,
`int8` || `int2` AS `int8_||_int2`,
`int8` || `uint2` AS `int8_||_uint2`,
`int8` || `int4` AS `int8_||_int4`,
`int8` || `uint4` AS `int8_||_uint4`,
`int8` || `int8` AS `int8_||_int8`,
`int8` || `uint8` AS `int8_||_uint8`,
`int8` || `float4` AS `int8_||_float4`,
`int8` || `float8` AS `int8_||_float8`,
`int8` || `numeric` AS `int8_||_numeric`,
`int8` || `bit1` AS `int8_||_bit1`,
`int8` || `bit64` AS `int8_||_bit64`,
`int8` || `boolean` AS `int8_||_boolean`,
`int8` || `char` AS `int8_||_char`,
`int8` || `varchar` AS `int8_||_varchar`,
`int8` || `binary` AS `int8_||_binary`,
`int8` || `varbinary` AS `int8_||_varbinary`,
`int8` || `text` AS `int8_||_text`,
`uint8` || `int1` AS `uint8_||_int1`,
`uint8` || `uint1` AS `uint8_||_uint1`,
`uint8` || `int2` AS `uint8_||_int2`,
`uint8` || `uint2` AS `uint8_||_uint2`,
`uint8` || `int4` AS `uint8_||_int4`,
`uint8` || `uint4` AS `uint8_||_uint4`,
`uint8` || `int8` AS `uint8_||_int8`,
`uint8` || `uint8` AS `uint8_||_uint8`,
`uint8` || `float4` AS `uint8_||_float4`,
`uint8` || `float8` AS `uint8_||_float8`,
`uint8` || `numeric` AS `uint8_||_numeric`,
`uint8` || `bit1` AS `uint8_||_bit1`,
`uint8` || `bit64` AS `uint8_||_bit64`,
`uint8` || `boolean` AS `uint8_||_boolean`,
`uint8` || `char` AS `uint8_||_char`,
`uint8` || `varchar` AS `uint8_||_varchar`,
`uint8` || `binary` AS `uint8_||_binary`,
`uint8` || `varbinary` AS `uint8_||_varbinary`,
`uint8` || `text` AS `uint8_||_text`,
`float4` || `int1` AS `float4_||_int1`,
`float4` || `uint1` AS `float4_||_uint1`,
`float4` || `int2` AS `float4_||_int2`,
`float4` || `uint2` AS `float4_||_uint2`,
`float4` || `int4` AS `float4_||_int4`,
`float4` || `uint4` AS `float4_||_uint4`,
`float4` || `int8` AS `float4_||_int8`,
`float4` || `uint8` AS `float4_||_uint8`,
`float4` || `float4` AS `float4_||_float4`,
`float4` || `float8` AS `float4_||_float8`,
`float4` || `numeric` AS `float4_||_numeric`,
`float4` || `bit1` AS `float4_||_bit1`,
`float4` || `bit64` AS `float4_||_bit64`,
`float4` || `boolean` AS `float4_||_boolean`,
`float4` || `char` AS `float4_||_char`,
`float4` || `varchar` AS `float4_||_varchar`,
`float4` || `binary` AS `float4_||_binary`,
`float4` || `varbinary` AS `float4_||_varbinary`,
`float4` || `text` AS `float4_||_text`,
`float8` || `int1` AS `float8_||_int1`,
`float8` || `uint1` AS `float8_||_uint1`,
`float8` || `int2` AS `float8_||_int2`,
`float8` || `uint2` AS `float8_||_uint2`,
`float8` || `int4` AS `float8_||_int4`,
`float8` || `uint4` AS `float8_||_uint4`,
`float8` || `int8` AS `float8_||_int8`,
`float8` || `uint8` AS `float8_||_uint8`,
`float8` || `float4` AS `float8_||_float4`,
`float8` || `float8` AS `float8_||_float8`,
`float8` || `numeric` AS `float8_||_numeric`,
`float8` || `bit1` AS `float8_||_bit1`,
`float8` || `bit64` AS `float8_||_bit64`,
`float8` || `boolean` AS `float8_||_boolean`,
`float8` || `char` AS `float8_||_char`,
`float8` || `varchar` AS `float8_||_varchar`,
`float8` || `binary` AS `float8_||_binary`,
`float8` || `varbinary` AS `float8_||_varbinary`,
`float8` || `text` AS `float8_||_text`,
`numeric` || `int1` AS `numeric_||_int1`,
`numeric` || `uint1` AS `numeric_||_uint1`,
`numeric` || `int2` AS `numeric_||_int2`,
`numeric` || `uint2` AS `numeric_||_uint2`,
`numeric` || `int4` AS `numeric_||_int4`,
`numeric` || `uint4` AS `numeric_||_uint4`,
`numeric` || `int8` AS `numeric_||_int8`,
`numeric` || `uint8` AS `numeric_||_uint8`,
`numeric` || `float4` AS `numeric_||_float4`,
`numeric` || `float8` AS `numeric_||_float8`,
`numeric` || `numeric` AS `numeric_||_numeric`,
`numeric` || `bit1` AS `numeric_||_bit1`,
`numeric` || `bit64` AS `numeric_||_bit64`,
`numeric` || `boolean` AS `numeric_||_boolean`,
`numeric` || `char` AS `numeric_||_char`,
`numeric` || `varchar` AS `numeric_||_varchar`,
`numeric` || `binary` AS `numeric_||_binary`,
`numeric` || `varbinary` AS `numeric_||_varbinary`,
`numeric` || `text` AS `numeric_||_text`,
`bit1` || `int1` AS `bit1_||_int1`,
`bit1` || `uint1` AS `bit1_||_uint1`,
`bit1` || `int2` AS `bit1_||_int2`,
`bit1` || `uint2` AS `bit1_||_uint2`,
`bit1` || `int4` AS `bit1_||_int4`,
`bit1` || `uint4` AS `bit1_||_uint4`,
`bit1` || `int8` AS `bit1_||_int8`,
`bit1` || `uint8` AS `bit1_||_uint8`,
`bit1` || `float4` AS `bit1_||_float4`,
`bit1` || `float8` AS `bit1_||_float8`,
`bit1` || `numeric` AS `bit1_||_numeric`,
`bit1` || `bit1` AS `bit1_||_bit1`,
`bit1` || `bit64` AS `bit1_||_bit64`,
`bit1` || `boolean` AS `bit1_||_boolean`,
`bit1` || `char` AS `bit1_||_char`,
`bit1` || `varchar` AS `bit1_||_varchar`,
`bit1` || `binary` AS `bit1_||_binary`,
`bit1` || `varbinary` AS `bit1_||_varbinary`,
`bit1` || `text` AS `bit1_||_text`,
`bit64` || `int1` AS `bit64_||_int1`,
`bit64` || `uint1` AS `bit64_||_uint1`,
`bit64` || `int2` AS `bit64_||_int2`,
`bit64` || `uint2` AS `bit64_||_uint2`,
`bit64` || `int4` AS `bit64_||_int4`,
`bit64` || `uint4` AS `bit64_||_uint4`,
`bit64` || `int8` AS `bit64_||_int8`,
`bit64` || `uint8` AS `bit64_||_uint8`,
`bit64` || `float4` AS `bit64_||_float4`,
`bit64` || `float8` AS `bit64_||_float8`,
`bit64` || `numeric` AS `bit64_||_numeric`,
`bit64` || `bit1` AS `bit64_||_bit1`,
`bit64` || `bit64` AS `bit64_||_bit64`,
`bit64` || `boolean` AS `bit64_||_boolean`,
`bit64` || `char` AS `bit64_||_char`,
`bit64` || `varchar` AS `bit64_||_varchar`,
`bit64` || `binary` AS `bit64_||_binary`,
`bit64` || `varbinary` AS `bit64_||_varbinary`,
`bit64` || `text` AS `bit64_||_text`,
`boolean` || `int1` AS `boolean_||_int1`,
`boolean` || `uint1` AS `boolean_||_uint1`,
`boolean` || `int2` AS `boolean_||_int2`,
`boolean` || `uint2` AS `boolean_||_uint2`,
`boolean` || `int4` AS `boolean_||_int4`,
`boolean` || `uint4` AS `boolean_||_uint4`,
`boolean` || `int8` AS `boolean_||_int8`,
`boolean` || `uint8` AS `boolean_||_uint8`,
`boolean` || `float4` AS `boolean_||_float4`,
`boolean` || `float8` AS `boolean_||_float8`,
`boolean` || `numeric` AS `boolean_||_numeric`,
`boolean` || `bit1` AS `boolean_||_bit1`,
`boolean` || `bit64` AS `boolean_||_bit64`,
`boolean` || `boolean` AS `boolean_||_boolean`,
`boolean` || `char` AS `boolean_||_char`,
`boolean` || `varchar` AS `boolean_||_varchar`,
`boolean` || `binary` AS `boolean_||_binary`,
`boolean` || `varbinary` AS `boolean_||_varbinary`,
`boolean` || `text` AS `boolean_||_text`,
`char` || `int1` AS `char_||_int1`,
`char` || `uint1` AS `char_||_uint1`,
`char` || `int2` AS `char_||_int2`,
`char` || `uint2` AS `char_||_uint2`,
`char` || `int4` AS `char_||_int4`,
`char` || `uint4` AS `char_||_uint4`,
`char` || `int8` AS `char_||_int8`,
`char` || `uint8` AS `char_||_uint8`,
`char` || `float4` AS `char_||_float4`,
`char` || `float8` AS `char_||_float8`,
`char` || `numeric` AS `char_||_numeric`,
`char` || `bit1` AS `char_||_bit1`,
`char` || `bit64` AS `char_||_bit64`,
`char` || `boolean` AS `char_||_boolean`,
`char` || `char` AS `char_||_char`,
`char` || `varchar` AS `char_||_varchar`,
`char` || `binary` AS `char_||_binary`,
`char` || `varbinary` AS `char_||_varbinary`,
`char` || `text` AS `char_||_text`,
`varchar` || `int1` AS `varchar_||_int1`,
`varchar` || `uint1` AS `varchar_||_uint1`,
`varchar` || `int2` AS `varchar_||_int2`,
`varchar` || `uint2` AS `varchar_||_uint2`,
`varchar` || `int4` AS `varchar_||_int4`,
`varchar` || `uint4` AS `varchar_||_uint4`,
`varchar` || `int8` AS `varchar_||_int8`,
`varchar` || `uint8` AS `varchar_||_uint8`,
`varchar` || `float4` AS `varchar_||_float4`,
`varchar` || `float8` AS `varchar_||_float8`,
`varchar` || `numeric` AS `varchar_||_numeric`,
`varchar` || `bit1` AS `varchar_||_bit1`,
`varchar` || `bit64` AS `varchar_||_bit64`,
`varchar` || `boolean` AS `varchar_||_boolean`,
`varchar` || `char` AS `varchar_||_char`,
`varchar` || `varchar` AS `varchar_||_varchar`,
`varchar` || `binary` AS `varchar_||_binary`,
`varchar` || `varbinary` AS `varchar_||_varbinary`,
`varchar` || `text` AS `varchar_||_text`,
`binary` || `int1` AS `binary_||_int1`,
`binary` || `uint1` AS `binary_||_uint1`,
`binary` || `int2` AS `binary_||_int2`,
`binary` || `uint2` AS `binary_||_uint2`,
`binary` || `int4` AS `binary_||_int4`,
`binary` || `uint4` AS `binary_||_uint4`,
`binary` || `int8` AS `binary_||_int8`,
`binary` || `uint8` AS `binary_||_uint8`,
`binary` || `float4` AS `binary_||_float4`,
`binary` || `float8` AS `binary_||_float8`,
`binary` || `numeric` AS `binary_||_numeric`,
`binary` || `bit1` AS `binary_||_bit1`,
`binary` || `bit64` AS `binary_||_bit64`,
`binary` || `boolean` AS `binary_||_boolean`,
`binary` || `char` AS `binary_||_char`,
`binary` || `varchar` AS `binary_||_varchar`,
`binary` || `binary` AS `binary_||_binary`,
`binary` || `varbinary` AS `binary_||_varbinary`,
`binary` || `text` AS `binary_||_text`,
`varbinary` || `int1` AS `varbinary_||_int1`,
`varbinary` || `uint1` AS `varbinary_||_uint1`,
`varbinary` || `int2` AS `varbinary_||_int2`,
`varbinary` || `uint2` AS `varbinary_||_uint2`,
`varbinary` || `int4` AS `varbinary_||_int4`,
`varbinary` || `uint4` AS `varbinary_||_uint4`,
`varbinary` || `int8` AS `varbinary_||_int8`,
`varbinary` || `uint8` AS `varbinary_||_uint8`,
`varbinary` || `float4` AS `varbinary_||_float4`,
`varbinary` || `float8` AS `varbinary_||_float8`,
`varbinary` || `numeric` AS `varbinary_||_numeric`,
`varbinary` || `bit1` AS `varbinary_||_bit1`,
`varbinary` || `bit64` AS `varbinary_||_bit64`,
`varbinary` || `boolean` AS `varbinary_||_boolean`,
`varbinary` || `char` AS `varbinary_||_char`,
`varbinary` || `varchar` AS `varbinary_||_varchar`,
`varbinary` || `binary` AS `varbinary_||_binary`,
`varbinary` || `varbinary` AS `varbinary_||_varbinary`,
`varbinary` || `text` AS `varbinary_||_text`,
`text` || `int1` AS `text_||_int1`,
`text` || `uint1` AS `text_||_uint1`,
`text` || `int2` AS `text_||_int2`,
`text` || `uint2` AS `text_||_uint2`,
`text` || `int4` AS `text_||_int4`,
`text` || `uint4` AS `text_||_uint4`,
`text` || `int8` AS `text_||_int8`,
`text` || `uint8` AS `text_||_uint8`,
`text` || `float4` AS `text_||_float4`,
`text` || `float8` AS `text_||_float8`,
`text` || `numeric` AS `text_||_numeric`,
`text` || `bit1` AS `text_||_bit1`,
`text` || `bit64` AS `text_||_bit64`,
`text` || `boolean` AS `text_||_boolean`,
`text` || `char` AS `text_||_char`,
`text` || `varchar` AS `text_||_varchar`,
`text` || `binary` AS `text_||_binary`,
`text` || `varbinary` AS `text_||_varbinary`,
`text` || `text` AS `text_||_text`
FROM test_multi_type_table;
\x
SHOW COLUMNS FROM test_multi_type;
DELETE FROM test_multi_type;
INSERT INTO test_multi_type VALUES(null);

UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int1_||_int1` = test_multi_type_table.`int1` || test_multi_type_table.`int1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int1_||_uint1` = test_multi_type_table.`int1` || test_multi_type_table.`uint1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int1_||_int2` = test_multi_type_table.`int1` || test_multi_type_table.`int2`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int1_||_uint2` = test_multi_type_table.`int1` || test_multi_type_table.`uint2`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int1_||_int4` = test_multi_type_table.`int1` || test_multi_type_table.`int4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int1_||_uint4` = test_multi_type_table.`int1` || test_multi_type_table.`uint4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int1_||_int8` = test_multi_type_table.`int1` || test_multi_type_table.`int8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int1_||_uint8` = test_multi_type_table.`int1` || test_multi_type_table.`uint8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int1_||_float4` = test_multi_type_table.`int1` || test_multi_type_table.`float4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int1_||_float8` = test_multi_type_table.`int1` || test_multi_type_table.`float8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int1_||_numeric` = test_multi_type_table.`int1` || test_multi_type_table.`numeric`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int1_||_bit1` = test_multi_type_table.`int1` || test_multi_type_table.`bit1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int1_||_bit64` = test_multi_type_table.`int1` || test_multi_type_table.`bit64`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int1_||_boolean` = test_multi_type_table.`int1` || test_multi_type_table.`boolean`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int1_||_char` = test_multi_type_table.`int1` || test_multi_type_table.`char`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int1_||_varchar` = test_multi_type_table.`int1` || test_multi_type_table.`varchar`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int1_||_binary` = test_multi_type_table.`int1` || test_multi_type_table.`binary`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int1_||_varbinary` = test_multi_type_table.`int1` || test_multi_type_table.`varbinary`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int1_||_text` = test_multi_type_table.`int1` || test_multi_type_table.`text`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint1_||_int1` = test_multi_type_table.`uint1` || test_multi_type_table.`int1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint1_||_uint1` = test_multi_type_table.`uint1` || test_multi_type_table.`uint1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint1_||_int2` = test_multi_type_table.`uint1` || test_multi_type_table.`int2`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint1_||_uint2` = test_multi_type_table.`uint1` || test_multi_type_table.`uint2`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint1_||_int4` = test_multi_type_table.`uint1` || test_multi_type_table.`int4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint1_||_uint4` = test_multi_type_table.`uint1` || test_multi_type_table.`uint4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint1_||_int8` = test_multi_type_table.`uint1` || test_multi_type_table.`int8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint1_||_uint8` = test_multi_type_table.`uint1` || test_multi_type_table.`uint8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint1_||_float4` = test_multi_type_table.`uint1` || test_multi_type_table.`float4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint1_||_float8` = test_multi_type_table.`uint1` || test_multi_type_table.`float8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint1_||_numeric` = test_multi_type_table.`uint1` || test_multi_type_table.`numeric`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint1_||_bit1` = test_multi_type_table.`uint1` || test_multi_type_table.`bit1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint1_||_bit64` = test_multi_type_table.`uint1` || test_multi_type_table.`bit64`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint1_||_boolean` = test_multi_type_table.`uint1` || test_multi_type_table.`boolean`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint1_||_char` = test_multi_type_table.`uint1` || test_multi_type_table.`char`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint1_||_varchar` = test_multi_type_table.`uint1` || test_multi_type_table.`varchar`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint1_||_binary` = test_multi_type_table.`uint1` || test_multi_type_table.`binary`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint1_||_varbinary` = test_multi_type_table.`uint1` || test_multi_type_table.`varbinary`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint1_||_text` = test_multi_type_table.`uint1` || test_multi_type_table.`text`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int2_||_int1` = test_multi_type_table.`int2` || test_multi_type_table.`int1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int2_||_uint1` = test_multi_type_table.`int2` || test_multi_type_table.`uint1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int2_||_int2` = test_multi_type_table.`int2` || test_multi_type_table.`int2`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int2_||_uint2` = test_multi_type_table.`int2` || test_multi_type_table.`uint2`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int2_||_int4` = test_multi_type_table.`int2` || test_multi_type_table.`int4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int2_||_uint4` = test_multi_type_table.`int2` || test_multi_type_table.`uint4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int2_||_int8` = test_multi_type_table.`int2` || test_multi_type_table.`int8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int2_||_uint8` = test_multi_type_table.`int2` || test_multi_type_table.`uint8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int2_||_float4` = test_multi_type_table.`int2` || test_multi_type_table.`float4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int2_||_float8` = test_multi_type_table.`int2` || test_multi_type_table.`float8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int2_||_numeric` = test_multi_type_table.`int2` || test_multi_type_table.`numeric`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int2_||_bit1` = test_multi_type_table.`int2` || test_multi_type_table.`bit1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int2_||_bit64` = test_multi_type_table.`int2` || test_multi_type_table.`bit64`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int2_||_boolean` = test_multi_type_table.`int2` || test_multi_type_table.`boolean`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int2_||_char` = test_multi_type_table.`int2` || test_multi_type_table.`char`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int2_||_varchar` = test_multi_type_table.`int2` || test_multi_type_table.`varchar`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int2_||_binary` = test_multi_type_table.`int2` || test_multi_type_table.`binary`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int2_||_varbinary` = test_multi_type_table.`int2` || test_multi_type_table.`varbinary`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int2_||_text` = test_multi_type_table.`int2` || test_multi_type_table.`text`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint2_||_int1` = test_multi_type_table.`uint2` || test_multi_type_table.`int1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint2_||_uint1` = test_multi_type_table.`uint2` || test_multi_type_table.`uint1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint2_||_int2` = test_multi_type_table.`uint2` || test_multi_type_table.`int2`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint2_||_uint2` = test_multi_type_table.`uint2` || test_multi_type_table.`uint2`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint2_||_int4` = test_multi_type_table.`uint2` || test_multi_type_table.`int4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint2_||_uint4` = test_multi_type_table.`uint2` || test_multi_type_table.`uint4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint2_||_int8` = test_multi_type_table.`uint2` || test_multi_type_table.`int8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint2_||_uint8` = test_multi_type_table.`uint2` || test_multi_type_table.`uint8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint2_||_float4` = test_multi_type_table.`uint2` || test_multi_type_table.`float4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint2_||_float8` = test_multi_type_table.`uint2` || test_multi_type_table.`float8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint2_||_numeric` = test_multi_type_table.`uint2` || test_multi_type_table.`numeric`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint2_||_bit1` = test_multi_type_table.`uint2` || test_multi_type_table.`bit1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint2_||_bit64` = test_multi_type_table.`uint2` || test_multi_type_table.`bit64`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint2_||_boolean` = test_multi_type_table.`uint2` || test_multi_type_table.`boolean`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint2_||_char` = test_multi_type_table.`uint2` || test_multi_type_table.`char`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint2_||_varchar` = test_multi_type_table.`uint2` || test_multi_type_table.`varchar`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint2_||_binary` = test_multi_type_table.`uint2` || test_multi_type_table.`binary`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint2_||_varbinary` = test_multi_type_table.`uint2` || test_multi_type_table.`varbinary`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint2_||_text` = test_multi_type_table.`uint2` || test_multi_type_table.`text`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int4_||_int1` = test_multi_type_table.`int4` || test_multi_type_table.`int1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int4_||_uint1` = test_multi_type_table.`int4` || test_multi_type_table.`uint1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int4_||_int2` = test_multi_type_table.`int4` || test_multi_type_table.`int2`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int4_||_uint2` = test_multi_type_table.`int4` || test_multi_type_table.`uint2`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int4_||_int4` = test_multi_type_table.`int4` || test_multi_type_table.`int4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int4_||_uint4` = test_multi_type_table.`int4` || test_multi_type_table.`uint4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int4_||_int8` = test_multi_type_table.`int4` || test_multi_type_table.`int8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int4_||_uint8` = test_multi_type_table.`int4` || test_multi_type_table.`uint8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int4_||_float4` = test_multi_type_table.`int4` || test_multi_type_table.`float4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int4_||_float8` = test_multi_type_table.`int4` || test_multi_type_table.`float8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int4_||_numeric` = test_multi_type_table.`int4` || test_multi_type_table.`numeric`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int4_||_bit1` = test_multi_type_table.`int4` || test_multi_type_table.`bit1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int4_||_bit64` = test_multi_type_table.`int4` || test_multi_type_table.`bit64`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int4_||_boolean` = test_multi_type_table.`int4` || test_multi_type_table.`boolean`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int4_||_char` = test_multi_type_table.`int4` || test_multi_type_table.`char`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int4_||_varchar` = test_multi_type_table.`int4` || test_multi_type_table.`varchar`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int4_||_binary` = test_multi_type_table.`int4` || test_multi_type_table.`binary`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int4_||_varbinary` = test_multi_type_table.`int4` || test_multi_type_table.`varbinary`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int4_||_text` = test_multi_type_table.`int4` || test_multi_type_table.`text`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint4_||_int1` = test_multi_type_table.`uint4` || test_multi_type_table.`int1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint4_||_uint1` = test_multi_type_table.`uint4` || test_multi_type_table.`uint1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint4_||_int2` = test_multi_type_table.`uint4` || test_multi_type_table.`int2`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint4_||_uint2` = test_multi_type_table.`uint4` || test_multi_type_table.`uint2`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint4_||_int4` = test_multi_type_table.`uint4` || test_multi_type_table.`int4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint4_||_uint4` = test_multi_type_table.`uint4` || test_multi_type_table.`uint4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint4_||_int8` = test_multi_type_table.`uint4` || test_multi_type_table.`int8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint4_||_uint8` = test_multi_type_table.`uint4` || test_multi_type_table.`uint8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint4_||_float4` = test_multi_type_table.`uint4` || test_multi_type_table.`float4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint4_||_float8` = test_multi_type_table.`uint4` || test_multi_type_table.`float8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint4_||_numeric` = test_multi_type_table.`uint4` || test_multi_type_table.`numeric`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint4_||_bit1` = test_multi_type_table.`uint4` || test_multi_type_table.`bit1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint4_||_bit64` = test_multi_type_table.`uint4` || test_multi_type_table.`bit64`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint4_||_boolean` = test_multi_type_table.`uint4` || test_multi_type_table.`boolean`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint4_||_char` = test_multi_type_table.`uint4` || test_multi_type_table.`char`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint4_||_varchar` = test_multi_type_table.`uint4` || test_multi_type_table.`varchar`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint4_||_binary` = test_multi_type_table.`uint4` || test_multi_type_table.`binary`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint4_||_varbinary` = test_multi_type_table.`uint4` || test_multi_type_table.`varbinary`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint4_||_text` = test_multi_type_table.`uint4` || test_multi_type_table.`text`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int8_||_int1` = test_multi_type_table.`int8` || test_multi_type_table.`int1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int8_||_uint1` = test_multi_type_table.`int8` || test_multi_type_table.`uint1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int8_||_int2` = test_multi_type_table.`int8` || test_multi_type_table.`int2`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int8_||_uint2` = test_multi_type_table.`int8` || test_multi_type_table.`uint2`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int8_||_int4` = test_multi_type_table.`int8` || test_multi_type_table.`int4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int8_||_uint4` = test_multi_type_table.`int8` || test_multi_type_table.`uint4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int8_||_int8` = test_multi_type_table.`int8` || test_multi_type_table.`int8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int8_||_uint8` = test_multi_type_table.`int8` || test_multi_type_table.`uint8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int8_||_float4` = test_multi_type_table.`int8` || test_multi_type_table.`float4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int8_||_float8` = test_multi_type_table.`int8` || test_multi_type_table.`float8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int8_||_numeric` = test_multi_type_table.`int8` || test_multi_type_table.`numeric`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int8_||_bit1` = test_multi_type_table.`int8` || test_multi_type_table.`bit1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int8_||_bit64` = test_multi_type_table.`int8` || test_multi_type_table.`bit64`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int8_||_boolean` = test_multi_type_table.`int8` || test_multi_type_table.`boolean`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int8_||_char` = test_multi_type_table.`int8` || test_multi_type_table.`char`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int8_||_varchar` = test_multi_type_table.`int8` || test_multi_type_table.`varchar`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int8_||_binary` = test_multi_type_table.`int8` || test_multi_type_table.`binary`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int8_||_varbinary` = test_multi_type_table.`int8` || test_multi_type_table.`varbinary`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int8_||_text` = test_multi_type_table.`int8` || test_multi_type_table.`text`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint8_||_int1` = test_multi_type_table.`uint8` || test_multi_type_table.`int1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint8_||_uint1` = test_multi_type_table.`uint8` || test_multi_type_table.`uint1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint8_||_int2` = test_multi_type_table.`uint8` || test_multi_type_table.`int2`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint8_||_uint2` = test_multi_type_table.`uint8` || test_multi_type_table.`uint2`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint8_||_int4` = test_multi_type_table.`uint8` || test_multi_type_table.`int4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint8_||_uint4` = test_multi_type_table.`uint8` || test_multi_type_table.`uint4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint8_||_int8` = test_multi_type_table.`uint8` || test_multi_type_table.`int8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint8_||_uint8` = test_multi_type_table.`uint8` || test_multi_type_table.`uint8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint8_||_float4` = test_multi_type_table.`uint8` || test_multi_type_table.`float4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint8_||_float8` = test_multi_type_table.`uint8` || test_multi_type_table.`float8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint8_||_numeric` = test_multi_type_table.`uint8` || test_multi_type_table.`numeric`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint8_||_bit1` = test_multi_type_table.`uint8` || test_multi_type_table.`bit1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint8_||_bit64` = test_multi_type_table.`uint8` || test_multi_type_table.`bit64`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint8_||_boolean` = test_multi_type_table.`uint8` || test_multi_type_table.`boolean`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint8_||_char` = test_multi_type_table.`uint8` || test_multi_type_table.`char`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint8_||_varchar` = test_multi_type_table.`uint8` || test_multi_type_table.`varchar`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint8_||_binary` = test_multi_type_table.`uint8` || test_multi_type_table.`binary`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint8_||_varbinary` = test_multi_type_table.`uint8` || test_multi_type_table.`varbinary`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint8_||_text` = test_multi_type_table.`uint8` || test_multi_type_table.`text`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`float4_||_int1` = test_multi_type_table.`float4` || test_multi_type_table.`int1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`float4_||_uint1` = test_multi_type_table.`float4` || test_multi_type_table.`uint1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`float4_||_int2` = test_multi_type_table.`float4` || test_multi_type_table.`int2`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`float4_||_uint2` = test_multi_type_table.`float4` || test_multi_type_table.`uint2`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`float4_||_int4` = test_multi_type_table.`float4` || test_multi_type_table.`int4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`float4_||_uint4` = test_multi_type_table.`float4` || test_multi_type_table.`uint4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`float4_||_int8` = test_multi_type_table.`float4` || test_multi_type_table.`int8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`float4_||_uint8` = test_multi_type_table.`float4` || test_multi_type_table.`uint8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`float4_||_float4` = test_multi_type_table.`float4` || test_multi_type_table.`float4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`float4_||_float8` = test_multi_type_table.`float4` || test_multi_type_table.`float8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`float4_||_numeric` = test_multi_type_table.`float4` || test_multi_type_table.`numeric`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`float4_||_bit1` = test_multi_type_table.`float4` || test_multi_type_table.`bit1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`float4_||_bit64` = test_multi_type_table.`float4` || test_multi_type_table.`bit64`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`float4_||_boolean` = test_multi_type_table.`float4` || test_multi_type_table.`boolean`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`float4_||_char` = test_multi_type_table.`float4` || test_multi_type_table.`char`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`float4_||_varchar` = test_multi_type_table.`float4` || test_multi_type_table.`varchar`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`float4_||_binary` = test_multi_type_table.`float4` || test_multi_type_table.`binary`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`float4_||_varbinary` = test_multi_type_table.`float4` || test_multi_type_table.`varbinary`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`float4_||_text` = test_multi_type_table.`float4` || test_multi_type_table.`text`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`float8_||_int1` = test_multi_type_table.`float8` || test_multi_type_table.`int1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`float8_||_uint1` = test_multi_type_table.`float8` || test_multi_type_table.`uint1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`float8_||_int2` = test_multi_type_table.`float8` || test_multi_type_table.`int2`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`float8_||_uint2` = test_multi_type_table.`float8` || test_multi_type_table.`uint2`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`float8_||_int4` = test_multi_type_table.`float8` || test_multi_type_table.`int4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`float8_||_uint4` = test_multi_type_table.`float8` || test_multi_type_table.`uint4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`float8_||_int8` = test_multi_type_table.`float8` || test_multi_type_table.`int8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`float8_||_uint8` = test_multi_type_table.`float8` || test_multi_type_table.`uint8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`float8_||_float4` = test_multi_type_table.`float8` || test_multi_type_table.`float4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`float8_||_float8` = test_multi_type_table.`float8` || test_multi_type_table.`float8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`float8_||_numeric` = test_multi_type_table.`float8` || test_multi_type_table.`numeric`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`float8_||_bit1` = test_multi_type_table.`float8` || test_multi_type_table.`bit1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`float8_||_bit64` = test_multi_type_table.`float8` || test_multi_type_table.`bit64`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`float8_||_boolean` = test_multi_type_table.`float8` || test_multi_type_table.`boolean`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`float8_||_char` = test_multi_type_table.`float8` || test_multi_type_table.`char`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`float8_||_varchar` = test_multi_type_table.`float8` || test_multi_type_table.`varchar`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`float8_||_binary` = test_multi_type_table.`float8` || test_multi_type_table.`binary`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`float8_||_varbinary` = test_multi_type_table.`float8` || test_multi_type_table.`varbinary`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`float8_||_text` = test_multi_type_table.`float8` || test_multi_type_table.`text`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`numeric_||_int1` = test_multi_type_table.`numeric` || test_multi_type_table.`int1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`numeric_||_uint1` = test_multi_type_table.`numeric` || test_multi_type_table.`uint1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`numeric_||_int2` = test_multi_type_table.`numeric` || test_multi_type_table.`int2`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`numeric_||_uint2` = test_multi_type_table.`numeric` || test_multi_type_table.`uint2`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`numeric_||_int4` = test_multi_type_table.`numeric` || test_multi_type_table.`int4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`numeric_||_uint4` = test_multi_type_table.`numeric` || test_multi_type_table.`uint4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`numeric_||_int8` = test_multi_type_table.`numeric` || test_multi_type_table.`int8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`numeric_||_uint8` = test_multi_type_table.`numeric` || test_multi_type_table.`uint8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`numeric_||_float4` = test_multi_type_table.`numeric` || test_multi_type_table.`float4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`numeric_||_float8` = test_multi_type_table.`numeric` || test_multi_type_table.`float8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`numeric_||_numeric` = test_multi_type_table.`numeric` || test_multi_type_table.`numeric`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`numeric_||_bit1` = test_multi_type_table.`numeric` || test_multi_type_table.`bit1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`numeric_||_bit64` = test_multi_type_table.`numeric` || test_multi_type_table.`bit64`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`numeric_||_boolean` = test_multi_type_table.`numeric` || test_multi_type_table.`boolean`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`numeric_||_char` = test_multi_type_table.`numeric` || test_multi_type_table.`char`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`numeric_||_varchar` = test_multi_type_table.`numeric` || test_multi_type_table.`varchar`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`numeric_||_binary` = test_multi_type_table.`numeric` || test_multi_type_table.`binary`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`numeric_||_varbinary` = test_multi_type_table.`numeric` || test_multi_type_table.`varbinary`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`numeric_||_text` = test_multi_type_table.`numeric` || test_multi_type_table.`text`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`bit1_||_int1` = test_multi_type_table.`bit1` || test_multi_type_table.`int1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`bit1_||_uint1` = test_multi_type_table.`bit1` || test_multi_type_table.`uint1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`bit1_||_int2` = test_multi_type_table.`bit1` || test_multi_type_table.`int2`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`bit1_||_uint2` = test_multi_type_table.`bit1` || test_multi_type_table.`uint2`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`bit1_||_int4` = test_multi_type_table.`bit1` || test_multi_type_table.`int4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`bit1_||_uint4` = test_multi_type_table.`bit1` || test_multi_type_table.`uint4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`bit1_||_int8` = test_multi_type_table.`bit1` || test_multi_type_table.`int8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`bit1_||_uint8` = test_multi_type_table.`bit1` || test_multi_type_table.`uint8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`bit1_||_float4` = test_multi_type_table.`bit1` || test_multi_type_table.`float4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`bit1_||_float8` = test_multi_type_table.`bit1` || test_multi_type_table.`float8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`bit1_||_numeric` = test_multi_type_table.`bit1` || test_multi_type_table.`numeric`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`bit1_||_bit1` = test_multi_type_table.`bit1` || test_multi_type_table.`bit1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`bit1_||_bit64` = test_multi_type_table.`bit1` || test_multi_type_table.`bit64`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`bit1_||_boolean` = test_multi_type_table.`bit1` || test_multi_type_table.`boolean`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`bit1_||_char` = test_multi_type_table.`bit1` || test_multi_type_table.`char`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`bit1_||_varchar` = test_multi_type_table.`bit1` || test_multi_type_table.`varchar`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`bit1_||_binary` = test_multi_type_table.`bit1` || test_multi_type_table.`binary`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`bit1_||_varbinary` = test_multi_type_table.`bit1` || test_multi_type_table.`varbinary`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`bit1_||_text` = test_multi_type_table.`bit1` || test_multi_type_table.`text`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`bit64_||_int1` = test_multi_type_table.`bit64` || test_multi_type_table.`int1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`bit64_||_uint1` = test_multi_type_table.`bit64` || test_multi_type_table.`uint1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`bit64_||_int2` = test_multi_type_table.`bit64` || test_multi_type_table.`int2`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`bit64_||_uint2` = test_multi_type_table.`bit64` || test_multi_type_table.`uint2`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`bit64_||_int4` = test_multi_type_table.`bit64` || test_multi_type_table.`int4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`bit64_||_uint4` = test_multi_type_table.`bit64` || test_multi_type_table.`uint4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`bit64_||_int8` = test_multi_type_table.`bit64` || test_multi_type_table.`int8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`bit64_||_uint8` = test_multi_type_table.`bit64` || test_multi_type_table.`uint8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`bit64_||_float4` = test_multi_type_table.`bit64` || test_multi_type_table.`float4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`bit64_||_float8` = test_multi_type_table.`bit64` || test_multi_type_table.`float8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`bit64_||_numeric` = test_multi_type_table.`bit64` || test_multi_type_table.`numeric`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`bit64_||_bit1` = test_multi_type_table.`bit64` || test_multi_type_table.`bit1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`bit64_||_bit64` = test_multi_type_table.`bit64` || test_multi_type_table.`bit64`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`bit64_||_boolean` = test_multi_type_table.`bit64` || test_multi_type_table.`boolean`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`bit64_||_char` = test_multi_type_table.`bit64` || test_multi_type_table.`char`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`bit64_||_varchar` = test_multi_type_table.`bit64` || test_multi_type_table.`varchar`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`bit64_||_binary` = test_multi_type_table.`bit64` || test_multi_type_table.`binary`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`bit64_||_varbinary` = test_multi_type_table.`bit64` || test_multi_type_table.`varbinary`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`bit64_||_text` = test_multi_type_table.`bit64` || test_multi_type_table.`text`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`boolean_||_int1` = test_multi_type_table.`boolean` || test_multi_type_table.`int1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`boolean_||_uint1` = test_multi_type_table.`boolean` || test_multi_type_table.`uint1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`boolean_||_int2` = test_multi_type_table.`boolean` || test_multi_type_table.`int2`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`boolean_||_uint2` = test_multi_type_table.`boolean` || test_multi_type_table.`uint2`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`boolean_||_int4` = test_multi_type_table.`boolean` || test_multi_type_table.`int4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`boolean_||_uint4` = test_multi_type_table.`boolean` || test_multi_type_table.`uint4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`boolean_||_int8` = test_multi_type_table.`boolean` || test_multi_type_table.`int8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`boolean_||_uint8` = test_multi_type_table.`boolean` || test_multi_type_table.`uint8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`boolean_||_float4` = test_multi_type_table.`boolean` || test_multi_type_table.`float4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`boolean_||_float8` = test_multi_type_table.`boolean` || test_multi_type_table.`float8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`boolean_||_numeric` = test_multi_type_table.`boolean` || test_multi_type_table.`numeric`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`boolean_||_bit1` = test_multi_type_table.`boolean` || test_multi_type_table.`bit1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`boolean_||_bit64` = test_multi_type_table.`boolean` || test_multi_type_table.`bit64`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`boolean_||_boolean` = test_multi_type_table.`boolean` || test_multi_type_table.`boolean`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`boolean_||_char` = test_multi_type_table.`boolean` || test_multi_type_table.`char`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`boolean_||_varchar` = test_multi_type_table.`boolean` || test_multi_type_table.`varchar`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`boolean_||_binary` = test_multi_type_table.`boolean` || test_multi_type_table.`binary`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`boolean_||_varbinary` = test_multi_type_table.`boolean` || test_multi_type_table.`varbinary`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`boolean_||_text` = test_multi_type_table.`boolean` || test_multi_type_table.`text`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`char_||_int1` = test_multi_type_table.`char` || test_multi_type_table.`int1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`char_||_uint1` = test_multi_type_table.`char` || test_multi_type_table.`uint1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`char_||_int2` = test_multi_type_table.`char` || test_multi_type_table.`int2`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`char_||_uint2` = test_multi_type_table.`char` || test_multi_type_table.`uint2`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`char_||_int4` = test_multi_type_table.`char` || test_multi_type_table.`int4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`char_||_uint4` = test_multi_type_table.`char` || test_multi_type_table.`uint4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`char_||_int8` = test_multi_type_table.`char` || test_multi_type_table.`int8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`char_||_uint8` = test_multi_type_table.`char` || test_multi_type_table.`uint8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`char_||_float4` = test_multi_type_table.`char` || test_multi_type_table.`float4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`char_||_float8` = test_multi_type_table.`char` || test_multi_type_table.`float8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`char_||_numeric` = test_multi_type_table.`char` || test_multi_type_table.`numeric`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`char_||_bit1` = test_multi_type_table.`char` || test_multi_type_table.`bit1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`char_||_bit64` = test_multi_type_table.`char` || test_multi_type_table.`bit64`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`char_||_boolean` = test_multi_type_table.`char` || test_multi_type_table.`boolean`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`char_||_char` = test_multi_type_table.`char` || test_multi_type_table.`char`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`char_||_varchar` = test_multi_type_table.`char` || test_multi_type_table.`varchar`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`char_||_binary` = test_multi_type_table.`char` || test_multi_type_table.`binary`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`char_||_varbinary` = test_multi_type_table.`char` || test_multi_type_table.`varbinary`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`char_||_text` = test_multi_type_table.`char` || test_multi_type_table.`text`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`varchar_||_int1` = test_multi_type_table.`varchar` || test_multi_type_table.`int1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`varchar_||_uint1` = test_multi_type_table.`varchar` || test_multi_type_table.`uint1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`varchar_||_int2` = test_multi_type_table.`varchar` || test_multi_type_table.`int2`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`varchar_||_uint2` = test_multi_type_table.`varchar` || test_multi_type_table.`uint2`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`varchar_||_int4` = test_multi_type_table.`varchar` || test_multi_type_table.`int4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`varchar_||_uint4` = test_multi_type_table.`varchar` || test_multi_type_table.`uint4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`varchar_||_int8` = test_multi_type_table.`varchar` || test_multi_type_table.`int8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`varchar_||_uint8` = test_multi_type_table.`varchar` || test_multi_type_table.`uint8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`varchar_||_float4` = test_multi_type_table.`varchar` || test_multi_type_table.`float4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`varchar_||_float8` = test_multi_type_table.`varchar` || test_multi_type_table.`float8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`varchar_||_numeric` = test_multi_type_table.`varchar` || test_multi_type_table.`numeric`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`varchar_||_bit1` = test_multi_type_table.`varchar` || test_multi_type_table.`bit1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`varchar_||_bit64` = test_multi_type_table.`varchar` || test_multi_type_table.`bit64`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`varchar_||_boolean` = test_multi_type_table.`varchar` || test_multi_type_table.`boolean`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`varchar_||_char` = test_multi_type_table.`varchar` || test_multi_type_table.`char`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`varchar_||_varchar` = test_multi_type_table.`varchar` || test_multi_type_table.`varchar`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`varchar_||_binary` = test_multi_type_table.`varchar` || test_multi_type_table.`binary`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`varchar_||_varbinary` = test_multi_type_table.`varchar` || test_multi_type_table.`varbinary`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`varchar_||_text` = test_multi_type_table.`varchar` || test_multi_type_table.`text`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`binary_||_int1` = test_multi_type_table.`binary` || test_multi_type_table.`int1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`binary_||_uint1` = test_multi_type_table.`binary` || test_multi_type_table.`uint1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`binary_||_int2` = test_multi_type_table.`binary` || test_multi_type_table.`int2`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`binary_||_uint2` = test_multi_type_table.`binary` || test_multi_type_table.`uint2`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`binary_||_int4` = test_multi_type_table.`binary` || test_multi_type_table.`int4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`binary_||_uint4` = test_multi_type_table.`binary` || test_multi_type_table.`uint4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`binary_||_int8` = test_multi_type_table.`binary` || test_multi_type_table.`int8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`binary_||_uint8` = test_multi_type_table.`binary` || test_multi_type_table.`uint8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`binary_||_float4` = test_multi_type_table.`binary` || test_multi_type_table.`float4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`binary_||_float8` = test_multi_type_table.`binary` || test_multi_type_table.`float8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`binary_||_numeric` = test_multi_type_table.`binary` || test_multi_type_table.`numeric`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`binary_||_bit1` = test_multi_type_table.`binary` || test_multi_type_table.`bit1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`binary_||_bit64` = test_multi_type_table.`binary` || test_multi_type_table.`bit64`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`binary_||_boolean` = test_multi_type_table.`binary` || test_multi_type_table.`boolean`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`binary_||_char` = test_multi_type_table.`binary` || test_multi_type_table.`char`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`binary_||_varchar` = test_multi_type_table.`binary` || test_multi_type_table.`varchar`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`binary_||_binary` = test_multi_type_table.`binary` || test_multi_type_table.`binary`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`binary_||_varbinary` = test_multi_type_table.`binary` || test_multi_type_table.`varbinary`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`binary_||_text` = test_multi_type_table.`binary` || test_multi_type_table.`text`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`varbinary_||_int1` = test_multi_type_table.`varbinary` || test_multi_type_table.`int1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`varbinary_||_uint1` = test_multi_type_table.`varbinary` || test_multi_type_table.`uint1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`varbinary_||_int2` = test_multi_type_table.`varbinary` || test_multi_type_table.`int2`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`varbinary_||_uint2` = test_multi_type_table.`varbinary` || test_multi_type_table.`uint2`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`varbinary_||_int4` = test_multi_type_table.`varbinary` || test_multi_type_table.`int4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`varbinary_||_uint4` = test_multi_type_table.`varbinary` || test_multi_type_table.`uint4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`varbinary_||_int8` = test_multi_type_table.`varbinary` || test_multi_type_table.`int8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`varbinary_||_uint8` = test_multi_type_table.`varbinary` || test_multi_type_table.`uint8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`varbinary_||_float4` = test_multi_type_table.`varbinary` || test_multi_type_table.`float4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`varbinary_||_float8` = test_multi_type_table.`varbinary` || test_multi_type_table.`float8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`varbinary_||_numeric` = test_multi_type_table.`varbinary` || test_multi_type_table.`numeric`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`varbinary_||_bit1` = test_multi_type_table.`varbinary` || test_multi_type_table.`bit1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`varbinary_||_bit64` = test_multi_type_table.`varbinary` || test_multi_type_table.`bit64`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`varbinary_||_boolean` = test_multi_type_table.`varbinary` || test_multi_type_table.`boolean`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`varbinary_||_char` = test_multi_type_table.`varbinary` || test_multi_type_table.`char`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`varbinary_||_varchar` = test_multi_type_table.`varbinary` || test_multi_type_table.`varchar`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`varbinary_||_binary` = test_multi_type_table.`varbinary` || test_multi_type_table.`binary`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`varbinary_||_varbinary` = test_multi_type_table.`varbinary` || test_multi_type_table.`varbinary`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`varbinary_||_text` = test_multi_type_table.`varbinary` || test_multi_type_table.`text`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`text_||_int1` = test_multi_type_table.`text` || test_multi_type_table.`int1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`text_||_uint1` = test_multi_type_table.`text` || test_multi_type_table.`uint1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`text_||_int2` = test_multi_type_table.`text` || test_multi_type_table.`int2`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`text_||_uint2` = test_multi_type_table.`text` || test_multi_type_table.`uint2`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`text_||_int4` = test_multi_type_table.`text` || test_multi_type_table.`int4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`text_||_uint4` = test_multi_type_table.`text` || test_multi_type_table.`uint4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`text_||_int8` = test_multi_type_table.`text` || test_multi_type_table.`int8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`text_||_uint8` = test_multi_type_table.`text` || test_multi_type_table.`uint8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`text_||_float4` = test_multi_type_table.`text` || test_multi_type_table.`float4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`text_||_float8` = test_multi_type_table.`text` || test_multi_type_table.`float8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`text_||_numeric` = test_multi_type_table.`text` || test_multi_type_table.`numeric`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`text_||_bit1` = test_multi_type_table.`text` || test_multi_type_table.`bit1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`text_||_bit64` = test_multi_type_table.`text` || test_multi_type_table.`bit64`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`text_||_boolean` = test_multi_type_table.`text` || test_multi_type_table.`boolean`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`text_||_char` = test_multi_type_table.`text` || test_multi_type_table.`char`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`text_||_varchar` = test_multi_type_table.`text` || test_multi_type_table.`varchar`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`text_||_binary` = test_multi_type_table.`text` || test_multi_type_table.`binary`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`text_||_varbinary` = test_multi_type_table.`text` || test_multi_type_table.`varbinary`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`text_||_text` = test_multi_type_table.`text` || test_multi_type_table.`text`;
SELECT * FROM test_multi_type order by 1;
DROP TABLE test_multi_type;

-- strict select
set dolphin.sql_mode to 'sql_mode_strict,sql_mode_full_group,pipes_as_concat,ansi_quotes,no_zero_date';
SELECT
    `int1` || `uint1` AS `int1_||_uint1`,
    `int2` || `uint2` AS `int2_||_uint2`,
    `int4` || `uint4` AS `int4_||_uint4`,
    `int8` || `uint8` AS `int8_||_uint8`,
    `float4` || `float8` AS `float4_||_float8`,
    `numeric` || `bit1` AS `numeric_||_bit1`,
    `bit64` || `boolean` AS `bit64_||_boolean`,
    `char` || `varchar` AS `char_||_varchar`,
    `binary` || `varbinary` AS `binary_||_varbinary`,
    `text` || `text` AS `text_||_text`
FROM test_multi_type_table;

-- strict insert
CREATE TABLE test_text(a text);
CREATE TABLE test_blob(b blob);
INSERT INTO test_text SELECT `int1` || `uint1` FROM test_multi_type_table;
INSERT INTO test_text SELECT `int2` || `uint2` FROM test_multi_type_table;
INSERT INTO test_text SELECT `int4` || `uint4` FROM test_multi_type_table;
INSERT INTO test_text SELECT `int8` || `uint8` FROM test_multi_type_table;
INSERT INTO test_text SELECT `float4` || `float8` FROM test_multi_type_table;
INSERT INTO test_text SELECT `numeric` || `boolean` FROM test_multi_type_table;
INSERT INTO test_text SELECT `char` || `varchar` FROM test_multi_type_table;
INSERT INTO test_text SELECT `text` || `text` FROM test_multi_type_table;
INSERT INTO test_text SELECT 'openGauss' || `int4` FROM test_multi_type_table;
INSERT INTO test_text SELECT 'int4' || 'openGauss' FROM test_multi_type_table;
INSERT INTO test_text SELECT 'openGauss' || 'openGauss' FROM test_multi_type_table;
INSERT INTO test_blob SELECT `bit1` || `bit64` FROM test_multi_type_table;
INSERT INTO test_blob SELECT `binary` || `varbinary` FROM test_multi_type_table;

\x
-- unknown test
SELECT '1.23a' || 'a.123';
SELECT '1.23a' || `int1` FROM test_multi_type_table;
SELECT `int1` || '1.23a' FROM test_multi_type_table;
SELECT '1.23a' || `uint1` FROM test_multi_type_table;
SELECT `uint1` || '1.23a' FROM test_multi_type_table;
SELECT '1.23a' || `int2` FROM test_multi_type_table;
SELECT `int2` || '1.23a' FROM test_multi_type_table;
SELECT '1.23a' || `uint2` FROM test_multi_type_table;
SELECT `uint2` || '1.23a' FROM test_multi_type_table;
SELECT '1.23a' || `int4` FROM test_multi_type_table;
SELECT `int4` || '1.23a' FROM test_multi_type_table;
SELECT '1.23a' || `uint4` FROM test_multi_type_table;
SELECT `uint4` || '1.23a' FROM test_multi_type_table;
SELECT '1.23a' || `int8` FROM test_multi_type_table;
SELECT `int8` || '1.23a' FROM test_multi_type_table;
SELECT '1.23a' || `uint8` FROM test_multi_type_table;
SELECT `uint8` || '1.23a' FROM test_multi_type_table;
SELECT '1.23a' || `float4` FROM test_multi_type_table;
SELECT `float4` || '1.23a' FROM test_multi_type_table;
SELECT '1.23a' || `float8` FROM test_multi_type_table;
SELECT `float8` || '1.23a' FROM test_multi_type_table;
SELECT '1.23a' || `numeric` FROM test_multi_type_table;
SELECT `numeric` || '1.23a' FROM test_multi_type_table;
SELECT '1.23a' || `bit1` FROM test_multi_type_table;
SELECT `bit1` || '1.23a' FROM test_multi_type_table;
SELECT '1.23a' || `bit64` FROM test_multi_type_table;
SELECT `bit64` || '1.23a' FROM test_multi_type_table;
SELECT '1.23a' || `boolean` FROM test_multi_type_table;
SELECT `boolean` || '1.23a' FROM test_multi_type_table;
SELECT '1.23a' || `char` FROM test_multi_type_table;
SELECT `char` || '1.23a' FROM test_multi_type_table;
SELECT '1.23a' || `varchar` FROM test_multi_type_table;
SELECT `varchar` || '1.23a' FROM test_multi_type_table;
SELECT '1.23a' || `binary` FROM test_multi_type_table;
SELECT `binary` || '1.23a' FROM test_multi_type_table;
SELECT '1.23a' || `varbinary` FROM test_multi_type_table;
SELECT `varbinary` || '1.23a' FROM test_multi_type_table;
SELECT '1.23a' || `text` FROM test_multi_type_table;
SELECT `text` || '1.23a' FROM test_multi_type_table;

DROP TABLE test_text;
DROP TABLE test_blob;
DROP TABLE test_multi_type_table;

-- precedence test
SELECT 4*2 || 1+4+5;
SELECT (4*2) || 1+4+5;
SELECT 2+3+4+3 || 4*4;
SELECT 2+3+4+3 || (4*4);

---------- tail ----------
drop schema pipes_as_concat_test_schema cascade;
reset current_schema;
drop schema if exists multi_type_xor_test_schema cascade;
create schema multi_type_xor_test_schema;
set current_schema to 'multi_type_xor_test_schema';
---------- head ----------
set dolphin.b_compatibility_mode to on;
set dolphin.sql_mode to 'sql_mode_full_group,pipes_as_concat,ansi_quotes,no_zero_date';
drop table if exists test_multi_type_table;
CREATE TABLE test_multi_type_table (`int1` tinyint, `uint1` tinyint unsigned, `int2` smallint, `uint2` smallint unsigned, `int4` integer, `uint4` integer unsigned, `int8` bigint, `uint8` bigint unsigned, `float4` float4, `float8` float8, `numeric` decimal(20, 6), `bit1` bit(1), `bit64` bit(64), `boolean` boolean,  `char` char(100), `varchar` varchar(100), `binary` binary(100), `varbinary` varbinary(100), `text` text);
insert into test_multi_type_table values(0x7F, 0xFF, 0x7FFF, 0xFFFF, 0x7FFFFFFF, 0xFFFFFFFF, 0x7FFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 0.9884282, 5.288433915413254, 43391541, b'1', b'100110100110', true, '1.23a', '1.23a', '1.23a', '1.23a', '1.23a');

DROP TABLE IF EXISTS test_multi_type;
CREATE TABLE test_multi_type AS SELECT
`int1` xor `int1` AS `int1_xor_int1`,
`int1` xor `uint1` AS `int1_xor_uint1`,
`int1` xor `int2` AS `int1_xor_int2`,
`int1` xor `uint2` AS `int1_xor_uint2`,
`int1` xor `int4` AS `int1_xor_int4`,
`int1` xor `uint4` AS `int1_xor_uint4`,
`int1` xor `int8` AS `int1_xor_int8`,
`int1` xor `uint8` AS `int1_xor_uint8`,
`int1` xor `float4` AS `int1_xor_float4`,
`int1` xor `float8` AS `int1_xor_float8`,
`int1` xor `numeric` AS `int1_xor_numeric`,
`int1` xor `bit1` AS `int1_xor_bit1`,
`int1` xor `bit64` AS `int1_xor_bit64`,
`int1` xor `boolean` AS `int1_xor_boolean`,
`int1` xor `char` AS `int1_xor_char`,
`int1` xor `varchar` AS `int1_xor_varchar`,
`int1` xor `binary` AS `int1_xor_binary`,
`int1` xor `varbinary` AS `int1_xor_varbinary`,
`int1` xor `text` AS `int1_xor_text`,
`uint1` xor `int1` AS `uint1_xor_int1`,
`uint1` xor `uint1` AS `uint1_xor_uint1`,
`uint1` xor `int2` AS `uint1_xor_int2`,
`uint1` xor `uint2` AS `uint1_xor_uint2`,
`uint1` xor `int4` AS `uint1_xor_int4`,
`uint1` xor `uint4` AS `uint1_xor_uint4`,
`uint1` xor `int8` AS `uint1_xor_int8`,
`uint1` xor `uint8` AS `uint1_xor_uint8`,
`uint1` xor `float4` AS `uint1_xor_float4`,
`uint1` xor `float8` AS `uint1_xor_float8`,
`uint1` xor `numeric` AS `uint1_xor_numeric`,
`uint1` xor `bit1` AS `uint1_xor_bit1`,
`uint1` xor `bit64` AS `uint1_xor_bit64`,
`uint1` xor `boolean` AS `uint1_xor_boolean`,
`uint1` xor `char` AS `uint1_xor_char`,
`uint1` xor `varchar` AS `uint1_xor_varchar`,
`uint1` xor `binary` AS `uint1_xor_binary`,
`uint1` xor `varbinary` AS `uint1_xor_varbinary`,
`uint1` xor `text` AS `uint1_xor_text`,
`int2` xor `int1` AS `int2_xor_int1`,
`int2` xor `uint1` AS `int2_xor_uint1`,
`int2` xor `int2` AS `int2_xor_int2`,
`int2` xor `uint2` AS `int2_xor_uint2`,
`int2` xor `int4` AS `int2_xor_int4`,
`int2` xor `uint4` AS `int2_xor_uint4`,
`int2` xor `int8` AS `int2_xor_int8`,
`int2` xor `uint8` AS `int2_xor_uint8`,
`int2` xor `float4` AS `int2_xor_float4`,
`int2` xor `float8` AS `int2_xor_float8`,
`int2` xor `numeric` AS `int2_xor_numeric`,
`int2` xor `bit1` AS `int2_xor_bit1`,
`int2` xor `bit64` AS `int2_xor_bit64`,
`int2` xor `boolean` AS `int2_xor_boolean`,
`int2` xor `char` AS `int2_xor_char`,
`int2` xor `varchar` AS `int2_xor_varchar`,
`int2` xor `binary` AS `int2_xor_binary`,
`int2` xor `varbinary` AS `int2_xor_varbinary`,
`int2` xor `text` AS `int2_xor_text`,
`uint2` xor `int1` AS `uint2_xor_int1`,
`uint2` xor `uint1` AS `uint2_xor_uint1`,
`uint2` xor `int2` AS `uint2_xor_int2`,
`uint2` xor `uint2` AS `uint2_xor_uint2`,
`uint2` xor `int4` AS `uint2_xor_int4`,
`uint2` xor `uint4` AS `uint2_xor_uint4`,
`uint2` xor `int8` AS `uint2_xor_int8`,
`uint2` xor `uint8` AS `uint2_xor_uint8`,
`uint2` xor `float4` AS `uint2_xor_float4`,
`uint2` xor `float8` AS `uint2_xor_float8`,
`uint2` xor `numeric` AS `uint2_xor_numeric`,
`uint2` xor `bit1` AS `uint2_xor_bit1`,
`uint2` xor `bit64` AS `uint2_xor_bit64`,
`uint2` xor `boolean` AS `uint2_xor_boolean`,
`uint2` xor `char` AS `uint2_xor_char`,
`uint2` xor `varchar` AS `uint2_xor_varchar`,
`uint2` xor `binary` AS `uint2_xor_binary`,
`uint2` xor `varbinary` AS `uint2_xor_varbinary`,
`uint2` xor `text` AS `uint2_xor_text`,
`int4` xor `int1` AS `int4_xor_int1`,
`int4` xor `uint1` AS `int4_xor_uint1`,
`int4` xor `int2` AS `int4_xor_int2`,
`int4` xor `uint2` AS `int4_xor_uint2`,
`int4` xor `int4` AS `int4_xor_int4`,
`int4` xor `uint4` AS `int4_xor_uint4`,
`int4` xor `int8` AS `int4_xor_int8`,
`int4` xor `uint8` AS `int4_xor_uint8`,
`int4` xor `float4` AS `int4_xor_float4`,
`int4` xor `float8` AS `int4_xor_float8`,
`int4` xor `numeric` AS `int4_xor_numeric`,
`int4` xor `bit1` AS `int4_xor_bit1`,
`int4` xor `bit64` AS `int4_xor_bit64`,
`int4` xor `boolean` AS `int4_xor_boolean`,
`int4` xor `char` AS `int4_xor_char`,
`int4` xor `varchar` AS `int4_xor_varchar`,
`int4` xor `binary` AS `int4_xor_binary`,
`int4` xor `varbinary` AS `int4_xor_varbinary`,
`int4` xor `text` AS `int4_xor_text`,
`uint4` xor `int1` AS `uint4_xor_int1`,
`uint4` xor `uint1` AS `uint4_xor_uint1`,
`uint4` xor `int2` AS `uint4_xor_int2`,
`uint4` xor `uint2` AS `uint4_xor_uint2`,
`uint4` xor `int4` AS `uint4_xor_int4`,
`uint4` xor `uint4` AS `uint4_xor_uint4`,
`uint4` xor `int8` AS `uint4_xor_int8`,
`uint4` xor `uint8` AS `uint4_xor_uint8`,
`uint4` xor `float4` AS `uint4_xor_float4`,
`uint4` xor `float8` AS `uint4_xor_float8`,
`uint4` xor `numeric` AS `uint4_xor_numeric`,
`uint4` xor `bit1` AS `uint4_xor_bit1`,
`uint4` xor `bit64` AS `uint4_xor_bit64`,
`uint4` xor `boolean` AS `uint4_xor_boolean`,
`uint4` xor `char` AS `uint4_xor_char`,
`uint4` xor `varchar` AS `uint4_xor_varchar`,
`uint4` xor `binary` AS `uint4_xor_binary`,
`uint4` xor `varbinary` AS `uint4_xor_varbinary`,
`uint4` xor `text` AS `uint4_xor_text`,
`int8` xor `int1` AS `int8_xor_int1`,
`int8` xor `uint1` AS `int8_xor_uint1`,
`int8` xor `int2` AS `int8_xor_int2`,
`int8` xor `uint2` AS `int8_xor_uint2`,
`int8` xor `int4` AS `int8_xor_int4`,
`int8` xor `uint4` AS `int8_xor_uint4`,
`int8` xor `int8` AS `int8_xor_int8`,
`int8` xor `uint8` AS `int8_xor_uint8`,
`int8` xor `float4` AS `int8_xor_float4`,
`int8` xor `float8` AS `int8_xor_float8`,
`int8` xor `numeric` AS `int8_xor_numeric`,
`int8` xor `bit1` AS `int8_xor_bit1`,
`int8` xor `bit64` AS `int8_xor_bit64`,
`int8` xor `boolean` AS `int8_xor_boolean`,
`int8` xor `char` AS `int8_xor_char`,
`int8` xor `varchar` AS `int8_xor_varchar`,
`int8` xor `binary` AS `int8_xor_binary`,
`int8` xor `varbinary` AS `int8_xor_varbinary`,
`int8` xor `text` AS `int8_xor_text`,
`uint8` xor `int1` AS `uint8_xor_int1`,
`uint8` xor `uint1` AS `uint8_xor_uint1`,
`uint8` xor `int2` AS `uint8_xor_int2`,
`uint8` xor `uint2` AS `uint8_xor_uint2`,
`uint8` xor `int4` AS `uint8_xor_int4`,
`uint8` xor `uint4` AS `uint8_xor_uint4`,
`uint8` xor `int8` AS `uint8_xor_int8`,
`uint8` xor `uint8` AS `uint8_xor_uint8`,
`uint8` xor `float4` AS `uint8_xor_float4`,
`uint8` xor `float8` AS `uint8_xor_float8`,
`uint8` xor `numeric` AS `uint8_xor_numeric`,
`uint8` xor `bit1` AS `uint8_xor_bit1`,
`uint8` xor `bit64` AS `uint8_xor_bit64`,
`uint8` xor `boolean` AS `uint8_xor_boolean`,
`uint8` xor `char` AS `uint8_xor_char`,
`uint8` xor `varchar` AS `uint8_xor_varchar`,
`uint8` xor `binary` AS `uint8_xor_binary`,
`uint8` xor `varbinary` AS `uint8_xor_varbinary`,
`uint8` xor `text` AS `uint8_xor_text`,
`float4` xor `int1` AS `float4_xor_int1`,
`float4` xor `uint1` AS `float4_xor_uint1`,
`float4` xor `int2` AS `float4_xor_int2`,
`float4` xor `uint2` AS `float4_xor_uint2`,
`float4` xor `int4` AS `float4_xor_int4`,
`float4` xor `uint4` AS `float4_xor_uint4`,
`float4` xor `int8` AS `float4_xor_int8`,
`float4` xor `uint8` AS `float4_xor_uint8`,
`float4` xor `float4` AS `float4_xor_float4`,
`float4` xor `float8` AS `float4_xor_float8`,
`float4` xor `numeric` AS `float4_xor_numeric`,
`float4` xor `bit1` AS `float4_xor_bit1`,
`float4` xor `bit64` AS `float4_xor_bit64`,
`float4` xor `boolean` AS `float4_xor_boolean`,
`float4` xor `char` AS `float4_xor_char`,
`float4` xor `varchar` AS `float4_xor_varchar`,
`float4` xor `binary` AS `float4_xor_binary`,
`float4` xor `varbinary` AS `float4_xor_varbinary`,
`float4` xor `text` AS `float4_xor_text`,
`float8` xor `int1` AS `float8_xor_int1`,
`float8` xor `uint1` AS `float8_xor_uint1`,
`float8` xor `int2` AS `float8_xor_int2`,
`float8` xor `uint2` AS `float8_xor_uint2`,
`float8` xor `int4` AS `float8_xor_int4`,
`float8` xor `uint4` AS `float8_xor_uint4`,
`float8` xor `int8` AS `float8_xor_int8`,
`float8` xor `uint8` AS `float8_xor_uint8`,
`float8` xor `float4` AS `float8_xor_float4`,
`float8` xor `float8` AS `float8_xor_float8`,
`float8` xor `numeric` AS `float8_xor_numeric`,
`float8` xor `bit1` AS `float8_xor_bit1`,
`float8` xor `bit64` AS `float8_xor_bit64`,
`float8` xor `boolean` AS `float8_xor_boolean`,
`float8` xor `char` AS `float8_xor_char`,
`float8` xor `varchar` AS `float8_xor_varchar`,
`float8` xor `binary` AS `float8_xor_binary`,
`float8` xor `varbinary` AS `float8_xor_varbinary`,
`float8` xor `text` AS `float8_xor_text`,
`numeric` xor `int1` AS `numeric_xor_int1`,
`numeric` xor `uint1` AS `numeric_xor_uint1`,
`numeric` xor `int2` AS `numeric_xor_int2`,
`numeric` xor `uint2` AS `numeric_xor_uint2`,
`numeric` xor `int4` AS `numeric_xor_int4`,
`numeric` xor `uint4` AS `numeric_xor_uint4`,
`numeric` xor `int8` AS `numeric_xor_int8`,
`numeric` xor `uint8` AS `numeric_xor_uint8`,
`numeric` xor `float4` AS `numeric_xor_float4`,
`numeric` xor `float8` AS `numeric_xor_float8`,
`numeric` xor `numeric` AS `numeric_xor_numeric`,
`numeric` xor `bit1` AS `numeric_xor_bit1`,
`numeric` xor `bit64` AS `numeric_xor_bit64`,
`numeric` xor `boolean` AS `numeric_xor_boolean`,
`numeric` xor `char` AS `numeric_xor_char`,
`numeric` xor `varchar` AS `numeric_xor_varchar`,
`numeric` xor `binary` AS `numeric_xor_binary`,
`numeric` xor `varbinary` AS `numeric_xor_varbinary`,
`numeric` xor `text` AS `numeric_xor_text`,
`bit1` xor `int1` AS `bit1_xor_int1`,
`bit1` xor `uint1` AS `bit1_xor_uint1`,
`bit1` xor `int2` AS `bit1_xor_int2`,
`bit1` xor `uint2` AS `bit1_xor_uint2`,
`bit1` xor `int4` AS `bit1_xor_int4`,
`bit1` xor `uint4` AS `bit1_xor_uint4`,
`bit1` xor `int8` AS `bit1_xor_int8`,
`bit1` xor `uint8` AS `bit1_xor_uint8`,
`bit1` xor `float4` AS `bit1_xor_float4`,
`bit1` xor `float8` AS `bit1_xor_float8`,
`bit1` xor `numeric` AS `bit1_xor_numeric`,
`bit1` xor `bit1` AS `bit1_xor_bit1`,
`bit1` xor `bit64` AS `bit1_xor_bit64`,
`bit1` xor `boolean` AS `bit1_xor_boolean`,
`bit1` xor `char` AS `bit1_xor_char`,
`bit1` xor `varchar` AS `bit1_xor_varchar`,
`bit1` xor `binary` AS `bit1_xor_binary`,
`bit1` xor `varbinary` AS `bit1_xor_varbinary`,
`bit1` xor `text` AS `bit1_xor_text`,
`bit64` xor `int1` AS `bit64_xor_int1`,
`bit64` xor `uint1` AS `bit64_xor_uint1`,
`bit64` xor `int2` AS `bit64_xor_int2`,
`bit64` xor `uint2` AS `bit64_xor_uint2`,
`bit64` xor `int4` AS `bit64_xor_int4`,
`bit64` xor `uint4` AS `bit64_xor_uint4`,
`bit64` xor `int8` AS `bit64_xor_int8`,
`bit64` xor `uint8` AS `bit64_xor_uint8`,
`bit64` xor `float4` AS `bit64_xor_float4`,
`bit64` xor `float8` AS `bit64_xor_float8`,
`bit64` xor `numeric` AS `bit64_xor_numeric`,
`bit64` xor `bit1` AS `bit64_xor_bit1`,
`bit64` xor `bit64` AS `bit64_xor_bit64`,
`bit64` xor `boolean` AS `bit64_xor_boolean`,
`bit64` xor `char` AS `bit64_xor_char`,
`bit64` xor `varchar` AS `bit64_xor_varchar`,
`bit64` xor `binary` AS `bit64_xor_binary`,
`bit64` xor `varbinary` AS `bit64_xor_varbinary`,
`bit64` xor `text` AS `bit64_xor_text`,
`boolean` xor `int1` AS `boolean_xor_int1`,
`boolean` xor `uint1` AS `boolean_xor_uint1`,
`boolean` xor `int2` AS `boolean_xor_int2`,
`boolean` xor `uint2` AS `boolean_xor_uint2`,
`boolean` xor `int4` AS `boolean_xor_int4`,
`boolean` xor `uint4` AS `boolean_xor_uint4`,
`boolean` xor `int8` AS `boolean_xor_int8`,
`boolean` xor `uint8` AS `boolean_xor_uint8`,
`boolean` xor `float4` AS `boolean_xor_float4`,
`boolean` xor `float8` AS `boolean_xor_float8`,
`boolean` xor `numeric` AS `boolean_xor_numeric`,
`boolean` xor `bit1` AS `boolean_xor_bit1`,
`boolean` xor `bit64` AS `boolean_xor_bit64`,
`boolean` xor `boolean` AS `boolean_xor_boolean`,
`boolean` xor `char` AS `boolean_xor_char`,
`boolean` xor `varchar` AS `boolean_xor_varchar`,
`boolean` xor `binary` AS `boolean_xor_binary`,
`boolean` xor `varbinary` AS `boolean_xor_varbinary`,
`boolean` xor `text` AS `boolean_xor_text`,
`char` xor `int1` AS `char_xor_int1`,
`char` xor `uint1` AS `char_xor_uint1`,
`char` xor `int2` AS `char_xor_int2`,
`char` xor `uint2` AS `char_xor_uint2`,
`char` xor `int4` AS `char_xor_int4`,
`char` xor `uint4` AS `char_xor_uint4`,
`char` xor `int8` AS `char_xor_int8`,
`char` xor `uint8` AS `char_xor_uint8`,
`char` xor `float4` AS `char_xor_float4`,
`char` xor `float8` AS `char_xor_float8`,
`char` xor `numeric` AS `char_xor_numeric`,
`char` xor `bit1` AS `char_xor_bit1`,
`char` xor `bit64` AS `char_xor_bit64`,
`char` xor `boolean` AS `char_xor_boolean`,
`char` xor `char` AS `char_xor_char`,
`char` xor `varchar` AS `char_xor_varchar`,
`char` xor `binary` AS `char_xor_binary`,
`char` xor `varbinary` AS `char_xor_varbinary`,
`char` xor `text` AS `char_xor_text`,
`varchar` xor `int1` AS `varchar_xor_int1`,
`varchar` xor `uint1` AS `varchar_xor_uint1`,
`varchar` xor `int2` AS `varchar_xor_int2`,
`varchar` xor `uint2` AS `varchar_xor_uint2`,
`varchar` xor `int4` AS `varchar_xor_int4`,
`varchar` xor `uint4` AS `varchar_xor_uint4`,
`varchar` xor `int8` AS `varchar_xor_int8`,
`varchar` xor `uint8` AS `varchar_xor_uint8`,
`varchar` xor `float4` AS `varchar_xor_float4`,
`varchar` xor `float8` AS `varchar_xor_float8`,
`varchar` xor `numeric` AS `varchar_xor_numeric`,
`varchar` xor `bit1` AS `varchar_xor_bit1`,
`varchar` xor `bit64` AS `varchar_xor_bit64`,
`varchar` xor `boolean` AS `varchar_xor_boolean`,
`varchar` xor `char` AS `varchar_xor_char`,
`varchar` xor `varchar` AS `varchar_xor_varchar`,
`varchar` xor `binary` AS `varchar_xor_binary`,
`varchar` xor `varbinary` AS `varchar_xor_varbinary`,
`varchar` xor `text` AS `varchar_xor_text`,
`binary` xor `int1` AS `binary_xor_int1`,
`binary` xor `uint1` AS `binary_xor_uint1`,
`binary` xor `int2` AS `binary_xor_int2`,
`binary` xor `uint2` AS `binary_xor_uint2`,
`binary` xor `int4` AS `binary_xor_int4`,
`binary` xor `uint4` AS `binary_xor_uint4`,
`binary` xor `int8` AS `binary_xor_int8`,
`binary` xor `uint8` AS `binary_xor_uint8`,
`binary` xor `float4` AS `binary_xor_float4`,
`binary` xor `float8` AS `binary_xor_float8`,
`binary` xor `numeric` AS `binary_xor_numeric`,
`binary` xor `bit1` AS `binary_xor_bit1`,
`binary` xor `bit64` AS `binary_xor_bit64`,
`binary` xor `boolean` AS `binary_xor_boolean`,
`binary` xor `char` AS `binary_xor_char`,
`binary` xor `varchar` AS `binary_xor_varchar`,
`binary` xor `binary` AS `binary_xor_binary`,
`binary` xor `varbinary` AS `binary_xor_varbinary`,
`binary` xor `text` AS `binary_xor_text`,
`varbinary` xor `int1` AS `varbinary_xor_int1`,
`varbinary` xor `uint1` AS `varbinary_xor_uint1`,
`varbinary` xor `int2` AS `varbinary_xor_int2`,
`varbinary` xor `uint2` AS `varbinary_xor_uint2`,
`varbinary` xor `int4` AS `varbinary_xor_int4`,
`varbinary` xor `uint4` AS `varbinary_xor_uint4`,
`varbinary` xor `int8` AS `varbinary_xor_int8`,
`varbinary` xor `uint8` AS `varbinary_xor_uint8`,
`varbinary` xor `float4` AS `varbinary_xor_float4`,
`varbinary` xor `float8` AS `varbinary_xor_float8`,
`varbinary` xor `numeric` AS `varbinary_xor_numeric`,
`varbinary` xor `bit1` AS `varbinary_xor_bit1`,
`varbinary` xor `bit64` AS `varbinary_xor_bit64`,
`varbinary` xor `boolean` AS `varbinary_xor_boolean`,
`varbinary` xor `char` AS `varbinary_xor_char`,
`varbinary` xor `varchar` AS `varbinary_xor_varchar`,
`varbinary` xor `binary` AS `varbinary_xor_binary`,
`varbinary` xor `varbinary` AS `varbinary_xor_varbinary`,
`varbinary` xor `text` AS `varbinary_xor_text`,
`text` xor `int1` AS `text_xor_int1`,
`text` xor `uint1` AS `text_xor_uint1`,
`text` xor `int2` AS `text_xor_int2`,
`text` xor `uint2` AS `text_xor_uint2`,
`text` xor `int4` AS `text_xor_int4`,
`text` xor `uint4` AS `text_xor_uint4`,
`text` xor `int8` AS `text_xor_int8`,
`text` xor `uint8` AS `text_xor_uint8`,
`text` xor `float4` AS `text_xor_float4`,
`text` xor `float8` AS `text_xor_float8`,
`text` xor `numeric` AS `text_xor_numeric`,
`text` xor `bit1` AS `text_xor_bit1`,
`text` xor `bit64` AS `text_xor_bit64`,
`text` xor `boolean` AS `text_xor_boolean`,
`text` xor `char` AS `text_xor_char`,
`text` xor `varchar` AS `text_xor_varchar`,
`text` xor `binary` AS `text_xor_binary`,
`text` xor `varbinary` AS `text_xor_varbinary`,
`text` xor `text` AS `text_xor_text`
FROM test_multi_type_table;
SHOW COLUMNS FROM test_multi_type;
-- test value
delete from test_multi_type;
delete from test_multi_type_table;
insert into test_multi_type values(null);
insert into test_multi_type_table values(0x7F, 0xFF, 0x7FFF, 0xFFFF, 0x7FFFFFFF, 0xFFFFFFFF, 0x7FFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 0.9884282, 5.288433915413254, 43391541, b'1', b'100110100110', true, '1.23a', '1.23a', '1.23a', '1.23a', '1.23a');

UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int1_xor_int1` = test_multi_type_table.`int1` xor test_multi_type_table.`int1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int1_xor_uint1` = test_multi_type_table.`int1` xor test_multi_type_table.`uint1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int1_xor_int2` = test_multi_type_table.`int1` xor test_multi_type_table.`int2`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int1_xor_uint2` = test_multi_type_table.`int1` xor test_multi_type_table.`uint2`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int1_xor_int4` = test_multi_type_table.`int1` xor test_multi_type_table.`int4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int1_xor_uint4` = test_multi_type_table.`int1` xor test_multi_type_table.`uint4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int1_xor_int8` = test_multi_type_table.`int1` xor test_multi_type_table.`int8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int1_xor_uint8` = test_multi_type_table.`int1` xor test_multi_type_table.`uint8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int1_xor_float4` = test_multi_type_table.`int1` xor test_multi_type_table.`float4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int1_xor_float8` = test_multi_type_table.`int1` xor test_multi_type_table.`float8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int1_xor_numeric` = test_multi_type_table.`int1` xor test_multi_type_table.`numeric`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int1_xor_bit1` = test_multi_type_table.`int1` xor test_multi_type_table.`bit1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int1_xor_bit64` = test_multi_type_table.`int1` xor test_multi_type_table.`bit64`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int1_xor_boolean` = test_multi_type_table.`int1` xor test_multi_type_table.`boolean`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int1_xor_char` = test_multi_type_table.`int1` xor test_multi_type_table.`char`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int1_xor_varchar` = test_multi_type_table.`int1` xor test_multi_type_table.`varchar`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int1_xor_binary` = test_multi_type_table.`int1` xor test_multi_type_table.`binary`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int1_xor_varbinary` = test_multi_type_table.`int1` xor test_multi_type_table.`varbinary`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int1_xor_text` = test_multi_type_table.`int1` xor test_multi_type_table.`text`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint1_xor_int1` = test_multi_type_table.`uint1` xor test_multi_type_table.`int1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint1_xor_uint1` = test_multi_type_table.`uint1` xor test_multi_type_table.`uint1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint1_xor_int2` = test_multi_type_table.`uint1` xor test_multi_type_table.`int2`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint1_xor_uint2` = test_multi_type_table.`uint1` xor test_multi_type_table.`uint2`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint1_xor_int4` = test_multi_type_table.`uint1` xor test_multi_type_table.`int4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint1_xor_uint4` = test_multi_type_table.`uint1` xor test_multi_type_table.`uint4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint1_xor_int8` = test_multi_type_table.`uint1` xor test_multi_type_table.`int8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint1_xor_uint8` = test_multi_type_table.`uint1` xor test_multi_type_table.`uint8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint1_xor_float4` = test_multi_type_table.`uint1` xor test_multi_type_table.`float4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint1_xor_float8` = test_multi_type_table.`uint1` xor test_multi_type_table.`float8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint1_xor_numeric` = test_multi_type_table.`uint1` xor test_multi_type_table.`numeric`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint1_xor_bit1` = test_multi_type_table.`uint1` xor test_multi_type_table.`bit1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint1_xor_bit64` = test_multi_type_table.`uint1` xor test_multi_type_table.`bit64`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint1_xor_boolean` = test_multi_type_table.`uint1` xor test_multi_type_table.`boolean`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint1_xor_char` = test_multi_type_table.`uint1` xor test_multi_type_table.`char`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint1_xor_varchar` = test_multi_type_table.`uint1` xor test_multi_type_table.`varchar`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint1_xor_binary` = test_multi_type_table.`uint1` xor test_multi_type_table.`binary`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint1_xor_varbinary` = test_multi_type_table.`uint1` xor test_multi_type_table.`varbinary`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint1_xor_text` = test_multi_type_table.`uint1` xor test_multi_type_table.`text`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int2_xor_int1` = test_multi_type_table.`int2` xor test_multi_type_table.`int1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int2_xor_uint1` = test_multi_type_table.`int2` xor test_multi_type_table.`uint1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int2_xor_int2` = test_multi_type_table.`int2` xor test_multi_type_table.`int2`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int2_xor_uint2` = test_multi_type_table.`int2` xor test_multi_type_table.`uint2`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int2_xor_int4` = test_multi_type_table.`int2` xor test_multi_type_table.`int4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int2_xor_uint4` = test_multi_type_table.`int2` xor test_multi_type_table.`uint4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int2_xor_int8` = test_multi_type_table.`int2` xor test_multi_type_table.`int8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int2_xor_uint8` = test_multi_type_table.`int2` xor test_multi_type_table.`uint8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int2_xor_float4` = test_multi_type_table.`int2` xor test_multi_type_table.`float4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int2_xor_float8` = test_multi_type_table.`int2` xor test_multi_type_table.`float8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int2_xor_numeric` = test_multi_type_table.`int2` xor test_multi_type_table.`numeric`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int2_xor_bit1` = test_multi_type_table.`int2` xor test_multi_type_table.`bit1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int2_xor_bit64` = test_multi_type_table.`int2` xor test_multi_type_table.`bit64`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int2_xor_boolean` = test_multi_type_table.`int2` xor test_multi_type_table.`boolean`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int2_xor_char` = test_multi_type_table.`int2` xor test_multi_type_table.`char`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int2_xor_varchar` = test_multi_type_table.`int2` xor test_multi_type_table.`varchar`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int2_xor_binary` = test_multi_type_table.`int2` xor test_multi_type_table.`binary`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int2_xor_varbinary` = test_multi_type_table.`int2` xor test_multi_type_table.`varbinary`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int2_xor_text` = test_multi_type_table.`int2` xor test_multi_type_table.`text`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint2_xor_int1` = test_multi_type_table.`uint2` xor test_multi_type_table.`int1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint2_xor_uint1` = test_multi_type_table.`uint2` xor test_multi_type_table.`uint1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint2_xor_int2` = test_multi_type_table.`uint2` xor test_multi_type_table.`int2`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint2_xor_uint2` = test_multi_type_table.`uint2` xor test_multi_type_table.`uint2`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint2_xor_int4` = test_multi_type_table.`uint2` xor test_multi_type_table.`int4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint2_xor_uint4` = test_multi_type_table.`uint2` xor test_multi_type_table.`uint4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint2_xor_int8` = test_multi_type_table.`uint2` xor test_multi_type_table.`int8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint2_xor_uint8` = test_multi_type_table.`uint2` xor test_multi_type_table.`uint8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint2_xor_float4` = test_multi_type_table.`uint2` xor test_multi_type_table.`float4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint2_xor_float8` = test_multi_type_table.`uint2` xor test_multi_type_table.`float8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint2_xor_numeric` = test_multi_type_table.`uint2` xor test_multi_type_table.`numeric`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint2_xor_bit1` = test_multi_type_table.`uint2` xor test_multi_type_table.`bit1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint2_xor_bit64` = test_multi_type_table.`uint2` xor test_multi_type_table.`bit64`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint2_xor_boolean` = test_multi_type_table.`uint2` xor test_multi_type_table.`boolean`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint2_xor_char` = test_multi_type_table.`uint2` xor test_multi_type_table.`char`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint2_xor_varchar` = test_multi_type_table.`uint2` xor test_multi_type_table.`varchar`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint2_xor_binary` = test_multi_type_table.`uint2` xor test_multi_type_table.`binary`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint2_xor_varbinary` = test_multi_type_table.`uint2` xor test_multi_type_table.`varbinary`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint2_xor_text` = test_multi_type_table.`uint2` xor test_multi_type_table.`text`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int4_xor_int1` = test_multi_type_table.`int4` xor test_multi_type_table.`int1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int4_xor_uint1` = test_multi_type_table.`int4` xor test_multi_type_table.`uint1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int4_xor_int2` = test_multi_type_table.`int4` xor test_multi_type_table.`int2`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int4_xor_uint2` = test_multi_type_table.`int4` xor test_multi_type_table.`uint2`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int4_xor_int4` = test_multi_type_table.`int4` xor test_multi_type_table.`int4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int4_xor_uint4` = test_multi_type_table.`int4` xor test_multi_type_table.`uint4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int4_xor_int8` = test_multi_type_table.`int4` xor test_multi_type_table.`int8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int4_xor_uint8` = test_multi_type_table.`int4` xor test_multi_type_table.`uint8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int4_xor_float4` = test_multi_type_table.`int4` xor test_multi_type_table.`float4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int4_xor_float8` = test_multi_type_table.`int4` xor test_multi_type_table.`float8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int4_xor_numeric` = test_multi_type_table.`int4` xor test_multi_type_table.`numeric`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int4_xor_bit1` = test_multi_type_table.`int4` xor test_multi_type_table.`bit1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int4_xor_bit64` = test_multi_type_table.`int4` xor test_multi_type_table.`bit64`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int4_xor_boolean` = test_multi_type_table.`int4` xor test_multi_type_table.`boolean`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int4_xor_char` = test_multi_type_table.`int4` xor test_multi_type_table.`char`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int4_xor_varchar` = test_multi_type_table.`int4` xor test_multi_type_table.`varchar`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int4_xor_binary` = test_multi_type_table.`int4` xor test_multi_type_table.`binary`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int4_xor_varbinary` = test_multi_type_table.`int4` xor test_multi_type_table.`varbinary`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int4_xor_text` = test_multi_type_table.`int4` xor test_multi_type_table.`text`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint4_xor_int1` = test_multi_type_table.`uint4` xor test_multi_type_table.`int1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint4_xor_uint1` = test_multi_type_table.`uint4` xor test_multi_type_table.`uint1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint4_xor_int2` = test_multi_type_table.`uint4` xor test_multi_type_table.`int2`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint4_xor_uint2` = test_multi_type_table.`uint4` xor test_multi_type_table.`uint2`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint4_xor_int4` = test_multi_type_table.`uint4` xor test_multi_type_table.`int4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint4_xor_uint4` = test_multi_type_table.`uint4` xor test_multi_type_table.`uint4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint4_xor_int8` = test_multi_type_table.`uint4` xor test_multi_type_table.`int8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint4_xor_uint8` = test_multi_type_table.`uint4` xor test_multi_type_table.`uint8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint4_xor_float4` = test_multi_type_table.`uint4` xor test_multi_type_table.`float4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint4_xor_float8` = test_multi_type_table.`uint4` xor test_multi_type_table.`float8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint4_xor_numeric` = test_multi_type_table.`uint4` xor test_multi_type_table.`numeric`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint4_xor_bit1` = test_multi_type_table.`uint4` xor test_multi_type_table.`bit1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint4_xor_bit64` = test_multi_type_table.`uint4` xor test_multi_type_table.`bit64`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint4_xor_boolean` = test_multi_type_table.`uint4` xor test_multi_type_table.`boolean`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint4_xor_char` = test_multi_type_table.`uint4` xor test_multi_type_table.`char`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint4_xor_varchar` = test_multi_type_table.`uint4` xor test_multi_type_table.`varchar`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint4_xor_binary` = test_multi_type_table.`uint4` xor test_multi_type_table.`binary`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint4_xor_varbinary` = test_multi_type_table.`uint4` xor test_multi_type_table.`varbinary`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint4_xor_text` = test_multi_type_table.`uint4` xor test_multi_type_table.`text`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int8_xor_int1` = test_multi_type_table.`int8` xor test_multi_type_table.`int1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int8_xor_uint1` = test_multi_type_table.`int8` xor test_multi_type_table.`uint1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int8_xor_int2` = test_multi_type_table.`int8` xor test_multi_type_table.`int2`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int8_xor_uint2` = test_multi_type_table.`int8` xor test_multi_type_table.`uint2`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int8_xor_int4` = test_multi_type_table.`int8` xor test_multi_type_table.`int4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int8_xor_uint4` = test_multi_type_table.`int8` xor test_multi_type_table.`uint4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int8_xor_int8` = test_multi_type_table.`int8` xor test_multi_type_table.`int8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int8_xor_uint8` = test_multi_type_table.`int8` xor test_multi_type_table.`uint8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int8_xor_float4` = test_multi_type_table.`int8` xor test_multi_type_table.`float4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int8_xor_float8` = test_multi_type_table.`int8` xor test_multi_type_table.`float8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int8_xor_numeric` = test_multi_type_table.`int8` xor test_multi_type_table.`numeric`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int8_xor_bit1` = test_multi_type_table.`int8` xor test_multi_type_table.`bit1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int8_xor_bit64` = test_multi_type_table.`int8` xor test_multi_type_table.`bit64`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int8_xor_boolean` = test_multi_type_table.`int8` xor test_multi_type_table.`boolean`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int8_xor_char` = test_multi_type_table.`int8` xor test_multi_type_table.`char`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int8_xor_varchar` = test_multi_type_table.`int8` xor test_multi_type_table.`varchar`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int8_xor_binary` = test_multi_type_table.`int8` xor test_multi_type_table.`binary`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int8_xor_varbinary` = test_multi_type_table.`int8` xor test_multi_type_table.`varbinary`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`int8_xor_text` = test_multi_type_table.`int8` xor test_multi_type_table.`text`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint8_xor_int1` = test_multi_type_table.`uint8` xor test_multi_type_table.`int1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint8_xor_uint1` = test_multi_type_table.`uint8` xor test_multi_type_table.`uint1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint8_xor_int2` = test_multi_type_table.`uint8` xor test_multi_type_table.`int2`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint8_xor_uint2` = test_multi_type_table.`uint8` xor test_multi_type_table.`uint2`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint8_xor_int4` = test_multi_type_table.`uint8` xor test_multi_type_table.`int4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint8_xor_uint4` = test_multi_type_table.`uint8` xor test_multi_type_table.`uint4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint8_xor_int8` = test_multi_type_table.`uint8` xor test_multi_type_table.`int8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint8_xor_uint8` = test_multi_type_table.`uint8` xor test_multi_type_table.`uint8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint8_xor_float4` = test_multi_type_table.`uint8` xor test_multi_type_table.`float4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint8_xor_float8` = test_multi_type_table.`uint8` xor test_multi_type_table.`float8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint8_xor_numeric` = test_multi_type_table.`uint8` xor test_multi_type_table.`numeric`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint8_xor_bit1` = test_multi_type_table.`uint8` xor test_multi_type_table.`bit1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint8_xor_bit64` = test_multi_type_table.`uint8` xor test_multi_type_table.`bit64`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint8_xor_boolean` = test_multi_type_table.`uint8` xor test_multi_type_table.`boolean`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint8_xor_char` = test_multi_type_table.`uint8` xor test_multi_type_table.`char`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint8_xor_varchar` = test_multi_type_table.`uint8` xor test_multi_type_table.`varchar`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint8_xor_binary` = test_multi_type_table.`uint8` xor test_multi_type_table.`binary`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint8_xor_varbinary` = test_multi_type_table.`uint8` xor test_multi_type_table.`varbinary`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`uint8_xor_text` = test_multi_type_table.`uint8` xor test_multi_type_table.`text`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`float4_xor_int1` = test_multi_type_table.`float4` xor test_multi_type_table.`int1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`float4_xor_uint1` = test_multi_type_table.`float4` xor test_multi_type_table.`uint1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`float4_xor_int2` = test_multi_type_table.`float4` xor test_multi_type_table.`int2`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`float4_xor_uint2` = test_multi_type_table.`float4` xor test_multi_type_table.`uint2`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`float4_xor_int4` = test_multi_type_table.`float4` xor test_multi_type_table.`int4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`float4_xor_uint4` = test_multi_type_table.`float4` xor test_multi_type_table.`uint4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`float4_xor_int8` = test_multi_type_table.`float4` xor test_multi_type_table.`int8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`float4_xor_uint8` = test_multi_type_table.`float4` xor test_multi_type_table.`uint8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`float4_xor_float4` = test_multi_type_table.`float4` xor test_multi_type_table.`float4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`float4_xor_float8` = test_multi_type_table.`float4` xor test_multi_type_table.`float8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`float4_xor_numeric` = test_multi_type_table.`float4` xor test_multi_type_table.`numeric`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`float4_xor_bit1` = test_multi_type_table.`float4` xor test_multi_type_table.`bit1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`float4_xor_bit64` = test_multi_type_table.`float4` xor test_multi_type_table.`bit64`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`float4_xor_boolean` = test_multi_type_table.`float4` xor test_multi_type_table.`boolean`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`float4_xor_char` = test_multi_type_table.`float4` xor test_multi_type_table.`char`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`float4_xor_varchar` = test_multi_type_table.`float4` xor test_multi_type_table.`varchar`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`float4_xor_binary` = test_multi_type_table.`float4` xor test_multi_type_table.`binary`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`float4_xor_varbinary` = test_multi_type_table.`float4` xor test_multi_type_table.`varbinary`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`float4_xor_text` = test_multi_type_table.`float4` xor test_multi_type_table.`text`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`float8_xor_int1` = test_multi_type_table.`float8` xor test_multi_type_table.`int1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`float8_xor_uint1` = test_multi_type_table.`float8` xor test_multi_type_table.`uint1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`float8_xor_int2` = test_multi_type_table.`float8` xor test_multi_type_table.`int2`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`float8_xor_uint2` = test_multi_type_table.`float8` xor test_multi_type_table.`uint2`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`float8_xor_int4` = test_multi_type_table.`float8` xor test_multi_type_table.`int4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`float8_xor_uint4` = test_multi_type_table.`float8` xor test_multi_type_table.`uint4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`float8_xor_int8` = test_multi_type_table.`float8` xor test_multi_type_table.`int8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`float8_xor_uint8` = test_multi_type_table.`float8` xor test_multi_type_table.`uint8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`float8_xor_float4` = test_multi_type_table.`float8` xor test_multi_type_table.`float4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`float8_xor_float8` = test_multi_type_table.`float8` xor test_multi_type_table.`float8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`float8_xor_numeric` = test_multi_type_table.`float8` xor test_multi_type_table.`numeric`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`float8_xor_bit1` = test_multi_type_table.`float8` xor test_multi_type_table.`bit1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`float8_xor_bit64` = test_multi_type_table.`float8` xor test_multi_type_table.`bit64`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`float8_xor_boolean` = test_multi_type_table.`float8` xor test_multi_type_table.`boolean`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`float8_xor_char` = test_multi_type_table.`float8` xor test_multi_type_table.`char`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`float8_xor_varchar` = test_multi_type_table.`float8` xor test_multi_type_table.`varchar`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`float8_xor_binary` = test_multi_type_table.`float8` xor test_multi_type_table.`binary`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`float8_xor_varbinary` = test_multi_type_table.`float8` xor test_multi_type_table.`varbinary`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`float8_xor_text` = test_multi_type_table.`float8` xor test_multi_type_table.`text`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`numeric_xor_int1` = test_multi_type_table.`numeric` xor test_multi_type_table.`int1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`numeric_xor_uint1` = test_multi_type_table.`numeric` xor test_multi_type_table.`uint1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`numeric_xor_int2` = test_multi_type_table.`numeric` xor test_multi_type_table.`int2`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`numeric_xor_uint2` = test_multi_type_table.`numeric` xor test_multi_type_table.`uint2`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`numeric_xor_int4` = test_multi_type_table.`numeric` xor test_multi_type_table.`int4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`numeric_xor_uint4` = test_multi_type_table.`numeric` xor test_multi_type_table.`uint4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`numeric_xor_int8` = test_multi_type_table.`numeric` xor test_multi_type_table.`int8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`numeric_xor_uint8` = test_multi_type_table.`numeric` xor test_multi_type_table.`uint8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`numeric_xor_float4` = test_multi_type_table.`numeric` xor test_multi_type_table.`float4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`numeric_xor_float8` = test_multi_type_table.`numeric` xor test_multi_type_table.`float8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`numeric_xor_numeric` = test_multi_type_table.`numeric` xor test_multi_type_table.`numeric`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`numeric_xor_bit1` = test_multi_type_table.`numeric` xor test_multi_type_table.`bit1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`numeric_xor_bit64` = test_multi_type_table.`numeric` xor test_multi_type_table.`bit64`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`numeric_xor_boolean` = test_multi_type_table.`numeric` xor test_multi_type_table.`boolean`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`numeric_xor_char` = test_multi_type_table.`numeric` xor test_multi_type_table.`char`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`numeric_xor_varchar` = test_multi_type_table.`numeric` xor test_multi_type_table.`varchar`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`numeric_xor_binary` = test_multi_type_table.`numeric` xor test_multi_type_table.`binary`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`numeric_xor_varbinary` = test_multi_type_table.`numeric` xor test_multi_type_table.`varbinary`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`numeric_xor_text` = test_multi_type_table.`numeric` xor test_multi_type_table.`text`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`bit1_xor_int1` = test_multi_type_table.`bit1` xor test_multi_type_table.`int1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`bit1_xor_uint1` = test_multi_type_table.`bit1` xor test_multi_type_table.`uint1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`bit1_xor_int2` = test_multi_type_table.`bit1` xor test_multi_type_table.`int2`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`bit1_xor_uint2` = test_multi_type_table.`bit1` xor test_multi_type_table.`uint2`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`bit1_xor_int4` = test_multi_type_table.`bit1` xor test_multi_type_table.`int4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`bit1_xor_uint4` = test_multi_type_table.`bit1` xor test_multi_type_table.`uint4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`bit1_xor_int8` = test_multi_type_table.`bit1` xor test_multi_type_table.`int8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`bit1_xor_uint8` = test_multi_type_table.`bit1` xor test_multi_type_table.`uint8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`bit1_xor_float4` = test_multi_type_table.`bit1` xor test_multi_type_table.`float4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`bit1_xor_float8` = test_multi_type_table.`bit1` xor test_multi_type_table.`float8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`bit1_xor_numeric` = test_multi_type_table.`bit1` xor test_multi_type_table.`numeric`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`bit1_xor_bit1` = test_multi_type_table.`bit1` xor test_multi_type_table.`bit1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`bit1_xor_bit64` = test_multi_type_table.`bit1` xor test_multi_type_table.`bit64`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`bit1_xor_boolean` = test_multi_type_table.`bit1` xor test_multi_type_table.`boolean`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`bit1_xor_char` = test_multi_type_table.`bit1` xor test_multi_type_table.`char`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`bit1_xor_varchar` = test_multi_type_table.`bit1` xor test_multi_type_table.`varchar`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`bit1_xor_binary` = test_multi_type_table.`bit1` xor test_multi_type_table.`binary`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`bit1_xor_varbinary` = test_multi_type_table.`bit1` xor test_multi_type_table.`varbinary`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`bit1_xor_text` = test_multi_type_table.`bit1` xor test_multi_type_table.`text`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`bit64_xor_int1` = test_multi_type_table.`bit64` xor test_multi_type_table.`int1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`bit64_xor_uint1` = test_multi_type_table.`bit64` xor test_multi_type_table.`uint1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`bit64_xor_int2` = test_multi_type_table.`bit64` xor test_multi_type_table.`int2`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`bit64_xor_uint2` = test_multi_type_table.`bit64` xor test_multi_type_table.`uint2`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`bit64_xor_int4` = test_multi_type_table.`bit64` xor test_multi_type_table.`int4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`bit64_xor_uint4` = test_multi_type_table.`bit64` xor test_multi_type_table.`uint4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`bit64_xor_int8` = test_multi_type_table.`bit64` xor test_multi_type_table.`int8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`bit64_xor_uint8` = test_multi_type_table.`bit64` xor test_multi_type_table.`uint8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`bit64_xor_float4` = test_multi_type_table.`bit64` xor test_multi_type_table.`float4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`bit64_xor_float8` = test_multi_type_table.`bit64` xor test_multi_type_table.`float8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`bit64_xor_numeric` = test_multi_type_table.`bit64` xor test_multi_type_table.`numeric`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`bit64_xor_bit1` = test_multi_type_table.`bit64` xor test_multi_type_table.`bit1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`bit64_xor_bit64` = test_multi_type_table.`bit64` xor test_multi_type_table.`bit64`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`bit64_xor_boolean` = test_multi_type_table.`bit64` xor test_multi_type_table.`boolean`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`bit64_xor_char` = test_multi_type_table.`bit64` xor test_multi_type_table.`char`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`bit64_xor_varchar` = test_multi_type_table.`bit64` xor test_multi_type_table.`varchar`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`bit64_xor_binary` = test_multi_type_table.`bit64` xor test_multi_type_table.`binary`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`bit64_xor_varbinary` = test_multi_type_table.`bit64` xor test_multi_type_table.`varbinary`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`bit64_xor_text` = test_multi_type_table.`bit64` xor test_multi_type_table.`text`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`boolean_xor_int1` = test_multi_type_table.`boolean` xor test_multi_type_table.`int1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`boolean_xor_uint1` = test_multi_type_table.`boolean` xor test_multi_type_table.`uint1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`boolean_xor_int2` = test_multi_type_table.`boolean` xor test_multi_type_table.`int2`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`boolean_xor_uint2` = test_multi_type_table.`boolean` xor test_multi_type_table.`uint2`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`boolean_xor_int4` = test_multi_type_table.`boolean` xor test_multi_type_table.`int4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`boolean_xor_uint4` = test_multi_type_table.`boolean` xor test_multi_type_table.`uint4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`boolean_xor_int8` = test_multi_type_table.`boolean` xor test_multi_type_table.`int8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`boolean_xor_uint8` = test_multi_type_table.`boolean` xor test_multi_type_table.`uint8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`boolean_xor_float4` = test_multi_type_table.`boolean` xor test_multi_type_table.`float4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`boolean_xor_float8` = test_multi_type_table.`boolean` xor test_multi_type_table.`float8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`boolean_xor_numeric` = test_multi_type_table.`boolean` xor test_multi_type_table.`numeric`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`boolean_xor_bit1` = test_multi_type_table.`boolean` xor test_multi_type_table.`bit1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`boolean_xor_bit64` = test_multi_type_table.`boolean` xor test_multi_type_table.`bit64`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`boolean_xor_boolean` = test_multi_type_table.`boolean` xor test_multi_type_table.`boolean`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`boolean_xor_char` = test_multi_type_table.`boolean` xor test_multi_type_table.`char`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`boolean_xor_varchar` = test_multi_type_table.`boolean` xor test_multi_type_table.`varchar`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`boolean_xor_binary` = test_multi_type_table.`boolean` xor test_multi_type_table.`binary`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`boolean_xor_varbinary` = test_multi_type_table.`boolean` xor test_multi_type_table.`varbinary`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`boolean_xor_text` = test_multi_type_table.`boolean` xor test_multi_type_table.`text`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`char_xor_int1` = test_multi_type_table.`char` xor test_multi_type_table.`int1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`char_xor_uint1` = test_multi_type_table.`char` xor test_multi_type_table.`uint1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`char_xor_int2` = test_multi_type_table.`char` xor test_multi_type_table.`int2`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`char_xor_uint2` = test_multi_type_table.`char` xor test_multi_type_table.`uint2`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`char_xor_int4` = test_multi_type_table.`char` xor test_multi_type_table.`int4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`char_xor_uint4` = test_multi_type_table.`char` xor test_multi_type_table.`uint4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`char_xor_int8` = test_multi_type_table.`char` xor test_multi_type_table.`int8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`char_xor_uint8` = test_multi_type_table.`char` xor test_multi_type_table.`uint8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`char_xor_float4` = test_multi_type_table.`char` xor test_multi_type_table.`float4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`char_xor_float8` = test_multi_type_table.`char` xor test_multi_type_table.`float8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`char_xor_numeric` = test_multi_type_table.`char` xor test_multi_type_table.`numeric`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`char_xor_bit1` = test_multi_type_table.`char` xor test_multi_type_table.`bit1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`char_xor_bit64` = test_multi_type_table.`char` xor test_multi_type_table.`bit64`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`char_xor_boolean` = test_multi_type_table.`char` xor test_multi_type_table.`boolean`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`char_xor_char` = test_multi_type_table.`char` xor test_multi_type_table.`char`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`char_xor_varchar` = test_multi_type_table.`char` xor test_multi_type_table.`varchar`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`char_xor_binary` = test_multi_type_table.`char` xor test_multi_type_table.`binary`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`char_xor_varbinary` = test_multi_type_table.`char` xor test_multi_type_table.`varbinary`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`char_xor_text` = test_multi_type_table.`char` xor test_multi_type_table.`text`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`varchar_xor_int1` = test_multi_type_table.`varchar` xor test_multi_type_table.`int1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`varchar_xor_uint1` = test_multi_type_table.`varchar` xor test_multi_type_table.`uint1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`varchar_xor_int2` = test_multi_type_table.`varchar` xor test_multi_type_table.`int2`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`varchar_xor_uint2` = test_multi_type_table.`varchar` xor test_multi_type_table.`uint2`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`varchar_xor_int4` = test_multi_type_table.`varchar` xor test_multi_type_table.`int4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`varchar_xor_uint4` = test_multi_type_table.`varchar` xor test_multi_type_table.`uint4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`varchar_xor_int8` = test_multi_type_table.`varchar` xor test_multi_type_table.`int8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`varchar_xor_uint8` = test_multi_type_table.`varchar` xor test_multi_type_table.`uint8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`varchar_xor_float4` = test_multi_type_table.`varchar` xor test_multi_type_table.`float4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`varchar_xor_float8` = test_multi_type_table.`varchar` xor test_multi_type_table.`float8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`varchar_xor_numeric` = test_multi_type_table.`varchar` xor test_multi_type_table.`numeric`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`varchar_xor_bit1` = test_multi_type_table.`varchar` xor test_multi_type_table.`bit1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`varchar_xor_bit64` = test_multi_type_table.`varchar` xor test_multi_type_table.`bit64`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`varchar_xor_boolean` = test_multi_type_table.`varchar` xor test_multi_type_table.`boolean`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`varchar_xor_char` = test_multi_type_table.`varchar` xor test_multi_type_table.`char`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`varchar_xor_varchar` = test_multi_type_table.`varchar` xor test_multi_type_table.`varchar`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`varchar_xor_binary` = test_multi_type_table.`varchar` xor test_multi_type_table.`binary`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`varchar_xor_varbinary` = test_multi_type_table.`varchar` xor test_multi_type_table.`varbinary`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`varchar_xor_text` = test_multi_type_table.`varchar` xor test_multi_type_table.`text`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`binary_xor_int1` = test_multi_type_table.`binary` xor test_multi_type_table.`int1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`binary_xor_uint1` = test_multi_type_table.`binary` xor test_multi_type_table.`uint1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`binary_xor_int2` = test_multi_type_table.`binary` xor test_multi_type_table.`int2`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`binary_xor_uint2` = test_multi_type_table.`binary` xor test_multi_type_table.`uint2`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`binary_xor_int4` = test_multi_type_table.`binary` xor test_multi_type_table.`int4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`binary_xor_uint4` = test_multi_type_table.`binary` xor test_multi_type_table.`uint4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`binary_xor_int8` = test_multi_type_table.`binary` xor test_multi_type_table.`int8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`binary_xor_uint8` = test_multi_type_table.`binary` xor test_multi_type_table.`uint8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`binary_xor_float4` = test_multi_type_table.`binary` xor test_multi_type_table.`float4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`binary_xor_float8` = test_multi_type_table.`binary` xor test_multi_type_table.`float8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`binary_xor_numeric` = test_multi_type_table.`binary` xor test_multi_type_table.`numeric`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`binary_xor_bit1` = test_multi_type_table.`binary` xor test_multi_type_table.`bit1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`binary_xor_bit64` = test_multi_type_table.`binary` xor test_multi_type_table.`bit64`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`binary_xor_boolean` = test_multi_type_table.`binary` xor test_multi_type_table.`boolean`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`binary_xor_char` = test_multi_type_table.`binary` xor test_multi_type_table.`char`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`binary_xor_varchar` = test_multi_type_table.`binary` xor test_multi_type_table.`varchar`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`binary_xor_binary` = test_multi_type_table.`binary` xor test_multi_type_table.`binary`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`binary_xor_varbinary` = test_multi_type_table.`binary` xor test_multi_type_table.`varbinary`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`binary_xor_text` = test_multi_type_table.`binary` xor test_multi_type_table.`text`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`varbinary_xor_int1` = test_multi_type_table.`varbinary` xor test_multi_type_table.`int1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`varbinary_xor_uint1` = test_multi_type_table.`varbinary` xor test_multi_type_table.`uint1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`varbinary_xor_int2` = test_multi_type_table.`varbinary` xor test_multi_type_table.`int2`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`varbinary_xor_uint2` = test_multi_type_table.`varbinary` xor test_multi_type_table.`uint2`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`varbinary_xor_int4` = test_multi_type_table.`varbinary` xor test_multi_type_table.`int4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`varbinary_xor_uint4` = test_multi_type_table.`varbinary` xor test_multi_type_table.`uint4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`varbinary_xor_int8` = test_multi_type_table.`varbinary` xor test_multi_type_table.`int8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`varbinary_xor_uint8` = test_multi_type_table.`varbinary` xor test_multi_type_table.`uint8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`varbinary_xor_float4` = test_multi_type_table.`varbinary` xor test_multi_type_table.`float4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`varbinary_xor_float8` = test_multi_type_table.`varbinary` xor test_multi_type_table.`float8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`varbinary_xor_numeric` = test_multi_type_table.`varbinary` xor test_multi_type_table.`numeric`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`varbinary_xor_bit1` = test_multi_type_table.`varbinary` xor test_multi_type_table.`bit1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`varbinary_xor_bit64` = test_multi_type_table.`varbinary` xor test_multi_type_table.`bit64`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`varbinary_xor_boolean` = test_multi_type_table.`varbinary` xor test_multi_type_table.`boolean`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`varbinary_xor_char` = test_multi_type_table.`varbinary` xor test_multi_type_table.`char`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`varbinary_xor_varchar` = test_multi_type_table.`varbinary` xor test_multi_type_table.`varchar`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`varbinary_xor_binary` = test_multi_type_table.`varbinary` xor test_multi_type_table.`binary`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`varbinary_xor_varbinary` = test_multi_type_table.`varbinary` xor test_multi_type_table.`varbinary`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`varbinary_xor_text` = test_multi_type_table.`varbinary` xor test_multi_type_table.`text`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`text_xor_int1` = test_multi_type_table.`text` xor test_multi_type_table.`int1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`text_xor_uint1` = test_multi_type_table.`text` xor test_multi_type_table.`uint1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`text_xor_int2` = test_multi_type_table.`text` xor test_multi_type_table.`int2`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`text_xor_uint2` = test_multi_type_table.`text` xor test_multi_type_table.`uint2`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`text_xor_int4` = test_multi_type_table.`text` xor test_multi_type_table.`int4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`text_xor_uint4` = test_multi_type_table.`text` xor test_multi_type_table.`uint4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`text_xor_int8` = test_multi_type_table.`text` xor test_multi_type_table.`int8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`text_xor_uint8` = test_multi_type_table.`text` xor test_multi_type_table.`uint8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`text_xor_float4` = test_multi_type_table.`text` xor test_multi_type_table.`float4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`text_xor_float8` = test_multi_type_table.`text` xor test_multi_type_table.`float8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`text_xor_numeric` = test_multi_type_table.`text` xor test_multi_type_table.`numeric`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`text_xor_bit1` = test_multi_type_table.`text` xor test_multi_type_table.`bit1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`text_xor_bit64` = test_multi_type_table.`text` xor test_multi_type_table.`bit64`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`text_xor_boolean` = test_multi_type_table.`text` xor test_multi_type_table.`boolean`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`text_xor_char` = test_multi_type_table.`text` xor test_multi_type_table.`char`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`text_xor_varchar` = test_multi_type_table.`text` xor test_multi_type_table.`varchar`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`text_xor_binary` = test_multi_type_table.`text` xor test_multi_type_table.`binary`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`text_xor_varbinary` = test_multi_type_table.`text` xor test_multi_type_table.`varbinary`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`text_xor_text` = test_multi_type_table.`text` xor test_multi_type_table.`text`;
\x
SELECT * FROM test_multi_type;
DROP TABLE test_multi_type;

set dolphin.sql_mode to 'sql_mode_strict,sql_mode_full_group,pipes_as_concat,ansi_quotes,no_zero_date';
-- strict select
SELECT
    `int1` xor `uint1` AS `int1_xor_uint1`,
    `int2` xor `uint2` AS `int2_xor_uint2`,
    `int4` xor `uint4` AS `int4_xor_uint4`,
    `int8` xor `uint8` AS `int8_xor_uint8`,
    `float4` xor `float8` AS `float4_xor_float8`,
    `numeric` xor `bit1` AS `numeric_xor_bit1`,
    `bit64` xor `boolean` AS `bit64_xor_boolean`,
    `char` xor `varchar` AS `char_xor_varchar`,
    `binary` xor `varbinary` AS `binary_xor_varbinary`,
    `text` xor `text` AS `text_xor_text`
FROM test_multi_type_table;

-- strict insert
CREATE TABLE TEST(a int8);
INSERT INTO TEST SELECT `int1` xor `uint1` FROM test_multi_type_table;
INSERT INTO TEST SELECT `int2` xor `uint2` FROM test_multi_type_table;
INSERT INTO TEST SELECT `int4` xor `uint4` FROM test_multi_type_table;
INSERT INTO TEST SELECT `int8` xor `uint8` FROM test_multi_type_table;
INSERT INTO TEST SELECT `float4` xor `float8` FROM test_multi_type_table;
INSERT INTO TEST SELECT `numeric` xor `bit1` FROM test_multi_type_table;
INSERT INTO TEST SELECT `bit64` xor `boolean` FROM test_multi_type_table;
INSERT INTO TEST SELECT `char` xor `int1` FROM test_multi_type_table;
INSERT INTO TEST SELECT `varchar` xor `int1` FROM test_multi_type_table;
INSERT INTO TEST SELECT `binary` xor `int1` FROM test_multi_type_table;
INSERT INTO TEST SELECT `varbinary` xor `int1` FROM test_multi_type_table;
INSERT INTO TEST SELECT `text` xor `text` FROM test_multi_type_table;

-- strict insert ignore
INSERT IGNORE INTO TEST SELECT `int1` xor `uint1` FROM test_multi_type_table;
INSERT IGNORE INTO TEST SELECT `int2` xor `uint2` FROM test_multi_type_table;
INSERT IGNORE INTO TEST SELECT `int4` xor `uint4` FROM test_multi_type_table;
INSERT IGNORE INTO TEST SELECT `int8` xor `uint8` FROM test_multi_type_table;
INSERT IGNORE INTO TEST SELECT `float4` xor `float8` FROM test_multi_type_table;
INSERT IGNORE INTO TEST SELECT `numeric` xor `bit1` FROM test_multi_type_table;
INSERT IGNORE INTO TEST SELECT `bit64` xor `boolean` FROM test_multi_type_table;
INSERT IGNORE INTO TEST SELECT `char` xor `varchar` FROM test_multi_type_table;
INSERT IGNORE INTO TEST SELECT `binary` xor `varbinary` FROM test_multi_type_table;
INSERT IGNORE INTO TEST SELECT `text` xor `text` FROM test_multi_type_table;

\x
-- precedence test
SELECT 1 or null xor null;
SELECT 0 or null xor null;
SELECT 1 or null xor 1;
SELECT 0 or null xor 1;
SELECT 1 or 0 xor null;
SELECT 0 or 0 xor null;
SELECT 1 or 0 xor 1;
SELECT 0 or 0 xor 1;
SELECT 1 or 1 xor 1;
SELECT 0 or 1 xor 1;

---------- tail ----------
drop schema multi_type_xor_test_schema cascade;
reset current_schema;
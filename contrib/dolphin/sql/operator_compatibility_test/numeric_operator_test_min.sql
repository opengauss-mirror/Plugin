drop schema if exists numeric_operator_test_schema_min cascade;
create schema numeric_operator_test_schema_min;
set current_schema to 'numeric_operator_test_schema_min';
---------- head ----------
drop table if exists test_numeric_table_min;
CREATE TABLE test_numeric_table_min
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
    `bit64` bit(64)
);

-- type test when b_compatibility_mode is on
set dolphin.b_compatibility_mode to on;
drop table if exists test_numeric_type_min;
CREATE TABLE test_numeric_type_min AS SELECT
`int1` + `int1` AS `int1+int1`,
`int1` - `int1` AS `int1-int1`,
`int1` * `int1` AS `int1*int1`,
`int1` / `int1` AS `int1/int1`,
`int1` + `uint1` AS `int1+uint1`,
`int1` - `uint1` AS `int1-uint1`,
`int1` * `uint1` AS `int1*uint1`,
`int1` / `uint1` AS `int1/uint1`,
`int1` + `int2` AS `int1+int2`,
`int1` - `int2` AS `int1-int2`,
`int1` * `int2` AS `int1*int2`,
`int1` / `int2` AS `int1/int2`,
`int1` + `uint2` AS `int1+uint2`,
`int1` - `uint2` AS `int1-uint2`,
`int1` * `uint2` AS `int1*uint2`,
`int1` / `uint2` AS `int1/uint2`,
`int1` + `int4` AS `int1+int4`,
`int1` - `int4` AS `int1-int4`,
`int1` * `int4` AS `int1*int4`,
`int1` / `int4` AS `int1/int4`,
`int1` + `uint4` AS `int1+uint4`,
`int1` - `uint4` AS `int1-uint4`,
`int1` * `uint4` AS `int1*uint4`,
`int1` / `uint4` AS `int1/uint4`,
`int1` + `int8` AS `int1+int8`,
`int1` - `int8` AS `int1-int8`,
`int1` * `int8` AS `int1*int8`,
`int1` / `int8` AS `int1/int8`,
`int1` + `uint8` AS `int1+uint8`,
`int1` - `uint8` AS `int1-uint8`,
`int1` * `uint8` AS `int1*uint8`,
`int1` / `uint8` AS `int1/uint8`,
`int1` + `float4` AS `int1+float4`,
`int1` - `float4` AS `int1-float4`,
`int1` * `float4` AS `int1*float4`,
`int1` / `float4` AS `int1/float4`,
`int1` + `float8` AS `int1+float8`,
`int1` - `float8` AS `int1-float8`,
`int1` * `float8` AS `int1*float8`,
`int1` / `float8` AS `int1/float8`,
`int1` + `numeric` AS `int1+numeric`,
`int1` - `numeric` AS `int1-numeric`,
`int1` * `numeric` AS `int1*numeric`,
`int1` / `numeric` AS `int1/numeric`,
`int1` + `bit1` AS `int1+bit1`,
`int1` - `bit1` AS `int1-bit1`,
`int1` * `bit1` AS `int1*bit1`,
`int1` / `bit1` AS `int1/bit1`,
`int1` + `bit64` AS `int1+bit64`,
`int1` - `bit64` AS `int1-bit64`,
`int1` * `bit64` AS `int1*bit64`,
`int1` / `bit64` AS `int1/bit64`,
`uint1` + `int1` AS `uint1+int1`,
`uint1` - `int1` AS `uint1-int1`,
`uint1` * `int1` AS `uint1*int1`,
`uint1` / `int1` AS `uint1/int1`,
`uint1` + `uint1` AS `uint1+uint1`,
`uint1` - `uint1` AS `uint1-uint1`,
`uint1` * `uint1` AS `uint1*uint1`,
`uint1` / `uint1` AS `uint1/uint1`,
`uint1` + `int2` AS `uint1+int2`,
`uint1` - `int2` AS `uint1-int2`,
`uint1` * `int2` AS `uint1*int2`,
`uint1` / `int2` AS `uint1/int2`,
`uint1` + `uint2` AS `uint1+uint2`,
`uint1` - `uint2` AS `uint1-uint2`,
`uint1` * `uint2` AS `uint1*uint2`,
`uint1` / `uint2` AS `uint1/uint2`,
`uint1` + `int4` AS `uint1+int4`,
`uint1` - `int4` AS `uint1-int4`,
`uint1` * `int4` AS `uint1*int4`,
`uint1` / `int4` AS `uint1/int4`,
`uint1` + `uint4` AS `uint1+uint4`,
`uint1` - `uint4` AS `uint1-uint4`,
`uint1` * `uint4` AS `uint1*uint4`,
`uint1` / `uint4` AS `uint1/uint4`,
`uint1` + `int8` AS `uint1+int8`,
`uint1` - `int8` AS `uint1-int8`,
`uint1` * `int8` AS `uint1*int8`,
`uint1` / `int8` AS `uint1/int8`,
`uint1` + `uint8` AS `uint1+uint8`,
`uint1` - `uint8` AS `uint1-uint8`,
`uint1` * `uint8` AS `uint1*uint8`,
`uint1` / `uint8` AS `uint1/uint8`,
`uint1` + `float4` AS `uint1+float4`,
`uint1` - `float4` AS `uint1-float4`,
`uint1` * `float4` AS `uint1*float4`,
`uint1` / `float4` AS `uint1/float4`,
`uint1` + `float8` AS `uint1+float8`,
`uint1` - `float8` AS `uint1-float8`,
`uint1` * `float8` AS `uint1*float8`,
`uint1` / `float8` AS `uint1/float8`,
`uint1` + `numeric` AS `uint1+numeric`,
`uint1` - `numeric` AS `uint1-numeric`,
`uint1` * `numeric` AS `uint1*numeric`,
`uint1` / `numeric` AS `uint1/numeric`,
`uint1` + `bit1` AS `uint1+bit1`,
`uint1` - `bit1` AS `uint1-bit1`,
`uint1` * `bit1` AS `uint1*bit1`,
`uint1` / `bit1` AS `uint1/bit1`,
`uint1` + `bit64` AS `uint1+bit64`,
`uint1` - `bit64` AS `uint1-bit64`,
`uint1` * `bit64` AS `uint1*bit64`,
`uint1` / `bit64` AS `uint1/bit64`,
`int2` + `int1` AS `int2+int1`,
`int2` - `int1` AS `int2-int1`,
`int2` * `int1` AS `int2*int1`,
`int2` / `int1` AS `int2/int1`,
`int2` + `uint1` AS `int2+uint1`,
`int2` - `uint1` AS `int2-uint1`,
`int2` * `uint1` AS `int2*uint1`,
`int2` / `uint1` AS `int2/uint1`,
`int2` + `int2` AS `int2+int2`,
`int2` - `int2` AS `int2-int2`,
`int2` * `int2` AS `int2*int2`,
`int2` / `int2` AS `int2/int2`,
`int2` + `uint2` AS `int2+uint2`,
`int2` - `uint2` AS `int2-uint2`,
`int2` * `uint2` AS `int2*uint2`,
`int2` / `uint2` AS `int2/uint2`,
`int2` + `int4` AS `int2+int4`,
`int2` - `int4` AS `int2-int4`,
`int2` * `int4` AS `int2*int4`,
`int2` / `int4` AS `int2/int4`,
`int2` + `uint4` AS `int2+uint4`,
`int2` - `uint4` AS `int2-uint4`,
`int2` * `uint4` AS `int2*uint4`,
`int2` / `uint4` AS `int2/uint4`,
`int2` + `int8` AS `int2+int8`,
`int2` - `int8` AS `int2-int8`,
`int2` * `int8` AS `int2*int8`,
`int2` / `int8` AS `int2/int8`,
`int2` + `uint8` AS `int2+uint8`,
`int2` - `uint8` AS `int2-uint8`,
`int2` * `uint8` AS `int2*uint8`,
`int2` / `uint8` AS `int2/uint8`,
`int2` + `float4` AS `int2+float4`,
`int2` - `float4` AS `int2-float4`,
`int2` * `float4` AS `int2*float4`,
`int2` / `float4` AS `int2/float4`,
`int2` + `float8` AS `int2+float8`,
`int2` - `float8` AS `int2-float8`,
`int2` * `float8` AS `int2*float8`,
`int2` / `float8` AS `int2/float8`,
`int2` + `numeric` AS `int2+numeric`,
`int2` - `numeric` AS `int2-numeric`,
`int2` * `numeric` AS `int2*numeric`,
`int2` / `numeric` AS `int2/numeric`,
`int2` + `bit1` AS `int2+bit1`,
`int2` - `bit1` AS `int2-bit1`,
`int2` * `bit1` AS `int2*bit1`,
`int2` / `bit1` AS `int2/bit1`,
`int2` + `bit64` AS `int2+bit64`,
`int2` - `bit64` AS `int2-bit64`,
`int2` * `bit64` AS `int2*bit64`,
`int2` / `bit64` AS `int2/bit64`,
`uint2` + `int1` AS `uint2+int1`,
`uint2` - `int1` AS `uint2-int1`,
`uint2` * `int1` AS `uint2*int1`,
`uint2` / `int1` AS `uint2/int1`,
`uint2` + `uint1` AS `uint2+uint1`,
`uint2` - `uint1` AS `uint2-uint1`,
`uint2` * `uint1` AS `uint2*uint1`,
`uint2` / `uint1` AS `uint2/uint1`,
`uint2` + `int2` AS `uint2+int2`,
`uint2` - `int2` AS `uint2-int2`,
`uint2` * `int2` AS `uint2*int2`,
`uint2` / `int2` AS `uint2/int2`,
`uint2` + `uint2` AS `uint2+uint2`,
`uint2` - `uint2` AS `uint2-uint2`,
`uint2` * `uint2` AS `uint2*uint2`,
`uint2` / `uint2` AS `uint2/uint2`,
`uint2` + `int4` AS `uint2+int4`,
`uint2` - `int4` AS `uint2-int4`,
`uint2` * `int4` AS `uint2*int4`,
`uint2` / `int4` AS `uint2/int4`,
`uint2` + `uint4` AS `uint2+uint4`,
`uint2` - `uint4` AS `uint2-uint4`,
`uint2` * `uint4` AS `uint2*uint4`,
`uint2` / `uint4` AS `uint2/uint4`,
`uint2` + `int8` AS `uint2+int8`,
`uint2` - `int8` AS `uint2-int8`,
`uint2` * `int8` AS `uint2*int8`,
`uint2` / `int8` AS `uint2/int8`,
`uint2` + `uint8` AS `uint2+uint8`,
`uint2` - `uint8` AS `uint2-uint8`,
`uint2` * `uint8` AS `uint2*uint8`,
`uint2` / `uint8` AS `uint2/uint8`,
`uint2` + `float4` AS `uint2+float4`,
`uint2` - `float4` AS `uint2-float4`,
`uint2` * `float4` AS `uint2*float4`,
`uint2` / `float4` AS `uint2/float4`,
`uint2` + `float8` AS `uint2+float8`,
`uint2` - `float8` AS `uint2-float8`,
`uint2` * `float8` AS `uint2*float8`,
`uint2` / `float8` AS `uint2/float8`,
`uint2` + `numeric` AS `uint2+numeric`,
`uint2` - `numeric` AS `uint2-numeric`,
`uint2` * `numeric` AS `uint2*numeric`,
`uint2` / `numeric` AS `uint2/numeric`,
`uint2` + `bit1` AS `uint2+bit1`,
`uint2` - `bit1` AS `uint2-bit1`,
`uint2` * `bit1` AS `uint2*bit1`,
`uint2` / `bit1` AS `uint2/bit1`,
`uint2` + `bit64` AS `uint2+bit64`,
`uint2` - `bit64` AS `uint2-bit64`,
`uint2` * `bit64` AS `uint2*bit64`,
`uint2` / `bit64` AS `uint2/bit64`,
`int4` + `int1` AS `int4+int1`,
`int4` - `int1` AS `int4-int1`,
`int4` * `int1` AS `int4*int1`,
`int4` / `int1` AS `int4/int1`,
`int4` + `uint1` AS `int4+uint1`,
`int4` - `uint1` AS `int4-uint1`,
`int4` * `uint1` AS `int4*uint1`,
`int4` / `uint1` AS `int4/uint1`,
`int4` + `int2` AS `int4+int2`,
`int4` - `int2` AS `int4-int2`,
`int4` * `int2` AS `int4*int2`,
`int4` / `int2` AS `int4/int2`,
`int4` + `uint2` AS `int4+uint2`,
`int4` - `uint2` AS `int4-uint2`,
`int4` * `uint2` AS `int4*uint2`,
`int4` / `uint2` AS `int4/uint2`,
`int4` + `int4` AS `int4+int4`,
`int4` - `int4` AS `int4-int4`,
`int4` * `int4` AS `int4*int4`,
`int4` / `int4` AS `int4/int4`,
`int4` + `uint4` AS `int4+uint4`,
`int4` - `uint4` AS `int4-uint4`,
`int4` * `uint4` AS `int4*uint4`,
`int4` / `uint4` AS `int4/uint4`,
`int4` + `int8` AS `int4+int8`,
`int4` - `int8` AS `int4-int8`,
`int4` * `int8` AS `int4*int8`,
`int4` / `int8` AS `int4/int8`,
`int4` + `uint8` AS `int4+uint8`,
`int4` - `uint8` AS `int4-uint8`,
`int4` * `uint8` AS `int4*uint8`,
`int4` / `uint8` AS `int4/uint8`,
`int4` + `float4` AS `int4+float4`,
`int4` - `float4` AS `int4-float4`,
`int4` * `float4` AS `int4*float4`,
`int4` / `float4` AS `int4/float4`,
`int4` + `float8` AS `int4+float8`,
`int4` - `float8` AS `int4-float8`,
`int4` * `float8` AS `int4*float8`,
`int4` / `float8` AS `int4/float8`,
`int4` + `numeric` AS `int4+numeric`,
`int4` - `numeric` AS `int4-numeric`,
`int4` * `numeric` AS `int4*numeric`,
`int4` / `numeric` AS `int4/numeric`,
`int4` + `bit1` AS `int4+bit1`,
`int4` - `bit1` AS `int4-bit1`,
`int4` * `bit1` AS `int4*bit1`,
`int4` / `bit1` AS `int4/bit1`,
`int4` + `bit64` AS `int4+bit64`,
`int4` - `bit64` AS `int4-bit64`,
`int4` * `bit64` AS `int4*bit64`,
`int4` / `bit64` AS `int4/bit64`,
`uint4` + `int1` AS `uint4+int1`,
`uint4` - `int1` AS `uint4-int1`,
`uint4` * `int1` AS `uint4*int1`,
`uint4` / `int1` AS `uint4/int1`,
`uint4` + `uint1` AS `uint4+uint1`,
`uint4` - `uint1` AS `uint4-uint1`,
`uint4` * `uint1` AS `uint4*uint1`,
`uint4` / `uint1` AS `uint4/uint1`,
`uint4` + `int2` AS `uint4+int2`,
`uint4` - `int2` AS `uint4-int2`,
`uint4` * `int2` AS `uint4*int2`,
`uint4` / `int2` AS `uint4/int2`,
`uint4` + `uint2` AS `uint4+uint2`,
`uint4` - `uint2` AS `uint4-uint2`,
`uint4` * `uint2` AS `uint4*uint2`,
`uint4` / `uint2` AS `uint4/uint2`,
`uint4` + `int4` AS `uint4+int4`,
`uint4` - `int4` AS `uint4-int4`,
`uint4` * `int4` AS `uint4*int4`,
`uint4` / `int4` AS `uint4/int4`,
`uint4` + `uint4` AS `uint4+uint4`,
`uint4` - `uint4` AS `uint4-uint4`,
`uint4` * `uint4` AS `uint4*uint4`,
`uint4` / `uint4` AS `uint4/uint4`,
`uint4` + `int8` AS `uint4+int8`,
`uint4` - `int8` AS `uint4-int8`,
`uint4` * `int8` AS `uint4*int8`,
`uint4` / `int8` AS `uint4/int8`,
`uint4` + `uint8` AS `uint4+uint8`,
`uint4` - `uint8` AS `uint4-uint8`,
`uint4` * `uint8` AS `uint4*uint8`,
`uint4` / `uint8` AS `uint4/uint8`,
`uint4` + `float4` AS `uint4+float4`,
`uint4` - `float4` AS `uint4-float4`,
`uint4` * `float4` AS `uint4*float4`,
`uint4` / `float4` AS `uint4/float4`,
`uint4` + `float8` AS `uint4+float8`,
`uint4` - `float8` AS `uint4-float8`,
`uint4` * `float8` AS `uint4*float8`,
`uint4` / `float8` AS `uint4/float8`,
`uint4` + `numeric` AS `uint4+numeric`,
`uint4` - `numeric` AS `uint4-numeric`,
`uint4` * `numeric` AS `uint4*numeric`,
`uint4` / `numeric` AS `uint4/numeric`,
`uint4` + `bit1` AS `uint4+bit1`,
`uint4` - `bit1` AS `uint4-bit1`,
`uint4` * `bit1` AS `uint4*bit1`,
`uint4` / `bit1` AS `uint4/bit1`,
`uint4` + `bit64` AS `uint4+bit64`,
`uint4` - `bit64` AS `uint4-bit64`,
`uint4` * `bit64` AS `uint4*bit64`,
`uint4` / `bit64` AS `uint4/bit64`,
`int8` + `int1` AS `int8+int1`,
`int8` - `int1` AS `int8-int1`,
`int8` * `int1` AS `int8*int1`,
`int8` / `int1` AS `int8/int1`,
`int8` + `uint1` AS `int8+uint1`,
`int8` - `uint1` AS `int8-uint1`,
`int8` * `uint1` AS `int8*uint1`,
`int8` / `uint1` AS `int8/uint1`,
`int8` + `int2` AS `int8+int2`,
`int8` - `int2` AS `int8-int2`,
`int8` * `int2` AS `int8*int2`,
`int8` / `int2` AS `int8/int2`,
`int8` + `uint2` AS `int8+uint2`,
`int8` - `uint2` AS `int8-uint2`,
`int8` * `uint2` AS `int8*uint2`,
`int8` / `uint2` AS `int8/uint2`,
`int8` + `int4` AS `int8+int4`,
`int8` - `int4` AS `int8-int4`,
`int8` * `int4` AS `int8*int4`,
`int8` / `int4` AS `int8/int4`,
`int8` + `uint4` AS `int8+uint4`,
`int8` - `uint4` AS `int8-uint4`,
`int8` * `uint4` AS `int8*uint4`,
`int8` / `uint4` AS `int8/uint4`,
`int8` + `int8` AS `int8+int8`,
`int8` - `int8` AS `int8-int8`,
`int8` * `int8` AS `int8*int8`,
`int8` / `int8` AS `int8/int8`,
`int8` + `uint8` AS `int8+uint8`,
`int8` - `uint8` AS `int8-uint8`,
`int8` * `uint8` AS `int8*uint8`,
`int8` / `uint8` AS `int8/uint8`,
`int8` + `float4` AS `int8+float4`,
`int8` - `float4` AS `int8-float4`,
`int8` * `float4` AS `int8*float4`,
`int8` / `float4` AS `int8/float4`,
`int8` + `float8` AS `int8+float8`,
`int8` - `float8` AS `int8-float8`,
`int8` * `float8` AS `int8*float8`,
`int8` / `float8` AS `int8/float8`,
`int8` + `numeric` AS `int8+numeric`,
`int8` - `numeric` AS `int8-numeric`,
`int8` * `numeric` AS `int8*numeric`,
`int8` / `numeric` AS `int8/numeric`,
`int8` + `bit1` AS `int8+bit1`,
`int8` - `bit1` AS `int8-bit1`,
`int8` * `bit1` AS `int8*bit1`,
`int8` / `bit1` AS `int8/bit1`,
`int8` + `bit64` AS `int8+bit64`,
`int8` - `bit64` AS `int8-bit64`,
`int8` * `bit64` AS `int8*bit64`,
`int8` / `bit64` AS `int8/bit64`,
`uint8` + `int1` AS `uint8+int1`,
`uint8` - `int1` AS `uint8-int1`,
`uint8` * `int1` AS `uint8*int1`,
`uint8` / `int1` AS `uint8/int1`,
`uint8` + `uint1` AS `uint8+uint1`,
`uint8` - `uint1` AS `uint8-uint1`,
`uint8` * `uint1` AS `uint8*uint1`,
`uint8` / `uint1` AS `uint8/uint1`,
`uint8` + `int2` AS `uint8+int2`,
`uint8` - `int2` AS `uint8-int2`,
`uint8` * `int2` AS `uint8*int2`,
`uint8` / `int2` AS `uint8/int2`,
`uint8` + `uint2` AS `uint8+uint2`,
`uint8` - `uint2` AS `uint8-uint2`,
`uint8` * `uint2` AS `uint8*uint2`,
`uint8` / `uint2` AS `uint8/uint2`,
`uint8` + `int4` AS `uint8+int4`,
`uint8` - `int4` AS `uint8-int4`,
`uint8` * `int4` AS `uint8*int4`,
`uint8` / `int4` AS `uint8/int4`,
`uint8` + `uint4` AS `uint8+uint4`,
`uint8` - `uint4` AS `uint8-uint4`,
`uint8` * `uint4` AS `uint8*uint4`,
`uint8` / `uint4` AS `uint8/uint4`,
`uint8` + `int8` AS `uint8+int8`,
`uint8` - `int8` AS `uint8-int8`,
`uint8` * `int8` AS `uint8*int8`,
`uint8` / `int8` AS `uint8/int8`,
`uint8` + `uint8` AS `uint8+uint8`,
`uint8` - `uint8` AS `uint8-uint8`,
`uint8` * `uint8` AS `uint8*uint8`,
`uint8` / `uint8` AS `uint8/uint8`,
`uint8` + `float4` AS `uint8+float4`,
`uint8` - `float4` AS `uint8-float4`,
`uint8` * `float4` AS `uint8*float4`,
`uint8` / `float4` AS `uint8/float4`,
`uint8` + `float8` AS `uint8+float8`,
`uint8` - `float8` AS `uint8-float8`,
`uint8` * `float8` AS `uint8*float8`,
`uint8` / `float8` AS `uint8/float8`,
`uint8` + `numeric` AS `uint8+numeric`,
`uint8` - `numeric` AS `uint8-numeric`,
`uint8` * `numeric` AS `uint8*numeric`,
`uint8` / `numeric` AS `uint8/numeric`,
`uint8` + `bit1` AS `uint8+bit1`,
`uint8` - `bit1` AS `uint8-bit1`,
`uint8` * `bit1` AS `uint8*bit1`,
`uint8` / `bit1` AS `uint8/bit1`,
`uint8` + `bit64` AS `uint8+bit64`,
`uint8` - `bit64` AS `uint8-bit64`,
`uint8` * `bit64` AS `uint8*bit64`,
`uint8` / `bit64` AS `uint8/bit64`,
`float4` + `int1` AS `float4+int1`,
`float4` - `int1` AS `float4-int1`,
`float4` * `int1` AS `float4*int1`,
`float4` / `int1` AS `float4/int1`,
`float4` + `uint1` AS `float4+uint1`,
`float4` - `uint1` AS `float4-uint1`,
`float4` * `uint1` AS `float4*uint1`,
`float4` / `uint1` AS `float4/uint1`,
`float4` + `int2` AS `float4+int2`,
`float4` - `int2` AS `float4-int2`,
`float4` * `int2` AS `float4*int2`,
`float4` / `int2` AS `float4/int2`,
`float4` + `uint2` AS `float4+uint2`,
`float4` - `uint2` AS `float4-uint2`,
`float4` * `uint2` AS `float4*uint2`,
`float4` / `uint2` AS `float4/uint2`,
`float4` + `int4` AS `float4+int4`,
`float4` - `int4` AS `float4-int4`,
`float4` * `int4` AS `float4*int4`,
`float4` / `int4` AS `float4/int4`,
`float4` + `uint4` AS `float4+uint4`,
`float4` - `uint4` AS `float4-uint4`,
`float4` * `uint4` AS `float4*uint4`,
`float4` / `uint4` AS `float4/uint4`,
`float4` + `int8` AS `float4+int8`,
`float4` - `int8` AS `float4-int8`,
`float4` * `int8` AS `float4*int8`,
`float4` / `int8` AS `float4/int8`,
`float4` + `uint8` AS `float4+uint8`,
`float4` - `uint8` AS `float4-uint8`,
`float4` * `uint8` AS `float4*uint8`,
`float4` / `uint8` AS `float4/uint8`,
`float4` + `float4` AS `float4+float4`,
`float4` - `float4` AS `float4-float4`,
`float4` * `float4` AS `float4*float4`,
`float4` / `float4` AS `float4/float4`,
`float4` + `float8` AS `float4+float8`,
`float4` - `float8` AS `float4-float8`,
`float4` * `float8` AS `float4*float8`,
`float4` / `float8` AS `float4/float8`,
`float4` + `numeric` AS `float4+numeric`,
`float4` - `numeric` AS `float4-numeric`,
`float4` * `numeric` AS `float4*numeric`,
`float4` / `numeric` AS `float4/numeric`,
`float4` + `bit1` AS `float4+bit1`,
`float4` - `bit1` AS `float4-bit1`,
`float4` * `bit1` AS `float4*bit1`,
`float4` / `bit1` AS `float4/bit1`,
`float4` + `bit64` AS `float4+bit64`,
`float4` - `bit64` AS `float4-bit64`,
`float4` * `bit64` AS `float4*bit64`,
`float4` / `bit64` AS `float4/bit64`,
`float8` + `int1` AS `float8+int1`,
`float8` - `int1` AS `float8-int1`,
`float8` * `int1` AS `float8*int1`,
`float8` / `int1` AS `float8/int1`,
`float8` + `uint1` AS `float8+uint1`,
`float8` - `uint1` AS `float8-uint1`,
`float8` * `uint1` AS `float8*uint1`,
`float8` / `uint1` AS `float8/uint1`,
`float8` + `int2` AS `float8+int2`,
`float8` - `int2` AS `float8-int2`,
`float8` * `int2` AS `float8*int2`,
`float8` / `int2` AS `float8/int2`,
`float8` + `uint2` AS `float8+uint2`,
`float8` - `uint2` AS `float8-uint2`,
`float8` * `uint2` AS `float8*uint2`,
`float8` / `uint2` AS `float8/uint2`,
`float8` + `int4` AS `float8+int4`,
`float8` - `int4` AS `float8-int4`,
`float8` * `int4` AS `float8*int4`,
`float8` / `int4` AS `float8/int4`,
`float8` + `uint4` AS `float8+uint4`,
`float8` - `uint4` AS `float8-uint4`,
`float8` * `uint4` AS `float8*uint4`,
`float8` / `uint4` AS `float8/uint4`,
`float8` + `int8` AS `float8+int8`,
`float8` - `int8` AS `float8-int8`,
`float8` * `int8` AS `float8*int8`,
`float8` / `int8` AS `float8/int8`,
`float8` + `uint8` AS `float8+uint8`,
`float8` - `uint8` AS `float8-uint8`,
`float8` * `uint8` AS `float8*uint8`,
`float8` / `uint8` AS `float8/uint8`,
`float8` + `float4` AS `float8+float4`,
`float8` - `float4` AS `float8-float4`,
`float8` * `float4` AS `float8*float4`,
`float8` / `float4` AS `float8/float4`,
`float8` + `float8` AS `float8+float8`,
`float8` - `float8` AS `float8-float8`,
`float8` * `float8` AS `float8*float8`,
`float8` / `float8` AS `float8/float8`,
`float8` + `numeric` AS `float8+numeric`,
`float8` - `numeric` AS `float8-numeric`,
`float8` * `numeric` AS `float8*numeric`,
`float8` / `numeric` AS `float8/numeric`,
`float8` + `bit1` AS `float8+bit1`,
`float8` - `bit1` AS `float8-bit1`,
`float8` * `bit1` AS `float8*bit1`,
`float8` / `bit1` AS `float8/bit1`,
`float8` + `bit64` AS `float8+bit64`,
`float8` - `bit64` AS `float8-bit64`,
`float8` * `bit64` AS `float8*bit64`,
`float8` / `bit64` AS `float8/bit64`,
`numeric` + `int1` AS `numeric+int1`,
`numeric` - `int1` AS `numeric-int1`,
`numeric` * `int1` AS `numeric*int1`,
`numeric` / `int1` AS `numeric/int1`,
`numeric` + `uint1` AS `numeric+uint1`,
`numeric` - `uint1` AS `numeric-uint1`,
`numeric` * `uint1` AS `numeric*uint1`,
`numeric` / `uint1` AS `numeric/uint1`,
`numeric` + `int2` AS `numeric+int2`,
`numeric` - `int2` AS `numeric-int2`,
`numeric` * `int2` AS `numeric*int2`,
`numeric` / `int2` AS `numeric/int2`,
`numeric` + `uint2` AS `numeric+uint2`,
`numeric` - `uint2` AS `numeric-uint2`,
`numeric` * `uint2` AS `numeric*uint2`,
`numeric` / `uint2` AS `numeric/uint2`,
`numeric` + `int4` AS `numeric+int4`,
`numeric` - `int4` AS `numeric-int4`,
`numeric` * `int4` AS `numeric*int4`,
`numeric` / `int4` AS `numeric/int4`,
`numeric` + `uint4` AS `numeric+uint4`,
`numeric` - `uint4` AS `numeric-uint4`,
`numeric` * `uint4` AS `numeric*uint4`,
`numeric` / `uint4` AS `numeric/uint4`,
`numeric` + `int8` AS `numeric+int8`,
`numeric` - `int8` AS `numeric-int8`,
`numeric` * `int8` AS `numeric*int8`,
`numeric` / `int8` AS `numeric/int8`,
`numeric` + `uint8` AS `numeric+uint8`,
`numeric` - `uint8` AS `numeric-uint8`,
`numeric` * `uint8` AS `numeric*uint8`,
`numeric` / `uint8` AS `numeric/uint8`,
`numeric` + `float4` AS `numeric+float4`,
`numeric` - `float4` AS `numeric-float4`,
`numeric` * `float4` AS `numeric*float4`,
`numeric` / `float4` AS `numeric/float4`,
`numeric` + `float8` AS `numeric+float8`,
`numeric` - `float8` AS `numeric-float8`,
`numeric` * `float8` AS `numeric*float8`,
`numeric` / `float8` AS `numeric/float8`,
`numeric` + `numeric` AS `numeric+numeric`,
`numeric` - `numeric` AS `numeric-numeric`,
`numeric` * `numeric` AS `numeric*numeric`,
`numeric` / `numeric` AS `numeric/numeric`,
`numeric` + `bit1` AS `numeric+bit1`,
`numeric` - `bit1` AS `numeric-bit1`,
`numeric` * `bit1` AS `numeric*bit1`,
`numeric` / `bit1` AS `numeric/bit1`,
`numeric` + `bit64` AS `numeric+bit64`,
`numeric` - `bit64` AS `numeric-bit64`,
`numeric` * `bit64` AS `numeric*bit64`,
`numeric` / `bit64` AS `numeric/bit64`,
`bit1` + `int1` AS `bit1+int1`,
`bit1` - `int1` AS `bit1-int1`,
`bit1` * `int1` AS `bit1*int1`,
`bit1` / `int1` AS `bit1/int1`,
`bit1` + `uint1` AS `bit1+uint1`,
`bit1` - `uint1` AS `bit1-uint1`,
`bit1` * `uint1` AS `bit1*uint1`,
`bit1` / `uint1` AS `bit1/uint1`,
`bit1` + `int2` AS `bit1+int2`,
`bit1` - `int2` AS `bit1-int2`,
`bit1` * `int2` AS `bit1*int2`,
`bit1` / `int2` AS `bit1/int2`,
`bit1` + `uint2` AS `bit1+uint2`,
`bit1` - `uint2` AS `bit1-uint2`,
`bit1` * `uint2` AS `bit1*uint2`,
`bit1` / `uint2` AS `bit1/uint2`,
`bit1` + `int4` AS `bit1+int4`,
`bit1` - `int4` AS `bit1-int4`,
`bit1` * `int4` AS `bit1*int4`,
`bit1` / `int4` AS `bit1/int4`,
`bit1` + `uint4` AS `bit1+uint4`,
`bit1` - `uint4` AS `bit1-uint4`,
`bit1` * `uint4` AS `bit1*uint4`,
`bit1` / `uint4` AS `bit1/uint4`,
`bit1` + `int8` AS `bit1+int8`,
`bit1` - `int8` AS `bit1-int8`,
`bit1` * `int8` AS `bit1*int8`,
`bit1` / `int8` AS `bit1/int8`,
`bit1` + `uint8` AS `bit1+uint8`,
`bit1` - `uint8` AS `bit1-uint8`,
`bit1` * `uint8` AS `bit1*uint8`,
`bit1` / `uint8` AS `bit1/uint8`,
`bit1` + `float4` AS `bit1+float4`,
`bit1` - `float4` AS `bit1-float4`,
`bit1` * `float4` AS `bit1*float4`,
`bit1` / `float4` AS `bit1/float4`,
`bit1` + `float8` AS `bit1+float8`,
`bit1` - `float8` AS `bit1-float8`,
`bit1` * `float8` AS `bit1*float8`,
`bit1` / `float8` AS `bit1/float8`,
`bit1` + `numeric` AS `bit1+numeric`,
`bit1` - `numeric` AS `bit1-numeric`,
`bit1` * `numeric` AS `bit1*numeric`,
`bit1` / `numeric` AS `bit1/numeric`,
`bit1` + `bit1` AS `bit1+bit1`,
`bit1` - `bit1` AS `bit1-bit1`,
`bit1` * `bit1` AS `bit1*bit1`,
`bit1` / `bit1` AS `bit1/bit1`,
`bit1` + `bit64` AS `bit1+bit64`,
`bit1` - `bit64` AS `bit1-bit64`,
`bit1` * `bit64` AS `bit1*bit64`,
`bit1` / `bit64` AS `bit1/bit64`,
`bit64` + `int1` AS `bit64+int1`,
`bit64` - `int1` AS `bit64-int1`,
`bit64` * `int1` AS `bit64*int1`,
`bit64` / `int1` AS `bit64/int1`,
`bit64` + `uint1` AS `bit64+uint1`,
`bit64` - `uint1` AS `bit64-uint1`,
`bit64` * `uint1` AS `bit64*uint1`,
`bit64` / `uint1` AS `bit64/uint1`,
`bit64` + `int2` AS `bit64+int2`,
`bit64` - `int2` AS `bit64-int2`,
`bit64` * `int2` AS `bit64*int2`,
`bit64` / `int2` AS `bit64/int2`,
`bit64` + `uint2` AS `bit64+uint2`,
`bit64` - `uint2` AS `bit64-uint2`,
`bit64` * `uint2` AS `bit64*uint2`,
`bit64` / `uint2` AS `bit64/uint2`,
`bit64` + `int4` AS `bit64+int4`,
`bit64` - `int4` AS `bit64-int4`,
`bit64` * `int4` AS `bit64*int4`,
`bit64` / `int4` AS `bit64/int4`,
`bit64` + `uint4` AS `bit64+uint4`,
`bit64` - `uint4` AS `bit64-uint4`,
`bit64` * `uint4` AS `bit64*uint4`,
`bit64` / `uint4` AS `bit64/uint4`,
`bit64` + `int8` AS `bit64+int8`,
`bit64` - `int8` AS `bit64-int8`,
`bit64` * `int8` AS `bit64*int8`,
`bit64` / `int8` AS `bit64/int8`,
`bit64` + `uint8` AS `bit64+uint8`,
`bit64` - `uint8` AS `bit64-uint8`,
`bit64` * `uint8` AS `bit64*uint8`,
`bit64` / `uint8` AS `bit64/uint8`,
`bit64` + `float4` AS `bit64+float4`,
`bit64` - `float4` AS `bit64-float4`,
`bit64` * `float4` AS `bit64*float4`,
`bit64` / `float4` AS `bit64/float4`,
`bit64` + `float8` AS `bit64+float8`,
`bit64` - `float8` AS `bit64-float8`,
`bit64` * `float8` AS `bit64*float8`,
`bit64` / `float8` AS `bit64/float8`,
`bit64` + `numeric` AS `bit64+numeric`,
`bit64` - `numeric` AS `bit64-numeric`,
`bit64` * `numeric` AS `bit64*numeric`,
`bit64` / `numeric` AS `bit64/numeric`,
`bit64` + `bit1` AS `bit64+bit1`,
`bit64` - `bit1` AS `bit64-bit1`,
`bit64` * `bit1` AS `bit64*bit1`,
`bit64` / `bit1` AS `bit64/bit1`,
`bit64` + `bit64` AS `bit64+bit64`,
`bit64` - `bit64` AS `bit64-bit64`,
`bit64` * `bit64` AS `bit64*bit64`,
`bit64` / `bit64` AS `bit64/bit64`
FROM
	test_numeric_table_min;

INSERT INTO test_numeric_type_min VALUES(null);
-- min value test
INSERT INTO test_numeric_table_min VALUES (-128, 0, -32768, 0, -2147483648, 0, -9223372036854775808, 0, -3.402e+38, -1.79e+308, -99999999999999.999999, b'0'::bit, b'0000000000000000000000000000000000000000000000000000000000000000'::bit(64));
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int1+int1` = test_numeric_table_min.`int1`+test_numeric_table_min.`int1`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int1-int1` = test_numeric_table_min.`int1`-test_numeric_table_min.`int1`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int1*int1` = test_numeric_table_min.`int1`*test_numeric_table_min.`int1`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int1/int1` = test_numeric_table_min.`int1`/test_numeric_table_min.`int1`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int1+uint1` = test_numeric_table_min.`int1`+test_numeric_table_min.`uint1`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int1-uint1` = test_numeric_table_min.`int1`-test_numeric_table_min.`uint1`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int1*uint1` = test_numeric_table_min.`int1`*test_numeric_table_min.`uint1`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int1/uint1` = test_numeric_table_min.`int1`/test_numeric_table_min.`uint1`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int1+int2` = test_numeric_table_min.`int1`+test_numeric_table_min.`int2`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int1-int2` = test_numeric_table_min.`int1`-test_numeric_table_min.`int2`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int1*int2` = test_numeric_table_min.`int1`*test_numeric_table_min.`int2`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int1/int2` = test_numeric_table_min.`int1`/test_numeric_table_min.`int2`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int1+uint2` = test_numeric_table_min.`int1`+test_numeric_table_min.`uint2`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int1-uint2` = test_numeric_table_min.`int1`-test_numeric_table_min.`uint2`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int1*uint2` = test_numeric_table_min.`int1`*test_numeric_table_min.`uint2`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int1/uint2` = test_numeric_table_min.`int1`/test_numeric_table_min.`uint2`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int1+int4` = test_numeric_table_min.`int1`+test_numeric_table_min.`int4`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int1-int4` = test_numeric_table_min.`int1`-test_numeric_table_min.`int4`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int1*int4` = test_numeric_table_min.`int1`*test_numeric_table_min.`int4`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int1/int4` = test_numeric_table_min.`int1`/test_numeric_table_min.`int4`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int1+uint4` = test_numeric_table_min.`int1`+test_numeric_table_min.`uint4`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int1-uint4` = test_numeric_table_min.`int1`-test_numeric_table_min.`uint4`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int1*uint4` = test_numeric_table_min.`int1`*test_numeric_table_min.`uint4`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int1/uint4` = test_numeric_table_min.`int1`/test_numeric_table_min.`uint4`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int1+int8` = test_numeric_table_min.`int1`+test_numeric_table_min.`int8`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int1-int8` = test_numeric_table_min.`int1`-test_numeric_table_min.`int8`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int1*int8` = test_numeric_table_min.`int1`*test_numeric_table_min.`int8`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int1/int8` = test_numeric_table_min.`int1`/test_numeric_table_min.`int8`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int1+uint8` = test_numeric_table_min.`int1`+test_numeric_table_min.`uint8`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int1-uint8` = test_numeric_table_min.`int1`-test_numeric_table_min.`uint8`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int1*uint8` = test_numeric_table_min.`int1`*test_numeric_table_min.`uint8`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int1/uint8` = test_numeric_table_min.`int1`/test_numeric_table_min.`uint8`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int1+float4` = test_numeric_table_min.`int1`+test_numeric_table_min.`float4`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int1-float4` = test_numeric_table_min.`int1`-test_numeric_table_min.`float4`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int1*float4` = test_numeric_table_min.`int1`*test_numeric_table_min.`float4`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int1/float4` = test_numeric_table_min.`int1`/test_numeric_table_min.`float4`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int1+float8` = test_numeric_table_min.`int1`+test_numeric_table_min.`float8`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int1-float8` = test_numeric_table_min.`int1`-test_numeric_table_min.`float8`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int1*float8` = test_numeric_table_min.`int1`*test_numeric_table_min.`float8`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int1/float8` = test_numeric_table_min.`int1`/test_numeric_table_min.`float8`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int1+numeric` = test_numeric_table_min.`int1`+test_numeric_table_min.`numeric`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int1-numeric` = test_numeric_table_min.`int1`-test_numeric_table_min.`numeric`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int1*numeric` = test_numeric_table_min.`int1`*test_numeric_table_min.`numeric`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int1/numeric` = test_numeric_table_min.`int1`/test_numeric_table_min.`numeric`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int1+bit1` = test_numeric_table_min.`int1`+test_numeric_table_min.`bit1`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int1-bit1` = test_numeric_table_min.`int1`-test_numeric_table_min.`bit1`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int1*bit1` = test_numeric_table_min.`int1`*test_numeric_table_min.`bit1`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int1/bit1` = test_numeric_table_min.`int1`/test_numeric_table_min.`bit1`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int1+bit64` = test_numeric_table_min.`int1`+test_numeric_table_min.`bit64`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int1-bit64` = test_numeric_table_min.`int1`-test_numeric_table_min.`bit64`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int1*bit64` = test_numeric_table_min.`int1`*test_numeric_table_min.`bit64`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int1/bit64` = test_numeric_table_min.`int1`/test_numeric_table_min.`bit64`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint1+int1` = test_numeric_table_min.`uint1`+test_numeric_table_min.`int1`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint1-int1` = test_numeric_table_min.`uint1`-test_numeric_table_min.`int1`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint1*int1` = test_numeric_table_min.`uint1`*test_numeric_table_min.`int1`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint1/int1` = test_numeric_table_min.`uint1`/test_numeric_table_min.`int1`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint1+uint1` = test_numeric_table_min.`uint1`+test_numeric_table_min.`uint1`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint1-uint1` = test_numeric_table_min.`uint1`-test_numeric_table_min.`uint1`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint1*uint1` = test_numeric_table_min.`uint1`*test_numeric_table_min.`uint1`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint1/uint1` = test_numeric_table_min.`uint1`/test_numeric_table_min.`uint1`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint1+int2` = test_numeric_table_min.`uint1`+test_numeric_table_min.`int2`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint1-int2` = test_numeric_table_min.`uint1`-test_numeric_table_min.`int2`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint1*int2` = test_numeric_table_min.`uint1`*test_numeric_table_min.`int2`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint1/int2` = test_numeric_table_min.`uint1`/test_numeric_table_min.`int2`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint1+uint2` = test_numeric_table_min.`uint1`+test_numeric_table_min.`uint2`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint1-uint2` = test_numeric_table_min.`uint1`-test_numeric_table_min.`uint2`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint1*uint2` = test_numeric_table_min.`uint1`*test_numeric_table_min.`uint2`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint1/uint2` = test_numeric_table_min.`uint1`/test_numeric_table_min.`uint2`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint1+int4` = test_numeric_table_min.`uint1`+test_numeric_table_min.`int4`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint1-int4` = test_numeric_table_min.`uint1`-test_numeric_table_min.`int4`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint1*int4` = test_numeric_table_min.`uint1`*test_numeric_table_min.`int4`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint1/int4` = test_numeric_table_min.`uint1`/test_numeric_table_min.`int4`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint1+uint4` = test_numeric_table_min.`uint1`+test_numeric_table_min.`uint4`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint1-uint4` = test_numeric_table_min.`uint1`-test_numeric_table_min.`uint4`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint1*uint4` = test_numeric_table_min.`uint1`*test_numeric_table_min.`uint4`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint1/uint4` = test_numeric_table_min.`uint1`/test_numeric_table_min.`uint4`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint1+int8` = test_numeric_table_min.`uint1`+test_numeric_table_min.`int8`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint1-int8` = test_numeric_table_min.`uint1`-test_numeric_table_min.`int8`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint1*int8` = test_numeric_table_min.`uint1`*test_numeric_table_min.`int8`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint1/int8` = test_numeric_table_min.`uint1`/test_numeric_table_min.`int8`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint1+uint8` = test_numeric_table_min.`uint1`+test_numeric_table_min.`uint8`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint1-uint8` = test_numeric_table_min.`uint1`-test_numeric_table_min.`uint8`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint1*uint8` = test_numeric_table_min.`uint1`*test_numeric_table_min.`uint8`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint1/uint8` = test_numeric_table_min.`uint1`/test_numeric_table_min.`uint8`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint1+float4` = test_numeric_table_min.`uint1`+test_numeric_table_min.`float4`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint1-float4` = test_numeric_table_min.`uint1`-test_numeric_table_min.`float4`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint1*float4` = test_numeric_table_min.`uint1`*test_numeric_table_min.`float4`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint1/float4` = test_numeric_table_min.`uint1`/test_numeric_table_min.`float4`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint1+float8` = test_numeric_table_min.`uint1`+test_numeric_table_min.`float8`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint1-float8` = test_numeric_table_min.`uint1`-test_numeric_table_min.`float8`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint1*float8` = test_numeric_table_min.`uint1`*test_numeric_table_min.`float8`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint1/float8` = test_numeric_table_min.`uint1`/test_numeric_table_min.`float8`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint1+numeric` = test_numeric_table_min.`uint1`+test_numeric_table_min.`numeric`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint1-numeric` = test_numeric_table_min.`uint1`-test_numeric_table_min.`numeric`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint1*numeric` = test_numeric_table_min.`uint1`*test_numeric_table_min.`numeric`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint1/numeric` = test_numeric_table_min.`uint1`/test_numeric_table_min.`numeric`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint1+bit1` = test_numeric_table_min.`uint1`+test_numeric_table_min.`bit1`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint1-bit1` = test_numeric_table_min.`uint1`-test_numeric_table_min.`bit1`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint1*bit1` = test_numeric_table_min.`uint1`*test_numeric_table_min.`bit1`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint1/bit1` = test_numeric_table_min.`uint1`/test_numeric_table_min.`bit1`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint1+bit64` = test_numeric_table_min.`uint1`+test_numeric_table_min.`bit64`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint1-bit64` = test_numeric_table_min.`uint1`-test_numeric_table_min.`bit64`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint1*bit64` = test_numeric_table_min.`uint1`*test_numeric_table_min.`bit64`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint1/bit64` = test_numeric_table_min.`uint1`/test_numeric_table_min.`bit64`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int2+int1` = test_numeric_table_min.`int2`+test_numeric_table_min.`int1`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int2-int1` = test_numeric_table_min.`int2`-test_numeric_table_min.`int1`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int2*int1` = test_numeric_table_min.`int2`*test_numeric_table_min.`int1`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int2/int1` = test_numeric_table_min.`int2`/test_numeric_table_min.`int1`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int2+uint1` = test_numeric_table_min.`int2`+test_numeric_table_min.`uint1`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int2-uint1` = test_numeric_table_min.`int2`-test_numeric_table_min.`uint1`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int2*uint1` = test_numeric_table_min.`int2`*test_numeric_table_min.`uint1`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int2/uint1` = test_numeric_table_min.`int2`/test_numeric_table_min.`uint1`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int2+int2` = test_numeric_table_min.`int2`+test_numeric_table_min.`int2`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int2-int2` = test_numeric_table_min.`int2`-test_numeric_table_min.`int2`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int2*int2` = test_numeric_table_min.`int2`*test_numeric_table_min.`int2`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int2/int2` = test_numeric_table_min.`int2`/test_numeric_table_min.`int2`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int2+uint2` = test_numeric_table_min.`int2`+test_numeric_table_min.`uint2`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int2-uint2` = test_numeric_table_min.`int2`-test_numeric_table_min.`uint2`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int2*uint2` = test_numeric_table_min.`int2`*test_numeric_table_min.`uint2`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int2/uint2` = test_numeric_table_min.`int2`/test_numeric_table_min.`uint2`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int2+int4` = test_numeric_table_min.`int2`+test_numeric_table_min.`int4`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int2-int4` = test_numeric_table_min.`int2`-test_numeric_table_min.`int4`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int2*int4` = test_numeric_table_min.`int2`*test_numeric_table_min.`int4`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int2/int4` = test_numeric_table_min.`int2`/test_numeric_table_min.`int4`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int2+uint4` = test_numeric_table_min.`int2`+test_numeric_table_min.`uint4`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int2-uint4` = test_numeric_table_min.`int2`-test_numeric_table_min.`uint4`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int2*uint4` = test_numeric_table_min.`int2`*test_numeric_table_min.`uint4`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int2/uint4` = test_numeric_table_min.`int2`/test_numeric_table_min.`uint4`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int2+int8` = test_numeric_table_min.`int2`+test_numeric_table_min.`int8`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int2-int8` = test_numeric_table_min.`int2`-test_numeric_table_min.`int8`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int2*int8` = test_numeric_table_min.`int2`*test_numeric_table_min.`int8`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int2/int8` = test_numeric_table_min.`int2`/test_numeric_table_min.`int8`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int2+uint8` = test_numeric_table_min.`int2`+test_numeric_table_min.`uint8`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int2-uint8` = test_numeric_table_min.`int2`-test_numeric_table_min.`uint8`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int2*uint8` = test_numeric_table_min.`int2`*test_numeric_table_min.`uint8`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int2/uint8` = test_numeric_table_min.`int2`/test_numeric_table_min.`uint8`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int2+float4` = test_numeric_table_min.`int2`+test_numeric_table_min.`float4`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int2-float4` = test_numeric_table_min.`int2`-test_numeric_table_min.`float4`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int2*float4` = test_numeric_table_min.`int2`*test_numeric_table_min.`float4`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int2/float4` = test_numeric_table_min.`int2`/test_numeric_table_min.`float4`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int2+float8` = test_numeric_table_min.`int2`+test_numeric_table_min.`float8`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int2-float8` = test_numeric_table_min.`int2`-test_numeric_table_min.`float8`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int2*float8` = test_numeric_table_min.`int2`*test_numeric_table_min.`float8`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int2/float8` = test_numeric_table_min.`int2`/test_numeric_table_min.`float8`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int2+numeric` = test_numeric_table_min.`int2`+test_numeric_table_min.`numeric`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int2-numeric` = test_numeric_table_min.`int2`-test_numeric_table_min.`numeric`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int2*numeric` = test_numeric_table_min.`int2`*test_numeric_table_min.`numeric`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int2/numeric` = test_numeric_table_min.`int2`/test_numeric_table_min.`numeric`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int2+bit1` = test_numeric_table_min.`int2`+test_numeric_table_min.`bit1`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int2-bit1` = test_numeric_table_min.`int2`-test_numeric_table_min.`bit1`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int2*bit1` = test_numeric_table_min.`int2`*test_numeric_table_min.`bit1`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int2/bit1` = test_numeric_table_min.`int2`/test_numeric_table_min.`bit1`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int2+bit64` = test_numeric_table_min.`int2`+test_numeric_table_min.`bit64`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int2-bit64` = test_numeric_table_min.`int2`-test_numeric_table_min.`bit64`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int2*bit64` = test_numeric_table_min.`int2`*test_numeric_table_min.`bit64`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int2/bit64` = test_numeric_table_min.`int2`/test_numeric_table_min.`bit64`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint2+int1` = test_numeric_table_min.`uint2`+test_numeric_table_min.`int1`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint2-int1` = test_numeric_table_min.`uint2`-test_numeric_table_min.`int1`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint2*int1` = test_numeric_table_min.`uint2`*test_numeric_table_min.`int1`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint2/int1` = test_numeric_table_min.`uint2`/test_numeric_table_min.`int1`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint2+uint1` = test_numeric_table_min.`uint2`+test_numeric_table_min.`uint1`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint2-uint1` = test_numeric_table_min.`uint2`-test_numeric_table_min.`uint1`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint2*uint1` = test_numeric_table_min.`uint2`*test_numeric_table_min.`uint1`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint2/uint1` = test_numeric_table_min.`uint2`/test_numeric_table_min.`uint1`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint2+int2` = test_numeric_table_min.`uint2`+test_numeric_table_min.`int2`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint2-int2` = test_numeric_table_min.`uint2`-test_numeric_table_min.`int2`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint2*int2` = test_numeric_table_min.`uint2`*test_numeric_table_min.`int2`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint2/int2` = test_numeric_table_min.`uint2`/test_numeric_table_min.`int2`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint2+uint2` = test_numeric_table_min.`uint2`+test_numeric_table_min.`uint2`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint2-uint2` = test_numeric_table_min.`uint2`-test_numeric_table_min.`uint2`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint2*uint2` = test_numeric_table_min.`uint2`*test_numeric_table_min.`uint2`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint2/uint2` = test_numeric_table_min.`uint2`/test_numeric_table_min.`uint2`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint2+int4` = test_numeric_table_min.`uint2`+test_numeric_table_min.`int4`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint2-int4` = test_numeric_table_min.`uint2`-test_numeric_table_min.`int4`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint2*int4` = test_numeric_table_min.`uint2`*test_numeric_table_min.`int4`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint2/int4` = test_numeric_table_min.`uint2`/test_numeric_table_min.`int4`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint2+uint4` = test_numeric_table_min.`uint2`+test_numeric_table_min.`uint4`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint2-uint4` = test_numeric_table_min.`uint2`-test_numeric_table_min.`uint4`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint2*uint4` = test_numeric_table_min.`uint2`*test_numeric_table_min.`uint4`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint2/uint4` = test_numeric_table_min.`uint2`/test_numeric_table_min.`uint4`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint2+int8` = test_numeric_table_min.`uint2`+test_numeric_table_min.`int8`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint2-int8` = test_numeric_table_min.`uint2`-test_numeric_table_min.`int8`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint2*int8` = test_numeric_table_min.`uint2`*test_numeric_table_min.`int8`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint2/int8` = test_numeric_table_min.`uint2`/test_numeric_table_min.`int8`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint2+uint8` = test_numeric_table_min.`uint2`+test_numeric_table_min.`uint8`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint2-uint8` = test_numeric_table_min.`uint2`-test_numeric_table_min.`uint8`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint2*uint8` = test_numeric_table_min.`uint2`*test_numeric_table_min.`uint8`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint2/uint8` = test_numeric_table_min.`uint2`/test_numeric_table_min.`uint8`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint2+float4` = test_numeric_table_min.`uint2`+test_numeric_table_min.`float4`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint2-float4` = test_numeric_table_min.`uint2`-test_numeric_table_min.`float4`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint2*float4` = test_numeric_table_min.`uint2`*test_numeric_table_min.`float4`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint2/float4` = test_numeric_table_min.`uint2`/test_numeric_table_min.`float4`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint2+float8` = test_numeric_table_min.`uint2`+test_numeric_table_min.`float8`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint2-float8` = test_numeric_table_min.`uint2`-test_numeric_table_min.`float8`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint2*float8` = test_numeric_table_min.`uint2`*test_numeric_table_min.`float8`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint2/float8` = test_numeric_table_min.`uint2`/test_numeric_table_min.`float8`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint2+numeric` = test_numeric_table_min.`uint2`+test_numeric_table_min.`numeric`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint2-numeric` = test_numeric_table_min.`uint2`-test_numeric_table_min.`numeric`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint2*numeric` = test_numeric_table_min.`uint2`*test_numeric_table_min.`numeric`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint2/numeric` = test_numeric_table_min.`uint2`/test_numeric_table_min.`numeric`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint2+bit1` = test_numeric_table_min.`uint2`+test_numeric_table_min.`bit1`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint2-bit1` = test_numeric_table_min.`uint2`-test_numeric_table_min.`bit1`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint2*bit1` = test_numeric_table_min.`uint2`*test_numeric_table_min.`bit1`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint2/bit1` = test_numeric_table_min.`uint2`/test_numeric_table_min.`bit1`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint2+bit64` = test_numeric_table_min.`uint2`+test_numeric_table_min.`bit64`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint2-bit64` = test_numeric_table_min.`uint2`-test_numeric_table_min.`bit64`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint2*bit64` = test_numeric_table_min.`uint2`*test_numeric_table_min.`bit64`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint2/bit64` = test_numeric_table_min.`uint2`/test_numeric_table_min.`bit64`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int4+int1` = test_numeric_table_min.`int4`+test_numeric_table_min.`int1`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int4-int1` = test_numeric_table_min.`int4`-test_numeric_table_min.`int1`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int4*int1` = test_numeric_table_min.`int4`*test_numeric_table_min.`int1`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int4/int1` = test_numeric_table_min.`int4`/test_numeric_table_min.`int1`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int4+uint1` = test_numeric_table_min.`int4`+test_numeric_table_min.`uint1`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int4-uint1` = test_numeric_table_min.`int4`-test_numeric_table_min.`uint1`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int4*uint1` = test_numeric_table_min.`int4`*test_numeric_table_min.`uint1`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int4/uint1` = test_numeric_table_min.`int4`/test_numeric_table_min.`uint1`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int4+int2` = test_numeric_table_min.`int4`+test_numeric_table_min.`int2`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int4-int2` = test_numeric_table_min.`int4`-test_numeric_table_min.`int2`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int4*int2` = test_numeric_table_min.`int4`*test_numeric_table_min.`int2`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int4/int2` = test_numeric_table_min.`int4`/test_numeric_table_min.`int2`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int4+uint2` = test_numeric_table_min.`int4`+test_numeric_table_min.`uint2`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int4-uint2` = test_numeric_table_min.`int4`-test_numeric_table_min.`uint2`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int4*uint2` = test_numeric_table_min.`int4`*test_numeric_table_min.`uint2`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int4/uint2` = test_numeric_table_min.`int4`/test_numeric_table_min.`uint2`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int4+int4` = test_numeric_table_min.`int4`+test_numeric_table_min.`int4`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int4-int4` = test_numeric_table_min.`int4`-test_numeric_table_min.`int4`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int4*int4` = test_numeric_table_min.`int4`*test_numeric_table_min.`int4`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int4/int4` = test_numeric_table_min.`int4`/test_numeric_table_min.`int4`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int4+uint4` = test_numeric_table_min.`int4`+test_numeric_table_min.`uint4`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int4-uint4` = test_numeric_table_min.`int4`-test_numeric_table_min.`uint4`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int4*uint4` = test_numeric_table_min.`int4`*test_numeric_table_min.`uint4`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int4/uint4` = test_numeric_table_min.`int4`/test_numeric_table_min.`uint4`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int4+int8` = test_numeric_table_min.`int4`+test_numeric_table_min.`int8`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int4-int8` = test_numeric_table_min.`int4`-test_numeric_table_min.`int8`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int4*int8` = test_numeric_table_min.`int4`*test_numeric_table_min.`int8`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int4/int8` = test_numeric_table_min.`int4`/test_numeric_table_min.`int8`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int4+uint8` = test_numeric_table_min.`int4`+test_numeric_table_min.`uint8`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int4-uint8` = test_numeric_table_min.`int4`-test_numeric_table_min.`uint8`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int4*uint8` = test_numeric_table_min.`int4`*test_numeric_table_min.`uint8`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int4/uint8` = test_numeric_table_min.`int4`/test_numeric_table_min.`uint8`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int4+float4` = test_numeric_table_min.`int4`+test_numeric_table_min.`float4`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int4-float4` = test_numeric_table_min.`int4`-test_numeric_table_min.`float4`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int4*float4` = test_numeric_table_min.`int4`*test_numeric_table_min.`float4`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int4/float4` = test_numeric_table_min.`int4`/test_numeric_table_min.`float4`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int4+float8` = test_numeric_table_min.`int4`+test_numeric_table_min.`float8`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int4-float8` = test_numeric_table_min.`int4`-test_numeric_table_min.`float8`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int4*float8` = test_numeric_table_min.`int4`*test_numeric_table_min.`float8`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int4/float8` = test_numeric_table_min.`int4`/test_numeric_table_min.`float8`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int4+numeric` = test_numeric_table_min.`int4`+test_numeric_table_min.`numeric`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int4-numeric` = test_numeric_table_min.`int4`-test_numeric_table_min.`numeric`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int4*numeric` = test_numeric_table_min.`int4`*test_numeric_table_min.`numeric`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int4/numeric` = test_numeric_table_min.`int4`/test_numeric_table_min.`numeric`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int4+bit1` = test_numeric_table_min.`int4`+test_numeric_table_min.`bit1`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int4-bit1` = test_numeric_table_min.`int4`-test_numeric_table_min.`bit1`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int4*bit1` = test_numeric_table_min.`int4`*test_numeric_table_min.`bit1`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int4/bit1` = test_numeric_table_min.`int4`/test_numeric_table_min.`bit1`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int4+bit64` = test_numeric_table_min.`int4`+test_numeric_table_min.`bit64`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int4-bit64` = test_numeric_table_min.`int4`-test_numeric_table_min.`bit64`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int4*bit64` = test_numeric_table_min.`int4`*test_numeric_table_min.`bit64`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int4/bit64` = test_numeric_table_min.`int4`/test_numeric_table_min.`bit64`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint4+int1` = test_numeric_table_min.`uint4`+test_numeric_table_min.`int1`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint4-int1` = test_numeric_table_min.`uint4`-test_numeric_table_min.`int1`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint4*int1` = test_numeric_table_min.`uint4`*test_numeric_table_min.`int1`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint4/int1` = test_numeric_table_min.`uint4`/test_numeric_table_min.`int1`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint4+uint1` = test_numeric_table_min.`uint4`+test_numeric_table_min.`uint1`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint4-uint1` = test_numeric_table_min.`uint4`-test_numeric_table_min.`uint1`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint4*uint1` = test_numeric_table_min.`uint4`*test_numeric_table_min.`uint1`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint4/uint1` = test_numeric_table_min.`uint4`/test_numeric_table_min.`uint1`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint4+int2` = test_numeric_table_min.`uint4`+test_numeric_table_min.`int2`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint4-int2` = test_numeric_table_min.`uint4`-test_numeric_table_min.`int2`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint4*int2` = test_numeric_table_min.`uint4`*test_numeric_table_min.`int2`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint4/int2` = test_numeric_table_min.`uint4`/test_numeric_table_min.`int2`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint4+uint2` = test_numeric_table_min.`uint4`+test_numeric_table_min.`uint2`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint4-uint2` = test_numeric_table_min.`uint4`-test_numeric_table_min.`uint2`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint4*uint2` = test_numeric_table_min.`uint4`*test_numeric_table_min.`uint2`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint4/uint2` = test_numeric_table_min.`uint4`/test_numeric_table_min.`uint2`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint4+int4` = test_numeric_table_min.`uint4`+test_numeric_table_min.`int4`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint4-int4` = test_numeric_table_min.`uint4`-test_numeric_table_min.`int4`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint4*int4` = test_numeric_table_min.`uint4`*test_numeric_table_min.`int4`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint4/int4` = test_numeric_table_min.`uint4`/test_numeric_table_min.`int4`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint4+uint4` = test_numeric_table_min.`uint4`+test_numeric_table_min.`uint4`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint4-uint4` = test_numeric_table_min.`uint4`-test_numeric_table_min.`uint4`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint4*uint4` = test_numeric_table_min.`uint4`*test_numeric_table_min.`uint4`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint4/uint4` = test_numeric_table_min.`uint4`/test_numeric_table_min.`uint4`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint4+int8` = test_numeric_table_min.`uint4`+test_numeric_table_min.`int8`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint4-int8` = test_numeric_table_min.`uint4`-test_numeric_table_min.`int8`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint4*int8` = test_numeric_table_min.`uint4`*test_numeric_table_min.`int8`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint4/int8` = test_numeric_table_min.`uint4`/test_numeric_table_min.`int8`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint4+uint8` = test_numeric_table_min.`uint4`+test_numeric_table_min.`uint8`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint4-uint8` = test_numeric_table_min.`uint4`-test_numeric_table_min.`uint8`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint4*uint8` = test_numeric_table_min.`uint4`*test_numeric_table_min.`uint8`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint4/uint8` = test_numeric_table_min.`uint4`/test_numeric_table_min.`uint8`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint4+float4` = test_numeric_table_min.`uint4`+test_numeric_table_min.`float4`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint4-float4` = test_numeric_table_min.`uint4`-test_numeric_table_min.`float4`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint4*float4` = test_numeric_table_min.`uint4`*test_numeric_table_min.`float4`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint4/float4` = test_numeric_table_min.`uint4`/test_numeric_table_min.`float4`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint4+float8` = test_numeric_table_min.`uint4`+test_numeric_table_min.`float8`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint4-float8` = test_numeric_table_min.`uint4`-test_numeric_table_min.`float8`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint4*float8` = test_numeric_table_min.`uint4`*test_numeric_table_min.`float8`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint4/float8` = test_numeric_table_min.`uint4`/test_numeric_table_min.`float8`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint4+numeric` = test_numeric_table_min.`uint4`+test_numeric_table_min.`numeric`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint4-numeric` = test_numeric_table_min.`uint4`-test_numeric_table_min.`numeric`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint4*numeric` = test_numeric_table_min.`uint4`*test_numeric_table_min.`numeric`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint4/numeric` = test_numeric_table_min.`uint4`/test_numeric_table_min.`numeric`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint4+bit1` = test_numeric_table_min.`uint4`+test_numeric_table_min.`bit1`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint4-bit1` = test_numeric_table_min.`uint4`-test_numeric_table_min.`bit1`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint4*bit1` = test_numeric_table_min.`uint4`*test_numeric_table_min.`bit1`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint4/bit1` = test_numeric_table_min.`uint4`/test_numeric_table_min.`bit1`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint4+bit64` = test_numeric_table_min.`uint4`+test_numeric_table_min.`bit64`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint4-bit64` = test_numeric_table_min.`uint4`-test_numeric_table_min.`bit64`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint4*bit64` = test_numeric_table_min.`uint4`*test_numeric_table_min.`bit64`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint4/bit64` = test_numeric_table_min.`uint4`/test_numeric_table_min.`bit64`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int8+int1` = test_numeric_table_min.`int8`+test_numeric_table_min.`int1`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int8-int1` = test_numeric_table_min.`int8`-test_numeric_table_min.`int1`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int8*int1` = test_numeric_table_min.`int8`*test_numeric_table_min.`int1`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int8/int1` = test_numeric_table_min.`int8`/test_numeric_table_min.`int1`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int8+uint1` = test_numeric_table_min.`int8`+test_numeric_table_min.`uint1`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int8-uint1` = test_numeric_table_min.`int8`-test_numeric_table_min.`uint1`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int8*uint1` = test_numeric_table_min.`int8`*test_numeric_table_min.`uint1`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int8/uint1` = test_numeric_table_min.`int8`/test_numeric_table_min.`uint1`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int8+int2` = test_numeric_table_min.`int8`+test_numeric_table_min.`int2`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int8-int2` = test_numeric_table_min.`int8`-test_numeric_table_min.`int2`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int8*int2` = test_numeric_table_min.`int8`*test_numeric_table_min.`int2`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int8/int2` = test_numeric_table_min.`int8`/test_numeric_table_min.`int2`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int8+uint2` = test_numeric_table_min.`int8`+test_numeric_table_min.`uint2`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int8-uint2` = test_numeric_table_min.`int8`-test_numeric_table_min.`uint2`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int8*uint2` = test_numeric_table_min.`int8`*test_numeric_table_min.`uint2`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int8/uint2` = test_numeric_table_min.`int8`/test_numeric_table_min.`uint2`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int8+int4` = test_numeric_table_min.`int8`+test_numeric_table_min.`int4`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int8-int4` = test_numeric_table_min.`int8`-test_numeric_table_min.`int4`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int8*int4` = test_numeric_table_min.`int8`*test_numeric_table_min.`int4`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int8/int4` = test_numeric_table_min.`int8`/test_numeric_table_min.`int4`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int8+uint4` = test_numeric_table_min.`int8`+test_numeric_table_min.`uint4`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int8-uint4` = test_numeric_table_min.`int8`-test_numeric_table_min.`uint4`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int8*uint4` = test_numeric_table_min.`int8`*test_numeric_table_min.`uint4`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int8/uint4` = test_numeric_table_min.`int8`/test_numeric_table_min.`uint4`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int8+int8` = test_numeric_table_min.`int8`+test_numeric_table_min.`int8`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int8-int8` = test_numeric_table_min.`int8`-test_numeric_table_min.`int8`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int8*int8` = test_numeric_table_min.`int8`*test_numeric_table_min.`int8`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int8/int8` = test_numeric_table_min.`int8`/test_numeric_table_min.`int8`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int8+uint8` = test_numeric_table_min.`int8`+test_numeric_table_min.`uint8`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int8-uint8` = test_numeric_table_min.`int8`-test_numeric_table_min.`uint8`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int8*uint8` = test_numeric_table_min.`int8`*test_numeric_table_min.`uint8`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int8/uint8` = test_numeric_table_min.`int8`/test_numeric_table_min.`uint8`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int8+float4` = test_numeric_table_min.`int8`+test_numeric_table_min.`float4`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int8-float4` = test_numeric_table_min.`int8`-test_numeric_table_min.`float4`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int8*float4` = test_numeric_table_min.`int8`*test_numeric_table_min.`float4`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int8/float4` = test_numeric_table_min.`int8`/test_numeric_table_min.`float4`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int8+float8` = test_numeric_table_min.`int8`+test_numeric_table_min.`float8`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int8-float8` = test_numeric_table_min.`int8`-test_numeric_table_min.`float8`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int8*float8` = test_numeric_table_min.`int8`*test_numeric_table_min.`float8`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int8/float8` = test_numeric_table_min.`int8`/test_numeric_table_min.`float8`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int8+numeric` = test_numeric_table_min.`int8`+test_numeric_table_min.`numeric`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int8-numeric` = test_numeric_table_min.`int8`-test_numeric_table_min.`numeric`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int8*numeric` = test_numeric_table_min.`int8`*test_numeric_table_min.`numeric`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int8/numeric` = test_numeric_table_min.`int8`/test_numeric_table_min.`numeric`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int8+bit1` = test_numeric_table_min.`int8`+test_numeric_table_min.`bit1`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int8-bit1` = test_numeric_table_min.`int8`-test_numeric_table_min.`bit1`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int8*bit1` = test_numeric_table_min.`int8`*test_numeric_table_min.`bit1`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int8/bit1` = test_numeric_table_min.`int8`/test_numeric_table_min.`bit1`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int8+bit64` = test_numeric_table_min.`int8`+test_numeric_table_min.`bit64`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int8-bit64` = test_numeric_table_min.`int8`-test_numeric_table_min.`bit64`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int8*bit64` = test_numeric_table_min.`int8`*test_numeric_table_min.`bit64`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`int8/bit64` = test_numeric_table_min.`int8`/test_numeric_table_min.`bit64`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint8+int1` = test_numeric_table_min.`uint8`+test_numeric_table_min.`int1`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint8-int1` = test_numeric_table_min.`uint8`-test_numeric_table_min.`int1`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint8*int1` = test_numeric_table_min.`uint8`*test_numeric_table_min.`int1`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint8/int1` = test_numeric_table_min.`uint8`/test_numeric_table_min.`int1`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint8+uint1` = test_numeric_table_min.`uint8`+test_numeric_table_min.`uint1`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint8-uint1` = test_numeric_table_min.`uint8`-test_numeric_table_min.`uint1`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint8*uint1` = test_numeric_table_min.`uint8`*test_numeric_table_min.`uint1`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint8/uint1` = test_numeric_table_min.`uint8`/test_numeric_table_min.`uint1`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint8+int2` = test_numeric_table_min.`uint8`+test_numeric_table_min.`int2`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint8-int2` = test_numeric_table_min.`uint8`-test_numeric_table_min.`int2`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint8*int2` = test_numeric_table_min.`uint8`*test_numeric_table_min.`int2`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint8/int2` = test_numeric_table_min.`uint8`/test_numeric_table_min.`int2`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint8+uint2` = test_numeric_table_min.`uint8`+test_numeric_table_min.`uint2`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint8-uint2` = test_numeric_table_min.`uint8`-test_numeric_table_min.`uint2`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint8*uint2` = test_numeric_table_min.`uint8`*test_numeric_table_min.`uint2`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint8/uint2` = test_numeric_table_min.`uint8`/test_numeric_table_min.`uint2`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint8+int4` = test_numeric_table_min.`uint8`+test_numeric_table_min.`int4`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint8-int4` = test_numeric_table_min.`uint8`-test_numeric_table_min.`int4`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint8*int4` = test_numeric_table_min.`uint8`*test_numeric_table_min.`int4`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint8/int4` = test_numeric_table_min.`uint8`/test_numeric_table_min.`int4`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint8+uint4` = test_numeric_table_min.`uint8`+test_numeric_table_min.`uint4`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint8-uint4` = test_numeric_table_min.`uint8`-test_numeric_table_min.`uint4`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint8*uint4` = test_numeric_table_min.`uint8`*test_numeric_table_min.`uint4`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint8/uint4` = test_numeric_table_min.`uint8`/test_numeric_table_min.`uint4`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint8+int8` = test_numeric_table_min.`uint8`+test_numeric_table_min.`int8`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint8-int8` = test_numeric_table_min.`uint8`-test_numeric_table_min.`int8`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint8*int8` = test_numeric_table_min.`uint8`*test_numeric_table_min.`int8`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint8/int8` = test_numeric_table_min.`uint8`/test_numeric_table_min.`int8`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint8+uint8` = test_numeric_table_min.`uint8`+test_numeric_table_min.`uint8`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint8-uint8` = test_numeric_table_min.`uint8`-test_numeric_table_min.`uint8`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint8*uint8` = test_numeric_table_min.`uint8`*test_numeric_table_min.`uint8`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint8/uint8` = test_numeric_table_min.`uint8`/test_numeric_table_min.`uint8`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint8+float4` = test_numeric_table_min.`uint8`+test_numeric_table_min.`float4`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint8-float4` = test_numeric_table_min.`uint8`-test_numeric_table_min.`float4`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint8*float4` = test_numeric_table_min.`uint8`*test_numeric_table_min.`float4`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint8/float4` = test_numeric_table_min.`uint8`/test_numeric_table_min.`float4`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint8+float8` = test_numeric_table_min.`uint8`+test_numeric_table_min.`float8`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint8-float8` = test_numeric_table_min.`uint8`-test_numeric_table_min.`float8`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint8*float8` = test_numeric_table_min.`uint8`*test_numeric_table_min.`float8`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint8/float8` = test_numeric_table_min.`uint8`/test_numeric_table_min.`float8`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint8+numeric` = test_numeric_table_min.`uint8`+test_numeric_table_min.`numeric`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint8-numeric` = test_numeric_table_min.`uint8`-test_numeric_table_min.`numeric`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint8*numeric` = test_numeric_table_min.`uint8`*test_numeric_table_min.`numeric`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint8/numeric` = test_numeric_table_min.`uint8`/test_numeric_table_min.`numeric`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint8+bit1` = test_numeric_table_min.`uint8`+test_numeric_table_min.`bit1`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint8-bit1` = test_numeric_table_min.`uint8`-test_numeric_table_min.`bit1`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint8*bit1` = test_numeric_table_min.`uint8`*test_numeric_table_min.`bit1`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint8/bit1` = test_numeric_table_min.`uint8`/test_numeric_table_min.`bit1`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint8+bit64` = test_numeric_table_min.`uint8`+test_numeric_table_min.`bit64`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint8-bit64` = test_numeric_table_min.`uint8`-test_numeric_table_min.`bit64`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint8*bit64` = test_numeric_table_min.`uint8`*test_numeric_table_min.`bit64`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`uint8/bit64` = test_numeric_table_min.`uint8`/test_numeric_table_min.`bit64`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`float4+int1` = test_numeric_table_min.`float4`+test_numeric_table_min.`int1`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`float4-int1` = test_numeric_table_min.`float4`-test_numeric_table_min.`int1`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`float4*int1` = test_numeric_table_min.`float4`*test_numeric_table_min.`int1`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`float4/int1` = test_numeric_table_min.`float4`/test_numeric_table_min.`int1`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`float4+uint1` = test_numeric_table_min.`float4`+test_numeric_table_min.`uint1`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`float4-uint1` = test_numeric_table_min.`float4`-test_numeric_table_min.`uint1`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`float4*uint1` = test_numeric_table_min.`float4`*test_numeric_table_min.`uint1`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`float4/uint1` = test_numeric_table_min.`float4`/test_numeric_table_min.`uint1`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`float4+int2` = test_numeric_table_min.`float4`+test_numeric_table_min.`int2`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`float4-int2` = test_numeric_table_min.`float4`-test_numeric_table_min.`int2`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`float4*int2` = test_numeric_table_min.`float4`*test_numeric_table_min.`int2`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`float4/int2` = test_numeric_table_min.`float4`/test_numeric_table_min.`int2`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`float4+uint2` = test_numeric_table_min.`float4`+test_numeric_table_min.`uint2`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`float4-uint2` = test_numeric_table_min.`float4`-test_numeric_table_min.`uint2`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`float4*uint2` = test_numeric_table_min.`float4`*test_numeric_table_min.`uint2`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`float4/uint2` = test_numeric_table_min.`float4`/test_numeric_table_min.`uint2`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`float4+int4` = test_numeric_table_min.`float4`+test_numeric_table_min.`int4`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`float4-int4` = test_numeric_table_min.`float4`-test_numeric_table_min.`int4`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`float4*int4` = test_numeric_table_min.`float4`*test_numeric_table_min.`int4`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`float4/int4` = test_numeric_table_min.`float4`/test_numeric_table_min.`int4`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`float4+uint4` = test_numeric_table_min.`float4`+test_numeric_table_min.`uint4`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`float4-uint4` = test_numeric_table_min.`float4`-test_numeric_table_min.`uint4`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`float4*uint4` = test_numeric_table_min.`float4`*test_numeric_table_min.`uint4`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`float4/uint4` = test_numeric_table_min.`float4`/test_numeric_table_min.`uint4`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`float4+int8` = test_numeric_table_min.`float4`+test_numeric_table_min.`int8`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`float4-int8` = test_numeric_table_min.`float4`-test_numeric_table_min.`int8`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`float4*int8` = test_numeric_table_min.`float4`*test_numeric_table_min.`int8`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`float4/int8` = test_numeric_table_min.`float4`/test_numeric_table_min.`int8`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`float4+uint8` = test_numeric_table_min.`float4`+test_numeric_table_min.`uint8`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`float4-uint8` = test_numeric_table_min.`float4`-test_numeric_table_min.`uint8`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`float4*uint8` = test_numeric_table_min.`float4`*test_numeric_table_min.`uint8`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`float4/uint8` = test_numeric_table_min.`float4`/test_numeric_table_min.`uint8`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`float4+float4` = test_numeric_table_min.`float4`+test_numeric_table_min.`float4`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`float4-float4` = test_numeric_table_min.`float4`-test_numeric_table_min.`float4`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`float4*float4` = test_numeric_table_min.`float4`*test_numeric_table_min.`float4`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`float4/float4` = test_numeric_table_min.`float4`/test_numeric_table_min.`float4`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`float4+float8` = test_numeric_table_min.`float4`+test_numeric_table_min.`float8`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`float4-float8` = test_numeric_table_min.`float4`-test_numeric_table_min.`float8`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`float4*float8` = test_numeric_table_min.`float4`*test_numeric_table_min.`float8`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`float4/float8` = test_numeric_table_min.`float4`/test_numeric_table_min.`float8`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`float4+numeric` = test_numeric_table_min.`float4`+test_numeric_table_min.`numeric`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`float4-numeric` = test_numeric_table_min.`float4`-test_numeric_table_min.`numeric`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`float4*numeric` = test_numeric_table_min.`float4`*test_numeric_table_min.`numeric`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`float4/numeric` = test_numeric_table_min.`float4`/test_numeric_table_min.`numeric`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`float4+bit1` = test_numeric_table_min.`float4`+test_numeric_table_min.`bit1`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`float4-bit1` = test_numeric_table_min.`float4`-test_numeric_table_min.`bit1`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`float4*bit1` = test_numeric_table_min.`float4`*test_numeric_table_min.`bit1`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`float4/bit1` = test_numeric_table_min.`float4`/test_numeric_table_min.`bit1`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`float4+bit64` = test_numeric_table_min.`float4`+test_numeric_table_min.`bit64`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`float4-bit64` = test_numeric_table_min.`float4`-test_numeric_table_min.`bit64`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`float4*bit64` = test_numeric_table_min.`float4`*test_numeric_table_min.`bit64`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`float4/bit64` = test_numeric_table_min.`float4`/test_numeric_table_min.`bit64`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`float8+int1` = test_numeric_table_min.`float8`+test_numeric_table_min.`int1`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`float8-int1` = test_numeric_table_min.`float8`-test_numeric_table_min.`int1`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`float8*int1` = test_numeric_table_min.`float8`*test_numeric_table_min.`int1`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`float8/int1` = test_numeric_table_min.`float8`/test_numeric_table_min.`int1`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`float8+uint1` = test_numeric_table_min.`float8`+test_numeric_table_min.`uint1`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`float8-uint1` = test_numeric_table_min.`float8`-test_numeric_table_min.`uint1`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`float8*uint1` = test_numeric_table_min.`float8`*test_numeric_table_min.`uint1`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`float8/uint1` = test_numeric_table_min.`float8`/test_numeric_table_min.`uint1`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`float8+int2` = test_numeric_table_min.`float8`+test_numeric_table_min.`int2`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`float8-int2` = test_numeric_table_min.`float8`-test_numeric_table_min.`int2`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`float8*int2` = test_numeric_table_min.`float8`*test_numeric_table_min.`int2`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`float8/int2` = test_numeric_table_min.`float8`/test_numeric_table_min.`int2`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`float8+uint2` = test_numeric_table_min.`float8`+test_numeric_table_min.`uint2`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`float8-uint2` = test_numeric_table_min.`float8`-test_numeric_table_min.`uint2`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`float8*uint2` = test_numeric_table_min.`float8`*test_numeric_table_min.`uint2`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`float8/uint2` = test_numeric_table_min.`float8`/test_numeric_table_min.`uint2`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`float8+int4` = test_numeric_table_min.`float8`+test_numeric_table_min.`int4`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`float8-int4` = test_numeric_table_min.`float8`-test_numeric_table_min.`int4`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`float8*int4` = test_numeric_table_min.`float8`*test_numeric_table_min.`int4`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`float8/int4` = test_numeric_table_min.`float8`/test_numeric_table_min.`int4`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`float8+uint4` = test_numeric_table_min.`float8`+test_numeric_table_min.`uint4`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`float8-uint4` = test_numeric_table_min.`float8`-test_numeric_table_min.`uint4`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`float8*uint4` = test_numeric_table_min.`float8`*test_numeric_table_min.`uint4`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`float8/uint4` = test_numeric_table_min.`float8`/test_numeric_table_min.`uint4`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`float8+int8` = test_numeric_table_min.`float8`+test_numeric_table_min.`int8`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`float8-int8` = test_numeric_table_min.`float8`-test_numeric_table_min.`int8`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`float8*int8` = test_numeric_table_min.`float8`*test_numeric_table_min.`int8`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`float8/int8` = test_numeric_table_min.`float8`/test_numeric_table_min.`int8`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`float8+uint8` = test_numeric_table_min.`float8`+test_numeric_table_min.`uint8`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`float8-uint8` = test_numeric_table_min.`float8`-test_numeric_table_min.`uint8`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`float8*uint8` = test_numeric_table_min.`float8`*test_numeric_table_min.`uint8`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`float8/uint8` = test_numeric_table_min.`float8`/test_numeric_table_min.`uint8`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`float8+float4` = test_numeric_table_min.`float8`+test_numeric_table_min.`float4`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`float8-float4` = test_numeric_table_min.`float8`-test_numeric_table_min.`float4`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`float8*float4` = test_numeric_table_min.`float8`*test_numeric_table_min.`float4`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`float8/float4` = test_numeric_table_min.`float8`/test_numeric_table_min.`float4`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`float8+float8` = test_numeric_table_min.`float8`+test_numeric_table_min.`float8`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`float8-float8` = test_numeric_table_min.`float8`-test_numeric_table_min.`float8`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`float8*float8` = test_numeric_table_min.`float8`*test_numeric_table_min.`float8`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`float8/float8` = test_numeric_table_min.`float8`/test_numeric_table_min.`float8`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`float8+numeric` = test_numeric_table_min.`float8`+test_numeric_table_min.`numeric`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`float8-numeric` = test_numeric_table_min.`float8`-test_numeric_table_min.`numeric`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`float8*numeric` = test_numeric_table_min.`float8`*test_numeric_table_min.`numeric`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`float8/numeric` = test_numeric_table_min.`float8`/test_numeric_table_min.`numeric`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`float8+bit1` = test_numeric_table_min.`float8`+test_numeric_table_min.`bit1`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`float8-bit1` = test_numeric_table_min.`float8`-test_numeric_table_min.`bit1`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`float8*bit1` = test_numeric_table_min.`float8`*test_numeric_table_min.`bit1`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`float8/bit1` = test_numeric_table_min.`float8`/test_numeric_table_min.`bit1`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`float8+bit64` = test_numeric_table_min.`float8`+test_numeric_table_min.`bit64`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`float8-bit64` = test_numeric_table_min.`float8`-test_numeric_table_min.`bit64`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`float8*bit64` = test_numeric_table_min.`float8`*test_numeric_table_min.`bit64`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`float8/bit64` = test_numeric_table_min.`float8`/test_numeric_table_min.`bit64`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`numeric+int1` = test_numeric_table_min.`numeric`+test_numeric_table_min.`int1`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`numeric-int1` = test_numeric_table_min.`numeric`-test_numeric_table_min.`int1`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`numeric*int1` = test_numeric_table_min.`numeric`*test_numeric_table_min.`int1`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`numeric/int1` = test_numeric_table_min.`numeric`/test_numeric_table_min.`int1`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`numeric+uint1` = test_numeric_table_min.`numeric`+test_numeric_table_min.`uint1`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`numeric-uint1` = test_numeric_table_min.`numeric`-test_numeric_table_min.`uint1`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`numeric*uint1` = test_numeric_table_min.`numeric`*test_numeric_table_min.`uint1`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`numeric/uint1` = test_numeric_table_min.`numeric`/test_numeric_table_min.`uint1`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`numeric+int2` = test_numeric_table_min.`numeric`+test_numeric_table_min.`int2`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`numeric-int2` = test_numeric_table_min.`numeric`-test_numeric_table_min.`int2`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`numeric*int2` = test_numeric_table_min.`numeric`*test_numeric_table_min.`int2`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`numeric/int2` = test_numeric_table_min.`numeric`/test_numeric_table_min.`int2`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`numeric+uint2` = test_numeric_table_min.`numeric`+test_numeric_table_min.`uint2`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`numeric-uint2` = test_numeric_table_min.`numeric`-test_numeric_table_min.`uint2`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`numeric*uint2` = test_numeric_table_min.`numeric`*test_numeric_table_min.`uint2`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`numeric/uint2` = test_numeric_table_min.`numeric`/test_numeric_table_min.`uint2`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`numeric+int4` = test_numeric_table_min.`numeric`+test_numeric_table_min.`int4`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`numeric-int4` = test_numeric_table_min.`numeric`-test_numeric_table_min.`int4`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`numeric*int4` = test_numeric_table_min.`numeric`*test_numeric_table_min.`int4`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`numeric/int4` = test_numeric_table_min.`numeric`/test_numeric_table_min.`int4`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`numeric+uint4` = test_numeric_table_min.`numeric`+test_numeric_table_min.`uint4`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`numeric-uint4` = test_numeric_table_min.`numeric`-test_numeric_table_min.`uint4`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`numeric*uint4` = test_numeric_table_min.`numeric`*test_numeric_table_min.`uint4`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`numeric/uint4` = test_numeric_table_min.`numeric`/test_numeric_table_min.`uint4`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`numeric+int8` = test_numeric_table_min.`numeric`+test_numeric_table_min.`int8`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`numeric-int8` = test_numeric_table_min.`numeric`-test_numeric_table_min.`int8`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`numeric*int8` = test_numeric_table_min.`numeric`*test_numeric_table_min.`int8`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`numeric/int8` = test_numeric_table_min.`numeric`/test_numeric_table_min.`int8`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`numeric+uint8` = test_numeric_table_min.`numeric`+test_numeric_table_min.`uint8`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`numeric-uint8` = test_numeric_table_min.`numeric`-test_numeric_table_min.`uint8`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`numeric*uint8` = test_numeric_table_min.`numeric`*test_numeric_table_min.`uint8`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`numeric/uint8` = test_numeric_table_min.`numeric`/test_numeric_table_min.`uint8`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`numeric+float4` = test_numeric_table_min.`numeric`+test_numeric_table_min.`float4`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`numeric-float4` = test_numeric_table_min.`numeric`-test_numeric_table_min.`float4`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`numeric*float4` = test_numeric_table_min.`numeric`*test_numeric_table_min.`float4`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`numeric/float4` = test_numeric_table_min.`numeric`/test_numeric_table_min.`float4`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`numeric+float8` = test_numeric_table_min.`numeric`+test_numeric_table_min.`float8`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`numeric-float8` = test_numeric_table_min.`numeric`-test_numeric_table_min.`float8`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`numeric*float8` = test_numeric_table_min.`numeric`*test_numeric_table_min.`float8`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`numeric/float8` = test_numeric_table_min.`numeric`/test_numeric_table_min.`float8`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`numeric+numeric` = test_numeric_table_min.`numeric`+test_numeric_table_min.`numeric`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`numeric-numeric` = test_numeric_table_min.`numeric`-test_numeric_table_min.`numeric`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`numeric*numeric` = test_numeric_table_min.`numeric`*test_numeric_table_min.`numeric`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`numeric/numeric` = test_numeric_table_min.`numeric`/test_numeric_table_min.`numeric`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`numeric+bit1` = test_numeric_table_min.`numeric`+test_numeric_table_min.`bit1`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`numeric-bit1` = test_numeric_table_min.`numeric`-test_numeric_table_min.`bit1`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`numeric*bit1` = test_numeric_table_min.`numeric`*test_numeric_table_min.`bit1`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`numeric/bit1` = test_numeric_table_min.`numeric`/test_numeric_table_min.`bit1`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`numeric+bit64` = test_numeric_table_min.`numeric`+test_numeric_table_min.`bit64`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`numeric-bit64` = test_numeric_table_min.`numeric`-test_numeric_table_min.`bit64`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`numeric*bit64` = test_numeric_table_min.`numeric`*test_numeric_table_min.`bit64`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`numeric/bit64` = test_numeric_table_min.`numeric`/test_numeric_table_min.`bit64`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`bit1+int1` = test_numeric_table_min.`bit1`+test_numeric_table_min.`int1`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`bit1-int1` = test_numeric_table_min.`bit1`-test_numeric_table_min.`int1`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`bit1*int1` = test_numeric_table_min.`bit1`*test_numeric_table_min.`int1`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`bit1/int1` = test_numeric_table_min.`bit1`/test_numeric_table_min.`int1`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`bit1+uint1` = test_numeric_table_min.`bit1`+test_numeric_table_min.`uint1`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`bit1-uint1` = test_numeric_table_min.`bit1`-test_numeric_table_min.`uint1`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`bit1*uint1` = test_numeric_table_min.`bit1`*test_numeric_table_min.`uint1`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`bit1/uint1` = test_numeric_table_min.`bit1`/test_numeric_table_min.`uint1`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`bit1+int2` = test_numeric_table_min.`bit1`+test_numeric_table_min.`int2`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`bit1-int2` = test_numeric_table_min.`bit1`-test_numeric_table_min.`int2`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`bit1*int2` = test_numeric_table_min.`bit1`*test_numeric_table_min.`int2`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`bit1/int2` = test_numeric_table_min.`bit1`/test_numeric_table_min.`int2`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`bit1+uint2` = test_numeric_table_min.`bit1`+test_numeric_table_min.`uint2`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`bit1-uint2` = test_numeric_table_min.`bit1`-test_numeric_table_min.`uint2`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`bit1*uint2` = test_numeric_table_min.`bit1`*test_numeric_table_min.`uint2`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`bit1/uint2` = test_numeric_table_min.`bit1`/test_numeric_table_min.`uint2`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`bit1+int4` = test_numeric_table_min.`bit1`+test_numeric_table_min.`int4`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`bit1-int4` = test_numeric_table_min.`bit1`-test_numeric_table_min.`int4`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`bit1*int4` = test_numeric_table_min.`bit1`*test_numeric_table_min.`int4`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`bit1/int4` = test_numeric_table_min.`bit1`/test_numeric_table_min.`int4`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`bit1+uint4` = test_numeric_table_min.`bit1`+test_numeric_table_min.`uint4`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`bit1-uint4` = test_numeric_table_min.`bit1`-test_numeric_table_min.`uint4`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`bit1*uint4` = test_numeric_table_min.`bit1`*test_numeric_table_min.`uint4`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`bit1/uint4` = test_numeric_table_min.`bit1`/test_numeric_table_min.`uint4`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`bit1+int8` = test_numeric_table_min.`bit1`+test_numeric_table_min.`int8`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`bit1-int8` = test_numeric_table_min.`bit1`-test_numeric_table_min.`int8`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`bit1*int8` = test_numeric_table_min.`bit1`*test_numeric_table_min.`int8`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`bit1/int8` = test_numeric_table_min.`bit1`/test_numeric_table_min.`int8`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`bit1+uint8` = test_numeric_table_min.`bit1`+test_numeric_table_min.`uint8`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`bit1-uint8` = test_numeric_table_min.`bit1`-test_numeric_table_min.`uint8`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`bit1*uint8` = test_numeric_table_min.`bit1`*test_numeric_table_min.`uint8`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`bit1/uint8` = test_numeric_table_min.`bit1`/test_numeric_table_min.`uint8`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`bit1+float4` = test_numeric_table_min.`bit1`+test_numeric_table_min.`float4`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`bit1-float4` = test_numeric_table_min.`bit1`-test_numeric_table_min.`float4`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`bit1*float4` = test_numeric_table_min.`bit1`*test_numeric_table_min.`float4`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`bit1/float4` = test_numeric_table_min.`bit1`/test_numeric_table_min.`float4`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`bit1+float8` = test_numeric_table_min.`bit1`+test_numeric_table_min.`float8`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`bit1-float8` = test_numeric_table_min.`bit1`-test_numeric_table_min.`float8`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`bit1*float8` = test_numeric_table_min.`bit1`*test_numeric_table_min.`float8`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`bit1/float8` = test_numeric_table_min.`bit1`/test_numeric_table_min.`float8`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`bit1+numeric` = test_numeric_table_min.`bit1`+test_numeric_table_min.`numeric`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`bit1-numeric` = test_numeric_table_min.`bit1`-test_numeric_table_min.`numeric`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`bit1*numeric` = test_numeric_table_min.`bit1`*test_numeric_table_min.`numeric`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`bit1/numeric` = test_numeric_table_min.`bit1`/test_numeric_table_min.`numeric`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`bit1+bit1` = test_numeric_table_min.`bit1`+test_numeric_table_min.`bit1`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`bit1-bit1` = test_numeric_table_min.`bit1`-test_numeric_table_min.`bit1`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`bit1*bit1` = test_numeric_table_min.`bit1`*test_numeric_table_min.`bit1`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`bit1/bit1` = test_numeric_table_min.`bit1`/test_numeric_table_min.`bit1`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`bit1+bit64` = test_numeric_table_min.`bit1`+test_numeric_table_min.`bit64`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`bit1-bit64` = test_numeric_table_min.`bit1`-test_numeric_table_min.`bit64`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`bit1*bit64` = test_numeric_table_min.`bit1`*test_numeric_table_min.`bit64`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`bit1/bit64` = test_numeric_table_min.`bit1`/test_numeric_table_min.`bit64`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`bit64+int1` = test_numeric_table_min.`bit64`+test_numeric_table_min.`int1`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`bit64-int1` = test_numeric_table_min.`bit64`-test_numeric_table_min.`int1`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`bit64*int1` = test_numeric_table_min.`bit64`*test_numeric_table_min.`int1`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`bit64/int1` = test_numeric_table_min.`bit64`/test_numeric_table_min.`int1`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`bit64+uint1` = test_numeric_table_min.`bit64`+test_numeric_table_min.`uint1`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`bit64-uint1` = test_numeric_table_min.`bit64`-test_numeric_table_min.`uint1`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`bit64*uint1` = test_numeric_table_min.`bit64`*test_numeric_table_min.`uint1`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`bit64/uint1` = test_numeric_table_min.`bit64`/test_numeric_table_min.`uint1`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`bit64+int2` = test_numeric_table_min.`bit64`+test_numeric_table_min.`int2`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`bit64-int2` = test_numeric_table_min.`bit64`-test_numeric_table_min.`int2`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`bit64*int2` = test_numeric_table_min.`bit64`*test_numeric_table_min.`int2`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`bit64/int2` = test_numeric_table_min.`bit64`/test_numeric_table_min.`int2`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`bit64+uint2` = test_numeric_table_min.`bit64`+test_numeric_table_min.`uint2`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`bit64-uint2` = test_numeric_table_min.`bit64`-test_numeric_table_min.`uint2`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`bit64*uint2` = test_numeric_table_min.`bit64`*test_numeric_table_min.`uint2`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`bit64/uint2` = test_numeric_table_min.`bit64`/test_numeric_table_min.`uint2`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`bit64+int4` = test_numeric_table_min.`bit64`+test_numeric_table_min.`int4`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`bit64-int4` = test_numeric_table_min.`bit64`-test_numeric_table_min.`int4`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`bit64*int4` = test_numeric_table_min.`bit64`*test_numeric_table_min.`int4`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`bit64/int4` = test_numeric_table_min.`bit64`/test_numeric_table_min.`int4`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`bit64+uint4` = test_numeric_table_min.`bit64`+test_numeric_table_min.`uint4`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`bit64-uint4` = test_numeric_table_min.`bit64`-test_numeric_table_min.`uint4`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`bit64*uint4` = test_numeric_table_min.`bit64`*test_numeric_table_min.`uint4`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`bit64/uint4` = test_numeric_table_min.`bit64`/test_numeric_table_min.`uint4`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`bit64+int8` = test_numeric_table_min.`bit64`+test_numeric_table_min.`int8`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`bit64-int8` = test_numeric_table_min.`bit64`-test_numeric_table_min.`int8`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`bit64*int8` = test_numeric_table_min.`bit64`*test_numeric_table_min.`int8`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`bit64/int8` = test_numeric_table_min.`bit64`/test_numeric_table_min.`int8`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`bit64+uint8` = test_numeric_table_min.`bit64`+test_numeric_table_min.`uint8`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`bit64-uint8` = test_numeric_table_min.`bit64`-test_numeric_table_min.`uint8`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`bit64*uint8` = test_numeric_table_min.`bit64`*test_numeric_table_min.`uint8`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`bit64/uint8` = test_numeric_table_min.`bit64`/test_numeric_table_min.`uint8`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`bit64+float4` = test_numeric_table_min.`bit64`+test_numeric_table_min.`float4`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`bit64-float4` = test_numeric_table_min.`bit64`-test_numeric_table_min.`float4`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`bit64*float4` = test_numeric_table_min.`bit64`*test_numeric_table_min.`float4`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`bit64/float4` = test_numeric_table_min.`bit64`/test_numeric_table_min.`float4`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`bit64+float8` = test_numeric_table_min.`bit64`+test_numeric_table_min.`float8`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`bit64-float8` = test_numeric_table_min.`bit64`-test_numeric_table_min.`float8`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`bit64*float8` = test_numeric_table_min.`bit64`*test_numeric_table_min.`float8`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`bit64/float8` = test_numeric_table_min.`bit64`/test_numeric_table_min.`float8`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`bit64+numeric` = test_numeric_table_min.`bit64`+test_numeric_table_min.`numeric`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`bit64-numeric` = test_numeric_table_min.`bit64`-test_numeric_table_min.`numeric`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`bit64*numeric` = test_numeric_table_min.`bit64`*test_numeric_table_min.`numeric`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`bit64/numeric` = test_numeric_table_min.`bit64`/test_numeric_table_min.`numeric`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`bit64+bit1` = test_numeric_table_min.`bit64`+test_numeric_table_min.`bit1`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`bit64-bit1` = test_numeric_table_min.`bit64`-test_numeric_table_min.`bit1`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`bit64*bit1` = test_numeric_table_min.`bit64`*test_numeric_table_min.`bit1`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`bit64/bit1` = test_numeric_table_min.`bit64`/test_numeric_table_min.`bit1`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`bit64+bit64` = test_numeric_table_min.`bit64`+test_numeric_table_min.`bit64`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`bit64-bit64` = test_numeric_table_min.`bit64`-test_numeric_table_min.`bit64`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`bit64*bit64` = test_numeric_table_min.`bit64`*test_numeric_table_min.`bit64`;
UPDATE test_numeric_type_min, test_numeric_table_min SET test_numeric_type_min.`bit64/bit64` = test_numeric_table_min.`bit64`/test_numeric_table_min.`bit64`;

SELECT * FROM test_numeric_type_min;

---------- tail ----------
drop schema numeric_operator_test_schema_min cascade;
reset current_schema;

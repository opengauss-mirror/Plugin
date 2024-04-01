drop schema if exists not_test_schema cascade;
create schema not_test_schema;
set current_schema to 'not_test_schema';
---------- head ----------
set dolphin.b_compatibility_mode to on;
set dolphin.sql_mode to 'sql_mode_full_group,pipes_as_concat,ansi_quotes,no_zero_date';
drop table if exists test_multi_type_table;
CREATE TABLE test_multi_type_table (`int1` tinyint, `uint1` tinyint unsigned, `int2` smallint, `uint2` smallint unsigned, `int4` integer, `uint4` integer unsigned, `int8` bigint, `uint8` bigint unsigned, `float4` float4, `float8` float8, `numeric` decimal(20, 6), `bit1` bit(1), `bit64` bit(64), `boolean` boolean,  `char` char(100), `varchar` varchar(100), `binary` binary(100), `varbinary` varbinary(100), `text` text);
insert into test_multi_type_table values(0x7F, 0xFF, 0x7FFF, 0xFFFF, 0x7FFFFFFF, 0xFFFFFFFF, 0x7FFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 0.9884282, 5.288433915413254, 43391541, b'1', b'100110100110', true, '1.23a', '1.23a', '1.23a', '1.23a', '1.23a');

DROP TABLE IF EXISTS test_multi_type;
CREATE TABLE test_multi_type AS SELECT
not `int1` as `not_int1`,
not `uint1` as `not_uint1`,
not `int2` as `not_int2`,
not `uint2` as `not_uint2`,
not `int4` as `not_int4`,
not `uint4` as `not_uint4`,
not `int8` as `not_int8`,
not `uint8` as `not_uint8`,
not `float4` as `not_float4`,
not `float8` as `not_float8`,
not `numeric` as `not_numeric`,
not `bit1` as `not_bit1`,
not `bit64` as `not_bit64`,
not `boolean` as `not_boolean`,
not `char` as `not_char`,
not `varchar` as `not_varchar`,
not `binary` as `not_binary`,
not `varbinary` as `not_varbinary`,
not `text` as `not_text`,
! `int1` as `!_int1`,
! `uint1` as `!_uint1`,
! `int2` as `!_int2`,
! `uint2` as `!_uint2`,
! `int4` as `!_int4`,
! `uint4` as `!_uint4`,
! `int8` as `!_int8`,
! `uint8` as `!_uint8`,
! `float4` as `!_float4`,
! `float8` as `!_float8`,
! `numeric` as `!_numeric`,
! `bit1` as `!_bit1`,
! `bit64` as `!_bit64`,
! `boolean` as `!_boolean`,
! `char` as `!_char`,
! `varchar` as `!_varchar`,
! `binary` as `!_binary`,
! `varbinary` as `!_varbinary`,
! `text` as `!_text` 
from test_multi_type_table;
\x
SHOW COLUMNS FROM test_multi_type;
-- test value
delete from test_multi_type;
delete from test_multi_type_table;
insert into test_multi_type values(null);
insert into test_multi_type_table values(0x7F, 0xFF, 0x7FFF, 0xFFFF, 0x7FFFFFFF, 0xFFFFFFFF, 0x7FFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 0.9884282, 5.288433915413254, 43391541, b'1', b'100110100110', true, '1.23a', '1.23a', '1.23a', '1.23a', '1.23a');

UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`not_int1` = not test_multi_type_table.`int1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`not_uint1` = not test_multi_type_table.`uint1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`not_int2` = not test_multi_type_table.`int2`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`not_uint2` = not test_multi_type_table.`uint2`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`not_int4` = not test_multi_type_table.`int4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`not_uint4` = not test_multi_type_table.`uint4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`not_int8` = not test_multi_type_table.`int8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`not_uint8` = not test_multi_type_table.`uint8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`not_float4` = not test_multi_type_table.`float4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`not_float8` = not test_multi_type_table.`float8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`not_numeric` = not test_multi_type_table.`numeric`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`not_bit1` = not test_multi_type_table.`bit1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`not_bit64` = not test_multi_type_table.`bit64`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`not_boolean` = not test_multi_type_table.`boolean`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`not_char` = not test_multi_type_table.`char`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`not_varchar` = not test_multi_type_table.`varchar`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`not_binary` = not test_multi_type_table.`binary`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`not_varbinary` = not test_multi_type_table.`varbinary`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`not_text` = not test_multi_type_table.`text`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`!_int1` = ! test_multi_type_table.`int1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`!_uint1` = ! test_multi_type_table.`uint1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`!_int2` = ! test_multi_type_table.`int2`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`!_uint2` = ! test_multi_type_table.`uint2`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`!_int4` = ! test_multi_type_table.`int4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`!_uint4` = ! test_multi_type_table.`uint4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`!_int8` = ! test_multi_type_table.`int8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`!_uint8` = ! test_multi_type_table.`uint8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`!_float4` = ! test_multi_type_table.`float4`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`!_float8` = ! test_multi_type_table.`float8`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`!_numeric` = ! test_multi_type_table.`numeric`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`!_bit1` = ! test_multi_type_table.`bit1`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`!_bit64` = ! test_multi_type_table.`bit64`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`!_boolean` = ! test_multi_type_table.`boolean`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`!_char` = ! test_multi_type_table.`char`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`!_varchar` = ! test_multi_type_table.`varchar`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`!_binary` = ! test_multi_type_table.`binary`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`!_varbinary` = ! test_multi_type_table.`varbinary`;
UPDATE test_multi_type, test_multi_type_table SET test_multi_type.`!_text` = ! test_multi_type_table.`text`;
SELECT * FROM test_multi_type;
DROP TABLE test_multi_type;

set dolphin.sql_mode to 'sql_mode_strict,sql_mode_full_group,pipes_as_concat,ansi_quotes,no_zero_date';
-- strict select
SELECT
    not `int1` as `not_int1`,
    not `uint1` as `not_uint1`,
    not `int2` as `not_int2`,
    not `uint2` as `not_uint2`,
    not `int4` as `not_int4`,
    not `uint4` as `not_uint4`,
    not `int8` as `not_int8`,
    not `uint8` as `not_uint8`,
    not `float4` as `not_float4`,
    not `float8` as `not_float8`,
    not `numeric` as `not_numeric`,
    not `bit1` as `not_bit1`,
    not `bit64` as `not_bit64`,
    not `boolean` as `not_boolean`,
    not `char` as `not_char`,
    not `varchar` as `not_varchar`,
    not `binary` as `not_binary`,
    not `varbinary` as `not_varbinary`,
    not `text` as `not_text`,
    ! `int1` as `!_int1`,
    ! `uint1` as `!_uint1`,
    ! `int2` as `!_int2`,
    ! `uint2` as `!_uint2`,
    ! `int4` as `!_int4`,
    ! `uint4` as `!_uint4`,
    ! `int8` as `!_int8`,
    ! `uint8` as `!_uint8`,
    ! `float4` as `!_float4`,
    ! `float8` as `!_float8`,
    ! `numeric` as `!_numeric`,
    ! `bit1` as `!_bit1`,
    ! `bit64` as `!_bit64`,
    ! `boolean` as `!_boolean`,
    ! `char` as `!_char`,
    ! `varchar` as `!_varchar`,
    ! `binary` as `!_binary`,
    ! `varbinary` as `!_varbinary`,
    ! `text` as `!_text` 
FROM test_multi_type_table;
\x

-- strict insert
CREATE TABLE TEST(a boolean);
INSERT INTO TEST SELECT not `int1` from test_multi_type_table;
INSERT INTO TEST SELECT not `uint1` from test_multi_type_table;
INSERT INTO TEST SELECT not `int2` from test_multi_type_table;
INSERT INTO TEST SELECT not `uint2` from test_multi_type_table;
INSERT INTO TEST SELECT not `int4` from test_multi_type_table;
INSERT INTO TEST SELECT not `uint4` from test_multi_type_table;
INSERT INTO TEST SELECT not `int8` from test_multi_type_table;
INSERT INTO TEST SELECT not `uint8` from test_multi_type_table;
INSERT INTO TEST SELECT not `float4` from test_multi_type_table;
INSERT INTO TEST SELECT not `float8` from test_multi_type_table;
INSERT INTO TEST SELECT not `numeric` from test_multi_type_table;
INSERT INTO TEST SELECT not `bit1` from test_multi_type_table;
INSERT INTO TEST SELECT not `bit64` from test_multi_type_table;
INSERT INTO TEST SELECT not `boolean` from test_multi_type_table;
INSERT INTO TEST SELECT not `char` from test_multi_type_table;
INSERT INTO TEST SELECT not `varchar` from test_multi_type_table;
INSERT INTO TEST SELECT not `binary` from test_multi_type_table;
INSERT INTO TEST SELECT not `varbinary` from test_multi_type_table;
INSERT INTO TEST SELECT not `text` from test_multi_type_table;
INSERT INTO TEST SELECT ! `int1` from test_multi_type_table;
INSERT INTO TEST SELECT ! `uint1` from test_multi_type_table;
INSERT INTO TEST SELECT ! `int2` from test_multi_type_table;
INSERT INTO TEST SELECT ! `uint2` from test_multi_type_table;
INSERT INTO TEST SELECT ! `int4` from test_multi_type_table;
INSERT INTO TEST SELECT ! `uint4` from test_multi_type_table;
INSERT INTO TEST SELECT ! `int8` from test_multi_type_table;
INSERT INTO TEST SELECT ! `uint8` from test_multi_type_table;
INSERT INTO TEST SELECT ! `float4` from test_multi_type_table;
INSERT INTO TEST SELECT ! `float8` from test_multi_type_table;
INSERT INTO TEST SELECT ! `numeric` from test_multi_type_table;
INSERT INTO TEST SELECT ! `bit1` from test_multi_type_table;
INSERT INTO TEST SELECT ! `bit64` from test_multi_type_table;
INSERT INTO TEST SELECT ! `boolean` from test_multi_type_table;
INSERT INTO TEST SELECT ! `char` from test_multi_type_table;
INSERT INTO TEST SELECT ! `varchar` from test_multi_type_table;
INSERT INTO TEST SELECT ! `binary` from test_multi_type_table;
INSERT INTO TEST SELECT ! `varbinary` from test_multi_type_table;
INSERT INTO TEST SELECT ! `text` from test_multi_type_table;

-- strict insert ignore
INSERT IGNORE INTO TEST SELECT not `int1` from test_multi_type_table;
INSERT IGNORE INTO TEST SELECT not `uint1` from test_multi_type_table;
INSERT IGNORE INTO TEST SELECT not `int2` from test_multi_type_table;
INSERT IGNORE INTO TEST SELECT not `uint2` from test_multi_type_table;
INSERT IGNORE INTO TEST SELECT not `int4` from test_multi_type_table;
INSERT IGNORE INTO TEST SELECT not `uint4` from test_multi_type_table;
INSERT IGNORE INTO TEST SELECT not `int8` from test_multi_type_table;
INSERT IGNORE INTO TEST SELECT not `uint8` from test_multi_type_table;
INSERT IGNORE INTO TEST SELECT not `float4` from test_multi_type_table;
INSERT IGNORE INTO TEST SELECT not `float8` from test_multi_type_table;
INSERT IGNORE INTO TEST SELECT not `numeric` from test_multi_type_table;
INSERT IGNORE INTO TEST SELECT not `bit1` from test_multi_type_table;
INSERT IGNORE INTO TEST SELECT not `bit64` from test_multi_type_table;
INSERT IGNORE INTO TEST SELECT not `boolean` from test_multi_type_table;
INSERT IGNORE INTO TEST SELECT not `char` from test_multi_type_table;
INSERT IGNORE INTO TEST SELECT not `varchar` from test_multi_type_table;
INSERT IGNORE INTO TEST SELECT not `binary` from test_multi_type_table;
INSERT IGNORE INTO TEST SELECT not `varbinary` from test_multi_type_table;
INSERT IGNORE INTO TEST SELECT not `text` from test_multi_type_table;
INSERT IGNORE INTO TEST SELECT ! `int1` from test_multi_type_table;
INSERT IGNORE INTO TEST SELECT ! `uint1` from test_multi_type_table;
INSERT IGNORE INTO TEST SELECT ! `int2` from test_multi_type_table;
INSERT IGNORE INTO TEST SELECT ! `uint2` from test_multi_type_table;
INSERT IGNORE INTO TEST SELECT ! `int4` from test_multi_type_table;
INSERT IGNORE INTO TEST SELECT ! `uint4` from test_multi_type_table;
INSERT IGNORE INTO TEST SELECT ! `int8` from test_multi_type_table;
INSERT IGNORE INTO TEST SELECT ! `uint8` from test_multi_type_table;
INSERT IGNORE INTO TEST SELECT ! `float4` from test_multi_type_table;
INSERT IGNORE INTO TEST SELECT ! `float8` from test_multi_type_table;
INSERT IGNORE INTO TEST SELECT ! `numeric` from test_multi_type_table;
INSERT IGNORE INTO TEST SELECT ! `bit1` from test_multi_type_table;
INSERT IGNORE INTO TEST SELECT ! `bit64` from test_multi_type_table;
INSERT IGNORE INTO TEST SELECT ! `boolean` from test_multi_type_table;
INSERT IGNORE INTO TEST SELECT ! `char` from test_multi_type_table;
INSERT IGNORE INTO TEST SELECT ! `varchar` from test_multi_type_table;
INSERT IGNORE INTO TEST SELECT ! `binary` from test_multi_type_table;
INSERT IGNORE INTO TEST SELECT ! `varbinary` from test_multi_type_table;
INSERT IGNORE INTO TEST SELECT ! `text` from test_multi_type_table;

-- unknown test
SELECT NOT '1.23a';
SELECT NOT '1.23';
SELECT NOT 'h2YOf#/P4m7>*Gr$BCD)2CitpI(\VkB+ie6HzIS[]rrG$rbXU0ANTZc]]D3a_O.m';
SELECT NOT '4u5b*gA;+-vm<yj^,Q%Z6p9EtLCJlPSCrsGvrkBNlZ6<Ez,Nf]2~ox[UbbNc(~R;';
SELECT ! '1.23a';
SELECT ! '1.23';
SELECT ! 'h2YOf#/P4m7>*Gr$BCD)2CitpI(\VkB+ie6HzIS[]rrG$rbXU0ANTZc]]D3a_O.m';
SELECT ! '4u5b*gA;+-vm<yj^,Q%Z6p9EtLCJlPSCrsGvrkBNlZ6<Ez,Nf]2~ox[UbbNc(~R;';

DROP TABLE TEST;
DROP TABLE test_multi_type_table;

---------- tail ----------
drop schema not_test_schema;
reset current_schema;
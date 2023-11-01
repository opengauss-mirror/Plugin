---------- note ----------
-- binary non cmp opers --
---- 1. + - * / div
---- 2. % mod ^ xor
---- 3. >>(shr) <<(shl)
---- 4. |(or) &(and)
---- 5. || or && and
-- left opers
---- 1. +(uplus) -(uminus)
---- 2. ~(invention) @(abs)
---- 3. ! not
drop schema if exists json_operator_test_schema cascade;
create schema json_operator_test_schema;
set current_schema to 'json_operator_test_schema';
---------- head ----------
set dolphin.b_compatibility_mode to on;
set dolphin.sql_mode to 'sql_mode_full_group,ansi_quotes,no_zero_date';
DROP TABLE IF EXISTS test_json_table;
CREATE TABLE test_json_table
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
-- type test
DROP TABLE IF EXISTS test_json_type;
CREATE TABLE test_json_type AS SELECT
`int1` + `json` AS `int1+json`,
`int1` - `json` AS `int1-json`,
`int1` * `json` AS `int1*json`,
`int1` / `json` AS `int1/json`,
`int1` % `json` AS `int1%json`,
`int1` ^ `json` AS `int1^json`,
`int1` >> `json` AS `int1>>json`,
`int1` << `json` AS `int1<<json`,
`int1` | `json` AS `int1|json`,
`int1` & `json` AS `int1&json`,
`int1` || `json` AS `int1||json`,
`int1` && `json` AS `int1&&json`,
`int1` div `json` AS `int1_div_json`,
`int1` mod `json` AS `int1_mod_json`,
`int1` xor `json` AS `int1_xor_json`,
`int1` and `json` AS `int1_and_json`,
`int1` or `json` AS `int1_or_json`,
`uint1` + `json` AS `uint1+json`,
`uint1` - `json` AS `uint1-json`,
`uint1` * `json` AS `uint1*json`,
`uint1` / `json` AS `uint1/json`,
`uint1` % `json` AS `uint1%json`,
`uint1` ^ `json` AS `uint1^json`,
`uint1` >> `json` AS `uint1>>json`,
`uint1` << `json` AS `uint1<<json`,
`uint1` | `json` AS `uint1|json`,
`uint1` & `json` AS `uint1&json`,
`uint1` || `json` AS `uint1||json`,
`uint1` && `json` AS `uint1&&json`,
`uint1` div `json` AS `uint1_div_json`,
`uint1` mod `json` AS `uint1_mod_json`,
`uint1` xor `json` AS `uint1_xor_json`,
`uint1` and `json` AS `uint1_and_json`,
`uint1` or `json` AS `uint1_or_json`,
`int2` + `json` AS `int2+json`,
`int2` - `json` AS `int2-json`,
`int2` * `json` AS `int2*json`,
`int2` / `json` AS `int2/json`,
`int2` % `json` AS `int2%json`,
`int2` ^ `json` AS `int2^json`,
`int2` >> `json` AS `int2>>json`,
`int2` << `json` AS `int2<<json`,
`int2` | `json` AS `int2|json`,
`int2` & `json` AS `int2&json`,
`int2` || `json` AS `int2||json`,
`int2` && `json` AS `int2&&json`,
`int2` div `json` AS `int2_div_json`,
`int2` mod `json` AS `int2_mod_json`,
`int2` xor `json` AS `int2_xor_json`,
`int2` and `json` AS `int2_and_json`,
`int2` or `json` AS `int2_or_json`,
`uint2` + `json` AS `uint2+json`,
`uint2` - `json` AS `uint2-json`,
`uint2` * `json` AS `uint2*json`,
`uint2` / `json` AS `uint2/json`,
`uint2` % `json` AS `uint2%json`,
`uint2` ^ `json` AS `uint2^json`,
`uint2` >> `json` AS `uint2>>json`,
`uint2` << `json` AS `uint2<<json`,
`uint2` | `json` AS `uint2|json`,
`uint2` & `json` AS `uint2&json`,
`uint2` || `json` AS `uint2||json`,
`uint2` && `json` AS `uint2&&json`,
`uint2` div `json` AS `uint2_div_json`,
`uint2` mod `json` AS `uint2_mod_json`,
`uint2` xor `json` AS `uint2_xor_json`,
`uint2` and `json` AS `uint2_and_json`,
`uint2` or `json` AS `uint2_or_json`,
`int4` + `json` AS `int4+json`,
`int4` - `json` AS `int4-json`,
`int4` * `json` AS `int4*json`,
`int4` / `json` AS `int4/json`,
`int4` % `json` AS `int4%json`,
`int4` ^ `json` AS `int4^json`,
`int4` >> `json` AS `int4>>json`,
`int4` << `json` AS `int4<<json`,
`int4` | `json` AS `int4|json`,
`int4` & `json` AS `int4&json`,
`int4` || `json` AS `int4||json`,
`int4` && `json` AS `int4&&json`,
`int4` div `json` AS `int4_div_json`,
`int4` mod `json` AS `int4_mod_json`,
`int4` xor `json` AS `int4_xor_json`,
`int4` and `json` AS `int4_and_json`,
`int4` or `json` AS `int4_or_json`,
`uint4` + `json` AS `uint4+json`,
`uint4` - `json` AS `uint4-json`,
`uint4` * `json` AS `uint4*json`,
`uint4` / `json` AS `uint4/json`,
`uint4` % `json` AS `uint4%json`,
`uint4` ^ `json` AS `uint4^json`,
`uint4` >> `json` AS `uint4>>json`,
`uint4` << `json` AS `uint4<<json`,
`uint4` | `json` AS `uint4|json`,
`uint4` & `json` AS `uint4&json`,
`uint4` || `json` AS `uint4||json`,
`uint4` && `json` AS `uint4&&json`,
`uint4` div `json` AS `uint4_div_json`,
`uint4` mod `json` AS `uint4_mod_json`,
`uint4` xor `json` AS `uint4_xor_json`,
`uint4` and `json` AS `uint4_and_json`,
`uint4` or `json` AS `uint4_or_json`,
`int8` + `json` AS `int8+json`,
`int8` - `json` AS `int8-json`,
`int8` * `json` AS `int8*json`,
`int8` / `json` AS `int8/json`,
`int8` % `json` AS `int8%json`,
`int8` ^ `json` AS `int8^json`,
`int8` >> `json` AS `int8>>json`,
`int8` << `json` AS `int8<<json`,
`int8` | `json` AS `int8|json`,
`int8` & `json` AS `int8&json`,
`int8` || `json` AS `int8||json`,
`int8` && `json` AS `int8&&json`,
`int8` div `json` AS `int8_div_json`,
`int8` mod `json` AS `int8_mod_json`,
`int8` xor `json` AS `int8_xor_json`,
`int8` and `json` AS `int8_and_json`,
`int8` or `json` AS `int8_or_json`,
`uint8` + `json` AS `uint8+json`,
`uint8` - `json` AS `uint8-json`,
`uint8` * `json` AS `uint8*json`,
`uint8` / `json` AS `uint8/json`,
`uint8` % `json` AS `uint8%json`,
`uint8` ^ `json` AS `uint8^json`,
`uint8` >> `json` AS `uint8>>json`,
`uint8` << `json` AS `uint8<<json`,
`uint8` | `json` AS `uint8|json`,
`uint8` & `json` AS `uint8&json`,
`uint8` || `json` AS `uint8||json`,
`uint8` && `json` AS `uint8&&json`,
`uint8` div `json` AS `uint8_div_json`,
`uint8` mod `json` AS `uint8_mod_json`,
`uint8` xor `json` AS `uint8_xor_json`,
`uint8` and `json` AS `uint8_and_json`,
`uint8` or `json` AS `uint8_or_json`,
`float4` + `json` AS `float4+json`,
`float4` - `json` AS `float4-json`,
`float4` * `json` AS `float4*json`,
`float4` / `json` AS `float4/json`,
`float4` % `json` AS `float4%json`,
`float4` ^ `json` AS `float4^json`,
`float4` >> `json` AS `float4>>json`,
`float4` << `json` AS `float4<<json`,
`float4` | `json` AS `float4|json`,
`float4` & `json` AS `float4&json`,
`float4` || `json` AS `float4||json`,
`float4` && `json` AS `float4&&json`,
`float4` div `json` AS `float4_div_json`,
`float4` mod `json` AS `float4_mod_json`,
`float4` xor `json` AS `float4_xor_json`,
`float4` and `json` AS `float4_and_json`,
`float4` or `json` AS `float4_or_json`,
`float8` + `json` AS `float8+json`,
`float8` - `json` AS `float8-json`,
`float8` * `json` AS `float8*json`,
`float8` / `json` AS `float8/json`,
`float8` % `json` AS `float8%json`,
`float8` ^ `json` AS `float8^json`,
`float8` >> `json` AS `float8>>json`,
`float8` << `json` AS `float8<<json`,
`float8` | `json` AS `float8|json`,
`float8` & `json` AS `float8&json`,
`float8` || `json` AS `float8||json`,
`float8` && `json` AS `float8&&json`,
`float8` div `json` AS `float8_div_json`,
`float8` mod `json` AS `float8_mod_json`,
`float8` xor `json` AS `float8_xor_json`,
`float8` and `json` AS `float8_and_json`,
`float8` or `json` AS `float8_or_json`,
`numeric` + `json` AS `numeric+json`,
`numeric` - `json` AS `numeric-json`,
`numeric` * `json` AS `numeric*json`,
`numeric` / `json` AS `numeric/json`,
`numeric` % `json` AS `numeric%json`,
`numeric` ^ `json` AS `numeric^json`,
`numeric` >> `json` AS `numeric>>json`,
`numeric` << `json` AS `numeric<<json`,
`numeric` | `json` AS `numeric|json`,
`numeric` & `json` AS `numeric&json`,
`numeric` || `json` AS `numeric||json`,
`numeric` && `json` AS `numeric&&json`,
`numeric` div `json` AS `numeric_div_json`,
`numeric` mod `json` AS `numeric_mod_json`,
`numeric` xor `json` AS `numeric_xor_json`,
`numeric` and `json` AS `numeric_and_json`,
`numeric` or `json` AS `numeric_or_json`,
`bit1` + `json` AS `bit1+json`,
`bit1` - `json` AS `bit1-json`,
`bit1` * `json` AS `bit1*json`,
`bit1` / `json` AS `bit1/json`,
`bit1` % `json` AS `bit1%json`,
`bit1` ^ `json` AS `bit1^json`,
`bit1` >> `json` AS `bit1>>json`,
`bit1` << `json` AS `bit1<<json`,
`bit1` | `json` AS `bit1|json`,
`bit1` & `json` AS `bit1&json`,
`bit1` || `json` AS `bit1||json`,
`bit1` && `json` AS `bit1&&json`,
`bit1` div `json` AS `bit1_div_json`,
`bit1` mod `json` AS `bit1_mod_json`,
`bit1` xor `json` AS `bit1_xor_json`,
`bit1` and `json` AS `bit1_and_json`,
`bit1` or `json` AS `bit1_or_json`,
`bit64` + `json` AS `bit64+json`,
`bit64` - `json` AS `bit64-json`,
`bit64` * `json` AS `bit64*json`,
`bit64` / `json` AS `bit64/json`,
`bit64` % `json` AS `bit64%json`,
`bit64` ^ `json` AS `bit64^json`,
`bit64` >> `json` AS `bit64>>json`,
`bit64` << `json` AS `bit64<<json`,
`bit64` | `json` AS `bit64|json`,
`bit64` & `json` AS `bit64&json`,
`bit64` || `json` AS `bit64||json`,
`bit64` && `json` AS `bit64&&json`,
`bit64` div `json` AS `bit64_div_json`,
`bit64` mod `json` AS `bit64_mod_json`,
`bit64` xor `json` AS `bit64_xor_json`,
`bit64` and `json` AS `bit64_and_json`,
`bit64` or `json` AS `bit64_or_json`,
`boolean` + `json` AS `boolean+json`,
`boolean` - `json` AS `boolean-json`,
`boolean` * `json` AS `boolean*json`,
`boolean` / `json` AS `boolean/json`,
`boolean` % `json` AS `boolean%json`,
-- `boolean` ^ `json` AS `boolean^json`,
`boolean` >> `json` AS `boolean>>json`,
`boolean` << `json` AS `boolean<<json`,
`boolean` | `json` AS `boolean|json`,
`boolean` & `json` AS `boolean&json`,
`boolean` || `json` AS `boolean||json`,
`boolean` && `json` AS `boolean&&json`,
`boolean` div `json` AS `boolean_div_json`,
`boolean` mod `json` AS `boolean_mod_json`,
-- `boolean` xor `json` AS `boolean_xor_json`,
`boolean` and `json` AS `boolean_and_json`,
`boolean` or `json` AS `boolean_or_json`,
`date` + `json` AS `date+json`,
`date` - `json` AS `date-json`,
`date` * `json` AS `date*json`,
`date` / `json` AS `date/json`,
`date` % `json` AS `date%json`,
`date` ^ `json` AS `date^json`,
`date` >> `json` AS `date>>json`,
`date` << `json` AS `date<<json`,
`date` | `json` AS `date|json`,
`date` & `json` AS `date&json`,
`date` || `json` AS `date||json`,
`date` && `json` AS `date&&json`,
`date` div `json` AS `date_div_json`,
`date` mod `json` AS `date_mod_json`,
`date` xor `json` AS `date_xor_json`,
`date` and `json` AS `date_and_json`,
`date` or `json` AS `date_or_json`,
`time` + `json` AS `time+json`,
`time` - `json` AS `time-json`,
`time` * `json` AS `time*json`,
`time` / `json` AS `time/json`,
`time` % `json` AS `time%json`,
`time` ^ `json` AS `time^json`,
`time` >> `json` AS `time>>json`,
`time` << `json` AS `time<<json`,
`time` | `json` AS `time|json`,
`time` & `json` AS `time&json`,
`time` || `json` AS `time||json`,
`time` && `json` AS `time&&json`,
`time` div `json` AS `time_div_json`,
`time` mod `json` AS `time_mod_json`,
`time` xor `json` AS `time_xor_json`,
`time` and `json` AS `time_and_json`,
`time` or `json` AS `time_or_json`,
`time(4)` + `json` AS `time(4)+json`,
`time(4)` - `json` AS `time(4)-json`,
`time(4)` * `json` AS `time(4)*json`,
`time(4)` / `json` AS `time(4)/json`,
`time(4)` % `json` AS `time(4)%json`,
`time(4)` ^ `json` AS `time(4)^json`,
`time(4)` >> `json` AS `time(4)>>json`,
`time(4)` << `json` AS `time(4)<<json`,
`time(4)` | `json` AS `time(4)|json`,
`time(4)` & `json` AS `time(4)&json`,
`time(4)` || `json` AS `time(4)||json`,
`time(4)` && `json` AS `time(4)&&json`,
`time(4)` div `json` AS `time(4)_div_json`,
`time(4)` mod `json` AS `time(4)_mod_json`,
`time(4)` xor `json` AS `time(4)_xor_json`,
`time(4)` and `json` AS `time(4)_and_json`,
`time(4)` or `json` AS `time(4)_or_json`,
`datetime` + `json` AS `datetime+json`,
`datetime` - `json` AS `datetime-json`,
`datetime` * `json` AS `datetime*json`,
`datetime` / `json` AS `datetime/json`,
`datetime` % `json` AS `datetime%json`,
`datetime` ^ `json` AS `datetime^json`,
`datetime` >> `json` AS `datetime>>json`,
`datetime` << `json` AS `datetime<<json`,
`datetime` | `json` AS `datetime|json`,
`datetime` & `json` AS `datetime&json`,
`datetime` || `json` AS `datetime||json`,
`datetime` && `json` AS `datetime&&json`,
`datetime` div `json` AS `datetime_div_json`,
`datetime` mod `json` AS `datetime_mod_json`,
`datetime` xor `json` AS `datetime_xor_json`,
`datetime` and `json` AS `datetime_and_json`,
`datetime` or `json` AS `datetime_or_json`,
`datetime(4)` + `json` AS `datetime(4)+json`,
`datetime(4)` - `json` AS `datetime(4)-json`,
`datetime(4)` * `json` AS `datetime(4)*json`,
`datetime(4)` / `json` AS `datetime(4)/json`,
`datetime(4)` % `json` AS `datetime(4)%json`,
`datetime(4)` ^ `json` AS `datetime(4)^json`,
`datetime(4)` >> `json` AS `datetime(4)>>json`,
`datetime(4)` << `json` AS `datetime(4)<<json`,
`datetime(4)` | `json` AS `datetime(4)|json`,
`datetime(4)` & `json` AS `datetime(4)&json`,
`datetime(4)` || `json` AS `datetime(4)||json`,
`datetime(4)` && `json` AS `datetime(4)&&json`,
`datetime(4)` div `json` AS `datetime(4)_div_json`,
`datetime(4)` mod `json` AS `datetime(4)_mod_json`,
`datetime(4)` xor `json` AS `datetime(4)_xor_json`,
`datetime(4)` and `json` AS `datetime(4)_and_json`,
`datetime(4)` or `json` AS `datetime(4)_or_json`,
`timestamp` + `json` AS `timestamp+json`,
`timestamp` - `json` AS `timestamp-json`,
`timestamp` * `json` AS `timestamp*json`,
`timestamp` / `json` AS `timestamp/json`,
`timestamp` % `json` AS `timestamp%json`,
`timestamp` ^ `json` AS `timestamp^json`,
`timestamp` >> `json` AS `timestamp>>json`,
`timestamp` << `json` AS `timestamp<<json`,
`timestamp` | `json` AS `timestamp|json`,
`timestamp` & `json` AS `timestamp&json`,
`timestamp` || `json` AS `timestamp||json`,
`timestamp` && `json` AS `timestamp&&json`,
`timestamp` div `json` AS `timestamp_div_json`,
`timestamp` mod `json` AS `timestamp_mod_json`,
`timestamp` xor `json` AS `timestamp_xor_json`,
`timestamp` and `json` AS `timestamp_and_json`,
`timestamp` or `json` AS `timestamp_or_json`,
`timestamp(4)` + `json` AS `timestamp(4)+json`,
`timestamp(4)` - `json` AS `timestamp(4)-json`,
`timestamp(4)` * `json` AS `timestamp(4)*json`,
`timestamp(4)` / `json` AS `timestamp(4)/json`,
`timestamp(4)` % `json` AS `timestamp(4)%json`,
`timestamp(4)` ^ `json` AS `timestamp(4)^json`,
`timestamp(4)` >> `json` AS `timestamp(4)>>json`,
`timestamp(4)` << `json` AS `timestamp(4)<<json`,
`timestamp(4)` | `json` AS `timestamp(4)|json`,
`timestamp(4)` & `json` AS `timestamp(4)&json`,
`timestamp(4)` || `json` AS `timestamp(4)||json`,
`timestamp(4)` && `json` AS `timestamp(4)&&json`,
`timestamp(4)` div `json` AS `timestamp(4)_div_json`,
`timestamp(4)` mod `json` AS `timestamp(4)_mod_json`,
`timestamp(4)` xor `json` AS `timestamp(4)_xor_json`,
`timestamp(4)` and `json` AS `timestamp(4)_and_json`,
`timestamp(4)` or `json` AS `timestamp(4)_or_json`,
`year` + `json` AS `year+json`,
`year` - `json` AS `year-json`,
`year` * `json` AS `year*json`,
`year` / `json` AS `year/json`,
`year` % `json` AS `year%json`,
`year` ^ `json` AS `year^json`,
`year` >> `json` AS `year>>json`,
`year` << `json` AS `year<<json`,
`year` | `json` AS `year|json`,
`year` & `json` AS `year&json`,
-- `year` || `json` AS `year||json`,
-- `year` && `json` AS `year&&json`,
`year` div `json` AS `year_div_json`,
`year` mod `json` AS `year_mod_json`,
`year` xor `json` AS `year_xor_json`,
-- `year` and `json` AS `year_and_json`,
-- `year` or `json` AS `year_or_json`,
`char` + `json` AS `char+json`,
`char` - `json` AS `char-json`,
`char` * `json` AS `char*json`,
`char` / `json` AS `char/json`,
`char` % `json` AS `char%json`,
`char` ^ `json` AS `char^json`,
`char` >> `json` AS `char>>json`,
`char` << `json` AS `char<<json`,
`char` | `json` AS `char|json`,
`char` & `json` AS `char&json`,
`char` || `json` AS `char||json`,
`char` && `json` AS `char&&json`,
`char` div `json` AS `char_div_json`,
`char` mod `json` AS `char_mod_json`,
`char` xor `json` AS `char_xor_json`,
`char` and `json` AS `char_and_json`,
`char` or `json` AS `char_or_json`,
`varchar` + `json` AS `varchar+json`,
`varchar` - `json` AS `varchar-json`,
`varchar` * `json` AS `varchar*json`,
`varchar` / `json` AS `varchar/json`,
`varchar` % `json` AS `varchar%json`,
`varchar` ^ `json` AS `varchar^json`,
`varchar` >> `json` AS `varchar>>json`,
`varchar` << `json` AS `varchar<<json`,
`varchar` | `json` AS `varchar|json`,
`varchar` & `json` AS `varchar&json`,
`varchar` || `json` AS `varchar||json`,
`varchar` && `json` AS `varchar&&json`,
`varchar` div `json` AS `varchar_div_json`,
`varchar` mod `json` AS `varchar_mod_json`,
`varchar` xor `json` AS `varchar_xor_json`,
`varchar` and `json` AS `varchar_and_json`,
`varchar` or `json` AS `varchar_or_json`,
`binary` + `json` AS `binary+json`,
`binary` - `json` AS `binary-json`,
`binary` * `json` AS `binary*json`,
`binary` / `json` AS `binary/json`,
-- `binary` % `json` AS `binary%json`,
`binary` ^ `json` AS `binary^json`,
-- `binary` >> `json` AS `binary>>json`,
-- `binary` << `json` AS `binary<<json`,
-- `binary` | `json` AS `binary|json`,
-- `binary` & `json` AS `binary&json`,
-- `binary` || `json` AS `binary||json`,
-- `binary` && `json` AS `binary&&json`,
`binary` div `json` AS `binary_div_json`,
`binary` mod `json` AS `binary_mod_json`,
`binary` xor `json` AS `binary_xor_json`,
-- `binary` and `json` AS `binary_and_json`,
-- `binary` or `json` AS `binary_or_json`,
`varbinary` + `json` AS `varbinary+json`,
`varbinary` - `json` AS `varbinary-json`,
`varbinary` * `json` AS `varbinary*json`,
`varbinary` / `json` AS `varbinary/json`,
-- `varbinary` % `json` AS `varbinary%json`,
`varbinary` ^ `json` AS `varbinary^json`,
-- `varbinary` >> `json` AS `varbinary>>json`,
-- `varbinary` << `json` AS `varbinary<<json`,
-- `varbinary` | `json` AS `varbinary|json`,
-- `varbinary` & `json` AS `varbinary&json`,
-- `varbinary` || `json` AS `varbinary||json`,
-- `varbinary` && `json` AS `varbinary&&json`,
`varbinary` div `json` AS `varbinary_div_json`,
`varbinary` mod `json` AS `varbinary_mod_json`,
`varbinary` xor `json` AS `varbinary_xor_json`,
-- `varbinary` and `json` AS `varbinary_and_json`,
-- `varbinary` or `json` AS `varbinary_or_json`,
`tinyblob` + `json` AS `tinyblob+json`,
`tinyblob` - `json` AS `tinyblob-json`,
`tinyblob` * `json` AS `tinyblob*json`,
`tinyblob` / `json` AS `tinyblob/json`,
-- `tinyblob` % `json` AS `tinyblob%json`,
`tinyblob` ^ `json` AS `tinyblob^json`,
-- `tinyblob` >> `json` AS `tinyblob>>json`,
-- `tinyblob` << `json` AS `tinyblob<<json`,
-- `tinyblob` | `json` AS `tinyblob|json`,
-- `tinyblob` & `json` AS `tinyblob&json`,
-- `tinyblob` || `json` AS `tinyblob||json`,
-- `tinyblob` && `json` AS `tinyblob&&json`,
`tinyblob` div `json` AS `tinyblob_div_json`,
`tinyblob` mod `json` AS `tinyblob_mod_json`,
`tinyblob` xor `json` AS `tinyblob_xor_json`,
-- `tinyblob` and `json` AS `tinyblob_and_json`,
-- `tinyblob` or `json` AS `tinyblob_or_json`,
`blob` + `json` AS `blob+json`,
`blob` - `json` AS `blob-json`,
`blob` * `json` AS `blob*json`,
`blob` / `json` AS `blob/json`,
-- `blob` % `json` AS `blob%json`,
`blob` ^ `json` AS `blob^json`,
-- `blob` >> `json` AS `blob>>json`,
-- `blob` << `json` AS `blob<<json`,
-- `blob` | `json` AS `blob|json`,
-- `blob` & `json` AS `blob&json`,
-- `blob` || `json` AS `blob||json`,
-- `blob` && `json` AS `blob&&json`,
`blob` div `json` AS `blob_div_json`,
`blob` mod `json` AS `blob_mod_json`,
`blob` xor `json` AS `blob_xor_json`,
-- `blob` and `json` AS `blob_and_json`,
-- `blob` or `json` AS `blob_or_json`,
`mediumblob` + `json` AS `mediumblob+json`,
`mediumblob` - `json` AS `mediumblob-json`,
`mediumblob` * `json` AS `mediumblob*json`,
`mediumblob` / `json` AS `mediumblob/json`,
-- `mediumblob` % `json` AS `mediumblob%json`,
`mediumblob` ^ `json` AS `mediumblob^json`,
-- `mediumblob` >> `json` AS `mediumblob>>json`,
-- `mediumblob` << `json` AS `mediumblob<<json`,
-- `mediumblob` | `json` AS `mediumblob|json`,
-- `mediumblob` & `json` AS `mediumblob&json`,
-- `mediumblob` || `json` AS `mediumblob||json`,
-- `mediumblob` && `json` AS `mediumblob&&json`,
`mediumblob` div `json` AS `mediumblob_div_json`,
`mediumblob` mod `json` AS `mediumblob_mod_json`,
`mediumblob` xor `json` AS `mediumblob_xor_json`,
-- `mediumblob` and `json` AS `mediumblob_and_json`,
-- `mediumblob` or `json` AS `mediumblob_or_json`,
`longblob` + `json` AS `longblob+json`,
`longblob` - `json` AS `longblob-json`,
`longblob` * `json` AS `longblob*json`,
`longblob` / `json` AS `longblob/json`,
-- `longblob` % `json` AS `longblob%json`,
`longblob` ^ `json` AS `longblob^json`,
-- `longblob` >> `json` AS `longblob>>json`,
-- `longblob` << `json` AS `longblob<<json`,
-- `longblob` | `json` AS `longblob|json`,
-- `longblob` & `json` AS `longblob&json`,
-- `longblob` || `json` AS `longblob||json`,
-- `longblob` && `json` AS `longblob&&json`,
`longblob` div `json` AS `longblob_div_json`,
`longblob` mod `json` AS `longblob_mod_json`,
`longblob` xor `json` AS `longblob_xor_json`,
-- `longblob` and `json` AS `longblob_and_json`,
-- `longblob` or `json` AS `longblob_or_json`,
`text` + `json` AS `text+json`,
`text` - `json` AS `text-json`,
`text` * `json` AS `text*json`,
`text` / `json` AS `text/json`,
`text` % `json` AS `text%json`,
`text` ^ `json` AS `text^json`,
`text` >> `json` AS `text>>json`,
`text` << `json` AS `text<<json`,
`text` | `json` AS `text|json`,
`text` & `json` AS `text&json`,
`text` || `json` AS `text||json`,
`text` && `json` AS `text&&json`,
`text` div `json` AS `text_div_json`,
`text` mod `json` AS `text_mod_json`,
`text` xor `json` AS `text_xor_json`,
`text` and `json` AS `text_and_json`,
`text` or `json` AS `text_or_json`,
`enum_t` + `json` AS `enum_t+json`,
`enum_t` - `json` AS `enum_t-json`,
`enum_t` * `json` AS `enum_t*json`,
`enum_t` / `json` AS `enum_t/json`,
`enum_t` % `json` AS `enum_t%json`,
`enum_t` ^ `json` AS `enum_t^json`,
`enum_t` >> `json` AS `enum_t>>json`,
`enum_t` << `json` AS `enum_t<<json`,
`enum_t` | `json` AS `enum_t|json`,
`enum_t` & `json` AS `enum_t&json`,
-- `enum_t` || `json` AS `enum_t||json`,
-- `enum_t` && `json` AS `enum_t&&json`,
`enum_t` div `json` AS `enum_t_div_json`,
`enum_t` mod `json` AS `enum_t_mod_json`,
`enum_t` xor `json` AS `enum_t_xor_json`,
-- `enum_t` and `json` AS `enum_t_and_json`,
-- `enum_t` or `json` AS `enum_t_or_json`,
`set_t` + `json` AS `set_t+json`,
`set_t` - `json` AS `set_t-json`,
`set_t` * `json` AS `set_t*json`,
`set_t` / `json` AS `set_t/json`,
`set_t` % `json` AS `set_t%json`,
`set_t` ^ `json` AS `set_t^json`,
`set_t` >> `json` AS `set_t>>json`,
`set_t` << `json` AS `set_t<<json`,
`set_t` | `json` AS `set_t|json`,
`set_t` & `json` AS `set_t&json`,
-- `set_t` || `json` AS `set_t||json`,
-- `set_t` && `json` AS `set_t&&json`,
`set_t` div `json` AS `set_t_div_json`,
`set_t` mod `json` AS `set_t_mod_json`,
`set_t` xor `json` AS `set_t_xor_json`,
-- `set_t` and `json` AS `set_t_and_json`,
-- `set_t` or `json` AS `set_t_or_json`,
`json` + `int1` AS `json+int1`,
`json` - `int1` AS `json-int1`,
`json` * `int1` AS `json*int1`,
`json` / `int1` AS `json/int1`,
`json` % `int1` AS `json%int1`,
`json` ^ `int1` AS `json^int1`,
`json` >> `int1` AS `json>>int1`,
`json` << `int1` AS `json<<int1`,
`json` | `int1` AS `json|int1`,
`json` & `int1` AS `json&int1`,
`json` || `int1` AS `json||int1`,
`json` && `int1` AS `json&&int1`,
`json` div `int1` AS `json_div_int1`,
`json` mod `int1` AS `json_mod_int1`,
`json` xor `int1` AS `json_xor_int1`,
`json` and `int1` AS `json_and_int1`,
`json` or `int1` AS `json_or_int1`,
`json` + `uint1` AS `json+uint1`,
`json` - `uint1` AS `json-uint1`,
`json` * `uint1` AS `json*uint1`,
`json` / `uint1` AS `json/uint1`,
`json` % `uint1` AS `json%uint1`,
`json` ^ `uint1` AS `json^uint1`,
`json` >> `uint1` AS `json>>uint1`,
`json` << `uint1` AS `json<<uint1`,
`json` | `uint1` AS `json|uint1`,
`json` & `uint1` AS `json&uint1`,
`json` || `uint1` AS `json||uint1`,
`json` && `uint1` AS `json&&uint1`,
`json` div `uint1` AS `json_div_uint1`,
`json` mod `uint1` AS `json_mod_uint1`,
`json` xor `uint1` AS `json_xor_uint1`,
`json` and `uint1` AS `json_and_uint1`,
`json` or `uint1` AS `json_or_uint1`,
`json` + `int2` AS `json+int2`,
`json` - `int2` AS `json-int2`,
`json` * `int2` AS `json*int2`,
`json` / `int2` AS `json/int2`,
`json` % `int2` AS `json%int2`,
`json` ^ `int2` AS `json^int2`,
`json` >> `int2` AS `json>>int2`,
`json` << `int2` AS `json<<int2`,
`json` | `int2` AS `json|int2`,
`json` & `int2` AS `json&int2`,
`json` || `int2` AS `json||int2`,
`json` && `int2` AS `json&&int2`,
`json` div `int2` AS `json_div_int2`,
`json` mod `int2` AS `json_mod_int2`,
`json` xor `int2` AS `json_xor_int2`,
`json` and `int2` AS `json_and_int2`,
`json` or `int2` AS `json_or_int2`,
`json` + `uint2` AS `json+uint2`,
`json` - `uint2` AS `json-uint2`,
`json` * `uint2` AS `json*uint2`,
`json` / `uint2` AS `json/uint2`,
`json` % `uint2` AS `json%uint2`,
`json` ^ `uint2` AS `json^uint2`,
`json` >> `uint2` AS `json>>uint2`,
`json` << `uint2` AS `json<<uint2`,
`json` | `uint2` AS `json|uint2`,
`json` & `uint2` AS `json&uint2`,
`json` || `uint2` AS `json||uint2`,
`json` && `uint2` AS `json&&uint2`,
`json` div `uint2` AS `json_div_uint2`,
`json` mod `uint2` AS `json_mod_uint2`,
`json` xor `uint2` AS `json_xor_uint2`,
`json` and `uint2` AS `json_and_uint2`,
`json` or `uint2` AS `json_or_uint2`,
`json` + `int4` AS `json+int4`,
`json` - `int4` AS `json-int4`,
`json` * `int4` AS `json*int4`,
`json` / `int4` AS `json/int4`,
`json` % `int4` AS `json%int4`,
`json` ^ `int4` AS `json^int4`,
`json` >> `int4` AS `json>>int4`,
`json` << `int4` AS `json<<int4`,
`json` | `int4` AS `json|int4`,
`json` & `int4` AS `json&int4`,
`json` || `int4` AS `json||int4`,
`json` && `int4` AS `json&&int4`,
`json` div `int4` AS `json_div_int4`,
`json` mod `int4` AS `json_mod_int4`,
`json` xor `int4` AS `json_xor_int4`,
`json` and `int4` AS `json_and_int4`,
`json` or `int4` AS `json_or_int4`,
`json` + `uint4` AS `json+uint4`,
`json` - `uint4` AS `json-uint4`,
`json` * `uint4` AS `json*uint4`,
`json` / `uint4` AS `json/uint4`,
`json` % `uint4` AS `json%uint4`,
`json` ^ `uint4` AS `json^uint4`,
`json` >> `uint4` AS `json>>uint4`,
`json` << `uint4` AS `json<<uint4`,
`json` | `uint4` AS `json|uint4`,
`json` & `uint4` AS `json&uint4`,
`json` || `uint4` AS `json||uint4`,
`json` && `uint4` AS `json&&uint4`,
`json` div `uint4` AS `json_div_uint4`,
`json` mod `uint4` AS `json_mod_uint4`,
`json` xor `uint4` AS `json_xor_uint4`,
`json` and `uint4` AS `json_and_uint4`,
`json` or `uint4` AS `json_or_uint4`,
`json` + `int8` AS `json+int8`,
`json` - `int8` AS `json-int8`,
`json` * `int8` AS `json*int8`,
`json` / `int8` AS `json/int8`,
`json` % `int8` AS `json%int8`,
`json` ^ `int8` AS `json^int8`,
`json` >> `int8` AS `json>>int8`,
`json` << `int8` AS `json<<int8`,
`json` | `int8` AS `json|int8`,
`json` & `int8` AS `json&int8`,
`json` || `int8` AS `json||int8`,
`json` && `int8` AS `json&&int8`,
`json` div `int8` AS `json_div_int8`,
`json` mod `int8` AS `json_mod_int8`,
`json` xor `int8` AS `json_xor_int8`,
`json` and `int8` AS `json_and_int8`,
`json` or `int8` AS `json_or_int8`,
`json` + `uint8` AS `json+uint8`,
`json` - `uint8` AS `json-uint8`,
`json` * `uint8` AS `json*uint8`,
`json` / `uint8` AS `json/uint8`,
`json` % `uint8` AS `json%uint8`,
`json` ^ `uint8` AS `json^uint8`,
`json` >> `uint8` AS `json>>uint8`,
`json` << `uint8` AS `json<<uint8`,
`json` | `uint8` AS `json|uint8`,
`json` & `uint8` AS `json&uint8`,
`json` || `uint8` AS `json||uint8`,
`json` && `uint8` AS `json&&uint8`,
`json` div `uint8` AS `json_div_uint8`,
`json` mod `uint8` AS `json_mod_uint8`,
`json` xor `uint8` AS `json_xor_uint8`,
`json` and `uint8` AS `json_and_uint8`,
`json` or `uint8` AS `json_or_uint8`,
`json` + `float4` AS `json+float4`,
`json` - `float4` AS `json-float4`,
`json` * `float4` AS `json*float4`,
`json` / `float4` AS `json/float4`,
`json` % `float4` AS `json%float4`,
`json` ^ `float4` AS `json^float4`,
`json` >> `float4` AS `json>>float4`,
`json` << `float4` AS `json<<float4`,
`json` | `float4` AS `json|float4`,
`json` & `float4` AS `json&float4`,
`json` || `float4` AS `json||float4`,
`json` && `float4` AS `json&&float4`,
`json` div `float4` AS `json_div_float4`,
`json` mod `float4` AS `json_mod_float4`,
`json` xor `float4` AS `json_xor_float4`,
`json` and `float4` AS `json_and_float4`,
`json` or `float4` AS `json_or_float4`,
`json` + `float8` AS `json+float8`,
`json` - `float8` AS `json-float8`,
`json` * `float8` AS `json*float8`,
`json` / `float8` AS `json/float8`,
`json` % `float8` AS `json%float8`,
`json` ^ `float8` AS `json^float8`,
`json` >> `float8` AS `json>>float8`,
`json` << `float8` AS `json<<float8`,
`json` | `float8` AS `json|float8`,
`json` & `float8` AS `json&float8`,
`json` || `float8` AS `json||float8`,
`json` && `float8` AS `json&&float8`,
`json` div `float8` AS `json_div_float8`,
`json` mod `float8` AS `json_mod_float8`,
`json` xor `float8` AS `json_xor_float8`,
`json` and `float8` AS `json_and_float8`,
`json` or `float8` AS `json_or_float8`,
`json` + `numeric` AS `json+numeric`,
`json` - `numeric` AS `json-numeric`,
`json` * `numeric` AS `json*numeric`,
`json` / `numeric` AS `json/numeric`,
`json` % `numeric` AS `json%numeric`,
`json` ^ `numeric` AS `json^numeric`,
`json` >> `numeric` AS `json>>numeric`,
`json` << `numeric` AS `json<<numeric`,
`json` | `numeric` AS `json|numeric`,
`json` & `numeric` AS `json&numeric`,
`json` || `numeric` AS `json||numeric`,
`json` && `numeric` AS `json&&numeric`,
`json` div `numeric` AS `json_div_numeric`,
`json` mod `numeric` AS `json_mod_numeric`,
`json` xor `numeric` AS `json_xor_numeric`,
`json` and `numeric` AS `json_and_numeric`,
`json` or `numeric` AS `json_or_numeric`,
`json` + `bit1` AS `json+bit1`,
`json` - `bit1` AS `json-bit1`,
`json` * `bit1` AS `json*bit1`,
`json` / `bit1` AS `json/bit1`,
`json` % `bit1` AS `json%bit1`,
`json` ^ `bit1` AS `json^bit1`,
-- `json` >> `bit1` AS `json>>bit1`,
-- `json` << `bit1` AS `json<<bit1`,
`json` | `bit1` AS `json|bit1`,
`json` & `bit1` AS `json&bit1`,
`json` || `bit1` AS `json||bit1`,
`json` && `bit1` AS `json&&bit1`,
`json` div `bit1` AS `json_div_bit1`,
`json` mod `bit1` AS `json_mod_bit1`,
`json` xor `bit1` AS `json_xor_bit1`,
`json` and `bit1` AS `json_and_bit1`,
`json` or `bit1` AS `json_or_bit1`,
`json` + `bit64` AS `json+bit64`,
`json` - `bit64` AS `json-bit64`,
`json` * `bit64` AS `json*bit64`,
`json` / `bit64` AS `json/bit64`,
`json` % `bit64` AS `json%bit64`,
`json` ^ `bit64` AS `json^bit64`,
-- `json` >> `bit64` AS `json>>bit64`,
-- `json` << `bit64` AS `json<<bit64`,
`json` | `bit64` AS `json|bit64`,
`json` & `bit64` AS `json&bit64`,
`json` || `bit64` AS `json||bit64`,
`json` && `bit64` AS `json&&bit64`,
`json` div `bit64` AS `json_div_bit64`,
`json` mod `bit64` AS `json_mod_bit64`,
`json` xor `bit64` AS `json_xor_bit64`,
`json` and `bit64` AS `json_and_bit64`,
`json` or `bit64` AS `json_or_bit64`,
`json` + `boolean` AS `json+boolean`,
`json` - `boolean` AS `json-boolean`,
`json` * `boolean` AS `json*boolean`,
`json` / `boolean` AS `json/boolean`,
`json` % `boolean` AS `json%boolean`,
-- `json` ^ `boolean` AS `json^boolean`,
`json` >> `boolean` AS `json>>boolean`,
`json` << `boolean` AS `json<<boolean`,
`json` | `boolean` AS `json|boolean`,
`json` & `boolean` AS `json&boolean`,
`json` || `boolean` AS `json||boolean`,
`json` && `boolean` AS `json&&boolean`,
`json` div `boolean` AS `json_div_boolean`,
`json` mod `boolean` AS `json_mod_boolean`,
-- `json` xor `boolean` AS `json_xor_boolean`,
`json` and `boolean` AS `json_and_boolean`,
`json` or `boolean` AS `json_or_boolean`,
`json` + `date` AS `json+date`,
`json` - `date` AS `json-date`,
`json` * `date` AS `json*date`,
`json` / `date` AS `json/date`,
`json` % `date` AS `json%date`,
`json` ^ `date` AS `json^date`,
`json` >> `date` AS `json>>date`,
`json` << `date` AS `json<<date`,
`json` | `date` AS `json|date`,
`json` & `date` AS `json&date`,
`json` || `date` AS `json||date`,
`json` && `date` AS `json&&date`,
`json` div `date` AS `json_div_date`,
`json` mod `date` AS `json_mod_date`,
`json` xor `date` AS `json_xor_date`,
`json` and `date` AS `json_and_date`,
`json` or `date` AS `json_or_date`,
`json` + `time` AS `json+time`,
`json` - `time` AS `json-time`,
`json` * `time` AS `json*time`,
`json` / `time` AS `json/time`,
`json` % `time` AS `json%time`,
`json` ^ `time` AS `json^time`,
`json` >> `time` AS `json>>time`,
`json` << `time` AS `json<<time`,
`json` | `time` AS `json|time`,
`json` & `time` AS `json&time`,
`json` || `time` AS `json||time`,
`json` && `time` AS `json&&time`,
`json` div `time` AS `json_div_time`,
`json` mod `time` AS `json_mod_time`,
`json` xor `time` AS `json_xor_time`,
`json` and `time` AS `json_and_time`,
`json` or `time` AS `json_or_time`,
`json` + `time(4)` AS `json+time(4)`,
`json` - `time(4)` AS `json-time(4)`,
`json` * `time(4)` AS `json*time(4)`,
`json` / `time(4)` AS `json/time(4)`,
`json` % `time(4)` AS `json%time(4)`,
`json` ^ `time(4)` AS `json^time(4)`,
`json` >> `time(4)` AS `json>>time(4)`,
`json` << `time(4)` AS `json<<time(4)`,
`json` | `time(4)` AS `json|time(4)`,
`json` & `time(4)` AS `json&time(4)`,
`json` || `time(4)` AS `json||time(4)`,
`json` && `time(4)` AS `json&&time(4)`,
`json` div `time(4)` AS `json_div_time(4)`,
`json` mod `time(4)` AS `json_mod_time(4)`,
`json` xor `time(4)` AS `json_xor_time(4)`,
`json` and `time(4)` AS `json_and_time(4)`,
`json` or `time(4)` AS `json_or_time(4)`,
`json` + `datetime` AS `json+datetime`,
`json` - `datetime` AS `json-datetime`,
`json` * `datetime` AS `json*datetime`,
`json` / `datetime` AS `json/datetime`,
`json` % `datetime` AS `json%datetime`,
`json` ^ `datetime` AS `json^datetime`,
`json` >> `datetime` AS `json>>datetime`,
`json` << `datetime` AS `json<<datetime`,
`json` | `datetime` AS `json|datetime`,
`json` & `datetime` AS `json&datetime`,
`json` || `datetime` AS `json||datetime`,
`json` && `datetime` AS `json&&datetime`,
`json` div `datetime` AS `json_div_datetime`,
`json` mod `datetime` AS `json_mod_datetime`,
`json` xor `datetime` AS `json_xor_datetime`,
`json` and `datetime` AS `json_and_datetime`,
`json` or `datetime` AS `json_or_datetime`,
`json` + `datetime(4)` AS `json+datetime(4)`,
`json` - `datetime(4)` AS `json-datetime(4)`,
`json` * `datetime(4)` AS `json*datetime(4)`,
`json` / `datetime(4)` AS `json/datetime(4)`,
`json` % `datetime(4)` AS `json%datetime(4)`,
`json` ^ `datetime(4)` AS `json^datetime(4)`,
`json` >> `datetime(4)` AS `json>>datetime(4)`,
`json` << `datetime(4)` AS `json<<datetime(4)`,
`json` | `datetime(4)` AS `json|datetime(4)`,
`json` & `datetime(4)` AS `json&datetime(4)`,
`json` || `datetime(4)` AS `json||datetime(4)`,
`json` && `datetime(4)` AS `json&&datetime(4)`,
`json` div `datetime(4)` AS `json_div_datetime(4)`,
`json` mod `datetime(4)` AS `json_mod_datetime(4)`,
`json` xor `datetime(4)` AS `json_xor_datetime(4)`,
`json` and `datetime(4)` AS `json_and_datetime(4)`,
`json` or `datetime(4)` AS `json_or_datetime(4)`,
`json` + `timestamp` AS `json+timestamp`,
`json` - `timestamp` AS `json-timestamp`,
`json` * `timestamp` AS `json*timestamp`,
`json` / `timestamp` AS `json/timestamp`,
`json` % `timestamp` AS `json%timestamp`,
`json` ^ `timestamp` AS `json^timestamp`,
`json` >> `timestamp` AS `json>>timestamp`,
`json` << `timestamp` AS `json<<timestamp`,
`json` | `timestamp` AS `json|timestamp`,
`json` & `timestamp` AS `json&timestamp`,
`json` || `timestamp` AS `json||timestamp`,
`json` && `timestamp` AS `json&&timestamp`,
`json` div `timestamp` AS `json_div_timestamp`,
`json` mod `timestamp` AS `json_mod_timestamp`,
`json` xor `timestamp` AS `json_xor_timestamp`,
`json` and `timestamp` AS `json_and_timestamp`,
`json` or `timestamp` AS `json_or_timestamp`,
`json` + `timestamp(4)` AS `json+timestamp(4)`,
`json` - `timestamp(4)` AS `json-timestamp(4)`,
`json` * `timestamp(4)` AS `json*timestamp(4)`,
`json` / `timestamp(4)` AS `json/timestamp(4)`,
`json` % `timestamp(4)` AS `json%timestamp(4)`,
`json` ^ `timestamp(4)` AS `json^timestamp(4)`,
`json` >> `timestamp(4)` AS `json>>timestamp(4)`,
`json` << `timestamp(4)` AS `json<<timestamp(4)`,
`json` | `timestamp(4)` AS `json|timestamp(4)`,
`json` & `timestamp(4)` AS `json&timestamp(4)`,
`json` || `timestamp(4)` AS `json||timestamp(4)`,
`json` && `timestamp(4)` AS `json&&timestamp(4)`,
`json` div `timestamp(4)` AS `json_div_timestamp(4)`,
`json` mod `timestamp(4)` AS `json_mod_timestamp(4)`,
`json` xor `timestamp(4)` AS `json_xor_timestamp(4)`,
`json` and `timestamp(4)` AS `json_and_timestamp(4)`,
`json` or `timestamp(4)` AS `json_or_timestamp(4)`,
`json` + `year` AS `json+year`,
`json` - `year` AS `json-year`,
`json` * `year` AS `json*year`,
`json` / `year` AS `json/year`,
`json` % `year` AS `json%year`,
`json` ^ `year` AS `json^year`,
`json` >> `year` AS `json>>year`,
`json` << `year` AS `json<<year`,
`json` | `year` AS `json|year`,
`json` & `year` AS `json&year`,
-- `json` || `year` AS `json||year`,
-- `json` && `year` AS `json&&year`,
`json` div `year` AS `json_div_year`,
`json` mod `year` AS `json_mod_year`,
`json` xor `year` AS `json_xor_year`,
-- `json` and `year` AS `json_and_year`,
-- `json` or `year` AS `json_or_year`,
`json` + `char` AS `json+char`,
`json` - `char` AS `json-char`,
`json` * `char` AS `json*char`,
`json` / `char` AS `json/char`,
`json` % `char` AS `json%char`,
`json` ^ `char` AS `json^char`,
`json` >> `char` AS `json>>char`,
`json` << `char` AS `json<<char`,
`json` | `char` AS `json|char`,
`json` & `char` AS `json&char`,
`json` || `char` AS `json||char`,
`json` && `char` AS `json&&char`,
`json` div `char` AS `json_div_char`,
`json` mod `char` AS `json_mod_char`,
`json` xor `char` AS `json_xor_char`,
`json` and `char` AS `json_and_char`,
`json` or `char` AS `json_or_char`,
`json` + `varchar` AS `json+varchar`,
`json` - `varchar` AS `json-varchar`,
`json` * `varchar` AS `json*varchar`,
`json` / `varchar` AS `json/varchar`,
`json` % `varchar` AS `json%varchar`,
`json` ^ `varchar` AS `json^varchar`,
`json` >> `varchar` AS `json>>varchar`,
`json` << `varchar` AS `json<<varchar`,
`json` | `varchar` AS `json|varchar`,
`json` & `varchar` AS `json&varchar`,
`json` || `varchar` AS `json||varchar`,
`json` && `varchar` AS `json&&varchar`,
`json` div `varchar` AS `json_div_varchar`,
`json` mod `varchar` AS `json_mod_varchar`,
`json` xor `varchar` AS `json_xor_varchar`,
`json` and `varchar` AS `json_and_varchar`,
`json` or `varchar` AS `json_or_varchar`,
`json` + `binary` AS `json+binary`,
`json` - `binary` AS `json-binary`,
`json` * `binary` AS `json*binary`,
`json` / `binary` AS `json/binary`,
-- `json` % `binary` AS `json%binary`,
`json` ^ `binary` AS `json^binary`,
-- `json` >> `binary` AS `json>>binary`,
-- `json` << `binary` AS `json<<binary`,
-- `json` | `binary` AS `json|binary`,
-- `json` & `binary` AS `json&binary`,
-- `json` || `binary` AS `json||binary`,
-- `json` && `binary` AS `json&&binary`,
`json` div `binary` AS `json_div_binary`,
`json` mod `binary` AS `json_mod_binary`,
`json` xor `binary` AS `json_xor_binary`,
-- `json` and `binary` AS `json_and_binary`,
-- `json` or `binary` AS `json_or_binary`,
`json` + `varbinary` AS `json+varbinary`,
`json` - `varbinary` AS `json-varbinary`,
`json` * `varbinary` AS `json*varbinary`,
`json` / `varbinary` AS `json/varbinary`,
-- `json` % `varbinary` AS `json%varbinary`,
`json` ^ `varbinary` AS `json^varbinary`,
-- `json` >> `varbinary` AS `json>>varbinary`,
-- `json` << `varbinary` AS `json<<varbinary`,
-- `json` | `varbinary` AS `json|varbinary`,
-- `json` & `varbinary` AS `json&varbinary`,
-- `json` || `varbinary` AS `json||varbinary`,
-- `json` && `varbinary` AS `json&&varbinary`,
`json` div `varbinary` AS `json_div_varbinary`,
`json` mod `varbinary` AS `json_mod_varbinary`,
`json` xor `varbinary` AS `json_xor_varbinary`,
-- `json` and `varbinary` AS `json_and_varbinary`,
-- `json` or `varbinary` AS `json_or_varbinary`,
`json` + `tinyblob` AS `json+tinyblob`,
`json` - `tinyblob` AS `json-tinyblob`,
`json` * `tinyblob` AS `json*tinyblob`,
`json` / `tinyblob` AS `json/tinyblob`,
-- `json` % `tinyblob` AS `json%tinyblob`,
`json` ^ `tinyblob` AS `json^tinyblob`,
-- `json` >> `tinyblob` AS `json>>tinyblob`,
-- `json` << `tinyblob` AS `json<<tinyblob`,
-- `json` | `tinyblob` AS `json|tinyblob`,
-- `json` & `tinyblob` AS `json&tinyblob`,
-- `json` || `tinyblob` AS `json||tinyblob`,
-- `json` && `tinyblob` AS `json&&tinyblob`,
`json` div `tinyblob` AS `json_div_tinyblob`,
`json` mod `tinyblob` AS `json_mod_tinyblob`,
`json` xor `tinyblob` AS `json_xor_tinyblob`,
-- `json` and `tinyblob` AS `json_and_tinyblob`,
-- `json` or `tinyblob` AS `json_or_tinyblob`,
`json` + `blob` AS `json+blob`,
`json` - `blob` AS `json-blob`,
`json` * `blob` AS `json*blob`,
`json` / `blob` AS `json/blob`,
-- `json` % `blob` AS `json%blob`,
`json` ^ `blob` AS `json^blob`,
-- `json` >> `blob` AS `json>>blob`,
-- `json` << `blob` AS `json<<blob`,
-- `json` | `blob` AS `json|blob`,
-- `json` & `blob` AS `json&blob`,
-- `json` || `blob` AS `json||blob`,
-- `json` && `blob` AS `json&&blob`,
`json` div `blob` AS `json_div_blob`,
`json` mod `blob` AS `json_mod_blob`,
`json` xor `blob` AS `json_xor_blob`,
-- `json` and `blob` AS `json_and_blob`,
-- `json` or `blob` AS `json_or_blob`,
`json` + `mediumblob` AS `json+mediumblob`,
`json` - `mediumblob` AS `json-mediumblob`,
`json` * `mediumblob` AS `json*mediumblob`,
`json` / `mediumblob` AS `json/mediumblob`,
-- `json` % `mediumblob` AS `json%mediumblob`,
`json` ^ `mediumblob` AS `json^mediumblob`,
-- `json` >> `mediumblob` AS `json>>mediumblob`,
-- `json` << `mediumblob` AS `json<<mediumblob`,
-- `json` | `mediumblob` AS `json|mediumblob`,
-- `json` & `mediumblob` AS `json&mediumblob`,
-- `json` || `mediumblob` AS `json||mediumblob`,
-- `json` && `mediumblob` AS `json&&mediumblob`,
`json` div `mediumblob` AS `json_div_mediumblob`,
`json` mod `mediumblob` AS `json_mod_mediumblob`,
`json` xor `mediumblob` AS `json_xor_mediumblob`,
-- `json` and `mediumblob` AS `json_and_mediumblob`,
-- `json` or `mediumblob` AS `json_or_mediumblob`,
`json` + `longblob` AS `json+longblob`,
`json` - `longblob` AS `json-longblob`,
`json` * `longblob` AS `json*longblob`,
`json` / `longblob` AS `json/longblob`,
-- `json` % `longblob` AS `json%longblob`,
`json` ^ `longblob` AS `json^longblob`,
-- `json` >> `longblob` AS `json>>longblob`,
-- `json` << `longblob` AS `json<<longblob`,
-- `json` | `longblob` AS `json|longblob`,
-- `json` & `longblob` AS `json&longblob`,
-- `json` || `longblob` AS `json||longblob`,
-- `json` && `longblob` AS `json&&longblob`,
`json` div `longblob` AS `json_div_longblob`,
`json` mod `longblob` AS `json_mod_longblob`,
`json` xor `longblob` AS `json_xor_longblob`,
-- `json` and `longblob` AS `json_and_longblob`,
-- `json` or `longblob` AS `json_or_longblob`,
`json` + `text` AS `json+text`,
`json` - `text` AS `json-text`,
`json` * `text` AS `json*text`,
`json` / `text` AS `json/text`,
`json` % `text` AS `json%text`,
`json` ^ `text` AS `json^text`,
`json` >> `text` AS `json>>text`,
`json` << `text` AS `json<<text`,
`json` | `text` AS `json|text`,
`json` & `text` AS `json&text`,
`json` || `text` AS `json||text`,
`json` && `text` AS `json&&text`,
`json` div `text` AS `json_div_text`,
`json` mod `text` AS `json_mod_text`,
`json` xor `text` AS `json_xor_text`,
`json` and `text` AS `json_and_text`,
`json` or `text` AS `json_or_text`,
`json` + `enum_t` AS `json+enum_t`,
`json` - `enum_t` AS `json-enum_t`,
`json` * `enum_t` AS `json*enum_t`,
`json` / `enum_t` AS `json/enum_t`,
`json` % `enum_t` AS `json%enum_t`,
`json` ^ `enum_t` AS `json^enum_t`,
`json` >> `enum_t` AS `json>>enum_t`,
`json` << `enum_t` AS `json<<enum_t`,
`json` | `enum_t` AS `json|enum_t`,
`json` & `enum_t` AS `json&enum_t`,
-- `json` || `enum_t` AS `json||enum_t`,
-- `json` && `enum_t` AS `json&&enum_t`,
`json` div `enum_t` AS `json_div_enum_t`,
`json` mod `enum_t` AS `json_mod_enum_t`,
`json` xor `enum_t` AS `json_xor_enum_t`,
-- `json` and `enum_t` AS `json_and_enum_t`,
-- `json` or `enum_t` AS `json_or_enum_t`,
`json` + `set_t` AS `json+set_t`,
`json` - `set_t` AS `json-set_t`,
`json` * `set_t` AS `json*set_t`,
`json` / `set_t` AS `json/set_t`,
`json` % `set_t` AS `json%set_t`,
`json` ^ `set_t` AS `json^set_t`,
`json` >> `set_t` AS `json>>set_t`,
`json` << `set_t` AS `json<<set_t`,
`json` | `set_t` AS `json|set_t`,
`json` & `set_t` AS `json&set_t`,
-- `json` || `set_t` AS `json||set_t`,
-- `json` && `set_t` AS `json&&set_t`,
`json` div `set_t` AS `json_div_set_t`,
`json` mod `set_t` AS `json_mod_set_t`,
`json` xor `set_t` AS `json_xor_set_t`,
-- `json` and `set_t` AS `json_and_set_t`,
-- `json` or `set_t` AS `json_or_set_t`,
`json` + `json` AS `json+json`,
`json` - `json` AS `json-json`,
`json` * `json` AS `json*json`,
`json` / `json` AS `json/json`,
`json` % `json` AS `json%json`,
`json` ^ `json` AS `json^json`,
`json` >> `json` AS `json>>json`,
`json` << `json` AS `json<<json`,
`json` | `json` AS `json|json`,
`json` & `json` AS `json&json`,
`json` || `json` AS `json||json`,
`json` && `json` AS `json&&json`,
`json` div `json` AS `json_div_json`,
`json` mod `json` AS `json_mod_json`,
`json` xor `json` AS `json_xor_json`,
`json` and `json` AS `json_and_json`,
`json` or `json` AS `json_or_json`,
+ `json` AS `+json`,
- `json` AS `-json`,
~ `json` AS `~json`,
@ `json` AS `@json`,
! `json` AS `!json`,
not `json` AS `not_json`
FROM test_json_table;
SHOW COLUMNS FROM test_json_type;
-- test value
delete from test_json_type;
delete from test_json_table;
insert into test_json_type values(null);
insert into test_json_table values(1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                                   b'1', b'111', true,
                                   '2023-02-05', '19:10:50', '19:10:50.3456', '2023-02-05 19:10:50', '2023-02-05 19:10:50.456', '2023-02-05 19:10:50', '2023-02-05 19:10:50.456', '2023',
                                   '1.23a', '1.23a', '1.23a', '1.23a', '1.23a', '1.23a', '1.23a', '1.23a', '1.23a',
                                   'a', 'a,c',
                                   json_object('a', 1, 'b', 2));
UPDATE test_json_type, test_json_table SET test_json_type.`int1+json` = test_json_table.`int1` + test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`int1-json` = test_json_table.`int1` - test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`int1*json` = test_json_table.`int1` * test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`int1/json` = test_json_table.`int1` / test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`int1%json` = test_json_table.`int1` % test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`int1^json` = test_json_table.`int1` ^ test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`int1>>json` = test_json_table.`int1` >> test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`int1<<json` = test_json_table.`int1` << test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`int1|json` = test_json_table.`int1` | test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`int1&json` = test_json_table.`int1` & test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`int1||json` = test_json_table.`int1` || test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`int1&&json` = test_json_table.`int1` && test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`int1_div_json` = test_json_table.`int1` div test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`int1_mod_json` = test_json_table.`int1` mod test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`int1_xor_json` = test_json_table.`int1` xor test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`int1_and_json` = test_json_table.`int1` and test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`int1_or_json` = test_json_table.`int1` or test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`uint1+json` = test_json_table.`uint1` + test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`uint1-json` = test_json_table.`uint1` - test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`uint1*json` = test_json_table.`uint1` * test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`uint1/json` = test_json_table.`uint1` / test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`uint1%json` = test_json_table.`uint1` % test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`uint1^json` = test_json_table.`uint1` ^ test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`uint1>>json` = test_json_table.`uint1` >> test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`uint1<<json` = test_json_table.`uint1` << test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`uint1|json` = test_json_table.`uint1` | test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`uint1&json` = test_json_table.`uint1` & test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`uint1||json` = test_json_table.`uint1` || test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`uint1&&json` = test_json_table.`uint1` && test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`uint1_div_json` = test_json_table.`uint1` div test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`uint1_mod_json` = test_json_table.`uint1` mod test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`uint1_xor_json` = test_json_table.`uint1` xor test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`uint1_and_json` = test_json_table.`uint1` and test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`uint1_or_json` = test_json_table.`uint1` or test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`int2+json` = test_json_table.`int2` + test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`int2-json` = test_json_table.`int2` - test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`int2*json` = test_json_table.`int2` * test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`int2/json` = test_json_table.`int2` / test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`int2%json` = test_json_table.`int2` % test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`int2^json` = test_json_table.`int2` ^ test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`int2>>json` = test_json_table.`int2` >> test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`int2<<json` = test_json_table.`int2` << test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`int2|json` = test_json_table.`int2` | test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`int2&json` = test_json_table.`int2` & test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`int2||json` = test_json_table.`int2` || test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`int2&&json` = test_json_table.`int2` && test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`int2_div_json` = test_json_table.`int2` div test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`int2_mod_json` = test_json_table.`int2` mod test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`int2_xor_json` = test_json_table.`int2` xor test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`int2_and_json` = test_json_table.`int2` and test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`int2_or_json` = test_json_table.`int2` or test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`uint2+json` = test_json_table.`uint2` + test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`uint2-json` = test_json_table.`uint2` - test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`uint2*json` = test_json_table.`uint2` * test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`uint2/json` = test_json_table.`uint2` / test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`uint2%json` = test_json_table.`uint2` % test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`uint2^json` = test_json_table.`uint2` ^ test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`uint2>>json` = test_json_table.`uint2` >> test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`uint2<<json` = test_json_table.`uint2` << test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`uint2|json` = test_json_table.`uint2` | test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`uint2&json` = test_json_table.`uint2` & test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`uint2||json` = test_json_table.`uint2` || test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`uint2&&json` = test_json_table.`uint2` && test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`uint2_div_json` = test_json_table.`uint2` div test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`uint2_mod_json` = test_json_table.`uint2` mod test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`uint2_xor_json` = test_json_table.`uint2` xor test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`uint2_and_json` = test_json_table.`uint2` and test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`uint2_or_json` = test_json_table.`uint2` or test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`int4+json` = test_json_table.`int4` + test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`int4-json` = test_json_table.`int4` - test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`int4*json` = test_json_table.`int4` * test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`int4/json` = test_json_table.`int4` / test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`int4%json` = test_json_table.`int4` % test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`int4^json` = test_json_table.`int4` ^ test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`int4>>json` = test_json_table.`int4` >> test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`int4<<json` = test_json_table.`int4` << test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`int4|json` = test_json_table.`int4` | test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`int4&json` = test_json_table.`int4` & test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`int4||json` = test_json_table.`int4` || test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`int4&&json` = test_json_table.`int4` && test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`int4_div_json` = test_json_table.`int4` div test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`int4_mod_json` = test_json_table.`int4` mod test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`int4_xor_json` = test_json_table.`int4` xor test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`int4_and_json` = test_json_table.`int4` and test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`int4_or_json` = test_json_table.`int4` or test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`uint4+json` = test_json_table.`uint4` + test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`uint4-json` = test_json_table.`uint4` - test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`uint4*json` = test_json_table.`uint4` * test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`uint4/json` = test_json_table.`uint4` / test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`uint4%json` = test_json_table.`uint4` % test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`uint4^json` = test_json_table.`uint4` ^ test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`uint4>>json` = test_json_table.`uint4` >> test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`uint4<<json` = test_json_table.`uint4` << test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`uint4|json` = test_json_table.`uint4` | test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`uint4&json` = test_json_table.`uint4` & test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`uint4||json` = test_json_table.`uint4` || test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`uint4&&json` = test_json_table.`uint4` && test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`uint4_div_json` = test_json_table.`uint4` div test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`uint4_mod_json` = test_json_table.`uint4` mod test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`uint4_xor_json` = test_json_table.`uint4` xor test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`uint4_and_json` = test_json_table.`uint4` and test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`uint4_or_json` = test_json_table.`uint4` or test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`int8+json` = test_json_table.`int8` + test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`int8-json` = test_json_table.`int8` - test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`int8*json` = test_json_table.`int8` * test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`int8/json` = test_json_table.`int8` / test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`int8%json` = test_json_table.`int8` % test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`int8^json` = test_json_table.`int8` ^ test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`int8>>json` = test_json_table.`int8` >> test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`int8<<json` = test_json_table.`int8` << test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`int8|json` = test_json_table.`int8` | test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`int8&json` = test_json_table.`int8` & test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`int8||json` = test_json_table.`int8` || test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`int8&&json` = test_json_table.`int8` && test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`int8_div_json` = test_json_table.`int8` div test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`int8_mod_json` = test_json_table.`int8` mod test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`int8_xor_json` = test_json_table.`int8` xor test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`int8_and_json` = test_json_table.`int8` and test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`int8_or_json` = test_json_table.`int8` or test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`uint8+json` = test_json_table.`uint8` + test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`uint8-json` = test_json_table.`uint8` - test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`uint8*json` = test_json_table.`uint8` * test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`uint8/json` = test_json_table.`uint8` / test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`uint8%json` = test_json_table.`uint8` % test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`uint8^json` = test_json_table.`uint8` ^ test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`uint8>>json` = test_json_table.`uint8` >> test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`uint8<<json` = test_json_table.`uint8` << test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`uint8|json` = test_json_table.`uint8` | test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`uint8&json` = test_json_table.`uint8` & test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`uint8||json` = test_json_table.`uint8` || test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`uint8&&json` = test_json_table.`uint8` && test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`uint8_div_json` = test_json_table.`uint8` div test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`uint8_mod_json` = test_json_table.`uint8` mod test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`uint8_xor_json` = test_json_table.`uint8` xor test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`uint8_and_json` = test_json_table.`uint8` and test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`uint8_or_json` = test_json_table.`uint8` or test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`float4+json` = test_json_table.`float4` + test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`float4-json` = test_json_table.`float4` - test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`float4*json` = test_json_table.`float4` * test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`float4/json` = test_json_table.`float4` / test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`float4%json` = test_json_table.`float4` % test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`float4^json` = test_json_table.`float4` ^ test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`float4>>json` = test_json_table.`float4` >> test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`float4<<json` = test_json_table.`float4` << test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`float4|json` = test_json_table.`float4` | test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`float4&json` = test_json_table.`float4` & test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`float4||json` = test_json_table.`float4` || test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`float4&&json` = test_json_table.`float4` && test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`float4_div_json` = test_json_table.`float4` div test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`float4_mod_json` = test_json_table.`float4` mod test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`float4_xor_json` = test_json_table.`float4` xor test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`float4_and_json` = test_json_table.`float4` and test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`float4_or_json` = test_json_table.`float4` or test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`float8+json` = test_json_table.`float8` + test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`float8-json` = test_json_table.`float8` - test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`float8*json` = test_json_table.`float8` * test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`float8/json` = test_json_table.`float8` / test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`float8%json` = test_json_table.`float8` % test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`float8^json` = test_json_table.`float8` ^ test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`float8>>json` = test_json_table.`float8` >> test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`float8<<json` = test_json_table.`float8` << test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`float8|json` = test_json_table.`float8` | test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`float8&json` = test_json_table.`float8` & test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`float8||json` = test_json_table.`float8` || test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`float8&&json` = test_json_table.`float8` && test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`float8_div_json` = test_json_table.`float8` div test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`float8_mod_json` = test_json_table.`float8` mod test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`float8_xor_json` = test_json_table.`float8` xor test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`float8_and_json` = test_json_table.`float8` and test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`float8_or_json` = test_json_table.`float8` or test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`numeric+json` = test_json_table.`numeric` + test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`numeric-json` = test_json_table.`numeric` - test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`numeric*json` = test_json_table.`numeric` * test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`numeric/json` = test_json_table.`numeric` / test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`numeric%json` = test_json_table.`numeric` % test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`numeric^json` = test_json_table.`numeric` ^ test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`numeric>>json` = test_json_table.`numeric` >> test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`numeric<<json` = test_json_table.`numeric` << test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`numeric|json` = test_json_table.`numeric` | test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`numeric&json` = test_json_table.`numeric` & test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`numeric||json` = test_json_table.`numeric` || test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`numeric&&json` = test_json_table.`numeric` && test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`numeric_div_json` = test_json_table.`numeric` div test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`numeric_mod_json` = test_json_table.`numeric` mod test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`numeric_xor_json` = test_json_table.`numeric` xor test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`numeric_and_json` = test_json_table.`numeric` and test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`numeric_or_json` = test_json_table.`numeric` or test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`bit1+json` = test_json_table.`bit1` + test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`bit1-json` = test_json_table.`bit1` - test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`bit1*json` = test_json_table.`bit1` * test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`bit1/json` = test_json_table.`bit1` / test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`bit1%json` = test_json_table.`bit1` % test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`bit1^json` = test_json_table.`bit1` ^ test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`bit1>>json` = test_json_table.`bit1` >> test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`bit1<<json` = test_json_table.`bit1` << test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`bit1|json` = test_json_table.`bit1` | test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`bit1&json` = test_json_table.`bit1` & test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`bit1||json` = test_json_table.`bit1` || test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`bit1&&json` = test_json_table.`bit1` && test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`bit1_div_json` = test_json_table.`bit1` div test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`bit1_mod_json` = test_json_table.`bit1` mod test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`bit1_xor_json` = test_json_table.`bit1` xor test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`bit1_and_json` = test_json_table.`bit1` and test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`bit1_or_json` = test_json_table.`bit1` or test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`bit64+json` = test_json_table.`bit64` + test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`bit64-json` = test_json_table.`bit64` - test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`bit64*json` = test_json_table.`bit64` * test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`bit64/json` = test_json_table.`bit64` / test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`bit64%json` = test_json_table.`bit64` % test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`bit64^json` = test_json_table.`bit64` ^ test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`bit64>>json` = test_json_table.`bit64` >> test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`bit64<<json` = test_json_table.`bit64` << test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`bit64|json` = test_json_table.`bit64` | test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`bit64&json` = test_json_table.`bit64` & test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`bit64||json` = test_json_table.`bit64` || test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`bit64&&json` = test_json_table.`bit64` && test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`bit64_div_json` = test_json_table.`bit64` div test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`bit64_mod_json` = test_json_table.`bit64` mod test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`bit64_xor_json` = test_json_table.`bit64` xor test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`bit64_and_json` = test_json_table.`bit64` and test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`bit64_or_json` = test_json_table.`bit64` or test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`boolean+json` = test_json_table.`boolean` + test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`boolean-json` = test_json_table.`boolean` - test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`boolean*json` = test_json_table.`boolean` * test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`boolean/json` = test_json_table.`boolean` / test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`boolean%json` = test_json_table.`boolean` % test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`boolean^json` = test_json_table.`boolean` ^ test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`boolean>>json` = test_json_table.`boolean` >> test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`boolean<<json` = test_json_table.`boolean` << test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`boolean|json` = test_json_table.`boolean` | test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`boolean&json` = test_json_table.`boolean` & test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`boolean||json` = test_json_table.`boolean` || test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`boolean&&json` = test_json_table.`boolean` && test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`boolean_div_json` = test_json_table.`boolean` div test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`boolean_mod_json` = test_json_table.`boolean` mod test_json_table.`json`;
-- UPDATE test_json_type, test_json_table SET test_json_type.`boolean_xor_json` = test_json_table.`boolean` xor test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`boolean_and_json` = test_json_table.`boolean` and test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`boolean_or_json` = test_json_table.`boolean` or test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`date+json` = test_json_table.`date` + test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`date-json` = test_json_table.`date` - test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`date*json` = test_json_table.`date` * test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`date/json` = test_json_table.`date` / test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`date%json` = test_json_table.`date` % test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`date^json` = test_json_table.`date` ^ test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`date>>json` = test_json_table.`date` >> test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`date<<json` = test_json_table.`date` << test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`date|json` = test_json_table.`date` | test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`date&json` = test_json_table.`date` & test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`date||json` = test_json_table.`date` || test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`date&&json` = test_json_table.`date` && test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`date_div_json` = test_json_table.`date` div test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`date_mod_json` = test_json_table.`date` mod test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`date_xor_json` = test_json_table.`date` xor test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`date_and_json` = test_json_table.`date` and test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`date_or_json` = test_json_table.`date` or test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`time+json` = test_json_table.`time` + test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`time-json` = test_json_table.`time` - test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`time*json` = test_json_table.`time` * test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`time/json` = test_json_table.`time` / test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`time%json` = test_json_table.`time` % test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`time^json` = test_json_table.`time` ^ test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`time>>json` = test_json_table.`time` >> test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`time<<json` = test_json_table.`time` << test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`time|json` = test_json_table.`time` | test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`time&json` = test_json_table.`time` & test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`time||json` = test_json_table.`time` || test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`time&&json` = test_json_table.`time` && test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`time_div_json` = test_json_table.`time` div test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`time_mod_json` = test_json_table.`time` mod test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`time_xor_json` = test_json_table.`time` xor test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`time_and_json` = test_json_table.`time` and test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`time_or_json` = test_json_table.`time` or test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`time(4)+json` = test_json_table.`time(4)` + test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`time(4)-json` = test_json_table.`time(4)` - test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`time(4)*json` = test_json_table.`time(4)` * test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`time(4)/json` = test_json_table.`time(4)` / test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`time(4)%json` = test_json_table.`time(4)` % test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`time(4)^json` = test_json_table.`time(4)` ^ test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`time(4)>>json` = test_json_table.`time(4)` >> test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`time(4)<<json` = test_json_table.`time(4)` << test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`time(4)|json` = test_json_table.`time(4)` | test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`time(4)&json` = test_json_table.`time(4)` & test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`time(4)||json` = test_json_table.`time(4)` || test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`time(4)&&json` = test_json_table.`time(4)` && test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`time(4)_div_json` = test_json_table.`time(4)` div test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`time(4)_mod_json` = test_json_table.`time(4)` mod test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`time(4)_xor_json` = test_json_table.`time(4)` xor test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`time(4)_and_json` = test_json_table.`time(4)` and test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`time(4)_or_json` = test_json_table.`time(4)` or test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`datetime+json` = test_json_table.`datetime` + test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`datetime-json` = test_json_table.`datetime` - test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`datetime*json` = test_json_table.`datetime` * test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`datetime/json` = test_json_table.`datetime` / test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`datetime%json` = test_json_table.`datetime` % test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`datetime^json` = test_json_table.`datetime` ^ test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`datetime>>json` = test_json_table.`datetime` >> test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`datetime<<json` = test_json_table.`datetime` << test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`datetime|json` = test_json_table.`datetime` | test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`datetime&json` = test_json_table.`datetime` & test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`datetime||json` = test_json_table.`datetime` || test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`datetime&&json` = test_json_table.`datetime` && test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`datetime_div_json` = test_json_table.`datetime` div test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`datetime_mod_json` = test_json_table.`datetime` mod test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`datetime_xor_json` = test_json_table.`datetime` xor test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`datetime_and_json` = test_json_table.`datetime` and test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`datetime_or_json` = test_json_table.`datetime` or test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`datetime(4)+json` = test_json_table.`datetime(4)` + test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`datetime(4)-json` = test_json_table.`datetime(4)` - test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`datetime(4)*json` = test_json_table.`datetime(4)` * test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`datetime(4)/json` = test_json_table.`datetime(4)` / test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`datetime(4)%json` = test_json_table.`datetime(4)` % test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`datetime(4)^json` = test_json_table.`datetime(4)` ^ test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`datetime(4)>>json` = test_json_table.`datetime(4)` >> test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`datetime(4)<<json` = test_json_table.`datetime(4)` << test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`datetime(4)|json` = test_json_table.`datetime(4)` | test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`datetime(4)&json` = test_json_table.`datetime(4)` & test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`datetime(4)||json` = test_json_table.`datetime(4)` || test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`datetime(4)&&json` = test_json_table.`datetime(4)` && test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`datetime(4)_div_json` = test_json_table.`datetime(4)` div test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`datetime(4)_mod_json` = test_json_table.`datetime(4)` mod test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`datetime(4)_xor_json` = test_json_table.`datetime(4)` xor test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`datetime(4)_and_json` = test_json_table.`datetime(4)` and test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`datetime(4)_or_json` = test_json_table.`datetime(4)` or test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`timestamp+json` = test_json_table.`timestamp` + test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`timestamp-json` = test_json_table.`timestamp` - test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`timestamp*json` = test_json_table.`timestamp` * test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`timestamp/json` = test_json_table.`timestamp` / test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`timestamp%json` = test_json_table.`timestamp` % test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`timestamp^json` = test_json_table.`timestamp` ^ test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`timestamp>>json` = test_json_table.`timestamp` >> test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`timestamp<<json` = test_json_table.`timestamp` << test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`timestamp|json` = test_json_table.`timestamp` | test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`timestamp&json` = test_json_table.`timestamp` & test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`timestamp||json` = test_json_table.`timestamp` || test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`timestamp&&json` = test_json_table.`timestamp` && test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`timestamp_div_json` = test_json_table.`timestamp` div test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`timestamp_mod_json` = test_json_table.`timestamp` mod test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`timestamp_xor_json` = test_json_table.`timestamp` xor test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`timestamp_and_json` = test_json_table.`timestamp` and test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`timestamp_or_json` = test_json_table.`timestamp` or test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`timestamp(4)+json` = test_json_table.`timestamp(4)` + test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`timestamp(4)-json` = test_json_table.`timestamp(4)` - test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`timestamp(4)*json` = test_json_table.`timestamp(4)` * test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`timestamp(4)/json` = test_json_table.`timestamp(4)` / test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`timestamp(4)%json` = test_json_table.`timestamp(4)` % test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`timestamp(4)^json` = test_json_table.`timestamp(4)` ^ test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`timestamp(4)>>json` = test_json_table.`timestamp(4)` >> test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`timestamp(4)<<json` = test_json_table.`timestamp(4)` << test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`timestamp(4)|json` = test_json_table.`timestamp(4)` | test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`timestamp(4)&json` = test_json_table.`timestamp(4)` & test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`timestamp(4)||json` = test_json_table.`timestamp(4)` || test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`timestamp(4)&&json` = test_json_table.`timestamp(4)` && test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`timestamp(4)_div_json` = test_json_table.`timestamp(4)` div test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`timestamp(4)_mod_json` = test_json_table.`timestamp(4)` mod test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`timestamp(4)_xor_json` = test_json_table.`timestamp(4)` xor test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`timestamp(4)_and_json` = test_json_table.`timestamp(4)` and test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`timestamp(4)_or_json` = test_json_table.`timestamp(4)` or test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`year+json` = test_json_table.`year` + test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`year-json` = test_json_table.`year` - test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`year*json` = test_json_table.`year` * test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`year/json` = test_json_table.`year` / test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`year%json` = test_json_table.`year` % test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`year^json` = test_json_table.`year` ^ test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`year>>json` = test_json_table.`year` >> test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`year<<json` = test_json_table.`year` << test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`year|json` = test_json_table.`year` | test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`year&json` = test_json_table.`year` & test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`year||json` = test_json_table.`year` || test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`year&&json` = test_json_table.`year` && test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`year_div_json` = test_json_table.`year` div test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`year_mod_json` = test_json_table.`year` mod test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`year_xor_json` = test_json_table.`year` xor test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`year_and_json` = test_json_table.`year` and test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`year_or_json` = test_json_table.`year` or test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`char+json` = test_json_table.`char` + test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`char-json` = test_json_table.`char` - test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`char*json` = test_json_table.`char` * test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`char/json` = test_json_table.`char` / test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`char%json` = test_json_table.`char` % test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`char^json` = test_json_table.`char` ^ test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`char>>json` = test_json_table.`char` >> test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`char<<json` = test_json_table.`char` << test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`char|json` = test_json_table.`char` | test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`char&json` = test_json_table.`char` & test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`char||json` = test_json_table.`char` || test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`char&&json` = test_json_table.`char` && test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`char_div_json` = test_json_table.`char` div test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`char_mod_json` = test_json_table.`char` mod test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`char_xor_json` = test_json_table.`char` xor test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`char_and_json` = test_json_table.`char` and test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`char_or_json` = test_json_table.`char` or test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`varchar+json` = test_json_table.`varchar` + test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`varchar-json` = test_json_table.`varchar` - test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`varchar*json` = test_json_table.`varchar` * test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`varchar/json` = test_json_table.`varchar` / test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`varchar%json` = test_json_table.`varchar` % test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`varchar^json` = test_json_table.`varchar` ^ test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`varchar>>json` = test_json_table.`varchar` >> test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`varchar<<json` = test_json_table.`varchar` << test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`varchar|json` = test_json_table.`varchar` | test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`varchar&json` = test_json_table.`varchar` & test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`varchar||json` = test_json_table.`varchar` || test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`varchar&&json` = test_json_table.`varchar` && test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`varchar_div_json` = test_json_table.`varchar` div test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`varchar_mod_json` = test_json_table.`varchar` mod test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`varchar_xor_json` = test_json_table.`varchar` xor test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`varchar_and_json` = test_json_table.`varchar` and test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`varchar_or_json` = test_json_table.`varchar` or test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`binary+json` = test_json_table.`binary` + test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`binary-json` = test_json_table.`binary` - test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`binary*json` = test_json_table.`binary` * test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`binary/json` = test_json_table.`binary` / test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`binary%json` = test_json_table.`binary` % test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`binary^json` = test_json_table.`binary` ^ test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`binary>>json` = test_json_table.`binary` >> test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`binary<<json` = test_json_table.`binary` << test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`binary|json` = test_json_table.`binary` | test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`binary&json` = test_json_table.`binary` & test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`binary||json` = test_json_table.`binary` || test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`binary&&json` = test_json_table.`binary` && test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`binary_div_json` = test_json_table.`binary` div test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`binary_mod_json` = test_json_table.`binary` mod test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`binary_xor_json` = test_json_table.`binary` xor test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`binary_and_json` = test_json_table.`binary` and test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`binary_or_json` = test_json_table.`binary` or test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`varbinary+json` = test_json_table.`varbinary` + test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`varbinary-json` = test_json_table.`varbinary` - test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`varbinary*json` = test_json_table.`varbinary` * test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`varbinary/json` = test_json_table.`varbinary` / test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`varbinary%json` = test_json_table.`varbinary` % test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`varbinary^json` = test_json_table.`varbinary` ^ test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`varbinary>>json` = test_json_table.`varbinary` >> test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`varbinary<<json` = test_json_table.`varbinary` << test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`varbinary|json` = test_json_table.`varbinary` | test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`varbinary&json` = test_json_table.`varbinary` & test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`varbinary||json` = test_json_table.`varbinary` || test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`varbinary&&json` = test_json_table.`varbinary` && test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`varbinary_div_json` = test_json_table.`varbinary` div test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`varbinary_mod_json` = test_json_table.`varbinary` mod test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`varbinary_xor_json` = test_json_table.`varbinary` xor test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`varbinary_and_json` = test_json_table.`varbinary` and test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`varbinary_or_json` = test_json_table.`varbinary` or test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`tinyblob+json` = test_json_table.`tinyblob` + test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`tinyblob-json` = test_json_table.`tinyblob` - test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`tinyblob*json` = test_json_table.`tinyblob` * test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`tinyblob/json` = test_json_table.`tinyblob` / test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`tinyblob%json` = test_json_table.`tinyblob` % test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`tinyblob^json` = test_json_table.`tinyblob` ^ test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`tinyblob>>json` = test_json_table.`tinyblob` >> test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`tinyblob<<json` = test_json_table.`tinyblob` << test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`tinyblob|json` = test_json_table.`tinyblob` | test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`tinyblob&json` = test_json_table.`tinyblob` & test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`tinyblob||json` = test_json_table.`tinyblob` || test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`tinyblob&&json` = test_json_table.`tinyblob` && test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`tinyblob_div_json` = test_json_table.`tinyblob` div test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`tinyblob_mod_json` = test_json_table.`tinyblob` mod test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`tinyblob_xor_json` = test_json_table.`tinyblob` xor test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`tinyblob_and_json` = test_json_table.`tinyblob` and test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`tinyblob_or_json` = test_json_table.`tinyblob` or test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`blob+json` = test_json_table.`blob` + test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`blob-json` = test_json_table.`blob` - test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`blob*json` = test_json_table.`blob` * test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`blob/json` = test_json_table.`blob` / test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`blob%json` = test_json_table.`blob` % test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`blob^json` = test_json_table.`blob` ^ test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`blob>>json` = test_json_table.`blob` >> test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`blob<<json` = test_json_table.`blob` << test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`blob|json` = test_json_table.`blob` | test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`blob&json` = test_json_table.`blob` & test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`blob||json` = test_json_table.`blob` || test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`blob&&json` = test_json_table.`blob` && test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`blob_div_json` = test_json_table.`blob` div test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`blob_mod_json` = test_json_table.`blob` mod test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`blob_xor_json` = test_json_table.`blob` xor test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`blob_and_json` = test_json_table.`blob` and test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`blob_or_json` = test_json_table.`blob` or test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`mediumblob+json` = test_json_table.`mediumblob` + test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`mediumblob-json` = test_json_table.`mediumblob` - test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`mediumblob*json` = test_json_table.`mediumblob` * test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`mediumblob/json` = test_json_table.`mediumblob` / test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`mediumblob%json` = test_json_table.`mediumblob` % test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`mediumblob^json` = test_json_table.`mediumblob` ^ test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`mediumblob>>json` = test_json_table.`mediumblob` >> test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`mediumblob<<json` = test_json_table.`mediumblob` << test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`mediumblob|json` = test_json_table.`mediumblob` | test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`mediumblob&json` = test_json_table.`mediumblob` & test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`mediumblob||json` = test_json_table.`mediumblob` || test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`mediumblob&&json` = test_json_table.`mediumblob` && test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`mediumblob_div_json` = test_json_table.`mediumblob` div test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`mediumblob_mod_json` = test_json_table.`mediumblob` mod test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`mediumblob_xor_json` = test_json_table.`mediumblob` xor test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`mediumblob_and_json` = test_json_table.`mediumblob` and test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`mediumblob_or_json` = test_json_table.`mediumblob` or test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`longblob+json` = test_json_table.`longblob` + test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`longblob-json` = test_json_table.`longblob` - test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`longblob*json` = test_json_table.`longblob` * test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`longblob/json` = test_json_table.`longblob` / test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`longblob%json` = test_json_table.`longblob` % test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`longblob^json` = test_json_table.`longblob` ^ test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`longblob>>json` = test_json_table.`longblob` >> test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`longblob<<json` = test_json_table.`longblob` << test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`longblob|json` = test_json_table.`longblob` | test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`longblob&json` = test_json_table.`longblob` & test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`longblob||json` = test_json_table.`longblob` || test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`longblob&&json` = test_json_table.`longblob` && test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`longblob_div_json` = test_json_table.`longblob` div test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`longblob_mod_json` = test_json_table.`longblob` mod test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`longblob_xor_json` = test_json_table.`longblob` xor test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`longblob_and_json` = test_json_table.`longblob` and test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`longblob_or_json` = test_json_table.`longblob` or test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`text+json` = test_json_table.`text` + test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`text-json` = test_json_table.`text` - test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`text*json` = test_json_table.`text` * test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`text/json` = test_json_table.`text` / test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`text%json` = test_json_table.`text` % test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`text^json` = test_json_table.`text` ^ test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`text>>json` = test_json_table.`text` >> test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`text<<json` = test_json_table.`text` << test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`text|json` = test_json_table.`text` | test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`text&json` = test_json_table.`text` & test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`text||json` = test_json_table.`text` || test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`text&&json` = test_json_table.`text` && test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`text_div_json` = test_json_table.`text` div test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`text_mod_json` = test_json_table.`text` mod test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`text_xor_json` = test_json_table.`text` xor test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`text_and_json` = test_json_table.`text` and test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`text_or_json` = test_json_table.`text` or test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`enum_t+json` = test_json_table.`enum_t` + test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`enum_t-json` = test_json_table.`enum_t` - test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`enum_t*json` = test_json_table.`enum_t` * test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`enum_t/json` = test_json_table.`enum_t` / test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`enum_t%json` = test_json_table.`enum_t` % test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`enum_t^json` = test_json_table.`enum_t` ^ test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`enum_t>>json` = test_json_table.`enum_t` >> test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`enum_t<<json` = test_json_table.`enum_t` << test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`enum_t|json` = test_json_table.`enum_t` | test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`enum_t&json` = test_json_table.`enum_t` & test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`enum_t||json` = test_json_table.`enum_t` || test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`enum_t&&json` = test_json_table.`enum_t` && test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`enum_t_div_json` = test_json_table.`enum_t` div test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`enum_t_mod_json` = test_json_table.`enum_t` mod test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`enum_t_xor_json` = test_json_table.`enum_t` xor test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`enum_t_and_json` = test_json_table.`enum_t` and test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`enum_t_or_json` = test_json_table.`enum_t` or test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`set_t+json` = test_json_table.`set_t` + test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`set_t-json` = test_json_table.`set_t` - test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`set_t*json` = test_json_table.`set_t` * test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`set_t/json` = test_json_table.`set_t` / test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`set_t%json` = test_json_table.`set_t` % test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`set_t^json` = test_json_table.`set_t` ^ test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`set_t>>json` = test_json_table.`set_t` >> test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`set_t<<json` = test_json_table.`set_t` << test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`set_t|json` = test_json_table.`set_t` | test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`set_t&json` = test_json_table.`set_t` & test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`set_t||json` = test_json_table.`set_t` || test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`set_t&&json` = test_json_table.`set_t` && test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`set_t_div_json` = test_json_table.`set_t` div test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`set_t_mod_json` = test_json_table.`set_t` mod test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`set_t_xor_json` = test_json_table.`set_t` xor test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`set_t_and_json` = test_json_table.`set_t` and test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`set_t_or_json` = test_json_table.`set_t` or test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`json+int1` = test_json_table.`json` + test_json_table.`int1`;
UPDATE test_json_type, test_json_table SET test_json_type.`json-int1` = test_json_table.`json` - test_json_table.`int1`;
UPDATE test_json_type, test_json_table SET test_json_type.`json*int1` = test_json_table.`json` * test_json_table.`int1`;
UPDATE test_json_type, test_json_table SET test_json_type.`json/int1` = test_json_table.`json` / test_json_table.`int1`;
UPDATE test_json_type, test_json_table SET test_json_type.`json%int1` = test_json_table.`json` % test_json_table.`int1`;
UPDATE test_json_type, test_json_table SET test_json_type.`json^int1` = test_json_table.`json` ^ test_json_table.`int1`;
UPDATE test_json_type, test_json_table SET test_json_type.`json>>int1` = test_json_table.`json` >> test_json_table.`int1`;
UPDATE test_json_type, test_json_table SET test_json_type.`json<<int1` = test_json_table.`json` << test_json_table.`int1`;
UPDATE test_json_type, test_json_table SET test_json_type.`json|int1` = test_json_table.`json` | test_json_table.`int1`;
UPDATE test_json_type, test_json_table SET test_json_type.`json&int1` = test_json_table.`json` & test_json_table.`int1`;
UPDATE test_json_type, test_json_table SET test_json_type.`json||int1` = test_json_table.`json` || test_json_table.`int1`;
UPDATE test_json_type, test_json_table SET test_json_type.`json&&int1` = test_json_table.`json` && test_json_table.`int1`;
UPDATE test_json_type, test_json_table SET test_json_type.`json_div_int1` = test_json_table.`json` div test_json_table.`int1`;
UPDATE test_json_type, test_json_table SET test_json_type.`json_mod_int1` = test_json_table.`json` mod test_json_table.`int1`;
UPDATE test_json_type, test_json_table SET test_json_type.`json_xor_int1` = test_json_table.`json` xor test_json_table.`int1`;
UPDATE test_json_type, test_json_table SET test_json_type.`json_and_int1` = test_json_table.`json` and test_json_table.`int1`;
UPDATE test_json_type, test_json_table SET test_json_type.`json_or_int1` = test_json_table.`json` or test_json_table.`int1`;
UPDATE test_json_type, test_json_table SET test_json_type.`json+uint1` = test_json_table.`json` + test_json_table.`uint1`;
UPDATE test_json_type, test_json_table SET test_json_type.`json-uint1` = test_json_table.`json` - test_json_table.`uint1`;
UPDATE test_json_type, test_json_table SET test_json_type.`json*uint1` = test_json_table.`json` * test_json_table.`uint1`;
UPDATE test_json_type, test_json_table SET test_json_type.`json/uint1` = test_json_table.`json` / test_json_table.`uint1`;
UPDATE test_json_type, test_json_table SET test_json_type.`json%uint1` = test_json_table.`json` % test_json_table.`uint1`;
UPDATE test_json_type, test_json_table SET test_json_type.`json^uint1` = test_json_table.`json` ^ test_json_table.`uint1`;
UPDATE test_json_type, test_json_table SET test_json_type.`json>>uint1` = test_json_table.`json` >> test_json_table.`uint1`;
UPDATE test_json_type, test_json_table SET test_json_type.`json<<uint1` = test_json_table.`json` << test_json_table.`uint1`;
UPDATE test_json_type, test_json_table SET test_json_type.`json|uint1` = test_json_table.`json` | test_json_table.`uint1`;
UPDATE test_json_type, test_json_table SET test_json_type.`json&uint1` = test_json_table.`json` & test_json_table.`uint1`;
UPDATE test_json_type, test_json_table SET test_json_type.`json||uint1` = test_json_table.`json` || test_json_table.`uint1`;
UPDATE test_json_type, test_json_table SET test_json_type.`json&&uint1` = test_json_table.`json` && test_json_table.`uint1`;
UPDATE test_json_type, test_json_table SET test_json_type.`json_div_uint1` = test_json_table.`json` div test_json_table.`uint1`;
UPDATE test_json_type, test_json_table SET test_json_type.`json_mod_uint1` = test_json_table.`json` mod test_json_table.`uint1`;
UPDATE test_json_type, test_json_table SET test_json_type.`json_xor_uint1` = test_json_table.`json` xor test_json_table.`uint1`;
UPDATE test_json_type, test_json_table SET test_json_type.`json_and_uint1` = test_json_table.`json` and test_json_table.`uint1`;
UPDATE test_json_type, test_json_table SET test_json_type.`json_or_uint1` = test_json_table.`json` or test_json_table.`uint1`;
UPDATE test_json_type, test_json_table SET test_json_type.`json+int2` = test_json_table.`json` + test_json_table.`int2`;
UPDATE test_json_type, test_json_table SET test_json_type.`json-int2` = test_json_table.`json` - test_json_table.`int2`;
UPDATE test_json_type, test_json_table SET test_json_type.`json*int2` = test_json_table.`json` * test_json_table.`int2`;
UPDATE test_json_type, test_json_table SET test_json_type.`json/int2` = test_json_table.`json` / test_json_table.`int2`;
UPDATE test_json_type, test_json_table SET test_json_type.`json%int2` = test_json_table.`json` % test_json_table.`int2`;
UPDATE test_json_type, test_json_table SET test_json_type.`json^int2` = test_json_table.`json` ^ test_json_table.`int2`;
UPDATE test_json_type, test_json_table SET test_json_type.`json>>int2` = test_json_table.`json` >> test_json_table.`int2`;
UPDATE test_json_type, test_json_table SET test_json_type.`json<<int2` = test_json_table.`json` << test_json_table.`int2`;
UPDATE test_json_type, test_json_table SET test_json_type.`json|int2` = test_json_table.`json` | test_json_table.`int2`;
UPDATE test_json_type, test_json_table SET test_json_type.`json&int2` = test_json_table.`json` & test_json_table.`int2`;
UPDATE test_json_type, test_json_table SET test_json_type.`json||int2` = test_json_table.`json` || test_json_table.`int2`;
UPDATE test_json_type, test_json_table SET test_json_type.`json&&int2` = test_json_table.`json` && test_json_table.`int2`;
UPDATE test_json_type, test_json_table SET test_json_type.`json_div_int2` = test_json_table.`json` div test_json_table.`int2`;
UPDATE test_json_type, test_json_table SET test_json_type.`json_mod_int2` = test_json_table.`json` mod test_json_table.`int2`;
UPDATE test_json_type, test_json_table SET test_json_type.`json_xor_int2` = test_json_table.`json` xor test_json_table.`int2`;
UPDATE test_json_type, test_json_table SET test_json_type.`json_and_int2` = test_json_table.`json` and test_json_table.`int2`;
UPDATE test_json_type, test_json_table SET test_json_type.`json_or_int2` = test_json_table.`json` or test_json_table.`int2`;
UPDATE test_json_type, test_json_table SET test_json_type.`json+uint2` = test_json_table.`json` + test_json_table.`uint2`;
UPDATE test_json_type, test_json_table SET test_json_type.`json-uint2` = test_json_table.`json` - test_json_table.`uint2`;
UPDATE test_json_type, test_json_table SET test_json_type.`json*uint2` = test_json_table.`json` * test_json_table.`uint2`;
UPDATE test_json_type, test_json_table SET test_json_type.`json/uint2` = test_json_table.`json` / test_json_table.`uint2`;
UPDATE test_json_type, test_json_table SET test_json_type.`json%uint2` = test_json_table.`json` % test_json_table.`uint2`;
UPDATE test_json_type, test_json_table SET test_json_type.`json^uint2` = test_json_table.`json` ^ test_json_table.`uint2`;
UPDATE test_json_type, test_json_table SET test_json_type.`json>>uint2` = test_json_table.`json` >> test_json_table.`uint2`;
UPDATE test_json_type, test_json_table SET test_json_type.`json<<uint2` = test_json_table.`json` << test_json_table.`uint2`;
UPDATE test_json_type, test_json_table SET test_json_type.`json|uint2` = test_json_table.`json` | test_json_table.`uint2`;
UPDATE test_json_type, test_json_table SET test_json_type.`json&uint2` = test_json_table.`json` & test_json_table.`uint2`;
UPDATE test_json_type, test_json_table SET test_json_type.`json||uint2` = test_json_table.`json` || test_json_table.`uint2`;
UPDATE test_json_type, test_json_table SET test_json_type.`json&&uint2` = test_json_table.`json` && test_json_table.`uint2`;
UPDATE test_json_type, test_json_table SET test_json_type.`json_div_uint2` = test_json_table.`json` div test_json_table.`uint2`;
UPDATE test_json_type, test_json_table SET test_json_type.`json_mod_uint2` = test_json_table.`json` mod test_json_table.`uint2`;
UPDATE test_json_type, test_json_table SET test_json_type.`json_xor_uint2` = test_json_table.`json` xor test_json_table.`uint2`;
UPDATE test_json_type, test_json_table SET test_json_type.`json_and_uint2` = test_json_table.`json` and test_json_table.`uint2`;
UPDATE test_json_type, test_json_table SET test_json_type.`json_or_uint2` = test_json_table.`json` or test_json_table.`uint2`;
UPDATE test_json_type, test_json_table SET test_json_type.`json+int4` = test_json_table.`json` + test_json_table.`int4`;
UPDATE test_json_type, test_json_table SET test_json_type.`json-int4` = test_json_table.`json` - test_json_table.`int4`;
UPDATE test_json_type, test_json_table SET test_json_type.`json*int4` = test_json_table.`json` * test_json_table.`int4`;
UPDATE test_json_type, test_json_table SET test_json_type.`json/int4` = test_json_table.`json` / test_json_table.`int4`;
UPDATE test_json_type, test_json_table SET test_json_type.`json%int4` = test_json_table.`json` % test_json_table.`int4`;
UPDATE test_json_type, test_json_table SET test_json_type.`json^int4` = test_json_table.`json` ^ test_json_table.`int4`;
UPDATE test_json_type, test_json_table SET test_json_type.`json>>int4` = test_json_table.`json` >> test_json_table.`int4`;
UPDATE test_json_type, test_json_table SET test_json_type.`json<<int4` = test_json_table.`json` << test_json_table.`int4`;
UPDATE test_json_type, test_json_table SET test_json_type.`json|int4` = test_json_table.`json` | test_json_table.`int4`;
UPDATE test_json_type, test_json_table SET test_json_type.`json&int4` = test_json_table.`json` & test_json_table.`int4`;
UPDATE test_json_type, test_json_table SET test_json_type.`json||int4` = test_json_table.`json` || test_json_table.`int4`;
UPDATE test_json_type, test_json_table SET test_json_type.`json&&int4` = test_json_table.`json` && test_json_table.`int4`;
UPDATE test_json_type, test_json_table SET test_json_type.`json_div_int4` = test_json_table.`json` div test_json_table.`int4`;
UPDATE test_json_type, test_json_table SET test_json_type.`json_mod_int4` = test_json_table.`json` mod test_json_table.`int4`;
UPDATE test_json_type, test_json_table SET test_json_type.`json_xor_int4` = test_json_table.`json` xor test_json_table.`int4`;
UPDATE test_json_type, test_json_table SET test_json_type.`json_and_int4` = test_json_table.`json` and test_json_table.`int4`;
UPDATE test_json_type, test_json_table SET test_json_type.`json_or_int4` = test_json_table.`json` or test_json_table.`int4`;
UPDATE test_json_type, test_json_table SET test_json_type.`json+uint4` = test_json_table.`json` + test_json_table.`uint4`;
UPDATE test_json_type, test_json_table SET test_json_type.`json-uint4` = test_json_table.`json` - test_json_table.`uint4`;
UPDATE test_json_type, test_json_table SET test_json_type.`json*uint4` = test_json_table.`json` * test_json_table.`uint4`;
UPDATE test_json_type, test_json_table SET test_json_type.`json/uint4` = test_json_table.`json` / test_json_table.`uint4`;
UPDATE test_json_type, test_json_table SET test_json_type.`json%uint4` = test_json_table.`json` % test_json_table.`uint4`;
UPDATE test_json_type, test_json_table SET test_json_type.`json^uint4` = test_json_table.`json` ^ test_json_table.`uint4`;
UPDATE test_json_type, test_json_table SET test_json_type.`json>>uint4` = test_json_table.`json` >> test_json_table.`uint4`;
UPDATE test_json_type, test_json_table SET test_json_type.`json<<uint4` = test_json_table.`json` << test_json_table.`uint4`;
UPDATE test_json_type, test_json_table SET test_json_type.`json|uint4` = test_json_table.`json` | test_json_table.`uint4`;
UPDATE test_json_type, test_json_table SET test_json_type.`json&uint4` = test_json_table.`json` & test_json_table.`uint4`;
UPDATE test_json_type, test_json_table SET test_json_type.`json||uint4` = test_json_table.`json` || test_json_table.`uint4`;
UPDATE test_json_type, test_json_table SET test_json_type.`json&&uint4` = test_json_table.`json` && test_json_table.`uint4`;
UPDATE test_json_type, test_json_table SET test_json_type.`json_div_uint4` = test_json_table.`json` div test_json_table.`uint4`;
UPDATE test_json_type, test_json_table SET test_json_type.`json_mod_uint4` = test_json_table.`json` mod test_json_table.`uint4`;
UPDATE test_json_type, test_json_table SET test_json_type.`json_xor_uint4` = test_json_table.`json` xor test_json_table.`uint4`;
UPDATE test_json_type, test_json_table SET test_json_type.`json_and_uint4` = test_json_table.`json` and test_json_table.`uint4`;
UPDATE test_json_type, test_json_table SET test_json_type.`json_or_uint4` = test_json_table.`json` or test_json_table.`uint4`;
UPDATE test_json_type, test_json_table SET test_json_type.`json+int8` = test_json_table.`json` + test_json_table.`int8`;
UPDATE test_json_type, test_json_table SET test_json_type.`json-int8` = test_json_table.`json` - test_json_table.`int8`;
UPDATE test_json_type, test_json_table SET test_json_type.`json*int8` = test_json_table.`json` * test_json_table.`int8`;
UPDATE test_json_type, test_json_table SET test_json_type.`json/int8` = test_json_table.`json` / test_json_table.`int8`;
UPDATE test_json_type, test_json_table SET test_json_type.`json%int8` = test_json_table.`json` % test_json_table.`int8`;
UPDATE test_json_type, test_json_table SET test_json_type.`json^int8` = test_json_table.`json` ^ test_json_table.`int8`;
UPDATE test_json_type, test_json_table SET test_json_type.`json>>int8` = test_json_table.`json` >> test_json_table.`int8`;
UPDATE test_json_type, test_json_table SET test_json_type.`json<<int8` = test_json_table.`json` << test_json_table.`int8`;
UPDATE test_json_type, test_json_table SET test_json_type.`json|int8` = test_json_table.`json` | test_json_table.`int8`;
UPDATE test_json_type, test_json_table SET test_json_type.`json&int8` = test_json_table.`json` & test_json_table.`int8`;
UPDATE test_json_type, test_json_table SET test_json_type.`json||int8` = test_json_table.`json` || test_json_table.`int8`;
UPDATE test_json_type, test_json_table SET test_json_type.`json&&int8` = test_json_table.`json` && test_json_table.`int8`;
UPDATE test_json_type, test_json_table SET test_json_type.`json_div_int8` = test_json_table.`json` div test_json_table.`int8`;
UPDATE test_json_type, test_json_table SET test_json_type.`json_mod_int8` = test_json_table.`json` mod test_json_table.`int8`;
UPDATE test_json_type, test_json_table SET test_json_type.`json_xor_int8` = test_json_table.`json` xor test_json_table.`int8`;
UPDATE test_json_type, test_json_table SET test_json_type.`json_and_int8` = test_json_table.`json` and test_json_table.`int8`;
UPDATE test_json_type, test_json_table SET test_json_type.`json_or_int8` = test_json_table.`json` or test_json_table.`int8`;
UPDATE test_json_type, test_json_table SET test_json_type.`json+uint8` = test_json_table.`json` + test_json_table.`uint8`;
UPDATE test_json_type, test_json_table SET test_json_type.`json-uint8` = test_json_table.`json` - test_json_table.`uint8`;
UPDATE test_json_type, test_json_table SET test_json_type.`json*uint8` = test_json_table.`json` * test_json_table.`uint8`;
UPDATE test_json_type, test_json_table SET test_json_type.`json/uint8` = test_json_table.`json` / test_json_table.`uint8`;
UPDATE test_json_type, test_json_table SET test_json_type.`json%uint8` = test_json_table.`json` % test_json_table.`uint8`;
UPDATE test_json_type, test_json_table SET test_json_type.`json^uint8` = test_json_table.`json` ^ test_json_table.`uint8`;
UPDATE test_json_type, test_json_table SET test_json_type.`json>>uint8` = test_json_table.`json` >> test_json_table.`uint8`;
UPDATE test_json_type, test_json_table SET test_json_type.`json<<uint8` = test_json_table.`json` << test_json_table.`uint8`;
UPDATE test_json_type, test_json_table SET test_json_type.`json|uint8` = test_json_table.`json` | test_json_table.`uint8`;
UPDATE test_json_type, test_json_table SET test_json_type.`json&uint8` = test_json_table.`json` & test_json_table.`uint8`;
UPDATE test_json_type, test_json_table SET test_json_type.`json||uint8` = test_json_table.`json` || test_json_table.`uint8`;
UPDATE test_json_type, test_json_table SET test_json_type.`json&&uint8` = test_json_table.`json` && test_json_table.`uint8`;
UPDATE test_json_type, test_json_table SET test_json_type.`json_div_uint8` = test_json_table.`json` div test_json_table.`uint8`;
UPDATE test_json_type, test_json_table SET test_json_type.`json_mod_uint8` = test_json_table.`json` mod test_json_table.`uint8`;
UPDATE test_json_type, test_json_table SET test_json_type.`json_xor_uint8` = test_json_table.`json` xor test_json_table.`uint8`;
UPDATE test_json_type, test_json_table SET test_json_type.`json_and_uint8` = test_json_table.`json` and test_json_table.`uint8`;
UPDATE test_json_type, test_json_table SET test_json_type.`json_or_uint8` = test_json_table.`json` or test_json_table.`uint8`;
UPDATE test_json_type, test_json_table SET test_json_type.`json+float4` = test_json_table.`json` + test_json_table.`float4`;
UPDATE test_json_type, test_json_table SET test_json_type.`json-float4` = test_json_table.`json` - test_json_table.`float4`;
UPDATE test_json_type, test_json_table SET test_json_type.`json*float4` = test_json_table.`json` * test_json_table.`float4`;
UPDATE test_json_type, test_json_table SET test_json_type.`json/float4` = test_json_table.`json` / test_json_table.`float4`;
UPDATE test_json_type, test_json_table SET test_json_type.`json%float4` = test_json_table.`json` % test_json_table.`float4`;
UPDATE test_json_type, test_json_table SET test_json_type.`json^float4` = test_json_table.`json` ^ test_json_table.`float4`;
UPDATE test_json_type, test_json_table SET test_json_type.`json>>float4` = test_json_table.`json` >> test_json_table.`float4`;
UPDATE test_json_type, test_json_table SET test_json_type.`json<<float4` = test_json_table.`json` << test_json_table.`float4`;
UPDATE test_json_type, test_json_table SET test_json_type.`json|float4` = test_json_table.`json` | test_json_table.`float4`;
UPDATE test_json_type, test_json_table SET test_json_type.`json&float4` = test_json_table.`json` & test_json_table.`float4`;
UPDATE test_json_type, test_json_table SET test_json_type.`json||float4` = test_json_table.`json` || test_json_table.`float4`;
UPDATE test_json_type, test_json_table SET test_json_type.`json&&float4` = test_json_table.`json` && test_json_table.`float4`;
UPDATE test_json_type, test_json_table SET test_json_type.`json_div_float4` = test_json_table.`json` div test_json_table.`float4`;
UPDATE test_json_type, test_json_table SET test_json_type.`json_mod_float4` = test_json_table.`json` mod test_json_table.`float4`;
UPDATE test_json_type, test_json_table SET test_json_type.`json_xor_float4` = test_json_table.`json` xor test_json_table.`float4`;
UPDATE test_json_type, test_json_table SET test_json_type.`json_and_float4` = test_json_table.`json` and test_json_table.`float4`;
UPDATE test_json_type, test_json_table SET test_json_type.`json_or_float4` = test_json_table.`json` or test_json_table.`float4`;
UPDATE test_json_type, test_json_table SET test_json_type.`json+float8` = test_json_table.`json` + test_json_table.`float8`;
UPDATE test_json_type, test_json_table SET test_json_type.`json-float8` = test_json_table.`json` - test_json_table.`float8`;
UPDATE test_json_type, test_json_table SET test_json_type.`json*float8` = test_json_table.`json` * test_json_table.`float8`;
UPDATE test_json_type, test_json_table SET test_json_type.`json/float8` = test_json_table.`json` / test_json_table.`float8`;
UPDATE test_json_type, test_json_table SET test_json_type.`json%float8` = test_json_table.`json` % test_json_table.`float8`;
UPDATE test_json_type, test_json_table SET test_json_type.`json^float8` = test_json_table.`json` ^ test_json_table.`float8`;
UPDATE test_json_type, test_json_table SET test_json_type.`json>>float8` = test_json_table.`json` >> test_json_table.`float8`;
UPDATE test_json_type, test_json_table SET test_json_type.`json<<float8` = test_json_table.`json` << test_json_table.`float8`;
UPDATE test_json_type, test_json_table SET test_json_type.`json|float8` = test_json_table.`json` | test_json_table.`float8`;
UPDATE test_json_type, test_json_table SET test_json_type.`json&float8` = test_json_table.`json` & test_json_table.`float8`;
UPDATE test_json_type, test_json_table SET test_json_type.`json||float8` = test_json_table.`json` || test_json_table.`float8`;
UPDATE test_json_type, test_json_table SET test_json_type.`json&&float8` = test_json_table.`json` && test_json_table.`float8`;
UPDATE test_json_type, test_json_table SET test_json_type.`json_div_float8` = test_json_table.`json` div test_json_table.`float8`;
UPDATE test_json_type, test_json_table SET test_json_type.`json_mod_float8` = test_json_table.`json` mod test_json_table.`float8`;
UPDATE test_json_type, test_json_table SET test_json_type.`json_xor_float8` = test_json_table.`json` xor test_json_table.`float8`;
UPDATE test_json_type, test_json_table SET test_json_type.`json_and_float8` = test_json_table.`json` and test_json_table.`float8`;
UPDATE test_json_type, test_json_table SET test_json_type.`json_or_float8` = test_json_table.`json` or test_json_table.`float8`;
UPDATE test_json_type, test_json_table SET test_json_type.`json+numeric` = test_json_table.`json` + test_json_table.`numeric`;
UPDATE test_json_type, test_json_table SET test_json_type.`json-numeric` = test_json_table.`json` - test_json_table.`numeric`;
UPDATE test_json_type, test_json_table SET test_json_type.`json*numeric` = test_json_table.`json` * test_json_table.`numeric`;
UPDATE test_json_type, test_json_table SET test_json_type.`json/numeric` = test_json_table.`json` / test_json_table.`numeric`;
UPDATE test_json_type, test_json_table SET test_json_type.`json%numeric` = test_json_table.`json` % test_json_table.`numeric`;
UPDATE test_json_type, test_json_table SET test_json_type.`json^numeric` = test_json_table.`json` ^ test_json_table.`numeric`;
UPDATE test_json_type, test_json_table SET test_json_type.`json>>numeric` = test_json_table.`json` >> test_json_table.`numeric`;
UPDATE test_json_type, test_json_table SET test_json_type.`json<<numeric` = test_json_table.`json` << test_json_table.`numeric`;
UPDATE test_json_type, test_json_table SET test_json_type.`json|numeric` = test_json_table.`json` | test_json_table.`numeric`;
UPDATE test_json_type, test_json_table SET test_json_type.`json&numeric` = test_json_table.`json` & test_json_table.`numeric`;
UPDATE test_json_type, test_json_table SET test_json_type.`json||numeric` = test_json_table.`json` || test_json_table.`numeric`;
UPDATE test_json_type, test_json_table SET test_json_type.`json&&numeric` = test_json_table.`json` && test_json_table.`numeric`;
UPDATE test_json_type, test_json_table SET test_json_type.`json_div_numeric` = test_json_table.`json` div test_json_table.`numeric`;
UPDATE test_json_type, test_json_table SET test_json_type.`json_mod_numeric` = test_json_table.`json` mod test_json_table.`numeric`;
UPDATE test_json_type, test_json_table SET test_json_type.`json_xor_numeric` = test_json_table.`json` xor test_json_table.`numeric`;
UPDATE test_json_type, test_json_table SET test_json_type.`json_and_numeric` = test_json_table.`json` and test_json_table.`numeric`;
UPDATE test_json_type, test_json_table SET test_json_type.`json_or_numeric` = test_json_table.`json` or test_json_table.`numeric`;
UPDATE test_json_type, test_json_table SET test_json_type.`json+bit1` = test_json_table.`json` + test_json_table.`bit1`;
UPDATE test_json_type, test_json_table SET test_json_type.`json-bit1` = test_json_table.`json` - test_json_table.`bit1`;
UPDATE test_json_type, test_json_table SET test_json_type.`json*bit1` = test_json_table.`json` * test_json_table.`bit1`;
UPDATE test_json_type, test_json_table SET test_json_type.`json/bit1` = test_json_table.`json` / test_json_table.`bit1`;
UPDATE test_json_type, test_json_table SET test_json_type.`json%bit1` = test_json_table.`json` % test_json_table.`bit1`;
UPDATE test_json_type, test_json_table SET test_json_type.`json^bit1` = test_json_table.`json` ^ test_json_table.`bit1`;
UPDATE test_json_type, test_json_table SET test_json_type.`json>>bit1` = test_json_table.`json` >> test_json_table.`bit1`;
UPDATE test_json_type, test_json_table SET test_json_type.`json<<bit1` = test_json_table.`json` << test_json_table.`bit1`;
UPDATE test_json_type, test_json_table SET test_json_type.`json|bit1` = test_json_table.`json` | test_json_table.`bit1`;
UPDATE test_json_type, test_json_table SET test_json_type.`json&bit1` = test_json_table.`json` & test_json_table.`bit1`;
UPDATE test_json_type, test_json_table SET test_json_type.`json||bit1` = test_json_table.`json` || test_json_table.`bit1`;
UPDATE test_json_type, test_json_table SET test_json_type.`json&&bit1` = test_json_table.`json` && test_json_table.`bit1`;
UPDATE test_json_type, test_json_table SET test_json_type.`json_div_bit1` = test_json_table.`json` div test_json_table.`bit1`;
UPDATE test_json_type, test_json_table SET test_json_type.`json_mod_bit1` = test_json_table.`json` mod test_json_table.`bit1`;
UPDATE test_json_type, test_json_table SET test_json_type.`json_xor_bit1` = test_json_table.`json` xor test_json_table.`bit1`;
UPDATE test_json_type, test_json_table SET test_json_type.`json_and_bit1` = test_json_table.`json` and test_json_table.`bit1`;
UPDATE test_json_type, test_json_table SET test_json_type.`json_or_bit1` = test_json_table.`json` or test_json_table.`bit1`;
UPDATE test_json_type, test_json_table SET test_json_type.`json+bit64` = test_json_table.`json` + test_json_table.`bit64`;
UPDATE test_json_type, test_json_table SET test_json_type.`json-bit64` = test_json_table.`json` - test_json_table.`bit64`;
UPDATE test_json_type, test_json_table SET test_json_type.`json*bit64` = test_json_table.`json` * test_json_table.`bit64`;
UPDATE test_json_type, test_json_table SET test_json_type.`json/bit64` = test_json_table.`json` / test_json_table.`bit64`;
UPDATE test_json_type, test_json_table SET test_json_type.`json%bit64` = test_json_table.`json` % test_json_table.`bit64`;
UPDATE test_json_type, test_json_table SET test_json_type.`json^bit64` = test_json_table.`json` ^ test_json_table.`bit64`;
UPDATE test_json_type, test_json_table SET test_json_type.`json>>bit64` = test_json_table.`json` >> test_json_table.`bit64`;
UPDATE test_json_type, test_json_table SET test_json_type.`json<<bit64` = test_json_table.`json` << test_json_table.`bit64`;
UPDATE test_json_type, test_json_table SET test_json_type.`json|bit64` = test_json_table.`json` | test_json_table.`bit64`;
UPDATE test_json_type, test_json_table SET test_json_type.`json&bit64` = test_json_table.`json` & test_json_table.`bit64`;
UPDATE test_json_type, test_json_table SET test_json_type.`json||bit64` = test_json_table.`json` || test_json_table.`bit64`;
UPDATE test_json_type, test_json_table SET test_json_type.`json&&bit64` = test_json_table.`json` && test_json_table.`bit64`;
UPDATE test_json_type, test_json_table SET test_json_type.`json_div_bit64` = test_json_table.`json` div test_json_table.`bit64`;
UPDATE test_json_type, test_json_table SET test_json_type.`json_mod_bit64` = test_json_table.`json` mod test_json_table.`bit64`;
UPDATE test_json_type, test_json_table SET test_json_type.`json_xor_bit64` = test_json_table.`json` xor test_json_table.`bit64`;
UPDATE test_json_type, test_json_table SET test_json_type.`json_and_bit64` = test_json_table.`json` and test_json_table.`bit64`;
UPDATE test_json_type, test_json_table SET test_json_type.`json_or_bit64` = test_json_table.`json` or test_json_table.`bit64`;
UPDATE test_json_type, test_json_table SET test_json_type.`json+boolean` = test_json_table.`json` + test_json_table.`boolean`;
UPDATE test_json_type, test_json_table SET test_json_type.`json-boolean` = test_json_table.`json` - test_json_table.`boolean`;
UPDATE test_json_type, test_json_table SET test_json_type.`json*boolean` = test_json_table.`json` * test_json_table.`boolean`;
UPDATE test_json_type, test_json_table SET test_json_type.`json/boolean` = test_json_table.`json` / test_json_table.`boolean`;
UPDATE test_json_type, test_json_table SET test_json_type.`json%boolean` = test_json_table.`json` % test_json_table.`boolean`;
UPDATE test_json_type, test_json_table SET test_json_type.`json^boolean` = test_json_table.`json` ^ test_json_table.`boolean`;
UPDATE test_json_type, test_json_table SET test_json_type.`json>>boolean` = test_json_table.`json` >> test_json_table.`boolean`;
UPDATE test_json_type, test_json_table SET test_json_type.`json<<boolean` = test_json_table.`json` << test_json_table.`boolean`;
UPDATE test_json_type, test_json_table SET test_json_type.`json|boolean` = test_json_table.`json` | test_json_table.`boolean`;
UPDATE test_json_type, test_json_table SET test_json_type.`json&boolean` = test_json_table.`json` & test_json_table.`boolean`;
UPDATE test_json_type, test_json_table SET test_json_type.`json||boolean` = test_json_table.`json` || test_json_table.`boolean`;
UPDATE test_json_type, test_json_table SET test_json_type.`json&&boolean` = test_json_table.`json` && test_json_table.`boolean`;
UPDATE test_json_type, test_json_table SET test_json_type.`json_div_boolean` = test_json_table.`json` div test_json_table.`boolean`;
UPDATE test_json_type, test_json_table SET test_json_type.`json_mod_boolean` = test_json_table.`json` mod test_json_table.`boolean`;
-- UPDATE test_json_type, test_json_table SET test_json_type.`json_xor_boolean` = test_json_table.`json` xor test_json_table.`boolean`;
UPDATE test_json_type, test_json_table SET test_json_type.`json_and_boolean` = test_json_table.`json` and test_json_table.`boolean`;
UPDATE test_json_type, test_json_table SET test_json_type.`json_or_boolean` = test_json_table.`json` or test_json_table.`boolean`;
UPDATE test_json_type, test_json_table SET test_json_type.`json+date` = test_json_table.`json` + test_json_table.`date`;
UPDATE test_json_type, test_json_table SET test_json_type.`json-date` = test_json_table.`json` - test_json_table.`date`;
UPDATE test_json_type, test_json_table SET test_json_type.`json*date` = test_json_table.`json` * test_json_table.`date`;
UPDATE test_json_type, test_json_table SET test_json_type.`json/date` = test_json_table.`json` / test_json_table.`date`;
UPDATE test_json_type, test_json_table SET test_json_type.`json%date` = test_json_table.`json` % test_json_table.`date`;
UPDATE test_json_type, test_json_table SET test_json_type.`json^date` = test_json_table.`json` ^ test_json_table.`date`;
UPDATE test_json_type, test_json_table SET test_json_type.`json>>date` = test_json_table.`json` >> test_json_table.`date`;
UPDATE test_json_type, test_json_table SET test_json_type.`json<<date` = test_json_table.`json` << test_json_table.`date`;
UPDATE test_json_type, test_json_table SET test_json_type.`json|date` = test_json_table.`json` | test_json_table.`date`;
UPDATE test_json_type, test_json_table SET test_json_type.`json&date` = test_json_table.`json` & test_json_table.`date`;
UPDATE test_json_type, test_json_table SET test_json_type.`json||date` = test_json_table.`json` || test_json_table.`date`;
UPDATE test_json_type, test_json_table SET test_json_type.`json&&date` = test_json_table.`json` && test_json_table.`date`;
UPDATE test_json_type, test_json_table SET test_json_type.`json_div_date` = test_json_table.`json` div test_json_table.`date`;
UPDATE test_json_type, test_json_table SET test_json_type.`json_mod_date` = test_json_table.`json` mod test_json_table.`date`;
UPDATE test_json_type, test_json_table SET test_json_type.`json_xor_date` = test_json_table.`json` xor test_json_table.`date`;
UPDATE test_json_type, test_json_table SET test_json_type.`json_and_date` = test_json_table.`json` and test_json_table.`date`;
UPDATE test_json_type, test_json_table SET test_json_type.`json_or_date` = test_json_table.`json` or test_json_table.`date`;
UPDATE test_json_type, test_json_table SET test_json_type.`json+time` = test_json_table.`json` + test_json_table.`time`;
UPDATE test_json_type, test_json_table SET test_json_type.`json-time` = test_json_table.`json` - test_json_table.`time`;
UPDATE test_json_type, test_json_table SET test_json_type.`json*time` = test_json_table.`json` * test_json_table.`time`;
UPDATE test_json_type, test_json_table SET test_json_type.`json/time` = test_json_table.`json` / test_json_table.`time`;
UPDATE test_json_type, test_json_table SET test_json_type.`json%time` = test_json_table.`json` % test_json_table.`time`;
UPDATE test_json_type, test_json_table SET test_json_type.`json^time` = test_json_table.`json` ^ test_json_table.`time`;
UPDATE test_json_type, test_json_table SET test_json_type.`json>>time` = test_json_table.`json` >> test_json_table.`time`;
UPDATE test_json_type, test_json_table SET test_json_type.`json<<time` = test_json_table.`json` << test_json_table.`time`;
UPDATE test_json_type, test_json_table SET test_json_type.`json|time` = test_json_table.`json` | test_json_table.`time`;
UPDATE test_json_type, test_json_table SET test_json_type.`json&time` = test_json_table.`json` & test_json_table.`time`;
UPDATE test_json_type, test_json_table SET test_json_type.`json||time` = test_json_table.`json` || test_json_table.`time`;
UPDATE test_json_type, test_json_table SET test_json_type.`json&&time` = test_json_table.`json` && test_json_table.`time`;
UPDATE test_json_type, test_json_table SET test_json_type.`json_div_time` = test_json_table.`json` div test_json_table.`time`;
UPDATE test_json_type, test_json_table SET test_json_type.`json_mod_time` = test_json_table.`json` mod test_json_table.`time`;
UPDATE test_json_type, test_json_table SET test_json_type.`json_xor_time` = test_json_table.`json` xor test_json_table.`time`;
UPDATE test_json_type, test_json_table SET test_json_type.`json_and_time` = test_json_table.`json` and test_json_table.`time`;
UPDATE test_json_type, test_json_table SET test_json_type.`json_or_time` = test_json_table.`json` or test_json_table.`time`;
UPDATE test_json_type, test_json_table SET test_json_type.`json+time(4)` = test_json_table.`json` + test_json_table.`time(4)`;
UPDATE test_json_type, test_json_table SET test_json_type.`json-time(4)` = test_json_table.`json` - test_json_table.`time(4)`;
UPDATE test_json_type, test_json_table SET test_json_type.`json*time(4)` = test_json_table.`json` * test_json_table.`time(4)`;
UPDATE test_json_type, test_json_table SET test_json_type.`json/time(4)` = test_json_table.`json` / test_json_table.`time(4)`;
UPDATE test_json_type, test_json_table SET test_json_type.`json%time(4)` = test_json_table.`json` % test_json_table.`time(4)`;
UPDATE test_json_type, test_json_table SET test_json_type.`json^time(4)` = test_json_table.`json` ^ test_json_table.`time(4)`;
UPDATE test_json_type, test_json_table SET test_json_type.`json>>time(4)` = test_json_table.`json` >> test_json_table.`time(4)`;
UPDATE test_json_type, test_json_table SET test_json_type.`json<<time(4)` = test_json_table.`json` << test_json_table.`time(4)`;
UPDATE test_json_type, test_json_table SET test_json_type.`json|time(4)` = test_json_table.`json` | test_json_table.`time(4)`;
UPDATE test_json_type, test_json_table SET test_json_type.`json&time(4)` = test_json_table.`json` & test_json_table.`time(4)`;
UPDATE test_json_type, test_json_table SET test_json_type.`json||time(4)` = test_json_table.`json` || test_json_table.`time(4)`;
UPDATE test_json_type, test_json_table SET test_json_type.`json&&time(4)` = test_json_table.`json` && test_json_table.`time(4)`;
UPDATE test_json_type, test_json_table SET test_json_type.`json_div_time(4)` = test_json_table.`json` div test_json_table.`time(4)`;
UPDATE test_json_type, test_json_table SET test_json_type.`json_mod_time(4)` = test_json_table.`json` mod test_json_table.`time(4)`;
UPDATE test_json_type, test_json_table SET test_json_type.`json_xor_time(4)` = test_json_table.`json` xor test_json_table.`time(4)`;
UPDATE test_json_type, test_json_table SET test_json_type.`json_and_time(4)` = test_json_table.`json` and test_json_table.`time(4)`;
UPDATE test_json_type, test_json_table SET test_json_type.`json_or_time(4)` = test_json_table.`json` or test_json_table.`time(4)`;
UPDATE test_json_type, test_json_table SET test_json_type.`json+datetime` = test_json_table.`json` + test_json_table.`datetime`;
UPDATE test_json_type, test_json_table SET test_json_type.`json-datetime` = test_json_table.`json` - test_json_table.`datetime`;
UPDATE test_json_type, test_json_table SET test_json_type.`json*datetime` = test_json_table.`json` * test_json_table.`datetime`;
UPDATE test_json_type, test_json_table SET test_json_type.`json/datetime` = test_json_table.`json` / test_json_table.`datetime`;
UPDATE test_json_type, test_json_table SET test_json_type.`json%datetime` = test_json_table.`json` % test_json_table.`datetime`;
UPDATE test_json_type, test_json_table SET test_json_type.`json^datetime` = test_json_table.`json` ^ test_json_table.`datetime`;
UPDATE test_json_type, test_json_table SET test_json_type.`json>>datetime` = test_json_table.`json` >> test_json_table.`datetime`;
UPDATE test_json_type, test_json_table SET test_json_type.`json<<datetime` = test_json_table.`json` << test_json_table.`datetime`;
UPDATE test_json_type, test_json_table SET test_json_type.`json|datetime` = test_json_table.`json` | test_json_table.`datetime`;
UPDATE test_json_type, test_json_table SET test_json_type.`json&datetime` = test_json_table.`json` & test_json_table.`datetime`;
UPDATE test_json_type, test_json_table SET test_json_type.`json||datetime` = test_json_table.`json` || test_json_table.`datetime`;
UPDATE test_json_type, test_json_table SET test_json_type.`json&&datetime` = test_json_table.`json` && test_json_table.`datetime`;
UPDATE test_json_type, test_json_table SET test_json_type.`json_div_datetime` = test_json_table.`json` div test_json_table.`datetime`;
UPDATE test_json_type, test_json_table SET test_json_type.`json_mod_datetime` = test_json_table.`json` mod test_json_table.`datetime`;
UPDATE test_json_type, test_json_table SET test_json_type.`json_xor_datetime` = test_json_table.`json` xor test_json_table.`datetime`;
UPDATE test_json_type, test_json_table SET test_json_type.`json_and_datetime` = test_json_table.`json` and test_json_table.`datetime`;
UPDATE test_json_type, test_json_table SET test_json_type.`json_or_datetime` = test_json_table.`json` or test_json_table.`datetime`;
UPDATE test_json_type, test_json_table SET test_json_type.`json+datetime(4)` = test_json_table.`json` + test_json_table.`datetime(4)`;
UPDATE test_json_type, test_json_table SET test_json_type.`json-datetime(4)` = test_json_table.`json` - test_json_table.`datetime(4)`;
UPDATE test_json_type, test_json_table SET test_json_type.`json*datetime(4)` = test_json_table.`json` * test_json_table.`datetime(4)`;
UPDATE test_json_type, test_json_table SET test_json_type.`json/datetime(4)` = test_json_table.`json` / test_json_table.`datetime(4)`;
UPDATE test_json_type, test_json_table SET test_json_type.`json%datetime(4)` = test_json_table.`json` % test_json_table.`datetime(4)`;
UPDATE test_json_type, test_json_table SET test_json_type.`json^datetime(4)` = test_json_table.`json` ^ test_json_table.`datetime(4)`;
UPDATE test_json_type, test_json_table SET test_json_type.`json>>datetime(4)` = test_json_table.`json` >> test_json_table.`datetime(4)`;
UPDATE test_json_type, test_json_table SET test_json_type.`json<<datetime(4)` = test_json_table.`json` << test_json_table.`datetime(4)`;
UPDATE test_json_type, test_json_table SET test_json_type.`json|datetime(4)` = test_json_table.`json` | test_json_table.`datetime(4)`;
UPDATE test_json_type, test_json_table SET test_json_type.`json&datetime(4)` = test_json_table.`json` & test_json_table.`datetime(4)`;
UPDATE test_json_type, test_json_table SET test_json_type.`json||datetime(4)` = test_json_table.`json` || test_json_table.`datetime(4)`;
UPDATE test_json_type, test_json_table SET test_json_type.`json&&datetime(4)` = test_json_table.`json` && test_json_table.`datetime(4)`;
UPDATE test_json_type, test_json_table SET test_json_type.`json_div_datetime(4)` = test_json_table.`json` div test_json_table.`datetime(4)`;
UPDATE test_json_type, test_json_table SET test_json_type.`json_mod_datetime(4)` = test_json_table.`json` mod test_json_table.`datetime(4)`;
UPDATE test_json_type, test_json_table SET test_json_type.`json_xor_datetime(4)` = test_json_table.`json` xor test_json_table.`datetime(4)`;
UPDATE test_json_type, test_json_table SET test_json_type.`json_and_datetime(4)` = test_json_table.`json` and test_json_table.`datetime(4)`;
UPDATE test_json_type, test_json_table SET test_json_type.`json_or_datetime(4)` = test_json_table.`json` or test_json_table.`datetime(4)`;
UPDATE test_json_type, test_json_table SET test_json_type.`json+timestamp` = test_json_table.`json` + test_json_table.`timestamp`;
UPDATE test_json_type, test_json_table SET test_json_type.`json-timestamp` = test_json_table.`json` - test_json_table.`timestamp`;
UPDATE test_json_type, test_json_table SET test_json_type.`json*timestamp` = test_json_table.`json` * test_json_table.`timestamp`;
UPDATE test_json_type, test_json_table SET test_json_type.`json/timestamp` = test_json_table.`json` / test_json_table.`timestamp`;
UPDATE test_json_type, test_json_table SET test_json_type.`json%timestamp` = test_json_table.`json` % test_json_table.`timestamp`;
UPDATE test_json_type, test_json_table SET test_json_type.`json^timestamp` = test_json_table.`json` ^ test_json_table.`timestamp`;
UPDATE test_json_type, test_json_table SET test_json_type.`json>>timestamp` = test_json_table.`json` >> test_json_table.`timestamp`;
UPDATE test_json_type, test_json_table SET test_json_type.`json<<timestamp` = test_json_table.`json` << test_json_table.`timestamp`;
UPDATE test_json_type, test_json_table SET test_json_type.`json|timestamp` = test_json_table.`json` | test_json_table.`timestamp`;
UPDATE test_json_type, test_json_table SET test_json_type.`json&timestamp` = test_json_table.`json` & test_json_table.`timestamp`;
UPDATE test_json_type, test_json_table SET test_json_type.`json||timestamp` = test_json_table.`json` || test_json_table.`timestamp`;
UPDATE test_json_type, test_json_table SET test_json_type.`json&&timestamp` = test_json_table.`json` && test_json_table.`timestamp`;
UPDATE test_json_type, test_json_table SET test_json_type.`json_div_timestamp` = test_json_table.`json` div test_json_table.`timestamp`;
UPDATE test_json_type, test_json_table SET test_json_type.`json_mod_timestamp` = test_json_table.`json` mod test_json_table.`timestamp`;
UPDATE test_json_type, test_json_table SET test_json_type.`json_xor_timestamp` = test_json_table.`json` xor test_json_table.`timestamp`;
UPDATE test_json_type, test_json_table SET test_json_type.`json_and_timestamp` = test_json_table.`json` and test_json_table.`timestamp`;
UPDATE test_json_type, test_json_table SET test_json_type.`json_or_timestamp` = test_json_table.`json` or test_json_table.`timestamp`;
UPDATE test_json_type, test_json_table SET test_json_type.`json+timestamp(4)` = test_json_table.`json` + test_json_table.`timestamp(4)`;
UPDATE test_json_type, test_json_table SET test_json_type.`json-timestamp(4)` = test_json_table.`json` - test_json_table.`timestamp(4)`;
UPDATE test_json_type, test_json_table SET test_json_type.`json*timestamp(4)` = test_json_table.`json` * test_json_table.`timestamp(4)`;
UPDATE test_json_type, test_json_table SET test_json_type.`json/timestamp(4)` = test_json_table.`json` / test_json_table.`timestamp(4)`;
UPDATE test_json_type, test_json_table SET test_json_type.`json%timestamp(4)` = test_json_table.`json` % test_json_table.`timestamp(4)`;
UPDATE test_json_type, test_json_table SET test_json_type.`json^timestamp(4)` = test_json_table.`json` ^ test_json_table.`timestamp(4)`;
UPDATE test_json_type, test_json_table SET test_json_type.`json>>timestamp(4)` = test_json_table.`json` >> test_json_table.`timestamp(4)`;
UPDATE test_json_type, test_json_table SET test_json_type.`json<<timestamp(4)` = test_json_table.`json` << test_json_table.`timestamp(4)`;
UPDATE test_json_type, test_json_table SET test_json_type.`json|timestamp(4)` = test_json_table.`json` | test_json_table.`timestamp(4)`;
UPDATE test_json_type, test_json_table SET test_json_type.`json&timestamp(4)` = test_json_table.`json` & test_json_table.`timestamp(4)`;
UPDATE test_json_type, test_json_table SET test_json_type.`json||timestamp(4)` = test_json_table.`json` || test_json_table.`timestamp(4)`;
UPDATE test_json_type, test_json_table SET test_json_type.`json&&timestamp(4)` = test_json_table.`json` && test_json_table.`timestamp(4)`;
UPDATE test_json_type, test_json_table SET test_json_type.`json_div_timestamp(4)` = test_json_table.`json` div test_json_table.`timestamp(4)`;
UPDATE test_json_type, test_json_table SET test_json_type.`json_mod_timestamp(4)` = test_json_table.`json` mod test_json_table.`timestamp(4)`;
UPDATE test_json_type, test_json_table SET test_json_type.`json_xor_timestamp(4)` = test_json_table.`json` xor test_json_table.`timestamp(4)`;
UPDATE test_json_type, test_json_table SET test_json_type.`json_and_timestamp(4)` = test_json_table.`json` and test_json_table.`timestamp(4)`;
UPDATE test_json_type, test_json_table SET test_json_type.`json_or_timestamp(4)` = test_json_table.`json` or test_json_table.`timestamp(4)`;
UPDATE test_json_type, test_json_table SET test_json_type.`json+year` = test_json_table.`json` + test_json_table.`year`;
UPDATE test_json_type, test_json_table SET test_json_type.`json-year` = test_json_table.`json` - test_json_table.`year`;
UPDATE test_json_type, test_json_table SET test_json_type.`json*year` = test_json_table.`json` * test_json_table.`year`;
UPDATE test_json_type, test_json_table SET test_json_type.`json/year` = test_json_table.`json` / test_json_table.`year`;
UPDATE test_json_type, test_json_table SET test_json_type.`json%year` = test_json_table.`json` % test_json_table.`year`;
UPDATE test_json_type, test_json_table SET test_json_type.`json^year` = test_json_table.`json` ^ test_json_table.`year`;
UPDATE test_json_type, test_json_table SET test_json_type.`json>>year` = test_json_table.`json` >> test_json_table.`year`;
UPDATE test_json_type, test_json_table SET test_json_type.`json<<year` = test_json_table.`json` << test_json_table.`year`;
UPDATE test_json_type, test_json_table SET test_json_type.`json|year` = test_json_table.`json` | test_json_table.`year`;
UPDATE test_json_type, test_json_table SET test_json_type.`json&year` = test_json_table.`json` & test_json_table.`year`;
UPDATE test_json_type, test_json_table SET test_json_type.`json||year` = test_json_table.`json` || test_json_table.`year`;
UPDATE test_json_type, test_json_table SET test_json_type.`json&&year` = test_json_table.`json` && test_json_table.`year`;
UPDATE test_json_type, test_json_table SET test_json_type.`json_div_year` = test_json_table.`json` div test_json_table.`year`;
UPDATE test_json_type, test_json_table SET test_json_type.`json_mod_year` = test_json_table.`json` mod test_json_table.`year`;
UPDATE test_json_type, test_json_table SET test_json_type.`json_xor_year` = test_json_table.`json` xor test_json_table.`year`;
UPDATE test_json_type, test_json_table SET test_json_type.`json_and_year` = test_json_table.`json` and test_json_table.`year`;
UPDATE test_json_type, test_json_table SET test_json_type.`json_or_year` = test_json_table.`json` or test_json_table.`year`;
UPDATE test_json_type, test_json_table SET test_json_type.`json+char` = test_json_table.`json` + test_json_table.`char`;
UPDATE test_json_type, test_json_table SET test_json_type.`json-char` = test_json_table.`json` - test_json_table.`char`;
UPDATE test_json_type, test_json_table SET test_json_type.`json*char` = test_json_table.`json` * test_json_table.`char`;
UPDATE test_json_type, test_json_table SET test_json_type.`json/char` = test_json_table.`json` / test_json_table.`char`;
UPDATE test_json_type, test_json_table SET test_json_type.`json%char` = test_json_table.`json` % test_json_table.`char`;
UPDATE test_json_type, test_json_table SET test_json_type.`json^char` = test_json_table.`json` ^ test_json_table.`char`;
UPDATE test_json_type, test_json_table SET test_json_type.`json>>char` = test_json_table.`json` >> test_json_table.`char`;
UPDATE test_json_type, test_json_table SET test_json_type.`json<<char` = test_json_table.`json` << test_json_table.`char`;
UPDATE test_json_type, test_json_table SET test_json_type.`json|char` = test_json_table.`json` | test_json_table.`char`;
UPDATE test_json_type, test_json_table SET test_json_type.`json&char` = test_json_table.`json` & test_json_table.`char`;
UPDATE test_json_type, test_json_table SET test_json_type.`json||char` = test_json_table.`json` || test_json_table.`char`;
UPDATE test_json_type, test_json_table SET test_json_type.`json&&char` = test_json_table.`json` && test_json_table.`char`;
UPDATE test_json_type, test_json_table SET test_json_type.`json_div_char` = test_json_table.`json` div test_json_table.`char`;
UPDATE test_json_type, test_json_table SET test_json_type.`json_mod_char` = test_json_table.`json` mod test_json_table.`char`;
UPDATE test_json_type, test_json_table SET test_json_type.`json_xor_char` = test_json_table.`json` xor test_json_table.`char`;
UPDATE test_json_type, test_json_table SET test_json_type.`json_and_char` = test_json_table.`json` and test_json_table.`char`;
UPDATE test_json_type, test_json_table SET test_json_type.`json_or_char` = test_json_table.`json` or test_json_table.`char`;
UPDATE test_json_type, test_json_table SET test_json_type.`json+varchar` = test_json_table.`json` + test_json_table.`varchar`;
UPDATE test_json_type, test_json_table SET test_json_type.`json-varchar` = test_json_table.`json` - test_json_table.`varchar`;
UPDATE test_json_type, test_json_table SET test_json_type.`json*varchar` = test_json_table.`json` * test_json_table.`varchar`;
UPDATE test_json_type, test_json_table SET test_json_type.`json/varchar` = test_json_table.`json` / test_json_table.`varchar`;
UPDATE test_json_type, test_json_table SET test_json_type.`json%varchar` = test_json_table.`json` % test_json_table.`varchar`;
UPDATE test_json_type, test_json_table SET test_json_type.`json^varchar` = test_json_table.`json` ^ test_json_table.`varchar`;
UPDATE test_json_type, test_json_table SET test_json_type.`json>>varchar` = test_json_table.`json` >> test_json_table.`varchar`;
UPDATE test_json_type, test_json_table SET test_json_type.`json<<varchar` = test_json_table.`json` << test_json_table.`varchar`;
UPDATE test_json_type, test_json_table SET test_json_type.`json|varchar` = test_json_table.`json` | test_json_table.`varchar`;
UPDATE test_json_type, test_json_table SET test_json_type.`json&varchar` = test_json_table.`json` & test_json_table.`varchar`;
UPDATE test_json_type, test_json_table SET test_json_type.`json||varchar` = test_json_table.`json` || test_json_table.`varchar`;
UPDATE test_json_type, test_json_table SET test_json_type.`json&&varchar` = test_json_table.`json` && test_json_table.`varchar`;
UPDATE test_json_type, test_json_table SET test_json_type.`json_div_varchar` = test_json_table.`json` div test_json_table.`varchar`;
UPDATE test_json_type, test_json_table SET test_json_type.`json_mod_varchar` = test_json_table.`json` mod test_json_table.`varchar`;
UPDATE test_json_type, test_json_table SET test_json_type.`json_xor_varchar` = test_json_table.`json` xor test_json_table.`varchar`;
UPDATE test_json_type, test_json_table SET test_json_type.`json_and_varchar` = test_json_table.`json` and test_json_table.`varchar`;
UPDATE test_json_type, test_json_table SET test_json_type.`json_or_varchar` = test_json_table.`json` or test_json_table.`varchar`;
UPDATE test_json_type, test_json_table SET test_json_type.`json+binary` = test_json_table.`json` + test_json_table.`binary`;
UPDATE test_json_type, test_json_table SET test_json_type.`json-binary` = test_json_table.`json` - test_json_table.`binary`;
UPDATE test_json_type, test_json_table SET test_json_type.`json*binary` = test_json_table.`json` * test_json_table.`binary`;
UPDATE test_json_type, test_json_table SET test_json_type.`json/binary` = test_json_table.`json` / test_json_table.`binary`;
UPDATE test_json_type, test_json_table SET test_json_type.`json%binary` = test_json_table.`json` % test_json_table.`binary`;
UPDATE test_json_type, test_json_table SET test_json_type.`json^binary` = test_json_table.`json` ^ test_json_table.`binary`;
UPDATE test_json_type, test_json_table SET test_json_type.`json>>binary` = test_json_table.`json` >> test_json_table.`binary`;
UPDATE test_json_type, test_json_table SET test_json_type.`json<<binary` = test_json_table.`json` << test_json_table.`binary`;
UPDATE test_json_type, test_json_table SET test_json_type.`json|binary` = test_json_table.`json` | test_json_table.`binary`;
UPDATE test_json_type, test_json_table SET test_json_type.`json&binary` = test_json_table.`json` & test_json_table.`binary`;
UPDATE test_json_type, test_json_table SET test_json_type.`json||binary` = test_json_table.`json` || test_json_table.`binary`;
UPDATE test_json_type, test_json_table SET test_json_type.`json&&binary` = test_json_table.`json` && test_json_table.`binary`;
UPDATE test_json_type, test_json_table SET test_json_type.`json_div_binary` = test_json_table.`json` div test_json_table.`binary`;
UPDATE test_json_type, test_json_table SET test_json_type.`json_mod_binary` = test_json_table.`json` mod test_json_table.`binary`;
UPDATE test_json_type, test_json_table SET test_json_type.`json_xor_binary` = test_json_table.`json` xor test_json_table.`binary`;
UPDATE test_json_type, test_json_table SET test_json_type.`json_and_binary` = test_json_table.`json` and test_json_table.`binary`;
UPDATE test_json_type, test_json_table SET test_json_type.`json_or_binary` = test_json_table.`json` or test_json_table.`binary`;
UPDATE test_json_type, test_json_table SET test_json_type.`json+varbinary` = test_json_table.`json` + test_json_table.`varbinary`;
UPDATE test_json_type, test_json_table SET test_json_type.`json-varbinary` = test_json_table.`json` - test_json_table.`varbinary`;
UPDATE test_json_type, test_json_table SET test_json_type.`json*varbinary` = test_json_table.`json` * test_json_table.`varbinary`;
UPDATE test_json_type, test_json_table SET test_json_type.`json/varbinary` = test_json_table.`json` / test_json_table.`varbinary`;
UPDATE test_json_type, test_json_table SET test_json_type.`json%varbinary` = test_json_table.`json` % test_json_table.`varbinary`;
UPDATE test_json_type, test_json_table SET test_json_type.`json^varbinary` = test_json_table.`json` ^ test_json_table.`varbinary`;
UPDATE test_json_type, test_json_table SET test_json_type.`json>>varbinary` = test_json_table.`json` >> test_json_table.`varbinary`;
UPDATE test_json_type, test_json_table SET test_json_type.`json<<varbinary` = test_json_table.`json` << test_json_table.`varbinary`;
UPDATE test_json_type, test_json_table SET test_json_type.`json|varbinary` = test_json_table.`json` | test_json_table.`varbinary`;
UPDATE test_json_type, test_json_table SET test_json_type.`json&varbinary` = test_json_table.`json` & test_json_table.`varbinary`;
UPDATE test_json_type, test_json_table SET test_json_type.`json||varbinary` = test_json_table.`json` || test_json_table.`varbinary`;
UPDATE test_json_type, test_json_table SET test_json_type.`json&&varbinary` = test_json_table.`json` && test_json_table.`varbinary`;
UPDATE test_json_type, test_json_table SET test_json_type.`json_div_varbinary` = test_json_table.`json` div test_json_table.`varbinary`;
UPDATE test_json_type, test_json_table SET test_json_type.`json_mod_varbinary` = test_json_table.`json` mod test_json_table.`varbinary`;
UPDATE test_json_type, test_json_table SET test_json_type.`json_xor_varbinary` = test_json_table.`json` xor test_json_table.`varbinary`;
UPDATE test_json_type, test_json_table SET test_json_type.`json_and_varbinary` = test_json_table.`json` and test_json_table.`varbinary`;
UPDATE test_json_type, test_json_table SET test_json_type.`json_or_varbinary` = test_json_table.`json` or test_json_table.`varbinary`;
UPDATE test_json_type, test_json_table SET test_json_type.`json+tinyblob` = test_json_table.`json` + test_json_table.`tinyblob`;
UPDATE test_json_type, test_json_table SET test_json_type.`json-tinyblob` = test_json_table.`json` - test_json_table.`tinyblob`;
UPDATE test_json_type, test_json_table SET test_json_type.`json*tinyblob` = test_json_table.`json` * test_json_table.`tinyblob`;
UPDATE test_json_type, test_json_table SET test_json_type.`json/tinyblob` = test_json_table.`json` / test_json_table.`tinyblob`;
UPDATE test_json_type, test_json_table SET test_json_type.`json%tinyblob` = test_json_table.`json` % test_json_table.`tinyblob`;
UPDATE test_json_type, test_json_table SET test_json_type.`json^tinyblob` = test_json_table.`json` ^ test_json_table.`tinyblob`;
UPDATE test_json_type, test_json_table SET test_json_type.`json>>tinyblob` = test_json_table.`json` >> test_json_table.`tinyblob`;
UPDATE test_json_type, test_json_table SET test_json_type.`json<<tinyblob` = test_json_table.`json` << test_json_table.`tinyblob`;
UPDATE test_json_type, test_json_table SET test_json_type.`json|tinyblob` = test_json_table.`json` | test_json_table.`tinyblob`;
UPDATE test_json_type, test_json_table SET test_json_type.`json&tinyblob` = test_json_table.`json` & test_json_table.`tinyblob`;
UPDATE test_json_type, test_json_table SET test_json_type.`json||tinyblob` = test_json_table.`json` || test_json_table.`tinyblob`;
UPDATE test_json_type, test_json_table SET test_json_type.`json&&tinyblob` = test_json_table.`json` && test_json_table.`tinyblob`;
UPDATE test_json_type, test_json_table SET test_json_type.`json_div_tinyblob` = test_json_table.`json` div test_json_table.`tinyblob`;
UPDATE test_json_type, test_json_table SET test_json_type.`json_mod_tinyblob` = test_json_table.`json` mod test_json_table.`tinyblob`;
UPDATE test_json_type, test_json_table SET test_json_type.`json_xor_tinyblob` = test_json_table.`json` xor test_json_table.`tinyblob`;
UPDATE test_json_type, test_json_table SET test_json_type.`json_and_tinyblob` = test_json_table.`json` and test_json_table.`tinyblob`;
UPDATE test_json_type, test_json_table SET test_json_type.`json_or_tinyblob` = test_json_table.`json` or test_json_table.`tinyblob`;
UPDATE test_json_type, test_json_table SET test_json_type.`json+blob` = test_json_table.`json` + test_json_table.`blob`;
UPDATE test_json_type, test_json_table SET test_json_type.`json-blob` = test_json_table.`json` - test_json_table.`blob`;
UPDATE test_json_type, test_json_table SET test_json_type.`json*blob` = test_json_table.`json` * test_json_table.`blob`;
UPDATE test_json_type, test_json_table SET test_json_type.`json/blob` = test_json_table.`json` / test_json_table.`blob`;
UPDATE test_json_type, test_json_table SET test_json_type.`json%blob` = test_json_table.`json` % test_json_table.`blob`;
UPDATE test_json_type, test_json_table SET test_json_type.`json^blob` = test_json_table.`json` ^ test_json_table.`blob`;
UPDATE test_json_type, test_json_table SET test_json_type.`json>>blob` = test_json_table.`json` >> test_json_table.`blob`;
UPDATE test_json_type, test_json_table SET test_json_type.`json<<blob` = test_json_table.`json` << test_json_table.`blob`;
UPDATE test_json_type, test_json_table SET test_json_type.`json|blob` = test_json_table.`json` | test_json_table.`blob`;
UPDATE test_json_type, test_json_table SET test_json_type.`json&blob` = test_json_table.`json` & test_json_table.`blob`;
UPDATE test_json_type, test_json_table SET test_json_type.`json||blob` = test_json_table.`json` || test_json_table.`blob`;
UPDATE test_json_type, test_json_table SET test_json_type.`json&&blob` = test_json_table.`json` && test_json_table.`blob`;
UPDATE test_json_type, test_json_table SET test_json_type.`json_div_blob` = test_json_table.`json` div test_json_table.`blob`;
UPDATE test_json_type, test_json_table SET test_json_type.`json_mod_blob` = test_json_table.`json` mod test_json_table.`blob`;
UPDATE test_json_type, test_json_table SET test_json_type.`json_xor_blob` = test_json_table.`json` xor test_json_table.`blob`;
UPDATE test_json_type, test_json_table SET test_json_type.`json_and_blob` = test_json_table.`json` and test_json_table.`blob`;
UPDATE test_json_type, test_json_table SET test_json_type.`json_or_blob` = test_json_table.`json` or test_json_table.`blob`;
UPDATE test_json_type, test_json_table SET test_json_type.`json+mediumblob` = test_json_table.`json` + test_json_table.`mediumblob`;
UPDATE test_json_type, test_json_table SET test_json_type.`json-mediumblob` = test_json_table.`json` - test_json_table.`mediumblob`;
UPDATE test_json_type, test_json_table SET test_json_type.`json*mediumblob` = test_json_table.`json` * test_json_table.`mediumblob`;
UPDATE test_json_type, test_json_table SET test_json_type.`json/mediumblob` = test_json_table.`json` / test_json_table.`mediumblob`;
UPDATE test_json_type, test_json_table SET test_json_type.`json%mediumblob` = test_json_table.`json` % test_json_table.`mediumblob`;
UPDATE test_json_type, test_json_table SET test_json_type.`json^mediumblob` = test_json_table.`json` ^ test_json_table.`mediumblob`;
UPDATE test_json_type, test_json_table SET test_json_type.`json>>mediumblob` = test_json_table.`json` >> test_json_table.`mediumblob`;
UPDATE test_json_type, test_json_table SET test_json_type.`json<<mediumblob` = test_json_table.`json` << test_json_table.`mediumblob`;
UPDATE test_json_type, test_json_table SET test_json_type.`json|mediumblob` = test_json_table.`json` | test_json_table.`mediumblob`;
UPDATE test_json_type, test_json_table SET test_json_type.`json&mediumblob` = test_json_table.`json` & test_json_table.`mediumblob`;
UPDATE test_json_type, test_json_table SET test_json_type.`json||mediumblob` = test_json_table.`json` || test_json_table.`mediumblob`;
UPDATE test_json_type, test_json_table SET test_json_type.`json&&mediumblob` = test_json_table.`json` && test_json_table.`mediumblob`;
UPDATE test_json_type, test_json_table SET test_json_type.`json_div_mediumblob` = test_json_table.`json` div test_json_table.`mediumblob`;
UPDATE test_json_type, test_json_table SET test_json_type.`json_mod_mediumblob` = test_json_table.`json` mod test_json_table.`mediumblob`;
UPDATE test_json_type, test_json_table SET test_json_type.`json_xor_mediumblob` = test_json_table.`json` xor test_json_table.`mediumblob`;
UPDATE test_json_type, test_json_table SET test_json_type.`json_and_mediumblob` = test_json_table.`json` and test_json_table.`mediumblob`;
UPDATE test_json_type, test_json_table SET test_json_type.`json_or_mediumblob` = test_json_table.`json` or test_json_table.`mediumblob`;
UPDATE test_json_type, test_json_table SET test_json_type.`json+longblob` = test_json_table.`json` + test_json_table.`longblob`;
UPDATE test_json_type, test_json_table SET test_json_type.`json-longblob` = test_json_table.`json` - test_json_table.`longblob`;
UPDATE test_json_type, test_json_table SET test_json_type.`json*longblob` = test_json_table.`json` * test_json_table.`longblob`;
UPDATE test_json_type, test_json_table SET test_json_type.`json/longblob` = test_json_table.`json` / test_json_table.`longblob`;
UPDATE test_json_type, test_json_table SET test_json_type.`json%longblob` = test_json_table.`json` % test_json_table.`longblob`;
UPDATE test_json_type, test_json_table SET test_json_type.`json^longblob` = test_json_table.`json` ^ test_json_table.`longblob`;
UPDATE test_json_type, test_json_table SET test_json_type.`json>>longblob` = test_json_table.`json` >> test_json_table.`longblob`;
UPDATE test_json_type, test_json_table SET test_json_type.`json<<longblob` = test_json_table.`json` << test_json_table.`longblob`;
UPDATE test_json_type, test_json_table SET test_json_type.`json|longblob` = test_json_table.`json` | test_json_table.`longblob`;
UPDATE test_json_type, test_json_table SET test_json_type.`json&longblob` = test_json_table.`json` & test_json_table.`longblob`;
UPDATE test_json_type, test_json_table SET test_json_type.`json||longblob` = test_json_table.`json` || test_json_table.`longblob`;
UPDATE test_json_type, test_json_table SET test_json_type.`json&&longblob` = test_json_table.`json` && test_json_table.`longblob`;
UPDATE test_json_type, test_json_table SET test_json_type.`json_div_longblob` = test_json_table.`json` div test_json_table.`longblob`;
UPDATE test_json_type, test_json_table SET test_json_type.`json_mod_longblob` = test_json_table.`json` mod test_json_table.`longblob`;
UPDATE test_json_type, test_json_table SET test_json_type.`json_xor_longblob` = test_json_table.`json` xor test_json_table.`longblob`;
UPDATE test_json_type, test_json_table SET test_json_type.`json_and_longblob` = test_json_table.`json` and test_json_table.`longblob`;
UPDATE test_json_type, test_json_table SET test_json_type.`json_or_longblob` = test_json_table.`json` or test_json_table.`longblob`;
UPDATE test_json_type, test_json_table SET test_json_type.`json+text` = test_json_table.`json` + test_json_table.`text`;
UPDATE test_json_type, test_json_table SET test_json_type.`json-text` = test_json_table.`json` - test_json_table.`text`;
UPDATE test_json_type, test_json_table SET test_json_type.`json*text` = test_json_table.`json` * test_json_table.`text`;
UPDATE test_json_type, test_json_table SET test_json_type.`json/text` = test_json_table.`json` / test_json_table.`text`;
UPDATE test_json_type, test_json_table SET test_json_type.`json%text` = test_json_table.`json` % test_json_table.`text`;
UPDATE test_json_type, test_json_table SET test_json_type.`json^text` = test_json_table.`json` ^ test_json_table.`text`;
UPDATE test_json_type, test_json_table SET test_json_type.`json>>text` = test_json_table.`json` >> test_json_table.`text`;
UPDATE test_json_type, test_json_table SET test_json_type.`json<<text` = test_json_table.`json` << test_json_table.`text`;
UPDATE test_json_type, test_json_table SET test_json_type.`json|text` = test_json_table.`json` | test_json_table.`text`;
UPDATE test_json_type, test_json_table SET test_json_type.`json&text` = test_json_table.`json` & test_json_table.`text`;
UPDATE test_json_type, test_json_table SET test_json_type.`json||text` = test_json_table.`json` || test_json_table.`text`;
UPDATE test_json_type, test_json_table SET test_json_type.`json&&text` = test_json_table.`json` && test_json_table.`text`;
UPDATE test_json_type, test_json_table SET test_json_type.`json_div_text` = test_json_table.`json` div test_json_table.`text`;
UPDATE test_json_type, test_json_table SET test_json_type.`json_mod_text` = test_json_table.`json` mod test_json_table.`text`;
UPDATE test_json_type, test_json_table SET test_json_type.`json_xor_text` = test_json_table.`json` xor test_json_table.`text`;
UPDATE test_json_type, test_json_table SET test_json_type.`json_and_text` = test_json_table.`json` and test_json_table.`text`;
UPDATE test_json_type, test_json_table SET test_json_type.`json_or_text` = test_json_table.`json` or test_json_table.`text`;
UPDATE test_json_type, test_json_table SET test_json_type.`json+enum_t` = test_json_table.`json` + test_json_table.`enum_t`;
UPDATE test_json_type, test_json_table SET test_json_type.`json-enum_t` = test_json_table.`json` - test_json_table.`enum_t`;
UPDATE test_json_type, test_json_table SET test_json_type.`json*enum_t` = test_json_table.`json` * test_json_table.`enum_t`;
UPDATE test_json_type, test_json_table SET test_json_type.`json/enum_t` = test_json_table.`json` / test_json_table.`enum_t`;
UPDATE test_json_type, test_json_table SET test_json_type.`json%enum_t` = test_json_table.`json` % test_json_table.`enum_t`;
UPDATE test_json_type, test_json_table SET test_json_type.`json^enum_t` = test_json_table.`json` ^ test_json_table.`enum_t`;
UPDATE test_json_type, test_json_table SET test_json_type.`json>>enum_t` = test_json_table.`json` >> test_json_table.`enum_t`;
UPDATE test_json_type, test_json_table SET test_json_type.`json<<enum_t` = test_json_table.`json` << test_json_table.`enum_t`;
UPDATE test_json_type, test_json_table SET test_json_type.`json|enum_t` = test_json_table.`json` | test_json_table.`enum_t`;
UPDATE test_json_type, test_json_table SET test_json_type.`json&enum_t` = test_json_table.`json` & test_json_table.`enum_t`;
UPDATE test_json_type, test_json_table SET test_json_type.`json||enum_t` = test_json_table.`json` || test_json_table.`enum_t`;
UPDATE test_json_type, test_json_table SET test_json_type.`json&&enum_t` = test_json_table.`json` && test_json_table.`enum_t`;
UPDATE test_json_type, test_json_table SET test_json_type.`json_div_enum_t` = test_json_table.`json` div test_json_table.`enum_t`;
UPDATE test_json_type, test_json_table SET test_json_type.`json_mod_enum_t` = test_json_table.`json` mod test_json_table.`enum_t`;
UPDATE test_json_type, test_json_table SET test_json_type.`json_xor_enum_t` = test_json_table.`json` xor test_json_table.`enum_t`;
UPDATE test_json_type, test_json_table SET test_json_type.`json_and_enum_t` = test_json_table.`json` and test_json_table.`enum_t`;
UPDATE test_json_type, test_json_table SET test_json_type.`json_or_enum_t` = test_json_table.`json` or test_json_table.`enum_t`;
UPDATE test_json_type, test_json_table SET test_json_type.`json+set_t` = test_json_table.`json` + test_json_table.`set_t`;
UPDATE test_json_type, test_json_table SET test_json_type.`json-set_t` = test_json_table.`json` - test_json_table.`set_t`;
UPDATE test_json_type, test_json_table SET test_json_type.`json*set_t` = test_json_table.`json` * test_json_table.`set_t`;
UPDATE test_json_type, test_json_table SET test_json_type.`json/set_t` = test_json_table.`json` / test_json_table.`set_t`;
UPDATE test_json_type, test_json_table SET test_json_type.`json%set_t` = test_json_table.`json` % test_json_table.`set_t`;
UPDATE test_json_type, test_json_table SET test_json_type.`json^set_t` = test_json_table.`json` ^ test_json_table.`set_t`;
UPDATE test_json_type, test_json_table SET test_json_type.`json>>set_t` = test_json_table.`json` >> test_json_table.`set_t`;
UPDATE test_json_type, test_json_table SET test_json_type.`json<<set_t` = test_json_table.`json` << test_json_table.`set_t`;
UPDATE test_json_type, test_json_table SET test_json_type.`json|set_t` = test_json_table.`json` | test_json_table.`set_t`;
UPDATE test_json_type, test_json_table SET test_json_type.`json&set_t` = test_json_table.`json` & test_json_table.`set_t`;
UPDATE test_json_type, test_json_table SET test_json_type.`json||set_t` = test_json_table.`json` || test_json_table.`set_t`;
UPDATE test_json_type, test_json_table SET test_json_type.`json&&set_t` = test_json_table.`json` && test_json_table.`set_t`;
UPDATE test_json_type, test_json_table SET test_json_type.`json_div_set_t` = test_json_table.`json` div test_json_table.`set_t`;
UPDATE test_json_type, test_json_table SET test_json_type.`json_mod_set_t` = test_json_table.`json` mod test_json_table.`set_t`;
UPDATE test_json_type, test_json_table SET test_json_type.`json_xor_set_t` = test_json_table.`json` xor test_json_table.`set_t`;
UPDATE test_json_type, test_json_table SET test_json_type.`json_and_set_t` = test_json_table.`json` and test_json_table.`set_t`;
UPDATE test_json_type, test_json_table SET test_json_type.`json_or_set_t` = test_json_table.`json` or test_json_table.`set_t`;
UPDATE test_json_type, test_json_table SET test_json_type.`json+json` = test_json_table.`json` + test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`json-json` = test_json_table.`json` - test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`json*json` = test_json_table.`json` * test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`json/json` = test_json_table.`json` / test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`json%json` = test_json_table.`json` % test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`json^json` = test_json_table.`json` ^ test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`json>>json` = test_json_table.`json` >> test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`json<<json` = test_json_table.`json` << test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`json|json` = test_json_table.`json` | test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`json&json` = test_json_table.`json` & test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`json||json` = test_json_table.`json` || test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`json&&json` = test_json_table.`json` && test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`json_div_json` = test_json_table.`json` div test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`json_mod_json` = test_json_table.`json` mod test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`json_xor_json` = test_json_table.`json` xor test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`json_and_json` = test_json_table.`json` and test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`json_or_json` = test_json_table.`json` or test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`+json` = + test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`-json` = - test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`~json` = ~ test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`@json` = @ test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`!json` = ! test_json_table.`json`;
UPDATE test_json_type, test_json_table SET test_json_type.`not_json` = not test_json_table.`json`;
\x
SELECT * FROM test_json_type;
---------- tail ----------
drop schema json_operator_test_schema cascade;
reset current_schema;

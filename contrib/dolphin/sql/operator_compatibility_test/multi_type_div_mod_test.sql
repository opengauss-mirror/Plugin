------------------------------------
-- test bit_expr / DIV % MOD bit_expr
-- test type: number and str
--      number(int1, uint1, int2, uint2, int4, uint4, int8, uint8, float4, float8, bool, bit)、
--      str(char, varchar, text, binary, varbinary)
------------------------------------
create schema test_div_mod;
set search_path to test_div_mod;
set dolphin.b_compatibility_mode to on;
set dolphin.sql_mode = 'sql_mode_strict,sql_mode_full_group,pipes_as_concat,ansi_quotes,no_zero_date,pad_char_to_full_length,auto_recompile_function,error_for_division_by_zero';

-- prepare data
drop table if exists t_number;
create table t_number
(
	id integer,
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
	`boolean` boolean
);

insert into t_number values (1, 1, 1, 1, 1, 1, 1, 1, 1, 1.0, 1.0, 3.14259, 1);
insert into t_number values (2, 127, 255, 32767, 65535, 0x7FFFFFFF, 0xFFFFFFFF, 0x7FFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 3.402823, 1.79769313486231, 3.141592, 0);
insert into t_number values (3, -127, 0, -32768, 0, -2147483648, 0, -9223372036854775808, 0, -1234.567890, -1002345.78456892, -99999999999999.999999, 1);

drop table if exists t_str;
create table t_str
(
	id integer,
	`char` char(100),
	`varchar` varchar(100),
	`binary` binary(100),
	`varbinary` varbinary(100),
	`text` text
);

insert into t_str values (1, '62.345*67-89', '62.345*67-89', '62.345*67-89', '62.345*67-89', '62.345*67-89');
insert into t_str values (2, 'Today is a good day.  ', 'Today is a good day.  ', 'Today is a good day.  ', 'Today is a good day.  ', 'Today is a good day.  ');
insert into t_str values (3, '  ', '  ', '  ', '  ', '  ');

drop table if exists t_bit;
create table t_bit (id integer, `bit1` bit(1), `bit8` bit(8), `bit15` bit(15), `bit64` bit(64));

insert into t_bit values (1, 0, 0x68, 0x4d45, 0x536f6d65006f6e65);
insert into t_bit values (2, 1, 0x7d, 0x0057, 0x00536f6d656f6e65);
insert into t_bit values (3, 0, 0x77, 0x5700, 0x536f6d656f6e6500);

create table t_res (id int, a_id int, b_id int, name varchar(100), res varchar(100));

-- test / result_type
create view v_int1_number as select a.`int1` / b.`int1` as int1_int1, a.`int1` / b.`uint1` as int1_uint1, a.`int1` / b.`int2` as int1_int2, a.`int1` / b.`uint2` as int1_uint2, a.`int1` / b.`int4` as int1_int4, a.`int1` / b.`uint4` as int1_uint4, a.`int1` / b.`int8` as int1_int8, a.`int1` / b.`uint8` as int1_uint8, a.`int1` / b.`float4` as int1_float4, a.`int1` / b.`float8` as int1_float8, a.`int1` / b.`numeric` as int1_numeric, a.`int1` / b.`boolean` as int1_boolean from `t_number` as a, `t_number` as b order by a.id, b.id;
desc v_int1_number;
drop view v_int1_number;

create view v_uint1_number as select a.`uint1` / b.`int1` as uint1_int1, a.`uint1` / b.`uint1` as uint1_uint1, a.`uint1` / b.`int2` as uint1_int2, a.`uint1` / b.`uint2` as uint1_uint2, a.`uint1` / b.`int4` as uint1_int4, a.`uint1` / b.`uint4` as uint1_uint4, a.`uint1` / b.`int8` as uint1_int8, a.`uint1` / b.`uint8` as uint1_uint8, a.`uint1` / b.`float4` as uint1_float4, a.`uint1` / b.`float8` as uint1_float8, a.`uint1` / b.`numeric` as uint1_numeric, a.`uint1` / b.`boolean` as uint1_boolean from `t_number` as a, `t_number` as b order by a.id, b.id;
desc v_uint1_number;
drop view v_uint1_number;

create view v_int2_number as select a.`int2` / b.`int1` as int2_int1, a.`int2` / b.`uint1` as int2_uint1, a.`int2` / b.`int2` as int2_int2, a.`int2` / b.`uint2` as int2_uint2, a.`int2` / b.`int4` as int2_int4, a.`int2` / b.`uint4` as int2_uint4, a.`int2` / b.`int8` as int2_int8, a.`int2` / b.`uint8` as int2_uint8, a.`int2` / b.`float4` as int2_float4, a.`int2` / b.`float8` as int2_float8, a.`int2` / b.`numeric` as int2_numeric, a.`int2` / b.`boolean` as int2_boolean from `t_number` as a, `t_number` as b order by a.id, b.id;
desc v_int2_number;
drop view v_int2_number;

create view v_uint2_number as select a.`uint2` / b.`int1` as uint2_int1, a.`uint2` / b.`uint1` as uint2_uint1, a.`uint2` / b.`int2` as uint2_int2, a.`uint2` / b.`uint2` as uint2_uint2, a.`uint2` / b.`int4` as uint2_int4, a.`uint2` / b.`uint4` as uint2_uint4, a.`uint2` / b.`int8` as uint2_int8, a.`uint2` / b.`uint8` as uint2_uint8, a.`uint2` / b.`float4` as uint2_float4, a.`uint2` / b.`float8` as uint2_float8, a.`uint2` / b.`numeric` as uint2_numeric, a.`uint2` / b.`boolean` as uint2_boolean from `t_number` as a, `t_number` as b order by a.id, b.id;
desc v_uint2_number;
drop view v_uint2_number;

create view v_int4_number as select a.`int4` / b.`int1` as int4_int1, a.`int4` / b.`uint1` as int4_uint1, a.`int4` / b.`int2` as int4_int2, a.`int4` / b.`uint2` as int4_uint2, a.`int4` / b.`int4` as int4_int4, a.`int4` / b.`uint4` as int4_uint4, a.`int4` / b.`int8` as int4_int8, a.`int4` / b.`uint8` as int4_uint8, a.`int4` / b.`float4` as int4_float4, a.`int4` / b.`float8` as int4_float8, a.`int4` / b.`numeric` as int4_numeric, a.`int4` / b.`boolean` as int4_boolean from `t_number` as a, `t_number` as b order by a.id, b.id;
desc v_int4_number;
drop view v_int4_number;

create view v_uint4_number as select a.`uint4` / b.`int1` as uint4_int1, a.`uint4` / b.`uint1` as uint4_uint1, a.`uint4` / b.`int2` as uint4_int2, a.`uint4` / b.`uint2` as uint4_uint2, a.`uint4` / b.`int4` as uint4_int4, a.`uint4` / b.`uint4` as uint4_uint4, a.`uint4` / b.`int8` as uint4_int8, a.`uint4` / b.`uint8` as uint4_uint8, a.`uint4` / b.`float4` as uint4_float4, a.`uint4` / b.`float8` as uint4_float8, a.`uint4` / b.`numeric` as uint4_numeric, a.`uint4` / b.`boolean` as uint4_boolean from `t_number` as a, `t_number` as b order by a.id, b.id;
desc v_uint4_number;
drop view v_uint4_number;

create view v_int8_number as select a.`int8` / b.`int1` as int8_int1, a.`int8` / b.`uint1` as int8_uint1, a.`int8` / b.`int2` as int8_int2, a.`int8` / b.`uint2` as int8_uint2, a.`int8` / b.`int4` as int8_int4, a.`int8` / b.`uint4` as int8_uint4, a.`int8` / b.`int8` as int8_int8, a.`int8` / b.`uint8` as int8_uint8, a.`int8` / b.`float4` as int8_float4, a.`int8` / b.`float8` as int8_float8, a.`int8` / b.`numeric` as int8_numeric, a.`int8` / b.`boolean` as int8_boolean from `t_number` as a, `t_number` as b order by a.id, b.id;
desc v_int8_number;
drop view v_int8_number;

create view v_uint8_number as select a.`uint8` / b.`int1` as uint8_int1, a.`uint8` / b.`uint1` as uint8_uint1, a.`uint8` / b.`int2` as uint8_int2, a.`uint8` / b.`uint2` as uint8_uint2, a.`uint8` / b.`int4` as uint8_int4, a.`uint8` / b.`uint4` as uint8_uint4, a.`uint8` / b.`int8` as uint8_int8, a.`uint8` / b.`uint8` as uint8_uint8, a.`uint8` / b.`float4` as uint8_float4, a.`uint8` / b.`float8` as uint8_float8, a.`uint8` / b.`numeric` as uint8_numeric, a.`uint8` / b.`boolean` as uint8_boolean from `t_number` as a, `t_number` as b order by a.id, b.id;
desc v_uint8_number;
drop view v_uint8_number;

create view v_float4_number as select a.`float4` / b.`int1` as float4_int1, a.`float4` / b.`uint1` as float4_uint1, a.`float4` / b.`int2` as float4_int2, a.`float4` / b.`uint2` as float4_uint2, a.`float4` / b.`int4` as float4_int4, a.`float4` / b.`uint4` as float4_uint4, a.`float4` / b.`int8` as float4_int8, a.`float4` / b.`uint8` as float4_uint8, a.`float4` / b.`float4` as float4_float4, a.`float4` / b.`float8` as float4_float8, a.`float4` / b.`numeric` as float4_numeric, a.`float4` / b.`boolean` as float4_boolean from `t_number` as a, `t_number` as b order by a.id, b.id;
desc v_float4_number;
drop view v_float4_number;

create view v_float8_number as select a.`float8` / b.`int1` as float8_int1, a.`float8` / b.`uint1` as float8_uint1, a.`float8` / b.`int2` as float8_int2, a.`float8` / b.`uint2` as float8_uint2, a.`float8` / b.`int4` as float8_int4, a.`float8` / b.`uint4` as float8_uint4, a.`float8` / b.`int8` as float8_int8, a.`float8` / b.`uint8` as float8_uint8, a.`float8` / b.`float4` as float8_float4, a.`float8` / b.`float8` as float8_float8, a.`float8` / b.`numeric` as float8_numeric, a.`float8` / b.`boolean` as float8_boolean from `t_number` as a, `t_number` as b order by a.id, b.id;
desc v_float8_number;
drop view v_float8_number;

create view v_numeric_number as select a.`numeric` / b.`int1` as numeric_int1, a.`numeric` / b.`uint1` as numeric_uint1, a.`numeric` / b.`int2` as numeric_int2, a.`numeric` / b.`uint2` as numeric_uint2, a.`numeric` / b.`int4` as numeric_int4, a.`numeric` / b.`uint4` as numeric_uint4, a.`numeric` / b.`int8` as numeric_int8, a.`numeric` / b.`uint8` as numeric_uint8, a.`numeric` / b.`float4` as numeric_float4, a.`numeric` / b.`float8` as numeric_float8, a.`numeric` / b.`numeric` as numeric_numeric, a.`numeric` / b.`boolean` as numeric_boolean from `t_number` as a, `t_number` as b order by a.id, b.id;
desc v_numeric_number;
drop view v_numeric_number;

create view v_boolean_number as select a.`boolean` / b.`int1` as boolean_int1, a.`boolean` / b.`uint1` as boolean_uint1, a.`boolean` / b.`int2` as boolean_int2, a.`boolean` / b.`uint2` as boolean_uint2, a.`boolean` / b.`int4` as boolean_int4, a.`boolean` / b.`uint4` as boolean_uint4, a.`boolean` / b.`int8` as boolean_int8, a.`boolean` / b.`uint8` as boolean_uint8, a.`boolean` / b.`float4` as boolean_float4, a.`boolean` / b.`float8` as boolean_float8, a.`boolean` / b.`numeric` as boolean_numeric, a.`boolean` / b.`boolean` as boolean_boolean from `t_number` as a, `t_number` as b order by a.id, b.id;
desc v_boolean_number;
drop view v_boolean_number;

create view v_int1_string as select a.`int1` / b.`char` as int1_char, a.`int1` / b.`varchar` as int1_varchar, a.`int1` / b.`binary` as int1_binary, a.`int1` / b.`varbinary` as int1_varbinary, a.`int1` / b.`text` as int1_text from `t_number` as a, `t_str` as b order by a.id, b.id;
desc v_int1_string;
drop view v_int1_string;

create view v_uint1_string as select a.`uint1` / b.`char` as uint1_char, a.`uint1` / b.`varchar` as uint1_varchar, a.`uint1` / b.`binary` as uint1_binary, a.`uint1` / b.`varbinary` as uint1_varbinary, a.`uint1` / b.`text` as uint1_text from `t_number` as a, `t_str` as b order by a.id, b.id;
desc v_uint1_string;
drop view v_uint1_string;

create view v_int2_string as select a.`int2` / b.`char` as int2_char, a.`int2` / b.`varchar` as int2_varchar, a.`int2` / b.`binary` as int2_binary, a.`int2` / b.`varbinary` as int2_varbinary, a.`int2` / b.`text` as int2_text from `t_number` as a, `t_str` as b order by a.id, b.id;
desc v_int2_string;
drop view v_int2_string;

create view v_uint2_string as select a.`uint2` / b.`char` as uint2_char, a.`uint2` / b.`varchar` as uint2_varchar, a.`uint2` / b.`binary` as uint2_binary, a.`uint2` / b.`varbinary` as uint2_varbinary, a.`uint2` / b.`text` as uint2_text from `t_number` as a, `t_str` as b order by a.id, b.id;
desc v_uint2_string;
drop view v_uint2_string;

create view v_int4_string as select a.`int4` / b.`char` as int4_char, a.`int4` / b.`varchar` as int4_varchar, a.`int4` / b.`binary` as int4_binary, a.`int4` / b.`varbinary` as int4_varbinary, a.`int4` / b.`text` as int4_text from `t_number` as a, `t_str` as b order by a.id, b.id;
desc v_int4_string;
drop view v_int4_string;

create view v_uint4_string as select a.`uint4` / b.`char` as uint4_char, a.`uint4` / b.`varchar` as uint4_varchar, a.`uint4` / b.`binary` as uint4_binary, a.`uint4` / b.`varbinary` as uint4_varbinary, a.`uint4` / b.`text` as uint4_text from `t_number` as a, `t_str` as b order by a.id, b.id;
desc v_uint4_string;
drop view v_uint4_string;

create view v_int8_string as select a.`int8` / b.`char` as int8_char, a.`int8` / b.`varchar` as int8_varchar, a.`int8` / b.`binary` as int8_binary, a.`int8` / b.`varbinary` as int8_varbinary, a.`int8` / b.`text` as int8_text from `t_number` as a, `t_str` as b order by a.id, b.id;
desc v_int8_string;
drop view v_int8_string;

create view v_uint8_string as select a.`uint8` / b.`char` as uint8_char, a.`uint8` / b.`varchar` as uint8_varchar, a.`uint8` / b.`binary` as uint8_binary, a.`uint8` / b.`varbinary` as uint8_varbinary, a.`uint8` / b.`text` as uint8_text from `t_number` as a, `t_str` as b order by a.id, b.id;
desc v_uint8_string;
drop view v_uint8_string;

create view v_float4_string as select a.`float4` / b.`char` as float4_char, a.`float4` / b.`varchar` as float4_varchar, a.`float4` / b.`binary` as float4_binary, a.`float4` / b.`varbinary` as float4_varbinary, a.`float4` / b.`text` as float4_text from `t_number` as a, `t_str` as b order by a.id, b.id;
desc v_float4_string;
drop view v_float4_string;

create view v_float8_string as select a.`float8` / b.`char` as float8_char, a.`float8` / b.`varchar` as float8_varchar, a.`float8` / b.`binary` as float8_binary, a.`float8` / b.`varbinary` as float8_varbinary, a.`float8` / b.`text` as float8_text from `t_number` as a, `t_str` as b order by a.id, b.id;
desc v_float8_string;
drop view v_float8_string;

create view v_numeric_string as select a.`numeric` / b.`char` as numeric_char, a.`numeric` / b.`varchar` as numeric_varchar, a.`numeric` / b.`binary` as numeric_binary, a.`numeric` / b.`varbinary` as numeric_varbinary, a.`numeric` / b.`text` as numeric_text from `t_number` as a, `t_str` as b order by a.id, b.id;
desc v_numeric_string;
drop view v_numeric_string;

create view v_boolean_string as select a.`boolean` / b.`char` as boolean_char, a.`boolean` / b.`varchar` as boolean_varchar, a.`boolean` / b.`binary` as boolean_binary, a.`boolean` / b.`varbinary` as boolean_varbinary, a.`boolean` / b.`text` as boolean_text from `t_number` as a, `t_str` as b order by a.id, b.id;
desc v_boolean_string;
drop view v_boolean_string;

create view v_int1_bit as select a.`int1` / b.`bit1` as int1_bit1, a.`int1` / b.`bit8` as int1_bit8, a.`int1` / b.`bit15` as int1_bit15, a.`int1` / b.`bit64` as int1_bit64 from `t_number` as a, `t_bit` as b order by a.id, b.id;
desc v_int1_bit;
drop view v_int1_bit;

create view v_uint1_bit as select a.`uint1` / b.`bit1` as uint1_bit1, a.`uint1` / b.`bit8` as uint1_bit8, a.`uint1` / b.`bit15` as uint1_bit15, a.`uint1` / b.`bit64` as uint1_bit64 from `t_number` as a, `t_bit` as b order by a.id, b.id;
desc v_uint1_bit;
drop view v_uint1_bit;

create view v_int2_bit as select a.`int2` / b.`bit1` as int2_bit1, a.`int2` / b.`bit8` as int2_bit8, a.`int2` / b.`bit15` as int2_bit15, a.`int2` / b.`bit64` as int2_bit64 from `t_number` as a, `t_bit` as b order by a.id, b.id;
desc v_int2_bit;
drop view v_int2_bit;

create view v_uint2_bit as select a.`uint2` / b.`bit1` as uint2_bit1, a.`uint2` / b.`bit8` as uint2_bit8, a.`uint2` / b.`bit15` as uint2_bit15, a.`uint2` / b.`bit64` as uint2_bit64 from `t_number` as a, `t_bit` as b order by a.id, b.id;
desc v_uint2_bit;
drop view v_uint2_bit;

create view v_int4_bit as select a.`int4` / b.`bit1` as int4_bit1, a.`int4` / b.`bit8` as int4_bit8, a.`int4` / b.`bit15` as int4_bit15, a.`int4` / b.`bit64` as int4_bit64 from `t_number` as a, `t_bit` as b order by a.id, b.id;
desc v_int4_bit;
drop view v_int4_bit;

create view v_uint4_bit as select a.`uint4` / b.`bit1` as uint4_bit1, a.`uint4` / b.`bit8` as uint4_bit8, a.`uint4` / b.`bit15` as uint4_bit15, a.`uint4` / b.`bit64` as uint4_bit64 from `t_number` as a, `t_bit` as b order by a.id, b.id;
desc v_uint4_bit;
drop view v_uint4_bit;

create view v_int8_bit as select a.`int8` / b.`bit1` as int8_bit1, a.`int8` / b.`bit8` as int8_bit8, a.`int8` / b.`bit15` as int8_bit15, a.`int8` / b.`bit64` as int8_bit64 from `t_number` as a, `t_bit` as b order by a.id, b.id;
desc v_int8_bit;
drop view v_int8_bit;

create view v_uint8_bit as select a.`uint8` / b.`bit1` as uint8_bit1, a.`uint8` / b.`bit8` as uint8_bit8, a.`uint8` / b.`bit15` as uint8_bit15, a.`uint8` / b.`bit64` as uint8_bit64 from `t_number` as a, `t_bit` as b order by a.id, b.id;
desc v_uint8_bit;
drop view v_uint8_bit;

create view v_float4_bit as select a.`float4` / b.`bit1` as float4_bit1, a.`float4` / b.`bit8` as float4_bit8, a.`float4` / b.`bit15` as float4_bit15, a.`float4` / b.`bit64` as float4_bit64 from `t_number` as a, `t_bit` as b order by a.id, b.id;
desc v_float4_bit;
drop view v_float4_bit;

create view v_float8_bit as select a.`float8` / b.`bit1` as float8_bit1, a.`float8` / b.`bit8` as float8_bit8, a.`float8` / b.`bit15` as float8_bit15, a.`float8` / b.`bit64` as float8_bit64 from `t_number` as a, `t_bit` as b order by a.id, b.id;
desc v_float8_bit;
drop view v_float8_bit;

create view v_numeric_bit as select a.`numeric` / b.`bit1` as numeric_bit1, a.`numeric` / b.`bit8` as numeric_bit8, a.`numeric` / b.`bit15` as numeric_bit15, a.`numeric` / b.`bit64` as numeric_bit64 from `t_number` as a, `t_bit` as b order by a.id, b.id;
desc v_numeric_bit;
drop view v_numeric_bit;

create view v_boolean_bit as select a.`boolean` / b.`bit1` as boolean_bit1, a.`boolean` / b.`bit8` as boolean_bit8, a.`boolean` / b.`bit15` as boolean_bit15, a.`boolean` / b.`bit64` as boolean_bit64 from `t_number` as a, `t_bit` as b order by a.id, b.id;
desc v_boolean_bit;
drop view v_boolean_bit;

create view v_char_string as select a.`char` / b.`char` as char_char, a.`char` / b.`varchar` as char_varchar, a.`char` / b.`binary` as char_binary, a.`char` / b.`varbinary` as char_varbinary, a.`char` / b.`text` as char_text from `t_str` as a, `t_str` as b order by a.id, b.id;
desc v_char_string;
drop view v_char_string;

create view v_varchar_string as select a.`varchar` / b.`char` as varchar_char, a.`varchar` / b.`varchar` as varchar_varchar, a.`varchar` / b.`binary` as varchar_binary, a.`varchar` / b.`varbinary` as varchar_varbinary, a.`varchar` / b.`text` as varchar_text from `t_str` as a, `t_str` as b order by a.id, b.id;
desc v_varchar_string;
drop view v_varchar_string;

create view v_binary_string as select a.`binary` / b.`char` as binary_char, a.`binary` / b.`varchar` as binary_varchar, a.`binary` / b.`binary` as binary_binary, a.`binary` / b.`varbinary` as binary_varbinary, a.`binary` / b.`text` as binary_text from `t_str` as a, `t_str` as b order by a.id, b.id;
desc v_binary_string;
drop view v_binary_string;

create view v_varbinary_string as select a.`varbinary` / b.`char` as varbinary_char, a.`varbinary` / b.`varchar` as varbinary_varchar, a.`varbinary` / b.`binary` as varbinary_binary, a.`varbinary` / b.`varbinary` as varbinary_varbinary, a.`varbinary` / b.`text` as varbinary_text from `t_str` as a, `t_str` as b order by a.id, b.id;
desc v_varbinary_string;
drop view v_varbinary_string;

create view v_text_string as select a.`text` / b.`char` as text_char, a.`text` / b.`varchar` as text_varchar, a.`text` / b.`binary` as text_binary, a.`text` / b.`varbinary` as text_varbinary, a.`text` / b.`text` as text_text from `t_str` as a, `t_str` as b order by a.id, b.id;
desc v_text_string;
drop view v_text_string;

create view v_char_bit as select a.`char` / b.`bit1` as char_bit1, a.`char` / b.`bit8` as char_bit8, a.`char` / b.`bit15` as char_bit15, a.`char` / b.`bit64` as char_bit64 from `t_str` as a, `t_bit` as b order by a.id, b.id;
desc v_char_bit;
drop view v_char_bit;

create view v_varchar_bit as select a.`varchar` / b.`bit1` as varchar_bit1, a.`varchar` / b.`bit8` as varchar_bit8, a.`varchar` / b.`bit15` as varchar_bit15, a.`varchar` / b.`bit64` as varchar_bit64 from `t_str` as a, `t_bit` as b order by a.id, b.id;
desc v_varchar_bit;
drop view v_varchar_bit;

create view v_binary_bit as select a.`binary` / b.`bit1` as binary_bit1, a.`binary` / b.`bit8` as binary_bit8, a.`binary` / b.`bit15` as binary_bit15, a.`binary` / b.`bit64` as binary_bit64 from `t_str` as a, `t_bit` as b order by a.id, b.id;
desc v_binary_bit;
drop view v_binary_bit;

create view v_varbinary_bit as select a.`varbinary` / b.`bit1` as varbinary_bit1, a.`varbinary` / b.`bit8` as varbinary_bit8, a.`varbinary` / b.`bit15` as varbinary_bit15, a.`varbinary` / b.`bit64` as varbinary_bit64 from `t_str` as a, `t_bit` as b order by a.id, b.id;
desc v_varbinary_bit;
drop view v_varbinary_bit;

create view v_text_bit as select a.`text` / b.`bit1` as text_bit1, a.`text` / b.`bit8` as text_bit8, a.`text` / b.`bit15` as text_bit15, a.`text` / b.`bit64` as text_bit64 from `t_str` as a, `t_bit` as b order by a.id, b.id;
desc v_text_bit;
drop view v_text_bit;

create view v_bit1_bit as select a.`bit1` / b.`bit1` as bit1_bit1, a.`bit1` / b.`bit8` as bit1_bit8, a.`bit1` / b.`bit15` as bit1_bit15, a.`bit1` / b.`bit64` as bit1_bit64 from `t_bit` as a, `t_bit` as b order by a.id, b.id;
desc v_bit1_bit;
drop view v_bit1_bit;

create view v_bit8_bit as select a.`bit8` / b.`bit1` as bit8_bit1, a.`bit8` / b.`bit8` as bit8_bit8, a.`bit8` / b.`bit15` as bit8_bit15, a.`bit8` / b.`bit64` as bit8_bit64 from `t_bit` as a, `t_bit` as b order by a.id, b.id;
desc v_bit8_bit;
drop view v_bit8_bit;

create view v_bit15_bit as select a.`bit15` / b.`bit1` as bit15_bit1, a.`bit15` / b.`bit8` as bit15_bit8, a.`bit15` / b.`bit15` as bit15_bit15, a.`bit15` / b.`bit64` as bit15_bit64 from `t_bit` as a, `t_bit` as b order by a.id, b.id;
desc v_bit15_bit;
drop view v_bit15_bit;

create view v_bit64_bit as select a.`bit64` / b.`bit1` as bit64_bit1, a.`bit64` / b.`bit8` as bit64_bit8, a.`bit64` / b.`bit15` as bit64_bit15, a.`bit64` / b.`bit64` as bit64_bit64 from `t_bit` as a, `t_bit` as b order by a.id, b.id;
desc v_bit64_bit;
drop view v_bit64_bit;

-- test / result
truncate t_res;

insert into t_res select 1, a.id, b.id, 'int1/int1', a.`int1` / b.`int1` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 2, a.id, b.id, 'int1/uint1', a.`int1` / b.`uint1` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 3, a.id, b.id, 'int1/int2', a.`int1` / b.`int2` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 4, a.id, b.id, 'int1/uint2', a.`int1` / b.`uint2` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 5, a.id, b.id, 'int1/int4', a.`int1` / b.`int4` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 6, a.id, b.id, 'int1/uint4', a.`int1` / b.`uint4` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 7, a.id, b.id, 'int1/int8', a.`int1` / b.`int8` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 8, a.id, b.id, 'int1/uint8', a.`int1` / b.`uint8` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 9, a.id, b.id, 'int1/float4', a.`int1` / b.`float4` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 10, a.id, b.id, 'int1/float8', a.`int1` / b.`float8` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 11, a.id, b.id, 'int1/numeric', a.`int1` / b.`numeric` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 12, a.id, b.id, 'int1/boolean', a.`int1` / b.`boolean` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 13, a.id, b.id, 'uint1/int1', a.`uint1` / b.`int1` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 14, a.id, b.id, 'uint1/uint1', a.`uint1` / b.`uint1` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 15, a.id, b.id, 'uint1/int2', a.`uint1` / b.`int2` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 16, a.id, b.id, 'uint1/uint2', a.`uint1` / b.`uint2` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 17, a.id, b.id, 'uint1/int4', a.`uint1` / b.`int4` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 18, a.id, b.id, 'uint1/uint4', a.`uint1` / b.`uint4` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 19, a.id, b.id, 'uint1/int8', a.`uint1` / b.`int8` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 20, a.id, b.id, 'uint1/uint8', a.`uint1` / b.`uint8` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 21, a.id, b.id, 'uint1/float4', a.`uint1` / b.`float4` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 22, a.id, b.id, 'uint1/float8', a.`uint1` / b.`float8` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 23, a.id, b.id, 'uint1/numeric', a.`uint1` / b.`numeric` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 24, a.id, b.id, 'uint1/boolean', a.`uint1` / b.`boolean` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 25, a.id, b.id, 'int2/int1', a.`int2` / b.`int1` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 26, a.id, b.id, 'int2/uint1', a.`int2` / b.`uint1` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 27, a.id, b.id, 'int2/int2', a.`int2` / b.`int2` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 28, a.id, b.id, 'int2/uint2', a.`int2` / b.`uint2` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 29, a.id, b.id, 'int2/int4', a.`int2` / b.`int4` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 30, a.id, b.id, 'int2/uint4', a.`int2` / b.`uint4` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 31, a.id, b.id, 'int2/int8', a.`int2` / b.`int8` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 32, a.id, b.id, 'int2/uint8', a.`int2` / b.`uint8` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 33, a.id, b.id, 'int2/float4', a.`int2` / b.`float4` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 34, a.id, b.id, 'int2/float8', a.`int2` / b.`float8` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 35, a.id, b.id, 'int2/numeric', a.`int2` / b.`numeric` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 36, a.id, b.id, 'int2/boolean', a.`int2` / b.`boolean` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 37, a.id, b.id, 'uint2/int1', a.`uint2` / b.`int1` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 38, a.id, b.id, 'uint2/uint1', a.`uint2` / b.`uint1` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 39, a.id, b.id, 'uint2/int2', a.`uint2` / b.`int2` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 40, a.id, b.id, 'uint2/uint2', a.`uint2` / b.`uint2` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 41, a.id, b.id, 'uint2/int4', a.`uint2` / b.`int4` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 42, a.id, b.id, 'uint2/uint4', a.`uint2` / b.`uint4` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 43, a.id, b.id, 'uint2/int8', a.`uint2` / b.`int8` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 44, a.id, b.id, 'uint2/uint8', a.`uint2` / b.`uint8` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 45, a.id, b.id, 'uint2/float4', a.`uint2` / b.`float4` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 46, a.id, b.id, 'uint2/float8', a.`uint2` / b.`float8` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 47, a.id, b.id, 'uint2/numeric', a.`uint2` / b.`numeric` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 48, a.id, b.id, 'uint2/boolean', a.`uint2` / b.`boolean` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 49, a.id, b.id, 'int4/int1', a.`int4` / b.`int1` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 50, a.id, b.id, 'int4/uint1', a.`int4` / b.`uint1` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 51, a.id, b.id, 'int4/int2', a.`int4` / b.`int2` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 52, a.id, b.id, 'int4/uint2', a.`int4` / b.`uint2` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 53, a.id, b.id, 'int4/int4', a.`int4` / b.`int4` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 54, a.id, b.id, 'int4/uint4', a.`int4` / b.`uint4` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 55, a.id, b.id, 'int4/int8', a.`int4` / b.`int8` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 56, a.id, b.id, 'int4/uint8', a.`int4` / b.`uint8` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 57, a.id, b.id, 'int4/float4', a.`int4` / b.`float4` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 58, a.id, b.id, 'int4/float8', a.`int4` / b.`float8` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 59, a.id, b.id, 'int4/numeric', a.`int4` / b.`numeric` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 60, a.id, b.id, 'int4/boolean', a.`int4` / b.`boolean` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 61, a.id, b.id, 'uint4/int1', a.`uint4` / b.`int1` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 62, a.id, b.id, 'uint4/uint1', a.`uint4` / b.`uint1` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 63, a.id, b.id, 'uint4/int2', a.`uint4` / b.`int2` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 64, a.id, b.id, 'uint4/uint2', a.`uint4` / b.`uint2` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 65, a.id, b.id, 'uint4/int4', a.`uint4` / b.`int4` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 66, a.id, b.id, 'uint4/uint4', a.`uint4` / b.`uint4` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 67, a.id, b.id, 'uint4/int8', a.`uint4` / b.`int8` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 68, a.id, b.id, 'uint4/uint8', a.`uint4` / b.`uint8` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 69, a.id, b.id, 'uint4/float4', a.`uint4` / b.`float4` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 70, a.id, b.id, 'uint4/float8', a.`uint4` / b.`float8` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 71, a.id, b.id, 'uint4/numeric', a.`uint4` / b.`numeric` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 72, a.id, b.id, 'uint4/boolean', a.`uint4` / b.`boolean` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 73, a.id, b.id, 'int8/int1', a.`int8` / b.`int1` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 74, a.id, b.id, 'int8/uint1', a.`int8` / b.`uint1` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 75, a.id, b.id, 'int8/int2', a.`int8` / b.`int2` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 76, a.id, b.id, 'int8/uint2', a.`int8` / b.`uint2` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 77, a.id, b.id, 'int8/int4', a.`int8` / b.`int4` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 78, a.id, b.id, 'int8/uint4', a.`int8` / b.`uint4` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 79, a.id, b.id, 'int8/int8', a.`int8` / b.`int8` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 80, a.id, b.id, 'int8/uint8', a.`int8` / b.`uint8` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 81, a.id, b.id, 'int8/float4', a.`int8` / b.`float4` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 82, a.id, b.id, 'int8/float8', a.`int8` / b.`float8` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 83, a.id, b.id, 'int8/numeric', a.`int8` / b.`numeric` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 84, a.id, b.id, 'int8/boolean', a.`int8` / b.`boolean` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 85, a.id, b.id, 'uint8/int1', a.`uint8` / b.`int1` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 86, a.id, b.id, 'uint8/uint1', a.`uint8` / b.`uint1` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 87, a.id, b.id, 'uint8/int2', a.`uint8` / b.`int2` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 88, a.id, b.id, 'uint8/uint2', a.`uint8` / b.`uint2` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 89, a.id, b.id, 'uint8/int4', a.`uint8` / b.`int4` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 90, a.id, b.id, 'uint8/uint4', a.`uint8` / b.`uint4` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 91, a.id, b.id, 'uint8/int8', a.`uint8` / b.`int8` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 92, a.id, b.id, 'uint8/uint8', a.`uint8` / b.`uint8` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 93, a.id, b.id, 'uint8/float4', a.`uint8` / b.`float4` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 94, a.id, b.id, 'uint8/float8', a.`uint8` / b.`float8` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 95, a.id, b.id, 'uint8/numeric', a.`uint8` / b.`numeric` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 96, a.id, b.id, 'uint8/boolean', a.`uint8` / b.`boolean` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 97, a.id, b.id, 'float4/int1', a.`float4` / b.`int1` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 98, a.id, b.id, 'float4/uint1', a.`float4` / b.`uint1` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 99, a.id, b.id, 'float4/int2', a.`float4` / b.`int2` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 100, a.id, b.id, 'float4/uint2', a.`float4` / b.`uint2` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 101, a.id, b.id, 'float4/int4', a.`float4` / b.`int4` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 102, a.id, b.id, 'float4/uint4', a.`float4` / b.`uint4` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 103, a.id, b.id, 'float4/int8', a.`float4` / b.`int8` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 104, a.id, b.id, 'float4/uint8', a.`float4` / b.`uint8` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 105, a.id, b.id, 'float4/float4', a.`float4` / b.`float4` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 106, a.id, b.id, 'float4/float8', a.`float4` / b.`float8` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 107, a.id, b.id, 'float4/numeric', a.`float4` / b.`numeric` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 108, a.id, b.id, 'float4/boolean', a.`float4` / b.`boolean` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 109, a.id, b.id, 'float8/int1', a.`float8` / b.`int1` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 110, a.id, b.id, 'float8/uint1', a.`float8` / b.`uint1` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 111, a.id, b.id, 'float8/int2', a.`float8` / b.`int2` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 112, a.id, b.id, 'float8/uint2', a.`float8` / b.`uint2` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 113, a.id, b.id, 'float8/int4', a.`float8` / b.`int4` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 114, a.id, b.id, 'float8/uint4', a.`float8` / b.`uint4` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 115, a.id, b.id, 'float8/int8', a.`float8` / b.`int8` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 116, a.id, b.id, 'float8/uint8', a.`float8` / b.`uint8` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 117, a.id, b.id, 'float8/float4', a.`float8` / b.`float4` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 118, a.id, b.id, 'float8/float8', a.`float8` / b.`float8` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 119, a.id, b.id, 'float8/numeric', a.`float8` / b.`numeric` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 120, a.id, b.id, 'float8/boolean', a.`float8` / b.`boolean` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 121, a.id, b.id, 'numeric/int1', a.`numeric` / b.`int1` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 122, a.id, b.id, 'numeric/uint1', a.`numeric` / b.`uint1` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 123, a.id, b.id, 'numeric/int2', a.`numeric` / b.`int2` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 124, a.id, b.id, 'numeric/uint2', a.`numeric` / b.`uint2` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 125, a.id, b.id, 'numeric/int4', a.`numeric` / b.`int4` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 126, a.id, b.id, 'numeric/uint4', a.`numeric` / b.`uint4` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 127, a.id, b.id, 'numeric/int8', a.`numeric` / b.`int8` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 128, a.id, b.id, 'numeric/uint8', a.`numeric` / b.`uint8` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 129, a.id, b.id, 'numeric/float4', a.`numeric` / b.`float4` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 130, a.id, b.id, 'numeric/float8', a.`numeric` / b.`float8` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 131, a.id, b.id, 'numeric/numeric', a.`numeric` / b.`numeric` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 132, a.id, b.id, 'numeric/boolean', a.`numeric` / b.`boolean` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 133, a.id, b.id, 'boolean/int1', a.`boolean` / b.`int1` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 134, a.id, b.id, 'boolean/uint1', a.`boolean` / b.`uint1` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 135, a.id, b.id, 'boolean/int2', a.`boolean` / b.`int2` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 136, a.id, b.id, 'boolean/uint2', a.`boolean` / b.`uint2` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 137, a.id, b.id, 'boolean/int4', a.`boolean` / b.`int4` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 138, a.id, b.id, 'boolean/uint4', a.`boolean` / b.`uint4` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 139, a.id, b.id, 'boolean/int8', a.`boolean` / b.`int8` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 140, a.id, b.id, 'boolean/uint8', a.`boolean` / b.`uint8` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 141, a.id, b.id, 'boolean/float4', a.`boolean` / b.`float4` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 142, a.id, b.id, 'boolean/float8', a.`boolean` / b.`float8` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 143, a.id, b.id, 'boolean/numeric', a.`boolean` / b.`numeric` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 144, a.id, b.id, 'boolean/boolean', a.`boolean` / b.`boolean` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 145, a.id, b.id, 'int1/char', a.`int1` / b.`char` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 146, a.id, b.id, 'int1/varchar', a.`int1` / b.`varchar` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 147, a.id, b.id, 'int1/binary', a.`int1` / b.`binary` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 148, a.id, b.id, 'int1/varbinary', a.`int1` / b.`varbinary` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 149, a.id, b.id, 'int1/text', a.`int1` / b.`text` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 150, a.id, b.id, 'uint1/char', a.`uint1` / b.`char` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 151, a.id, b.id, 'uint1/varchar', a.`uint1` / b.`varchar` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 152, a.id, b.id, 'uint1/binary', a.`uint1` / b.`binary` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 153, a.id, b.id, 'uint1/varbinary', a.`uint1` / b.`varbinary` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 154, a.id, b.id, 'uint1/text', a.`uint1` / b.`text` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 155, a.id, b.id, 'int2/char', a.`int2` / b.`char` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 156, a.id, b.id, 'int2/varchar', a.`int2` / b.`varchar` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 157, a.id, b.id, 'int2/binary', a.`int2` / b.`binary` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 158, a.id, b.id, 'int2/varbinary', a.`int2` / b.`varbinary` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 159, a.id, b.id, 'int2/text', a.`int2` / b.`text` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 160, a.id, b.id, 'uint2/char', a.`uint2` / b.`char` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 161, a.id, b.id, 'uint2/varchar', a.`uint2` / b.`varchar` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 162, a.id, b.id, 'uint2/binary', a.`uint2` / b.`binary` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 163, a.id, b.id, 'uint2/varbinary', a.`uint2` / b.`varbinary` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 164, a.id, b.id, 'uint2/text', a.`uint2` / b.`text` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 165, a.id, b.id, 'int4/char', a.`int4` / b.`char` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 166, a.id, b.id, 'int4/varchar', a.`int4` / b.`varchar` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 167, a.id, b.id, 'int4/binary', a.`int4` / b.`binary` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 168, a.id, b.id, 'int4/varbinary', a.`int4` / b.`varbinary` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 169, a.id, b.id, 'int4/text', a.`int4` / b.`text` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 170, a.id, b.id, 'uint4/char', a.`uint4` / b.`char` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 171, a.id, b.id, 'uint4/varchar', a.`uint4` / b.`varchar` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 172, a.id, b.id, 'uint4/binary', a.`uint4` / b.`binary` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 173, a.id, b.id, 'uint4/varbinary', a.`uint4` / b.`varbinary` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 174, a.id, b.id, 'uint4/text', a.`uint4` / b.`text` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 175, a.id, b.id, 'int8/char', a.`int8` / b.`char` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 176, a.id, b.id, 'int8/varchar', a.`int8` / b.`varchar` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 177, a.id, b.id, 'int8/binary', a.`int8` / b.`binary` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 178, a.id, b.id, 'int8/varbinary', a.`int8` / b.`varbinary` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 179, a.id, b.id, 'int8/text', a.`int8` / b.`text` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 180, a.id, b.id, 'uint8/char', a.`uint8` / b.`char` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 181, a.id, b.id, 'uint8/varchar', a.`uint8` / b.`varchar` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 182, a.id, b.id, 'uint8/binary', a.`uint8` / b.`binary` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 183, a.id, b.id, 'uint8/varbinary', a.`uint8` / b.`varbinary` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 184, a.id, b.id, 'uint8/text', a.`uint8` / b.`text` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 185, a.id, b.id, 'float4/char', a.`float4` / b.`char` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 186, a.id, b.id, 'float4/varchar', a.`float4` / b.`varchar` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 187, a.id, b.id, 'float4/binary', a.`float4` / b.`binary` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 188, a.id, b.id, 'float4/varbinary', a.`float4` / b.`varbinary` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 189, a.id, b.id, 'float4/text', a.`float4` / b.`text` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 190, a.id, b.id, 'float8/char', a.`float8` / b.`char` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 191, a.id, b.id, 'float8/varchar', a.`float8` / b.`varchar` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 192, a.id, b.id, 'float8/binary', a.`float8` / b.`binary` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 193, a.id, b.id, 'float8/varbinary', a.`float8` / b.`varbinary` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 194, a.id, b.id, 'float8/text', a.`float8` / b.`text` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 195, a.id, b.id, 'numeric/char', a.`numeric` / b.`char` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 196, a.id, b.id, 'numeric/varchar', a.`numeric` / b.`varchar` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 197, a.id, b.id, 'numeric/binary', a.`numeric` / b.`binary` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 198, a.id, b.id, 'numeric/varbinary', a.`numeric` / b.`varbinary` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 199, a.id, b.id, 'numeric/text', a.`numeric` / b.`text` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 200, a.id, b.id, 'boolean/char', a.`boolean` / b.`char` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 201, a.id, b.id, 'boolean/varchar', a.`boolean` / b.`varchar` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 202, a.id, b.id, 'boolean/binary', a.`boolean` / b.`binary` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 203, a.id, b.id, 'boolean/varbinary', a.`boolean` / b.`varbinary` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 204, a.id, b.id, 'boolean/text', a.`boolean` / b.`text` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 205, a.id, b.id, 'int1/bit1', a.`int1` / b.`bit1` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 206, a.id, b.id, 'int1/bit8', a.`int1` / b.`bit8` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 207, a.id, b.id, 'int1/bit15', a.`int1` / b.`bit15` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 208, a.id, b.id, 'int1/bit64', a.`int1` / b.`bit64` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 209, a.id, b.id, 'uint1/bit1', a.`uint1` / b.`bit1` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 210, a.id, b.id, 'uint1/bit8', a.`uint1` / b.`bit8` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 211, a.id, b.id, 'uint1/bit15', a.`uint1` / b.`bit15` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 212, a.id, b.id, 'uint1/bit64', a.`uint1` / b.`bit64` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 213, a.id, b.id, 'int2/bit1', a.`int2` / b.`bit1` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 214, a.id, b.id, 'int2/bit8', a.`int2` / b.`bit8` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 215, a.id, b.id, 'int2/bit15', a.`int2` / b.`bit15` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 216, a.id, b.id, 'int2/bit64', a.`int2` / b.`bit64` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 217, a.id, b.id, 'uint2/bit1', a.`uint2` / b.`bit1` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 218, a.id, b.id, 'uint2/bit8', a.`uint2` / b.`bit8` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 219, a.id, b.id, 'uint2/bit15', a.`uint2` / b.`bit15` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 220, a.id, b.id, 'uint2/bit64', a.`uint2` / b.`bit64` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 221, a.id, b.id, 'int4/bit1', a.`int4` / b.`bit1` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 222, a.id, b.id, 'int4/bit8', a.`int4` / b.`bit8` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 223, a.id, b.id, 'int4/bit15', a.`int4` / b.`bit15` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 224, a.id, b.id, 'int4/bit64', a.`int4` / b.`bit64` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 225, a.id, b.id, 'uint4/bit1', a.`uint4` / b.`bit1` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 226, a.id, b.id, 'uint4/bit8', a.`uint4` / b.`bit8` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 227, a.id, b.id, 'uint4/bit15', a.`uint4` / b.`bit15` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 228, a.id, b.id, 'uint4/bit64', a.`uint4` / b.`bit64` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 229, a.id, b.id, 'int8/bit1', a.`int8` / b.`bit1` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 230, a.id, b.id, 'int8/bit8', a.`int8` / b.`bit8` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 231, a.id, b.id, 'int8/bit15', a.`int8` / b.`bit15` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 232, a.id, b.id, 'int8/bit64', a.`int8` / b.`bit64` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 233, a.id, b.id, 'uint8/bit1', a.`uint8` / b.`bit1` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 234, a.id, b.id, 'uint8/bit8', a.`uint8` / b.`bit8` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 235, a.id, b.id, 'uint8/bit15', a.`uint8` / b.`bit15` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 236, a.id, b.id, 'uint8/bit64', a.`uint8` / b.`bit64` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 237, a.id, b.id, 'float4/bit1', a.`float4` / b.`bit1` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 238, a.id, b.id, 'float4/bit8', a.`float4` / b.`bit8` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 239, a.id, b.id, 'float4/bit15', a.`float4` / b.`bit15` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 240, a.id, b.id, 'float4/bit64', a.`float4` / b.`bit64` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 241, a.id, b.id, 'float8/bit1', a.`float8` / b.`bit1` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 242, a.id, b.id, 'float8/bit8', a.`float8` / b.`bit8` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 243, a.id, b.id, 'float8/bit15', a.`float8` / b.`bit15` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 244, a.id, b.id, 'float8/bit64', a.`float8` / b.`bit64` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 245, a.id, b.id, 'numeric/bit1', a.`numeric` / b.`bit1` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 246, a.id, b.id, 'numeric/bit8', a.`numeric` / b.`bit8` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 247, a.id, b.id, 'numeric/bit15', a.`numeric` / b.`bit15` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 248, a.id, b.id, 'numeric/bit64', a.`numeric` / b.`bit64` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 249, a.id, b.id, 'boolean/bit1', a.`boolean` / b.`bit1` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 250, a.id, b.id, 'boolean/bit8', a.`boolean` / b.`bit8` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 251, a.id, b.id, 'boolean/bit15', a.`boolean` / b.`bit15` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 252, a.id, b.id, 'boolean/bit64', a.`boolean` / b.`bit64` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 253, a.id, b.id, 'char/char', a.`char` / b.`char` from t_str as a, t_str as b order by a.id, b.id;
insert into t_res select 254, a.id, b.id, 'char/varchar', a.`char` / b.`varchar` from t_str as a, t_str as b order by a.id, b.id;
insert into t_res select 255, a.id, b.id, 'char/binary', a.`char` / b.`binary` from t_str as a, t_str as b order by a.id, b.id;
insert into t_res select 256, a.id, b.id, 'char/varbinary', a.`char` / b.`varbinary` from t_str as a, t_str as b order by a.id, b.id;
insert into t_res select 257, a.id, b.id, 'char/text', a.`char` / b.`text` from t_str as a, t_str as b order by a.id, b.id;
insert into t_res select 258, a.id, b.id, 'varchar/char', a.`varchar` / b.`char` from t_str as a, t_str as b order by a.id, b.id;
insert into t_res select 259, a.id, b.id, 'varchar/varchar', a.`varchar` / b.`varchar` from t_str as a, t_str as b order by a.id, b.id;
insert into t_res select 260, a.id, b.id, 'varchar/binary', a.`varchar` / b.`binary` from t_str as a, t_str as b order by a.id, b.id;
insert into t_res select 261, a.id, b.id, 'varchar/varbinary', a.`varchar` / b.`varbinary` from t_str as a, t_str as b order by a.id, b.id;
insert into t_res select 262, a.id, b.id, 'varchar/text', a.`varchar` / b.`text` from t_str as a, t_str as b order by a.id, b.id;
insert into t_res select 263, a.id, b.id, 'binary/char', a.`binary` / b.`char` from t_str as a, t_str as b order by a.id, b.id;
insert into t_res select 264, a.id, b.id, 'binary/varchar', a.`binary` / b.`varchar` from t_str as a, t_str as b order by a.id, b.id;
insert into t_res select 265, a.id, b.id, 'binary/binary', a.`binary` / b.`binary` from t_str as a, t_str as b order by a.id, b.id;
insert into t_res select 266, a.id, b.id, 'binary/varbinary', a.`binary` / b.`varbinary` from t_str as a, t_str as b order by a.id, b.id;
insert into t_res select 267, a.id, b.id, 'binary/text', a.`binary` / b.`text` from t_str as a, t_str as b order by a.id, b.id;
insert into t_res select 268, a.id, b.id, 'varbinary/char', a.`varbinary` / b.`char` from t_str as a, t_str as b order by a.id, b.id;
insert into t_res select 269, a.id, b.id, 'varbinary/varchar', a.`varbinary` / b.`varchar` from t_str as a, t_str as b order by a.id, b.id;
insert into t_res select 270, a.id, b.id, 'varbinary/binary', a.`varbinary` / b.`binary` from t_str as a, t_str as b order by a.id, b.id;
insert into t_res select 271, a.id, b.id, 'varbinary/varbinary', a.`varbinary` / b.`varbinary` from t_str as a, t_str as b order by a.id, b.id;
insert into t_res select 272, a.id, b.id, 'varbinary/text', a.`varbinary` / b.`text` from t_str as a, t_str as b order by a.id, b.id;
insert into t_res select 273, a.id, b.id, 'text/char', a.`text` / b.`char` from t_str as a, t_str as b order by a.id, b.id;
insert into t_res select 274, a.id, b.id, 'text/varchar', a.`text` / b.`varchar` from t_str as a, t_str as b order by a.id, b.id;
insert into t_res select 275, a.id, b.id, 'text/binary', a.`text` / b.`binary` from t_str as a, t_str as b order by a.id, b.id;
insert into t_res select 276, a.id, b.id, 'text/varbinary', a.`text` / b.`varbinary` from t_str as a, t_str as b order by a.id, b.id;
insert into t_res select 277, a.id, b.id, 'text/text', a.`text` / b.`text` from t_str as a, t_str as b order by a.id, b.id;
insert into t_res select 278, a.id, b.id, 'char/bit1', a.`char` / b.`bit1` from t_str as a, t_bit as b order by a.id, b.id;
insert into t_res select 279, a.id, b.id, 'char/bit8', a.`char` / b.`bit8` from t_str as a, t_bit as b order by a.id, b.id;
insert into t_res select 280, a.id, b.id, 'char/bit15', a.`char` / b.`bit15` from t_str as a, t_bit as b order by a.id, b.id;
insert into t_res select 281, a.id, b.id, 'char/bit64', a.`char` / b.`bit64` from t_str as a, t_bit as b order by a.id, b.id;
insert into t_res select 282, a.id, b.id, 'varchar/bit1', a.`varchar` / b.`bit1` from t_str as a, t_bit as b order by a.id, b.id;
insert into t_res select 283, a.id, b.id, 'varchar/bit8', a.`varchar` / b.`bit8` from t_str as a, t_bit as b order by a.id, b.id;
insert into t_res select 284, a.id, b.id, 'varchar/bit15', a.`varchar` / b.`bit15` from t_str as a, t_bit as b order by a.id, b.id;
insert into t_res select 285, a.id, b.id, 'varchar/bit64', a.`varchar` / b.`bit64` from t_str as a, t_bit as b order by a.id, b.id;
insert into t_res select 286, a.id, b.id, 'binary/bit1', a.`binary` / b.`bit1` from t_str as a, t_bit as b order by a.id, b.id;
insert into t_res select 287, a.id, b.id, 'binary/bit8', a.`binary` / b.`bit8` from t_str as a, t_bit as b order by a.id, b.id;
insert into t_res select 288, a.id, b.id, 'binary/bit15', a.`binary` / b.`bit15` from t_str as a, t_bit as b order by a.id, b.id;
insert into t_res select 289, a.id, b.id, 'binary/bit64', a.`binary` / b.`bit64` from t_str as a, t_bit as b order by a.id, b.id;
insert into t_res select 290, a.id, b.id, 'varbinary/bit1', a.`varbinary` / b.`bit1` from t_str as a, t_bit as b order by a.id, b.id;
insert into t_res select 291, a.id, b.id, 'varbinary/bit8', a.`varbinary` / b.`bit8` from t_str as a, t_bit as b order by a.id, b.id;
insert into t_res select 292, a.id, b.id, 'varbinary/bit15', a.`varbinary` / b.`bit15` from t_str as a, t_bit as b order by a.id, b.id;
insert into t_res select 293, a.id, b.id, 'varbinary/bit64', a.`varbinary` / b.`bit64` from t_str as a, t_bit as b order by a.id, b.id;
insert into t_res select 294, a.id, b.id, 'text/bit1', a.`text` / b.`bit1` from t_str as a, t_bit as b order by a.id, b.id;
insert into t_res select 295, a.id, b.id, 'text/bit8', a.`text` / b.`bit8` from t_str as a, t_bit as b order by a.id, b.id;
insert into t_res select 296, a.id, b.id, 'text/bit15', a.`text` / b.`bit15` from t_str as a, t_bit as b order by a.id, b.id;
insert into t_res select 297, a.id, b.id, 'text/bit64', a.`text` / b.`bit64` from t_str as a, t_bit as b order by a.id, b.id;
insert into t_res select 298, a.id, b.id, 'bit1/bit1', a.`bit1` / b.`bit1` from t_bit as a, t_bit as b order by a.id, b.id;
insert into t_res select 299, a.id, b.id, 'bit1/bit8', a.`bit1` / b.`bit8` from t_bit as a, t_bit as b order by a.id, b.id;
insert into t_res select 300, a.id, b.id, 'bit1/bit15', a.`bit1` / b.`bit15` from t_bit as a, t_bit as b order by a.id, b.id;
insert into t_res select 301, a.id, b.id, 'bit1/bit64', a.`bit1` / b.`bit64` from t_bit as a, t_bit as b order by a.id, b.id;
insert into t_res select 302, a.id, b.id, 'bit8/bit1', a.`bit8` / b.`bit1` from t_bit as a, t_bit as b order by a.id, b.id;
insert into t_res select 303, a.id, b.id, 'bit8/bit8', a.`bit8` / b.`bit8` from t_bit as a, t_bit as b order by a.id, b.id;
insert into t_res select 304, a.id, b.id, 'bit8/bit15', a.`bit8` / b.`bit15` from t_bit as a, t_bit as b order by a.id, b.id;
insert into t_res select 305, a.id, b.id, 'bit8/bit64', a.`bit8` / b.`bit64` from t_bit as a, t_bit as b order by a.id, b.id;
insert into t_res select 306, a.id, b.id, 'bit15/bit1', a.`bit15` / b.`bit1` from t_bit as a, t_bit as b order by a.id, b.id;
insert into t_res select 307, a.id, b.id, 'bit15/bit8', a.`bit15` / b.`bit8` from t_bit as a, t_bit as b order by a.id, b.id;
insert into t_res select 308, a.id, b.id, 'bit15/bit15', a.`bit15` / b.`bit15` from t_bit as a, t_bit as b order by a.id, b.id;
insert into t_res select 309, a.id, b.id, 'bit15/bit64', a.`bit15` / b.`bit64` from t_bit as a, t_bit as b order by a.id, b.id;
insert into t_res select 310, a.id, b.id, 'bit64/bit1', a.`bit64` / b.`bit1` from t_bit as a, t_bit as b order by a.id, b.id;
insert into t_res select 311, a.id, b.id, 'bit64/bit8', a.`bit64` / b.`bit8` from t_bit as a, t_bit as b order by a.id, b.id;
insert into t_res select 312, a.id, b.id, 'bit64/bit15', a.`bit64` / b.`bit15` from t_bit as a, t_bit as b order by a.id, b.id;
insert into t_res select 313, a.id, b.id, 'bit64/bit64', a.`bit64` / b.`bit64` from t_bit as a, t_bit as b order by a.id, b.id;

select * from t_res order by id, a_id, b_id;


-- test DIV result_type
create view v_int1_number as select a.`int1` DIV b.`int1` as int1_int1, a.`int1` DIV b.`uint1` as int1_uint1, a.`int1` DIV b.`int2` as int1_int2, a.`int1` DIV b.`uint2` as int1_uint2, a.`int1` DIV b.`int4` as int1_int4, a.`int1` DIV b.`uint4` as int1_uint4, a.`int1` DIV b.`int8` as int1_int8, a.`int1` DIV b.`uint8` as int1_uint8, a.`int1` DIV b.`float4` as int1_float4, a.`int1` DIV b.`float8` as int1_float8, a.`int1` DIV b.`numeric` as int1_numeric, a.`int1` DIV b.`boolean` as int1_boolean from `t_number` as a, `t_number` as b order by a.id, b.id;
desc v_int1_number;
drop view v_int1_number;

create view v_uint1_number as select a.`uint1` DIV b.`int1` as uint1_int1, a.`uint1` DIV b.`uint1` as uint1_uint1, a.`uint1` DIV b.`int2` as uint1_int2, a.`uint1` DIV b.`uint2` as uint1_uint2, a.`uint1` DIV b.`int4` as uint1_int4, a.`uint1` DIV b.`uint4` as uint1_uint4, a.`uint1` DIV b.`int8` as uint1_int8, a.`uint1` DIV b.`uint8` as uint1_uint8, a.`uint1` DIV b.`float4` as uint1_float4, a.`uint1` DIV b.`float8` as uint1_float8, a.`uint1` DIV b.`numeric` as uint1_numeric, a.`uint1` DIV b.`boolean` as uint1_boolean from `t_number` as a, `t_number` as b order by a.id, b.id;
desc v_uint1_number;
drop view v_uint1_number;

create view v_int2_number as select a.`int2` DIV b.`int1` as int2_int1, a.`int2` DIV b.`uint1` as int2_uint1, a.`int2` DIV b.`int2` as int2_int2, a.`int2` DIV b.`uint2` as int2_uint2, a.`int2` DIV b.`int4` as int2_int4, a.`int2` DIV b.`uint4` as int2_uint4, a.`int2` DIV b.`int8` as int2_int8, a.`int2` DIV b.`uint8` as int2_uint8, a.`int2` DIV b.`float4` as int2_float4, a.`int2` DIV b.`float8` as int2_float8, a.`int2` DIV b.`numeric` as int2_numeric, a.`int2` DIV b.`boolean` as int2_boolean from `t_number` as a, `t_number` as b order by a.id, b.id;
desc v_int2_number;
drop view v_int2_number;

create view v_uint2_number as select a.`uint2` DIV b.`int1` as uint2_int1, a.`uint2` DIV b.`uint1` as uint2_uint1, a.`uint2` DIV b.`int2` as uint2_int2, a.`uint2` DIV b.`uint2` as uint2_uint2, a.`uint2` DIV b.`int4` as uint2_int4, a.`uint2` DIV b.`uint4` as uint2_uint4, a.`uint2` DIV b.`int8` as uint2_int8, a.`uint2` DIV b.`uint8` as uint2_uint8, a.`uint2` DIV b.`float4` as uint2_float4, a.`uint2` DIV b.`float8` as uint2_float8, a.`uint2` DIV b.`numeric` as uint2_numeric, a.`uint2` DIV b.`boolean` as uint2_boolean from `t_number` as a, `t_number` as b order by a.id, b.id;
desc v_uint2_number;
drop view v_uint2_number;

create view v_int4_number as select a.`int4` DIV b.`int1` as int4_int1, a.`int4` DIV b.`uint1` as int4_uint1, a.`int4` DIV b.`int2` as int4_int2, a.`int4` DIV b.`uint2` as int4_uint2, a.`int4` DIV b.`int4` as int4_int4, a.`int4` DIV b.`uint4` as int4_uint4, a.`int4` DIV b.`int8` as int4_int8, a.`int4` DIV b.`uint8` as int4_uint8, a.`int4` DIV b.`float4` as int4_float4, a.`int4` DIV b.`float8` as int4_float8, a.`int4` DIV b.`numeric` as int4_numeric, a.`int4` DIV b.`boolean` as int4_boolean from `t_number` as a, `t_number` as b order by a.id, b.id;
desc v_int4_number;
drop view v_int4_number;

create view v_uint4_number as select a.`uint4` DIV b.`int1` as uint4_int1, a.`uint4` DIV b.`uint1` as uint4_uint1, a.`uint4` DIV b.`int2` as uint4_int2, a.`uint4` DIV b.`uint2` as uint4_uint2, a.`uint4` DIV b.`int4` as uint4_int4, a.`uint4` DIV b.`uint4` as uint4_uint4, a.`uint4` DIV b.`int8` as uint4_int8, a.`uint4` DIV b.`uint8` as uint4_uint8, a.`uint4` DIV b.`float4` as uint4_float4, a.`uint4` DIV b.`float8` as uint4_float8, a.`uint4` DIV b.`numeric` as uint4_numeric, a.`uint4` DIV b.`boolean` as uint4_boolean from `t_number` as a, `t_number` as b order by a.id, b.id;
desc v_uint4_number;
drop view v_uint4_number;

create view v_int8_number as select a.`int8` DIV b.`int1` as int8_int1, a.`int8` DIV b.`uint1` as int8_uint1, a.`int8` DIV b.`int2` as int8_int2, a.`int8` DIV b.`uint2` as int8_uint2, a.`int8` DIV b.`int4` as int8_int4, a.`int8` DIV b.`uint4` as int8_uint4, a.`int8` DIV b.`int8` as int8_int8, a.`int8` DIV b.`uint8` as int8_uint8, a.`int8` DIV b.`float4` as int8_float4, a.`int8` DIV b.`float8` as int8_float8, a.`int8` DIV b.`numeric` as int8_numeric, a.`int8` DIV b.`boolean` as int8_boolean from `t_number` as a, `t_number` as b order by a.id, b.id;
desc v_int8_number;
drop view v_int8_number;

create view v_uint8_number as select a.`uint8` DIV b.`int1` as uint8_int1, a.`uint8` DIV b.`uint1` as uint8_uint1, a.`uint8` DIV b.`int2` as uint8_int2, a.`uint8` DIV b.`uint2` as uint8_uint2, a.`uint8` DIV b.`int4` as uint8_int4, a.`uint8` DIV b.`uint4` as uint8_uint4, a.`uint8` DIV b.`int8` as uint8_int8, a.`uint8` DIV b.`uint8` as uint8_uint8, a.`uint8` DIV b.`float4` as uint8_float4, a.`uint8` DIV b.`float8` as uint8_float8, a.`uint8` DIV b.`numeric` as uint8_numeric, a.`uint8` DIV b.`boolean` as uint8_boolean from `t_number` as a, `t_number` as b order by a.id, b.id;
desc v_uint8_number;
drop view v_uint8_number;

create view v_float4_number as select a.`float4` DIV b.`int1` as float4_int1, a.`float4` DIV b.`uint1` as float4_uint1, a.`float4` DIV b.`int2` as float4_int2, a.`float4` DIV b.`uint2` as float4_uint2, a.`float4` DIV b.`int4` as float4_int4, a.`float4` DIV b.`uint4` as float4_uint4, a.`float4` DIV b.`int8` as float4_int8, a.`float4` DIV b.`uint8` as float4_uint8, a.`float4` DIV b.`float4` as float4_float4, a.`float4` DIV b.`float8` as float4_float8, a.`float4` DIV b.`numeric` as float4_numeric, a.`float4` DIV b.`boolean` as float4_boolean from `t_number` as a, `t_number` as b order by a.id, b.id;
desc v_float4_number;
drop view v_float4_number;

create view v_float8_number as select a.`float8` DIV b.`int1` as float8_int1, a.`float8` DIV b.`uint1` as float8_uint1, a.`float8` DIV b.`int2` as float8_int2, a.`float8` DIV b.`uint2` as float8_uint2, a.`float8` DIV b.`int4` as float8_int4, a.`float8` DIV b.`uint4` as float8_uint4, a.`float8` DIV b.`int8` as float8_int8, a.`float8` DIV b.`uint8` as float8_uint8, a.`float8` DIV b.`float4` as float8_float4, a.`float8` DIV b.`float8` as float8_float8, a.`float8` DIV b.`numeric` as float8_numeric, a.`float8` DIV b.`boolean` as float8_boolean from `t_number` as a, `t_number` as b order by a.id, b.id;
desc v_float8_number;
drop view v_float8_number;

create view v_numeric_number as select a.`numeric` DIV b.`int1` as numeric_int1, a.`numeric` DIV b.`uint1` as numeric_uint1, a.`numeric` DIV b.`int2` as numeric_int2, a.`numeric` DIV b.`uint2` as numeric_uint2, a.`numeric` DIV b.`int4` as numeric_int4, a.`numeric` DIV b.`uint4` as numeric_uint4, a.`numeric` DIV b.`int8` as numeric_int8, a.`numeric` DIV b.`uint8` as numeric_uint8, a.`numeric` DIV b.`float4` as numeric_float4, a.`numeric` DIV b.`float8` as numeric_float8, a.`numeric` DIV b.`numeric` as numeric_numeric, a.`numeric` DIV b.`boolean` as numeric_boolean from `t_number` as a, `t_number` as b order by a.id, b.id;
desc v_numeric_number;
drop view v_numeric_number;

create view v_boolean_number as select a.`boolean` DIV b.`int1` as boolean_int1, a.`boolean` DIV b.`uint1` as boolean_uint1, a.`boolean` DIV b.`int2` as boolean_int2, a.`boolean` DIV b.`uint2` as boolean_uint2, a.`boolean` DIV b.`int4` as boolean_int4, a.`boolean` DIV b.`uint4` as boolean_uint4, a.`boolean` DIV b.`int8` as boolean_int8, a.`boolean` DIV b.`uint8` as boolean_uint8, a.`boolean` DIV b.`float4` as boolean_float4, a.`boolean` DIV b.`float8` as boolean_float8, a.`boolean` DIV b.`numeric` as boolean_numeric, a.`boolean` DIV b.`boolean` as boolean_boolean from `t_number` as a, `t_number` as b order by a.id, b.id;
desc v_boolean_number;
drop view v_boolean_number;

create view v_int1_string as select a.`int1` DIV b.`char` as int1_char, a.`int1` DIV b.`varchar` as int1_varchar, a.`int1` DIV b.`binary` as int1_binary, a.`int1` DIV b.`varbinary` as int1_varbinary, a.`int1` DIV b.`text` as int1_text from `t_number` as a, `t_str` as b order by a.id, b.id;
desc v_int1_string;
drop view v_int1_string;

create view v_uint1_string as select a.`uint1` DIV b.`char` as uint1_char, a.`uint1` DIV b.`varchar` as uint1_varchar, a.`uint1` DIV b.`binary` as uint1_binary, a.`uint1` DIV b.`varbinary` as uint1_varbinary, a.`uint1` DIV b.`text` as uint1_text from `t_number` as a, `t_str` as b order by a.id, b.id;
desc v_uint1_string;
drop view v_uint1_string;

create view v_int2_string as select a.`int2` DIV b.`char` as int2_char, a.`int2` DIV b.`varchar` as int2_varchar, a.`int2` DIV b.`binary` as int2_binary, a.`int2` DIV b.`varbinary` as int2_varbinary, a.`int2` DIV b.`text` as int2_text from `t_number` as a, `t_str` as b order by a.id, b.id;
desc v_int2_string;
drop view v_int2_string;

create view v_uint2_string as select a.`uint2` DIV b.`char` as uint2_char, a.`uint2` DIV b.`varchar` as uint2_varchar, a.`uint2` DIV b.`binary` as uint2_binary, a.`uint2` DIV b.`varbinary` as uint2_varbinary, a.`uint2` DIV b.`text` as uint2_text from `t_number` as a, `t_str` as b order by a.id, b.id;
desc v_uint2_string;
drop view v_uint2_string;

create view v_int4_string as select a.`int4` DIV b.`char` as int4_char, a.`int4` DIV b.`varchar` as int4_varchar, a.`int4` DIV b.`binary` as int4_binary, a.`int4` DIV b.`varbinary` as int4_varbinary, a.`int4` DIV b.`text` as int4_text from `t_number` as a, `t_str` as b order by a.id, b.id;
desc v_int4_string;
drop view v_int4_string;

create view v_uint4_string as select a.`uint4` DIV b.`char` as uint4_char, a.`uint4` DIV b.`varchar` as uint4_varchar, a.`uint4` DIV b.`binary` as uint4_binary, a.`uint4` DIV b.`varbinary` as uint4_varbinary, a.`uint4` DIV b.`text` as uint4_text from `t_number` as a, `t_str` as b order by a.id, b.id;
desc v_uint4_string;
drop view v_uint4_string;

create view v_int8_string as select a.`int8` DIV b.`char` as int8_char, a.`int8` DIV b.`varchar` as int8_varchar, a.`int8` DIV b.`binary` as int8_binary, a.`int8` DIV b.`varbinary` as int8_varbinary, a.`int8` DIV b.`text` as int8_text from `t_number` as a, `t_str` as b order by a.id, b.id;
desc v_int8_string;
drop view v_int8_string;

create view v_uint8_string as select a.`uint8` DIV b.`char` as uint8_char, a.`uint8` DIV b.`varchar` as uint8_varchar, a.`uint8` DIV b.`binary` as uint8_binary, a.`uint8` DIV b.`varbinary` as uint8_varbinary, a.`uint8` DIV b.`text` as uint8_text from `t_number` as a, `t_str` as b order by a.id, b.id;
desc v_uint8_string;
drop view v_uint8_string;

create view v_float4_string as select a.`float4` DIV b.`char` as float4_char, a.`float4` DIV b.`varchar` as float4_varchar, a.`float4` DIV b.`binary` as float4_binary, a.`float4` DIV b.`varbinary` as float4_varbinary, a.`float4` DIV b.`text` as float4_text from `t_number` as a, `t_str` as b order by a.id, b.id;
desc v_float4_string;
drop view v_float4_string;

create view v_float8_string as select a.`float8` DIV b.`char` as float8_char, a.`float8` DIV b.`varchar` as float8_varchar, a.`float8` DIV b.`binary` as float8_binary, a.`float8` DIV b.`varbinary` as float8_varbinary, a.`float8` DIV b.`text` as float8_text from `t_number` as a, `t_str` as b order by a.id, b.id;
desc v_float8_string;
drop view v_float8_string;

create view v_numeric_string as select a.`numeric` DIV b.`char` as numeric_char, a.`numeric` DIV b.`varchar` as numeric_varchar, a.`numeric` DIV b.`binary` as numeric_binary, a.`numeric` DIV b.`varbinary` as numeric_varbinary, a.`numeric` DIV b.`text` as numeric_text from `t_number` as a, `t_str` as b order by a.id, b.id;
desc v_numeric_string;
drop view v_numeric_string;

create view v_boolean_string as select a.`boolean` DIV b.`char` as boolean_char, a.`boolean` DIV b.`varchar` as boolean_varchar, a.`boolean` DIV b.`binary` as boolean_binary, a.`boolean` DIV b.`varbinary` as boolean_varbinary, a.`boolean` DIV b.`text` as boolean_text from `t_number` as a, `t_str` as b order by a.id, b.id;
desc v_boolean_string;
drop view v_boolean_string;

create view v_int1_bit as select a.`int1` DIV b.`bit1` as int1_bit1, a.`int1` DIV b.`bit8` as int1_bit8, a.`int1` DIV b.`bit15` as int1_bit15, a.`int1` DIV b.`bit64` as int1_bit64 from `t_number` as a, `t_bit` as b order by a.id, b.id;
desc v_int1_bit;
drop view v_int1_bit;

create view v_uint1_bit as select a.`uint1` DIV b.`bit1` as uint1_bit1, a.`uint1` DIV b.`bit8` as uint1_bit8, a.`uint1` DIV b.`bit15` as uint1_bit15, a.`uint1` DIV b.`bit64` as uint1_bit64 from `t_number` as a, `t_bit` as b order by a.id, b.id;
desc v_uint1_bit;
drop view v_uint1_bit;

create view v_int2_bit as select a.`int2` DIV b.`bit1` as int2_bit1, a.`int2` DIV b.`bit8` as int2_bit8, a.`int2` DIV b.`bit15` as int2_bit15, a.`int2` DIV b.`bit64` as int2_bit64 from `t_number` as a, `t_bit` as b order by a.id, b.id;
desc v_int2_bit;
drop view v_int2_bit;

create view v_uint2_bit as select a.`uint2` DIV b.`bit1` as uint2_bit1, a.`uint2` DIV b.`bit8` as uint2_bit8, a.`uint2` DIV b.`bit15` as uint2_bit15, a.`uint2` DIV b.`bit64` as uint2_bit64 from `t_number` as a, `t_bit` as b order by a.id, b.id;
desc v_uint2_bit;
drop view v_uint2_bit;

create view v_int4_bit as select a.`int4` DIV b.`bit1` as int4_bit1, a.`int4` DIV b.`bit8` as int4_bit8, a.`int4` DIV b.`bit15` as int4_bit15, a.`int4` DIV b.`bit64` as int4_bit64 from `t_number` as a, `t_bit` as b order by a.id, b.id;
desc v_int4_bit;
drop view v_int4_bit;

create view v_uint4_bit as select a.`uint4` DIV b.`bit1` as uint4_bit1, a.`uint4` DIV b.`bit8` as uint4_bit8, a.`uint4` DIV b.`bit15` as uint4_bit15, a.`uint4` DIV b.`bit64` as uint4_bit64 from `t_number` as a, `t_bit` as b order by a.id, b.id;
desc v_uint4_bit;
drop view v_uint4_bit;

create view v_int8_bit as select a.`int8` DIV b.`bit1` as int8_bit1, a.`int8` DIV b.`bit8` as int8_bit8, a.`int8` DIV b.`bit15` as int8_bit15, a.`int8` DIV b.`bit64` as int8_bit64 from `t_number` as a, `t_bit` as b order by a.id, b.id;
desc v_int8_bit;
drop view v_int8_bit;

create view v_uint8_bit as select a.`uint8` DIV b.`bit1` as uint8_bit1, a.`uint8` DIV b.`bit8` as uint8_bit8, a.`uint8` DIV b.`bit15` as uint8_bit15, a.`uint8` DIV b.`bit64` as uint8_bit64 from `t_number` as a, `t_bit` as b order by a.id, b.id;
desc v_uint8_bit;
drop view v_uint8_bit;

create view v_float4_bit as select a.`float4` DIV b.`bit1` as float4_bit1, a.`float4` DIV b.`bit8` as float4_bit8, a.`float4` DIV b.`bit15` as float4_bit15, a.`float4` DIV b.`bit64` as float4_bit64 from `t_number` as a, `t_bit` as b order by a.id, b.id;
desc v_float4_bit;
drop view v_float4_bit;

create view v_float8_bit as select a.`float8` DIV b.`bit1` as float8_bit1, a.`float8` DIV b.`bit8` as float8_bit8, a.`float8` DIV b.`bit15` as float8_bit15, a.`float8` DIV b.`bit64` as float8_bit64 from `t_number` as a, `t_bit` as b order by a.id, b.id;
desc v_float8_bit;
drop view v_float8_bit;

create view v_numeric_bit as select a.`numeric` DIV b.`bit1` as numeric_bit1, a.`numeric` DIV b.`bit8` as numeric_bit8, a.`numeric` DIV b.`bit15` as numeric_bit15, a.`numeric` DIV b.`bit64` as numeric_bit64 from `t_number` as a, `t_bit` as b order by a.id, b.id;
desc v_numeric_bit;
drop view v_numeric_bit;

create view v_boolean_bit as select a.`boolean` DIV b.`bit1` as boolean_bit1, a.`boolean` DIV b.`bit8` as boolean_bit8, a.`boolean` DIV b.`bit15` as boolean_bit15, a.`boolean` DIV b.`bit64` as boolean_bit64 from `t_number` as a, `t_bit` as b order by a.id, b.id;
desc v_boolean_bit;
drop view v_boolean_bit;

create view v_char_string as select a.`char` DIV b.`char` as char_char, a.`char` DIV b.`varchar` as char_varchar, a.`char` DIV b.`binary` as char_binary, a.`char` DIV b.`varbinary` as char_varbinary, a.`char` DIV b.`text` as char_text from `t_str` as a, `t_str` as b order by a.id, b.id;
desc v_char_string;
drop view v_char_string;

create view v_varchar_string as select a.`varchar` DIV b.`char` as varchar_char, a.`varchar` DIV b.`varchar` as varchar_varchar, a.`varchar` DIV b.`binary` as varchar_binary, a.`varchar` DIV b.`varbinary` as varchar_varbinary, a.`varchar` DIV b.`text` as varchar_text from `t_str` as a, `t_str` as b order by a.id, b.id;
desc v_varchar_string;
drop view v_varchar_string;

create view v_binary_string as select a.`binary` DIV b.`char` as binary_char, a.`binary` DIV b.`varchar` as binary_varchar, a.`binary` DIV b.`binary` as binary_binary, a.`binary` DIV b.`varbinary` as binary_varbinary, a.`binary` DIV b.`text` as binary_text from `t_str` as a, `t_str` as b order by a.id, b.id;
desc v_binary_string;
drop view v_binary_string;

create view v_varbinary_string as select a.`varbinary` DIV b.`char` as varbinary_char, a.`varbinary` DIV b.`varchar` as varbinary_varchar, a.`varbinary` DIV b.`binary` as varbinary_binary, a.`varbinary` DIV b.`varbinary` as varbinary_varbinary, a.`varbinary` DIV b.`text` as varbinary_text from `t_str` as a, `t_str` as b order by a.id, b.id;
desc v_varbinary_string;
drop view v_varbinary_string;

create view v_text_string as select a.`text` DIV b.`char` as text_char, a.`text` DIV b.`varchar` as text_varchar, a.`text` DIV b.`binary` as text_binary, a.`text` DIV b.`varbinary` as text_varbinary, a.`text` DIV b.`text` as text_text from `t_str` as a, `t_str` as b order by a.id, b.id;
desc v_text_string;
drop view v_text_string;

create view v_char_bit as select a.`char` DIV b.`bit1` as char_bit1, a.`char` DIV b.`bit8` as char_bit8, a.`char` DIV b.`bit15` as char_bit15, a.`char` DIV b.`bit64` as char_bit64 from `t_str` as a, `t_bit` as b order by a.id, b.id;
desc v_char_bit;
drop view v_char_bit;

create view v_varchar_bit as select a.`varchar` DIV b.`bit1` as varchar_bit1, a.`varchar` DIV b.`bit8` as varchar_bit8, a.`varchar` DIV b.`bit15` as varchar_bit15, a.`varchar` DIV b.`bit64` as varchar_bit64 from `t_str` as a, `t_bit` as b order by a.id, b.id;
desc v_varchar_bit;
drop view v_varchar_bit;

create view v_binary_bit as select a.`binary` DIV b.`bit1` as binary_bit1, a.`binary` DIV b.`bit8` as binary_bit8, a.`binary` DIV b.`bit15` as binary_bit15, a.`binary` DIV b.`bit64` as binary_bit64 from `t_str` as a, `t_bit` as b order by a.id, b.id;
desc v_binary_bit;
drop view v_binary_bit;

create view v_varbinary_bit as select a.`varbinary` DIV b.`bit1` as varbinary_bit1, a.`varbinary` DIV b.`bit8` as varbinary_bit8, a.`varbinary` DIV b.`bit15` as varbinary_bit15, a.`varbinary` DIV b.`bit64` as varbinary_bit64 from `t_str` as a, `t_bit` as b order by a.id, b.id;
desc v_varbinary_bit;
drop view v_varbinary_bit;

create view v_text_bit as select a.`text` DIV b.`bit1` as text_bit1, a.`text` DIV b.`bit8` as text_bit8, a.`text` DIV b.`bit15` as text_bit15, a.`text` DIV b.`bit64` as text_bit64 from `t_str` as a, `t_bit` as b order by a.id, b.id;
desc v_text_bit;
drop view v_text_bit;

create view v_bit1_bit as select a.`bit1` DIV b.`bit1` as bit1_bit1, a.`bit1` DIV b.`bit8` as bit1_bit8, a.`bit1` DIV b.`bit15` as bit1_bit15, a.`bit1` DIV b.`bit64` as bit1_bit64 from `t_bit` as a, `t_bit` as b order by a.id, b.id;
desc v_bit1_bit;
drop view v_bit1_bit;

create view v_bit8_bit as select a.`bit8` DIV b.`bit1` as bit8_bit1, a.`bit8` DIV b.`bit8` as bit8_bit8, a.`bit8` DIV b.`bit15` as bit8_bit15, a.`bit8` DIV b.`bit64` as bit8_bit64 from `t_bit` as a, `t_bit` as b order by a.id, b.id;
desc v_bit8_bit;
drop view v_bit8_bit;

create view v_bit15_bit as select a.`bit15` DIV b.`bit1` as bit15_bit1, a.`bit15` DIV b.`bit8` as bit15_bit8, a.`bit15` DIV b.`bit15` as bit15_bit15, a.`bit15` DIV b.`bit64` as bit15_bit64 from `t_bit` as a, `t_bit` as b order by a.id, b.id;
desc v_bit15_bit;
drop view v_bit15_bit;

create view v_bit64_bit as select a.`bit64` DIV b.`bit1` as bit64_bit1, a.`bit64` DIV b.`bit8` as bit64_bit8, a.`bit64` DIV b.`bit15` as bit64_bit15, a.`bit64` DIV b.`bit64` as bit64_bit64 from `t_bit` as a, `t_bit` as b order by a.id, b.id;
desc v_bit64_bit;
drop view v_bit64_bit;

-- test DIV result
truncate t_res;

insert into t_res select 1, a.id, b.id, 'int1DIVint1', a.`int1` DIV b.`int1` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 2, a.id, b.id, 'int1DIVuint1', a.`int1` DIV b.`uint1` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 3, a.id, b.id, 'int1DIVint2', a.`int1` DIV b.`int2` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 4, a.id, b.id, 'int1DIVuint2', a.`int1` DIV b.`uint2` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 5, a.id, b.id, 'int1DIVint4', a.`int1` DIV b.`int4` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 6, a.id, b.id, 'int1DIVuint4', a.`int1` DIV b.`uint4` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 7, a.id, b.id, 'int1DIVint8', a.`int1` DIV b.`int8` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 8, a.id, b.id, 'int1DIVuint8', a.`int1` DIV b.`uint8` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 9, a.id, b.id, 'int1DIVfloat4', a.`int1` DIV b.`float4` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 10, a.id, b.id, 'int1DIVfloat8', a.`int1` DIV b.`float8` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 11, a.id, b.id, 'int1DIVnumeric', a.`int1` DIV b.`numeric` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 12, a.id, b.id, 'int1DIVboolean', a.`int1` DIV b.`boolean` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 13, a.id, b.id, 'uint1DIVint1', a.`uint1` DIV b.`int1` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 14, a.id, b.id, 'uint1DIVuint1', a.`uint1` DIV b.`uint1` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 15, a.id, b.id, 'uint1DIVint2', a.`uint1` DIV b.`int2` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 16, a.id, b.id, 'uint1DIVuint2', a.`uint1` DIV b.`uint2` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 17, a.id, b.id, 'uint1DIVint4', a.`uint1` DIV b.`int4` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 18, a.id, b.id, 'uint1DIVuint4', a.`uint1` DIV b.`uint4` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 19, a.id, b.id, 'uint1DIVint8', a.`uint1` DIV b.`int8` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 20, a.id, b.id, 'uint1DIVuint8', a.`uint1` DIV b.`uint8` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 21, a.id, b.id, 'uint1DIVfloat4', a.`uint1` DIV b.`float4` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 22, a.id, b.id, 'uint1DIVfloat8', a.`uint1` DIV b.`float8` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 23, a.id, b.id, 'uint1DIVnumeric', a.`uint1` DIV b.`numeric` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 24, a.id, b.id, 'uint1DIVboolean', a.`uint1` DIV b.`boolean` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 25, a.id, b.id, 'int2DIVint1', a.`int2` DIV b.`int1` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 26, a.id, b.id, 'int2DIVuint1', a.`int2` DIV b.`uint1` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 27, a.id, b.id, 'int2DIVint2', a.`int2` DIV b.`int2` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 28, a.id, b.id, 'int2DIVuint2', a.`int2` DIV b.`uint2` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 29, a.id, b.id, 'int2DIVint4', a.`int2` DIV b.`int4` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 30, a.id, b.id, 'int2DIVuint4', a.`int2` DIV b.`uint4` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 31, a.id, b.id, 'int2DIVint8', a.`int2` DIV b.`int8` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 32, a.id, b.id, 'int2DIVuint8', a.`int2` DIV b.`uint8` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 33, a.id, b.id, 'int2DIVfloat4', a.`int2` DIV b.`float4` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 34, a.id, b.id, 'int2DIVfloat8', a.`int2` DIV b.`float8` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 35, a.id, b.id, 'int2DIVnumeric', a.`int2` DIV b.`numeric` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 36, a.id, b.id, 'int2DIVboolean', a.`int2` DIV b.`boolean` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 37, a.id, b.id, 'uint2DIVint1', a.`uint2` DIV b.`int1` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 38, a.id, b.id, 'uint2DIVuint1', a.`uint2` DIV b.`uint1` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 39, a.id, b.id, 'uint2DIVint2', a.`uint2` DIV b.`int2` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 40, a.id, b.id, 'uint2DIVuint2', a.`uint2` DIV b.`uint2` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 41, a.id, b.id, 'uint2DIVint4', a.`uint2` DIV b.`int4` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 42, a.id, b.id, 'uint2DIVuint4', a.`uint2` DIV b.`uint4` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 43, a.id, b.id, 'uint2DIVint8', a.`uint2` DIV b.`int8` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 44, a.id, b.id, 'uint2DIVuint8', a.`uint2` DIV b.`uint8` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 45, a.id, b.id, 'uint2DIVfloat4', a.`uint2` DIV b.`float4` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 46, a.id, b.id, 'uint2DIVfloat8', a.`uint2` DIV b.`float8` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 47, a.id, b.id, 'uint2DIVnumeric', a.`uint2` DIV b.`numeric` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 48, a.id, b.id, 'uint2DIVboolean', a.`uint2` DIV b.`boolean` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 49, a.id, b.id, 'int4DIVint1', a.`int4` DIV b.`int1` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 50, a.id, b.id, 'int4DIVuint1', a.`int4` DIV b.`uint1` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 51, a.id, b.id, 'int4DIVint2', a.`int4` DIV b.`int2` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 52, a.id, b.id, 'int4DIVuint2', a.`int4` DIV b.`uint2` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 53, a.id, b.id, 'int4DIVint4', a.`int4` DIV b.`int4` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 54, a.id, b.id, 'int4DIVuint4', a.`int4` DIV b.`uint4` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 55, a.id, b.id, 'int4DIVint8', a.`int4` DIV b.`int8` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 56, a.id, b.id, 'int4DIVuint8', a.`int4` DIV b.`uint8` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 57, a.id, b.id, 'int4DIVfloat4', a.`int4` DIV b.`float4` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 58, a.id, b.id, 'int4DIVfloat8', a.`int4` DIV b.`float8` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 59, a.id, b.id, 'int4DIVnumeric', a.`int4` DIV b.`numeric` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 60, a.id, b.id, 'int4DIVboolean', a.`int4` DIV b.`boolean` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 61, a.id, b.id, 'uint4DIVint1', a.`uint4` DIV b.`int1` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 62, a.id, b.id, 'uint4DIVuint1', a.`uint4` DIV b.`uint1` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 63, a.id, b.id, 'uint4DIVint2', a.`uint4` DIV b.`int2` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 64, a.id, b.id, 'uint4DIVuint2', a.`uint4` DIV b.`uint2` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 65, a.id, b.id, 'uint4DIVint4', a.`uint4` DIV b.`int4` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 66, a.id, b.id, 'uint4DIVuint4', a.`uint4` DIV b.`uint4` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 67, a.id, b.id, 'uint4DIVint8', a.`uint4` DIV b.`int8` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 68, a.id, b.id, 'uint4DIVuint8', a.`uint4` DIV b.`uint8` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 69, a.id, b.id, 'uint4DIVfloat4', a.`uint4` DIV b.`float4` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 70, a.id, b.id, 'uint4DIVfloat8', a.`uint4` DIV b.`float8` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 71, a.id, b.id, 'uint4DIVnumeric', a.`uint4` DIV b.`numeric` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 72, a.id, b.id, 'uint4DIVboolean', a.`uint4` DIV b.`boolean` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 73, a.id, b.id, 'int8DIVint1', a.`int8` DIV b.`int1` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 74, a.id, b.id, 'int8DIVuint1', a.`int8` DIV b.`uint1` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 75, a.id, b.id, 'int8DIVint2', a.`int8` DIV b.`int2` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 76, a.id, b.id, 'int8DIVuint2', a.`int8` DIV b.`uint2` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 77, a.id, b.id, 'int8DIVint4', a.`int8` DIV b.`int4` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 78, a.id, b.id, 'int8DIVuint4', a.`int8` DIV b.`uint4` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 79, a.id, b.id, 'int8DIVint8', a.`int8` DIV b.`int8` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 80, a.id, b.id, 'int8DIVuint8', a.`int8` DIV b.`uint8` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 81, a.id, b.id, 'int8DIVfloat4', a.`int8` DIV b.`float4` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 82, a.id, b.id, 'int8DIVfloat8', a.`int8` DIV b.`float8` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 83, a.id, b.id, 'int8DIVnumeric', a.`int8` DIV b.`numeric` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 84, a.id, b.id, 'int8DIVboolean', a.`int8` DIV b.`boolean` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 85, a.id, b.id, 'uint8DIVint1', a.`uint8` DIV b.`int1` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 86, a.id, b.id, 'uint8DIVuint1', a.`uint8` DIV b.`uint1` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 87, a.id, b.id, 'uint8DIVint2', a.`uint8` DIV b.`int2` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 88, a.id, b.id, 'uint8DIVuint2', a.`uint8` DIV b.`uint2` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 89, a.id, b.id, 'uint8DIVint4', a.`uint8` DIV b.`int4` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 90, a.id, b.id, 'uint8DIVuint4', a.`uint8` DIV b.`uint4` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 91, a.id, b.id, 'uint8DIVint8', a.`uint8` DIV b.`int8` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 92, a.id, b.id, 'uint8DIVuint8', a.`uint8` DIV b.`uint8` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 93, a.id, b.id, 'uint8DIVfloat4', a.`uint8` DIV b.`float4` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 94, a.id, b.id, 'uint8DIVfloat8', a.`uint8` DIV b.`float8` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 95, a.id, b.id, 'uint8DIVnumeric', a.`uint8` DIV b.`numeric` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 96, a.id, b.id, 'uint8DIVboolean', a.`uint8` DIV b.`boolean` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 97, a.id, b.id, 'float4DIVint1', a.`float4` DIV b.`int1` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 98, a.id, b.id, 'float4DIVuint1', a.`float4` DIV b.`uint1` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 99, a.id, b.id, 'float4DIVint2', a.`float4` DIV b.`int2` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 100, a.id, b.id, 'float4DIVuint2', a.`float4` DIV b.`uint2` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 101, a.id, b.id, 'float4DIVint4', a.`float4` DIV b.`int4` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 102, a.id, b.id, 'float4DIVuint4', a.`float4` DIV b.`uint4` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 103, a.id, b.id, 'float4DIVint8', a.`float4` DIV b.`int8` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 104, a.id, b.id, 'float4DIVuint8', a.`float4` DIV b.`uint8` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 105, a.id, b.id, 'float4DIVfloat4', a.`float4` DIV b.`float4` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 106, a.id, b.id, 'float4DIVfloat8', a.`float4` DIV b.`float8` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 107, a.id, b.id, 'float4DIVnumeric', a.`float4` DIV b.`numeric` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 108, a.id, b.id, 'float4DIVboolean', a.`float4` DIV b.`boolean` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 109, a.id, b.id, 'float8DIVint1', a.`float8` DIV b.`int1` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 110, a.id, b.id, 'float8DIVuint1', a.`float8` DIV b.`uint1` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 111, a.id, b.id, 'float8DIVint2', a.`float8` DIV b.`int2` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 112, a.id, b.id, 'float8DIVuint2', a.`float8` DIV b.`uint2` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 113, a.id, b.id, 'float8DIVint4', a.`float8` DIV b.`int4` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 114, a.id, b.id, 'float8DIVuint4', a.`float8` DIV b.`uint4` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 115, a.id, b.id, 'float8DIVint8', a.`float8` DIV b.`int8` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 116, a.id, b.id, 'float8DIVuint8', a.`float8` DIV b.`uint8` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 117, a.id, b.id, 'float8DIVfloat4', a.`float8` DIV b.`float4` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 118, a.id, b.id, 'float8DIVfloat8', a.`float8` DIV b.`float8` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 119, a.id, b.id, 'float8DIVnumeric', a.`float8` DIV b.`numeric` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 120, a.id, b.id, 'float8DIVboolean', a.`float8` DIV b.`boolean` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 121, a.id, b.id, 'numericDIVint1', a.`numeric` DIV b.`int1` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 122, a.id, b.id, 'numericDIVuint1', a.`numeric` DIV b.`uint1` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 123, a.id, b.id, 'numericDIVint2', a.`numeric` DIV b.`int2` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 124, a.id, b.id, 'numericDIVuint2', a.`numeric` DIV b.`uint2` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 125, a.id, b.id, 'numericDIVint4', a.`numeric` DIV b.`int4` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 126, a.id, b.id, 'numericDIVuint4', a.`numeric` DIV b.`uint4` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 127, a.id, b.id, 'numericDIVint8', a.`numeric` DIV b.`int8` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 128, a.id, b.id, 'numericDIVuint8', a.`numeric` DIV b.`uint8` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 129, a.id, b.id, 'numericDIVfloat4', a.`numeric` DIV b.`float4` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 130, a.id, b.id, 'numericDIVfloat8', a.`numeric` DIV b.`float8` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 131, a.id, b.id, 'numericDIVnumeric', a.`numeric` DIV b.`numeric` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 132, a.id, b.id, 'numericDIVboolean', a.`numeric` DIV b.`boolean` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 133, a.id, b.id, 'booleanDIVint1', a.`boolean` DIV b.`int1` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 134, a.id, b.id, 'booleanDIVuint1', a.`boolean` DIV b.`uint1` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 135, a.id, b.id, 'booleanDIVint2', a.`boolean` DIV b.`int2` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 136, a.id, b.id, 'booleanDIVuint2', a.`boolean` DIV b.`uint2` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 137, a.id, b.id, 'booleanDIVint4', a.`boolean` DIV b.`int4` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 138, a.id, b.id, 'booleanDIVuint4', a.`boolean` DIV b.`uint4` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 139, a.id, b.id, 'booleanDIVint8', a.`boolean` DIV b.`int8` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 140, a.id, b.id, 'booleanDIVuint8', a.`boolean` DIV b.`uint8` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 141, a.id, b.id, 'booleanDIVfloat4', a.`boolean` DIV b.`float4` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 142, a.id, b.id, 'booleanDIVfloat8', a.`boolean` DIV b.`float8` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 143, a.id, b.id, 'booleanDIVnumeric', a.`boolean` DIV b.`numeric` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 144, a.id, b.id, 'booleanDIVboolean', a.`boolean` DIV b.`boolean` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 145, a.id, b.id, 'int1DIVchar', a.`int1` DIV b.`char` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 146, a.id, b.id, 'int1DIVvarchar', a.`int1` DIV b.`varchar` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 147, a.id, b.id, 'int1DIVbinary', a.`int1` DIV b.`binary` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 148, a.id, b.id, 'int1DIVvarbinary', a.`int1` DIV b.`varbinary` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 149, a.id, b.id, 'int1DIVtext', a.`int1` DIV b.`text` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 150, a.id, b.id, 'uint1DIVchar', a.`uint1` DIV b.`char` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 151, a.id, b.id, 'uint1DIVvarchar', a.`uint1` DIV b.`varchar` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 152, a.id, b.id, 'uint1DIVbinary', a.`uint1` DIV b.`binary` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 153, a.id, b.id, 'uint1DIVvarbinary', a.`uint1` DIV b.`varbinary` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 154, a.id, b.id, 'uint1DIVtext', a.`uint1` DIV b.`text` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 155, a.id, b.id, 'int2DIVchar', a.`int2` DIV b.`char` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 156, a.id, b.id, 'int2DIVvarchar', a.`int2` DIV b.`varchar` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 157, a.id, b.id, 'int2DIVbinary', a.`int2` DIV b.`binary` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 158, a.id, b.id, 'int2DIVvarbinary', a.`int2` DIV b.`varbinary` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 159, a.id, b.id, 'int2DIVtext', a.`int2` DIV b.`text` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 160, a.id, b.id, 'uint2DIVchar', a.`uint2` DIV b.`char` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 161, a.id, b.id, 'uint2DIVvarchar', a.`uint2` DIV b.`varchar` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 162, a.id, b.id, 'uint2DIVbinary', a.`uint2` DIV b.`binary` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 163, a.id, b.id, 'uint2DIVvarbinary', a.`uint2` DIV b.`varbinary` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 164, a.id, b.id, 'uint2DIVtext', a.`uint2` DIV b.`text` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 165, a.id, b.id, 'int4DIVchar', a.`int4` DIV b.`char` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 166, a.id, b.id, 'int4DIVvarchar', a.`int4` DIV b.`varchar` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 167, a.id, b.id, 'int4DIVbinary', a.`int4` DIV b.`binary` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 168, a.id, b.id, 'int4DIVvarbinary', a.`int4` DIV b.`varbinary` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 169, a.id, b.id, 'int4DIVtext', a.`int4` DIV b.`text` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 170, a.id, b.id, 'uint4DIVchar', a.`uint4` DIV b.`char` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 171, a.id, b.id, 'uint4DIVvarchar', a.`uint4` DIV b.`varchar` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 172, a.id, b.id, 'uint4DIVbinary', a.`uint4` DIV b.`binary` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 173, a.id, b.id, 'uint4DIVvarbinary', a.`uint4` DIV b.`varbinary` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 174, a.id, b.id, 'uint4DIVtext', a.`uint4` DIV b.`text` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 175, a.id, b.id, 'int8DIVchar', a.`int8` DIV b.`char` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 176, a.id, b.id, 'int8DIVvarchar', a.`int8` DIV b.`varchar` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 177, a.id, b.id, 'int8DIVbinary', a.`int8` DIV b.`binary` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 178, a.id, b.id, 'int8DIVvarbinary', a.`int8` DIV b.`varbinary` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 179, a.id, b.id, 'int8DIVtext', a.`int8` DIV b.`text` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 180, a.id, b.id, 'uint8DIVchar', a.`uint8` DIV b.`char` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 181, a.id, b.id, 'uint8DIVvarchar', a.`uint8` DIV b.`varchar` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 182, a.id, b.id, 'uint8DIVbinary', a.`uint8` DIV b.`binary` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 183, a.id, b.id, 'uint8DIVvarbinary', a.`uint8` DIV b.`varbinary` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 184, a.id, b.id, 'uint8DIVtext', a.`uint8` DIV b.`text` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 185, a.id, b.id, 'float4DIVchar', a.`float4` DIV b.`char` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 186, a.id, b.id, 'float4DIVvarchar', a.`float4` DIV b.`varchar` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 187, a.id, b.id, 'float4DIVbinary', a.`float4` DIV b.`binary` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 188, a.id, b.id, 'float4DIVvarbinary', a.`float4` DIV b.`varbinary` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 189, a.id, b.id, 'float4DIVtext', a.`float4` DIV b.`text` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 190, a.id, b.id, 'float8DIVchar', a.`float8` DIV b.`char` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 191, a.id, b.id, 'float8DIVvarchar', a.`float8` DIV b.`varchar` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 192, a.id, b.id, 'float8DIVbinary', a.`float8` DIV b.`binary` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 193, a.id, b.id, 'float8DIVvarbinary', a.`float8` DIV b.`varbinary` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 194, a.id, b.id, 'float8DIVtext', a.`float8` DIV b.`text` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 195, a.id, b.id, 'numericDIVchar', a.`numeric` DIV b.`char` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 196, a.id, b.id, 'numericDIVvarchar', a.`numeric` DIV b.`varchar` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 197, a.id, b.id, 'numericDIVbinary', a.`numeric` DIV b.`binary` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 198, a.id, b.id, 'numericDIVvarbinary', a.`numeric` DIV b.`varbinary` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 199, a.id, b.id, 'numericDIVtext', a.`numeric` DIV b.`text` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 200, a.id, b.id, 'booleanDIVchar', a.`boolean` DIV b.`char` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 201, a.id, b.id, 'booleanDIVvarchar', a.`boolean` DIV b.`varchar` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 202, a.id, b.id, 'booleanDIVbinary', a.`boolean` DIV b.`binary` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 203, a.id, b.id, 'booleanDIVvarbinary', a.`boolean` DIV b.`varbinary` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 204, a.id, b.id, 'booleanDIVtext', a.`boolean` DIV b.`text` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 205, a.id, b.id, 'int1DIVbit1', a.`int1` DIV b.`bit1` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 206, a.id, b.id, 'int1DIVbit8', a.`int1` DIV b.`bit8` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 207, a.id, b.id, 'int1DIVbit15', a.`int1` DIV b.`bit15` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 208, a.id, b.id, 'int1DIVbit64', a.`int1` DIV b.`bit64` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 209, a.id, b.id, 'uint1DIVbit1', a.`uint1` DIV b.`bit1` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 210, a.id, b.id, 'uint1DIVbit8', a.`uint1` DIV b.`bit8` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 211, a.id, b.id, 'uint1DIVbit15', a.`uint1` DIV b.`bit15` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 212, a.id, b.id, 'uint1DIVbit64', a.`uint1` DIV b.`bit64` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 213, a.id, b.id, 'int2DIVbit1', a.`int2` DIV b.`bit1` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 214, a.id, b.id, 'int2DIVbit8', a.`int2` DIV b.`bit8` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 215, a.id, b.id, 'int2DIVbit15', a.`int2` DIV b.`bit15` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 216, a.id, b.id, 'int2DIVbit64', a.`int2` DIV b.`bit64` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 217, a.id, b.id, 'uint2DIVbit1', a.`uint2` DIV b.`bit1` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 218, a.id, b.id, 'uint2DIVbit8', a.`uint2` DIV b.`bit8` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 219, a.id, b.id, 'uint2DIVbit15', a.`uint2` DIV b.`bit15` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 220, a.id, b.id, 'uint2DIVbit64', a.`uint2` DIV b.`bit64` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 221, a.id, b.id, 'int4DIVbit1', a.`int4` DIV b.`bit1` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 222, a.id, b.id, 'int4DIVbit8', a.`int4` DIV b.`bit8` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 223, a.id, b.id, 'int4DIVbit15', a.`int4` DIV b.`bit15` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 224, a.id, b.id, 'int4DIVbit64', a.`int4` DIV b.`bit64` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 225, a.id, b.id, 'uint4DIVbit1', a.`uint4` DIV b.`bit1` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 226, a.id, b.id, 'uint4DIVbit8', a.`uint4` DIV b.`bit8` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 227, a.id, b.id, 'uint4DIVbit15', a.`uint4` DIV b.`bit15` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 228, a.id, b.id, 'uint4DIVbit64', a.`uint4` DIV b.`bit64` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 229, a.id, b.id, 'int8DIVbit1', a.`int8` DIV b.`bit1` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 230, a.id, b.id, 'int8DIVbit8', a.`int8` DIV b.`bit8` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 231, a.id, b.id, 'int8DIVbit15', a.`int8` DIV b.`bit15` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 232, a.id, b.id, 'int8DIVbit64', a.`int8` DIV b.`bit64` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 233, a.id, b.id, 'uint8DIVbit1', a.`uint8` DIV b.`bit1` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 234, a.id, b.id, 'uint8DIVbit8', a.`uint8` DIV b.`bit8` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 235, a.id, b.id, 'uint8DIVbit15', a.`uint8` DIV b.`bit15` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 236, a.id, b.id, 'uint8DIVbit64', a.`uint8` DIV b.`bit64` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 237, a.id, b.id, 'float4DIVbit1', a.`float4` DIV b.`bit1` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 238, a.id, b.id, 'float4DIVbit8', a.`float4` DIV b.`bit8` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 239, a.id, b.id, 'float4DIVbit15', a.`float4` DIV b.`bit15` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 240, a.id, b.id, 'float4DIVbit64', a.`float4` DIV b.`bit64` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 241, a.id, b.id, 'float8DIVbit1', a.`float8` DIV b.`bit1` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 242, a.id, b.id, 'float8DIVbit8', a.`float8` DIV b.`bit8` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 243, a.id, b.id, 'float8DIVbit15', a.`float8` DIV b.`bit15` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 244, a.id, b.id, 'float8DIVbit64', a.`float8` DIV b.`bit64` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 245, a.id, b.id, 'numericDIVbit1', a.`numeric` DIV b.`bit1` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 246, a.id, b.id, 'numericDIVbit8', a.`numeric` DIV b.`bit8` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 247, a.id, b.id, 'numericDIVbit15', a.`numeric` DIV b.`bit15` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 248, a.id, b.id, 'numericDIVbit64', a.`numeric` DIV b.`bit64` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 249, a.id, b.id, 'booleanDIVbit1', a.`boolean` DIV b.`bit1` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 250, a.id, b.id, 'booleanDIVbit8', a.`boolean` DIV b.`bit8` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 251, a.id, b.id, 'booleanDIVbit15', a.`boolean` DIV b.`bit15` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 252, a.id, b.id, 'booleanDIVbit64', a.`boolean` DIV b.`bit64` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 253, a.id, b.id, 'charDIVchar', a.`char` DIV b.`char` from t_str as a, t_str as b order by a.id, b.id;
insert into t_res select 254, a.id, b.id, 'charDIVvarchar', a.`char` DIV b.`varchar` from t_str as a, t_str as b order by a.id, b.id;
insert into t_res select 255, a.id, b.id, 'charDIVbinary', a.`char` DIV b.`binary` from t_str as a, t_str as b order by a.id, b.id;
insert into t_res select 256, a.id, b.id, 'charDIVvarbinary', a.`char` DIV b.`varbinary` from t_str as a, t_str as b order by a.id, b.id;
insert into t_res select 257, a.id, b.id, 'charDIVtext', a.`char` DIV b.`text` from t_str as a, t_str as b order by a.id, b.id;
insert into t_res select 258, a.id, b.id, 'varcharDIVchar', a.`varchar` DIV b.`char` from t_str as a, t_str as b order by a.id, b.id;
insert into t_res select 259, a.id, b.id, 'varcharDIVvarchar', a.`varchar` DIV b.`varchar` from t_str as a, t_str as b order by a.id, b.id;
insert into t_res select 260, a.id, b.id, 'varcharDIVbinary', a.`varchar` DIV b.`binary` from t_str as a, t_str as b order by a.id, b.id;
insert into t_res select 261, a.id, b.id, 'varcharDIVvarbinary', a.`varchar` DIV b.`varbinary` from t_str as a, t_str as b order by a.id, b.id;
insert into t_res select 262, a.id, b.id, 'varcharDIVtext', a.`varchar` DIV b.`text` from t_str as a, t_str as b order by a.id, b.id;
insert into t_res select 263, a.id, b.id, 'binaryDIVchar', a.`binary` DIV b.`char` from t_str as a, t_str as b order by a.id, b.id;
insert into t_res select 264, a.id, b.id, 'binaryDIVvarchar', a.`binary` DIV b.`varchar` from t_str as a, t_str as b order by a.id, b.id;
insert into t_res select 265, a.id, b.id, 'binaryDIVbinary', a.`binary` DIV b.`binary` from t_str as a, t_str as b order by a.id, b.id;
insert into t_res select 266, a.id, b.id, 'binaryDIVvarbinary', a.`binary` DIV b.`varbinary` from t_str as a, t_str as b order by a.id, b.id;
insert into t_res select 267, a.id, b.id, 'binaryDIVtext', a.`binary` DIV b.`text` from t_str as a, t_str as b order by a.id, b.id;
insert into t_res select 268, a.id, b.id, 'varbinaryDIVchar', a.`varbinary` DIV b.`char` from t_str as a, t_str as b order by a.id, b.id;
insert into t_res select 269, a.id, b.id, 'varbinaryDIVvarchar', a.`varbinary` DIV b.`varchar` from t_str as a, t_str as b order by a.id, b.id;
insert into t_res select 270, a.id, b.id, 'varbinaryDIVbinary', a.`varbinary` DIV b.`binary` from t_str as a, t_str as b order by a.id, b.id;
insert into t_res select 271, a.id, b.id, 'varbinaryDIVvarbinary', a.`varbinary` DIV b.`varbinary` from t_str as a, t_str as b order by a.id, b.id;
insert into t_res select 272, a.id, b.id, 'varbinaryDIVtext', a.`varbinary` DIV b.`text` from t_str as a, t_str as b order by a.id, b.id;
insert into t_res select 273, a.id, b.id, 'textDIVchar', a.`text` DIV b.`char` from t_str as a, t_str as b order by a.id, b.id;
insert into t_res select 274, a.id, b.id, 'textDIVvarchar', a.`text` DIV b.`varchar` from t_str as a, t_str as b order by a.id, b.id;
insert into t_res select 275, a.id, b.id, 'textDIVbinary', a.`text` DIV b.`binary` from t_str as a, t_str as b order by a.id, b.id;
insert into t_res select 276, a.id, b.id, 'textDIVvarbinary', a.`text` DIV b.`varbinary` from t_str as a, t_str as b order by a.id, b.id;
insert into t_res select 277, a.id, b.id, 'textDIVtext', a.`text` DIV b.`text` from t_str as a, t_str as b order by a.id, b.id;
insert into t_res select 278, a.id, b.id, 'charDIVbit1', a.`char` DIV b.`bit1` from t_str as a, t_bit as b order by a.id, b.id;
insert into t_res select 279, a.id, b.id, 'charDIVbit8', a.`char` DIV b.`bit8` from t_str as a, t_bit as b order by a.id, b.id;
insert into t_res select 280, a.id, b.id, 'charDIVbit15', a.`char` DIV b.`bit15` from t_str as a, t_bit as b order by a.id, b.id;
insert into t_res select 281, a.id, b.id, 'charDIVbit64', a.`char` DIV b.`bit64` from t_str as a, t_bit as b order by a.id, b.id;
insert into t_res select 282, a.id, b.id, 'varcharDIVbit1', a.`varchar` DIV b.`bit1` from t_str as a, t_bit as b order by a.id, b.id;
insert into t_res select 283, a.id, b.id, 'varcharDIVbit8', a.`varchar` DIV b.`bit8` from t_str as a, t_bit as b order by a.id, b.id;
insert into t_res select 284, a.id, b.id, 'varcharDIVbit15', a.`varchar` DIV b.`bit15` from t_str as a, t_bit as b order by a.id, b.id;
insert into t_res select 285, a.id, b.id, 'varcharDIVbit64', a.`varchar` DIV b.`bit64` from t_str as a, t_bit as b order by a.id, b.id;
insert into t_res select 286, a.id, b.id, 'binaryDIVbit1', a.`binary` DIV b.`bit1` from t_str as a, t_bit as b order by a.id, b.id;
insert into t_res select 287, a.id, b.id, 'binaryDIVbit8', a.`binary` DIV b.`bit8` from t_str as a, t_bit as b order by a.id, b.id;
insert into t_res select 288, a.id, b.id, 'binaryDIVbit15', a.`binary` DIV b.`bit15` from t_str as a, t_bit as b order by a.id, b.id;
insert into t_res select 289, a.id, b.id, 'binaryDIVbit64', a.`binary` DIV b.`bit64` from t_str as a, t_bit as b order by a.id, b.id;
insert into t_res select 290, a.id, b.id, 'varbinaryDIVbit1', a.`varbinary` DIV b.`bit1` from t_str as a, t_bit as b order by a.id, b.id;
insert into t_res select 291, a.id, b.id, 'varbinaryDIVbit8', a.`varbinary` DIV b.`bit8` from t_str as a, t_bit as b order by a.id, b.id;
insert into t_res select 292, a.id, b.id, 'varbinaryDIVbit15', a.`varbinary` DIV b.`bit15` from t_str as a, t_bit as b order by a.id, b.id;
insert into t_res select 293, a.id, b.id, 'varbinaryDIVbit64', a.`varbinary` DIV b.`bit64` from t_str as a, t_bit as b order by a.id, b.id;
insert into t_res select 294, a.id, b.id, 'textDIVbit1', a.`text` DIV b.`bit1` from t_str as a, t_bit as b order by a.id, b.id;
insert into t_res select 295, a.id, b.id, 'textDIVbit8', a.`text` DIV b.`bit8` from t_str as a, t_bit as b order by a.id, b.id;
insert into t_res select 296, a.id, b.id, 'textDIVbit15', a.`text` DIV b.`bit15` from t_str as a, t_bit as b order by a.id, b.id;
insert into t_res select 297, a.id, b.id, 'textDIVbit64', a.`text` DIV b.`bit64` from t_str as a, t_bit as b order by a.id, b.id;
insert into t_res select 298, a.id, b.id, 'bit1DIVbit1', a.`bit1` DIV b.`bit1` from t_bit as a, t_bit as b order by a.id, b.id;
insert into t_res select 299, a.id, b.id, 'bit1DIVbit8', a.`bit1` DIV b.`bit8` from t_bit as a, t_bit as b order by a.id, b.id;
insert into t_res select 300, a.id, b.id, 'bit1DIVbit15', a.`bit1` DIV b.`bit15` from t_bit as a, t_bit as b order by a.id, b.id;
insert into t_res select 301, a.id, b.id, 'bit1DIVbit64', a.`bit1` DIV b.`bit64` from t_bit as a, t_bit as b order by a.id, b.id;
insert into t_res select 302, a.id, b.id, 'bit8DIVbit1', a.`bit8` DIV b.`bit1` from t_bit as a, t_bit as b order by a.id, b.id;
insert into t_res select 303, a.id, b.id, 'bit8DIVbit8', a.`bit8` DIV b.`bit8` from t_bit as a, t_bit as b order by a.id, b.id;
insert into t_res select 304, a.id, b.id, 'bit8DIVbit15', a.`bit8` DIV b.`bit15` from t_bit as a, t_bit as b order by a.id, b.id;
insert into t_res select 305, a.id, b.id, 'bit8DIVbit64', a.`bit8` DIV b.`bit64` from t_bit as a, t_bit as b order by a.id, b.id;
insert into t_res select 306, a.id, b.id, 'bit15DIVbit1', a.`bit15` DIV b.`bit1` from t_bit as a, t_bit as b order by a.id, b.id;
insert into t_res select 307, a.id, b.id, 'bit15DIVbit8', a.`bit15` DIV b.`bit8` from t_bit as a, t_bit as b order by a.id, b.id;
insert into t_res select 308, a.id, b.id, 'bit15DIVbit15', a.`bit15` DIV b.`bit15` from t_bit as a, t_bit as b order by a.id, b.id;
insert into t_res select 309, a.id, b.id, 'bit15DIVbit64', a.`bit15` DIV b.`bit64` from t_bit as a, t_bit as b order by a.id, b.id;
insert into t_res select 310, a.id, b.id, 'bit64DIVbit1', a.`bit64` DIV b.`bit1` from t_bit as a, t_bit as b order by a.id, b.id;
insert into t_res select 311, a.id, b.id, 'bit64DIVbit8', a.`bit64` DIV b.`bit8` from t_bit as a, t_bit as b order by a.id, b.id;
insert into t_res select 312, a.id, b.id, 'bit64DIVbit15', a.`bit64` DIV b.`bit15` from t_bit as a, t_bit as b order by a.id, b.id;
insert into t_res select 313, a.id, b.id, 'bit64DIVbit64', a.`bit64` DIV b.`bit64` from t_bit as a, t_bit as b order by a.id, b.id;

select * from t_res order by id, a_id, b_id;

-- test % result_type
create view v_int1_number as select a.`int1` % b.`int1` as int1_int1, a.`int1` % b.`uint1` as int1_uint1, a.`int1` % b.`int2` as int1_int2, a.`int1` % b.`uint2` as int1_uint2, a.`int1` % b.`int4` as int1_int4, a.`int1` % b.`uint4` as int1_uint4, a.`int1` % b.`int8` as int1_int8, a.`int1` % b.`uint8` as int1_uint8, a.`int1` % b.`float4` as int1_float4, a.`int1` % b.`float8` as int1_float8, a.`int1` % b.`numeric` as int1_numeric, a.`int1` % b.`boolean` as int1_boolean from `t_number` as a, `t_number` as b order by a.id, b.id;
desc v_int1_number;
drop view v_int1_number;

create view v_uint1_number as select a.`uint1` % b.`int1` as uint1_int1, a.`uint1` % b.`uint1` as uint1_uint1, a.`uint1` % b.`int2` as uint1_int2, a.`uint1` % b.`uint2` as uint1_uint2, a.`uint1` % b.`int4` as uint1_int4, a.`uint1` % b.`uint4` as uint1_uint4, a.`uint1` % b.`int8` as uint1_int8, a.`uint1` % b.`uint8` as uint1_uint8, a.`uint1` % b.`float4` as uint1_float4, a.`uint1` % b.`float8` as uint1_float8, a.`uint1` % b.`numeric` as uint1_numeric, a.`uint1` % b.`boolean` as uint1_boolean from `t_number` as a, `t_number` as b order by a.id, b.id;
desc v_uint1_number;
drop view v_uint1_number;

create view v_int2_number as select a.`int2` % b.`int1` as int2_int1, a.`int2` % b.`uint1` as int2_uint1, a.`int2` % b.`int2` as int2_int2, a.`int2` % b.`uint2` as int2_uint2, a.`int2` % b.`int4` as int2_int4, a.`int2` % b.`uint4` as int2_uint4, a.`int2` % b.`int8` as int2_int8, a.`int2` % b.`uint8` as int2_uint8, a.`int2` % b.`float4` as int2_float4, a.`int2` % b.`float8` as int2_float8, a.`int2` % b.`numeric` as int2_numeric, a.`int2` % b.`boolean` as int2_boolean from `t_number` as a, `t_number` as b order by a.id, b.id;
desc v_int2_number;
drop view v_int2_number;

create view v_uint2_number as select a.`uint2` % b.`int1` as uint2_int1, a.`uint2` % b.`uint1` as uint2_uint1, a.`uint2` % b.`int2` as uint2_int2, a.`uint2` % b.`uint2` as uint2_uint2, a.`uint2` % b.`int4` as uint2_int4, a.`uint2` % b.`uint4` as uint2_uint4, a.`uint2` % b.`int8` as uint2_int8, a.`uint2` % b.`uint8` as uint2_uint8, a.`uint2` % b.`float4` as uint2_float4, a.`uint2` % b.`float8` as uint2_float8, a.`uint2` % b.`numeric` as uint2_numeric, a.`uint2` % b.`boolean` as uint2_boolean from `t_number` as a, `t_number` as b order by a.id, b.id;
desc v_uint2_number;
drop view v_uint2_number;

create view v_int4_number as select a.`int4` % b.`int1` as int4_int1, a.`int4` % b.`uint1` as int4_uint1, a.`int4` % b.`int2` as int4_int2, a.`int4` % b.`uint2` as int4_uint2, a.`int4` % b.`int4` as int4_int4, a.`int4` % b.`uint4` as int4_uint4, a.`int4` % b.`int8` as int4_int8, a.`int4` % b.`uint8` as int4_uint8, a.`int4` % b.`float4` as int4_float4, a.`int4` % b.`float8` as int4_float8, a.`int4` % b.`numeric` as int4_numeric, a.`int4` % b.`boolean` as int4_boolean from `t_number` as a, `t_number` as b order by a.id, b.id;
desc v_int4_number;
drop view v_int4_number;

create view v_uint4_number as select a.`uint4` % b.`int1` as uint4_int1, a.`uint4` % b.`uint1` as uint4_uint1, a.`uint4` % b.`int2` as uint4_int2, a.`uint4` % b.`uint2` as uint4_uint2, a.`uint4` % b.`int4` as uint4_int4, a.`uint4` % b.`uint4` as uint4_uint4, a.`uint4` % b.`int8` as uint4_int8, a.`uint4` % b.`uint8` as uint4_uint8, a.`uint4` % b.`float4` as uint4_float4, a.`uint4` % b.`float8` as uint4_float8, a.`uint4` % b.`numeric` as uint4_numeric, a.`uint4` % b.`boolean` as uint4_boolean from `t_number` as a, `t_number` as b order by a.id, b.id;
desc v_uint4_number;
drop view v_uint4_number;

create view v_int8_number as select a.`int8` % b.`int1` as int8_int1, a.`int8` % b.`uint1` as int8_uint1, a.`int8` % b.`int2` as int8_int2, a.`int8` % b.`uint2` as int8_uint2, a.`int8` % b.`int4` as int8_int4, a.`int8` % b.`uint4` as int8_uint4, a.`int8` % b.`int8` as int8_int8, a.`int8` % b.`uint8` as int8_uint8, a.`int8` % b.`float4` as int8_float4, a.`int8` % b.`float8` as int8_float8, a.`int8` % b.`numeric` as int8_numeric, a.`int8` % b.`boolean` as int8_boolean from `t_number` as a, `t_number` as b order by a.id, b.id;
desc v_int8_number;
drop view v_int8_number;

create view v_uint8_number as select a.`uint8` % b.`int1` as uint8_int1, a.`uint8` % b.`uint1` as uint8_uint1, a.`uint8` % b.`int2` as uint8_int2, a.`uint8` % b.`uint2` as uint8_uint2, a.`uint8` % b.`int4` as uint8_int4, a.`uint8` % b.`uint4` as uint8_uint4, a.`uint8` % b.`int8` as uint8_int8, a.`uint8` % b.`uint8` as uint8_uint8, a.`uint8` % b.`float4` as uint8_float4, a.`uint8` % b.`float8` as uint8_float8, a.`uint8` % b.`numeric` as uint8_numeric, a.`uint8` % b.`boolean` as uint8_boolean from `t_number` as a, `t_number` as b order by a.id, b.id;
desc v_uint8_number;
drop view v_uint8_number;

create view v_float4_number as select a.`float4` % b.`int1` as float4_int1, a.`float4` % b.`uint1` as float4_uint1, a.`float4` % b.`int2` as float4_int2, a.`float4` % b.`uint2` as float4_uint2, a.`float4` % b.`int4` as float4_int4, a.`float4` % b.`uint4` as float4_uint4, a.`float4` % b.`int8` as float4_int8, a.`float4` % b.`uint8` as float4_uint8, a.`float4` % b.`float4` as float4_float4, a.`float4` % b.`float8` as float4_float8, a.`float4` % b.`numeric` as float4_numeric, a.`float4` % b.`boolean` as float4_boolean from `t_number` as a, `t_number` as b order by a.id, b.id;
desc v_float4_number;
drop view v_float4_number;

create view v_float8_number as select a.`float8` % b.`int1` as float8_int1, a.`float8` % b.`uint1` as float8_uint1, a.`float8` % b.`int2` as float8_int2, a.`float8` % b.`uint2` as float8_uint2, a.`float8` % b.`int4` as float8_int4, a.`float8` % b.`uint4` as float8_uint4, a.`float8` % b.`int8` as float8_int8, a.`float8` % b.`uint8` as float8_uint8, a.`float8` % b.`float4` as float8_float4, a.`float8` % b.`float8` as float8_float8, a.`float8` % b.`numeric` as float8_numeric, a.`float8` % b.`boolean` as float8_boolean from `t_number` as a, `t_number` as b order by a.id, b.id;
desc v_float8_number;
drop view v_float8_number;

create view v_numeric_number as select a.`numeric` % b.`int1` as numeric_int1, a.`numeric` % b.`uint1` as numeric_uint1, a.`numeric` % b.`int2` as numeric_int2, a.`numeric` % b.`uint2` as numeric_uint2, a.`numeric` % b.`int4` as numeric_int4, a.`numeric` % b.`uint4` as numeric_uint4, a.`numeric` % b.`int8` as numeric_int8, a.`numeric` % b.`uint8` as numeric_uint8, a.`numeric` % b.`float4` as numeric_float4, a.`numeric` % b.`float8` as numeric_float8, a.`numeric` % b.`numeric` as numeric_numeric, a.`numeric` % b.`boolean` as numeric_boolean from `t_number` as a, `t_number` as b order by a.id, b.id;
desc v_numeric_number;
drop view v_numeric_number;

create view v_boolean_number as select a.`boolean` % b.`int1` as boolean_int1, a.`boolean` % b.`uint1` as boolean_uint1, a.`boolean` % b.`int2` as boolean_int2, a.`boolean` % b.`uint2` as boolean_uint2, a.`boolean` % b.`int4` as boolean_int4, a.`boolean` % b.`uint4` as boolean_uint4, a.`boolean` % b.`int8` as boolean_int8, a.`boolean` % b.`uint8` as boolean_uint8, a.`boolean` % b.`float4` as boolean_float4, a.`boolean` % b.`float8` as boolean_float8, a.`boolean` % b.`numeric` as boolean_numeric, a.`boolean` % b.`boolean` as boolean_boolean from `t_number` as a, `t_number` as b order by a.id, b.id;
desc v_boolean_number;
drop view v_boolean_number;

create view v_int1_string as select a.`int1` % b.`char` as int1_char, a.`int1` % b.`varchar` as int1_varchar, a.`int1` % b.`binary` as int1_binary, a.`int1` % b.`varbinary` as int1_varbinary, a.`int1` % b.`text` as int1_text from `t_number` as a, `t_str` as b order by a.id, b.id;
desc v_int1_string;
drop view v_int1_string;

create view v_uint1_string as select a.`uint1` % b.`char` as uint1_char, a.`uint1` % b.`varchar` as uint1_varchar, a.`uint1` % b.`binary` as uint1_binary, a.`uint1` % b.`varbinary` as uint1_varbinary, a.`uint1` % b.`text` as uint1_text from `t_number` as a, `t_str` as b order by a.id, b.id;
desc v_uint1_string;
drop view v_uint1_string;

create view v_int2_string as select a.`int2` % b.`char` as int2_char, a.`int2` % b.`varchar` as int2_varchar, a.`int2` % b.`binary` as int2_binary, a.`int2` % b.`varbinary` as int2_varbinary, a.`int2` % b.`text` as int2_text from `t_number` as a, `t_str` as b order by a.id, b.id;
desc v_int2_string;
drop view v_int2_string;

create view v_uint2_string as select a.`uint2` % b.`char` as uint2_char, a.`uint2` % b.`varchar` as uint2_varchar, a.`uint2` % b.`binary` as uint2_binary, a.`uint2` % b.`varbinary` as uint2_varbinary, a.`uint2` % b.`text` as uint2_text from `t_number` as a, `t_str` as b order by a.id, b.id;
desc v_uint2_string;
drop view v_uint2_string;

create view v_int4_string as select a.`int4` % b.`char` as int4_char, a.`int4` % b.`varchar` as int4_varchar, a.`int4` % b.`binary` as int4_binary, a.`int4` % b.`varbinary` as int4_varbinary, a.`int4` % b.`text` as int4_text from `t_number` as a, `t_str` as b order by a.id, b.id;
desc v_int4_string;
drop view v_int4_string;

create view v_uint4_string as select a.`uint4` % b.`char` as uint4_char, a.`uint4` % b.`varchar` as uint4_varchar, a.`uint4` % b.`binary` as uint4_binary, a.`uint4` % b.`varbinary` as uint4_varbinary, a.`uint4` % b.`text` as uint4_text from `t_number` as a, `t_str` as b order by a.id, b.id;
desc v_uint4_string;
drop view v_uint4_string;

create view v_int8_string as select a.`int8` % b.`char` as int8_char, a.`int8` % b.`varchar` as int8_varchar, a.`int8` % b.`binary` as int8_binary, a.`int8` % b.`varbinary` as int8_varbinary, a.`int8` % b.`text` as int8_text from `t_number` as a, `t_str` as b order by a.id, b.id;
desc v_int8_string;
drop view v_int8_string;

create view v_uint8_string as select a.`uint8` % b.`char` as uint8_char, a.`uint8` % b.`varchar` as uint8_varchar, a.`uint8` % b.`binary` as uint8_binary, a.`uint8` % b.`varbinary` as uint8_varbinary, a.`uint8` % b.`text` as uint8_text from `t_number` as a, `t_str` as b order by a.id, b.id;
desc v_uint8_string;
drop view v_uint8_string;

create view v_float4_string as select a.`float4` % b.`char` as float4_char, a.`float4` % b.`varchar` as float4_varchar, a.`float4` % b.`binary` as float4_binary, a.`float4` % b.`varbinary` as float4_varbinary, a.`float4` % b.`text` as float4_text from `t_number` as a, `t_str` as b order by a.id, b.id;
desc v_float4_string;
drop view v_float4_string;

create view v_float8_string as select a.`float8` % b.`char` as float8_char, a.`float8` % b.`varchar` as float8_varchar, a.`float8` % b.`binary` as float8_binary, a.`float8` % b.`varbinary` as float8_varbinary, a.`float8` % b.`text` as float8_text from `t_number` as a, `t_str` as b order by a.id, b.id;
desc v_float8_string;
drop view v_float8_string;

create view v_numeric_string as select a.`numeric` % b.`char` as numeric_char, a.`numeric` % b.`varchar` as numeric_varchar, a.`numeric` % b.`binary` as numeric_binary, a.`numeric` % b.`varbinary` as numeric_varbinary, a.`numeric` % b.`text` as numeric_text from `t_number` as a, `t_str` as b order by a.id, b.id;
desc v_numeric_string;
drop view v_numeric_string;

create view v_boolean_string as select a.`boolean` % b.`char` as boolean_char, a.`boolean` % b.`varchar` as boolean_varchar, a.`boolean` % b.`binary` as boolean_binary, a.`boolean` % b.`varbinary` as boolean_varbinary, a.`boolean` % b.`text` as boolean_text from `t_number` as a, `t_str` as b order by a.id, b.id;
desc v_boolean_string;
drop view v_boolean_string;

create view v_int1_bit as select a.`int1` % b.`bit1` as int1_bit1, a.`int1` % b.`bit8` as int1_bit8, a.`int1` % b.`bit15` as int1_bit15, a.`int1` % b.`bit64` as int1_bit64 from `t_number` as a, `t_bit` as b order by a.id, b.id;
desc v_int1_bit;
drop view v_int1_bit;

create view v_uint1_bit as select a.`uint1` % b.`bit1` as uint1_bit1, a.`uint1` % b.`bit8` as uint1_bit8, a.`uint1` % b.`bit15` as uint1_bit15, a.`uint1` % b.`bit64` as uint1_bit64 from `t_number` as a, `t_bit` as b order by a.id, b.id;
desc v_uint1_bit;
drop view v_uint1_bit;

create view v_int2_bit as select a.`int2` % b.`bit1` as int2_bit1, a.`int2` % b.`bit8` as int2_bit8, a.`int2` % b.`bit15` as int2_bit15, a.`int2` % b.`bit64` as int2_bit64 from `t_number` as a, `t_bit` as b order by a.id, b.id;
desc v_int2_bit;
drop view v_int2_bit;

create view v_uint2_bit as select a.`uint2` % b.`bit1` as uint2_bit1, a.`uint2` % b.`bit8` as uint2_bit8, a.`uint2` % b.`bit15` as uint2_bit15, a.`uint2` % b.`bit64` as uint2_bit64 from `t_number` as a, `t_bit` as b order by a.id, b.id;
desc v_uint2_bit;
drop view v_uint2_bit;

create view v_int4_bit as select a.`int4` % b.`bit1` as int4_bit1, a.`int4` % b.`bit8` as int4_bit8, a.`int4` % b.`bit15` as int4_bit15, a.`int4` % b.`bit64` as int4_bit64 from `t_number` as a, `t_bit` as b order by a.id, b.id;
desc v_int4_bit;
drop view v_int4_bit;

create view v_uint4_bit as select a.`uint4` % b.`bit1` as uint4_bit1, a.`uint4` % b.`bit8` as uint4_bit8, a.`uint4` % b.`bit15` as uint4_bit15, a.`uint4` % b.`bit64` as uint4_bit64 from `t_number` as a, `t_bit` as b order by a.id, b.id;
desc v_uint4_bit;
drop view v_uint4_bit;

create view v_int8_bit as select a.`int8` % b.`bit1` as int8_bit1, a.`int8` % b.`bit8` as int8_bit8, a.`int8` % b.`bit15` as int8_bit15, a.`int8` % b.`bit64` as int8_bit64 from `t_number` as a, `t_bit` as b order by a.id, b.id;
desc v_int8_bit;
drop view v_int8_bit;

create view v_uint8_bit as select a.`uint8` % b.`bit1` as uint8_bit1, a.`uint8` % b.`bit8` as uint8_bit8, a.`uint8` % b.`bit15` as uint8_bit15, a.`uint8` % b.`bit64` as uint8_bit64 from `t_number` as a, `t_bit` as b order by a.id, b.id;
desc v_uint8_bit;
drop view v_uint8_bit;

create view v_float4_bit as select a.`float4` % b.`bit1` as float4_bit1, a.`float4` % b.`bit8` as float4_bit8, a.`float4` % b.`bit15` as float4_bit15, a.`float4` % b.`bit64` as float4_bit64 from `t_number` as a, `t_bit` as b order by a.id, b.id;
desc v_float4_bit;
drop view v_float4_bit;

create view v_float8_bit as select a.`float8` % b.`bit1` as float8_bit1, a.`float8` % b.`bit8` as float8_bit8, a.`float8` % b.`bit15` as float8_bit15, a.`float8` % b.`bit64` as float8_bit64 from `t_number` as a, `t_bit` as b order by a.id, b.id;
desc v_float8_bit;
drop view v_float8_bit;

create view v_numeric_bit as select a.`numeric` % b.`bit1` as numeric_bit1, a.`numeric` % b.`bit8` as numeric_bit8, a.`numeric` % b.`bit15` as numeric_bit15, a.`numeric` % b.`bit64` as numeric_bit64 from `t_number` as a, `t_bit` as b order by a.id, b.id;
desc v_numeric_bit;
drop view v_numeric_bit;

create view v_boolean_bit as select a.`boolean` % b.`bit1` as boolean_bit1, a.`boolean` % b.`bit8` as boolean_bit8, a.`boolean` % b.`bit15` as boolean_bit15, a.`boolean` % b.`bit64` as boolean_bit64 from `t_number` as a, `t_bit` as b order by a.id, b.id;
desc v_boolean_bit;
drop view v_boolean_bit;

create view v_char_string as select a.`char` % b.`char` as char_char, a.`char` % b.`varchar` as char_varchar, a.`char` % b.`binary` as char_binary, a.`char` % b.`varbinary` as char_varbinary, a.`char` % b.`text` as char_text from `t_str` as a, `t_str` as b order by a.id, b.id;
desc v_char_string;
drop view v_char_string;

create view v_varchar_string as select a.`varchar` % b.`char` as varchar_char, a.`varchar` % b.`varchar` as varchar_varchar, a.`varchar` % b.`binary` as varchar_binary, a.`varchar` % b.`varbinary` as varchar_varbinary, a.`varchar` % b.`text` as varchar_text from `t_str` as a, `t_str` as b order by a.id, b.id;
desc v_varchar_string;
drop view v_varchar_string;

create view v_binary_string as select a.`binary` % b.`char` as binary_char, a.`binary` % b.`varchar` as binary_varchar, a.`binary` % b.`binary` as binary_binary, a.`binary` % b.`varbinary` as binary_varbinary, a.`binary` % b.`text` as binary_text from `t_str` as a, `t_str` as b order by a.id, b.id;
desc v_binary_string;
drop view v_binary_string;

create view v_varbinary_string as select a.`varbinary` % b.`char` as varbinary_char, a.`varbinary` % b.`varchar` as varbinary_varchar, a.`varbinary` % b.`binary` as varbinary_binary, a.`varbinary` % b.`varbinary` as varbinary_varbinary, a.`varbinary` % b.`text` as varbinary_text from `t_str` as a, `t_str` as b order by a.id, b.id;
desc v_varbinary_string;
drop view v_varbinary_string;

create view v_text_string as select a.`text` % b.`char` as text_char, a.`text` % b.`varchar` as text_varchar, a.`text` % b.`binary` as text_binary, a.`text` % b.`varbinary` as text_varbinary, a.`text` % b.`text` as text_text from `t_str` as a, `t_str` as b order by a.id, b.id;
desc v_text_string;
drop view v_text_string;

create view v_char_bit as select a.`char` % b.`bit1` as char_bit1, a.`char` % b.`bit8` as char_bit8, a.`char` % b.`bit15` as char_bit15, a.`char` % b.`bit64` as char_bit64 from `t_str` as a, `t_bit` as b order by a.id, b.id;
desc v_char_bit;
drop view v_char_bit;

create view v_varchar_bit as select a.`varchar` % b.`bit1` as varchar_bit1, a.`varchar` % b.`bit8` as varchar_bit8, a.`varchar` % b.`bit15` as varchar_bit15, a.`varchar` % b.`bit64` as varchar_bit64 from `t_str` as a, `t_bit` as b order by a.id, b.id;
desc v_varchar_bit;
drop view v_varchar_bit;

create view v_binary_bit as select a.`binary` % b.`bit1` as binary_bit1, a.`binary` % b.`bit8` as binary_bit8, a.`binary` % b.`bit15` as binary_bit15, a.`binary` % b.`bit64` as binary_bit64 from `t_str` as a, `t_bit` as b order by a.id, b.id;
desc v_binary_bit;
drop view v_binary_bit;

create view v_varbinary_bit as select a.`varbinary` % b.`bit1` as varbinary_bit1, a.`varbinary` % b.`bit8` as varbinary_bit8, a.`varbinary` % b.`bit15` as varbinary_bit15, a.`varbinary` % b.`bit64` as varbinary_bit64 from `t_str` as a, `t_bit` as b order by a.id, b.id;
desc v_varbinary_bit;
drop view v_varbinary_bit;

create view v_text_bit as select a.`text` % b.`bit1` as text_bit1, a.`text` % b.`bit8` as text_bit8, a.`text` % b.`bit15` as text_bit15, a.`text` % b.`bit64` as text_bit64 from `t_str` as a, `t_bit` as b order by a.id, b.id;
desc v_text_bit;
drop view v_text_bit;

create view v_bit1_bit as select a.`bit1` % b.`bit1` as bit1_bit1, a.`bit1` % b.`bit8` as bit1_bit8, a.`bit1` % b.`bit15` as bit1_bit15, a.`bit1` % b.`bit64` as bit1_bit64 from `t_bit` as a, `t_bit` as b order by a.id, b.id;
desc v_bit1_bit;
drop view v_bit1_bit;

create view v_bit8_bit as select a.`bit8` % b.`bit1` as bit8_bit1, a.`bit8` % b.`bit8` as bit8_bit8, a.`bit8` % b.`bit15` as bit8_bit15, a.`bit8` % b.`bit64` as bit8_bit64 from `t_bit` as a, `t_bit` as b order by a.id, b.id;
desc v_bit8_bit;
drop view v_bit8_bit;

create view v_bit15_bit as select a.`bit15` % b.`bit1` as bit15_bit1, a.`bit15` % b.`bit8` as bit15_bit8, a.`bit15` % b.`bit15` as bit15_bit15, a.`bit15` % b.`bit64` as bit15_bit64 from `t_bit` as a, `t_bit` as b order by a.id, b.id;
desc v_bit15_bit;
drop view v_bit15_bit;

create view v_bit64_bit as select a.`bit64` % b.`bit1` as bit64_bit1, a.`bit64` % b.`bit8` as bit64_bit8, a.`bit64` % b.`bit15` as bit64_bit15, a.`bit64` % b.`bit64` as bit64_bit64 from `t_bit` as a, `t_bit` as b order by a.id, b.id;
desc v_bit64_bit;
drop view v_bit64_bit;

-- test % result
truncate t_res;

insert into t_res select 1, a.id, b.id, 'int1%int1', a.`int1` % b.`int1` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 2, a.id, b.id, 'int1%uint1', a.`int1` % b.`uint1` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 3, a.id, b.id, 'int1%int2', a.`int1` % b.`int2` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 4, a.id, b.id, 'int1%uint2', a.`int1` % b.`uint2` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 5, a.id, b.id, 'int1%int4', a.`int1` % b.`int4` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 6, a.id, b.id, 'int1%uint4', a.`int1` % b.`uint4` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 7, a.id, b.id, 'int1%int8', a.`int1` % b.`int8` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 8, a.id, b.id, 'int1%uint8', a.`int1` % b.`uint8` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 9, a.id, b.id, 'int1%float4', a.`int1` % b.`float4` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 10, a.id, b.id, 'int1%float8', a.`int1` % b.`float8` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 11, a.id, b.id, 'int1%numeric', a.`int1` % b.`numeric` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 12, a.id, b.id, 'int1%boolean', a.`int1` % b.`boolean` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 13, a.id, b.id, 'uint1%int1', a.`uint1` % b.`int1` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 14, a.id, b.id, 'uint1%uint1', a.`uint1` % b.`uint1` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 15, a.id, b.id, 'uint1%int2', a.`uint1` % b.`int2` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 16, a.id, b.id, 'uint1%uint2', a.`uint1` % b.`uint2` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 17, a.id, b.id, 'uint1%int4', a.`uint1` % b.`int4` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 18, a.id, b.id, 'uint1%uint4', a.`uint1` % b.`uint4` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 19, a.id, b.id, 'uint1%int8', a.`uint1` % b.`int8` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 20, a.id, b.id, 'uint1%uint8', a.`uint1` % b.`uint8` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 21, a.id, b.id, 'uint1%float4', a.`uint1` % b.`float4` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 22, a.id, b.id, 'uint1%float8', a.`uint1` % b.`float8` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 23, a.id, b.id, 'uint1%numeric', a.`uint1` % b.`numeric` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 24, a.id, b.id, 'uint1%boolean', a.`uint1` % b.`boolean` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 25, a.id, b.id, 'int2%int1', a.`int2` % b.`int1` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 26, a.id, b.id, 'int2%uint1', a.`int2` % b.`uint1` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 27, a.id, b.id, 'int2%int2', a.`int2` % b.`int2` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 28, a.id, b.id, 'int2%uint2', a.`int2` % b.`uint2` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 29, a.id, b.id, 'int2%int4', a.`int2` % b.`int4` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 30, a.id, b.id, 'int2%uint4', a.`int2` % b.`uint4` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 31, a.id, b.id, 'int2%int8', a.`int2` % b.`int8` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 32, a.id, b.id, 'int2%uint8', a.`int2` % b.`uint8` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 33, a.id, b.id, 'int2%float4', a.`int2` % b.`float4` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 34, a.id, b.id, 'int2%float8', a.`int2` % b.`float8` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 35, a.id, b.id, 'int2%numeric', a.`int2` % b.`numeric` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 36, a.id, b.id, 'int2%boolean', a.`int2` % b.`boolean` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 37, a.id, b.id, 'uint2%int1', a.`uint2` % b.`int1` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 38, a.id, b.id, 'uint2%uint1', a.`uint2` % b.`uint1` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 39, a.id, b.id, 'uint2%int2', a.`uint2` % b.`int2` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 40, a.id, b.id, 'uint2%uint2', a.`uint2` % b.`uint2` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 41, a.id, b.id, 'uint2%int4', a.`uint2` % b.`int4` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 42, a.id, b.id, 'uint2%uint4', a.`uint2` % b.`uint4` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 43, a.id, b.id, 'uint2%int8', a.`uint2` % b.`int8` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 44, a.id, b.id, 'uint2%uint8', a.`uint2` % b.`uint8` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 45, a.id, b.id, 'uint2%float4', a.`uint2` % b.`float4` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 46, a.id, b.id, 'uint2%float8', a.`uint2` % b.`float8` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 47, a.id, b.id, 'uint2%numeric', a.`uint2` % b.`numeric` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 48, a.id, b.id, 'uint2%boolean', a.`uint2` % b.`boolean` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 49, a.id, b.id, 'int4%int1', a.`int4` % b.`int1` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 50, a.id, b.id, 'int4%uint1', a.`int4` % b.`uint1` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 51, a.id, b.id, 'int4%int2', a.`int4` % b.`int2` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 52, a.id, b.id, 'int4%uint2', a.`int4` % b.`uint2` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 53, a.id, b.id, 'int4%int4', a.`int4` % b.`int4` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 54, a.id, b.id, 'int4%uint4', a.`int4` % b.`uint4` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 55, a.id, b.id, 'int4%int8', a.`int4` % b.`int8` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 56, a.id, b.id, 'int4%uint8', a.`int4` % b.`uint8` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 57, a.id, b.id, 'int4%float4', a.`int4` % b.`float4` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 58, a.id, b.id, 'int4%float8', a.`int4` % b.`float8` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 59, a.id, b.id, 'int4%numeric', a.`int4` % b.`numeric` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 60, a.id, b.id, 'int4%boolean', a.`int4` % b.`boolean` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 61, a.id, b.id, 'uint4%int1', a.`uint4` % b.`int1` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 62, a.id, b.id, 'uint4%uint1', a.`uint4` % b.`uint1` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 63, a.id, b.id, 'uint4%int2', a.`uint4` % b.`int2` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 64, a.id, b.id, 'uint4%uint2', a.`uint4` % b.`uint2` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 65, a.id, b.id, 'uint4%int4', a.`uint4` % b.`int4` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 66, a.id, b.id, 'uint4%uint4', a.`uint4` % b.`uint4` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 67, a.id, b.id, 'uint4%int8', a.`uint4` % b.`int8` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 68, a.id, b.id, 'uint4%uint8', a.`uint4` % b.`uint8` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 69, a.id, b.id, 'uint4%float4', a.`uint4` % b.`float4` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 70, a.id, b.id, 'uint4%float8', a.`uint4` % b.`float8` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 71, a.id, b.id, 'uint4%numeric', a.`uint4` % b.`numeric` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 72, a.id, b.id, 'uint4%boolean', a.`uint4` % b.`boolean` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 73, a.id, b.id, 'int8%int1', a.`int8` % b.`int1` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 74, a.id, b.id, 'int8%uint1', a.`int8` % b.`uint1` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 75, a.id, b.id, 'int8%int2', a.`int8` % b.`int2` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 76, a.id, b.id, 'int8%uint2', a.`int8` % b.`uint2` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 77, a.id, b.id, 'int8%int4', a.`int8` % b.`int4` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 78, a.id, b.id, 'int8%uint4', a.`int8` % b.`uint4` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 79, a.id, b.id, 'int8%int8', a.`int8` % b.`int8` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 80, a.id, b.id, 'int8%uint8', a.`int8` % b.`uint8` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 81, a.id, b.id, 'int8%float4', a.`int8` % b.`float4` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 82, a.id, b.id, 'int8%float8', a.`int8` % b.`float8` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 83, a.id, b.id, 'int8%numeric', a.`int8` % b.`numeric` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 84, a.id, b.id, 'int8%boolean', a.`int8` % b.`boolean` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 85, a.id, b.id, 'uint8%int1', a.`uint8` % b.`int1` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 86, a.id, b.id, 'uint8%uint1', a.`uint8` % b.`uint1` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 87, a.id, b.id, 'uint8%int2', a.`uint8` % b.`int2` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 88, a.id, b.id, 'uint8%uint2', a.`uint8` % b.`uint2` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 89, a.id, b.id, 'uint8%int4', a.`uint8` % b.`int4` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 90, a.id, b.id, 'uint8%uint4', a.`uint8` % b.`uint4` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 91, a.id, b.id, 'uint8%int8', a.`uint8` % b.`int8` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 92, a.id, b.id, 'uint8%uint8', a.`uint8` % b.`uint8` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 93, a.id, b.id, 'uint8%float4', a.`uint8` % b.`float4` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 94, a.id, b.id, 'uint8%float8', a.`uint8` % b.`float8` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 95, a.id, b.id, 'uint8%numeric', a.`uint8` % b.`numeric` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 96, a.id, b.id, 'uint8%boolean', a.`uint8` % b.`boolean` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 97, a.id, b.id, 'float4%int1', a.`float4` % b.`int1` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 98, a.id, b.id, 'float4%uint1', a.`float4` % b.`uint1` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 99, a.id, b.id, 'float4%int2', a.`float4` % b.`int2` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 100, a.id, b.id, 'float4%uint2', a.`float4` % b.`uint2` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 101, a.id, b.id, 'float4%int4', a.`float4` % b.`int4` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 102, a.id, b.id, 'float4%uint4', a.`float4` % b.`uint4` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 103, a.id, b.id, 'float4%int8', a.`float4` % b.`int8` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 104, a.id, b.id, 'float4%uint8', a.`float4` % b.`uint8` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 105, a.id, b.id, 'float4%float4', a.`float4` % b.`float4` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 106, a.id, b.id, 'float4%float8', a.`float4` % b.`float8` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 107, a.id, b.id, 'float4%numeric', a.`float4` % b.`numeric` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 108, a.id, b.id, 'float4%boolean', a.`float4` % b.`boolean` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 109, a.id, b.id, 'float8%int1', a.`float8` % b.`int1` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 110, a.id, b.id, 'float8%uint1', a.`float8` % b.`uint1` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 111, a.id, b.id, 'float8%int2', a.`float8` % b.`int2` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 112, a.id, b.id, 'float8%uint2', a.`float8` % b.`uint2` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 113, a.id, b.id, 'float8%int4', a.`float8` % b.`int4` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 114, a.id, b.id, 'float8%uint4', a.`float8` % b.`uint4` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 115, a.id, b.id, 'float8%int8', a.`float8` % b.`int8` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 116, a.id, b.id, 'float8%uint8', a.`float8` % b.`uint8` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 117, a.id, b.id, 'float8%float4', a.`float8` % b.`float4` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 118, a.id, b.id, 'float8%float8', a.`float8` % b.`float8` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 119, a.id, b.id, 'float8%numeric', a.`float8` % b.`numeric` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 120, a.id, b.id, 'float8%boolean', a.`float8` % b.`boolean` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 121, a.id, b.id, 'numeric%int1', a.`numeric` % b.`int1` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 122, a.id, b.id, 'numeric%uint1', a.`numeric` % b.`uint1` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 123, a.id, b.id, 'numeric%int2', a.`numeric` % b.`int2` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 124, a.id, b.id, 'numeric%uint2', a.`numeric` % b.`uint2` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 125, a.id, b.id, 'numeric%int4', a.`numeric` % b.`int4` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 126, a.id, b.id, 'numeric%uint4', a.`numeric` % b.`uint4` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 127, a.id, b.id, 'numeric%int8', a.`numeric` % b.`int8` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 128, a.id, b.id, 'numeric%uint8', a.`numeric` % b.`uint8` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 129, a.id, b.id, 'numeric%float4', a.`numeric` % b.`float4` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 130, a.id, b.id, 'numeric%float8', a.`numeric` % b.`float8` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 131, a.id, b.id, 'numeric%numeric', a.`numeric` % b.`numeric` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 132, a.id, b.id, 'numeric%boolean', a.`numeric` % b.`boolean` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 133, a.id, b.id, 'boolean%int1', a.`boolean` % b.`int1` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 134, a.id, b.id, 'boolean%uint1', a.`boolean` % b.`uint1` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 135, a.id, b.id, 'boolean%int2', a.`boolean` % b.`int2` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 136, a.id, b.id, 'boolean%uint2', a.`boolean` % b.`uint2` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 137, a.id, b.id, 'boolean%int4', a.`boolean` % b.`int4` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 138, a.id, b.id, 'boolean%uint4', a.`boolean` % b.`uint4` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 139, a.id, b.id, 'boolean%int8', a.`boolean` % b.`int8` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 140, a.id, b.id, 'boolean%uint8', a.`boolean` % b.`uint8` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 141, a.id, b.id, 'boolean%float4', a.`boolean` % b.`float4` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 142, a.id, b.id, 'boolean%float8', a.`boolean` % b.`float8` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 143, a.id, b.id, 'boolean%numeric', a.`boolean` % b.`numeric` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 144, a.id, b.id, 'boolean%boolean', a.`boolean` % b.`boolean` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 145, a.id, b.id, 'int1%char', a.`int1` % b.`char` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 146, a.id, b.id, 'int1%varchar', a.`int1` % b.`varchar` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 147, a.id, b.id, 'int1%binary', a.`int1` % b.`binary` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 148, a.id, b.id, 'int1%varbinary', a.`int1` % b.`varbinary` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 149, a.id, b.id, 'int1%text', a.`int1` % b.`text` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 150, a.id, b.id, 'uint1%char', a.`uint1` % b.`char` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 151, a.id, b.id, 'uint1%varchar', a.`uint1` % b.`varchar` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 152, a.id, b.id, 'uint1%binary', a.`uint1` % b.`binary` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 153, a.id, b.id, 'uint1%varbinary', a.`uint1` % b.`varbinary` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 154, a.id, b.id, 'uint1%text', a.`uint1` % b.`text` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 155, a.id, b.id, 'int2%char', a.`int2` % b.`char` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 156, a.id, b.id, 'int2%varchar', a.`int2` % b.`varchar` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 157, a.id, b.id, 'int2%binary', a.`int2` % b.`binary` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 158, a.id, b.id, 'int2%varbinary', a.`int2` % b.`varbinary` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 159, a.id, b.id, 'int2%text', a.`int2` % b.`text` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 160, a.id, b.id, 'uint2%char', a.`uint2` % b.`char` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 161, a.id, b.id, 'uint2%varchar', a.`uint2` % b.`varchar` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 162, a.id, b.id, 'uint2%binary', a.`uint2` % b.`binary` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 163, a.id, b.id, 'uint2%varbinary', a.`uint2` % b.`varbinary` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 164, a.id, b.id, 'uint2%text', a.`uint2` % b.`text` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 165, a.id, b.id, 'int4%char', a.`int4` % b.`char` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 166, a.id, b.id, 'int4%varchar', a.`int4` % b.`varchar` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 167, a.id, b.id, 'int4%binary', a.`int4` % b.`binary` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 168, a.id, b.id, 'int4%varbinary', a.`int4` % b.`varbinary` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 169, a.id, b.id, 'int4%text', a.`int4` % b.`text` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 170, a.id, b.id, 'uint4%char', a.`uint4` % b.`char` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 171, a.id, b.id, 'uint4%varchar', a.`uint4` % b.`varchar` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 172, a.id, b.id, 'uint4%binary', a.`uint4` % b.`binary` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 173, a.id, b.id, 'uint4%varbinary', a.`uint4` % b.`varbinary` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 174, a.id, b.id, 'uint4%text', a.`uint4` % b.`text` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 175, a.id, b.id, 'int8%char', a.`int8` % b.`char` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 176, a.id, b.id, 'int8%varchar', a.`int8` % b.`varchar` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 177, a.id, b.id, 'int8%binary', a.`int8` % b.`binary` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 178, a.id, b.id, 'int8%varbinary', a.`int8` % b.`varbinary` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 179, a.id, b.id, 'int8%text', a.`int8` % b.`text` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 180, a.id, b.id, 'uint8%char', a.`uint8` % b.`char` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 181, a.id, b.id, 'uint8%varchar', a.`uint8` % b.`varchar` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 182, a.id, b.id, 'uint8%binary', a.`uint8` % b.`binary` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 183, a.id, b.id, 'uint8%varbinary', a.`uint8` % b.`varbinary` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 184, a.id, b.id, 'uint8%text', a.`uint8` % b.`text` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 185, a.id, b.id, 'float4%char', a.`float4` % b.`char` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 186, a.id, b.id, 'float4%varchar', a.`float4` % b.`varchar` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 187, a.id, b.id, 'float4%binary', a.`float4` % b.`binary` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 188, a.id, b.id, 'float4%varbinary', a.`float4` % b.`varbinary` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 189, a.id, b.id, 'float4%text', a.`float4` % b.`text` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 190, a.id, b.id, 'float8%char', a.`float8` % b.`char` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 191, a.id, b.id, 'float8%varchar', a.`float8` % b.`varchar` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 192, a.id, b.id, 'float8%binary', a.`float8` % b.`binary` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 193, a.id, b.id, 'float8%varbinary', a.`float8` % b.`varbinary` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 194, a.id, b.id, 'float8%text', a.`float8` % b.`text` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 195, a.id, b.id, 'numeric%char', a.`numeric` % b.`char` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 196, a.id, b.id, 'numeric%varchar', a.`numeric` % b.`varchar` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 197, a.id, b.id, 'numeric%binary', a.`numeric` % b.`binary` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 198, a.id, b.id, 'numeric%varbinary', a.`numeric` % b.`varbinary` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 199, a.id, b.id, 'numeric%text', a.`numeric` % b.`text` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 200, a.id, b.id, 'boolean%char', a.`boolean` % b.`char` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 201, a.id, b.id, 'boolean%varchar', a.`boolean` % b.`varchar` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 202, a.id, b.id, 'boolean%binary', a.`boolean` % b.`binary` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 203, a.id, b.id, 'boolean%varbinary', a.`boolean` % b.`varbinary` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 204, a.id, b.id, 'boolean%text', a.`boolean` % b.`text` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 205, a.id, b.id, 'int1%bit1', a.`int1` % b.`bit1` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 206, a.id, b.id, 'int1%bit8', a.`int1` % b.`bit8` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 207, a.id, b.id, 'int1%bit15', a.`int1` % b.`bit15` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 208, a.id, b.id, 'int1%bit64', a.`int1` % b.`bit64` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 209, a.id, b.id, 'uint1%bit1', a.`uint1` % b.`bit1` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 210, a.id, b.id, 'uint1%bit8', a.`uint1` % b.`bit8` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 211, a.id, b.id, 'uint1%bit15', a.`uint1` % b.`bit15` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 212, a.id, b.id, 'uint1%bit64', a.`uint1` % b.`bit64` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 213, a.id, b.id, 'int2%bit1', a.`int2` % b.`bit1` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 214, a.id, b.id, 'int2%bit8', a.`int2` % b.`bit8` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 215, a.id, b.id, 'int2%bit15', a.`int2` % b.`bit15` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 216, a.id, b.id, 'int2%bit64', a.`int2` % b.`bit64` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 217, a.id, b.id, 'uint2%bit1', a.`uint2` % b.`bit1` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 218, a.id, b.id, 'uint2%bit8', a.`uint2` % b.`bit8` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 219, a.id, b.id, 'uint2%bit15', a.`uint2` % b.`bit15` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 220, a.id, b.id, 'uint2%bit64', a.`uint2` % b.`bit64` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 221, a.id, b.id, 'int4%bit1', a.`int4` % b.`bit1` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 222, a.id, b.id, 'int4%bit8', a.`int4` % b.`bit8` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 223, a.id, b.id, 'int4%bit15', a.`int4` % b.`bit15` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 224, a.id, b.id, 'int4%bit64', a.`int4` % b.`bit64` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 225, a.id, b.id, 'uint4%bit1', a.`uint4` % b.`bit1` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 226, a.id, b.id, 'uint4%bit8', a.`uint4` % b.`bit8` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 227, a.id, b.id, 'uint4%bit15', a.`uint4` % b.`bit15` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 228, a.id, b.id, 'uint4%bit64', a.`uint4` % b.`bit64` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 229, a.id, b.id, 'int8%bit1', a.`int8` % b.`bit1` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 230, a.id, b.id, 'int8%bit8', a.`int8` % b.`bit8` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 231, a.id, b.id, 'int8%bit15', a.`int8` % b.`bit15` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 232, a.id, b.id, 'int8%bit64', a.`int8` % b.`bit64` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 233, a.id, b.id, 'uint8%bit1', a.`uint8` % b.`bit1` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 234, a.id, b.id, 'uint8%bit8', a.`uint8` % b.`bit8` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 235, a.id, b.id, 'uint8%bit15', a.`uint8` % b.`bit15` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 236, a.id, b.id, 'uint8%bit64', a.`uint8` % b.`bit64` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 237, a.id, b.id, 'float4%bit1', a.`float4` % b.`bit1` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 238, a.id, b.id, 'float4%bit8', a.`float4` % b.`bit8` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 239, a.id, b.id, 'float4%bit15', a.`float4` % b.`bit15` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 240, a.id, b.id, 'float4%bit64', a.`float4` % b.`bit64` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 241, a.id, b.id, 'float8%bit1', a.`float8` % b.`bit1` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 242, a.id, b.id, 'float8%bit8', a.`float8` % b.`bit8` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 243, a.id, b.id, 'float8%bit15', a.`float8` % b.`bit15` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 244, a.id, b.id, 'float8%bit64', a.`float8` % b.`bit64` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 245, a.id, b.id, 'numeric%bit1', a.`numeric` % b.`bit1` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 246, a.id, b.id, 'numeric%bit8', a.`numeric` % b.`bit8` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 247, a.id, b.id, 'numeric%bit15', a.`numeric` % b.`bit15` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 248, a.id, b.id, 'numeric%bit64', a.`numeric` % b.`bit64` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 249, a.id, b.id, 'boolean%bit1', a.`boolean` % b.`bit1` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 250, a.id, b.id, 'boolean%bit8', a.`boolean` % b.`bit8` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 251, a.id, b.id, 'boolean%bit15', a.`boolean` % b.`bit15` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 252, a.id, b.id, 'boolean%bit64', a.`boolean` % b.`bit64` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 253, a.id, b.id, 'char%char', a.`char` % b.`char` from t_str as a, t_str as b order by a.id, b.id;
insert into t_res select 254, a.id, b.id, 'char%varchar', a.`char` % b.`varchar` from t_str as a, t_str as b order by a.id, b.id;
insert into t_res select 255, a.id, b.id, 'char%binary', a.`char` % b.`binary` from t_str as a, t_str as b order by a.id, b.id;
insert into t_res select 256, a.id, b.id, 'char%varbinary', a.`char` % b.`varbinary` from t_str as a, t_str as b order by a.id, b.id;
insert into t_res select 257, a.id, b.id, 'char%text', a.`char` % b.`text` from t_str as a, t_str as b order by a.id, b.id;
insert into t_res select 258, a.id, b.id, 'varchar%char', a.`varchar` % b.`char` from t_str as a, t_str as b order by a.id, b.id;
insert into t_res select 259, a.id, b.id, 'varchar%varchar', a.`varchar` % b.`varchar` from t_str as a, t_str as b order by a.id, b.id;
insert into t_res select 260, a.id, b.id, 'varchar%binary', a.`varchar` % b.`binary` from t_str as a, t_str as b order by a.id, b.id;
insert into t_res select 261, a.id, b.id, 'varchar%varbinary', a.`varchar` % b.`varbinary` from t_str as a, t_str as b order by a.id, b.id;
insert into t_res select 262, a.id, b.id, 'varchar%text', a.`varchar` % b.`text` from t_str as a, t_str as b order by a.id, b.id;
insert into t_res select 263, a.id, b.id, 'binary%char', a.`binary` % b.`char` from t_str as a, t_str as b order by a.id, b.id;
insert into t_res select 264, a.id, b.id, 'binary%varchar', a.`binary` % b.`varchar` from t_str as a, t_str as b order by a.id, b.id;
insert into t_res select 265, a.id, b.id, 'binary%binary', a.`binary` % b.`binary` from t_str as a, t_str as b order by a.id, b.id;
insert into t_res select 266, a.id, b.id, 'binary%varbinary', a.`binary` % b.`varbinary` from t_str as a, t_str as b order by a.id, b.id;
insert into t_res select 267, a.id, b.id, 'binary%text', a.`binary` % b.`text` from t_str as a, t_str as b order by a.id, b.id;
insert into t_res select 268, a.id, b.id, 'varbinary%char', a.`varbinary` % b.`char` from t_str as a, t_str as b order by a.id, b.id;
insert into t_res select 269, a.id, b.id, 'varbinary%varchar', a.`varbinary` % b.`varchar` from t_str as a, t_str as b order by a.id, b.id;
insert into t_res select 270, a.id, b.id, 'varbinary%binary', a.`varbinary` % b.`binary` from t_str as a, t_str as b order by a.id, b.id;
insert into t_res select 271, a.id, b.id, 'varbinary%varbinary', a.`varbinary` % b.`varbinary` from t_str as a, t_str as b order by a.id, b.id;
insert into t_res select 272, a.id, b.id, 'varbinary%text', a.`varbinary` % b.`text` from t_str as a, t_str as b order by a.id, b.id;
insert into t_res select 273, a.id, b.id, 'text%char', a.`text` % b.`char` from t_str as a, t_str as b order by a.id, b.id;
insert into t_res select 274, a.id, b.id, 'text%varchar', a.`text` % b.`varchar` from t_str as a, t_str as b order by a.id, b.id;
insert into t_res select 275, a.id, b.id, 'text%binary', a.`text` % b.`binary` from t_str as a, t_str as b order by a.id, b.id;
insert into t_res select 276, a.id, b.id, 'text%varbinary', a.`text` % b.`varbinary` from t_str as a, t_str as b order by a.id, b.id;
insert into t_res select 277, a.id, b.id, 'text%text', a.`text` % b.`text` from t_str as a, t_str as b order by a.id, b.id;
insert into t_res select 278, a.id, b.id, 'char%bit1', a.`char` % b.`bit1` from t_str as a, t_bit as b order by a.id, b.id;
insert into t_res select 279, a.id, b.id, 'char%bit8', a.`char` % b.`bit8` from t_str as a, t_bit as b order by a.id, b.id;
insert into t_res select 280, a.id, b.id, 'char%bit15', a.`char` % b.`bit15` from t_str as a, t_bit as b order by a.id, b.id;
insert into t_res select 281, a.id, b.id, 'char%bit64', a.`char` % b.`bit64` from t_str as a, t_bit as b order by a.id, b.id;
insert into t_res select 282, a.id, b.id, 'varchar%bit1', a.`varchar` % b.`bit1` from t_str as a, t_bit as b order by a.id, b.id;
insert into t_res select 283, a.id, b.id, 'varchar%bit8', a.`varchar` % b.`bit8` from t_str as a, t_bit as b order by a.id, b.id;
insert into t_res select 284, a.id, b.id, 'varchar%bit15', a.`varchar` % b.`bit15` from t_str as a, t_bit as b order by a.id, b.id;
insert into t_res select 285, a.id, b.id, 'varchar%bit64', a.`varchar` % b.`bit64` from t_str as a, t_bit as b order by a.id, b.id;
insert into t_res select 286, a.id, b.id, 'binary%bit1', a.`binary` % b.`bit1` from t_str as a, t_bit as b order by a.id, b.id;
insert into t_res select 287, a.id, b.id, 'binary%bit8', a.`binary` % b.`bit8` from t_str as a, t_bit as b order by a.id, b.id;
insert into t_res select 288, a.id, b.id, 'binary%bit15', a.`binary` % b.`bit15` from t_str as a, t_bit as b order by a.id, b.id;
insert into t_res select 289, a.id, b.id, 'binary%bit64', a.`binary` % b.`bit64` from t_str as a, t_bit as b order by a.id, b.id;
insert into t_res select 290, a.id, b.id, 'varbinary%bit1', a.`varbinary` % b.`bit1` from t_str as a, t_bit as b order by a.id, b.id;
insert into t_res select 291, a.id, b.id, 'varbinary%bit8', a.`varbinary` % b.`bit8` from t_str as a, t_bit as b order by a.id, b.id;
insert into t_res select 292, a.id, b.id, 'varbinary%bit15', a.`varbinary` % b.`bit15` from t_str as a, t_bit as b order by a.id, b.id;
insert into t_res select 293, a.id, b.id, 'varbinary%bit64', a.`varbinary` % b.`bit64` from t_str as a, t_bit as b order by a.id, b.id;
insert into t_res select 294, a.id, b.id, 'text%bit1', a.`text` % b.`bit1` from t_str as a, t_bit as b order by a.id, b.id;
insert into t_res select 295, a.id, b.id, 'text%bit8', a.`text` % b.`bit8` from t_str as a, t_bit as b order by a.id, b.id;
insert into t_res select 296, a.id, b.id, 'text%bit15', a.`text` % b.`bit15` from t_str as a, t_bit as b order by a.id, b.id;
insert into t_res select 297, a.id, b.id, 'text%bit64', a.`text` % b.`bit64` from t_str as a, t_bit as b order by a.id, b.id;
insert into t_res select 298, a.id, b.id, 'bit1%bit1', a.`bit1` % b.`bit1` from t_bit as a, t_bit as b order by a.id, b.id;
insert into t_res select 299, a.id, b.id, 'bit1%bit8', a.`bit1` % b.`bit8` from t_bit as a, t_bit as b order by a.id, b.id;
insert into t_res select 300, a.id, b.id, 'bit1%bit15', a.`bit1` % b.`bit15` from t_bit as a, t_bit as b order by a.id, b.id;
insert into t_res select 301, a.id, b.id, 'bit1%bit64', a.`bit1` % b.`bit64` from t_bit as a, t_bit as b order by a.id, b.id;
insert into t_res select 302, a.id, b.id, 'bit8%bit1', a.`bit8` % b.`bit1` from t_bit as a, t_bit as b order by a.id, b.id;
insert into t_res select 303, a.id, b.id, 'bit8%bit8', a.`bit8` % b.`bit8` from t_bit as a, t_bit as b order by a.id, b.id;
insert into t_res select 304, a.id, b.id, 'bit8%bit15', a.`bit8` % b.`bit15` from t_bit as a, t_bit as b order by a.id, b.id;
insert into t_res select 305, a.id, b.id, 'bit8%bit64', a.`bit8` % b.`bit64` from t_bit as a, t_bit as b order by a.id, b.id;
insert into t_res select 306, a.id, b.id, 'bit15%bit1', a.`bit15` % b.`bit1` from t_bit as a, t_bit as b order by a.id, b.id;
insert into t_res select 307, a.id, b.id, 'bit15%bit8', a.`bit15` % b.`bit8` from t_bit as a, t_bit as b order by a.id, b.id;
insert into t_res select 308, a.id, b.id, 'bit15%bit15', a.`bit15` % b.`bit15` from t_bit as a, t_bit as b order by a.id, b.id;
insert into t_res select 309, a.id, b.id, 'bit15%bit64', a.`bit15` % b.`bit64` from t_bit as a, t_bit as b order by a.id, b.id;
insert into t_res select 310, a.id, b.id, 'bit64%bit1', a.`bit64` % b.`bit1` from t_bit as a, t_bit as b order by a.id, b.id;
insert into t_res select 311, a.id, b.id, 'bit64%bit8', a.`bit64` % b.`bit8` from t_bit as a, t_bit as b order by a.id, b.id;
insert into t_res select 312, a.id, b.id, 'bit64%bit15', a.`bit64` % b.`bit15` from t_bit as a, t_bit as b order by a.id, b.id;
insert into t_res select 313, a.id, b.id, 'bit64%bit64', a.`bit64` % b.`bit64` from t_bit as a, t_bit as b order by a.id, b.id;

select * from t_res order by id, a_id, b_id;


-- test MOD result_type
create view v_int1_number as select a.`int1` MOD b.`int1` as int1_int1, a.`int1` MOD b.`uint1` as int1_uint1, a.`int1` MOD b.`int2` as int1_int2, a.`int1` MOD b.`uint2` as int1_uint2, a.`int1` MOD b.`int4` as int1_int4, a.`int1` MOD b.`uint4` as int1_uint4, a.`int1` MOD b.`int8` as int1_int8, a.`int1` MOD b.`uint8` as int1_uint8, a.`int1` MOD b.`float4` as int1_float4, a.`int1` MOD b.`float8` as int1_float8, a.`int1` MOD b.`numeric` as int1_numeric, a.`int1` MOD b.`boolean` as int1_boolean from `t_number` as a, `t_number` as b order by a.id, b.id;
desc v_int1_number;
drop view v_int1_number;

create view v_uint1_number as select a.`uint1` MOD b.`int1` as uint1_int1, a.`uint1` MOD b.`uint1` as uint1_uint1, a.`uint1` MOD b.`int2` as uint1_int2, a.`uint1` MOD b.`uint2` as uint1_uint2, a.`uint1` MOD b.`int4` as uint1_int4, a.`uint1` MOD b.`uint4` as uint1_uint4, a.`uint1` MOD b.`int8` as uint1_int8, a.`uint1` MOD b.`uint8` as uint1_uint8, a.`uint1` MOD b.`float4` as uint1_float4, a.`uint1` MOD b.`float8` as uint1_float8, a.`uint1` MOD b.`numeric` as uint1_numeric, a.`uint1` MOD b.`boolean` as uint1_boolean from `t_number` as a, `t_number` as b order by a.id, b.id;
desc v_uint1_number;
drop view v_uint1_number;

create view v_int2_number as select a.`int2` MOD b.`int1` as int2_int1, a.`int2` MOD b.`uint1` as int2_uint1, a.`int2` MOD b.`int2` as int2_int2, a.`int2` MOD b.`uint2` as int2_uint2, a.`int2` MOD b.`int4` as int2_int4, a.`int2` MOD b.`uint4` as int2_uint4, a.`int2` MOD b.`int8` as int2_int8, a.`int2` MOD b.`uint8` as int2_uint8, a.`int2` MOD b.`float4` as int2_float4, a.`int2` MOD b.`float8` as int2_float8, a.`int2` MOD b.`numeric` as int2_numeric, a.`int2` MOD b.`boolean` as int2_boolean from `t_number` as a, `t_number` as b order by a.id, b.id;
desc v_int2_number;
drop view v_int2_number;

create view v_uint2_number as select a.`uint2` MOD b.`int1` as uint2_int1, a.`uint2` MOD b.`uint1` as uint2_uint1, a.`uint2` MOD b.`int2` as uint2_int2, a.`uint2` MOD b.`uint2` as uint2_uint2, a.`uint2` MOD b.`int4` as uint2_int4, a.`uint2` MOD b.`uint4` as uint2_uint4, a.`uint2` MOD b.`int8` as uint2_int8, a.`uint2` MOD b.`uint8` as uint2_uint8, a.`uint2` MOD b.`float4` as uint2_float4, a.`uint2` MOD b.`float8` as uint2_float8, a.`uint2` MOD b.`numeric` as uint2_numeric, a.`uint2` MOD b.`boolean` as uint2_boolean from `t_number` as a, `t_number` as b order by a.id, b.id;
desc v_uint2_number;
drop view v_uint2_number;

create view v_int4_number as select a.`int4` MOD b.`int1` as int4_int1, a.`int4` MOD b.`uint1` as int4_uint1, a.`int4` MOD b.`int2` as int4_int2, a.`int4` MOD b.`uint2` as int4_uint2, a.`int4` MOD b.`int4` as int4_int4, a.`int4` MOD b.`uint4` as int4_uint4, a.`int4` MOD b.`int8` as int4_int8, a.`int4` MOD b.`uint8` as int4_uint8, a.`int4` MOD b.`float4` as int4_float4, a.`int4` MOD b.`float8` as int4_float8, a.`int4` MOD b.`numeric` as int4_numeric, a.`int4` MOD b.`boolean` as int4_boolean from `t_number` as a, `t_number` as b order by a.id, b.id;
desc v_int4_number;
drop view v_int4_number;

create view v_uint4_number as select a.`uint4` MOD b.`int1` as uint4_int1, a.`uint4` MOD b.`uint1` as uint4_uint1, a.`uint4` MOD b.`int2` as uint4_int2, a.`uint4` MOD b.`uint2` as uint4_uint2, a.`uint4` MOD b.`int4` as uint4_int4, a.`uint4` MOD b.`uint4` as uint4_uint4, a.`uint4` MOD b.`int8` as uint4_int8, a.`uint4` MOD b.`uint8` as uint4_uint8, a.`uint4` MOD b.`float4` as uint4_float4, a.`uint4` MOD b.`float8` as uint4_float8, a.`uint4` MOD b.`numeric` as uint4_numeric, a.`uint4` MOD b.`boolean` as uint4_boolean from `t_number` as a, `t_number` as b order by a.id, b.id;
desc v_uint4_number;
drop view v_uint4_number;

create view v_int8_number as select a.`int8` MOD b.`int1` as int8_int1, a.`int8` MOD b.`uint1` as int8_uint1, a.`int8` MOD b.`int2` as int8_int2, a.`int8` MOD b.`uint2` as int8_uint2, a.`int8` MOD b.`int4` as int8_int4, a.`int8` MOD b.`uint4` as int8_uint4, a.`int8` MOD b.`int8` as int8_int8, a.`int8` MOD b.`uint8` as int8_uint8, a.`int8` MOD b.`float4` as int8_float4, a.`int8` MOD b.`float8` as int8_float8, a.`int8` MOD b.`numeric` as int8_numeric, a.`int8` MOD b.`boolean` as int8_boolean from `t_number` as a, `t_number` as b order by a.id, b.id;
desc v_int8_number;
drop view v_int8_number;

create view v_uint8_number as select a.`uint8` MOD b.`int1` as uint8_int1, a.`uint8` MOD b.`uint1` as uint8_uint1, a.`uint8` MOD b.`int2` as uint8_int2, a.`uint8` MOD b.`uint2` as uint8_uint2, a.`uint8` MOD b.`int4` as uint8_int4, a.`uint8` MOD b.`uint4` as uint8_uint4, a.`uint8` MOD b.`int8` as uint8_int8, a.`uint8` MOD b.`uint8` as uint8_uint8, a.`uint8` MOD b.`float4` as uint8_float4, a.`uint8` MOD b.`float8` as uint8_float8, a.`uint8` MOD b.`numeric` as uint8_numeric, a.`uint8` MOD b.`boolean` as uint8_boolean from `t_number` as a, `t_number` as b order by a.id, b.id;
desc v_uint8_number;
drop view v_uint8_number;

create view v_float4_number as select a.`float4` MOD b.`int1` as float4_int1, a.`float4` MOD b.`uint1` as float4_uint1, a.`float4` MOD b.`int2` as float4_int2, a.`float4` MOD b.`uint2` as float4_uint2, a.`float4` MOD b.`int4` as float4_int4, a.`float4` MOD b.`uint4` as float4_uint4, a.`float4` MOD b.`int8` as float4_int8, a.`float4` MOD b.`uint8` as float4_uint8, a.`float4` MOD b.`float4` as float4_float4, a.`float4` MOD b.`float8` as float4_float8, a.`float4` MOD b.`numeric` as float4_numeric, a.`float4` MOD b.`boolean` as float4_boolean from `t_number` as a, `t_number` as b order by a.id, b.id;
desc v_float4_number;
drop view v_float4_number;

create view v_float8_number as select a.`float8` MOD b.`int1` as float8_int1, a.`float8` MOD b.`uint1` as float8_uint1, a.`float8` MOD b.`int2` as float8_int2, a.`float8` MOD b.`uint2` as float8_uint2, a.`float8` MOD b.`int4` as float8_int4, a.`float8` MOD b.`uint4` as float8_uint4, a.`float8` MOD b.`int8` as float8_int8, a.`float8` MOD b.`uint8` as float8_uint8, a.`float8` MOD b.`float4` as float8_float4, a.`float8` MOD b.`float8` as float8_float8, a.`float8` MOD b.`numeric` as float8_numeric, a.`float8` MOD b.`boolean` as float8_boolean from `t_number` as a, `t_number` as b order by a.id, b.id;
desc v_float8_number;
drop view v_float8_number;

create view v_numeric_number as select a.`numeric` MOD b.`int1` as numeric_int1, a.`numeric` MOD b.`uint1` as numeric_uint1, a.`numeric` MOD b.`int2` as numeric_int2, a.`numeric` MOD b.`uint2` as numeric_uint2, a.`numeric` MOD b.`int4` as numeric_int4, a.`numeric` MOD b.`uint4` as numeric_uint4, a.`numeric` MOD b.`int8` as numeric_int8, a.`numeric` MOD b.`uint8` as numeric_uint8, a.`numeric` MOD b.`float4` as numeric_float4, a.`numeric` MOD b.`float8` as numeric_float8, a.`numeric` MOD b.`numeric` as numeric_numeric, a.`numeric` MOD b.`boolean` as numeric_boolean from `t_number` as a, `t_number` as b order by a.id, b.id;
desc v_numeric_number;
drop view v_numeric_number;

create view v_boolean_number as select a.`boolean` MOD b.`int1` as boolean_int1, a.`boolean` MOD b.`uint1` as boolean_uint1, a.`boolean` MOD b.`int2` as boolean_int2, a.`boolean` MOD b.`uint2` as boolean_uint2, a.`boolean` MOD b.`int4` as boolean_int4, a.`boolean` MOD b.`uint4` as boolean_uint4, a.`boolean` MOD b.`int8` as boolean_int8, a.`boolean` MOD b.`uint8` as boolean_uint8, a.`boolean` MOD b.`float4` as boolean_float4, a.`boolean` MOD b.`float8` as boolean_float8, a.`boolean` MOD b.`numeric` as boolean_numeric, a.`boolean` MOD b.`boolean` as boolean_boolean from `t_number` as a, `t_number` as b order by a.id, b.id;
desc v_boolean_number;
drop view v_boolean_number;

create view v_int1_string as select a.`int1` MOD b.`char` as int1_char, a.`int1` MOD b.`varchar` as int1_varchar, a.`int1` MOD b.`binary` as int1_binary, a.`int1` MOD b.`varbinary` as int1_varbinary, a.`int1` MOD b.`text` as int1_text from `t_number` as a, `t_str` as b order by a.id, b.id;
desc v_int1_string;
drop view v_int1_string;

create view v_uint1_string as select a.`uint1` MOD b.`char` as uint1_char, a.`uint1` MOD b.`varchar` as uint1_varchar, a.`uint1` MOD b.`binary` as uint1_binary, a.`uint1` MOD b.`varbinary` as uint1_varbinary, a.`uint1` MOD b.`text` as uint1_text from `t_number` as a, `t_str` as b order by a.id, b.id;
desc v_uint1_string;
drop view v_uint1_string;

create view v_int2_string as select a.`int2` MOD b.`char` as int2_char, a.`int2` MOD b.`varchar` as int2_varchar, a.`int2` MOD b.`binary` as int2_binary, a.`int2` MOD b.`varbinary` as int2_varbinary, a.`int2` MOD b.`text` as int2_text from `t_number` as a, `t_str` as b order by a.id, b.id;
desc v_int2_string;
drop view v_int2_string;

create view v_uint2_string as select a.`uint2` MOD b.`char` as uint2_char, a.`uint2` MOD b.`varchar` as uint2_varchar, a.`uint2` MOD b.`binary` as uint2_binary, a.`uint2` MOD b.`varbinary` as uint2_varbinary, a.`uint2` MOD b.`text` as uint2_text from `t_number` as a, `t_str` as b order by a.id, b.id;
desc v_uint2_string;
drop view v_uint2_string;

create view v_int4_string as select a.`int4` MOD b.`char` as int4_char, a.`int4` MOD b.`varchar` as int4_varchar, a.`int4` MOD b.`binary` as int4_binary, a.`int4` MOD b.`varbinary` as int4_varbinary, a.`int4` MOD b.`text` as int4_text from `t_number` as a, `t_str` as b order by a.id, b.id;
desc v_int4_string;
drop view v_int4_string;

create view v_uint4_string as select a.`uint4` MOD b.`char` as uint4_char, a.`uint4` MOD b.`varchar` as uint4_varchar, a.`uint4` MOD b.`binary` as uint4_binary, a.`uint4` MOD b.`varbinary` as uint4_varbinary, a.`uint4` MOD b.`text` as uint4_text from `t_number` as a, `t_str` as b order by a.id, b.id;
desc v_uint4_string;
drop view v_uint4_string;

create view v_int8_string as select a.`int8` MOD b.`char` as int8_char, a.`int8` MOD b.`varchar` as int8_varchar, a.`int8` MOD b.`binary` as int8_binary, a.`int8` MOD b.`varbinary` as int8_varbinary, a.`int8` MOD b.`text` as int8_text from `t_number` as a, `t_str` as b order by a.id, b.id;
desc v_int8_string;
drop view v_int8_string;

create view v_uint8_string as select a.`uint8` MOD b.`char` as uint8_char, a.`uint8` MOD b.`varchar` as uint8_varchar, a.`uint8` MOD b.`binary` as uint8_binary, a.`uint8` MOD b.`varbinary` as uint8_varbinary, a.`uint8` MOD b.`text` as uint8_text from `t_number` as a, `t_str` as b order by a.id, b.id;
desc v_uint8_string;
drop view v_uint8_string;

create view v_float4_string as select a.`float4` MOD b.`char` as float4_char, a.`float4` MOD b.`varchar` as float4_varchar, a.`float4` MOD b.`binary` as float4_binary, a.`float4` MOD b.`varbinary` as float4_varbinary, a.`float4` MOD b.`text` as float4_text from `t_number` as a, `t_str` as b order by a.id, b.id;
desc v_float4_string;
drop view v_float4_string;

create view v_float8_string as select a.`float8` MOD b.`char` as float8_char, a.`float8` MOD b.`varchar` as float8_varchar, a.`float8` MOD b.`binary` as float8_binary, a.`float8` MOD b.`varbinary` as float8_varbinary, a.`float8` MOD b.`text` as float8_text from `t_number` as a, `t_str` as b order by a.id, b.id;
desc v_float8_string;
drop view v_float8_string;

create view v_numeric_string as select a.`numeric` MOD b.`char` as numeric_char, a.`numeric` MOD b.`varchar` as numeric_varchar, a.`numeric` MOD b.`binary` as numeric_binary, a.`numeric` MOD b.`varbinary` as numeric_varbinary, a.`numeric` MOD b.`text` as numeric_text from `t_number` as a, `t_str` as b order by a.id, b.id;
desc v_numeric_string;
drop view v_numeric_string;

create view v_boolean_string as select a.`boolean` MOD b.`char` as boolean_char, a.`boolean` MOD b.`varchar` as boolean_varchar, a.`boolean` MOD b.`binary` as boolean_binary, a.`boolean` MOD b.`varbinary` as boolean_varbinary, a.`boolean` MOD b.`text` as boolean_text from `t_number` as a, `t_str` as b order by a.id, b.id;
desc v_boolean_string;
drop view v_boolean_string;

create view v_int1_bit as select a.`int1` MOD b.`bit1` as int1_bit1, a.`int1` MOD b.`bit8` as int1_bit8, a.`int1` MOD b.`bit15` as int1_bit15, a.`int1` MOD b.`bit64` as int1_bit64 from `t_number` as a, `t_bit` as b order by a.id, b.id;
desc v_int1_bit;
drop view v_int1_bit;

create view v_uint1_bit as select a.`uint1` MOD b.`bit1` as uint1_bit1, a.`uint1` MOD b.`bit8` as uint1_bit8, a.`uint1` MOD b.`bit15` as uint1_bit15, a.`uint1` MOD b.`bit64` as uint1_bit64 from `t_number` as a, `t_bit` as b order by a.id, b.id;
desc v_uint1_bit;
drop view v_uint1_bit;

create view v_int2_bit as select a.`int2` MOD b.`bit1` as int2_bit1, a.`int2` MOD b.`bit8` as int2_bit8, a.`int2` MOD b.`bit15` as int2_bit15, a.`int2` MOD b.`bit64` as int2_bit64 from `t_number` as a, `t_bit` as b order by a.id, b.id;
desc v_int2_bit;
drop view v_int2_bit;

create view v_uint2_bit as select a.`uint2` MOD b.`bit1` as uint2_bit1, a.`uint2` MOD b.`bit8` as uint2_bit8, a.`uint2` MOD b.`bit15` as uint2_bit15, a.`uint2` MOD b.`bit64` as uint2_bit64 from `t_number` as a, `t_bit` as b order by a.id, b.id;
desc v_uint2_bit;
drop view v_uint2_bit;

create view v_int4_bit as select a.`int4` MOD b.`bit1` as int4_bit1, a.`int4` MOD b.`bit8` as int4_bit8, a.`int4` MOD b.`bit15` as int4_bit15, a.`int4` MOD b.`bit64` as int4_bit64 from `t_number` as a, `t_bit` as b order by a.id, b.id;
desc v_int4_bit;
drop view v_int4_bit;

create view v_uint4_bit as select a.`uint4` MOD b.`bit1` as uint4_bit1, a.`uint4` MOD b.`bit8` as uint4_bit8, a.`uint4` MOD b.`bit15` as uint4_bit15, a.`uint4` MOD b.`bit64` as uint4_bit64 from `t_number` as a, `t_bit` as b order by a.id, b.id;
desc v_uint4_bit;
drop view v_uint4_bit;

create view v_int8_bit as select a.`int8` MOD b.`bit1` as int8_bit1, a.`int8` MOD b.`bit8` as int8_bit8, a.`int8` MOD b.`bit15` as int8_bit15, a.`int8` MOD b.`bit64` as int8_bit64 from `t_number` as a, `t_bit` as b order by a.id, b.id;
desc v_int8_bit;
drop view v_int8_bit;

create view v_uint8_bit as select a.`uint8` MOD b.`bit1` as uint8_bit1, a.`uint8` MOD b.`bit8` as uint8_bit8, a.`uint8` MOD b.`bit15` as uint8_bit15, a.`uint8` MOD b.`bit64` as uint8_bit64 from `t_number` as a, `t_bit` as b order by a.id, b.id;
desc v_uint8_bit;
drop view v_uint8_bit;

create view v_float4_bit as select a.`float4` MOD b.`bit1` as float4_bit1, a.`float4` MOD b.`bit8` as float4_bit8, a.`float4` MOD b.`bit15` as float4_bit15, a.`float4` MOD b.`bit64` as float4_bit64 from `t_number` as a, `t_bit` as b order by a.id, b.id;
desc v_float4_bit;
drop view v_float4_bit;

create view v_float8_bit as select a.`float8` MOD b.`bit1` as float8_bit1, a.`float8` MOD b.`bit8` as float8_bit8, a.`float8` MOD b.`bit15` as float8_bit15, a.`float8` MOD b.`bit64` as float8_bit64 from `t_number` as a, `t_bit` as b order by a.id, b.id;
desc v_float8_bit;
drop view v_float8_bit;

create view v_numeric_bit as select a.`numeric` MOD b.`bit1` as numeric_bit1, a.`numeric` MOD b.`bit8` as numeric_bit8, a.`numeric` MOD b.`bit15` as numeric_bit15, a.`numeric` MOD b.`bit64` as numeric_bit64 from `t_number` as a, `t_bit` as b order by a.id, b.id;
desc v_numeric_bit;
drop view v_numeric_bit;

create view v_boolean_bit as select a.`boolean` MOD b.`bit1` as boolean_bit1, a.`boolean` MOD b.`bit8` as boolean_bit8, a.`boolean` MOD b.`bit15` as boolean_bit15, a.`boolean` MOD b.`bit64` as boolean_bit64 from `t_number` as a, `t_bit` as b order by a.id, b.id;
desc v_boolean_bit;
drop view v_boolean_bit;

create view v_char_string as select a.`char` MOD b.`char` as char_char, a.`char` MOD b.`varchar` as char_varchar, a.`char` MOD b.`binary` as char_binary, a.`char` MOD b.`varbinary` as char_varbinary, a.`char` MOD b.`text` as char_text from `t_str` as a, `t_str` as b order by a.id, b.id;
desc v_char_string;
drop view v_char_string;

create view v_varchar_string as select a.`varchar` MOD b.`char` as varchar_char, a.`varchar` MOD b.`varchar` as varchar_varchar, a.`varchar` MOD b.`binary` as varchar_binary, a.`varchar` MOD b.`varbinary` as varchar_varbinary, a.`varchar` MOD b.`text` as varchar_text from `t_str` as a, `t_str` as b order by a.id, b.id;
desc v_varchar_string;
drop view v_varchar_string;

create view v_binary_string as select a.`binary` MOD b.`char` as binary_char, a.`binary` MOD b.`varchar` as binary_varchar, a.`binary` MOD b.`binary` as binary_binary, a.`binary` MOD b.`varbinary` as binary_varbinary, a.`binary` MOD b.`text` as binary_text from `t_str` as a, `t_str` as b order by a.id, b.id;
desc v_binary_string;
drop view v_binary_string;

create view v_varbinary_string as select a.`varbinary` MOD b.`char` as varbinary_char, a.`varbinary` MOD b.`varchar` as varbinary_varchar, a.`varbinary` MOD b.`binary` as varbinary_binary, a.`varbinary` MOD b.`varbinary` as varbinary_varbinary, a.`varbinary` MOD b.`text` as varbinary_text from `t_str` as a, `t_str` as b order by a.id, b.id;
desc v_varbinary_string;
drop view v_varbinary_string;

create view v_text_string as select a.`text` MOD b.`char` as text_char, a.`text` MOD b.`varchar` as text_varchar, a.`text` MOD b.`binary` as text_binary, a.`text` MOD b.`varbinary` as text_varbinary, a.`text` MOD b.`text` as text_text from `t_str` as a, `t_str` as b order by a.id, b.id;
desc v_text_string;
drop view v_text_string;

create view v_char_bit as select a.`char` MOD b.`bit1` as char_bit1, a.`char` MOD b.`bit8` as char_bit8, a.`char` MOD b.`bit15` as char_bit15, a.`char` MOD b.`bit64` as char_bit64 from `t_str` as a, `t_bit` as b order by a.id, b.id;
desc v_char_bit;
drop view v_char_bit;

create view v_varchar_bit as select a.`varchar` MOD b.`bit1` as varchar_bit1, a.`varchar` MOD b.`bit8` as varchar_bit8, a.`varchar` MOD b.`bit15` as varchar_bit15, a.`varchar` MOD b.`bit64` as varchar_bit64 from `t_str` as a, `t_bit` as b order by a.id, b.id;
desc v_varchar_bit;
drop view v_varchar_bit;

create view v_binary_bit as select a.`binary` MOD b.`bit1` as binary_bit1, a.`binary` MOD b.`bit8` as binary_bit8, a.`binary` MOD b.`bit15` as binary_bit15, a.`binary` MOD b.`bit64` as binary_bit64 from `t_str` as a, `t_bit` as b order by a.id, b.id;
desc v_binary_bit;
drop view v_binary_bit;

create view v_varbinary_bit as select a.`varbinary` MOD b.`bit1` as varbinary_bit1, a.`varbinary` MOD b.`bit8` as varbinary_bit8, a.`varbinary` MOD b.`bit15` as varbinary_bit15, a.`varbinary` MOD b.`bit64` as varbinary_bit64 from `t_str` as a, `t_bit` as b order by a.id, b.id;
desc v_varbinary_bit;
drop view v_varbinary_bit;

create view v_text_bit as select a.`text` MOD b.`bit1` as text_bit1, a.`text` MOD b.`bit8` as text_bit8, a.`text` MOD b.`bit15` as text_bit15, a.`text` MOD b.`bit64` as text_bit64 from `t_str` as a, `t_bit` as b order by a.id, b.id;
desc v_text_bit;
drop view v_text_bit;

create view v_bit1_bit as select a.`bit1` MOD b.`bit1` as bit1_bit1, a.`bit1` MOD b.`bit8` as bit1_bit8, a.`bit1` MOD b.`bit15` as bit1_bit15, a.`bit1` MOD b.`bit64` as bit1_bit64 from `t_bit` as a, `t_bit` as b order by a.id, b.id;
desc v_bit1_bit;
drop view v_bit1_bit;

create view v_bit8_bit as select a.`bit8` MOD b.`bit1` as bit8_bit1, a.`bit8` MOD b.`bit8` as bit8_bit8, a.`bit8` MOD b.`bit15` as bit8_bit15, a.`bit8` MOD b.`bit64` as bit8_bit64 from `t_bit` as a, `t_bit` as b order by a.id, b.id;
desc v_bit8_bit;
drop view v_bit8_bit;

create view v_bit15_bit as select a.`bit15` MOD b.`bit1` as bit15_bit1, a.`bit15` MOD b.`bit8` as bit15_bit8, a.`bit15` MOD b.`bit15` as bit15_bit15, a.`bit15` MOD b.`bit64` as bit15_bit64 from `t_bit` as a, `t_bit` as b order by a.id, b.id;
desc v_bit15_bit;
drop view v_bit15_bit;

create view v_bit64_bit as select a.`bit64` MOD b.`bit1` as bit64_bit1, a.`bit64` MOD b.`bit8` as bit64_bit8, a.`bit64` MOD b.`bit15` as bit64_bit15, a.`bit64` MOD b.`bit64` as bit64_bit64 from `t_bit` as a, `t_bit` as b order by a.id, b.id;
desc v_bit64_bit;
drop view v_bit64_bit;

-- test MOD result
truncate t_res;

insert into t_res select 1, a.id, b.id, 'int1MODint1', a.`int1` MOD b.`int1` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 2, a.id, b.id, 'int1MODuint1', a.`int1` MOD b.`uint1` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 3, a.id, b.id, 'int1MODint2', a.`int1` MOD b.`int2` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 4, a.id, b.id, 'int1MODuint2', a.`int1` MOD b.`uint2` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 5, a.id, b.id, 'int1MODint4', a.`int1` MOD b.`int4` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 6, a.id, b.id, 'int1MODuint4', a.`int1` MOD b.`uint4` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 7, a.id, b.id, 'int1MODint8', a.`int1` MOD b.`int8` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 8, a.id, b.id, 'int1MODuint8', a.`int1` MOD b.`uint8` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 9, a.id, b.id, 'int1MODfloat4', a.`int1` MOD b.`float4` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 10, a.id, b.id, 'int1MODfloat8', a.`int1` MOD b.`float8` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 11, a.id, b.id, 'int1MODnumeric', a.`int1` MOD b.`numeric` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 12, a.id, b.id, 'int1MODboolean', a.`int1` MOD b.`boolean` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 13, a.id, b.id, 'uint1MODint1', a.`uint1` MOD b.`int1` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 14, a.id, b.id, 'uint1MODuint1', a.`uint1` MOD b.`uint1` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 15, a.id, b.id, 'uint1MODint2', a.`uint1` MOD b.`int2` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 16, a.id, b.id, 'uint1MODuint2', a.`uint1` MOD b.`uint2` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 17, a.id, b.id, 'uint1MODint4', a.`uint1` MOD b.`int4` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 18, a.id, b.id, 'uint1MODuint4', a.`uint1` MOD b.`uint4` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 19, a.id, b.id, 'uint1MODint8', a.`uint1` MOD b.`int8` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 20, a.id, b.id, 'uint1MODuint8', a.`uint1` MOD b.`uint8` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 21, a.id, b.id, 'uint1MODfloat4', a.`uint1` MOD b.`float4` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 22, a.id, b.id, 'uint1MODfloat8', a.`uint1` MOD b.`float8` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 23, a.id, b.id, 'uint1MODnumeric', a.`uint1` MOD b.`numeric` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 24, a.id, b.id, 'uint1MODboolean', a.`uint1` MOD b.`boolean` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 25, a.id, b.id, 'int2MODint1', a.`int2` MOD b.`int1` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 26, a.id, b.id, 'int2MODuint1', a.`int2` MOD b.`uint1` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 27, a.id, b.id, 'int2MODint2', a.`int2` MOD b.`int2` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 28, a.id, b.id, 'int2MODuint2', a.`int2` MOD b.`uint2` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 29, a.id, b.id, 'int2MODint4', a.`int2` MOD b.`int4` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 30, a.id, b.id, 'int2MODuint4', a.`int2` MOD b.`uint4` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 31, a.id, b.id, 'int2MODint8', a.`int2` MOD b.`int8` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 32, a.id, b.id, 'int2MODuint8', a.`int2` MOD b.`uint8` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 33, a.id, b.id, 'int2MODfloat4', a.`int2` MOD b.`float4` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 34, a.id, b.id, 'int2MODfloat8', a.`int2` MOD b.`float8` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 35, a.id, b.id, 'int2MODnumeric', a.`int2` MOD b.`numeric` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 36, a.id, b.id, 'int2MODboolean', a.`int2` MOD b.`boolean` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 37, a.id, b.id, 'uint2MODint1', a.`uint2` MOD b.`int1` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 38, a.id, b.id, 'uint2MODuint1', a.`uint2` MOD b.`uint1` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 39, a.id, b.id, 'uint2MODint2', a.`uint2` MOD b.`int2` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 40, a.id, b.id, 'uint2MODuint2', a.`uint2` MOD b.`uint2` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 41, a.id, b.id, 'uint2MODint4', a.`uint2` MOD b.`int4` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 42, a.id, b.id, 'uint2MODuint4', a.`uint2` MOD b.`uint4` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 43, a.id, b.id, 'uint2MODint8', a.`uint2` MOD b.`int8` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 44, a.id, b.id, 'uint2MODuint8', a.`uint2` MOD b.`uint8` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 45, a.id, b.id, 'uint2MODfloat4', a.`uint2` MOD b.`float4` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 46, a.id, b.id, 'uint2MODfloat8', a.`uint2` MOD b.`float8` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 47, a.id, b.id, 'uint2MODnumeric', a.`uint2` MOD b.`numeric` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 48, a.id, b.id, 'uint2MODboolean', a.`uint2` MOD b.`boolean` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 49, a.id, b.id, 'int4MODint1', a.`int4` MOD b.`int1` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 50, a.id, b.id, 'int4MODuint1', a.`int4` MOD b.`uint1` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 51, a.id, b.id, 'int4MODint2', a.`int4` MOD b.`int2` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 52, a.id, b.id, 'int4MODuint2', a.`int4` MOD b.`uint2` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 53, a.id, b.id, 'int4MODint4', a.`int4` MOD b.`int4` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 54, a.id, b.id, 'int4MODuint4', a.`int4` MOD b.`uint4` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 55, a.id, b.id, 'int4MODint8', a.`int4` MOD b.`int8` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 56, a.id, b.id, 'int4MODuint8', a.`int4` MOD b.`uint8` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 57, a.id, b.id, 'int4MODfloat4', a.`int4` MOD b.`float4` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 58, a.id, b.id, 'int4MODfloat8', a.`int4` MOD b.`float8` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 59, a.id, b.id, 'int4MODnumeric', a.`int4` MOD b.`numeric` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 60, a.id, b.id, 'int4MODboolean', a.`int4` MOD b.`boolean` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 61, a.id, b.id, 'uint4MODint1', a.`uint4` MOD b.`int1` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 62, a.id, b.id, 'uint4MODuint1', a.`uint4` MOD b.`uint1` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 63, a.id, b.id, 'uint4MODint2', a.`uint4` MOD b.`int2` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 64, a.id, b.id, 'uint4MODuint2', a.`uint4` MOD b.`uint2` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 65, a.id, b.id, 'uint4MODint4', a.`uint4` MOD b.`int4` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 66, a.id, b.id, 'uint4MODuint4', a.`uint4` MOD b.`uint4` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 67, a.id, b.id, 'uint4MODint8', a.`uint4` MOD b.`int8` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 68, a.id, b.id, 'uint4MODuint8', a.`uint4` MOD b.`uint8` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 69, a.id, b.id, 'uint4MODfloat4', a.`uint4` MOD b.`float4` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 70, a.id, b.id, 'uint4MODfloat8', a.`uint4` MOD b.`float8` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 71, a.id, b.id, 'uint4MODnumeric', a.`uint4` MOD b.`numeric` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 72, a.id, b.id, 'uint4MODboolean', a.`uint4` MOD b.`boolean` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 73, a.id, b.id, 'int8MODint1', a.`int8` MOD b.`int1` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 74, a.id, b.id, 'int8MODuint1', a.`int8` MOD b.`uint1` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 75, a.id, b.id, 'int8MODint2', a.`int8` MOD b.`int2` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 76, a.id, b.id, 'int8MODuint2', a.`int8` MOD b.`uint2` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 77, a.id, b.id, 'int8MODint4', a.`int8` MOD b.`int4` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 78, a.id, b.id, 'int8MODuint4', a.`int8` MOD b.`uint4` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 79, a.id, b.id, 'int8MODint8', a.`int8` MOD b.`int8` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 80, a.id, b.id, 'int8MODuint8', a.`int8` MOD b.`uint8` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 81, a.id, b.id, 'int8MODfloat4', a.`int8` MOD b.`float4` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 82, a.id, b.id, 'int8MODfloat8', a.`int8` MOD b.`float8` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 83, a.id, b.id, 'int8MODnumeric', a.`int8` MOD b.`numeric` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 84, a.id, b.id, 'int8MODboolean', a.`int8` MOD b.`boolean` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 85, a.id, b.id, 'uint8MODint1', a.`uint8` MOD b.`int1` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 86, a.id, b.id, 'uint8MODuint1', a.`uint8` MOD b.`uint1` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 87, a.id, b.id, 'uint8MODint2', a.`uint8` MOD b.`int2` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 88, a.id, b.id, 'uint8MODuint2', a.`uint8` MOD b.`uint2` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 89, a.id, b.id, 'uint8MODint4', a.`uint8` MOD b.`int4` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 90, a.id, b.id, 'uint8MODuint4', a.`uint8` MOD b.`uint4` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 91, a.id, b.id, 'uint8MODint8', a.`uint8` MOD b.`int8` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 92, a.id, b.id, 'uint8MODuint8', a.`uint8` MOD b.`uint8` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 93, a.id, b.id, 'uint8MODfloat4', a.`uint8` MOD b.`float4` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 94, a.id, b.id, 'uint8MODfloat8', a.`uint8` MOD b.`float8` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 95, a.id, b.id, 'uint8MODnumeric', a.`uint8` MOD b.`numeric` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 96, a.id, b.id, 'uint8MODboolean', a.`uint8` MOD b.`boolean` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 97, a.id, b.id, 'float4MODint1', a.`float4` MOD b.`int1` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 98, a.id, b.id, 'float4MODuint1', a.`float4` MOD b.`uint1` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 99, a.id, b.id, 'float4MODint2', a.`float4` MOD b.`int2` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 100, a.id, b.id, 'float4MODuint2', a.`float4` MOD b.`uint2` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 101, a.id, b.id, 'float4MODint4', a.`float4` MOD b.`int4` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 102, a.id, b.id, 'float4MODuint4', a.`float4` MOD b.`uint4` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 103, a.id, b.id, 'float4MODint8', a.`float4` MOD b.`int8` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 104, a.id, b.id, 'float4MODuint8', a.`float4` MOD b.`uint8` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 105, a.id, b.id, 'float4MODfloat4', a.`float4` MOD b.`float4` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 106, a.id, b.id, 'float4MODfloat8', a.`float4` MOD b.`float8` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 107, a.id, b.id, 'float4MODnumeric', a.`float4` MOD b.`numeric` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 108, a.id, b.id, 'float4MODboolean', a.`float4` MOD b.`boolean` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 109, a.id, b.id, 'float8MODint1', a.`float8` MOD b.`int1` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 110, a.id, b.id, 'float8MODuint1', a.`float8` MOD b.`uint1` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 111, a.id, b.id, 'float8MODint2', a.`float8` MOD b.`int2` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 112, a.id, b.id, 'float8MODuint2', a.`float8` MOD b.`uint2` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 113, a.id, b.id, 'float8MODint4', a.`float8` MOD b.`int4` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 114, a.id, b.id, 'float8MODuint4', a.`float8` MOD b.`uint4` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 115, a.id, b.id, 'float8MODint8', a.`float8` MOD b.`int8` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 116, a.id, b.id, 'float8MODuint8', a.`float8` MOD b.`uint8` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 117, a.id, b.id, 'float8MODfloat4', a.`float8` MOD b.`float4` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 118, a.id, b.id, 'float8MODfloat8', a.`float8` MOD b.`float8` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 119, a.id, b.id, 'float8MODnumeric', a.`float8` MOD b.`numeric` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 120, a.id, b.id, 'float8MODboolean', a.`float8` MOD b.`boolean` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 121, a.id, b.id, 'numericMODint1', a.`numeric` MOD b.`int1` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 122, a.id, b.id, 'numericMODuint1', a.`numeric` MOD b.`uint1` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 123, a.id, b.id, 'numericMODint2', a.`numeric` MOD b.`int2` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 124, a.id, b.id, 'numericMODuint2', a.`numeric` MOD b.`uint2` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 125, a.id, b.id, 'numericMODint4', a.`numeric` MOD b.`int4` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 126, a.id, b.id, 'numericMODuint4', a.`numeric` MOD b.`uint4` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 127, a.id, b.id, 'numericMODint8', a.`numeric` MOD b.`int8` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 128, a.id, b.id, 'numericMODuint8', a.`numeric` MOD b.`uint8` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 129, a.id, b.id, 'numericMODfloat4', a.`numeric` MOD b.`float4` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 130, a.id, b.id, 'numericMODfloat8', a.`numeric` MOD b.`float8` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 131, a.id, b.id, 'numericMODnumeric', a.`numeric` MOD b.`numeric` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 132, a.id, b.id, 'numericMODboolean', a.`numeric` MOD b.`boolean` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 133, a.id, b.id, 'booleanMODint1', a.`boolean` MOD b.`int1` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 134, a.id, b.id, 'booleanMODuint1', a.`boolean` MOD b.`uint1` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 135, a.id, b.id, 'booleanMODint2', a.`boolean` MOD b.`int2` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 136, a.id, b.id, 'booleanMODuint2', a.`boolean` MOD b.`uint2` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 137, a.id, b.id, 'booleanMODint4', a.`boolean` MOD b.`int4` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 138, a.id, b.id, 'booleanMODuint4', a.`boolean` MOD b.`uint4` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 139, a.id, b.id, 'booleanMODint8', a.`boolean` MOD b.`int8` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 140, a.id, b.id, 'booleanMODuint8', a.`boolean` MOD b.`uint8` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 141, a.id, b.id, 'booleanMODfloat4', a.`boolean` MOD b.`float4` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 142, a.id, b.id, 'booleanMODfloat8', a.`boolean` MOD b.`float8` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 143, a.id, b.id, 'booleanMODnumeric', a.`boolean` MOD b.`numeric` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 144, a.id, b.id, 'booleanMODboolean', a.`boolean` MOD b.`boolean` from t_number as a, t_number as b order by a.id, b.id;
insert into t_res select 145, a.id, b.id, 'int1MODchar', a.`int1` MOD b.`char` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 146, a.id, b.id, 'int1MODvarchar', a.`int1` MOD b.`varchar` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 147, a.id, b.id, 'int1MODbinary', a.`int1` MOD b.`binary` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 148, a.id, b.id, 'int1MODvarbinary', a.`int1` MOD b.`varbinary` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 149, a.id, b.id, 'int1MODtext', a.`int1` MOD b.`text` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 150, a.id, b.id, 'uint1MODchar', a.`uint1` MOD b.`char` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 151, a.id, b.id, 'uint1MODvarchar', a.`uint1` MOD b.`varchar` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 152, a.id, b.id, 'uint1MODbinary', a.`uint1` MOD b.`binary` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 153, a.id, b.id, 'uint1MODvarbinary', a.`uint1` MOD b.`varbinary` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 154, a.id, b.id, 'uint1MODtext', a.`uint1` MOD b.`text` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 155, a.id, b.id, 'int2MODchar', a.`int2` MOD b.`char` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 156, a.id, b.id, 'int2MODvarchar', a.`int2` MOD b.`varchar` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 157, a.id, b.id, 'int2MODbinary', a.`int2` MOD b.`binary` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 158, a.id, b.id, 'int2MODvarbinary', a.`int2` MOD b.`varbinary` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 159, a.id, b.id, 'int2MODtext', a.`int2` MOD b.`text` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 160, a.id, b.id, 'uint2MODchar', a.`uint2` MOD b.`char` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 161, a.id, b.id, 'uint2MODvarchar', a.`uint2` MOD b.`varchar` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 162, a.id, b.id, 'uint2MODbinary', a.`uint2` MOD b.`binary` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 163, a.id, b.id, 'uint2MODvarbinary', a.`uint2` MOD b.`varbinary` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 164, a.id, b.id, 'uint2MODtext', a.`uint2` MOD b.`text` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 165, a.id, b.id, 'int4MODchar', a.`int4` MOD b.`char` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 166, a.id, b.id, 'int4MODvarchar', a.`int4` MOD b.`varchar` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 167, a.id, b.id, 'int4MODbinary', a.`int4` MOD b.`binary` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 168, a.id, b.id, 'int4MODvarbinary', a.`int4` MOD b.`varbinary` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 169, a.id, b.id, 'int4MODtext', a.`int4` MOD b.`text` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 170, a.id, b.id, 'uint4MODchar', a.`uint4` MOD b.`char` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 171, a.id, b.id, 'uint4MODvarchar', a.`uint4` MOD b.`varchar` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 172, a.id, b.id, 'uint4MODbinary', a.`uint4` MOD b.`binary` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 173, a.id, b.id, 'uint4MODvarbinary', a.`uint4` MOD b.`varbinary` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 174, a.id, b.id, 'uint4MODtext', a.`uint4` MOD b.`text` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 175, a.id, b.id, 'int8MODchar', a.`int8` MOD b.`char` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 176, a.id, b.id, 'int8MODvarchar', a.`int8` MOD b.`varchar` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 177, a.id, b.id, 'int8MODbinary', a.`int8` MOD b.`binary` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 178, a.id, b.id, 'int8MODvarbinary', a.`int8` MOD b.`varbinary` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 179, a.id, b.id, 'int8MODtext', a.`int8` MOD b.`text` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 180, a.id, b.id, 'uint8MODchar', a.`uint8` MOD b.`char` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 181, a.id, b.id, 'uint8MODvarchar', a.`uint8` MOD b.`varchar` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 182, a.id, b.id, 'uint8MODbinary', a.`uint8` MOD b.`binary` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 183, a.id, b.id, 'uint8MODvarbinary', a.`uint8` MOD b.`varbinary` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 184, a.id, b.id, 'uint8MODtext', a.`uint8` MOD b.`text` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 185, a.id, b.id, 'float4MODchar', a.`float4` MOD b.`char` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 186, a.id, b.id, 'float4MODvarchar', a.`float4` MOD b.`varchar` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 187, a.id, b.id, 'float4MODbinary', a.`float4` MOD b.`binary` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 188, a.id, b.id, 'float4MODvarbinary', a.`float4` MOD b.`varbinary` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 189, a.id, b.id, 'float4MODtext', a.`float4` MOD b.`text` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 190, a.id, b.id, 'float8MODchar', a.`float8` MOD b.`char` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 191, a.id, b.id, 'float8MODvarchar', a.`float8` MOD b.`varchar` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 192, a.id, b.id, 'float8MODbinary', a.`float8` MOD b.`binary` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 193, a.id, b.id, 'float8MODvarbinary', a.`float8` MOD b.`varbinary` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 194, a.id, b.id, 'float8MODtext', a.`float8` MOD b.`text` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 195, a.id, b.id, 'numericMODchar', a.`numeric` MOD b.`char` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 196, a.id, b.id, 'numericMODvarchar', a.`numeric` MOD b.`varchar` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 197, a.id, b.id, 'numericMODbinary', a.`numeric` MOD b.`binary` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 198, a.id, b.id, 'numericMODvarbinary', a.`numeric` MOD b.`varbinary` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 199, a.id, b.id, 'numericMODtext', a.`numeric` MOD b.`text` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 200, a.id, b.id, 'booleanMODchar', a.`boolean` MOD b.`char` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 201, a.id, b.id, 'booleanMODvarchar', a.`boolean` MOD b.`varchar` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 202, a.id, b.id, 'booleanMODbinary', a.`boolean` MOD b.`binary` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 203, a.id, b.id, 'booleanMODvarbinary', a.`boolean` MOD b.`varbinary` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 204, a.id, b.id, 'booleanMODtext', a.`boolean` MOD b.`text` from t_number as a, t_str as b order by a.id, b.id;
insert into t_res select 205, a.id, b.id, 'int1MODbit1', a.`int1` MOD b.`bit1` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 206, a.id, b.id, 'int1MODbit8', a.`int1` MOD b.`bit8` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 207, a.id, b.id, 'int1MODbit15', a.`int1` MOD b.`bit15` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 208, a.id, b.id, 'int1MODbit64', a.`int1` MOD b.`bit64` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 209, a.id, b.id, 'uint1MODbit1', a.`uint1` MOD b.`bit1` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 210, a.id, b.id, 'uint1MODbit8', a.`uint1` MOD b.`bit8` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 211, a.id, b.id, 'uint1MODbit15', a.`uint1` MOD b.`bit15` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 212, a.id, b.id, 'uint1MODbit64', a.`uint1` MOD b.`bit64` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 213, a.id, b.id, 'int2MODbit1', a.`int2` MOD b.`bit1` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 214, a.id, b.id, 'int2MODbit8', a.`int2` MOD b.`bit8` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 215, a.id, b.id, 'int2MODbit15', a.`int2` MOD b.`bit15` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 216, a.id, b.id, 'int2MODbit64', a.`int2` MOD b.`bit64` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 217, a.id, b.id, 'uint2MODbit1', a.`uint2` MOD b.`bit1` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 218, a.id, b.id, 'uint2MODbit8', a.`uint2` MOD b.`bit8` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 219, a.id, b.id, 'uint2MODbit15', a.`uint2` MOD b.`bit15` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 220, a.id, b.id, 'uint2MODbit64', a.`uint2` MOD b.`bit64` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 221, a.id, b.id, 'int4MODbit1', a.`int4` MOD b.`bit1` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 222, a.id, b.id, 'int4MODbit8', a.`int4` MOD b.`bit8` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 223, a.id, b.id, 'int4MODbit15', a.`int4` MOD b.`bit15` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 224, a.id, b.id, 'int4MODbit64', a.`int4` MOD b.`bit64` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 225, a.id, b.id, 'uint4MODbit1', a.`uint4` MOD b.`bit1` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 226, a.id, b.id, 'uint4MODbit8', a.`uint4` MOD b.`bit8` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 227, a.id, b.id, 'uint4MODbit15', a.`uint4` MOD b.`bit15` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 228, a.id, b.id, 'uint4MODbit64', a.`uint4` MOD b.`bit64` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 229, a.id, b.id, 'int8MODbit1', a.`int8` MOD b.`bit1` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 230, a.id, b.id, 'int8MODbit8', a.`int8` MOD b.`bit8` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 231, a.id, b.id, 'int8MODbit15', a.`int8` MOD b.`bit15` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 232, a.id, b.id, 'int8MODbit64', a.`int8` MOD b.`bit64` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 233, a.id, b.id, 'uint8MODbit1', a.`uint8` MOD b.`bit1` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 234, a.id, b.id, 'uint8MODbit8', a.`uint8` MOD b.`bit8` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 235, a.id, b.id, 'uint8MODbit15', a.`uint8` MOD b.`bit15` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 236, a.id, b.id, 'uint8MODbit64', a.`uint8` MOD b.`bit64` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 237, a.id, b.id, 'float4MODbit1', a.`float4` MOD b.`bit1` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 238, a.id, b.id, 'float4MODbit8', a.`float4` MOD b.`bit8` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 239, a.id, b.id, 'float4MODbit15', a.`float4` MOD b.`bit15` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 240, a.id, b.id, 'float4MODbit64', a.`float4` MOD b.`bit64` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 241, a.id, b.id, 'float8MODbit1', a.`float8` MOD b.`bit1` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 242, a.id, b.id, 'float8MODbit8', a.`float8` MOD b.`bit8` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 243, a.id, b.id, 'float8MODbit15', a.`float8` MOD b.`bit15` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 244, a.id, b.id, 'float8MODbit64', a.`float8` MOD b.`bit64` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 245, a.id, b.id, 'numericMODbit1', a.`numeric` MOD b.`bit1` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 246, a.id, b.id, 'numericMODbit8', a.`numeric` MOD b.`bit8` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 247, a.id, b.id, 'numericMODbit15', a.`numeric` MOD b.`bit15` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 248, a.id, b.id, 'numericMODbit64', a.`numeric` MOD b.`bit64` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 249, a.id, b.id, 'booleanMODbit1', a.`boolean` MOD b.`bit1` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 250, a.id, b.id, 'booleanMODbit8', a.`boolean` MOD b.`bit8` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 251, a.id, b.id, 'booleanMODbit15', a.`boolean` MOD b.`bit15` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 252, a.id, b.id, 'booleanMODbit64', a.`boolean` MOD b.`bit64` from t_number as a, t_bit as b order by a.id, b.id;
insert into t_res select 253, a.id, b.id, 'charMODchar', a.`char` MOD b.`char` from t_str as a, t_str as b order by a.id, b.id;
insert into t_res select 254, a.id, b.id, 'charMODvarchar', a.`char` MOD b.`varchar` from t_str as a, t_str as b order by a.id, b.id;
insert into t_res select 255, a.id, b.id, 'charMODbinary', a.`char` MOD b.`binary` from t_str as a, t_str as b order by a.id, b.id;
insert into t_res select 256, a.id, b.id, 'charMODvarbinary', a.`char` MOD b.`varbinary` from t_str as a, t_str as b order by a.id, b.id;
insert into t_res select 257, a.id, b.id, 'charMODtext', a.`char` MOD b.`text` from t_str as a, t_str as b order by a.id, b.id;
insert into t_res select 258, a.id, b.id, 'varcharMODchar', a.`varchar` MOD b.`char` from t_str as a, t_str as b order by a.id, b.id;
insert into t_res select 259, a.id, b.id, 'varcharMODvarchar', a.`varchar` MOD b.`varchar` from t_str as a, t_str as b order by a.id, b.id;
insert into t_res select 260, a.id, b.id, 'varcharMODbinary', a.`varchar` MOD b.`binary` from t_str as a, t_str as b order by a.id, b.id;
insert into t_res select 261, a.id, b.id, 'varcharMODvarbinary', a.`varchar` MOD b.`varbinary` from t_str as a, t_str as b order by a.id, b.id;
insert into t_res select 262, a.id, b.id, 'varcharMODtext', a.`varchar` MOD b.`text` from t_str as a, t_str as b order by a.id, b.id;
insert into t_res select 263, a.id, b.id, 'binaryMODchar', a.`binary` MOD b.`char` from t_str as a, t_str as b order by a.id, b.id;
insert into t_res select 264, a.id, b.id, 'binaryMODvarchar', a.`binary` MOD b.`varchar` from t_str as a, t_str as b order by a.id, b.id;
insert into t_res select 265, a.id, b.id, 'binaryMODbinary', a.`binary` MOD b.`binary` from t_str as a, t_str as b order by a.id, b.id;
insert into t_res select 266, a.id, b.id, 'binaryMODvarbinary', a.`binary` MOD b.`varbinary` from t_str as a, t_str as b order by a.id, b.id;
insert into t_res select 267, a.id, b.id, 'binaryMODtext', a.`binary` MOD b.`text` from t_str as a, t_str as b order by a.id, b.id;
insert into t_res select 268, a.id, b.id, 'varbinaryMODchar', a.`varbinary` MOD b.`char` from t_str as a, t_str as b order by a.id, b.id;
insert into t_res select 269, a.id, b.id, 'varbinaryMODvarchar', a.`varbinary` MOD b.`varchar` from t_str as a, t_str as b order by a.id, b.id;
insert into t_res select 270, a.id, b.id, 'varbinaryMODbinary', a.`varbinary` MOD b.`binary` from t_str as a, t_str as b order by a.id, b.id;
insert into t_res select 271, a.id, b.id, 'varbinaryMODvarbinary', a.`varbinary` MOD b.`varbinary` from t_str as a, t_str as b order by a.id, b.id;
insert into t_res select 272, a.id, b.id, 'varbinaryMODtext', a.`varbinary` MOD b.`text` from t_str as a, t_str as b order by a.id, b.id;
insert into t_res select 273, a.id, b.id, 'textMODchar', a.`text` MOD b.`char` from t_str as a, t_str as b order by a.id, b.id;
insert into t_res select 274, a.id, b.id, 'textMODvarchar', a.`text` MOD b.`varchar` from t_str as a, t_str as b order by a.id, b.id;
insert into t_res select 275, a.id, b.id, 'textMODbinary', a.`text` MOD b.`binary` from t_str as a, t_str as b order by a.id, b.id;
insert into t_res select 276, a.id, b.id, 'textMODvarbinary', a.`text` MOD b.`varbinary` from t_str as a, t_str as b order by a.id, b.id;
insert into t_res select 277, a.id, b.id, 'textMODtext', a.`text` MOD b.`text` from t_str as a, t_str as b order by a.id, b.id;
insert into t_res select 278, a.id, b.id, 'charMODbit1', a.`char` MOD b.`bit1` from t_str as a, t_bit as b order by a.id, b.id;
insert into t_res select 279, a.id, b.id, 'charMODbit8', a.`char` MOD b.`bit8` from t_str as a, t_bit as b order by a.id, b.id;
insert into t_res select 280, a.id, b.id, 'charMODbit15', a.`char` MOD b.`bit15` from t_str as a, t_bit as b order by a.id, b.id;
insert into t_res select 281, a.id, b.id, 'charMODbit64', a.`char` MOD b.`bit64` from t_str as a, t_bit as b order by a.id, b.id;
insert into t_res select 282, a.id, b.id, 'varcharMODbit1', a.`varchar` MOD b.`bit1` from t_str as a, t_bit as b order by a.id, b.id;
insert into t_res select 283, a.id, b.id, 'varcharMODbit8', a.`varchar` MOD b.`bit8` from t_str as a, t_bit as b order by a.id, b.id;
insert into t_res select 284, a.id, b.id, 'varcharMODbit15', a.`varchar` MOD b.`bit15` from t_str as a, t_bit as b order by a.id, b.id;
insert into t_res select 285, a.id, b.id, 'varcharMODbit64', a.`varchar` MOD b.`bit64` from t_str as a, t_bit as b order by a.id, b.id;
insert into t_res select 286, a.id, b.id, 'binaryMODbit1', a.`binary` MOD b.`bit1` from t_str as a, t_bit as b order by a.id, b.id;
insert into t_res select 287, a.id, b.id, 'binaryMODbit8', a.`binary` MOD b.`bit8` from t_str as a, t_bit as b order by a.id, b.id;
insert into t_res select 288, a.id, b.id, 'binaryMODbit15', a.`binary` MOD b.`bit15` from t_str as a, t_bit as b order by a.id, b.id;
insert into t_res select 289, a.id, b.id, 'binaryMODbit64', a.`binary` MOD b.`bit64` from t_str as a, t_bit as b order by a.id, b.id;
insert into t_res select 290, a.id, b.id, 'varbinaryMODbit1', a.`varbinary` MOD b.`bit1` from t_str as a, t_bit as b order by a.id, b.id;
insert into t_res select 291, a.id, b.id, 'varbinaryMODbit8', a.`varbinary` MOD b.`bit8` from t_str as a, t_bit as b order by a.id, b.id;
insert into t_res select 292, a.id, b.id, 'varbinaryMODbit15', a.`varbinary` MOD b.`bit15` from t_str as a, t_bit as b order by a.id, b.id;
insert into t_res select 293, a.id, b.id, 'varbinaryMODbit64', a.`varbinary` MOD b.`bit64` from t_str as a, t_bit as b order by a.id, b.id;
insert into t_res select 294, a.id, b.id, 'textMODbit1', a.`text` MOD b.`bit1` from t_str as a, t_bit as b order by a.id, b.id;
insert into t_res select 295, a.id, b.id, 'textMODbit8', a.`text` MOD b.`bit8` from t_str as a, t_bit as b order by a.id, b.id;
insert into t_res select 296, a.id, b.id, 'textMODbit15', a.`text` MOD b.`bit15` from t_str as a, t_bit as b order by a.id, b.id;
insert into t_res select 297, a.id, b.id, 'textMODbit64', a.`text` MOD b.`bit64` from t_str as a, t_bit as b order by a.id, b.id;
insert into t_res select 298, a.id, b.id, 'bit1MODbit1', a.`bit1` MOD b.`bit1` from t_bit as a, t_bit as b order by a.id, b.id;
insert into t_res select 299, a.id, b.id, 'bit1MODbit8', a.`bit1` MOD b.`bit8` from t_bit as a, t_bit as b order by a.id, b.id;
insert into t_res select 300, a.id, b.id, 'bit1MODbit15', a.`bit1` MOD b.`bit15` from t_bit as a, t_bit as b order by a.id, b.id;
insert into t_res select 301, a.id, b.id, 'bit1MODbit64', a.`bit1` MOD b.`bit64` from t_bit as a, t_bit as b order by a.id, b.id;
insert into t_res select 302, a.id, b.id, 'bit8MODbit1', a.`bit8` MOD b.`bit1` from t_bit as a, t_bit as b order by a.id, b.id;
insert into t_res select 303, a.id, b.id, 'bit8MODbit8', a.`bit8` MOD b.`bit8` from t_bit as a, t_bit as b order by a.id, b.id;
insert into t_res select 304, a.id, b.id, 'bit8MODbit15', a.`bit8` MOD b.`bit15` from t_bit as a, t_bit as b order by a.id, b.id;
insert into t_res select 305, a.id, b.id, 'bit8MODbit64', a.`bit8` MOD b.`bit64` from t_bit as a, t_bit as b order by a.id, b.id;
insert into t_res select 306, a.id, b.id, 'bit15MODbit1', a.`bit15` MOD b.`bit1` from t_bit as a, t_bit as b order by a.id, b.id;
insert into t_res select 307, a.id, b.id, 'bit15MODbit8', a.`bit15` MOD b.`bit8` from t_bit as a, t_bit as b order by a.id, b.id;
insert into t_res select 308, a.id, b.id, 'bit15MODbit15', a.`bit15` MOD b.`bit15` from t_bit as a, t_bit as b order by a.id, b.id;
insert into t_res select 309, a.id, b.id, 'bit15MODbit64', a.`bit15` MOD b.`bit64` from t_bit as a, t_bit as b order by a.id, b.id;
insert into t_res select 310, a.id, b.id, 'bit64MODbit1', a.`bit64` MOD b.`bit1` from t_bit as a, t_bit as b order by a.id, b.id;
insert into t_res select 311, a.id, b.id, 'bit64MODbit8', a.`bit64` MOD b.`bit8` from t_bit as a, t_bit as b order by a.id, b.id;
insert into t_res select 312, a.id, b.id, 'bit64MODbit15', a.`bit64` MOD b.`bit15` from t_bit as a, t_bit as b order by a.id, b.id;
insert into t_res select 313, a.id, b.id, 'bit64MODbit64', a.`bit64` MOD b.`bit64` from t_bit as a, t_bit as b order by a.id, b.id;

select * from t_res order by id, a_id, b_id;

drop table t_res;
drop table t_bit;
drop table t_str;
drop table t_number;
reset search_path;
reset dolphin.sql_mode;
drop schema test_div_mod;

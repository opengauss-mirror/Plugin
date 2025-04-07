------------------------------------
-- test bit_expr << | >> bit_expr
-- test type: number and str
--      number(int1, uint1, int2, uint2, int4, uint4, int8, uint8, float4, float8, bool, bit)、
--      str(char, varchar, text, binary, varbinary)
------------------------------------
create schema test_left_right_shift;
set search_path to test_left_right_shift;
set dolphin.b_compatibility_mode to on;

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

-- test >> result_type
create view v_int1_number as select a.`int1` >> b.`int1` as int1_int1, a.`int1` >> b.`uint1` as int1_uint1, a.`int1` >> b.`int2` as int1_int2, a.`int1` >> b.`uint2` as int1_uint2, a.`int1` >> b.`int4` as int1_int4, a.`int1` >> b.`uint4` as int1_uint4, a.`int1` >> b.`int8` as int1_int8, a.`int1` >> b.`uint8` as int1_uint8, a.`int1` >> b.`float4` as int1_float4, a.`int1` >> b.`float8` as int1_float8, a.`int1` >> b.`numeric` as int1_numeric, a.`int1` >> b.`boolean` as int1_boolean from `t_number` as a, `t_number` as b order by a.id, b.id;
desc v_int1_number; -- expect all bigint unsigned
drop view v_int1_number;

create view v_uint1_number as select a.`uint1` >> b.`int1` as uint1_int1, a.`uint1` >> b.`uint1` as uint1_uint1, a.`uint1` >> b.`int2` as uint1_int2, a.`uint1` >> b.`uint2` as uint1_uint2, a.`uint1` >> b.`int4` as uint1_int4, a.`uint1` >> b.`uint4` as uint1_uint4, a.`uint1` >> b.`int8` as uint1_int8, a.`uint1` >> b.`uint8` as uint1_uint8, a.`uint1` >> b.`float4` as uint1_float4, a.`uint1` >> b.`float8` as uint1_float8, a.`uint1` >> b.`numeric` as uint1_numeric, a.`uint1` >> b.`boolean` as uint1_boolean from `t_number` as a, `t_number` as b order by a.id, b.id;
desc v_uint1_number; -- expect all bigint unsigned
drop view v_uint1_number;

create view v_int2_number as select a.`int2` >> b.`int1` as int2_int1, a.`int2` >> b.`uint1` as int2_uint1, a.`int2` >> b.`int2` as int2_int2, a.`int2` >> b.`uint2` as int2_uint2, a.`int2` >> b.`int4` as int2_int4, a.`int2` >> b.`uint4` as int2_uint4, a.`int2` >> b.`int8` as int2_int8, a.`int2` >> b.`uint8` as int2_uint8, a.`int2` >> b.`float4` as int2_float4, a.`int2` >> b.`float8` as int2_float8, a.`int2` >> b.`numeric` as int2_numeric, a.`int2` >> b.`boolean` as int2_boolean from `t_number` as a, `t_number` as b order by a.id, b.id;
desc v_int2_number; -- expect all bigint unsigned
drop view v_int2_number;

create view v_uint2_number as select a.`uint2` >> b.`int1` as uint2_int1, a.`uint2` >> b.`uint1` as uint2_uint1, a.`uint2` >> b.`int2` as uint2_int2, a.`uint2` >> b.`uint2` as uint2_uint2, a.`uint2` >> b.`int4` as uint2_int4, a.`uint2` >> b.`uint4` as uint2_uint4, a.`uint2` >> b.`int8` as uint2_int8, a.`uint2` >> b.`uint8` as uint2_uint8, a.`uint2` >> b.`float4` as uint2_float4, a.`uint2` >> b.`float8` as uint2_float8, a.`uint2` >> b.`numeric` as uint2_numeric, a.`uint2` >> b.`boolean` as uint2_boolean from `t_number` as a, `t_number` as b order by a.id, b.id;
desc v_uint2_number; -- expect all bigint unsigned
drop view v_uint2_number;

create view v_int4_number as select a.`int4` >> b.`int1` as int4_int1, a.`int4` >> b.`uint1` as int4_uint1, a.`int4` >> b.`int2` as int4_int2, a.`int4` >> b.`uint2` as int4_uint2, a.`int4` >> b.`int4` as int4_int4, a.`int4` >> b.`uint4` as int4_uint4, a.`int4` >> b.`int8` as int4_int8, a.`int4` >> b.`uint8` as int4_uint8, a.`int4` >> b.`float4` as int4_float4, a.`int4` >> b.`float8` as int4_float8, a.`int4` >> b.`numeric` as int4_numeric, a.`int4` >> b.`boolean` as int4_boolean from `t_number` as a, `t_number` as b order by a.id, b.id;
desc v_int4_number; -- expect all bigint unsigned
drop view v_int4_number;

create view v_uint4_number as select a.`uint4` >> b.`int1` as uint4_int1, a.`uint4` >> b.`uint1` as uint4_uint1, a.`uint4` >> b.`int2` as uint4_int2, a.`uint4` >> b.`uint2` as uint4_uint2, a.`uint4` >> b.`int4` as uint4_int4, a.`uint4` >> b.`uint4` as uint4_uint4, a.`uint4` >> b.`int8` as uint4_int8, a.`uint4` >> b.`uint8` as uint4_uint8, a.`uint4` >> b.`float4` as uint4_float4, a.`uint4` >> b.`float8` as uint4_float8, a.`uint4` >> b.`numeric` as uint4_numeric, a.`uint4` >> b.`boolean` as uint4_boolean from `t_number` as a, `t_number` as b order by a.id, b.id;
desc v_uint4_number; -- expect all bigint unsigned
drop view v_uint4_number;

create view v_int8_number as select a.`int8` >> b.`int1` as int8_int1, a.`int8` >> b.`uint1` as int8_uint1, a.`int8` >> b.`int2` as int8_int2, a.`int8` >> b.`uint2` as int8_uint2, a.`int8` >> b.`int4` as int8_int4, a.`int8` >> b.`uint4` as int8_uint4, a.`int8` >> b.`int8` as int8_int8, a.`int8` >> b.`uint8` as int8_uint8, a.`int8` >> b.`float4` as int8_float4, a.`int8` >> b.`float8` as int8_float8, a.`int8` >> b.`numeric` as int8_numeric, a.`int8` >> b.`boolean` as int8_boolean from `t_number` as a, `t_number` as b order by a.id, b.id;
desc v_int8_number; -- expect all bigint unsigned
drop view v_int8_number;

create view v_uint8_number as select a.`uint8` >> b.`int1` as uint8_int1, a.`uint8` >> b.`uint1` as uint8_uint1, a.`uint8` >> b.`int2` as uint8_int2, a.`uint8` >> b.`uint2` as uint8_uint2, a.`uint8` >> b.`int4` as uint8_int4, a.`uint8` >> b.`uint4` as uint8_uint4, a.`uint8` >> b.`int8` as uint8_int8, a.`uint8` >> b.`uint8` as uint8_uint8, a.`uint8` >> b.`float4` as uint8_float4, a.`uint8` >> b.`float8` as uint8_float8, a.`uint8` >> b.`numeric` as uint8_numeric, a.`uint8` >> b.`boolean` as uint8_boolean from `t_number` as a, `t_number` as b order by a.id, b.id;
desc v_uint8_number;
drop view v_uint8_number;

create view v_float4_number as select a.`float4` >> b.`int1` as float4_int1, a.`float4` >> b.`uint1` as float4_uint1, a.`float4` >> b.`int2` as float4_int2, a.`float4` >> b.`uint2` as float4_uint2, a.`float4` >> b.`int4` as float4_int4, a.`float4` >> b.`uint4` as float4_uint4, a.`float4` >> b.`int8` as float4_int8, a.`float4` >> b.`uint8` as float4_uint8, a.`float4` >> b.`float4` as float4_float4, a.`float4` >> b.`float8` as float4_float8, a.`float4` >> b.`numeric` as float4_numeric, a.`float4` >> b.`boolean` as float4_boolean from `t_number` as a, `t_number` as b order by a.id, b.id;
desc v_float4_number; -- expect all bigint unsigned
drop view v_float4_number;

create view v_float8_number as select a.`float8` >> b.`int1` as float8_int1, a.`float8` >> b.`uint1` as float8_uint1, a.`float8` >> b.`int2` as float8_int2, a.`float8` >> b.`uint2` as float8_uint2, a.`float8` >> b.`int4` as float8_int4, a.`float8` >> b.`uint4` as float8_uint4, a.`float8` >> b.`int8` as float8_int8, a.`float8` >> b.`uint8` as float8_uint8, a.`float8` >> b.`float4` as float8_float4, a.`float8` >> b.`float8` as float8_float8, a.`float8` >> b.`numeric` as float8_numeric, a.`float8` >> b.`boolean` as float8_boolean from `t_number` as a, `t_number` as b order by a.id, b.id;
desc v_float8_number; -- expect all bigint unsigned
drop view v_float8_number;

create view v_numeric_number as select a.`numeric` >> b.`int1` as numeric_int1, a.`numeric` >> b.`uint1` as numeric_uint1, a.`numeric` >> b.`int2` as numeric_int2, a.`numeric` >> b.`uint2` as numeric_uint2, a.`numeric` >> b.`int4` as numeric_int4, a.`numeric` >> b.`uint4` as numeric_uint4, a.`numeric` >> b.`int8` as numeric_int8, a.`numeric` >> b.`uint8` as numeric_uint8, a.`numeric` >> b.`float4` as numeric_float4, a.`numeric` >> b.`float8` as numeric_float8, a.`numeric` >> b.`numeric` as numeric_numeric, a.`numeric` >> b.`boolean` as numeric_boolean from `t_number` as a, `t_number` as b order by a.id, b.id;
desc v_numeric_number; -- expect all bigint unsigned
drop view v_numeric_number;

create view v_boolean_number as select a.`boolean` >> b.`int1` as boolean_int1, a.`boolean` >> b.`uint1` as boolean_uint1, a.`boolean` >> b.`int2` as boolean_int2, a.`boolean` >> b.`uint2` as boolean_uint2, a.`boolean` >> b.`int4` as boolean_int4, a.`boolean` >> b.`uint4` as boolean_uint4, a.`boolean` >> b.`int8` as boolean_int8, a.`boolean` >> b.`uint8` as boolean_uint8, a.`boolean` >> b.`float4` as boolean_float4, a.`boolean` >> b.`float8` as boolean_float8, a.`boolean` >> b.`numeric` as boolean_numeric, a.`boolean` >> b.`boolean` as boolean_boolean from `t_number` as a, `t_number` as b order by a.id, b.id;
desc v_boolean_number; -- expect all bigint unsigned
drop view v_boolean_number;

create view v_int1_string as select a.`int1` >> b.`char` as int1_char, a.`int1` >> b.`varchar` as int1_varchar, a.`int1` >> b.`binary` as int1_binary, a.`int1` >> b.`varbinary` as int1_varbinary, a.`int1` >> b.`text` as int1_text from `t_number` as a, `t_str` as b order by a.id, b.id;
desc v_int1_string; -- expect all bigint unsigned
drop view v_int1_string;

create view v_uint1_string as select a.`uint1` >> b.`char` as uint1_char, a.`uint1` >> b.`varchar` as uint1_varchar, a.`uint1` >> b.`binary` as uint1_binary, a.`uint1` >> b.`varbinary` as uint1_varbinary, a.`uint1` >> b.`text` as uint1_text from `t_number` as a, `t_str` as b order by a.id, b.id;
desc v_uint1_string; -- expect all bigint unsigned
drop view v_uint1_string;

create view v_int2_string as select a.`int2` >> b.`char` as int2_char, a.`int2` >> b.`varchar` as int2_varchar, a.`int2` >> b.`binary` as int2_binary, a.`int2` >> b.`varbinary` as int2_varbinary, a.`int2` >> b.`text` as int2_text from `t_number` as a, `t_str` as b order by a.id, b.id;
desc v_int2_string; -- expect all bigint unsigned
drop view v_int2_string;

create view v_uint2_string as select a.`uint2` >> b.`char` as uint2_char, a.`uint2` >> b.`varchar` as uint2_varchar, a.`uint2` >> b.`binary` as uint2_binary, a.`uint2` >> b.`varbinary` as uint2_varbinary, a.`uint2` >> b.`text` as uint2_text from `t_number` as a, `t_str` as b order by a.id, b.id;
desc v_uint2_string; -- expect all bigint unsigned
drop view v_uint2_string;

create view v_int4_string as select a.`int4` >> b.`char` as int4_char, a.`int4` >> b.`varchar` as int4_varchar, a.`int4` >> b.`binary` as int4_binary, a.`int4` >> b.`varbinary` as int4_varbinary, a.`int4` >> b.`text` as int4_text from `t_number` as a, `t_str` as b order by a.id, b.id;
desc v_int4_string; -- expect all bigint unsigned
drop view v_int4_string;

create view v_uint4_string as select a.`uint4` >> b.`char` as uint4_char, a.`uint4` >> b.`varchar` as uint4_varchar, a.`uint4` >> b.`binary` as uint4_binary, a.`uint4` >> b.`varbinary` as uint4_varbinary, a.`uint4` >> b.`text` as uint4_text from `t_number` as a, `t_str` as b order by a.id, b.id;
desc v_uint4_string; -- expect all bigint unsigned
drop view v_uint4_string;

create view v_int8_string as select a.`int8` >> b.`char` as int8_char, a.`int8` >> b.`varchar` as int8_varchar, a.`int8` >> b.`binary` as int8_binary, a.`int8` >> b.`varbinary` as int8_varbinary, a.`int8` >> b.`text` as int8_text from `t_number` as a, `t_str` as b order by a.id, b.id;
desc v_int8_string; -- expect all bigint unsigned
drop view v_int8_string;

create view v_uint8_string as select a.`uint8` >> b.`char` as uint8_char, a.`uint8` >> b.`varchar` as uint8_varchar, a.`uint8` >> b.`binary` as uint8_binary, a.`uint8` >> b.`varbinary` as uint8_varbinary, a.`uint8` >> b.`text` as uint8_text from `t_number` as a, `t_str` as b order by a.id, b.id;
desc v_uint8_string;
drop view v_uint8_string;

create view v_float4_string as select a.`float4` >> b.`char` as float4_char, a.`float4` >> b.`varchar` as float4_varchar, a.`float4` >> b.`binary` as float4_binary, a.`float4` >> b.`varbinary` as float4_varbinary, a.`float4` >> b.`text` as float4_text from `t_number` as a, `t_str` as b order by a.id, b.id;
desc v_float4_string; -- expect all bigint unsigned
drop view v_float4_string;

create view v_float8_string as select a.`float8` >> b.`char` as float8_char, a.`float8` >> b.`varchar` as float8_varchar, a.`float8` >> b.`binary` as float8_binary, a.`float8` >> b.`varbinary` as float8_varbinary, a.`float8` >> b.`text` as float8_text from `t_number` as a, `t_str` as b order by a.id, b.id;
desc v_float8_string; -- expect all bigint unsigned
drop view v_float8_string;

create view v_numeric_string as select a.`numeric` >> b.`char` as numeric_char, a.`numeric` >> b.`varchar` as numeric_varchar, a.`numeric` >> b.`binary` as numeric_binary, a.`numeric` >> b.`varbinary` as numeric_varbinary, a.`numeric` >> b.`text` as numeric_text from `t_number` as a, `t_str` as b order by a.id, b.id;
desc v_numeric_string; -- expect all bigint unsigned、numeric >> binary error
drop view v_numeric_string;

create view v_boolean_string as select a.`boolean` >> b.`char` as boolean_char, a.`boolean` >> b.`varchar` as boolean_varchar, a.`boolean` >> b.`binary` as boolean_binary, a.`boolean` >> b.`varbinary` as boolean_varbinary, a.`boolean` >> b.`text` as boolean_text from `t_number` as a, `t_str` as b order by a.id, b.id;
desc v_boolean_string;
drop view v_boolean_string;

create view v_int1_bit as select a.`int1` >> b.`bit1` as int1_bit1, a.`int1` >> b.`bit8` as int1_bit8, a.`int1` >> b.`bit15` as int1_bit15, a.`int1` >> b.`bit64` as int1_bit64 from `t_number` as a, `t_bit` as b order by a.id, b.id;
desc v_int1_bit;
drop view v_int1_bit;

create view v_uint1_bit as select a.`uint1` >> b.`bit1` as uint1_bit1, a.`uint1` >> b.`bit8` as uint1_bit8, a.`uint1` >> b.`bit15` as uint1_bit15, a.`uint1` >> b.`bit64` as uint1_bit64 from `t_number` as a, `t_bit` as b order by a.id, b.id;
desc v_uint1_bit;
drop view v_uint1_bit;

create view v_int2_bit as select a.`int2` >> b.`bit1` as int2_bit1, a.`int2` >> b.`bit8` as int2_bit8, a.`int2` >> b.`bit15` as int2_bit15, a.`int2` >> b.`bit64` as int2_bit64 from `t_number` as a, `t_bit` as b order by a.id, b.id;
desc v_int2_bit;
drop view v_int2_bit;

create view v_uint2_bit as select a.`uint2` >> b.`bit1` as uint2_bit1, a.`uint2` >> b.`bit8` as uint2_bit8, a.`uint2` >> b.`bit15` as uint2_bit15, a.`uint2` >> b.`bit64` as uint2_bit64 from `t_number` as a, `t_bit` as b order by a.id, b.id;
desc v_uint2_bit;
drop view v_uint2_bit;

create view v_int4_bit as select a.`int4` >> b.`bit1` as int4_bit1, a.`int4` >> b.`bit8` as int4_bit8, a.`int4` >> b.`bit15` as int4_bit15, a.`int4` >> b.`bit64` as int4_bit64 from `t_number` as a, `t_bit` as b order by a.id, b.id;
desc v_int4_bit;
drop view v_int4_bit;

create view v_uint4_bit as select a.`uint4` >> b.`bit1` as uint4_bit1, a.`uint4` >> b.`bit8` as uint4_bit8, a.`uint4` >> b.`bit15` as uint4_bit15, a.`uint4` >> b.`bit64` as uint4_bit64 from `t_number` as a, `t_bit` as b order by a.id, b.id;
desc v_uint4_bit;
drop view v_uint4_bit;

create view v_int8_bit as select a.`int8` >> b.`bit1` as int8_bit1, a.`int8` >> b.`bit8` as int8_bit8, a.`int8` >> b.`bit15` as int8_bit15, a.`int8` >> b.`bit64` as int8_bit64 from `t_number` as a, `t_bit` as b order by a.id, b.id;
desc v_int8_bit;
drop view v_int8_bit;

create view v_uint8_bit as select a.`uint8` >> b.`bit1` as uint8_bit1, a.`uint8` >> b.`bit8` as uint8_bit8, a.`uint8` >> b.`bit15` as uint8_bit15, a.`uint8` >> b.`bit64` as uint8_bit64 from `t_number` as a, `t_bit` as b order by a.id, b.id;
desc v_uint8_bit;
drop view v_uint8_bit;

create view v_float4_bit as select a.`float4` >> b.`bit1` as float4_bit1, a.`float4` >> b.`bit8` as float4_bit8, a.`float4` >> b.`bit15` as float4_bit15, a.`float4` >> b.`bit64` as float4_bit64 from `t_number` as a, `t_bit` as b order by a.id, b.id;
desc v_float4_bit;
drop view v_float4_bit;

create view v_float8_bit as select a.`float8` >> b.`bit1` as float8_bit1, a.`float8` >> b.`bit8` as float8_bit8, a.`float8` >> b.`bit15` as float8_bit15, a.`float8` >> b.`bit64` as float8_bit64 from `t_number` as a, `t_bit` as b order by a.id, b.id;
desc v_float8_bit;
drop view v_float8_bit;

create view v_numeric_bit as select a.`numeric` >> b.`bit1` as numeric_bit1, a.`numeric` >> b.`bit8` as numeric_bit8, a.`numeric` >> b.`bit15` as numeric_bit15, a.`numeric` >> b.`bit64` as numeric_bit64 from `t_number` as a, `t_bit` as b order by a.id, b.id;
desc v_numeric_bit;
drop view v_numeric_bit;

create view v_boolean_bit as select a.`boolean` >> b.`bit1` as boolean_bit1, a.`boolean` >> b.`bit8` as boolean_bit8, a.`boolean` >> b.`bit15` as boolean_bit15, a.`boolean` >> b.`bit64` as boolean_bit64 from `t_number` as a, `t_bit` as b order by a.id, b.id;
desc v_boolean_bit;
drop view v_boolean_bit;

create view v_char_string as select a.`char` >> b.`char` as char_char, a.`char` >> b.`varchar` as char_varchar, a.`char` >> b.`binary` as char_binary, a.`char` >> b.`varbinary` as char_varbinary, a.`char` >> b.`text` as char_text from `t_str` as a, `t_str` as b order by a.id, b.id;
desc v_char_string;
drop view v_char_string;

create view v_varchar_string as select a.`varchar` >> b.`char` as varchar_char, a.`varchar` >> b.`varchar` as varchar_varchar, a.`varchar` >> b.`binary` as varchar_binary, a.`varchar` >> b.`varbinary` as varchar_varbinary, a.`varchar` >> b.`text` as varchar_text from `t_str` as a, `t_str` as b order by a.id, b.id;
desc v_varchar_string;
drop view v_varchar_string;

create view v_binary_string as select a.`binary` >> b.`char` as binary_char, a.`binary` >> b.`varchar` as binary_varchar, a.`binary` >> b.`binary` as binary_binary, a.`binary` >> b.`varbinary` as binary_varbinary, a.`binary` >> b.`text` as binary_text from `t_str` as a, `t_str` as b order by a.id, b.id;
desc v_binary_string;
drop view v_binary_string;

create view v_varbinary_string as select a.`varbinary` >> b.`char` as varbinary_char, a.`varbinary` >> b.`varchar` as varbinary_varchar, a.`varbinary` >> b.`binary` as varbinary_binary, a.`varbinary` >> b.`varbinary` as varbinary_varbinary, a.`varbinary` >> b.`text` as varbinary_text from `t_str` as a, `t_str` as b order by a.id, b.id;
desc v_varbinary_string;
drop view v_varbinary_string;

create view v_text_string as select a.`text` >> b.`char` as text_char, a.`text` >> b.`varchar` as text_varchar, a.`text` >> b.`binary` as text_binary, a.`text` >> b.`varbinary` as text_varbinary, a.`text` >> b.`text` as text_text from `t_str` as a, `t_str` as b order by a.id, b.id;
desc v_text_string;
drop view v_text_string;

create view v_char_bit as select a.`char` >> b.`bit1` as char_bit1, a.`char` >> b.`bit8` as char_bit8, a.`char` >> b.`bit15` as char_bit15, a.`char` >> b.`bit64` as char_bit64 from `t_str` as a, `t_bit` as b order by a.id, b.id;
desc v_char_bit;
drop view v_char_bit;

create view v_varchar_bit as select a.`varchar` >> b.`bit1` as varchar_bit1, a.`varchar` >> b.`bit8` as varchar_bit8, a.`varchar` >> b.`bit15` as varchar_bit15, a.`varchar` >> b.`bit64` as varchar_bit64 from `t_str` as a, `t_bit` as b order by a.id, b.id;
desc v_varchar_bit;
drop view v_varchar_bit;

create view v_binary_bit as select a.`binary` >> b.`bit1` as binary_bit1, a.`binary` >> b.`bit8` as binary_bit8, a.`binary` >> b.`bit15` as binary_bit15, a.`binary` >> b.`bit64` as binary_bit64 from `t_str` as a, `t_bit` as b order by a.id, b.id;
desc v_binary_bit;
drop view v_binary_bit;

create view v_varbinary_bit as select a.`varbinary` >> b.`bit1` as varbinary_bit1, a.`varbinary` >> b.`bit8` as varbinary_bit8, a.`varbinary` >> b.`bit15` as varbinary_bit15, a.`varbinary` >> b.`bit64` as varbinary_bit64 from `t_str` as a, `t_bit` as b order by a.id, b.id;
desc v_varbinary_bit;
drop view v_varbinary_bit;

create view v_text_bit as select a.`text` >> b.`bit1` as text_bit1, a.`text` >> b.`bit8` as text_bit8, a.`text` >> b.`bit15` as text_bit15, a.`text` >> b.`bit64` as text_bit64 from `t_str` as a, `t_bit` as b order by a.id, b.id;
desc v_text_bit;
drop view v_text_bit;

create view v_bit1_bit as select a.`bit1` >> b.`bit1` as bit1_bit1, a.`bit1` >> b.`bit8` as bit1_bit8, a.`bit1` >> b.`bit15` as bit1_bit15, a.`bit1` >> b.`bit64` as bit1_bit64 from `t_bit` as a, `t_bit` as b order by a.id, b.id;
desc v_bit1_bit;
drop view v_bit1_bit;

create view v_bit8_bit as select a.`bit8` >> b.`bit1` as bit8_bit1, a.`bit8` >> b.`bit8` as bit8_bit8, a.`bit8` >> b.`bit15` as bit8_bit15, a.`bit8` >> b.`bit64` as bit8_bit64 from `t_bit` as a, `t_bit` as b order by a.id, b.id;
desc v_bit8_bit;
drop view v_bit8_bit;

create view v_bit15_bit as select a.`bit15` >> b.`bit1` as bit15_bit1, a.`bit15` >> b.`bit8` as bit15_bit8, a.`bit15` >> b.`bit15` as bit15_bit15, a.`bit15` >> b.`bit64` as bit15_bit64 from `t_bit` as a, `t_bit` as b order by a.id, b.id;
desc v_bit15_bit;
drop view v_bit15_bit;

create view v_bit64_bit as select a.`bit64` >> b.`bit1` as bit64_bit1, a.`bit64` >> b.`bit8` as bit64_bit8, a.`bit64` >> b.`bit15` as bit64_bit15, a.`bit64` >> b.`bit64` as bit64_bit64 from `t_bit` as a, `t_bit` as b order by a.id, b.id;
desc v_bit64_bit;
drop view v_bit64_bit;

-- test >> result
select a.id as a_id, b.id as b_id, a.`int1` >> b.`int1` as int1_int1, a.`int1` >> b.`uint1` as int1_uint1, a.`int1` >> b.`int2` as int1_int2, a.`int1` >> b.`uint2` as int1_uint2, a.`int1` >> b.`int4` as int1_int4, a.`int1` >> b.`uint4` as int1_uint4, a.`int1` >> b.`int8` as int1_int8, a.`int1` >> b.`uint8` as int1_uint8, a.`int1` >> b.`float4` as int1_float4, a.`int1` >> b.`float8` as int1_float8, a.`int1` >> b.`numeric` as int1_numeric, a.`int1` >> b.`boolean` as int1_boolean from t_number as a,t_number as b order by a.id, b.id;
select a.id as a_id, b.id as b_id, a.`uint1` >> b.`int1` as uint1_int1, a.`uint1` >> b.`uint1` as uint1_uint1, a.`uint1` >> b.`int2` as uint1_int2, a.`uint1` >> b.`uint2` as uint1_uint2, a.`uint1` >> b.`int4` as uint1_int4, a.`uint1` >> b.`uint4` as uint1_uint4, a.`uint1` >> b.`int8` as uint1_int8, a.`uint1` >> b.`uint8` as uint1_uint8, a.`uint1` >> b.`float4` as uint1_float4, a.`uint1` >> b.`float8` as uint1_float8, a.`uint1` >> b.`numeric` as uint1_numeric, a.`uint1` >> b.`boolean` as uint1_boolean from t_number as a,t_number as b order by a.id, b.id;
select a.id as a_id, b.id as b_id, a.`int2` >> b.`int1` as int2_int1, a.`int2` >> b.`uint1` as int2_uint1, a.`int2` >> b.`int2` as int2_int2, a.`int2` >> b.`uint2` as int2_uint2, a.`int2` >> b.`int4` as int2_int4, a.`int2` >> b.`uint4` as int2_uint4, a.`int2` >> b.`int8` as int2_int8, a.`int2` >> b.`uint8` as int2_uint8, a.`int2` >> b.`float4` as int2_float4, a.`int2` >> b.`float8` as int2_float8, a.`int2` >> b.`numeric` as int2_numeric, a.`int2` >> b.`boolean` as int2_boolean from t_number as a,t_number as b order by a.id, b.id;
select a.id as a_id, b.id as b_id, a.`uint2` >> b.`int1` as uint2_int1, a.`uint2` >> b.`uint1` as uint2_uint1, a.`uint2` >> b.`int2` as uint2_int2, a.`uint2` >> b.`uint2` as uint2_uint2, a.`uint2` >> b.`int4` as uint2_int4, a.`uint2` >> b.`uint4` as uint2_uint4, a.`uint2` >> b.`int8` as uint2_int8, a.`uint2` >> b.`uint8` as uint2_uint8, a.`uint2` >> b.`float4` as uint2_float4, a.`uint2` >> b.`float8` as uint2_float8, a.`uint2` >> b.`numeric` as uint2_numeric, a.`uint2` >> b.`boolean` as uint2_boolean from t_number as a,t_number as b order by a.id, b.id;
select a.id as a_id, b.id as b_id, a.`int4` >> b.`int1` as int4_int1, a.`int4` >> b.`uint1` as int4_uint1, a.`int4` >> b.`int2` as int4_int2, a.`int4` >> b.`uint2` as int4_uint2, a.`int4` >> b.`int4` as int4_int4, a.`int4` >> b.`uint4` as int4_uint4, a.`int4` >> b.`int8` as int4_int8, a.`int4` >> b.`uint8` as int4_uint8, a.`int4` >> b.`float4` as int4_float4, a.`int4` >> b.`float8` as int4_float8, a.`int4` >> b.`numeric` as int4_numeric, a.`int4` >> b.`boolean` as int4_boolean from t_number as a,t_number as b order by a.id, b.id;
select a.id as a_id, b.id as b_id, a.`uint4` >> b.`int1` as uint4_int1, a.`uint4` >> b.`uint1` as uint4_uint1, a.`uint4` >> b.`int2` as uint4_int2, a.`uint4` >> b.`uint2` as uint4_uint2, a.`uint4` >> b.`int4` as uint4_int4, a.`uint4` >> b.`uint4` as uint4_uint4, a.`uint4` >> b.`int8` as uint4_int8, a.`uint4` >> b.`uint8` as uint4_uint8, a.`uint4` >> b.`float4` as uint4_float4, a.`uint4` >> b.`float8` as uint4_float8, a.`uint4` >> b.`numeric` as uint4_numeric, a.`uint4` >> b.`boolean` as uint4_boolean from t_number as a,t_number as b order by a.id, b.id;
select a.id as a_id, b.id as b_id, a.`int8` >> b.`int1` as int8_int1, a.`int8` >> b.`uint1` as int8_uint1, a.`int8` >> b.`int2` as int8_int2, a.`int8` >> b.`uint2` as int8_uint2, a.`int8` >> b.`int4` as int8_int4, a.`int8` >> b.`uint4` as int8_uint4, a.`int8` >> b.`int8` as int8_int8, a.`int8` >> b.`uint8` as int8_uint8, a.`int8` >> b.`float4` as int8_float4, a.`int8` >> b.`float8` as int8_float8, a.`int8` >> b.`numeric` as int8_numeric, a.`int8` >> b.`boolean` as int8_boolean from t_number as a,t_number as b order by a.id, b.id;
select a.id as a_id, b.id as b_id, a.`uint8` >> b.`int1` as uint8_int1, a.`uint8` >> b.`uint1` as uint8_uint1, a.`uint8` >> b.`int2` as uint8_int2, a.`uint8` >> b.`uint2` as uint8_uint2, a.`uint8` >> b.`int4` as uint8_int4, a.`uint8` >> b.`uint4` as uint8_uint4, a.`uint8` >> b.`int8` as uint8_int8, a.`uint8` >> b.`uint8` as uint8_uint8, a.`uint8` >> b.`float4` as uint8_float4, a.`uint8` >> b.`float8` as uint8_float8, a.`uint8` >> b.`numeric` as uint8_numeric, a.`uint8` >> b.`boolean` as uint8_boolean from t_number as a,t_number as b order by a.id, b.id;
select a.id as a_id, b.id as b_id, a.`float4` >> b.`int1` as float4_int1, a.`float4` >> b.`uint1` as float4_uint1, a.`float4` >> b.`int2` as float4_int2, a.`float4` >> b.`uint2` as float4_uint2, a.`float4` >> b.`int4` as float4_int4, a.`float4` >> b.`uint4` as float4_uint4, a.`float4` >> b.`int8` as float4_int8, a.`float4` >> b.`uint8` as float4_uint8, a.`float4` >> b.`float4` as float4_float4, a.`float4` >> b.`float8` as float4_float8, a.`float4` >> b.`numeric` as float4_numeric, a.`float4` >> b.`boolean` as float4_boolean from t_number as a,t_number as b order by a.id, b.id;
select a.id as a_id, b.id as b_id, a.`float8` >> b.`int1` as float8_int1, a.`float8` >> b.`uint1` as float8_uint1, a.`float8` >> b.`int2` as float8_int2, a.`float8` >> b.`uint2` as float8_uint2, a.`float8` >> b.`int4` as float8_int4, a.`float8` >> b.`uint4` as float8_uint4, a.`float8` >> b.`int8` as float8_int8, a.`float8` >> b.`uint8` as float8_uint8, a.`float8` >> b.`float4` as float8_float4, a.`float8` >> b.`float8` as float8_float8, a.`float8` >> b.`numeric` as float8_numeric, a.`float8` >> b.`boolean` as float8_boolean from t_number as a,t_number as b order by a.id, b.id;
select a.id as a_id, b.id as b_id, a.`numeric` >> b.`int1` as numeric_int1, a.`numeric` >> b.`uint1` as numeric_uint1, a.`numeric` >> b.`int2` as numeric_int2, a.`numeric` >> b.`uint2` as numeric_uint2, a.`numeric` >> b.`int4` as numeric_int4, a.`numeric` >> b.`uint4` as numeric_uint4, a.`numeric` >> b.`int8` as numeric_int8, a.`numeric` >> b.`uint8` as numeric_uint8, a.`numeric` >> b.`float4` as numeric_float4, a.`numeric` >> b.`float8` as numeric_float8, a.`numeric` >> b.`numeric` as numeric_numeric, a.`numeric` >> b.`boolean` as numeric_boolean from t_number as a,t_number as b order by a.id, b.id;
select a.id as a_id, b.id as b_id, a.`boolean` >> b.`int1` as boolean_int1, a.`boolean` >> b.`uint1` as boolean_uint1, a.`boolean` >> b.`int2` as boolean_int2, a.`boolean` >> b.`uint2` as boolean_uint2, a.`boolean` >> b.`int4` as boolean_int4, a.`boolean` >> b.`uint4` as boolean_uint4, a.`boolean` >> b.`int8` as boolean_int8, a.`boolean` >> b.`uint8` as boolean_uint8, a.`boolean` >> b.`float4` as boolean_float4, a.`boolean` >> b.`float8` as boolean_float8, a.`boolean` >> b.`numeric` as boolean_numeric, a.`boolean` >> b.`boolean` as boolean_boolean from t_number as a,t_number as b order by a.id, b.id;
select a.id as a_id, b.id as b_id, a.`int1` >> b.`char` as int1_char, a.`int1` >> b.`varchar` as int1_varchar, a.`int1` >> b.`binary` as int1_binary, a.`int1` >> b.`varbinary` as int1_varbinary, a.`int1` >> b.`text` as int1_text from t_number as a,t_str as b order by a.id, b.id;
select a.id as a_id, b.id as b_id, a.`uint1` >> b.`char` as uint1_char, a.`uint1` >> b.`varchar` as uint1_varchar, a.`uint1` >> b.`binary` as uint1_binary, a.`uint1` >> b.`varbinary` as uint1_varbinary, a.`uint1` >> b.`text` as uint1_text from t_number as a,t_str as b order by a.id, b.id;
select a.id as a_id, b.id as b_id, a.`int2` >> b.`char` as int2_char, a.`int2` >> b.`varchar` as int2_varchar, a.`int2` >> b.`binary` as int2_binary, a.`int2` >> b.`varbinary` as int2_varbinary, a.`int2` >> b.`text` as int2_text from t_number as a,t_str as b order by a.id, b.id;
select a.id as a_id, b.id as b_id, a.`uint2` >> b.`char` as uint2_char, a.`uint2` >> b.`varchar` as uint2_varchar, a.`uint2` >> b.`binary` as uint2_binary, a.`uint2` >> b.`varbinary` as uint2_varbinary, a.`uint2` >> b.`text` as uint2_text from t_number as a,t_str as b order by a.id, b.id;
select a.id as a_id, b.id as b_id, a.`int4` >> b.`char` as int4_char, a.`int4` >> b.`varchar` as int4_varchar, a.`int4` >> b.`binary` as int4_binary, a.`int4` >> b.`varbinary` as int4_varbinary, a.`int4` >> b.`text` as int4_text from t_number as a,t_str as b order by a.id, b.id;
select a.id as a_id, b.id as b_id, a.`uint4` >> b.`char` as uint4_char, a.`uint4` >> b.`varchar` as uint4_varchar, a.`uint4` >> b.`binary` as uint4_binary, a.`uint4` >> b.`varbinary` as uint4_varbinary, a.`uint4` >> b.`text` as uint4_text from t_number as a,t_str as b order by a.id, b.id;
select a.id as a_id, b.id as b_id, a.`int8` >> b.`char` as int8_char, a.`int8` >> b.`varchar` as int8_varchar, a.`int8` >> b.`binary` as int8_binary, a.`int8` >> b.`varbinary` as int8_varbinary, a.`int8` >> b.`text` as int8_text from t_number as a,t_str as b order by a.id, b.id;
select a.id as a_id, b.id as b_id, a.`uint8` >> b.`char` as uint8_char, a.`uint8` >> b.`varchar` as uint8_varchar, a.`uint8` >> b.`binary` as uint8_binary, a.`uint8` >> b.`varbinary` as uint8_varbinary, a.`uint8` >> b.`text` as uint8_text from t_number as a,t_str as b order by a.id, b.id;
select a.id as a_id, b.id as b_id, a.`float4` >> b.`char` as float4_char, a.`float4` >> b.`varchar` as float4_varchar, a.`float4` >> b.`binary` as float4_binary, a.`float4` >> b.`varbinary` as float4_varbinary, a.`float4` >> b.`text` as float4_text from t_number as a,t_str as b order by a.id, b.id;
select a.id as a_id, b.id as b_id, a.`float8` >> b.`char` as float8_char, a.`float8` >> b.`varchar` as float8_varchar, a.`float8` >> b.`binary` as float8_binary, a.`float8` >> b.`varbinary` as float8_varbinary, a.`float8` >> b.`text` as float8_text from t_number as a,t_str as b order by a.id, b.id;
select a.id as a_id, b.id as b_id, a.`numeric` >> b.`char` as numeric_char, a.`numeric` >> b.`varchar` as numeric_varchar, a.`numeric` >> b.`binary` as numeric_binary, a.`numeric` >> b.`varbinary` as numeric_varbinary, a.`numeric` >> b.`text` as numeric_text from t_number as a,t_str as b order by a.id, b.id;
select a.id as a_id, b.id as b_id, a.`boolean` >> b.`char` as boolean_char, a.`boolean` >> b.`varchar` as boolean_varchar, a.`boolean` >> b.`binary` as boolean_binary, a.`boolean` >> b.`varbinary` as boolean_varbinary, a.`boolean` >> b.`text` as boolean_text from t_number as a,t_str as b order by a.id, b.id;
select a.id as a_id, b.id as b_id, a.`int1` >> b.`bit1` as int1_bit1, a.`int1` >> b.`bit8` as int1_bit8, a.`int1` >> b.`bit15` as int1_bit15, a.`int1` >> b.`bit64` as int1_bit64 from t_number as a,t_bit as b order by a.id, b.id;
select a.id as a_id, b.id as b_id, a.`uint1` >> b.`bit1` as uint1_bit1, a.`uint1` >> b.`bit8` as uint1_bit8, a.`uint1` >> b.`bit15` as uint1_bit15, a.`uint1` >> b.`bit64` as uint1_bit64 from t_number as a,t_bit as b order by a.id, b.id;
select a.id as a_id, b.id as b_id, a.`int2` >> b.`bit1` as int2_bit1, a.`int2` >> b.`bit8` as int2_bit8, a.`int2` >> b.`bit15` as int2_bit15, a.`int2` >> b.`bit64` as int2_bit64 from t_number as a,t_bit as b order by a.id, b.id;
select a.id as a_id, b.id as b_id, a.`uint2` >> b.`bit1` as uint2_bit1, a.`uint2` >> b.`bit8` as uint2_bit8, a.`uint2` >> b.`bit15` as uint2_bit15, a.`uint2` >> b.`bit64` as uint2_bit64 from t_number as a,t_bit as b order by a.id, b.id;
select a.id as a_id, b.id as b_id, a.`int4` >> b.`bit1` as int4_bit1, a.`int4` >> b.`bit8` as int4_bit8, a.`int4` >> b.`bit15` as int4_bit15, a.`int4` >> b.`bit64` as int4_bit64 from t_number as a,t_bit as b order by a.id, b.id;
select a.id as a_id, b.id as b_id, a.`uint4` >> b.`bit1` as uint4_bit1, a.`uint4` >> b.`bit8` as uint4_bit8, a.`uint4` >> b.`bit15` as uint4_bit15, a.`uint4` >> b.`bit64` as uint4_bit64 from t_number as a,t_bit as b order by a.id, b.id;
select a.id as a_id, b.id as b_id, a.`int8` >> b.`bit1` as int8_bit1, a.`int8` >> b.`bit8` as int8_bit8, a.`int8` >> b.`bit15` as int8_bit15, a.`int8` >> b.`bit64` as int8_bit64 from t_number as a,t_bit as b order by a.id, b.id;
select a.id as a_id, b.id as b_id, a.`uint8` >> b.`bit1` as uint8_bit1, a.`uint8` >> b.`bit8` as uint8_bit8, a.`uint8` >> b.`bit15` as uint8_bit15, a.`uint8` >> b.`bit64` as uint8_bit64 from t_number as a,t_bit as b order by a.id, b.id;
select a.id as a_id, b.id as b_id, a.`float4` >> b.`bit1` as float4_bit1, a.`float4` >> b.`bit8` as float4_bit8, a.`float4` >> b.`bit15` as float4_bit15, a.`float4` >> b.`bit64` as float4_bit64 from t_number as a,t_bit as b order by a.id, b.id;
select a.id as a_id, b.id as b_id, a.`float8` >> b.`bit1` as float8_bit1, a.`float8` >> b.`bit8` as float8_bit8, a.`float8` >> b.`bit15` as float8_bit15, a.`float8` >> b.`bit64` as float8_bit64 from t_number as a,t_bit as b order by a.id, b.id;
select a.id as a_id, b.id as b_id, a.`numeric` >> b.`bit1` as numeric_bit1, a.`numeric` >> b.`bit8` as numeric_bit8, a.`numeric` >> b.`bit15` as numeric_bit15, a.`numeric` >> b.`bit64` as numeric_bit64 from t_number as a,t_bit as b order by a.id, b.id;
select a.id as a_id, b.id as b_id, a.`boolean` >> b.`bit1` as boolean_bit1, a.`boolean` >> b.`bit8` as boolean_bit8, a.`boolean` >> b.`bit15` as boolean_bit15, a.`boolean` >> b.`bit64` as boolean_bit64 from t_number as a,t_bit as b order by a.id, b.id;
select a.id as a_id, b.id as b_id, a.`char` >> b.`char` as char_char, a.`char` >> b.`varchar` as char_varchar, a.`char` >> b.`binary` as char_binary, a.`char` >> b.`varbinary` as char_varbinary, a.`char` >> b.`text` as char_text from t_str as a,t_str as b order by a.id, b.id;
select a.id as a_id, b.id as b_id, a.`varchar` >> b.`char` as varchar_char, a.`varchar` >> b.`varchar` as varchar_varchar, a.`varchar` >> b.`binary` as varchar_binary, a.`varchar` >> b.`varbinary` as varchar_varbinary, a.`varchar` >> b.`text` as varchar_text from t_str as a,t_str as b order by a.id, b.id;
select a.id as a_id, b.id as b_id, a.`binary` >> b.`char` as binary_char, a.`binary` >> b.`varchar` as binary_varchar, a.`binary` >> b.`binary` as binary_binary, a.`binary` >> b.`varbinary` as binary_varbinary, a.`binary` >> b.`text` as binary_text from t_str as a,t_str as b order by a.id, b.id;
select a.id as a_id, b.id as b_id, a.`varbinary` >> b.`char` as varbinary_char, a.`varbinary` >> b.`varchar` as varbinary_varchar, a.`varbinary` >> b.`binary` as varbinary_binary, a.`varbinary` >> b.`varbinary` as varbinary_varbinary, a.`varbinary` >> b.`text` as varbinary_text from t_str as a,t_str as b order by a.id, b.id;
select a.id as a_id, b.id as b_id, a.`text` >> b.`char` as text_char, a.`text` >> b.`varchar` as text_varchar, a.`text` >> b.`binary` as text_binary, a.`text` >> b.`varbinary` as text_varbinary, a.`text` >> b.`text` as text_text from t_str as a,t_str as b order by a.id, b.id;
select a.id as a_id, b.id as b_id, a.`char` >> b.`bit1` as char_bit1, a.`char` >> b.`bit8` as char_bit8, a.`char` >> b.`bit15` as char_bit15, a.`char` >> b.`bit64` as char_bit64 from t_str as a,t_bit as b order by a.id, b.id;
select a.id as a_id, b.id as b_id, a.`varchar` >> b.`bit1` as varchar_bit1, a.`varchar` >> b.`bit8` as varchar_bit8, a.`varchar` >> b.`bit15` as varchar_bit15, a.`varchar` >> b.`bit64` as varchar_bit64 from t_str as a,t_bit as b order by a.id, b.id;
select a.id as a_id, b.id as b_id, a.`binary` >> b.`bit1` as binary_bit1, a.`binary` >> b.`bit8` as binary_bit8, a.`binary` >> b.`bit15` as binary_bit15, a.`binary` >> b.`bit64` as binary_bit64 from t_str as a,t_bit as b order by a.id, b.id;
select a.id as a_id, b.id as b_id, a.`varbinary` >> b.`bit1` as varbinary_bit1, a.`varbinary` >> b.`bit8` as varbinary_bit8, a.`varbinary` >> b.`bit15` as varbinary_bit15, a.`varbinary` >> b.`bit64` as varbinary_bit64 from t_str as a,t_bit as b order by a.id, b.id;
select a.id as a_id, b.id as b_id, a.`text` >> b.`bit1` as text_bit1, a.`text` >> b.`bit8` as text_bit8, a.`text` >> b.`bit15` as text_bit15, a.`text` >> b.`bit64` as text_bit64 from t_str as a,t_bit as b order by a.id, b.id;
select a.id as a_id, b.id as b_id, a.`bit1` >> b.`bit1` as bit1_bit1, a.`bit1` >> b.`bit8` as bit1_bit8, a.`bit1` >> b.`bit15` as bit1_bit15, a.`bit1` >> b.`bit64` as bit1_bit64 from t_bit as a,t_bit as b order by a.id, b.id;
select a.id as a_id, b.id as b_id, a.`bit8` >> b.`bit1` as bit8_bit1, a.`bit8` >> b.`bit8` as bit8_bit8, a.`bit8` >> b.`bit15` as bit8_bit15, a.`bit8` >> b.`bit64` as bit8_bit64 from t_bit as a,t_bit as b order by a.id, b.id;
select a.id as a_id, b.id as b_id, a.`bit15` >> b.`bit1` as bit15_bit1, a.`bit15` >> b.`bit8` as bit15_bit8, a.`bit15` >> b.`bit15` as bit15_bit15, a.`bit15` >> b.`bit64` as bit15_bit64 from t_bit as a,t_bit as b order by a.id, b.id;
select a.id as a_id, b.id as b_id, a.`bit64` >> b.`bit1` as bit64_bit1, a.`bit64` >> b.`bit8` as bit64_bit8, a.`bit64` >> b.`bit15` as bit64_bit15, a.`bit64` >> b.`bit64` as bit64_bit64 from t_bit as a,t_bit as b order by a.id, b.id;


-- test << result_type
create view v_int1_number as select a.`int1` << b.`int1` as int1_int1, a.`int1` << b.`uint1` as int1_uint1, a.`int1` << b.`int2` as int1_int2, a.`int1` << b.`uint2` as int1_uint2, a.`int1` << b.`int4` as int1_int4, a.`int1` << b.`uint4` as int1_uint4, a.`int1` << b.`int8` as int1_int8, a.`int1` << b.`uint8` as int1_uint8, a.`int1` << b.`float4` as int1_float4, a.`int1` << b.`float8` as int1_float8, a.`int1` << b.`numeric` as int1_numeric, a.`int1` << b.`boolean` as int1_boolean from `t_number` as a, `t_number` as b order by a.id, b.id;
desc v_int1_number; -- expect all bigint unsigned
drop view v_int1_number;

create view v_uint1_number as select a.`uint1` << b.`int1` as uint1_int1, a.`uint1` << b.`uint1` as uint1_uint1, a.`uint1` << b.`int2` as uint1_int2, a.`uint1` << b.`uint2` as uint1_uint2, a.`uint1` << b.`int4` as uint1_int4, a.`uint1` << b.`uint4` as uint1_uint4, a.`uint1` << b.`int8` as uint1_int8, a.`uint1` << b.`uint8` as uint1_uint8, a.`uint1` << b.`float4` as uint1_float4, a.`uint1` << b.`float8` as uint1_float8, a.`uint1` << b.`numeric` as uint1_numeric, a.`uint1` << b.`boolean` as uint1_boolean from `t_number` as a, `t_number` as b order by a.id, b.id;
desc v_uint1_number;
drop view v_uint1_number;

create view v_int2_number as select a.`int2` << b.`int1` as int2_int1, a.`int2` << b.`uint1` as int2_uint1, a.`int2` << b.`int2` as int2_int2, a.`int2` << b.`uint2` as int2_uint2, a.`int2` << b.`int4` as int2_int4, a.`int2` << b.`uint4` as int2_uint4, a.`int2` << b.`int8` as int2_int8, a.`int2` << b.`uint8` as int2_uint8, a.`int2` << b.`float4` as int2_float4, a.`int2` << b.`float8` as int2_float8, a.`int2` << b.`numeric` as int2_numeric, a.`int2` << b.`boolean` as int2_boolean from `t_number` as a, `t_number` as b order by a.id, b.id;
desc v_int2_number; -- expect all bigint unsigned
drop view v_int2_number;

create view v_uint2_number as select a.`uint2` << b.`int1` as uint2_int1, a.`uint2` << b.`uint1` as uint2_uint1, a.`uint2` << b.`int2` as uint2_int2, a.`uint2` << b.`uint2` as uint2_uint2, a.`uint2` << b.`int4` as uint2_int4, a.`uint2` << b.`uint4` as uint2_uint4, a.`uint2` << b.`int8` as uint2_int8, a.`uint2` << b.`uint8` as uint2_uint8, a.`uint2` << b.`float4` as uint2_float4, a.`uint2` << b.`float8` as uint2_float8, a.`uint2` << b.`numeric` as uint2_numeric, a.`uint2` << b.`boolean` as uint2_boolean from `t_number` as a, `t_number` as b order by a.id, b.id;
desc v_uint2_number;
drop view v_uint2_number;

create view v_int4_number as select a.`int4` << b.`int1` as int4_int1, a.`int4` << b.`uint1` as int4_uint1, a.`int4` << b.`int2` as int4_int2, a.`int4` << b.`uint2` as int4_uint2, a.`int4` << b.`int4` as int4_int4, a.`int4` << b.`uint4` as int4_uint4, a.`int4` << b.`int8` as int4_int8, a.`int4` << b.`uint8` as int4_uint8, a.`int4` << b.`float4` as int4_float4, a.`int4` << b.`float8` as int4_float8, a.`int4` << b.`numeric` as int4_numeric, a.`int4` << b.`boolean` as int4_boolean from `t_number` as a, `t_number` as b order by a.id, b.id;
desc v_int4_number; -- expect all bigint unsigned
drop view v_int4_number;

create view v_uint4_number as select a.`uint4` << b.`int1` as uint4_int1, a.`uint4` << b.`uint1` as uint4_uint1, a.`uint4` << b.`int2` as uint4_int2, a.`uint4` << b.`uint2` as uint4_uint2, a.`uint4` << b.`int4` as uint4_int4, a.`uint4` << b.`uint4` as uint4_uint4, a.`uint4` << b.`int8` as uint4_int8, a.`uint4` << b.`uint8` as uint4_uint8, a.`uint4` << b.`float4` as uint4_float4, a.`uint4` << b.`float8` as uint4_float8, a.`uint4` << b.`numeric` as uint4_numeric, a.`uint4` << b.`boolean` as uint4_boolean from `t_number` as a, `t_number` as b order by a.id, b.id;
desc v_uint4_number;
drop view v_uint4_number;

create view v_int8_number as select a.`int8` << b.`int1` as int8_int1, a.`int8` << b.`uint1` as int8_uint1, a.`int8` << b.`int2` as int8_int2, a.`int8` << b.`uint2` as int8_uint2, a.`int8` << b.`int4` as int8_int4, a.`int8` << b.`uint4` as int8_uint4, a.`int8` << b.`int8` as int8_int8, a.`int8` << b.`uint8` as int8_uint8, a.`int8` << b.`float4` as int8_float4, a.`int8` << b.`float8` as int8_float8, a.`int8` << b.`numeric` as int8_numeric, a.`int8` << b.`boolean` as int8_boolean from `t_number` as a, `t_number` as b order by a.id, b.id;
desc v_int8_number; -- expect all bigint unsigned
drop view v_int8_number;

create view v_uint8_number as select a.`uint8` << b.`int1` as uint8_int1, a.`uint8` << b.`uint1` as uint8_uint1, a.`uint8` << b.`int2` as uint8_int2, a.`uint8` << b.`uint2` as uint8_uint2, a.`uint8` << b.`int4` as uint8_int4, a.`uint8` << b.`uint4` as uint8_uint4, a.`uint8` << b.`int8` as uint8_int8, a.`uint8` << b.`uint8` as uint8_uint8, a.`uint8` << b.`float4` as uint8_float4, a.`uint8` << b.`float8` as uint8_float8, a.`uint8` << b.`numeric` as uint8_numeric, a.`uint8` << b.`boolean` as uint8_boolean from `t_number` as a, `t_number` as b order by a.id, b.id;
desc v_uint8_number;
drop view v_uint8_number;

create view v_float4_number as select a.`float4` << b.`int1` as float4_int1, a.`float4` << b.`uint1` as float4_uint1, a.`float4` << b.`int2` as float4_int2, a.`float4` << b.`uint2` as float4_uint2, a.`float4` << b.`int4` as float4_int4, a.`float4` << b.`uint4` as float4_uint4, a.`float4` << b.`int8` as float4_int8, a.`float4` << b.`uint8` as float4_uint8, a.`float4` << b.`float4` as float4_float4, a.`float4` << b.`float8` as float4_float8, a.`float4` << b.`numeric` as float4_numeric, a.`float4` << b.`boolean` as float4_boolean from `t_number` as a, `t_number` as b order by a.id, b.id;
desc v_float4_number; -- expect all bigint unsigned
drop view v_float4_number;

create view v_float8_number as select a.`float8` << b.`int1` as float8_int1, a.`float8` << b.`uint1` as float8_uint1, a.`float8` << b.`int2` as float8_int2, a.`float8` << b.`uint2` as float8_uint2, a.`float8` << b.`int4` as float8_int4, a.`float8` << b.`uint4` as float8_uint4, a.`float8` << b.`int8` as float8_int8, a.`float8` << b.`uint8` as float8_uint8, a.`float8` << b.`float4` as float8_float4, a.`float8` << b.`float8` as float8_float8, a.`float8` << b.`numeric` as float8_numeric, a.`float8` << b.`boolean` as float8_boolean from `t_number` as a, `t_number` as b order by a.id, b.id;
desc v_float8_number; -- expect all bigint unsigned
drop view v_float8_number;

create view v_numeric_number as select a.`numeric` << b.`int1` as numeric_int1, a.`numeric` << b.`uint1` as numeric_uint1, a.`numeric` << b.`int2` as numeric_int2, a.`numeric` << b.`uint2` as numeric_uint2, a.`numeric` << b.`int4` as numeric_int4, a.`numeric` << b.`uint4` as numeric_uint4, a.`numeric` << b.`int8` as numeric_int8, a.`numeric` << b.`uint8` as numeric_uint8, a.`numeric` << b.`float4` as numeric_float4, a.`numeric` << b.`float8` as numeric_float8, a.`numeric` << b.`numeric` as numeric_numeric, a.`numeric` << b.`boolean` as numeric_boolean from `t_number` as a, `t_number` as b order by a.id, b.id;
desc v_numeric_number; -- expect all bigint unsigned
drop view v_numeric_number;

create view v_boolean_number as select a.`boolean` << b.`int1` as boolean_int1, a.`boolean` << b.`uint1` as boolean_uint1, a.`boolean` << b.`int2` as boolean_int2, a.`boolean` << b.`uint2` as boolean_uint2, a.`boolean` << b.`int4` as boolean_int4, a.`boolean` << b.`uint4` as boolean_uint4, a.`boolean` << b.`int8` as boolean_int8, a.`boolean` << b.`uint8` as boolean_uint8, a.`boolean` << b.`float4` as boolean_float4, a.`boolean` << b.`float8` as boolean_float8, a.`boolean` << b.`numeric` as boolean_numeric, a.`boolean` << b.`boolean` as boolean_boolean from `t_number` as a, `t_number` as b order by a.id, b.id;
desc v_boolean_number; -- expect all bigint unsigned
drop view v_boolean_number;

create view v_int1_string as select a.`int1` << b.`char` as int1_char, a.`int1` << b.`varchar` as int1_varchar, a.`int1` << b.`binary` as int1_binary, a.`int1` << b.`varbinary` as int1_varbinary, a.`int1` << b.`text` as int1_text from `t_number` as a, `t_str` as b order by a.id, b.id;
desc v_int1_string; -- expect all bigint unsigned、int1 << binary error
drop view v_int1_string;

create view v_uint1_string as select a.`uint1` << b.`char` as uint1_char, a.`uint1` << b.`varchar` as uint1_varchar, a.`uint1` << b.`binary` as uint1_binary, a.`uint1` << b.`varbinary` as uint1_varbinary, a.`uint1` << b.`text` as uint1_text from `t_number` as a, `t_str` as b order by a.id, b.id;
desc v_uint1_string; -- uint1 << binary error
drop view v_uint1_string;

create view v_int2_string as select a.`int2` << b.`char` as int2_char, a.`int2` << b.`varchar` as int2_varchar, a.`int2` << b.`binary` as int2_binary, a.`int2` << b.`varbinary` as int2_varbinary, a.`int2` << b.`text` as int2_text from `t_number` as a, `t_str` as b order by a.id, b.id;
desc v_int2_string; -- expect all bigint unsigned、int2 << binary error
drop view v_int2_string;

create view v_uint2_string as select a.`uint2` << b.`char` as uint2_char, a.`uint2` << b.`varchar` as uint2_varchar, a.`uint2` << b.`binary` as uint2_binary, a.`uint2` << b.`varbinary` as uint2_varbinary, a.`uint2` << b.`text` as uint2_text from `t_number` as a, `t_str` as b order by a.id, b.id;
desc v_uint2_string; -- uint2 << binary error
drop view v_uint2_string;

create view v_int4_string as select a.`int4` << b.`char` as int4_char, a.`int4` << b.`varchar` as int4_varchar, a.`int4` << b.`binary` as int4_binary, a.`int4` << b.`varbinary` as int4_varbinary, a.`int4` << b.`text` as int4_text from `t_number` as a, `t_str` as b order by a.id, b.id;
desc v_int4_string; -- expect all bigint unsigned、int4 << binary error
drop view v_int4_string;

create view v_uint4_string as select a.`uint4` << b.`char` as uint4_char, a.`uint4` << b.`varchar` as uint4_varchar, a.`uint4` << b.`binary` as uint4_binary, a.`uint4` << b.`varbinary` as uint4_varbinary, a.`uint4` << b.`text` as uint4_text from `t_number` as a, `t_str` as b order by a.id, b.id;
desc v_uint4_string; -- uint4 << binary error
drop view v_uint4_string;

create view v_int8_string as select a.`int8` << b.`char` as int8_char, a.`int8` << b.`varchar` as int8_varchar, a.`int8` << b.`binary` as int8_binary, a.`int8` << b.`varbinary` as int8_varbinary, a.`int8` << b.`text` as int8_text from `t_number` as a, `t_str` as b order by a.id, b.id;
desc v_int8_string; -- expect all bigint unsigned、int8 << binary error
drop view v_int8_string;

create view v_uint8_string as select a.`uint8` << b.`char` as uint8_char, a.`uint8` << b.`varchar` as uint8_varchar, a.`uint8` << b.`binary` as uint8_binary, a.`uint8` << b.`varbinary` as uint8_varbinary, a.`uint8` << b.`text` as uint8_text from `t_number` as a, `t_str` as b order by a.id, b.id;
desc v_uint8_string; -- uint8 << binary error
drop view v_uint8_string;

create view v_float4_string as select a.`float4` << b.`char` as float4_char, a.`float4` << b.`varchar` as float4_varchar, a.`float4` << b.`binary` as float4_binary, a.`float4` << b.`varbinary` as float4_varbinary, a.`float4` << b.`text` as float4_text from `t_number` as a, `t_str` as b order by a.id, b.id;
desc v_float4_string; -- expect all bigint unsigned、float4 << binary error
drop view v_float4_string;

create view v_float8_string as select a.`float8` << b.`char` as float8_char, a.`float8` << b.`varchar` as float8_varchar, a.`float8` << b.`binary` as float8_binary, a.`float8` << b.`varbinary` as float8_varbinary, a.`float8` << b.`text` as float8_text from `t_number` as a, `t_str` as b order by a.id, b.id;
desc v_float8_string; -- expect all bigint unsigned、float8 << binary error
drop view v_float8_string;

create view v_numeric_string as select a.`numeric` << b.`char` as numeric_char, a.`numeric` << b.`varchar` as numeric_varchar, a.`numeric` << b.`binary` as numeric_binary, a.`numeric` << b.`varbinary` as numeric_varbinary, a.`numeric` << b.`text` as numeric_text from `t_number` as a, `t_str` as b order by a.id, b.id;
desc v_numeric_string; -- expect all bigint unsigned、numeric << binary error
drop view v_numeric_string;

create view v_boolean_string as select a.`boolean` << b.`char` as boolean_char, a.`boolean` << b.`varchar` as boolean_varchar, a.`boolean` << b.`binary` as boolean_binary, a.`boolean` << b.`varbinary` as boolean_varbinary, a.`boolean` << b.`text` as boolean_text from `t_number` as a, `t_str` as b order by a.id, b.id;
desc v_boolean_string; -- expect all bigint unsigned、boolean << binary error
drop view v_boolean_string;

create view v_int1_bit as select a.`int1` << b.`bit1` as int1_bit1, a.`int1` << b.`bit8` as int1_bit8, a.`int1` << b.`bit15` as int1_bit15, a.`int1` << b.`bit64` as int1_bit64 from `t_number` as a, `t_bit` as b order by a.id, b.id;
desc v_int1_bit; -- int << bit error
drop view v_int1_bit;

create view v_uint1_bit as select a.`uint1` << b.`bit1` as uint1_bit1, a.`uint1` << b.`bit8` as uint1_bit8, a.`uint1` << b.`bit15` as uint1_bit15, a.`uint1` << b.`bit64` as uint1_bit64 from `t_number` as a, `t_bit` as b order by a.id, b.id;
desc v_uint1_bit; -- uint << bit error
drop view v_uint1_bit;

create view v_int2_bit as select a.`int2` << b.`bit1` as int2_bit1, a.`int2` << b.`bit8` as int2_bit8, a.`int2` << b.`bit15` as int2_bit15, a.`int2` << b.`bit64` as int2_bit64 from `t_number` as a, `t_bit` as b order by a.id, b.id;
desc v_int2_bit; -- int2 << bit error
drop view v_int2_bit;

create view v_uint2_bit as select a.`uint2` << b.`bit1` as uint2_bit1, a.`uint2` << b.`bit8` as uint2_bit8, a.`uint2` << b.`bit15` as uint2_bit15, a.`uint2` << b.`bit64` as uint2_bit64 from `t_number` as a, `t_bit` as b order by a.id, b.id;
desc v_uint2_bit; -- uint2 << bit error
drop view v_uint2_bit;

create view v_int4_bit as select a.`int4` << b.`bit1` as int4_bit1, a.`int4` << b.`bit8` as int4_bit8, a.`int4` << b.`bit15` as int4_bit15, a.`int4` << b.`bit64` as int4_bit64 from `t_number` as a, `t_bit` as b order by a.id, b.id;
desc v_int4_bit; -- int4 << bit error
drop view v_int4_bit;

create view v_uint4_bit as select a.`uint4` << b.`bit1` as uint4_bit1, a.`uint4` << b.`bit8` as uint4_bit8, a.`uint4` << b.`bit15` as uint4_bit15, a.`uint4` << b.`bit64` as uint4_bit64 from `t_number` as a, `t_bit` as b order by a.id, b.id;
desc v_uint4_bit; -- uint4 << bit error
drop view v_uint4_bit;

create view v_int8_bit as select a.`int8` << b.`bit1` as int8_bit1, a.`int8` << b.`bit8` as int8_bit8, a.`int8` << b.`bit15` as int8_bit15, a.`int8` << b.`bit64` as int8_bit64 from `t_number` as a, `t_bit` as b order by a.id, b.id;
desc v_int8_bit; -- int8 << bit error
drop view v_int8_bit;

create view v_uint8_bit as select a.`uint8` << b.`bit1` as uint8_bit1, a.`uint8` << b.`bit8` as uint8_bit8, a.`uint8` << b.`bit15` as uint8_bit15, a.`uint8` << b.`bit64` as uint8_bit64 from `t_number` as a, `t_bit` as b order by a.id, b.id;
desc v_uint8_bit; -- uint8 << bit error
drop view v_uint8_bit;

create view v_float4_bit as select a.`float4` << b.`bit1` as float4_bit1, a.`float4` << b.`bit8` as float4_bit8, a.`float4` << b.`bit15` as float4_bit15, a.`float4` << b.`bit64` as float4_bit64 from `t_number` as a, `t_bit` as b order by a.id, b.id;
desc v_float4_bit; -- float4 << bit error
drop view v_float4_bit;

create view v_float8_bit as select a.`float8` << b.`bit1` as float8_bit1, a.`float8` << b.`bit8` as float8_bit8, a.`float8` << b.`bit15` as float8_bit15, a.`float8` << b.`bit64` as float8_bit64 from `t_number` as a, `t_bit` as b order by a.id, b.id;
desc v_float8_bit; -- float8 << bit error
drop view v_float8_bit;

create view v_numeric_bit as select a.`numeric` << b.`bit1` as numeric_bit1, a.`numeric` << b.`bit8` as numeric_bit8, a.`numeric` << b.`bit15` as numeric_bit15, a.`numeric` << b.`bit64` as numeric_bit64 from `t_number` as a, `t_bit` as b order by a.id, b.id;
desc v_numeric_bit; -- numeric << bit error
drop view v_numeric_bit;

create view v_boolean_bit as select a.`boolean` << b.`bit1` as boolean_bit1, a.`boolean` << b.`bit8` as boolean_bit8, a.`boolean` << b.`bit15` as boolean_bit15, a.`boolean` << b.`bit64` as boolean_bit64 from `t_number` as a, `t_bit` as b order by a.id, b.id;
desc v_boolean_bit; -- boolean << bit error
drop view v_boolean_bit;

create view v_char_string as select a.`char` << b.`char` as char_char, a.`char` << b.`varchar` as char_varchar, a.`char` << b.`binary` as char_binary, a.`char` << b.`varbinary` as char_varbinary, a.`char` << b.`text` as char_text from `t_str` as a, `t_str` as b order by a.id, b.id;
desc v_char_string; -- char << bit error
drop view v_char_string;

create view v_varchar_string as select a.`varchar` << b.`char` as varchar_char, a.`varchar` << b.`varchar` as varchar_varchar, a.`varchar` << b.`binary` as varchar_binary, a.`varchar` << b.`varbinary` as varchar_varbinary, a.`varchar` << b.`text` as varchar_text from `t_str` as a, `t_str` as b order by a.id, b.id;
desc v_varchar_string; -- varchar << bit error
drop view v_varchar_string;

create view v_binary_string as select a.`binary` << b.`char` as binary_char, a.`binary` << b.`varchar` as binary_varchar, a.`binary` << b.`binary` as binary_binary, a.`binary` << b.`varbinary` as binary_varbinary, a.`binary` << b.`text` as binary_text from `t_str` as a, `t_str` as b order by a.id, b.id;
desc v_binary_string; -- binary << char error
drop view v_binary_string;

create view v_varbinary_string as select a.`varbinary` << b.`char` as varbinary_char, a.`varbinary` << b.`varchar` as varbinary_varchar, a.`varbinary` << b.`binary` as varbinary_binary, a.`varbinary` << b.`varbinary` as varbinary_varbinary, a.`varbinary` << b.`text` as varbinary_text from `t_str` as a, `t_str` as b order by a.id, b.id;
desc v_varbinary_string; -- varbinary << char error
drop view v_varbinary_string;

create view v_text_string as select a.`text` << b.`char` as text_char, a.`text` << b.`varchar` as text_varchar, a.`text` << b.`binary` as text_binary, a.`text` << b.`varbinary` as text_varbinary, a.`text` << b.`text` as text_text from `t_str` as a, `t_str` as b order by a.id, b.id;
desc v_text_string; -- text << binary error
drop view v_text_string;

create view v_char_bit as select a.`char` << b.`bit1` as char_bit1, a.`char` << b.`bit8` as char_bit8, a.`char` << b.`bit15` as char_bit15, a.`char` << b.`bit64` as char_bit64 from `t_str` as a, `t_bit` as b order by a.id, b.id;
desc v_char_bit; -- char << bit error
drop view v_char_bit;

create view v_varchar_bit as select a.`varchar` << b.`bit1` as varchar_bit1, a.`varchar` << b.`bit8` as varchar_bit8, a.`varchar` << b.`bit15` as varchar_bit15, a.`varchar` << b.`bit64` as varchar_bit64 from `t_str` as a, `t_bit` as b order by a.id, b.id;
desc v_varchar_bit; -- varchar << bit error
drop view v_varchar_bit;

create view v_binary_bit as select a.`binary` << b.`bit1` as binary_bit1, a.`binary` << b.`bit8` as binary_bit8, a.`binary` << b.`bit15` as binary_bit15, a.`binary` << b.`bit64` as binary_bit64 from `t_str` as a, `t_bit` as b order by a.id, b.id;
desc v_binary_bit; -- bianry << bit error
drop view v_binary_bit;

create view v_varbinary_bit as select a.`varbinary` << b.`bit1` as varbinary_bit1, a.`varbinary` << b.`bit8` as varbinary_bit8, a.`varbinary` << b.`bit15` as varbinary_bit15, a.`varbinary` << b.`bit64` as varbinary_bit64 from `t_str` as a, `t_bit` as b order by a.id, b.id;
desc v_varbinary_bit; -- varbinary << bit error
drop view v_varbinary_bit;

create view v_text_bit as select a.`text` << b.`bit1` as text_bit1, a.`text` << b.`bit8` as text_bit8, a.`text` << b.`bit15` as text_bit15, a.`text` << b.`bit64` as text_bit64 from `t_str` as a, `t_bit` as b order by a.id, b.id;
desc v_text_bit; -- text << bit error
drop view v_text_bit;

create view v_bit1_bit as select a.`bit1` << b.`bit1` as bit1_bit1, a.`bit1` << b.`bit8` as bit1_bit8, a.`bit1` << b.`bit15` as bit1_bit15, a.`bit1` << b.`bit64` as bit1_bit64 from `t_bit` as a, `t_bit` as b order by a.id, b.id;
desc v_bit1_bit; -- error
drop view v_bit1_bit;

create view v_bit8_bit as select a.`bit8` << b.`bit1` as bit8_bit1, a.`bit8` << b.`bit8` as bit8_bit8, a.`bit8` << b.`bit15` as bit8_bit15, a.`bit8` << b.`bit64` as bit8_bit64 from `t_bit` as a, `t_bit` as b order by a.id, b.id;
desc v_bit8_bit; -- error
drop view v_bit8_bit;

create view v_bit15_bit as select a.`bit15` << b.`bit1` as bit15_bit1, a.`bit15` << b.`bit8` as bit15_bit8, a.`bit15` << b.`bit15` as bit15_bit15, a.`bit15` << b.`bit64` as bit15_bit64 from `t_bit` as a, `t_bit` as b order by a.id, b.id;
desc v_bit15_bit; -- error
drop view v_bit15_bit;

create view v_bit64_bit as select a.`bit64` << b.`bit1` as bit64_bit1, a.`bit64` << b.`bit8` as bit64_bit8, a.`bit64` << b.`bit15` as bit64_bit15, a.`bit64` << b.`bit64` as bit64_bit64 from `t_bit` as a, `t_bit` as b order by a.id, b.id;
desc v_bit64_bit; -- error
drop view v_bit64_bit;

-- test << result
select a.id as a_id, b.id as b_id, a.`int1` << b.`int1` as int1_int1, a.`int1` << b.`uint1` as int1_uint1, a.`int1` << b.`int2` as int1_int2, a.`int1` << b.`uint2` as int1_uint2, a.`int1` << b.`int4` as int1_int4, a.`int1` << b.`uint4` as int1_uint4, a.`int1` << b.`int8` as int1_int8, a.`int1` << b.`uint8` as int1_uint8, a.`int1` << b.`float4` as int1_float4, a.`int1` << b.`float8` as int1_float8, a.`int1` << b.`numeric` as int1_numeric, a.`int1` << b.`boolean` as int1_boolean from t_number as a,t_number as b order by a.id, b.id;
select a.id as a_id, b.id as b_id, a.`uint1` << b.`int1` as uint1_int1, a.`uint1` << b.`uint1` as uint1_uint1, a.`uint1` << b.`int2` as uint1_int2, a.`uint1` << b.`uint2` as uint1_uint2, a.`uint1` << b.`int4` as uint1_int4, a.`uint1` << b.`uint4` as uint1_uint4, a.`uint1` << b.`int8` as uint1_int8, a.`uint1` << b.`uint8` as uint1_uint8, a.`uint1` << b.`float4` as uint1_float4, a.`uint1` << b.`float8` as uint1_float8, a.`uint1` << b.`numeric` as uint1_numeric, a.`uint1` << b.`boolean` as uint1_boolean from t_number as a,t_number as b order by a.id, b.id;
select a.id as a_id, b.id as b_id, a.`int2` << b.`int1` as int2_int1, a.`int2` << b.`uint1` as int2_uint1, a.`int2` << b.`int2` as int2_int2, a.`int2` << b.`uint2` as int2_uint2, a.`int2` << b.`int4` as int2_int4, a.`int2` << b.`uint4` as int2_uint4, a.`int2` << b.`int8` as int2_int8, a.`int2` << b.`uint8` as int2_uint8, a.`int2` << b.`float4` as int2_float4, a.`int2` << b.`float8` as int2_float8, a.`int2` << b.`numeric` as int2_numeric, a.`int2` << b.`boolean` as int2_boolean from t_number as a,t_number as b order by a.id, b.id;
select a.id as a_id, b.id as b_id, a.`uint2` << b.`int1` as uint2_int1, a.`uint2` << b.`uint1` as uint2_uint1, a.`uint2` << b.`int2` as uint2_int2, a.`uint2` << b.`uint2` as uint2_uint2, a.`uint2` << b.`int4` as uint2_int4, a.`uint2` << b.`uint4` as uint2_uint4, a.`uint2` << b.`int8` as uint2_int8, a.`uint2` << b.`uint8` as uint2_uint8, a.`uint2` << b.`float4` as uint2_float4, a.`uint2` << b.`float8` as uint2_float8, a.`uint2` << b.`numeric` as uint2_numeric, a.`uint2` << b.`boolean` as uint2_boolean from t_number as a,t_number as b order by a.id, b.id;
select a.id as a_id, b.id as b_id, a.`int4` << b.`int1` as int4_int1, a.`int4` << b.`uint1` as int4_uint1, a.`int4` << b.`int2` as int4_int2, a.`int4` << b.`uint2` as int4_uint2, a.`int4` << b.`int4` as int4_int4, a.`int4` << b.`uint4` as int4_uint4, a.`int4` << b.`int8` as int4_int8, a.`int4` << b.`uint8` as int4_uint8, a.`int4` << b.`float4` as int4_float4, a.`int4` << b.`float8` as int4_float8, a.`int4` << b.`numeric` as int4_numeric, a.`int4` << b.`boolean` as int4_boolean from t_number as a,t_number as b order by a.id, b.id;
select a.id as a_id, b.id as b_id, a.`uint4` << b.`int1` as uint4_int1, a.`uint4` << b.`uint1` as uint4_uint1, a.`uint4` << b.`int2` as uint4_int2, a.`uint4` << b.`uint2` as uint4_uint2, a.`uint4` << b.`int4` as uint4_int4, a.`uint4` << b.`uint4` as uint4_uint4, a.`uint4` << b.`int8` as uint4_int8, a.`uint4` << b.`uint8` as uint4_uint8, a.`uint4` << b.`float4` as uint4_float4, a.`uint4` << b.`float8` as uint4_float8, a.`uint4` << b.`numeric` as uint4_numeric, a.`uint4` << b.`boolean` as uint4_boolean from t_number as a,t_number as b order by a.id, b.id;
select a.id as a_id, b.id as b_id, a.`int8` << b.`int1` as int8_int1, a.`int8` << b.`uint1` as int8_uint1, a.`int8` << b.`int2` as int8_int2, a.`int8` << b.`uint2` as int8_uint2, a.`int8` << b.`int4` as int8_int4, a.`int8` << b.`uint4` as int8_uint4, a.`int8` << b.`int8` as int8_int8, a.`int8` << b.`uint8` as int8_uint8, a.`int8` << b.`float4` as int8_float4, a.`int8` << b.`float8` as int8_float8, a.`int8` << b.`numeric` as int8_numeric, a.`int8` << b.`boolean` as int8_boolean from t_number as a,t_number as b order by a.id, b.id;
select a.id as a_id, b.id as b_id, a.`uint8` << b.`int1` as uint8_int1, a.`uint8` << b.`uint1` as uint8_uint1, a.`uint8` << b.`int2` as uint8_int2, a.`uint8` << b.`uint2` as uint8_uint2, a.`uint8` << b.`int4` as uint8_int4, a.`uint8` << b.`uint4` as uint8_uint4, a.`uint8` << b.`int8` as uint8_int8, a.`uint8` << b.`uint8` as uint8_uint8, a.`uint8` << b.`float4` as uint8_float4, a.`uint8` << b.`float8` as uint8_float8, a.`uint8` << b.`numeric` as uint8_numeric, a.`uint8` << b.`boolean` as uint8_boolean from t_number as a,t_number as b order by a.id, b.id;
select a.id as a_id, b.id as b_id, a.`float4` << b.`int1` as float4_int1, a.`float4` << b.`uint1` as float4_uint1, a.`float4` << b.`int2` as float4_int2, a.`float4` << b.`uint2` as float4_uint2, a.`float4` << b.`int4` as float4_int4, a.`float4` << b.`uint4` as float4_uint4, a.`float4` << b.`int8` as float4_int8, a.`float4` << b.`uint8` as float4_uint8, a.`float4` << b.`float4` as float4_float4, a.`float4` << b.`float8` as float4_float8, a.`float4` << b.`numeric` as float4_numeric, a.`float4` << b.`boolean` as float4_boolean from t_number as a,t_number as b order by a.id, b.id;
select a.id as a_id, b.id as b_id, a.`float8` << b.`int1` as float8_int1, a.`float8` << b.`uint1` as float8_uint1, a.`float8` << b.`int2` as float8_int2, a.`float8` << b.`uint2` as float8_uint2, a.`float8` << b.`int4` as float8_int4, a.`float8` << b.`uint4` as float8_uint4, a.`float8` << b.`int8` as float8_int8, a.`float8` << b.`uint8` as float8_uint8, a.`float8` << b.`float4` as float8_float4, a.`float8` << b.`float8` as float8_float8, a.`float8` << b.`numeric` as float8_numeric, a.`float8` << b.`boolean` as float8_boolean from t_number as a,t_number as b order by a.id, b.id;
select a.id as a_id, b.id as b_id, a.`numeric` << b.`int1` as numeric_int1, a.`numeric` << b.`uint1` as numeric_uint1, a.`numeric` << b.`int2` as numeric_int2, a.`numeric` << b.`uint2` as numeric_uint2, a.`numeric` << b.`int4` as numeric_int4, a.`numeric` << b.`uint4` as numeric_uint4, a.`numeric` << b.`int8` as numeric_int8, a.`numeric` << b.`uint8` as numeric_uint8, a.`numeric` << b.`float4` as numeric_float4, a.`numeric` << b.`float8` as numeric_float8, a.`numeric` << b.`numeric` as numeric_numeric, a.`numeric` << b.`boolean` as numeric_boolean from t_number as a,t_number as b order by a.id, b.id;
select a.id as a_id, b.id as b_id, a.`boolean` << b.`int1` as boolean_int1, a.`boolean` << b.`uint1` as boolean_uint1, a.`boolean` << b.`int2` as boolean_int2, a.`boolean` << b.`uint2` as boolean_uint2, a.`boolean` << b.`int4` as boolean_int4, a.`boolean` << b.`uint4` as boolean_uint4, a.`boolean` << b.`int8` as boolean_int8, a.`boolean` << b.`uint8` as boolean_uint8, a.`boolean` << b.`float4` as boolean_float4, a.`boolean` << b.`float8` as boolean_float8, a.`boolean` << b.`numeric` as boolean_numeric, a.`boolean` << b.`boolean` as boolean_boolean from t_number as a,t_number as b order by a.id, b.id;
select a.id as a_id, b.id as b_id, a.`int1` << b.`char` as int1_char, a.`int1` << b.`varchar` as int1_varchar, a.`int1` << b.`binary` as int1_binary, a.`int1` << b.`varbinary` as int1_varbinary, a.`int1` << b.`text` as int1_text from t_number as a,t_str as b order by a.id, b.id;
select a.id as a_id, b.id as b_id, a.`uint1` << b.`char` as uint1_char, a.`uint1` << b.`varchar` as uint1_varchar, a.`uint1` << b.`binary` as uint1_binary, a.`uint1` << b.`varbinary` as uint1_varbinary, a.`uint1` << b.`text` as uint1_text from t_number as a,t_str as b order by a.id, b.id;
select a.id as a_id, b.id as b_id, a.`int2` << b.`char` as int2_char, a.`int2` << b.`varchar` as int2_varchar, a.`int2` << b.`binary` as int2_binary, a.`int2` << b.`varbinary` as int2_varbinary, a.`int2` << b.`text` as int2_text from t_number as a,t_str as b order by a.id, b.id;
select a.id as a_id, b.id as b_id, a.`uint2` << b.`char` as uint2_char, a.`uint2` << b.`varchar` as uint2_varchar, a.`uint2` << b.`binary` as uint2_binary, a.`uint2` << b.`varbinary` as uint2_varbinary, a.`uint2` << b.`text` as uint2_text from t_number as a,t_str as b order by a.id, b.id;
select a.id as a_id, b.id as b_id, a.`int4` << b.`char` as int4_char, a.`int4` << b.`varchar` as int4_varchar, a.`int4` << b.`binary` as int4_binary, a.`int4` << b.`varbinary` as int4_varbinary, a.`int4` << b.`text` as int4_text from t_number as a,t_str as b order by a.id, b.id;
select a.id as a_id, b.id as b_id, a.`uint4` << b.`char` as uint4_char, a.`uint4` << b.`varchar` as uint4_varchar, a.`uint4` << b.`binary` as uint4_binary, a.`uint4` << b.`varbinary` as uint4_varbinary, a.`uint4` << b.`text` as uint4_text from t_number as a,t_str as b order by a.id, b.id;
select a.id as a_id, b.id as b_id, a.`int8` << b.`char` as int8_char, a.`int8` << b.`varchar` as int8_varchar, a.`int8` << b.`binary` as int8_binary, a.`int8` << b.`varbinary` as int8_varbinary, a.`int8` << b.`text` as int8_text from t_number as a,t_str as b order by a.id, b.id;
select a.id as a_id, b.id as b_id, a.`uint8` << b.`char` as uint8_char, a.`uint8` << b.`varchar` as uint8_varchar, a.`uint8` << b.`binary` as uint8_binary, a.`uint8` << b.`varbinary` as uint8_varbinary, a.`uint8` << b.`text` as uint8_text from t_number as a,t_str as b order by a.id, b.id;
select a.id as a_id, b.id as b_id, a.`float4` << b.`char` as float4_char, a.`float4` << b.`varchar` as float4_varchar, a.`float4` << b.`binary` as float4_binary, a.`float4` << b.`varbinary` as float4_varbinary, a.`float4` << b.`text` as float4_text from t_number as a,t_str as b order by a.id, b.id;
select a.id as a_id, b.id as b_id, a.`float8` << b.`char` as float8_char, a.`float8` << b.`varchar` as float8_varchar, a.`float8` << b.`binary` as float8_binary, a.`float8` << b.`varbinary` as float8_varbinary, a.`float8` << b.`text` as float8_text from t_number as a,t_str as b order by a.id, b.id;
select a.id as a_id, b.id as b_id, a.`numeric` << b.`char` as numeric_char, a.`numeric` << b.`varchar` as numeric_varchar, a.`numeric` << b.`binary` as numeric_binary, a.`numeric` << b.`varbinary` as numeric_varbinary, a.`numeric` << b.`text` as numeric_text from t_number as a,t_str as b order by a.id, b.id;
select a.id as a_id, b.id as b_id, a.`boolean` << b.`char` as boolean_char, a.`boolean` << b.`varchar` as boolean_varchar, a.`boolean` << b.`binary` as boolean_binary, a.`boolean` << b.`varbinary` as boolean_varbinary, a.`boolean` << b.`text` as boolean_text from t_number as a,t_str as b order by a.id, b.id;
select a.id as a_id, b.id as b_id, a.`int1` << b.`bit1` as int1_bit1, a.`int1` << b.`bit8` as int1_bit8, a.`int1` << b.`bit15` as int1_bit15, a.`int1` << b.`bit64` as int1_bit64 from t_number as a,t_bit as b order by a.id, b.id;
select a.id as a_id, b.id as b_id, a.`uint1` << b.`bit1` as uint1_bit1, a.`uint1` << b.`bit8` as uint1_bit8, a.`uint1` << b.`bit15` as uint1_bit15, a.`uint1` << b.`bit64` as uint1_bit64 from t_number as a,t_bit as b order by a.id, b.id;
select a.id as a_id, b.id as b_id, a.`int2` << b.`bit1` as int2_bit1, a.`int2` << b.`bit8` as int2_bit8, a.`int2` << b.`bit15` as int2_bit15, a.`int2` << b.`bit64` as int2_bit64 from t_number as a,t_bit as b order by a.id, b.id;
select a.id as a_id, b.id as b_id, a.`uint2` << b.`bit1` as uint2_bit1, a.`uint2` << b.`bit8` as uint2_bit8, a.`uint2` << b.`bit15` as uint2_bit15, a.`uint2` << b.`bit64` as uint2_bit64 from t_number as a,t_bit as b order by a.id, b.id;
select a.id as a_id, b.id as b_id, a.`int4` << b.`bit1` as int4_bit1, a.`int4` << b.`bit8` as int4_bit8, a.`int4` << b.`bit15` as int4_bit15, a.`int4` << b.`bit64` as int4_bit64 from t_number as a,t_bit as b order by a.id, b.id;
select a.id as a_id, b.id as b_id, a.`uint4` << b.`bit1` as uint4_bit1, a.`uint4` << b.`bit8` as uint4_bit8, a.`uint4` << b.`bit15` as uint4_bit15, a.`uint4` << b.`bit64` as uint4_bit64 from t_number as a,t_bit as b order by a.id, b.id;
select a.id as a_id, b.id as b_id, a.`int8` << b.`bit1` as int8_bit1, a.`int8` << b.`bit8` as int8_bit8, a.`int8` << b.`bit15` as int8_bit15, a.`int8` << b.`bit64` as int8_bit64 from t_number as a,t_bit as b order by a.id, b.id;
select a.id as a_id, b.id as b_id, a.`uint8` << b.`bit1` as uint8_bit1, a.`uint8` << b.`bit8` as uint8_bit8, a.`uint8` << b.`bit15` as uint8_bit15, a.`uint8` << b.`bit64` as uint8_bit64 from t_number as a,t_bit as b order by a.id, b.id;
select a.id as a_id, b.id as b_id, a.`float4` << b.`bit1` as float4_bit1, a.`float4` << b.`bit8` as float4_bit8, a.`float4` << b.`bit15` as float4_bit15, a.`float4` << b.`bit64` as float4_bit64 from t_number as a,t_bit as b order by a.id, b.id;
select a.id as a_id, b.id as b_id, a.`float8` << b.`bit1` as float8_bit1, a.`float8` << b.`bit8` as float8_bit8, a.`float8` << b.`bit15` as float8_bit15, a.`float8` << b.`bit64` as float8_bit64 from t_number as a,t_bit as b order by a.id, b.id;
select a.id as a_id, b.id as b_id, a.`numeric` << b.`bit1` as numeric_bit1, a.`numeric` << b.`bit8` as numeric_bit8, a.`numeric` << b.`bit15` as numeric_bit15, a.`numeric` << b.`bit64` as numeric_bit64 from t_number as a,t_bit as b order by a.id, b.id;
select a.id as a_id, b.id as b_id, a.`boolean` << b.`bit1` as boolean_bit1, a.`boolean` << b.`bit8` as boolean_bit8, a.`boolean` << b.`bit15` as boolean_bit15, a.`boolean` << b.`bit64` as boolean_bit64 from t_number as a,t_bit as b order by a.id, b.id;
select a.id as a_id, b.id as b_id, a.`char` << b.`char` as char_char, a.`char` << b.`varchar` as char_varchar, a.`char` << b.`binary` as char_binary, a.`char` << b.`varbinary` as char_varbinary, a.`char` << b.`text` as char_text from t_str as a,t_str as b order by a.id, b.id;
select a.id as a_id, b.id as b_id, a.`varchar` << b.`char` as varchar_char, a.`varchar` << b.`varchar` as varchar_varchar, a.`varchar` << b.`binary` as varchar_binary, a.`varchar` << b.`varbinary` as varchar_varbinary, a.`varchar` << b.`text` as varchar_text from t_str as a,t_str as b order by a.id, b.id;
select a.id as a_id, b.id as b_id, a.`binary` << b.`char` as binary_char, a.`binary` << b.`varchar` as binary_varchar, a.`binary` << b.`binary` as binary_binary, a.`binary` << b.`varbinary` as binary_varbinary, a.`binary` << b.`text` as binary_text from t_str as a,t_str as b order by a.id, b.id;
select a.id as a_id, b.id as b_id, a.`varbinary` << b.`char` as varbinary_char, a.`varbinary` << b.`varchar` as varbinary_varchar, a.`varbinary` << b.`binary` as varbinary_binary, a.`varbinary` << b.`varbinary` as varbinary_varbinary, a.`varbinary` << b.`text` as varbinary_text from t_str as a,t_str as b order by a.id, b.id;
select a.id as a_id, b.id as b_id, a.`text` << b.`char` as text_char, a.`text` << b.`varchar` as text_varchar, a.`text` << b.`binary` as text_binary, a.`text` << b.`varbinary` as text_varbinary, a.`text` << b.`text` as text_text from t_str as a,t_str as b order by a.id, b.id;
select a.id as a_id, b.id as b_id, a.`char` << b.`bit1` as char_bit1, a.`char` << b.`bit8` as char_bit8, a.`char` << b.`bit15` as char_bit15, a.`char` << b.`bit64` as char_bit64 from t_str as a,t_bit as b order by a.id, b.id;
select a.id as a_id, b.id as b_id, a.`varchar` << b.`bit1` as varchar_bit1, a.`varchar` << b.`bit8` as varchar_bit8, a.`varchar` << b.`bit15` as varchar_bit15, a.`varchar` << b.`bit64` as varchar_bit64 from t_str as a,t_bit as b order by a.id, b.id;
select a.id as a_id, b.id as b_id, a.`binary` << b.`bit1` as binary_bit1, a.`binary` << b.`bit8` as binary_bit8, a.`binary` << b.`bit15` as binary_bit15, a.`binary` << b.`bit64` as binary_bit64 from t_str as a,t_bit as b order by a.id, b.id;
select a.id as a_id, b.id as b_id, a.`varbinary` << b.`bit1` as varbinary_bit1, a.`varbinary` << b.`bit8` as varbinary_bit8, a.`varbinary` << b.`bit15` as varbinary_bit15, a.`varbinary` << b.`bit64` as varbinary_bit64 from t_str as a,t_bit as b order by a.id, b.id;
select a.id as a_id, b.id as b_id, a.`text` << b.`bit1` as text_bit1, a.`text` << b.`bit8` as text_bit8, a.`text` << b.`bit15` as text_bit15, a.`text` << b.`bit64` as text_bit64 from t_str as a,t_bit as b order by a.id, b.id;
select a.id as a_id, b.id as b_id, a.`bit1` << b.`bit1` as bit1_bit1, a.`bit1` << b.`bit8` as bit1_bit8, a.`bit1` << b.`bit15` as bit1_bit15, a.`bit1` << b.`bit64` as bit1_bit64 from t_bit as a,t_bit as b order by a.id, b.id;
select a.id as a_id, b.id as b_id, a.`bit8` << b.`bit1` as bit8_bit1, a.`bit8` << b.`bit8` as bit8_bit8, a.`bit8` << b.`bit15` as bit8_bit15, a.`bit8` << b.`bit64` as bit8_bit64 from t_bit as a,t_bit as b order by a.id, b.id;
select a.id as a_id, b.id as b_id, a.`bit15` << b.`bit1` as bit15_bit1, a.`bit15` << b.`bit8` as bit15_bit8, a.`bit15` << b.`bit15` as bit15_bit15, a.`bit15` << b.`bit64` as bit15_bit64 from t_bit as a,t_bit as b order by a.id, b.id;
select a.id as a_id, b.id as b_id, a.`bit64` << b.`bit1` as bit64_bit1, a.`bit64` << b.`bit8` as bit64_bit8, a.`bit64` << b.`bit15` as bit64_bit15, a.`bit64` << b.`bit64` as bit64_bit64 from t_bit as a,t_bit as b order by a.id, b.id;


drop table t_bit;
drop table t_str;
drop table t_number;
reset search_path;
drop schema test_left_right_shift;

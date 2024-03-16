create schema is_clause;
set current_schema to 'is_clause';
set dolphin.b_compatibility_mode to on;

create table test_type_table
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
	`numeric` decimal(20,6),
	`bit1` bit(1),
	`bit64` bit(64),
	`boolean` boolean,
	`text` text,
	`char` char(100),
	`varchar` varchar(100),
	`binary` binary(100),
	`varbinary` varbinary(100)
);

insert into test_type_table values (1,1,1,1,1,1,1,1,1,1,1,b'1',b'111',true,'1.23a','1.23a','1.23a','1.23a','1.23a');

select `int1` is true, `int1` is not true, `int1` is false, `int1` is not false, `int1` is unknown, `int1` is not unknown, `int1` is null, `int1` is not null from test_type_table;
select `uint1` is true, `uint1` is not true, `uint1` is false, `uint1` is not false, `uint1` is unknown, `uint1` is not unknown, `uint1` is null, `uint1` is not null from test_type_table;
select `int2` is true, `int2` is not true, `int2` is false, `int2` is not false, `int2` is unknown, `int2` is not unknown, `int2` is null, `int2` is not null from test_type_table;
select `uint2` is true, `uint2` is not true, `uint2` is false, `uint2` is not false, `uint2` is unknown, `uint2` is not unknown, `uint2` is null, `uint2` is not null from test_type_table;
select `int4` is true, `int4` is not true, `int4` is false, `int4` is not false, `int4` is unknown, `int4` is not unknown, `int4` is null, `int4` is not null from test_type_table;
select `uint4` is true, `uint4` is not true, `uint4` is false, `uint4` is not false, `uint4` is unknown, `uint4` is not unknown, `uint4` is null, `uint4` is not null from test_type_table;
select `int8` is true, `int8` is not true, `int8` is false, `int8` is not false, `int8` is unknown, `int8` is not unknown, `int8` is null, `int8` is not null from test_type_table;
select `uint8` is true, `uint8` is not true, `uint8` is false, `uint8` is not false, `uint8` is unknown, `uint8` is not unknown, `uint8` is null, `uint8` is not null from test_type_table;
select `float4` is true, `float4` is not true, `float4` is false, `float4` is not false, `float4` is unknown, `float4` is not unknown, `float4` is null, `float4` is not null from test_type_table;
select `float8` is true, `float8` is not true, `float8` is false, `float8` is not false, `float8` is unknown, `float8` is not unknown, `float8` is null, `float8` is not null from test_type_table;
select `numeric` is true, `numeric` is not true, `numeric` is false, `numeric` is not false, `numeric` is unknown, `numeric` is not unknown, `numeric` is null, `numeric` is not null from test_type_table;
select `bit1` is true, `bit1` is not true, `bit1` is false, `bit1` is not false, `bit1` is unknown, `bit1` is not unknown, `bit1` is null, `bit1` is not null from test_type_table;
select `bit64` is true, `bit64` is not true, `bit64` is false, `bit64` is not false, `bit64` is unknown, `bit64` is not unknown, `bit64` is null, `bit64` is not null from test_type_table;
select `boolean` is true, `boolean` is not true, `boolean` is false, `boolean` is not false, `boolean` is unknown, `boolean` is not unknown, `boolean` is null, `boolean` is not null from test_type_table;
select `text` is true, `text` is not true, `text` is false, `text` is not false, `text` is unknown, `text` is not unknown, `text` is null, `text` is not null from test_type_table;
select `char` is true, `char` is not true, `char` is false, `char` is not false, `char` is unknown, `char` is not unknown, `char` is null, `char` is not null from test_type_table;
select `varchar` is true, `varchar` is not true, `varchar` is false, `varchar` is not false, `varchar` is unknown, `varchar` is not unknown, `varchar` is null, `varchar` is not null from test_type_table;
select `binary` is true, `binary` is not true, `binary` is false, `binary` is not false, `binary` is unknown, `binary` is not unknown, `binary` is null, `binary` is not null from test_type_table;
select `varbinary` is true, `varbinary` is not true, `varbinary` is false, `varbinary` is not false, `varbinary` is unknown, `varbinary` is not unknown, `varbinary` is null, `varbinary` is not null from test_type_table;

select '' is true, '1.23' is true, '1.23a' is true, '1.23$@' is true, '     1.23       ' is true, '     1.23       a' is true;
select 'true' is true, 'true' is false, 'false' is true, 'false' is false;

drop schema is_clause cascade;
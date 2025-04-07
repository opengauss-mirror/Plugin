create schema test_assign_oper;
set current_schema to 'test_assign_oper';

set dolphin.b_compatibility_mode = on;
create TYPE test_enum AS ENUM ('a', 'b', 'c');

-- test for =
drop table if exists test_type_table;
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
    `numeric` decimal(20, 6),
    `bit1` bit(1),
    `bit64` bit(64),
    `boolean` boolean,
    `date` date,
    `time` time,
    `time(4)` time(4),
    `datetime` datetime,
    `datetime(4)` datetime(4) default '2022-11-11 11:11:11',
    `timestamp` timestamp,
    `timestamp(4)` timestamp(4) default '2022-11-11 11:11:11',
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
    `enum_t` test_enum,
    `set_t` set('a', 'b', 'c'),
    `json` json   
);

insert into test_type_table values (1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, b'1', b'111', true, '2023-02-05', '19:10:50', '19:10:50.3456', '2023-02-05 19:10:50', '2023-02-05 19:10:50.456', '2023-02-05 19:10:50', '2023-02-05 19:10:50.456',
'2023', '1.23a', '1.23a', '1.23a', '1.23a', '1.23a', '1.23a', '1.23a', '1.23a', '1.23a', 'a', 'a,c', json_object('a', 1, 'b', 2));

update test_type_table set `int1` = 1;
update test_type_table set `uint1` = 1;
update test_type_table set `int2` = 1;
update test_type_table set `uint2` = 1;
update test_type_table set `int4` = 1;
update test_type_table set `uint4` = 1;
update test_type_table set `int8` = 1;
update test_type_table set `uint8` = 1;
update test_type_table set `float4` = 1;
update test_type_table set `float8` = 1;
update test_type_table set `numeric` = 1;
update test_type_table set `bit1` = b'1';
update test_type_table set `bit64` = b'111';
update test_type_table set `boolean` = true;
update test_type_table set `date` = '2023-02-05';
update test_type_table set `time` = '19:10:50';
update test_type_table set `time(4)` = '19:10:50.3456';
update test_type_table set `datetime` = '2023-02-05 19:10:50';
update test_type_table set `datetime(4)` = '2023-02-05 19:10:50.456';
update test_type_table set `timestamp` = '2023-02-05 19:10:50';
update test_type_table set `timestamp(4)` = '2023-02-05 19:10:50.456';
update test_type_table set `year` = '2023';
update test_type_table set `char` = '1.23a';
update test_type_table set `varchar` = '1.23a';
update test_type_table set `binary` = '1.23a';
update test_type_table set `varbinary` = '1.23a';
update test_type_table set `tinyblob` = '1.23a';
update test_type_table set `blob` = '1.23a';
update test_type_table set `mediumblob` = '1.23a';
update test_type_table set `longblob` = '1.23a';
update test_type_table set `text` = '1.23a';
update test_type_table set `enum_t` = 'a';
update test_type_table set `set_t` = 'a,c';
update test_type_table set `json` = json_object('a', 1, 'b', 2);

select * from test_type_table;

drop table test_type_table;

CREATE OR REPLACE PROCEDURE test_data_types() AS
DECLARE
    -- Declare variables
    int1 tinyint;
    uint1 tinyint unsigned;
    int2 smallint;
    uint2 smallint unsigned;
    int4 integer;
    uint4 integer unsigned;
    int8 bigint;
    uint8 bigint unsigned;
    float4 float4;
    float8 float8;
    numeric_t decimal(20, 6);
    -- bit1 bit(1);
    -- bit64 bit(64);
    boolean_t boolean;
    date_t date;
    time_t time;
    time4_t time(4);
    datetime_t datetime;
    datetime4_t datetime(4);
    timestamp_t timestamp;
    timestamp4_t timestamp(4);
    year_t year;
    char_t char(100);
    varchar_t varchar(100); 
    binary_t binary(100);
    varbinary_t varbinary(100);
    tinyblob tinyblob;
    blob blob;
    mediumblob mediumblob;
    longblob longblob;
    text_t text;
    enum_t test_enum;
    -- set_t set('a', 'b', 'c');
    json_t json;
BEGIN
    -- Assign values to variables
    SET int1 = 1;
    SET uint1 = 1;
    SET int2 = 1;
    SET uint2 = 1;
    SET int4 = 1;
    SET uint4 = 1;
    SET int8 = 1;
    SET uint8 = 1;
    SET float4 = 1.0;
    SET float8 = 1.0;
    SET numeric_t = 1.0;
    -- SET bit1 = b'1';
    -- SET bit64 = b'111';
    SET boolean_t = true;
    SET date_t = '2023-02-05';
    SET time_t = '19:10:50';
    SET time4_t = '19:10:50.3456';
    SET datetime_t = '2023-02-05 19:10:50';
    SET datetime4_t = '2023-02-05 19:10:50.456';
    SET timestamp_t = '2023-02-05 19:10:50';
    SET timestamp4_t = '2023-02-05 19:10:50.456';
    SET year_t = '2023';
    SET char_t = '1.23a';
    SET varchar_t = '1.23a';
    SET binary_t = '1.23a';
    SET varbinary_t = '1.23a';
    SET tinyblob = '1.23a';
    SET blob = '1.23a';
    SET mediumblob = '1.23a';
    SET longblob = '1.23a';
    SET text_t = '1.23a';
    SET enum_t = 'a';
    -- SET set_t = 'a,c';
    SET json_t = JSON_OBJECT('a', 1, 'b', 2);

    -- Print variable values (MySQL does not have RAISE NOTICE, using SELECT)
	raise NOTICE 'int1: %', int1;
    raise NOTICE 'uint1: %', uint1;
    raise NOTICE 'int2: %', int2;
    raise NOTICE 'uint2: %', uint2;
    raise NOTICE 'int4: %', int4;
    raise NOTICE 'uint4: %', uint4;
    raise NOTICE 'int8: %', int8;
    raise NOTICE 'uint8: %', uint8;
    raise NOTICE 'float4: %', float4;
    raise NOTICE 'float8: %', float8;
    raise NOTICE 'numeric: %', numeric_t;
    -- raise NOTICE 'bit1: %', bit1;
    -- raise NOTICE 'bit64: %', bit64;
    raise NOTICE 'boolean: %', boolean_t;
    raise NOTICE 'date: %', date_t;
    raise NOTICE 'time: %', time_t;
    raise NOTICE 'time(4): %', time4_t;
    raise NOTICE 'datetime: %', datetime_t;
    raise NOTICE 'datetime(4): %', datetime4_t;
    raise NOTICE 'timestamp: %', timestamp_t;
    raise NOTICE 'timestamp(4): %', timestamp4_t;
    raise NOTICE 'year: %', year_t;
    raise NOTICE 'char: %', char_t;
	raise NOTICE 'varchar: %', varchar_t;
	raise NOTICE 'binary: %', binary_t;
    raise NOTICE 'varbinary: %', varbinary_t;
    raise NOTICE 'tinyblob: %', tinyblob;
    raise NOTICE 'blob: %', blob;
    raise NOTICE 'mediumblob: %', mediumblob;
    raise NOTICE 'longblob: %', longblob;
    raise NOTICE 'text: %', text_t;
    raise NOTICE 'enum_t: %', enum_t;
    -- raise NOTICE 'set_t: %', set_t;
    raise NOTICE 'json: %', json_t;
END;
/

call test_data_types();

drop procedure test_data_types;

-- test for :=
drop table if exists test_type_table;
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
    `numeric` decimal(20, 6),
    `bit1` bit(1),
    `bit64` bit(64),
    `boolean` boolean,
    `date` date,
    `time` time,
    `time(4)` time(4),
    `datetime` datetime,
    `datetime(4)` datetime(4) default '2022-11-11 11:11:11',
    `timestamp` timestamp,
    `timestamp(4)` timestamp(4) default '2022-11-11 11:11:11',
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
    `enum_t` test_enum,
    `set_t` set('a', 'b', 'c'),
    `json` json   
);

insert into test_type_table values (1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, b'1', b'111', true, '2023-02-05', '19:10:50', '19:10:50.3456', '2023-02-05 19:10:50', '2023-02-05 19:10:50.456', '2023-02-05 19:10:50', '2023-02-05 19:10:50.456',
'2023', '1.23a', '1.23a', '1.23a', '1.23a', '1.23a', '1.23a', '1.23a', '1.23a', '1.23a', 'a', 'a,c', json_object('a', 1, 'b', 2));

update test_type_table set `int1` := 1;
update test_type_table set `uint1` := 1;
update test_type_table set `int2` := 1;
update test_type_table set `uint2` := 1;
update test_type_table set `int4` := 1;
update test_type_table set `uint4` := 1;
update test_type_table set `int8` := 1;
update test_type_table set `uint8` := 1;
update test_type_table set `float4` := 1;
update test_type_table set `float8` := 1;
update test_type_table set `numeric` := 1;
update test_type_table set `bit1` := b'1';
update test_type_table set `bit64` := b'111';
update test_type_table set `boolean` := true;
update test_type_table set `date` := '2023-02-05';
update test_type_table set `time` := '19:10:50';
update test_type_table set `time(4)` := '19:10:50.3456';
update test_type_table set `datetime` := '2023-02-05 19:10:50';
update test_type_table set `datetime(4)` := '2023-02-05 19:10:50.456';
update test_type_table set `timestamp` := '2023-02-05 19:10:50';
update test_type_table set `timestamp(4)` := '2023-02-05 19:10:50.456';
update test_type_table set `year` := '2023';
update test_type_table set `char` := '1.23a';
update test_type_table set `varchar` := '1.23a';
update test_type_table set `binary` := '1.23a';
update test_type_table set `varbinary` := '1.23a';
update test_type_table set `tinyblob` := '1.23a';
update test_type_table set `blob` := '1.23a';
update test_type_table set `mediumblob` := '1.23a';
update test_type_table set `longblob` := '1.23a';
update test_type_table set `text` := '1.23a';
update test_type_table set `enum_t` := 'a';
update test_type_table set `set_t` := 'a,c';
update test_type_table set `json` := json_object('a', 1, 'b', 2);

select * from test_type_table;

drop table test_type_table;

CREATE OR REPLACE PROCEDURE test_data_types() AS
DECLARE
    -- Declare variables
    int1 tinyint;
    uint1 tinyint unsigned;
    int2 smallint;
    uint2 smallint unsigned;
    int4 integer;
    uint4 integer unsigned;
    int8 bigint;
    uint8 bigint unsigned;
    float4 float4;
    float8 float8;
    numeric_t decimal(20, 6);
    -- bit1 bit(1);
    -- bit64 bit(64);
    boolean_t boolean;
    date_t date;
    time_t time;
    time4_t time(4);
    datetime_t datetime;
    datetime4_t datetime(4);
    timestamp_t timestamp;
    timestamp4_t timestamp(4);
    year_t year;
    char_t char(100);
    varchar_t varchar(100); 
    binary_t binary(100);
    varbinary_t varbinary(100);
    tinyblob tinyblob;
    blob blob;
    mediumblob mediumblob;
    longblob longblob;
    text_t text;
    enum_t test_enum;
    -- set_t set('a', 'b', 'c');
    json_t json;
BEGIN
    -- Assign values to variables
    SET int1 := 1;
    SET uint1 := 1;
    SET int2 := 1;
    SET uint2 := 1;
    SET int4 := 1;
    SET uint4 := 1;
    SET int8 := 1;
    SET uint8 := 1;
    SET float4 := 1.0;
    SET float8 := 1.0;
    SET numeric_t := 1.0;
    -- SET bit1 := b'1';
    -- SET bit64 := b'111';
    SET boolean_t := true;
    SET date_t := '2023-02-05';
    SET time_t := '19:10:50';
    SET time4_t := '19:10:50.3456';
    SET datetime_t := '2023-02-05 19:10:50';
    SET datetime4_t := '2023-02-05 19:10:50.456';
    SET timestamp_t := '2023-02-05 19:10:50';
    SET timestamp4_t := '2023-02-05 19:10:50.456';
    SET year_t := '2023';
    SET char_t := '1.23a';
    SET varchar_t := '1.23a';
    SET binary_t := '1.23a';
    SET varbinary_t := '1.23a';
    SET tinyblob := '1.23a';
    SET blob := '1.23a';
    SET mediumblob := '1.23a';
    SET longblob := '1.23a';
    SET text_t := '1.23a';
    SET enum_t := 'a';
    -- SET set_t := 'a,c';
    SET json_t := JSON_OBJECT('a', 1, 'b', 2);

    -- Print variable values (MySQL does not have RAISE NOTICE, using SELECT)
	raise NOTICE 'int1: %', int1;
    raise NOTICE 'uint1: %', uint1;
    raise NOTICE 'int2: %', int2;
    raise NOTICE 'uint2: %', uint2;
    raise NOTICE 'int4: %', int4;
    raise NOTICE 'uint4: %', uint4;
    raise NOTICE 'int8: %', int8;
    raise NOTICE 'uint8: %', uint8;
    raise NOTICE 'float4: %', float4;
    raise NOTICE 'float8: %', float8;
    raise NOTICE 'numeric: %', numeric_t;
    -- raise NOTICE 'bit1: %', bit1;
    -- raise NOTICE 'bit64: %', bit64;
    raise NOTICE 'boolean: %', boolean_t;
    raise NOTICE 'date: %', date_t;
    raise NOTICE 'time: %', time_t;
    raise NOTICE 'time(4): %', time4_t;
    raise NOTICE 'datetime: %', datetime_t;
    raise NOTICE 'datetime(4): %', datetime4_t;
    raise NOTICE 'timestamp: %', timestamp_t;
    raise NOTICE 'timestamp(4): %', timestamp4_t;
    raise NOTICE 'year: %', year_t;
    raise NOTICE 'char: %', char_t;
	raise NOTICE 'varchar: %', varchar_t;
	raise NOTICE 'binary: %', binary_t;
    raise NOTICE 'varbinary: %', varbinary_t;
    raise NOTICE 'tinyblob: %', tinyblob;
    raise NOTICE 'blob: %', blob;
    raise NOTICE 'mediumblob: %', mediumblob;
    raise NOTICE 'longblob: %', longblob;
    raise NOTICE 'text: %', text_t;
    raise NOTICE 'enum_t: %', enum_t;
    -- raise NOTICE 'set_t: %', set_t;
    raise NOTICE 'json: %', json_t;
END;
/

call test_data_types();

drop procedure test_data_types;

drop schema test_assign_oper cascade;
reset current_schema;
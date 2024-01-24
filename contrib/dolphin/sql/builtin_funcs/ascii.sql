create schema test_builtin_funcs;
set current_schema to 'test_builtin_funcs';
set dolphin.sql_mode to ''; -- remove pad_char_to_full_length
set dolphin.b_compatibility_mode = on;

create table test_type_table
(
    `int1`         tinyint,
    `uint1`        tinyint unsigned,
    `int2`         smallint,
    `uint2`        smallint unsigned,
    `int4`         integer,
    `uint4`        integer unsigned,
    `int8`         bigint,
    `uint8`        bigint unsigned,
    `float4`       float4,
    `float8`       float8,
    `numeric`      decimal(20, 6),
    `bit1`         bit(1),
    `bit64`        bit(64),
    `boolean`      boolean,
    `date`         date,
    `time`         time,
    `time4`      time(4),
    `datetime`     datetime,
    `datetime4`  datetime(4) default '2022-11-11 11:11:11',
    `timestamp`    timestamp,
    `timestamp4` timestamp(4) default '2022-11-11 11:11:11',
    `year` year,
    `char`         char(100),
    `varchar`      varchar(100),
    `binary`       binary(100),
    `varbinary`    varbinary(100),
    `tinyblob`     tinyblob,
    `blob`         blob,
    `mediumblob`   mediumblob,
    `longblob`     longblob,
    `text`         text,
    `enum_t`       enum('a', 'b', 'c'),
    `set_t` set('a', 'b', 'c'),
    `json`         json
);

insert into test_type_table values
(
    1, 1, 1, 1, 1, 1, 1, 1, 1.0, 1.0, 3.14259,
    1, 10101100, 1,
    '2024-01-11', '11:47:58', '11:47:58.789012', '2024-01-11 11:49:25', '2024-01-11 11:49:25.159', '2024-01-11 11:49:25', '2024-01-11 11:49:25.159', '2023',
    '62.345*67-89', '62.345*67-89',
    '62.345*67-89', '62.345*67-89',
    '62.345*67-89', '62.345*67-89', '62.345*67-89', '62.345*67-89',
    '62.345*67-89',
    'b',
    'a,b',
    json_object('a', 2, 'b', 3)
);
set bytea_output TO escape;

SELECT
    ascii(`int1`),
    ascii(`uint1`),
    ascii(`int2`),
    ascii(`uint2`),
    ascii(`int4`),
    ascii(`uint4`),
    ascii(`int8`),
    ascii(`uint8`),
    ascii(`float4`),
    ascii(`float8`),
    ascii(`numeric`),
    ascii(`bit1`),
    ascii(`bit64`),
    ascii(`boolean`),
    ascii(`date`),
    ascii(`time`),
    ascii(`time4`),
    ascii(`datetime`),
    ascii(`datetime4`),
    ascii(`timestamp`),
    ascii(`timestamp4`),
    ascii(`year`),
    ascii(`char`),
    ascii(`varchar`),
    ascii(`binary`),
    ascii(`varbinary`),
    ascii(`tinyblob`),
    ascii(`blob`),
    ascii(`mediumblob`),
    ascii(`longblob`),
    ascii(`text`),
    ascii(`enum_t`),
    ascii(`set_t`),
    ascii(`json`)
FROM test_type_table;

-- bit test
create table bit_test(`bit1` bit(1), `bit5` bit(5), `bit8` bit(8), `bit15` bit(15), `bit16` bit(16));
insert into bit_test values(0,0,0,0,0);
insert into bit_test values(1,1,1,1,1);
insert into bit_test values(1,0x7,0xF,0xF5,0xF6);
select ascii(`bit1`), ascii(`bit5`), ascii(`bit8`), ascii(`bit15`), ascii(`bit16`) from bit_test;

-- test bit length = 0
select bit_length(b''), ascii(b'');

drop schema test_builtin_funcs cascade;
reset bytea_output;
reset dolphin.sql_mode;
reset dolphin.b_compatibility_mode;
reset current_schema;

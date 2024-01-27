create schema db_test_bit;
set current_schema to 'db_test_bit';
set dolphin.b_compatibility_mode = on;

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
    `time4` time(4),
    `datetime` datetime,
    `datetime4` datetime(4) default '2023-12-30 12:00:12',
    `timestamp` timestamp,
    `timestamp4` timestamp(4) default '2023-12-30 12:00:12',
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

insert into test_type_table values
    (
        1, 1, 1, 1, 1, 1, 1, 1, 1.0, 1.0, 3.14259,
        1, 10101100, 1,
        '2024-01-11', '11:47:58', '11:47:58.7896', '2024-01-11 11:49:25', '2024-01-11 11:49:25.1234', '2024-01-11 11:49:25', '2024-01-11 11:49:25.1234', '2024',
        '62.345*67-89', '62.345*67-89',
        '67890 - 12345 = 55545', '67890 - 12345 = 55545',
        '67890 - 12345 = 55545', '67890 - 12345 = 55545', '67890 - 12345 = 55545', '67890 - 12345 = 55545',
        '67890 - 12345 = 55545',
        'b',
        'a,b',
        json_object('a', 2, 'b', 3)
    );
insert into test_type_table values
    (
        127, 255, 32767, 65535, 0x7FFFFFFF, 0xFFFFFFFF, 0x7FFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 3.402823, 1.79769313486231, 3.141592654,
        1, 0xFFFFFFFFFFFFFFFF, 1,
        '2222-02-22', '11:59:58', '11:59:58.9999', '2222-02-22 11:59:58', '2222-02-22 11:59:58.9999', '2038-01-19 03:14:07', '2038-01-19 03:14:07.9999', '2155',
        'Today is a good day.  ', 'Today is a good day.  ',
        'Today is a good day.  ', 'Today is a good day.  ',
        'Today is a good day.  ', 'Today is a good day.  ', 'Today is a good day.  ', 'Today is a good day.  ',
        'Today is a good day.  ',
        'b',
        'a,b',
        json_object('a', 2, 'b', 3)
    );
insert into test_type_table values
    (
        -127, 0, -32768, 0, -2147483648, 0, -9223372036854775808, 0,
        -1234.567890, -1002345.78456892, -99999999999999.999999,
        0, 0x01, 0,
        '1999-09-19', '00:00:01', '00:00:01.0001', '1999-09-19 00:00:0001', '1999-09-19 00:00:01.0001', '1970-07-07 00:00:59', '1970-07-07 00:00:59.0001', '1999',
        '', '', '', '',
        '', '', '', '',
        '',
        'a', 'a,b,c',
        json_object('a', 0)
    );

SELECT
    bit_length(`int1`),
    bit_length(uint1),
    bit_length(`int2`),
    bit_length(uint2),
    bit_length(`int4`),
    bit_length(uint4),
    bit_length(`int8`),
    bit_length(uint8),
    bit_length(`float4`),
    bit_length(`float8`),
    bit_length(`numeric`),
    bit_length(bit1),
    bit_length(bit64),
    bit_length(`boolean`),
    bit_length(`date`),
    bit_length(`time`),
    bit_length(time4),
    bit_length(`datetime`),
    bit_length(datetime4),
    bit_length(`timestamp`),
    bit_length(timestamp4),
    bit_length(`year`),
    bit_length(`char`),
    bit_length(`varchar`),
    bit_length(`binary`),
    bit_length(`varbinary`),
    bit_length(`tinyblob`),
    bit_length(`blob`),
    bit_length(`mediumblob`),
    bit_length(`longblob`),
    bit_length(`text`),
    bit_length(enum_t),
    bit_length(set_t),
    bit_length(json)
FROM test_type_table;
drop table test_type_table;

-- valid binary type
create table binary_test (`binary20` binary(20), `binary45` binary(45), `binary88` binary(88));
insert into binary_test values ('abcd', '98765 - 43210 = 55555', 'Today is a good day.');
insert into binary_test values ('  abcd  ', '98765 - 43210 = 55555  ', '');
select bit_length(`binary20`), bit_length(`binary45`), bit_length(`binary88`) from binary_test;
drop table binary_test;

-- valid char type
create table char_test (char10 char(10), char48 char(48), char97 char(97));
insert into char_test values ('abcd', '98765 - 43210 = 55555', 'Today is a good day.');
insert into char_test values ('  abcd  ', '', '   ');
select bit_length(char10), bit_length(char48), bit_length(char97) from char_test;
drop table char_test;

drop schema db_test_bit cascade;
reset bytea_output;
reset dolphin.sql_mode;
reset dolphin.b_compatibility_mode;
reset current_schema;

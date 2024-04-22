create schema substr_func_test;
set current_schema to 'substr_func_test';
show dolphin.sql_mode;
set dolphin.b_compatibility_mode = on;

-- test function: left, type: number、text、blob
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
    `char` char(100),
    `varchar` varchar(100),
    `binary` binary(100),
    `varbinary` varbinary(100),
    `tinyblob` tinyblob,
    `blob` blob,
    `mediumblob` mediumblob,
    `longblob` longblob,
    `text` text
);

insert into test_type_table values
    (
        1, 1, 1, 1, 1, 1, 1, 1, 1.0, 1.0, 3.14259,
        1, 10101100, 1,
        '62.345*67-89', '62.345*67-89',
        '67890 - 12345 = 55545', '67890 - 12345 = 55545',
        '67890 - 12345 = 55545', '67890 - 12345 = 55545', '67890 - 12345 = 55545', '67890 - 12345 = 55545',
        '67890 - 12345 = 55545'
    );

insert into test_type_table values
    (
        127, 255, 32767, 65535, x'7FFFFFFF', x'FFFFFFFF', x'7FFFFFFFFFFFFFFF', x'FFFFFFFFFFFFFFFF', 3.402823, 1.79769313486231, 3.141592654,
        1, x'FFFFFFFFFFFFFFFF', 1,
        'Today is a good day.  ', 'Today is a good day.  ',
        'Today is a good day.  ', 'Today is a good day.  ',
        'Today is a good day.  ', 'Today is a good day.  ', 'Today is a good day.  ', 'Today is a good day.  ',
        'Today is a good day.  '
    );

\x
SELECT
    left(`int1`, 1),
    left(`uint1`, 2),
    left(`int2`, 3),
    left(`uint2`, 2),
    left(`int4`, 1),
    left(`uint4`, 2),
    left(`int8`, 3),
    left(`uint8`, 0),
    left(`float4`, 2),
    left(`float8`, 3),
    left(`numeric`, 1),
    left(`bit1`, 2),
    left(`bit64`, 3),
    left(`boolean`, 4),
    left(`char`, 4),
    left(`varchar`, -3),
    left(`binary`, 5),
    left(`varbinary`, 6),
    left(`text`, 10),
    left(`tinyblob`, 7),
    left(`blob`, 8),
    left(`mediumblob`, 9),
    left(`longblob`, 10)
FROM test_type_table;
\x


-- test bit type
create table bit_test (`bit1` bit(1), `bit8` bit(8), `bit15` bit(15), `bit64` bit(64));
insert into bit_test values (1, x'68', x'4d45', x'536f6d65006f6e65');
insert into bit_test values (0, x'7d', x'0057', x'00536f6d656f6e65');
insert into bit_test values (0, x'77', x'5700', x'536f6d656f6e6500');
select left(`bit1`, 2), left(`bit8`, 0), left(`bit15`, -12), left(`bit64`, 6) from bit_test;
-- test integer overflow
select left(`bit64`, 2147483647) from bit_test;

-- test boolean type
create table boolean_test(`boolean` boolean);
insert into boolean_test values (1);
insert into boolean_test values (0);
select left(`boolean`, 2) from boolean_test;

-- test blob type
create table blob_test(`tinyblob` tinyblob, `blob` blob, `mediumblob` mediumblob, `longblob` longblob);
insert into blob_test values (' 1.23*45-67!89', ' 1.23*45-67!89', ' 1.23*45-67!89', ' 1.23*45-67!89');
insert into blob_test values ('abc-098+home  ', 'abc-098+home  ', 'abc-098+home  ', 'abc-098+home  ');
select left(`tinyblob`, 4), left(`blob`, 5), left(`mediumblob`, 6), left(`longblob`, 7) from blob_test;

drop table test_type_table;
drop table bit_test;
drop table boolean_test;
drop table blob_test;

drop schema substr_func_test cascade;
reset bytea_output;
reset dolphin.sql_mode;
reset dolphin.b_compatibility_mode;
reset current_schema;

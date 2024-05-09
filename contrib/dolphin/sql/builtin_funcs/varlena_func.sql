create schema db_test_varlena;
set current_schema to 'db_test_varlena';
set dolphin.b_compatibility_mode = on;
set dolphin.sql_mode = 'sql_mode_strict,sql_mode_full_group,pipes_as_concat,ansi_quotes,no_zero_date';

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

\x
-- strict select
SELECT
    concat(`int1`, `uint1`),
    concat(`uint1`, `boolean`),
    concat(`int2`, `time4`),
    concat(`uint2`, `datetime4`),
    concat(`int4`, `varchar`),
    concat(`uint4`, `binary`),
    concat(`int8`, `enum_t`),
    concat(`uint8`, `blob`),
    concat(`float4`, `json`),
    concat(`float8`, `bit1`),
    concat(`numeric`, `set_t`),
    concat(`bit1`, `char`),
    concat(`bit64`, `text`),
    concat(`boolean`, `datetime`),
    concat(`date`, `mediumblob`),
    concat(`time`, `longblob`),
    concat(`time4`, `text`),
    concat(`datetime`, `varbinary`),
    concat(`datetime4`, `binary`),
    concat(`timestamp`, `json`),
    concat(`timestamp4`, `bit64`),
    concat(`year`, `int4`),
    concat(`char`, `bit64`),
    concat(`varchar`, `varbinary`),
    concat(`binary`, `boolean`),
    concat(`varbinary`, `varchar`),
    concat(`tinyblob`, `varchar`),
    concat(`blob`, `varchar`),
    concat(`mediumblob`, `varchar`),
    concat(`longblob`, `varchar`),
    concat(`text`, `varchar`),
    concat(`enum_t`, `varchar`),
    concat(`set_t`, `varchar`),
    concat(`json`, `varchar`)
FROM test_type_table;
SELECT
    concat_ws('-', `int1`, `uint1`),
    concat_ws('-', `uint1`, `boolean`),
    concat_ws('-', `int2`, `time4`),
    concat_ws('-', `uint2`, `datetime4`),
    concat_ws('-', `int4`, `varchar`),
    concat_ws('-', `uint4`, `binary`),
    concat_ws('-', `int8`, `enum_t`),
    concat_ws('-', `uint8`, `blob`),
    concat_ws('-', `float4`, `json`),
    concat_ws('-', `float8`, `bit1`),
    concat_ws('-', `numeric`, `set_t`),
    concat_ws('-', `bit1`, `char`),
    concat_ws('-', `bit64`, `text`),
    concat_ws('-', `boolean`, `datetime`),
    concat_ws('-', `date`, `mediumblob`),
    concat_ws('-', `time`, `longblob`),
    concat_ws('-', `time4`, `text`),
    concat_ws('-', `datetime`, `varbinary`),
    concat_ws('-', `datetime4`, `binary`),
    concat_ws('-', `timestamp`, `json`),
    concat_ws('-', `timestamp4`, `bit64`),
    concat_ws('-', `year`, `int4`),
    concat_ws('-', `char`, `bit64`),
    concat_ws('-', `varchar`, `varbinary`),
    concat_ws('-', `binary`, `boolean`),
    concat_ws('-', `varbinary`, `varchar`),
    concat_ws('-', `tinyblob`, `varchar`),
    concat_ws('-', `blob`, `varchar`),
    concat_ws('-', `mediumblob`, `varchar`),
    concat_ws('-', `longblob`, `varchar`),
    concat_ws('-', `text`, `varchar`),
    concat_ws('-', `enum_t`, `varchar`),
    concat_ws('-', `set_t`, `varchar`),
    concat_ws('-', `json`, `varchar`)
FROM test_type_table;

set dolphin.sql_mode = 'sql_mode_full_group,pipes_as_concat,ansi_quotes';
SELECT
    concat(`int1`, `uint1`),
    concat(`uint1`, `boolean`),
    concat(`int2`, `time4`),
    concat(`uint2`, `datetime4`),
    concat(`int4`, `varchar`),
    concat(`uint4`, `binary`),
    concat(`int8`, `enum_t`),
    concat(`uint8`, `blob`),
    concat(`float4`, `json`),
    concat(`float8`, `bit1`),
    concat(`numeric`, `set_t`),
    concat(`bit1`, `char`),
    concat(`bit64`, `text`),
    concat(`boolean`, `datetime`),
    concat(`date`, `mediumblob`),
    concat(`time`, `longblob`),
    concat(`time4`, `text`),
    concat(`datetime`, `varbinary`),
    concat(`datetime4`, `binary`),
    concat(`timestamp`, `json`),
    concat(`timestamp4`, `bit64`),
    concat(`year`, `int4`),
    concat(`char`, `bit64`),
    concat(`varchar`, `varbinary`),
    concat(`binary`, `boolean`),
    concat(`varbinary`, `varchar`),
    concat(`tinyblob`, `varchar`),
    concat(`blob`, `varchar`),
    concat(`mediumblob`, `varchar`),
    concat(`longblob`, `varchar`),
    concat(`text`, `varchar`),
    concat(`enum_t`, `varchar`),
    concat(`set_t`, `varchar`),
    concat(`json`, `varchar`)
FROM test_type_table;
SELECT
    concat_ws('-', `int1`, `uint1`),
    concat_ws('-', `uint1`, `boolean`),
    concat_ws('-', `int2`, `time4`),
    concat_ws('-', `uint2`, `datetime4`),
    concat_ws('-', `int4`, `varchar`),
    concat_ws('-', `uint4`, `binary`),
    concat_ws('-', `int8`, `enum_t`),
    concat_ws('-', `uint8`, `blob`),
    concat_ws('-', `float4`, `json`),
    concat_ws('-', `float8`, `bit1`),
    concat_ws('-', `numeric`, `set_t`),
    concat_ws('-', `bit1`, `char`),
    concat_ws('-', `bit64`, `text`),
    concat_ws('-', `boolean`, `datetime`),
    concat_ws('-', `date`, `mediumblob`),
    concat_ws('-', `time`, `longblob`),
    concat_ws('-', `time4`, `text`),
    concat_ws('-', `datetime`, `varbinary`),
    concat_ws('-', `datetime4`, `binary`),
    concat_ws('-', `timestamp`, `json`),
    concat_ws('-', `timestamp4`, `bit64`),
    concat_ws('-', `year`, `int4`),
    concat_ws('-', `char`, `bit64`),
    concat_ws('-', `varchar`, `varbinary`),
    concat_ws('-', `binary`, `boolean`),
    concat_ws('-', `varbinary`, `varchar`),
    concat_ws('-', `tinyblob`, `varchar`),
    concat_ws('-', `blob`, `varchar`),
    concat_ws('-', `mediumblob`, `varchar`),
    concat_ws('-', `longblob`, `varchar`),
    concat_ws('-', `text`, `varchar`),
    concat_ws('-', `enum_t`, `varchar`),
    concat_ws('-', `set_t`, `varchar`),
    concat_ws('-', `json`, `varchar`)
FROM test_type_table;
\x

-- test result type
SELECT
    pg_typeof(concat(`int1`, `uint1`)),
    pg_typeof(concat(`uint1`, `boolean`)),
    pg_typeof(concat(`int2`, `time4`)),
    pg_typeof(concat(`uint2`, `datetime4`)),
    pg_typeof(concat(`int4`, `varchar`)),
    pg_typeof(concat(`uint4`, `binary`)),
    pg_typeof(concat(`int8`, `enum_t`)),
    pg_typeof(concat(`uint8`, `blob`)),
    pg_typeof(concat(`float4`, `json`)),
    pg_typeof(concat(`float8`, `bit1`)),
    pg_typeof(concat(`numeric`, `set_t`)),
    pg_typeof(concat(`bit1`, `char`)),
    pg_typeof(concat(`bit64`, `text`)),
    pg_typeof(concat(`boolean`, `datetime`)),
    pg_typeof(concat(`date`, `mediumblob`)),
    pg_typeof(concat(`time`, `longblob`)),
    pg_typeof(concat(`time4`, `text`)),
    pg_typeof(concat(`datetime`, `varbinary`)),
    pg_typeof(concat(`datetime4`, `binary`)),
    pg_typeof(concat(`timestamp`, `json`)),
    pg_typeof(concat(`timestamp4`, `bit64`)),
    pg_typeof(concat(`year`, `int4`)),
    pg_typeof(concat(`char`, `bit64`)),
    pg_typeof(concat(`varchar`, `varbinary`)),
    pg_typeof(concat(`binary`, `boolean`)),
    pg_typeof(concat(`varbinary`, `varchar`)),
    pg_typeof(concat(`tinyblob`, `varchar`)),
    pg_typeof(concat(`blob`, `varchar`)),
    pg_typeof(concat(`mediumblob`, `varchar`)),
    pg_typeof(concat(`longblob`, `varchar`)),
    pg_typeof(concat(`text`, `varchar`)),
    pg_typeof(concat(`enum_t`, `varchar`)),
    pg_typeof(concat(`set_t`, `varchar`)),
    pg_typeof(concat(`json`, `varchar`))
FROM test_type_table;
SELECT
    pg_typeof(concat_ws('-', `int1`, `uint1`)),
    pg_typeof(concat_ws('-', `uint1`, `boolean`)),
    pg_typeof(concat_ws('-', `int2`, `time4`)),
    pg_typeof(concat_ws('-', `uint2`, `datetime4`)),
    pg_typeof(concat_ws('-', `int4`, `varchar`)),
    pg_typeof(concat_ws('-', `uint4`, `binary`)),
    pg_typeof(concat_ws('-', `int8`, `enum_t`)),
    pg_typeof(concat_ws('-', `uint8`, `blob`)),
    pg_typeof(concat_ws('-', `float4`, `json`)),
    pg_typeof(concat_ws('-', `float8`, `bit1`)),
    pg_typeof(concat_ws('-', `numeric`, `set_t`)),
    pg_typeof(concat_ws('-', `bit1`, `char`)),
    pg_typeof(concat_ws('-', `bit64`, `text`)),
    pg_typeof(concat_ws('-', `boolean`, `datetime`)),
    pg_typeof(concat_ws('-', `date`, `mediumblob`)),
    pg_typeof(concat_ws('-', `time`, `longblob`)),
    pg_typeof(concat_ws('-', `time4`, `text`)),
    pg_typeof(concat_ws('-', `datetime`, `varbinary`)),
    pg_typeof(concat_ws('-', `datetime4`, `binary`)),
    pg_typeof(concat_ws('-', `timestamp`, `json`)),
    pg_typeof(concat_ws('-', `timestamp4`, `bit64`)),
    pg_typeof(concat_ws('-', `year`, `int4`)),
    pg_typeof(concat_ws('-', `char`, `bit64`)),
    pg_typeof(concat_ws('-', `varchar`, `varbinary`)),
    pg_typeof(concat_ws('-', `binary`, `boolean`)),
    pg_typeof(concat_ws('-', `varbinary`, `varchar`)),
    pg_typeof(concat_ws('-', `tinyblob`, `varchar`)),
    pg_typeof(concat_ws('-', `blob`, `varchar`)),
    pg_typeof(concat_ws('-', `mediumblob`, `varchar`)),
    pg_typeof(concat_ws('-', `longblob`, `varchar`)),
    pg_typeof(concat_ws('-', `text`, `varchar`)),
    pg_typeof(concat_ws('-', `enum_t`, `varchar`)),
    pg_typeof(concat_ws('-', `set_t`, `varchar`)),
    pg_typeof(concat_ws('-', `json`, `varchar`))
FROM test_type_table;

-- test sql_mode pad_char_to_full_length
set dolphin.sql_mode = 'pad_char_to_full_length';
select concat(`char`, `int2`) from test_type_table;
select concat_ws('-', `char`, `int2`) from test_type_table;

drop table test_type_table;

-- test bit type
create table bit_test (`bit1` bit(1), `bit6` bit(6), `bit8` bit(8), `bit15` bit(15), `bit16` bit(16));
insert into bit_test values (1, 0x33, 0xA6, 0x6E00, 0xCC00);
insert into bit_test values (0, 0x2F, 0xF0, 0x006E, 0x00CC);
set dolphin.sql_mode = 'sql_mode_strict,sql_mode_full_group,pipes_as_concat,ansi_quotes,no_zero_date,pad_char_to_full_length';
select concat(`bit1`, 'openGauss'), concat(`bit6`, 'openGauss'), concat(`bit8`, 'openGauss'), concat(`bit15`, 'openGauss'), concat(`bit16`, 'openGauss') from bit_test;
select concat_ws('-', `bit1`, 'openGauss'), concat_ws('-', `bit6`, 'openGauss'), concat_ws('-', `bit8`, 'openGauss'), concat_ws('-', `bit15`, 'openGauss'), concat_ws('-', `bit16`, 'openGauss') from bit_test;
set dolphin.sql_mode = 'sql_mode_full_group,pipes_as_concat,ansi_quotes,pad_char_to_full_length';
select concat(`bit1`, 'openGauss'), concat(`bit6`, 'openGauss'), concat(`bit8`, 'openGauss'), concat(`bit15`, 'openGauss'), concat(`bit16`, 'openGauss') from bit_test;
select concat_ws('-', `bit1`, 'openGauss'), concat_ws('-', `bit6`, 'openGauss'), concat_ws('-', `bit8`, 'openGauss'), concat_ws('-', `bit15`, 'openGauss'), concat_ws('-', `bit16`, 'openGauss') from bit_test;
drop table bit_test;

create table t1(a text);
explain(costs off) select * from t1 where a like concat('%', '1', '%');
explain(costs off) select * from t1 where a like concat('%', '1'::bool, '%');
explain(costs off) select * from t1 where a like concat('%', '1'::"char", '%');
explain(costs off) select * from t1 where a like concat('%', '1'::name, '%');
explain(costs off) select * from t1 where a like concat('%', '1'::int1, '%');
explain(costs off) select * from t1 where a like concat('%', '1'::int2, '%');
explain(costs off) select * from t1 where a like concat('%', '1'::int4, '%');
explain(costs off) select * from t1 where a like concat('%', '1'::int8, '%');
explain(costs off) select * from t1 where a like concat('%', '1'::int16, '%');
explain(costs off) select * from t1 where a like concat('%', '1'::text, '%');
explain(costs off) select * from t1 where a like concat('%', '1'::oid, '%');
explain(costs off) select * from t1 where a like concat('%', '1'::clob, '%');
explain(costs off) select * from t1 where a like concat('%', '{"a":1}'::json, '%');
explain(costs off) select * from t1 where a like concat('%', '1'::xml, '%');
explain(costs off) select * from t1 where a like concat('%', '1'::varchar, '%');
explain(costs off) select * from t1 where a like concat('%', '1'::varbit, '%')::text;
explain(costs off) select * from t1 where a like concat('%', '1'::cstring, '%');
explain(costs off) select * from t1 where a like concat('%', '{"a":1}'::jsonb, '%');
explain(costs off) select * from t1 where a like concat('%', '1'::nvarchar2, '%');
explain(costs off) select * from t1 where a like concat('%', '1'::year, '%');
explain(costs off) select * from t1 where a like concat('%', '1'::uint1, '%');
explain(costs off) select * from t1 where a like concat('%', '1'::uint2, '%');
explain(costs off) select * from t1 where a like concat('%', '1'::uint4, '%');
explain(costs off) select * from t1 where a like concat('%', '1'::uint8, '%');

-- some can't do simply func
explain(costs off) select * from t1 where a like concat('%', '1'::money, '%');
explain(costs off) select * from t1 where a like concat('%', '1'::raw, '%');
explain(costs off) select * from t1 where a like concat('%', '1'::bytea, '%');
explain(costs off) select * from t1 where a like concat('%', '2022-11-11'::date, '%');
explain(costs off) select * from t1 where a like concat('%', '2022-11-11 11:11:11'::timestamp, '%');
explain(costs off) select * from t1 where a like concat('%', '11:11:11'::time, '%');
explain(costs off) select * from t1 where a like concat('%', '1'::numeric, '%');
explain(costs off) select * from t1 where a like concat('%', '1'::bpchar, '%');
explain(costs off) select * from t1 where a like concat('%', '1'::float4, '%');
explain(costs off) select * from t1 where a like concat('%', '1'::float8, '%');
explain(costs off) select * from t1 where a like concat('%', '1'::binary, '%')::text;
explain(costs off) select * from t1 where a like concat('%', '1'::varbinary, '%')::text;
explain(costs off) select * from t1 where a like concat('%', '1'::blob, '%')::text;
explain(costs off) select * from t1 where a like concat('%', '1'::tinyblob, '%')::text;
explain(costs off) select * from t1 where a like concat('%', '1'::mediumblob, '%')::text;
explain(costs off) select * from t1 where a like concat('%', '1'::longblob, '%')::text;
explain(costs off) select * from t1 where a like concat('%', '1'::bit(8), '%')::text;
drop table t1;

drop schema db_test_varlena cascade;
reset bytea_output;
reset dolphin.sql_mode;
reset dolphin.b_compatibility_mode;
reset current_schema;

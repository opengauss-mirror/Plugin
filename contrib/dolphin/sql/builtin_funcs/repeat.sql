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
    `time(4)`      time(4),
    `datetime`     datetime,
    `datetime(4)`  datetime(4) default '2022-11-11 11:11:11',
    `timestamp`    timestamp,
    `timestamp(4)` timestamp(4) default '2022-11-11 11:11:11',
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

insert into test_type_table
values (1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, b'1', b'111', true, '2023-02-05', '19:10:50', '19:10:50.3456',
        '2023-02-05 19:10:50', '2023-02-05 19:10:50.456', '2023-02-05 19:10:50', '2023-02-05 19:10:50.456',
        '2023', '1.23a', '1.23a', '1.23a', '1.23a', '1.23a', '1.23a', '1.23a', '1.23a', '1.23a', 'a', 'a,c',
        json_object('a', 1, 'b', 2));
set bytea_output TO escape;
\x
select repeat(`int1`, 10),
       repeat(`uint1`, 10),
       repeat(`int2`, 10),
       repeat(`uint2`, 10),
       repeat(`int4`, 10),
       repeat(`uint4`, 10),
       repeat(`int8`, 10),
       repeat(`uint8`, 10),
       repeat(`float4`, 10),
       repeat(`float8`, 10),
       repeat(`numeric`, 10),
       repeat(`bit1`, 10),
       repeat(`bit64`, 10),
       repeat(`boolean`, 10),
       repeat(`date`, 10),
       repeat(`time`, 10),
       repeat(`time(4)`, 10),
       repeat(`datetime`, 10),
       repeat(`datetime(4)`, 10),
       repeat(`timestamp`, 10),
       repeat(`timestamp(4)`, 10),
       repeat(`year`, 10),
       repeat(`char`, 10),
       repeat(`varchar`, 10),
       repeat(`binary`, 10),
       repeat(`varbinary`, 10),
       repeat(`tinyblob`, 10),
       repeat(`blob`, 10),
       repeat(`mediumblob`, 10),
       repeat(`longblob`, 10),
       repeat(`text`, 10),
       repeat(`enum_t`, 10),
       repeat(`set_t`, 10),
       repeat(`json`, 10)
from test_type_table;
\x
-- bit test
create table bit_test(bit1 bit(1), bit5 bit(5), bit8 bit(8), bit15 bit(15), bit16 bit(16));
insert into bit_test values(0,0,0,0,0);
insert into bit_test values(1,1,1,1,1);
insert into bit_test values(1,x'7',x'F',x'F5',x'F6');
select repeat(bit1,5),repeat(bit5,5),repeat(bit8,5),repeat(bit15,5),repeat(bit16,5) from bit_test;
drop schema test_builtin_funcs cascade;
reset bytea_output;
reset dolphin.sql_mode;
reset dolphin.b_compatibility_mode;
reset current_schema;
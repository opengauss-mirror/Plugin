create schema db_test_lower_func;
set current_schema to 'db_test_lower_func';
set dolphin.sql_mode = '';
set dolphin.b_compatibility_mode = on;

create table t1
(
    c1 integer,
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
    `char`         char(100),
    `varchar`      varchar(100),
    `binary`       binary(100),
    `varbinary`    varbinary(100),
    `tinyblob`     tinyblob,
    `blob`         blob,
    `mediumblob`   mediumblob,
    `longblob`     longblob,
    `text`         text
);

insert into t1 values
(
1,
1, 1, 1, 1, 1, 1, 1, 1, 1.0, 1.0, 3.14259, 
1, 10101100, 1, 
'62.345*67-89', '62.345*67-89', 
'67890 - 12345 = 55545', '67890 - 12345 = 55545', 
'67890 - 12345 = 55545', '67890 - 12345 = 55545', '67890 - 12345 = 55545', '67890 - 12345 = 55545', 
'67890 - 12345 = 55545'
);

insert into t1 values
(
2, 
127, 255, 32767, 65535, 0x7FFFFFFF, 0xFFFFFFFF, 0x7FFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 3.402823, 1.79769313486231, 3.141592654, 
1, 0xFFFFFFFFFFFFFFFF, 1, 
'Today is a good day.  ', 'Today is a good day.  ', 
'Today is a good day.  ', 'Today is a good day.  ', 
'Today is a good day.  ', 'Today is a good day.  ', 'Today is a good day.  ', 'Today is a good day.  ', 
'Today is a good day.  '
);

insert into t1 values
(
3, 
-127, 0, -32768, 0, -2147483648, 0, -9223372036854775808, 0,
-1234.567890, -1002345.78456892, -99999999999999.999999, 
0, 0x01, 0,
'', '', '', '',
'', '', '', '', 
''
);

\x
SELECT
    c1, 
    lower(`int1`),
    lower(`uint1`),
    lower(`int2`),
    lower(`uint2`),
    lower(`int4`),
    lower(`uint4`),
    lower(`int8`),
    lower(`uint8`),
    lower(`float4`),
    lower(`float8`),
    lower(`numeric`),
    lower(`bit1`),
    lower(`bit64`),
    lower(`boolean`),
    lower(`char`),
    lower(`varchar`),
    lower(`binary`),
    lower(`varbinary`),
    lower(`tinyblob`),
    lower(`blob`),
    lower(`mediumblob`),
    lower(`longblob`),
    lower(`text`)
FROM t1 order by c1;

SELECT
    c1, 
    lcase(`int1`),
    lcase(`uint1`),
    lcase(`int2`),
    lcase(`uint2`),
    lcase(`int4`),
    lcase(`uint4`),
    lcase(`int8`),
    lcase(`uint8`),
    lcase(`float4`),
    lcase(`float8`),
    lcase(`numeric`),
    lcase(`bit1`),
    lcase(`bit64`),
    lcase(`boolean`),
    lcase(`char`),
    lcase(`varchar`),
    lcase(`binary`),
    lcase(`varbinary`),
    lcase(`tinyblob`),
    lcase(`blob`),
    lcase(`mediumblob`),
    lcase(`longblob`),
    lcase(`text`)
FROM t1 order by c1;

\x
SELECT
    pg_typeof(lower(`int1`)),
    pg_typeof(lower(`uint1`)),
    pg_typeof(lower(`int2`)),
    pg_typeof(lower(`uint2`)),
    pg_typeof(lower(`int4`)),
    pg_typeof(lower(`uint4`)),
    pg_typeof(lower(`int8`)),
    pg_typeof(lower(`uint8`)),
    pg_typeof(lower(`float4`)),
    pg_typeof(lower(`float8`)),
    pg_typeof(lower(`numeric`)),
    pg_typeof(lower(`bit1`)),
    pg_typeof(lower(`bit64`)),
    pg_typeof(lower(`boolean`)),
    pg_typeof(lower(`char`)),
    pg_typeof(lower(`varchar`)),
    pg_typeof(lower(`binary`)),
    pg_typeof(lower(`varbinary`)),
    pg_typeof(lower(`tinyblob`)),
    pg_typeof(lower(`blob`)),
    pg_typeof(lower(`mediumblob`)),
    pg_typeof(lower(`longblob`)),
    pg_typeof(lower(`text`))
FROM t1 where c1 = 1;

SELECT
    pg_typeof(lcase(`int1`)),
    pg_typeof(lcase(`uint1`)),
    pg_typeof(lcase(`int2`)),
    pg_typeof(lcase(`uint2`)),
    pg_typeof(lcase(`int4`)),
    pg_typeof(lcase(`uint4`)),
    pg_typeof(lcase(`int8`)),
    pg_typeof(lcase(`uint8`)),
    pg_typeof(lcase(`float4`)),
    pg_typeof(lcase(`float8`)),
    pg_typeof(lcase(`numeric`)),
    pg_typeof(lcase(`bit1`)),
    pg_typeof(lcase(`bit64`)),
    pg_typeof(lcase(`boolean`)),
    pg_typeof(lcase(`char`)),
    pg_typeof(lcase(`varchar`)),
    pg_typeof(lcase(`binary`)),
    pg_typeof(lcase(`varbinary`)),
    pg_typeof(lcase(`tinyblob`)),
    pg_typeof(lcase(`blob`)),
    pg_typeof(lcase(`mediumblob`)),
    pg_typeof(lcase(`longblob`)),
    pg_typeof(lcase(`text`))
FROM t1 where c1 = 1;

drop table t1;

create table bit_test (`bit1` bit(1), `bit6` bit(6), `bit8` bit(8), `bit15` bit(15), `bit16` bit(16));
insert into bit_test values (1, 0x33, 0x68, 0x4d45, 0x5400);
select lower(`bit1`), lower(`bit6`), lower(`bit8`), lower(`bit15`), lower(`bit16`) from bit_test;
select lcase(`bit1`), lcase(`bit6`), lcase(`bit8`), lcase(`bit15`), lcase(`bit16`) from bit_test;

drop table bit_test;

drop schema db_test_lower_func cascade;
reset bytea_output;
reset dolphin.sql_mode;
reset dolphin.b_compatibility_mode;
reset current_schema;

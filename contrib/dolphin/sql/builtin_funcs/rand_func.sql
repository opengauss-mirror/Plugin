create schema db_test_rand_func;
set current_schema to 'db_test_rand_func';
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
    rand(`int1`),
    rand(`uint1`),
    rand(`int2`),
    rand(`uint2`),
    rand(`int4`),
    rand(`uint4`),
    rand(`int8`),
    rand(`uint8`),
    rand(`float4`),
    rand(`float8`),
    rand(`numeric`),
    rand(`bit1`),
    rand(`bit64`),
    rand(`boolean`),
    rand(`char`),
    rand(`varchar`),
    rand(`binary`),
    rand(`varbinary`),
    rand(`tinyblob`),
    rand(`blob`),
    rand(`mediumblob`),
    rand(`longblob`),
    rand(`text`)
FROM t1 order by c1;

SELECT
    c1, 
    round(`int1`),
    round(`uint1`),
    round(`int2`),
    round(`uint2`),
    round(`int4`),
    round(`uint4`),
    round(`int8`),
    round(`uint8`),
    round(`float4`),
    round(`float8`),
    round(`numeric`),
    round(`bit1`),
    round(`bit64`),
    round(`boolean`),
    round(`char`),
    round(`varchar`),
    round(`binary`),
    round(`varbinary`),
    round(`tinyblob`),
    round(`blob`),
    round(`mediumblob`),
    round(`longblob`),
    round(`text`)
FROM t1 order by c1;

SELECT
    c1, 
    round(`int1`, `int1`),
    round(`uint1`, `uint1`),
    round(`int2`, `int2`),
    round(`uint2`, `uint2`),
    round(`int4`, `int4`),
    round(`uint4`, `uint4`),
    round(`int8`, `int8`),
    round(`uint8`, `uint8`),
    round(`float4`, `float4`),
    round(`float8`, `float8`),
    round(`numeric`, `numeric`),
    round(`bit1`, `bit1`),
    round(`bit64`, `bit64`),
    round(`boolean`, `boolean`),
    round(`char`, `char`),
    round(`varchar`, `varchar`),
    round(`binary`, `binary`),
    round(`varbinary`, `varbinary`),
    round(`tinyblob`, `tinyblob`),
    round(`blob`, `blob`),
    round(`mediumblob`, `mediumblob`),
    round(`longblob`, `longblob`),
    round(`text`, `text`)
FROM t1 order by c1;

SELECT mod(`int1`, `int1`) FROM t1;
SELECT mod(`uint1`, `uint1`) FROM t1;
SELECT mod(`int2`, `int2`) FROM t1;
SELECT mod(`uint2`, `uint2`) FROM t1;
SELECT mod(`int4`, `int4`) FROM t1;
SELECT mod(`uint4`, `uint4`) FROM t1;
SELECT mod(`int8`, `int8`) FROM t1;
SELECT mod(`uint8`, `uint8`) FROM t1;
SELECT mod(`float4`, `float4`) FROM t1;
SELECT mod(`float8`, `float8`) FROM t1;
SELECT mod(`numeric`, `numeric`) FROM t1;
SELECT mod(`bit1`, `bit1`) FROM t1;
SELECT mod(`bit64`, `bit64`) FROM t1;
SELECT mod(`boolean`, `boolean`) FROM t1;
SELECT mod(`char`, `char`) FROM t1;
SELECT mod(`varchar`, `varchar`) FROM t1;
SELECT mod(`binary`, `binary`) FROM t1;
SELECT mod(`varbinary`, `varbinary`) FROM t1;
SELECT mod(`tinyblob`, `tinyblob`) FROM t1;
SELECT mod(`blob`, `blob`) FROM t1;
SELECT mod(`mediumblob`, `mediumblob`) FROM t1;
SELECT mod(`longblob`, `longblob`) FROM t1;
SELECT mod(`text`, `text`) FROM t1;

SELECT pow(`int1`, `int1`) FROM t1;
SELECT pow(`uint1`, `uint1`) FROM t1;
SELECT pow(`int2`, `int2`) FROM t1;
SELECT pow(`uint2`, `uint2`) FROM t1;
SELECT pow(`int4`, `int4`) FROM t1;
SELECT pow(`uint4`, `uint4`) FROM t1;
SELECT pow(`int8`, `int8`) FROM t1;
SELECT pow(`uint8`, `uint8`) FROM t1;
SELECT pow(`float4`, `float4`) FROM t1;
SELECT pow(`float8`, `float8`) FROM t1;
SELECT pow(`numeric`, `numeric`) FROM t1;
SELECT pow(`bit1`, `bit1`) FROM t1;
SELECT pow(`bit64`, `bit64`) FROM t1;
SELECT pow(`boolean`, `boolean`) FROM t1;
SELECT pow(`char`, `char`) FROM t1;
SELECT pow(`varchar`, `varchar`) FROM t1;
SELECT pow(`binary`, `binary`) FROM t1;
SELECT pow(`varbinary`, `varbinary`) FROM t1;
SELECT pow(`tinyblob`, `tinyblob`) FROM t1;
SELECT pow(`blob`, `blob`) FROM t1;
SELECT pow(`mediumblob`, `mediumblob`) FROM t1;
SELECT pow(`longblob`, `longblob`) FROM t1;
SELECT pow(`text`, `text`) FROM t1;

SELECT power(`int1`, `int1`) FROM t1;
SELECT power(`uint1`, `uint1`) FROM t1;
SELECT power(`int2`, `int2`) FROM t1;
SELECT power(`uint2`, `uint2`) FROM t1;
SELECT power(`int4`, `int4`) FROM t1;
SELECT power(`uint4`, `uint4`) FROM t1;
SELECT power(`int8`, `int8`) FROM t1;
SELECT power(`uint8`, `uint8`) FROM t1;
SELECT power(`float4`, `float4`) FROM t1;
SELECT power(`float8`, `float8`) FROM t1;
SELECT power(`numeric`, `numeric`) FROM t1;
SELECT power(`bit1`, `bit1`) FROM t1;
SELECT power(`bit64`, `bit64`) FROM t1;
SELECT power(`boolean`, `boolean`) FROM t1;
SELECT power(`char`, `char`) FROM t1;
SELECT power(`varchar`, `varchar`) FROM t1;
SELECT power(`binary`, `binary`) FROM t1;
SELECT power(`varbinary`, `varbinary`) FROM t1;
SELECT power(`tinyblob`, `tinyblob`) FROM t1;
SELECT power(`blob`, `blob`) FROM t1;
SELECT power(`mediumblob`, `mediumblob`) FROM t1;
SELECT power(`longblob`, `longblob`) FROM t1;
SELECT power(`text`, `text`) FROM t1;


\x
SELECT
    pg_typeof(rand(`int1`)),
    pg_typeof(rand(`uint1`)),
    pg_typeof(rand(`int2`)),
    pg_typeof(rand(`uint2`)),
    pg_typeof(rand(`int4`)),
    pg_typeof(rand(`uint4`)),
    pg_typeof(rand(`int8`)),
    pg_typeof(rand(`uint8`)),
    pg_typeof(rand(`float4`)),
    pg_typeof(rand(`float8`)),
    pg_typeof(rand(`numeric`)),
    pg_typeof(rand(`bit1`)),
    pg_typeof(rand(`bit64`)),
    pg_typeof(rand(`boolean`)),
    pg_typeof(rand(`char`)),
    pg_typeof(rand(`varchar`)),
    pg_typeof(rand(`binary`)),
    pg_typeof(rand(`varbinary`)),
    pg_typeof(rand(`tinyblob`)),
    pg_typeof(rand(`blob`)),
    pg_typeof(rand(`mediumblob`)),
    pg_typeof(rand(`longblob`)),
    pg_typeof(rand(`text`))
FROM t1 where c1 = 1;

SELECT
    pg_typeof(round(`int1`)),
    pg_typeof(round(`uint1`)),
    pg_typeof(round(`int2`)),
    pg_typeof(round(`uint2`)),
    pg_typeof(round(`int4`)),
    pg_typeof(round(`uint4`)),
    pg_typeof(round(`int8`)),
    pg_typeof(round(`uint8`)),
    pg_typeof(round(`float4`)),
    pg_typeof(round(`float8`)),
    pg_typeof(round(`numeric`)),
    pg_typeof(round(`bit1`)),
    pg_typeof(round(`bit64`)),
    pg_typeof(round(`boolean`)),
    pg_typeof(round(`char`)),
    pg_typeof(round(`varchar`)),
    pg_typeof(round(`binary`)),
    pg_typeof(round(`varbinary`)),
    pg_typeof(round(`tinyblob`)),
    pg_typeof(round(`blob`)),
    pg_typeof(round(`mediumblob`)),
    pg_typeof(round(`longblob`)),
    pg_typeof(round(`text`))
FROM t1 where c1 = 1;

SELECT pg_typeof(mod(`int1`,`int1`)) FROM t1 where c1 = 1;
SELECT pg_typeof(mod(`uint1`,`uint1`)) FROM t1 where c1 = 1;
SELECT pg_typeof(mod(`int2`,`int2`)) FROM t1 where c1 = 1;
SELECT pg_typeof(mod(`uint2`,`uint2`)) FROM t1 where c1 = 1;
SELECT pg_typeof(mod(`int4`,`int4`)) FROM t1 where c1 = 1;
SELECT pg_typeof(mod(`uint4`,`uint4`)) FROM t1 where c1 = 1;
SELECT pg_typeof(mod(`int8`,`int8`)) FROM t1 where c1 = 1;
SELECT pg_typeof(mod(`uint8`,`uint8`)) FROM t1 where c1 = 1;
SELECT pg_typeof(mod(`float4`,`float4`)) FROM t1 where c1 = 1;
SELECT pg_typeof(mod(`float8`,`float8`)) FROM t1 where c1 = 1;
SELECT pg_typeof(mod(`numeric`,`numeric`)) FROM t1 where c1 = 1;
SELECT pg_typeof(mod(`bit1`,`bit1`)) FROM t1 where c1 = 1;
SELECT pg_typeof(mod(`bit64`,`bit64`)) FROM t1 where c1 = 1;
SELECT pg_typeof(mod(`boolean`,`boolean`)) FROM t1 where c1 = 1;
SELECT pg_typeof(mod(`char`,`char`)) FROM t1 where c1 = 1;
SELECT pg_typeof(mod(`varchar`,`varchar`)) FROM t1 where c1 = 1;
SELECT pg_typeof(mod(`binary`,`binary`)) FROM t1 where c1 = 1;
SELECT pg_typeof(mod(`varbinary`,`varbinary`)) FROM t1 where c1 = 1;
SELECT pg_typeof(mod(`tinyblob`,`tinyblob`)) FROM t1 where c1 = 1;
SELECT pg_typeof(mod(`blob`,`blob`)) FROM t1 where c1 = 1;
SELECT pg_typeof(mod(`mediumblob`,`mediumblob`)) FROM t1 where c1 = 1;
SELECT pg_typeof(mod(`longblob`,`longblob`)) FROM t1 where c1 = 1;
SELECT pg_typeof(mod(`text`,`text`)) FROM t1 where c1 = 1;

SELECT pg_typeof(pow(`int1`,`int1`)) FROM t1 where c1 = 1;
SELECT pg_typeof(pow(`uint1`,`uint1`)) FROM t1 where c1 = 1;
SELECT pg_typeof(pow(`int2`,`int2`)) FROM t1 where c1 = 1;
SELECT pg_typeof(pow(`uint2`,`uint2`)) FROM t1 where c1 = 1;
SELECT pg_typeof(pow(`int4`,`int4`)) FROM t1 where c1 = 1;
SELECT pg_typeof(pow(`uint4`,`uint4`)) FROM t1 where c1 = 1;
SELECT pg_typeof(pow(`int8`,`int8`)) FROM t1 where c1 = 1;
SELECT pg_typeof(pow(`uint8`,`uint8`)) FROM t1 where c1 = 1;
SELECT pg_typeof(pow(`float4`,`float4`)) FROM t1 where c1 = 1;
SELECT pg_typeof(pow(`float8`,`float8`)) FROM t1 where c1 = 1;
SELECT pg_typeof(pow(`numeric`,`numeric`)) FROM t1 where c1 = 1;
SELECT pg_typeof(pow(`bit1`,`bit1`)) FROM t1 where c1 = 1;
SELECT pg_typeof(pow(`bit64`,`bit64`)) FROM t1 where c1 = 1;
SELECT pg_typeof(pow(`boolean`,`boolean`)) FROM t1 where c1 = 1;
SELECT pg_typeof(pow(`char`,`char`)) FROM t1 where c1 = 1;
SELECT pg_typeof(pow(`varchar`,`varchar`)) FROM t1 where c1 = 1;
SELECT pg_typeof(pow(`binary`,`binary`)) FROM t1 where c1 = 1;
SELECT pg_typeof(pow(`varbinary`,`varbinary`)) FROM t1 where c1 = 1;
SELECT pg_typeof(pow(`tinyblob`,`tinyblob`)) FROM t1 where c1 = 1;
SELECT pg_typeof(pow(`blob`,`blob`)) FROM t1 where c1 = 1;
SELECT pg_typeof(pow(`mediumblob`,`mediumblob`)) FROM t1 where c1 = 1;
SELECT pg_typeof(pow(`longblob`,`longblob`)) FROM t1 where c1 = 1;
SELECT pg_typeof(pow(`text`,`text`)) FROM t1 where c1 = 1;

SELECT pg_typeof(power(`int1`,`int1`)) FROM t1 where c1 = 1;
SELECT pg_typeof(power(`uint1`,`uint1`)) FROM t1 where c1 = 1;
SELECT pg_typeof(power(`int2`,`int2`)) FROM t1 where c1 = 1;
SELECT pg_typeof(power(`uint2`,`uint2`)) FROM t1 where c1 = 1;
SELECT pg_typeof(power(`int4`,`int4`)) FROM t1 where c1 = 1;
SELECT pg_typeof(power(`uint4`,`uint4`)) FROM t1 where c1 = 1;
SELECT pg_typeof(power(`int8`,`int8`)) FROM t1 where c1 = 1;
SELECT pg_typeof(power(`uint8`,`uint8`)) FROM t1 where c1 = 1;
SELECT pg_typeof(power(`float4`,`float4`)) FROM t1 where c1 = 1;
SELECT pg_typeof(power(`float8`,`float8`)) FROM t1 where c1 = 1;
SELECT pg_typeof(power(`numeric`,`numeric`)) FROM t1 where c1 = 1;
SELECT pg_typeof(power(`bit1`,`bit1`)) FROM t1 where c1 = 1;
SELECT pg_typeof(power(`bit64`,`bit64`)) FROM t1 where c1 = 1;
SELECT pg_typeof(power(`boolean`,`boolean`)) FROM t1 where c1 = 1;
SELECT pg_typeof(power(`char`,`char`)) FROM t1 where c1 = 1;
SELECT pg_typeof(power(`varchar`,`varchar`)) FROM t1 where c1 = 1;
SELECT pg_typeof(power(`binary`,`binary`)) FROM t1 where c1 = 1;
SELECT pg_typeof(power(`varbinary`,`varbinary`)) FROM t1 where c1 = 1;
SELECT pg_typeof(power(`tinyblob`,`tinyblob`)) FROM t1 where c1 = 1;
SELECT pg_typeof(power(`blob`,`blob`)) FROM t1 where c1 = 1;
SELECT pg_typeof(power(`mediumblob`,`mediumblob`)) FROM t1 where c1 = 1;
SELECT pg_typeof(power(`longblob`,`longblob`)) FROM t1 where c1 = 1;
SELECT pg_typeof(power(`text`,`text`)) FROM t1 where c1 = 1;

drop table t1;

create table bit_test (`bit1` bit(1), `bit6` bit(6), `bit8` bit(8), `bit15` bit(15), `bit16` bit(16));
insert into bit_test values (1, 0x33, 0x68, 0x4d45, 0x5400);
select rand(`bit1`), rand(`bit6`), rand(`bit8`), rand(`bit15`), rand(`bit16`) from bit_test;
select round(`bit1`), round(`bit6`), round(`bit8`), round(`bit15`), round(`bit16`) from bit_test;

drop table bit_test;

drop schema db_test_rand_func cascade;
reset bytea_output;
reset dolphin.sql_mode;
reset dolphin.b_compatibility_mode;
reset current_schema;

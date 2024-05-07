create schema test_time_expr;
set current_schema to test_time_expr;
set dolphin.b_compatibility_mode = on;

-- test for 'bit_expr ± interval expr unit'
drop table if exists test_type_table cascade;
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

  `date`         date,
  `time`         time,
  `time(4)`      time(4),
  `datetime`     datetime,
  `datetime(4)`  datetime(4) default '2022-11-11 11:11:11',
  `timestamp`    timestamp,
  `timestamp(4)` timestamp(4) default '2022-11-11 11:11:11',
  `year`         year,

  `char`         char(100),
  `varchar`      varchar(100),
  `text`         text
);
insert into test_type_table
values (111, 111, 11111, 11111, 1111111111, 1111111111, 11111111111111, 11111111111111, 111, 1111111111111.1, 111,
        '2023-02-05', '19:10:50', '19:10:50.3456', '2023-02-05 19:10:50', '2023-02-05 19:10:50.456',
        '2023-02-05 19:10:50', '2023-02-05 19:10:50.456', '2023',
        '2023-02-05', '2023-02-05', '2023-02-05');

drop view if exists res_view;
create view res_view as select
`int1`  + interval 1 day as 'int1 + interval 1 day',
`uint1`  + interval 1 day as 'uint1 + interval 1 day',
`int2`  + interval 1 day as 'int2 + interval 1 day',
`uint2`  + interval 1 day as 'uint2 + interval 1 day',
`int4`  + interval 1 day as 'int4 + interval 1 day',
`uint4`  + interval 1 day as 'uint4 + interval 1 day',
`int8`  + interval 1 day as 'int8 + interval 1 day',
`uint8`  + interval 1 day as 'uint8 + interval 1 day',
`float4`  + interval 1 day as 'float4 + interval 1 day',
`float8`  + interval 1 day as 'float8 + interval 1 day',
`numeric`  + interval 1 day as 'numeric + interval 1 day',
`date`  + interval 1 day as 'date + interval 1 day',
`time`  + interval 1 day as 'time + interval 1 day',
`time(4)`  + interval 1 day as 'time(4) + interval 1 day',
`datetime`  + interval 1 day as 'datetime + interval 1 day',
`datetime(4)`  + interval 1 day as 'datetime(4) + interval 1 day',
`timestamp`  + interval 1 day as 'timestamp + interval 1 day',
`timestamp(4)`  + interval 1 day as 'timestamp(4) + interval 1 day',
`year`  + interval 1 day as 'year + interval 1 day',
`char`  + interval 1 day as 'char + interval 1 day',
`varchar`  + interval 1 day as 'varchar + interval 1 day',
`text`  + interval 1 day as 'text + interval 1 day' 
from test_type_table;
\x
select * from res_view;
\x
desc res_view;

drop view if exists res_view;
create view res_view as select
interval 1 day + `int1` as 'interval 1 day + int1',
interval 1 day + `uint1` as 'interval 1 day + uint1',
interval 1 day + `int2` as 'interval 1 day + int2',
interval 1 day + `uint2` as 'interval 1 day + uint2',
interval 1 day + `int4` as 'interval 1 day + int4',
interval 1 day + `uint4` as 'interval 1 day + uint4',
interval 1 day + `int8` as 'interval 1 day + int8',
interval 1 day + `uint8` as 'interval 1 day + uint8',
interval 1 day + `float4` as 'interval 1 day + float4',
interval 1 day + `float8` as 'interval 1 day + float8',
interval 1 day + `numeric` as 'interval 1 day + numeric',
interval 1 day + `date` as 'interval 1 day + date',
interval 1 day + `time` as 'interval 1 day + time',
interval 1 day + `time(4)` as 'interval 1 day + time(4)',
interval 1 day + `datetime` as 'interval 1 day + datetime',
interval 1 day + `datetime(4)` as 'interval 1 day + datetime(4)',
interval 1 day + `timestamp` as 'interval 1 day + timestamp',
interval 1 day + `timestamp(4)` as 'interval 1 day + timestamp(4)',
interval 1 day + `year` as 'interval 1 day + year',
interval 1 day + `char` as 'interval 1 day + char',
interval 1 day + `varchar` as 'interval 1 day + varchar',
interval 1 day + `text` as 'interval 1 day + text'
from test_type_table;
\x
select * from res_view;
\x
desc res_view;

drop view if exists res_view;
create view res_view as select
`int1`  - interval 1 day as 'int1 - interval 1 day',
`uint1`  - interval 1 day as 'uint1 - interval 1 day',
`int2`  - interval 1 day as 'int2 - interval 1 day',
`uint2`  - interval 1 day as 'uint2 - interval 1 day',
`int4`  - interval 1 day as 'int4 - interval 1 day',
`uint4`  - interval 1 day as 'uint4 - interval 1 day',
`int8`  - interval 1 day as 'int8 - interval 1 day',
`uint8`  - interval 1 day as 'uint8 - interval 1 day',
`float4`  - interval 1 day as 'float4 - interval 1 day',
`float8`  - interval 1 day as 'float8 - interval 1 day',
`numeric`  - interval 1 day as 'numeric - interval 1 day',
`date`  - interval 1 day as 'date - interval 1 day',
`time`  - interval 1 day as 'time - interval 1 day',
`time(4)`  - interval 1 day as 'time(4) - interval 1 day',
`datetime`  - interval 1 day as 'datetime - interval 1 day',
`datetime(4)`  - interval 1 day as 'datetime(4) - interval 1 day',
`timestamp`  - interval 1 day as 'timestamp - interval 1 day',
`timestamp(4)`  - interval 1 day as 'timestamp(4) - interval 1 day',
`year`  - interval 1 day as 'year - interval 1 day',
`char`  - interval 1 day as 'char - interval 1 day',
`varchar`  - interval 1 day as 'varchar - interval 1 day',
`text`  - interval 1 day as 'text - interval 1 day' 
from test_type_table;
\x
select * from res_view;
\x
desc res_view;

-- test whether support default
drop table if exists test_def_interval_pl;
create table test_def_interval_pl(
  a datetime default ('2020-01-01 00:00:00' + interval 1 day),
  b datetime default (interval 1 day + '2020-01-01 00:00:00'),
  c datetime default ('2020-01-01 00:00:00' - interval 1 day)
);
desc test_def_interval_pl;
insert into test_def_interval_pl values(default, default, default);
select * from test_def_interval_pl;
drop table if exists test_def_interval_pl;

-- no strict mode
set dolphin.sql_mode = '';
drop table if exists test_type_table_null;
create table test_type_table_null (like test_type_table);
insert into test_type_table_null values(null, null, null, null, null, null, null, null, null, null, null, null, null, null, null, null, null, null, null, null, null, null);

drop table if exists test_type_table_zero;
create table test_type_table_zero (like test_type_table);
insert into test_type_table_zero values(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);

drop table if exists test_type_table_invalid;
create table test_type_table_invalid (like test_type_table);
insert into test_type_table_invalid values(-1, 1, -1, 1, -1, 1, -1, 1, -1, -1, -1, 0, 0, 0, 0, 0, 0, 0, '2023', 'a', 'a', 'a');

drop table if exists res_tab;
create table res_tab as select
`int1` as 'int1', `int1`  + interval 1 day as 'int1 + interval 1 day',
`uint1` as 'uint1', `uint1`  + interval 1 day as 'uint1 + interval 1 day',
`int2` as 'int2', `int2`  + interval 1 day as 'int2 + interval 1 day',
`uint2` as 'uint2', `uint2`  + interval 1 day as 'uint2 + interval 1 day',
`int4` as 'int4', `int4`  + interval 1 day as 'int4 + interval 1 day',
`uint4` as 'uint4', `uint4`  + interval 1 day as 'uint4 + interval 1 day',
`int8` as 'int8', `int8`  + interval 1 day as 'int8 + interval 1 day',
`uint8` as 'uint8', `uint8`  + interval 1 day as 'uint8 + interval 1 day',
`float4` as 'float4', `float4`  + interval 1 day as 'float4 + interval 1 day',
`float8` as 'float8', `float8`  + interval 1 day as 'float8 + interval 1 day',
`numeric` as 'numeric', `numeric`  + interval 1 day as 'numeric + interval 1 day',
`date` as 'date', `date`  + interval 1 day as 'date + interval 1 day',
`time` as 'time', `time`  + interval 1 day as 'time + interval 1 day',
`time(4)` as 'time(4)', `time(4)`  + interval 1 day as 'time(4) + interval 1 day',
`datetime` as 'datetime', `datetime`  + interval 1 day as 'datetime + interval 1 day',
`datetime(4)` as 'datetime(4)', `datetime(4)`  + interval 1 day as 'datetime(4) + interval 1 day',
`timestamp` as 'timestamp', `timestamp`  + interval 1 day as 'timestamp + interval 1 day',
`timestamp(4)` as 'timestamp(4)', `timestamp(4)`  + interval 1 day as 'timestamp(4) + interval 1 day',
`year` as 'year', `year`  + interval 1 day as 'year + interval 1 day',
`char` as 'char', `char`  + interval 1 day as 'char + interval 1 day',
`varchar` as 'varchar', `varchar`  + interval 1 day as 'varchar + interval 1 day',
`text` as 'text', `text`  + interval 1 day as 'text + interval 1 day' 
from test_type_table_null;
\x
select * from res_tab;
\x
desc res_tab;
drop table if exists res_tab;
create table res_tab as select
`int1` as 'int1', interval 1 day + `int1` as 'interval 1 day + int1',
`uint1` as 'uint1', interval 1 day + `uint1` as 'interval 1 day + uint1',
`int2` as 'int2', interval 1 day + `int2` as 'interval 1 day + int2',
`uint2` as 'uint2', interval 1 day + `uint2` as 'interval 1 day + uint2',
`int4` as 'int4', interval 1 day + `int4` as 'interval 1 day + int4',
`uint4` as 'uint4', interval 1 day + `uint4` as 'interval 1 day + uint4',
`int8` as 'int8', interval 1 day + `int8` as 'interval 1 day + int8',
`uint8` as 'uint8', interval 1 day + `uint8` as 'interval 1 day + uint8',
`float4` as 'float4', interval 1 day + `float4` as 'interval 1 day + float4',
`float8` as 'float8', interval 1 day + `float8` as 'interval 1 day + float8',
`numeric` as 'numeric', interval 1 day + `numeric` as 'interval 1 day + numeric',
`date` as 'date', interval 1 day + `date` as 'interval 1 day + date',
`time` as 'time', interval 1 day + `time` as 'interval 1 day + time',
`time(4)` as 'time(4)', interval 1 day + `time(4)` as 'interval 1 day + time(4)',
`datetime` as 'datetime', interval 1 day + `datetime` as 'interval 1 day + datetime',
`datetime(4)` as 'datetime(4)', interval 1 day + `datetime(4)` as 'interval 1 day + datetime(4)',
`timestamp` as 'timestamp', interval 1 day + `timestamp` as 'interval 1 day + timestamp',
`timestamp(4)` as 'timestamp(4)', interval 1 day + `timestamp(4)` as 'interval 1 day + timestamp(4)',
`year` as 'year', interval 1 day + `year` as 'interval 1 day + year',
`char` as 'char', interval 1 day + `char` as 'interval 1 day + char',
`varchar` as 'varchar', interval 1 day + `varchar` as 'interval 1 day + varchar',
`text` as 'text', interval 1 day + `text` as 'interval 1 day + text' 
from test_type_table_null;
\x
select * from res_tab;
\x
desc res_tab;
drop table if exists res_tab;
create table res_tab as select
`int1` as 'int1', `int1`  - interval 1 day as 'int1 - interval 1 day',
`uint1` as 'uint1', `uint1`  - interval 1 day as 'uint1 - interval 1 day',
`int2` as 'int2', `int2`  - interval 1 day as 'int2 - interval 1 day',
`uint2` as 'uint2', `uint2`  - interval 1 day as 'uint2 - interval 1 day',
`int4` as 'int4', `int4`  - interval 1 day as 'int4 - interval 1 day',
`uint4` as 'uint4', `uint4`  - interval 1 day as 'uint4 - interval 1 day',
`int8` as 'int8', `int8`  - interval 1 day as 'int8 - interval 1 day',
`uint8` as 'uint8', `uint8`  - interval 1 day as 'uint8 - interval 1 day',
`float4` as 'float4', `float4`  - interval 1 day as 'float4 - interval 1 day',
`float8` as 'float8', `float8`  - interval 1 day as 'float8 - interval 1 day',
`numeric` as 'numeric', `numeric`  - interval 1 day as 'numeric - interval 1 day',
`date` as 'date', `date`  - interval 1 day as 'date - interval 1 day',
`time` as 'time', `time`  - interval 1 day as 'time - interval 1 day',
`time(4)` as 'time(4)', `time(4)`  - interval 1 day as 'time(4) - interval 1 day',
`datetime` as 'datetime', `datetime`  - interval 1 day as 'datetime - interval 1 day',
`datetime(4)` as 'datetime(4)', `datetime(4)`  - interval 1 day as 'datetime(4) - interval 1 day',
`timestamp` as 'timestamp', `timestamp`  - interval 1 day as 'timestamp - interval 1 day',
`timestamp(4)` as 'timestamp(4)', `timestamp(4)`  - interval 1 day as 'timestamp(4) - interval 1 day',
`year` as 'year', `year`  - interval 1 day as 'year - interval 1 day',
`char` as 'char', `char`  - interval 1 day as 'char - interval 1 day',
`varchar` as 'varchar', `varchar`  - interval 1 day as 'varchar - interval 1 day',
`text` as 'text', `text`  - interval 1 day as 'text - interval 1 day' 
from test_type_table_null;
\x
select * from res_tab;
\x
desc res_tab;

drop table if exists res_tab;
create table res_tab as select
`int1` as 'int1', `int1`  + interval 1 day as 'int1 + interval 1 day',
`uint1` as 'uint1', `uint1`  + interval 1 day as 'uint1 + interval 1 day',
`int2` as 'int2', `int2`  + interval 1 day as 'int2 + interval 1 day',
`uint2` as 'uint2', `uint2`  + interval 1 day as 'uint2 + interval 1 day',
`int4` as 'int4', `int4`  + interval 1 day as 'int4 + interval 1 day',
`uint4` as 'uint4', `uint4`  + interval 1 day as 'uint4 + interval 1 day',
`int8` as 'int8', `int8`  + interval 1 day as 'int8 + interval 1 day',
`uint8` as 'uint8', `uint8`  + interval 1 day as 'uint8 + interval 1 day',
`float4` as 'float4', `float4`  + interval 1 day as 'float4 + interval 1 day',
`float8` as 'float8', `float8`  + interval 1 day as 'float8 + interval 1 day',
`numeric` as 'numeric', `numeric`  + interval 1 day as 'numeric + interval 1 day',
`date` as 'date', `date`  + interval 1 day as 'date + interval 1 day',
`time` as 'time', `time`  + interval 1 day as 'time + interval 1 day',
`time(4)` as 'time(4)', `time(4)`  + interval 1 day as 'time(4) + interval 1 day',
`datetime` as 'datetime', `datetime`  + interval 1 day as 'datetime + interval 1 day',
`datetime(4)` as 'datetime(4)', `datetime(4)`  + interval 1 day as 'datetime(4) + interval 1 day',
`timestamp` as 'timestamp', `timestamp`  + interval 1 day as 'timestamp + interval 1 day',
`timestamp(4)` as 'timestamp(4)', `timestamp(4)`  + interval 1 day as 'timestamp(4) + interval 1 day',
`year` as 'year', `year`  + interval 1 day as 'year + interval 1 day',
`char` as 'char', `char`  + interval 1 day as 'char + interval 1 day',
`varchar` as 'varchar', `varchar`  + interval 1 day as 'varchar + interval 1 day',
`text` as 'text', `text`  + interval 1 day as 'text + interval 1 day' 
from test_type_table_zero;
\x
select * from res_tab;
\x
desc res_tab;
drop table if exists res_tab;
create table res_tab as select
`int1` as 'int1', interval 1 day + `int1` as 'interval 1 day + int1',
`uint1` as 'uint1', interval 1 day + `uint1` as 'interval 1 day + uint1',
`int2` as 'int2', interval 1 day + `int2` as 'interval 1 day + int2',
`uint2` as 'uint2', interval 1 day + `uint2` as 'interval 1 day + uint2',
`int4` as 'int4', interval 1 day + `int4` as 'interval 1 day + int4',
`uint4` as 'uint4', interval 1 day + `uint4` as 'interval 1 day + uint4',
`int8` as 'int8', interval 1 day + `int8` as 'interval 1 day + int8',
`uint8` as 'uint8', interval 1 day + `uint8` as 'interval 1 day + uint8',
`float4` as 'float4', interval 1 day + `float4` as 'interval 1 day + float4',
`float8` as 'float8', interval 1 day + `float8` as 'interval 1 day + float8',
`numeric` as 'numeric', interval 1 day + `numeric` as 'interval 1 day + numeric',
`date` as 'date', interval 1 day + `date` as 'interval 1 day + date',
`time` as 'time', interval 1 day + `time` as 'interval 1 day + time',
`time(4)` as 'time(4)', interval 1 day + `time(4)` as 'interval 1 day + time(4)',
`datetime` as 'datetime', interval 1 day + `datetime` as 'interval 1 day + datetime',
`datetime(4)` as 'datetime(4)', interval 1 day + `datetime(4)` as 'interval 1 day + datetime(4)',
`timestamp` as 'timestamp', interval 1 day + `timestamp` as 'interval 1 day + timestamp',
`timestamp(4)` as 'timestamp(4)', interval 1 day + `timestamp(4)` as 'interval 1 day + timestamp(4)',
`year` as 'year', interval 1 day + `year` as 'interval 1 day + year',
`char` as 'char', interval 1 day + `char` as 'interval 1 day + char',
`varchar` as 'varchar', interval 1 day + `varchar` as 'interval 1 day + varchar',
`text` as 'text', interval 1 day + `text` as 'interval 1 day + text' 
from test_type_table_zero;
\x
select * from res_tab;
\x
desc res_tab;
drop table if exists res_tab;
create table res_tab as select
`int1` as 'int1', `int1`  - interval 1 day as 'int1 - interval 1 day',
`uint1` as 'uint1', `uint1`  - interval 1 day as 'uint1 - interval 1 day',
`int2` as 'int2', `int2`  - interval 1 day as 'int2 - interval 1 day',
`uint2` as 'uint2', `uint2`  - interval 1 day as 'uint2 - interval 1 day',
`int4` as 'int4', `int4`  - interval 1 day as 'int4 - interval 1 day',
`uint4` as 'uint4', `uint4`  - interval 1 day as 'uint4 - interval 1 day',
`int8` as 'int8', `int8`  - interval 1 day as 'int8 - interval 1 day',
`uint8` as 'uint8', `uint8`  - interval 1 day as 'uint8 - interval 1 day',
`float4` as 'float4', `float4`  - interval 1 day as 'float4 - interval 1 day',
`float8` as 'float8', `float8`  - interval 1 day as 'float8 - interval 1 day',
`numeric` as 'numeric', `numeric`  - interval 1 day as 'numeric - interval 1 day',
`date` as 'date', `date`  - interval 1 day as 'date - interval 1 day',
`time` as 'time', `time`  - interval 1 day as 'time - interval 1 day',
`time(4)` as 'time(4)', `time(4)`  - interval 1 day as 'time(4) - interval 1 day',
`datetime` as 'datetime', `datetime`  - interval 1 day as 'datetime - interval 1 day',
`datetime(4)` as 'datetime(4)', `datetime(4)`  - interval 1 day as 'datetime(4) - interval 1 day',
`timestamp` as 'timestamp', `timestamp`  - interval 1 day as 'timestamp - interval 1 day',
`timestamp(4)` as 'timestamp(4)', `timestamp(4)`  - interval 1 day as 'timestamp(4) - interval 1 day',
`year` as 'year', `year`  - interval 1 day as 'year - interval 1 day',
`char` as 'char', `char`  - interval 1 day as 'char - interval 1 day',
`varchar` as 'varchar', `varchar`  - interval 1 day as 'varchar - interval 1 day',
`text` as 'text', `text`  - interval 1 day as 'text - interval 1 day' 
from test_type_table_zero;
\x
select * from res_tab;
\x
desc res_tab;

drop table if exists res_tab;
create table res_tab as select
`int1` as 'int1', `int1`  + interval 1 day as 'int1 + interval 1 day',
`uint1` as 'uint1', `uint1`  + interval 1 day as 'uint1 + interval 1 day',
`int2` as 'int2', `int2`  + interval 1 day as 'int2 + interval 1 day',
`uint2` as 'uint2', `uint2`  + interval 1 day as 'uint2 + interval 1 day',
`int4` as 'int4', `int4`  + interval 1 day as 'int4 + interval 1 day',
`uint4` as 'uint4', `uint4`  + interval 1 day as 'uint4 + interval 1 day',
`int8` as 'int8', `int8`  + interval 1 day as 'int8 + interval 1 day',
`uint8` as 'uint8', `uint8`  + interval 1 day as 'uint8 + interval 1 day',
`float4` as 'float4', `float4`  + interval 1 day as 'float4 + interval 1 day',
`float8` as 'float8', `float8`  + interval 1 day as 'float8 + interval 1 day',
`numeric` as 'numeric', `numeric`  + interval 1 day as 'numeric + interval 1 day',
`date` as 'date', `date`  + interval 1 day as 'date + interval 1 day',
`time` as 'time', `time`  + interval 1 day as 'time + interval 1 day',
`time(4)` as 'time(4)', `time(4)`  + interval 1 day as 'time(4) + interval 1 day',
`datetime` as 'datetime', `datetime`  + interval 1 day as 'datetime + interval 1 day',
`datetime(4)` as 'datetime(4)', `datetime(4)`  + interval 1 day as 'datetime(4) + interval 1 day',
`timestamp` as 'timestamp', `timestamp`  + interval 1 day as 'timestamp + interval 1 day',
`timestamp(4)` as 'timestamp(4)', `timestamp(4)`  + interval 1 day as 'timestamp(4) + interval 1 day',
`year` as 'year', `year`  + interval 1 day as 'year + interval 1 day',
`char` as 'char', `char`  + interval 1 day as 'char + interval 1 day',
`varchar` as 'varchar', `varchar`  + interval 1 day as 'varchar + interval 1 day',
`text` as 'text', `text`  + interval 1 day as 'text + interval 1 day' 
from test_type_table_invalid;
\x
select * from res_tab;
\x
desc res_tab;
drop table if exists res_tab;
create table res_tab as select
`int1` as 'int1', interval 1 day + `int1` as 'interval 1 day + int1',
`uint1` as 'uint1', interval 1 day + `uint1` as 'interval 1 day + uint1',
`int2` as 'int2', interval 1 day + `int2` as 'interval 1 day + int2',
`uint2` as 'uint2', interval 1 day + `uint2` as 'interval 1 day + uint2',
`int4` as 'int4', interval 1 day + `int4` as 'interval 1 day + int4',
`uint4` as 'uint4', interval 1 day + `uint4` as 'interval 1 day + uint4',
`int8` as 'int8', interval 1 day + `int8` as 'interval 1 day + int8',
`uint8` as 'uint8', interval 1 day + `uint8` as 'interval 1 day + uint8',
`float4` as 'float4', interval 1 day + `float4` as 'interval 1 day + float4',
`float8` as 'float8', interval 1 day + `float8` as 'interval 1 day + float8',
`numeric` as 'numeric', interval 1 day + `numeric` as 'interval 1 day + numeric',
`date` as 'date', interval 1 day + `date` as 'interval 1 day + date',
`time` as 'time', interval 1 day + `time` as 'interval 1 day + time',
`time(4)` as 'time(4)', interval 1 day + `time(4)` as 'interval 1 day + time(4)',
`datetime` as 'datetime', interval 1 day + `datetime` as 'interval 1 day + datetime',
`datetime(4)` as 'datetime(4)', interval 1 day + `datetime(4)` as 'interval 1 day + datetime(4)',
`timestamp` as 'timestamp', interval 1 day + `timestamp` as 'interval 1 day + timestamp',
`timestamp(4)` as 'timestamp(4)', interval 1 day + `timestamp(4)` as 'interval 1 day + timestamp(4)',
`year` as 'year', interval 1 day + `year` as 'interval 1 day + year',
`char` as 'char', interval 1 day + `char` as 'interval 1 day + char',
`varchar` as 'varchar', interval 1 day + `varchar` as 'interval 1 day + varchar',
`text` as 'text', interval 1 day + `text` as 'interval 1 day + text' 
from test_type_table_invalid;
\x
select * from res_tab;
\x
desc res_tab;
drop table if exists res_tab;
create table res_tab as select
`int1` as 'int1', `int1`  - interval 1 day as 'int1 - interval 1 day',
`uint1` as 'uint1', `uint1`  - interval 1 day as 'uint1 - interval 1 day',
`int2` as 'int2', `int2`  - interval 1 day as 'int2 - interval 1 day',
`uint2` as 'uint2', `uint2`  - interval 1 day as 'uint2 - interval 1 day',
`int4` as 'int4', `int4`  - interval 1 day as 'int4 - interval 1 day',
`uint4` as 'uint4', `uint4`  - interval 1 day as 'uint4 - interval 1 day',
`int8` as 'int8', `int8`  - interval 1 day as 'int8 - interval 1 day',
`uint8` as 'uint8', `uint8`  - interval 1 day as 'uint8 - interval 1 day',
`float4` as 'float4', `float4`  - interval 1 day as 'float4 - interval 1 day',
`float8` as 'float8', `float8`  - interval 1 day as 'float8 - interval 1 day',
`numeric` as 'numeric', `numeric`  - interval 1 day as 'numeric - interval 1 day',
`date` as 'date', `date`  - interval 1 day as 'date - interval 1 day',
`time` as 'time', `time`  - interval 1 day as 'time - interval 1 day',
`time(4)` as 'time(4)', `time(4)`  - interval 1 day as 'time(4) - interval 1 day',
`datetime` as 'datetime', `datetime`  - interval 1 day as 'datetime - interval 1 day',
`datetime(4)` as 'datetime(4)', `datetime(4)`  - interval 1 day as 'datetime(4) - interval 1 day',
`timestamp` as 'timestamp', `timestamp`  - interval 1 day as 'timestamp - interval 1 day',
`timestamp(4)` as 'timestamp(4)', `timestamp(4)`  - interval 1 day as 'timestamp(4) - interval 1 day',
`year` as 'year', `year`  - interval 1 day as 'year - interval 1 day',
`char` as 'char', `char`  - interval 1 day as 'char - interval 1 day',
`varchar` as 'varchar', `varchar`  - interval 1 day as 'varchar - interval 1 day',
`text` as 'text', `text`  - interval 1 day as 'text - interval 1 day' 
from test_type_table_invalid;
\x
select * from res_tab;
\x
desc res_tab;
reset dolphin.sql_mode;

-- strict mode
set dolphin.sql_mode = 'sql_mode_strict';
drop table if exists res_tab;
create table res_tab as select
`int1` as 'int1', `int1`  + interval 1 day as 'int1 + interval 1 day',
`uint1` as 'uint1', `uint1`  + interval 1 day as 'uint1 + interval 1 day',
`int2` as 'int2', `int2`  + interval 1 day as 'int2 + interval 1 day',
`uint2` as 'uint2', `uint2`  + interval 1 day as 'uint2 + interval 1 day',
`int4` as 'int4', `int4`  + interval 1 day as 'int4 + interval 1 day',
`uint4` as 'uint4', `uint4`  + interval 1 day as 'uint4 + interval 1 day',
`int8` as 'int8', `int8`  + interval 1 day as 'int8 + interval 1 day',
`uint8` as 'uint8', `uint8`  + interval 1 day as 'uint8 + interval 1 day',
`float4` as 'float4', `float4`  + interval 1 day as 'float4 + interval 1 day',
`float8` as 'float8', `float8`  + interval 1 day as 'float8 + interval 1 day',
`numeric` as 'numeric', `numeric`  + interval 1 day as 'numeric + interval 1 day',
`date` as 'date', `date`  + interval 1 day as 'date + interval 1 day',
`time` as 'time', `time`  + interval 1 day as 'time + interval 1 day',
`time(4)` as 'time(4)', `time(4)`  + interval 1 day as 'time(4) + interval 1 day',
`datetime` as 'datetime', `datetime`  + interval 1 day as 'datetime + interval 1 day',
`datetime(4)` as 'datetime(4)', `datetime(4)`  + interval 1 day as 'datetime(4) + interval 1 day',
`timestamp` as 'timestamp', `timestamp`  + interval 1 day as 'timestamp + interval 1 day',
`timestamp(4)` as 'timestamp(4)', `timestamp(4)`  + interval 1 day as 'timestamp(4) + interval 1 day',
`year` as 'year', `year`  + interval 1 day as 'year + interval 1 day',
`char` as 'char', `char`  + interval 1 day as 'char + interval 1 day',
`varchar` as 'varchar', `varchar`  + interval 1 day as 'varchar + interval 1 day',
`text` as 'text', `text`  + interval 1 day as 'text + interval 1 day' 
from test_type_table_null;
\x
select * from res_tab;
\x
desc res_tab;
drop table if exists res_tab;
create table res_tab as select
`int1` as 'int1', interval 1 day + `int1` as 'interval 1 day + int1',
`uint1` as 'uint1', interval 1 day + `uint1` as 'interval 1 day + uint1',
`int2` as 'int2', interval 1 day + `int2` as 'interval 1 day + int2',
`uint2` as 'uint2', interval 1 day + `uint2` as 'interval 1 day + uint2',
`int4` as 'int4', interval 1 day + `int4` as 'interval 1 day + int4',
`uint4` as 'uint4', interval 1 day + `uint4` as 'interval 1 day + uint4',
`int8` as 'int8', interval 1 day + `int8` as 'interval 1 day + int8',
`uint8` as 'uint8', interval 1 day + `uint8` as 'interval 1 day + uint8',
`float4` as 'float4', interval 1 day + `float4` as 'interval 1 day + float4',
`float8` as 'float8', interval 1 day + `float8` as 'interval 1 day + float8',
`numeric` as 'numeric', interval 1 day + `numeric` as 'interval 1 day + numeric',
`date` as 'date', interval 1 day + `date` as 'interval 1 day + date',
`time` as 'time', interval 1 day + `time` as 'interval 1 day + time',
`time(4)` as 'time(4)', interval 1 day + `time(4)` as 'interval 1 day + time(4)',
`datetime` as 'datetime', interval 1 day + `datetime` as 'interval 1 day + datetime',
`datetime(4)` as 'datetime(4)', interval 1 day + `datetime(4)` as 'interval 1 day + datetime(4)',
`timestamp` as 'timestamp', interval 1 day + `timestamp` as 'interval 1 day + timestamp',
`timestamp(4)` as 'timestamp(4)', interval 1 day + `timestamp(4)` as 'interval 1 day + timestamp(4)',
`year` as 'year', interval 1 day + `year` as 'interval 1 day + year',
`char` as 'char', interval 1 day + `char` as 'interval 1 day + char',
`varchar` as 'varchar', interval 1 day + `varchar` as 'interval 1 day + varchar',
`text` as 'text', interval 1 day + `text` as 'interval 1 day + text' 
from test_type_table_null;
\x
select * from res_tab;
\x
desc res_tab;
drop table if exists res_tab;
create table res_tab as select
`int1` as 'int1', `int1`  - interval 1 day as 'int1 - interval 1 day',
`uint1` as 'uint1', `uint1`  - interval 1 day as 'uint1 - interval 1 day',
`int2` as 'int2', `int2`  - interval 1 day as 'int2 - interval 1 day',
`uint2` as 'uint2', `uint2`  - interval 1 day as 'uint2 - interval 1 day',
`int4` as 'int4', `int4`  - interval 1 day as 'int4 - interval 1 day',
`uint4` as 'uint4', `uint4`  - interval 1 day as 'uint4 - interval 1 day',
`int8` as 'int8', `int8`  - interval 1 day as 'int8 - interval 1 day',
`uint8` as 'uint8', `uint8`  - interval 1 day as 'uint8 - interval 1 day',
`float4` as 'float4', `float4`  - interval 1 day as 'float4 - interval 1 day',
`float8` as 'float8', `float8`  - interval 1 day as 'float8 - interval 1 day',
`numeric` as 'numeric', `numeric`  - interval 1 day as 'numeric - interval 1 day',
`date` as 'date', `date`  - interval 1 day as 'date - interval 1 day',
`time` as 'time', `time`  - interval 1 day as 'time - interval 1 day',
`time(4)` as 'time(4)', `time(4)`  - interval 1 day as 'time(4) - interval 1 day',
`datetime` as 'datetime', `datetime`  - interval 1 day as 'datetime - interval 1 day',
`datetime(4)` as 'datetime(4)', `datetime(4)`  - interval 1 day as 'datetime(4) - interval 1 day',
`timestamp` as 'timestamp', `timestamp`  - interval 1 day as 'timestamp - interval 1 day',
`timestamp(4)` as 'timestamp(4)', `timestamp(4)`  - interval 1 day as 'timestamp(4) - interval 1 day',
`year` as 'year', `year`  - interval 1 day as 'year - interval 1 day',
`char` as 'char', `char`  - interval 1 day as 'char - interval 1 day',
`varchar` as 'varchar', `varchar`  - interval 1 day as 'varchar - interval 1 day',
`text` as 'text', `text`  - interval 1 day as 'text - interval 1 day' 
from test_type_table_null;
\x
select * from res_tab;
\x
desc res_tab;

drop table if exists res_tab;
create table res_tab as select
`int1` as 'int1', `int1`  + interval 1 day as 'int1 + interval 1 day',
`uint1` as 'uint1', `uint1`  + interval 1 day as 'uint1 + interval 1 day',
`int2` as 'int2', `int2`  + interval 1 day as 'int2 + interval 1 day',
`uint2` as 'uint2', `uint2`  + interval 1 day as 'uint2 + interval 1 day',
`int4` as 'int4', `int4`  + interval 1 day as 'int4 + interval 1 day',
`uint4` as 'uint4', `uint4`  + interval 1 day as 'uint4 + interval 1 day',
`int8` as 'int8', `int8`  + interval 1 day as 'int8 + interval 1 day',
`uint8` as 'uint8', `uint8`  + interval 1 day as 'uint8 + interval 1 day',
`float4` as 'float4', `float4`  + interval 1 day as 'float4 + interval 1 day',
`float8` as 'float8', `float8`  + interval 1 day as 'float8 + interval 1 day',
`numeric` as 'numeric', `numeric`  + interval 1 day as 'numeric + interval 1 day',
`date` as 'date', `date`  + interval 1 day as 'date + interval 1 day',
`time` as 'time', `time`  + interval 1 day as 'time + interval 1 day',
`time(4)` as 'time(4)', `time(4)`  + interval 1 day as 'time(4) + interval 1 day',
`datetime` as 'datetime', `datetime`  + interval 1 day as 'datetime + interval 1 day',
`datetime(4)` as 'datetime(4)', `datetime(4)`  + interval 1 day as 'datetime(4) + interval 1 day',
`timestamp` as 'timestamp', `timestamp`  + interval 1 day as 'timestamp + interval 1 day',
`timestamp(4)` as 'timestamp(4)', `timestamp(4)`  + interval 1 day as 'timestamp(4) + interval 1 day',
`year` as 'year', `year`  + interval 1 day as 'year + interval 1 day',
`char` as 'char', `char`  + interval 1 day as 'char + interval 1 day',
`varchar` as 'varchar', `varchar`  + interval 1 day as 'varchar + interval 1 day',
`text` as 'text', `text`  + interval 1 day as 'text + interval 1 day' 
from test_type_table_zero;
\x
select * from res_tab;
\x
desc res_tab;
drop table if exists res_tab;
create table res_tab as select
`int1` as 'int1', interval 1 day + `int1` as 'interval 1 day + int1',
`uint1` as 'uint1', interval 1 day + `uint1` as 'interval 1 day + uint1',
`int2` as 'int2', interval 1 day + `int2` as 'interval 1 day + int2',
`uint2` as 'uint2', interval 1 day + `uint2` as 'interval 1 day + uint2',
`int4` as 'int4', interval 1 day + `int4` as 'interval 1 day + int4',
`uint4` as 'uint4', interval 1 day + `uint4` as 'interval 1 day + uint4',
`int8` as 'int8', interval 1 day + `int8` as 'interval 1 day + int8',
`uint8` as 'uint8', interval 1 day + `uint8` as 'interval 1 day + uint8',
`float4` as 'float4', interval 1 day + `float4` as 'interval 1 day + float4',
`float8` as 'float8', interval 1 day + `float8` as 'interval 1 day + float8',
`numeric` as 'numeric', interval 1 day + `numeric` as 'interval 1 day + numeric',
`date` as 'date', interval 1 day + `date` as 'interval 1 day + date',
`time` as 'time', interval 1 day + `time` as 'interval 1 day + time',
`time(4)` as 'time(4)', interval 1 day + `time(4)` as 'interval 1 day + time(4)',
`datetime` as 'datetime', interval 1 day + `datetime` as 'interval 1 day + datetime',
`datetime(4)` as 'datetime(4)', interval 1 day + `datetime(4)` as 'interval 1 day + datetime(4)',
`timestamp` as 'timestamp', interval 1 day + `timestamp` as 'interval 1 day + timestamp',
`timestamp(4)` as 'timestamp(4)', interval 1 day + `timestamp(4)` as 'interval 1 day + timestamp(4)',
`year` as 'year', interval 1 day + `year` as 'interval 1 day + year',
`char` as 'char', interval 1 day + `char` as 'interval 1 day + char',
`varchar` as 'varchar', interval 1 day + `varchar` as 'interval 1 day + varchar',
`text` as 'text', interval 1 day + `text` as 'interval 1 day + text' 
from test_type_table_zero;
\x
select * from res_tab;
\x
desc res_tab;
drop table if exists res_tab;
create table res_tab as select
`int1` as 'int1', `int1`  - interval 1 day as 'int1 - interval 1 day',
`uint1` as 'uint1', `uint1`  - interval 1 day as 'uint1 - interval 1 day',
`int2` as 'int2', `int2`  - interval 1 day as 'int2 - interval 1 day',
`uint2` as 'uint2', `uint2`  - interval 1 day as 'uint2 - interval 1 day',
`int4` as 'int4', `int4`  - interval 1 day as 'int4 - interval 1 day',
`uint4` as 'uint4', `uint4`  - interval 1 day as 'uint4 - interval 1 day',
`int8` as 'int8', `int8`  - interval 1 day as 'int8 - interval 1 day',
`uint8` as 'uint8', `uint8`  - interval 1 day as 'uint8 - interval 1 day',
`float4` as 'float4', `float4`  - interval 1 day as 'float4 - interval 1 day',
`float8` as 'float8', `float8`  - interval 1 day as 'float8 - interval 1 day',
`numeric` as 'numeric', `numeric`  - interval 1 day as 'numeric - interval 1 day',
`date` as 'date', `date`  - interval 1 day as 'date - interval 1 day',
`time` as 'time', `time`  - interval 1 day as 'time - interval 1 day',
`time(4)` as 'time(4)', `time(4)`  - interval 1 day as 'time(4) - interval 1 day',
`datetime` as 'datetime', `datetime`  - interval 1 day as 'datetime - interval 1 day',
`datetime(4)` as 'datetime(4)', `datetime(4)`  - interval 1 day as 'datetime(4) - interval 1 day',
`timestamp` as 'timestamp', `timestamp`  - interval 1 day as 'timestamp - interval 1 day',
`timestamp(4)` as 'timestamp(4)', `timestamp(4)`  - interval 1 day as 'timestamp(4) - interval 1 day',
`year` as 'year', `year`  - interval 1 day as 'year - interval 1 day',
`char` as 'char', `char`  - interval 1 day as 'char - interval 1 day',
`varchar` as 'varchar', `varchar`  - interval 1 day as 'varchar - interval 1 day',
`text` as 'text', `text`  - interval 1 day as 'text - interval 1 day' 
from test_type_table_zero;
\x
select * from res_tab;
\x
desc res_tab;

drop table if exists res_tab;
create table res_tab as select
`int1` as 'int1', `int1`  + interval 1 day as 'int1 + interval 1 day',
`uint1` as 'uint1', `uint1`  + interval 1 day as 'uint1 + interval 1 day',
`int2` as 'int2', `int2`  + interval 1 day as 'int2 + interval 1 day',
`uint2` as 'uint2', `uint2`  + interval 1 day as 'uint2 + interval 1 day',
`int4` as 'int4', `int4`  + interval 1 day as 'int4 + interval 1 day',
`uint4` as 'uint4', `uint4`  + interval 1 day as 'uint4 + interval 1 day',
`int8` as 'int8', `int8`  + interval 1 day as 'int8 + interval 1 day',
`uint8` as 'uint8', `uint8`  + interval 1 day as 'uint8 + interval 1 day',
`float4` as 'float4', `float4`  + interval 1 day as 'float4 + interval 1 day',
`float8` as 'float8', `float8`  + interval 1 day as 'float8 + interval 1 day',
`numeric` as 'numeric', `numeric`  + interval 1 day as 'numeric + interval 1 day',
`date` as 'date', `date`  + interval 1 day as 'date + interval 1 day',
`time` as 'time', `time`  + interval 1 day as 'time + interval 1 day',
`time(4)` as 'time(4)', `time(4)`  + interval 1 day as 'time(4) + interval 1 day',
`datetime` as 'datetime', `datetime`  + interval 1 day as 'datetime + interval 1 day',
`datetime(4)` as 'datetime(4)', `datetime(4)`  + interval 1 day as 'datetime(4) + interval 1 day',
`timestamp` as 'timestamp', `timestamp`  + interval 1 day as 'timestamp + interval 1 day',
`timestamp(4)` as 'timestamp(4)', `timestamp(4)`  + interval 1 day as 'timestamp(4) + interval 1 day',
`year` as 'year', `year`  + interval 1 day as 'year + interval 1 day',
`char` as 'char', `char`  + interval 1 day as 'char + interval 1 day',
`varchar` as 'varchar', `varchar`  + interval 1 day as 'varchar + interval 1 day',
`text` as 'text', `text`  + interval 1 day as 'text + interval 1 day' 
from test_type_table_invalid;
\x
select * from res_tab;
\x
desc res_tab;
drop table if exists res_tab;
create table res_tab as select
`int1` as 'int1', interval 1 day + `int1` as 'interval 1 day + int1',
`uint1` as 'uint1', interval 1 day + `uint1` as 'interval 1 day + uint1',
`int2` as 'int2', interval 1 day + `int2` as 'interval 1 day + int2',
`uint2` as 'uint2', interval 1 day + `uint2` as 'interval 1 day + uint2',
`int4` as 'int4', interval 1 day + `int4` as 'interval 1 day + int4',
`uint4` as 'uint4', interval 1 day + `uint4` as 'interval 1 day + uint4',
`int8` as 'int8', interval 1 day + `int8` as 'interval 1 day + int8',
`uint8` as 'uint8', interval 1 day + `uint8` as 'interval 1 day + uint8',
`float4` as 'float4', interval 1 day + `float4` as 'interval 1 day + float4',
`float8` as 'float8', interval 1 day + `float8` as 'interval 1 day + float8',
`numeric` as 'numeric', interval 1 day + `numeric` as 'interval 1 day + numeric',
`date` as 'date', interval 1 day + `date` as 'interval 1 day + date',
`time` as 'time', interval 1 day + `time` as 'interval 1 day + time',
`time(4)` as 'time(4)', interval 1 day + `time(4)` as 'interval 1 day + time(4)',
`datetime` as 'datetime', interval 1 day + `datetime` as 'interval 1 day + datetime',
`datetime(4)` as 'datetime(4)', interval 1 day + `datetime(4)` as 'interval 1 day + datetime(4)',
`timestamp` as 'timestamp', interval 1 day + `timestamp` as 'interval 1 day + timestamp',
`timestamp(4)` as 'timestamp(4)', interval 1 day + `timestamp(4)` as 'interval 1 day + timestamp(4)',
`year` as 'year', interval 1 day + `year` as 'interval 1 day + year',
`char` as 'char', interval 1 day + `char` as 'interval 1 day + char',
`varchar` as 'varchar', interval 1 day + `varchar` as 'interval 1 day + varchar',
`text` as 'text', interval 1 day + `text` as 'interval 1 day + text' 
from test_type_table_invalid;
\x
select * from res_tab;
\x
desc res_tab;
drop table if exists res_tab;
create table res_tab as select
`int1` as 'int1', `int1`  - interval 1 day as 'int1 - interval 1 day',
`uint1` as 'uint1', `uint1`  - interval 1 day as 'uint1 - interval 1 day',
`int2` as 'int2', `int2`  - interval 1 day as 'int2 - interval 1 day',
`uint2` as 'uint2', `uint2`  - interval 1 day as 'uint2 - interval 1 day',
`int4` as 'int4', `int4`  - interval 1 day as 'int4 - interval 1 day',
`uint4` as 'uint4', `uint4`  - interval 1 day as 'uint4 - interval 1 day',
`int8` as 'int8', `int8`  - interval 1 day as 'int8 - interval 1 day',
`uint8` as 'uint8', `uint8`  - interval 1 day as 'uint8 - interval 1 day',
`float4` as 'float4', `float4`  - interval 1 day as 'float4 - interval 1 day',
`float8` as 'float8', `float8`  - interval 1 day as 'float8 - interval 1 day',
`numeric` as 'numeric', `numeric`  - interval 1 day as 'numeric - interval 1 day',
`date` as 'date', `date`  - interval 1 day as 'date - interval 1 day',
`time` as 'time', `time`  - interval 1 day as 'time - interval 1 day',
`time(4)` as 'time(4)', `time(4)`  - interval 1 day as 'time(4) - interval 1 day',
`datetime` as 'datetime', `datetime`  - interval 1 day as 'datetime - interval 1 day',
`datetime(4)` as 'datetime(4)', `datetime(4)`  - interval 1 day as 'datetime(4) - interval 1 day',
`timestamp` as 'timestamp', `timestamp`  - interval 1 day as 'timestamp - interval 1 day',
`timestamp(4)` as 'timestamp(4)', `timestamp(4)`  - interval 1 day as 'timestamp(4) - interval 1 day',
`year` as 'year', `year`  - interval 1 day as 'year - interval 1 day',
`char` as 'char', `char`  - interval 1 day as 'char - interval 1 day',
`varchar` as 'varchar', `varchar`  - interval 1 day as 'varchar - interval 1 day',
`text` as 'text', `text`  - interval 1 day as 'text - interval 1 day' 
from test_type_table_invalid;
\x
select * from res_tab;
\x
desc res_tab;
reset dolphin.sql_mode;

-- test sql_mode NO_ZERO_DATE
set dolphin.sql_mode = 'no_zero_date';
drop table if exists res_tab;
create table res_tab as select
`int1` as 'int1', `int1`  + interval 1 day as 'int1 + interval 1 day',
`uint1` as 'uint1', `uint1`  + interval 1 day as 'uint1 + interval 1 day',
`int2` as 'int2', `int2`  + interval 1 day as 'int2 + interval 1 day',
`uint2` as 'uint2', `uint2`  + interval 1 day as 'uint2 + interval 1 day',
`int4` as 'int4', `int4`  + interval 1 day as 'int4 + interval 1 day',
`uint4` as 'uint4', `uint4`  + interval 1 day as 'uint4 + interval 1 day',
`int8` as 'int8', `int8`  + interval 1 day as 'int8 + interval 1 day',
`uint8` as 'uint8', `uint8`  + interval 1 day as 'uint8 + interval 1 day',
`float4` as 'float4', `float4`  + interval 1 day as 'float4 + interval 1 day',
`float8` as 'float8', `float8`  + interval 1 day as 'float8 + interval 1 day',
`numeric` as 'numeric', `numeric`  + interval 1 day as 'numeric + interval 1 day',
`date` as 'date', `date`  + interval 1 day as 'date + interval 1 day',
`time` as 'time', `time`  + interval 1 day as 'time + interval 1 day',
`time(4)` as 'time(4)', `time(4)`  + interval 1 day as 'time(4) + interval 1 day',
`datetime` as 'datetime', `datetime`  + interval 1 day as 'datetime + interval 1 day',
`datetime(4)` as 'datetime(4)', `datetime(4)`  + interval 1 day as 'datetime(4) + interval 1 day',
`timestamp` as 'timestamp', `timestamp`  + interval 1 day as 'timestamp + interval 1 day',
`timestamp(4)` as 'timestamp(4)', `timestamp(4)`  + interval 1 day as 'timestamp(4) + interval 1 day',
`year` as 'year', `year`  + interval 1 day as 'year + interval 1 day',
`char` as 'char', `char`  + interval 1 day as 'char + interval 1 day',
`varchar` as 'varchar', `varchar`  + interval 1 day as 'varchar + interval 1 day',
`text` as 'text', `text`  + interval 1 day as 'text + interval 1 day' 
from test_type_table_null;
\x
select * from res_tab;
\x
desc res_tab;
drop table if exists res_tab;
create table res_tab as select
`int1` as 'int1', interval 1 day + `int1` as 'interval 1 day + int1',
`uint1` as 'uint1', interval 1 day + `uint1` as 'interval 1 day + uint1',
`int2` as 'int2', interval 1 day + `int2` as 'interval 1 day + int2',
`uint2` as 'uint2', interval 1 day + `uint2` as 'interval 1 day + uint2',
`int4` as 'int4', interval 1 day + `int4` as 'interval 1 day + int4',
`uint4` as 'uint4', interval 1 day + `uint4` as 'interval 1 day + uint4',
`int8` as 'int8', interval 1 day + `int8` as 'interval 1 day + int8',
`uint8` as 'uint8', interval 1 day + `uint8` as 'interval 1 day + uint8',
`float4` as 'float4', interval 1 day + `float4` as 'interval 1 day + float4',
`float8` as 'float8', interval 1 day + `float8` as 'interval 1 day + float8',
`numeric` as 'numeric', interval 1 day + `numeric` as 'interval 1 day + numeric',
`date` as 'date', interval 1 day + `date` as 'interval 1 day + date',
`time` as 'time', interval 1 day + `time` as 'interval 1 day + time',
`time(4)` as 'time(4)', interval 1 day + `time(4)` as 'interval 1 day + time(4)',
`datetime` as 'datetime', interval 1 day + `datetime` as 'interval 1 day + datetime',
`datetime(4)` as 'datetime(4)', interval 1 day + `datetime(4)` as 'interval 1 day + datetime(4)',
`timestamp` as 'timestamp', interval 1 day + `timestamp` as 'interval 1 day + timestamp',
`timestamp(4)` as 'timestamp(4)', interval 1 day + `timestamp(4)` as 'interval 1 day + timestamp(4)',
`year` as 'year', interval 1 day + `year` as 'interval 1 day + year',
`char` as 'char', interval 1 day + `char` as 'interval 1 day + char',
`varchar` as 'varchar', interval 1 day + `varchar` as 'interval 1 day + varchar',
`text` as 'text', interval 1 day + `text` as 'interval 1 day + text' 
from test_type_table_null;
\x
select * from res_tab;
\x
desc res_tab;
drop table if exists res_tab;
create table res_tab as select
`int1` as 'int1', `int1`  - interval 1 day as 'int1 - interval 1 day',
`uint1` as 'uint1', `uint1`  - interval 1 day as 'uint1 - interval 1 day',
`int2` as 'int2', `int2`  - interval 1 day as 'int2 - interval 1 day',
`uint2` as 'uint2', `uint2`  - interval 1 day as 'uint2 - interval 1 day',
`int4` as 'int4', `int4`  - interval 1 day as 'int4 - interval 1 day',
`uint4` as 'uint4', `uint4`  - interval 1 day as 'uint4 - interval 1 day',
`int8` as 'int8', `int8`  - interval 1 day as 'int8 - interval 1 day',
`uint8` as 'uint8', `uint8`  - interval 1 day as 'uint8 - interval 1 day',
`float4` as 'float4', `float4`  - interval 1 day as 'float4 - interval 1 day',
`float8` as 'float8', `float8`  - interval 1 day as 'float8 - interval 1 day',
`numeric` as 'numeric', `numeric`  - interval 1 day as 'numeric - interval 1 day',
`date` as 'date', `date`  - interval 1 day as 'date - interval 1 day',
`time` as 'time', `time`  - interval 1 day as 'time - interval 1 day',
`time(4)` as 'time(4)', `time(4)`  - interval 1 day as 'time(4) - interval 1 day',
`datetime` as 'datetime', `datetime`  - interval 1 day as 'datetime - interval 1 day',
`datetime(4)` as 'datetime(4)', `datetime(4)`  - interval 1 day as 'datetime(4) - interval 1 day',
`timestamp` as 'timestamp', `timestamp`  - interval 1 day as 'timestamp - interval 1 day',
`timestamp(4)` as 'timestamp(4)', `timestamp(4)`  - interval 1 day as 'timestamp(4) - interval 1 day',
`year` as 'year', `year`  - interval 1 day as 'year - interval 1 day',
`char` as 'char', `char`  - interval 1 day as 'char - interval 1 day',
`varchar` as 'varchar', `varchar`  - interval 1 day as 'varchar - interval 1 day',
`text` as 'text', `text`  - interval 1 day as 'text - interval 1 day' 
from test_type_table_null;
\x
select * from res_tab;
\x
desc res_tab;

drop table if exists res_tab;
create table res_tab as select
`int1` as 'int1', `int1`  + interval 1 day as 'int1 + interval 1 day',
`uint1` as 'uint1', `uint1`  + interval 1 day as 'uint1 + interval 1 day',
`int2` as 'int2', `int2`  + interval 1 day as 'int2 + interval 1 day',
`uint2` as 'uint2', `uint2`  + interval 1 day as 'uint2 + interval 1 day',
`int4` as 'int4', `int4`  + interval 1 day as 'int4 + interval 1 day',
`uint4` as 'uint4', `uint4`  + interval 1 day as 'uint4 + interval 1 day',
`int8` as 'int8', `int8`  + interval 1 day as 'int8 + interval 1 day',
`uint8` as 'uint8', `uint8`  + interval 1 day as 'uint8 + interval 1 day',
`float4` as 'float4', `float4`  + interval 1 day as 'float4 + interval 1 day',
`float8` as 'float8', `float8`  + interval 1 day as 'float8 + interval 1 day',
`numeric` as 'numeric', `numeric`  + interval 1 day as 'numeric + interval 1 day',
`date` as 'date', `date`  + interval 1 day as 'date + interval 1 day',
`time` as 'time', `time`  + interval 1 day as 'time + interval 1 day',
`time(4)` as 'time(4)', `time(4)`  + interval 1 day as 'time(4) + interval 1 day',
`datetime` as 'datetime', `datetime`  + interval 1 day as 'datetime + interval 1 day',
`datetime(4)` as 'datetime(4)', `datetime(4)`  + interval 1 day as 'datetime(4) + interval 1 day',
`timestamp` as 'timestamp', `timestamp`  + interval 1 day as 'timestamp + interval 1 day',
`timestamp(4)` as 'timestamp(4)', `timestamp(4)`  + interval 1 day as 'timestamp(4) + interval 1 day',
`year` as 'year', `year`  + interval 1 day as 'year + interval 1 day',
`char` as 'char', `char`  + interval 1 day as 'char + interval 1 day',
`varchar` as 'varchar', `varchar`  + interval 1 day as 'varchar + interval 1 day',
`text` as 'text', `text`  + interval 1 day as 'text + interval 1 day' 
from test_type_table_zero;
\x
select * from res_tab;
\x
desc res_tab;
drop table if exists res_tab;
create table res_tab as select
`int1` as 'int1', interval 1 day + `int1` as 'interval 1 day + int1',
`uint1` as 'uint1', interval 1 day + `uint1` as 'interval 1 day + uint1',
`int2` as 'int2', interval 1 day + `int2` as 'interval 1 day + int2',
`uint2` as 'uint2', interval 1 day + `uint2` as 'interval 1 day + uint2',
`int4` as 'int4', interval 1 day + `int4` as 'interval 1 day + int4',
`uint4` as 'uint4', interval 1 day + `uint4` as 'interval 1 day + uint4',
`int8` as 'int8', interval 1 day + `int8` as 'interval 1 day + int8',
`uint8` as 'uint8', interval 1 day + `uint8` as 'interval 1 day + uint8',
`float4` as 'float4', interval 1 day + `float4` as 'interval 1 day + float4',
`float8` as 'float8', interval 1 day + `float8` as 'interval 1 day + float8',
`numeric` as 'numeric', interval 1 day + `numeric` as 'interval 1 day + numeric',
`date` as 'date', interval 1 day + `date` as 'interval 1 day + date',
`time` as 'time', interval 1 day + `time` as 'interval 1 day + time',
`time(4)` as 'time(4)', interval 1 day + `time(4)` as 'interval 1 day + time(4)',
`datetime` as 'datetime', interval 1 day + `datetime` as 'interval 1 day + datetime',
`datetime(4)` as 'datetime(4)', interval 1 day + `datetime(4)` as 'interval 1 day + datetime(4)',
`timestamp` as 'timestamp', interval 1 day + `timestamp` as 'interval 1 day + timestamp',
`timestamp(4)` as 'timestamp(4)', interval 1 day + `timestamp(4)` as 'interval 1 day + timestamp(4)',
`year` as 'year', interval 1 day + `year` as 'interval 1 day + year',
`char` as 'char', interval 1 day + `char` as 'interval 1 day + char',
`varchar` as 'varchar', interval 1 day + `varchar` as 'interval 1 day + varchar',
`text` as 'text', interval 1 day + `text` as 'interval 1 day + text' 
from test_type_table_zero;
\x
select * from res_tab;
\x
desc res_tab;
drop table if exists res_tab;
create table res_tab as select
`int1` as 'int1', `int1`  - interval 1 day as 'int1 - interval 1 day',
`uint1` as 'uint1', `uint1`  - interval 1 day as 'uint1 - interval 1 day',
`int2` as 'int2', `int2`  - interval 1 day as 'int2 - interval 1 day',
`uint2` as 'uint2', `uint2`  - interval 1 day as 'uint2 - interval 1 day',
`int4` as 'int4', `int4`  - interval 1 day as 'int4 - interval 1 day',
`uint4` as 'uint4', `uint4`  - interval 1 day as 'uint4 - interval 1 day',
`int8` as 'int8', `int8`  - interval 1 day as 'int8 - interval 1 day',
`uint8` as 'uint8', `uint8`  - interval 1 day as 'uint8 - interval 1 day',
`float4` as 'float4', `float4`  - interval 1 day as 'float4 - interval 1 day',
`float8` as 'float8', `float8`  - interval 1 day as 'float8 - interval 1 day',
`numeric` as 'numeric', `numeric`  - interval 1 day as 'numeric - interval 1 day',
`date` as 'date', `date`  - interval 1 day as 'date - interval 1 day',
`time` as 'time', `time`  - interval 1 day as 'time - interval 1 day',
`time(4)` as 'time(4)', `time(4)`  - interval 1 day as 'time(4) - interval 1 day',
`datetime` as 'datetime', `datetime`  - interval 1 day as 'datetime - interval 1 day',
`datetime(4)` as 'datetime(4)', `datetime(4)`  - interval 1 day as 'datetime(4) - interval 1 day',
`timestamp` as 'timestamp', `timestamp`  - interval 1 day as 'timestamp - interval 1 day',
`timestamp(4)` as 'timestamp(4)', `timestamp(4)`  - interval 1 day as 'timestamp(4) - interval 1 day',
`year` as 'year', `year`  - interval 1 day as 'year - interval 1 day',
`char` as 'char', `char`  - interval 1 day as 'char - interval 1 day',
`varchar` as 'varchar', `varchar`  - interval 1 day as 'varchar - interval 1 day',
`text` as 'text', `text`  - interval 1 day as 'text - interval 1 day' 
from test_type_table_zero;
\x
select * from res_tab;
\x
desc res_tab;

drop table if exists res_tab;
create table res_tab as select
`int1` as 'int1', `int1`  + interval 1 day as 'int1 + interval 1 day',
`uint1` as 'uint1', `uint1`  + interval 1 day as 'uint1 + interval 1 day',
`int2` as 'int2', `int2`  + interval 1 day as 'int2 + interval 1 day',
`uint2` as 'uint2', `uint2`  + interval 1 day as 'uint2 + interval 1 day',
`int4` as 'int4', `int4`  + interval 1 day as 'int4 + interval 1 day',
`uint4` as 'uint4', `uint4`  + interval 1 day as 'uint4 + interval 1 day',
`int8` as 'int8', `int8`  + interval 1 day as 'int8 + interval 1 day',
`uint8` as 'uint8', `uint8`  + interval 1 day as 'uint8 + interval 1 day',
`float4` as 'float4', `float4`  + interval 1 day as 'float4 + interval 1 day',
`float8` as 'float8', `float8`  + interval 1 day as 'float8 + interval 1 day',
`numeric` as 'numeric', `numeric`  + interval 1 day as 'numeric + interval 1 day',
`date` as 'date', `date`  + interval 1 day as 'date + interval 1 day',
`time` as 'time', `time`  + interval 1 day as 'time + interval 1 day',
`time(4)` as 'time(4)', `time(4)`  + interval 1 day as 'time(4) + interval 1 day',
`datetime` as 'datetime', `datetime`  + interval 1 day as 'datetime + interval 1 day',
`datetime(4)` as 'datetime(4)', `datetime(4)`  + interval 1 day as 'datetime(4) + interval 1 day',
`timestamp` as 'timestamp', `timestamp`  + interval 1 day as 'timestamp + interval 1 day',
`timestamp(4)` as 'timestamp(4)', `timestamp(4)`  + interval 1 day as 'timestamp(4) + interval 1 day',
`year` as 'year', `year`  + interval 1 day as 'year + interval 1 day',
`char` as 'char', `char`  + interval 1 day as 'char + interval 1 day',
`varchar` as 'varchar', `varchar`  + interval 1 day as 'varchar + interval 1 day',
`text` as 'text', `text`  + interval 1 day as 'text + interval 1 day' 
from test_type_table_invalid;
\x
select * from res_tab;
\x
desc res_tab;
drop table if exists res_tab;
create table res_tab as select
`int1` as 'int1', interval 1 day + `int1` as 'interval 1 day + int1',
`uint1` as 'uint1', interval 1 day + `uint1` as 'interval 1 day + uint1',
`int2` as 'int2', interval 1 day + `int2` as 'interval 1 day + int2',
`uint2` as 'uint2', interval 1 day + `uint2` as 'interval 1 day + uint2',
`int4` as 'int4', interval 1 day + `int4` as 'interval 1 day + int4',
`uint4` as 'uint4', interval 1 day + `uint4` as 'interval 1 day + uint4',
`int8` as 'int8', interval 1 day + `int8` as 'interval 1 day + int8',
`uint8` as 'uint8', interval 1 day + `uint8` as 'interval 1 day + uint8',
`float4` as 'float4', interval 1 day + `float4` as 'interval 1 day + float4',
`float8` as 'float8', interval 1 day + `float8` as 'interval 1 day + float8',
`numeric` as 'numeric', interval 1 day + `numeric` as 'interval 1 day + numeric',
`date` as 'date', interval 1 day + `date` as 'interval 1 day + date',
`time` as 'time', interval 1 day + `time` as 'interval 1 day + time',
`time(4)` as 'time(4)', interval 1 day + `time(4)` as 'interval 1 day + time(4)',
`datetime` as 'datetime', interval 1 day + `datetime` as 'interval 1 day + datetime',
`datetime(4)` as 'datetime(4)', interval 1 day + `datetime(4)` as 'interval 1 day + datetime(4)',
`timestamp` as 'timestamp', interval 1 day + `timestamp` as 'interval 1 day + timestamp',
`timestamp(4)` as 'timestamp(4)', interval 1 day + `timestamp(4)` as 'interval 1 day + timestamp(4)',
`year` as 'year', interval 1 day + `year` as 'interval 1 day + year',
`char` as 'char', interval 1 day + `char` as 'interval 1 day + char',
`varchar` as 'varchar', interval 1 day + `varchar` as 'interval 1 day + varchar',
`text` as 'text', interval 1 day + `text` as 'interval 1 day + text' 
from test_type_table_invalid;
\x
select * from res_tab;
\x
desc res_tab;
drop table if exists res_tab;
create table res_tab as select
`int1` as 'int1', `int1`  - interval 1 day as 'int1 - interval 1 day',
`uint1` as 'uint1', `uint1`  - interval 1 day as 'uint1 - interval 1 day',
`int2` as 'int2', `int2`  - interval 1 day as 'int2 - interval 1 day',
`uint2` as 'uint2', `uint2`  - interval 1 day as 'uint2 - interval 1 day',
`int4` as 'int4', `int4`  - interval 1 day as 'int4 - interval 1 day',
`uint4` as 'uint4', `uint4`  - interval 1 day as 'uint4 - interval 1 day',
`int8` as 'int8', `int8`  - interval 1 day as 'int8 - interval 1 day',
`uint8` as 'uint8', `uint8`  - interval 1 day as 'uint8 - interval 1 day',
`float4` as 'float4', `float4`  - interval 1 day as 'float4 - interval 1 day',
`float8` as 'float8', `float8`  - interval 1 day as 'float8 - interval 1 day',
`numeric` as 'numeric', `numeric`  - interval 1 day as 'numeric - interval 1 day',
`date` as 'date', `date`  - interval 1 day as 'date - interval 1 day',
`time` as 'time', `time`  - interval 1 day as 'time - interval 1 day',
`time(4)` as 'time(4)', `time(4)`  - interval 1 day as 'time(4) - interval 1 day',
`datetime` as 'datetime', `datetime`  - interval 1 day as 'datetime - interval 1 day',
`datetime(4)` as 'datetime(4)', `datetime(4)`  - interval 1 day as 'datetime(4) - interval 1 day',
`timestamp` as 'timestamp', `timestamp`  - interval 1 day as 'timestamp - interval 1 day',
`timestamp(4)` as 'timestamp(4)', `timestamp(4)`  - interval 1 day as 'timestamp(4) - interval 1 day',
`year` as 'year', `year`  - interval 1 day as 'year - interval 1 day',
`char` as 'char', `char`  - interval 1 day as 'char - interval 1 day',
`varchar` as 'varchar', `varchar`  - interval 1 day as 'varchar - interval 1 day',
`text` as 'text', `text`  - interval 1 day as 'text - interval 1 day' 
from test_type_table_invalid;
\x
select * from res_tab;
\x
desc res_tab;
reset dolphin.sql_mode;

drop table if exists res_tab;
drop view if exists res_view;
drop table if exists test_type_table_null cascade;
drop table if exists test_type_table_zero cascade;
drop table if exists test_type_table_invalid cascade;
drop table if exists test_type_table cascade;
-- end test for 'bit_expr ± interval expr unit'

reset dolphin.b_compatibility_mode;
drop schema test_time_expr cascade;
reset current_schema;
alter system set upgrade_mode to 0;


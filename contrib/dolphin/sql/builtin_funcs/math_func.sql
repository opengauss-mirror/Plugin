create schema test_math_func;
set current_schema to 'test_math_func';

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
   `enum_t` enum('a', 'b', 'c'),
   `set_t` set('a', 'b', 'c'),
   `json` json   
);

insert into test_type_table values (1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, b'1', b'111', true, '2023-02-05', '19:10:50', '19:10:50.3456', '2023-02-05 19:10:50', '2023-02-05 19:10:50.456', '2023-02-05 19:10:50', '2023-02-05 19:10:50.456',
'2023', '1.23a', '1.23a', '1.23a', '1.23a', '1.23a', '1.23a', '1.23a', '1.23a', '1.23a', 'a', 'a,c', json_object('a', 1, 'b', 2));


-- degrees math function
select degrees(`int1`), degrees(`uint1`), degrees(`int2`), degrees(`uint2`), degrees(`int4`), degrees(`uint4`), degrees(`int8`), degrees(`uint8`), degrees(`float4`), degrees(`float8`), degrees(`numeric`),degrees(`bit1`), degrees(`bit64`), degrees(`boolean`), degrees(`date`), degrees(`time`), degrees(`time(4)`), degrees(`datetime`),degrees(`datetime(4)`), degrees(`timestamp`), degrees(`timestamp(4)`), degrees(`year`), degrees(`char`), degrees(`varchar`), degrees(`binary`), degrees(`varbinary`), degrees(`tinyblob`), degrees(`blob`), degrees(`mediumblob`), degrees(`longblob`), degrees(`text`), degrees(`enum_t`), degrees(`set_t`), degrees(`json`) from test_type_table;   

create table test_double_degrees(d1 double,  d2 double,  d3 double,  d4 double,  d5 double,  d6 double,  d7 double,  d8 double,  d9 double,  d10 double,  d11 double,  d12 double,  d13 double,  d14 double,  d15 double,  d16 double,  d17 double,  d18 double,  d19 double,  d20 double,  d21 double,  d22 double,  d23 double,  d24 double,  d25 double,  d26 double,  d27 double,  d28 double,  d29 double,  d30 double,  d31 double,  d32 double,  d33 double,  d34 double);

insert ignore into test_double_degrees  select degrees(`int1`), degrees(`uint1`), degrees(`int2`), degrees(`uint2`), degrees(`int4`), degrees(`uint4`), degrees(`int8`), degrees(`uint8`), degrees(`float4`), degrees(`float8`), degrees(`numeric`),degrees(`bit1`), degrees(`bit64`),degrees(`boolean`), degrees(`date`), degrees(`time`), degrees(`time(4)`), degrees(`datetime`),degrees(`datetime(4)`), degrees(`timestamp`), degrees(`timestamp(4)`), degrees(`year`), degrees(`char`), degrees(`varchar`), degrees(`binary`),degrees(`varbinary`), degrees(`tinyblob`), degrees(`blob`), degrees(`mediumblob`), degrees(`longblob`), degrees(`text`), degrees(`enum_t`), degrees(`set_t`), degrees(`json`) from test_type_table;

set dolphin.sql_mode = 'sql_mode_full_group,pipes_as_concat,ansi_quotes';

insert into test_double_degrees  select degrees(`int1`), degrees(`uint1`), degrees(`int2`), degrees(`uint2`), degrees(`int4`), degrees(`uint4`), degrees(`int8`), degrees(`uint8`), degrees(`float4`), degrees(`float8`), degrees(`numeric`),degrees(`bit1`), degrees(`bit64`),degrees(`boolean`), degrees(`date`), degrees(`time`), degrees(`time(4)`), degrees(`datetime`),degrees(`datetime(4)`), degrees(`timestamp`), degrees(`timestamp(4)`), degrees(`year`), degrees(`char`), degrees(`varchar`), degrees(`binary`),degrees(`varbinary`), degrees(`tinyblob`), degrees(`blob`), degrees(`mediumblob`), degrees(`longblob`), degrees(`text`), degrees(`enum_t`), degrees(`set_t`), degrees(`json`) from test_type_table;

select * from test_double_degrees order by 1;


-- exp math function
reset dolphin.sql_mode;
select exp(`int1`), exp(`uint1`), exp(`int2`), exp(`uint2`), exp(`int4`), exp(`uint4`), exp(`int8`), exp(`uint8`), exp(`float4`), exp(`float8`), exp(`numeric`),exp(`bit1`), exp(`bit64`), exp(`boolean`), exp(`date`), exp(`time`), exp(`time(4)`), exp(`datetime`),exp(`datetime(4)`), exp(`timestamp`), exp(`timestamp(4)`), exp(`year`), exp(`char`), exp(`varchar`), exp(`binary`), exp(`varbinary`), exp(`tinyblob`), exp(`blob`), exp(`mediumblob`), exp(`longblob`), exp(`text`), exp(`enum_t`), exp(`set_t`), exp(`json`) from test_type_table;


select exp(`int1`), exp(`uint1`), exp(`int2`), exp(`uint2`), exp(`int4`), exp(`uint4`), exp(`int8`), exp(`uint8`), exp(`float4`), exp(`float8`), exp(`numeric`), exp(`bit1`), exp(`bit64`), exp(`boolean`), exp(`char`), exp(`varchar`),exp(`binary`), exp(`varbinary`), exp(`tinyblob`), exp(`blob`), exp(`mediumblob`), exp(`longblob`), exp(`text`), exp(`enum_t`), exp(`set_t`), exp(`json`) from test_type_table;

create table test_double_exp(d1 double,  d2 double,  d3 double,  d4 double,  d5 double,  d6 double,  d7 double,  d8 double,  d9 double,  d10 double,  d11 double,  d12 double,  d13 double,  d14 double,  d15 double,  d16 double,  d17 double,  d18 double,  d19 double,  d20 double,  d21 double,  d22 double,  d23 double,  d24 double,  d25 double,  d26 double);

insert ignore into test_double_exp select exp(`int1`), exp(`uint1`), exp(`int2`), exp(`uint2`), exp(`int4`), exp(`uint4`), exp(`int8`), exp(`uint8`), exp(`float4`), exp(`float8`), exp(`numeric`), exp(`bit1`), exp(`bit64`), exp(`boolean`), exp(`char`), exp(`varchar`),exp(`binary`), exp(`varbinary`), exp(`tinyblob`), exp(`blob`), exp(`mediumblob`), exp(`longblob`), exp(`text`), exp(`enum_t`), exp(`set_t`), exp(`json`) from test_type_table;

set dolphin.sql_mode = 'sql_mode_full_group,pipes_as_concat,ansi_quotes';

insert into test_double_exp select exp(`int1`), exp(`uint1`), exp(`int2`), exp(`uint2`), exp(`int4`), exp(`uint4`), exp(`int8`), exp(`uint8`), exp(`float4`), exp(`float8`), exp(`numeric`), exp(`bit1`), exp(`bit64`), exp(`boolean`), exp(`char`), exp(`varchar`),exp(`binary`), exp(`varbinary`), exp(`tinyblob`), exp(`blob`), exp(`mediumblob`), exp(`longblob`), exp(`text`), exp(`enum_t`), exp(`set_t`), exp(`json`) from test_type_table;

select * from test_double_exp order by 1;

-- acos math function
reset dolphin.sql_mode;
select acos(`int1`), acos(`uint1`), acos(`int2`), acos(`uint2`), acos(`int4`), acos(`uint4`), acos(`int8`), acos(`uint8`), acos(`float4`), acos(`float8`), acos(`numeric`),acos(`bit1`), acos(`bit64`), acos(`boolean`), acos(`date`), acos(`time`), acos(`time(4)`), acos(`datetime`),acos(`datetime(4)`), acos(`timestamp`), acos(`timestamp(4)`), acos(`year`), acos(`char`), acos(`varchar`), acos(`binary`), acos(`varbinary`), acos(`tinyblob`), acos(`blob`), acos(`mediumblob`), acos(`longblob`), acos(`text`), acos(`enum_t`), acos(`set_t`), acos(`json`) from test_type_table;   

create table test_double_acos(d1 double,  d2 double,  d3 double,  d4 double,  d5 double,  d6 double,  d7 double,  d8 double,  d9 double,  d10 double,  d11 double,  d12 double,  d13 double,  d14 double,  d15 double,  d16 double,  d17 double,  d18 double,  d19 double,  d20 double,  d21 double,  d22 double,  d23 double,  d24 double,  d25 double,  d26 double,  d27 double,  d28 double,  d29 double,  d30 double,  d31 double,  d32 double,  d33 double,  d34 double);

insert ignore into test_double_acos  select acos(`int1`), acos(`uint1`), acos(`int2`), acos(`uint2`), acos(`int4`), acos(`uint4`), acos(`int8`), acos(`uint8`), acos(`float4`), acos(`float8`), acos(`numeric`),acos(`bit1`), acos(`bit64`),acos(`boolean`), acos(`date`), acos(`time`), acos(`time(4)`), acos(`datetime`),acos(`datetime(4)`), acos(`timestamp`), acos(`timestamp(4)`), acos(`year`), acos(`char`), acos(`varchar`), acos(`binary`),acos(`varbinary`), acos(`tinyblob`), acos(`blob`), acos(`mediumblob`), acos(`longblob`), acos(`text`), acos(`enum_t`), acos(`set_t`), acos(`json`) from test_type_table;

set dolphin.sql_mode = 'sql_mode_full_group,pipes_as_concat,ansi_quotes';

insert into test_double_acos  select acos(`int1`), acos(`uint1`), acos(`int2`), acos(`uint2`), acos(`int4`), acos(`uint4`), acos(`int8`), acos(`uint8`), acos(`float4`), acos(`float8`), acos(`numeric`),acos(`bit1`), acos(`bit64`),acos(`boolean`), acos(`date`), acos(`time`), acos(`time(4)`), acos(`datetime`),acos(`datetime(4)`), acos(`timestamp`), acos(`timestamp(4)`), acos(`year`), acos(`char`), acos(`varchar`), acos(`binary`),acos(`varbinary`), acos(`tinyblob`), acos(`blob`), acos(`mediumblob`), acos(`longblob`), acos(`text`), acos(`enum_t`), acos(`set_t`), acos(`json`) from test_type_table;

select * from test_double_acos order by 1;

-- log math function
reset dolphin.sql_mode;
set dolphin.sql_mode = 'sql_mode_strict,error_for_division_by_zero';
create table test_log(value double);

insert into test_log select log(1,10);
insert ignore into test_log select log(1,10);
select * from test_log;
select log(1,10);

drop table if exists test_log;

reset dolphin.sql_mode;
set dolphin.sql_mode = 'error_for_division_by_zero';
create table test_log(value double);

insert into test_log select log(1,10);
insert ignore into test_log select log(1,10);
select * from test_log;
select log(1,10);

drop table if exists test_log;

reset dolphin.sql_mode;
set dolphin.sql_mode = 'sql_mode_strict';
create table test_log(value double);

insert into test_log select log(1,10);
insert ignore into test_log select log(1,10);
select * from test_log;
select log(1,10);

drop table if exists test_log;
reset dolphin.sql_mode;
set dolphin.sql_mode = '';
create table test_log(value double);

insert into test_log select log(1,10);
insert ignore into test_log select log(1,10);
select * from test_log;
select log(1,10);

drop table if exists test_log;

select
log(`int1`,`int1`),
log(`uint1`,`uint1`),
log(`int2`,`int2`),
log(`uint2`,`uint2`),
log(`int4`,`int4`),
log(`uint4`,`uint4`),
log(`int8`,`int8`),
log(`uint8`,`uint8`),
log(`float4`,`float4`),
log(`float8`,`float8`),
log(`numeric`,`numeric`),
log(`bit1`,`bit1`),
log(`bit64`,`bit64`),
log(`boolean`,`boolean`),
log(`date`,`date`),
log(`time`,`time`),
log(`time(4)`,`time(4)`),
log(`datetime`,`datetime`),
log(`datetime(4)`,`datetime(4)`),
log(`timestamp`,`timestamp`),
log(`timestamp(4)`,`timestamp(4)`),
log(`year`,`year`),
log(`char`,`char`),
log(`varchar`,`varchar`),
log(`binary`, `binary`),
log(`varbinary`, `varbinary`),
log(`tinyblob`, `tinyblob`),
log(`blob`, `blob`),
log(`mediumblob`, `mediumblob`),
log(`longblob`, `longblob`),
log(`text`, `text`),
log(`enum_t`, `enum_t`),
log(`set_t`, `set_t`),
log(`json`, `json`)
from test_type_table;

set dolphin.sql_mode = 'sql_mode_full_group,pipes_as_concat,ansi_quotes';

create table test_log as select  log(`int1`, `int1`) as log1, 
log(`uint1`, `uint1`) as log2, log(`int2`, `int2`) as log3,  log(`uint2`, `uint2`) as log4,  log(`int4`, `int4`) as log5, log(`uint4`, `uint4`) as log6,  log(`int8`, `int8`) as log7,  log(`uint8`, `uint8`) as log8, log(`float4`, `float4`) as log9, log(`float8`, `float8`) as log10,  log(`numeric`, `numeric`) as log11, log(`bit1`, `bit1`) as log12,  log(`bit64`, `bit64`) as log13,  log(`boolean`, `boolean`) as log14, log(`date`, `date`) as log15, log(`time`, `time`) as log16, log(`time(4)`, `time(4)`) as log17, log(`datetime`, `datetime`) as log18, log(`datetime(4)`, `datetime(4)`) as log19, log(`timestamp`, `timestamp`) as log20, log(`timestamp(4)`, `timestamp(4)`) as log21, log(`year`, `year`) as log22, log(`char`, `char`) as log23,  log(`varchar`, `varchar`) as log24,  log(`binary`, `binary`) as log25, log(`varbinary`, `varbinary`) as log26, log(`tinyblob`, `tinyblob`) as log27, log(`blob`, `blob`) as log28, log(`mediumblob`, `mediumblob`) as log29, log(`longblob`, `longblob`) as log30, log(`text`, `text`) as log31, log(`enum_t`, `enum_t`) as log32, log(`set_t`, `set_t`) as log33, log(`json`, `json`) as log34 from test_type_table; 

reset dolphin.sql_mode;

insert into test_log select  log(`int1`, `int1`) as log1, 
log(`uint1`, `uint1`) as log2, log(`int2`, `int2`) as log3,  log(`uint2`, `uint2`) as log4,  log(`int4`, `int4`) as log5, log(`uint4`, `uint4`) as log6,  log(`int8`, `int8`) as log7,  log(`uint8`, `uint8`) as log8, log(`float4`, `float4`) as log9, log(`float8`, `float8`) as log10,  log(`numeric`, `numeric`) as log11, log(`bit1`, `bit1`) as log12,  log(`bit64`, `bit64`) as log13,  log(`boolean`, `boolean`) as log14, log(`date`, `date`) as log15, log(`time`, `time`) as log16, log(`time(4)`, `time(4)`) as log17, log(`datetime`, `datetime`) as log18, log(`datetime(4)`, `datetime(4)`) as log19, log(`timestamp`, `timestamp`) as log20, log(`timestamp(4)`, `timestamp(4)`) as log21, log(`year`, `year`) as log22, log(`char`, `char`) as log23,  log(`varchar`, `varchar`) as log24,  log(`binary`, `binary`) as log25, log(`varbinary`, `varbinary`) as log26, log(`tinyblob`, `tinyblob`) as log27, log(`blob`, `blob`) as log28, log(`mediumblob`, `mediumblob`) as log29, log(`longblob`, `longblob`) as log30, log(`text`, `text`) as log31, log(`enum_t`, `enum_t`) as log32, log(`set_t`, `set_t`) as log33, log(`json`, `json`) as log34 from test_type_table; 

insert ignore into test_log select  log(`int1`, `int1`) as log1, 
log(`uint1`, `uint1`) as log2, log(`int2`, `int2`) as log3,  log(`uint2`, `uint2`) as log4,  log(`int4`, `int4`) as log5, log(`uint4`, `uint4`) as log6,  log(`int8`, `int8`) as log7,  log(`uint8`, `uint8`) as log8, log(`float4`, `float4`) as log9, log(`float8`, `float8`) as log10,  log(`numeric`, `numeric`) as log11, log(`bit1`, `bit1`) as log12,  log(`bit64`, `bit64`) as log13,  log(`boolean`, `boolean`) as log14, log(`date`, `date`) as log15, log(`time`, `time`) as log16, log(`time(4)`, `time(4)`) as log17, log(`datetime`, `datetime`) as log18, log(`datetime(4)`, `datetime(4)`) as log19, log(`timestamp`, `timestamp`) as log20, log(`timestamp(4)`, `timestamp(4)`) as log21, log(`year`, `year`) as log22, log(`char`, `char`) as log23,  log(`varchar`, `varchar`) as log24,  log(`binary`, `binary`) as log25, log(`varbinary`, `varbinary`) as log26, log(`tinyblob`, `tinyblob`) as log27, log(`blob`, `blob`) as log28, log(`mediumblob`, `mediumblob`) as log29, log(`longblob`, `longblob`) as log30, log(`text`, `text`) as log31, log(`enum_t`, `enum_t`) as log32, log(`set_t`, `set_t`) as log33, log(`json`, `json`) as log34 from test_type_table; 

select * from test_log order by 1,10;

drop table if exists test_log;
-- ln math function
select
ln(`int1`),
ln(`uint1`),
ln(`int2`),
ln(`uint2`),
ln(`int4`),
ln(`uint4`),
ln(`int8`),
ln(`uint8`),
ln(`float4`),
ln(`float8`),
ln(`numeric`),
ln(`bit1`),
ln(`bit64`),
ln(`boolean`),
ln(`date`),
ln(`time`),
ln(`time(4)`),
ln(`datetime`),
ln(`datetime(4)`),
ln(`timestamp`),
ln(`timestamp(4)`),
ln(`year`),
ln(`char`),
ln(`varchar`),
ln(`binary`),
ln(`varbinary`),
ln(`tinyblob`),
ln(`blob`),
ln(`mediumblob`),
ln(`longblob`),
ln(`text`),
ln(`enum_t`),
ln(`set_t`),
ln(`json`)
from test_type_table;

-- floor math function
select
floor(`int1`),
floor(`uint1`),
floor(`int2`),
floor(`uint2`),
floor(`int4`),
floor(`uint4`),
floor(`int8`),
floor(`uint8`),
floor(`float4`),
floor(`float8`),
floor(`numeric`),
floor(`bit1`),
floor(`bit64`),
floor(`boolean`),
floor(`date`),
floor(`time`),
floor(`time(4)`),
floor(`datetime`),
floor(`datetime(4)`),
floor(`timestamp`),
floor(`timestamp(4)`),
floor(`year`),
floor(`char`),
floor(`varchar`),
floor(`binary`),
floor(`varbinary`),
floor(`tinyblob`),
floor(`blob`),
floor(`mediumblob`),
floor(`longblob`),
floor(`text`),
floor(`enum_t`),
floor(`set_t`),
floor(`json`)
from test_type_table;

set dolphin.sql_mode = 'sql_mode_full_group,pipes_as_concat,ansi_quotes';

create table test_floor_cts select  floor(`int1`) as floor_1, floor(`uint1`) as floor_2,
   floor(`int2`) as floor_3, floor(`uint2`) as floor_4, floor(`int4`) as floor_5, floor(`uint4`) as floor_6,
   floor(`int8`) as floor_7, floor(`uint8`) as floor_8, floor(`float4`) as floor_9, floor(`float8`) as floor_10,
   floor(`numeric`) as floor_11, floor(`bit1`) as floor_12, floor(`bit64`) as floor_13, floor(`boolean`) as floor_14,
   floor(`date`) as floor_15, floor(`time`) as floor_16, floor(`time(4)`) as floor_17, floor(`datetime`) as floor_18,
   floor(`datetime(4)`) as floor_19, floor(`timestamp`) as floor_20, floor(`timestamp(4)`) as floor_21,
   floor(`char`) as floor_22, floor(`varchar`) as floor_23,  floor(`binary`) as floor_24,
   floor(`varbinary`) as floor_25, floor(`tinyblob`) as floor_26, floor(`blob`) as floor_27, floor(`mediumblob`) as floor_28, floor(`longblob`) as floor_29,
   floor(`text`) as floor_30, floor(`enum_t`) as floor_31, floor(`set_t`) as floor_32 from test_type_table;   

create table test_ln_cts as select 
   ln(`int1`) as ln_1, ln(`uint1`) as ln_2, ln(`int2`) as ln_3, ln(`uint2`) as ln_4,
   ln(`int4`) as ln_5, ln(`uint4`) as ln_6, ln(`int8`) as ln_7, ln(`uint8`) as ln_8,
   ln(`float4`) as ln_9, ln(`float8`) as ln_10, ln(`numeric`) as ln_11, ln(`bit1`) as ln_12,
   ln(`bit64`) as ln_13, ln(`boolean`) as ln_14, ln(`date`) as ln_15, ln(`time`) as ln_16,
   ln(`time(4)`) as ln_17, ln(`datetime`) as ln_18, ln(`datetime(4)`) as ln_19, ln(`timestamp`) as ln_20,
   ln(`timestamp(4)`) as ln_21, ln(`char`) as ln_23, ln(`varchar`) as ln_24,  ln(`binary`) as ln_25,
   ln(`varbinary`) as ln_26, ln(`tinyblob`) as ln_27, ln(`blob`) as ln_28, ln(`mediumblob`) as ln_29, ln(`longblob`) as ln_30,
   ln(`text`) as ln_31, ln(`enum_t`) as ln_32, ln(`set_t`) as ln_33 from test_type_table;   

select * from test_floor_cts;
select * from test_ln_cts;

select exp(709);
select exp(710);
select exp(-1000); 

drop table if exists test_double_acos;
drop table if exists test_double_exp;
drop table if exists test_double_degrees;
drop table if exists test_type_table;
drop table if exists test_floor_cts;
drop table if exists test_ln_cts;

drop schema test_math_func cascade;
reset current_schema;
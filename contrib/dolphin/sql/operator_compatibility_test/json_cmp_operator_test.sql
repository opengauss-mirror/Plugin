---------- note ----------
-- binary cmp opers --
---- 1. = <=>
---- 2. != <>
---- 3. < <= > >=
---------- head ----------drop schema if exists json_operator_test_schema cascade;
create schema json_cmp_operator_test_schema;
set current_schema to 'json_cmp_operator_test_schema';
---------- head ----------
set dolphin.b_compatibility_mode to on;
DROP TABLE IF EXISTS test_json_table;
CREATE TABLE test_json_table
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
    `date` date,
    `time` time,
    `time(4)` time(4),
    `datetime` datetime,
    `datetime(4)` datetime(4),
    `timestamp` timestamp,
    `timestamp(4)` timestamp(4),
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
insert into test_json_table values(1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                                   b'1', b'111',
                                   '2023-02-05', '19:10:50', '19:10:50.3456', '2023-02-05 19:10:50', '2023-02-05 19:10:50.456', '2023-02-05 19:10:50', '2023-02-05 19:10:50.456', '2023',
                                   '1.23a', '1.23a', '1.23a', '1.23a', '1.23a', '1.23a', '1.23a', '1.23a', '1.23a',
                                   'a', 'a,c',
                                   json_object('a', 1, 'b', 2));
select `int1`, `json`,
       `int1` > `json` as `int1>json`, `int1` >= `json` as `int1>=json`,
       `int1` < `json` as `int1<json`, `int1` <= `json` as `int1<=json`,
       `int1` != `json` as `int1!=json`, `int1` <> `json` as `int1<>json`,
       `int1` = `json` as `int1=json`, `int1` <=> `json` as `int1<=>json` from test_json_table;
select `int1`, `json`,
       `json` > `int1` as `json>int1`, `json` >= `int1` as `json>=int1`,
       `json` < `int1` as `json<int1`, `json` <= `int1` as `json<=int1`,
       `json` != `int1` as `json!=int1`, `json` <> `int1` as `json<>int1`,
       `json` = `int1` as `json=int1`, `json` <=> `int1` as `json<=>int1` from test_json_table;
select `uint1`, `json`,
       `uint1` > `json` as `uint1>json`, `uint1` >= `json` as `uint1>=json`,
       `uint1` < `json` as `uint1<json`, `uint1` <= `json` as `uint1<=json`,
       `uint1` != `json` as `uint1!=json`, `uint1` <> `json` as `uint1<>json`,
       `uint1` = `json` as `uint1=json`, `uint1` <=> `json` as `uint1<=>json` from test_json_table;
select `uint1`, `json`,
       `json` > `uint1` as `json>uint1`, `json` >= `uint1` as `json>=uint1`,
       `json` < `uint1` as `json<uint1`, `json` <= `uint1` as `json<=uint1`,
       `json` != `uint1` as `json!=uint1`, `json` <> `uint1` as `json<>uint1`,
       `json` = `uint1` as `json=uint1`, `json` <=> `uint1` as `json<=>uint1` from test_json_table;
select `int2`, `json`,
       `int2` > `json` as `int2>json`, `int2` >= `json` as `int2>=json`,
       `int2` < `json` as `int2<json`, `int2` <= `json` as `int2<=json`,
       `int2` != `json` as `int2!=json`, `int2` <> `json` as `int2<>json`,
       `int2` = `json` as `int2=json`, `int2` <=> `json` as `int2<=>json` from test_json_table;
select `int2`, `json`,
       `json` > `int2` as `json>int2`, `json` >= `int2` as `json>=int2`,
       `json` < `int2` as `json<int2`, `json` <= `int2` as `json<=int2`,
       `json` != `int2` as `json!=int2`, `json` <> `int2` as `json<>int2`,
       `json` = `int2` as `json=int2`, `json` <=> `int2` as `json<=>int2` from test_json_table;
select `uint2`, `json`,
       `uint2` > `json` as `uint2>json`, `uint2` >= `json` as `uint2>=json`,
       `uint2` < `json` as `uint2<json`, `uint2` <= `json` as `uint2<=json`,
       `uint2` != `json` as `uint2!=json`, `uint2` <> `json` as `uint2<>json`,
       `uint2` = `json` as `uint2=json`, `uint2` <=> `json` as `uint2<=>json` from test_json_table;
select `uint2`, `json`,
       `json` > `uint2` as `json>uint2`, `json` >= `uint2` as `json>=uint2`,
       `json` < `uint2` as `json<uint2`, `json` <= `uint2` as `json<=uint2`,
       `json` != `uint2` as `json!=uint2`, `json` <> `uint2` as `json<>uint2`,
       `json` = `uint2` as `json=uint2`, `json` <=> `uint2` as `json<=>uint2` from test_json_table;
select `int4`, `json`,
       `int4` > `json` as `int4>json`, `int4` >= `json` as `int4>=json`,
       `int4` < `json` as `int4<json`, `int4` <= `json` as `int4<=json`,
       `int4` != `json` as `int4!=json`, `int4` <> `json` as `int4<>json`,
       `int4` = `json` as `int4=json`, `int4` <=> `json` as `int4<=>json` from test_json_table;
select `int4`, `json`,
       `json` > `int4` as `json>int4`, `json` >= `int4` as `json>=int4`,
       `json` < `int4` as `json<int4`, `json` <= `int4` as `json<=int4`,
       `json` != `int4` as `json!=int4`, `json` <> `int4` as `json<>int4`,
       `json` = `int4` as `json=int4`, `json` <=> `int4` as `json<=>int4` from test_json_table;
select `uint4`, `json`,
       `uint4` > `json` as `uint4>json`, `uint4` >= `json` as `uint4>=json`,
       `uint4` < `json` as `uint4<json`, `uint4` <= `json` as `uint4<=json`,
       `uint4` != `json` as `uint4!=json`, `uint4` <> `json` as `uint4<>json`,
       `uint4` = `json` as `uint4=json`, `uint4` <=> `json` as `uint4<=>json` from test_json_table;
select `uint4`, `json`,
       `json` > `uint4` as `json>uint4`, `json` >= `uint4` as `json>=uint4`,
       `json` < `uint4` as `json<uint4`, `json` <= `uint4` as `json<=uint4`,
       `json` != `uint4` as `json!=uint4`, `json` <> `uint4` as `json<>uint4`,
       `json` = `uint4` as `json=uint4`, `json` <=> `uint4` as `json<=>uint4` from test_json_table;
select `int8`, `json`,
       `int8` > `json` as `int8>json`, `int8` >= `json` as `int8>=json`,
       `int8` < `json` as `int8<json`, `int8` <= `json` as `int8<=json`,
       `int8` != `json` as `int8!=json`, `int8` <> `json` as `int8<>json`,
       `int8` = `json` as `int8=json`, `int8` <=> `json` as `int8<=>json` from test_json_table;
select `int8`, `json`,
       `json` > `int8` as `json>int8`, `json` >= `int8` as `json>=int8`,
       `json` < `int8` as `json<int8`, `json` <= `int8` as `json<=int8`,
       `json` != `int8` as `json!=int8`, `json` <> `int8` as `json<>int8`,
       `json` = `int8` as `json=int8`, `json` <=> `int8` as `json<=>int8` from test_json_table;
select `uint8`, `json`,
       `uint8` > `json` as `uint8>json`, `uint8` >= `json` as `uint8>=json`,
       `uint8` < `json` as `uint8<json`, `uint8` <= `json` as `uint8<=json`,
       `uint8` != `json` as `uint8!=json`, `uint8` <> `json` as `uint8<>json`,
       `uint8` = `json` as `uint8=json`, `uint8` <=> `json` as `uint8<=>json` from test_json_table;
select `uint8`, `json`,
       `json` > `uint8` as `json>uint8`, `json` >= `uint8` as `json>=uint8`,
       `json` < `uint8` as `json<uint8`, `json` <= `uint8` as `json<=uint8`,
       `json` != `uint8` as `json!=uint8`, `json` <> `uint8` as `json<>uint8`,
       `json` = `uint8` as `json=uint8`, `json` <=> `uint8` as `json<=>uint8` from test_json_table;
select `float4`, `json`,
       `float4` > `json` as `float4>json`, `float4` >= `json` as `float4>=json`,
       `float4` < `json` as `float4<json`, `float4` <= `json` as `float4<=json`,
       `float4` != `json` as `float4!=json`, `float4` <> `json` as `float4<>json`,
       `float4` = `json` as `float4=json`, `float4` <=> `json` as `float4<=>json` from test_json_table;
select `float4`, `json`,
       `json` > `float4` as `json>float4`, `json` >= `float4` as `json>=float4`,
       `json` < `float4` as `json<float4`, `json` <= `float4` as `json<=float4`,
       `json` != `float4` as `json!=float4`, `json` <> `float4` as `json<>float4`,
       `json` = `float4` as `json=float4`, `json` <=> `float4` as `json<=>float4` from test_json_table;
select `float8`, `json`,
       `float8` > `json` as `float8>json`, `float8` >= `json` as `float8>=json`,
       `float8` < `json` as `float8<json`, `float8` <= `json` as `float8<=json`,
       `float8` != `json` as `float8!=json`, `float8` <> `json` as `float8<>json`,
       `float8` = `json` as `float8=json`, `float8` <=> `json` as `float8<=>json` from test_json_table;
select `float8`, `json`,
       `json` > `float8` as `json>float8`, `json` >= `float8` as `json>=float8`,
       `json` < `float8` as `json<float8`, `json` <= `float8` as `json<=float8`,
       `json` != `float8` as `json!=float8`, `json` <> `float8` as `json<>float8`,
       `json` = `float8` as `json=float8`, `json` <=> `float8` as `json<=>float8` from test_json_table;
select `numeric`, `json`,
       `numeric` > `json` as `numeric>json`, `numeric` >= `json` as `numeric>=json`,
       `numeric` < `json` as `numeric<json`, `numeric` <= `json` as `numeric<=json`,
       `numeric` != `json` as `numeric!=json`, `numeric` <> `json` as `numeric<>json`,
       `numeric` = `json` as `numeric=json`, `numeric` <=> `json` as `numeric<=>json` from test_json_table;
select `numeric`, `json`,
       `json` > `numeric` as `json>numeric`, `json` >= `numeric` as `json>=numeric`,
       `json` < `numeric` as `json<numeric`, `json` <= `numeric` as `json<=numeric`,
       `json` != `numeric` as `json!=numeric`, `json` <> `numeric` as `json<>numeric`,
       `json` = `numeric` as `json=numeric`, `json` <=> `numeric` as `json<=>numeric` from test_json_table;
select `bit1`, `json`,
       `bit1` > `json` as `bit1>json`, `bit1` >= `json` as `bit1>=json`,
       `bit1` < `json` as `bit1<json`, `bit1` <= `json` as `bit1<=json`,
       `bit1` != `json` as `bit1!=json`, `bit1` <> `json` as `bit1<>json`,
       `bit1` = `json` as `bit1=json`, `bit1` <=> `json` as `bit1<=>json` from test_json_table;
select `bit1`, `json`,
       `json` > `bit1` as `json>bit1`, `json` >= `bit1` as `json>=bit1`,
       `json` < `bit1` as `json<bit1`, `json` <= `bit1` as `json<=bit1`,
       `json` != `bit1` as `json!=bit1`, `json` <> `bit1` as `json<>bit1`,
       `json` = `bit1` as `json=bit1`, `json` <=> `bit1` as `json<=>bit1` from test_json_table;
select `bit64`, `json`,
       `bit64` > `json` as `bit64>json`, `bit64` >= `json` as `bit64>=json`,
       `bit64` < `json` as `bit64<json`, `bit64` <= `json` as `bit64<=json`,
       `bit64` != `json` as `bit64!=json`, `bit64` <> `json` as `bit64<>json`,
       `bit64` = `json` as `bit64=json`, `bit64` <=> `json` as `bit64<=>json` from test_json_table;
select `bit64`, `json`,
       `json` > `bit64` as `json>bit64`, `json` >= `bit64` as `json>=bit64`,
       `json` < `bit64` as `json<bit64`, `json` <= `bit64` as `json<=bit64`,
       `json` != `bit64` as `json!=bit64`, `json` <> `bit64` as `json<>bit64`,
       `json` = `bit64` as `json=bit64`, `json` <=> `bit64` as `json<=>bit64` from test_json_table;
select `date`, `json`,
       `date` > `json` as `date>json`, `date` >= `json` as `date>=json`,
       `date` < `json` as `date<json`, `date` <= `json` as `date<=json`,
       `date` != `json` as `date!=json`, `date` <> `json` as `date<>json`,
       `date` = `json` as `date=json`, `date` <=> `json` as `date<=>json` from test_json_table;
select `date`, `json`,
       `json` > `date` as `json>date`, `json` >= `date` as `json>=date`,
       `json` < `date` as `json<date`, `json` <= `date` as `json<=date`,
       `json` != `date` as `json!=date`, `json` <> `date` as `json<>date`,
       `json` = `date` as `json=date`, `json` <=> `date` as `json<=>date` from test_json_table;
select `time`, `json`,
       `time` > `json` as `time>json`, `time` >= `json` as `time>=json`,
       `time` < `json` as `time<json`, `time` <= `json` as `time<=json`,
       `time` != `json` as `time!=json`, `time` <> `json` as `time<>json`,
       `time` = `json` as `time=json`, `time` <=> `json` as `time<=>json` from test_json_table;
select `time`, `json`,
       `json` > `time` as `json>time`, `json` >= `time` as `json>=time`,
       `json` < `time` as `json<time`, `json` <= `time` as `json<=time`,
       `json` != `time` as `json!=time`, `json` <> `time` as `json<>time`,
       `json` = `time` as `json=time`, `json` <=> `time` as `json<=>time` from test_json_table;
select `time(4)`, `json`,
       `time(4)` > `json` as `time(4)>json`, `time(4)` >= `json` as `time(4)>=json`,
       `time(4)` < `json` as `time(4)<json`, `time(4)` <= `json` as `time(4)<=json`,
       `time(4)` != `json` as `time(4)!=json`, `time(4)` <> `json` as `time(4)<>json`,
       `time(4)` = `json` as `time(4)=json`, `time(4)` <=> `json` as `time(4)<=>json` from test_json_table;
select `time(4)`, `json`,
       `json` > `time(4)` as `json>time(4)`, `json` >= `time(4)` as `json>=time(4)`,
       `json` < `time(4)` as `json<time(4)`, `json` <= `time(4)` as `json<=time(4)`,
       `json` != `time(4)` as `json!=time(4)`, `json` <> `time(4)` as `json<>time(4)`,
       `json` = `time(4)` as `json=time(4)`, `json` <=> `time(4)` as `json<=>time(4)` from test_json_table;
select `datetime`, `json`,
       `datetime` > `json` as `datetime>json`, `datetime` >= `json` as `datetime>=json`,
       `datetime` < `json` as `datetime<json`, `datetime` <= `json` as `datetime<=json`,
       `datetime` != `json` as `datetime!=json`, `datetime` <> `json` as `datetime<>json`,
       `datetime` = `json` as `datetime=json`, `datetime` <=> `json` as `datetime<=>json` from test_json_table;
select `datetime`, `json`,
       `json` > `datetime` as `json>datetime`, `json` >= `datetime` as `json>=datetime`,
       `json` < `datetime` as `json<datetime`, `json` <= `datetime` as `json<=datetime`,
       `json` != `datetime` as `json!=datetime`, `json` <> `datetime` as `json<>datetime`,
       `json` = `datetime` as `json=datetime`, `json` <=> `datetime` as `json<=>datetime` from test_json_table;
select `datetime(4)`, `json`,
       `datetime(4)` > `json` as `datetime(4)>json`, `datetime(4)` >= `json` as `datetime(4)>=json`,
       `datetime(4)` < `json` as `datetime(4)<json`, `datetime(4)` <= `json` as `datetime(4)<=json`,
       `datetime(4)` != `json` as `datetime(4)!=json`, `datetime(4)` <> `json` as `datetime(4)<>json`,
       `datetime(4)` = `json` as `datetime(4)=json`, `datetime(4)` <=> `json` as `datetime(4)<=>json` from test_json_table;
select `datetime(4)`, `json`,
       `json` > `datetime(4)` as `json>datetime(4)`, `json` >= `datetime(4)` as `json>=datetime(4)`,
       `json` < `datetime(4)` as `json<datetime(4)`, `json` <= `datetime(4)` as `json<=datetime(4)`,
       `json` != `datetime(4)` as `json!=datetime(4)`, `json` <> `datetime(4)` as `json<>datetime(4)`,
       `json` = `datetime(4)` as `json=datetime(4)`, `json` <=> `datetime(4)` as `json<=>datetime(4)` from test_json_table;
select `timestamp`, `json`,
       `timestamp` > `json` as `timestamp>json`, `timestamp` >= `json` as `timestamp>=json`,
       `timestamp` < `json` as `timestamp<json`, `timestamp` <= `json` as `timestamp<=json`,
       `timestamp` != `json` as `timestamp!=json`, `timestamp` <> `json` as `timestamp<>json`,
       `timestamp` = `json` as `timestamp=json`, `timestamp` <=> `json` as `timestamp<=>json` from test_json_table;
select `timestamp`, `json`,
       `json` > `timestamp` as `json>timestamp`, `json` >= `timestamp` as `json>=timestamp`,
       `json` < `timestamp` as `json<timestamp`, `json` <= `timestamp` as `json<=timestamp`,
       `json` != `timestamp` as `json!=timestamp`, `json` <> `timestamp` as `json<>timestamp`,
       `json` = `timestamp` as `json=timestamp`, `json` <=> `timestamp` as `json<=>timestamp` from test_json_table;
select `timestamp(4)`, `json`,
       `timestamp(4)` > `json` as `timestamp(4)>json`, `timestamp(4)` >= `json` as `timestamp(4)>=json`,
       `timestamp(4)` < `json` as `timestamp(4)<json`, `timestamp(4)` <= `json` as `timestamp(4)<=json`,
       `timestamp(4)` != `json` as `timestamp(4)!=json`, `timestamp(4)` <> `json` as `timestamp(4)<>json`,
       `timestamp(4)` = `json` as `timestamp(4)=json`, `timestamp(4)` <=> `json` as `timestamp(4)<=>json` from test_json_table;
select `timestamp(4)`, `json`,
       `json` > `timestamp(4)` as `json>timestamp(4)`, `json` >= `timestamp(4)` as `json>=timestamp(4)`,
       `json` < `timestamp(4)` as `json<timestamp(4)`, `json` <= `timestamp(4)` as `json<=timestamp(4)`,
       `json` != `timestamp(4)` as `json!=timestamp(4)`, `json` <> `timestamp(4)` as `json<>timestamp(4)`,
       `json` = `timestamp(4)` as `json=timestamp(4)`, `json` <=> `timestamp(4)` as `json<=>timestamp(4)` from test_json_table;
select `year`, `json`,
       `year` > `json` as `year>json`, `year` >= `json` as `year>=json`,
       `year` < `json` as `year<json`, `year` <= `json` as `year<=json`,
       `year` != `json` as `year!=json`, `year` <> `json` as `year<>json`,
       `year` = `json` as `year=json`, `year` <=> `json` as `year<=>json` from test_json_table;
select `year`, `json`,
       `json` > `year` as `json>year`, `json` >= `year` as `json>=year`,
       `json` < `year` as `json<year`, `json` <= `year` as `json<=year`,
       `json` != `year` as `json!=year`, `json` <> `year` as `json<>year`,
       `json` = `year` as `json=year`, `json` <=> `year` as `json<=>year` from test_json_table;
select `char`, `json`,
       `char` > `json` as `char>json`, `char` >= `json` as `char>=json`,
       `char` < `json` as `char<json`, `char` <= `json` as `char<=json`,
       `char` != `json` as `char!=json`, `char` <> `json` as `char<>json`,
       `char` = `json` as `char=json`, `char` <=> `json` as `char<=>json` from test_json_table;
select `char`, `json`,
       `json` > `char` as `json>char`, `json` >= `char` as `json>=char`,
       `json` < `char` as `json<char`, `json` <= `char` as `json<=char`,
       `json` != `char` as `json!=char`, `json` <> `char` as `json<>char`,
       `json` = `char` as `json=char`, `json` <=> `char` as `json<=>char` from test_json_table;
select `varchar`, `json`,
       `varchar` > `json` as `varchar>json`, `varchar` >= `json` as `varchar>=json`,
       `varchar` < `json` as `varchar<json`, `varchar` <= `json` as `varchar<=json`,
       `varchar` != `json` as `varchar!=json`, `varchar` <> `json` as `varchar<>json`,
       `varchar` = `json` as `varchar=json`, `varchar` <=> `json` as `varchar<=>json` from test_json_table;
select `varchar`, `json`,
       `json` > `varchar` as `json>varchar`, `json` >= `varchar` as `json>=varchar`,
       `json` < `varchar` as `json<varchar`, `json` <= `varchar` as `json<=varchar`,
       `json` != `varchar` as `json!=varchar`, `json` <> `varchar` as `json<>varchar`,
       `json` = `varchar` as `json=varchar`, `json` <=> `varchar` as `json<=>varchar` from test_json_table;
select `binary`, `json`,
       `binary` > `json` as `binary>json`, `binary` >= `json` as `binary>=json`,
       `binary` < `json` as `binary<json`, `binary` <= `json` as `binary<=json`,
       `binary` != `json` as `binary!=json`, `binary` <> `json` as `binary<>json`,
       `binary` = `json` as `binary=json`, `binary` <=> `json` as `binary<=>json` from test_json_table;
select `binary`, `json`,
       `json` > `binary` as `json>binary`, `json` >= `binary` as `json>=binary`,
       `json` < `binary` as `json<binary`, `json` <= `binary` as `json<=binary`,
       `json` != `binary` as `json!=binary`, `json` <> `binary` as `json<>binary`,
       `json` = `binary` as `json=binary`, `json` <=> `binary` as `json<=>binary` from test_json_table;
select `varbinary`, `json`,
       `varbinary` > `json` as `varbinary>json`, `varbinary` >= `json` as `varbinary>=json`,
       `varbinary` < `json` as `varbinary<json`, `varbinary` <= `json` as `varbinary<=json`,
       `varbinary` != `json` as `varbinary!=json`, `varbinary` <> `json` as `varbinary<>json`,
       `varbinary` = `json` as `varbinary=json`, `varbinary` <=> `json` as `varbinary<=>json` from test_json_table;
select `varbinary`, `json`,
       `json` > `varbinary` as `json>varbinary`, `json` >= `varbinary` as `json>=varbinary`,
       `json` < `varbinary` as `json<varbinary`, `json` <= `varbinary` as `json<=varbinary`,
       `json` != `varbinary` as `json!=varbinary`, `json` <> `varbinary` as `json<>varbinary`,
       `json` = `varbinary` as `json=varbinary`, `json` <=> `varbinary` as `json<=>varbinary` from test_json_table;
select `tinyblob`, `json`,
       `tinyblob` > `json` as `tinyblob>json`, `tinyblob` >= `json` as `tinyblob>=json`,
       `tinyblob` < `json` as `tinyblob<json`, `tinyblob` <= `json` as `tinyblob<=json`,
       `tinyblob` != `json` as `tinyblob!=json`, `tinyblob` <> `json` as `tinyblob<>json`,
       `tinyblob` = `json` as `tinyblob=json`, `tinyblob` <=> `json` as `tinyblob<=>json` from test_json_table;
select `tinyblob`, `json`,
       `json` > `tinyblob` as `json>tinyblob`, `json` >= `tinyblob` as `json>=tinyblob`,
       `json` < `tinyblob` as `json<tinyblob`, `json` <= `tinyblob` as `json<=tinyblob`,
       `json` != `tinyblob` as `json!=tinyblob`, `json` <> `tinyblob` as `json<>tinyblob`,
       `json` = `tinyblob` as `json=tinyblob`, `json` <=> `tinyblob` as `json<=>tinyblob` from test_json_table;
select `blob`, `json`,
       `blob` > `json` as `blob>json`, `blob` >= `json` as `blob>=json`,
       `blob` < `json` as `blob<json`, `blob` <= `json` as `blob<=json`,
       `blob` != `json` as `blob!=json`, `blob` <> `json` as `blob<>json`,
       `blob` = `json` as `blob=json`, `blob` <=> `json` as `blob<=>json` from test_json_table;
select `blob`, `json`,
       `json` > `blob` as `json>blob`, `json` >= `blob` as `json>=blob`,
       `json` < `blob` as `json<blob`, `json` <= `blob` as `json<=blob`,
       `json` != `blob` as `json!=blob`, `json` <> `blob` as `json<>blob`,
       `json` = `blob` as `json=blob`, `json` <=> `blob` as `json<=>blob` from test_json_table;
select `mediumblob`, `json`,
       `mediumblob` > `json` as `mediumblob>json`, `mediumblob` >= `json` as `mediumblob>=json`,
       `mediumblob` < `json` as `mediumblob<json`, `mediumblob` <= `json` as `mediumblob<=json`,
       `mediumblob` != `json` as `mediumblob!=json`, `mediumblob` <> `json` as `mediumblob<>json`,
       `mediumblob` = `json` as `mediumblob=json`, `mediumblob` <=> `json` as `mediumblob<=>json` from test_json_table;
select `mediumblob`, `json`,
       `json` > `mediumblob` as `json>mediumblob`, `json` >= `mediumblob` as `json>=mediumblob`,
       `json` < `mediumblob` as `json<mediumblob`, `json` <= `mediumblob` as `json<=mediumblob`,
       `json` != `mediumblob` as `json!=mediumblob`, `json` <> `mediumblob` as `json<>mediumblob`,
       `json` = `mediumblob` as `json=mediumblob`, `json` <=> `mediumblob` as `json<=>mediumblob` from test_json_table;
select `longblob`, `json`,
       `longblob` > `json` as `longblob>json`, `longblob` >= `json` as `longblob>=json`,
       `longblob` < `json` as `longblob<json`, `longblob` <= `json` as `longblob<=json`,
       `longblob` != `json` as `longblob!=json`, `longblob` <> `json` as `longblob<>json`,
       `longblob` = `json` as `longblob=json`, `longblob` <=> `json` as `longblob<=>json` from test_json_table;
select `longblob`, `json`,
       `json` > `longblob` as `json>longblob`, `json` >= `longblob` as `json>=longblob`,
       `json` < `longblob` as `json<longblob`, `json` <= `longblob` as `json<=longblob`,
       `json` != `longblob` as `json!=longblob`, `json` <> `longblob` as `json<>longblob`,
       `json` = `longblob` as `json=longblob`, `json` <=> `longblob` as `json<=>longblob` from test_json_table;
select `text`, `json`,
       `text` > `json` as `text>json`, `text` >= `json` as `text>=json`,
       `text` < `json` as `text<json`, `text` <= `json` as `text<=json`,
       `text` != `json` as `text!=json`, `text` <> `json` as `text<>json`,
       `text` = `json` as `text=json`, `text` <=> `json` as `text<=>json` from test_json_table;
select `text`, `json`,
       `json` > `text` as `json>text`, `json` >= `text` as `json>=text`,
       `json` < `text` as `json<text`, `json` <= `text` as `json<=text`,
       `json` != `text` as `json!=text`, `json` <> `text` as `json<>text`,
       `json` = `text` as `json=text`, `json` <=> `text` as `json<=>text` from test_json_table;
select `enum_t`, `json`,
       `enum_t` > `json` as `enum_t>json`, `enum_t` >= `json` as `enum_t>=json`,
       `enum_t` < `json` as `enum_t<json`, `enum_t` <= `json` as `enum_t<=json`,
       `enum_t` != `json` as `enum_t!=json`, `enum_t` <> `json` as `enum_t<>json`,
       `enum_t` = `json` as `enum_t=json`, `enum_t` <=> `json` as `enum_t<=>json` from test_json_table;
select `enum_t`, `json`,
       `json` > `enum_t` as `json>enum_t`, `json` >= `enum_t` as `json>=enum_t`,
       `json` < `enum_t` as `json<enum_t`, `json` <= `enum_t` as `json<=enum_t`,
       `json` != `enum_t` as `json!=enum_t`, `json` <> `enum_t` as `json<>enum_t`,
       `json` = `enum_t` as `json=enum_t`, `json` <=> `enum_t` as `json<=>enum_t` from test_json_table;
select `set_t`, `json`,
       `set_t` > `json` as `set_t>json`, `set_t` >= `json` as `set_t>=json`,
       `set_t` < `json` as `set_t<json`, `set_t` <= `json` as `set_t<=json`,
       `set_t` != `json` as `set_t!=json`, `set_t` <> `json` as `set_t<>json`,
       `set_t` = `json` as `set_t=json`, `set_t` <=> `json` as `set_t<=>json` from test_json_table;
select `set_t`, `json`,
       `json` > `set_t` as `json>set_t`, `json` >= `set_t` as `json>=set_t`,
       `json` < `set_t` as `json<set_t`, `json` <= `set_t` as `json<=set_t`,
       `json` != `set_t` as `json!=set_t`, `json` <> `set_t` as `json<>set_t`,
       `json` = `set_t` as `json=set_t`, `json` <=> `set_t` as `json<=>set_t` from test_json_table;
select `json`, `json`,
       `json` > `json` as `json>json`, `json` >= `json` as `json>=json`,
       `json` < `json` as `json<json`, `json` <= `json` as `json<=json`,
       `json` != `json` as `json!=json`, `json` <> `json` as `json<>json`,
       `json` = `json` as `json=json`, `json` <=> `json` as `json<=>json` from test_json_table;
DROP TABLE IF EXISTS test_json_cmp_table;
CREATE TABLE test_json_cmp_table
(
    id int NOT NULL,
    json1 json DEFAULT NULL,
    json2 json DEFAULT NULL
);
-- json1 = NULL, json2 = NULL
insert into test_json_cmp_table(id) values(1);
-- json2 = NULL
insert into test_json_cmp_table(id,json1) values(2, '{"num": 1, "name": "abc"}');
-- json1 = NULL
insert into test_json_cmp_table(id,json2) values(3, '{"num": 2, "name": "edf"}');
-- json1 = json2
insert into test_json_cmp_table(id,json1,json2) values(41, '{"num": 1, "name": "abc"}', '{"num": 1, "name": "abc"}');
insert into test_json_cmp_table(id,json1,json2) values(42, '{"num": 1, "name": "abc"}', '{"name": "abc", "num": 1}');
-- json1 < json2
insert into test_json_cmp_table(id,json1,json2) values(501, '{"num": 1, "name": "abc"}', '{"num": 2, "name": "abc"}');
insert into test_json_cmp_table(id,json1,json2) values(502, '{"num": 1, "name": "abc"}', '{"num": 1, "name": "edf"}');
insert into test_json_cmp_table(id,json1,json2) values(503, '{"num": 1, "name": "abc"}', '{"num": 1, "name": "ac"}');
insert into test_json_cmp_table(id,json1,json2) values(504, '{"num": 1, "name": "abc"}', '{"num": 1, "name": "abd"}');
insert into test_json_cmp_table(id,json1,json2) values(504, '{"num": 1, "name": "abc"}', '{"num": 1, "name": "abcd"}');
insert into test_json_cmp_table(id,json1,json2) values(505, '{"nam": 2, "name": "abc"}', '{"num1": 1, "name": "abc"}');
insert into test_json_cmp_table(id,json1,json2) values(506, '{"num": 1, "name": "edf"}', '{"num": 1, "name1": "abc"}');
insert into test_json_cmp_table(id,json1,json2) values(507, '{"num": 1, "name": "abc"}', '{"num": 1, "name": "abc", "more": 1.1}');
insert into test_json_cmp_table(id,json1,json2) values(508, '["a","b","c"]', '[1,2,3]');
insert into test_json_cmp_table(id,json1,json2) values(509, '[1,2,"a"]', '[1,2,3]');
insert into test_json_cmp_table(id,json1,json2) values(510, '[1,2,3]', '[1,2,3,4]');
insert into test_json_cmp_table(id,json1,json2) values(511, '[1,2,3]', '[1,2,"a","b"]');
insert into test_json_cmp_table(id,json1,json2) values(512, '[1,2,3]', '{"num": 1, "name": "abc"}');
select `id`, `json1`, `json2`,
       `json1` > `json2` as `json1>json2`, `json1` >= `json2` as `json1>=json2`,
       `json1` < `json2` as `json1<json2`, `json1` <= `json2` as `json1<=json2`,
       `json1` != `json2` as `json1!=json2`, `json1` <> `json2` as `json1<>json2`,
       `json1` = `json2` as `json1=json2`, `json1` <=> `json2` as `json1<=>json2` from test_json_cmp_table;
select `id`, `json1`, `json2`,
       `json2` > `json1` as `json2>json1`, `json2` >= `json1` as `json2>=json1`,
       `json2` < `json1` as `json2<json1`, `json2` <= `json1` as `json2<=json1`,
       `json2` != `json1` as `json2!=json1`, `json2` <> `json1` as `json2<>json1`,
       `json2` = `json1` as `json2=json1`, `json2` <=> `json1` as `json2<=>json1` from test_json_cmp_table;
select `id`, `json1`, '{"num": 1, "name": "edf"}' as `jsonc`,
       `json1` > `jsonc` as `json1>jsonc`, `json1` >= `jsonc` as `json1>=jsonc`,
       `json1` < `jsonc` as `json1<jsonc`, `json1` <= `jsonc` as `json1<=jsonc`,
       `json1` != `jsonc` as `json1!=jsonc`, `json1` <> `jsonc` as `json1<>jsonc`,
       `json1` = `jsonc` as `json1=jsonc`, `json1` <=> `jsonc` as `json1<=>jsonc` from test_json_cmp_table;
select `id`, `json1`, '{"num": 1, "name": "edf"}' as `jsonc`,
       `jsonc` > `json1` as `jsonc>json1`, `jsonc` >= `json1` as `jsonc>=json1`,
       `jsonc` < `json1` as `jsonc<json1`, `jsonc` <= `json1` as `jsonc<=json1`,
       `jsonc` != `json1` as `jsonc!=json1`, `jsonc` <> `json1` as `jsonc<>json1`,
       `jsonc` = `json1` as `jsonc=json1`, `jsonc` <=> `json1` as `jsonc<=>json1` from test_json_cmp_table;
---------- tail ----------
drop schema json_cmp_operator_test_schema cascade;
reset current_schema;
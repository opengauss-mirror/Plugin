create schema test_json_valid;
set current_schema to 'test_json_valid';

select json_valid(NULL);

select json_valid('1');

select json_valid('31.35621312');

select json_valid('31.356213.12');

select json_valid('03135621312');

select json_valid('0.3135621312');

select json_valid('true');

select json_valid('truee');

select json_valid('false');

select json_valid('null');

select json_valid('"hello world"');

select json_valid('3a');

select json_valid('"3a"');

select json_valid('{"a":1}');

select json_valid('{"a":1');

select json_valid('"a":1}');

select json_valid('{a":1}');

select json_valid('{"a"1}');

select json_valid('{"a":true}');

select json_valid('{"a":false}');

select json_valid('{"a":null}');

select json_valid('{"a":[1,2,3]}');

select json_valid('{"a":[[1,2,3]}');

select json_valid('{"a":{"b":"happy"}}');

select json_valid('{"a":{}}');

select json_valid('{"a":{}');

select json_valid('[1, 2, 3, true, false, "hello", "world"]');

select json_valid('[1, 2, 3');

select json_valid('1, 2, 3]');

select json_valid('[1, , 3]');

select json_valid('[1, 2 3]');

select json_valid('[1, {"hello": "world"}]');

select json_valid('[1, {"hello": world"}]');

select json_valid('[1, [[[[["test"]]]], "huawei"]]');

select json_valid('[1, [[[#["test"]]]], "huawei"]]');

select json_valid('"\uG123"');

select json_valid('"\uA123"');

select json_valid('"\123"');

select json_valid('"\t"');

select json_valid('"\a"');

select json_valid('"\udbff\udfff"');

select json_valid('"\udfff\udfff"');

select json_valid('-1.1');

select json_valid('-1.1.1');

select json_valid('"""');

select json_valid('{"a":""}');

select json_valid('{"a":"""}');

select json_valid('{"a":"\u"}');

select json_valid('{"a":truee}');

select json_valid('{"a":{"b":ffalse}}');

select json_valid('[{"a":[{"b":-2.3.3}]}]');

select json_valid('[{"a":[{"b":"\u"}]}]');

select json_valid('[{"a":[{"b":nul}]}]');


select json_valid('1'::json);

select json_valid('31.35621312'::json);

select json_valid('31.356213.12'::json);

select json_valid('03135621312'::json);

select json_valid('0.3135621312'::json);

select json_valid('true'::json);

select json_valid('truee'::json);

select json_valid('false'::json);

select json_valid('null'::json);

select json_valid('"hello world"'::json);

select json_valid('3a'::json);

select json_valid('"3a"'::json);

select json_valid('{"a":1}'::json);

select json_valid('{"a":1'::json);

select json_valid('"a":1}'::json);

select json_valid('{a":1}'::json);

select json_valid('{"a"1}'::json);

select json_valid('{"a":true}'::json);

select json_valid('{"a":false}'::json);

select json_valid('{"a":null}'::json);

select json_valid('{"a":[1,2,3]}'::json);

select json_valid('{"a":[[1,2,3]}'::json);

select json_valid('{"a":{"b":"happy"}}'::json);

select json_valid('{"a":{}}'::json);

select json_valid('{"a":{}'::json);

select json_valid('[1, 2, 3, true, false, "hello", "world"]'::json);

select json_valid('[1, 2, 3'::json);

select json_valid('1, 2, 3]'::json);

select json_valid('[1, , 3]'::json);

select json_valid('[1, 2 3]'::json);

select json_valid('[1, {"hello": "world"}]'::json);

select json_valid('[1, {"hello": world"}]'::json);

select json_valid('[1, [[[[["test"]]]], "huawei"]]'::json);

select json_valid('[1, [[[#["test"]]]], "huawei"]]'::json);

select json_valid('"\uG123"'::json);

select json_valid('"\uA123"'::json);

select json_valid('"\123"'::json);

select json_valid('"\t"'::json);

select json_valid('"\a"'::json);

select json_valid('-1.1'::json);

select json_valid('-1.1.1'::json);

select json_valid('"""'::json);

select json_valid('{"a":""}'::json);

select json_valid('{"a":"""}'::json);

select json_valid('{"a":"\u"}'::json);

select json_valid('{"a":truee}'::json);

select json_valid('{"a":{"b":ffalse}}'::json);

select json_valid('[{"a":[{"b":-2.3.3}]}]'::json);

select json_valid('[{"a":[{"b":"\u"}]}]'::json);

select json_valid('[{"a":[{"b":nul}]}]'::json);

create table json_valid_test (
    target text
);

insert into json_valid_test values('[1, [[[[["test"]]]], "huawei"]]');
insert into json_valid_test values('{"a":{"b":"happy"}}');
insert into json_valid_test values('[1, , 3]');

select target, json_valid(target) from json_valid_test;

drop table json_valid_test;


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


select json_valid(`int1`), json_valid(`uint1`), json_valid(`int2`), json_valid(`uint2`), json_valid(`int4`), json_valid(`uint4`), json_valid(`int8`), json_valid(`uint8`), json_valid(`float4`), json_valid(`float8`), json_valid(`numeric`), json_valid(`bit1`), json_valid(`bit64`), json_valid(`boolean`), json_valid(`date`), json_valid(`time`), json_valid(`time(4)`), json_valid(`datetime`), json_valid(`datetime(4)`), json_valid(`timestamp`), json_valid(`timestamp(4)`), json_valid(`year`), json_valid(`char`), json_valid(`varchar`), json_valid(`binary`), json_valid(`varbinary`), json_valid(`tinyblob`), json_valid(`blob`), json_valid(`mediumblob`), json_valid(`longblob`), json_valid(`text`), json_valid(`enum_t`), json_valid(`set_t`), json_valid(`json`),
pg_typeof(json_valid(`int1`)), pg_typeof(json_valid(`uint1`)), pg_typeof(json_valid(`int2`)), pg_typeof(json_valid(`uint2`)), pg_typeof(json_valid(`int4`)), pg_typeof(json_valid(`uint4`)), pg_typeof(json_valid(`int8`)), pg_typeof(json_valid(`uint8`)), pg_typeof(json_valid(`float4`)), pg_typeof(json_valid(`float8`)), pg_typeof(json_valid(`numeric`)), pg_typeof(json_valid(`bit1`)), pg_typeof(json_valid(`bit64`)), pg_typeof(json_valid(`boolean`)), pg_typeof(json_valid(`date`)), pg_typeof(json_valid(`time`)), pg_typeof(json_valid(`time(4)`)), pg_typeof(json_valid(`datetime`)), pg_typeof(json_valid(`datetime(4)`)), pg_typeof(json_valid(`timestamp`)), pg_typeof(json_valid(`timestamp(4)`)), pg_typeof(json_valid(`year`)), pg_typeof(json_valid(`char`)), pg_typeof(json_valid(`varchar`)), pg_typeof(json_valid(`binary`)), pg_typeof(json_valid(`varbinary`)), pg_typeof(json_valid(`tinyblob`)), pg_typeof(json_valid(`blob`)), pg_typeof(json_valid(`mediumblob`)), pg_typeof(json_valid(`longblob`)), pg_typeof(json_valid(`text`)), pg_typeof(json_valid(`enum_t`)), pg_typeof(json_valid(`set_t`)), pg_typeof(json_valid(`json`)) from test_type_table;

create table test_json_valid_cts as select json_valid(`int1`) as json_valid_1, json_valid(`uint1`) as json_valid_2, json_valid(`int2`) as json_valid_3, json_valid(`uint2`) as json_valid_4, json_valid(`int4`) as json_valid_5, json_valid(`uint4`) as json_valid_6, json_valid(`int8`) as json_valid_7, json_valid(`uint8`) as json_valid_8, json_valid(`float4`) as json_valid_9, json_valid(`float8`) as json_valid_10, json_valid(`numeric`) as json_valid_11, json_valid(`bit1`) as json_valid_12, json_valid(`bit64`) as json_valid_13, json_valid(`boolean`) as json_valid_14, json_valid(`date`) as json_valid_15, json_valid(`time`) as json_valid_16, json_valid(`time(4)`) as json_valid_17, json_valid(`datetime`) as json_valid_18, json_valid(`datetime(4)`) as json_valid_19, json_valid(`timestamp`) as json_valid_20, json_valid(`timestamp(4)`) as json_valid_21, json_valid(`char`) as json_valid_23, json_valid(`varchar`) as json_valid_24,  json_valid(`binary`) as json_valid_25, json_valid(`varbinary`) as json_valid_26, json_valid(`tinyblob`) as json_valid_27, json_valid(`blob`) as json_valid_28, json_valid(`mediumblob`) as json_valid_29, json_valid(`longblob`) as json_valid_30, json_valid(`text`) as json_valid_31, json_valid(`enum_t`) as json_valid_32, json_valid(`set_t`) as json_valid_33 from test_type_table;


select * from test_json_valid_cts;
drop table test_json_valid_cts;
drop table test_type_table;
drop schema test_json_valid cascade;
reset current_schema;



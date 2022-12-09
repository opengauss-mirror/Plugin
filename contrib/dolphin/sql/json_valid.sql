drop database if exists test_json_valid;
create database test_json_valid dbcompatibility 'b';
\c test_json_valid

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

\c postgres
drop database if exists test_json_valid;



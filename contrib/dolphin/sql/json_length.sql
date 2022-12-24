create schema test_json_length;
set current_schema to 'test_json_length';

select json_length(NULL);
select json_length('NULL');
select json_length('true');
select json_length('false');
select json_length('"abc"');
select json_length('"成都"');
select json_length('1');
select json_length('1','2');

select json_length('[]');
select json_length('{}');
select json_length('[1,2,3]');
select json_length('{"a":1,"b":2,"c":3,"d":4}');
select json_length('{"a":1:"b":2,"c":3,"d":4}');

select json_length('[null,1,true,[1,2,3],{"a":1,"b":2},"a"]');
select json_length('[1,{"a":[{"b":[1,2,3]}]},2]');
select json_length('{"a":"abc","b":["abc",{"c":"1","d":{"e":["1",2,3]}}]}');
select json_length('[{"a":"abc","b":["abc",{"c":"1","d":{"e":["1",2,3]}}]},1]');
select json_length('{"a":"abc","b":[1,2,3],"c":[{"abc":"a"},{"a":[2,3,{"c":{"a":1}}]}]}');
select json_length('[{"a":"abc","b":["abc",{"c":"1","d":{"e":["1",2,3]}}]},[1,2,3],{"a":1}]');
select json_length('[{"a":"abc","b":["abc",{"c":"1","d":{"e":["1",2,3]}}]},[1,2,3],{"a":1},[1,2,3,{"a":1,"b":2,"c":3}],1,2,3,true]');

select json_length('{"a":"abc","b":"abc"}','$.a');
select json_length('{"a":"abc","b":"abc"}','$.*');
select json_length('{"a":"abc","b":"abc"}','$.a','$.b');
select json_length('{"a":"abc","b":{"a":"abc","a":"abc"}}','$**.a');

select json_length(json_merge_patch('{"a":{"d":[1,2,3,4],"c":[5,6,7,{"e":"test"}]}}','{"b":{"c":2}}'));
select json_length(json_merge_preserve(null,'1','{"d":3,"c":5}','{"e":4,"a":{"b":1,"a":2}}'));

create table test (
    doc text
);
insert into test values
('{"a":"abc","b":["abc",{"c":"1","d":{"e":["1",2,3]}}]}'),
('[{"a":"abc","b":["abc",{"c":"1","d":{"e":["1",2,3]}}]},1]'),
('{"a":"abc","b":[1,2,3],"c":[{"abc":"a"},{"a":[2,3,{"c":{"a":1}}]}]}');
select *,json_length(doc) from test;
drop table test;

create table test (
    result int
);
insert into test values
(json_length('[{"a":"abc","b":["abc",{"c":"1","d":{"e":["1",2,3]}}]},[1,2,3],{"a":1}]')),
(json_length('[{"a":"abc","b":["abc",{"c":"1","d":{"e":["1",2,3]}}]},[1,2,3],{"a":1},[1,2,3,{"a":1,"b":2,"c":3}],1,2,3,true]'));
 select * from test;
 drop table test;

drop schema test_json_length cascade;
reset current_schema;

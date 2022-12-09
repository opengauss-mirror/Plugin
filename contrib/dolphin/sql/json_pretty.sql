drop database if exists test_json_pretty;
create database test_json_pretty dbcompatibility'B';
\c test_json_pretty

-- test for basic functionality of json_replace
select JSON_PRETTY('{"a": 43}');
select JSON_PRETTY('true');
select JSON_PRETTY('false');
select JSON_PRETTY('1');
select JSON_PRETTY('"hello world"');
select JSON_PRETTY('[1,3,4]');
select JSON_PRETTY('["a"]');
select JSON_PRETTY('["a", "b"]');
select JSON_PRETTY('["a", 232]');
select JSON_PRETTY('["a", true, false]');
select JSON_PRETTY('["a", ["a"]]');
select JSON_PRETTY('["a", {"a": "hello"}]');
select JSON_PRETTY('[111, {"a": "hello"}]');
-- test for empty object and array
select JSON_PRETTY('{}');
select JSON_PRETTY('[]');
select JSON_PRETTY('{"a": {}}');
select JSON_PRETTY('{"a": []}');
select JSON_PRETTY('[{}]');
select JSON_PRETTY('[[]]');
select JSON_PRETTY('["a", []]');
select JSON_PRETTY('["a", {}]');
-- test for null
select JSON_PRETTY(null);
-- test for invalid json doc
select JSON_PRETTY('{aa,b}');
select JSON_PRETTY('[*]');
select JSON_PRETTY('["a, []]');
select JSON_PRETTY('{'a':3}');
SELECT JSON_PRETTY('');
SELECT JSON_PRETTY(122323);
select JSON_PRETTY('{"a": }');
select JSON_PRETTY('{a: 43}');
-- test for large json doc
SELECT JSON_PRETTY('{  
    "Person": {    
       "Name": "Homer", 
       "Age": 39,
       "Hobbies": ["Eating", "Sleeping"]  
    }
 }');
select JSON_PRETTY('{"a":[{"age": 43, "name": "lihua"}, [[[[43,33, []]]]], "hello"]}');
-- test for call other functions
SELECT JSON_PRETTY(json_build_object('name', 'Lihua'));
select JSON_PRETTY(json_build_object('name', 'Lihua', 'age', '43'));
select json_build_object('ob',JSON_PRETTY('{"a": 43}'));

-- test for table
create temp table test (
    textjson json
);
insert into test values
('{"a": {"b": 32, "c":"hello"}, "d": 0.3443}'),
('["dog", "pig", {"a": "here"}]'),
('["pig", "dog", {"a": "there"}]'),
('{"a": "abc","b": {"b": "abc", "a": "abc"}}');

select JSON_PRETTY(textjson) from test;

insert into test values
(JSON_PRETTY('{"a": 43, "b": {"c": true}}'));
select * from test;
\c postgres
drop database test_json_pretty;

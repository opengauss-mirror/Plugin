create schema test_json_insert;
set current_schema to 'test_json_insert';

-- test for basic functionality of json_replace
select JSON_INSERT('{"a": 43}', '$.b', 55);
select JSON_INSERT('{"a": 43}', '$.a', 55);
select JSON_INSERT('{"a": 43}', '$.a[1]', 55);
select JSON_INSERT('{"a": 43}', '$.a[0]', 55);
select JSON_INSERT('{"a": 43}', '$.b[1]', 55);
select JSON_INSERT('{"a": 43}', '$.b[0]', 55);

select JSON_INSERT('{"a": 43}', '$.b', 'hello');
select JSON_INSERT('{"a": 43}', '$.b', cast('{"nn":"lihua"}'as json));
select JSON_INSERT('{"a": 43}', '$.b', '{"nn":"lihua"}');
select JSON_INSERT('{"a": 43}', '$.b', true);
select JSON_INSERT('{"a": 43}', '$.b', false);

-- test for null
select JSON_INSERT('{"a": 43}', '$.b', null);
select JSON_INSERT('{"a": 43}', null, 'hello');
select JSON_INSERT(null, '$.b', 'hello');

-- test for multiple path
select JSON_INSERT('{"a": 43}', '$.b', 4323, '$.c', 'hello');

-- test for invalid path
SELECT JSON_INSERT('{"x":1}',2,2);
SELECT JSON_INSERT('{"x":1}','2.as',2);

-- test for invalid json text
SELECT JSON_INSERT('', '$[1]', 9, '$[3]', 22);
SELECT JSON_INSERT(122323, '$.b', 9, '$.a', 22);
select JSON_INSERT('{"a": }', '$.b', 55);
select JSON_INSERT('{a: 43}', '$.b', 55);

-- test for path with wildcard
select JSON_INSERT('{"a": 43}', '$.*.b', 55);
select JSON_INSERT('{"a": 43}', '$**.a', 55);
select JSON_INSERT('{"a": 43}', '$[*]', 55);

-- test for incorrect arg number
SELECT JSON_INSERT('"x"');
SELECT JSON_INSERT('"x"', 2);
SELECT JSON_INSERT('"x"','a',3,true);

-- test for A Larger JSON Document
SELECT JSON_INSERT('{  
    "Person": {    
       "Name": "Homer", 
       "Age": 39,
       "Hobbies": ["Eating", "Sleeping"]  
    }
 }', '$.Person.Name', 'Bart', '$.Person.Age', 10, '$.Person.Hobbies[1]', 'Base Jumping');

-- test for call other functions
SELECT JSON_INSERT(json_build_object('name', 'Lihua'), '$.age', 43);
select JSON_INSERT(json_build_object('name', 'Lihua'), '$.b', json_build_object('mm', 43));
select json_build_object('ob',JSON_INSERT('{"a": 43}', '$.b', 66));

select JSON_INSERT('{"a": 43}', '$.b', 341522654875451.12345678901234567890123456789012345678901234567890);

-- test for table
create temp table test (
    textjson json
);
insert into test values
('{"a": {"b": 32, "c":"hello"}, "d": 0.3443}'),
('["dog", "pig", {"a": "here"}]'),
('["pig", "dog", {"a": "there"}]'),
('{"a": "abc","b": {"b": "abc", "a": "abc"}}');

select JSON_INSERT(textjson, '$.name', 'Lihua') from test;
select JSON_INSERT(textjson, '$.a[5]', 'Hi') from test;

insert into test values
(JSON_INSERT('{"a": 43, "b": {"c": true}}', '$.b[4]', 'Test'));
select * from test;


create table t_json(c1 json);
insert into t_json values ('{"k1":"v1","k2":"v2"}'::text);
insert into t_json values ('{"k1":"v1","k2":"v2"}'::varchar);
insert into t_json values ('{"k1":"v1","k2":"v2"}'::char(30));
insert into t_json values ('{"k1":"v1","k2"}'::text);
insert into t_json values ('{"k1":"v1","k2"}'::varchar);
insert into t_json values ('{"k1":"v1","k2"}'::char(30));
select * from t_json;
update t_json set c1 = '{"k1":"v1","k3":"v3"}'::text;
update t_json set c1 = '{"k1":"v1","k4":"v4"}'::varchar;
update t_json set c1 = '{"k1":"v1","k4":"v4"}'::char(30);
select * from t_json;

insert ignore into t_json values ('{"k1":"v1","k2"}'::text);
insert ignore into t_json values ('{"k1":"v1","k2"}'::varchar);
insert ignore into t_json values ('{"k1":"v1","k2"}'::char(30));

drop table t_json;
drop schema test_json_insert cascade;
reset current_schema;
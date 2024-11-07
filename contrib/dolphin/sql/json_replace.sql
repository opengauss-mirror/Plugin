create schema test_json_replace;
set current_schema to 'test_json_replace';

-- test for basic functionality of json_replace
SELECT JSON_REPLACE('{"a": 1, "b": 2, "c": 3}', '$.b', 9);
SELECT JSON_REPLACE('{"Name": "Bart", "Age": 10}', '$.Name', 'Nelson - Ha Ha!');
SELECT JSON_REPLACE('{"x": 1}', '$.x', '{"z": 2}');
SELECT JSON_REPLACE('{"x": 1}', '$.x', '{"z": 2}'::json);
SELECT JSON_REPLACE('{"x": 1}', '$.x', 'true');
SELECT JSON_REPLACE('{"x": 1}', '$.x', true);
SELECT JSON_REPLACE('{"x": 1}', '$.x', false);
SELECT JSON_REPLACE('{"x": 1}', '$.x', NULL);
SELECT JSON_REPLACE('{"x": 1}', '$.x', 'null');
SELECT JSON_REPLACE('{"x": 1}', '$.x', 'true'::json);
SELECT JSON_REPLACE('{"x": 1}', '$.x', '{"q":false}'::jsonb);
SELECT JSON_REPLACE('{"x": 1}', '$.x', '{"q":false}'::cstring);
SELECT JSON_REPLACE('{"x": 1}'::text, '$.x', '{"q":false}'::text);
SELECT JSON_REPLACE('{"x": 1}'::json, '$.x', '{"q":false}'::text);
SELECT JSON_REPLACE('{"x": 1}'::jsonb, '$.x', '{"q":false}'::cstring);
SELECT JSON_REPLACE('{"x": 1}'::cstring, '$.x', '{"q":false}');
select json_replace('{"x":3}','$',1);
select json_replace('{"x":3}','$',true);
select json_replace('{"x":3}','$','');
select json_replace('{"x":3}','$','a');
select json_replace('{"x":3}','$','{"a":2}');
select json_replace('{"x":3}','$','{"a":2}','$',2);
select json_replace('{"x":3}','$','{"a":2}'::json,'$.a',3);
select json_replace('{"x":3}','$','{"a":2}'::json,'$',null);
select json_replace('{"x": 1}', '$.x', 341522654875451.12345678901234567890123456789012345678901234567890);

-- test for Non-Existent Path
SELECT JSON_REPLACE('{"Name": "Homer", "Age": 39}', '$.Gender', 'Male');
SELECT JSON_REPLACE('{"Name": "Adam", "Age": 88}', '$.Value', 89);

-- test for Arrays
SELECT JSON_REPLACE('[1, 2, 3]', '$[0]', 9);
SELECT JSON_REPLACE('[1, 2, [3, 4, 5]]', '$[2][1]', 9);
SELECT JSON_REPLACE('[1, [2, 3]]', '$[0]', 0, '$[2]', 6);

-- test for Multiple Paths
SELECT JSON_REPLACE('[1, 2, 3, 4, 5]', '$[1]', 9, '$[3]', 22);

-- test for A Larger JSON Document
SELECT JSON_REPLACE('{  
    "Person": {    
       "Name": "Homer", 
       "Age": 39,
       "Hobbies": ["Eating", "Sleeping"]  
    }
 }', '$.Person.Name', 'Bart', '$.Person.Age', 10, '$.Person.Hobbies[1]', 'Base Jumping');

-- test for invalid path
SELECT JSON_REPLACE('{"x":1}',2,2);
SELECT JSON_REPLACE('{"x":1}','2.as',2);

-- test for invalid json text
SELECT JSON_REPLACE('', '$[1]', 9, '$[3]', 22);
SELECT JSON_REPLACE(122323, '$.b', 9, '$.a', 22);

-- test for NULL
SELECT JSON_REPLACE(null,2,2);
SELECT JSON_REPLACE(null,'$.a',true);
SELECT JSON_REPLACE(null,11);

-- test for incorrect number
SELECT JSON_REPLACE('x');
SELECT JSON_REPLACE('x', 2);
SELECT JSON_REPLACE('x','a',3,true);

-- test for invalid json document
SELECT JSON_REPLACE('x',2,2);


-- replace function for json
select replace(cast('{"key1":"values1"}' as json),'"','');
select replace(cast('{"key1"}' as json),'"','');

create table test1(attr_json json);
insert into test1 values('{"key1":"values1","description":"value1"}');
select  REPLACE (a.attr_json-> '$.description','"','') from test1 a;
drop table test1;

drop schema test_json_replace cascade;
reset current_schema;
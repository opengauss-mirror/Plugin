drop database if exists test_json_array_insert;
create database test_json_array_insert dbcompatibility 'B';
\c test_json_array_insert
SELECT JSON_ARRAY_INSERT('[1, [2, 3], {"a": [4, 5]}]', '$[0]', 0);

SELECT JSON_ARRAY_INSERT('[1, [2, 3], {"a": [4, 5]}]', '$[2]', 4);

SELECT JSON_ARRAY_INSERT('[1, [2, 3], {"a": [4, 5]}]', '$[9]', 4);

SELECT JSON_ARRAY_INSERT('[1, [2, 3], {"a": [4, 5]}]', '$.a', 4);

SELECT JSON_ARRAY_INSERT('[1, [2, 3], {"a": [4, 5]}]', '$[2]', 'x');

SELECT JSON_ARRAY_INSERT('[1, [2, 3], {"a": [4, 5]}]', '$[1][0]', 4);

SELECT JSON_ARRAY_INSERT('[1, [2, 3], {"a": [4, 5]}]', '$[2].a[0]', 4);

SELECT JSON_ARRAY_INSERT('null', '$[2]', 4);

SELECT JSON_ARRAY_INSERT('[1, [2, 3], {"a": [4, 5]}]', 'null', 4);

SELECT JSON_ARRAY_INSERT('[1, [2, 3]]', '$[1]', 4);

SELECT JSON_ARRAY_INSERT('[1, [2, 3]]', '$[0]', 4);

SELECT JSON_ARRAY_INSERT('[1, [2, 3]]', '$[1][0]', 4);

SELECT JSON_ARRAY_INSERT('{"name": "Tim", "hobby": ["car"]}', '$.hobby', 'food');

SELECT JSON_ARRAY_INSERT('{"name": "Tim", "hobby": ["car"]}', '$.name', 'food');

SELECT JSON_ARRAY_INSERT('{"name": "Tim", "hobby": ["car"]}', '$.hobby[0]', 'food');

SELECT JSON_ARRAY_INSERT('[1, 2]', '$[1]', 3);

SELECT JSON_ARRAY_INSERT('[1, 2]', '$[0]', 3);

SELECT JSON_ARRAY_INSERT('[1, 2]', '$[1]', 4);

SELECT JSON_ARRAY_INSERT('{"name": "Tim", "hobby": "car"}', '$.hobby', 'food');

SELECT JSON_ARRAY_INSERT('{"name": "Tim", "hobby": "car"}', '$.name', 'food');

SELECT JSON_ARRAY_INSERT('{"name": "Tim", "hobby": "car"}', '$.hobby', 'drink');

SELECT JSON_ARRAY_INSERT(null, '$[1]', 4);

SELECT JSON_ARRAY_INSERT('[1, [2, 3]]', null, 4);

SELECT JSON_ARRAY_INSERT('[1, [2, 3]]', '$', 4);

SELECT JSON_ARRAY_INSERT('[1, [2, 3]]', '$[3]', 4);

SELECT JSON_ARRAY_INSERT(null, '$.hobby', 'food');

SELECT JSON_ARRAY_INSERT('{"name": "Tim", "hobby": ["car"]}', null, 'food');

SELECT JSON_ARRAY_INSERT('{"name": "Tim", "hobby": ["car"]}', '$.car', 'food');

SELECT JSON_ARRAY_INSERT('{"name": "Tim", "hobby": ["car"]}', '$', 'food');

SELECT JSON_ARRAY_INSERT('[1, [2, 3]]', '$[1]', 4, '$[1]', 5);

SELECT JSON_ARRAY_INSERT('[1, [2, 3]]', '$[1]', null);

SELECT JSON_ARRAY_INSERT('[1, [2, 3]]', '$[1]', true);

SELECT JSON_ARRAY_INSERT('[1, [2, 3]]', '$[1]', '');

SELECT JSON_ARRAY_INSERT('[1, [2, 3]]', '$[1]', json '{"x": 3, "y": [1,2,3]}');

SELECT JSON_ARRAY_INSERT('[1, [2, 3]]', '$[1]', 1.2);

SELECT JSON_ARRAY_INSERT('[1, [2, 3]]', '[1]', 4);

SELECT JSON_ARRAY_INSERT('[1, [2, 3]]', '$[]', 4);

SELECT JSON_ARRAY_INSERT('[1, [2, 3]]', '$[1', 4);

SELECT JSON_ARRAY_INSERT('[1, [2, 3]]', '$.1', 4);

SELECT JSON_ARRAY_INSERT('[1, [2, 3]]', '$..1', 4);

SELECT JSON_ARRAY_INSERT('[1, [2, 3]]', '$[*]', 4);

SELECT JSON_ARRAY_INSERT('[1, [2, 3]]', ' ', 4);



\c postgres
drop database test_json_array_insert;
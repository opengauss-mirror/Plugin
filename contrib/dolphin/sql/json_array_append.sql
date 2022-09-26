drop database if exists test_json_array_append;
create database test_json_array_append dbcompatibility'B';
\c test_json_array_append

select JSON_ARRAY_APPEND('[1, [2, 3]]', '$[1]', 4);
select JSON_ARRAY_APPEND('[1, [2, 3]]', '$[0]', 4);
select JSON_ARRAY_APPEND('[1, [2, 3]]', '$[1][0]', 4);
select JSON_ARRAY_APPEND('{"name": "Tim", "hobby": ["car"]}', '$.hobby', 'food');
select JSON_ARRAY_APPEND('{"name": "Tim", "hobby": ["car"]}', '$.name', 'food');
select JSON_ARRAY_APPEND('{"name": "Tim", "hobby": ["car"]}', '$.hobby[0]', 'food');
select JSON_ARRAY_APPEND('[1, 2]', '$[1]', 3);
select JSON_ARRAY_APPEND('[1, 2]', '$[0]', 3);
select JSON_ARRAY_APPEND('[1, 2]', '$[1]', 4);
select JSON_ARRAY_APPEND('{"name": "Tim", "hobby": "car"}', '$.hobby', 'food');
select JSON_ARRAY_APPEND('{"name": "Tim", "hobby": "car"}', '$.name', 'food');
select JSON_ARRAY_APPEND('{"name": "Tim", "hobby": "car"}', '$.hobby', 'drink');
select JSON_ARRAY_APPEND(null, '$[1]', 4);
select JSON_ARRAY_APPEND('[1, [2, 3]]', null, 4);
select JSON_ARRAY_APPEND('[1, [2, 3]]', '$', 4);
select JSON_ARRAY_APPEND('[1, [2, 3]]', '$[3]', 4);
select JSON_ARRAY_APPEND(null, '$.hobby', 'food');
select JSON_ARRAY_APPEND('{"name": "Tim", "hobby": ["car"]}', null, 'food');
select JSON_ARRAY_APPEND('{"name": "Tim", "hobby": ["car"]}', '$.car', 'food');
select JSON_ARRAY_APPEND('{"name": "Tim", "hobby": ["car"]}', '$', 'food');
select JSON_ARRAY_APPEND('[1, [2, 3]]', '$[1]', 4, '$[1]', 5);
select JSON_ARRAY_APPEND('[1, [2, 3]]', '$[1]', null);
select JSON_ARRAY_APPEND('[1, [2, 3]]', '$[1]', true);
select JSON_ARRAY_APPEND('[1, [2, 3]]', '$[1]', '');
select JSON_ARRAY_APPEND('[1, [2, 3]]', '$[1]', json '{"x": 3, "y": [1,2,3]}');
select JSON_ARRAY_APPEND('[1, [2, 3]]', '$[1]', 1.2);
select JSON_ARRAY_APPEND('[1, [2, 3]]', '[1]', 4);
select JSON_ARRAY_APPEND('[1, [2, 3]]', '$[]', 4);
select JSON_ARRAY_APPEND('[1, [2, 3]]', '$[1', 4);
select JSON_ARRAY_APPEND('[1, [2, 3]]', '$.1', 4);
select JSON_ARRAY_APPEND('[1, [2, 3]]', '$..1', 4);
select JSON_ARRAY_APPEND('[1, [2, 3]]', '$[*]', 4);
select JSON_ARRAY_APPEND('[1, [2, 3]]', '$[*]', 日);
select JSON_ARRAY_APPEND('[1, [2, 3]]', ' ', 4);

\c postgres
DROP DATABASE test_json_array_append;
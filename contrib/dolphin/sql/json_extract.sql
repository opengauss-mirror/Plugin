create schema test_json_extract;
set current_schema to 'test_json_extract';

select json_extract('{"a": "lihua"}', '$.a');
select json_extract('{"a"}', '$.a');
select json_extract('{"a":[1, true, null]}', '$.a [1]');
select json_extract('{"a":[1, true, null]}', '$.a [*]');
select json_extract('{"a":{"b": "hi"}}', '$ .* .b');
select json_extract('{"a": 3233}', '$');
select json_extract('{"a": {"a": true}, "b": [{"a": "lihua"}]}', '$**.a');
select json_extract('{"a": "true", "b":{"a": 43}}', '$.a');
select json_extract('{"a": "true", "b":{"a": 43}}', '$."a"');
select json_extract(json_extract_path('{"a":{"b": [43, {"n": "hello"}]}}', 'a'), '$**.n');
select json_extract(json_build_object('a',1,'b',1.2,'c',true,'d',null,'e',json '{"x": 3, "y": [1,2,3]}'), '$.a');
select json_extract(json_build_array('a',1,'b',1.2,'c',true,'d',null,'e',json '{"x": 3, "y": [1,2,3]}'), '$[2]');
select json_extract(cast('1' as json), '$[0]', '$[1]') as extra;
select json_extract(cast('"qweqwdas"' as json), '$[1]', '$[0]') as extra;

select json_extract('["a", ["b", "c"], "d"]', '$[1]', '$[2]', '$[3]') as extra;
select json_extract('["a", ["b", "c"], "d"]', '$.a', '$[2]', '$[3]') as extra;
select json_extract('["a", ["b", "c"], {"d":"12.587"}]', '$[1][1]', '$[2]') as extra;
select json_extract('["a", ["b", "c"], {"d":"1"}]', '$[2].d', '$[5]') as extra;

select json_extract('{"a": 1, "b": 2, "c": {"d": 4}, "d":["e","x"]}', '$.a', '$.e');
select json_extract('{"a": 1, "b": 2, "c": {"d": 4}, "d":["e","x"]}', '$.z', '$.e');
select json_extract('{"a": 1, "b": 2, "c": {"d": 4}, "d":["e","x"]}', '$.a', '$.c.d', '$.d[1]');
select json_extract('{"a": 1, "b": 2, "c": {"d": 4}, "d":["e","x"]}', '$.a', '$.e');

select json_extract('"az"', null);
select json_extract('[1,"a"]', null);
select json_extract('{"a":1}', null);
select json_extract(cast('1' as json), null) as extra;
select json_extract(cast('"qweqwdas"' as json), NULL) as extra;
select json_extract('["a", ["b", "c"], {"d":"1"}]', null, '$[0]') as extra;
select json_extract('["a", ["b", "c"], {"d":"1"}]', NULL, '$[2]') as extra;

select json_extract(1, null);

create temp table test (
    textjson json
);
insert into test values
('{"a": {"b": 32, "c":"hello"}, "d": 0.3443}'),
('["dog", "pig", {"a": "here"}]'),
('["pig", "dog", {"a": "there"}]'),
('{"a": "abc","b": {"b": "abc", "a": "abc"}}');

select json_extract(textjson, '$.*.c') from test;
select json_extract(textjson, '$**.a') from test;

insert into test values
(json_extract('{"a": 43, "b": {"c": true}}', '$.b'));
select * from test;

drop schema test_json_extract cascade;
reset current_schema;
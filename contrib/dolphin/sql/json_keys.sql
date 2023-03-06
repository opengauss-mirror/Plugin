create schema test_json_keys;
set current_schema to 'test_json_keys';

SELECT JSON_KEYS('{"a":"t1"}');
SELECT JSON_KEYS('{"a":"t1","b":"t2"}');
SELECT JSON_KEYS('{"a":12}');
SELECT JSON_KEYS('{"a":12,"b":34}');
SELECT JSON_KEYS('{"a":123,"b":{"c":"qwe"}}');
SELECT JSON_KEYS('{"电子科技大学":123}');
SELECT JSON_KEYS('{efguewfioegwgf}');
SELECT JSON_KEYS('');
SELECT JSON_KEYS();
SELECT JSON_KEYS(NULL);
SELECT JSON_KEYS('{"a":123,"b":{"c":"qwe"}}',NULL);
SELECT JSON_KEYS(NULL,'$[0]');
SELECT JSON_KEYS('{}');
SELECT JSON_KEYS('[]');
SELECT JSON_KEYS('[1,2,3,4]');
SELECT JSON_KEYS('[1,2,[114,514],4]','$[2]');
SELECT JSON_KEYS(true);
SELECT JSON_KEYS(true,false);
SELECT JSON_KEYS('{"a":1,"b":{"c":2}}','$.b');
SELECT JSON_KEYS('{"a":1,"b":{"c":{"d":114},"d":514}}','$.b.c');
SELECT JSON_KEYS('[1,2,{"a":1,"b":{"c":2}},4]','$[2]');
SELECT JSON_KEYS('{"a":1,"b":{"c":2}}','$.z');
SELECT JSON_KEYS('[1,2,{"a":1,"b":{"c":2}},4]','$[8]');
SELECT JSON_KEYS('{"a":1,"b":{"c":2}}','$[0]');
SELECT JSON_KEYS('{"a":1,"b":{"c":2}}','$');
select json_keys('{"a":1,"b":2,"a":1}');
select json_keys('{"c":{"a":1,"b":2,"a":1},"z":2}','$.c');
select json_keys('{"d":1,"b":2,"a":1}');

create table student(name json);
insert into student (name) value(json_keys('{"a":123,"b":{"c":"qwe"}}'));
select name from student;

drop schema test_json_keys cascade;
reset current_schema;
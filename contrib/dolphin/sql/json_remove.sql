drop database if exists test_json_remove;
create database test_json_remove dbcompatibility'B';
\c test_json_remove


SELECT JSON_REMOVE('[0, 1, 2, [3, 4]]', '$[0]', '$[2]');
SELECT JSON_REMOVE('{"x": 1, "y": 2}', '$.x');
SELECT JSON_REMOVE('{"Name": "Homer", "Age": 39}', '$.Gender');
SELECT JSON_REMOVE('[1, 2, [3, 4, 5]]', '$[2][1]');
SELECT JSON_REMOVE('[1, 2, [3, 4, 5]]', '$[8]');
SELECT JSON_REMOVE('[1, 2, 3, 4, 5]', '$[1]','$[3]');
SELECT JSON_REMOVE('[1, 2, 3, 4, 5, 6, 7, 8]', '$[1]','$[3]','$[2]');
SELECT JSON_REMOVE('{"x": {"z":2,"a":3}, "y": 2}', '$.x.z');
SELECT JSON_REMOVE('{"x": {"z":2,"a":3}, "y": 2}', '$.x.z','$.y');
SELECT JSON_REMOVE('{"x": {"z":2,"a":3}, "y": 2}', NULL);
SELECT JSON_REMOVE(NULL, '$.x.z');
SELECT JSON_REMOVE(NULL, NULL);
SELECT JSON_REMOVE('{"x": {"z":2,"a":3}, "y": 2}', '$**');
SELECT JSON_REMOVE('[1,2,3,4]', '$[*]');
SELECT JSON_REMOVE('[1,2,3,4]', '$[1');
SELECT JSON_REMOVE('qwertyuiopasdfghjkl', '$[1]');
SELECT JSON_REMOVE('[1,2,3,4]', '$');
SELECT JSON_REMOVE('[1,2,3,4]');
SELECT JSON_REMOVE();
SELECT JSON_REMOVE('','');
SELECT JSON_REMOVE(NULL);


create table tab_json1(data json);
create table info1(name json,address varchar(100));
insert into info1 values ('{"x": {"z":2,"a":3}, "y": 2}', '$.x.z');
insert into tab_json1 SELECT JSON_REMOVE(name,address) from info1;
select * from tab_json1;

\c postgres
DROP DATABASE test_json_remove;

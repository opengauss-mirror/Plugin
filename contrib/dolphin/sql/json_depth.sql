drop database if exists test_json_depth;
create database test_json_depth dbcompatibility 'B';
\c test_json_depth

select json_depth('{}');
select json_depth('[]');
select json_depth('[10,20]');
select json_depth('[{},[]]');
select json_depth('true');
select json_depth('null');
select json_depth('[}');
select json_depth('true');
select json_depth('"dan"');
select json_depth('null');
select json_depth('[[1,{"a":1}],[1],[[[[[1]]]]]]');
select json_depth('[2,[2],[],{"a":1,"b":{"c":"daf"}}]');
select json_depth('[2,[2],[],{"a":1,"b":{"c":"d[{]{af"}}]');
SELECT JSON_DEPTH('[10, {"a": 20}]');
SELECT JSON_DEPTH('[10,{"a":1,"b":{"d":2,"e":{"balec":"bule"}}}]');
select json_depth('{          }');
select json_depth('{"a":{"b":2},"c":3}');
select json_depth('{"a":{"b":{"c":{"d":{"e":{"f":2}}}}},"c":3}');
select json_depth('[[[[[[[1]]]]]],2,3,4,5]');
select json_depth('{"a":[{"b":2},3]}');

create table test1(data json);
insert into test1 values('{"a":1}');
insert into test1 values('{"a":1}');
insert into test1 values('[1,{"a":1},{"a":1,"B":{"c":1}}]');

select *from test1;
select json_depth(data) from test1;
drop table test1;

\c postgres
drop database if exists test_json_depth;
create schema test_json_depth;
set current_schema to 'test_json_depth';

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

set dolphin.b_compatibility_mode = on;
create table test_json_table
(
   `char` char(100),
   `varchar` varchar(100),
   `json` json,
   `int1` tinyint,
   `binary` binary(100)
);

insert into test_json_table values ('[1,2,3,4,5]', '[1,2,3,4,5]', json_object('a', 1, 'b', 2), 1, '[1,2,3,4,5]');

select json_depth(`char`), json_depth(`varchar`), json_depth(`json`), pg_typeof(json_depth(`char`)), pg_typeof(json_depth(`varchar`)), pg_typeof(json_depth(`json`)) from test_json_table;

create table test1 as select json_depth(`char`) as c1, json_depth(`varchar`) as c2, json_depth(`json`) as c3 from test_json_table;

-- expect error
select json_depth(`int1`) from test_json_table;
select json_depth(`binary`) from test_json_table;

drop table test1;
drop table test_json_table;

drop schema test_json_depth cascade;
reset current_schema;
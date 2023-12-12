create schema json_objectagg_test;
set current_schema to 'json_objectagg_test';
set dolphin.b_compatibility_mode = 1;
-- create table for test
create table City(District varchar(30), Name varchar(30), Population int);
insert into City values ('Capital Region','Canberra',322723);
insert into City values ('New South Wales','Sydney',3276207);
insert into City values ('New South Wales','Newcastle',270324);
insert into City values ('New South Wales','Central Coast',227657);
insert into City values ('New South Wales','Wollongong',219761);
insert into City values ('Queensland','Brisbane',1291117);
insert into City values ('Queensland','Gold Coast',311932);
insert into City values ('Queensland','Townsville',109914);
insert into City values ('Queensland','Cairns',92273);
insert into City values ('South Australia','Adelaide',978100);
insert into City values ('Tasmania','Hobart',126118);
insert into City values ('Victoria','Melbourne',2865329);
insert into City values ('Victoria','Geelong',125382);
insert into City values ('West Australia','Perth',1096829);

-- test function
SELECT 
  JSON_OBJECTAGG(Name, Population) AS "City/Population"
FROM City;

SELECT 
  District AS State,
  JSON_OBJECTAGG(Name, Population) AS "City/Population"
FROM City
GROUP BY State;

CREATE TEMP TABLE foo (serial_num int, name text, type text);
INSERT INTO foo VALUES (847001,'t15','GE1043');
INSERT INTO foo VALUES (847002,'t16','GE1043');
INSERT INTO foo VALUES (847003,'sub-alpha','GESS90');
SELECT json_objectagg(serial_num,json_build_object('name',name,'type',type))
FROM foo;

create temp table time_table(a date, b numeric);
insert into time_table values(20200822, 1);
insert into time_table values(20211001, 2);
insert into time_table values(20221204, 3);
select json_objectagg(b, a) from time_table;
CREATE TEMP TABLE null_value (a int, b int);
SELECT json_objectagg(a, b) FROM null_value;
select json_objectagg(true,null);
select json_objectagg(FALSE,'null');
select json_objectagg('id',87);
select json_objectagg('哈哈哈','select');
select json_objectagg('["a", ["b", "c"], "d"]', null);
select json_objectagg('["a", ["b", "c"], "d"]', '[]');
select json_objectagg('{"a": 1}', null);
select json_objectagg('{"b": [1, 2]}', '{"c":{"c1":[11,null]}}');
select json_objectagg('', null);
select json_objectagg(1, 341522654875451.12345678901234567890123456789012345678901234567890);

--test for json key
create temp table json_table(a json, b text);
insert into json_table values ('{"a": "b"}', 'Json');
insert into json_table values (json_object('name', 'Jim', 'age', 20, 'name', 'Tim'), 'Text');
insert into json_table values ('{"Number": 123, "Bool": false}', null);
select json_objectagg(a, b) from json_table;

-- special type, type typcategory is N(numeric), but out result is string actually
select pg_catalog.json_objectagg(1, cast(1247 as regclass));
select pg_catalog.json_objectagg(1, cast(598 as regproc));
select pg_catalog.json_objectagg(1, cast(15 as regoperator));
select pg_catalog.json_objectagg(1, cast(15 as regoper));
select pg_catalog.json_objectagg(1, cast(598 as regprocedure));
select pg_catalog.json_objectagg(1, cast(23 as regtype));
select pg_catalog.json_objectagg(1, cast(3748 as regconfig));
select pg_catalog.json_objectagg(1, cast(3765 as regdictionary));

drop schema json_objectagg_test cascade;
reset current_schema;

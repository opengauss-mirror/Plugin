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

--test for json key
create temp table json_table(a json, b text);
insert into json_table values ('{"a": "b"}', 'Json');
insert into json_table values (json_object('name', 'Jim', 'age', 20, 'name', 'Tim'), 'Text');
insert into json_table values ('{"Number": 123, "Bool": false}', null);
select json_objectagg(a, b) from json_table;

drop schema json_objectagg_test cascade;
reset current_schema;

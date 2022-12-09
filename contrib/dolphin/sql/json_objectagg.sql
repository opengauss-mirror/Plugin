drop database if exists json_objectagg_test;
create database json_objectagg_test dbcompatibility 'B';
\c json_objectagg_test
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

\c postgres
drop database json_objectagg_test;
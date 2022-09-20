drop database if exists test_object;
create database test_object dbcompatibility 'B';
\c test_object
-- test for json_mysql_mode = false
select json_object('{a,1,b,2,3,NULL,"d e f","a b c"}');
select json_object('{a,b,"a b c"}', '{a,1,1}');

-- test for json_mysql_mode = true
set json_mysql_mode = 1;

-- test for basic functionality of json_object
select json_object('name', 'Jim', 'age', 20);
select json_object('name', 'Jim', 'age', 20, 'name', 'Tim');
select json_object('name', 'Tim', 'age', 20, 'friend', json_object('name', 'Jim', 'age', 20), 'hobby', json_build_array('games', 'sports'));
select json_object('City', 'Cairns', 'Population', 139693);

-- test for empty strings
select json_object('City', '', 'Population', 139693);
select json_object('', 'Cairns', 'Population', 139693);

-- test for empty lists
select json_object();

-- test for null values
select json_object('City', 'Cairns', 'Population', NULL);
select json_object('City', 'Cairns', NULL, 139693);

-- test for float number
select json_object('City', '', 'Population', 139693.123);
select json_object('City', '', 'Population', 0.123);
select json_object('City', '', 'Population', 5/7);

-- test for enormous number
select json_object('a', 132454676878465464652222238888856744654563565446554798798451344787945.3153);

-- test for bool values
select json_object('Red', true, 'Blue', false);

-- test for lots of pairs
select json_object('Region', 'Asia', 'color', 'red', 'City', '', 'Population', 139693.123, 'name', 'Tim', 'age', 20, 'friend', json_object('name', 'Jim', 'age', 20), 'hobby', json_build_array('games', 'sports'), 'name', 'Tim', 'age', 20, 'friend', json_object('name', 'Jim', 'age', 20), 'hobby', json_build_array('games', 'sports'));

-- test for negative input
select json_object('Number', -123);
select json_object('Number', -123.12134);
select json_object('Number', -1234564564868789790867567.456);

-- test for numeric input starting with '+'
select json_object('Number', +123);
select json_object('Number', ++123);



-- test for type check
select json_object('Type', abc);
select json_object('Type', t);

-- wrong number of param
select json_object('key');
select json_object('key', 'abc', 'value');

-- test for trim
select json_object( 'City',            'Cairns',          'Population'          , NULL );

-- test for inserting object into tables
create table tab_json1(data json);
insert into tab_json1 values (json_object('names','David','address','Beijing','Tel',1324053333,'names','Mike'));
select * from tab_json1;

-- use the data in tables
create table info1(name varchar(30),address varchar(20),tel int);
insert into info1 values ('Jack','Zhongguo',1323394);
insert into info1 values ('Tobbo','Meiguo',132333394);
insert into info1 values ('Hnana','Riben',3403234);
insert into tab_json1 select json_object('name',name,'address',address,'tel',tel) from info1;
select * from tab_json1;

-- test for setting json_mysql_mode back to false
set json_mysql_mode = 0;
select json_object('{a,1,b,2,3,NULL,"d e f","a b c"}');
select json_object('{a,b,"a b c"}', '{a,1,1}');

\c postgres
drop database test_object;
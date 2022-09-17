drop database if exists test_json_set;
create database test_json_set dbcompatibility 'B';
\c test_json_set

select json_set('{"1":2}','$."1"',6);
select json_set('{"1":2,"b":"r","q":0}','$.b',6);
select json_set('[1,2,[3,4,5]]','$[2]',9,'$[3]','[1,2]');
select json_set('[1,2,[3,4,5]]','$[1]',9,'$[2][3]',22);
select json_set('[1,2,[3,4,5]]','$[2]',9,'$[2][3]',22);
select json_set('{"student":{"id":1,"gender":"man"}}','$.age',23);
select json_set('{"student":{"id":1,"gender":"man"}}','$.student.age',23);
select json_set('{"student":{"id":1,"gender":"man"}}','$.age',23,'$.student.id',3);
select json_set('[1,2]','$.[1]');
select json_set(null);
select json_set('{"name":"lc"}',2,3);
select json_set(null,2,2);
select json_set(null,2);
select json_set('{"name":"lc"}','$.name','lcc','$.id',45);
select json_set('x',2,2);
select json_set('{"s":2}','$**.s',4);
select json_set('{"s":3}',null,4);
select json_set('{"s":3}','$.s',null);
select json_set('{"s":3}','$.s','d');

create table dataa(name VARCHAR(20) PRIMARY KEY);
insert into dataa (name) value(json_set('{"s":1}','$.s',3,'$.w',5));
select name from dataa;
drop table dataa;
\c postgres
drop database test_json_set;
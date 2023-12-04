create schema test_json_set;
set current_schema to 'test_json_set';

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
select json_set('{"s":3}','$.s',341522654875451.12345678901234567890123456789012345678901234567890);

create table dataa(name VARCHAR(20) PRIMARY KEY);
insert into dataa (name) value(json_set('{"s":1}','$.s',3,'$.w',5));
select name from dataa;
drop table dataa;

create table t1_z (jdoc JSON);
insert into t1_z select ('{' || string_agg('"key' || i || '": "value' || i || '"', ',') || '}')::json from generate_series(1,10) as i;
select json_set(jdoc,'$.key09','test09','$.key1','test1') from t1_z;
drop table t1_z;

drop schema test_json_set cascade;
reset current_schema;
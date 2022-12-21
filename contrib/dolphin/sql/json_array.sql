create schema test_json_array;
set current_schema to 'test_json_array';

select json_array(1,2,3,4);
select json_array(1,3,4);
select json_array(1,4);
select json_array(1);
select json_array(1,'a',2,3,4);
select json_array(1,2,3,'abc',4);
select json_array(1,2,3,4,true);
select json_array(1,2,3,4,null);
select json_array(1,2,3,4,true,null);
select json_array(1,255,3,414564);
select json_array(1,2,adjiw,4);

create table dataa(name VARCHAR(20) PRIMARY KEY);
insert into dataa(name) value(json_array('sjy'));
select name from dataa;
drop table dataa;

drop schema test_json_array cascade;
reset current_schema;
create schema test_json_type;
set current_schema to 'test_json_type';


---string 
select json_type('"aa"');
select json_type('""');
select json_type('"dajhdjhasgda"');
select json_type('"[1,2]"');
select json_type('"true"');
select json_type('"false"');
select json_type('"11"');


---null
select json_type('null');
select json_type(null);



---array
select json_type('[1,2]');
select json_type('[1,2,2,3,3,4,4,4,4,4,4,4,4]');
select json_type('[1,[1,2,3]]');

---object
select json_type('{"w":1}');
select json_type('{"2":[1,2,3]}');
select json_type('{"w":2,"q":[1,2,3]}');
select json_type('{"w":{"2":3},"2":4}');



---数字类型
select json_type('11');
select json_type('11.2');
select json_type('331243657894584726574382970');
select json_type('0');
select json_type('1');
select json_type('22.467253186970835436578690');


---boolean 
select json_type('true');
select json_type('false');

---error
select json_type('212asajh');
select json_type('aaaa');
select json_type('truee');
select json_type('falsee');
select json_type('nulll');
select json_type('[1,,2]');
select json_type('{w:2}');

create table test_type (t1 text ,t2 float, t3 char, t4 text);
insert into test_type values('{"a":1}',3,null,null);

select json_type(t1) from test_type;

drop schema test_json_type cascade;
reset current_schema;

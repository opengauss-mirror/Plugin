drop database if exists test_c;
create database test_c dbcompatibility 'b';
\c test_c

select json_contains('1',null);

select json_contains(null,'1');

select json_contains('[1,2,3]','1','$[0]');

select json_contains('[1,2,3]','1',null);

select json_contains('[1,2,3]',null,'$[0]');

select json_contains(null,'1','$[0]');

select json_contains('1','null');

select json_contains('null','null');

select json_contains('1',1);

select json_contains('-1','-1');

select json_contains('true','true');

select json_contains('true','false');

select json_contains('true','"true"');

select json_contains('6','6');

select json_contains('6','"6"');

select json_contains('null','null');

select json_contains('null','"null"');

select json_contains('null','true');

select json_contains('[1,2,3,4,5]','1');

select json_contains('[1,2,3,4,5]','[1,3,5]');

select json_contains('[[[[[[[1]]]]]],2,3,4,5]','[[[1]]]');

select json_contains('[[[[[[[1]]]]]],2,3,4,5]','[[[1,3,5]]]');

select json_contains('[1,2,3,[4]]','[1,4]');

select json_contains('[1,2,3,[4]]','[[1,4]]');

select json_contains('{"a":{"b":2},"c":3}','{"c":3}');

select json_contains('{"a":[1,2,3],"b":4,"c":5}','{"a":2}');

select json_contains('{"a":{"b":2},"c":3}','{"a":{"b":1}}');

select json_contains('{"a":{"b":2},"c":3}','{"a":{"b":2}}');

select json_contains('{"a":[1,2,3],"b":4,"c":5}','[1,2]');

select json_contains('{"a":[1,2,3],"b":4,"c":5}','4');

select json_contains('[1,2,3,4,5]','');

select json_contains('[1,2,3,4,5]','[1,2');

select json_contains('[1,2,3,4,5]','[1,]');

select json_contains('','1');

select json_contains('[1,2,3','1');

select json_contains('[1,2,]','1');

select json_contains('{"a":{"b":2},"c":3}','{"c:3}');

select json_contains('{"a":{"b":2},"c":3}','{"c":}');

select json_contains('{"a":{"b":2},"c":3}','{"c":null}');

select json_contains('{"a":{"b":2},null:3}','{"c":3}');

select json_contains('[1,2,3,4]','[2,4]','');

select json_contains('[1,2,3,4]','[2,4]',);

select json_contains('{"a":{"b":{"c":{"d":{"e":{"f":2}}}}},"c":3}','2','$.a.b.c.d.e.f');

select json_contains('{"a":{"b":{"c":{"d":{"e":{"f":2}}}}},"c":3}','2','$.a.b.c.d.e');

select json_contains('[1,2,3,[4,5,6,[7,8,9]]]','[7,8]','$[3][3]');

select json_contains('[1,2,3,[4,5,6,[7,8,9]]]','[7,8]','$[3]');

select json_contains('[1,2,3,[4,5,6,{"a":[7,8,9]}]]','[7,9]','$[3][3].a');

select json_contains('{"a":[{"b":2},3]}','2','$.a[0]."b"');

select json_contains('[1,2,3,4]','2','$[1]');

select json_contains('[1,2,3,4]','2','$[5]');

select json_contains('{"a":1,"b":2}','2','$.a');

select json_contains('{"a":1,"b":2}','2','$.b');

select json_contains('{"a":1,"b":2}','2','$.b.a');

select json_contains('{"a":1,"b":2}','2','$."b"');

select json_contains('{"a":1,"b":2}','2','$.c');

select json_contains('{"a":1,"b":2}','2','$.');


select json_contains('{"a":1,"b":2}','2','$$.b');

select json_contains('{"a":1,"b":2}','2','.b');

select json_contains('{"a":1,"b":2}','2','$..b');

select json_contains('{"a":1,"b":2}','2','$."b".');

select json_contains('[1,2,3,4]','2','$[1]');

select json_contains('[1,2,3,4]','2','$[1]]');

select json_contains('[1,2,3,4]','2','$[[1]]');

select json_contains('[1,2,3,4]','1','$[]');

select json_contains('[1,2,3,4]','2','$1');


select json_contains('[1,2,3,4]','[2,4]','$');

select json_contains('[1,2,3,4]','[2,4]','$[*]');

select json_contains('[1,2,3,[4,5]]','2','$**[1]');

create table json_contains_test (
    target json,
    candidate json,
    path text
);

insert into json_contains_test values('{"a":{"b":{"c":{"d":{"e":{"f":2}}}}},"c":3}','2','$.a.b.c.d.e.f');
insert into json_contains_test values('{"a":{"b":{"c":{"d":{"e":{"f":2}}}}},"c":3}','2','$.a.b.c.d.e');
insert into json_contains_test values('[1,2,3,[4,5,6,[7,8,9]]]','[7,8]','$[3][3]');
insert into json_contains_test values('[1,2,3,[4,5,6,[7,8,9]]]','[7,8]','$[3]');
insert into json_contains_test values('[1,2,3,[4,5,6,{"a":[7,8,9]}]]','[7,9]','$[3][3].a');
insert into json_contains_test values('[1,2,3,[4,5,6,{"a":[7,8,9]}]]','[7,9]','$[3][3].a');
insert into json_contains_test values('{"a":[{"b":2},3]}','2','$.a[0]."b"');
insert into json_contains_test values('[1,2,3,4]','[2,4]','$');

select *, json_contains(target, candidate, path) from json_contains_test;

drop table json_contains_test;

\c postgres
drop database if exists test_c;

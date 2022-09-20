drop database if exists test_json_merge_preserve;
create database test_json_merge_preserve dbcompatibility 'B';
\c test_json_merge_preserve

select json_merge_preserve(NULL);
select json_merge_preserve(NULL,NULL);
select json_merge_preserve('"a"');
select json_merge_preserve('1','"b"');
select json_merge_preserve('"a"','"b"');
select json_merge_preserve(1,'"b"');
select json_merge_preserve('["a",1,{"a":"abc"}]','["b",2,{"b":"bcd"}]');
select json_merge_preserve('["a",1,{"a":"abc"}]',NULL);
select json_merge_preserve(NULL,'["b",2,{"b":"bcd"}]');
select json_merge_preserve('{"a":"abc"}','{"b":"bcd"}');
select json_merge_preserve(NULL,'{"b":"bcd"}');
select json_merge_preserve('{"a":"abc"}','{"a":"bcd"}');
select json_merge_preserve('{"a":"abc"}','{" ":"bcd"}');
select json_merge_preserve('{"a":"abc"}','{"":"abc"}');
select json_merge_preserve('{"a":"abc"}','{"a":"bcd"}','{"c":"成都"}');
select json_merge_preserve('{"a":[{"b":["abc","abc"]},"ced"],"d":["efg","hij"]}','{"a":[{"c":["abc","abc"]},"ced"],"c":[1,2]}');
select json_merge_preserve('[{"a":"abc"},"bcd"]','[1]');
select json_merge_preserve('[{"a":"abc"},"bcd"]','{"1":"jks"}');
select json_merge_preserve('{"colin":[{"a":"abc"},"bcd"]}','{"colinew":"handsome"}');
select json_merge_preserve('{"colin":[{"a":"abc"},"bcd"]}','{"colinew":"handsome"}','{"colin":"huawei"}','{"a":[1,2]}');
select json_merge_preserve('1','{"a":123}','true',NULL,'{"b":true}','{"b":1}','{"c":"23"}');
select json_merge_preserve('1','{"a":123}','true',NULL,'{"b":true}','{"b":1}','[1,2,3]');
select json_merge_preserve('1','{"a":123}','true',NULL,'{"b":true}','{"b":1}','[1,2,3]','{"a":1}');
select json_merge_preserve('{"a":"abd"}','{"a":null}');
select json_merge_preserve('{"a:":"abd"}','{"a:":"bcd"}');
select json_merge_preserve('{"a:":"abd"}','{"a:":null}');
select json_merge_preserve('{"a":{"b":"abc"}}','{"a":{"b":"bcd"}}');
select json_merge_preserve('{"a":{"b":"abc"}}','{"a":{"b":null}}');
select json_merge_preserve('{"a":{"d":[1,2,3,4],"c":[5,6,7,{"e":"test"}]}}','{"b":{"c":2}}');
select json_merge_preserve('{"a":1}','[1,2,{"b":1,"a":2}]');
select json_merge_preserve('[]','[1,2,{"b":1,"a":2}]');
select json_merge_preserve('[]','{"c":{"b":1,"a":2}}');
select json_merge_preserve(null,'1','{"c":3,"b":5}');
select json_merge_preserve(null,'1','{"d":3,"c":5}','{"e":4,"a":{"b":1,"a":2}}');
select json_merge_preserve('{"c":3,"a":5}','1');
select json_merge_preserve('1','{"c":{"f":1,"e":2},"a":5}');
select json_merge_preserve('{"a":[null,true,false]}','[[1,2,3],[4,5,6],[7,8,9]]');
select json_merge_preserve('{"a":[[1,2,3],[4,5,6],[7,8,9]]}','null','true','false');
select json_merge_preserve('[[1,2,3],[4,5,6],[7,8,9]]','"gary"');
select json_merge_preserve('[[1,2,3],[4,5,6],[7,8,9]]','[[1,2,3],[4,5,6],[7,8,9]]');
select json_merge_preserve('[[1,2,3],[4,5,6],[7,8,9]]','{"a":[null,true,false]}');
select json_merge_preserve('"opengauss"','[[[[[[[[1,2]]]]]],3],"test"]');
select json_merge_preserve('"opengauss"','{"a":{"b":{"c":"test"}}}');
select json_merge_preserve('"opengauss"','"server"');
select json_merge_preserve('{}','{}');
select json_merge_preserve('{}','[]');
select json_merge_preserve('[]','{}');
select json_merge_preserve('[]','1');
select json_merge_preserve('[]','{"a":1}');
select json_merge_preserve('1','[]');
select json_merge_preserve('{"a":1}','[]');
select json_merge_preserve('[1,2,3]','[]');
select json_merge_preserve('[]','[1,2,3]');

create table test (
    doc1 json,
    doc2 json
);
insert into test values
('{"a":"abc"}','{"a":"bcd"}'),
('{"colin":[{"a":"abc"},"bcd"]}','{"colinew":"handsome"}'),
('{"a":[1,2]}','{" ":"bcd"}');
select *,json_merge_preserve(doc1,doc2) from test;

create table test1 (
    doc1 json,
    doc2 json
);
insert into test1 values
(json_merge_preserve('{"colin":[{"a":"abc"},"bcd"]}','{"colinew":"handsome"}','{"colin":"huawei"}','{"a":[1,2]}'),
 json_merge_preserve('[{"a":"abc"},"bcd"]','{"1":"jks"}'));
 select * from test1;

\c postgres;
drop database if exists test_json_merge_preserve;
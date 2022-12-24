create schema test_json_search;
set current_schema to 'test_json_search';

select json_search('null','one','null','&','$');
select json_search(null,'one','null','&','$');
select json_search('"null"',null,'null','&','$');
select json_search('null','one',null,'&','$');
select json_search('"null"','one','null',null,'$');
select json_search('"null"','one','null','&','$',null);
select json_search('"null"','one','null','&');

select json_search(true,'one','true','&','$');
select json_search('"true"',true,'true','&','$');
select json_search('"true"','one',true,'&','$');
select json_search('"true"','one','true',true,'$');
select json_search('"true"','one','true','&',true);

select json_search('"1"','one',1,'&');
select json_search('1.2','one',1.2);
select json_search('"1"','one','1');

select json_search('"abc"','one','abc');
select json_search('"abc"','one','"abc"');
select json_search('"abc"','all','abc');
select json_search('"*"','all','*');
select json_search('"&*(%"','all','&*(%');
select json_search('"、——》。*“”"','all','、——》。*“”');
select json_search('"华为mate40"','all','华为mate40');
select json_search('{"abc":"22:30:20"}','one',time'22:30:20');

select json_search('"abc"','all','a_c');
select json_search('"abdc"','all','a_c');
select json_search('"abdc"','all','a__c');
select json_search('"abdc"','all','a%c');

select json_search('"a%c"','all','a\%c');
select json_search('"a%c"','all','a&%c','&');
select json_search('"a%c"','all','a1%c','1');
select json_search('"a%c"','all','a1%c',1);
select json_search('"a%c"','all','a1.2%c',1.2);
select json_search('"a%c"','all','aa%c','a');
select json_search('"%c"','all','a%c','a');
select json_search('"a%c"','all','a&@%c','&@');
select json_search('"a%c"','one','a\%c',true);
select json_search('"a%c"','one','a\%c',false);

select json_search('{"a":}','one','abc');
select json_search('{"a":"abc"}','one','abc');
select json_search('{"a":"abc"}aaa','one','abc');
select json_search('{"[1].a":"abc"}','one','abc');
select json_search('{"a":"1"}','one',1);
select json_search('{"a":"1"}','one','1');
select json_search('{"a":1}','one','1');
select json_search('{"":"abc"}','one','abc');
select json_search('{" ":"abc"}','one','abc');
select json_search('{"a":"abc"}','all','abc');
select json_search('{"a":"abc"}','all','ab');
select json_search('{"a":"阿斯蒂芬"}','all','阿斯蒂芬');
select json_search('{"a c":"abc"}','one','abc');
select json_search('{"a.c":"abc"}','one','abc');
select json_search('{"a[]c":"abc"}','one','abc');
select json_search('{"0":"abc"}','one','abc');
select json_search('{"abc.bcd":"hello"}','one','hello',NULL,'$."abc.bcd"');
select json_search('{"abc,bcd":"hello"}','all','hello',NULL,'$."abc,bcd"');
select json_search('{"a":{"abc":{"b":"abc"}}}','one','abc',NULL,'$.a."abc".b');
select json_search('{"a":{"abc,cbd":{"b":"abc"}}}','one','abc',NULL,'$.a."abc,cbd".b');
select json_search('{"abc,bcd":"hello"}', 'all','hello');
select json_search('{"a":"abc","b":"bcd"}','all','abc');
select json_search('{"a":"abc","a":"abc"}','all','abc');
select json_search('{"a":"abc","a":"abc"}','one','abc');
select json_search('{"a":"abc","b":"bcd"}','one','bcd');
select json_search('{"a":"abc","b":"abc"}','all','abc');
select json_search('{"a":"abc","b":"abc","c":"abc"}','all','abc');

select json_search('["a"."abc"]','one','abc');
select json_search('["a","abc"]','one','abc');
select json_search('["a","abc"]aaa','one','abc');
select json_search('[1,"abc"]','one','1');
select json_search('[1,"abc"]','one',1);
select json_search('["1","abc"]','one','1');
select json_search('["1","abc"]','one',1);
select json_search('[" ","abc"]','one',' ');
select json_search('["","abc"]','one','');
select json_search('["abc","abc","abc"]','one','abc');
select json_search('["abc","abc","abc"]','all','abc');

select json_search('{"a":["abc","bcd"],"b":"bcd"}','all','bcd');
select json_search('[{"a":"abc"},"bcd"]','one','abc');
select json_search('[{"a":"abc"},"bcd"]','one','ab');
select json_search('{"a":{"b":"abc"}}','all','abc');
select json_search('{"a":{"b":["abc"]}}','all','abc');
select json_search('{"a":{"b":["abc","abc"]}}','all','abc');
select json_search('{"a":[{"b":["abc","abc"]}]}','all','abc');
select json_search('{"a":[{"b":["abc","abc"]},"ced"]}','all','abc');
select json_search('{"a":[{"b":["abc","abc"]},"ced"],"c":["abc","hij"]}','all','abc');

select json_search('{"abc.bcd":"hello"}', 'one','hello',NULL,'$.    "abc.bcd"    ');
select json_search('{"abc":"abc"}','all','abc',NULL,'$   .  abc   ');
select json_search('["abc"]','all','abc',NULL,'$   [  0  ]   ');
select json_search('"abc"','one','abc',null,'');
select json_search('"abc"','one','abc',null,'$.1');
select json_search('{"0":"abc"}','one','abc',NULL,'$.0');
select json_search('"abc"','one','abc',null,'$[a]');
select json_search('"abc"','one','abc',null,'$[a');
select json_search('"abc"','one','abc',null,'$[a');
select json_search('"abc"','one','abc',null,'$.');
select json_search('{"":"abc"}','one','abc',NULL,'$.');
select json_search('{" ":"abc"}','one','abc',NULL,'$. ');
select json_search('[{"a":"abc"},"bcd"]','one','abc',null,'$');
select json_search('{"a":"abc","b":"abc"}','all','abc',null,'$');
select json_search('"abc"','one','abc',NULL,'$','$.a');
select json_search('"abc"','one','abc',NULL,'$.a','$');
select json_search('{"a":["abc","bcd"],"b":"bcd"}','all','bcd',null,'$.b');
select json_search('{"a":["abc","bcd"],"b":"bcd"}','all','bcd',null,'$.b','$','$.a');
select json_search('{"abc":["abc","bcd"],"abc":"bcd"}','all','bcd',null,'$.abc','$','$[1]');
select json_search('{"a": "abc","b": {"b": "abc", "a": "abc"}}','all','abc');
select json_search('{"a":"abc"}','all','abc',NULL,'$','$');

select json_search('{"a":["abc","bcd"],"b":"bcd"}','one','bcd',null,'$.*');
select json_search('{"a":["abc","bcd"],"b":"bcd"}','one','bcd',null,'$[*]');
select json_search('{"a":["abc","bcd"],"b":"bcd"}','all','bcd',null,'$.a[*]');
select json_search('{"a":["abc","bcd"],"b":"bcd"}','one','bcd',null,'$.a[**]');
select json_search('["abc","bcd"]','one','bcd',null,'$[*]');
select json_search('["abc","bcd"]','one','bcd',null,'$**[1]');
select json_search('["abc","bcd"]','one','bcd',null,'$**[2]');
select json_search('["abc","bcd"]','one','bcd',null,'$***[2]');
select json_search('[{"a":"abc"},"bcd"]','one','bcd',null,'$**.a');
select json_search('[{"a":"abc"},"bcd"]','all','abc',null,'$**.a');
select json_search('[{"a":"abc"},"bcd"]','one','abc',null,'$**.a[*]');
select json_search('[{"a":"abc"},"bcd"]','one','bcd',null,'$**a[*]');
select json_search('{"a":["abc","bcd"],"b":"bcd"}','one','bcd',null,'$**[1]');
select json_search('[{"a":"abc"},{"a":"abc"}]','one','abc',null,'$**.a');
select json_search('[{"a":"abc"},{"a":"abc"}]','all','abc',null,'$**.a');
select json_search('[{"a":"abc"},{"a":"abc"}]','all','abc',null,'$**.b');
select json_search('{"a":[{"b":["abc","abc"]},"ced"],"c":["efg","abc"]}','all','abc',null,'$','$');
select json_search('{"a":[{"b":["abc","bcd"]},"ced"],"c":["efg","abc"]}','all','abc',null,'$');
select json_search('{"a":[{"b":["abc","abc"]},"ced"],"c":["efg","abc"]}','all','abc',null,'$**[1]');
select json_search('{"a":[{"b":["abc","abc"]},"ced"],"c":["efg","abc"]}','all','abc',null,'$**.*[*].*[*]');
select json_search('{"a":[{"b":["abc","abc"]},"ced"],"c":["efg","abc"]}','all','abc',null,'$**.*[*].*[*].*');
select json_search('{"a":[{"b":["abc","abc"]},"ced"],"c":["efg","abc"]}','all','abc',null,'$**.*[*].*[*].*','$**.*[*].*[*]');
select json_search('{"a":[{"b":["abc","abc"]},"ced"],"c":["efg","abc"]}','all','abc',null,'$**.*[*].*[*].*','$.');
select json_search('{"a":[{"b":["abc","abc"]},"ced"],"c":["efg","abc"]}','one','abc',null,'$','$.*.*');
select json_search('[{"a":"abc","b":"abc"},"abc"]','all','abc',NULL,'$**.b','$[*]','$**.a');
select json_search('[{"a":"abc","b":"abc"},"abc"]','all','abc',NULL,'$**.b','$[*]','$**.a','$[0]','$[1]','$[0].a','$[0].b');

create table student(name VARCHAR(20) PRIMARY KEY);
insert into student (name) value(json_search('{"a":[{"b":["abc","abc"]},"ced"],"c":["efg","hij"]}','all','abc',null,'$**[1]'));
select name from student;
drop table student;

create table c (
    doc text);
insert into c values('{"a":"abc"}');
select json_search(doc,'one','abc') from c;
drop table a;

create temp table test (
    doc text
);
insert into test values
('{"a": {"b": 32, "c":"hello"}, "d": 0.3443}'),
('["dog", "pig", {"a": "here"}]'),
('["pig", "dog", {"a": "there"}]'),
('{"a": "abc","b": {"b": "abc", "a": "abc"}}');
select *,json_search(doc, 'one','hello',NULL,'$**.c') from test;

create temp table test1 (
    doc text,
    one_or_all text,
    path text
);
insert into test1 values
('{"a": {"b": 32, "c":"hello"}, "d": 0.3443}','one','$**.c'),
('["dog", "pig", {"a": "here"}]','one','$'),
('["pig", "there", {"a": "there"}]','all','$'),
('{"a": "abc","b": {"b": "abc", "a": "abc"}}','all','$**.a');
select *,json_search(doc, one_or_all,'abc',NULL,path) from test1;

create table json_search_test (
    a text
);
insert into json_search_test values
('{"a":[{"d":["abc","bcd"]},"ced"],"c":["efg","hij"]}'),
('["abc","bcd"]'),
('"abc"'),
('{"a":"阿斯蒂芬"}'),
('[" ","abc"]');
insert into json_search_test values
(json_search('{"a": 43, "b": {"c": "true"}}', 'one','true',NULL,'$.b'));
select * from json_search_test;
drop table json_search_test;

drop schema test_json_search cascade;
reset current_schema;
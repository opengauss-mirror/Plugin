drop database if exists test_operator;
create database test_operator dbcompatibility 'B';
\c test_operator

drop table if exists test1;
create table test1(data json);
insert into test1 values('[10,20,[30,40]]');
select data->'$[1]' from test1;
select data->'$[2][1]' from test1;
drop table if exists test2;
create table test2(data json);
insert into test2 values('{"a":"lihua"}');
insert into test2 values('{"b":"zhanghai"}');
insert into test2 values('{"c":"susan"}');
select data->'$.a' from test2;
select data->'$.b' from test2;
select data->'$.c' from test2;
select data->'$.*' from test2;
select data->>'$.*' from test2;
select data->>'$.d' from test2;
select data->'$.d' from test2;
select data->>'$.a' from test2;
select data->>'$.b' from test2;
select data->>'$.c' from test2;
select data->'a' from test2;
select data->'b' from test2;
select data->'c' from test2;
select data->>'a' from test2;
select data->>'b' from test2;
select data->>'c' from test2;


\c postgres
drop database test_operator;


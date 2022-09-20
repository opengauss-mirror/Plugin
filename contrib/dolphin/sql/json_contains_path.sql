drop database if exists test_cp;
create database test_cp dbcompatibility 'b';
\c test_cp

select json_contains_path(null,'one','$[0]');

select json_contains_path('[1,2,3]',null,'$[0]');

select json_contains_path('[1,2,3]','one','$[0]','$[1]');

select json_contains_path('[1,2,3]','one',null,'$[0]','$[1]');

select json_contains_path('[1,2,3]','one','$[0]',null,'$[1]');

select json_contains_path('[1,2,3]','one','$[3]',null,'$[1]');

select json_contains_path('[1,2,3]','all',null,'$[0]','$[1]');

select json_contains_path('[1,2,3]','all','$[3]',null,'$[1]');

select json_contains_path('[1,2,3]','all','$[0]','$[1]');

select json_contains_path('{"a": 1, "b": 2}', 'one', '$.a' );

select json_contains_path('{"a": 1, "b": 2}', 'one', '$.c' );

select json_contains_path('{"!@#$%^&*() ": 1, "b": 2}', 'one', '$."!@#$%^&*() "' );

select json_contains_path('[0,1,2]','one','$[0]');

select json_contains_path('[0,1,2]','one','$[3]');

select json_contains_path('{"a": 1, "b": 2}', 'all', '$.a' );

select json_contains_path('{"a": 1, "b": 2}', 'all', '$.c' );

select json_contains_path('{"!@#$%^&*() ": 1, "b": 2}', 'all', '$."!@#$%^&*() "' );

select json_contains_path('[0,1,2]','all','$[0]');

select json_contains_path('[0,1,2]','all','$[3]');



select json_contains_path('{"a": 1, "b": 2, "c": {"d": 4}}', 'one', '$.a', '$.e');

select json_contains_path('{"a": 1, "b": 2, "c": {"d": 4}}', 'all', '$.a', '$.b','$."c".d');

select json_contains_path('{"a": 1, "b": 2, "c": {"d": [3,4,5]}}', 'one', '$.c.d[3]');

select json_contains_path('{"a": 1, "b": 2, "c": {"d": 4}}', 'all', '$.a.d');




select json_contains_path('{"a": 1, "b": 2, "c": {"d": 4}}', 'one', '$.');

select json_contains_path('{"a": 1, "b": 2, "c": {"d": 4}}', 'one', '$.a', '$.');

select json_contains_path('{"a": 1, "b": 2, "c": {"d": 4}}', 'one', '$.', '$.a');

select json_contains_path('[0,1,2]', 'one', '$[0]','$1]');

select json_contains_path('[0,1,2]', 'one', '$[0','$[1]');



select json_contains_path('{"a": 1, "b": 2, "c": {"d": 4}}', 'all', '$.a', '$.b','$."c.d');

create table json_contains_path_test (
    target json,
    mode text
);

insert into json_contains_path_test values('{"a": 1, "b": 2, "c": {"d": 4}}', 'one');
insert into json_contains_path_test values('{"a": 1, "b": 2, "c": {"d": 4}}', 'all');

select *, json_contains_path(target, mode, '$.a.d', '$.c.d') from json_contains_path_test;

drop table json_contains_path_test;

\c postgres
drop database if exists test_cp;
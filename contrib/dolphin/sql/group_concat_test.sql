create schema t;
set current_schema to 't';

create table t(id text, v text);
insert into t(id, v) values('1','a'),('2','b'),('1','c'),('2','d');

select group_concat(id,v separator ';') into tmp_table from t;
select * from tmp_table;

set explain_perf_mode=pretty;
explain verbose select id, group_concat(VARIADIC ARRAY[id,':',v] order by id) as title from t group by id;

drop schema t cascade;
reset current_schema;

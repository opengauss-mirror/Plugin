create database t dbcompatibility 'B';
\c t;

create table t(id text, v text);
insert into t(id, v) values('1','a'),('2','b'),('1','c'),('2','d');

select group_concat(id,v separator ';') into tmp_table from t;
select * from tmp_table;

set explain_perf_mode=pretty;
explain verbose select id, group_concat(VARIADIC ARRAY[id,':',v] order by id) as title from t group by id;

\c postgres
drop database t;

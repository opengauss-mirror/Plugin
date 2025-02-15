create schema m_testcase_fix;
set current_schema to 'm_testcase_fix';

create table t1 (a char(2) character set binary);
insert into t1 values ('aa'), ('bb');
select * from t1 where a in (NULL, 'aa');

select * from t1 where a in ('bb', NULL, 'aa');
drop table t1;

drop schema m_testcase_fix cascade;
reset current_schema;
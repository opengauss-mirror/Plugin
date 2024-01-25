drop schema if exists test_default;
create schema test_default;
set current_schema = test_default;

create table t1(a timestamp default pg_systimestamp()::timestamp(0));
\d t1;
show create table t1;
create table t2(a int, b int GENERATED ALWAYS AS (length((a)::text)) STORED);
\d t2;
show create table t2;
create table t3(a timestamp default pg_systimestamp()::timestamp(0), b int GENERATED ALWAYS AS (length((a)::text)) STORED);
\d t3
show create table t3;
create table t4(a timestamp default pg_systimestamp()::timestamp(0) on update current_timestamp);
\d t4
show create table t4;
create table t5(id int, role varchar(100), data varchar(100));
ALTER TABLE t5 ENABLE ROW LEVEL SECURITY;
CREATE ROW LEVEL SECURITY POLICY t5_rls ON t5 USING(role = CURRENT_USER);
select pg_catalog.pg_get_expr(r.polqual, r.polrelid) from pg_catalog.pg_rlspolicy r left join pg_catalog.pg_class c on c.oid = r.polrelid where c.relname = 't5';
create table t6(a timestamp, b time);
create index idx_t6 on t6(a) where a > b::timestamp;
select pg_catalog.pg_get_expr(i.indpred, i.indrelid) from pg_catalog.pg_index i left join pg_catalog.pg_class c on c.oid = i.indrelid where c.relname = 't6';
create table t7(a timestamp check(a > pg_systimestamp()::timestamp(0)));
select pg_catalog.pg_get_expr(co.conbin, co.conrelid) from pg_catalog.pg_constraint co left join pg_catalog.pg_class cl on cl.oid = co.conrelid where cl.relname = 't7';

CREATE TABLE test(
id BIGINT NOT NULL DEFAULT '-1',
name INT(11) NOT NULL DEFAULT -1,
col1 float default '-1.1', col2 double default '+2.2', col3 float default '-1.1e3');
show create table test;
drop table test;

drop table t1;
drop table t2;
drop table t3;
drop table t4;
drop table t5;
drop table t6;
drop table t7;

create table t1(a int, b int as (a) stored);
alter table t1 add gc bigint as (a) stored;
show create table t1;
drop table t1;

reset current_schema;
drop schema test_default cascade;

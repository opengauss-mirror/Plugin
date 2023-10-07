create schema sql_mode_full_group;
set current_schema to 'sql_mode_full_group';
create table test_group(a int, b int, c int, d int);
create table test_group1(a int, b int, c int, d int);
insert into test_group values(1,2,3,4);
insert into test_group values(1,3,4,5);
insert into test_group values(2,4,5,6);
insert into test_group values(3,5,6,7);
insert into test_group values(2,4,4,5);
insert into test_group values(3,3,4,5);
insert into test_group values(3,3,4,6);
insert into test_group1 values(1,2,3,4);
insert into test_group1 values(1,3,4,5);
select a, b from test_group group by a;
select a, d as items, (select count(*) from test_group t where t.a = i.a and b in (select b from test_group1 where c = 4)) as third from test_group i group by a;
select t.a, (select sum(b) from test_group i where i.b = t.b ) from test_group t where t.a > 1+1 or (t.b < 8 and t.b > 1) group by t.a;
set dolphin.sql_mode = '';
--min/max without optimize
create table t1(a int, b int, primary key(b));
insert into t1 values(1,10);
--can't use index scan
explain(costs off) select a, max(b) from t1;
select a, max(b) from t1;
--use index scan
explain(costs off) select max(b) from t1;
select max(b) from t1;
drop table t1;

select a, b from test_group group by a;
select a, d as items, (select count(*) from test_group t where t.a = i.a and b in (select b from test_group1 where c = 4)) as third from test_group i group by a;
select t.a, (select sum(b) from test_group i where i.b = t.b ) from test_group t where t.a > 1+1 or (t.b < 8 and t.b > 1) group by t.a;
drop schema sql_mode_full_group cascade;
reset current_schema;
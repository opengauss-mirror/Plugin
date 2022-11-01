drop database if exists tinyint_smp;
create database tinyint_smp dbcompatibility 'b';
\c tinyint_smp

set enable_opfusion = on;
set opfusion_debug_mode = log;

drop table if exists t1;
drop table if exists t2;
create table t1(a int1);
create table t2(a int4);
insert into t2 select generate_series(0, 1000000);
insert into t1 select a % 128 from t2;


explain(costs off, verbose) select * from t2 where a = 2;
explain(costs off, verbose) select * from t2 where a = 2 and t2.a in (select a from t2 where a > 500);
 set query_dop = 4;
explain(costs off, verbose)  select * from t2 where a = 2;
explain(costs off, verbose) select * from t2 where a = 2 and t2.a in (select a from t2 where a > 500);

set query_dop = 1;
set opfusion_debug_mode = off;

create table join_1(a int1);
create table join_2(a int1);

insert into join_1 select a % 128 from t2;
insert into join_2 select a % 128 from t2;

set query_dop = 2;
explain(costs off, verbose) select * from join_1 join join_2;
explain(costs off, verbose) select * from join_1 join join_2 on join_1.a = join_2.a;
explain(costs off, verbose) select * from join_1 left join join_2 on join_1.a = join_2.a;
explain(costs off, verbose) select * from join_1 right join join_2 on join_1.a = join_2.a;
explain(costs off, verbose) select * from join_1 inner join join_2 on join_1.a = join_2.a;

explain(costs off, verbose) select /*+ nestloop(join_1 join_2)*/ * from join_1 left join join_2 on join_1.a = join_2.a;
explain(costs off, verbose) select /*+ hashjoin(join_1 join_2)*/ * from join_1 left join join_2 on join_1.a = join_2.a;
explain(costs off, verbose) select /*+ mergejoin(join_1 join_2)*/ * from join_1 left join join_2 on join_1.a = join_2.a;

create procedure test_p1(int1, tinyint)
SHIPPABLE VOLATILE
as
begin
    insert into test1 values($1 + $2);
end;
/

create table test1(a int1);
select test_p1(1, 3);
select test_p1(-1, -3);
select * from test1;

\c postgres
drop database tinyint_smp;
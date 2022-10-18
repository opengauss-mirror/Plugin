drop database if exists uint_smp;
create database uint_smp dbcompatibility 'b';
\c uint_smp

set enable_opfusion = on;
set opfusion_debug_mode = log;

drop table if exists t2 ;
create table t2(a uint1);
insert into t2 select generate_series(1,255);
insert into t2 select generate_series(1,255);
insert into t2 select generate_series(1,255);
insert into t2 select generate_series(1,255);
insert into t2 select generate_series(1,255);
insert into t2 select generate_series(1,255);
insert into t2 select generate_series(1,255);
insert into t2 select generate_series(1,255);
insert into t2 select generate_series(1,255);
insert into t2 select generate_series(1,255);
insert into t2 select generate_series(1,255);
insert into t2 select generate_series(1,255);

explain(costs off, verbose) select * from t2 where a = 2;
explain(costs off, verbose) select * from t2 where a = 2 and t2.a in (select a from t2 where a > 500);
 set query_dop = 4;
explain(costs off, verbose)  select * from t2 where a = 2;
explain(costs off, verbose) select * from t2 where a = 2 and t2.a in (select a from t2 where a > 500);
 set query_dop = 1;

drop table if exists t2 ;
create table t2(a uint2);
insert into t2 select generate_series(1,25500);
insert into t2 select generate_series(1,25500);
insert into t2 select generate_series(1,25500);
insert into t2 select generate_series(1,25500);
insert into t2 select generate_series(1,25500);
insert into t2 select generate_series(1,25500);
insert into t2 select generate_series(1,25500);
insert into t2 select generate_series(1,25500);
explain(costs off, verbose) select * from t2 where a = 2;
explain(costs off, verbose) select * from t2 where a = 2 and t2.a in (select a from t2 where a > 500);
set query_dop = 4;
explain(costs off, verbose)  select * from t2 where a = 2;
explain(costs off, verbose) select * from t2 where a = 2 and t2.a in (select a from t2 where a > 500);
set query_dop = 1;

drop table if exists t2 ;
create table t2(a uint4);
insert into t2 select generate_series(1,200000);
insert into t2 select generate_series(1,200000);
explain(costs off, verbose) select * from t2 where a = 2;
explain(costs off, verbose) select * from t2 where a = 2 and t2.a in (select a from t2 where a < 500);
 set query_dop = 4;
explain(costs off, verbose)  select * from t2 where a = 2;
explain(costs off, verbose) select * from t2 where a = 2 and t2.a in (select a from t2 where a < 500);
 set query_dop = 1;

drop table if exists t2 ;
create table t2(a uint8);
insert into t2 select generate_series(1,200000);
insert into t2 select generate_series(1,200000);
explain(costs off, verbose)   select * from t2 where a = 2;
explain(costs off, verbose) select * from t2 where a = 2 and t2.a in (select a from t2 where a < 500);
 set query_dop = 4;
explain(costs off, verbose)  select * from t2 where a = 2;
explain(costs off, verbose) select * from t2 where a = 2 and t2.a in (select a from t2 where a < 500);
 set query_dop = 1;

set opfusion_debug_mode = off;

create table join_1(a uint4);
create table join_2(a uint8);

insert into join_1 select generate_series(1, 150000);
insert into join_2 select generate_series(1, 150000);

set query_dop = 2;
explain(costs off, verbose) select * from join_1 join join_2;
explain(costs off, verbose) select * from join_1 join join_2 on join_1.a = join_2.a;
explain(costs off, verbose) select * from join_1 left join join_2 on join_1.a = join_2.a;
explain(costs off, verbose) select * from join_1 right join join_2 on join_1.a = join_2.a;
explain(costs off, verbose) select * from join_1 inner join join_2 on join_1.a = join_2.a;

explain(costs off, verbose) select /*+ nestloop(join_1 join_2)*/ * from join_1 left join join_2 on join_1.a = join_2.a;
explain(costs off, verbose) select /*+ hashjoin(join_1 join_2)*/ * from join_1 left join join_2 on join_1.a = join_2.a;
explain(costs off, verbose) select /*+ mergejoin(join_1 join_2)*/ * from join_1 left join join_2 on join_1.a = join_2.a;

\c postgres
drop database uint_smp;
create schema tinyint_agg;
set current_schema to 'tinyint_agg';


create table u1(a int1, b int2);
insert into u1 values(null, null),(127, 127),(0, 0),(-128, -128),(null, null);
select avg(a), avg(b) from u1;
select bit_and(a), bit_and(b) from u1;
select bit_or(a), bit_or(b) from u1;
select count(a), count(b) from u1;
select count(distinct a), count(distinct b) from u1;
select max(a), max(b) from u1;
select min(a), min(b) from u1;
select stddev(a), stddev(b) from u1;
select stddev_pop(a), stddev_pop(b) from u1;
select stddev_samp(a), stddev_samp(b) from u1;
select sum(a), sum(b) from u1;
select var_pop(a), var_pop(b) from u1;
select var_samp(a), var_samp(b) from u1;
select variance(a), variance(b) from u1;
select listagg(a) within group(order by a) from u1;
select listagg(a, ',') within group(order by a) from u1;

create table t_count(a int, b int);
insert into t_count values(1,1);
insert into t_count values(1,2);
insert into t_count values(1,3);

insert into t_count values(2,1);
insert into t_count values(2,2);
insert into t_count values(2,3);

insert into t_count values(2,1);
insert into t_count values(2,2);
insert into t_count values(2,3);

select count(distinct a,b) from t_count;
select count(distinct a) from t_count;
select count(distinct b) from t_count;
drop table t_count;

drop table if exists smp_test;
drop table if exists t1;
create table smp_test(a int1);
create table t1(a int4);
insert into t1 select generate_series(1, 1000000);
insert into smp_test select a % 128 from t1;

set query_dop = 2;
explain(costs off, verbose) select avg(a) from smp_test;
explain(costs off, verbose) select bit_and(a) from smp_test;
explain(costs off, verbose) select bit_or(a) from smp_test;
explain(costs off, verbose) select count(a) from smp_test;
explain(costs off, verbose) select count(distinct a) from smp_test;
explain(costs off, verbose) select max(a) from smp_test;
explain(costs off, verbose) select min(a)from smp_test;
explain(costs off, verbose) select stddev(a) from smp_test;
explain(costs off, verbose) select stddev_pop(a) from smp_test;
explain(costs off, verbose) select stddev_samp(a) from smp_test;
explain(costs off, verbose) select sum(a)from smp_test;
explain(costs off, verbose) select var_pop(a) from smp_test;
explain(costs off, verbose) select var_samp(a) from smp_test;
explain(costs off, verbose) select variance(a)from smp_test;
explain(costs off, verbose) select listagg(a) within group(order by a) from smp_test;
explain(costs off, verbose) select listagg(a, ',') within group(order by a) from smp_test;

drop schema tinyint_agg cascade;
reset current_schema;
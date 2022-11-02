drop database if exists uint_agg;
create database uint_agg dbcompatibility 'b';
\c uint_agg

--uint1
create table u1(a uint1, b int2);
insert into u1 values(null, null),(255, 255),(0, 0),(255, 255),(null, null);
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

--uint2
create table u2(a uint2, b int4);
insert into u2 values(null, null),(65535, 65535),(0, 0),(65535, 65535),(null, null);
select avg(a), avg(b) from u2;
select bit_and(a), bit_and(b) from u2;
select bit_or(a), bit_or(b) from u2;
select count(a), count(b) from u2;
select count(distinct a), count(distinct b) from u2;
select max(a), max(b) from u2;
select min(a), min(b) from u2;
select stddev(a), stddev(b) from u2;
select stddev_pop(a), stddev_pop(b) from u2;
select stddev_samp(a), stddev_samp(b) from u2;
select sum(a), sum(b) from u2;
select var_pop(a), var_pop(b) from u2;
select var_samp(a), var_samp(b) from u2;
select variance(a), variance(b) from u2;
select listagg(a) within group(order by a) from u2;
select listagg(a, ',') within group(order by a) from u2;

--uint4
create table u4(a uint4, b int8);
insert into u4 values(null, null),(4294967295, 4294967295),(0, 0),(4294967295, 4294967295),(null, null);
select avg(a), avg(b) from u4;
select bit_and(a), bit_and(b) from u4;
select bit_or(a), bit_or(b) from u4;
select count(a), count(b) from u4;
select count(distinct a), count(distinct b) from u4;
select max(a), max(b) from u4;
select min(a), min(b) from u4;
select stddev(a), stddev(b) from u4;
select stddev_pop(a), stddev_pop(b) from u4;
select stddev_samp(a), stddev_samp(b) from u4;
select sum(a), sum(b) from u4;
select var_pop(a), var_pop(b) from u4;
select var_samp(a), var_samp(b) from u4;
select variance(a), variance(b) from u4;
select listagg(a) within group(order by a) from u4;
select listagg(a, ',') within group(order by a) from u4;

--uint8
create table u8(a uint8, b numeric(64, 0));
insert into u8 values(null, null),(18446744073709551615, 18446744073709551615),(0, 0),(18446744073709551615, 18446744073709551615),(null, null);
select avg(a), avg(b) from u8;
select bit_and(a) from u8;
select bit_or(a) from u8;
select count(a), count(b) from u8;
select count(distinct a), count(distinct b) from u8;
select max(a), max(b) from u8;
select min(a), min(b) from u8;
select stddev(a), stddev(b) from u8;
select stddev_pop(a), stddev_pop(b) from u8;
select stddev_samp(a), stddev_samp(b) from u8;
select sum(a), sum(b) from u8;
select var_pop(a), var_pop(b) from u8;
select var_samp(a), var_samp(b) from u8;
select variance(a), variance(b) from u8;
select listagg(a) within group(order by a) from u8;
select listagg(a, ',') within group(order by a) from u8;

create table smp_test(a uint4, b uint8);
insert into smp_test select generate_series(1, 100000), generate_series(2, 100001);
insert into smp_test select a + 100000, b from smp_test;

set query_dop = 2;
explain(costs off, verbose) select avg(a), avg(b) from smp_test;
explain(costs off, verbose) select bit_and(a), bit_and(b) from smp_test;
explain(costs off, verbose) select bit_or(a), bit_or(b) from smp_test;
explain(costs off, verbose) select count(a), count(b) from smp_test;
explain(costs off, verbose) select count(distinct a), count(distinct b) from smp_test;
explain(costs off, verbose) select max(a), max(b) from smp_test;
explain(costs off, verbose) select min(a), min(b) from smp_test;
explain(costs off, verbose) select stddev(a), stddev(b) from smp_test;
explain(costs off, verbose) select stddev_pop(a), stddev_pop(b) from smp_test;
explain(costs off, verbose) select stddev_samp(a), stddev_samp(b) from smp_test;
explain(costs off, verbose) select sum(a), sum(b) from smp_test;
explain(costs off, verbose) select var_pop(a), var_pop(b) from smp_test;
explain(costs off, verbose) select var_samp(a), var_samp(b) from smp_test;
explain(costs off, verbose) select variance(a), variance(b) from smp_test;
explain(costs off, verbose) select listagg(a) within group(order by a) from smp_test;
explain(costs off, verbose) select listagg(a, ',') within group(order by a) from smp_test;

\c postgres
drop database uint_agg;
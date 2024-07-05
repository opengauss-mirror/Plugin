create schema null_in_partition;
set search_path to null_in_partition;
set dolphin.b_compatibility_mode to on;
create table t_range (c1 int, c2 int) partition by range(c1) (partition p1 values less than (10), partition p2 values less than(maxvalue));
insert into t_range values(null),(5),(100);
create index t_range_c1_idx on t_range (c1) local;
select * from t_range partition(p1); --expected 2
explain (costs off) select /*+ indexscan(t_range) */* from t_range order by c1; -- INDEX
select /*+ indexscan(t_range) */* from t_range order by c1;
explain (costs off) select /*+ indexscan(t_range) */* from t_range order by c1 desc; -- INDEX
select /*+ indexscan(t_range) */* from t_range order by c1 desc;

create table t_range_desc (c1 int, c2 int) partition by range(c1) (partition p1 values less than (10), partition p2 values less than(maxvalue));
insert into t_range_desc values(null),(5),(100);
select * from t_range_desc partition(p1); --expected 2
create index t_range_desc_c1_idx on t_range (c1 desc) local;
explain (costs off) select /*+ indexscan(t_range) */* from t_range_desc order by c1; -- NO INDEX
select /*+ indexscan(t_range) */* from t_range_desc order by c1;
explain (costs off) select /*+ indexscan(t_range) */* from t_range_desc order by c1 desc; -- NO INDEX
select /*+ indexscan(t_range) */* from t_range_desc order by c1 desc;
drop schema null_in_partition cascade;

create schema null_in_partition;
set dolphin.nulls_minimal_policy to off;
create table t_range (c1 int, c2 int) partition by range(c1) (partition p1 values less than (10), partition p2 values less than(maxvalue));
insert into t_range values(null),(5),(100);
create index t_range_ci_idx on t_range (c1) local;
select * from t_range partition(p1); --expected 1
explain (costs off) select /*+ indexscan(t_range) */* from t_range order by c1; -- INDEX
select /*+ indexscan(t_range) */* from t_range order by c1;
explain (costs off) select /*+ indexscan(t_range) */* from t_range order by c1 desc; -- INDEX
select /*+ indexscan(t_range) */* from t_range order by c1 desc;

create table t_range_desc (c1 int, c2 int) partition by range(c1) (partition p1 values less than (10), partition p2 values less than(maxvalue));
insert into t_range_desc values(null),(5),(100);
select * from t_range_desc partition(p1); --expected 1
create index t_range_desc_c1_idx on t_range (c1 desc) local;
explain (costs off) select /*+ indexscan(t_range) */* from t_range_desc order by c1; -- NO INDEX
select /*+ indexscan(t_range) */* from t_range_desc order by c1;
explain (costs off) select /*+ indexscan(t_range) */* from t_range_desc order by c1 desc; -- NO INDEX
select /*+ indexscan(t_range) */* from t_range_desc order by c1 desc;
drop schema null_in_partition cascade;

reset search_path;

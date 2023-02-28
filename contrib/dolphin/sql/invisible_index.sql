create schema invisible_index;
set current_schema to 'invisible_index';

-- create table with index
create table t1 (a int, b int, index key_a(a) visible);
create index key_b on t1 (b) invisible;

select indkey, indisvisible from pg_index where indrelid = 't1'::regclass order by indkey;

insert into t1 values (generate_series(1, 100), generate_series(1, 100));
analyze t1;

set enable_seqscan = off;

-- modify invisible status
explain (costs off) select * from t1 where a < 10;
alter table t1 alter index key_a invisible;
explain (costs off) select * from t1 where a < 10;

explain (costs off) select * from t1 where b < 10;
alter table t1 alter index key_b visible;
explain (costs off) select * from t1 where b < 10;

-- test alter table add constraint with visible
drop index key_b;
alter table t1 add constraint key_b unique (b) visible;
explain (costs off) select * from t1 where b < 10;
alter table t1 alter index key_b invisible;
explain (costs off) select * from t1 where b < 10;

-- test partitioned table
create table t_invisible_0012(col1 int,col2 numeric,col3 text)
partition by hash(col1) (partition p1,partition p2);

create index idx_invisible_0012_01 on t_invisible_0012(col1) local invisible;
create index idx_invisible_0012_02 on t_invisible_0012(col2,col3) global invisible;

insert into t_invisible_0012 values (random()*100,generate_series(1,100),random()*100);
analyze t_invisible_0012;

explain(costs false) select col1 from t_invisible_0012 where col1<70;
alter table t_invisible_0012 alter index idx_invisible_0012_01 visible;
explain(costs false) select col1 from t_invisible_0012 where col1<70;

explain(costs false) select col2,col3 from t_invisible_0012 where col2<37125 and col3>50;
alter table t_invisible_0012 alter index idx_invisible_0012_02 visible;
explain(costs false) select col2,col3 from t_invisible_0012 where col2<37125 and col3>50;

-- test oriented-column table
create table c1 (a int, b int, index ckey_a(a) visible) with (orientation = column);
insert into c1 values (generate_series(1, 100), generate_series(1, 100));
analyze c1;

set enable_seqscan = off;

-- modify invisible status
explain (costs off) select * from c1 where a < 10;
alter table c1 alter index ckey_a invisible;
explain (costs off) select * from c1 where a < 10;

-- test use_invisible_indexes
set dolphin.optimizer_switch = 'use_invisible_indexes=on';
explain (costs off) select * from t1 where a < 10;

set dolphin.optimizer_switch = 123;  --error
set dolphin.optimizer_switch = 'use_invisible_indexes';  --error
set dolphin.optimizer_switch = 'use_invisible_indexes=invalid';  --error
set dolphin.optimizer_switch = 'use_invisible_indexes=on,use_invisible_indexes=off'; --error
set dolphin.optimizer_switch = 'use_invisible_indexes=default';
explain (costs off) select * from t1 where a < 10;

-- test set_var
explain (costs off) select /*+ set_var(dolphin.optimizer_switch = 'use_invisible_indexes=on') */ * from t1 where a < 10;
explain (costs off) select * from t1 where a < 10;
explain (costs off) select /*+ set_var(dolphin.optimizer_switch = 'use_invisible_indexes=off') */ * from t1 where a < 10;
explain (costs off) select /*+ set_var(dolphin.optimizer_switch = 'use_invisible_indexes=default') */ * from t1 where a < 10;
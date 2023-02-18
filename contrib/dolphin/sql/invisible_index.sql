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

set dolphin.optimizer_switch = 'use_invisible_indexes=invalid';  --error
set dolphin.optimizer_switch = 'use_invisible_indexes=on,use_invisible_indexes=off'; --error
set dolphin.optimizer_switch = 'use_invisible_indexes=default';
explain (costs off) select * from t1 where a < 10;

-- test set_var
explain (costs off) select /*+ set_var(dolphin.optimizer_switch = 'use_invisible_indexes=on') */ * from t1 where a < 10;
explain (costs off) select * from t1 where a < 10;
explain (costs off) select /*+ set_var(dolphin.optimizer_switch = 'use_invisible_indexes=off') */ * from t1 where a < 10;
explain (costs off) select /*+ set_var(dolphin.optimizer_switch = 'use_invisible_indexes=default') */ * from t1 where a < 10;
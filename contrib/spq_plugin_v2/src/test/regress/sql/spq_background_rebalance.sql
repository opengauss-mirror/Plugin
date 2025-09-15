----------------------- test 1: add new node && start rebalance -----------------------------
\c - - - :master_port

SET spq.next_shard_id TO 2000000;

SELECT spq_remove_node('localhost', :worker_2_port);

create table t1 (id int primary key, b varchar(10));
insert into t1 values (1, '[1, 2]');
insert into t1 values (2, '[1, 2]');
insert into t1 values (3, '[1, 2]');
insert into t1 values (4, '[1, 2]');
insert into t1 values (5, '[1, 2]');
insert into t1 values (6, '[1, 2]');
insert into t1 values (7, '[1, 2]');
insert into t1 values (8, '[1, 2]');
insert into t1 values (9, '[1, 2]');
insert into t1 values (10, '[1, 2]');
insert into t1 values (11, '[1, 2]');
insert into t1 values (12, '[1, 2]');
insert into t1 values (13, '[1, 2]');
insert into t1 values (14, '[1, 2]');
insert into t1 values (15, '[1, 2]');
insert into t1 values (16, '[1, 2]');
insert into t1 values (17, '[1, 2]');
insert into t1 values (18, '[1, 2]');
insert into t1 values (19, '[1, 2]');
insert into t1 values (20, '[1, 2]');

select create_distributed_table('t1', 'id', shard_count:=10);

select * from spq_shards;

SELECT spq_add_node('localhost', :worker_2_port);

select spq_rebalance_start();

--wait till all tasks done
select pg_sleep(120);

select * from spq_shards;

----------------------- test 2: move shard && rebalance -----------------------------

-- move  3 shards from worker1 to worker2
-- expect error shard already on worker 2
select spq_move_shard_placement(2000000, 'localhost', :worker_1_port, 'localhost', :worker_2_port);

select spq_move_shard_placement(2000005, 'localhost', :worker_1_port, 'localhost', :worker_2_port);

select spq_move_shard_placement(2000006, 'localhost', :worker_1_port, 'localhost', :worker_2_port);

select spq_move_shard_placement(2000007, 'localhost', :worker_1_port, 'localhost', :worker_2_port);

select * from spq_shards;

select spq_rebalance_start();

--wait till all tasks done
select pg_sleep(120);

select * from spq_shards;

drop table t1;
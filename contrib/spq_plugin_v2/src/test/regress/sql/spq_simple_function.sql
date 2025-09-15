-- check the cluster's start state
SELECT count(*) FROM spq_get_active_worker_nodes();

-- I am coordinator
SELECT spq_is_coordinator();

----------------------- TEST 1: test for undistribute_table alter_distributed_table ------------------------------------------
CREATE TABLE t1(id BIGINT PRIMARY KEY, a int, name CHAR(100));
INSERT INTO t1 VALUES(1, 1,'a'), (2, 2,'a'), (3, 3, 'a'), (4, 4, 'a');
SELECT create_distributed_table('t1', 'id', shard_count:=4);
SELECT count(1) FROM pg_dist_shard s JOIN pg_dist_shard_placement p ON s.shardid = p.shardid WHERE s.logicalrelid = 't1'::regclass;

-- error wrong column name
select alter_distributed_table('t1', 'b');

-- error because has primary key constraint on column id
select alter_distributed_table('t1', 'a');
-- error cannot create unique index on non-partition column
create unique index on t1(a);

alter table t1 DROP CONSTRAINT t1_pkey;
-- success alter distribute key
select alter_distributed_table('t1', 'a');

-- success alter shard count
select alter_distributed_table('t1',shard_count:=8);
SELECT count(1) FROM pg_dist_shard s JOIN pg_dist_shard_placement p ON s.shardid = p.shardid WHERE s.logicalrelid = 't1'::regclass;
-- undistribute_table
select undistribute_table('t1');
SELECT count(1) FROM pg_dist_shard s JOIN pg_dist_shard_placement p ON s.shardid = p.shardid WHERE s.logicalrelid = 't1'::regclass;

DROP TABLE t1;

----------------------- TEST 2: test for spq_relation_size spq_table_size spq_total_relation_size ------------------------------------------
CREATE TABLE t1(id BIGINT PRIMARY KEY, name CHAR(100));
SELECT create_distributed_table('t1', 'id', shard_count:=4);
INSERT INTO t1 VALUES(1,'a'), (2,'a'), (3,'a'), (4,'a');

select spq_relation_size('t1');
select spq_table_size('t1');
select spq_total_relation_size('t1');

SELECT logicalrelid,
    pg_size_pretty(spq_relation_size(logicalrelid)) AS size1,
    pg_size_pretty(spq_table_size(logicalrelid)) AS size2,
    pg_size_pretty(spq_total_relation_size(logicalrelid)) AS size3
FROM pg_dist_partition where logicalrelid = 't1'::regclass;

DROP TABLE t1;

----------------------- TEST 3: test for generate as stored column ------------------------------------------
CREATE TABLE t1(id BIGINT PRIMARY KEY, name CHAR(100), fullname TEXT GENERATED ALWAYS AS (name || ' (user)') STORED);
SELECT create_distributed_table('t1', 'id', shard_count:=4);
INSERT INTO t1 VALUES(1,'a'), (2,'b'), (3,'c'), (4,'d');
select id, name, fullname from t1 order by id;

-- error
insert into t1 (id, name, fullname) values (5, 'e', 'full e');

-- error can not distribute by a generate as stored column
select alter_distributed_table('t1', 'fullname');

DROP TABLE t1;

-- check the cluster's final state
SELECT count(*) from spq_get_active_worker_nodes();

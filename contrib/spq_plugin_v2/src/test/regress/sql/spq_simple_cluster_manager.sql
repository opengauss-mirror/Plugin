-- check the cluster's start state
SELECT count(*) FROM spq_get_active_worker_nodes();

-- I am coordinator
SELECT spq_is_coordinator();

--------------------------- TEST 1 : test for remove/add node ----------------------------
-- try to remove a node (with no placements)
SELECT spq_remove_node('localhost', :worker_2_port);
-- verify that the node has been deleted
SELECT count(*) FROM spq_get_active_worker_nodes();

-- verify the data can be only placed in only one node
CREATE TABLE t1(id BIGINT PRIMARY KEY, name CHAR(100));
INSERT INTO t1 VALUES(1,'a'), (2,'a'), (3,'a'), (4,'a');
SELECT create_distributed_table('t1', 'id', shard_count:=4);

SELECT COUNT(DISTINCT(pdsp.nodeport)) FROM pg_dist_shard pds JOIN pg_dist_shard_placement pdsp ON pds.shardid = 
    pdsp.shardid WHERE pds.logicalrelid='t1'::regclass;

DROP TABLE t1;

-- add node
SELECT 1 FROM spq_add_node('localhost', :worker_2_port);

CREATE TABLE t1(id BIGINT PRIMARY KEY, name CHAR(100));
INSERT INTO t1 VALUES(1,'a'), (2,'a'), (3,'a'), (4,'a');
SELECT create_distributed_table('t1', 'id', shard_count:=4);

-- verify the data are placed in 2 nodes
SELECT COUNT(DISTINCT(pdsp.nodeport)) FROM pg_dist_shard pds JOIN pg_dist_shard_placement pdsp ON pds.shardid = 
    pdsp.shardid WHERE pds.logicalrelid='t1'::regclass;

-- try to remove a node (with placements), there is a error emitting
SELECT spq_remove_node('localhost', :worker_2_port);

SELECT count(*) FROM spq_get_active_worker_nodes();
DROP TABLE t1;

--------------------------- TEST 2 : test for activate node ----------------------------
SELECT spq_remove_node('localhost', :worker_1_port);
-- verify that the node has been deleted
SELECT count(*) FROM spq_get_active_worker_nodes();

-- add an inactive node worker 1
SELECT 1 FROM spq_add_inactive_node('localhost', :worker_1_port);
SELECT count(*) FROM spq_get_active_worker_nodes();

-- verify table is not placed in the activate node
CREATE TABLE t1(id BIGINT PRIMARY KEY, name CHAR(100));
INSERT INTO t1 VALUES(1,'a'), (2,'a'), (3,'a'), (4,'a');
SELECT create_distributed_table('t1', 'id', shard_count:=4);

SELECT COUNT(DISTINCT(pdsp.nodeport)) FROM pg_dist_shard pds JOIN pg_dist_shard_placement pdsp ON pds.shardid = 
    pdsp.shardid WHERE pds.logicalrelid='t1'::regclass;

SELECT 1 FROM spq_activate_node('localhost', :worker_1_port);

-- verify table is placed in the activate node
CREATE TABLE t2(id BIGINT PRIMARY KEY, name CHAR(100));
INSERT INTO t2 VALUES(1,'a'), (2,'a'), (3,'a'), (4,'a');
SELECT create_distributed_table('t2', 'id', shard_count:=4);

SELECT COUNT(DISTINCT(pdsp.nodeport)) FROM pg_dist_shard pds JOIN pg_dist_shard_placement pdsp ON pds.shardid = 
    pdsp.shardid WHERE pds.logicalrelid='t2'::regclass;

-- disable(inactivate) node work2;
-- error will be emitted
SELECT 1 FROM spq_disable_node('localhost', :worker_2_port);

DROP TABLE t2;
DROP TABLE t1;
-- succeed to disable node worker1
SELECT 1 FROM spq_disable_node('localhost', :worker_2_port);
SELECT count(*) FROM spq_get_active_worker_nodes();

SELECT 1 FROM spq_activate_node('localhost', :worker_2_port);
SELECT count(*) FROM spq_get_active_worker_nodes();

-------------------------- TEST 3:  check health -------------------------------------
SELECT COUNT(*) FROM spq_check_cluster_nodes();


------------------------- TEST 4: set node property ---------------------------------
CREATE TABLE t1(id BIGINT PRIMARY KEY, name CHAR(100));
INSERT INTO t1 VALUES(1,'a'), (2,'a'), (3,'a'), (4,'a');
SELECT create_distributed_table('t1', 'id', shard_count:=4);

SELECT COUNT(DISTINCT(pdsp.nodeport)) FROM pg_dist_shard pds JOIN pg_dist_shard_placement pdsp ON pds.shardid = 
    pdsp.shardid WHERE pds.logicalrelid='t1'::regclass;

SELECT 1 FROM spq_set_node_property('localhost', :worker_1_port, 'shouldhaveshards', false);

CREATE TABLE t2(id BIGINT PRIMARY KEY, name CHAR(100));
INSERT INTO t2 VALUES(1,'a'), (2,'a'), (3,'a'), (4,'a');
SELECT create_distributed_table('t2', 'id', shard_count:=4);

SELECT COUNT(DISTINCT(pdsp.nodeport)) FROM pg_dist_shard pds JOIN pg_dist_shard_placement pdsp ON pds.shardid = 
    pdsp.shardid WHERE pds.logicalrelid='t2'::regclass;

SELECT 1 FROM spq_set_node_property('localhost', :worker_1_port, 'shouldhaveshards', true);

DROP TABLE t1;
DROP TABLE t2;
-- add more testcases for rebalance ....
SELECT count(*) FROM spq_get_active_worker_nodes();

----------------------- TEST 5: update node ------------------------------------------
SELECT spq_update_node(nodeid, 'localhost', :worker_2_port + 100) FROM pg_dist_node WHERE nodeport = :worker_2_port;
SELECT COUNT(*) FROM pg_dist_node WHERE nodeport = :worker_2_port;

SELECT COUNT(*) FROM pg_dist_node WHERE nodename = 'localhost';
SELECT spq_update_node(nodeid, 'localhost.unknown', :worker_1_port) FROM pg_dist_node WHERE nodeport = :worker_1_port;
SELECT COUNT(*) FROM pg_dist_node WHERE nodename = 'localhost';

SELECT spq_update_node(nodeid, 'localhost', :worker_1_port) FROM pg_dist_node WHERE nodeport = :worker_1_port;
SELECT spq_update_node(nodeid, 'localhost', :worker_2_port) FROM pg_dist_node WHERE nodeport = (:worker_2_port + 100);


SELECT COUNT(*) FROM pg_dist_node WHERE nodeport = :worker_2_port AND nodename = 'localhost';
SELECT COUNT(*) FROM pg_dist_node WHERE nodeport = :worker_1_port AND nodename = 'localhost';

-- check the cluster's final state
SELECT count(*) from spq_get_active_worker_nodes();

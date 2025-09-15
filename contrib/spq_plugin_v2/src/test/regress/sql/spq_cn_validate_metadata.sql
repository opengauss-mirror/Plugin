------------------------ This testcase must run before other testcases ----------------------------
------------------------ validate all spq plugin's internal object --------------------------------
---- TEST 1:  validate all relations -----------------------
SELECT relname FROM pg_class WHERE relname LIKE 'pg_dist%' ORDER BY relname;
\d pg_dist_cleanup;
SELECT * FROM pg_dist_cleanup;
\d pg_dist_colocation;
SELECT * FROM pg_dist_colocation;
\d pg_dist_local_group;
SELECT * FROM pg_dist_local_group;
\d pg_dist_node;
SELECT * FROM pg_dist_node;
\d pg_dist_object;
SELECT * FROM pg_dist_object;
\d pg_dist_partition;
SELECT * FROM pg_dist_partition;
\d pg_dist_placement;
SELECT * FROM pg_dist_placement;
\d pg_dist_shard;
SELECT * FROM pg_dist_shard;
\d pg_dist_transaction;
SELECT * FROM pg_dist_transaction;

SELECT relname FROM pg_class WHERE relname LIKE 'spq%';
\d spq_dist_stat_activity;
\d spq_lock_waits;
\d spq_locks;
\d spq_shard_indexes_on_worker;
\d spq_shards;
\d spq_shards_on_worker;
\d spq_stat_activity;
\d spq_stat_statements;
\d spq_tables;
SELECT application_name FROM spq_dist_stat_activity order by 1;
SELECT * FROM spq_lock_waits;
SELECT * FROM spq_shard_indexes_on_worker;
SELECT * FROM spq_shards;
SELECT * FROM spq_shards_on_worker;
SELECT * FROM spq_tables;
----- TEST 2: validate all sequence ---------------------
\d pg_dist_groupid_seq;
\d pg_dist_cleanup_recordid_seq;
\d pg_dist_colocationid_seq;
\d pg_dist_node_nodeid_seq;
\d pg_dist_operationid_seq;
\d pg_dist_placement_placementid_seq;
\d pg_dist_shardid_seq;

----- TEST 2 : validate all functions --------------------
SELECT proname FROM pg_proc WHERE proname LIKE 'spq%';
SELECT relname FROM pg_class WHERE relname LIKE 'spq%';
SELECT tgname FROM pg_trigger WHERE tgname LIKE 'spq%';
SELECT evtname FROM pg_event_trigger WHERE evtname LIKE 'spq%';

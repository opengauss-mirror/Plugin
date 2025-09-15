\echo Use "CREATE EXTENSION spq" to load this file. \quit

----------------------------------------------------------------------------------
--------------------------- spq install precheck ---------------------------------
----------------------------------------------------------------------------------
#include "spq_precheck/2.0.0.sql"


CREATE SCHEMA __$spq$__;
CREATE SCHEMA IF NOT EXISTS __$spq_internal$__;
SET search_path = 'pg_catalog';
-------------------------------------------------------------------------------------
---------------------------- predefined functions -----------------------------------
-------------------------------------------------------------------------------------
-- spq_version/server_id functions for table pg_dist_node_metadata
#include "udfs/spq_version/2.0.0.sql"

--------------------------------------------------------------------------------------
--------------------------- all spq types definition ---------------------------------
--------------------------------------------------------------------------------------
#include "spq_dd/types/distribution_type/2.0.0.sql"
#include "spq_dd/types/shard_transfer_mode/2.0.0.sql"
#include "spq_dd/types/spq_job_status/2.0.0.sql"
#include "spq_dd/types/spq_task_status/2.0.0.sql"

-- table pg_dist_node
#include "spq_dd/types/noderole/2.0.0.sql"
-- the type needed by read_intermediate_result
#include "spq_system/types/spq_copy_format/2.0.0.sql"

---------------------------------------------------------------------------------------
------------------------------- Spq tables related objects ----------------------------
---------------------------------------------------------------------------------------
#include "spq_dd/tables/pg_dist_partition/2.0.0.sql"
#include "spq_dd/tables/pg_dist_shard/2.0.0.sql"
#include "spq_dd/sequences/pg_dist_shardid_seq/2.0.0.sql"
#include "spq_dd/tables/pg_dist_node/2.0.0.sql"
-- depended DD objects
#include "spq_dd/tables/pg_dist_placement/2.0.0.sql"
#include "spq_dd/views/pg_dist_shard_placement/2.0.0.sql"
#include "spq_dd/tables/pg_dist_rebalance_strategy/2.0.0.sql"
#include "spq_dd/tables/pg_dist_background_job/2.0.0.sql"
#include "spq_dd/tables/pg_dist_background_task/2.0.0.sql"
#include "spq_dd/tables/pg_dist_background_task_depend/2.0.0.sql"
-- end depended DD objects
#include "spq_dd/tables/pg_dist_local_group/2.0.0.sql"
#include "spq_dd/tables/pg_dist_transaction/2.0.0.sql"
#include "spq_dd/tables/pg_dist_colocation/2.0.0.sql"
#include "spq_dd/sequences/pg_dist_colocationid_seq/2.0.0.sql"

#include "spq_dd/tables/pg_dist_node_metadata/2.0.0.sql"
#include "spq_dd/tables/pg_dist_object/2.0.0.sql"

-- FIXME: need by append table's truncate, do we need this table in current version?
#include "spq_dd/tables/pg_dist_cleanup/2.0.0.sql"

----------------------------------------------------------------------------------------
------------------------------ Utility views -------------------------------------------
----------------------------------------------------------------------------------------
-- view spq_shards and related functions
#include "udfs/spq_shard_sizes/2.0.0.sql"
#include "udfs/shard_name/2.0.0.sql"
#include "spq_system/views/spq_shards/2.0.0.sql"

-- view spq_stat_activity & spq_dist_stat_activity
#include "spq_system/views/spq_stat_activity/2.0.0.sql"
#include "spq_system/views/spq_dist_stat_activity/2.0.0.sql"

-- view spq_locks and related function
#include "udfs/get_all_active_transactions/2.0.0.sql"
#include "spq_system/views/spq_locks/2.0.0.sql"

-- view spq_lock_waits and related functions
#include "udfs/spq_calculate_gpid/2.0.0.sql"
#include "udfs/spq_internal_global_blocked_processes/2.0.0.sql"
#include "spq_system/views/spq_lock_waits/2.0.0.sql"

-- view spq_shards_on_worker & spq_shard_indexes_on_worker and related function
#include "udfs/relation_is_a_known_shard/2.0.0.sql"
#include "spq_system/views/spq_shards_on_worker/2.0.0.sql"
#include "spq_system/views/spq_shard_indexes_on_worker/2.0.0.sql"

-- view stat statements and related functions
#include "udfs/spq_executor_name/2.0.0.sql"
#include "udfs/spq_query_stats/2.0.0.sql"
#include "udfs/spq_stat_statements/2.0.0.sql"
#include "udfs/spq_stat_statements_reset/2.0.0.sql"
#include "spq_system/views/spq_stat_statements/2.0.0.sql"

-- view spq_tables and related function
#include "udfs/column_to_column_name/2.0.0.sql"
#include "spq_system/views/spq_tables/2.0.0.sql"

-----------------------------------------------------------------------------------------
----------------------------- Spq utility functions -------------------------------------
-----------------------------------------------------------------------------------------
--------------------------------- node operator ----------------------------------------
#include "udfs/spq_get_active_worker_nodes/2.0.0.sql"
#include "udfs/spq_update_node/2.0.0.sql"
#include "udfs/spq_add_node/2.0.0.sql"
#include "udfs/spq_add_inactive_node/2.0.0.sql"
#include "udfs/spq_activate_node/2.0.0.sql"
#include "udfs/spq_disable_node/2.0.0.sql"
#include "udfs/spq_check_cluster_nodes/2.0.0.sql"
#include "udfs/spq_remove_node/2.0.0.sql"
#include "udfs/spq_set_coordinator_host/2.0.0.sql"
#include "udfs/spq_is_coordinator/2.0.0.sql"
#include "udfs/spq_check_connection_to_node/2.0.0.sql"
#include "udfs/spq_set_node_property/2.0.0.sql"

---------------------------- table operator --------------------------------------------
#include "udfs/create_distributed_table/2.0.0.sql"
#include "udfs/alter_distributed_table/2.0.0.sql"
#include "udfs/truncate_local_data_after_distributing_table/2.0.0.sql"
#include "udfs/undistribute_table/2.0.0.sql"

-- drop table related functions
#include "udfs/worker_drop_distributed_table/2.0.0.sql"
#include "udfs/master_remove_distributed_table_metadata_from_workers/2.0.0.sql"
#include "udfs/spq_drop_all_shards/2.0.0.sql"
#include "udfs/master_remove_partition_metadata/2.0.0.sql"
#include "udfs/spq_unmark_object_distributed/2.0.0.sql"
#include "udfs/notify_constraint_dropped/2.0.0.sql"
#include "udfs/spq_drop_trigger/2.0.0.sql"
CREATE EVENT TRIGGER spq_cascade_to_partition
   ON SQL_DROP
   EXECUTE PROCEDURE spq_drop_trigger();

-- truncate table stmt related
#include "udfs/spq_truncate_trigger/2.0.0.sql"

----------------------------------- transaction and locks -----------------------------
#include "udfs/assign_distributed_transaction_id/2.0.0.sql"
#include "udfs/recover_prepared_transactions/2.0.0.sql"
#include "udfs/lock_shard_resources/2.0.0.sql"
#include "udfs/lock_shard_metadata/2.0.0.sql"
#include "udfs/lock_relation_if_exists/2.0.0.sql"
#include "udfs/get_current_transaction_id/2.0.0.sql"
#include "udfs/check_distributed_deadlocks/2.0.0.sql"
#include "udfs/dump_local_wait_edges/2.0.0.sql"
#include "udfs/dump_global_wait_edges/2.0.0.sql"
#include "udfs/get_global_active_transactions/2.0.0.sql"
#include "udfs/spq_internal_local_blocked_processes/2.0.0.sql"
#include "udfs/spq_blocking_pids/2.0.0.sql"
#include "udfs/spq_backend_gpid/2.0.0.sql"

----------------------------- backup&restore ------------------------------------------
#include "udfs/spq_create_restore_point/2.0.0.sql"

------------------------------  Query: intermediate result   --------------------------
#include "udfs/create_intermediate_result/2.0.0.sql"
#include "udfs/broadcast_intermediate_result/2.0.0.sql"
#include "udfs/read_intermediate_result/2.0.0.sql"
#include "udfs/read_intermediate_results/2.0.0.sql"
#include "udfs/fetch_intermediate_results/2.0.0.sql"
#include "udfs/worker_partition_query_result/2.0.0.sql"

-------------------- DDL internal utility(for shard table) ----------------------------
#include "udfs/worker_apply_shard_ddl_command/2.0.0.sql"
#include "udfs/worker_apply_inter_shard_ddl_command/2.0.0.sql"
#include "udfs/worker_create_truncate_trigger/2.0.0.sql"

-------------- functions for running commands on workers and shards-------------------
#include "udfs/master_run_on_worker/2.0.0.sql"
#include "udfs/run_command_on_workers/2.0.0.sql"
#include "udfs/spq_run_local_command/2.0.0.sql"
#include "udfs/run_command_on_all_nodes/2.0.0.sql"

-------------------------  function aggs for multi-shard agg -------------------------
#include "udfs/any_value/2.0.0.sql"
#include "spq_system/aggregates/array_cat_agg/2.0.0.sql"
#include "spq_system/aggregates/json_cat_agg/2.0.0.sql"

---------------------------------- Copy utils ---------------------------------------
#include "udfs/spq_text_send_as_jsonb/2.0.0.sql"

---------------------------------- Role utils --------------------------------------
#include "udfs/alter_role_if_exists/2.0.0.sql"
#include "udfs/worker_create_or_alter_role/2.0.0.sql"

----------------------------- some pg util functions -------------------------------
#include "udfs/utils/2.0.0.sql"
GRANT USAGE ON SCHEMA __$spq$__ TO public;
#include "exec/ensure_truncate_trigger_is_after/2.0.0.sql"
REVOKE USAGE ON SCHEMA __$spq$__ FROM public;

---------------------------- placeholder function ---------------------------------
#include "udfs/spq_extradata_container/2.0.0.sql"

---------------------------- connection stat --------------------------------------
#include "udfs/spq_remote_connection_stats/2.0.0.sql"

--------------------------- explain utils ----------------------------------------
#include "udfs/worker_last_saved_explain_analyze/2.0.0.sql"
#include "udfs/worker_save_query_explain_analyze/2.0.0.sql"

----------------------------- table statistics ----------------------------------
#include "udfs/spq_table_size/2.0.0.sql"

--------------------------- distributed object utils ---------------------------
#include "udfs/worker_create_or_replace_object/2.0.0.sql"

#include "udfs/spq_nodeid_for_gpid/2.0.0.sql"
#include "udfs/spq_pid_for_gpid/2.0.0.sql"

-- Table of records to:
-- 1) Cleanup leftover resources after a failure
-- 2) Deferred drop of old shard placements after a split.
#include "udfs/spq_cleanup_orphaned_resources/2.0.0.sql"

--------------------------- table rebalance ---------------------------
#include "udfs/rebalance_table_shards/2.0.0.sql"
#include "udfs/spq_move_shard_placement/2.0.0.sql"
#include "udfs/spq_local_disk_space_stats/2.0.0.sql"
#include "udfs/worker_copy_table_to_node/2.0.0.sql"
#include "udfs/spq_rebalance_start/2.0.0.sql"

----------------------------------- test ----------------------------------------------
#include "udfs/isolation_tester/2.0.0.sql"
----------------------------- End of SPQ ---------------------------------------------
RESET search_path;

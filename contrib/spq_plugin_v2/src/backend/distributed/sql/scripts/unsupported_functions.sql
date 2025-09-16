-- do we need this?
--#include "exec/spq_setup_ssl/2.0.0.sql"
#include "udfs/create_distributed_table_concurrently/2.0.0.sql"
#include "udfs/create_reference_table/2.0.0.sql"
#include "udfs/create_distributed_function/11.0-1.sql"

--
-- distributed_tables_colocated returns true if given tables are co-located, false otherwise.
-- The function checks shard definitions, matches shard placements for given tables.
--
#include "udfs/distributed_tables_colocated/2.0.0.sql"
#include "udfs/update_distributed_table_colocation/9.3-2.sql"
#include "udfs/alter_table_set_access_method/10.0-1.sql"
#include "udfs/alter_old_partitions_set_access_method/10.0-1.sql"
#include "udfs/remove_local_tables_from_metadata/10.0-1.sql"
#include "udfs/get_colocated_shard_array/2.0.0.sql"

-- do we need it ?
--#include "exec/spq_check_defaults_for_sslmode/2.0.0.sql"

-- master_* functions
#include "udfs/master_get_table_ddl_events/2.0.0.sql"
#include "udfs/master_get_new_shardid/2.0.0.sql"
#include "udfs/master_create_empty_shard/2.0.0.sql"
#include "udfs/master_get_new_placementid/2.0.0.sql"

#include "udfs/column_name_to_column/2.0.0.sql"
#include "udfs/get_shard_id_for_distribution_column/2.0.0.sql"

-- for glabal sequece
#include "udfs/worker_nextval/10.2-1.sql"
#include "udfs/worker_apply_sequence_command/2.0.0.sql"
#include "udfs/worker_record_sequence_dependency/9.5-1.sql"
#include "udfs/worker_change_sequence_dependency/10.0-1.sql"
#include "udfs/worker_drop_sequence_dependency/11.0-1.sql"

-- statistics
#include "udfs/spq_update_shard_statistics/2.0.0.sql"

-- for mx isolation
#include "udfs/worker_hash/2.0.0.sql"

#include "udfs/spq_table_is_visible/2.0.0.sql"

-- shard move
#include "udfs/spq_local_disk_space_stats/2.0.0.sql"
#include "udfs/worker_split_copy/11.1-1.sql"
#include "udfs/worker_copy_table_to_node/11.1-1.sql"

-- old definition. By recreating it here upgrades also pick up the new changes.
#include "udfs/pg_terminate_backend/11.0-1.sql"

#include "udfs/spq_add_secondary_node/2.0.0.sql"

#include "udfs/worker_drop_shell_table/2.0.0.sql"
#include "udfs/worker_drop_all_shell_tables/2.0.0.sql"

#include "udfs/worker_partitioned_relation_total_size/2.0.0.sql"
#include "udfs/worker_partitioned_relation_size/2.0.0.sql"
#include "udfs/worker_partitioned_table_size/2.0.0.sql"
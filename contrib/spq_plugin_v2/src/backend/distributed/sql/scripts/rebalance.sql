-- for rebalance types
#include "spq_dd/types/shard_transfer_mode/2.0.0.sql"

-- pg_dist_background_job
#include "spq_dd/types/spq_job_status/2.0.0.sql"
-- pg_dist_background_task
#include "spq_dd/types/spq_task_status/2.0.0.sql"

-- for rebalance dd tables
#include "spq_dd/tables/pg_dist_rebalance_strategy/2.0.0.sql"
#include "spq_dd/tables/pg_dist_background_job/2.0.0.sql"
#include "spq_dd/tables/pg_dist_background_task/2.0.0.sql"
#include "spq_dd/tables/pg_dist_background_task_depend/2.0.0.sql"

-- util functions
#include "udfs/get_rebalance_table_shards_plan/10.1-1.sql"
#include "udfs/replicate_table_shards/9.0-1.sql"
#include "udfs/replicate_reference_tables/11.1-1.sql"
#include "udfs/rebalance_table_shards/9.2-1.sql"
#include "udfs/get_rebalance_progress/11.2-1.sql"

#include "udfs/citus_add_rebalance_strategy/2.0.0.sql"
#include "udfs/citus_set_default_rebalance_strategy/9.2-1.sql"

-- rebalance is disabled
--#include "udfs/citus_drain_node/10.0-1.sql"
-- end rebalance

#include "udfs/citus_job_wait/11.1-1.sql"
#include "udfs/citus_job_cancel/11.1-1.sql"
#include "udfs/spq_rebalance_start/11.1-1.sql"
#include "udfs/citus_rebalance_stop/11.1-1.sql"
#include "udfs/citus_rebalance_wait/11.1-1.sql"
#include "udfs/citus_job_list/11.2-1.sql"
--@FIXME
--#include "udfs/citus_job_status/11.2-1.sql"
--#include "udfs/citus_rebalance_status/11.2-1.sql"
#include "udfs/citus_task_wait/11.2-1.sql"

#include "udfs/citus_move_shard_placement/11.2-1.sql"
#include "udfs/citus_copy_shard_placement/2.0.0.sql"
#include "udfs/citus_split_shard_by_split_points/11.1-1.sql"
-- Update the default rebalance strategy to 'by_disk_size', but only if the
-- default is currently 'by_shard_count'
SELECT citus_set_default_rebalance_strategy(name)
  FROM pg_dist_rebalance_strategy
  WHERE name = 'by_disk_size'
      AND (SELECT default_strategy FROM pg_dist_rebalance_strategy WHERE name = 'by_shard_count');

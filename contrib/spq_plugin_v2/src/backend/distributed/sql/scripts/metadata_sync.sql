#include "udfs/citus_internal_add_partition_metadata/10.2-1.sql";
#include "udfs/citus_internal_add_shard_metadata/10.2-1.sql";
#include "udfs/citus_internal_add_placement_metadata/11.2-1.sql";
#include "udfs/citus_internal_update_placement_metadata/10.2-1.sql";
#include "udfs/citus_internal_delete_shard_metadata/10.2-1.sql";
#include "udfs/citus_internal_update_relation_colocation/10.2-1.sql";
#include "udfs/citus_internal_add_object_metadata/11.0-1.sql"
#include "udfs/citus_internal_add_colocation_metadata/11.0-1.sql"
#include "udfs/citus_internal_delete_colocation_metadata/11.0-1.sql"
#include "udfs/citus_internal_delete_partition_metadata/11.1-1.sql"
#include "udfs/citus_internal_mark_node_not_synced/11.3-1.sql"
#include "udfs/citus_internal_update_none_dist_table_metadata/12.1-1.sql"
#include "udfs/citus_internal_delete_placement_metadata/12.1-1.sql"

#include "udfs/start_metadata_sync_to_node/2.0.0.sql"
#include "udfs/stop_metadata_sync_to_node/2.0.0.sql"
#include "udfs/start_metadata_sync_to_all_nodes/11.0-2.sql"

#include "udfs/worker_adjust_identity_column_seq_ranges/11.2-2.sql"

CREATE TABLE __$spq$__.pg_dist_rebalance_strategy(
    name name NOT NULL,
    default_strategy boolean NOT NULL DEFAULT false,
    shard_cost_function regproc NOT NULL,
    node_capacity_function regproc NOT NULL,
    shard_allowed_on_node_function regproc NOT NULL,
    default_threshold float4 NOT NULL,
    minimum_threshold float4 NOT NULL DEFAULT 0,
    improvement_threshold float4 NOT NULL default 0,
    UNIQUE(name)
);
ALTER TABLE __$spq$__.pg_dist_rebalance_strategy SET SCHEMA pg_catalog;
GRANT SELECT ON pg_catalog.pg_dist_rebalance_strategy TO public;

#include "../../../udfs/citus_validate_rebalance_strategy_functions/9.2-1.sql"
#include "../../../udfs/pg_dist_rebalance_strategy_trigger_func/9.2-1.sql"
CREATE TRIGGER pg_dist_rebalance_strategy_validation_trigger
  BEFORE INSERT OR UPDATE ON pg_dist_rebalance_strategy
  FOR EACH ROW EXECUTE PROCEDURE __$spq_internal$__.pg_dist_rebalance_strategy_trigger_func();

#include "../../../udfs/citus_shard_cost_1/9.2-1.sql"
#include "../../../udfs/citus_node_capacity_1/9.2-1.sql"
#include "../../../udfs/citus_shard_allowed_on_node_true/9.2-1.sql"
#include "../../../udfs/citus_shard_cost_by_disk_size/9.2-1.sql"

INSERT INTO
    pg_catalog.pg_dist_rebalance_strategy(
        name,
        default_strategy,
        shard_cost_function,
        node_capacity_function,
        shard_allowed_on_node_function,
        default_threshold,
        minimum_threshold,
	improvement_threshold
    ) VALUES (
        'by_shard_count',
        true,
        'citus_shard_cost_1',
        'citus_node_capacity_1',
        'citus_shard_allowed_on_node_true',
        0,
        0,
	0
    ), (
        'by_disk_size',
        false,
        'citus_shard_cost_by_disk_size',
        'citus_node_capacity_1',
        'citus_shard_allowed_on_node_true',
        0.1,
        0.01,
	0.5
    );

ALTER TABLE pg_catalog.pg_dist_rebalance_strategy REPLICA IDENTITY USING INDEX pg_dist_rebalance_strategy_name_key;

CREATE OR REPLACE FUNCTION pg_catalog.spq_rebalance_start(
        rebalance_strategy name DEFAULT NULL,
        drain_only boolean DEFAULT false,
        shard_transfer_mode __$spq$__.shard_transfer_mode default 'auto'
    )
    RETURNS bigint
    AS 'MODULE_PATHNAME'
    LANGUAGE C VOLATILE;
COMMENT ON FUNCTION pg_catalog.spq_rebalance_start(name, boolean, __$spq$__.shard_transfer_mode)
    IS 'rebalance the shards in the cluster in the background';
GRANT EXECUTE ON FUNCTION pg_catalog.spq_rebalance_start(name, boolean, __$spq$__.shard_transfer_mode) TO PUBLIC;

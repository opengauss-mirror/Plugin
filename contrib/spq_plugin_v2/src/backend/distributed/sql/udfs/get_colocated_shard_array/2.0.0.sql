CREATE OR REPLACE FUNCTION get_colocated_shard_array(bigint)
 RETURNS BIGINT[]
 LANGUAGE C STRICT
 AS 'MODULE_PATHNAME', $$get_colocated_shard_array$$;
COMMENT ON FUNCTION get_colocated_shard_array(bigint)
 IS 'returns the array of colocated shards of the given shard';


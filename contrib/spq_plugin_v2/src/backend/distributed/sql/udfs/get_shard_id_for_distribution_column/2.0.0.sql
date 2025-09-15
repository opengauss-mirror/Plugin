CREATE FUNCTION get_shard_id_for_distribution_column(table_name regclass, distribution_value "any" DEFAULT NULL)
 RETURNS bigint
 LANGUAGE C
 AS 'MODULE_PATHNAME', $$get_shard_id_for_distribution_column$$;
COMMENT ON FUNCTION get_shard_id_for_distribution_column(table_name regclass, distribution_value "any")
    IS 'return shard id which belongs to given table and contains given value';

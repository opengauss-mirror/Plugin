CREATE OR REPLACE FUNCTION create_distributed_table(table_name regclass,
                                                    distribution_column text,
                                                    distribution_type __$spq$__.distribution_type DEFAULT 'hash',
                                                    shard_count int DEFAULT NULL)
    RETURNS void
    LANGUAGE C
    AS 'MODULE_PATHNAME', $$create_distributed_table$$;
COMMENT ON FUNCTION create_distributed_table(table_name regclass,
											 distribution_column text,
											 distribution_type __$spq$__.distribution_type,
                                             shard_count int)
    IS 'creates a distributed table';


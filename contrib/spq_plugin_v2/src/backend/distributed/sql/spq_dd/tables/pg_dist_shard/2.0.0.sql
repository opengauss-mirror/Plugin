-- add pg_dist_shard
CREATE TABLE __$spq$__.pg_dist_shard(
    logicalrelid regclass NOT NULL,
    shardid int8 NOT NULL,
    shardstorage "char" NOT NULL,
    shardalias text,
    shardminvalue text,
    shardmaxvalue text
);
ALTER TABLE __$spq$__.pg_dist_shard DROP shardalias;
CREATE UNIQUE INDEX pg_dist_shard_shardid_index
    ON __$spq$__.pg_dist_shard using btree(shardid);
CREATE INDEX pg_dist_shard_logical_relid_index
    ON __$spq$__.pg_dist_shard using btree(logicalrelid);
ALTER TABLE __$spq$__.pg_dist_shard SET SCHEMA pg_catalog;
GRANT SELECT ON pg_catalog.pg_dist_shard TO public;
ALTER TABLE pg_catalog.pg_dist_shard REPLICA IDENTITY USING INDEX pg_dist_shard_shardid_index;

#include "../../../udfs/spq_dist_shard_cache_invalidate/2.0.0.sql"
CREATE TRIGGER dist_shard_cache_invalidate
    AFTER INSERT OR UPDATE OR DELETE
    ON pg_catalog.pg_dist_shard
    FOR EACH ROW EXECUTE PROCEDURE spq_dist_shard_cache_invalidate();

-- add pg_dist_partition
CREATE TABLE __$spq$__.pg_dist_partition(
    logicalrelid regclass NOT NULL,
    partmethod "char" NOT NULL,
    partkey text,
    colocationid integer DEFAULT 0 NOT NULL,
    repmodel "char" DEFAULT 'c' NOT NULL,
    autoconverted boolean DEFAULT false
);

CREATE UNIQUE INDEX pg_dist_partition_logical_relid_index
    ON __$spq$__.pg_dist_partition using btree(logicalrelid);
ALTER TABLE __$spq$__.pg_dist_partition SET SCHEMA pg_catalog;
CREATE INDEX pg_dist_partition_colocationid_index
    ON pg_catalog.pg_dist_partition using btree(colocationid);
ALTER TABLE pg_catalog.pg_dist_partition REPLICA IDENTITY USING INDEX pg_dist_partition_logical_relid_index;
GRANT SELECT ON pg_catalog.pg_dist_partition TO public;

#include "../../../udfs/spq_dist_partition_cache_invalidate/2.0.0.sql"
CREATE TRIGGER dist_partition_cache_invalidate
    AFTER INSERT OR UPDATE OR DELETE
    ON pg_catalog.pg_dist_partition
    FOR EACH ROW EXECUTE PROCEDURE spq_dist_partition_cache_invalidate();

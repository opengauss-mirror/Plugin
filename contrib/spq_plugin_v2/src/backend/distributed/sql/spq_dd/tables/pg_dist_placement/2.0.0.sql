-- add pg_dist_placement
CREATE SEQUENCE pg_catalog.pg_dist_placement_placementid_seq NO CYCLE;
CREATE TABLE __$spq$__.pg_dist_placement (
  placementid BIGINT NOT NULL default nextval('pg_dist_placement_placementid_seq'::regclass),
  shardid BIGINT NOT NULL,
  shardstate INT NOT NULL,
  shardlength BIGINT NOT NULL,
  groupid INT NOT NULL
);
ALTER TABLE __$spq$__.pg_dist_placement SET SCHEMA pg_catalog;
GRANT SELECT ON pg_catalog.pg_dist_placement TO public;
CREATE INDEX pg_dist_placement_groupid_index
  ON pg_dist_placement USING btree(groupid);
CREATE INDEX pg_dist_placement_shardid_index
  ON pg_dist_placement USING btree(shardid);
CREATE UNIQUE INDEX pg_dist_placement_placementid_index
  ON pg_dist_placement USING btree(placementid);
-- the same shard cannot have placements on different nodes
ALTER TABLE pg_catalog.pg_dist_placement ADD CONSTRAINT placement_shardid_groupid_unique_index UNIQUE (shardid, groupid);
ALTER TABLE pg_catalog.pg_dist_placement REPLICA IDENTITY USING INDEX pg_dist_placement_placementid_index;

#include "../../../udfs/spq_dist_placement_cache_invalidate/2.0.0.sql"

CREATE TRIGGER dist_placement_cache_invalidate
    AFTER INSERT OR UPDATE OR DELETE
    ON pg_catalog.pg_dist_placement
    FOR EACH ROW EXECUTE PROCEDURE spq_dist_placement_cache_invalidate();

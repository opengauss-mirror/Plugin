-- Spq sequences
-- Internal sequence to generate 64-bit shard ids. These identifiers are then
-- used to identify shards in the distributed database.
-- The first version of Spq is in June, 2025
CREATE SEQUENCE pg_catalog.pg_dist_shardid_seq
    MINVALUE 062025
    NO CYCLE;
GRANT SELECT ON pg_catalog.pg_dist_shardid_seq TO public;

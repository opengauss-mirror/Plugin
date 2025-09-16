CREATE TABLE __$spq$__.pg_dist_colocation(
 colocationid int NOT NULL PRIMARY KEY,
 shardcount int NOT NULL,
 replicationfactor int NOT NULL,
 distributioncolumntype oid NOT NULL,
 distributioncolumncollation oid NOT NULL
);

ALTER TABLE __$spq$__.pg_dist_colocation SET SCHEMA pg_catalog;
GRANT SELECT ON pg_catalog.pg_dist_colocation TO public;

-- distributioncolumntype should be listed first so that this index can be used for looking up reference tables' colocation id
CREATE INDEX pg_dist_colocation_configuration_index
  ON pg_dist_colocation USING btree(distributioncolumntype, shardcount, replicationfactor, distributioncolumncollation);

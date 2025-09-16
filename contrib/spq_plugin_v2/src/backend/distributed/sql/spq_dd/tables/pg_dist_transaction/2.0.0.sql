CREATE TABLE __$spq$__.pg_dist_transaction (
    groupid int NOT NULL,
    gid text NOT NULL
);
CREATE INDEX pg_dist_transaction_group_index
  ON __$spq$__.pg_dist_transaction using btree(groupid);
ALTER TABLE __$spq$__.pg_dist_transaction SET SCHEMA pg_catalog;
ALTER TABLE pg_catalog.pg_dist_transaction
ADD CONSTRAINT pg_dist_transaction_unique_constraint UNIQUE (groupid, gid);
GRANT SELECT ON pg_catalog.pg_dist_transaction TO public;
ALTER TABLE pg_catalog.pg_dist_transaction REPLICA IDENTITY USING INDEX pg_dist_transaction_unique_constraint;

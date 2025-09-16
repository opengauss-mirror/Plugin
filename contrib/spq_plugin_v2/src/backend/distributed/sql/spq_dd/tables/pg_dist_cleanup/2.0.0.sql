CREATE TABLE __$spq$__.pg_dist_cleanup (
    record_id bigint primary key,
    operation_id bigint not null,
    object_type int not null,
    object_name text not null,
    node_group_id int not null,
    policy_type int not null
);
ALTER TABLE __$spq$__.pg_dist_cleanup SET SCHEMA pg_catalog;
GRANT SELECT ON pg_catalog.pg_dist_cleanup TO public;

-- Sequence used to generate operation Ids and record Ids in pg_dist_cleanup_record.
CREATE SEQUENCE pg_catalog.pg_dist_operationid_seq;
GRANT SELECT ON pg_catalog.pg_dist_operationid_seq TO public;

CREATE SEQUENCE pg_catalog.pg_dist_cleanup_recordid_seq;
GRANT SELECT ON pg_catalog.pg_dist_cleanup_recordid_seq TO public;

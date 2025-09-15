CREATE TABLE __$spq$__.pg_dist_node_metadata(
    metadata jsonb NOT NULL
);
ALTER TABLE __$spq$__.pg_dist_node_metadata SET SCHEMA pg_catalog;

GRANT SELECT ON pg_catalog.pg_dist_node_metadata TO public;
-- Insert the latest extension version into pg_dist_node_metadata
-- for new installations.
INSERT INTO pg_dist_node_metadata
  SELECT concat(concat('{"server_id":', concat(concat('"', spq_server_id()::text), '"')),
	',' , '"last_upgrade_version"', ':', concat('"', default_version, '"'),'}')::jsonb
    FROM pg_available_extensions WHERE name = 'spq';

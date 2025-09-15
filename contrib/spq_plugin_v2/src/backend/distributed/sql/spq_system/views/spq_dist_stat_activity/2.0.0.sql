DROP VIEW IF EXISTS pg_catalog.spq_dist_stat_activity;

CREATE OR REPLACE VIEW __$spq$__.spq_dist_stat_activity AS
  SELECT * FROM spq_stat_activity WHERE is_worker_query = false;

ALTER VIEW __$spq$__.spq_dist_stat_activity SET SCHEMA pg_catalog;
GRANT SELECT ON pg_catalog.spq_dist_stat_activity TO PUBLIC;

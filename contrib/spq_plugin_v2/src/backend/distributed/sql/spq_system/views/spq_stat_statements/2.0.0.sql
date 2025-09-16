CREATE VIEW __$spq$__.spq_stat_statements AS
SELECT
  unique_sql_id,
  user_id,
  node_id,
  query,
  pg_catalog.spq_executor_name(executor::int) AS executor,
  partition_key,
  calls
FROM pg_catalog.spq_stat_statements();
ALTER VIEW __$spq$__.spq_stat_statements SET SCHEMA pg_catalog;
GRANT SELECT ON pg_catalog.spq_stat_statements TO public;

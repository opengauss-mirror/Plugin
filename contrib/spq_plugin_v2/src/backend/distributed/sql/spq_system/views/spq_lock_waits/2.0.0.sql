CREATE OR REPLACE FUNCTION get_nodeid_for_groupid(groupIdInput int) RETURNS int AS $$
DECLARE
	returnNodeNodeId int := 0;
begin
	SELECT nodeId into returnNodeNodeId FROM pg_dist_node WHERE groupid = groupIdInput and nodecluster = current_setting('spq.cluster_name');
	RETURN returnNodeNodeId;
end
$$ LANGUAGE plpgsql;

CREATE VIEW __$spq$__.spq_lock_waits AS
WITH
coordinator_check AS (
  SELECT (COUNT(*) > 0) AS has_dist_nodes FROM pg_dist_node
),
unique_global_wait_edges_with_calculated_gpids AS (
SELECT
			-- if global_pid is NULL, it is most likely that a backend is blocked on a DDL
			-- also for legacy reasons spq_internal_global_blocked_processes() returns groupId, we replace that with nodeIds
			case WHEN waiting_global_pid  !=0 THEN waiting_global_pid   ELSE spq_calculate_gpid(get_nodeid_for_groupid(waiting_node_id),  waiting_pid)  END waiting_global_pid,
			case WHEN blocking_global_pid !=0 THEN blocking_global_pid  ELSE spq_calculate_gpid(get_nodeid_for_groupid(blocking_node_id), blocking_pid) END blocking_global_pid,

			-- spq_internal_global_blocked_processes returns groupId, we replace it here with actual
			-- nodeId to be consisten with the other views
			get_nodeid_for_groupid(blocking_node_id) as blocking_node_id,
			get_nodeid_for_groupid(waiting_node_id) as waiting_node_id,

			blocking_transaction_waiting

			FROM (select * from spq_internal_global_blocked_processes() where (select has_dist_nodes from coordinator_check)) t
),
unique_global_wait_edges AS
(
	SELECT DISTINCT ON(waiting_global_pid, blocking_global_pid) * FROM unique_global_wait_edges_with_calculated_gpids
),
citus_dist_stat_activity_with_calculated_gpids AS
(
	-- if global_pid is NULL, it is most likely that a backend is blocked on a DDL
	SELECT CASE WHEN global_pid != 0 THEN global_pid ELSE spq_calculate_gpid(nodeid, b.lwpid) END global_pid, nodeid, a.pid, query FROM spq_dist_stat_activity a join pg_os_threads b on a.pid=b.pid
)
SELECT
	waiting.global_pid as waiting_gpid,
	blocking.global_pid as blocking_gpid,
	waiting.query AS blocked_statement,
	blocking.query AS current_statement_in_blocking_process,
	waiting.nodeid AS waiting_nodeid,
	blocking.nodeid AS blocking_nodeid
FROM
	unique_global_wait_edges
		JOIN
	citus_dist_stat_activity_with_calculated_gpids waiting ON (unique_global_wait_edges.waiting_global_pid = waiting.global_pid)
		JOIN
	citus_dist_stat_activity_with_calculated_gpids blocking ON (unique_global_wait_edges.blocking_global_pid = blocking.global_pid);

ALTER VIEW __$spq$__.spq_lock_waits SET SCHEMA pg_catalog;
GRANT SELECT ON pg_catalog.spq_lock_waits TO PUBLIC;

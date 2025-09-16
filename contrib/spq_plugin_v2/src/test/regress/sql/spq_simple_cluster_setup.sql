-- Tests functions related to cluster membership
-- add the first node to the cluster in transactional mode
SELECT 1 FROM spq_add_node('localhost', :worker_1_port);
SELECT 1 FROM spq_add_node('localhost', :worker_2_port);

-- I am coordinator
SELECT spq_is_coordinator();
-- make sure coordinator is always in metadata.
SELECT spq_set_coordinator_host('localhost');

SELECT COUNT(*) FROM spq_get_active_worker_nodes();

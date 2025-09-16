CREATE OR REPLACE FUNCTION pg_catalog.spq_stat_statements(OUT unique_sql_id bigint,
												 OUT user_id oid,
												 OUT node_id int,
												 OUT query text,
												 OUT executor bigint,
												 OUT partition_key text,
												 OUT calls bigint)
RETURNS SETOF record
LANGUAGE plpgsql
AS $spq_stat_statements$
BEGIN

 	RETURN QUERY SELECT pss.unique_sql_id, pss.user_id, pss.node_id, pss.query, cqs.executor,
 						cqs.partition_key, cqs.calls
 				 FROM dbe_perf.statement pss
 				 	JOIN spq_query_stats() cqs
 				 	USING (unique_sql_id, user_id, node_id);
END;
$spq_stat_statements$;

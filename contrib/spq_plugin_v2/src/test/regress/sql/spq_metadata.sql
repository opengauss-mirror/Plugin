create schema metadate_test;
select reset_unique_sql('GLOBAL', 'ALL', 0); -- clean exist unique sql entry
set current_schema to metadate_test;
set spq.stat_statements_track to 'all';

CREATE TABLE dist_test_table (dist_test_table_id BIGINT PRIMARY KEY, dist_test_table_embedding vector(4));
SELECT create_distributed_table('dist_test_table', 'dist_test_table_id', shard_count:=4);

insert into dist_test_table VALUES(1,'[1,2,3,4]'),(2,'[1,2,3,4]'),(3,'[1,2,3,4]'),(4,'[1,2,3,4]'),(5,'[1,2,3,4]'),(6,'[1,2,3,4]'),(7,'[1,2,3,4]');
select * from dist_test_table order by 1;

select * from dist_test_table where dist_test_table_id = 1 order by dist_test_table_id;
select * from dist_test_table where dist_test_table_id = 1 order by dist_test_table_id;

select * from dist_test_table where dist_test_table_id = 2 order by dist_test_table_embedding;
select * from dist_test_table where dist_test_table_id = 2 order by dist_test_table_embedding;
select * from dist_test_table where dist_test_table_id = 2 order by dist_test_table_embedding;

SELECT pss.query, cqs.executor, cqs.partition_key, cqs.calls FROM dbe_perf.statement pss JOIN spq_query_stats() cqs USING (unique_sql_id, user_id, node_id) where query like 'insert into dist_test_table%';
SELECT pss.query, cqs.executor, cqs.partition_key, cqs.calls FROM dbe_perf.statement pss JOIN spq_query_stats() cqs USING (unique_sql_id, user_id, node_id) where query like '%dist_test_table order by 1%';
SELECT pss.query, cqs.executor, cqs.partition_key, cqs.calls FROM dbe_perf.statement pss JOIN spq_query_stats() cqs USING (unique_sql_id, user_id, node_id) where query like '%order by dist_test_table_id%';
SELECT pss.query, cqs.executor, cqs.partition_key, cqs.calls FROM dbe_perf.statement pss JOIN spq_query_stats() cqs USING (unique_sql_id, user_id, node_id) where query like '%order by dist_test_table_embedding%';

select node_id,query,executor,partition_key,calls from spq_stat_statements where query like 'insert into dist_test_table%';
select node_id,query,executor,partition_key,calls from spq_stat_statements where query like '%dist_test_table order by 1%';
select node_id,query,executor,partition_key,calls from spq_stat_statements where query like '%order by dist_test_table_id%';
select node_id,query,executor,partition_key,calls from spq_stat_statements where query like '%order by dist_test_table_embedding%';

select b.query,a.initiator_node_identifier,a.worker_query from get_all_active_transactions() a left join pg_stat_activity b on a.process_id=b.pid where b.query like '%get_all_active_transactions%';

select count(*) from get_global_active_transactions()  a left join pg_stat_activity b on a.process_id=b.pid where query like '%get_global_active_transactions%';

select nodeid from spq_stat_activity where query like '%spq_stat_activity%' order by nodeid;

begin;
select * from dist_test_table where dist_test_table_id = 1;
select nodeid,locktype,mode from spq_locks where relation_name like 'metadate_test.dist_test_table%' order by nodeid;
commit;

drop table dist_test_table;
reset spq.stat_statements_track;
drop schema metadate_test cascade;
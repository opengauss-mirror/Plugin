CREATE OR REPLACE FUNCTION pg_catalog.show_status(is_session boolean)
RETURNS TABLE (VARIABLE_NAME text, VALUE text) AS
$$
DECLARE sql_mode text;
BEGIN
EXECUTE IMMEDIATE 'show dolphin.sql_mode' into sql_mode;
set dolphin.sql_mode='ansi_quotes,pipes_as_concat';
RETURN QUERY
select 'os_runtime_count' as variable_name , count(*)::text as value from dbe_perf.global_os_runtime
union
select concat(nodename, '-', memorytype) as variable_name, memorymbytes::text as value from dbe_perf.global_memory_node_detail
union
select unnest(array['p80', 'p95']) as variable_name, unnest(array[p80, p95]::text[]) as value from dbe_perf.statement_responsetime_percentile
union
select 'global_instance_time_count' as variable_name, count(*)::text as value from dbe_perf.global_instance_time
union
select unnest(array['node_name', 'worker_info', 'session_info', 'stream_info']) as variable_name, unnest(array[node_name, worker_info, session_info, stream_info]) as value from dbe_perf.global_threadpool_status
union
select unnest(array['group_id', 'bind_numa_id', 'bind_cpu_number', 'listener']) as variable_name, unnest(array[group_id, bind_numa_id, bind_cpu_number, listener]::text[]) as value from dbe_perf.global_threadpool_status
union
select 'os_threads_count' as variable_name,count(*)::text as value from dbe_perf.global_os_threads
union
select unnest(array['node_name', 'user_name']) as variable_name, unnest(array[node_name, user_name]) as value from dbe_perf.summary_user_login
union
select unnest(array['login_counter', 'logout_counter', 'user_id']) as variable_name, unnest(array[login_counter, logout_counter, user_id]::text[]) as value from dbe_perf.summary_user_login
union
select unnest(array['select_count', 'update_count', 'insert_count', 'delete_count', 'ddl_count', 'dml_count', 'dcl_count']) as variable_name, unnest(array[select_count, update_count, insert_count, delete_count, ddl_count, dml_count, dcl_count]::text[]) as value from dbe_perf.summary_workload_sql_count
union
select unnest(array['node_name', 'workload']) as variable_name, unnest(array[node_name, workload]::text[]) as value from dbe_perf.global_workload_transaction
union
select unnest(array['commit_counter', 'rollback_counter', 'resp_min', 'resp_max', 'resp_avg', 'resp_total', 'bg_commit_counter', 'bg_rollback_counter', 'bg_resp_min', 'bg_resp_max', 'bg_resp_avg', 'bg_resp_total']) as variable_name,
unnest(array[commit_counter, rollback_counter, resp_min, resp_max, resp_avg, resp_total, bg_commit_counter, bg_rollback_counter, bg_resp_min, bg_resp_max, bg_resp_avg, bg_resp_total]::text[]) as value from dbe_perf.global_workload_transaction
union
select 'node_name' as variable_name, node_name::text as value from dbe_perf.global_bgwriter_stat
union
select 'stats_reset' as variable_name, stats_reset::text as value from dbe_perf.global_bgwriter_stat
union
select unnest(array['checkpoint_write_time', 'checkpoint_sync_time']) as variable_name, unnest(array[checkpoint_write_time, checkpoint_sync_time]::text[]) as value from dbe_perf.global_bgwriter_stat
union
select unnest(array['checkpoints_timed', 'checkpoints_req', 'buffers_checkpoint', 'buffers_clean', 'maxwritten_clean', 'buffers_backend', 'buffers_backend_fsync', 'buffers_alloc']) as variable_name, unnest(array[checkpoints_timed, checkpoints_req, buffers_checkpoint, buffers_clean, maxwritten_clean, buffers_backend, buffers_backend_fsync, buffers_alloc]::text[]) as value from dbe_perf.global_bgwriter_stat
union
select unnest(array['phyrds', 'phywrts', 'phyblkrd', 'phyblkwrt']) as variable_name, unnest(array[phyrds, phywrts, phyblkrd, phyblkwrt]::text[]) as value from dbe_perf.summary_rel_iostat
union
select 'summary_file_iostat_count' as variable_name, count(*)::text as value from dbe_perf.summary_file_iostat
union
select unnest(array['phywrts', 'phyblkwrt', 'writetim', 'avgiotim', 'lstiotim', 'miniotim', 'maxiowtm']) as variable_name, unnest(array[phywrts, phyblkwrt, writetim, avgiotim, lstiotim, miniotim, maxiowtm]::text[]) as value from dbe_perf.summary_file_redo_iostat
union
select unnest(array['node_name', 'worker_info']) as variable_name, unnest(array[node_name, worker_info]::text[]) as value from dbe_perf.global_redo_status union select unnest(array['redo_start_ptr', 'redo_start_time', 'redo_done_time', 'curr_time', 'min_recovery_point', 'read_ptr', 'last_replayed_read_ptr', 'recovery_done_ptr', 'read_xlog_io_counter', 'read_xlog_io_total_dur', 'read_data_io_counter', 'read_data_io_total_dur', 'write_data_io_counter', 'write_data_io_total_dur', 'process_pending_counter', 'process_pending_total_dur', 'apply_counter', 'apply_total_dur', 'speed', 'local_max_ptr', 'primary_flush_ptr']) as variable_name, unnest(array[redo_start_ptr, redo_start_time, redo_done_time, curr_time, min_recovery_point, read_ptr, last_replayed_read_ptr, recovery_done_ptr, read_xlog_io_counter, read_xlog_io_total_dur, read_data_io_counter, read_data_io_total_dur, write_data_io_counter, write_data_io_total_dur, process_pending_counter, process_pending_total_dur, apply_counter, apply_total_dur, speed, local_max_ptr, primary_flush_ptr]::text[]) as value from dbe_perf.global_redo_status
union
select unnest(array['node_name', 'ckpt_redo_point']) as variable_name, unnest(array[node_name, ckpt_redo_point]::text[]) as value from dbe_perf.global_ckpt_status union select unnest(array['ckpt_clog_flush_num', 'ckpt_csnlog_flush_num', 'ckpt_multixact_flush_num', 'ckpt_predicate_flush_num', 'ckpt_twophase_flush_num']) as variable_name, unnest(array[ckpt_clog_flush_num, ckpt_csnlog_flush_num, ckpt_multixact_flush_num, ckpt_predicate_flush_num, ckpt_twophase_flush_num]::text[]) as value from dbe_perf.global_ckpt_status
union
select unnest(array['databaseid', 'tablespaceid', 'relfilenode']) as variable_name, unnest(array[databaseid, tablespaceid, relfilenode]::text[]) as value from dbe_perf.summary_stat_bad_block
union
select unnest(array['forknum', 'error_count']) as variable_name, unnest(array[forknum, error_count]::text[]) as value from dbe_perf.summary_stat_bad_block
union
select unnest(array['first_time', 'last_time']) as variable_name, unnest(array[first_time, last_time]::text[]) as value from dbe_perf.summary_stat_bad_block
union
select unnest(array['node_name', 'queue_head_page_rec_lsn', 'queue_rec_lsn', 'current_xlog_insert_lsn', 'ckpt_redo_point']) as variable_name, unnest(array[node_name, queue_head_page_rec_lsn, queue_rec_lsn, current_xlog_insert_lsn, ckpt_redo_point]) as value from dbe_perf.global_pagewriter_status
union
select unnest(array['pgwr_actual_flush_total_num', 'pgwr_last_flush_num', 'remain_dirty_page_num']) as variable_name, unnest(array[pgwr_actual_flush_total_num, pgwr_last_flush_num, remain_dirty_page_num]::text[]) as value from dbe_perf.global_pagewriter_status
union
select unnest(array['confl_tablespace', 'confl_lock', 'confl_snapshot', 'confl_bufferpin', 'confl_deadlock']) as variable_name, unnest(array[confl_tablespace, confl_lock, confl_snapshot, confl_bufferpin, confl_deadlock]::text[]) as value from dbe_perf.summary_stat_database_conflicts where datname = current_database()
union
select 'wait_events_count' as variable_name, count(*)::text as value from dbe_perf.global_wait_events
union
select 'locks_count' as variable_name,count(*)::text as value from pg_locks where database in (select oid from pg_database where datname = current_database())
union
select 'datname' as variable_name, datname::text as value from pg_stat_database where datname = current_database()
union
select unnest(array['numbackends', 'xact_commit', 'xact_rollback', 'blks_read', 'blks_hit', 'tup_returned', 'tup_fetched', 'tup_inserted', 'tup_updated', 'tup_deleted', 'conflicts', 'temp_files', 'temp_bytes', 'deadlocks', 'datid']) as variable_name, unnest(array[numbackends, xact_commit, xact_rollback, blks_read, blks_hit, tup_returned, tup_fetched, tup_inserted, tup_updated, tup_deleted, conflicts, temp_files, temp_bytes, deadlocks, datid]::text[]) as value from pg_stat_database where datname = current_database()
union
select unnest(array['blk_read_time', 'blk_write_time']) as variable_name, unnest(array[blk_read_time, blk_write_time]::text[]) as value from pg_stat_database where datname = current_database()
union
select 'stats_reset' as variable_name, stats_reset::text as value from pg_stat_database where datname = current_database()
union
select unnest(array['curr_dwn', 'curr_start_page', 'file_trunc_num', 'file_reset_num', 'total_writes', 'low_threshold_writes', 'high_threshold_writes', 'total_pages', 'low_threshold_pages', 'high_threshold_pages', 'file_id']) as variable_name, unnest(array[curr_dwn, curr_start_page, file_trunc_num, file_reset_num, total_writes, low_threshold_writes, high_threshold_writes, total_pages, low_threshold_pages, high_threshold_pages, file_id]::text[]) as value from dbe_perf.global_double_write_status
union
select unnest(array['active', 'dummy_standby']) as variable_name, unnest(array[active, dummy_standby]::text[]) as value from dbe_perf.global_replication_slots
union
select 'datoid' as variable_name, datoid::text as value from dbe_perf.global_replication_slots
union
select unnest(array['node_name', 'database']) as variable_name, unnest(array[node_name, database]::text[]) as value from dbe_perf.global_replication_slots
union
select unnest(array['slot_name', 'plugin', 'slot_type', 'restart_lsn']) as variable_name, unnest(array[slot_name, plugin, slot_type, restart_lsn]) as value from dbe_perf.global_replication_slots
union
select unnest(array['x_min', 'catalog_xmin']) as variable_name, unnest(array[x_min, catalog_xmin]::text[]) as value from dbe_perf.global_replication_slots
union
select unnest(array['node_name', 'usename']) as variable_name, unnest(array[node_name, usename]::text[]) as value from dbe_perf.global_replication_stat
union
select unnest(array['pid', 'client_port', 'sync_priority']) as variable_name, unnest(array[pid, client_port, sync_priority]) as value from dbe_perf.global_replication_stat
union
select 'usesysid' as variable_name, usesysid::text as value from dbe_perf.global_replication_stat
union
select 'backend_start' as variable_name, backend_start::text as value from dbe_perf.global_replication_stat
union
select 'client_addr' as variable_name, client_addr::text as value from dbe_perf.global_replication_stat
union
select unnest(array['application_name', 'client_hostname', 'state', 'sender_sent_location', 'receiver_write_location', 'receiver_flush_location', 'receiver_replay_location', 'sync_state']) as variable_name, unnest(array[application_name, client_hostname, state, sender_sent_location, receiver_write_location, receiver_flush_location, receiver_replay_location, sync_state]) as value from dbe_perf.global_replication_stat
union
select unnest(array['owner', 'database']) as variable_name, unnest(array[owner, database]::text[]) as value from dbe_perf.summary_transactions_prepared_xacts
union
select 'transaction' as variable_name, transaction::text as value from dbe_perf.summary_transactions_prepared_xacts
union
select 'gid' as variable_name, gid as value from dbe_perf.summary_transactions_prepared_xacts
union
select 'prepared' as variable_name, prepared::text as value from dbe_perf.summary_transactions_prepared_xacts order by variable_name;
EXECUTE IMMEDIATE 'set dolphin.sql_mode=''' || sql_mode || '''';
END;
$$
LANGUAGE plpgsql;
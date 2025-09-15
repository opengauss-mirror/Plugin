create schema deadlock_test_schema;
set current_schema to deadlock_test_schema;

select count(*) from (SELECT global_pid != spq_calculate_gpid(nodeid, b.lwpid) as result FROM spq_dist_stat_activity a join pg_os_threads b on a.pid=b.pid where b.thread_name not like '%Spq Maintenance Daemon%') a where a.result = 't';

CREATE TABLE deadlock_test (a int primary key, b int);
SELECT create_distributed_table('deadlock_test', 'a', shard_count:=2);
insert into deadlock_test values(1,1);
insert into deadlock_test values(2,2);
\set VERBOSITY terse

\parallel on 2
begin
update deadlock_test set b=11 where a=1;
pg_sleep(1);
update deadlock_test set b=222 where a=2;
end;
/

declare
res record;
v int;
begin
update deadlock_test set b=22 where a=2;
pg_sleep(2);
select * into res from spq_lock_waits;
raise notice 'blocked_statement: %, current_statement_in_blocking_process:%', res.blocked_statement, res.current_statement_in_blocking_process;
update deadlock_test set b=111 where a=1;
end;
/
\parallel off

drop table deadlock_test;
drop schema deadlock_test_schema cascade;
\unset VERBOSITY
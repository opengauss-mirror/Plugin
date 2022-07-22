drop database if exists b_databaselock;
create database b_databaselock dbcompatibility 'b';
\c b_databaselock

\parallel on 2
declare
i int;
begin
select get_lock('slock1',1) into i;
raise notice 'session1:%',i;
select get_lock('slock1',1) into i;
raise notice 'session1:%',i;
select get_lock('slock1',1) into i;
raise notice 'session1:%',i;
select get_lock('slock2',1) into i;
raise notice 'session1:%',i;
select get_lock('slock2',1) into i;
raise notice 'session1:%',i;
select get_lock('slock2',1) into i;
raise notice 'session1:%',i;

perform pg_sleep(3);

select is_free_lock('slock1') into i;
raise notice 'session1:%',i;
select 1 into i where exists (select is_free_lock('slock3'));
raise notice 'session1:%',i;
select is_used_lock('slock3') into i;
raise notice 'session1:%',i;

select release_lock('slock1') into i;
raise notice 'session1:%',i;
select release_lock('slock3') into i;
raise notice 'session1:%',i;
select release_all_locks() into i;
raise notice 'session1:%',i;
end;
/
declare
i int;
begin
perform pg_sleep(1);
select is_free_lock('slock1') into i;
raise notice 'session2:%',i;
select is_free_lock('slock3') into i;
raise notice 'session2:%',i;
select is_used_lock('slock3') into i;
raise notice 'session2:%',i;

select release_lock('slock1') into i;
raise notice 'session2:%',i;
select release_lock('slock2') into i;
raise notice 'session2:%',i;
select release_lock('slock3') into i;
raise notice 'session2:%',i;
select release_all_locks() into i;
raise notice 'session2:%',i;
end;
/
\parallel off
\c postgres
drop database if exists b_databaselock;

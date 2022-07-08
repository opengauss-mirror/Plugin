drop database if exists b_databaselock;
create database b_databaselock dbcompatibility 'b';
\c b_databaselock

\parallel on 2
declare
i int;
begin
perform pg_sleep(0.5);
select get_lock('slock1',1) into i;
raise notice 'session1:%',i;
perform pg_sleep(1);
select get_lock('slock2',1) into i;
raise notice 'session1:%',i;
perform pg_sleep(5);
end;
/
declare
i int;
begin
select get_lock('slock2',1) into i;
raise notice 'session2:%',i;
perform pg_sleep(2);
select get_lock('slock1',1) into i;
raise notice 'session2:%',i;
perform pg_sleep(3);
end;
/
\parallel off
\c postgres
drop database if exists b_databaselock;

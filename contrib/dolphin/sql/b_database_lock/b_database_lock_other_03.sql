drop database if exists b_databaselock;
create database b_databaselock dbcompatibility 'b';
\c b_databaselock

select release_all_locks( );
--failed
select release_all_locks('');
--failed
select release_all_locks(' ');
--failed
select release_all_locks(a);
--failed
select release_all_locks(1);
--failed
select release_all_locks(a1);
--failed
select release_all_locks('a');
--failed
select release_all_locks('1');
--failed
select release_all_locks('a1');
--failed
select release_all_locks(.);
--failed
select release_all_locks('.');
\c postgres
drop database if exists b_databaselock;

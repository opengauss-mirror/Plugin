drop database if exists b_databaselock;
create database b_databaselock dbcompatibility 'b';
\c b_databaselock

select get_lock('slock','1');
select get_lock('slock','1a');
select get_lock('slock','a');
--failed
select get_lock('slock',a);
select get_lock('slock','.');
--failed
select get_lock('slock',.);
\c postgres
drop database if exists b_databaselock;

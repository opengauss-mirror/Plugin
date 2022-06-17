drop database if exists b_databaselock;
create database b_databaselock dbcompatibility 'b';
\c b_databaselock

--failed
select get_lock('slock',a);
select get_lock('slock','a');
select get_lock('slock','1');
--failed
select get_lock('slock',!);
select get_lock('slock','!');
\c postgres
drop database if exists b_databaselock;

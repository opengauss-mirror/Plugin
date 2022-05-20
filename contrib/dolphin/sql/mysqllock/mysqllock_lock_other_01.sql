drop database if exists mysqllock;
create database mysqllock dbcompatibility 'b';
\c mysqllock
create extension dolphin;
--all failed
select get_lock();
select get_lock('');
select get_lock('',1);
select is_free_lock();
select is_free_lock('');
select is_used_lock();
select is_used_lock('');
select release_lock();
select release_lock('');
\c postgres
drop database if exists mysqllock;

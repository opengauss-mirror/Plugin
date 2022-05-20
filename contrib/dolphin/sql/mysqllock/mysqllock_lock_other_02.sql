drop database if exists mysqllock;
create database mysqllock dbcompatibility 'b';
\c mysqllock
create extension dolphin;
select get_lock('slock','1');
select get_lock('slock','1a');
select get_lock('slock','a');
--failed
select get_lock('slock',a);
select get_lock('slock','.');
--failed
select get_lock('slock',.);
\c postgres
drop database if exists mysqllock;

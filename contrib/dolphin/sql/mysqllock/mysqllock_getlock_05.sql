drop database if exists mysqllock;
create database mysqllock dbcompatibility 'b';
\c mysqllock
create extension dolphin;
--failed
select get_lock('slock',a);
select get_lock('slock','a');
select get_lock('slock','1');
--failed
select get_lock('slock',!);
select get_lock('slock','!');
\c postgres
drop database if exists mysqllock;

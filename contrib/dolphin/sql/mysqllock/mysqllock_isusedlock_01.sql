drop database if exists mysqllock;
create database mysqllock dbcompatibility 'b';
\c mysqllock
create extension dolphin;
select is_used_lock('aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa');
select is_used_lock('aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa');
--failed
select is_used_lock('aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa');
select is_used_lock('获取一个锁2022');
select is_used_lock('1234567890');
select is_used_lock(1);
select is_used_lock(1234567890);
--failed
select is_used_lock(a);
--failed
select is_used_lock(`);
--failed
select is_used_lock(!);
--failed
select is_used_lock(@);
\c postgres
drop database if exists mysqllock;

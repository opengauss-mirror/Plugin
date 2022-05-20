drop database if exists mysqllock;
create database mysqllock dbcompatibility 'b';
\c mysqllock
create extension dolphin;
select get_lock('aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa',1);
select get_lock('aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa',1);
--failed
select get_lock('aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa',1);
select get_lock('aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa');
select get_lock('aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa');
--failed
select get_lock('aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa');
select get_lock('获取一个锁2022',1);
select get_lock('获取一个锁2022');
select get_lock('1234567890',1);
select get_lock('1234567890');
select get_lock(1);
select get_lock(1,1);
select get_lock(1234567890);
select get_lock(1234567890,1);
--failed
select get_lock(a);
--failed
select get_lock(a,1);
--failed
select get_lock(`);
--failed
select get_lock(!);
--failed
select get_lock(@);
--failed
select get_lock(`,1);
--failed
select get_lock(!,1);
--failed
select get_lock(@,1);
\c postgres
drop database if exists mysqllock;

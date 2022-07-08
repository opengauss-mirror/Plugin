drop database if exists b_databaselock;
create database b_databaselock dbcompatibility 'b';
\c b_databaselock

select release_lock('aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa');
select release_lock('aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa');
--failed
select release_lock('aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa');
select release_lock('获取一个锁2022');
select release_lock('1234567890');
select release_lock(1);
select release_lock(1234567890);
--failed
select release_lock(a);
--failed
select release_lock(`);
--failed
select release_lock(!);
--failed
select release_lock(@);
\c postgres
drop database if exists b_databaselock;

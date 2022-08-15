drop database if exists b_databaselock;
create database b_databaselock dbcompatibility 'b';
\c b_databaselock

select is_free_lock('aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa');
select is_free_lock('aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa');
--failed
select is_free_lock('aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa');
select is_free_lock('获取一个锁2022');
select is_free_lock('1234567890');
select is_free_lock(1);
select is_free_lock(1234567890);
--failed
select is_free_lock(a);
--failed
select is_free_lock(`);
--failed
select is_free_lock(!);
--failed
select is_free_lock(@);
\c postgres
drop database if exists b_databaselock;

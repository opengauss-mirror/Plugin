drop database if exists mysqllock;
create database mysqllock dbcompatibility 'b';
\c mysqllock
create extension dolphin;
select get_lock('號驛爾叁');
select get_lock('數據庫');
select get_lock('計算機');
select get_lock('奧特曼');
select get_lock('葫蘆娃');
select lockname from get_all_locks() order by lockname;

select is_free_lock('號驛爾叁');
select is_free_lock('數據庫');
select is_free_lock('計算機');
select is_free_lock('奧特曼');
select is_free_lock('葫蘆娃');

select release_lock('號驛爾叁');
select release_lock('數據庫');
select release_lock('計算機');
select release_lock('奧特曼');
select release_lock('葫蘆娃');
\c postgres
drop database if exists mysqllock;

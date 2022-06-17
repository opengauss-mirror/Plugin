drop database if exists b_databaselock;
create database b_databaselock dbcompatibility 'b';
\c b_databaselock

select get_lock('データベース');
select get_lock('警察だ');
select get_lock('光の化身');
select get_lock('砂のラチェットジュース');
select get_lock('ドラゴンソウルアタッチ');
select lockname from get_all_locks() order by lockname;

select is_free_lock('データベース');
select is_free_lock('警察だ');
select is_free_lock('光の化身');
select is_free_lock('砂のラチェットジュース');
select is_free_lock('ドラゴンソウルアタッチ');

select release_lock('データベース');
select release_lock('警察だ');
select release_lock('光の化身');
select release_lock('砂のラチェットジュース');
select release_lock('ドラゴンソウルアタッチ');
\c postgres
drop database if exists b_databaselock;

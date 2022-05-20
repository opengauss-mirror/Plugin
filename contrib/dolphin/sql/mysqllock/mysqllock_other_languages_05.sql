drop database if exists mysqllock;
create database mysqllock dbcompatibility 'b';
\c mysqllock
create extension dolphin;
select get_lock('데이터베이스입니다');
select get_lock('경찰');
select get_lock('빛의 화신');
select get_lock('산자나무 주스');
select get_lock('용혼이 빙의하다');
select lockname from get_all_locks() order by lockname;

select is_free_lock('데이터베이스입니다');
select is_free_lock('경찰');
select is_free_lock('빛의 화신');
select is_free_lock('산자나무 주스');
select is_free_lock('용혼이 빙의하다');

select release_lock('데이터베이스입니다');
select release_lock('경찰');
select release_lock('빛의 화신');
select release_lock('산자나무 주스');
select release_lock('용혼이 빙의하다');
\c postgres
drop database if exists mysqllock;

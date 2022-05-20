drop database if exists mysqllock;
create database mysqllock dbcompatibility 'b';
\c mysqllock
create extension dolphin;
select get_lock('database');
select get_lock('The police');
select get_lock('The embodiment of light');
select get_lock('Sea buckthorn juice');
select get_lock('Possessed by dragon souls');
select lockname from get_all_locks() order by lockname;

select is_free_lock('database');
select is_free_lock('The police');
select is_free_lock('The embodiment of light');
select is_free_lock('Sea buckthorn juice');
select is_free_lock('Possessed by dragon souls');

select release_lock('database');
select release_lock('The police');
select release_lock('The embodiment of light');
select release_lock('Sea buckthorn juice');
select release_lock('Possessed by dragon souls');
\c postgres
drop database if exists mysqllock;

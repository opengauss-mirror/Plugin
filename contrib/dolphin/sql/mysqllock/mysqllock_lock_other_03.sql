drop database if exists mysqllock;
create database mysqllock dbcompatibility 'b';
\c mysqllock
create extension dolphin;
select release_all_locks( );
--failed
select release_all_locks('');
--failed
select release_all_locks(' ');
--failed
select release_all_locks(a);
--failed
select release_all_locks(1);
--failed
select release_all_locks(a1);
--failed
select release_all_locks('a');
--failed
select release_all_locks('1');
--failed
select release_all_locks('a1');
--failed
select release_all_locks(.);
--failed
select release_all_locks('.');
\c postgres
drop database if exists mysqllock;

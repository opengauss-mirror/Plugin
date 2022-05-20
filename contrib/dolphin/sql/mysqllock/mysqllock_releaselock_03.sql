drop database if exists mysqllock;
create database mysqllock dbcompatibility 'b';
\c mysqllock
create extension dolphin;
select release_lock('a                                                             a');
select release_lock('a                                                              a');
--failed
select release_lock('a                                                               a');
--failed
select release_lock(1                                                              1);
select release_lock('1                                                              1');
select release_lock('a                                                              1');
select release_lock(' ');
\c postgres
drop database if exists mysqllock;

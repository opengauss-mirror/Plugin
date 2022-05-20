drop database if exists mysqllock;
create database mysqllock dbcompatibility 'b';
\c mysqllock
create extension dolphin;
select is_used_lock('a                                                             a');
select is_used_lock('a                                                              a');
--failed
select is_used_lock('a                                                               a');
--failed
select is_used_lock(1                                                              1);
select is_used_lock('1                                                              1');
select is_used_lock('a                                                              1');
select is_used_lock(' ');
\c postgres
drop database if exists mysqllock;

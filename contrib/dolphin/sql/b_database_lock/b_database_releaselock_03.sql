drop database if exists b_databaselock;
create database b_databaselock dbcompatibility 'b';
\c b_databaselock

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
drop database if exists b_databaselock;

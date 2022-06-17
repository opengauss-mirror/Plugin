drop database if exists b_databaselock;
create database b_databaselock dbcompatibility 'b';
\c b_databaselock

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
drop database if exists b_databaselock;

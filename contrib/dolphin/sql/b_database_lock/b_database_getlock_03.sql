drop database if exists b_databaselock;
create database b_databaselock dbcompatibility 'b';
\c b_databaselock

select get_lock('a                                                             a');
select get_lock('a                                                              a');
--failed
select get_lock('a                                                               a');
select get_lock('1                                                              1');
select get_lock('a                                                              1');
select get_lock('a                                                             a',1);
select get_lock('a                                                              a',1);
--failed
select get_lock('a                                                               a',1);
select get_lock('1                                                              1',1);
select get_lock('a                                                              1',1);
--failed
select get_lock(1                                                              1);
--failed
select get_lock(1                                                              1,1);
select get_lock(' ');
select get_lock(' ',1);
\c postgres
drop database if exists b_databaselock;

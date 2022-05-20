drop database if exists mysqllock;
create database mysqllock dbcompatibility 'b';
\c mysqllock
create extension dolphin;
create table table_mysql_01(c1 int,c2 text);
insert into table_mysql_01 values(1,'a');
begin;
select get_lock('slock1',1);
insert into table_mysql_01 values(2,'b');
select get_lock('slock2',1);
rollback;
select * from table_mysql_01;
select a.lockname from get_all_locks() a where a.lockname like 'slock%' order by lockname;
drop table table_mysql_01;
\c postgres
drop database if exists mysqllock;

drop database if exists mysqllock;
create database mysqllock dbcompatibility 'b';
\c mysqllock
create extension dolphin;
create table mysql_lock_t1(c1 int);
insert into mysql_lock_t1 values (9);
select get_lock('mysqltest_lock', 100);
update mysql_lock_t1 set c1 = 2 and get_lock('mysqltest_lock', 100);
select * from mysql_lock_t1;
update mysql_lock_t1 set c1 = 4;
select c1 from mysql_lock_t1;
select release_lock('mysqltest_lock');
select release_lock('mysqltest_lock');
select * from mysql_lock_t1;
drop table mysql_lock_t1;
create table mysql_lock_t1(c1 int);
\parallel on 3
declare
i int;
begin
select get_lock('mysqltest_lock', 100) into i;
raise notice 'session1_1:%',i;
perform pg_sleep(1);
select release_lock('mysqltest_lock') into i;
raise notice 'session1_1:%',i;
end;
/
declare
i int;
begin
perform pg_sleep(1);
insert into mysql_lock_t1 select get_lock('mysqltest_lock', 5);
select * into i from mysql_lock_t1;
raise notice 'session1_2:%',i;
select release_lock('mysqltest_lock') into i;
raise notice 'session1_2:%',i;
end;
/
declare
i int;
begin
perform pg_sleep(3);
update mysql_lock_t1 set c1 = 4;
select * into i from mysql_lock_t1;
raise notice 'session1_3:%',i;
end;
/
\parallel off
drop table mysql_lock_t1;
create table mysql_lock_t1(c1 int,c2 text);
create table mysql_lock_t2(c1 int,c2 text);
\parallel on 2
begin
raise notice 'session2_1';
insert into mysql_lock_t1(c1,c2) values(1,'a');
insert into mysql_lock_t1(c1,c2) values(2,'b');
insert into mysql_lock_t2(c1,c2) values(3,'c');
insert into mysql_lock_t2(c1,c2) values(4,'d');
perform pg_sleep(1);
end;
/
declare
j int;
begin
perform pg_sleep(2);
perform get_lock('mysqltest_lock', 100) FROM mysql_lock_t1, mysql_lock_t2;
perform get_lock('mysqltest_lock', 100) FROM mysql_lock_t1;
perform get_lock('mysqltest_lock', 100) FROM mysql_lock_t2;
select release_lock('mysqltest_lock') into j;
raise notice 'session2_2:%',j;
select release_lock('mysqltest_lock') into j;
raise notice 'session2_2:%',j;
select release_lock('mysqltest_lock') into j;
raise notice 'session2_2:%',j;
select release_lock('mysqltest_lock') into j;
raise notice 'session2_2:%',j;
select release_lock('mysqltest_lock') into j;
raise notice 'session2_2:%',j;
end;
/
\parallel off
drop table mysql_lock_t1;
drop table mysql_lock_t2;
\c postgres
drop database if exists mysqllock;

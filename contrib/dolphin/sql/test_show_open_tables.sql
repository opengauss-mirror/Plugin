-- init
create user user1 with sysadmin password 'Show@123';
create user user2 password 'Show@456';
-- B库执行
drop database if exists show_open_tables_b;
create database show_open_tables_b dbcompatibility 'b';
\c show_open_tables_b
-- create schema
create schema show_open_tables_scm;
set search_path to show_open_tables_scm;
-- create table
create table show_open_tables_test_1(id int);
create table show_open_tables_test_2(id int);
-- 管理员用户执行
-- set session authorization user1 password 'Show@123';
set role user1 password 'Show@123';
-- no table touched in schema
SHOW OPEN TABLES IN show_open_tables_scm;
SHOW OPEN TABLES FROM show_open_tables_scm;
-- 1 and 1+ tables touched
insert into show_open_tables_test_1 values(1);
SHOW OPEN TABLES IN show_open_tables_scm;
SHOW OPEN TABLES FROM show_open_tables_scm;
insert into show_open_tables_test_2 values(1);
SHOW OPEN TABLES IN show_open_tables_scm;
SHOW OPEN TABLES FROM show_open_tables_scm;
-- 1 and 1+ locks on table
lock tables show_open_tables_test_1 read;
select pg_class.relname, pg_locks.mode, pg_locks.granted
    from pg_locks, pg_class where pg_locks.relation = pg_class.oid and pg_class.relname = 'show_open_tables_test_1';
SHOW OPEN TABLES IN show_open_tables_scm;
lock tables show_open_tables_test_1 write;
select pg_class.relname, pg_locks.mode, pg_locks.granted
    from pg_locks, pg_class where pg_locks.relation = pg_class.oid and pg_class.relname = 'show_open_tables_test_1';
SHOW OPEN TABLES IN show_open_tables_scm;
unlock tables;
select pg_class.relname, pg_locks.mode, pg_locks.granted
    from pg_locks, pg_class where pg_locks.relation = pg_class.oid and pg_class.relname = 'show_open_tables_test_1';
SHOW OPEN TABLES FROM show_open_tables_scm;
begin;
lock table show_open_tables_test_1 in ACCESS EXCLUSIVE mode;
select pg_class.relname, pg_locks.mode, pg_locks.granted
    from pg_locks, pg_class where pg_locks.relation = pg_class.oid and pg_class.relname = 'show_open_tables_test_1';
SHOW OPEN TABLES IN show_open_tables_scm;
lock table show_open_tables_test_1 in EXCLUSIVE mode;
select pg_class.relname, pg_locks.mode, pg_locks.granted
    from pg_locks, pg_class where pg_locks.relation = pg_class.oid and pg_class.relname = 'show_open_tables_test_1';
SHOW OPEN TABLES IN show_open_tables_scm;
lock table show_open_tables_test_1 in  SHARE ROW EXCLUSIVE mode;
select pg_class.relname, pg_locks.mode, pg_locks.granted
    from pg_locks, pg_class where pg_locks.relation = pg_class.oid and pg_class.relname = 'show_open_tables_test_1';
SHOW OPEN TABLES IN show_open_tables_scm;
lock table show_open_tables_test_1 in SHARE mode;
select pg_class.relname, pg_locks.mode, pg_locks.granted
    from pg_locks, pg_class where pg_locks.relation = pg_class.oid and pg_class.relname = 'show_open_tables_test_1';
SHOW OPEN TABLES IN show_open_tables_scm;
lock table show_open_tables_test_1 in SHARE UPDATE EXCLUSIVE mode;
select pg_class.relname, pg_locks.mode, pg_locks.granted
    from pg_locks, pg_class where pg_locks.relation = pg_class.oid and pg_class.relname = 'show_open_tables_test_1';
SHOW OPEN TABLES IN show_open_tables_scm;
lock table show_open_tables_test_1 in ROW EXCLUSIVE mode;
select pg_class.relname, pg_locks.mode, pg_locks.granted
    from pg_locks, pg_class where pg_locks.relation = pg_class.oid and pg_class.relname = 'show_open_tables_test_1';
SHOW OPEN TABLES IN show_open_tables_scm;
lock table show_open_tables_test_1 in ROW SHARE mode;
select pg_class.relname, pg_locks.mode, pg_locks.granted
    from pg_locks, pg_class where pg_locks.relation = pg_class.oid and pg_class.relname = 'show_open_tables_test_1';
SHOW OPEN TABLES IN show_open_tables_scm;
lock table show_open_tables_test_1 in ACCESS SHARE mode;
select pg_class.relname, pg_locks.mode, pg_locks.granted
    from pg_locks, pg_class where pg_locks.relation = pg_class.oid and pg_class.relname = 'show_open_tables_test_1';
SHOW OPEN TABLES IN show_open_tables_scm;
commit;
select pg_class.relname, pg_locks.mode, pg_locks.granted
    from pg_locks, pg_class where pg_locks.relation = pg_class.oid and pg_class.relname = 'show_open_tables_test_1';
SHOW OPEN TABLES FROM show_open_tables_scm;
-- [LIKE 'pattern' | WHERE expr]
SHOW OPEN TABLES LIKE 'abc%';
SHOW OPEN TABLES LIKE 'show%';
SHOW OPEN TABLES IN show_open_tables_scm LIKE 'abc%';
SHOW OPEN TABLES IN show_open_tables_scm LIKE 'show%';
SHOW OPEN TABLES FROM show_open_tables_scm LIKE 'abc%';
SHOW OPEN TABLES FROM show_open_tables_scm LIKE 'show%';
SHOW OPEN TABLES WHERE "Table" = 'abc';
SHOW OPEN TABLES WHERE "Table" = 'show_open_tables_test_1';
SHOW OPEN TABLES WHERE Database = 'show_open_tables_scm';
SHOW OPEN TABLES WHERE In_use > 0;
SHOW OPEN TABLES IN show_open_tables_scm WHERE "Table" = 'abc';
SHOW OPEN TABLES IN show_open_tables_scm WHERE "Table" = 'show_open_tables_test_1';
SHOW OPEN TABLES IN show_open_tables_scm WHERE Database = 'show_open_tables_scm';
SHOW OPEN TABLES IN show_open_tables_scm WHERE In_use > 0;
SHOW OPEN TABLES FROM show_open_tables_scm WHERE "Table" = 'abc';
SHOW OPEN TABLES FROM show_open_tables_scm WHERE "Table" = 'show_open_tables_test_1';
SHOW OPEN TABLES FROM show_open_tables_scm WHERE Database = 'show_open_tables_scm';
SHOW OPEN TABLES FROM show_open_tables_scm WHERE In_use > 0;
-- 普通用户执行
-- set session authorization user2 password 'Show@456';
set role user2 password 'Show@456';
SHOW OPEN TABLES IN show_open_tables_scm;
SHOW OPEN TABLES FROM show_open_tables_scm;
-- 非B库执行
set role user1 password 'Show@123';
drop database if exists show_open_tables_nb;
create database show_open_tables_nb;
\c show_open_tables_nb
SHOW OPEN TABLES;
SHOW OPEN TABLES IN show_open_tables_scm;
SHOW OPEN TABLES FROM show_open_tables_scm;
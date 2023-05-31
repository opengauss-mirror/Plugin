-- init
create user user1 with sysadmin password 'Show@123';
create user user2 password 'Show@456';
-- B库执行
drop database if exists show_storage_engines_b;
create database show_storage_engines_b dbcompatibility 'b';
\c show_storage_engines_b
-- 管理员用户执行
-- set session authorization user1 password 'Show@123';
set role user1 password 'Show@123';
-- row DEFAULT/column YES
create table row_default_tb(id int);
create table row_tb(id int) with (orientation=row);
create table column_tb(id int) with (orientation=column);
SHOW ENGINES;
SHOW STORAGE ENGINES;
-- ASTORE/USTORE
-- enable_ustore=on
show enable_ustore;
-- enable_default_ustore_table on: ASTORE YES, USTORE DEFAULT
set enable_default_ustore_table = on;
show enable_default_ustore_table;
create table ustore_default_tb_1(id int);
create table astore_tb_1(id int) with (storage_type=astore);
create table ustore_tb_1(id int) with (storage_type=ustore);
SHOW ENGINES;
-- enable_default_ustore_table off: ASTORE DEFAULT, USTORE YES
set enable_default_ustore_table = off;
show enable_default_ustore_table;
create table astore_default_tb_2(id int);
create table astore_tb_2(id int) with (storage_type=astore);
create table ustore_tb_2(id int) with (storage_type=ustore);
SHOW ENGINES;
-- mot_fdw
-- ENABLE_MOT=on
-- enable_incremental_checkpoint=on
show enable_incremental_checkpoint;
-- failed
create foreign table mot_tb_1(id int) server mot_server;
-- enable_incremental_checkpoint=on: mot_fdw NO
SHOW ENGINES;
-- 普通用户执行
-- set session authorization user2 password 'Show@456';
set role user2 password 'Show@456';
SHOW ENGINES;
SHOW STORAGE ENGINES;
-- 非B库执行
set role user1 password 'Show@123';
drop database if exists show_storage_engines_nb;
create database show_storage_engines_nb;
\c show_storage_engines_nb
SHOW ENGINES;
SHOW STORAGE ENGINES;
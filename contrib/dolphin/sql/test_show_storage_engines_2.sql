-- B库执行
\c show_storage_engines_b
-- 管理员用户执行
-- set session authorization user1 password 'Show@123';
set role user1 password 'Show@123';
-- ASTORE/USTORE
-- enable_ustore=off
show enable_ustore;
-- ASTORE DEFAULT, USTORE NO
set enable_default_ustore_table = on;
show enable_default_ustore_table;
create table astore_default_tb_3(id int);
create table astore_tb_3(id int) with (storage_type=astore);
-- failed
create table ustore_tb_3(id int) with (storage_type=ustore);
SHOW ENGINES;
-- ASTORE DEFAULT, USTORE NO
set enable_default_ustore_table = off;
show enable_default_ustore_table;
create table astore_default_tb_4(id int);
create table astore_tb_4(id int) with (storage_type=astore);
-- failed
create table ustore_tb_4(id int) with (storage_type=ustore);
SHOW ENGINES;
-- mot_fdw
-- ENABLE_MOT=on
-- enable_incremental_checkpoint=off
show enable_incremental_checkpoint; 
grant create any table to user2;
set role user2 password 'Show@456';
-- failed
create foreign table mot_tb_3(id int) server mot_server;
-- user has no authority: mot_fdw NO
SHOW ENGINES;
set role user1 password 'Show@123';
grant usage on foreign server mot_server to user2;
set role user2 password 'Show@456';
-- succeed
create foreign table mot_tb_4(id int) server mot_server;
-- user has authority: mot_fdw YES
SHOW ENGINES;
set role user1 password 'Show@123';
revoke usage on foreign server mot_server from user2;
set role user2 password 'Show@456';
-- failed
create foreign table mot_tb_5(id int) server mot_server;
-- user has no authority: mot_fdw NO
SHOW ENGINES;
-- cleanup
\c contrib_regression
drop database show_storage_engines_b;
drop database show_storage_engines_nb;
drop user user1;
drop user user2;
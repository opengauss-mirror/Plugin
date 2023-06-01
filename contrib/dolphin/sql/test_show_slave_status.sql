-- init
create user user1 with sysadmin password 'Show@123';
create user user2 password 'Show@456';
-- B库执行
drop database if exists show_slave_status_b;
create database show_slave_status_b dbcompatibility 'b';
\c show_slave_status_b
-- 管理员用户执行
-- set session authorization user1 password 'Show@123';
set role user1 password 'Show@123';
SHOW SLAVE STATUS;
SHOW REPLICA STATUS;
SHOW SLAVE STATUS FOR CHANNEL 'no_qualified_row';
SHOW REPLICA STATUS FOR CHANNEL 'no_qualified_row';
-- 普通用户执行
-- set session authorization user2 password 'Show@456';
set role user2 password 'Show@456';
SHOW SLAVE STATUS;
SHOW REPLICA STATUS;
SHOW SLAVE STATUS FOR CHANNEL 'no_qualified_row';
SHOW REPLICA STATUS FOR CHANNEL 'no_qualified_row';
-- 非B库执行
set role user1 password 'Show@123';
drop database if exists show_slave_status_nb;
create database show_slave_status_nb;
\c show_slave_status_nb
SHOW SLAVE STATUS;
SHOW REPLICA STATUS;
SHOW SLAVE STATUS FOR CHANNEL 'no_qualified_row';
SHOW REPLICA STATUS FOR CHANNEL 'no_qualified_row';
-- cleanup
\c contrib_regression
drop database show_slave_status_b;
drop database show_slave_status_nb;
drop user user1;
drop user user2;
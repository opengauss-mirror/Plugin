drop database if exists show_createdatabase;
create database show_createdatabase dbcompatibility 'b';
\c show_createdatabase
CREATE USER test_showcreate_database WITH PASSWORD 'openGauss@123';
GRANT ALL PRIVILEGES TO test_showcreate_database;
SET ROLE test_showcreate_database PASSWORD 'openGauss@123';
create schema test_get_database;
show create database test_get_database;
show create schema test_get_database;
show create database if not exists test_get_database;
show create schema if not exists test_get_database;
create schema test_database_blockchain with blockchain;
show create database test_database_blockchain;
show create schema test_database_blockchain;
show create database if not exists test_database_blockchain;
show create schema if not exists test_database_blockchain;
drop schema test_database_blockchain cascade;
-- Check no existence
show create database aa;
drop schema test_get_database cascade;
RESET ROLE;
DROP USER test_showcreate_database;
\c postgres
drop database if exists show_createdatabase;
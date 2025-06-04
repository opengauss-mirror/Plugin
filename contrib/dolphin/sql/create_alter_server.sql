-- mysql-fdw
-- configure adding --enable-mysql-fdw
-- gs_guc generate -S XXX -D $GAUSSHOME/bin -o usermapping
create database create_alter_server_database with dbcompatibility = 'B';
\c create_alter_server_database
select oid, * from pg_foreign_data_wrapper;
create extension mysql_fdw;
create extension mysql; --error
select oid, * from pg_foreign_data_wrapper;
select oid, * from pg_foreign_server;
create server server_test foreign data wrapper mysql options(host '127.0.0.1', port '3306', user 'foreign_server_test',
password 'password@123', database 'my_db', owner 'test_user', socket 'my_socket');
create server server_test0 foreign data wrapper mysql options(host '127.0.0.1', port '3306', user 'foreign_server_test',
password 'password@123', database 'my_db', owner 'test_user');
create server server_test00 foreign data wrapper mysql_fdw options(host '127.0.0.1', port '3306', user 'foreign_server_test',
password 'password@123', database 'my_db', owner 'test_user', socket 'my_socket');
select * from pg_user_mapping;
create user mapping for current_user server server_test; --error, user mapping exists
drop user mapping for current_user server server_test;
create user mapping for current_user server server_test;
create server server_test1 foreign data wrapper mysql_fdw options(host '127.0.0.1', port '3306', user 'foreign_server_test',
password 'password@123', database 'my_db', owner 'test_user', socket 'my_socket');
create server server_test2 foreign data wrapper mysql_fdw;
create server server_test3 foreign data wrapper mysql_fdw options(host '127.0.0.1');
create server server_test4 foreign data wrapper mysql_fdw options(port '3306');
create server server_test5 foreign data wrapper mysql_fdw options(my_param 'my_param'); --error
select oid, * from pg_foreign_server;
alter server server_test options(host '192.168.0.1');
alter server server_test options(port '3310');
alter server server_test options(user 'foreign_server_test1');
alter server server_test options(password 'password@1234');
alter server server_test options(database 'my_db1');
alter server server_test options(owner 'test_user1');
alter server server_test options(socket 'my_socket1');
alter server server_test options(my_param 'my_param'); --error
select oid, * from pg_foreign_server;
drop server if exists server_test;
drop server if exists server_test cascade;
drop server server_test1;
drop server server_test1 cascade;
drop server if exists my_server;
drop server my_server;

drop extension mysql_fdw;
drop extension mysql_fdw cascade;

\c postgres
drop database create_alter_server_database;
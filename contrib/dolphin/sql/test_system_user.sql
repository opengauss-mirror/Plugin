drop database if exists test_system_user;
create database test_system_user dbcompatibility 'b';
\c test_system_user
select session_user;
select session_user();
select user;
select user();
select system_user();
\c postgres
drop database test_system_user;
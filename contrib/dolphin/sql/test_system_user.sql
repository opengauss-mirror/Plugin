create schema test_system_user;
set current_schema to 'test_system_user';
select session_user;
select session_user();
select user;
select user();
select system_user();
drop schema test_system_user cascade;
reset current_schema;
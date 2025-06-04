create schema test_system_user;
set current_schema to 'test_system_user';
select session_user();
select user();
select system_user();
drop schema test_system_user cascade;
reset current_schema;

-- test grant/revoke itself
drop user if exists user_revoke_001;
create user user_revoke_001 password 'Test_123';
grant proxy on user_revoke_001 to user_revoke_001;
\du user_revoke_001
revoke proxy on user_revoke_001 from user_revoke_001;
\du user_revoke_001
drop user if exists user_revoke_001;

-- test grant/revoke recursion
drop user if exists user001;
drop user if exists user002;
create user user001 password 'Test_123';
create user user002 password 'Test_123';
\du user001
\du user002
grant proxy on user001 to user002 with grant option;
grant proxy on user002 to user001;
\du user001
\du user002
revoke proxy on user001 from user002;
revoke proxy on user002 from user001;
\du user001
\du user002
drop user if exists user001;
drop user if exists user002;

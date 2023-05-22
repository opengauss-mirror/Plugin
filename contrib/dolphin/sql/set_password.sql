create schema test_set_password;
set current_schema to 'test_set_password';
set b_compatibility_user_host_auth to on;
set password = 'abc@1234';
set password for current_user() = 'abc@2345';
create user user1 password 'abc@1234';
set password for user1 = 'abc@2345';
set session authorization user1 password 'abc@2345';
set password for 'user1'@'%' = 'abc@3456' replace 'abc@2345';
set session authorization user1 password 'abc@3456';
set password for 'user1'@'%' = PASSWORD('abc@4567') replace 'abc@3456';
set session authorization user1 password 'abc@4567';

reset session authorization;
create user `backquoteuser` identified by 'Gauss@123';
set password for `backquoteuser`='test-1234';
set password for `backquoteuser`='test-12345' replace 'test-1234';
select object_name,detail_info from pg_query_audit('2022-01-13 9:30:00', '2031-12-12 22:00:00') where object_name = 'backquoteuser';

\c contrib_regression
drop user user1;
drop user backquoteuser;
drop schema test_set_password cascade;
reset current_schema;

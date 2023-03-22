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
\c contrib_regression
drop user user1;
drop schema test_set_password cascade;
reset current_schema;

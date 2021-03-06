drop database if exists test_set_password;
create database test_set_password dbcompatibility 'b';
\c test_set_password
set password = 'abc@1234';
set password for current_user() = 'abc@2345';
create user user1 password 'abc@1234';
set password for user1 = 'abc@2345';
set session authorization user1 password 'abc@2345';
set password for 'user1'@'%' = 'abc@3456' replace 'abc@2345';
set session authorization user1 password 'abc@3456';
set password for 'user1'@'%' = PASSWORD('abc@4567') replace 'abc@3456';
set session authorization user1 password 'abc@4567';
\c postgres
drop database if exists test_set_password;
drop user user1;
DROP DATABASE IF EXISTS role_test_db;
CREATE DATABASE role_test_db dbcompatibility 'b';
\c role_test_db;
SET lower_case_table_names TO 0;
CREATE TABLE `test_table` (`a` int);
CREATE USER `Test_User` WITH PASSWORD 'openGauss@123';
SELECT `rolname` FROM `pg_roles` WHERE `rolname` LIKE '%Test_%';
GRANT ALL PRIVILEGES TO `test_user`;
REVOKE ALL PRIVILEGES FROM `test_user`;
GRANT ALL PRIVILEGES TO `Test_User`;
REVOKE ALL PRIVILEGES FROM `Test_User`;

ALTER USER `test_user` RENAME TO `test_USer`;
ALTER USER `Test_User` RENAME TO `test_USer`;

CREATE SCHEMA `test_scheam` AUTHORIZATION `test_user`;
CREATE SCHEMA `test_scheam` AUTHORIZATION `test_USer`;

ALTER TABLE `test_table` OWNER TO `test_user`;
ALTER TABLE `test_table` OWNER TO `test_USer`;

DROP USER `test_user` CASCADE;
DROP USER `test_USer` CASCADE;

\c postgres
DROP DATABASE role_test_db;

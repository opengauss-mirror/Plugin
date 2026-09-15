set dolphin.b_compatibility_mode to off;
DROP DATABASE IF EXISTS role_test_db;
CREATE DATABASE role_test_db dbcompatibility 'b';
\c role_test_db;
set dolphin.lower_case_table_names TO 0;
CREATE TABLE test_table (a int);
CREATE USER Test_User WITH PASSWORD 'openGauss@123';
SELECT rolname FROM pg_roles WHERE rolname LIKE '%Test_%';
GRANT ALL PRIVILEGES TO test_user;
REVOKE ALL PRIVILEGES FROM test_user;
GRANT ALL PRIVILEGES TO Test_User;
REVOKE ALL PRIVILEGES FROM Test_User;

ALTER USER test_user RENAME TO test_USer;
ALTER USER Test_User RENAME TO test_USer;

CREATE SCHEMA test_scheam AUTHORIZATION test_user;
CREATE SCHEMA test_scheam AUTHORIZATION test_USer;

ALTER TABLE test_table OWNER TO test_user;
ALTER TABLE test_table OWNER TO test_USer;

DROP USER test_user CASCADE;
DROP USER test_USer CASCADE;

CREATE ROLE Aa PASSWORD 'gauss;123';
DROP ROLE Aa;

SET dolphin.lower_case_table_names TO 1;
CREATE ROLE TEST PASSWORD 'abcde;123';
GRANT ALL ON pg_roles TO TEST;
REVOKE ALL ON pg_roles FROM TEST;
DROP ROLE TEST;

\c postgres
DROP DATABASE role_test_db;

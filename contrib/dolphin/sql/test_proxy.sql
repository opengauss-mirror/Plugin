create schema db_proxy;
set current_schema to 'db_proxy';

CREATE SCHEMA tst_schema1;
SET SEARCH_PATH TO tst_schema1;

CREATE TABLE tst_t1
(
id int,
name varchar(20)
);

INSERT INTO tst_t1 values(20220101, 'proxy_example');

DROP ROLE if EXISTS test_proxy_u1;
CREATE ROLE test_proxy_u1 IDENTIFIED BY 'test_proxy_u1@123';

DROP ROLE if EXISTS test_proxy_u2;
CREATE ROLE test_proxy_u2 IDENTIFIED BY 'test_proxy_u2@123';

DROP ROLE if EXISTS test_proxy_u3;
CREATE ROLE test_proxy_u3 IDENTIFIED BY 'test_proxy_u3@123';

GRANT ALL ON SCHEMA tst_schema1 TO test_proxy_u1;
GRANT ALL ON SCHEMA tst_schema1 TO test_proxy_u2;
GRANT ALL ON SCHEMA tst_schema1 TO test_proxy_u3;

-- let test_proxy_u1 can read the tst_t1
GRANT ALL ON tst_t1 to test_proxy_u1;

SET ROLE test_proxy_u2 PASSWORD 'test_proxy_u2@123';
SELECT * FROM tst_schema1.tst_t1;
RESET ROLE;

-- GRANT PROXY
GRANT PROXY ON test_proxy_u1 TO test_proxy_u2;

SET ROLE test_proxy_u2 PASSWORD 'test_proxy_u2@123';
SELECT * FROM tst_schema1.tst_t1;

GRANT PROXY ON test_proxy_u1 TO test_proxy_u3;

RESET ROLE;

GRANT PROXY ON test_proxy_u1 TO test_proxy_u2 with grant option;

SET ROLE test_proxy_u2 PASSWORD 'test_proxy_u2@123';
GRANT PROXY ON test_proxy_u1 TO test_proxy_u3;

RESET ROLE;
SET ROLE test_proxy_u3 PASSWORD 'test_proxy_u3@123';
SELECT * FROM tst_schema1.tst_t1;

RESET ROLE;
REVOKE PROXY ON test_proxy_u1 FROM test_proxy_u2, test_proxy_u3;

SET ROLE test_proxy_u2 PASSWORD 'test_proxy_u2@123';
SELECT * FROM tst_schema1.tst_t1;

RESET ROLE;
SET ROLE test_proxy_u3 PASSWORD 'test_proxy_u3@123';
SELECT * FROM tst_schema1.tst_t1;

RESET ROLE;
REVOKE ALL ON tst_schema1.tst_t1 FROM test_proxy_u1;
REVOKE ALL ON SCHEMA tst_schema1 FROM test_proxy_u1;
REVOKE ALL ON SCHEMA tst_schema1 FROM test_proxy_u2;
REVOKE ALL ON SCHEMA tst_schema1 FROM test_proxy_u3;

--grant usage
--error, warn syntax
GRANT USAGE1 ON *.* to test_proxy_u1 IDENTIFIED BY 'test_proxy_u1@234';
GRANT USAGE ON * to test_proxy_u1 IDENTIFIED BY 'test_proxy_u1@234';
GRANT USAGE ON *.* to test_proxy_u1;
GRANT ALL ON *.* to test_proxy_u1 IDENTIFIED BY 'test_proxy_u1@234';
--success, change passwd
GRANT USAGE ON *.* to test_proxy_u1 IDENTIFIED BY 'test_proxy_u1@234';
SET ROLE test_proxy_u1 PASSWORD 'test_proxy_u1@234';
RESET ROLE;
--sucess, create role
set b_compatibility_user_host_auth to on;
GRANT USAGE ON *.* to does_not_exist@localhost IDENTIFIED BY 'does_not_exist1@localhost';
--SET ROLE does_not_exist@localhost PASSWORD 'does_not_exist1@localhost';
--RESET ROLE;
--sucess, change passwd
GRANT USAGE ON *.* to does_not_exist@localhost IDENTIFIED BY 'does_not_exist1@localhost2';
--SET ROLE does_not_exist@localhost PASSWORD 'does_not_exist1@localhost2';
--RESET ROLE;
--error, can't use b_mode_create_user_if_not_exist in alter user, it's a internal option
alter user does_not_exist@localhost b_mode_create_user_if_not_exist;
drop user does_not_exist@localhost;
reset b_compatibility_user_host_auth;


drop role test_proxy_u1;
drop role test_proxy_u2;
drop role test_proxy_u3;

drop schema db_proxy cascade;
drop schema tst_schema1 cascade;
reset current_schema;


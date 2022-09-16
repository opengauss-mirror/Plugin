drop database if exists db_proxy;
create database db_proxy dbcompatibility 'b';
\c db_proxy

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

drop role test_proxy_u1;
drop role test_proxy_u2;
drop role test_proxy_u3;

\c postgres
drop database if exists db_proxy;


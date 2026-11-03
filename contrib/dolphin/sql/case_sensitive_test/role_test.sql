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

-- Issue #1633: mode 0 preserves the spelling used by CREATE and DROP ROLE.
CREATE ROLE Aa PASSWORD 'gauss;123';
SELECT rolname FROM pg_roles WHERE rolname IN ('Aa', 'aa') ORDER BY rolname;
DROP ROLE Aa;
SELECT count(*) AS remaining_roles FROM pg_roles WHERE rolname IN ('Aa', 'aa');

CREATE ROLE CaseMemberA PASSWORD 'Gauss_2583@case';
CREATE ROLE CaseMemberB PASSWORD 'Gauss_2583@case';
CREATE ROLE CaseGroup PASSWORD 'Gauss_2583@case' ROLE CaseMemberA;
ALTER GROUP CaseGroup ADD USER CaseMemberB;
REVOKE "CaseGroup" FROM CaseMemberB;
DROP OWNED BY CaseMemberA, CaseMemberB;
DROP GROUP CaseGroup;
DROP ROLE CaseMemberA, CaseMemberB;

-- Issue #1633: mode 1 stores lowercase names and normalizes role references.
SET dolphin.lower_case_table_names TO 1;
CREATE ROLE TEST PASSWORD 'abcde;123';
SELECT rolname FROM pg_roles WHERE rolname IN ('TEST', 'Test', 'test') ORDER BY rolname;
SELECT has_table_privilege('test', 'pg_catalog.pg_roles', 'UPDATE')::int AS initial_update;
GRANT ALL ON pg_roles TO TEST;
SELECT has_table_privilege('test', 'pg_catalog.pg_roles', 'UPDATE')::int AS upper_grant;
REVOKE ALL ON pg_roles FROM Test;
SELECT has_table_privilege('test', 'pg_catalog.pg_roles', 'UPDATE')::int AS mixed_case_revoke;
GRANT ALL ON pg_roles TO test;
SELECT has_table_privilege('test', 'pg_catalog.pg_roles', 'UPDATE')::int AS lower_grant;
REVOKE ALL ON pg_roles FROM TEST;
SELECT has_table_privilege('test', 'pg_catalog.pg_roles', 'UPDATE')::int AS upper_revoke;
DROP ROLE tEsT;
SELECT count(*) AS remaining_roles FROM pg_roles WHERE rolname IN ('TEST', 'Test', 'test');

CREATE ROLE "QuotedRoleCase" PASSWORD 'Gauss_2583@case';
SELECT rolname FROM pg_roles WHERE rolname = 'QuotedRoleCase';
GRANT SELECT ON pg_roles TO "QuotedRoleCase";
REVOKE SELECT ON pg_roles FROM "QuotedRoleCase";
DROP ROLE "QuotedRoleCase";

\c postgres
DROP DATABASE role_test_db;

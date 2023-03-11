\c table_name_test_db;
set dolphin.lower_case_table_names TO 0;

CREATE USER Test_Schema_User WITH PASSWORD 'openGauss@123';
CREATE USER Test_Schema_User_bak WITH PASSWORD 'openGauss@123';

-- DolphinColId
CREATE SCHEMA test_1;
CREATE SCHEMA Test_1;
CREATE SCHEMA IF NOT EXISTS test_1;
CREATE SCHEMA IF NOT EXISTS Test_1;
CREATE SCHEMA IF NOT EXISTS test_2;
CREATE SCHEMA IF NOT EXISTS Test_2;
CREATE SCHEMA test_3 CHARSET 'TEST';
CREATE SCHEMA Test_3 CHARSET 'TEST';
\dn
ALTER SCHEMA test_1 WITHOUT BLOCKCHAIN;
ALTER SCHEMA Test_1 WITHOUT BLOCKCHAIN;
ALTER SCHEMA test_1 CHARSET DEFAULT;
ALTER SCHEMA Test_1 CHARSET DEFAULT;
ALTER SCHEMA test_1 RENAME TO Test_1_BAK;
ALTER SCHEMA Test_1 RENAME TO test_1_bak;
ALTER SCHEMA test_1_bak OWNER TO Test_Schema_User;
ALTER SCHEMA Test_1_BAK OWNER TO Test_Schema_User;
\dn

SET SEARCH_PATH TO test_1, Test1;
SHOW SEARCH_PATH;
SET SEARCH_PATH TO DEFAULT;
SET CURRENT_SCHEMA TO test_1, Test1;
SHOW CURRENT_SCHEMA;
SET CURRENT_SCHEMA TO DEFAULT;

-- dolphin_drop_type and dolphin_any_name_list
DROP SCHEMA IF EXISTS test_1, Test_1;
DROP SCHEMA table_name_test_db.test_1_bak, table_name_test_db.Test_1_BAK;
DROP SCHEMA test_1_bak, Test_1_BAK;
\dn

-- dolphin_any_name
DROP SCHEMA IF EXISTS Test_Schema_User CASCADE;
CREATE SCHEMA AUTHORIZATION Test_Schema_User;
\dn

-- dolphin_qualified_name
CREATE TABLE test_1_table (a int);
CREATE TABLE Test_1_table (a int);
CREATE TABLE test_2.test_1_table (a int);
CREATE TABLE Test_2.Test_1_table (a int);
CREATE TABLE table_name_test_db.test_3.test_1_table (a int);
CREATE TABLE table_name_test_db.Test_3.Test_1_table (a int);

SELECT a FROM test_1_table;
SELECT a FROM Test_1_table;
SELECT a FROM test_2.test_1_table;
SELECT a FROM Test_2.Test_1_table;
SELECT a FROM table_name_test_db.test_3.test_1_table;
SELECT a FROM table_name_test_db.Test_3.Test_1_table;

SELECT t1.a a1, t2.a a2, t3.a a3, t4.a a4 FROM
public.test_1_table t1,
PUBLIC.Test_1_table t2,
test_2.test_1_table t3,
Test_2.Test_1_table t4;

SELECT test_1_table.a FROM test_1_table;
SELECT test_1_table.a FROM Test_1_table;
SELECT test_2.test_1_table.a FROM test_2.test_1_table;
SELECT test_2.test_1_table.a FROM Test_2.Test_1_table;
SELECT table_name_test_db.test_3.test_1_table.a FROM table_name_test_db.test_3.test_1_table;
SELECT table_name_test_db.Test_3.Test_1_table.a FROM table_name_test_db.Test_3.Test_1_table;

GRANT CREATE ON SCHEMA Test_3,tesT_3 TO Test_Schema_User_bak;                            -- except: failed
GRANT CREATE ON SCHEMA Test_3,test_3 TO Test_Schema_User_bak;                            -- except: success
GRANT INDEX ON ALL TABLES IN SCHEMA Test_3,tesT_3 TO Test_Schema_User_bak;               -- except: failed
GRANT INDEX ON ALL TABLES IN SCHEMA Test_3,test_3 TO Test_Schema_User_bak;               -- except: success
GRANT INSERT ON ALL TABLES IN SCHEMA Test_3,tesT_3 TO Test_Schema_User_bak;              -- except: failed
GRANT INSERT ON ALL TABLES IN SCHEMA Test_3,test_3 TO Test_Schema_User_bak;              -- except: success
GRANT ALTER ON ALL FUNCTIONS IN SCHEMA Test_3,tesT_3 TO Test_Schema_User_bak;            -- except: failed
GRANT ALTER ON ALL FUNCTIONS IN SCHEMA Test_3,test_3 TO Test_Schema_User_bak;            -- except: success
GRANT ALTER ON ALL PACKAGES IN SCHEMA Test_3,tesT_3 TO Test_Schema_User_bak;             -- except: failed
GRANT ALTER ON ALL PACKAGES IN SCHEMA Test_3,test_3 TO Test_Schema_User_bak;             -- except: success
GRANT ALTER ON ALL SEQUENCES IN SCHEMA Test_3,tesT_3 TO Test_Schema_User_bak;            -- except: failed
GRANT ALTER ON ALL SEQUENCES IN SCHEMA Test_3,test_3 TO Test_Schema_User_bak;            -- except: success

CREATE TABLE test_alter_schema(a int);
ALTER TABLE PUBLIC.test_alter_schema SET SCHEMA Test_3;

DROP TABLE test_1_table;
DROP TABLE Test_1_table;
DROP TABLE test_2.test_1_table;
DROP TABLE Test_2.Test_1_table;
DROP TABLE table_name_test_db.test_3.test_1_table;
DROP TABLE table_name_test_db.Test_3.Test_1_table;

DROP TABLE IF EXISTS test_1_table;
DROP TABLE IF EXISTS Test_1_table;
DROP TABLE IF EXISTS test_2.test_1_table;
DROP TABLE IF EXISTS Test_2.Test_1_table;
DROP TABLE IF EXISTS table_name_test_db.test_3.test_1_table;
DROP TABLE IF EXISTS table_name_test_db.Test_3.Test_1_table;

-- test about system schema which the oid < 16384
SELECT * from PG_CATALOG.LENGTH('aaa');
SELECT * from pg_catalog.LENGTH('aaa');

-- clean
DROP USER Test_Schema_User CASCADE;
DROP USER Test_Schema_User_bak CASCADE;

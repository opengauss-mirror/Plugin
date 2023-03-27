create schema show_createdatabase;
set current_schema to 'show_createdatabase';
CREATE USER test_showcreate_database WITH PASSWORD 'openGauss@123';
GRANT ALL PRIVILEGES TO test_showcreate_database;
SET ROLE test_showcreate_database PASSWORD 'openGauss@123';
create schema test_get_database;
show create database test_get_database;
show create schema test_get_database;
show create database if not exists test_get_database;
show create schema if not exists test_get_database;
create schema test_database_blockchain with blockchain;
show create database test_database_blockchain;
show create schema test_database_blockchain;
show create database if not exists test_database_blockchain;
show create schema if not exists test_database_blockchain;
drop schema test_database_blockchain cascade;
-- tests for show quoted name of table|view|function|procedure|trigger
CREATE SCHEMA "TESTSHOW";
show create database "TESTSHOW";
show create schema "TESTSHOW";
-- table|view
CREATE TABLE "TESTSHOW_TEST_TABLE"(a int);
CREATE VIEW "TESTSHOW_TEST_VIEW"  AS SELECT * FROM "TESTSHOW_TEST_TABLE";
SHOW CREATE TABLE "TESTSHOW_TEST_TABLE"; --success
SHOW CREATE VIEW "TESTSHOW_TEST_VIEW";  --success
SHOW CREATE TABLE "TESTSHOW_TEST_VIEW"; --success
SHOW CREATE VIEW "TESTSHOW_TEST_TABLE"; --fail
-- function
create function "TESTSHOW_FUNCTEST_B"() returns trigger
as 
$$
DECLARE
BEGIN
    INSERT INTO "TESTSHOW_TEST_TABLE" VALUES(NEW.a);
    RETURN NEW;
END
$$LANGUAGE PLPGSQL;
SHOW CREATE FUNCTION "TESTSHOW_FUNCTEST_B";
-- procedure
CREATE OR REPLACE PROCEDURE "TESTSHOW_PROCEDURE_TEST"() 
SELECT a FROM "TESTSHOW_TEST_TABLE";
SHOW CREATE PROCEDURE "TESTSHOW_PROCEDURE_TEST";
-- trigger
CREATE TRIGGER "TESTSHOW_CREATE_TRIGGER"
BEFORE INSERT ON "TESTSHOW_TEST_TABLE"
FOR EACH ROW
EXECUTE PROCEDURE "TESTSHOW_FUNCTEST_B"();
SHOW CREATE TRIGGER "TESTSHOW_CREATE_TRIGGER";
DROP TRIGGER IF EXISTS "TESTSHOW_CREATE_TRIGGER";
DROP PROCEDURE IF EXISTS "TESTSHOW_PROCEDURE_TEST";
DROP FUNCTION IF EXISTS "TESTSHOW_FUNCTEST_B";
DROP VIEW IF EXISTS "TESTSHOW_TEST_VIEW";
DROP TABLE IF EXISTS "TESTSHOW_TEST_TABLE";
DROP SCHEMA "TESTSHOW" cascade;
-- Check no existence
show create database aa;
drop schema test_get_database cascade;
RESET ROLE;
DROP USER test_showcreate_database;
drop schema show_createdatabase cascade;
reset current_schema;
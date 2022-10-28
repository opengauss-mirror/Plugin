drop database if exists db_show;
create database db_show dbcompatibility 'b';
\c db_show

-- show privileges
show privileges;

-- create tables/views in tst_schema1 namespace
CREATE SCHEMA tst_schema1;
SET SEARCH_PATH TO tst_schema1;

CREATE TABLE tst_t1
(
id int,
name varchar(20),
addr text COLLATE "de_DE",
phone text COLLATE "es_ES",
addr_code text
);

CREATE TABLE t_t2
(
id int
);

CREATE VIEW tst_v1 AS SELECT * FROM tst_t1;
COMMENT ON COLUMN tst_t1.id IS 'identity';

-- create tables in tst_schema2 namespace
CREATE SCHEMA tst_schema2;
SET SEARCH_PATH TO tst_schema2;

CREATE TABLE tst_t3
(
id int
);

-- show plugins
show plugins;

-- show tables test
show tables;
show full tables;
show tables from tst_schema1;
show tables from tst_schema2;
show tables from tst_schema3;
show full tables from tst_schema1 like '%tst_%';
show full tables from tst_schema1 where Table_type = 'VIEW';
show full tables from tst_schema1 where table_type like '%VIEW%';

-- show columns
SET SEARCH_PATH TO tst_schema1;
show columns from tst_t1;
show columns from tst_t1 in tst_schema1;
show columns from tst_schema1.tst_t1;
show columns from tst_schema2.tst_t1;
show columns from tst_schema3.tst_t1;
show columns from tst_schema1.tst_t1 in tst_schema2;
show columns from tst_schema2.tst_t1 in tst_schema1;
show fields from tst_t1;
show full columns from tst_t1;
show full columns from tst_t1 like '%addr%';
show full columns from tst_t1 where Type = 'text';

-- test with normal role
DROP ROLE if EXISTS tst_shows_u1;
CREATE ROLE tst_shows_u1 IDENTIFIED BY 'tst_shows_u1@123';
SET ROLE tst_shows_u1 PASSWORD 'tst_shows_u1@123';
SELECT CURRENT_USER;
show full columns from tst_t1 in tst_schema1;

RESET role;
GRANT SELECT on tst_t1 to tst_shows_u1;
SET ROLE tst_shows_u1 PASSWORD 'tst_shows_u1@123';
SELECT CURRENT_USER;
show full columns from tst_t1 in tst_schema1;

RESET ROLE;
REVOKE all on tst_t1 from tst_shows_u1;
DROP USER tst_shows_u1;

\c postgres
drop database if exists db_show;

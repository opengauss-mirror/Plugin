create schema db_show_5;
set current_schema to 'db_show_5';

CREATE SCHEMA tst_schema5;

--orientation=row, normal primary key
-- uppercase USTORE 
CREATE TABLE db_show_5.t1
(
id int primary key,
name varchar(20),
phone text
) WITH(ORIENTATION=ROW, STORAGE_TYPE='USTORE');

-- lowercase ustore
CREATE TABLE db_show_5.t11
(
id int primary key,
name varchar(20),
phone text
) WITH(ORIENTATION=ROW, STORAGE_TYPE='ustore');

-- mixed case uStore
CREATE TABLE db_show_5.t111
(
id int primary key,
name varchar(20),
phone text
) WITH(ORIENTATION=ROW, STORAGE_TYPE='uStore');

set enable_default_ustore_table=on;
CREATE TABLE db_show_5.t1111
(
id int primary key,
name varchar(20),
phone text
) WITH(ORIENTATION=ROW);

CREATE TABLE db_show_5.t1112
(
id int primary key,
name varchar(20),
phone text
) WITH(ORIENTATION=ROW, STORAGE_TYPE='aStore');

set enable_default_ustore_table=off;

--orientation=column, serial primary key
--default astore
CREATE TABLE db_show_5.t2
(
id serial primary key,
name varchar(20),
phone text
) WITH(ORIENTATION=COLUMN);

-- lowercase astore
CREATE TABLE db_show_5.t21
(
id serial primary key,
name varchar(20),
phone text
) WITH(ORIENTATION=ROW, STORAGE_TYPE='astore');

-- uppercase USTORE
CREATE TABLE db_show_5.t22
(
id serial primary key,
name varchar(20),
phone text
) WITH(ORIENTATION=ROW, STORAGE_TYPE='ASTORE');

-- mixed case aStore
CREATE TABLE db_show_5.t23
(
id serial primary key,
name varchar(20),
phone text
) WITH(ORIENTATION=ROW, STORAGE_TYPE='aStore');

--create table in tst_schema5
CREATE TABLE tst_schema5.tst_t1
(
id int primary key,
name varchar(20),
phone text
) WITH(ORIENTATION=ROW, STORAGE_TYPE=USTORE);

CREATE TABLE tst_schema5.tst_t2
(
id serial primary key,
name varchar(20),
phone text
) WITH(ORIENTATION=COLUMN);

CREATE TABLE tst_schema5.tst_t3
(
id int,
name varchar(20),
phone text
);
COMMENT ON TABLE tst_schema5.tst_t3 IS 'this is comment';
COMMENT ON COLUMN tst_schema5.tst_t3.name IS 'comment on column';

CREATE VIEW tst_schema5.tst_v1 AS SELECT * FROM tst_schema5.tst_t2;

SHOW TABLE STATUS;
SHOW TABLE STATUS FROM tst_schema5;
SHOW TABLE STATUS FROM tst_schema5 like '%tst_t3%';
SHOW TABLE STATUS FROM tst_schema5 WHERE Engine='USTORE';
SHOW TABLE STATUS FROM tst_schema5 WHERE Row_format like '%ROW%';

DROP ROLE if EXISTS tst_shows_u5;
CREATE ROLE tst_shows_u5 IDENTIFIED BY 'tst_shows_u5@123';
SET ROLE tst_shows_u5 PASSWORD 'tst_shows_u5@123';
SELECT CURRENT_USER;
SHOW TABLE STATUS FROM tst_schema5;

RESET role;
GRANT SELECT ON ALL TABLES IN SCHEMA tst_schema5 TO tst_shows_u5;
GRANT SELECT ON ALL SEQUENCES IN SCHEMA tst_schema5 TO tst_shows_u5;
SET ROLE tst_shows_u5 PASSWORD 'tst_shows_u5@123';
SELECT CURRENT_USER;
SHOW TABLE STATUS FROM tst_schema5;

RESET ROLE;
REVOKE SELECT ON ALL TABLES IN SCHEMA tst_schema5 FROM tst_shows_u5;
REVOKE SELECT ON ALL SEQUENCES IN SCHEMA tst_schema5 FROM tst_shows_u5;
DROP USER tst_shows_u5;

SHOW TABLE STATUS FROM db_show_5 WHERE Engine='USTORE';
SHOW TABLE STATUS FROM db_show_5 WHERE Engine='ASTORE';
drop schema db_show_5 cascade;
reset current_schema;

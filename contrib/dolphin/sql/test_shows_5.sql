DROP DATABASE IF EXISTS db_show_5;
CREATE DATABASE db_show_5 DBCOMPATIBILITY 'b';
\c db_show_5

CREATE SCHEMA tst_schema5;

--orientation=row, normal primary key
CREATE TABLE public.t1
(
id int primary key,
name varchar(20),
phone text
) WITH(ORIENTATION=ROW, STORAGE_TYPE=USTORE);

--orientation=column, serial primary key
CREATE TABLE public.t2
(
id serial primary key,
name varchar(20),
phone text
) WITH(ORIENTATION=COLUMN);

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

\c postgres
DROP DATABASE IF EXISTS db_show_5;

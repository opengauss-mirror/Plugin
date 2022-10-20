CREATE DATABASE test_ansi_quotes DBCOMPATIBILITY 'B';
\c test_ansi_quotes
CREATE TABLE test_quotes (a text);
SHOW sql_mode;

-- test insert
INSERT INTO test_quotes VALUES('test1');    -- success
INSERT INTO "test_quotes" VALUES('test2');    -- success
INSERT INTO "test_quotes" VALUES("test3");    -- failed
SELECT * FROM test_quotes;
SET sql_mode TO 'sql_mode_strict,sql_mode_full_group,pipes_as_concat';
INSERT INTO test_quotes VALUES('test4');    -- success
INSERT INTO "test_quotes" VALUES('test5');    -- failed
INSERT INTO test_quotes VALUES("test6");    -- success
INSERT INTO "test_quotes" VALUES("test7");    -- failed
SELECT * FROM test_quotes;
SELECT * FROM "test_quotes";    -- failed
SELECT "a" as a1, a as a2 FROM test_quotes;    -- success, but "a" is a normal string, not column
CREATE TABLE "test_quotes_2" (a text);    -- failed
CREATE TABLE test_quotes_2 (a text) compression = "pglz";    -- success

-- test desc
desc test_quotes_2;

-- test show
show tables;
show full tables;
show create table test_quotes_2;
show columns from test_quotes;
show columns from test_quotes_2;

-- test functions
select length("ggg");
select left ("xxxx", 2);
select substr("aaasaaaa", 1, 3);

-- test expr
select "test" != "test";
select * from test_quotes where a like "%test%";
select * from test_quotes where a = "test1";

\c postgres
DROP DATABASE test_ansi_quotes;
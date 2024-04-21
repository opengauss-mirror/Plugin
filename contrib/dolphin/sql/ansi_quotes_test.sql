set current_schema to 'test_ansi_quotes';
CREATE TABLE test_quotes (a text);
show dolphin.sql_mode;

-- test insert
INSERT INTO test_quotes VALUES('test1');    -- success
INSERT INTO "test_quotes" VALUES('test2');    -- success
INSERT INTO "test_quotes" VALUES("test3");    -- failed
SELECT * FROM test_quotes;
set dolphin.sql_mode TO 'sql_mode_strict,sql_mode_full_group,pipes_as_concat';
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

-- test ctas
set dolphin.sql_mode TO 'treat_bxconst_as_binary';
create table ctas_test1 select convert(x'21', unsigned);
\d ctas_test1
create table ctas_test3 select convert(x'21', binary);
\d ctas_test3
set dolphin.sql_mode TO 'treat_bxconst_as_binary,ansi_quotes';
create table ctas_test2 select convert(x'21', unsigned);
\d ctas_test2
create table ctas_test4 select convert(x'21', binary);
\d ctas_test4
drop table ctas_test1;
drop table ctas_test2;
drop table ctas_test3;
drop table ctas_test4;

drop schema test_ansi_quotes cascade;
reset current_schema;
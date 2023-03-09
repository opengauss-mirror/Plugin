CREATE SCHEMA fulltext_test;
set current_schema to 'fulltext_test';

CREATE TABLE test (
id int unsigned auto_increment not null primary key,
title varchar,
boby text,
name name,
FULLTEXT (title, boby) WITH PARSER ngram
);
\d test
\d test_to_tsvector_to_tsvector1_idx
DROP INDEX test_to_tsvector_to_tsvector1_idx;
ALTER TABLE test ADD FULLTEXT INDEX test_index_1 (title, boby) WITH PARSER ngram;
DROP INDEX test_index_1;
CREATE FULLTEXT INDEX test_index_1 ON test (title, boby) WITH PARSER ngram;
\d test_index_1
INSERT INTO test(title, boby, name) VALUES('test', '&67575@gauss', 'opengauss');
INSERT INTO test(title, boby, name) VALUES('test1', 'gauss', 'opengauss');
INSERT INTO test(title, boby, name) VALUES('test2', 'gauss2', 'opengauss');
INSERT INTO test(title, boby, name) VALUES('test3', 'test', 'opengauss');
INSERT INTO test(title, boby, name) VALUES('gauss_123_@', 'test', 'opengauss');
INSERT INTO test(title, boby, name) VALUES('', '', 'opengauss');
INSERT INTO test(title, boby, name) VALUES(' ', ' ', ' ');
SELECT * FROM TEST;
SELECT * FROM TEST WHERE MATCH (title, boby) AGAINST ('test');
SELECT * FROM TEST WHERE MATCH (title, boby) AGAINST ('gauss');
DROP INDEX test_index_1;
CREATE FULLTEXT INDEX test_index_1 ON test (boby) WITH PARSER ngram;
\d test_index_1
SELECT * FROM test WHERE MATCH (boby) AGAINST ('test');
SELECT * FROM test WHERE MATCH (boby) AGAINST ('gauss');
DROP INDEX test_index_1;
CREATE FULLTEXT INDEX test_index_1 ON test (title, boby, name) WITH PARSER ngram;
\d test_index_1
SELECT * FROM test WHERE MATCH (title, boby, name) AGAINST ('test');
SELECT * FROM test WHERE MATCH (title, boby, name) AGAINST ('gauss');
SELECT * FROM test WHERE MATCH (title, boby, name) AGAINST ('opengauss');

-- FULLTEXT INDEX with index_name
drop table if exists articles;
CREATE TABLE articles (
ID int,
title VARCHAR(100),
FULLTEXT INDEX ngram_idx(title)WITH PARSER ngram
);
\d articles

-- FULLTEXT INDEX without index_name
drop table if exists articles;
CREATE TABLE articles (
ID int,
title VARCHAR(100),
FULLTEXT INDEX (title)WITH PARSER ngram
);
\d articles

-- FULLTEXT KEY with index_name
drop table if exists articles;
CREATE TABLE articles (
ID int,
title VARCHAR(100),
FULLTEXT KEY keyngram_idx(title)WITH PARSER ngram
);
\d articles

-- FULLTEXT KEY without index_name
drop table if exists articles;
CREATE TABLE articles (
ID int,
title VARCHAR(100),
FULLTEXT KEY (title)WITH PARSER ngram
);
\d articles

-- NO WITH PARSER ngram
create table table_ddl_0154(col1 int,col2 varchar(64), FULLTEXT idx_ddl_0154(col2));

create table table_ddl_0085(
id int(11) not null,
username varchar(50) default null,
sex varchar(5) default null,
address varchar(100) default null,
score_num int(11));
create fulltext index idx_ddl_0085_02 on table_ddl_0085(username);

insert into table_ddl_0085 values (1,'test','m','xi''an changanqu', 10001), (2,'tst','w','xi''an beilingqu', 10002),
(3,'es','w','xi''an yangtaqu', 10003),(4,'s','m','beijingchaoyangqu', 10004);
-- search_modifier, only gram compatibility, but not working
SELECT * FROM table_ddl_0085 WHERE MATCH (username) AGAINST ('te' IN NATURAL LANGUAGE MODE);
SELECT * FROM table_ddl_0085 WHERE MATCH (username) AGAINST ('ts' IN NATURAL LANGUAGE MODE WITH QUERY EXPANSION);
SELECT * FROM table_ddl_0085 WHERE MATCH (username) AGAINST ('test' IN BOOLEAN MODE);
SELECT * FROM table_ddl_0085 WHERE MATCH (username) AGAINST ('es' WITH QUERY EXPANSION);
SELECT * FROM table_ddl_0085 WHERE MATCH (username) AGAINST ('s');

insert into table_ddl_0085 select * from table_ddl_0085 where match (username) against ('te' IN NATURAL LANGUAGE MODE);
select * from table_ddl_0085;

create fulltext index idx_ddl_0085_03 on table_ddl_0085(username) with parser ngram visible;
create fulltext index idx_ddl_0085_04 on table_ddl_0085(username) visible with parser ngram;
create fulltext index idx_ddl_0085_05 on table_ddl_0085(username) visible;
create fulltext index idx_ddl_0085_06 on table_ddl_0085(username) with parser ngram comment 'TEST FULLTEXT INDEX COMMENT';
create fulltext index idx_ddl_0085_07 on table_ddl_0085(username) comment 'TEST FULLTEXT INDEX COMMENT' with parser ngram;
create fulltext index idx_ddl_0085_08 on table_ddl_0085(username) comment 'TEST FULLTEXT INDEX COMMENT';

drop schema fulltext_test cascade;
reset current_schema;

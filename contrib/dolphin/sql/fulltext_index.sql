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

drop schema fulltext_test cascade;
reset current_schema;

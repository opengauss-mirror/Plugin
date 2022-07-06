drop database if exists db_b_new_gram_test;
create database db_b_new_gram_test dbcompatibility 'B';
\c db_b_new_gram_test

-- CREATE TABLE engine test
CREATE TABLE test_engine_1 (a int) engine = InnoDB;
CREATE TABLE  IF NOT EXISTS test_engine_2 (a int) engine = InnoDB;
CREATE TYPE test_engine_type1 AS (a int, b text);
CREATE TABLE test_engine_type_table1 OF test_engine_type1 engine = InnoDB;
CREATE TYPE test_engine_type2 AS (a int, b text);
CREATE TABLE IF NOT EXISTS test_engine_type_table2 OF test_engine_type2 engine = InnoDB;

CREATE TABLE test_engine_1_1 (a int) engine = 'InnoDB';
CREATE TABLE  IF NOT EXISTS test_engine_2_1 (a int) engine = 'InnoDB';
CREATE TYPE test_engine_type1_1 AS (a int, b text);
CREATE TABLE test_engine_type_table1_1 OF test_engine_type1_1 engine = 'InnoDB';
CREATE TYPE test_engine_type2_1 AS (a int, b text);
CREATE TABLE IF NOT EXISTS test_engine_type_table2_1 OF test_engine_type2_1 engine = 'InnoDB';

CREATE TABLE test_engine_as engine = InnoDB as select a from test_engine_1;

-- CREATE TABLE COMPRESSION test
CREATE TABLE test_compression_1_pglz (a int) COMPRESSION = pglz;
\d+ test_compression_1_pglz
CREATE TABLE  IF NOT EXISTS test_compression_create_2_pglz (a int) COMPRESSION = pglz;
\d+ test_compression_create_2_pglz
CREATE TABLE test_compression_type_table_1_pglz OF test_engine_type1 COMPRESSION = pglz;
\d+ test_compression_type_table_1_pglz
CREATE TABLE IF NOT EXISTS test_compression_create_type_table_2_pglz OF test_engine_type2 COMPRESSION = pglz;
\d+ test_compression_create_type_table_2_pglz
CREATE TABLE test_compression_as_pglz COMPRESSION = pglz as select a from test_compression_1_pglz;
\d+ test_compression_as_pglz

CREATE TABLE test_compression_1_zstd (a int) COMPRESSION = zstd;
\d+ test_compression_1_zstd
CREATE TABLE  IF NOT EXISTS test_compression_create_2_zstd (a int) COMPRESSION = zstd;
\d+ test_compression_create_2_zstd
CREATE TABLE test_compression_type_table_1_zstd OF test_engine_type1 COMPRESSION = zstd;
\d+ test_compression_type_table_1_zstd
CREATE TABLE IF NOT EXISTS test_compression_create_type_table_2_zstd OF test_engine_type2 COMPRESSION = zstd;
\d+ test_compression_create_type_table_2_zstd
CREATE TABLE test_compression_as_zstd COMPRESSION = zstd as select a from test_compression_1_zstd;
\d+ test_compression_as_zstd

CREATE TABLE test_compression_1_none (a int) COMPRESSION = 'none';
\d+ test_compression_1_none
CREATE TABLE  IF NOT EXISTS test_compression_create_2_none (a int) COMPRESSION = 'none';
\d+ test_compression_create_2_none
CREATE TABLE test_compression_type_table_1_none OF test_engine_type1 COMPRESSION = 'none';
\d+ test_compression_type_table_1_none
CREATE TABLE IF NOT EXISTS test_compression_create_type_table_2_none OF test_engine_type2 COMPRESSION = 'none';
\d+ test_compression_create_type_table_2_none
CREATE TABLE test_compression_as_none COMPRESSION = none as select a from test_compression_1_none;
\d+ test_compression_as_none

CREATE TABLE test_compression_1_aaaa (a int) COMPRESSION = aaaa;
CREATE TABLE  IF NOT EXISTS test_compression_create_2_aaaa (a int) COMPRESSION = aaaa;
CREATE TABLE test_compression_type_table_1_aaaa OF test_engine_type1 COMPRESSION = aaaa;
CREATE TABLE IF NOT EXISTS test_compression_create_type_table_2_aaaa OF test_engine_type2 COMPRESSION = aaaa;
CREATE TABLE test_compression_as_aaaa COMPRESSION = aaaa as select a from test_compression_1_aaaa;

-- ALTER TABLE COMPRESSION test
ALTER TABLE test_compression_1_pglz COMPRESSION = zstd;

-- DROP INDEX ... ON TABLE
CREATE TABLE test (a int);
CREATE INDEX test_index1 ON test USING btree(a);
CREATE INDEX test_index2 ON test USING btree(a);
DROP INDEX test_index1 ON test;
DROP INDEX IF EXISTS test_index2 ON test;
DROP TABLE test;

-- LIST PARTITION TEST
CREATE TABLE test_list1 (col1 int, col2 int)
PARTITION BY list(col1)
(
PARTITION p1 VALUES (2000),
PARTITION p2 VALUES (3000),
PARTITION p3 VALUES (4000),
PARTITION p4 VALUES (5000)
);

\d+ test_list1

CREATE TABLE test_list2 (col1 int, col2 int)
PARTITION BY list(col1)
(
PARTITION p1 VALUES IN (2000),
PARTITION p2 VALUES IN (3000),
PARTITION p3 VALUES IN (4000),
PARTITION p4 VALUES IN (5000)
);

\d+ test_list2

-- INSERT
INSERT test_engine_1 VALUES (1);

-- VALUES and VALUE
INSERT INTO test_engine_1 VALUES (2);
INSERT INTO test_engine_1 VALUE (3);

-- INSERT and VALUE
INSERT test_engine_1 VALUE (4);

-- SELECT DISTINCT
INSERT INTO test_engine_1 VALUES (2) ORDER BY 1;

SELECT DISTINCT * FROM test_engine_1 ORDER BY 1;
SELECT DISTINCT ON (a % 2) * FROM test_engine_1 ORDER BY (a % 2);

SELECT DISTINCTROW * FROM test_engine_1 ORDER BY 1;
SELECT DISTINCTROW ON (a % 2) * FROM test_engine_1 ORDER BY (a % 2);

-- DROP PREPARE
PREPARE instr_test_1(int) AS INSERT INTO test_engine_1 VALUES ($1);
PREPARE instr_test_2(int) AS INSERT INTO test_engine_1 VALUES ($1);
PREPARE instr_test_3(int) AS INSERT INTO test_engine_1 VALUES ($1);

SELECT count(*) FROM PG_PREPARED_STATEMENTS WHERE name LIKE '%instr_test%';
DROP PREPARE instr_test_1;
SELECT count(*) FROM PG_PREPARED_STATEMENTS WHERE name LIKE '%instr_test%';
DROP PREPARE ALL;
SELECT count(*) FROM PG_PREPARED_STATEMENTS WHERE name LIKE '%instr_test%';

DROP TEMPORARY TABLE test_engine_1 CASCADE;

-- new grammar test for analyze table
CREATE TABLE t_new_analyze(c1 int, c2 text);
ANALYZE NO_WRITE_TO_BINLOG TABLE t_new_analyze;
ANALYZE LOCAL TABLE t_new_analyze;
ANALYZE TABLE t_new_analyze;
ANALYZE TABLE t_not_exist, t_new_analyze;

-- new grammar test for CREATE TABLESPACE
CREATE TABLESPACE test_tbspace ADD DATAFILE 'test_tbspace1';
CREATE TABLE t_tbspace(num int) TABLESPACE test_tbspace;
\d t_tbspace
DROP TABLE t_tbspace;
DROP TABLESPACE test_tbspace;

-- new grammar test for RENAME [TO | AS]
CREATE TABLE t_rename(c int);
\d t_rename
ALTER TABLE t_rename RENAME TO t_rename_to;
\d t_rename_to
ALTER TABLE t_rename_to RENAME AS t_rename_as;
\d t_rename_as
DROP TABLE t_rename_as;

-- new grammar test for CREATE INDEX
DROP TABLE IF EXISTS t_index_new_grammar;
CREATE TABLE t_index_new_grammar(c1 int, c2 int);
CREATE INDEX test_index_btree_1 ON t_index_new_grammar USING btree(c1);
CREATE INDEX test_index_btree_2 USING btree ON t_index_new_grammar(c2);
\d t_index_new_grammar
DROP INDEX test_index_btree_1;
DROP INDEX test_index_btree_2;
DROP TABLE t_index_new_grammar;

-- new grammar test for COLLATE, CHARSET, ROW_FORMAT in column and table level
CREATE TABLE t_column_collate(c text COLLATE test_collate);
DROP TABLE t_column_collate;
CREATE TABLE t_column_collate(c text COLLATE "en_US");
DROP TABLE t_column_collate;
CREATE TABLE t_column_collate(c text COLLATE en_US);
DROP TABLE t_column_collate;
CREATE TABLE t_column_charset(c text CHARSET test_charset);
DROP TABLE t_column_charset;
CREATE TABLE t_column_charset(c text CHARSET "test_charset");
DROP TABLE t_column_charset;
CREATE TABLE t_table_collate(c text) COLLATE test_collate;
DROP TABLE t_table_collate;
CREATE TABLE t_table_charset(c text) CHARSET test_charset;
DROP TABLE t_table_charset;
CREATE TABLE t_table_charset(c text) CHARACTER SET test_charset;
DROP TABLE t_table_charset;
CREATE TABLE t_table_row_format(c text) ROW_FORMAT test_row_format;
DROP TABLE t_table_row_format;
CREATE TABLE t_mixed_table_option(c text) ROW_FORMAT test_row_format CHARSET test_charset COLLATE test_collate;
DROP TABLE t_mixed_table_option;
CREATE TABLE t_character_set_column(c text CHARACTER SET test_character_set);
DROP TABLE t_character_set_column;
CREATE TABLE t_charset_column(c text CHARSET test_character_set);
DROP TABLE t_charset_column;
CREATE TABLE t_charset_column(c text(10) CHARSET test_character_set);
DROP TABLE t_charset_column;
CREATE TABLE t_charset_enum_column(c ENUM('a', 'b', 'c') CHARSET test_character_set);
DROP TABLE t_charset_enum_column;
-- test for integrity of function text
SELECT text(10);
SELECT text(true);
SELECT text('jack'::name);
SELECT text('hank');

-- new grammar test for group by xxx with rollup
CREATE TABLE t_with_rollup(id int, name varchar(20), area varchar(50), count int);
INSERT INTO t_with_rollup values(1, 'a', 'A', 10);
INSERT INTO t_with_rollup values(2, 'b', 'B', 15);
INSERT INTO t_with_rollup values(2, 'b', 'B', 20);
INSERT INTO t_with_rollup values(3, 'c', 'C', 50);
INSERT INTO t_with_rollup values(3, 'c', 'C', 15);
SELECT name, sum(count) FROM t_with_rollup GROUP BY ROLLUP(name);
SELECT name, sum(count) FROM t_with_rollup GROUP BY (name) WITH ROLLUP;
SELECT name, sum(count) FROM t_with_rollup GROUP BY id, name;
SELECT name, sum(count) FROM t_with_rollup GROUP BY (id, name);
SELECT name, sum(count) FROM t_with_rollup GROUP BY ROLLUP (id, name);
SELECT name, sum(count) FROM t_with_rollup GROUP BY (id, name) WITH ROLLUP;
SELECT id, sum(count) FROM t_with_rollup GROUP BY id, CUBE(area), ROLLUP(area);
SELECT id, sum(count) FROM t_with_rollup GROUP BY id, CUBE(area), area WITH ROLLUP;
SELECT id, name, area, sum(count) FROM t_with_rollup GROUP BY id, name, CUBE(name), ROLLUP(id, name, area);
SELECT id, name, area, sum(count) FROM t_with_rollup GROUP BY id, name, CUBE(name), id, name, area WITH ROLLUP;
SELECT id, name, area, sum(count) FROM t_with_rollup GROUP BY id, name, CUBE(name), (id, name, area) WITH ROLLUP;
-- test for integrity of grammar WITH DATA since we revised parser.
CREATE TABLE t_with_data AS SELECT id, name, sum(count) FROM t_with_rollup GROUP BY id, name WITH DATA;
SELECT * FROM t_with_data;
DROP TABLE t_with_rollup;
DROP TABLE t_with_data;

-- new grammar test for CREATE TABLE [AS] SELECT...
CREATE TABLE t_ctas(a int, b text);
INSERT INTO t_ctas VALUES(1, '1'), (2, '2'), (3, '3'), (4, '4');

CREATE TABLE t_ctas_new SELECT * FROM t_ctas;
SELECT COUNT(*) FROM t_ctas_new;
DROP TABLE t_ctas_new;

CREATE TABLE t_ctas_new SELECT * FROM t_ctas LIMIT 0, 2;
SELECT COUNT(*) FROM t_ctas_new;
DROP TABLE t_ctas_new;

CREATE TABLE t_ctas_new (SELECT * FROM t_ctas LIMIT 0, 2);
SELECT COUNT(*) FROM t_ctas_new;
DROP TABLE t_ctas_new;

CREATE TABLE t_ctas_new(new_c_a) (SELECT * FROM t_ctas LIMIT 0, 2);
SELECT COUNT(*) FROM t_ctas_new;
\d t_ctas_new
DROP TABLE t_ctas_new;

CREATE TABLE t_ctas_new(new_c_a, new_c_b) SELECT * FROM t_ctas LIMIT 0, 2;
SELECT COUNT(*) FROM t_ctas_new;
\d t_ctas_new
DROP TABLE t_ctas_new;

-- FAILED
CREATE TABLE t_ctas_new WITH temp_t(a, b) AS (SELECT a, b FROM t_ctas) SELECT * FROM temp_t;
CREATE TABLE t_ctas_new AS WITH temp_t(a, b) AS (SELECT a, b FROM t_ctas) SELECT * FROM temp_t;
SELECT COUNT(*) FROM t_ctas_new;
DROP TABLE t_ctas_new;

CREATE TABLE t_ctas_new (WITH temp_t(a, b) AS (SELECT a, b FROM t_ctas) SELECT * FROM temp_t);
SELECT COUNT(*) FROM t_ctas_new;
DROP TABLE t_ctas_new;
DROP TABLE t_ctas;

\c postgres
drop database db_b_new_gram_test;

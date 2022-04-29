drop database if exists test;
create database test dbcompatibility 'B';
\c test
create extension b_sql_plugin;

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

\c postgres
drop database test;

create schema db_b_new_gram_test;
set current_schema to 'db_b_new_gram_test';

-- CREATE TABLE engine test
CREATE TABLE test_engine_1 (a int) engine = InnoDB;
CREATE TABLE  IF NOT EXISTS test_engine_2 (a int) engine = InnoDB;
CREATE TYPE test_engine_type1 AS (a int, b text);
CREATE TABLE test_engine_type_table1 OF test_engine_type1 engine = InnoDB;
CREATE TYPE test_engine_type2 AS (a int, b text);
CREATE TABLE IF NOT EXISTS test_engine_type_table2 OF test_engine_type2 engine = InnoDB;
CREATE TABLE test_engine_unreserved_keyword (a int) engine = memory;
CREATE TABLE test_engine_colname_keyword (a int) engine = between;

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
ALTER TABLESPACE test_tbspace rename to test_tbspace_1 wait;
select * from pg_tablespace where spcname='test_tbspace_1';
ALTER TABLESPACE test_tbspace_1 rename to test_tbspace_2 wait engine='xx';
select * from pg_tablespace where spcname='test_tbspace_2';
ALTER TABLESPACE test_tbspace_2 rename to test_tbspace_3 engine='xx';
select * from pg_tablespace where spcname='test_tbspace_3';
ALTER TABLESPACE test_tbspace_3 rename to test_tbspace_4 engine='xx' wait;
select * from pg_tablespace where spcname='test_tbspace_4';
ALTER TABLESPACE test_tbspace_4 rename to test_tbspace_5 engine='xx' wait engine='yy';
select * from pg_tablespace where spcname='test_tbspace_5';
ALTER TABLESPACE test_tbspace_5 rename to test_tbspace_6;

ALTER TABLESPACE test_tbspace_6 set (seq_page_cost=111) wait;
select * from pg_tablespace where spcname='test_tbspace_6';
ALTER TABLESPACE test_tbspace_6 reset (seq_page_cost) wait engine='xx';
select * from pg_tablespace where spcname='test_tbspace_6';
ALTER TABLESPACE test_tbspace_6 set (seq_page_cost=222) wait engine='xx';
select * from pg_tablespace where spcname='test_tbspace_6';
ALTER TABLESPACE test_tbspace_6 reset (seq_page_cost) engine='xx';
select * from pg_tablespace where spcname='test_tbspace_6';
ALTER TABLESPACE test_tbspace_6 set (seq_page_cost=333) engine='xx';
select * from pg_tablespace where spcname='test_tbspace_6';
ALTER TABLESPACE test_tbspace_6 reset (seq_page_cost) engine='xx' wait;
select * from pg_tablespace where spcname='test_tbspace_6';
ALTER TABLESPACE test_tbspace_6 set (seq_page_cost=444) engine='xx' wait;
select * from pg_tablespace where spcname='test_tbspace_6';
ALTER TABLESPACE test_tbspace_6 reset (seq_page_cost) engine='xx' wait engine='yy';
select * from pg_tablespace where spcname='test_tbspace_6';
ALTER TABLESPACE test_tbspace_6 set (seq_page_cost=555) engine='xx' wait engine='yy';
select * from pg_tablespace where spcname='test_tbspace_6';
ALTER TABLESPACE test_tbspace_6 reset (seq_page_cost);
select * from pg_tablespace where spcname='test_tbspace_6';
ALTER TABLESPACE test_tbspace_6 set (seq_page_cost=666);
select * from pg_tablespace where spcname='test_tbspace_6';
ALTER TABLESPACE test_tbspace_6 reset (seq_page_cost) wait;
select * from pg_tablespace where spcname='test_tbspace_6';

ALTER TABLESPACE test_tbspace_6 RESIZE MAXSIZE '11M' wait;
select * from pg_tablespace where spcname='test_tbspace_6';
ALTER TABLESPACE test_tbspace_6 RESIZE MAXSIZE '22M' wait engine='xx';
select * from pg_tablespace where spcname='test_tbspace_6';
ALTER TABLESPACE test_tbspace_6 RESIZE MAXSIZE '33M' engine='xx';
select * from pg_tablespace where spcname='test_tbspace_6';
ALTER TABLESPACE test_tbspace_6 RESIZE MAXSIZE '44M' engine='xx' wait;
select * from pg_tablespace where spcname='test_tbspace_6';
ALTER TABLESPACE test_tbspace_6 RESIZE MAXSIZE '55M' engine='xx' wait engine='yy';
select * from pg_tablespace where spcname='test_tbspace_6';
ALTER TABLESPACE test_tbspace_6 RESIZE MAXSIZE '66M';
select * from pg_tablespace where spcname='test_tbspace_6';

create user u_test_tbspace identified by 'test-1234';
create user u_test_tbspace2 identified by 'test-1234';

ALTER TABLESPACE test_tbspace_6 owner to u_test_tbspace;
select usename from pg_tablespace left join pg_user on pg_tablespace.spcowner = pg_user.usesysid where spcname='test_tbspace_6';
ALTER TABLESPACE test_tbspace_6 owner to u_test_tbspace2 wait engine='xx';
select usename from pg_tablespace left join pg_user on pg_tablespace.spcowner = pg_user.usesysid where spcname='test_tbspace_6';
ALTER TABLESPACE test_tbspace_6 owner to u_test_tbspace engine='xx';
select usename from pg_tablespace left join pg_user on pg_tablespace.spcowner = pg_user.usesysid where spcname='test_tbspace_6';
ALTER TABLESPACE test_tbspace_6 owner to u_test_tbspace2 engine='xx' wait;
select usename from pg_tablespace left join pg_user on pg_tablespace.spcowner = pg_user.usesysid where spcname='test_tbspace_6';
ALTER TABLESPACE test_tbspace_6 owner to u_test_tbspace engine='xx' wait engine='yy';
select usename from pg_tablespace left join pg_user on pg_tablespace.spcowner = pg_user.usesysid where spcname='test_tbspace_6';
ALTER TABLESPACE test_tbspace_6 owner to u_test_tbspace2;
select usename from pg_tablespace left join pg_user on pg_tablespace.spcowner = pg_user.usesysid where spcname='test_tbspace_6';

DROP TABLESPACE test_tbspace_6 engine='zz';
drop user u_test_tbspace;
drop user u_test_tbspace2;

CREATE TABLESPACE test_tbspace ADD DATAFILE 'test_tbspace1';
CREATE TABLE t_tbspace(num int) TABLESPACE test_tbspace;
\d t_tbspace
DROP TABLE t_tbspace;
DROP TABLESPACE test_tbspace;

CREATE TABLESPACE test_tbspace_ibd ADD DATAFILE 'test_tbspace1.ibd';
CREATE TABLE t_tbspace(num int) TABLESPACE test_tbspace_ibd;
\d t_tbspace
DROP TABLE t_tbspace;
DROP TABLESPACE test_tbspace_ibd;

CREATE TABLESPACE test_tbspace_ibd_1 ADD DATAFILE 'abc';
DROP TABLESPACE test_tbspace_ibd_1;

CREATE TABLESPACE test_tbspace_ibd_2 ADD DATAFILE '.ibd';
DROP TABLESPACE test_tbspace_ibd_2;

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
CREATE TABLE t_table_default_collate(c text) DEFAULT COLLATE test_collate;
DROP TABLE t_table_default_collate;
CREATE TABLE t_table_charset(c text) CHARSET test_charset;
DROP TABLE t_table_charset;
CREATE TABLE t_table_charset(c text) CHARACTER SET test_charset;
DROP TABLE t_table_charset;
CREATE TABLE t_table_default_charset(c text) DEFAULT CHARACTER SET test_charset;
DROP TABLE t_table_default_charset;
CREATE TABLE t_table_default_charset(c text) DEFAULT CHARSET test_charset;
DROP TABLE t_table_default_charset;
CREATE TABLE t_table_row_format(c text) ROW_FORMAT test_row_format;
DROP TABLE t_table_row_format;
CREATE TABLE t_table_row_format_default(c text) ROW_FORMAT = DEFAULT;
DROP TABLE t_table_row_format_default;
CREATE TABLE t_table_row_format_default(c text) ROW_FORMAT DEFAULT;
DROP TABLE t_table_row_format_default;
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
-- test for alter statement
CREATE TABLE t_alter_test(c text);
ALTER TABLE t_alter_test DEFAULT COLLATE = test_collate;
ALTER TABLE t_alter_test DEFAULT CHARACTER SET = test_charset;
ALTER TABLE t_alter_test DEFAULT CHARSET = test_charset;
ALTER TABLE t_alter_test ROW_FORMAT = DEFAULT;
ALTER TABLE t_alter_test ROW_FORMAT = DYNAMIC;
ALTER TABLE t_alter_test ROW_FORMAT = FIXED;
ALTER TABLE t_alter_test ROW_FORMAT = COMPRESSED;
ALTER TABLE t_alter_test ROW_FORMAT = REDUNDANT;
ALTER TABLE t_alter_test ROW_FORMAT = COMPACT;
-- mixture and out of order
ALTER TABLE t_alter_test ROW_FORMAT = COMPACT, COLLATE = test_collate, CHARACTER SET test_charset;
DROP TABLE t_alter_test;
-- test for single quote and double quotes input
CREATE TABLE t_quote_test(c text) ROW_FORMAT = 'test_row_format' COLLATE = 'test_collate' CHARSET = 'test_charset';
DROP TABLE t_quote_test;
CREATE TABLE t_quote_test(c text) ROW_FORMAT = "test_row_format" COLLATE = "test_collate" CHARSET = "test_charset";
DROP TABLE t_quote_test;
CREATE TABLE t_quote_test(c text CHARSET 'test_charset' COLLATE 'test_collate');
DROP TABLE t_quote_test;
CREATE TABLE t_quote_test(c text CHARSET "test_charset" COLLATE "test_collate");
CREATE TABLE t_quote_test(c text CHARSET "default" COLLATE "default");
ALTER TABLE t_quote_test ROW_FORMAT = 'COMPACT', COLLATE = 'test_collate', CHARACTER SET 'test_charset';
ALTER TABLE t_quote_test ROW_FORMAT = "COMPACT", COLLATE = "test_collate", CHARACTER SET "test_charset";
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

-- create table like
CREATE TABLE t_like like t_ctas;
SELECT COUNT(*) FROM t_like;
\d t_like
DROP TABLE t_like;

CREATE TEMPORARY TABLE t_like like t_ctas;
CREATE TEMPORARY TABLE IF NOT EXISTS t_like like t_ctas;
SELECT COUNT(*) FROM t_like;
\d t_like

CREATE TABLE t_like2 like t_like;
CREATE TABLE IF NOT EXISTS t_like2 like t_ctas;
SELECT COUNT(*) FROM t_like2;
\d t_like2
DROP TABLE t_like;
DROP TABLE t_like2;

-- FAILED
CREATE TABLE t_ctas_new WITH temp_t(a, b) AS (SELECT a, b FROM t_ctas) SELECT * FROM temp_t;
CREATE TABLE t_ctas_new AS WITH temp_t(a, b) AS (SELECT a, b FROM t_ctas) SELECT * FROM temp_t;
SELECT COUNT(*) FROM t_ctas_new;
DROP TABLE t_ctas_new;

CREATE TABLE t_ctas_new (WITH temp_t(a, b) AS (SELECT a, b FROM t_ctas) SELECT * FROM temp_t);
SELECT COUNT(*) FROM t_ctas_new;
DROP TABLE t_ctas_new;
DROP TABLE t_ctas;

create schema test_m;
set current_schema to 'test_m';
create table test_unique(
    f1 int,
    f2 int,
    f3 int,
    f4 int,
    constraint con_name1 unique index u_idx_name using btree(f1),
    constraint con_name2 unique key u_key_name using btree (f2)
);
\d+ test_unique
drop table test_unique;
-- error
create table test_unique(f1 int, constraint con_name unique index index using btree(f1));
create table test_unique(f1 int, constraint con_name unique index key using btree(f1));
create table test_unique(f1 int, constraint con_name unique key index using btree(f1));
create table test_unique(f1 int, constraint con_name unique key key using btree(f1));

create table test_unique(f1 int, f2 int, f3 int, f4 int);
alter table test_unique add constraint con_name1 unique index u_idx_name using btree(f1);
alter table test_unique add constraint con_name2 unique key u_key_name using btree (f2);

-- error
alter table test_unique add constraint con_name unique index index using btree(f3);
alter table test_unique add constraint con_name unique index key using btree(f3);
alter table test_unique add constraint con_name unique key index using btree(f3);
alter table test_unique add constraint con_name unique key key using btree(f3);

-- test multi partition select for partition table
create table multi_partition_select_test(C_INT INTEGER) partition by range(C_INT)
(
    partition test_part1 values less than (400),
    partition test_part2 values less than (700),
    partition test_part3 values less than (1000)
);
insert into multi_partition_select_test values(111);
insert into multi_partition_select_test values(555);
insert into multi_partition_select_test values(888);
select a.* from multi_partition_select_test partition (test_part1) a;
select a.* from multi_partition_select_test partition (test_part1, test_part2) a;
select a.* from multi_partition_select_test partition (test_part1, test_part2, test_part3) a;
select * from multi_partition_select_test partition (test_part2, test_part3);
drop table multi_partition_select_test;

-- test multi partition select for subpartition table
CREATE TABLE ignore_range_range
(
    month_code VARCHAR2 ( 30 ) NOT NULL ,
    dept_code  VARCHAR2 ( 30 ) NOT NULL ,
    user_no    VARCHAR2 ( 30 ) NOT NULL ,
    sales_amt  int
)
    PARTITION BY RANGE (month_code) SUBPARTITION BY RANGE (dept_code)
(
  PARTITION p_201901 VALUES LESS THAN( '201901' )
  (
    SUBPARTITION p_201901_a VALUES LESS THAN( '2' ),
    SUBPARTITION p_201901_b VALUES LESS THAN( '5' )
  ),
  PARTITION p_201905 VALUES LESS THAN( '201905' )
  (
    SUBPARTITION p_201905_a VALUES LESS THAN( '2' ),
    SUBPARTITION p_201905_b VALUES LESS THAN( '5' )
  )
);
insert ignore into ignore_range_range values('201812', '1', '1', 1);
insert ignore into ignore_range_range values('201903', '4', '1', 1);
-- select from single partition
select * from ignore_range_range partition (p_201901);
-- select from single subpartition
select * from ignore_range_range partition (p_201901_a);
-- select only from subpartition
select * from ignore_range_range partition (p_201901_a, p_201905_a);
select * from ignore_range_range partition (p_201901_a, p_201905_b);
select * from ignore_range_range partition (p_201901_b, p_201905_a);
-- select from mixture of partition and subpartition
select * from ignore_range_range partition (p_201901, p_201905_a);
select * from ignore_range_range partition (p_201901, p_201905_b);
drop table ignore_range_range;

--test "authid"
create table authid(authid int);
insert into authid(authid) values(1);
insert into authid(authid) values(2);
select authid from authid where authid = 1;
create table authid_t1(c1 int);
insert into authid_t1(c1) values(1);
select c1 as authid from authid_t1 as authid;
drop table authid;
drop table authid_t1;

CREATE TABLE `Student` (
    `Sno` varchar(3) NOT NULL,
    `Sname` varchar(8) NOT NULL,
    `Ssex` varchar(2) NOT NULL,
    PRIMARY KEY (`Sno`)
);

CREATE TABLE `Score` (
    `S_no` varchar(3) NOT NULL,
    `Cno` varchar(5) NOT NULL,
    `Degree` decimal(4, 1) DEFAULT NULL,
    PRIMARY KEY (`S_no`, `Cno`),
    CONSTRAINT `Score_ibfk_1` FOREIGN KEY (`S_no`) REFERENCES `Student` (`Sno`)
);

INSERT INTO `Student` (`Sno`, `Sname`, `Ssex`)
VALUES ('101', '李军', '男'),
    ('103', '陆君', '男'),
    ('105', '匡明', '男');

INSERT INTO `Score` (`S_no`, `Cno`, `Degree`)
VALUES ('101', '3-105', '64.0'),
    ('101', '6-166', '85.0'),
    ('103', '3-105', '92.0'),
    ('103', '3-245', '86.0');
select a.Sno as no,a.Sname as name ,
                            (select  Degree from Score 
                             where Cno= '3-105' and S_no=no) 
from Student as a;

--expr
select (a.Sno+1) as no,a.Sname as name , (select  Degree from Score where Cno= '3-105' and S_no=no) from Student as a;
--multi var
select (a.Sno+a.Sname) as no,a.Sname as name , (select  Degree from Score where Cno= '3-105' and S_no=no) from Student as a;
--func
select (a.Sno is null) as no,a.Sname as name , (select  Degree from Score where Cno= '3-105' and S_no=no) from Student as a;
select hex(a.Sno) as no,a.Sname as name , (select  Degree from Score where Cno= '3-105' and S_no=no) from Student as a;
-- agg not support
select sum(a.Sno) as no,a.Sname as name , (select  Degree from Score where Cno= '3-105' and S_no=no) from Student as a;
drop table Score;
drop table Student;
drop schema test_m cascade;
drop schema db_b_new_gram_test cascade;
reset current_schema;

--to fix I9AZQB, be compatible with mysql, no error message if there exists only one index start with the given index hint
create table t1 (a int, d int, unique key bmw (d,a));
explain (costs off) select count(a) from t1 ignore index(b);
explain (costs off) select count(a) from t1 ignore index(bm);
explain (costs off) select count(a) from t1 ignore index(bmw);
explain (costs off) select count(a) from t1 ignore index(mw);
explain (costs off) select count(a) from t1 ignore index(bmws);
create index bmu1 on t1(a);
explain (costs off) select count(a) from t1 ignore index(bdx);
explain (costs off) select count(a) from t1 ignore index(bmu);
explain (costs off) select count(a) from t1 ignore index(bm);
drop table t1;
create schema create_index;
set current_schema to 'create_index';

create table t1(a int);

--error
CREATE INDEX idx_t1 ON t1(a) USING gin;
CREATE INDEX CONCURRENTLY idx_t1 ON t1(a) USING BTREE;
CREATE INDEX public.idx_t1 ON t1(a) USING BTREE;

--success
CREATE INDEX idx_t1 ON t1(a) USING BTREE;
CREATE INDEX idx_t2 ON t1(a) USING hash;
CREATE UNIQUE INDEX idx_t3 ON t1(a) USING BTREE;
drop table t1;

--create index with conflict name
CREATE TABLE temptest(col int);
CREATE INDEX i_temptest ON temptest(col);
CREATE INDEX i_temptest1 ON temptest(col);
CREATE INDEX i_temptest2 ON temptest(col);

--tmp table, should not report warning, different schema
CREATE TEMP TABLE temptest1(tcol int, index i_temptest1(tcol));
CREATE TEMP TABLE temptest(tcol int);
CREATE INDEX i_temptest ON temptest(tcol);
alter table temptest add index i_temptest2(tcol);

create table t1(id int, index idx_id(id));
--conflict, use new index name
create table t2(id int, index idx_id(id));
create index idx_id on t1(id);
create index t2_id_idx on t2(id);
alter table t2 add index t2_id_idx1(id);
-- with create if not exists, skip, no warning
create index if not exists t2_id_idx on t2(id);

-- change column will lead to delete and recreate index, should not report warning
CREATE TABLE test_prefix_table (
    id INTEGER,
    fchar CHAR(64),
    fvchar VARCHAR(64),
    ftext TEXT,
    fclob CLOB,
    fblob BLOB,
    fraw RAW,
    fbytea BYTEA,
    INDEX prefix_index_fchar_fbytea (fchar(5), fbytea(5)),
    INDEX prefix_index_fvchar (fvchar(5)),
    INDEX prefix_index_ftext (ftext(5)),
    INDEX prefix_index_fclob (fclob(5)),
    INDEX prefix_index_fblob (fblob(5))
) ;
\d test_prefix_table
ALTER TABLE test_prefix_table MODIFY ftext varchar(64);
ALTER TABLE test_prefix_table ALTER COLUMN ftext TYPE text;
\d test_prefix_table
drop table test_prefix_table;

--table does not exist
alter table doesnotexist add index i_temptest1(tcol);

--rename, still error
alter index t2_id_idx1 rename to t2_id_idx2;

-- test index
set dolphin.b_compatibility_mode to on;
set enable_opfusion to off;
set enable_indexonlyscan to off;
create table t5(a int);
create index t5_a_idx on t5(a);
insert into t5 values(1),(2),(3),(null);
\d t5
set dolphin.b_compatibility_mode to off;
\d t5
reset dolphin.b_compatibility_mode;
analyze t5;
-- use index
explain (costs off) select /*+ indexscan(t5 t5_a_idx) */* from t5 order by a;
select /*+ indexscan(t5 t5_a_idx) */* from t5 order by a;
-- index Backward
explain (costs off) select /*+ indexscan(t5 t5_a_idx) */* from t5 order by a desc;
select /*+ indexscan(t5 t5_a_idx) */* from t5 order by a desc;

drop table t5;
create table t5(a int);
create index t5_a_idx on t5(a desc);
insert into t5 values(1),(2),(3),(null);
\d t5
set dolphin.b_compatibility_mode to off;
\d t5
reset dolphin.b_compatibility_mode;
analyze t5;
-- use index
explain (costs off) select /*+ indexscan(t5 t5_a_idx) */* from t5 order by a;
select /*+ indexscan(t5 t5_a_idx) */* from t5 order by a;
-- index Backward
explain (costs off) select /*+ indexscan(t5 t5_a_idx) */* from t5 order by a desc;
select /*+ indexscan(t5 t5_a_idx) */* from t5 order by a desc;

reset enable_opfusion;
reset enable_indexonlyscan;

drop table temptest;
drop table t1;
drop table t2;

drop schema create_index cascade;
reset current_schema;
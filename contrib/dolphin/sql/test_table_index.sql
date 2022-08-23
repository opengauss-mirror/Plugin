---- b compatibility case
drop database if exists test_table_index;
create database test_table_index dbcompatibility 'b';
\c test_table_index

-- test crate normal table
create table t1(f1 int , index(f1));
create table t2(f1 int , index idx_f2(f1));
create table t3(f1 int , index idx_f3 using btree (f1));
create table t4(f1 int , index idx_f4 using btree (f1 desc));
create table t5(f1 int , key idx_f5 using btree (f1 asc));

\d+ t1
\d+ t2
\d+ t3
\d+ t4
\d+ t5

drop table if exists t1;
drop table if exists t2;
drop table if exists t3;
drop table if exists t4;
drop table if exists t5;

create table test_expr1(f1 int, f2 int, f3 int, index tbl_idx1 using btree(f1 desc, f2 asc));
create table test_expr2(f1 int, f2 int, f3 int, index tbl_idx2 using btree((abs(f1)) desc, f2 asc));
create table test_expr3(f1 int, f2 int, f3 int, index tbl_idx3 using btree((abs(f1)+10) desc, f2 asc));

\d+ test_expr1
\d+ test_expr2
\d+ test_expr3
drop table if exists test_expr1;
drop table if exists test_expr2;
drop table if exists test_expr3;

create table test_hash(f1 int, f2 int, f3 int, index tbl_idx4 using hash(f1 desc, f2 asc));
create table test_ubtree(f1 int, f2 int, f3 int, index using ubtree(f1 desc, f2 asc)) with (STORAGE_TYPE=USTORE);
create table text_column_table(f11 int, f12 varchar(20), f13 bool, index (f11)) with (orientation=column);
create table test_gist (t tsquery, s tsvector,index using gist(t));
create table test_gin (t tsquery, s tsvector,index using gin(s));
create table text_column_table(f1 int, index(f1)) with (orientation=column);
create table text_column_table_expr(f1 int, unique((f1+1))) with (orientation=column);

\d+ test_ubtree
\d+ test_gist
\d+ test_gin
\d+ text_column_table

drop table if exists test_ubtree;
drop table if exists test_gist;
drop table if exists test_gin;
drop table if exists text_column_table;
drop table if exists text_column_table_expr;

-- test crate partition table
CREATE TABLE test_partition_btree
(
    f1  INTEGER,
    f2  INTEGER,
    f3  INTEGER,
    key part_btree_idx using btree(f1)	
)
PARTITION BY RANGE(f1)
(
        PARTITION P1 VALUES LESS THAN(2450815),
        PARTITION P2 VALUES LESS THAN(2451179),
        PARTITION P3 VALUES LESS THAN(2451544),
        PARTITION P4 VALUES LESS THAN(MAXVALUE)
);
\d+ test_partition_btree
drop table if exists test_partition_btree;

CREATE TABLE test_partition_index
(
    f1  INTEGER,
    f2  INTEGER,
    f3  INTEGER,
    key part_btree_idx2 using btree(f1 desc, f2 asc)
)
PARTITION BY RANGE(f1)
(
        PARTITION P1 VALUES LESS THAN(2450815),
        PARTITION P2 VALUES LESS THAN(2451179),
        PARTITION P3 VALUES LESS THAN(2451544),
        PARTITION P4 VALUES LESS THAN(MAXVALUE)
);
\d+ test_partition_index
drop table if exists test_partition_index;

CREATE TABLE test_partition_func
(
    f1  INTEGER,
    f2  INTEGER,
    f3  INTEGER,
    key part_expr_idx using btree((abs(f1)))
)
PARTITION BY RANGE(f1)
(
        PARTITION P1 VALUES LESS THAN(2450815),
        PARTITION P2 VALUES LESS THAN(2451179),
        PARTITION P3 VALUES LESS THAN(2451544),
        PARTITION P4 VALUES LESS THAN(MAXVALUE)
);
\d+ test_partition_func
drop table if exists test_partition_func;

CREATE TABLE test_partition_expr
(
    f1  INTEGER,
    f2  INTEGER,
    f3  INTEGER,
    key part_expr_idx using btree((abs(f1)+1))
)
PARTITION BY RANGE(f1)
(
        PARTITION P1 VALUES LESS THAN(2450815),
        PARTITION P2 VALUES LESS THAN(2451179),
        PARTITION P3 VALUES LESS THAN(2451544),
        PARTITION P4 VALUES LESS THAN(MAXVALUE)
);
\d+ test_partition_expr
drop table if exists test_partition_expr;

CREATE TABLE test_partition_column
(
    f1  INTEGER,
    f2  INTEGER,
    f3  INTEGER,
    key part_column(f1)
) with (ORIENTATION = COLUMN)
PARTITION BY RANGE(f1)
(
        PARTITION P1 VALUES LESS THAN(2450815),
        PARTITION P2 VALUES LESS THAN(2451179),
        PARTITION P3 VALUES LESS THAN(2451544),
        PARTITION P4 VALUES LESS THAN(MAXVALUE)
);
\d+ test_partition_column
drop table if exists test_partition_column;

-- test crate subpartition table

CREATE TABLE test_subpartition_btree
(
month_code VARCHAR2 ( 30 ) NOT NULL ,
dept_code VARCHAR2 ( 30 ) NOT NULL ,
user_no VARCHAR2 ( 30 ) NOT NULL ,
sales_amt int,
index subpart_btree_idx using btree(month_code desc, dept_code)
)
PARTITION BY LIST (month_code) SUBPARTITION BY LIST (dept_code)
(
PARTITION p_201901 VALUES ( '201902' )
(
SUBPARTITION p_201901_a VALUES ( '1' ),
SUBPARTITION p_201901_b VALUES ( '2' )
),
PARTITION p_201902 VALUES ( '201903' )
(
SUBPARTITION p_201902_a VALUES ( '1' ),
SUBPARTITION p_201902_b VALUES ( '2' )
)
);
\d+ test_subpartition_btree
drop table if exists test_subpartition_btree;

CREATE TABLE test_subpartition_btree
(
month_code VARCHAR2 ( 30 ) NOT NULL ,
dept_code VARCHAR2 ( 30 ) NOT NULL ,
user_no VARCHAR2 ( 30 ) NOT NULL ,
sales_amt int,
index subpart_btree_idx using btree((sales_amt+1) desc, (dept_code))
)
PARTITION BY LIST (month_code) SUBPARTITION BY LIST (dept_code)
(
PARTITION p_201901 VALUES ( '201902' )
(
SUBPARTITION p_201901_a VALUES ( '1' ),
SUBPARTITION p_201901_b VALUES ( '2' )
),
PARTITION p_201902 VALUES ( '201903' )
(
SUBPARTITION p_201902_a VALUES ( '1' ),
SUBPARTITION p_201902_b VALUES ( '2' )
)
);
\d+ test_subpartition_btree
drop table if exists test_subpartition_btree;

-- test crate temporary table
create temporary table test_temporary_index1(f1 int, f2 int, f3 int, index using btree(f1 desc, f2 asc));
create local temporary table test_temporary_index2(f1 int, f2 int, f3 int, index  using btree(f1 desc, f2 asc));
create global temporary table test_temporary_index3(f1 int, f2 int, f3 int, index  using btree(f1 desc, f2 asc));
create global temporary table test_temporary_index4(f1 int, f2 int, f3 int, index  using hash(f1 desc, f2 asc));

drop table if exists test_temporary_index1;
drop table if exists test_temporary_index2;
drop table if exists test_temporary_index3;
drop table if exists test_temporary_index4;

--fail example
create table t1(a int , index idx_a (b));
create table t1(a int , index using btree idx_a(a));
create table t1(a int , index idx_a (a) using btree);
create table t1(a int , b int, index idx_a (a,a));
create table t1(a int , b int, index idx_a (a,b,a));
create table t1(a int , index idx_a (abs(a)));
create table t1(a int , index idx_a ((abs(a)),(abs(a))));
create table t1(a int , index idx_a ((abs(a)+1),(abs(a)+1)));

-- alter table add index
create table t1(a int , b int, index (a, b));
alter table  t1 add index (a);
alter table  t1 add index idx_a_1(a);
\d+ t1
drop table if exists t1;

create table t1(a int , b int, index (a, b));
alter table  t1 add index using btree(a), add index (b desc);
alter table  t1 add index idx_a_t1 using btree(a);
alter table  t1 add index idx_a_b_t1 using btree(a, b desc);
\d+ t1
drop table if exists t1;

create table t1(a int , b int, index (a, b));
alter table  t1 add key idx_a_b_t1 using btree(a, b desc);
alter table  t1 add key idx_a_b_expr_t1 using btree((abs(a+b) + a) desc);
\d+ t1
drop table if exists t1;

-- fail example
create table t1(a int , b int);
alter table  t1 add index (c);
alter table  t1 add index using btree idx1 (a);
alter table  t1 add index idx2 (a) using btree;
\d+ t1
drop table if exists t1;

-- test normal table : alter table add
-- normal row table
create table test_normal_index(f11 int, f12 varchar(20), f13 bool);
alter table test_normal_index add key using btree(f11, f12);
alter table test_normal_index add key using hash(f11);
alter table test_normal_index add key idx_f11_f12 using btree(f11, f12);
alter table test_normal_index add key((abs(f11)));
alter table test_normal_index add key((f11 * 2 + 1));
alter table test_normal_index add key(f11 desc, f12 asc);
alter table test_normal_index add key using btree(f11, f12),add unique(f11, f12),add primary key(f11, f12);
\d+ test_normal_index
drop table if exists test_normal_index;

-- not support
alter table test_normal_index add key using hash(f11, f12);

-- normal column table
create table text_column_table(f11 int, f12 varchar(20), f13 bool, index (f11)) with (orientation=column);
alter table text_column_table add key (f11);
alter table text_column_table add key (f11,f12);
alter table text_column_table add key using btree(f11);
alter table text_column_table add key using cbtree(f11);
\d+ text_column_table
drop table if exists text_column_table;

-- not support
alter table text_column_table add key using hash(f11);
alter table text_column_table add key ((abs(f11)));

-- test partition table
CREATE TABLE test_partition_btree
(
    f1  INTEGER,
    f2  INTEGER,
    f3  INTEGER
)
PARTITION BY RANGE(f1)
(
        PARTITION P1 VALUES LESS THAN(2450815),
        PARTITION P2 VALUES LESS THAN(2451179),
        PARTITION P3 VALUES LESS THAN(2451544),
        PARTITION P4 VALUES LESS THAN(MAXVALUE)
);
alter table test_partition_btree add index (f1 desc);
alter table test_partition_btree add key using btree(f1 desc, f2 asc, f3);
alter table test_partition_btree add key using btree((abs(f1)) desc, (f2 * 2 + 1) asc, f3);

\d+ test_partition_btree
drop table if exists test_partition_btree;

-- test temporary table
create temporary table test_temporary_index1(f11 int, f12 int, f13 int);

alter table test_temporary_index1 add key using btree(f11, f12);
alter table test_temporary_index1 add key using hash(f11);
alter table test_temporary_index1 add key idx_f11_f12 using btree(f11, f12);
alter table test_temporary_index1 add key((abs(f11)));
alter table test_temporary_index1 add key((f11 * 2 + 1));
alter table test_temporary_index1 add key(f11 desc, f12 asc);
alter table test_temporary_index1 add key using btree(f11, f12),add unique(f11, f12),add primary key(f11, f12);

drop table if exists test_temporary_index1;

\c contrib_regression
DROP DATABASE test_table_index;

-- test for ignore error of no partition matched
create database sql_ignore_no_matched_partition_test dbcompatibility 'B';
\c sql_ignore_no_matched_partition_test;

-- sqlbypass
set enable_opfusion = on;
set enable_partition_opfusion = on;
drop table if exists t_ignore;
CREATE TABLE t_ignore
(
    col1 integer NOT NULL,
    col2 character varying(60)
) PARTITION BY RANGE (col1)
(
    PARTITION P1 VALUES LESS THAN(5000),
    PARTITION P2 VALUES LESS THAN(10000),
    PARTITION P3 VALUES LESS THAN(15000)
)
ENABLE ROW MOVEMENT;
insert into t_ignore values(20000, 'abc');
insert into t_ignore values(3000, 'abc');
update t_ignore set col1 = 20000 where col1 = 3000;
select * from t_ignore;
explain(costs off) insert ignore into t_ignore values(20000, 'abc');
insert ignore into t_ignore values(20000, 'abc');
update ignore t_ignore set col1 = 20000 where col1 = 3000;
select * from t_ignore;

drop table if exists t_from;
create table t_from (num int, content character varying(60));
-- -- insert ignore from other tables with unmatchable rows
insert into t_from values(1000, 'valid row from t_from');
insert into t_from values(20000, 'INVALID row from t_from');
insert ignore into t_ignore select * from t_from;
select * from t_ignore;

-- no sqlbypass
set enable_opfusion = off;
set enable_partition_opfusion = off;
insert into t_ignore values(1000, 'abc');
insert into t_ignore values(30000, 'abc');
update t_ignore set col1 = 30000 where col1 = 1000;
select * from t_ignore;
explain(costs off) insert ignore into t_ignore values(30000, 'abc');
insert ignore into t_ignore values(30000, 'abc');
update ignore t_ignore set col1 = 30000 where col1 = 1000;
select * from t_ignore;

-- insert ignore from other tables with unmatchable rows
insert ignore into t_ignore select * from t_from;
select * from t_ignore;

-- test for subpartition table
drop table if exists ignore_range_range;
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
    SUBPARTITION p_201901_b VALUES LESS THAN( '3' )
  ),
  PARTITION p_201902 VALUES LESS THAN( '201902' )
  (
    SUBPARTITION p_201902_a VALUES LESS THAN( '2' ),
    SUBPARTITION p_201902_b VALUES LESS THAN( '3' )
  )
);
set enable_partition_opfusion = on;
insert ignore into ignore_range_range values('201901', '1', '1', 1);
insert ignore  into ignore_range_range values('201901', '4', '1', 1);
select * from ignore_range_range;
update ignore ignore_range_range set dept_code = '4' where dept_code = '1';
select * from ignore_range_range;
delete from ignore_range_range;
set enable_partition_opfusion = off;
insert ignore  into ignore_range_range values('201901', '1', '1', 1);
insert ignore  into ignore_range_range values('201901', '4', '1', 1);
select * from ignore_range_range;
update ignore ignore_range_range set dept_code = '4' where dept_code = '1';
select * from ignore_range_range;

-- test for ustore table
drop table if exists t_ignore;
CREATE TABLE t_ignore
(
    col1 integer NOT NULL,
    col2 character varying(60)
) WITH(storage_type=ustore) PARTITION BY RANGE (col1)
(
    PARTITION P1 VALUES LESS THAN(5000),
    PARTITION P2 VALUES LESS THAN(10000),
    PARTITION P3 VALUES LESS THAN(15000)
)
ENABLE ROW MOVEMENT;
-- test for ustore table, opfusion: on
set enable_opfusion = on;
set enable_partition_opfusion = on;
explain (costs off) insert ignore into t_ignore values(20000, 'abc');
insert ignore into t_ignore values(20000, 'abc');
select * from t_ignore;
insert into t_ignore values(3000, 'abc');
update ignore t_ignore set col1 = 20000 where col1 = 3000;
select * from t_ignore;

-- test for ustore table, opfusion: off
delete from t_ignore;
set enable_opfusion = off;
set enable_partition_opfusion = off;
explain (costs off) insert ignore into t_ignore values(20000, 'abc');
insert ignore into t_ignore values(20000, 'abc');
select * from t_ignore;
insert into t_ignore values(3000, 'abc');
update ignore t_ignore set col1 = 20000 where col1 = 3000;
select * from t_ignore;

-- test for segment table
drop table if exists t_ignore;
CREATE TABLE t_ignore
(
    col1 integer NOT NULL,
    col2 character varying(60)
) WITH(segment = on) PARTITION BY RANGE (col1)
(
    PARTITION P1 VALUES LESS THAN(5000),
    PARTITION P2 VALUES LESS THAN(10000),
    PARTITION P3 VALUES LESS THAN(15000)
)
ENABLE ROW MOVEMENT;

-- test for segment table, opfusion: on
set enable_partition_opfusion = on;
explain(costs off) insert ignore into t_ignore values(20000, 'abc');
insert ignore into t_ignore values(20000, 'abc');
select * from t_ignore;
insert into t_ignore values(3000, 'abc');
update ignore t_ignore set col1 = 20000 where col1 = 3000;
select * from t_ignore;

-- test for segment table, opfusion: off
set enable_partition_opfusion = off;
delete from t_ignore;
explain(costs off) insert ignore into t_ignore values(20000, 'abc');
insert ignore into t_ignore values(20000, 'abc');
select * from t_ignore;
insert into t_ignore values(3000, 'abc');
update ignore t_ignore set col1 = 20000 where col1 = 3000;
select * from t_ignore;

set enable_opfusion = on;
set enable_partition_opfusion = off;
drop table t_ignore;
drop table t_from;
\c postgres
drop database if exists sql_ignore_no_matched_partition_test;
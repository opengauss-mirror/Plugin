create schema partition_maxvalue_test;
set current_schema to 'partition_maxvalue_test';
--test MAXVALUE syntax
CREATE TABLE IF NOT EXISTS testsubpart
(
a int,
b int
) 
PARTITION BY RANGE(a) SUBPARTITION BY RANGE(b)
(
        PARTITION p0 VALUES LESS THAN (100)
        (
            SUBPARTITION p0_0 VALUES LESS THAN (1000),
            SUBPARTITION p0_1 VALUES LESS THAN (2000),
            SUBPARTITION p0_2 VALUES LESS THAN MAXVALUE
        ),
        PARTITION p1 VALUES LESS THAN (200)
        (
            SUBPARTITION p1_0 VALUES LESS THAN (1000),
            SUBPARTITION p1_1 VALUES LESS THAN (2000),
            SUBPARTITION p1_2 VALUES LESS THAN (MAXVALUE)
        ),
        PARTITION p2 VALUES LESS THAN MAXVALUE
        (
            SUBPARTITION p2_0 VALUES LESS THAN (1000),
            SUBPARTITION p2_1 VALUES LESS THAN (2000),
            SUBPARTITION p2_2 VALUES LESS THAN (10000)
        )
);
alter table testsubpart MODIFY PARTITION p2 ADD SUBPARTITION p2_3 VALUES LESS THAN MAXVALUE;
alter table testsubpart DROP PARTITION for maxvalue;
alter table testsubpart split subpartition p1_2 at MAXVALUE into (subpartition p1_3, subpartition p1_4);
alter table testsubpart split subpartition p0_2 values MAXVALUE into (subpartition p0_3, subpartition p0_4);
CREATE TABLE IF NOT EXISTS testpart(a int) PARTITION BY RANGE(a)
(
    PARTITION p0 VALUES LESS THAN (100),
    PARTITION p1 VALUES LESS THAN (200),
    PARTITION p2 VALUES LESS THAN MAXVALUE
);
create table testpart1(a int) partition by range(a) (partition p0 start (1) end MAXVALUE);
create table testpart2(a int) partition by range(a) (partition p0 start MAXVALUE end (200), partition p1 end(300));
alter table testpart split partition p2 at maxvalue into (partition p3, partition p4);
alter table testpart add partition p3 values less than maxvalue;
alter table testpart truncate partition for maxvalue;
alter table testpart split partition for maxvalue at maxvalue into (partition p3, partition p4);
alter table testpart split partition for maxvalue into (partition p3 values less than(1000), partition p4 values less than(2000));
alter table testpart add partition p3 start maxvalue end maxvalue;
alter table testpart add partition p3 end maxvalue;
alter table testpart add partition p3 start maxvalue;
alter table testpart move partition for maxvalue tablespace t1;
alter table testpart exchange partition for maxvalue with table t1 with validation;
alter table testpart exchange partition for maxvalue with table t1 without validation;
alter table testpart rename partition for maxvalue to t1;
CREATE TABLE testpart3 (a int) DISTRIBUTE BY RANGE(a)
(
    SLICE p0 VALUES LESS THAN (100),
    SLICE p1 VALUES LESS THAN (200),
    SLICE p2 VALUES LESS THAN MAXVALUE
);
create table testpart4(a int) DISTRIBUTE by range(a) (SLICE p0 start (1) end MAXVALUE);
create table testpart5(a int) DISTRIBUTE by range(a) (SLICE p0 start MAXVALUE end (200), SLICE p1 end(300));
drop schema partition_maxvalue_test cascade;
reset current_schema;

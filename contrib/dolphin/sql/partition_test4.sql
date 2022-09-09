drop DATABASE if exists partition_test4;
CREATE DATABASE partition_test4 dbcompatibility 'B';
\c partition_test4;
CREATE TABLE test_range_subpart
(
    a INT4 PRIMARY KEY,
    b INT4
)
PARTITION BY RANGE (a) SUBPARTITION BY HASH (b)
(
    PARTITION p1 VALUES LESS THAN (200)
    (
        SUBPARTITION s11,
        SUBPARTITION s12,
        SUBPARTITION s13,
        SUBPARTITION s14
    ),
    PARTITION p2 VALUES LESS THAN (500)
    (
        SUBPARTITION s21,
        SUBPARTITION s22
    ),
    PARTITION p3 VALUES LESS THAN (800),
    PARTITION p4 VALUES LESS THAN (1200)
    (
        SUBPARTITION s41
    )
);
CREATE TABLE test_range_part
(
    a INT4 PRIMARY KEY,
    b INT4
)
PARTITION BY RANGE (a)
(
    PARTITION p1 VALUES LESS THAN (200),
    PARTITION p2 VALUES LESS THAN (500),
    PARTITION p3 VALUES LESS THAN (800),
    PARTITION p4 VALUES LESS THAN (1200)
);
create table test_list_part (a int PRIMARY KEY, b int)
partition by list (a)
(
    partition p1 values (1, 2, 3, 4),
    partition p2 values (5, 6, 7, 8),
    partition p3 values (9, 10, 11, 12)
);
create table test_list_subpart (a int PRIMARY KEY, b int)
partition by list (a) SUBPARTITION BY HASH (b)
(
    partition p1 values (1, 2, 3, 4) (subpartition p11),
    partition p2 values (5, 6, 7, 8) (subpartition p12),
    partition p3 values (9, 10, 11, 12) (subpartition p13)
);
insert into test_range_subpart values(199,1),(499,1),(799,1),(1199,1);
insert into test_range_part values(199,1),(499,1),(799,1),(1199,1);
insert into test_list_part values(1,1),(5,1),(9,1);
insert into test_list_subpart values(1,1),(5,1),(9,1);

/*test test_range_subpart*/
alter table test_range_subpart reorganize partition p1,p2 into (partition m1 values less than(300),partition m2 values less than(600)(subpartition m21,subpartition m22));
alter table test_range_subpart reorganize partition p1,p2 into (partition m1 values less than(300),partition m2 values less than(400)(subpartition m21,subpartition m22));
alter table test_range_subpart reorganize partition p1,p2 into (partition m1 values less than(100),partition m2 values less than(500)(subpartition m21,subpartition m22));
select pg_get_tabledef('test_range_subpart');
select * from test_range_subpart subpartition(m22);
select * from test_range_subpart subpartition(m21);
select * from test_range_subpart partition(m1);
explain select /*+ indexscan(test_range_subpart test_range_subpart_pkey) */ * from test_range_subpart where a > 0;
select * from test_range_subpart;

/*test test_range_part*/
alter table test_range_part reorganize partition p1,p2 into (partition m1 values less than(300),partition m2 values less than(600));
alter table test_range_part reorganize partition p1,p2 into (partition m1 values less than(300),partition m2 values less than(400));
alter table test_range_part reorganize partition p1,p2 into (partition m1 values less than(100),partition m2 values less than(500));
select pg_get_tabledef('test_range_part');
select * from test_range_part partition(m2);
select * from test_range_part partition(m1);
explain select /*+ indexscan(test_range_part test_range_part_pkey) */ * from test_range_part where a > 0;
select * from test_range_part;

/*test test_list_part*/
alter table test_list_part reorganize partition p1,p2 into (partition m1 values(2,3),partition m2 values(4));
alter table test_list_part reorganize partition p1,p2 into (partition m1 values(2,3,5),partition m2 values(1,2,4));
alter table test_list_part reorganize partition p1,p2 into (partition m1 values(2,3,5,13),partition m2 values(1,4,9));
alter table test_list_part reorganize partition p1,p2 into (partition m1 values(2,3,5,13),partition m2 values(1,4,7));
select pg_get_tabledef('test_list_part');
select * from test_list_part partition(m2);
select * from test_list_part partition(m1);
explain select /*+ indexscan(test_list_part test_list_part_pkey) */ * from test_list_part where a > 0;
select * from test_list_part where a > 0;

/*test test_list_subpart*/
alter table test_list_subpart reorganize partition p1,p2 into (partition m1 values(2,3),partition m2 values(4));
alter table test_list_subpart reorganize partition p1,p2 into (partition m1 values(2,3,5),partition m2 values(1,2,4));
alter table test_list_subpart reorganize partition p1,p2 into (partition m1 values(2,3,5,13),partition m2 values(1,4,9));
alter table test_list_subpart reorganize partition p1,p2 into (partition m1 values(2,3,5,13)(subpartition m11,subpartition m12),partition m2 values(1,4,7));
select pg_get_tabledef('test_list_subpart');
select * from test_list_subpart partition(m2);
select * from test_list_subpart partition(m1);
select * from test_list_subpart subpartition(m11);
select * from test_list_subpart subpartition(m12);
explain select /*+ indexscan(test_list_subpart test_list_subpart_pkey) */ * from test_list_subpart where a > 0;
select * from test_list_subpart where a > 0;

-------test no-partitioned table
create table test_no_part(a int,b int);
alter table test_no_part reorganize partition p1,p2 into (partition m1 values less than(300),partition m2 values less than(600));

-------test ustore table
CREATE TABLE IF NOT EXISTS test_part_ustore
(
a int primary key not null,
b int
) 
PARTITION BY RANGE(a)
(
    PARTITION p1 VALUES LESS THAN (200),
    PARTITION p2 VALUES LESS THAN (500),
    PARTITION p3 VALUES LESS THAN (800),
    PARTITION p4 VALUES LESS THAN (1200)
) with(STORAGE_TYPE = USTORE);
insert into test_part_ustore values(199,1),(499,1),(799,1),(1199,1);
alter table test_part_ustore reorganize partition p1,p2 into (partition m1 values less than(300),partition m2 values less than(600));
alter table test_part_ustore reorganize partition p1,p2 into (partition m1 values less than(300),partition m2 values less than(400));
alter table test_part_ustore reorganize partition p1,p2 into (partition m1 values less than(100),partition m2 values less than(500));
select pg_get_tabledef('test_part_ustore');
select * from test_part_ustore partition(m2);
select * from test_part_ustore partition(m1);
explain select /*+ indexscan(test_part_ustore test_part_ustore_pkey) */ * from test_part_ustore where a > 0;
select * from test_part_ustore;

-------test segment table
CREATE TABLE IF NOT EXISTS test_part_segment
(
a int primary key not null,
b int
) 
PARTITION BY RANGE(a)
(
    PARTITION p1 VALUES LESS THAN (200),
    PARTITION p2 VALUES LESS THAN (500),
    PARTITION p3 VALUES LESS THAN (800),
    PARTITION p4 VALUES LESS THAN (1200)
) with(segment = on);
insert into test_part_segment values(199,1),(499,1),(799,1),(1199,1);
alter table test_part_segment reorganize partition p1,p2 into (partition m1 values less than(300),partition m2 values less than(600));
alter table test_part_segment reorganize partition p1,p2 into (partition m1 values less than(300),partition m2 values less than(400));
alter table test_part_segment reorganize partition p1,p2 into (partition m1 values less than(100),partition m2 values less than(500));
select pg_get_tabledef('test_part_segment');
select * from test_part_segment partition(m2);
select * from test_part_segment partition(m1);
explain select /*+ indexscan(test_part_segment test_part_segment_pkey) */ * from test_part_segment where a > 0;
select * from test_part_segment;
\c postgres;
drop DATABASE if exists partition_test4;

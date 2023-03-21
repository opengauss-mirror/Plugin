create schema partition_test4;
set current_schema to 'partition_test4';
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
select * from test_list_part where a > 0 order by a;

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
select * from test_list_subpart where a > 0 order by a desc;

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


-------test table with local index
drop table if exists b_range_hash_t01;
create table b_range_hash_t01(c1 int primary key,c2 int,c3 text)
partition by range(c1) subpartition by hash(c2)
(
partition p1 values less than (100)
(
subpartition p1_1,
subpartition p1_2
),
partition p2 values less than (200)
(
subpartition p2_1,
subpartition p2_2
),
partition p3 values less than (300)
(
subpartition p3_1,
subpartition p3_2
)
);
create index on b_range_hash_t01 (c1) global;
create index on b_range_hash_t01 (c2) local;
insert into b_range_hash_t01 values(1,2,3),(51,3,4);
select pg_get_tabledef('b_range_hash_t01');
alter table b_range_hash_t01 reorganize partition p1 into (partition m1 values less than(50) (subpartition m1_1,subpartition m1_2,subpartition m1_3),partition m2 values less than(100));
select pg_get_tabledef('b_range_hash_t01');
select * from b_range_hash_t01 partition(m1);
select * from b_range_hash_t01 partition(m2);

--test some cases about partition expr key
create table part_key_range_t1(col1 date, col2 int) partition by range(year(col1))
(
    partition p1 values less than(2000),
    partition p2 values less than(3000)
);
select pg_get_tabledef('part_key_range_t1');
alter table part_key_range_t1 add partition p3 values less than(4000);
select pg_get_tabledef('part_key_range_t1');
alter table part_key_range_t1 split partition p1 into (partition m1 values less than(1000), partition m2 values less than(2000));
select pg_get_tabledef('part_key_range_t1');
alter table part_key_range_t1 merge partitions m1,m2 into partition p1;
select pg_get_tabledef('part_key_range_t1');
alter table part_key_range_t1 reorganize partition p1,p2 into (partition m1 values less than(1000),partition m2 values less than(3000));
select pg_get_tabledef('part_key_range_t1');

create table part_key_range_t2(col1 date, col2 int) partition by range(to_days(col1))
(
    partition p1 values less than(60000),
    partition p2 values less than(80000)
);
select pg_get_tabledef('part_key_range_t2');
alter table part_key_range_t2 add partition p3 values less than(90000);
select pg_get_tabledef('part_key_range_t2');
alter table part_key_range_t2 split partition p1 into (partition m1 values less than(50000), partition m2 values less than(60000));
select pg_get_tabledef('part_key_range_t2');
alter table part_key_range_t2 merge partitions m1,m2 into partition p1;
select pg_get_tabledef('part_key_range_t2');
alter table part_key_range_t2 reorganize partition p1,p2 into (partition m1 values less than(70000),partition m2 values less than(80000));
select pg_get_tabledef('part_key_range_t2');

create table part_key_list_t1(col1 date, col2 int) partition by list(year(col1))
(
    partition p1 values(2000),
    partition p2 values(3000)
);
select pg_get_tabledef('part_key_list_t1');
alter table part_key_list_t1 add partition p3 values(4000);
select pg_get_tabledef('part_key_list_t1');
alter table part_key_list_t1 reorganize partition p1,p2 into (partition m1 values(1000,3000),partition m2 values(2000,5000));
select pg_get_tabledef('part_key_list_t1');

create table part_key_list_t2(col1 date, col2 int) partition by list(to_days(col1))
(
    partition p1 values(60000),
    partition p2 values(80000)
);
select pg_get_tabledef('part_key_list_t2');
alter table part_key_list_t2 add partition p3 values(90000);
select pg_get_tabledef('part_key_list_t2');
alter table part_key_list_t2 reorganize partition p1,p2 into (partition m1 values(10000,80000),partition m2 values(60000,50000));
select pg_get_tabledef('part_key_list_t2');

create table part_key_t1 (col1 date, col2 date) partition by range( year(col2) )
subpartition by hash( to_days(col1) )
(
    partition p1 values less than (2000)
    (
        SUBPARTITION p1sub1,
        SUBPARTITION p1sub2
    ),
    partition p2 values less than (3000)
    (
        SUBPARTITION p2sub1,
        SUBPARTITION p2sub2
    )
);
alter table part_key_t1 add partition p3 values less than(4000);
alter table part_key_t1 reorganize partition p1,p2 into (partition m1 values less than(1000), partition m2 values less than(3000));

create table b_range_at1(c1 date,c2 int)
partition by range(TO_SECONDS(c1)) (
partition p1 values less than(63838026063),
partition p2 values less than(63838026065),
partition p3 values less than(63838026067)
);
select pg_get_tabledef('b_range_at1');

--test some error cases
create table b_range_hash_t05(c1 int primary key,c2 int,c3 text)
with (segment=on)
partition by range(c1) subpartition by hash(c2)
(
partition p1 values less than (100)
(
subpartition p1_1,
subpartition p1_2
),
partition p2 values less than (200)
(
subpartition p2_1,
subpartition p2_2
),
partition p3 values less than (300)
(
subpartition p3_1,
subpartition p3_2
)
);
create index on b_range_hash_t05 (c1) global;
create index on b_range_hash_t05 (c2) local;
alter table b_range_hash_t05 reorganize partition p1 into 
(
	partition m1 values less than(50) 
	(subpartition m1_1 tablespace pg_global,subpartition m1_2,subpartition m1_3),
	partition m2 values less than(100) 
	(subpartition m2_1)
);
alter table b_range_hash_t01 reorganize partition m1 into (partition k1 values less than(2) (subpartition k1_1 values less than(2)));
alter table b_range_hash_t01 reorganize partition m1 into (partition k1 values less than(2) (subpartition k1_1 values (1)));
alter table b_range_hash_t01 reorganize partition m1 into (partition m11 values (1,2,3));
drop table if exists b_interval_t1;
create table b_interval_t1(c1 int primary key,c2 timestamp)
partition by range(c2)
interval('1 day')
(
partition p1 values less than ('1990-01-01 00:00:00'),
partition p2 values less than ('1990-01-02 00:00:00'),
partition p3 values less than ('1990-01-03 00:00:00'),
partition p4 values less than ('1990-01-04 00:00:00'),
partition p5 values less than ('1990-01-05 00:00:00'),
partition p6 values less than ('1990-01-06 00:00:00'),
partition p7 values less than ('1990-01-07 00:00:00'),
partition p8 values less than ('1990-01-08 00:00:00'),
partition p9 values less than ('1990-01-09 00:00:00'),
partition p10 values less than ('1990-01-10 00:00:00')
);
alter table b_interval_t1 reorganize partition p2,p3 into (partition m1 values less than('1990-01-03 00:00:00'));
alter table b_interval_t1 reorganize partition p4 into (partition m2 values less than('1990-01-03 12:00:00'),partition m3 values less than('1990-01-04 00:00:00'));
drop table if exists b_range_range_t01;
create table b_range_range_t01(c1 int primary key,c2 int,c3 int)
partition by range(c1) subpartition by range(c2)
(
partition p1 values less than (100)
(
subpartition p1_1 values less than (50),
subpartition p1_2 values less than (100)
),
partition p2 values less than (200)
(
subpartition p2_1 values less than (150),
subpartition p2_2 values less than (200)
),
partition p3 values less than (300)
(
subpartition p3_1 values less than (250),
subpartition p3_2 values less than (300)
)
);
alter table b_range_range_t01 reorganize partition p1 into (partition m1 values less than(100) (subpartition m1_1 values less than(50),subpartition m1_2 values less than(100)));
alter table b_range_range_t01 reorganize partition p1 into (partition m1 values less than(100) (subpartition m1_1));
drop table if exists b_range_mt1;
create table b_range_mt1(c1 int primary key,c2 int)
partition by range(c1) (
partition p0 start (0) end (100),
partition p1 start (100) end (200),
partition p2 start (200) end (300),
partition p3 start (300) end (maxvalue)
);
alter table b_range_mt1 reorganize partition p3 into (partition m1 start(300) end(400),partition m2 start(400) end(maxvalue));

drop table if exists b_range_mt2;
create table b_range_mt2(c1 int primary key,c2 int)
partition by range(2 div c1) (
partition p1 values less than(200),
partition p2 values less than(300),
partition p3 values less than (maxvalue)
);
drop schema partition_test4 cascade;
reset current_schema;
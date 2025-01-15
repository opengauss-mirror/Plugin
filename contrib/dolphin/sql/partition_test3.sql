create schema partition_test3;
set current_schema to 'partition_test3';
--test add and drop
CREATE TABLE IF NOT EXISTS test_part2
(
a int,
b int
) 
PARTITION BY RANGE(a)
(
    PARTITION p0 VALUES LESS THAN (100),
    PARTITION p1 VALUES LESS THAN (200),
    PARTITION p2 VALUES LESS THAN (300),
    PARTITION p3 VALUES LESS THAN (400)
);

CREATE TABLE IF NOT EXISTS test_subpart2
(
a int,
b int
) 
PARTITION BY RANGE(a) SUBPARTITION BY RANGE(b)
(
    PARTITION p0 VALUES LESS THAN (100)
    (
        SUBPARTITION p0_0 VALUES LESS THAN (100),
        SUBPARTITION p0_1 VALUES LESS THAN (200),
        SUBPARTITION p0_2 VALUES LESS THAN (300)
    ),
    PARTITION p1 VALUES LESS THAN (200)
    (
        SUBPARTITION p1_0 VALUES LESS THAN (100),
        SUBPARTITION p1_1 VALUES LESS THAN (200),
        SUBPARTITION p1_2 VALUES LESS THAN (300)
    ),
    PARTITION p2 VALUES LESS THAN (300)
    (
        SUBPARTITION p2_0 VALUES LESS THAN (100),
        SUBPARTITION p2_1 VALUES LESS THAN (200),
        SUBPARTITION p2_2 VALUES LESS THAN (300)
    ),
    PARTITION p3 VALUES LESS THAN (400)
    (
        SUBPARTITION p3_0 VALUES LESS THAN (100),
        SUBPARTITION p3_1 VALUES LESS THAN (200),
        SUBPARTITION p3_2 VALUES LESS THAN (300)
    )
);

--test b_compatibility drop and add partition syntax
select relname, boundaries from pg_partition where parentid in (select parentid from pg_partition where relname = 'test_part2') order by 1,2;
ALTER TABLE test_part2 DROP PARTITION p3;
select relname, boundaries from pg_partition where parentid in (select parentid from pg_partition where relname = 'test_part2') order by 1,2;
ALTER TABLE test_part2 add PARTITION (PARTITION p3 VALUES LESS THAN (400),PARTITION p4 VALUES LESS THAN (500),PARTITION p5 VALUES LESS THAN (600));
select relname, boundaries from pg_partition where parentid in (select parentid from pg_partition where relname = 'test_part2') order by 1,2;
ALTER TABLE test_part2 add PARTITION (PARTITION p6 VALUES LESS THAN (700),PARTITION p7 VALUES LESS THAN (800));
ALTER TABLE test_part2 DROP PARTITION p4,p5,p6;
select relname, boundaries from pg_partition where parentid in (select parentid from pg_partition where relname = 'test_part2') order by 1,2;
ALTER TABLE test_part2 add PARTITION (PARTITION p4 VALUES LESS THAN (500));


select relname, boundaries from pg_partition where parentid in (select oid from pg_partition where parentid in (select parentid from pg_partition where relname = 'test_subpart2')) order by 1,2;
ALTER TABLE test_subpart2 DROP SUBPARTITION p0_0;
ALTER TABLE test_subpart2 DROP SUBPARTITION p0_2, p1_0, p1_2;
select relname, boundaries from pg_partition where parentid in (select oid from pg_partition where parentid in (select parentid from pg_partition where relname = 'test_subpart2')) order by 1,2;

--test opengauss drop and add partition syntax
ALTER TABLE test_part2 add PARTITION (PARTITION p5 VALUES LESS THAN (600));
ALTER TABLE test_part2 DROP PARTITION p5, DROP PARTITION p0, DROP PARTITION p7;
select relname, boundaries from pg_partition where parentid in (select parentid from pg_partition where relname = 'test_part2') order by 1,2;
ALTER TABLE test_part2 add PARTITION p4 VALUES LESS THAN (100),add PARTITION p5 VALUES LESS THAN (200);
ALTER TABLE test_part2 add PARTITION p4 VALUES LESS THAN (500),add PARTITION p5 VALUES LESS THAN (600);
select relname, boundaries from pg_partition where parentid in (select parentid from pg_partition where relname = 'test_part2') order by 1,2;
ALTER TABLE test_subpart2 DROP SUBPARTITION p2_0, DROP SUBPARTITION p2_2, DROP SUBPARTITION p3_0;
select relname, boundaries from pg_partition where parentid in (select oid from pg_partition where parentid in (select parentid from pg_partition where relname = 'test_subpart2')) order by 1,2;

--test add partition other syntax
CREATE TABLE IF NOT EXISTS test_part2_1
(
a int,
b int
) 
PARTITION BY RANGE(a)
(
    PARTITION p0 VALUES LESS THAN (100)
);
ALTER TABLE test_part2_1 add PARTITION p1 START(200) END (300);
ALTER TABLE test_part2_1 add PARTITION p2 END (400);
ALTER TABLE test_part2_1 add PARTITION p3 START (500);
ALTER TABLE test_part2_1 add PARTITION p4 VALUES (DEFAULT);
ALTER TABLE test_part2_1 add PARTITION p5 VALUES (add(600,100));
ALTER TABLE test_part2_1 add PARTITION p4 VALUES in (DEFAULT);

CREATE TABLE IF NOT EXISTS test_subpart2_1
(
a int,
b int
) 
PARTITION BY RANGE(a) SUBPARTITION BY RANGE(b)
(
    PARTITION p0 VALUES LESS THAN (100)
    (
        SUBPARTITION p0_0 VALUES LESS THAN (100)
    )
);

ALTER TABLE test_part2_1 add PARTITION p1 VALUES LESS THAN (200) (SUBPARTITION p1_0 VALUES LESS THAN (100));
ALTER TABLE test_part2_1 add PARTITION p2 VALUES (add(600,100)) (SUBPARTITION p2_0 VALUES LESS THAN (100));
ALTER TABLE test_part2_1 add PARTITION p3 VALUES (DEFAULT) (SUBPARTITION p3_0 VALUES LESS THAN (100));
ALTER TABLE test_part2_1 add PARTITION p3 VALUES in (DEFAULT) (SUBPARTITION p3_0 VALUES LESS THAN (100));

set dolphin.sql_mode = '';
CREATE TABLE t1
(a INT,
b CHAR(2))
PARTITION BY LIST COLUMNS (a, b)
(PARTITION p0_a VALUES IN
((0, "a0"), (0, "a1"), (0, "a2"), (0, "a3"), (0, "a4"), (0, "a5"), (0, "a6"),
(0, "a7"), (0, "a8"), (0, "a9"), (0, "aa"), (0, "ab"), (0, "ac"), (0, "ad"),
(0, "ae"), (0, "af"), (0, "ag"), (0, "ah"), (0, "ai"), (0, "aj"), (0, "ak"),
(0, "al")));
ALTER TABLE t1 ADD PARTITION
(PARTITION p1_a VALUES IN
((1, "a0"), (1, "a1"), (1, "a2"), (1, "a3"), (1, "a4"), (1, "a5"), (1, "a6"),
(1, "a7"), (1, "a8"), (1, "a9"), (1, "aa"), (1, "ab"), (1, "ac"), (1, "ad"),
(1, "ae"), (1, "af"), (1, "ag"), (1, "ah"), (1, "ai"), (1, "aj"), (1, "ak"),
(1, "al")));
reset dolphin.sql_mode;
show create table t1;
insert into t1 values (0,'a0');
insert into t1 values (1,'ak');
insert into t1 values (1,'bc');

drop table t1;

drop schema partition_test3 cascade;
reset current_schema;
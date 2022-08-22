drop DATABASE if exists partition_test3;

CREATE DATABASE partition_test3 dbcompatibility 'B';
\c partition_test3;
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
select relname, boundaries from pg_partition where parentid in (select parentid from pg_partition where relname = 'test_part2');
ALTER TABLE test_part2 DROP PARTITION p3;
select relname, boundaries from pg_partition where parentid in (select parentid from pg_partition where relname = 'test_part2');
ALTER TABLE test_part2 add PARTITION (PARTITION p3 VALUES LESS THAN (400),PARTITION p4 VALUES LESS THAN (500),PARTITION p5 VALUES LESS THAN (600));
select relname, boundaries from pg_partition where parentid in (select parentid from pg_partition where relname = 'test_part2');
ALTER TABLE test_part2 add PARTITION (PARTITION p6 VALUES LESS THAN (700),PARTITION p7 VALUES LESS THAN (800));
ALTER TABLE test_part2 DROP PARTITION p4,p5,p6;
select relname, boundaries from pg_partition where parentid in (select parentid from pg_partition where relname = 'test_part2');
ALTER TABLE test_part2 add PARTITION (PARTITION p4 VALUES LESS THAN (500));


select relname, boundaries from pg_partition where parentid in (select oid from pg_partition where parentid in (select parentid from pg_partition where relname = 'test_subpart2'));
ALTER TABLE test_subpart2 DROP SUBPARTITION p0_0;
ALTER TABLE test_subpart2 DROP SUBPARTITION p0_2, p1_0, p1_2;
select relname, boundaries from pg_partition where parentid in (select oid from pg_partition where parentid in (select parentid from pg_partition where relname = 'test_subpart2'));

--test opengauss drop and add partition syntax
ALTER TABLE test_part2 add PARTITION (PARTITION p5 VALUES LESS THAN (600));
ALTER TABLE test_part2 DROP PARTITION p5, DROP PARTITION p0, DROP PARTITION p7;
select relname, boundaries from pg_partition where parentid in (select parentid from pg_partition where relname = 'test_part2');
ALTER TABLE test_part2 add PARTITION p4 VALUES LESS THAN (100),add PARTITION p5 VALUES LESS THAN (200);
ALTER TABLE test_part2 add PARTITION p4 VALUES LESS THAN (500),add PARTITION p5 VALUES LESS THAN (600);
select relname, boundaries from pg_partition where parentid in (select parentid from pg_partition where relname = 'test_part2');
ALTER TABLE test_subpart2 DROP SUBPARTITION p2_0, DROP SUBPARTITION p2_2, DROP SUBPARTITION p3_0;
select relname, boundaries from pg_partition where parentid in (select oid from pg_partition where parentid in (select parentid from pg_partition where relname = 'test_subpart2'));

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
\c postgres;
drop DATABASE if exists partition_test3;

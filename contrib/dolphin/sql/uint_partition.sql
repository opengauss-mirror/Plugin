drop database if exists uint_partition;
create database uint_partition dbcompatibility 'b';
\c uint_partition

CREATE TABLE t1
(
    col1 uint4 NOT NULL,
    col2 character varying(60)
) PARTITION BY RANGE (col1)
(
    PARTITION P1 VALUES LESS THAN(5000),
    PARTITION P2 VALUES LESS THAN(10000),
    PARTITION P3 VALUES LESS THAN(15000)
)
ENABLE ROW MOVEMENT;
insert into t1 values(3000);
insert into t1 values(11000);
insert into t1 values(9000);
insert into t1 values(16000);

select *from t1 partition(p1);
select *from t1 partition(p2);
select *from t1 partition(p3);

CREATE TABLE t2
(
    col1 uint4 NOT NULL,
    col2 character varying(60)
) PARTITION BY LIST (col1)
(
    PARTITION P1 VALUES IN(1, 2, 3),
    PARTITION P2 VALUES IN(4, 5, 6),
    PARTITION P3 VALUES IN(7, 8, 9, 10)
)
DISABLE ROW MOVEMENT;

insert into t2 values(1);
insert into t2 values(4);
insert into t2 values(10);
insert into t2 values(11);


CREATE TABLE t3
(
    col1 uint4 NOT NULL,
    col2 character varying(60)
) PARTITION BY hash (col1)
(
    PARTITION P1,
    PARTITION P2,
    PARTITION P3
);

insert into t3 values(1);
insert into t3 values(4);
insert into t3 values(10);
insert into t3 values(11);

--join and index
create table a1(a uint2, b uint4);
create table a2(a uint4, b uint8);
insert into a1 values(1, 3);
insert into a1 values(1, 4);
insert into a2 values(1, 5);
insert into a2 values(2, 3);

create index idx1 on a1(a, b);
create unique index idx2 on a1(a, b);
create index idx3 on a1 using hash (b);

--subpartition
CREATE TABLE subpartition_01 (
    col_1 uint1 primary key,
    col_2 uint2,
    col_3 uint4 unique,
    col_4 uint8
)
WITH (orientation=row, compression=no)
PARTITION BY LIST (col_2) SUBPARTITION BY HASH (col_3)
(
    PARTITION p_list_2 VALUES (1,2,3,4,5,6,7,8,9,10),
    PARTITION p_list_3 VALUES (11,12,13,14,15,16,17,18,19,20)
    (
        SUBPARTITION p_hash_3_1,
        SUBPARTITION p_hash_3_2
    ),
    PARTITION p_list_4 VALUES (21,22,23,24,25,26,27,28,29,30)
    (
        SUBPARTITION p_hash_4_1,
        SUBPARTITION p_hash_4_2,
        SUBPARTITION p_hash_4_3,
        SUBPARTITION p_hash_4_4,
        SUBPARTITION p_hash_4_5
    ),
    PARTITION p_list_5 VALUES (31,32,33,34,35,36,37,38,39,40),
    PARTITION p_list_6 VALUES (41,42,43,44,45,46,47,48,49,50)
    (
        SUBPARTITION p_hash_6_1,
        SUBPARTITION p_hash_6_2,
        SUBPARTITION p_hash_6_3,
        SUBPARTITION p_hash_6_4,
        SUBPARTITION p_hash_6_5
    ),
    PARTITION p_list_7 VALUES (DEFAULT)
);
create unique index subpartition_01_idx1 on subpartition_01(col_2, col_3, col_4) local;
create index subpartition_01_idx2 on subpartition_01(col_3, col_1) local;
create index subpartition_01_idx3 on subpartition_01(col_4) global;
INSERT INTO subpartition_01 VALUES (1, 1, 1, 1) ON DUPLICATE KEY UPDATE col_2 = 2;
select * from subpartition_01;
INSERT INTO subpartition_01 VALUES (1, 1, 1, 1) ON DUPLICATE KEY UPDATE col_2 = 2;
select * from subpartition_01;

--segment
CREATE TABLE subpartition_02 (
    col_1 integer primary key,
    col_2 uint2,
    col_3 uint8 unique,
    col_4 integer
)
WITH (orientation=row, compression=no, segment=on)
PARTITION BY LIST (col_2) SUBPARTITION BY HASH (col_3)
(
    PARTITION p_list_2 VALUES (1,2,3,4,5,6,7,8,9,10),
    PARTITION p_list_3 VALUES (11,12,13,14,15,16,17,18,19,20)
    (
        SUBPARTITION p_hash_3_1,
        SUBPARTITION p_hash_3_2
    ),
    PARTITION p_list_4 VALUES (21,22,23,24,25,26,27,28,29,30)
    (
        SUBPARTITION p_hash_4_1,
        SUBPARTITION p_hash_4_2,
        SUBPARTITION p_hash_4_3,
        SUBPARTITION p_hash_4_4,
        SUBPARTITION p_hash_4_5
    ),
    PARTITION p_list_5 VALUES (31,32,33,34,35,36,37,38,39,40),
    PARTITION p_list_6 VALUES (41,42,43,44,45,46,47,48,49,50)
    (
        SUBPARTITION p_hash_6_1,
        SUBPARTITION p_hash_6_2,
        SUBPARTITION p_hash_6_3,
        SUBPARTITION p_hash_6_4,
        SUBPARTITION p_hash_6_5
    ),
    PARTITION p_list_7 VALUES (DEFAULT)
);
create unique index subpartition_02_idx1 on subpartition_02(col_2, col_3, col_4) local;
create index subpartition_02_idx2 on subpartition_02(col_3, col_1) local;
create index subpartition_02_idx3 on subpartition_02(col_4) global;
INSERT INTO subpartition_02 VALUES (1, 1, 1, 1) ON DUPLICATE KEY UPDATE col_2 = 2;
select * from subpartition_02;
INSERT INTO subpartition_02 VALUES (1, 1, 1, 1) ON DUPLICATE KEY UPDATE col_2 = 2;
select * from subpartition_02;

--pk and unique and indexes
CREATE TABLE subpartition_03 (
    col_1 integer primary key,
    col_2 uint4,
    col_3 uint2 unique,
    col_4 integer,
    unique(col_1, col_2, col_3),
    unique(col_2, col_4)
)
WITH (orientation=row, compression=no)
PARTITION BY LIST (col_2) SUBPARTITION BY HASH (col_3)
(
    PARTITION p_list_2 VALUES (1,2,3,4,5,6,7,8,9,10),
    PARTITION p_list_3 VALUES (11,12,13,14,15,16,17,18,19,20)
    (
        SUBPARTITION p_hash_3_1,
        SUBPARTITION p_hash_3_2
    ),
    PARTITION p_list_4 VALUES (21,22,23,24,25,26,27,28,29,30)
    (
        SUBPARTITION p_hash_4_1,
        SUBPARTITION p_hash_4_2,
        SUBPARTITION p_hash_4_3,
        SUBPARTITION p_hash_4_4,
        SUBPARTITION p_hash_4_5
    ),
    PARTITION p_list_5 VALUES (31,32,33,34,35,36,37,38,39,40),
    PARTITION p_list_6 VALUES (41,42,43,44,45,46,47,48,49,50)
    (
        SUBPARTITION p_hash_6_1,
        SUBPARTITION p_hash_6_2,
        SUBPARTITION p_hash_6_3,
        SUBPARTITION p_hash_6_4,
        SUBPARTITION p_hash_6_5
    ),
    PARTITION p_list_7 VALUES (DEFAULT)
);
create unique index subpartition_03_idx1 on subpartition_03(col_2, col_3, col_4) local;
create index subpartition_03_idx2 on subpartition_03(col_3, col_1) local;
create index subpartition_03_idx3 on subpartition_03(col_4) global;
INSERT INTO subpartition_03 VALUES (1, 1, 1, 1) ON DUPLICATE KEY UPDATE col_2 = 2;
select * from subpartition_03;

drop table if exists t_unsigned_0030_5;
drop table if exists t_unsigned_0030_6;
drop table if exists t_unsigned_0030_7;
drop table if exists t_unsigned_0030_8;
create table t_unsigned_0030_5(col01 smallint unsigned)
partition by range(col01)(partition p start(1) end(255) every(50));
insert into t_unsigned_0030_5 values(1);

create table t_unsigned_0030_6(col01 smallint unsigned)
partition by range(col01)(partition p start(1) end(255) every(50));
insert into t_unsigned_0030_6 values(1);

create table t_unsigned_0030_7(col01 int unsigned)
partition by range(col01)(partition p start(1) end(255) every(50));
insert into t_unsigned_0030_7 values(1);

create table t_unsigned_0030_8(col01 bigint unsigned)
partition by range(col01)(partition p start(1) end(255) every(50));
insert into t_unsigned_0030_8 values(1);

\c postgres
drop database uint_partition;
create schema upsert;
set current_schema to 'upsert';

--normal test

--primary key and unique on multiple column 
create table normal_01(c1 int, c2 int, c3 int, c4 int unique, c5 int primary key, unique(c2,c3));
insert into normal_01 values(1,1,1,1,1) on duplicate key update c4 = 1;
select * from normal_01;
insert into normal_01 values(1,1,1,1,1) on duplicate key update c2 = 1, c3=2;
select * from normal_01;
insert into normal_01 values(1,1,1,1,1) on duplicate key update c2 = 1;
select * from normal_01;
insert into normal_01 values(1,1,1,1,1) on duplicate key update c3 = 2;
select * from normal_01;
insert into normal_01 values(1,1,1,1,1) on duplicate key update c1 =1, c2 = 1, c3=2, c4=1,c5=1;
select * from normal_01;
insert into normal_01 values(1,1,1,1,1) on duplicate key update c1 =values(c1) + values(c1), c2 = 1, c3=values(c2) + 1, c4=1,c5=1;
select * from normal_01;

--unique
create table normal_02(c1 int, c2 int, c3 int unique) ;
insert into normal_02 values(101, 1, 300) on duplicate key update c3=101;
select * from normal_02;
insert into normal_02 values(101, 1, 300) on duplicate key update c3=values(c1) * 2;
select * from normal_02;

--unique on multiple column 
create table normal_03(c1 int, c2 int, c3 int, unique(c2,c3)) ;
insert into normal_03 values(101, 1, 1) on duplicate key update c3=101;
select * from normal_03;
insert into normal_03 values(101, 1, 1) on duplicate key update c2=101;
select * from normal_03;
insert into normal_03 values(101, 1, 1) on duplicate key update c2 =1,c3=101;
select * from normal_03;
insert into normal_03 values(101, 1, 1) on duplicate key update c2 =1 + values(c2),c3 = values(c1) + values(c2) + values(c3);
select * from normal_03;

-- index on multiple column
CREATE TABLE normal_04 (
    col1 INT,
    col2 INT,
    col3 INT DEFAULT 1 NOT NULL,
    col5 BIGSERIAL
);
CREATE UNIQUE INDEX normal_04_ukey ON normal_04 (col2, col3);
INSERT INTO normal_04 VALUES (1, 1) ON DUPLICATE KEY UPDATE col3 = 3;
select * from normal_04;
INSERT INTO normal_04 VALUES (1, 1) ON DUPLICATE KEY UPDATE col3 = 3;
select * from normal_04;
INSERT INTO normal_04 VALUES (1, 3) ON DUPLICATE KEY UPDATE col3 = values(col3) * values(col3);
select * from normal_04;

--ustore test
--primary key
CREATE TABLE ustore_01 (col1 int PRIMARY KEY, col2 INT) with(storage_type=ustore);
INSERT INTO ustore_01 VALUES(1) ON DUPLICATE KEY UPDATE col1 = 1;
select * from ustore_01;
INSERT INTO ustore_01 VALUES(1) ON DUPLICATE KEY UPDATE col1 = 1;
select * from ustore_01;
INSERT INTO ustore_01 VALUES(1) ON DUPLICATE KEY UPDATE col1 = - values(col1);
select * from ustore_01;
--index on multiple column
CREATE TABLE ustore_02 (col1 INT, col2 INT) with(storage_type=ustore);
INSERT INTO ustore_02 VALUES (1, 2) ON DUPLICATE KEY UPDATE col1 = 5;
select * from ustore_02;
CREATE UNIQUE INDEX ustore_02_u_index ON ustore_02(col1, col2);
INSERT INTO ustore_02 VALUES (1, 2) ON DUPLICATE KEY UPDATE col2 = 5;
select * from ustore_02;
INSERT INTO ustore_02 VALUES (1, 5) ON DUPLICATE KEY UPDATE col2 = values(col1);
select * from ustore_02;

-- test ustore_03 with one primary key
CREATE TABLE ustore_03 (
    col1 INT,
    col2 INT PRIMARY KEY,
    col3 INT DEFAULT 1,
    col5 BIGSERIAL
)  with(storage_type=ustore);

INSERT INTO ustore_03 VALUES (1, 1) ON DUPLICATE KEY UPDATE col2 = 3;
select * from ustore_03;
INSERT INTO ustore_03 VALUES (1, 1) ON DUPLICATE KEY UPDATE ustore_03.col2 = 3;
select * from ustore_03;
INSERT INTO ustore_03 (col2, col3, col5)
    VALUES  (10, 10, 10),
            (20, 20, 20),
            (30, 30, 30)
    ON DUPLICATE KEY UPDATE
        col1 = 100,
        col2 = 100,
        col3 = 100,
        col5 = 100;
select * from ustore_03;
INSERT INTO ustore_03 (col2, col3, col5)
    VALUES  (10, 10, 10),
            (20, 20, 20),
            (30, 30, 30)
    ON DUPLICATE KEY UPDATE
        col1 = 100,
        col2 = values(col2) + 1,
        col3 = 100,
        col5 = 100;
select * from ustore_03;

-- test ustore_04 with one primary index with two columns
CREATE TABLE ustore_04 (
    col1 INT,
    col2 INT,
    col3 INT DEFAULT 1,
    col5 BIGSERIAL,
    PRIMARY KEY (col2, col3)
)  with(storage_type=ustore);
INSERT INTO ustore_04 VALUES (1, 1) ON DUPLICATE KEY UPDATE col3 = 3;
select * from ustore_04;
INSERT INTO ustore_04 VALUES (1, 1) ON DUPLICATE KEY UPDATE col3 = 3;
select * from ustore_04;
INSERT INTO ustore_04 VALUES (1, 1, 3) ON DUPLICATE KEY UPDATE col3 = values(col1) + values(col2);
select * from ustore_04;
-- test t6 with one more index
CREATE TABLE t6 (
    col1 INT,
    col2 INT DEFAULT 1,
    col3 INT DEFAULT 1,
    col5 INT DEFAULT 1,
    col6 INT DEFAULT 2,
    col7 TEXT,
	PRIMARY KEY(col1, col3)
) with(storage_type=ustore);
CREATE UNIQUE INDEX u_t6_index1 ON t6(col1, col5, col6);
INSERT INTO t6 (col1) VALUES (1), (2), (3), (4), (5);
select * from t6;
INSERT INTO t6 (col1) VALUES (1), (2), (3), (4), (5) ON DUPLICATE KEY UPDATE col6 = power(col1, col2);
SELECT col1, col2, col3, col6, col7 FROM t6 ORDER BY col3;

INSERT INTO t6 (col1) VALUES (6), (6), (6) ON DUPLICATE KEY UPDATE col2 = col1 + col3, col6 =  col2 * 10;
SELECT col1, col2, col3, col6, col7 FROM t6 WHERE col1 = 6 ORDER BY col3;


--segment test

--primary key and unique on multiple column 
create table segment_01(c1 int, c2 int, c3 int, c4 int unique, c5 int primary key, unique(c2,c3)) with(segment = on);
insert into segment_01 values(1,1,1,1,1) on duplicate key update c4 = 1;
select * from segment_01;
insert into segment_01 values(1,1,1,1,1) on duplicate key update c2 = 1, c3=2;
select * from segment_01;
insert into segment_01 values(1,1,1,1,1) on duplicate key update c2 = 1;
select * from segment_01;
insert into segment_01 values(1,1,1,1,1) on duplicate key update c3 = 2;
select * from segment_01;
insert into segment_01 values(1,1,1,1,1) on duplicate key update c1 =1, c2 = 1, c3=2, c4=1,c5=1;
select * from segment_01;

-- unique
create table segment_02(c1 int, c2 int, c3 int, unique(c2,c3))  with(segment = on);
insert into segment_02 values(101, 1, 1) on duplicate key update c3=101;
select * from segment_02;
insert into segment_02 values(101, 1, 1) on duplicate key update c2=101;
select * from segment_02;
insert into segment_02 values(101, 1, 1) on duplicate key update c2 =1,c3=101;
select * from segment_02;

-- index on multiple column
CREATE TABLE segment_03 (
    col1 INT,
    col2 INT,
    col3 INT DEFAULT 1 NOT NULL,
    col5 BIGSERIAL
) with(segment = on);
CREATE UNIQUE INDEX segment_03_ukey ON segment_03 (col2, col3);
INSERT INTO segment_03 VALUES (1, 1) ON DUPLICATE KEY UPDATE col3 = 3;
select * from segment_03;
INSERT INTO segment_03 VALUES (1, 1) ON DUPLICATE KEY UPDATE col3 = 3;
select * from segment_03;

-- indexex on multiple column
CREATE TABLE segment_04 (
    col1 INT,
    col2 INT,
    col3 INT DEFAULT 1 NOT NULL,
    col5 BIGSERIAL
) with(segment = on);
CREATE UNIQUE INDEX segment_04_ukey1 ON segment_04 (col2, col3);
CREATE UNIQUE INDEX segment_04_ukey2 ON segment_04 (col3, col5);
INSERT INTO segment_04 VALUES (1, 1) ON DUPLICATE KEY UPDATE col3 = 3;
select * from segment_04;
INSERT INTO segment_04 VALUES (1, 1) ON DUPLICATE KEY UPDATE col3 = 3;
select * from segment_04;


--partition
--LOCAL
CREATE TABLE partition_01
(
    num int,
    data1 text,
    data2 text
)
PARTITION BY RANGE(num)
(
    PARTITION num1 VALUES LESS THAN(10000),
    PARTITION num2 VALUES LESS THAN(20000),
    PARTITION num3 VALUES LESS THAN(30000)
);

CREATE UNIQUE INDEX partition_01_index ON partition_01 (num) LOCAL
(
    PARTITION data2_index_1,
    PARTITION data2_index_2,
    PARTITION data2_index_3
);
INSERT INTO partition_01 VALUES (1, '1', '1') ON DUPLICATE KEY UPDATE data2 = 'a';
select * from partition_01; 
INSERT INTO partition_01 VALUES (1, '1', '1') ON DUPLICATE KEY UPDATE data2 = 'a';
select * from partition_01; 


--GLOBAL
CREATE TABLE partition_02
(
    num int,
    data1 text,
    data2 text
)
PARTITION BY RANGE(num)
(
    PARTITION num1 VALUES LESS THAN(10000),
    PARTITION num2 VALUES LESS THAN(20000),
    PARTITION num3 VALUES LESS THAN(30000)
);

CREATE UNIQUE INDEX partition_02_index ON partition_02 (num) GLOBAL;
INSERT INTO partition_02 VALUES (1, '1', '1') ON DUPLICATE KEY UPDATE num = 2;
select * from partition_02; 
INSERT INTO partition_02 VALUES (1, '1', '1') ON DUPLICATE KEY UPDATE num = 2;
select * from partition_02; 

--pk and global index
CREATE TABLE partition_03
(
    num int primary key,
    data1 text,
    data2 text
)
PARTITION BY RANGE(num)
(
    PARTITION num1 VALUES LESS THAN(10000),
    PARTITION num2 VALUES LESS THAN(20000),
    PARTITION num3 VALUES LESS THAN(30000)
);

CREATE UNIQUE INDEX partition_03_index ON partition_03 (num, data1) GLOBAL;
INSERT INTO partition_03 VALUES (1, '1', '1') ON DUPLICATE KEY UPDATE data1 = 'a';
select * from partition_03; 
INSERT INTO partition_03 VALUES (1, '1', '1') ON DUPLICATE KEY UPDATE data1 = 'a';
select * from partition_03; 

--pk and indexes
CREATE TABLE partition_04
(
    num int primary key,
    data1 text,
    data2 text
)
PARTITION BY RANGE(num)
(
    PARTITION num1 VALUES LESS THAN(10000),
    PARTITION num2 VALUES LESS THAN(20000),
    PARTITION num3 VALUES LESS THAN(30000)
);
CREATE UNIQUE INDEX partition_04_index ON partition_04 (num, data1) LOCAL;
CREATE UNIQUE INDEX partition_04_index2 ON partition_04 (num, data2) GLOBAL;
INSERT INTO partition_04 VALUES (1, '1', '1') ON DUPLICATE KEY UPDATE data2 = 'a';
select * from partition_04; 
INSERT INTO partition_04 VALUES (1, '1', '1') ON DUPLICATE KEY UPDATE data2 = 'a';
select * from partition_04; 

--segment
CREATE TABLE partition_05
(
    num int primary key,
    data1 text,
    data2 text
) with(segment=on)
PARTITION BY RANGE(num)
(
    PARTITION num1 VALUES LESS THAN(10000),
    PARTITION num2 VALUES LESS THAN(20000),
    PARTITION num3 VALUES LESS THAN(30000)
);
CREATE UNIQUE INDEX partition_05_index ON partition_05 using btree(num, data1) LOCAL;
CREATE UNIQUE INDEX partition_05_index2 ON partition_05 using btree(num, data2) LOCAL;
INSERT INTO partition_05 VALUES (1, '1', '1') ON DUPLICATE KEY UPDATE data2 = 'a';
select * from partition_05; 
INSERT INTO partition_05 VALUES (1, '1', '1') ON DUPLICATE KEY UPDATE data2 = 'a';
select * from partition_05; 

CREATE TABLE partition_06
(
    num int primary key,
    data1 text unique,
    data2 text,
    unique(data1, data2)
)
PARTITION BY RANGE(num)
(
    PARTITION num1 VALUES LESS THAN(10000),
    PARTITION num2 VALUES LESS THAN(20000),
    PARTITION num3 VALUES LESS THAN(30000)
);
CREATE UNIQUE INDEX partition_06_index ON partition_06 (num, data1) LOCAL;
CREATE UNIQUE INDEX partition_06_index2 ON partition_06 (num, data2) GLOBAL;
INSERT INTO partition_06 VALUES (1, '1', '1') ON DUPLICATE KEY UPDATE data2 = 'a';
select * from partition_06; 
INSERT INTO partition_06 VALUES (2, '1', '1') ON DUPLICATE KEY UPDATE data2 = 'a';
select * from partition_06; 
INSERT INTO partition_06 VALUES (1, '2', '1') ON DUPLICATE KEY UPDATE num = 3;
select * from partition_06; 

--subpartition
CREATE TABLE subpartition_01 (
    col_1 integer primary key,
    col_2 integer,
    col_3 character varying(30) unique,
    col_4 integer
)
WITH (orientation=row, compression=no)
PARTITION BY LIST (col_2) SUBPARTITION BY HASH (col_3)
(
    PARTITION p_list_1 VALUES (-1,-2,-3,-4,-5,-6,-7,-8,-9,-10)
    (
        SUBPARTITION p_hash_1_1,
        SUBPARTITION p_hash_1_2,
        SUBPARTITION p_hash_1_3
    ),
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
INSERT INTO subpartition_01 VALUES (1, 1, '1', 1) ON DUPLICATE KEY UPDATE col_2 = 2;
select * from subpartition_01;
INSERT INTO subpartition_01 VALUES (1, 1, '1', 1) ON DUPLICATE KEY UPDATE col_2 = 2;
select * from subpartition_01;

--segment
CREATE TABLE subpartition_02 (
    col_1 integer primary key,
    col_2 integer,
    col_3 character varying(30) unique,
    col_4 integer
)
WITH (orientation=row, compression=no, segment=on)
PARTITION BY LIST (col_2) SUBPARTITION BY HASH (col_3)
(
    PARTITION p_list_1 VALUES (-1,-2,-3,-4,-5,-6,-7,-8,-9,-10)
    (
        SUBPARTITION p_hash_1_1,
        SUBPARTITION p_hash_1_2,
        SUBPARTITION p_hash_1_3
    ),
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
INSERT INTO subpartition_02 VALUES (1, 1, '1', 1) ON DUPLICATE KEY UPDATE col_2 = 2;
select * from subpartition_02;
INSERT INTO subpartition_02 VALUES (1, 1, '1', 1) ON DUPLICATE KEY UPDATE col_2 = 2;
select * from subpartition_02;

--pk and unique and indexes
CREATE TABLE subpartition_03 (
    col_1 integer primary key,
    col_2 integer,
    col_3 character varying(30) unique,
    col_4 integer,
    unique(col_1, col_2, col_3),
    unique(col_2, col_4)
)
WITH (orientation=row, compression=no)
PARTITION BY LIST (col_2) SUBPARTITION BY HASH (col_3)
(
    PARTITION p_list_1 VALUES (-1,-2,-3,-4,-5,-6,-7,-8,-9,-10)
    (
        SUBPARTITION p_hash_1_1,
        SUBPARTITION p_hash_1_2,
        SUBPARTITION p_hash_1_3
    ),
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

-- Test column name case sensitivity.
DROP TABLE IF EXISTS "TAB_STUDENT";
CREATE TABLE "TAB_STUDENT" (
"DI_ID" int8 NOT NULL,
"DC_NAME" varchar(100),
"DC_SEX" text,
"DI_AGE" int4
);
INSERT INTO "TAB_STUDENT" VALUES (1,'first','man',9) ON DUPLICATE KEY UPDATE DI_AGE = 10;
INSERT INTO "TAB_STUDENT" VALUES (2,'second','woman',9) ON DUPLICATE KEY UPDATE di_age = 10;
INSERT INTO "TAB_STUDENT" VALUES (3,'third','woman',9) ON DUPLICATE KEY UPDATE DI_age = 10;
SELECT * FROM "TAB_STUDENT";
DROP TABLE "TAB_STUDENT";

create unique index subpartition_03_idx1 on subpartition_03(col_2, col_3, col_4) local;
create index subpartition_03_idx2 on subpartition_03(col_3, col_1) local;
create index subpartition_03_idx3 on subpartition_03(col_4) global;
INSERT INTO subpartition_03 VALUES (1, 1, '1', 1) ON DUPLICATE KEY UPDATE col_2 = 2;
select * from subpartition_03;
INSERT INTO subpartition_03 VALUES (1, 1, '1', 1) ON DUPLICATE KEY UPDATE col_2 = 2;
select * from subpartition_03;
INSERT INTO subpartition_03 VALUES (1, 2, '1', 1) ON DUPLICATE KEY UPDATE col_1 = 2;
select * from subpartition_03;
drop schema upsert cascade;
reset current_schema;
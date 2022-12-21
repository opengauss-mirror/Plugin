create schema db_chk_tbl;
set current_schema to 'db_chk_tbl';

CREATE SCHEMA tst_schema1;
SET SEARCH_PATH TO tst_schema1;

CREATE TABLE tst_t1
(
id int,
name VARCHAR(20),
addr text,
phone text,
addr_code text
);

CREATE TABLE tst_t2 AS SELECT * FROM tst_t1;
CREATE VIEW tst_v1 AS SELECT * FROM tst_t1;

CREATE TABLE blog
(
id int,
title text,
content text
);

CREATE TABLE blog_v2 AS SELECT * FROM blog;

INSERT INTO tst_t1 values(2022001, 'tst_name1', 'tst_addr1', '15600000001', '000001');
INSERT INTO tst_t1 values(2022002, 'tst_name2', 'tst_addr2', '15600000002', '000002');
INSERT INTO tst_t1 values(2022003, 'tst_name3', 'tst_addr3', '15600000003', '000003');
INSERT INTO tst_t1 values(2022004, 'tst_name4', 'tst_addr4', '15600000004', '000004');

INSERT INTO tst_t2 (SELECT * FROM tst_t1 ORDER BY id DESC);

CHECKSUM TABLE tst_t1,tst_t2,xxx;
CHECKSUM TABLE tst_t1,tst_t2 QUICK;
CHECKSUM TABLE tst_t1,tst_t2 EXTENDED;

-- INSERT FOR toast
INSERT INTO blog values(1, 'title1', '01234567890'), (2, 'title2', '0987654321');

CREATE OR REPLACE FUNCTION loop_insert_result_toast(n integer)
RETURNS integer AS $$
DECLARE
    count integer := 0;
BEGIN
    LOOP
        EXIT WHEN count = n;
        UPDATE blog SET content=content||content where id = 2;
        count := count + 1;
    END LOOP;
    RETURN count;
END; $$
LANGUAGE PLPGSQL;

select loop_insert_result_toast(16);

INSERT INTO blog_v2 (SELECT * FROM blog);

CHECKSUM TABLE blog,blog_v2,xxx;

TRUNCATE TABLE tst_t1;
CHECKSUM TABLE tst_t1,tst_t2,xxx;

-- TEST SEGMENT TABLE
CREATE TABLE tst_seg_t1(id int, name VARCHAR(20)) WITH (segment=on);
CREATE TABLE tst_seg_t2(id int, name VARCHAR(20)) WITH (segment=on);

INSERT INTO tst_seg_t1 values(2022001, 'name_example_1');
INSERT INTO tst_seg_t1 values(2022002, 'name_example_2');
INSERT INTO tst_seg_t1 values(2022003, 'name_example_3');

INSERT INTO tst_seg_t2 (SELECT * FROM tst_seg_t1);
CHECKSUM TABLE tst_seg_t1, tst_seg_t2;

-- TEST PART-1 TABLE
CREATE TABLE tst_part_t1
(
c1 int,
c2 CHAR(2)
)
partition by range (c1)
(
partition part_t1_p0 values less than (50),
partition part_t1_p1 values less than (100),
partition part_t1_p2 values less than (150)
);

CREATE TABLE tst_part_t2
(
c1 int,
c2 CHAR(2)
)
partition by range (c1)
(
partition part_t1_p0 values less than (100),
partition part_t1_p1 values less than (200)
);

INSERT INTO tst_part_t1 values(30,'A'),(80,'B'),(120,'C');
INSERT INTO tst_part_t2 (SELECT * FROM tst_part_t1);

CHECKSUM TABLE tst_part_t1,tst_part_t2;

-- TEST PART-2 TABLE
CREATE TABLE sec_part_t1
(
month_code VARCHAR2 (30) NOT NULL ,
dept_code  VARCHAR2 (30) NOT NULL ,
user_no    VARCHAR2 (30) NOT NULL ,
sales_amt  int
)
PARTITION BY RANGE (month_code) SUBPARTITION BY RANGE (dept_code)
(
 PARTITION p_201901 VALUES LESS THAN( '201903' )
 (
   SUBPARTITION p_201901_a VALUES LESS THAN( '2' ),
   SUBPARTITION p_201901_b VALUES LESS THAN( '3' )
 ),
 PARTITION p_201902 VALUES LESS THAN( '201904' )
 (
   SUBPARTITION p_201902_a VALUES LESS THAN( '2' ),
   SUBPARTITION p_201902_b VALUES LESS THAN( '3' )
 )
);

CREATE TABLE sec_part_t2
(
    month_code VARCHAR2 ( 30 ) NOT NULL ,
    dept_code  VARCHAR2 ( 30 ) NOT NULL ,
    user_no    VARCHAR2 ( 30 ) NOT NULL ,
    sales_amt  int
)
PARTITION BY RANGE (month_code) SUBPARTITION BY RANGE (dept_code)
(
  PARTITION p_201901 VALUES LESS THAN( '201903' )
  (
    SUBPARTITION p_201901_a VALUES LESS THAN( '3' ),
    SUBPARTITION p_201901_b VALUES LESS THAN( '4' )
  ),
  PARTITION p_201902 VALUES LESS THAN( '201904' )
  (
    SUBPARTITION p_201902_a VALUES LESS THAN( '3' ),
    SUBPARTITION p_201902_b VALUES LESS THAN( '4' )
  )
);

insert into sec_part_t1 values('201902', '1', '1', 1);
insert into sec_part_t1 values('201902', '2', '1', 1);
insert into sec_part_t1 values('201902', '1', '1', 1);
insert into sec_part_t1 values('201903', '2', '1', 1);
insert into sec_part_t1 values('201903', '1', '1', 1);
insert into sec_part_t1 values('201903', '2', '1', 1);

INSERT INTO sec_part_t2 (SELECT * FROM sec_part_t1);

CHECKSUM TABLE sec_part_t1,sec_part_t2;

-- TEST USTORE TABLE
CREATE TABLE tst_ust_t1(id int, name VARCHAR(20)) WITH (storage_type=ustore);
CREATE TABLE tst_ust_t2(id int, name VARCHAR(20)) WITH (storage_type=ustore);

INSERT INTO tst_ust_t1 VALUES(2020001, 'ust_name_1');
INSERT INTO tst_ust_t1 VALUES(2020002, 'ust_name_2');
INSERT INTO tst_ust_t1 VALUES(2020003, 'ust_name_3');
INSERT INTO tst_ust_t1 VALUES(2020004, 'ust_name_4');

INSERT INTO tst_ust_t2 (SELECT * FROM tst_ust_t1);

CHECKSUM TABLE tst_ust_t1,tst_ust_t2;

-- TEST TEMP TABLE
CREATE TEMPORARY TABLE tst_tmp_t1(id int, name VARCHAR(20));
CREATE TEMPORARY TABLE tst_tmp_t2(id int, name VARCHAR(20));

INSERT INTO tst_tmp_t1 VALUES(2020001, 'ust_tmp_1');
INSERT INTO tst_tmp_t1 VALUES(2020002, 'ust_tmp_2');
INSERT INTO tst_tmp_t1 VALUES(2020003, 'ust_tmp_3');
INSERT INTO tst_tmp_t2 (SELECT * FROM tst_tmp_t1);

CHECKSUM TABLE tst_tmp_t1,tst_tmp_t2;

-- TEST SAME COLNAME WITH TABLENAME
CREATE TABLE t_same(t_same int, name text);
CREATE TABLE t_same_cmp(t_same int, name text);

INSERT INTO t_same VALUES(2022001, 'same check');
INSERT INTO t_same_cmp VALUES(2022001, 'same check');

CHECKSUM TABLE t_same, t_same_cmp;

drop schema db_chk_tbl cascade;
reset current_schema;

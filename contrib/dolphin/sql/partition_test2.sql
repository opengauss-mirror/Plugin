drop DATABASE if exists partition_test2;

CREATE DATABASE partition_test2 dbcompatibility 'B';
\c partition_test2;
CREATE TABLE IF NOT EXISTS test_part1
(
a int,
b int
) 
PARTITION BY RANGE(a)
(
    PARTITION p0 VALUES LESS THAN (100),
    PARTITION p1 VALUES LESS THAN (200),
    PARTITION p2 VALUES LESS THAN (300)
);

CREATE TABLE IF NOT EXISTS test_subpart
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
			SUBPARTITION p0_2 VALUES LESS THAN (10000)
		),
		PARTITION p1 VALUES LESS THAN (200)
		(
			SUBPARTITION p1_0 VALUES LESS THAN (1000),
			SUBPARTITION p1_1 VALUES LESS THAN (2000),
			SUBPARTITION p1_2 VALUES LESS THAN (10000)
		),
		PARTITION p2 VALUES LESS THAN (300)
		(
			SUBPARTITION p2_0 VALUES LESS THAN (1000),
			SUBPARTITION p2_1 VALUES LESS THAN (2000),
			SUBPARTITION p2_2 VALUES LESS THAN (10000)
		)
);

create table test_no_part1(a int, b int);
insert into test_part1 values(99,1),(199,1),(299,1);
select * from test_part1;
insert into test_subpart values(99,199),(199,1999),(299,2999);
select * from test_subpart;
--test b database truncate partition syntax
ALTER TABLE test_part1 truncate PARTITION p0, p1;
select * from test_part1;
insert into test_part1 (with RECURSIVE t_r(i,j) as(values(0,1) union all select i+1,j+2 from t_r where i < 20) select * from t_r);
select * from test_part1;
ALTER TABLE test_part1 truncate PARTITION all;
select * from test_part1;
ALTER TABLE test_subpart truncate PARTITION p0, p1, p2_2, p2_1;
select * from test_subpart;
insert into test_subpart (with RECURSIVE t_r(i,j) as(values(0,1) union all select i+1,j+2 from t_r where i < 20) select * from t_r);
select * from test_subpart;
ALTER TABLE test_subpart truncate PARTITION all;
select * from test_subpart;
--test opengauss truncate partition syntax
insert into test_part1 values(99,1),(199,1);
select * from test_part1;
ALTER TABLE test_part1 truncate PARTITION p0, truncate PARTITION p1;
select * from test_part1;
insert into test_subpart values(99,1),(199,1);
select * from test_subpart;
ALTER TABLE test_subpart truncate PARTITION p0, truncate PARTITION p1;
select * from test_subpart;

--test b database exchange partition syntax
insert into test_part1 values(99,1),(199,1),(299,1);
alter table test_part1 exchange partition p2 with table test_no_part1 without validation;
select * from test_part1;
select * from test_no_part1;
alter table test_part1 exchange partition p2 with table test_no_part1 without validation;
select * from test_part1;
select * from test_no_part1;
alter table test_part1 exchange partition p2 with table test_no_part1 with validation;
select * from test_part1;
select * from test_no_part1;
alter table test_part1 exchange partition p2 with table test_no_part1 with validation;
select * from test_part1;
select * from test_no_part1;
--test opengauss exchange partition syntax
alter table test_part1 exchange partition (p2) with table test_no_part1 without validation;
select * from test_part1;
select * from test_no_part1;
alter table test_part1 exchange partition (p2) with table test_no_part1 without validation;
select * from test_part1;
select * from test_no_part1;
alter table test_part1 exchange partition (p2) with table test_no_part1 with validation;
select * from test_part1;
select * from test_no_part1;
alter table test_part1 exchange partition (p2) with table test_no_part1 with validation;
select * from test_part1;
select * from test_no_part1;
--test b database exchange partition syntax
insert into test_subpart values(99,199),(199,1999),(299,2999);
alter table test_subpart exchange partition p2 with table test_no_part1 without validation;
--test opengauss exchange partition syntax
alter table test_subpart exchange partition (p2) with table test_no_part1 without validation;


--test b database analyze partition syntax
alter table test_part1 analyze partition p0,p1;
alter table test_part1 analyze partition all;
alter table test_subpart analyze partition all;
--test opengauss analyze partition syntax
analyze test_part1 partition (p1);
analyze test_subpart partition (p1);



-------test USTORE table
CREATE TABLE IF NOT EXISTS test_part_ustore
(
a int,
b int
) 
PARTITION BY RANGE(a)
(
    PARTITION p0 VALUES LESS THAN (100),
    PARTITION p1 VALUES LESS THAN (200),
    PARTITION p2 VALUES LESS THAN (300)
) with(STORAGE_TYPE = USTORE);
create table test_no_part2(a int, b int) with(STORAGE_TYPE = USTORE);
insert into test_part_ustore (with RECURSIVE t_r(i,j) as(values(0,1) union all select i+1,j+2 from t_r where i < 20) select * from t_r);
select * from test_part_ustore;
ALTER TABLE test_part_ustore truncate PARTITION all;
select * from test_part_ustore;
insert into test_part_ustore values(99,1),(199,1),(299,1);
alter table test_part_ustore exchange partition p2 with table test_no_part2 without validation;
select * from test_part_ustore;
select * from test_no_part2;
alter table test_part_ustore analyze partition p0,p1;
alter table test_part_ustore analyze partition all;



-------test segment table
CREATE TABLE IF NOT EXISTS test_part_segment
(
a int,
b int
) 
PARTITION BY RANGE(a)
(
    PARTITION p0 VALUES LESS THAN (100),
    PARTITION p1 VALUES LESS THAN (200),
    PARTITION p2 VALUES LESS THAN (300)
) with(segment = on);
insert into test_part_segment (with RECURSIVE t_r(i,j) as(values(0,1) union all select i+1,j+2 from t_r where i < 20) select * from t_r);
select * from test_part_segment;
ALTER TABLE test_part_segment truncate PARTITION all;
select * from test_part_segment;
insert into test_part_segment values(99,1),(199,1),(299,1);
alter table test_part_segment exchange partition p2 with table test_no_part1 without validation;
select * from test_part_segment;
select * from test_no_part1;
alter table test_part_segment analyze partition p0,p1;
alter table test_part_segment analyze partition all;


-------test list partition tables
CREATE TABLE test_part_list (a int, b int)
PARTITION BY list(a)
(
PARTITION p1 VALUES (2000,2001,2002,2003),
PARTITION p2 VALUES (3000,3001,3002,3003),
PARTITION p3 VALUES (4000,4001,4002,4003),
PARTITION p4 VALUES (5000,5001,5002,5003)
);
insert into test_part_list values(2000,1),(3000,2),(4000,3),(5000,4);
select * from test_part_list order by 1,2;
ALTER TABLE test_part_list truncate PARTITION all;
select * from test_part_list order by 1,2;
insert into test_part_list values(2000,1),(3000,2),(4000,3),(5000,4);
alter table test_part_list exchange partition p2 with table test_no_part1 without validation;
select * from test_part_list order by 1,2;
select * from test_no_part1;
alter table test_part_list analyze partition p1,p2;
alter table test_part_list analyze partition all;
-------test hash partition tables
CREATE TABLE IF NOT EXISTS test_part_hash
(
a int,
b int
) 
PARTITION BY HASH(a)
(
    PARTITION p0,
    PARTITION p1,
    PARTITION p2
);
insert into test_part_hash (with RECURSIVE t_r(i,j) as(values(0,1) union all select i+1,j+2 from t_r where i < 20) select * from t_r);
select * from test_part_hash;
ALTER TABLE test_part_hash truncate PARTITION all;
select * from test_part_hash;
insert into test_part_hash values(99,1),(199,1),(299,1);
alter table test_part_hash exchange partition p2 with table test_no_part1 without validation;
select * from test_part_hash;
select * from test_no_part1;
alter table test_part_hash analyze partition p0,p1;
alter table test_part_hash analyze partition all;
\c postgres;
drop DATABASE if exists partition_test2;

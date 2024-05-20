create schema partition_test1;
set current_schema to 'partition_test1';
-------test range partition tables
----test partition table
CREATE TABLE IF NOT EXISTS test_part
(
a int primary key not null default 5,
b int,
c int,
d int
) 
PARTITION BY RANGE(a)
(
    PARTITION p0 VALUES LESS THAN (1000),
    PARTITION p1 VALUES LESS THAN (2000),
    PARTITION p2 VALUES LESS THAN (3000)
);
create unique index idx_c on test_part (c);
create index idx_b on test_part using btree(b) local;
alter table test_part add constraint uidx_d unique(d);
alter table test_part add constraint uidx_c unique using index idx_c;
insert into test_part (with RECURSIVE t_r(i,j,k,m) as(values(0,1,2,3) union all select i+1,j+2,k+3,m+4 from t_r where i < 2500) select * from t_r);
select relname from pg_partition where (parentid in (select oid from pg_class where relname = 'test_part')) and parttype = 'p' and oid != relfilenode order by relname;
explain select * from test_part where ((980 < c and c < 1000) or (2180 < c and c < 2200));
select * from test_part where ((980 < c and c < 1000) or (2180 < c and c < 2200));
select * from test_part where ((980 < d and d < 1000) or (2180 < d and d < 2200));
select * from test_part where ((980 < b and b < 1000) or (2180 < b and b < 2200));
--test rebuild partition syntax
ALTER TABLE test_part REBUILD PARTITION perror;
ALTER TABLE test_part REBUILD PARTITION p0, p1;
select relname from pg_partition where (parentid in (select oid from pg_class where relname = 'test_part')) and parttype = 'p' and oid != relfilenode order by relname;
explain select * from test_part where ((980 < c and c < 1000) or (2180 < c and c < 2200));
select * from test_part where ((980 < c and c < 1000) or (2180 < c and c < 2200));
select * from test_part where ((980 < d and d < 1000) or (2180 < d and d < 2200));
select * from test_part where ((980 < b and b < 1000) or (2180 < b and b < 2200));
ALTER TABLE test_part REBUILD PARTITION all;
select relname from pg_partition where (parentid in (select oid from pg_class where relname = 'test_part')) and parttype = 'p' and oid != relfilenode order by relname;
explain select * from test_part where ((980 < c and c < 1000) or (2180 < c and c < 2200));
select * from test_part where ((980 < c and c < 1000) or (2180 < c and c < 2200));
select * from test_part where ((980 < d and d < 1000) or (2180 < d and d < 2200));
select * from test_part where ((980 < b and b < 1000) or (2180 < b and b < 2200));
--test repair check optimize partition syntax
ALTER TABLE test_part repair PARTITION p0,p1;
ALTER TABLE test_part check PARTITION p0,p1;
ALTER TABLE test_part optimize PARTITION p0,p1;
ALTER TABLE test_part repair PARTITION all;
ALTER TABLE test_part check PARTITION all;
ALTER TABLE test_part optimize PARTITION all;
--test remove partitioning syntax
select relname, boundaries from pg_partition where parentid in (select parentid from pg_partition where relname = 'test_part') order by relname;
select parttype,relname from pg_class where relname = 'test_part' and relfilenode != oid;
ALTER TABLE test_part remove PARTITIONING;
explain select * from test_part where ((980 < c and c < 1000) or (2180 < c and c < 2200));
select * from test_part where ((980 < c and c < 1000) or (2180 < c and c < 2200));
select relname, boundaries from pg_partition where parentid in (select parentid from pg_partition where relname = 'test_part') order by relname;
select parttype,relname from pg_class where relname = 'test_part' and relfilenode != oid;
drop table test_part cascade;
----test subpartition table
CREATE TABLE IF NOT EXISTS test_subpart
(
a int primary key not null default 5,
b int,
c int,
d int
) 
PARTITION BY RANGE(a) SUBPARTITION BY RANGE(b)
(
		PARTITION P_2021 VALUES LESS THAN (1000)
		(
			SUBPARTITION P_2021_1XXXX VALUES LESS THAN (1000),
			SUBPARTITION P_2021_2XXXX VALUES LESS THAN (2000),
			SUBPARTITION P_2021_3XXXX VALUES LESS THAN (10000)
		),
		PARTITION P_2022 VALUES LESS THAN (2000)
		(
			SUBPARTITION P_2022_1XXXX VALUES LESS THAN (1000),
			SUBPARTITION P_2022_2XXXX VALUES LESS THAN (2000),
			SUBPARTITION P_2022_3XXXX VALUES LESS THAN (10000)
		),
		PARTITION P_2023 VALUES LESS THAN (3000)
		(
			SUBPARTITION P_2023_1XXXX VALUES LESS THAN (1000),
			SUBPARTITION P_2023_2XXXX VALUES LESS THAN (2000),
			SUBPARTITION P_2023_3XXXX VALUES LESS THAN (10000)
		)
);
create index sidx_c on test_subpart (c) local;
create index sidx_b on test_subpart using btree(b);
alter table test_subpart add constraint sidx_d unique(d);
insert into test_subpart (with RECURSIVE t_r(i,j,k,m) as(values(0,1,2,3) union all select i+1,j+2,k+3,m+4 from t_r where i < 2500) select * from t_r);
explain select * from test_subpart where ((980 < c and ((980 < c and c < 1000) or (2180 < c and c < 2200))) or (2180 < c and c < 2200));
select * from test_subpart where ((980 < c and c < 1000) or (2180 < c and c < 2200));
--test rebuild partition syntax
select relname, parttype from pg_partition where (parentid in (select oid from pg_class where relname = 'test_subpart')) order by relname;
ALTER TABLE test_subpart REBUILD PARTITION P_2021, P_2022;
select relname, parttype from pg_partition where (parentid in (select oid from pg_class where relname = 'test_subpart')) order by relname;
select relname, parttype from pg_partition where parentid in (select oid from pg_partition where parentid in (select oid from pg_class where relname = 'test_subpart')) and oid != relfilenode order by relname;
explain select * from test_subpart where ((980 < c and c < 1000) or (2180 < c and c < 2200));
select * from test_subpart where ((980 < c and c < 1000) or (2180 < c and c < 2200));
select * from test_subpart where ((980 < d and d < 1000) or (2180 < d and d < 2200));
select * from test_subpart where ((980 < b and b < 1000) or (2180 < b and b < 2200));
ALTER TABLE test_subpart REBUILD PARTITION all;
select relname, parttype from pg_partition where (parentid in (select oid from pg_class where relname = 'test_subpart')) and oid = relfilenode order by relname;
select relname, parttype from pg_partition where parentid in (select oid from pg_partition where parentid in (select oid from pg_class where relname = 'test_subpart')) and oid != relfilenode order by relname;
explain select * from test_subpart where ((980 < c and c < 1000) or (2180 < c and c < 2200));
select * from test_subpart where ((980 < c and c < 1000) or (2180 < c and c < 2200));
select * from test_subpart where ((980 < d and d < 1000) or (2180 < d and d < 2200));
select * from test_subpart where ((980 < b and b < 1000) or (2180 < b and b < 2200));
--test repair check optimize partition syntax
ALTER TABLE test_subpart repair PARTITION P_2021,P_2022;
ALTER TABLE test_subpart check PARTITION P_2021,P_2022;
ALTER TABLE test_subpart optimize PARTITION P_2021,P_2022;
ALTER TABLE test_subpart repair PARTITION all;
ALTER TABLE test_subpart check PARTITION all;
ALTER TABLE test_subpart optimize PARTITION all;
--test remove partitioning syntax
select relname, boundaries from pg_partition where parentid in (select parentid from pg_partition where relname = 'test_subpart') order by relname;
select parttype,relname from pg_class where relname = 'test_subpart' and relfilenode != oid;
ALTER TABLE test_subpart remove PARTITIONING;
explain select * from test_subpart where ((980 < c and c < 1000) or (2180 < c and c < 2200));
select * from test_subpart where ((980 < c and c < 1000) or (2180 < c and c < 2200));
select relname, boundaries from pg_partition where parentid in (select parentid from pg_partition where relname = 'test_subpart') order by relname;
select parttype,relname from pg_class where relname = 'test_subpart' and relfilenode != oid;
drop table test_subpart cascade;




-------test list partition tables
CREATE TABLE test_part_list (a int, b int, c int, d int)
PARTITION BY list(a)
(
PARTITION p1 VALUES (2000,2001,2002,2003),
PARTITION p2 VALUES (3000,3001,3002,3003),
PARTITION p3 VALUES (4000,4001,4002,4003),
PARTITION p4 VALUES (5000,5001,5002,5003)
);
create unique index idx_c on test_part_list (c);
create index idx_b on test_part_list using btree(b) local;
alter table test_part_list add constraint uidx_d unique(d);
alter table test_part_list add constraint uidx_c unique using index idx_c;
insert into test_part_list values(2000,1,2,3),(3000,2,3,4),(4000,3,4,5),(5000,4,5,6);
select * from test_part_list order by a desc;
select relname, parttype from pg_partition where (parentid in (select oid from pg_class where relname = 'test_part_list')) and oid != relfilenode order by relname;
ALTER TABLE test_part_list REBUILD PARTITION p1, p2;
select * from test_part_list order by a desc;
select relname, parttype from pg_partition where (parentid in (select oid from pg_class where relname = 'test_part_list')) and oid != relfilenode order by relname;
ALTER TABLE test_part_list REBUILD PARTITION all;
select * from test_part_list order by a desc;
select relname, parttype from pg_partition where (parentid in (select oid from pg_class where relname = 'test_part_list')) and oid != relfilenode order by relname;




-------test hash partition tables
CREATE TABLE IF NOT EXISTS test_part_hash
(
a int primary key not null,
b int,
c int,
d int
) 
PARTITION BY HASH(a)
(
    PARTITION p0,
    PARTITION p1,
    PARTITION p2
);
insert into test_part_hash (with RECURSIVE t_r(i,j,k,m) as(values(0,1,2,3) union all select i+1,j+2,k+3,m+4 from t_r where i < 2500) select * from t_r);
explain select * from test_part_hash where ((980 < c and c < 1000) or (2180 < c and c < 2200));
select * from test_part_hash where ((980 < c and c < 1000) or (2180 < c and c < 2200)) order by 1,2,3,4;
select relname, parttype from pg_partition where (parentid in (select oid from pg_class where relname = 'test_part_hash')) and oid != relfilenode order by relname;
ALTER TABLE test_part_hash REBUILD PARTITION p0, p1;
explain select * from test_part_hash where ((980 < c and c < 1000) or (2180 < c and c < 2200));
select * from test_part_hash where ((980 < c and c < 1000) or (2180 < c and c < 2200)) order by 1,2,3,4;
select relname, parttype from pg_partition where (parentid in (select oid from pg_class where relname = 'test_part_hash')) and oid != relfilenode order by relname;
ALTER TABLE test_part_hash REBUILD PARTITION all;
explain select * from test_part_hash where ((980 < c and c < 1000) or (2180 < c and c < 2200));
select * from test_part_hash where ((980 < c and c < 1000) or (2180 < c and c < 2200)) order by 1,2,3,4;
select relname, parttype from pg_partition where (parentid in (select oid from pg_class where relname = 'test_part_hash')) and oid != relfilenode order by relname;




-------test the nonpartition tables
create table IF NOT EXISTS test_nopart(a int);
ALTER TABLE test_nopart REBUILD PARTITION all;
ALTER TABLE test_nopart remove partitioning;




-------test the tables whose relkind are not 'r'
CREATE UNLOGGED TABLE IF NOT EXISTS test_part_unlog
(
a int primary key not null default 5,
b int,
c int,
d int
) 
PARTITION BY RANGE(a)
(
    PARTITION p0 VALUES LESS THAN (1000),
    PARTITION p1 VALUES LESS THAN (2000),
    PARTITION p2 VALUES LESS THAN (3000)
);

CREATE LOCAL TEMPORARY TABLE IF NOT EXISTS test_part_local
(
a int primary key not null default 5,
b int,
c int,
d int
) 
PARTITION BY RANGE(a)
(
    PARTITION p0 VALUES LESS THAN (1000),
    PARTITION p1 VALUES LESS THAN (2000),
    PARTITION p2 VALUES LESS THAN (3000)
);

CREATE GLOBAL TEMPORARY TABLE IF NOT EXISTS test_part_global
(
a int primary key not null default 5,
b int,
c int,
d int
) 
PARTITION BY RANGE(a)
(
    PARTITION p0 VALUES LESS THAN (1000),
    PARTITION p1 VALUES LESS THAN (2000),
    PARTITION p2 VALUES LESS THAN (3000)
);
ALTER TABLE test_part_unlog REBUILD PARTITION all;
ALTER TABLE test_part_local REBUILD PARTITION all;
ALTER TABLE test_part_global REBUILD PARTITION all;
ALTER TABLE test_part_unlog remove partitioning;
ALTER TABLE test_part_local remove partitioning;
ALTER TABLE test_part_global remove partitioning;



-------test USTORE table
CREATE TABLE IF NOT EXISTS test_part_ustore
(
a int primary key not null default 5,
b int,
c int,
d int
) 
PARTITION BY RANGE(a)
(
    PARTITION p0 VALUES LESS THAN (1000),
    PARTITION p1 VALUES LESS THAN (2000),
    PARTITION p2 VALUES LESS THAN (3000)
) with(STORAGE_TYPE = USTORE);
create unique index idx_c1 on test_part_ustore (c);
create index idx_b1 on test_part_ustore using ubtree(b) local;
alter table test_part_ustore add constraint uidx_d1 unique(d);
alter table test_part_ustore add constraint uidx_c1 unique using index idx_c1;
insert into test_part_ustore (with RECURSIVE t_r(i,j,k,m) as(values(0,1,2,3) union all select i+1,j+2,k+3,m+4 from t_r where i < 2500) select * from t_r);
select relname from pg_partition where (parentid in (select oid from pg_class where relname = 'test_part_ustore')) and parttype = 'p' and oid != relfilenode order by relname;
explain (costs off) select * from test_part_ustore where ((980 < c and c < 1000) or (2180 < c and c < 2200));
select * from test_part_ustore where ((980 < c and c < 1000) or (2180 < c and c < 2200));
select * from test_part_ustore where ((980 < d and d < 1000) or (2180 < d and d < 2200));
select * from test_part_ustore where ((980 < b and b < 1000) or (2180 < b and b < 2200));
--test rebuild partition syntax
ALTER TABLE test_part_ustore REBUILD PARTITION p0, p1;
select relname from pg_partition where (parentid in (select oid from pg_class where relname = 'test_part_ustore')) and parttype = 'p' and oid != relfilenode order by relname;
explain (costs off) select * from test_part_ustore where ((980 < c and c < 1000) or (2180 < c and c < 2200));
select * from test_part_ustore where ((980 < c and c < 1000) or (2180 < c and c < 2200));
select * from test_part_ustore where ((980 < d and d < 1000) or (2180 < d and d < 2200));
select * from test_part_ustore where ((980 < b and b < 1000) or (2180 < b and b < 2200));
ALTER TABLE test_part_ustore REBUILD PARTITION all;
select relname from pg_partition where (parentid in (select oid from pg_class where relname = 'test_part_ustore')) and parttype = 'p' and oid != relfilenode order by relname;
explain (costs off) select * from test_part_ustore where ((980 < c and c < 1000) or (2180 < c and c < 2200));
select * from test_part_ustore where ((980 < c and c < 1000) or (2180 < c and c < 2200));
select * from test_part_ustore where ((980 < d and d < 1000) or (2180 < d and d < 2200));
select * from test_part_ustore where ((980 < b and b < 1000) or (2180 < b and b < 2200));



-------test segment table
CREATE TABLE IF NOT EXISTS test_part_segment
(
a int primary key not null default 5,
b int,
c int,
d int
) 
PARTITION BY RANGE(a)
(
    PARTITION p0 VALUES LESS THAN (1000),
    PARTITION p1 VALUES LESS THAN (2000),
    PARTITION p2 VALUES LESS THAN (3000)
) with(segment = on);
create unique index idx_c2 on test_part_segment (c);
create index idx_b2 on test_part_segment using btree(b) local;
alter table test_part_segment add constraint uidx_d2 unique(d);
alter table test_part_segment add constraint uidx_c2 unique using index idx_c2;
insert into test_part_segment (with RECURSIVE t_r(i,j,k,m) as(values(0,1,2,3) union all select i+1,j+2,k+3,m+4 from t_r where i < 2500) select * from t_r);
select relname from pg_partition where (parentid in (select oid from pg_class where relname = 'test_part_segment')) and parttype = 'p' and oid != relfilenode order by relname;
explain select * from test_part_segment where ((980 < c and c < 1000) or (2180 < c and c < 2200));
select * from test_part_segment where ((980 < c and c < 1000) or (2180 < c and c < 2200));
select * from test_part_segment where ((980 < d and d < 1000) or (2180 < d and d < 2200));
select * from test_part_segment where ((980 < b and b < 1000) or (2180 < b and b < 2200));
--test rebuild partition syntax
ALTER TABLE test_part_segment REBUILD PARTITION p0, p1;
select relname from pg_partition where (parentid in (select oid from pg_class where relname = 'test_part_segment')) and parttype = 'p' and oid != relfilenode order by relname;
explain select * from test_part_segment where ((980 < c and c < 1000) or (2180 < c and c < 2200));
select * from test_part_segment where ((980 < c and c < 1000) or (2180 < c and c < 2200));
select * from test_part_segment where ((980 < d and d < 1000) or (2180 < d and d < 2200));
select * from test_part_segment where ((980 < b and b < 1000) or (2180 < b and b < 2200));
ALTER TABLE test_part_segment REBUILD PARTITION all;
select relname from pg_partition where (parentid in (select oid from pg_class where relname = 'test_part_segment')) and parttype = 'p' and oid != relfilenode order by relname;
explain select * from test_part_segment where ((980 < c and c < 1000) or (2180 < c and c < 2200));
select * from test_part_segment where ((980 < c and c < 1000) or (2180 < c and c < 2200));
select * from test_part_segment where ((980 < d and d < 1000) or (2180 < d and d < 2200));
select * from test_part_segment where ((980 < b and b < 1000) or (2180 < b and b < 2200));
--test remove partitioning
alter table test_part_segment remove partitioning;

set dolphin.b_compatibility_mode = on;
CREATE TABLE tb_subpart(
    lid int,
    rtime timestamp WITHOUT TIME ZONE,
    col1 text
)PARTITION BY RANGE(rtime) subpartition by list(lid)
(
    PARTITION p0 VALUES LESS THAN('2024-02-01 00:00:00'::TIMESTAMP)
    (
        subpartition sub01 values (1),
        subpartition sub02 values (2)
    ),
    PARTITION p1 VALUES LESS THAN('2024-03-01 00:00:00'::TIMESTAMP)
    (
        subpartition sub11 values (1),
        subpartition sub12 values (2)
    )
);
insert into tb_subpart values(1,'2024-01-01'),(2,'2024-01-02'),(1,'2024-02-01'),(2,'2024-02-02');
select * from tb_subpart partition(p0);
select * from tb_subpart partition(p0) as a(b);
select * from tb_subpart partition for ('2024-02-01 00:00:00'::TIMESTAMP);
select * from tb_subpart subpartition(sub01);
select * from tb_subpart subpartition for ('2024-02-01 00:00:00'::TIMESTAMP, 1);
select * from tb_subpart partitio(p0); --error
select * from tb_subpart subpartitio(sub01); --error
select * from tb_subpart partition fo ('2024-02-01 00:00:00'::TIMESTAMP); --error
select * from tb_subpart as partitio(p0);
select * from tb_subpart as subpartitio(p0);
set dolphin.b_compatibility_mode = off;
select * from tb_subpart partitio(p0);
select * from tb_subpart subpartitio(p0);

drop schema partition_test1 cascade;
reset current_schema;

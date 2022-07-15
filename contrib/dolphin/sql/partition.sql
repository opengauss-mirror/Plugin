drop DATABASE if exists mysql_partition;

CREATE DATABASE mysql_partition dbcompatibility 'B';
\c mysql_partition;

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
    PARTITION p0 VALUES LESS THAN (100000),
    PARTITION p1 VALUES LESS THAN (200000),
    PARTITION p2 VALUES LESS THAN (300000)
);
create unique index idx_c on test_part (c);
create index idx_b on test_part using btree(b) local;
alter table test_part add constraint uidx_d unique(d);
alter table test_part add constraint uidx_c unique using index idx_c;
insert into test_part (with RECURSIVE t_r(i,j,k,m) as(values(0,1,2,3) union all select i+1,j+2,k+3,m+4 from t_r where i < 250000) select * from t_r);
select relname from pg_partition where (parentid in (select oid from pg_class where relname = 'test_part')) and parttype = 'p' and oid != relfilenode order by relname;
explain select * from test_part where ((99990 < c and c < 100000) or (219990 < c and c < 220000));
select * from test_part where ((99990 < c and c < 100000) or (219990 < c and c < 220000));
select * from test_part where ((99990 < d and d < 100000) or (219990 < d and d < 220000));
select * from test_part where ((99990 < b and b < 100000) or (219990 < b and b < 220000));
--test rebuild partition syntax
ALTER TABLE test_part REBUILD PARTITION p0, p1;
select relname from pg_partition where (parentid in (select oid from pg_class where relname = 'test_part')) and parttype = 'p' and oid != relfilenode order by relname;
explain select * from test_part where ((99990 < c and c < 100000) or (219990 < c and c < 220000));
select * from test_part where ((99990 < c and c < 100000) or (219990 < c and c < 220000));
select * from test_part where ((99990 < d and d < 100000) or (219990 < d and d < 220000));
select * from test_part where ((99990 < b and b < 100000) or (219990 < b and b < 220000));
ALTER TABLE test_part REBUILD PARTITION all;
select relname from pg_partition where (parentid in (select oid from pg_class where relname = 'test_part')) and parttype = 'p' and oid != relfilenode order by relname;
explain select * from test_part where ((99990 < c and c < 100000) or (219990 < c and c < 220000));
select * from test_part where ((99990 < c and c < 100000) or (219990 < c and c < 220000));
select * from test_part where ((99990 < d and d < 100000) or (219990 < d and d < 220000));
select * from test_part where ((99990 < b and b < 100000) or (219990 < b and b < 220000));
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
explain select * from test_part where ((99990 < c and c < 100000) or (219990 < c and c < 220000));
select * from test_part where ((99990 < c and c < 100000) or (219990 < c and c < 220000));
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
		PARTITION P_2021 VALUES LESS THAN (100000)
		(
			SUBPARTITION P_2021_1XXXX VALUES LESS THAN (100000),
			SUBPARTITION P_2021_2XXXX VALUES LESS THAN (200000),
			SUBPARTITION P_2021_3XXXX VALUES LESS THAN (1000000)
		),
		PARTITION P_2022 VALUES LESS THAN (200000)
		(
			SUBPARTITION P_2022_1XXXX VALUES LESS THAN (100000),
			SUBPARTITION P_2022_2XXXX VALUES LESS THAN (200000),
			SUBPARTITION P_2022_3XXXX VALUES LESS THAN (1000000)
		),
		PARTITION P_2023 VALUES LESS THAN (300000)
		(
			SUBPARTITION P_2023_1XXXX VALUES LESS THAN (100000),
			SUBPARTITION P_2023_2XXXX VALUES LESS THAN (200000),
			SUBPARTITION P_2023_3XXXX VALUES LESS THAN (1000000)
		)
);
create index sidx_c on test_subpart (c) local;
create index sidx_b on test_subpart using btree(b);
alter table test_subpart add constraint sidx_d unique(d);
insert into test_subpart (with RECURSIVE t_r(i,j,k,m) as(values(0,1,2,3) union all select i+1,j+2,k+3,m+4 from t_r where i < 250000) select * from t_r);
explain select * from test_subpart where ((99990 < c and ((99990 < c and c < 100000) or (219990 < c and c < 220000))) or (219990 < c and c < 220000));
select * from test_subpart where ((99990 < c and c < 100000) or (219990 < c and c < 220000));
--test rebuild partition syntax
select relname, parttype from pg_partition where (parentid in (select oid from pg_class where relname = 'test_subpart')) order by relname;
ALTER TABLE test_subpart REBUILD PARTITION P_2021, P_2022;
select relname, parttype from pg_partition where (parentid in (select oid from pg_class where relname = 'test_subpart')) order by relname;
select relname, parttype from pg_partition where parentid in (select oid from pg_partition where parentid in (select oid from pg_class where relname = 'test_subpart')) and oid != relfilenode order by relname;
explain select * from test_subpart where ((99990 < c and c < 100000) or (219990 < c and c < 220000));
select * from test_subpart where ((99990 < c and c < 100000) or (219990 < c and c < 220000));
select * from test_subpart where ((99990 < d and d < 100000) or (219990 < d and d < 220000));
select * from test_subpart where ((99990 < b and b < 100000) or (219990 < b and b < 220000));
ALTER TABLE test_subpart REBUILD PARTITION all;
select relname, parttype from pg_partition where (parentid in (select oid from pg_class where relname = 'test_subpart')) and oid = relfilenode order by relname;
select relname, parttype from pg_partition where parentid in (select oid from pg_partition where parentid in (select oid from pg_class where relname = 'test_subpart')) and oid != relfilenode order by relname;
explain select * from test_subpart where ((99990 < c and c < 100000) or (219990 < c and c < 220000));
select * from test_subpart where ((99990 < c and c < 100000) or (219990 < c and c < 220000));
select * from test_subpart where ((99990 < d and d < 100000) or (219990 < d and d < 220000));
select * from test_subpart where ((99990 < b and b < 100000) or (219990 < b and b < 220000));
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
explain select * from test_subpart where ((99990 < c and c < 100000) or (219990 < c and c < 220000));
select * from test_subpart where ((99990 < c and c < 100000) or (219990 < c and c < 220000));
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
select * from test_part_list;
select relname, parttype from pg_partition where (parentid in (select oid from pg_class where relname = 'test_part_list')) and oid != relfilenode order by relname;
ALTER TABLE test_part_list REBUILD PARTITION p1, p2;
select * from test_part_list;
select relname, parttype from pg_partition where (parentid in (select oid from pg_class where relname = 'test_part_list')) and oid != relfilenode order by relname;
ALTER TABLE test_part_list REBUILD PARTITION all;
select * from test_part_list;
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
insert into test_part_hash (with RECURSIVE t_r(i,j,k,m) as(values(0,1,2,3) union all select i+1,j+2,k+3,m+4 from t_r where i < 250000) select * from t_r);
explain select * from test_part_hash where ((99990 < c and c < 100000) or (219990 < c and c < 220000));
select * from test_part_hash where ((99990 < c and c < 100000) or (219990 < c and c < 220000));
select relname, parttype from pg_partition where (parentid in (select oid from pg_class where relname = 'test_part_hash')) and oid != relfilenode order by relname;
ALTER TABLE test_part_hash REBUILD PARTITION p0, p1;
explain select * from test_part_hash where ((99990 < c and c < 100000) or (219990 < c and c < 220000));
select * from test_part_hash where ((99990 < c and c < 100000) or (219990 < c and c < 220000));
select relname, parttype from pg_partition where (parentid in (select oid from pg_class where relname = 'test_part_hash')) and oid != relfilenode order by relname;
ALTER TABLE test_part_hash REBUILD PARTITION all;
explain select * from test_part_hash where ((99990 < c and c < 100000) or (219990 < c and c < 220000));
select * from test_part_hash where ((99990 < c and c < 100000) or (219990 < c and c < 220000));
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
    PARTITION p0 VALUES LESS THAN (100000),
    PARTITION p1 VALUES LESS THAN (200000),
    PARTITION p2 VALUES LESS THAN (300000)
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
    PARTITION p0 VALUES LESS THAN (100000),
    PARTITION p1 VALUES LESS THAN (200000),
    PARTITION p2 VALUES LESS THAN (300000)
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
    PARTITION p0 VALUES LESS THAN (100000),
    PARTITION p1 VALUES LESS THAN (200000),
    PARTITION p2 VALUES LESS THAN (300000)
);
ALTER TABLE test_part_unlog REBUILD PARTITION all;
ALTER TABLE test_part_local REBUILD PARTITION all;
ALTER TABLE test_part_global REBUILD PARTITION all;
ALTER TABLE test_part_unlog remove partitioning;
ALTER TABLE test_part_local remove partitioning;
ALTER TABLE test_part_global remove partitioning;

\c postgres;
drop DATABASE if exists mysql_partition;

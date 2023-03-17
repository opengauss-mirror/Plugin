create schema column_name_case_test;
set current_schema to 'column_name_case_test';

-- create  table  test. senstive column name;
create table t1 (AaA int, Bbb int);
insert into t1 values(1,2),(3,4);
select * from t1;
select column_name_case_test.t1.bBb from column_name_case_test.t1;
select xxxx.column_name_case_test.t1.bBb from column_name_case_test.t1;

--keyword
drop table if exists a1;
drop table if exists a2;
select currenT_User into a1;
select session_UsEr into a2;

--create use some index
drop table if exists warehouse_t22;
CREATE TABLE warehouse_t22
(
    W_WAREHOUSE_SK            INTEGER               NOT NULL,
    W_WAREHOUSE_ID            CHAR(16)              NOT NULL,
    W_WAREHOUSE_NAME          VARCHAR(20)                   ,
    W_WAREHOUSE_SQ_FT         INTEGER                       ,
    W_STREET_NUMBER           CHAR(10)                      ,
    W_STREET_NAME             VARCHAR(60)                   ,
    W_STREET_TYPE             CHAR(15)                      ,
    W_SUITE_NUMBER            CHAR(10)                      ,
    W_CITY                    VARCHAR(60)                   ,
    W_COUNTY                  VARCHAR(30)                   ,
    W_STATE                   CHAR(2)                       ,
    W_ZIP                     CHAR(10)                      ,
    W_COUNTRY                 VARCHAR(20)                   ,
    W_GMT_OFFSET              DECIMAL(5,2),
    CONSTRAINT W_CONSTR_KEY3 UNIQUE(W_WAREHOUSE_SK)
);

CREATE TABLE warehouse_tzz
(
    W_WArEHOUSE_sk            INTEGER               NOT NULL,
    W_WArEHOUSE_Id            CHAR(16)              NOT NULL,
    W_WAREHOUSE_NAME          VARCHAR(20)                   ,
    W_wAREHOUSE_Sq_FT         INTEGER                       ,
    W_STREET_NUMBER           CHAR(10)                      ,
    W_STREET_NAME             VARCHAR(60)                   ,
    W_STREET_TYPE             CHAR(15)                      ,
    W_SUITE_NUMBER            CHAR(10)                      ,
    W_CITY                    VARCHAR(60)                   ,
    W_COUNTY                  VARCHAR(30)                   ,
    W_STATE                   CHAR(2)                       ,
    W_ZIP                     CHAR(10)                      ,
    W_COUNTRY                 VARCHAR(20)                   ,
    W_GMT_OFFSET              DECIMAL(5,2)                  ,
    key using btree (W_WAREHOUSe_SK, W_wAREHOUSE_ID desc)   ,
    index idx_ssQ_FT using btree ((abs(W_WAREHOUSE_SQ_fT)))  ,
    key idx_SsK using btree ((abs(W_WAREHOUSe_SK)+1))
);


drop table if exists tk1;
create table tk1(Aa int check (aA is not null) );

 drop table if exists tk2;
create table tk2(Aa int PRIMARY KEY CHECK (aA > 0) );

-- test create table like 
create table t2 like t1;
select * from t2;

-- test create view
create view v1 as select * from t1;
select * from v1;
drop view v1;

-- test create table as select 
create table t3 as select * from t1;
select * from t3;

--test alter table 
create table t4(Aa int);
Alter table t4 add column AaAa int;
select * from t4;

--test  table partition
CREATE TABLE tp_3(                                                        
CA_ADDRESS_SK             INTEGER               NOT NULL,               
CA_ADDRESS_ID             CHAR(16)              NOT NULL,               
CA_STREET_NUMBER          CHAR(10)                      ,               
CA_STREET_NAME            VARCHAR(60)                   ,               
CA_STREET_TYPE            CHAR(15)                      ,               
CA_SUITE_NUMBER           CHAR(10)                      ,               
CA_CITY                   VARCHAR(60)                   ,               
CA_COUNTY                 VARCHAR(30)                   ,               
CA_STATE                  CHAR(2)                       ,               
CA_ZIP                    CHAR(10)                      ,               
CA_COUNTRY                VARCHAR(20)                   ,               
CA_GMT_OFFSET             DECIMAL(5,2)                  ,               
CA_LOCATION_TYPE          CHAR(20)                                      
)with (orientation=column,max_batchrow= 30700, compression = middle)    
PARTITION BY RANGE(CA_ADdRESS_SK)                                       
(    PARTITION t3_1 VALUES LESS THAN (3000),                            
         PARTITION t3_2 VALUES LESS THAN (5000),                        
         PARTITION t3_3 VALUES LESS THAN (MAXVALUE)                     
)ENABLE ROW MOVEMENT;                 
                                  

select CA_ADdREsS_ID from tp_3;


--test Multi table name
create table t5(Aa int ,AaAa int);
insert into t5 values(1,2),(3,4);
select * from t5;

-- test column name in select;
select Aa from t5;
select aa from t5;
select aaaa from t5;
select Aa from t5 where aaaa = 4;
select Aa from t5 order by aa desc;
select Aa from t5 group by  aaaa ;

-- test column name in update 
create table t6(Aa int ,AaAa int);
insert into t6 values(1,2),(3,4);
select * from t6;
update t6 set aa = 3;
update t6 set aa = 7 where aaaa > 2;

--test column name in delete 
create table t7(Aa int ,AaAa int);
insert into t7 values(1,2),(3,4);
select * from t7;
delete from t7 where a = 1;
delete from t7 where aA = 1;

--test alter table 
drop table if exists t8;
create table t8(aA int,a timestamp);
Alter table t8 add column AA  int;
Alter table t8 MODIFY(Aa not null);
Alter table t8 MODIFY(Aa float);
Alter table t8 MODIFY(Aa CONSTRAINT a not null);
Alter table t8 MODIFY(Aa CONSTRAINT a null);
Alter table t8 MODIFY(Aa NULL);
Alter table t8 Alter Aa drop default;
Alter table t8 Alter Aa drop not null;
Alter table t8 Alter Aa set not null;
Alter table t8 Alter Aa set STATISTICS  2;
Alter table t8 Alter Aa SET STATISTICS PERCENT 3;
Alter table t8 MODIFY  (a timestamp  ON UPDATE CURRENT_TIMESTAMP);
Alter table t8 drop column Aa ;
Alter table t8 drop column if exists Aa ;
select * from t8;

drop table if exists t8;
create table t8(Aa int,a timestamp);
Alter table t8 add column aa  int;

Alter table t8 MODIFY(Aa not null);
Alter table t8 MODIFY(aA double);
Alter table t8 MODIFY(aa float);
Alter table t8 MODIFY(Aa CONSTRAINT a not null);
Alter table t8 MODIFY(Aa CONSTRAINT a null);
Alter table t8 MODIFY(Aa NULL);
Alter table t8 Alter Aa drop default;
Alter table t8 Alter Aa drop not null;
Alter table t8 Alter Aa set not null;
Alter table t8 Alter Aa set STATISTICS  2;
Alter table t8 Alter Aa SET STATISTICS PERCENT 3;
Alter table t8 MODIFY  (a timestamp  ON UPDATE CURRENT_TIMESTAMP);
Alter table t8 drop column Aa ;
Alter table t8 drop column if exists Aa ;
select * from t8;

drop table if exists t9 cascade;
create table t9 (cascaDe int);
Alter table t9 MODIFY(casCAde double);
drop table if exists t9 cascade;
create table t9 (valUES int);
Alter table t9 MODIFY(vAlUes double);
Alter table t9 MODIFY(column_name_case_test.t9.vAlUes double);
Alter table t9 MODIFY(t9.vAlUes double);

drop table if exists tmp;
CREATE TABLE Tmp (inItIal int4);

COMMENT ON TABLE tmp_wrong IS 'table comment';
COMMENT ON TABLE Tmp IS 'table comment';
COMMENT ON TABLE Tmp IS NULL;

ALTER TABLE Tmp ADD COLUMN xMin integer; -- fails

ALTER TABLE Tmp ADD COLUMN A int4 default 3;

ALTER TABLE Tmp ADD COLUMN B name;

ALTER TABLE Tmp ADD COLUMN c text;

ALTER TABLE Tmp ADD COLUMN d float8;

ALTER TABLE Tmp ADD COLUMN e float4;

ALTER TABLE Tmp ADD COLUMN f int2;

ALTER TABLE Tmp ADD COLUMN g polygon;

ALTER TABLE Tmp ADD COLUMN h abstime;

ALTER TABLE Tmp ADD COLUMN i char;

ALTER TABLE Tmp ADD COLUMN j abstime[];

ALTER TABLE Tmp ADD COLUMN k int4;

ALTER TABLE Tmp ADD COLUMN l tid;

ALTER TABLE Tmp ADD COLUMN M xid;

ALTER TABLE Tmp ADD COLUMN n oidvector;

--ALTER TABLE Tmp ADD COLUMN o lock;
ALTER TABLE Tmp ADD COLUMN p smgr;

ALTER TABLE Tmp ADD COLUMN q point;

ALTER TABLE Tmp ADD COLUMN r lseg;

ALTER TABLE Tmp ADD COLUMN S path;

ALTER TABLE Tmp ADD COLUMN t box;

ALTER TABLE Tmp ADD COLUMN u tinterval;

ALTER TABLE Tmp ADD COLUMN v timestamp;

ALTER TABLE Tmp ADD COLUMN w interval;

ALTER TABLE Tmp ADD COLUMN X float8[];

ALTER TABLE Tmp ADD COLUMN Y float4[];

ALTER TABLE Tmp ADD COLUMN z int2[];

INSERT INTO Tmp (a, b, c, d, e, f, g, H, i, j, K, l, m, n, p, q, r, s, t, u,
	v, W, x, y, z)
   VALUES (4, 'name', 'text', 4.1, 4.1, 2, '(4.1,4.1,3.1,3.1)',
        'Mon May  1 00:30:30 1995', 'c', '{Mon May  1 00:30:30 1995, Monday Aug 24 14:43:07 1992, epoch}',
	314159, '(1,1)', '512',
	'1 2 3 4 5 6 7 8', 'magnetic disk', '(1.1,1.1)', '(4.1,4.1,3.1,3.1)',
	'(0,2,4.1,4.1,3.1,3.1)', '(4.1,4.1,3.1,3.1)', '["epoch" "infinity"]',
	'epoch', '01:00:10', '{1.0,2.0,3.0,4.0}', '{1.0,2.0,3.0,4.0}', '{1,2,3,4}');

SELECT * FROM Tmp;

DROP TABLE Tmp;

-- the wolf bug - schema mods caused inconsistent row descriptors
CREATE TABLE Tmp (
	inItial 	int4
);

ALTER TABLE Tmp ADD COLUMN a int4;

ALTER TABLE Tmp ADD COLUMN b name;

ALTER TABLE Tmp ADD COLUMN c text;

ALTER TABLE Tmp ADD COLUMN d float8;

ALTER TABLE Tmp ADD COLUMN e float4;

ALTER TABLE Tmp ADD COLUMN f int2;

ALTER TABLE Tmp ADD COLUMN g polygon;

ALTER TABLE Tmp ADD COLUMN h abstime;

ALTER TABLE Tmp ADD COLUMN i char;

ALTER TABLE Tmp ADD COLUMN J abstime[];

ALTER TABLE Tmp ADD COLUMN K int4;

ALTER TABLE Tmp ADD COLUMN L tid;

ALTER TABLE Tmp ADD COLUMN m xid;

ALTER TABLE Tmp ADD COLUMN n oidvector;

--ALTER TABLE Tmp ADD COLUMN o lock;
ALTER TABLE Tmp ADD COLUMN p smgr;

ALTER TABLE Tmp ADD COLUMN q point;

ALTER TABLE Tmp ADD COLUMN r lseg;

ALTER TABLE Tmp ADD COLUMN s path;

ALTER TABLE Tmp ADD COLUMN t box;

ALTER TABLE Tmp ADD COLUMN u tinterval;

ALTER TABLE Tmp ADD COLUMN v timestamp;

ALTER TABLE Tmp ADD COLUMN w interval;

ALTER TABLE Tmp ADD COLUMN x float8[];

ALTER TABLE Tmp ADD COLUMN y float4[];

ALTER TABLE Tmp ADD COLUMN z int2[];

INSERT INTO Tmp (A, b, c, d, e, f, g, h, i, J, k, l, m, n, p, q, r, s, t, u,
	v, w, x, Y, z)
   VALUES (4, 'name', 'text', 4.1, 4.1, 2, '(4.1,4.1,3.1,3.1)',
        'Mon May  1 00:30:30 1995', 'c', '{Mon May  1 00:30:30 1995, Monday Aug 24 14:43:07 1992, epoch}',
	314159, '(1,1)', '512',
	'1 2 3 4 5 6 7 8', 'magnetic disk', '(1.1,1.1)', '(4.1,4.1,3.1,3.1)',
	'(0,2,4.1,4.1,3.1,3.1)', '(4.1,4.1,3.1,3.1)', '["epoch" "infinity"]',
	'epoch', '01:00:10', '{1.0,2.0,3.0,4.0}', '{1.0,2.0,3.0,4.0}', '{1,2,3,4}');

SELECT * FROM Tmp;

DROP TABLE Tmp;


--
-- rename - check on both non-temp and temp tables
--
drop table if exists tmp;
CREATE TABLE Tmp (regtablE int);
-- Enforce use of COMMIT instead of 2PC for temporary objects
-- CREATE TEMP TABLE Tmp (tmptable int);

ALTER TABLE Tmp RENAME TO Tmp_new;


-- FOREIGN KEY CONSTRAINT adding TEST

CREATE TABLE Tmp2 (A int primary key);

CREATE TABLE Tmp3 (A int, b int);

CREATE TABLE Tmp4 (A int, b int, unique(a,b));

CREATE TABLE Tmp5 (A int, b int);

-- Insert rows into Tmp2 (PKTABLE)
INSERT INTO Tmp2 values (1);
INSERT INTO Tmp2 values (2);
INSERT INTO Tmp2 values (3);
INSERT INTO Tmp2 values (4);

-- Insert rows into Tmp3
INSERT INTO Tmp3 values (1,10);
INSERT INTO Tmp3 values (1,20);
INSERT INTO Tmp3 values (5,50);

-- Try (and fail) to add constraint due to invalid data
ALTER TABLE Tmp3 add constraint tmpconstr foreign key (a) references Tmp2 match full;

-- Delete failing row
DELETE FROM Tmp3 where a=5;

-- Try (and succeed)
ALTER TABLE Tmp3 add constraint tmpconstr foreign key (a) references Tmp2 match full;


--index 
CREATE INDEX t3_idx ON tp_3(CA_ADDRESS_sk) LOCAL;

--insert 
create table Inserttest (CoL1 int4, col2 int4 NOT NULL, col3 text default 'testing');
insert into Inserttest (col1, cOl2, Col3) values (DEFAULT, DEFAULT, DEFAULT);
select * from Inserttest;

-- in function
create table colfunctest (AaAa int,BbbB int);
insert into colfunctest values(1,2);
select sin(AaAA) from colfunctest;
select sin(colfunctest.AaAA) from colfunctest;
select sin(column_name_case_test.colfunctest.AaAA)  from colfunctest;

--in keyword func
create table colkwfunctest (AaAa int,BbbB int);
insert into colkwfunctest values(1,2);
select CAST(AaAA as text) from colkwfunctest;
select CAST(colkwfunctest.AaAA as text) from colkwfunctest;
select CAST(column_name_case_test.colkwfunctest.AaAA as text)  from colkwfunctest;
select BBBb from colkwfunctest;

-- select with keyword unreserved
drop table if exists kwtest;
create table kwtest (APPENd int, PREPAre int);
insert into kwtest values(1,2);
insert into kwtest (APPeNd,  PRePArE ) values(5,7);
select * from kwtest ;
select sin(APPeNd ) from kwtest ;
select sin(kwtest.APPeNd) from kwtest;
select sin(column_name_case_test.kwtest.APPeNd)  from kwtest;
update kwtest set APPeNd = 3;
delete from kwtest where APPeNd = 1;

-- select with keyword colnamekw
drop table if exists kwtest;
create table kwtest (NUllIF int, ifNUll int);
insert into kwtest values(1,2);
insert into kwtest (Nullif,  iFnull ) values(5,7);
select * from kwtest ;
select sin(Nullif) from kwtest ;
select sin(kwtest.Nullif) from kwtest ;
select sin(column_name_case_test.kwtest.Nullif)  from kwtest ;
update kwtest set Nullif = 3;
delete from kwtest where Nullif = 1;

--test in order by
drop table if exists t1;
drop table if exists t2;
create table t1 (AaA int, BbB int);
insert into t1 values(1,2),(3,4),(5,6);
select * from t1 order by Aaa DESC;
select aaa  from t1 order by AAA DESC;
select aaa as a from t1 order by A DESC;

--test in where
drop table if exists t1;
drop table if exists t2;
create table t1 (AaA int, BbB int);
insert into t1 values(1,2),(3,4),(5,6),(7,8);
select * from t1 where AAa > 1 order by Aaa DESC;
select aaa as a from t1  where A > 1 order by A DESC;

--test in group by
drop table if exists t1;
drop table if exists t2;
create table t1 (AaA int, BbB int);
insert into t1 values(1,2),(3,4),(5,6),(5,6);
select aAa,count(bbB) from t1 group by Aaa order by aaa; 
select aAa as a ,count(bbB) from t1 group by A order by aaa; 

--test in having 
drop table if exists t1;
drop table if exists t2;
create table t1 (AaA int, BbB int);
insert into t1 values(1,2),(3,4),(5,6),(5,6);
select aAa,count(bbB) from t1 group by Aaa having aaa > 1 order by aaa; 
select aAa as a ,count(bbB) from t1 group by A  having A > 1 order by aaa; 

--test sw 
drop table if exists t1;
create table t1(iD int, pId int);
insert into t1 values(1,2),(2,3),(3,4);
select * from t1 start with id = 1 connect by prior id = pid;
select ID from t1 start with id = 1 connect by prior id = pid;

--test muliti select
drop table if exists t1;
create table T1(AA int);
insert into T1 values(1);
update T1 set aa=2 where aa=1;
insert into T1 set aa=1;
select * from T1;

create table T2(AA int);
insert into T2 values(1);
update T2 set aa=2 where aa=1;
insert into T2 set aa=1;
select * from T2;

update T1,T2 set T1.aa=1, T2.aa=2;
DELETE FROM T1, T2 USING T1, T2 WHERE T1.aa = T2.aa;
select count(*) from pg_catalog.PG_Class where relname='pg_class';
select count(*) from pg_catalog.PG_ClaSS where relname='pg_class';
select count(*) from pg_cATAlog.pg_class where relname='pg_class';
select count(*) from pg_cATAloG.pg_class where relname='pg_class';
select PG_cataLOG.TIMESTAMPTZ_out('2022-12-31 23:11:11');
select PG_CATALOg.TIMESTAMptz_OUT('2022-12-31 23:11:11');
select '2022-12-31 23:59:59'::timestamp::smalldatetime;

select count(*) from pg_cATAlog.PG_Class where relname='pg_class';
select count(*) from pg_cATAloG.PG_ClaSS where relname='pg_class';
select PG_cataLOG.TIMESTAMPTZ_out('2022-12-31 23:11:11');
select PG_CATALOg.TIMESTAMptz_OUT('2022-12-31 23:11:11');
select '2022-12-31 23:59:59'::timestamp::smalldatetime;

drop schema column_name_case_test cascade;
reset current_schema;


create schema column_name_case_test;
set current_schema to 'column_name_case_test';

set dolphin.lower_case_table_names = 0;
-- create  table  test. senstive column name;
create table t1 (AaA int, Bbb int);
insert into t1 values(1,2),(3,4);
select * from t1;
select column_name_case_test.t1.bBb from column_name_case_test.t1;
select xxxx.column_name_case_test.t1.bBb from column_name_case_test.t1;

--keyword
drop table if exists a1;
drop table if exists a2;
select currenT_User into a1;
select session_UsEr into a2;

--create use some index
drop table if exists warehouse_t22;
CREATE TABLE warehouse_t22
(
    W_WAREHOUSE_SK            INTEGER               NOT NULL,
    W_WAREHOUSE_ID            CHAR(16)              NOT NULL,
    W_WAREHOUSE_NAME          VARCHAR(20)                   ,
    W_WAREHOUSE_SQ_FT         INTEGER                       ,
    W_STREET_NUMBER           CHAR(10)                      ,
    W_STREET_NAME             VARCHAR(60)                   ,
    W_STREET_TYPE             CHAR(15)                      ,
    W_SUITE_NUMBER            CHAR(10)                      ,
    W_CITY                    VARCHAR(60)                   ,
    W_COUNTY                  VARCHAR(30)                   ,
    W_STATE                   CHAR(2)                       ,
    W_ZIP                     CHAR(10)                      ,
    W_COUNTRY                 VARCHAR(20)                   ,
    W_GMT_OFFSET              DECIMAL(5,2),
    CONSTRAINT W_CONSTR_KEY3 UNIQUE(W_WAREHOUSE_SK)
);

CREATE TABLE warehouse_tzz
(
    W_WArEHOUSE_sk            INTEGER               NOT NULL,
    W_WArEHOUSE_Id            CHAR(16)              NOT NULL,
    W_WAREHOUSE_NAME          VARCHAR(20)                   ,
    W_wAREHOUSE_Sq_FT         INTEGER                       ,
    W_STREET_NUMBER           CHAR(10)                      ,
    W_STREET_NAME             VARCHAR(60)                   ,
    W_STREET_TYPE             CHAR(15)                      ,
    W_SUITE_NUMBER            CHAR(10)                      ,
    W_CITY                    VARCHAR(60)                   ,
    W_COUNTY                  VARCHAR(30)                   ,
    W_STATE                   CHAR(2)                       ,
    W_ZIP                     CHAR(10)                      ,
    W_COUNTRY                 VARCHAR(20)                   ,
    W_GMT_OFFSET              DECIMAL(5,2)                  ,
    key using btree (W_WAREHOUSe_SK, W_wAREHOUSE_ID desc)   ,
    index idx_ssQ_FT using btree ((abs(W_WAREHOUSE_SQ_fT)))  ,
    key idx_SsK using btree ((abs(W_WAREHOUSe_SK)+1))
);


drop table if exists tk1;
create table tk1(Aa int check (aA is not null) );

 drop table if exists tk2;
create table tk2(Aa int PRIMARY KEY CHECK (aA > 0) );

-- test create table like 
create table t2 like t1;
select * from t2;

-- test create view
create view v1 as select * from t1;
select * from v1;
drop view v1;

-- test create table as select 
create table t3 as select * from t1;
select * from t3;

--test alter table 
create table t4(Aa int);
Alter table t4 add column AaAa int;
select * from t4;

--test  table partition
CREATE TABLE tp_3(                                                        
CA_ADDRESS_SK             INTEGER               NOT NULL,               
CA_ADDRESS_ID             CHAR(16)              NOT NULL,               
CA_STREET_NUMBER          CHAR(10)                      ,               
CA_STREET_NAME            VARCHAR(60)                   ,               
CA_STREET_TYPE            CHAR(15)                      ,               
CA_SUITE_NUMBER           CHAR(10)                      ,               
CA_CITY                   VARCHAR(60)                   ,               
CA_COUNTY                 VARCHAR(30)                   ,               
CA_STATE                  CHAR(2)                       ,               
CA_ZIP                    CHAR(10)                      ,               
CA_COUNTRY                VARCHAR(20)                   ,               
CA_GMT_OFFSET             DECIMAL(5,2)                  ,               
CA_LOCATION_TYPE          CHAR(20)                                      
)with (orientation=column,max_batchrow= 30700, compression = middle)    
PARTITION BY RANGE(CA_ADdRESS_SK)                                       
(    PARTITION t3_1 VALUES LESS THAN (3000),                            
         PARTITION t3_2 VALUES LESS THAN (5000),                        
         PARTITION t3_3 VALUES LESS THAN (MAXVALUE)                     
)ENABLE ROW MOVEMENT;                 
                                  

select CA_ADdREsS_ID from tp_3;


--test Multi table name
create table t5(Aa int ,AaAa int);
insert into t5 values(1,2),(3,4);
select * from t5;

-- test column name in select;
select Aa from t5;
select aa from t5;
select aaaa from t5;
select Aa from t5 where aaaa = 4;
select Aa from t5 order by aa desc;
select Aa from t5 group by  aaaa ;

-- test column name in update 
create table t6(Aa int ,AaAa int);
insert into t6 values(1,2),(3,4);
select * from t6;
update t6 set aa = 3;
update t6 set aa = 7 where aaaa > 2;

--test column name in delete 
create table t7(Aa int ,AaAa int);
insert into t7 values(1,2),(3,4);
select * from t7;
delete from t7 where a = 1;
delete from t7 where aA = 1;

--test alter table 
drop table if exists t8;
create table t8(aA int,a timestamp);
Alter table t8 add column AA  int;
Alter table t8 MODIFY(Aa not null);
Alter table t8 MODIFY(Aa float);
Alter table t8 MODIFY(Aa CONSTRAINT a not null);
Alter table t8 MODIFY(Aa CONSTRAINT a null);
Alter table t8 MODIFY(Aa NULL);
Alter table t8 Alter Aa drop default;
Alter table t8 Alter Aa drop not null;
Alter table t8 Alter Aa set not null;
Alter table t8 Alter Aa set STATISTICS  2;
Alter table t8 Alter Aa SET STATISTICS PERCENT 3;
Alter table t8 MODIFY  (a timestamp  ON UPDATE CURRENT_TIMESTAMP);
Alter table t8 drop column Aa ;
Alter table t8 drop column if exists Aa ;
select * from t8;

drop table if exists t8;
create table t8(Aa int,a timestamp);
Alter table t8 add column aa  int;

Alter table t8 MODIFY(Aa not null);
Alter table t8 MODIFY(aA double);
Alter table t8 MODIFY(aa float);
Alter table t8 MODIFY(Aa CONSTRAINT a not null);
Alter table t8 MODIFY(Aa CONSTRAINT a null);
Alter table t8 MODIFY(Aa NULL);
Alter table t8 Alter Aa drop default;
Alter table t8 Alter Aa drop not null;
Alter table t8 Alter Aa set not null;
Alter table t8 Alter Aa set STATISTICS  2;
Alter table t8 Alter Aa SET STATISTICS PERCENT 3;
Alter table t8 MODIFY  (a timestamp  ON UPDATE CURRENT_TIMESTAMP);
Alter table t8 drop column Aa ;
Alter table t8 drop column if exists Aa ;
select * from t8;

drop table if exists t9 cascade;
create table t9 (cascaDe int);
Alter table t9 MODIFY(casCAde double);
drop table if exists t9 cascade;
create table t9 (valUES int);
Alter table t9 MODIFY(vAlUes double);
Alter table t9 MODIFY(column_name_case_test.t9.vAlUes double);
Alter table t9 MODIFY(t9.vAlUes double);

drop table if exists tmp;
CREATE TABLE Tmp (inItIal int4);

COMMENT ON TABLE tmp_wrong IS 'table comment';
COMMENT ON TABLE Tmp IS 'table comment';
COMMENT ON TABLE Tmp IS NULL;

ALTER TABLE Tmp ADD COLUMN xMin integer; -- fails

ALTER TABLE Tmp ADD COLUMN A int4 default 3;

ALTER TABLE Tmp ADD COLUMN B name;

ALTER TABLE Tmp ADD COLUMN c text;

ALTER TABLE Tmp ADD COLUMN d float8;

ALTER TABLE Tmp ADD COLUMN e float4;

ALTER TABLE Tmp ADD COLUMN f int2;

ALTER TABLE Tmp ADD COLUMN g polygon;

ALTER TABLE Tmp ADD COLUMN h abstime;

ALTER TABLE Tmp ADD COLUMN i char;

ALTER TABLE Tmp ADD COLUMN j abstime[];

ALTER TABLE Tmp ADD COLUMN k int4;

ALTER TABLE Tmp ADD COLUMN l tid;

ALTER TABLE Tmp ADD COLUMN M xid;

ALTER TABLE Tmp ADD COLUMN n oidvector;

--ALTER TABLE Tmp ADD COLUMN o lock;
ALTER TABLE Tmp ADD COLUMN p smgr;

ALTER TABLE Tmp ADD COLUMN q point;

ALTER TABLE Tmp ADD COLUMN r lseg;

ALTER TABLE Tmp ADD COLUMN S path;

ALTER TABLE Tmp ADD COLUMN t box;

ALTER TABLE Tmp ADD COLUMN u tinterval;

ALTER TABLE Tmp ADD COLUMN v timestamp;

ALTER TABLE Tmp ADD COLUMN w interval;

ALTER TABLE Tmp ADD COLUMN X float8[];

ALTER TABLE Tmp ADD COLUMN Y float4[];

ALTER TABLE Tmp ADD COLUMN z int2[];

INSERT INTO Tmp (a, b, c, d, e, f, g, H, i, j, K, l, m, n, p, q, r, s, t, u,
	v, W, x, y, z)
   VALUES (4, 'name', 'text', 4.1, 4.1, 2, '(4.1,4.1,3.1,3.1)',
        'Mon May  1 00:30:30 1995', 'c', '{Mon May  1 00:30:30 1995, Monday Aug 24 14:43:07 1992, epoch}',
	314159, '(1,1)', '512',
	'1 2 3 4 5 6 7 8', 'magnetic disk', '(1.1,1.1)', '(4.1,4.1,3.1,3.1)',
	'(0,2,4.1,4.1,3.1,3.1)', '(4.1,4.1,3.1,3.1)', '["epoch" "infinity"]',
	'epoch', '01:00:10', '{1.0,2.0,3.0,4.0}', '{1.0,2.0,3.0,4.0}', '{1,2,3,4}');

SELECT * FROM Tmp;

DROP TABLE Tmp;

-- the wolf bug - schema mods caused inconsistent row descriptors
CREATE TABLE Tmp (
	inItial 	int4
);

ALTER TABLE Tmp ADD COLUMN a int4;

ALTER TABLE Tmp ADD COLUMN b name;

ALTER TABLE Tmp ADD COLUMN c text;

ALTER TABLE Tmp ADD COLUMN d float8;

ALTER TABLE Tmp ADD COLUMN e float4;

ALTER TABLE Tmp ADD COLUMN f int2;

ALTER TABLE Tmp ADD COLUMN g polygon;

ALTER TABLE Tmp ADD COLUMN h abstime;

ALTER TABLE Tmp ADD COLUMN i char;

ALTER TABLE Tmp ADD COLUMN J abstime[];

ALTER TABLE Tmp ADD COLUMN K int4;

ALTER TABLE Tmp ADD COLUMN L tid;

ALTER TABLE Tmp ADD COLUMN m xid;

ALTER TABLE Tmp ADD COLUMN n oidvector;

--ALTER TABLE Tmp ADD COLUMN o lock;
ALTER TABLE Tmp ADD COLUMN p smgr;

ALTER TABLE Tmp ADD COLUMN q point;

ALTER TABLE Tmp ADD COLUMN r lseg;

ALTER TABLE Tmp ADD COLUMN s path;

ALTER TABLE Tmp ADD COLUMN t box;

ALTER TABLE Tmp ADD COLUMN u tinterval;

ALTER TABLE Tmp ADD COLUMN v timestamp;

ALTER TABLE Tmp ADD COLUMN w interval;

ALTER TABLE Tmp ADD COLUMN x float8[];

ALTER TABLE Tmp ADD COLUMN y float4[];

ALTER TABLE Tmp ADD COLUMN z int2[];

INSERT INTO Tmp (A, b, c, d, e, f, g, h, i, J, k, l, m, n, p, q, r, s, t, u,
	v, w, x, Y, z)
   VALUES (4, 'name', 'text', 4.1, 4.1, 2, '(4.1,4.1,3.1,3.1)',
        'Mon May  1 00:30:30 1995', 'c', '{Mon May  1 00:30:30 1995, Monday Aug 24 14:43:07 1992, epoch}',
	314159, '(1,1)', '512',
	'1 2 3 4 5 6 7 8', 'magnetic disk', '(1.1,1.1)', '(4.1,4.1,3.1,3.1)',
	'(0,2,4.1,4.1,3.1,3.1)', '(4.1,4.1,3.1,3.1)', '["epoch" "infinity"]',
	'epoch', '01:00:10', '{1.0,2.0,3.0,4.0}', '{1.0,2.0,3.0,4.0}', '{1,2,3,4}');

SELECT * FROM Tmp;

DROP TABLE Tmp;


--
-- rename - check on both non-temp and temp tables
--
drop table if exists tmp;
CREATE TABLE Tmp (regtablE int);
-- Enforce use of COMMIT instead of 2PC for temporary objects
-- CREATE TEMP TABLE Tmp (tmptable int);

ALTER TABLE Tmp RENAME TO Tmp_new;


-- FOREIGN KEY CONSTRAINT adding TEST

CREATE TABLE Tmp2 (A int primary key);

CREATE TABLE Tmp3 (A int, b int);

CREATE TABLE Tmp4 (A int, b int, unique(a,b));

CREATE TABLE Tmp5 (A int, b int);

-- Insert rows into Tmp2 (PKTABLE)
INSERT INTO Tmp2 values (1);
INSERT INTO Tmp2 values (2);
INSERT INTO Tmp2 values (3);
INSERT INTO Tmp2 values (4);

-- Insert rows into Tmp3
INSERT INTO Tmp3 values (1,10);
INSERT INTO Tmp3 values (1,20);
INSERT INTO Tmp3 values (5,50);

-- Try (and fail) to add constraint due to invalid data
ALTER TABLE Tmp3 add constraint tmpconstr foreign key (a) references Tmp2 match full;

-- Delete failing row
DELETE FROM Tmp3 where a=5;

-- Try (and succeed)
ALTER TABLE Tmp3 add constraint tmpconstr foreign key (a) references Tmp2 match full;


--index 
CREATE INDEX t3_idx ON tp_3(CA_ADDRESS_sk) LOCAL;

--insert 
create table Inserttest (CoL1 int4, col2 int4 NOT NULL, col3 text default 'testing');
insert into Inserttest (col1, cOl2, Col3) values (DEFAULT, DEFAULT, DEFAULT);
select * from Inserttest;

-- in function
create table colfunctest (AaAa int,BbbB int);
insert into colfunctest values(1,2);
select sin(AaAA) from colfunctest;
select sin(colfunctest.AaAA) from colfunctest;
select sin(column_name_case_test.colfunctest.AaAA)  from colfunctest;

--in keyword func
create table colkwfunctest (AaAa int,BbbB int);
insert into colkwfunctest values(1,2);
select CAST(AaAA as text) from colkwfunctest;
select CAST(colkwfunctest.AaAA as text) from colkwfunctest;
select CAST(column_name_case_test.colkwfunctest.AaAA as text)  from colkwfunctest;
select BBBb from colkwfunctest;

-- select with keyword unreserved
drop table if exists kwtest;
create table kwtest (APPENd int, PREPAre int);
insert into kwtest values(1,2);
insert into kwtest (APPeNd,  PRePArE ) values(5,7);
select * from kwtest ;
select sin(APPeNd ) from kwtest ;
select sin(kwtest.APPeNd) from kwtest;
select sin(column_name_case_test.kwtest.APPeNd)  from kwtest;
update kwtest set APPeNd = 3;
delete from kwtest where APPeNd = 1;

-- select with keyword colnamekw
drop table if exists kwtest;
create table kwtest (NUllIF int, ifNUll int);
insert into kwtest values(1,2);
insert into kwtest (Nullif,  iFnull ) values(5,7);
select * from kwtest ;
select sin(Nullif) from kwtest ;
select sin(kwtest.Nullif) from kwtest ;
select sin(column_name_case_test.kwtest.Nullif)  from kwtest ;
update kwtest set Nullif = 3;
delete from kwtest where Nullif = 1;

--test in order by
drop table if exists t1;
drop table if exists t2;
create table t1 (AaA int, BbB int);
insert into t1 values(1,2),(3,4),(5,6);
select * from t1 order by Aaa DESC;
select aaa  from t1 order by AAA DESC;
select aaa as a from t1 order by A DESC;

--test in where
drop table if exists t1;
drop table if exists t2;
create table t1 (AaA int, BbB int);
insert into t1 values(1,2),(3,4),(5,6),(7,8);
select * from t1 where AAa > 1 order by Aaa DESC;
select aaa as a from t1  where A > 1 order by A DESC;

--test in group by
drop table if exists t1;
drop table if exists t2;
create table t1 (AaA int, BbB int);
insert into t1 values(1,2),(3,4),(5,6),(5,6);
select aAa,count(bbB) from t1 group by Aaa order by aaa; 
select aAa as a ,count(bbB) from t1 group by A order by aaa; 

--test in having 
drop table if exists t1;
drop table if exists t2;
create table t1 (AaA int, BbB int);
insert into t1 values(1,2),(3,4),(5,6),(5,6);
select aAa,count(bbB) from t1 group by Aaa having aaa > 1 order by aaa; 
select aAa as a ,count(bbB) from t1 group by A  having A > 1 order by aaa; 

--test sw 
drop table if exists t1;
create table t1(iD int, pId int);
insert into t1 values(1,2),(2,3),(3,4);
select * from t1 start with id = 1 connect by prior id = pid;
select ID from t1 start with id = 1 connect by prior id = pid;

--test muliti select
drop table if exists t1;
create table T1(AA int);
insert into T1 values(1);
update T1 set aa=2 where aa=1;
insert into T1 set aa=1;
select * from T1;

create table T2(AA int);
insert into T2 values(1);
update T2 set aa=2 where aa=1;
insert into T2 set aa=1;
select * from T2;

update T1,T2 set T1.aa=1, T2.aa=2;
DELETE FROM T1, T2 USING T1, T2 WHERE T1.aa = T2.aa;
select count(*) from pg_catalog.PG_Class where relname='pg_class';
select count(*) from pg_catalog.PG_ClaSS where relname='pg_class';
select count(*) from pg_cATAlog.pg_class where relname='pg_class';
select count(*) from pg_cATAloG.pg_class where relname='pg_class';
select PG_cataLOG.TIMESTAMPTZ_out('2022-12-31 23:11:11');
select PG_CATALOg.TIMESTAMptz_OUT('2022-12-31 23:11:11');
select '2022-12-31 23:59:59'::timestamp::smalldatetime;

select count(*) from pg_cATAlog.PG_Class where relname='pg_class';
select count(*) from pg_cATAloG.PG_ClaSS where relname='pg_class';
select PG_cataLOG.TIMESTAMPTZ_out('2022-12-31 23:11:11');
select PG_CATALOg.TIMESTAMptz_OUT('2022-12-31 23:11:11');
select '2022-12-31 23:59:59'::timestamp::smalldatetime;

set dolphin.lower_case_table_names = 1;

CREATE OR REPLACE FUNCTION  func1(OUT p80 bigint, OUT p95 bigint)
RETURNS SETOF record
LANGUAGE plpgsql
NOT FENCED NOT SHIPPABLE
AS $function$
DECLARE
ROW_DATA RECORD;
ROW_NAME RECORD;
QUERY_STR TEXT;
QUERY_STR_NODES TEXT;
BEGIN
QUERY_STR_NODES := 'select * from dbe_perf.node_name';
FOR ROW_NAME IN EXECUTE(QUERY_STR_NODES) LOOP
  QUERY_STR := 'SELECT * FROM pg_catalog.get_instr_rt_percentile(0)';
  FOR ROW_DATA IN EXECUTE(QUERY_STR) LOOP
	p80 = ROW_DATA."P80";
	p95 = ROW_DATA."P95";
	RETURN NEXT;
  END LOOP;
END LOOP;
RETURN;
END; $function$;


select * from func1() limit 0;



CREATE OR REPLACE FUNCTION  func2(OUT aa text)
RETURNS text
LANGUAGE plpgsql
NOT FENCED NOT SHIPPABLE
AS $function$
DECLARE
QUERY_STR_NODES TEXT;
BEGIN
QUERY_StR_NODES := 'abc';
select  QUErY_STR_NoDES into aa;
RETURN aa;
END; $function$;

select * from  func2();


CREATE OR REPLACE FUNCTION  func3(OUT aa text)
RETURNS text
LANGUAGE plpgsql
NOT FENCED NOT SHIPPABLE
AS $function$
DECLARE
QUERY_STR_NODES TEXT;
BEGIN
QUERY_StR_NODES := 'abc';
select  QUErY_STR_NoDES into aa;
RETURN aa;
END; $function$;

select * from  func3();

CREATE OR REPLACE PACKAGE eMp_bonus IS
vAr1 text :='va1';--公有变量
vAR2 text :='va2';
END emp_bonUs;
/


CREATE OR REPLACE FUNCTION  func4(OUT aa text)
RETURNS text
LANGUAGE plpgsql
NOT FENCED NOT SHIPPABLE
AS $function$
DECLARE 
 type rec_type is record (naMe  varchar2(100), epno int);
   emPloyer rec_type;
BEGIN
emPloyeR.NaMe := 'abc';
select  EmPloyeR.NaME into aa;
RETURN aa;
END; $function$;

select * from  func4(); 

CREATE OR REPLACE FUNCTION  func5(OUT aa text)
RETURNS text
LANGUAGE plpgsql
NOT FENCED NOT SHIPPABLE
AS $function$
DECLARE 
 type rec_type is record (naMe  varchar2(100), epno int);
   emPloyer rec_type;
BEGIN
emPloyeR.NaMe := 'abc';
select  publiC.EmPloyeR.NaME into aa;
RETURN aa;
END; $function$;

select * from  func5(); 

CREATE OR REPLACE FUNCTION  func6(OUT aa text)
RETURNS text
LANGUAGE plpgsql
NOT FENCED NOT SHIPPABLE
AS $function$

BEGIN
create table tesf(aaAa int);
insert into tesf values (12);
RETURN 'aa';
END; $function$;

select * from  func6(); 
select * from tesf;

drop table if exists testfunc;
create table testfunc (a int,b text);
insert into testfunc values(7,'z');
CREATE OR REPLACE FUNCTION  func7(OUT aa text,OUT ab int)
RETURNS text
LANGUAGE plpgsql
NOT FENCED NOT SHIPPABLE
AS $function$
DECLARE 
 type rec_type is record (naMe  varchar2(100), epno int);
   emPloyer rec_type;
   type rec_type2 is record (
         name varchar2 not null := 'SCOTT', 
         epno int not null :=10);
    employer2 rec_type2;
BEGIN
emPloyeR.NaMe := 'abc';
emPloyeR.epno := 2;
select  testfunc.* into ab,aa from testfunc;
RETURN aa;
END; $function$;

select * from  func7(); 


drop table if exists tf1;
create table tf1(c2 int);
insert into tf1 values(1),(2),(3);
CREATE  OR REPLACE FUNCTION tfun3() RETURNS int AS $$
DECLARE
    curs1 refcursor;
    y tf1%ROWTYPE;
BEGIN
    open curs1 FOR SELECT * FROM tf1;
    fetch last from curs1 into y; 
    RAISE NOTICE 'curs1 : %', y.c2;
    
    delete from tf1 WHERE CURRENT OF curs1;
    return 0;
END;
$$ LANGUAGE plpgsql;
select tfun3();
select * from tf1;




drop table if exists t1;
create table t1(iD int, pId int);
insert into t1 values(1,2),(2,3),(3,4);
CREATE OR REPLACE FUNCTION  func8(OUT aa text,OUT ab int)
RETURNS text
LANGUAGE plpgsql
NOT FENCED NOT SHIPPABLE
AS $function$
DECLARE 
AbCd int;
BEGIN
AbCd := 2;
perform * from t1 start with Abcd = 1 connect by prior id = pid;
perform ID from t1 start with Abcd = 1 connect by prior id = pid;
RETURN aa;
END; $function$;

select * from  func8();

--test as keyword
select 1 as USER;
select 1 as ALSO;
select 1 as ADD;
select 1 as BIGINT;
select 1 as COMMON_WORD;

--test about alias
drop table if exists t1 cascade;
create table t1(AaA int, BbBb int);
insert into t1 values(1,2);

-- alias test
select * from t1;
select aaA as Abcd from t1;
select aaA abcD from t1;
select aaA as creatE from t1;
select aaA as duaL from t1;
select aaA as viEw from t1;
-- quoted alias test
select aaA as "Abcd" from t1;
select aaA "abcD" from t1;
select aaA as "creatE" from t1;
select aaA as "duaL" from t1;
select aaA as "viEw" from t1;
select "aAA" as "Abcd" from t1;
select "aaA" "abcD" from t1;
select "aaA" as "creatE" from t1;
select "aaA" as "duaL" from t1;
select "aaA" as "viEw" from t1;
--ansi quoted
select `aaA` as "Abcd" from t1;
select `aaA` "abcD" from t1;
select `aaA` as "creatE" from t1;
select `aaA` as "duaL" from t1;
select `aaA` as "viEw" from t1;
select `aAA` as `Abcd` from t1;
select `aaA` `abcD` from t1;
select `aaA` as `creatE` from t1;
select `aaA` as `duaL` from t1;
select `aaA` as `viEw` from t1;

--where test
insert into t1 values(3,4);
insert into t1 values(5,6);
select aaA as Abcd from t1 where AAA > 3;
select aaA as Abcd from t1 where AbcD > 3;
select "AaA" as `Abcd` from t1 where AAA > 3;
select `AaA` as `Abcd` from t1 where AbcD > 3;
select "AaA" as "Abcd" from t1 where `AAA` > 3;
select `AaA` as "Abcd" from t1 where "AbcD" > 3;

-- group by test
select aaA as Abcd from t1 where AAA > 3 group by `aAa`;
select aaA as Abcd from t1 where AbcD > 3 group by `Abcd`;
select "AaA" as `Abcd` from t1 where AAA > 3 group by "Abcd";
select `AaA` as `Abcd` from t1 where AbcD > 3 group by "aAa";
select "AaA" as "Abcd" from t1 where `AAA` > 3 group by Abcd;
select `AaA` as "Abcd" from t1 where "AbcD" > 3 group by aAa;

-- having  test
select aaA as Abcd from t1 where AAA > 1 group by `aAa` having AAA >3 ;
select aaA as Abcd from t1 where AbcD > 1 group by `Abcd` having Abcd >3;
select "AaA" as `Abcd` from t1 where AAA > 1 group by "Abcd" having `AAA` >3;
select `AaA` as `Abcd` from t1 where AbcD > 1 group by "aAa" having `Abcd` >3;
select "AaA" as "Abcd" from t1 where `AAA` > 1 group by Abcd having "Abcd" >3;
select `AaA` as "Abcd" from t1 where "AbcD" > 1 group by aAa having "AAA" >3;

-- order by   test
select aaA as Abcd from t1 where AAA > 1 group by `aAa` having AAA >3 order by AAA;
select aaA as Abcd from t1 where AbcD > 1 group by `Abcd` having Abcd >3 order by Abcd;
select "AaA" as `Abcd` from t1 where AAA > 1 group by "Abcd" having `AAA` >3 order by `AAA`;
select `AaA` as `Abcd` from t1 where AbcD > 1 group by "aAa" having `Abcd` >3 order by `Abcd`;
select "AaA" as "Abcd" from t1 where `AAA` > 1 group by Abcd having "Abcd" >3 order by "AAA";
select `AaA` as "Abcd" from t1 where "AbcD" > 1 group by aAa having "AAA" >3 order by "Abcd";

-- create view
create view v1 as select aaA as Abcd from t1 where AAA > 1 group by `aAa` having AAA >3 order by AAA;
create view v2 as select aaA as Abcd from t1 where AbcD > 1 group by `Abcd` having Abcd >3 order by Abcd;
create view v3 as select "AaA" as `Abcd` from t1 where AAA > 1 group by "Abcd" having `AAA` >3 order by `AAA`;
create view v4 as select `AaA` as `Abcd` from t1 where AbcD > 1 group by "aAa" having `Abcd` >3 order by `Abcd`;
create view v5 as select "AaA" as "Abcd" from t1 where `AAA` > 1 group by Abcd having "Abcd" >3 order by "AAA";
create view v6 as select `AaA` as "Abcd" from t1 where "AbcD" > 1 group by aAa having "AAA" >3 order by "Abcd";
select * from v1;
select * from v2;
select * from v3;
select * from v4;
select * from v5;
select * from v6;

drop view v1;
drop view v2;
drop view v3;
drop view v4;
drop view v5;
drop view v6;

-- insert select 
create table t2 (aA int);
insert into t2 select aaA as Abcd from t1 where AAA > 3;
insert into t2 select aaA as Abcd from t1 where AbcD > 3;
insert into t2 select "AaA" as `Abcd` from t1 where AAA > 3;
insert into t2 select `AaA` as `Abcd` from t1 where AbcD > 3;
insert into t2 select "AaA" as "Abcd" from t1 where `AAA` > 3;
insert into t2 select `AaA` as "Abcd" from t1 where "AbcD" > 3;
select * from t2;

--dML 
delete from t1 where (select aAA as ABVC where ABVC = 1) = AAa;
select * from t1;

drop table if exists t1 cascade;
drop table if exists t2 cascade;

SET check_function_bodies=off;
CREATE FUNCTION monot_incr(int) RETURNS bool LANGUAGE sql AS ' select $1 > max(a) from tst_dom_constr; ';
CREATE DOMAIN monot_int AS int CHECK (monot_incr(VALUE));
reset check_function_bodies;

drop schema column_name_case_test cascade;
reset current_schema;

create schema db_replace;
set current_schema to 'db_replace';

create table t1 (a int);
create table t2 (a int);
insert into t1 values(1);
insert into t2 values(2);
create table parts (id int) partition by range(id)
( partition p1 values less than(10),
partition p2 values less than(20),
partition p3 values less than(30),
partition p4 values less than(40),
partition p5 values less than(50));

Replace t1 table t2;
Replace into t1 table t2;
Replace LOW_PRIORITY into t1 table t2;
Replace DELAYED into t1 table t2;
Replace DELAYED t1 table t2;
Replace t1 table t2;
Replace t1 values (3);
Replace t1 (a) values (3),(4);
Replace t1 (a) select a from t2;
replace t1 set a = 7;
replace into parts partition(p1) table t2;
replace DELAYED into parts partition(p1) values(4);
replace DELAYED into parts partition(p1) values(4);
replace DELAYED into parts partition(p1) select a from t2 where a >=2 ;

set dolphin.lower_case_table_names = 0;

create table T1 (A int);
create table T2 (A int);
insert into T1 values(1);
insert into T2 values(2);
create table Parts (id int) partition by range(id)
( partition p1 values less than(10),
partition p2 values less than(20),
partition p3 values less than(30),
partition p4 values less than(40),
partition p5 values less than(50));

Replace T1 table T2;
Replace into T1 table T2;
Replace LOW_PRIORITY into T1 table T2;
Replace DELAYED into T1 table T2;
Replace DELAYED T1 table T2;
Replace T1 table T2;
Replace T1 values (3);
Replace T1 (A) values (3),(4);
Replace T1 (A) select A from T2;
replace T1 set A = 7;
replace into Parts partition(p1) table T2;
replace DELAYED into Parts partition(p1) values(4);
replace DELAYED into Parts partition(p1) values(4);
replace DELAYED into Parts partition(p1) select A from T2 where A >=2 ;

drop table if exists r_replace;

CREATE TABLE t_replace (
id int ,
t_id int ,
name varchar(200),
sex varchar(2),
PRIMARY KEY(id,t_id));
INSERT INTO t_replace VALUES(1,1,'zhangsan','m');
INSERT INTO t_replace VALUES(2,2,'lisi','m');
INSERT INTO t_replace VALUES(3,3,'wangwu','m');

REPLACE t_replace VALUES(1,1,(select name from t_replace where id =1),'f');
REPLACE INTO t_replace SET id=3,t_id=3,name=(select name from t_replace where id=3);

drop schema db_replace cascade;
reset current_schema;

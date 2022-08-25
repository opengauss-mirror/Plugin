drop database if exists db_replace;
create database db_replace dbcompatibility 'B';
\c db_replace;

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



\c postgres
drop database db_replace;

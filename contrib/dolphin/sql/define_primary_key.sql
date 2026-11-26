create schema define_primary_key;
set current_schema = define_primary_key;
CREATE TABLE t1 (
a2 int, b2 int, filler2 char(64) default ' ',
PRIMARY KEY idx(a2,b2,filler2)
) ;
CREATE TABLE t2 (
a2 int, b2 int, filler2 char(64) default ' ',
PRIMARY KEY idx using btree(a2,b2,filler2)
) ;
CREATE TABLE t3 (a INT KEY);
CREATE TABLE t4 (a INT UNIQUE);
CREATE TABLE t5 (a INT UNIQUE KEY);
desc t1;
desc t2;
desc t3;
desc t4;
desc t5;
drop table t1,t2,t3,t4,t5;
reset current_schema;
drop schema define_primary_key;

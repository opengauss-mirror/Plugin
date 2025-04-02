create schema m_testcase_fix;
set current_schema to 'm_testcase_fix';

create table t1 (a char(2) character set binary);
insert into t1 values ('aa'), ('bb');
select * from t1 where a in (NULL, 'aa');

select * from t1 where a in ('bb', NULL, 'aa');
drop table t1;


set dolphin.sql_mode = '';
CREATE TABLE t1
(id INT NOT NULL AUTO_INCREMENT PRIMARY KEY,
name TINYBLOB NOT NULL,
modified TIMESTAMP DEFAULT '0000-00-00 00:00:00',
INDEX namelocs (name(255))) ENGINE = InnoDB
PARTITION BY HASH(id) PARTITIONS 2;

reset dolphin.sql_mode;
insert into t1 values (1, 'aa', '2024-10-01 10:10:10');
select * from t1 where name = '\x6161';
drop table t1;

SELECT SUBTIME(120120519090607 | 120120519090607 ,'1 1:1:1.000002');



CREATE TABLE t1(a INT) ENGINE=MyISAM;
CREATE TABLE t2(b INT);
CREATE TABLE t3(c INT);
CREATE TABLE t4(d INT);
CREATE VIEW v1 AS SELECT * FROM t2;
RENAME TABLES t1 TO t5, v1 TO v2;

drop view v2;
drop table t2;
drop table t3;
drop table t4;
drop table t5;


CREATE TABLE t1 (a INT, b INT, c INT, UNIQUE (A), UNIQUE(B));
CREATE TABLE t2 (a INT, b INT, c INT, d INT);
insert into t1 values (1, 1, 1);
insert into t1 values (2, 2, 2);
insert into t2 values (1, 1, 1, 1);
insert into t2 values (2, 2, 2, 2);
INSERT t1 SELECT a,b,c FROM t2 WHERE d=2 ON DUPLICATE KEY UPDATE c=t1.c+VALUES(t1.a);
INSERT t1 SELECT a,b,c FROM t2 WHERE d=2 ON DUPLICATE KEY UPDATE c=t1.c+VALUES(t1.a);
select * from t1;
drop table t1;
drop table t2;



-- decimal(0)
create table t11(c DECIMAL(0,0));
insert into t11 values (9999999999);
insert into t11 values (10000000000);
create table t12(c DECIMAL(0));
insert into t12 values (9999999999);
insert into t12 values (10000000000);

create table t21(c numeric(0,0));
insert into t21 values (9999999999);
insert into t21 values (10000000000);
create table t22(c numeric(0));
insert into t22 values (9999999999);
insert into t22 values (10000000000);

create table t31(c fixed(0,0));
insert into t31 values (9999999999);
insert into t31 values (10000000000);
create table t32(c fixed(0));
insert into t32 values (9999999999);
insert into t32 values (10000000000);

create table t41(c dec(0,0));
insert into t41 values (9999999999);
insert into t41 values (10000000000);
create table t42(c dec(0));
insert into t42 values (9999999999);
insert into t42 values (10000000000);

\d+ t11;
\d+ t12;
\d+ t21;
\d+ t22;
\d+ t31;
\d+ t32;
\d+ t41;
\d+ t42;
drop table t11;
drop table t12;
drop table t21;
drop table t22;
drop table t31;
drop table t32;
drop table t41;
drop table t42;


CREATE TABLE t1 (id INT UNSIGNED AUTO_INCREMENT NOT NULL PRIMARY KEY,
a VARCHAR(200), b TEXT ) ENGINE = InnoDB;
CREATE FULLTEXT INDEX idx on t1 (a,b);

insert into t1(a, b) values('test1', 'test2');
insert into t1(a, b) values('test3', 'test1');
insert into t1(a, b) values('test1', 'test1');
insert into t1(a, b) values(NULL, NULL);

SELECT id FROM t1 WHERE MATCH (a,b) AGAINST ('test1' IN NATURAL LANGUAGE MODE);
SELECT id FROM t1 WHERE MATCH (a,b) AGAINST (NULL IN NATURAL LANGUAGE MODE);

drop table t1;

drop schema m_testcase_fix cascade;
reset current_schema;
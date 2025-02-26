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

drop schema m_testcase_fix cascade;
reset current_schema;
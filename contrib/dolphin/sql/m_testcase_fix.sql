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


drop schema m_testcase_fix cascade;
reset current_schema;
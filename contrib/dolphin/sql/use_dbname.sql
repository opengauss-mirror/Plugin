create schema use_dbname;
set current_schema to 'use_dbname';
CREATE schema db1;
CREATE schema db2;
USE db1;
CREATE TABLE test(a text);
INSERT INTO test VALUES('db1');
USE db2;
CREATE TABLE test(a text);
INSERT INTO test VALUES('db2');
select a from db1.test;
select a from db2.test;
select a from test;
USE db1;
select a from test;
use db3;
drop schema use_dbname cascade;
reset current_schema;
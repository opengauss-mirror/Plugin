drop database if exists use_dbname;
create database use_dbname dbcompatibility 'b';
\c use_dbname
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
\c postgres
drop database if exists use_dbname;
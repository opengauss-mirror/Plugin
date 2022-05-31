drop database if exists mysql;
create database mysql dbcompatibility 'b';
\c mysql
CREATE TABLE test2
(
id int PRIMARY KEY
);
create table test
(
a SERIAL,
b varchar(10),
c varchar(10),
d varchar(10),
e varchar(10),
f varchar(10),
g varchar(10) DEFAULT 'g',
h varchar(10) NOT NULL,
i int DEFAULT 0,
j int DEFAULT 0,
k int GENERATED ALWAYS AS (i + j) STORED,
l int DEFAULT 0,
m int CHECK (m < 50),
PRIMARY KEY (a, b),
FOREIGN KEY(l) REFERENCES test2(id)
);
CREATE UNIQUE INDEX idx_c on test (c);
CREATE UNIQUE INDEX idx_d_e on test (d, e);
CREATE INDEX idx_f on test (f);
desc test;
describe test;
create schema sc;
set search_path to 'sc';
drop table if exists test;
create table test
(
a SERIAL,
b varchar(10) DEFAULT 'b',
c varchar(10) NOT NULL,
primary key (a)
);
desc test;
describe test;
desc public.test;
desc sc.test;
desc public.test3;
desc sc.test3;
\c postgres
drop database if exists mysql;
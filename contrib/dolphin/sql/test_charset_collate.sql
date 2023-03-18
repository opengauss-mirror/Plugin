create schema test_charset;
set current_schema to 'test_charset';

set dolphin.b_compatibility_mode = off;
create database test1;
create database test2 encoding 'utf8';
create database test2 charset 'utf8';
drop database if exists test1;
drop database if exists test2;

set dolphin.b_compatibility_mode = on;
create database test3;
create database test4 encoding 'utf8';
create database test5 charset 'utf8';
create database test6 with charset 'utf8';
create database test7 with encoding 'utf8';
drop database if exists test3;
drop database if exists test4;
drop database if exists test5;
drop database if exists test6;
drop database if exists test7;

create database if not exists test4 encoding 'utf8';
drop database test4;
create database if not exists test5 charset 'utf8';
drop database test5;

set dolphin.b_compatibility_mode = off;
create database test1;
alter database test1 connection limit 200;
alter database test1;
alter database test1 charset 'utf8';
alter database test1 collate 'utf8';
drop database if exists test1;

set dolphin.b_compatibility_mode = on;
alter database test1 connection limit 200;
alter database test1 ;
alter database test1 charset 'utf8';
create database test1;
alter database test1 charset 'utf8';
alter database test1 collate 'utf8';
drop database if exists test1;

select 'a' > 'A' collate 'aaa';
select 'a' > 'A' collate 'utf8mb4_bin';

-- invalid column collate
create table test_collate0(c1 text charset 'utf8mb4' collate 'binary');
create table test_collate1 (c1 text collate 'aaa', c2 text collate 'utf8mb4_bin', c3 text charset 'aaa' collate 'aaa');
alter table test_collate1 add column c4 text charset 'aaa' collate 'aaa';
alter table test_collate1 modify column c1 text collate 'bbb';
select pg_get_tabledef('test_collate1');

-- invalid table collate
create table test_collate2(c1 text collate 'aaa', c2 text collate 'utf8mb4_bin')charset 'aaa';
alter table test_collate2 add column c4 text charset 'aaa' collate 'aaa';
alter table test_collate2 modify column c1 text collate 'bbb';
select pg_get_tabledef('test_collate2');

create table test_collate3(c1 text collate 'aaa', c2 text collate 'utf8mb4_bin')charset 'utf8' collate 'aaa';
create table test_collate4(c1 text collate 'aaa', c2 text charset 'aaa', c3 text collate 'utf8mb4_general_ci')charset 'utf8mb4' collate 'utf8mb4_bin';
alter table test_collate4 add column c4 text charset 'aaa' collate 'aaa';
alter table test_collate4 modify column c1 text collate 'bbb';
create table test_collate5(c1 text collate 'aaa', c2 text collate 'utf8mb4_bin', c3 text)collate 'aaa';

-- invalid schema collate
create schema test1 charset 'aaa';
set schema 'test1';
create table test_collate6(c1 text, c2 text collate 'utf8mb4_bin');
create table test_collate7(c1 text, c2 text collate 'utf8mb4_bin');
create table test_collate8(c1 text) charset 'utf8';
create table test_collate9(c1 text) charset 'aaa';
create table test_collate10(c1 text);

--test binary
set dolphin.b_compatibility_mode = true;
set bytea_output = 'escape';

drop table if exists test_binary1;
create table test_binary1(
    f1 blob collate 'binary'
);

insert into test_binary1 values('ppp'),('PpP'),('PPP'),('AAA'),('Aaa'),('aaa'),('Š'),('S');
select distinct f1 from test_binary1 order by f1;
select f1 from test_binary1 where f1 = 'ppp'::blob collate 'binary';
select f1 from test_binary1 where f1 = 'ppp'::blob collate 'utf8mb4_bin';

alter table test_binary1 add column f2 tinyblob collate 'binary';
insert into test_binary1 (f2) values('ppp'),('PpP'),('PPP'),('AAA'),('Aaa'),('aaa'),('Š'),('S');
select distinct f2 from test_binary1 order by f2;
select f2 from test_binary1 where f2 = 'ppp'::blob collate 'binary';
select f2 from test_binary1 where f2 = 'ppp'::blob collate 'utf8mb4_bin';

alter table test_binary1 add column f3 mediumblob collate 'binary';
alter table test_binary1 add column f4 binary collate 'binary';
alter table test_binary1 add column f5 varbinary collate 'binary';
alter table test_binary1 add column f6 text collate 'binary';
alter table test_binary1 add column f7 int collate 'binary';

drop table if exists test_binary2;
create table test_binary2(c1 text, c2 varchar(10), c3 char(10), c4 tinytext) collate 'binary';
alter table test_binary2 add column c5 text;
alter table test_binary2 add column c6 varchar(10);
alter table test_binary2 add column c6 varchar(10) collate 'utf8mb4_bin';
alter table test_binary2 add column c7 char(10);
alter table test_binary2 add column c8 tinytext;
select pg_get_tabledef('test_binary2');

drop schema test_charset cascade;
reset current_schema;
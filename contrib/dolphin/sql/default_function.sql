drop database if exists db_default;
create database db_default dbcompatibility 'b';
\c db_default

create table t1(id int default 100, name varchar default 'tt');
\d t1
select default(name) from t1;
insert into t1 values(1,'test');
select default(id), default(name) from t1;

create table t2(stime timestamp default current_timestamp);
\d t2
insert into t2 values(now());
select default(stime) from t2;

select default(does_not_exist) from t2;

create schema tt;

create table tt.t3(id int, stime timestamp on update current_timestamp);
\d tt.t3
insert into tt.t3 values(101, now());
select default(id), default(stime) from tt.t3;

select default(a.id) from t1 a;
select default(tt.t3.stime) from tt.t3;
select default(db_default.tt.t3.id) from tt.t3;

create table t4 (c1 INT NOT NULL AUTO_INCREMENT PRIMARY KEY, c2 VARCHAR(100), c3 VARCHAR(100));
\d t4
select default(c1) from t4;
drop table t4;

create table t5 (c1 int, c2 int, c3 int generated always as (c1-c2) stored);
insert into t5 values(1, 2);
select default(c3) from t5;
drop table t5;

\c postgres
drop database if exists db_default;

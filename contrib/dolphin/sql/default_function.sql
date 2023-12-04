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

create table a(a int default 10, b int primary key);
insert into a set a=1, b=1;
update a set a = default(b);
select default(b) from a;

drop table t1;
create table t1(a int default(1+2));
select default(a) from t1;
insert into t1 values(default);
select default(a) from t1;
create table t3 as select default(a) from t1;
select * from t3;

create table t4(a int default(abs(4+1) - abs(2-1)));
insert into t4 values (1);
select default(a) from t4;

create table t5(id int default (2 + (2 *3)), name int default (2 * (2 + 3)));
insert into t5 values (default);
create table t6(id int default (8 + (1 *3)), name int default (8 * (1 + 3))) as select default(id) as id, default (name) as name from t5;
select * from t6;

create table t7(c1 varchar(20) default concat('hello', ' world'), c2 varchar(20) default concat('hello', ' world'), c3 timestamp(6) default now(), c4 text default repeat('hello',2),
c5 bytea default E'\\000'::bytea, c6 int default 1+(2*3), c7 timestamp default timeofday()::timestamp, c8 number(9,1) default 10+238/5*3, c9 date default current_date, c10 date);
insert into t7 values (default);
select * from t7;

drop table t1;
drop table t3;
drop table t4;
drop table t5;
drop table t6;
drop table t7;

\c postgres
drop database if exists db_default;

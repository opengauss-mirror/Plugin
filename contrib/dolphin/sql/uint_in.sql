drop database if exists uint_in;
create database uint_in dbcompatibility 'b';
\c uint_in

create table t1(a uint1);
create table t2(a uint2);
create table t3(a uint4);
create table t4(a uint8);

insert into t1 values('');
insert into t1 values('1.4');
insert into t1 values('1.5');

select * from t1;

insert into t2 values('');
insert into t2 values('1.4');
insert into t2 values('1.5');

select * from t2;

insert into t3 values('');
insert into t3 values('1.4');
insert into t3 values('1.5');

select * from t3;

insert into t4 values('');
insert into t4 values('1.4');
insert into t4 values('1.5');

select * from t4;

\c postgres
drop database uint_in
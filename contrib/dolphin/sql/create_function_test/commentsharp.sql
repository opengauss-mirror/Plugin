drop database if exists db_comment_sharp;
create database db_comment_sharp dbcompatibility 'B';
\c db_comment_sharp

create table t1(a int,b int);

insert into t1 values(10,11);

insert into t1 values(12,13);

set B_COMPATIBILITY_MODE  = 0;

select a # b from t1;

select a -- b from t1;
;

set B_COMPATIBILITY_MODE  = 1;

select 2 # b from t1 ;

select a +# b from t1;
;

#select a + b from t1;


select a + b from t1;#comment aaa

select a + b from t1;#+===- comment aaa


select a + b from t1;=#- comment aaa
;

set B_COMPATIBILITY_MODE  = 0;

create table t1#t2 (a int);

insert into t1#t2 values(1),(2);

select * from t1#t2;

set B_COMPATIBILITY_MODE  = 1;

create table t3#t2
(a int);

insert  into t3 values(1),(2);

select * from t3;

create table t4 (a#b
int );

insert into t4 values(1),(3);

select * from t4;

set B_COMPATIBILITY_MODE  = 0;

\c postgres

drop database if exists db_comment_sharp;

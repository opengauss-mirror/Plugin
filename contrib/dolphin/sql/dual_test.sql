create schema dual_test;
set current_schema to 'dual_test';

select 1 ;

select 1 from dual;

select 1+1 from dual;

select sin(1),sin(2),1+3+4 from dual;

select 1*4 from dual;


-- update

create table t1 (a int);
insert into t1 values(1),(2),(3);

select * from t1;

update t1 set a = 3 from dual where a < 2 ;

select * from t1;

-- error

select * from dual;

-- keyword

create table t2 (dual int);

insert into t2 values(1),(10);

select * from t2;

select dual from t2;

create table dual(a int);

-- keyword quoted 

create table t3 (`dual` int);

insert into t3 values(1);

select `dual` from t3;

create table `dual` (a int);

insert into `dual` values(9);

select * from `dual`;

select a from dual;

drop schema dual_test cascade;
reset current_schema;

create schema uint_join;
set current_schema to 'uint_join';

create table t1(a int2, b uint2);
create table t2(a uint4, b uint4);
insert into t1 values(1, 1);
insert into t1 values(2, 1);
insert into t1 values(-1, 1);

insert into t2 values(1, 3);
insert into t2 values(2, 2);
insert into t2 values(3, 1);

select * from t1 join t2;
select * from t1 cross join t2;
select * from t1 left join t2 on t1.a = t2.a;
select * from t1 right join t2 on t1.a = t2.a;
select * from t1 inner join t2 on t1.a = t2.a;

select /*+ nestloop(t1 t2)*/ * from t1 join t2;
select /*+ hashjoin(t1 t2)*/ * from t1 join t2;
select /*+ mergejoin(t1 t2)*/ * from t1 join t2;

drop schema uint_join cascade;
reset current_schema;

create schema scott;

drop table if exists scott.emp;
drop table if exists scott.dept;

create table scott.dept(DEPTNO number(2) constraint pk_dept primary key,mgr number(4), dname varchar2(14) ,loc varchar2(13) );

create table scott.emp
(empno number(4) constraint pk_emp primary key,
dept varchar2(10),
job varchar2(9),
mgr number(4),
hiredate date,
sal number(7,2),
comm number(7,2),
deptno number(2) constraint fk_deptno references scott.dept);

select * from scott.emp join scott.dept on emp.deptno = dept.deptno;
--for converage 
select * from scott.emp join scott.dept using(deptno);
select * from scott.emp join scott.dept using(deptno, mgr);
select * from scott.emp natural join scott.dept;

drop schema scott cascade;
reset current_schema;
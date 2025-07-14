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

-- test merge join
set enable_hashjoin to off;
set enable_nestloop to off;
set enable_mergejoin to on;
drop table if exists business_order;
drop table if exists statement;
drop table if exists user_invoice_account;

create table business_order (id bigint NOT NULL);
create table statement (business_order_id bigint NOT NULL, user_invoice_account_id UINT1);
create table user_invoice_account (id bigint NOT NULL);
insert into business_order values(generate_series(-2, 3));
insert into statement values(generate_series(-1, 3), generate_series(2, 5));
insert into user_invoice_account values(generate_series(-3, 4));
explain (costs off) select * from business_order bo
    left join statement s on bo.id = s.business_order_id
    left join user_invoice_account uia on s.user_invoice_account_id = uia.id
    order by bo.id, business_order_id, user_invoice_account_id, uia.id;
select * from business_order bo
    left join statement s on bo.id = s.business_order_id
    left join user_invoice_account uia on s.user_invoice_account_id = uia.id
    order by bo.id, business_order_id, user_invoice_account_id, uia.id;

drop table if exists business_order;
drop table if exists statement;
drop table if exists user_invoice_account;
create table business_order (id bigint NOT NULL);
create table statement (business_order_id bigint NOT NULL, user_invoice_account_id uint2);
create table user_invoice_account (id bigint NOT NULL);
insert into business_order values(generate_series(-2, 3));
insert into statement values(generate_series(-1, 3), generate_series(2, 5));
insert into user_invoice_account values(generate_series(-3, 4));
explain (costs off) select * from business_order bo
    left join statement s on bo.id = s.business_order_id
    left join user_invoice_account uia on s.user_invoice_account_id = uia.id
    order by bo.id, business_order_id, user_invoice_account_id, uia.id;
select * from business_order bo
    left join statement s on bo.id = s.business_order_id
    left join user_invoice_account uia on s.user_invoice_account_id = uia.id
    order by bo.id, business_order_id, user_invoice_account_id, uia.id;

drop table if exists business_order;
drop table if exists statement;
drop table if exists user_invoice_account;
create table business_order (id bigint NOT NULL);
create table statement (business_order_id bigint NOT NULL, user_invoice_account_id UINT4);
create table user_invoice_account (id bigint NOT NULL);
insert into business_order values(generate_series(-2, 3));
insert into statement values(generate_series(-1, 3), generate_series(2, 5));
insert into user_invoice_account values(generate_series(-3, 4));
explain (costs off) select * from business_order bo
    left join statement s on bo.id = s.business_order_id
    left join user_invoice_account uia on s.user_invoice_account_id = uia.id
    order by bo.id, business_order_id, user_invoice_account_id, uia.id;
select * from business_order bo
    left join statement s on bo.id = s.business_order_id
    left join user_invoice_account uia on s.user_invoice_account_id = uia.id
    order by bo.id, business_order_id, user_invoice_account_id, uia.id;

drop table if exists business_order;
drop table if exists statement;
drop table if exists user_invoice_account;
create table business_order (id bigint NOT NULL);
create table statement (business_order_id bigint NOT NULL, user_invoice_account_id UINT8);
create table user_invoice_account (id bigint NOT NULL);
insert into business_order values(generate_series(-2, 3));
insert into statement values(generate_series(-1, 3), generate_series(2, 5));
insert into user_invoice_account values(generate_series(-3, 4));
explain (costs off) select * from business_order bo
    left join statement s on bo.id = s.business_order_id
    left join user_invoice_account uia on s.user_invoice_account_id = uia.id
    order by bo.id, business_order_id, user_invoice_account_id, uia.id;
select * from business_order bo
    left join statement s on bo.id = s.business_order_id
    left join user_invoice_account uia on s.user_invoice_account_id = uia.id
    order by bo.id, business_order_id, user_invoice_account_id, uia.id;

reset enable_hashjoin;
reset enable_nestloop;
reset enable_mergejoin;

set enable_hashjoin = on;
set enable_nestloop = off;
set enable_mergejoin = off;
create table test_tbl1(col1 uint1, col2 smallint, col3 int, col4 bigint);
create table test_tbl2(col1 uint1, col2 smallint, col3 int, col4 bigint);

insert into test_tbl1 values(0,0,0,0), (1,1,1,1);
insert into test_tbl2 values(0,0,0,0), (1,1,1,1);

select * from test_tbl1 t1 join test_tbl2 t2 on t1.col2=t2.col1;
select * from test_tbl1 t1 join test_tbl2 t2 on t1.col3=t2.col1;

explain (costs false) select * from test_tbl1 t1 join test_tbl2 t2 on t1.col2=t2.col1;
explain (costs false) select * from test_tbl1 t1 join test_tbl2 t2 on t1.col3=t2.col1;

reset enable_hashjoin;
reset enable_nestloop;
reset enable_mergejoin;

drop table test_tbl1;
drop table test_tbl2;

drop table if exists business_order;
drop table if exists statement;
drop table if exists user_invoice_account;

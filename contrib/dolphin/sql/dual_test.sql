set dolphin.use_const_value_as_colname=false;
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

-- success

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


-- select * from dual
select *;
select * from dual;
select *, 'aaa' as x from dual;
select dual.* from dual; -- error
select * from dual s; -- error
select * from dual left join t1 using (not_used); -- syntax error, dual

create table t4 (id int not null);
insert into t4 values(1);
insert into t4 values(2);

-- sublink
select distinct id from t4 where exists (select * from dual where (id = 2));
select distinct id from t4 where exists (select * from dual having id > 1);

select 1 from t4 where id in (select * from dual); -- 1
select 1 from t4 where id in (select null from dual); -- non

select id from t4 where id in (select * from dual); -- 1
select id from t4 where id in (select null from dual); -- non

select id from t4 where id = any(array(select * from dual));
select id from t4 where id = any(array[1::int, 2::int]) order by id;

select * from dual where exists(select * from dual); -- 1
select * from dual where not exists(select * from dual); -- non

select *, (select * from dual) dual1, (select * from dual) from t4;
select *, (select * from dual) as dual1(not_used), (select * from dual) from t4;

-- expr_sublink
select 1, (select * from dual) from dual; -- 1, 1
select *, (select * from dual) dual1, (select * from dual) from t4;

-- from subquery
select 1 from t4, (select * from dual) sub; -- 1, 1
select id from t4, (select * from dual) sub order by id; -- 1, 2

select 1 from t4, (select 2 as aa from dual) sub, (select 3 as bb from dual) sub1; -- 1,1
select id from t4, (select 2 as aa from dual) sub, (select 3 as bb from dual) sub1; -- 1,2

select sub1.not_used from (select * from dual) sub1; -- success
select sub1.not_used from (select * from dual) sub1(not_used);
select 1 as not_used, (select * from dual);

-- dml
insert into t4 values (3) returning *; -- success
update t4 set id = 4 from dual where id = 3 returning *; -- success
delete t4 from dual where id = 4 returning *; -- error
delete t4 using dual where id = 4 returning *; -- error

-- CTE
with dual1 as (
    select * from dual
) select * from dual1;

with dual1 as (
    select * from dual
) select not_used from dual1;

with dual1 as (
    update t4 set id = 5 from dual where id = 4 returning * -- success
) select * from dual1;

select * from t4 order by id;

-- view
create view v1 as select * from dual;
select * from v1;
\d+ v1

-- subqeury.* test
select dual1.* from (select * from dual) dual1;

-- not_used column visiable test
create table t4_1 ( id int);
create table t4_2 ( not_used int);
create table t4_3 ( a int);
insert into t4_1 values(1);
insert into t4_2 values(1);
insert into t4_3 values(1);
create table t5 ( not_used int);
insert into t5 values (10);

select not_used from dual;
select * from dual where not_used = 1; -- error
select * from dual order by not_used;
select * from dual group by not_used;
select not_used from t5, (select * from dual) sub1; -- ambiguous
select t5.not_used, sub1.not_used from t5, (select * from dual) sub1;
select * from dual, (select * from dual) dual1, (select * from dual) dual2; -- syntax error
select * from t5, (select * from dual) dual1, (select * from dual) dual2;
select * from t4_1 , lateral (select * , not_used from dual); -- error, t4_1 no not_used column
select * from t4_2 , lateral (select * , not_used from dual); -- lateral, not_used from t4_2
select * from t4_2 join (select * from dual) s1 on t4_2.not_used = not_used; -- ambiguous
select * from t4_2 join (select * from dual) s1 on t4_2.not_used = s1.not_used; -- success
select * from t4_1, (select * from dual) s1, lateral (select * from t4_3 where a = not_used); -- s1 exports not_used column, so WHERE in lateral subquery can see that
select * from t4_2, (select * from dual) s1, lateral (select * from t4_3 where a = not_used); -- ambiguous

update t5 set not_used = 11 from dual where not_used = 10; -- just see t5
select not_used from t5;

drop schema dual_test cascade;
reset current_schema;

drop database if exists uint_join;
create database uint_join dbcompatibility 'b';
\c uint_join

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

\c postgres
drop database uint_join;
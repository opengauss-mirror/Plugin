create schema test_straight_join;
set current_schema to 'test_straight_join';

create table t1 (c1 int);
create table t2 (c2 int);
insert into t1 values(1),(2),(2),(3),(4),(5);
insert into t2 values(2),(4),(6);
-- straight_join with on
explain select c1, c2 from t1 inner join t2 on c1 = c2;
explain select c1, c2 from t1 straight_join t2 on c1 = c2;
explain select straight_join c1, c2 from t1 join t2 on c1 = c2;
explain select c1, c2 from t2 straight_join t1 on c1 = c2;
explain select straight_join c1, c2 from t2 join t1 on c1 = c2;

select c1, c2 from t1 inner join t2 on c1 = c2;
select c1, c2 from t1 straight_join t2 on c1 = c2; 
select straight_join c1, c2 from t1 join t2 on c1 = c2;
select c1, c2 from t2 straight_join t1 on c1 = c2;
select straight_join c1, c2 from t2 join t1 on c1 = c2;

-- straight_join without on
explain select c1, c2 from t1 inner join t2;
explain select c1, c2 from t1 straight_join t2;
explain select straight_join c1, c2 from t1 join t2;
explain select c1, c2 from t2 straight_join t1;
explain select straight_join c1, c2 from t2 join t1;

select c1, c2 from t1 inner join t2;
select c1, c2 from t1 straight_join t2; 
select straight_join c1, c2 from t1 join t2;
select c1, c2 from t2 straight_join t1;
select straight_join c1, c2 from t2 join t1;

-- straight_join with distinct
explain select distinct c1, c2 from t1 inner join t2;
explain select distinct c1, c2 from t1 straight_join t2;
explain select distinct straight_join c1, c2 from t1 join t2;
explain select distinct c1, c2 from t2 straight_join t1;
explain select distinct straight_join c1, c2 from t2 join t1;

select distinct c1, c2 from t1 inner join t2;
select distinct c1, c2 from t1 straight_join t2; 
select distinct straight_join c1, c2 from t1 join t2;
select distinct c1, c2 from t2 straight_join t1;
select distinct straight_join c1, c2 from t2 join t1;

-- straight_join with distinct behind
explain select straight_join distinct c1, c2 from t1 join t2;
explain select straight_join distinct c1, c2 from t2 join t1;

select straight_join distinct c1, c2 from t1 join t2;
select straight_join distinct c1, c2 from t2 join t1;

-- straight_join with leading hint
explain select /*+ leading((t2 t1)) */ * from t1 join t2 on c1 = c2;
explain select /*+ leading((t2 t1)) */ * from t1 straight_join t2 on c1 = c2;

select /*+ leading((t2 t1)) */ * from t1 join t2 on c1 = c2;
select /*+ leading((t2 t1)) */ * from t1 straight_join t2 on c1 = c2;

drop schema test_straight_join cascade;
reset current_schema;

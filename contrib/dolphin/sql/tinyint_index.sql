create schema tinyint_index;
set current_schema to 'tinyint_index';

create table t1(a int1);
insert into t1 select generate_series(-128, 127);
insert into t1 select generate_series(-128, 127);
insert into t1 select generate_series(-128, 127);
insert into t1 select generate_series(-128, 127);
insert into t1 select generate_series(-128, 127);
insert into t1 select generate_series(-128, 127);
insert into t1 select generate_series(-128, 127);
insert into t1 select generate_series(-128, 127);
insert into t1 select generate_series(-128, 127);
insert into t1 select generate_series(-128, 127);


create index idx1 on t1(a);
analyze t1;

explain(costs off, verbose)select * from t1 where a = 1::int1;
explain(costs off, verbose)select * from t1 where a = 1::int2;
explain(costs off, verbose)select * from t1 where a = 1::int4;
explain(costs off, verbose)select * from t1 where a = 1::int8;

select * from t1 where a = 1::int1;
select * from t1 where a = 1::int2;
select * from t1 where a = 1::int4;
select * from t1 where a = 1::int8;

explain(costs off, verbose)select * from t1 where a > 1::int1 and a < 3::int1;
explain(costs off, verbose)select * from t1 where a > 1::int2 and a < 3::int2;
explain(costs off, verbose)select * from t1 where a > 1::int4 and a < 3::int4;
explain(costs off, verbose)select * from t1 where a > 1::int8 and a < 3::int8;

select * from t1 where a > 1::int1 and a < 3::int1;
select * from t1 where a > 1::int2 and a < 3::int2;
select * from t1 where a > 1::int4 and a < 3::int4;
select * from t1 where a > 1::int8 and a < 3::int8;

explain(costs off, verbose)select * from t1 where a >= -2::int1 and a <= -1::int1;
explain(costs off, verbose)select * from t1 where a >= -2::int2 and a <= -1::int2;
explain(costs off, verbose)select * from t1 where a >= -2::int4 and a <= -1::int4;
explain(costs off, verbose)select * from t1 where a >= -2::int8 and a <= -1::int8;

select * from t1 where a >= -2::int1 and a <= -1::int1;
select * from t1 where a >= -2::int2 and a <= -1::int2;
select * from t1 where a >= -2::int4 and a <= -1::int4;
select * from t1 where a >= -2::int8 and a <= -1::int8;

drop index idx1;
create index idx1 on t1 using hash (a);
analyze t1;

explain(costs off, verbose)select * from t1 where a = 1::int1;
explain(costs off, verbose)select * from t1 where a = 1::int2;
explain(costs off, verbose)select * from t1 where a = 1::int4;
explain(costs off, verbose)select * from t1 where a = 1::int8;

select * from t1 where a = 1::int1;
select * from t1 where a = 1::int2;
select * from t1 where a = 1::int4;
select * from t1 where a = 1::int8;


drop table t1;

create table t1(a int1) with (orientation = column);
create table t2(a int4) with (orientation = column);
insert into t2 select generate_series(0, 1000000);
insert into t1 select a % 128 from t2;

create index idx1 on t1 using btree (a);
analyze t1;

explain(costs off, verbose)select * from t1 where a >= -1::int1 and a <= 0::int1;
explain(costs off, verbose)select * from t1 where a >= -1::int2 and a <= 0::int2;
explain(costs off, verbose)select * from t1 where a >= -1::int4 and a <= 0::int4;
explain(costs off, verbose)select * from t1 where a >= -1::int8 and a <= 0::int8;

drop schema tinyint_index cascade;
reset current_schema;
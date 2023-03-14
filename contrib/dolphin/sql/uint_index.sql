create schema uint_index;
set current_schema to 'uint_index';

create table t1(a uint1);
insert into t1 select generate_series(1, 255);
insert into t1 select generate_series(1, 255);
insert into t1 select generate_series(1, 255);
insert into t1 select generate_series(1, 255);
insert into t1 select generate_series(1, 255);
insert into t1 select generate_series(1, 255);
insert into t1 select generate_series(1, 255);
insert into t1 select generate_series(1, 255);
insert into t1 select generate_series(1, 255);
insert into t1 select generate_series(1, 255);
create index idx1 on t1(a);
analyze t1;

explain(costs off, verbose)select * from t1 where a = 1::uint1;
explain(costs off, verbose)select * from t1 where a = 1::uint2;
explain(costs off, verbose)select * from t1 where a = 1::uint4;
explain(costs off, verbose)select * from t1 where a = 1::uint8;
explain(costs off, verbose)select * from t1 where a = 1::int1;
explain(costs off, verbose)select * from t1 where a = 1::int2;
explain(costs off, verbose)select * from t1 where a = 1::int4;
explain(costs off, verbose)select * from t1 where a = 1::int8;

explain(costs off, verbose)select * from t1 where a > 1::uint1 and a < 3::uint1;
explain(costs off, verbose)select * from t1 where a > 1::uint2 and a < 3::uint2;
explain(costs off, verbose)select * from t1 where a > 1::uint4 and a < 3::uint4;
explain(costs off, verbose)select * from t1 where a > 1::uint8 and a < 3::uint8;
explain(costs off, verbose)select * from t1 where a > 1::int1 and a < 3::int1;
explain(costs off, verbose)select * from t1 where a > 1::int2 and a < 3::int2;
explain(costs off, verbose)select * from t1 where a > 1::int4 and a < 3::int4;
explain(costs off, verbose)select * from t1 where a > 1::int8 and a < 3::int8;

explain(costs off, verbose)select * from t1 where a >= 1::uint1 and a <= 3::uint1;
explain(costs off, verbose)select * from t1 where a >= 1::uint2 and a <= 3::uint2;
explain(costs off, verbose)select * from t1 where a >= 1::uint4 and a <= 3::uint4;
explain(costs off, verbose)select * from t1 where a >= 1::uint8 and a <= 3::uint8;
explain(costs off, verbose)select * from t1 where a >= 1::int1 and a <= 3::int1;
explain(costs off, verbose)select * from t1 where a >= 1::int2 and a <= 3::int2;
explain(costs off, verbose)select * from t1 where a >= 1::int4 and a <= 3::int4;
explain(costs off, verbose)select * from t1 where a >= 1::int8 and a <= 3::int8;

drop index idx1;
create index idx1 on t1 using hash (a);
analyze t1;

explain(costs off, verbose)select * from t1 where a = 1::uint1;
explain(costs off, verbose)select * from t1 where a = 1::uint2;
explain(costs off, verbose)select * from t1 where a = 1::uint4;
explain(costs off, verbose)select * from t1 where a = 1::uint8;
explain(costs off, verbose)select * from t1 where a = 1::int1;
explain(costs off, verbose)select * from t1 where a = 1::int2;
explain(costs off, verbose)select * from t1 where a = 1::int4;
explain(costs off, verbose)select * from t1 where a = 1::int8;

create table t2(a uint2);
insert into t2 select generate_series(1, 255);
insert into t2 select generate_series(1, 255);
insert into t2 select generate_series(1, 255);
insert into t2 select generate_series(1, 255);
insert into t2 select generate_series(1, 255);
insert into t2 select generate_series(1, 255);
insert into t2 select generate_series(1, 255);
insert into t2 select generate_series(1, 255);
insert into t2 select generate_series(1, 255);
insert into t2 select generate_series(1, 255);

create index idx2 on t2(a);
analyze t2;

explain(costs off, verbose)select * from t2 where a = 1::uint1;
explain(costs off, verbose)select * from t2 where a = 1::uint2;
explain(costs off, verbose)select * from t2 where a = 1::uint4;
explain(costs off, verbose)select * from t2 where a = 1::uint8;
explain(costs off, verbose)select * from t2 where a = 1::int1;
explain(costs off, verbose)select * from t2 where a = 1::int2;
explain(costs off, verbose)select * from t2 where a = 1::int4;
explain(costs off, verbose)select * from t2 where a = 1::int8;

explain(costs off, verbose)select * from t2 where a > 1::uint1 and a < 3::uint1;
explain(costs off, verbose)select * from t2 where a > 1::uint2 and a < 3::uint2;
explain(costs off, verbose)select * from t2 where a > 1::uint4 and a < 3::uint4;
explain(costs off, verbose)select * from t2 where a > 1::uint8 and a < 3::uint8;
explain(costs off, verbose)select * from t2 where a > 1::int1 and a < 3::int1;
explain(costs off, verbose)select * from t2 where a > 1::int2 and a < 3::int2;
explain(costs off, verbose)select * from t2 where a > 1::int4 and a < 3::int4;
explain(costs off, verbose)select * from t2 where a > 1::int8 and a < 3::int8;

explain(costs off, verbose)select * from t2 where a >= 1::uint1 and a <= 3::uint1;
explain(costs off, verbose)select * from t2 where a >= 1::uint2 and a <= 3::uint2;
explain(costs off, verbose)select * from t2 where a >= 1::uint4 and a <= 3::uint4;
explain(costs off, verbose)select * from t2 where a >= 1::uint8 and a <= 3::uint8;
explain(costs off, verbose)select * from t2 where a >= 1::int1 and a <= 3::int1;
explain(costs off, verbose)select * from t2 where a >= 1::int2 and a <= 3::int2;
explain(costs off, verbose)select * from t2 where a >= 1::int4 and a <= 3::int4;
explain(costs off, verbose)select * from t2 where a >= 1::int8 and a <= 3::int8;

drop index idx2;
create index idx2 on t2 using hash (a);
analyze t2;

explain(costs off, verbose)select * from t2 where a = 1::uint1;
explain(costs off, verbose)select * from t2 where a = 1::uint2;
explain(costs off, verbose)select * from t2 where a = 1::uint4;
explain(costs off, verbose)select * from t2 where a = 1::uint8;
explain(costs off, verbose)select * from t2 where a = 1::int1;
explain(costs off, verbose)select * from t2 where a = 1::int2;
explain(costs off, verbose)select * from t2 where a = 1::int4;
explain(costs off, verbose)select * from t2 where a = 1::int8;


create table t3(a uint4);
insert into t3 select generate_series(1, 255);
insert into t3 select generate_series(1, 255);
insert into t3 select generate_series(1, 255);
insert into t3 select generate_series(1, 255);
insert into t3 select generate_series(1, 255);
insert into t3 select generate_series(1, 255);
insert into t3 select generate_series(1, 255);
insert into t3 select generate_series(1, 255);
insert into t3 select generate_series(1, 255);
insert into t3 select generate_series(1, 255);
create index idx3 on t3(a);
analyze t3;

explain(costs off, verbose)select * from t3 where a = 1::uint1;
explain(costs off, verbose)select * from t3 where a = 1::uint2;
explain(costs off, verbose)select * from t3 where a = 1::uint4;
explain(costs off, verbose)select * from t3 where a = 1::uint8;
explain(costs off, verbose)select * from t3 where a = 1::int1;
explain(costs off, verbose)select * from t3 where a = 1::int2;
explain(costs off, verbose)select * from t3 where a = 1::int4;
explain(costs off, verbose)select * from t3 where a = 1::int8;

explain(costs off, verbose)select * from t3 where a > 1::uint1 and a < 3::uint1;
explain(costs off, verbose)select * from t3 where a > 1::uint2 and a < 3::uint2;
explain(costs off, verbose)select * from t3 where a > 1::uint4 and a < 3::uint4;
explain(costs off, verbose)select * from t3 where a > 1::uint8 and a < 3::uint8;
explain(costs off, verbose)select * from t3 where a > 1::int1 and a < 3::int1;
explain(costs off, verbose)select * from t3 where a > 1::int2 and a < 3::int2;
explain(costs off, verbose)select * from t3 where a > 1::int4 and a < 3::int4;
explain(costs off, verbose)select * from t3 where a > 1::int8 and a < 3::int8;

explain(costs off, verbose)select * from t3 where a >= 1::uint1 and a <= 3::uint1;
explain(costs off, verbose)select * from t3 where a >= 1::uint2 and a <= 3::uint2;
explain(costs off, verbose)select * from t3 where a >= 1::uint4 and a <= 3::uint4;
explain(costs off, verbose)select * from t3 where a >= 1::uint8 and a <= 3::uint8;
explain(costs off, verbose)select * from t3 where a >= 1::int1 and a <= 3::int1;
explain(costs off, verbose)select * from t3 where a >= 1::int2 and a <= 3::int2;
explain(costs off, verbose)select * from t3 where a >= 1::int4 and a <= 3::int4;
explain(costs off, verbose)select * from t3 where a >= 1::int8 and a <= 3::int8;

drop index idx3;
create index idx3 on t3 using hash (a);
analyze t3;

explain(costs off, verbose)select * from t3 where a = 1::uint1;
explain(costs off, verbose)select * from t3 where a = 1::uint2;
explain(costs off, verbose)select * from t3 where a = 1::uint4;
explain(costs off, verbose)select * from t3 where a = 1::uint8;
explain(costs off, verbose)select * from t3 where a = 1::int1;
explain(costs off, verbose)select * from t3 where a = 1::int2;
explain(costs off, verbose)select * from t3 where a = 1::int4;
explain(costs off, verbose)select * from t3 where a = 1::int8;

create table t4(a uint8);
insert into t4 select generate_series(1, 255);
insert into t4 select generate_series(1, 255);
insert into t4 select generate_series(1, 255);
insert into t4 select generate_series(1, 255);
insert into t4 select generate_series(1, 255);
insert into t4 select generate_series(1, 255);
insert into t4 select generate_series(1, 255);
insert into t4 select generate_series(1, 255);
insert into t4 select generate_series(1, 255);
insert into t4 select generate_series(1, 255);
create index idx4 on t4(a);
analyze t4;

explain(costs off, verbose)select * from t4 where a = 1::uint1;
explain(costs off, verbose)select * from t4 where a = 1::uint2;
set enable_seqscan to off;
explain(costs off, verbose)select * from t4 where a = 1::uint4;
set enable_seqscan to on;
explain(costs off, verbose)select * from t4 where a = 1::uint8;
explain(costs off, verbose)select * from t4 where a = 1::int1;
explain(costs off, verbose)select * from t4 where a = 1::int2;
explain(costs off, verbose)select * from t4 where a = 1::int4;
explain(costs off, verbose)select * from t4 where a = 1::int8;

explain(costs off, verbose)select * from t4 where a > 1::uint1 and a < 3::uint1;
explain(costs off, verbose)select * from t4 where a > 1::uint2 and a < 3::uint2;
explain(costs off, verbose)select * from t4 where a > 1::uint4 and a < 3::uint4;
explain(costs off, verbose)select * from t4 where a > 1::uint8 and a < 3::uint8;
explain(costs off, verbose)select * from t4 where a > 1::int1 and a < 3::int1;
explain(costs off, verbose)select * from t4 where a > 1::int2 and a < 3::int2;
explain(costs off, verbose)select * from t4 where a > 1::int4 and a < 3::int4;
explain(costs off, verbose)select * from t4 where a > 1::int8 and a < 3::int8;

explain(costs off, verbose)select * from t4 where a >= 1::uint1 and a <= 3::uint1;
explain(costs off, verbose)select * from t4 where a >= 1::uint2 and a <= 3::uint2;
explain(costs off, verbose)select * from t4 where a >= 1::uint4 and a <= 3::uint4;
explain(costs off, verbose)select * from t4 where a >= 1::uint8 and a <= 3::uint8;
explain(costs off, verbose)select * from t4 where a >= 1::int1 and a <= 3::int1;
explain(costs off, verbose)select * from t4 where a >= 1::int2 and a <= 3::int2;
explain(costs off, verbose)select * from t4 where a >= 1::int4 and a <= 3::int4;
explain(costs off, verbose)select * from t4 where a >= 1::int8 and a <= 3::int8;

drop index idx4;
create index idx4 on t4 using hash (a);
analyze t4;

explain(costs off, verbose)select * from t4 where a = 1::uint1;
explain(costs off, verbose)select * from t4 where a = 1::uint2;
set enable_seqscan to off;
explain(costs off, verbose)select * from t4 where a = 1::uint4;
set enable_seqscan to on;
explain(costs off, verbose)select * from t4 where a = 1::uint8;
explain(costs off, verbose)select * from t4 where a = 1::int1;
explain(costs off, verbose)select * from t4 where a = 1::int2;
explain(costs off, verbose)select * from t4 where a = 1::int4;
explain(costs off, verbose)select * from t4 where a = 1::int8;

drop schema uint_index cascade;
reset current_schema;
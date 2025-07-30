-- case0: test plan
drop table if exists update_arithmetic_expr_t;
create table update_arithmetic_expr_t(
    n int,
    a int,
    b int
);
create index idx_arith_n on update_arithmetic_expr_t(n);
explain(costs off) update/*+ indexscan(update_arithmetic_expr_t idx_arith_n)*/ update_arithmetic_expr_t set a=666, b=666+1 where n = 1;
explain(costs off) update/*+ indexscan(update_arithmetic_expr_t idx_arith_n)*/ update_arithmetic_expr_t set a=666, b=b-1 where n = 1;
explain(costs off) update/*+ indexscan(update_arithmetic_expr_t idx_arith_n)*/ update_arithmetic_expr_t set a=666, b=666*1 where n = 1;
explain(costs off) update/*+ indexscan(update_arithmetic_expr_t idx_arith_n)*/ update_arithmetic_expr_t set a=666, b=b*2 where n = 1;

-- case1: int plus
drop table if exists update_arithmetic_expr_t;
create table update_arithmetic_expr_t(
    n int,
    a int,
    b int
);
insert into update_arithmetic_expr_t values(1, 1, 1);
insert into update_arithmetic_expr_t values(2, 2, 2);
insert into update_arithmetic_expr_t values(3, 3, 3);
update update_arithmetic_expr_t set a=666, b=666+1 where n = 1; -- ok, opt path
update update_arithmetic_expr_t set a=a+888, b=888 + b where n = 2; -- ok, opt path
update update_arithmetic_expr_t set a=a+a, b=1+888 + b where n = 3; -- ok, a: opt, b: original
select * from update_arithmetic_expr_t order by n;

-- case2: int minus
drop table if exists update_arithmetic_expr_t;
create table update_arithmetic_expr_t(
    n int,
    a int,
    b int
);
insert into update_arithmetic_expr_t values(1, 1, 1);
insert into update_arithmetic_expr_t values(2, 2, 2);
insert into update_arithmetic_expr_t values(3, 3, 3);
update update_arithmetic_expr_t set a=666, b=666-6 where n = 1; -- ok, opt path
update update_arithmetic_expr_t set a=a - 888, b= 888 - b where n = 2; -- ok, opt path
update update_arithmetic_expr_t set a=a - a, b=1 - 888 - b where n = 3; -- ok, a: opt, b: original
select * from update_arithmetic_expr_t order by n;

-- case3: int multiplication
drop table if exists update_arithmetic_expr_t;
create table update_arithmetic_expr_t(
    n int,
    a int,
    b int
);
insert into update_arithmetic_expr_t values(1, 1, 1);
insert into update_arithmetic_expr_t values(2, 2, 2);
insert into update_arithmetic_expr_t values(3, 3, 3);
update update_arithmetic_expr_t set a=666, b=-6*6 where n = 1; -- ok, opt path
update update_arithmetic_expr_t set a=a * 888, b= 888 * b where n = 2; -- ok, opt path
update update_arithmetic_expr_t set a=a * a, b=1 * 888 * b where n = 3; -- ok, a: opt, b: original
select * from update_arithmetic_expr_t order by n;

-- case4: int division ———— out of range of optimazation, but ok
drop table if exists update_arithmetic_expr_t;
create table update_arithmetic_expr_t(
    n int,
    a int,
    b int
);
insert into update_arithmetic_expr_t values(1, 1, 1);
insert into update_arithmetic_expr_t values(2, 2, 2);
insert into update_arithmetic_expr_t values(3, 3, 3);
update update_arithmetic_expr_t set a=666/a, b=60/2 where n = 1; -- ok, original pass
select * from update_arithmetic_expr_t order by n;

-- case5: int8 plus
drop table if exists update_arithmetic_expr_t;
create table update_arithmetic_expr_t(
    n int8,
    a int8,
    b int8
);

insert into update_arithmetic_expr_t values(1, 1, 1);
insert into update_arithmetic_expr_t values(2, 2, 2);
insert into update_arithmetic_expr_t values(3, 3, 3);

update update_arithmetic_expr_t set a=666, b=666+1 where n = 1; -- ok, opt path
update update_arithmetic_expr_t set a=a+888, b=888 + b where n = 2; -- ok, opt path
update update_arithmetic_expr_t set a=a+a, b=1+888 + b where n = 3; -- ok, a: opt, b: original

select * from update_arithmetic_expr_t order by n;

-- case6: int8 minus
drop table if exists update_arithmetic_expr_t;
create table update_arithmetic_expr_t(
    n int8,
    a int8,
    b int8
);

insert into update_arithmetic_expr_t values(1, 1, 1);
insert into update_arithmetic_expr_t values(2, 2, 2);
insert into update_arithmetic_expr_t values(3, 3, 3);

update update_arithmetic_expr_t set a=666, b=666-6 where n = 1; -- ok, opt path
update update_arithmetic_expr_t set a=a - 888, b= 888 - b where n = 2; -- ok, opt path
update update_arithmetic_expr_t set a=a - a, b=1 - 888 - b where n = 3; -- ok, a: opt, b: original

select * from update_arithmetic_expr_t order by n;

-- case7: int multiplication
drop table if exists update_arithmetic_expr_t;
create table update_arithmetic_expr_t(
    n int8,
    a int8,
    b int8
);

insert into update_arithmetic_expr_t values(1, 1, 1);
insert into update_arithmetic_expr_t values(2, 2, 2);
insert into update_arithmetic_expr_t values(3, 3, 3);
update update_arithmetic_expr_t set a=666, b=-6*6 where n = 1; -- ok, opt path
update update_arithmetic_expr_t set a=a * 888, b= 888 * b where n = 2; -- ok, opt path
update update_arithmetic_expr_t set a=a * a, b=1 * 888 * b where n = 3; -- ok, a: opt, b: original
select * from update_arithmetic_expr_t order by n;

-- case8: test out of range
drop table if exists update_arithmetic_expr_t;
create table update_arithmetic_expr_t(
    n int,
    a int,
    b int8
);
insert into update_arithmetic_expr_t values(1, 1, 1);
insert into update_arithmetic_expr_t values(2, 2, 2);
insert into update_arithmetic_expr_t values(3, 3, 3);
update update_arithmetic_expr_t set a=2147483647 + 666 where n = 1; -- out of range
update update_arithmetic_expr_t set a=-2147483648 - a where n = 2; -- out of range
update update_arithmetic_expr_t set b=9223372036854775807 + 666 where n = 3; -- out of range
select * from update_arithmetic_expr_t order by n;

-- case9: ref other column
drop table if exists update_arithmetic_expr_t;
create table update_arithmetic_expr_t(
    n int,
    a int,
    b int8
);
insert into update_arithmetic_expr_t values(1, 1, 1);
insert into update_arithmetic_expr_t values(2, 2, 2);
insert into update_arithmetic_expr_t values(3, 3, 3);
update update_arithmetic_expr_t set a=n + 666 where n = 1; -- ok, original path

-- case10: test multiple relation
drop table if exists update_arithmetic_expr_t;
create table update_arithmetic_expr_t(
    n int,
    a int,
    b int
);
drop table if exists update_arithmetic_expr_t2;
create table update_arithmetic_expr_t2(
    n int,
    a int,
    b int
);
insert into update_arithmetic_expr_t values(1, 1, 1);
insert into update_arithmetic_expr_t2 values(1, 2, 2);
update update_arithmetic_expr_t set a=a + 666 where n in (select n from update_arithmetic_expr_t2);
select * from update_arithmetic_expr_t order by n;

-- case11: test complex column ref
drop table if exists update_arithmetic_expr_t;
create table update_arithmetic_expr_t(
    n int,
    a int,
    b int
);
insert into update_arithmetic_expr_t values(1, 1, 1);
update update_arithmetic_expr_t t set t.a=t.a + 666 where n = 1;
select * from update_arithmetic_expr_t;

-- case12: partition table
drop table if exists update_arithmetic_expr_t;
create table update_arithmetic_expr_t(
    n int,
    a int,
    b int
) partition by range(n)
(
    partition p1 values less than (100),
    partition p2 values less than (1000),
    partition p3 values less than (maxvalue)
);
insert into update_arithmetic_expr_t values(1, 1, 1);
insert into update_arithmetic_expr_t values(2, 2, 2);
insert into update_arithmetic_expr_t values(3, 3, 3);
insert into update_arithmetic_expr_t values(4, 4, 4);
update update_arithmetic_expr_t set a=a + 666 where n = 1;
update update_arithmetic_expr_t set a=a - 666 where n = 2;
update update_arithmetic_expr_t set a=a * 666 where n = 3;
update update_arithmetic_expr_t set a=a / 666 where n = 4;
select * from update_arithmetic_expr_t order by n;

-- case13: params
drop table if exists update_arithmetic_expr_t;
create table update_arithmetic_expr_t(
    n int,
    a int,
    b int2
);
insert into update_arithmetic_expr_t values(1, 1, 1);
insert into update_arithmetic_expr_t values(2, 2, 2);
prepare p_arithmetic as update update_arithmetic_expr_t set a=a + $1 where n = 1;
prepare p_arithmetic2 as update update_arithmetic_expr_t set b=$1 + b where n = 2;
execute p_arithmetic(666);
execute p_arithmetic2(666);
select * from update_arithmetic_expr_t order by n;

-- case14: ignore
drop table if exists update_arithmetic_expr_t;
create table update_arithmetic_expr_t(
    n int,
    a int,
    b int8
);
insert into update_arithmetic_expr_t values(1, 1, 1);
insert into update_arithmetic_expr_t values(2, 2, 2);
insert into update_arithmetic_expr_t values(3, 3, 3);
update/*+ ignore_error */ update_arithmetic_expr_t set a=2147483647 + 666 where n = 1; -- warning: out of range
update/*+ ignore_error */ update_arithmetic_expr_t set a=-2147483648 - a where n = 2; -- warning: out of range
update/*+ ignore_error */ update_arithmetic_expr_t set b=9223372036854775807 + 666 where n = 3; -- error: out of range
select * from update_arithmetic_expr_t order by n;
drop table if exists update_arithmetic_expr_t;
drop table if exists update_arithmetic_expr_t2;

-- case15: test qual
drop table if exists qual_arithmetic_expr_t;
create table qual_arithmetic_expr_t(
    n int,
    a int,
    b int8
);
insert into qual_arithmetic_expr_t values(1, 1, 1);
insert into qual_arithmetic_expr_t values(2, 2, 2);
insert into qual_arithmetic_expr_t values(3, 3, 3);
insert into qual_arithmetic_expr_t values(4, 2, 3);
insert into qual_arithmetic_expr_t values(4, 3, 3);
select * from qual_arithmetic_expr_t where n = 1 + 1;
select * from qual_arithmetic_expr_t where n = 2 - 1;
select * from qual_arithmetic_expr_t where n = 1 * 1;
select * from qual_arithmetic_expr_t where n = 1 + 1 + 1;
select * from qual_arithmetic_expr_t where n = 1 + a;
select * from qual_arithmetic_expr_t where n = a + a;
select * from qual_arithmetic_expr_t where n = 1 + b;
select * from qual_arithmetic_expr_t where n = b + 1;
drop table if exists qual_arithmetic_expr_t;
